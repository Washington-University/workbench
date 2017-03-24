/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "AlgorithmVolumeSmoothing.h"
#include "AlgorithmException.h"
#include "VolumeFile.h"
#include "Vector3D.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CaretAssert.h"
#include <cmath>

using namespace caret;
using namespace std;

//makes the program issue warning only once per launch, prevents repeated calls by other algorithms from spamming
bool AlgorithmVolumeSmoothing::haveWarned = false;

AString AlgorithmVolumeSmoothing::getCommandSwitch()
{
    return "-volume-smoothing";
}

AString AlgorithmVolumeSmoothing::getShortDescription()
{
    return "SMOOTH A VOLUME FILE";
}

OperationParameters* AlgorithmVolumeSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the volume to smooth");
    
    ret->addDoubleParameter(2, "kernel", "the gaussian smoothing kernel sigma, in mm");
    
    ret->addVolumeOutputParameter(3, "volume-out", "the output volume");
    
    OptionalParameter* roiVolOpt = ret->createOptionalParameter(4, "-roi", "smooth only from data within an ROI");
    roiVolOpt->addVolumeParameter(1, "roivol", "the volume to use as an ROI");
    
    ret->createOptionalParameter(5, "-fix-zeros", "treat zero values as not being data");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to smooth");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Gaussian smoothing for volumes.  By default, smooths all subvolumes with no ROI, if ROI is given, only ") +
        "positive voxels in the ROI volume have their values used, and all other voxels are set to zero.  Smoothing a non-orthogonal volume will " +
        "be significantly slower, because the operation cannot be separated into 1-dimensional smoothings without distorting the kernel shape.\n\n" +
        "The -fix-zeros option causes the smoothing to not use an input value if it is zero, but still write a smoothed value to the voxel.  " +
        "This is useful for zeros that indicate lack of information, preventing them from pulling down the intensity of nearby voxels, while " +
        "giving the zero an extrapolated value."
    );
    return ret;
}

void AlgorithmVolumeSmoothing::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVol = myParams->getVolume(1);
    float myKernel = (float)myParams->getDouble(2);
    VolumeFile* myOutVol = myParams->getOutputVolume(3);
    OptionalParameter* roiVolOpt = myParams->getOptionalParameter(4);
    VolumeFile* roiVol = NULL;
    if (roiVolOpt->m_present)
    {
        roiVol = roiVolOpt->getVolume(1);
    }
    OptionalParameter* fixZerosOpt = myParams->getOptionalParameter(5);
    bool fixZeros = fixZerosOpt->m_present;
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(6);
    int subvolNum = -1;
    if (subvolSelect->m_present)
    {
        subvolNum = (int)myVol->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    AlgorithmVolumeSmoothing(myProgObj, myVol, myKernel, myOutVol, roiVol, fixZeros, subvolNum);
}

