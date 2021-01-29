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

#include "AlgorithmVolumeParcelResamplingGeneric.h"
#include "AlgorithmException.h"
#include "VolumeFile.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "Vector3D.h"
#include "CaretOMP.h"

#include <vector>
#include <map>
#include <utility>
#include <cmath>

using namespace caret;
using namespace std;

const int FIX_ZEROS_POST_ITERATIONS = 10;//number of times to do the "find remaining zeros and try to fill" code before giving up when -fix-zeros is specified

AString AlgorithmVolumeParcelResamplingGeneric::getCommandSwitch()
{
    return "-volume-parcel-resampling-generic";
}

AString AlgorithmVolumeParcelResamplingGeneric::getShortDescription()
{
    return "SMOOTH AND RESAMPLE VOLUME PARCELS FROM DIFFERENT VOLUME SPACE";
}

OperationParameters* AlgorithmVolumeParcelResamplingGeneric::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input data volume");
    
    ret->addVolumeParameter(2, "cur-parcels", "label volume of where the parcels currently are");
    
    ret->addVolumeParameter(3, "new-parcels", "label volume of where the parcels should be");
    
    ret->addDoubleParameter(4, "kernel", "gaussian kernel size in mm to smooth by during resampling, as sigma by default");
    
    ret->addVolumeOutputParameter(5, "volume-out", "output volume");
    
    ret->createOptionalParameter(8, "-fwhm", "smoothing kernel size is FWHM, not sigma");
    
    ret->createOptionalParameter(6, "-fix-zeros", "treat zero values as not being data");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(7, "-subvolume", "select a single subvolume as input");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Smooths and resamples the region inside each label in cur-parcels to the region of the same label name in new-parcels.  ") +
        "Any voxels in the output label region but outside the input label region will be extrapolated from nearby data.  " +
        "The -fix-zeros option causes the smoothing to not use an input value if it is zero, but still write a smoothed value to the voxel, and after smoothing " +
        "is complete, it will check for any remaining values of zero, and fill them in with extrapolated values.  " +
        "The output volume will use the volume space of new-parcels, which does not need to be in the same volume space as the input."
    );
    return ret;
}