AlgorithmVolumeSmoothing::AlgorithmVolumeSmoothing(ProgressObject* myProgObj, const VolumeFile* inVol, const float& kernel, VolumeFile* outVol, const VolumeFile* roiVol, const bool& fixZeros, const int& subvol) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(inVol != NULL);
    CaretAssert(outVol != NULL);
    LevelProgress myProgress(myProgObj);
    if (roiVol != NULL && !inVol->matchesVolumeSpace(roiVol))
    {
        throw AlgorithmException("volume roi space does not match input volume");
    }
    vector<int64_t> myDims;
    inVol->getDimensions(myDims);
    if (subvol < -1 || subvol >= myDims[3])
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    if (kernel <= 0.0f)
    {
        throw AlgorithmException("kernel too small");
    }
    CaretArray<float> scratchFrame(myDims[0] * myDims[1] * myDims[2]);//it could be faster to preinitialize with zeros, then generate a usable voxels list if there is a small ROI...
    float kernBox = kernel * 3.0f;
    vector<vector<float> > volSpace = inVol->getSform();
    Vector3D ivec, jvec, kvec, origin, ijorth, jkorth, kiorth;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2]; origin[0] = volSpace[0][3];
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2]; origin[1] = volSpace[1][3];
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2]; origin[2] = volSpace[2][3];
    const float ORTH_TOLERANCE = 0.001f;//tolerate this much deviation from orthogonal (dot product divided by product of lengths) to use orthogonal assumptions to smooth
    if (abs(ivec.dot(jvec.normal())) / ivec.length() < ORTH_TOLERANCE && abs(jvec.dot(kvec.normal())) / jvec.length() < ORTH_TOLERANCE && abs(kvec.dot(ivec.normal())) / kvec.length() < ORTH_TOLERANCE)
    {//if our axes are orthogonal, optimize by doing three 1-dimensional smoothings for O(voxels * (ki + kj + kk)) instead of O(voxels * (ki * kj * kk))
        CaretArray<float> scratchFrame2(myDims[0] * myDims[1] * myDims[2]), scratchWeights(myDims[0] * myDims[1] * myDims[2]), scratchWeights2(myDims[0] * myDims[1] * myDims[2]), scratchFrame3;
        if (roiVol != NULL)
        {
            scratchFrame3 = CaretArray<float> (myDims[0] * myDims[1] * myDims[2]);
        }
        float ispace = ivec.length(), jspace = jvec.length(), kspace = kvec.length();
        int irange = (int)floor(kernBox / ispace);
        int jrange = (int)floor(kernBox / jspace);
        int krange = (int)floor(kernBox / kspace);
        if (irange < 1) irange = 1;//don't underflow
        if (jrange < 1) jrange = 1;
        if (krange < 1) krange = 1;
        int isize = irange * 2 + 1;//and construct a precomputed kernel in the box
        int jsize = jrange * 2 + 1;
        int ksize = krange * 2 + 1;
        CaretArray<float> iweights(isize), jweights(jsize), kweights(ksize);
        for (int i = 0; i < isize; ++i)
        {
            float tempf = ispace * (i - irange) / kernel;
            iweights[i] = exp(-tempf * tempf / 2.0f);
        }
        for (int j = 0; j < jsize; ++j)
        {
            float tempf = jspace * (j - jrange) / kernel;
            jweights[j] = exp(-tempf * tempf / 2.0f);
        }
        for (int k = 0; k < ksize; ++k)
        {
            float tempf = kspace * (k - krange) / kernel;
            kweights[k] = exp(-tempf * tempf / 2.0f);
        }
        if (subvol == -1)
        {
            vector<int64_t> origDims = inVol->getOriginalDimensions();
            outVol->reinitialize(origDims, volSpace, myDims[4]);
            vector<int> lists[3];
            for (int s = 0; s < myDims[3]; ++s)
            {
                outVol->setMapName(s, inVol->getMapName(s) + ", smooth " + AString::number(kernel));
                for (int c = 0; c < myDims[4]; ++c)
                {
                    const float* inFrame = inVol->getFrame(s, c);
                    if (roiVol == NULL)
                    {
                        smoothFrame(inFrame, myDims, scratchFrame, scratchFrame2, scratchWeights, scratchWeights2, inVol, iweights, jweights, kweights, irange, jrange, krange, fixZeros);
                    } else {
                        smoothFrameROI(inFrame, myDims, scratchFrame, scratchFrame2, scratchFrame3, scratchWeights, scratchWeights2, lists, inVol, roiVol, iweights, jweights, kweights, irange, jrange, krange, fixZeros);
                    }
                    outVol->setFrame(scratchFrame, s, c);
                }
            }
        } else {
            vector<int64_t> origDims = inVol->getOriginalDimensions(), newDims;
            newDims.resize(3);
            newDims[0] = origDims[0];
            newDims[1] = origDims[1];
            newDims[2] = origDims[2];
            outVol->reinitialize(newDims, volSpace, myDims[4]);
            vector<int> lists[3];
            outVol->setMapName(0, inVol->getMapName(subvol) + ", smooth " + AString::number(kernel));
            for (int c = 0; c < myDims[4]; ++c)
            {
                const float* inFrame = inVol->getFrame(subvol, c);
                if (roiVol == NULL)
                {
                    smoothFrame(inFrame, myDims, scratchFrame, scratchFrame2, scratchWeights, scratchWeights2, inVol, iweights, jweights, kweights, irange, jrange, krange, fixZeros);
                } else {
                    smoothFrameROI(inFrame, myDims, scratchFrame, scratchFrame2, scratchFrame3, scratchWeights, scratchWeights2, lists, inVol, roiVol, iweights, jweights, kweights, irange, jrange, krange, fixZeros);
                }
                outVol->setFrame(scratchFrame, 0, c);
            }
        }
    } else {
        if (!haveWarned)
        {
            CaretLogWarning("input volume is not orthogonal, smoothing will take longer");
            haveWarned = true;
        }
        ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius kernBox
        jkorth = jvec.cross(kvec).normal();
        kiorth = kvec.cross(ivec).normal();
        int irange = (int)floor(abs(kernBox / ivec.dot(jkorth)));
        int jrange = (int)floor(abs(kernBox / jvec.dot(kiorth)));
        int krange = (int)floor(abs(kernBox / kvec.dot(ijorth)));
        if (irange < 1) irange = 1;//don't underflow
        if (jrange < 1) jrange = 1;
        if (krange < 1) krange = 1;
        int isize = irange * 2 + 1;//and construct a precomputed kernel in the box
        int jsize = jrange * 2 + 1;
        int ksize = krange * 2 + 1;
        CaretArray<float**> weights(ksize);//so I don't need to explicitly delete[] if I throw
        CaretArray<float*> weights2(ksize * jsize);//construct flat arrays and index them into 3D
        CaretArray<float> weights3(ksize * jsize * isize);//index i comes last because that is linear for volume frames
        Vector3D kscratch, jscratch, iscratch;
        for (int k = 0; k < ksize; ++k)
        {
            kscratch = kvec * (k - krange);
            weights[k] = weights2 + k * jsize;
            for (int j = 0; j < jsize; ++j)
            {
                jscratch = kscratch + jvec * (j - jrange);
                weights[k][j] = weights3 + ((k * jsize) + j) * isize;
                for (int i = 0; i < isize; ++i)
                {
                    iscratch = jscratch + ivec * (i - irange);
                    float tempf = iscratch.length();
                    if (tempf > kernBox)
                    {
                        weights[k][j][i] = 0.0f;//test for zero to avoid some multiplies/adds, cheaper or cleaner than checking bounds on indexes from an index list
                    } else {
                        weights[k][j][i] = exp(-tempf * tempf / kernel / kernel / 2.0f);//optimization here isn't critical
                    }
                }
            }
        }
        if (subvol == -1)
        {
            vector<int64_t> origDims = inVol->getOriginalDimensions();
            outVol->reinitialize(origDims, volSpace, myDims[4]);
            for (int s = 0; s < myDims[3]; ++s)
            {
                outVol->setMapName(s, inVol->getMapName(s) + ", smooth " + AString::number(kernel));
                for (int c = 0; c < myDims[4]; ++c)
                {
                    const float* inFrame = inVol->getFrame(s, c);
                    smoothFrameNonOrth(inFrame, myDims, scratchFrame, inVol, roiVol, weights, irange, jrange, krange, fixZeros);
                    outVol->setFrame(scratchFrame, s, c);
                }
            }
        } else {
            vector<int64_t> origDims = inVol->getOriginalDimensions(), newDims;
            newDims.resize(3);
            newDims[0] = origDims[0];
            newDims[1] = origDims[1];
            newDims[2] = origDims[2];
            outVol->reinitialize(newDims, volSpace, myDims[4]);
            outVol->setMapName(0, inVol->getMapName(subvol) + ", smooth " + AString::number(kernel));
            for (int c = 0; c < myDims[4]; ++c)
            {
                const float* inFrame = inVol->getFrame(subvol, c);
                smoothFrameNonOrth(inFrame, myDims, scratchFrame, inVol, roiVol, weights, irange, jrange, krange, fixZeros);
                outVol->setFrame(scratchFrame, 0, c);
            }
        }
    }
}

void AlgorithmVolumeSmoothing::smoothFrame(const float* inFrame, vector<int64_t> myDims, CaretArray<float> scratchFrame, CaretArray<float> scratchFrame2, CaretArray<float> scratchWeights, CaretArray<float> scratchWeights2, const VolumeFile* inVol, CaretArray<float> iweights, CaretArray<float> jweights, CaretArray<float> kweights, int irange, int jrange, int krange, const bool& fixZeros)
{//this function should ONLY get invoked when the volume is orthogonal (axes are perpendicular, not necessarily aligned with x, y, z, and not necessarily equal spacing)
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int k = 0; k < myDims[2]; ++k)//smooth along i axis
    {
        for (int j = 0; j < myDims[1]; ++j)
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                if (imin < 0) imin = 0;
                if (imax > myDims[0]) imax = myDims[0];
                float sum = 0.0f, weightsum = 0.0f;
                int64_t baseInd = inVol->getIndex(0, j, k, 0);//extra 0 on a default parameter is to prevent int->pointer vs int->int64 conversion ambiguity
                int64_t curInd = baseInd + i;
                for (int ikern = imin; ikern < imax; ++ikern)
                {
                    int64_t thisIndex = baseInd + ikern;
                    if ((!fixZeros || inFrame[thisIndex] != 0.0f))
                    {
                        float weight = iweights[ikern - i + irange];
                        weightsum += weight;
                        sum += weight * inFrame[thisIndex];
                    }
                }
                scratchWeights[curInd] = weightsum;
                scratchFrame[curInd] = sum;//don't divide yet, we will divide later after we gather the weighted sums of the weighted sums of the weight sums (yes, that repetition is right)
            }
        }
    }
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int k = 0; k < myDims[2]; ++k)//now j
    {
        for (int i = 0; i < myDims[0]; ++i)
        {
            for (int j = 0; j < myDims[1]; ++j)//step along the dimension being smoothed last for best cache coherence
            {
                int jmin = j - jrange, jmax = j + jrange + 1;//one-after array size convention
                if (jmin < 0) jmin = 0;
                if (jmax > myDims[1]) jmax = myDims[1];
                float sum = 0.0f, weightsum = 0.0f;
                int64_t baseInd = inVol->getIndex(i, 0, k);
                int64_t curInd = baseInd + j * myDims[0];
                for (int jkern = jmin; jkern < jmax; ++jkern)
                {
                    int64_t thisIndex = baseInd + jkern * myDims[0];
                    float weight = jweights[jkern - j + jrange];
                    weightsum += weight * scratchWeights[thisIndex];
                    sum += weight * scratchFrame[thisIndex];
                }
                scratchWeights2[curInd] = weightsum;
                scratchFrame2[curInd] = sum;//we now have the weighted sum of the weight sums
            }
        }
    }