void AlgorithmVolumeParcelResamplingGeneric::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* inVol = myParams->getVolume(1);
    VolumeFile* curLabel = myParams->getVolume(2);
    VolumeFile* newLabel = myParams->getVolume(3);
    float kernel = (float)myParams->getDouble(4);
    if (myParams->getOptionalParameter(8)->m_present)
    {
        kernel = kernel / (2.0f * sqrt(2.0f * log(2.0f)));
    }
    VolumeFile* outVol = myParams->getOutputVolume(5);
    bool fixZeros = false;
    if (myParams->getOptionalParameter(6)->m_present)
    {
        fixZeros = true;
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(7);
    int subvolNum = -1;
    if (subvolSelect->m_present)
    {
        subvolNum = (int)inVol->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    AlgorithmVolumeParcelResamplingGeneric(myProgObj, inVol, curLabel, newLabel, kernel, outVol, fixZeros, subvolNum);
}

AlgorithmVolumeParcelResamplingGeneric::AlgorithmVolumeParcelResamplingGeneric(ProgressObject* myProgObj, const VolumeFile* inVol, const VolumeFile* curLabel, const VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const bool& fixZeros, const int& subvolNum) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(inVol != NULL);
    CaretAssert(curLabel != NULL);
    CaretAssert(newLabel != NULL);
    CaretAssert(outVol != NULL);
    if (!inVol->matchesVolumeSpace(curLabel))
    {
        throw AlgorithmException("input label volume must be in the same space as input data volume");
    }
    if (curLabel->getType() != SubvolumeAttributes::LABEL || newLabel->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("parcel volumes are not of type label");
    }
    if (subvolNum < -1 || subvolNum >= inVol->getNumberOfMaps())
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    vector<pair<int, int> > matchedLabels;
    matchLabels(curLabel, newLabel, matchedLabels);
    if (matchedLabels.size() == 0)
    {
        throw AlgorithmException("no matching labels");
    }
    vector<vector<int64_t> > voxelLists;
    generateVoxelLists(matchedLabels, newLabel, voxelLists);
    int voxelListsSize = (int)voxelLists.size();
    LevelProgress myProgress(myProgObj);
    float kernBox = kernel * 3.0f;
    float kernelMult = -1.0f / kernel / kernel / 2.0f;//precompute the part of the kernel function that doesn't change
    vector<vector<float> > newVolSpace = newLabel->getSform();//copied from volume smoothing, perhaps this should be in a convenience method in VolumeFile
    Vector3D ivecnew, jvecnew, kvecnew, ijorthnew, jkorthnew, kiorthnew;
    ivecnew[0] = newVolSpace[0][0]; jvecnew[0] = newVolSpace[0][1]; kvecnew[0] = newVolSpace[0][2];
    ivecnew[1] = newVolSpace[1][0]; jvecnew[1] = newVolSpace[1][1]; kvecnew[1] = newVolSpace[1][2];
    ivecnew[2] = newVolSpace[2][0]; jvecnew[2] = newVolSpace[2][1]; kvecnew[2] = newVolSpace[2][2];
    ijorthnew = ivecnew.cross(jvecnew).normal();//find the bounding box that encloses a sphere of radius kernBox
    jkorthnew = jvecnew.cross(kvecnew).normal();
    kiorthnew = kvecnew.cross(ivecnew).normal();
    float irangenew = abs(kernBox / ivecnew.dot(jkorthnew));//note, these are in index space
    float jrangenew = abs(kernBox / jvecnew.dot(kiorthnew));
    float krangenew = abs(kernBox / kvecnew.dot(ijorthnew));
    if (irangenew < 1.0f) irangenew = 1.0f;//don't underflow, always use at least a 3x3x3 box
    if (jrangenew < 1.0f) jrangenew = 1.0f;
    if (krangenew < 1.0f) krangenew = 1.0f;
    vector<vector<float> > volSpace = inVol->getSform();//copied from volume smoothing, perhaps this should be in a convenience method in VolumeFile
    Vector3D ivec, jvec, kvec, ijorth, jkorth, kiorth;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2];//needs to be this verbose because the axis and origin vectors are column vectors
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2];//while vector<vector<> > is a column of row vectors
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2];
    ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius kernBox
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    float irange = abs(kernBox / ivec.dot(jkorth));//note, these are in index space
    float jrange = abs(kernBox / jvec.dot(kiorth));
    float krange = abs(kernBox / kvec.dot(ijorth));
    if (irange < 1.0f) irange = 1.0f;//don't underflow, always use at least a 3x3x3 box
    if (jrange < 1.0f) jrange = 1.0f;
    if (krange < 1.0f) krange = 1.0f;
    vector<int64_t> myDims;
    inVol->getDimensions(myDims);
    vector<int64_t> newDims;
    newLabel->getDimensions(newDims);
    if (subvolNum == -1)
    {
        vector<int64_t> outDims = newLabel->getOriginalDimensions(), inDims = inVol->getOriginalDimensions();
        outDims.resize(3);
        for (int i = 3; i < (int)inDims.size(); ++i)
        {
            outDims.push_back(inDims[i]);
        }
        outVol->reinitialize(outDims, newLabel->getSform(), myDims[4], inVol->getType(), inVol->m_header);
    } else {
        vector<int64_t> outDims = newLabel->getOriginalDimensions();
        outDims.resize(3);//discard nonspatial dimentions
        outVol->reinitialize(outDims, newLabel->getSform(), myDims[4], inVol->getType(), inVol->m_header);
    }
    outVol->setValueAllVoxels(0.0f);
    const float* labelFrame = curLabel->getFrame();
    const float* newLabelFrame = newLabel->getFrame();
    CaretArray<float> scratchFrame(newDims[0] * newDims[1] * newDims[2]), scratchFrame2(newDims[0] * newDims[1] * newDims[2]), tempFrame;
    for (int whichList = 0; whichList < voxelListsSize; ++whichList)
    {
        int curLabelValue = matchedLabels[whichList].first;
        int newLabelValue = matchedLabels[whichList].second;
        vector<int64_t>& thisList = voxelLists[whichList];
        int64_t listSize = (int64_t)thisList.size();
        if (subvolNum == -1)
        {
            for (int c = 0; c < myDims[4]; ++c)
            {
                for (int s = 0; s < myDims[3]; ++s)
                {
                    const float* inFrame = inVol->getFrame(s, c);
//#pragma omp CARET_PARFOR schedule(dynamic)
                    for (int64_t base = 0; base < listSize; base += 3)
                    {
                        float sum = 0.0f, weightsum = 0.0f;
                        int i = thisList[base], j = thisList[base + 1], k = thisList[base + 2];
                        float xyz[3], curijk[3];
                        newLabel->indexToSpace(i, j, k, xyz);
                        inVol->spaceToIndex(xyz, curijk);
                        int imin = (int)ceil(curijk[0] - irange), imax = (int)floor(curijk[0] + irange) + 1;
                        if (imin < 0) imin = 0;
                        if (imax > myDims[0]) imax = myDims[0];
                        int jmin = (int)ceil(curijk[1] - jrange), jmax = (int)floor(curijk[1] + jrange) + 1;
                        if (jmin < 0) jmin = 0;
                        if (jmax > myDims[1]) jmax = myDims[1];
                        int kmin = (int)ceil(curijk[2] - krange), kmax = (int)floor(curijk[2] + krange) + 1;
                        if (kmin < 0) kmin = 0;
                        if (kmax > myDims[2]) kmax = myDims[2];
                        for (int kkern = kmin; kkern < kmax; ++kkern)
                        {
                            Vector3D kscratch = kvec * (kkern - curijk[2]);
                            int64_t kindpart = kkern * myDims[1];
                            for (int jkern = jmin; jkern < jmax; ++jkern)
                            {
                                Vector3D jscratch = kscratch + jvec * (jkern - curijk[1]);
                                int64_t jindpart = (kindpart + jkern) * myDims[0];
                                for (int ikern = imin; ikern < imax; ++ikern)
                                {
                                    int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                    int curVal = (int)floor(labelFrame[thisIndex] + 0.5f);
                                    float dataVal = inFrame[thisIndex];
                                    if (curVal == curLabelValue && (!fixZeros || dataVal != 0.0f))
                                    {
                                        Vector3D iscratch = jscratch + ivec * (ikern - curijk[0]);
                                        float tempf = iscratch.length();
                                        float weight = exp(tempf * tempf * kernelMult);
                                        sum += weight * dataVal;
                                        weightsum += weight;
                                    }
                                }
                            }
                        }
                        if (weightsum != 0.0f)
                        {
                            scratchFrame[outVol->getIndex(i, j, k)] = sum / weightsum;
                        } else {
                            scratchFrame[outVol->getIndex(i, j, k)] = 0.0f;
                        }
                    }
                    if (fixZeros)
                    {
                        int fixIter;
                        for (fixIter = 0; fixIter < FIX_ZEROS_POST_ITERATIONS; ++fixIter)
                        {
                            bool again = false;
//#pragma omp CARET_PARFOR schedule(dynamic)
                            for (int64_t base = 0; base < listSize; base += 3)
                            {
                                int i = thisList[base], j = thisList[base + 1], k = thisList[base + 2];
                                int64_t outIndex = outVol->getIndex(i, j, k);
                                float dataVal = scratchFrame[outIndex];
                                if (dataVal == 0.0f)
                                {
                                    float sum = 0.0f, weightsum = 0.0f;
                                    int imin = (int)ceil(i - irangenew), imax = (int)floor(i + irangenew) + 1;
                                    if (imin < 0) imin = 0;
                                    if (imax > newDims[0]) imax = newDims[0];
                                    int jmin = (int)ceil(j - jrangenew), jmax = (int)floor(j + jrangenew) + 1;
                                    if (jmin < 0) jmin = 0;
                                    if (jmax > newDims[1]) jmax = newDims[1];
                                    int kmin = (int)ceil(k - krangenew), kmax = (int)floor(k + krangenew) + 1;
                                    if (kmin < 0) kmin = 0;
                                    if (kmax > newDims[2]) kmax = newDims[2];
                                    for (int kkern = kmin; kkern < kmax; ++kkern)
                                    {
                                        Vector3D kscratch = kvecnew * (kkern - k);
                                        int64_t kindpart = kkern * newDims[1];
                                        for (int jkern = jmin; jkern < jmax; ++jkern)
                                        {
                                            Vector3D jscratch = kscratch + jvecnew * (jkern - j);
                                            int64_t jindpart = (kindpart + jkern) * newDims[0];
                                            for (int ikern = imin; ikern < imax; ++ikern)
                                            {
                                                int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                                int newVal = (int)floor(newLabelFrame[thisIndex] + 0.5f);
                                                float dataVal = scratchFrame[thisIndex];
                                                if (newVal == newLabelValue && (!fixZeros || dataVal != 0.0f))
                                                {
                                                    Vector3D iscratch = jscratch + ivecnew * (ikern - i);
                                                    float tempf = iscratch.length();
                                                    float weight = exp(tempf * tempf * kernelMult);
                                                    sum += weight * dataVal;
                                                    weightsum += weight;
                                                }
                                            }
                                        }
                                    }
                                    if (weightsum != 0.0f)
                                    {
                                        scratchFrame2[outIndex] = sum / weightsum;
                                    } else {
                                        again = true;
                                        scratchFrame2[outIndex] = 0.0f;
                                    }
                                } else {
                                    scratchFrame2[outIndex] = scratchFrame[outIndex];
                                }
                            }
                            tempFrame = scratchFrame;//this is just pointer swapping, CaretArray is not like vector
                            scratchFrame = scratchFrame2;
                            scratchFrame2 = tempFrame;
                            if (!again) break;
                        }
                        if (fixIter == FIX_ZEROS_POST_ITERATIONS)
                        {
                            const GiftiLabelTable* curLabelTable = curLabel->getMapLabelTable(0);
                            CaretLogWarning("unable to fix all zeros in parcel " + curLabelTable->getLabelName(curLabelValue));
                        }
                    }
                    for (int64_t base = 0; base < listSize; base += 3)
                    {
                        int i = thisList[base], j = thisList[base + 1], k = thisList[base + 2];
                        int64_t outIndex = outVol->getIndex(i, j, k);
                        outVol->setValue(scratchFrame[outIndex], i, j, k, s, c);
                    }
                }
            }
        } else {
            for (int c = 0; c < myDims[4]; ++c)
            {
                const float* inFrame = inVol->getFrame(subvolNum, c);
#pragma omp CARET_PARFOR schedule(dynamic)
                for (int64_t base = 0; base < listSize; base += 3)
                {
                    float sum = 0.0f, weightsum = 0.0f;
                    int i = thisList[base], j = thisList[base + 1], k = thisList[base + 2];
                    float xyz[3], curijk[3];
                    newLabel->indexToSpace(i, j, k, xyz);
                    inVol->spaceToIndex(xyz, curijk);
                    int imin = (int)ceil(curijk[0] - irange), imax = (int)floor(curijk[0] + irange) + 1;
                    if (imin < 0) imin = 0;
                    if (imax > myDims[0]) imax = myDims[0];
                    int jmin = (int)ceil(curijk[1] - jrange), jmax = (int)floor(curijk[1] + jrange) + 1;
                    if (jmin < 0) jmin = 0;
                    if (jmax > myDims[1]) jmax = myDims[1];
                    int kmin = (int)ceil(curijk[2] - krange), kmax = (int)floor(curijk[2] + krange) + 1;
                    if (kmin < 0) kmin = 0;
                    if (kmax > myDims[2]) kmax = myDims[2];
                    for (int kkern = kmin; kkern < kmax; ++kkern)
                    {
                        Vector3D kscratch = kvec * (kkern - curijk[2]);
                        int64_t kindpart = kkern * myDims[1];
                        for (int jkern = jmin; jkern < jmax; ++jkern)
                        {
                            Vector3D jscratch = kscratch + jvec * (jkern - curijk[1]);
                            int64_t jindpart = (kindpart + jkern) * myDims[0];
                            for (int ikern = imin; ikern < imax; ++ikern)
                            {
                                int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                int newVal = (int)floor(newLabelFrame[thisIndex] + 0.5f);
                                float dataVal = inFrame[thisIndex];
                                if (newVal == newLabelValue && (!fixZeros || dataVal != 0.0f))
                                {
                                    Vector3D iscratch = jscratch + ivec * (ikern - curijk[0]);
                                    float tempf = iscratch.length();
                                    float weight = exp(tempf * tempf * kernelMult);
                                    sum += weight * dataVal;
                                    weightsum += weight;
                                }
                            }
                        }
                    }
                    if (weightsum != 0.0f)
                    {
                        scratchFrame[outVol->getIndex(i, j, k)] = sum / weightsum;
                    } else {
                        scratchFrame[outVol->getIndex(i, j, k)] = 0.0f;
                    }
                }
                if (fixZeros)
                {
                    int fixIter;
                    for (fixIter = 0; fixIter < FIX_ZEROS_POST_ITERATIONS; ++fixIter)
                    {
                        bool again = false;
#pragma omp CARET_PARFOR schedule(dynamic)
                        for (int64_t base = 0; base < listSize; base += 3)
                        {
                            int i = thisList[base], j = thisList[base + 1], k = thisList[base + 2];
                            int64_t outIndex = outVol->getIndex(i, j, k);
                            float dataVal = scratchFrame[outIndex];
                            if (dataVal == 0.0f)
                            {
                                float sum = 0.0f, weightsum = 0.0f;
                                int imin = (int)ceil(i - irangenew), imax = (int)floor(i + irangenew) + 1;
                                if (imin < 0) imin = 0;
                                if (imax > newDims[0]) imax = newDims[0];
                                int jmin = (int)ceil(j - jrangenew), jmax = (int)floor(j + jrangenew) + 1;
                                if (jmin < 0) jmin = 0;
                                if (jmax > newDims[1]) jmax = newDims[1];
                                int kmin = (int)ceil(k - krangenew), kmax = (int)floor(k + krangenew) + 1;
                                if (kmin < 0) kmin = 0;
                                if (kmax > newDims[2]) kmax = newDims[2];
                                for (int kkern = kmin; kkern < kmax; ++kkern)
                                {
                                    Vector3D kscratch = kvecnew * (kkern - k);
                                    int64_t kindpart = kkern * newDims[1];
                                    for (int jkern = jmin; jkern < jmax; ++jkern)
                                    {
                                        Vector3D jscratch = kscratch + jvecnew * (jkern - j);
                                        int64_t jindpart = (kindpart + jkern) * newDims[0];
                                        for (int ikern = imin; ikern < imax; ++ikern)
                                        {
                                            int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                            int curVal = (int)floor(labelFrame[thisIndex] + 0.5f);
                                            float dataVal = scratchFrame[thisIndex];
                                            if (curVal == curLabelValue && (!fixZeros || dataVal != 0.0f))
                                            {
                                                Vector3D iscratch = jscratch + ivecnew * (ikern - i);
                                                float tempf = iscratch.length();
                                                float weight = exp(tempf * tempf * kernelMult);
                                                sum += weight * dataVal;
                                                weightsum += weight;
                                            }
                                        }
                                    }
                                }
                                if (weightsum != 0.0f)
                                {
                                    scratchFrame2[outIndex] = sum / weightsum;
                                } else {
                                    again = true;
                                    scratchFrame2[outIndex] = 0.0f;
                                }
                            } else {
                                scratchFrame2[outIndex] = scratchFrame[outIndex];
                            }
                        }
                        tempFrame = scratchFrame;//this is just pointer swapping, CaretArray is not like vector
                        scratchFrame = scratchFrame2;
                        scratchFrame2 = tempFrame;
                        if (!again) break;
                    }
                    if (fixIter == FIX_ZEROS_POST_ITERATIONS)
                    {
                        const GiftiLabelTable* curLabelTable = curLabel->getMapLabelTable(0);
                        CaretLogWarning("unable to fix all zeros in parcel " + curLabelTable->getLabelName(curLabelValue));
                    }
                }
                for (int64_t base = 0; base < listSize; base += 3)
                {
                    int i = thisList[base], j = thisList[base + 1], k = thisList[base + 2];
                    int64_t outIndex = outVol->getIndex(i, j, k);
                    outVol->setValue(scratchFrame[outIndex], i, j, k, 0, c);
                }
            }
        }
    }
}