//somehow this loop set is helped by collapsing the two outers, while the other two are not helped by it
//however, we need to upgrade our windows compiler before we can use "collapse"
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int j = 0; j < myDims[1]; ++j)//and finally k
    {
        for (int i = 0; i < myDims[0]; ++i)
        {
            for (int k = 0; k < myDims[2]; ++k)//ditto
            {
                int64_t baseInd = inVol->getIndex(i, j, 0);
                int64_t curInd = baseInd + k * myDims[0] * myDims[1];
                int kmin = k - krange, kmax = k + krange + 1;//one-after array size convention
                if (kmin < 0) kmin = 0;
                if (kmax > myDims[2]) kmax = myDims[2];
                float sum = 0.0f, weightsum = 0.0f;
                for (int kkern = kmin; kkern < kmax; ++kkern)
                {
                    int64_t thisIndex = baseInd + kkern * myDims[0] * myDims[1];
                    float weight = kweights[kkern - k + krange];
                    weightsum += weight * scratchWeights2[thisIndex];
                    sum += weight * scratchFrame2[thisIndex];
                }
                if (weightsum != 0.0f)
                {
                    scratchFrame[curInd] = sum / weightsum;//NOW we can divide
                } else {
                    scratchFrame[curInd] = 0.0f;
                }
            }
        }
    }
}