void AlgorithmVolumeParcelResamplingGeneric::matchLabels(const VolumeFile* curLabel, const VolumeFile* newLabel, vector<pair<int, int> >& matchedLabels)
{
    const GiftiLabelTable* curTable = curLabel->getMapLabelTable(0), *newTable = newLabel->getMapLabelTable(0);
    vector<int32_t> curKeys;
    curTable->getKeys(curKeys);
    int32_t curUnused = curTable->getUnassignedLabelKey();
    for (int i = 0; i < (int)curKeys.size(); ++i)
    {
        if (curKeys[i] == curUnused) continue;//always skip the unassigned label
        if (newTable->getLabel(curKeys[i]) != NULL && newTable->getLabelName(curKeys[i]) == curTable->getLabelName(curKeys[i]))
        {//do the obvious check first
            matchedLabels.push_back(make_pair(curKeys[i], curKeys[i]));
        } else {
            int32_t newKey = newTable->getLabelKeyFromName(curTable->getLabelName(curKeys[i]));
            if (newKey != -1)
            {
                matchedLabels.push_back(make_pair(curKeys[i], newKey));
            }
        }
    }
}

void AlgorithmVolumeParcelResamplingGeneric::generateVoxelLists(const vector<pair<int, int> >& matchedLabels, const VolumeFile* newLabel, vector<vector<int64_t> >& voxelLists)
{
    map<int, int> newLabelReverse;
    for (int i = 0; i < (int)matchedLabels.size(); ++i)
    {
        newLabelReverse[matchedLabels[i].second] = i;
    }
    voxelLists.resize(matchedLabels.size());
    vector<int64_t> myDims;
    newLabel->getDimensions(myDims);
    for (int64_t k = 0; k < myDims[2]; ++k)
    {
        for (int64_t j = 0; j < myDims[1]; ++j)
        {
            for (int64_t i = 0; i < myDims[0]; ++i)
            {
                int newValue = (int)floor(newLabel->getValue(i, j, k) + 0.5f);
                map<int, int>::iterator newiter = newLabelReverse.find(newValue);
                if (newiter != newLabelReverse.end())
                {
                    voxelLists[newiter->second].push_back(i);
                    voxelLists[newiter->second].push_back(j);
                    voxelLists[newiter->second].push_back(k);
                }
            }
        }
    }
}

float AlgorithmVolumeParcelResamplingGeneric::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeParcelResamplingGeneric::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