void AlgorithmVolumeSmoothing::smoothFrameROI(const float* inFrame, vector<int64_t> myDims, CaretArray<float> scratchFrame, CaretArray<float> scratchFrame2, CaretArray<float> scratchFrame3,
                                              CaretArray<float> scratchWeights, CaretArray<float> scratchWeights2, vector<int> lists[3],
                                              const VolumeFile* inVol, const VolumeFile* roiVol, CaretArray<float> iweights, CaretArray<float> jweights, CaretArray<float> kweights,
                                              int irange, int jrange, int krange, const bool& fixZeros)
{//optimized for orthogonal, plus lists of voxels for ROI smoothing
    if (lists[0].size() == 0)
    {//this is our first time into this function, we must populate the lists
        const float* roiFrame = roiVol->getFrame();
        CaretArray<int> markROI(myDims[0] * myDims[1] * myDims[2], 0);//need a temporary array to sort out ROI zeros from -fix-zeros zeros
#pragma omp CARET_PARFOR
        for (int k = 0; k < myDims[2]; ++k)//smooth along i axis
        {
            for (int j = 0; j < myDims[1]; ++j)
            {
                for (int i = 0; i < myDims[0]; ++i)//don't test whether intermediate voxel is insode ROI, or we lose some data
                {
                    int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                    if (imin < 0) imin = 0;
                    if (imax > myDims[0]) imax = myDims[0];
                    float sum = 0.0f, weightsum = 0.0f;
                    int64_t baseInd = inVol->getIndex(0, j, k, 0);
                    int64_t curInd = baseInd + i;
                    bool used = false;
                    for (int ikern = imin; ikern < imax; ++ikern)
                    {
                        int64_t thisIndex = baseInd + ikern;
                        if (roiFrame[thisIndex] > 0.0f)//only test source and final voxels for being in ROI
                        {
                            if (!used)//keep a list of voxels whose i-kernel intersects the ROI
                            {
                                used = true;
                                markROI[curInd] = 1;
#pragma omp critical
                                {
                                    lists[0].push_back(i);
                                    lists[0].push_back(j);
                                    lists[0].push_back(k);
                                }
                            }
                            if (!fixZeros || inFrame[thisIndex] != 0.0f)
                            {
                                float weight = iweights[ikern - i + irange];
                                weightsum += weight;
                                sum += weight * inFrame[thisIndex];
                            }
                        }
                    }
                    scratchWeights[curInd] = weightsum;
                    scratchFrame3[curInd] = sum;//don't divide yet, we will divide later after we gather the weighted sums of the weighted sums of the weight sums (yes, that repetition is right)
                }
            }
        }
#pragma omp CARET_PARFOR
        for (int k = 0; k < myDims[2]; ++k)//now j
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                for (int j = 0; j < myDims[1]; ++j)//step along the dimension being smoothed last for best cache coherence
                {
                    int jmin = j - jrange, jmax = j + jrange + 1;//one-after array size convention
                    if (jmin < 0) jmin = 0;
                    if (jmax > myDims[1]) jmax = myDims[1];
                    float sum = 0.0f, weightsum = 0.0f;
                    int64_t baseInd = inVol->getIndex(i, 0, k);
                    int64_t curInd = baseInd + j * myDims[0];
                    bool used = false;
                    for (int jkern = jmin; jkern < jmax; ++jkern)
                    {
                        int64_t thisIndex = baseInd + jkern * myDims[0];//DO NOT test for this source voxel being outside ROI, or you will skip good data
                        if ((markROI[thisIndex] & 1) == 1)
                        {//skip voxels whose i-kernels don't touch the ROI, they will always have 0/0
                            if (!used)
                            {
                                used = true;
                                markROI[curInd] |= 2;//bitwise so i can track all 3 lists separately in one array
#pragma omp critical
                                {
                                    lists[1].push_back(i);
                                    lists[1].push_back(j);
                                    lists[1].push_back(k);
                                }
                            }
                            float weight = jweights[jkern - j + jrange];
                            weightsum += weight * scratchWeights[thisIndex];
                            sum += weight * scratchFrame3[thisIndex];
                        }
                    }
                    scratchWeights2[curInd] = weightsum;
                    scratchFrame2[curInd] = sum;//we now have the weighted sum of the weight sums
                }
            }
        }
#pragma omp CARET_PARFOR
        for (int j = 0; j < myDims[1]; ++j)//and finally k
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                for (int k = 0; k < myDims[2]; ++k)//ditto
                {
                    int64_t baseInd = inVol->getIndex(i, j, 0);
                    int64_t curInd = baseInd + k * myDims[0] * myDims[1];
                    if (roiFrame[curInd] > 0.0f)
                    {
#pragma omp critical
                        {
                            lists[2].push_back(i);//third list is a little different, since we don't output stuff outside the ROI, we can drop the voxels that "grew" from the ROI
                            lists[2].push_back(j);//we do need to calculate those grown voxels, though, since we use some of them within the k-kernel
                            lists[2].push_back(k);
                        }
                        int kmin = k - krange, kmax = k + krange + 1;//one-after array size convention
                        if (kmin < 0) kmin = 0;
                        if (kmax > myDims[2]) kmax = myDims[2];
                        float sum = 0.0f, weightsum = 0.0f;
                        for (int kkern = kmin; kkern < kmax; ++kkern)
                        {
                            int64_t thisIndex = baseInd + kkern * myDims[0] * myDims[1];//ditto
                            float weight = kweights[kkern - k + krange];
                            weightsum += weight * scratchWeights2[thisIndex];
                            sum += weight * scratchFrame2[thisIndex];
                        }
                        if (weightsum != 0.0f)
                        {
                            scratchFrame[curInd] = sum / weightsum;//NOW we can divide
                        } else {
                            scratchFrame[curInd] = 0.0f;
                        }
                    } else {
                        scratchFrame[curInd] = 0.0f;
                    }
                }
            }
        }
        if (lists[0].size() == 0)
        {
            lists[0].push_back(-1);//to keep it from scanning the ROI again when the ROI has no voxels, slightly hacky
        }
    } else {//lists already made, use them
        const float* roiFrame = roiVol->getFrame();
        int64_t ibasesize = (int64_t)lists[0].size();
        if (ibasesize < 3) return;//handle the case of empty ROI here (ibasesize will be 1)
        int64_t jbasesize = (int64_t)lists[1].size();
        int64_t kbasesize = (int64_t)lists[2].size();
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int ibase = 0; ibase < ibasesize; ibase += 3)
        {
            int i = lists[0][ibase];
            int j = lists[0][ibase + 1];
            int k = lists[0][ibase + 2];
            int imin = i - irange, imax = i + irange + 1;//one-after array size convention
            if (imin < 0) imin = 0;
            if (imax > myDims[0]) imax = myDims[0];
            float sum = 0.0f, weightsum = 0.0f;
            int64_t baseInd = inVol->getIndex(0, j, k, 0);
            int64_t curInd = baseInd + i;
            for (int ikern = imin; ikern < imax; ++ikern)
            {
                int64_t thisIndex = baseInd + ikern;
                if (roiFrame[thisIndex] > 0.0f && (!fixZeros || inFrame[thisIndex] != 0.0f))
                {
                    float weight = iweights[ikern - i + irange];
                    weightsum += weight;
                    sum += weight * inFrame[thisIndex];
                }
            }
            scratchWeights[curInd] = weightsum;
            scratchFrame3[curInd] = sum;//don't divide yet, we will divide later after we gather the weighted sums of the weighted sums of the weight sums (yes, that repetition is right)
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int jbase = 0; jbase < jbasesize; jbase += 3)
        {
            int i = lists[1][jbase];
            int j = lists[1][jbase + 1];
            int k = lists[1][jbase + 2];
            int jmin = j - jrange, jmax = j + jrange + 1;//one-after array size convention
            if (jmin < 0) jmin = 0;
            if (jmax > myDims[1]) jmax = myDims[1];
            float sum = 0.0f, weightsum = 0.0f;
            int64_t baseInd = inVol->getIndex(i, 0, k);
            int64_t curInd = baseInd + j * myDims[0];
            for (int jkern = jmin; jkern < jmax; ++jkern)
            {
                int64_t thisIndex = baseInd + jkern * myDims[0];//DO NOT test for this source voxel having zero data, or it will skip good data
                float weight = jweights[jkern - j + jrange];
                weightsum += weight * scratchWeights[thisIndex];
                sum += weight * scratchFrame3[thisIndex];
            }
            scratchWeights2[curInd] = weightsum;
            scratchFrame2[curInd] = sum;//we now have the weighted sum of the weight sums
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int kbase = 0; kbase < kbasesize; kbase += 3)
        {
            int i = lists[2][kbase];
            int j = lists[2][kbase + 1];
            int k = lists[2][kbase + 2];
            int64_t baseInd = inVol->getIndex(i, j, 0);
            int64_t curInd = baseInd + k * myDims[0] * myDims[1];
            int kmin = k - krange, kmax = k + krange + 1;//one-after array size convention
            if (kmin < 0) kmin = 0;
            if (kmax > myDims[2]) kmax = myDims[2];
            float sum = 0.0f, weightsum = 0.0f;
            for (int kkern = kmin; kkern < kmax; ++kkern)
            {
                int64_t thisIndex = baseInd + kkern * myDims[0] * myDims[1];//ditto
                float weight = kweights[kkern - k + krange];
                weightsum += weight * scratchWeights2[thisIndex];
                sum += weight * scratchFrame2[thisIndex];
            }
            if (weightsum != 0.0f)
            {
                scratchFrame[curInd] = sum / weightsum;//NOW we can divide
            } else {
                scratchFrame[curInd] = 0.0f;
            }
        }//the frame should be zeroed outside the ROI already due to the first time through
    }
}

void AlgorithmVolumeSmoothing::smoothFrameNonOrth(const float* inFrame, const vector<int64_t>& myDims, CaretArray<float>& scratchFrame, const VolumeFile* inVol, const VolumeFile* roiVol, const CaretArray<float**>& weights, const int& irange, const int& jrange, const int& krange, const bool& fixZeros)
{
    const float* roiFrame = NULL;
    if (roiVol != NULL)
    {
        roiFrame = roiVol->getFrame();
    }
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int k = 0; k < myDims[2]; ++k)
    {
        for (int j = 0; j < myDims[1]; ++j)
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                if (roiVol == NULL || roiVol->getValue(i, j, k) > 0.0f)
                {
                    int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                    if (imin < 0) imin = 0;
                    if (imax > myDims[0]) imax = myDims[0];
                    int jmin = j - jrange, jmax = j + jrange + 1;
                    if (jmin < 0) jmin = 0;
                    if (jmax > myDims[1]) jmax = myDims[1];
                    int kmin = k - krange, kmax = k + krange + 1;
                    if (kmin < 0) kmin = 0;
                    if (kmax > myDims[2]) kmax = myDims[2];
                    float sum = 0.0f, weightsum = 0.0f;
                    for (int kkern = kmin; kkern < kmax; ++kkern)
                    {
                        int64_t kindpart = kkern * myDims[1];
                        int kkernpart = kkern - k + krange;
                        for (int jkern = jmin; jkern < jmax; ++jkern)
                        {
                            int64_t jindpart = (kindpart + jkern) * myDims[0];
                            int jkernpart = jkern - j + jrange;
                            for (int ikern = imin; ikern < imax; ++ikern)
                            {
                                int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                float weight = weights[kkernpart][jkernpart][ikern - i + irange];
                                if (weight != 0.0f && (roiVol == NULL || roiFrame[thisIndex] > 0.0f) && (!fixZeros || inFrame[thisIndex] != 0.0f))
                                {
                                    weightsum += weight;
                                    sum += weight * inFrame[thisIndex];
                                }
                            }
                        }
                    }
                    if (weightsum != 0.0f)
                    {
                        scratchFrame[inVol->getIndex(i, j, k)] = sum / weightsum;
                    } else {
                        scratchFrame[inVol->getIndex(i, j, k)] = 0.0f;
                    }
                } else {
                    scratchFrame[inVol->getIndex(i, j, k)] = 0.0f;
                }
            }
        }
    }
}

float AlgorithmVolumeSmoothing::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeSmoothing::getSubAlgorithmWeight()
{
    return 0.0f;
}
