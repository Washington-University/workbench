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

#include "AlgorithmVolumeExtrema.h"
#include "AlgorithmException.h"
#include "AlgorithmVolumeSmoothing.h"
#include "CaretHeap.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "VolumeFile.h"
#include <cmath>
#include <utility>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeExtrema::getCommandSwitch()
{
    return "-volume-extrema";
}

AString AlgorithmVolumeExtrema::getShortDescription()
{
    return "FIND EXTREMA IN A VOLUME FILE";
}

OperationParameters* AlgorithmVolumeExtrema::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "volume file to find the extrema of");
    
    ret->addDoubleParameter(2, "distance", "the minimum distance between identified extrema of the same type");
    
    ret->addVolumeOutputParameter(3, "volume-out", "the output extrema volume");
    
    OptionalParameter* presmoothOpt = ret->createOptionalParameter(4, "-presmooth", "smooth the volume before finding extrema");
    presmoothOpt->addDoubleParameter(1, "kernel", "the sigma for the gaussian smoothing kernel, in mm");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-roi", "ignore values outside the selected area");
    roiOpt->addVolumeParameter(1, "roi-volume", "the area to find extrema in");
    
    OptionalParameter* thresholdOpt = ret->createOptionalParameter(6, "-threshold", "ignore small extrema");
    thresholdOpt->addDoubleParameter(1, "low", "the largest value to consider for being a minimum");
    thresholdOpt->addDoubleParameter(2, "high", "the smallest value to consider for being a maximum");
    
    ret->createOptionalParameter(7, "-sum-subvols", "output the sum of the extrema subvolumes instead of each subvolume separately");
    
    ret->createOptionalParameter(8, "-consolidate-mode", "use consolidation of local minima instead of a large neighborhood");
    
    ret->createOptionalParameter(10, "-only-maxima", "only find the maxima");
    
    ret->createOptionalParameter(11, "-only-minima", "only find the minima");
    
    OptionalParameter* subvolOpt = ret->createOptionalParameter(9, "-subvolume", "select a single subvolume to find extrema in");
    subvolOpt->addStringParameter(1, "subvolume", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Finds extrema in a volume file, such that no two extrema of the same type are within <distance> of each other.  ") +
        "The extrema are labeled as -1 for minima, 1 for maxima, 0 otherwise.  " +
        "If -only-maxima or -only-minima is specified, then it will ignore extrema not of the specified type.  These options are mutually exclusive.\n\n" +
        "If -sum-subvols is specified, these extrema subvolumes are summed, and the output has a single subvolume with this result.\n\n" +
        "By default, a datapoint is an extrema only if it is more extreme than every other datapoint that is within <distance> from it.  " +
        "If -consolidate-mode is used, it instead starts by finding all datapoints that are more extreme than their immediate neighbors, " +
        "then while there are any extrema within <distance> of each other, take the two extrema closest to each other and merge them into one by a weighted average " +
        "based on how many original extrema have been merged into each.\n\n" +
        "By default, all input subvolumes are used with no smoothing, use -subvolume to specify a single subvolume to use, and -presmooth to smooth the input before " +
        "finding the extrema."
    );
    return ret;
}

void AlgorithmVolumeExtrema::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVolIn = myParams->getVolume(1);
    float distance = (float)myParams->getDouble(2);
    VolumeFile* myVolOut = myParams->getOutputVolume(3);
    OptionalParameter* presmoothOpt = myParams->getOptionalParameter(4);
    float presmooth = -1.0f;
    if (presmoothOpt->m_present)
    {
        presmooth = presmoothOpt->getDouble(1);
        if (presmooth <= 0.0f)
        {
            throw AlgorithmException("smoothing kernel must be positive");
        }
    }
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    VolumeFile* myRoi = NULL;
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getVolume(1);
    }
    OptionalParameter* thresholdOpt = myParams->getOptionalParameter(6);
    bool thresholdMode = false;
    float lowThresh = 0.0f, highThresh = 0.0f;
    if (thresholdOpt->m_present)
    {
        thresholdMode = true;
        lowThresh = (float)thresholdOpt->getDouble(1);
        highThresh = (float)thresholdOpt->getDouble(2);
    }
    bool sumSubvols = myParams->getOptionalParameter(7)->m_present;
    bool consolidateMode = myParams->getOptionalParameter(8)->m_present;
    OptionalParameter* subvolOpt = myParams->getOptionalParameter(9);
    int subvol = -1;
    if (subvolOpt->m_present)
    {//set up to use the single column
        subvol = (int)myVolIn->getMapIndexFromNameOrNumber(subvolOpt->getString(1));
        if (subvol < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    bool ignoreMinima = myParams->getOptionalParameter(10)->m_present;
    bool ignoreMaxima = myParams->getOptionalParameter(11)->m_present;
    if (ignoreMinima && ignoreMaxima) throw AlgorithmException("you may not specify both -only-maxima and -only-minima");
    if (thresholdMode)
    {
        AlgorithmVolumeExtrema(myProgObj, myVolIn, distance, myVolOut, lowThresh, highThresh, myRoi, presmooth, sumSubvols, consolidateMode, ignoreMinima, ignoreMaxima, subvol);
    } else {
        AlgorithmVolumeExtrema(myProgObj, myVolIn, distance, myVolOut, myRoi, presmooth, sumSubvols, consolidateMode, ignoreMinima, ignoreMaxima, subvol);
    }
}

AlgorithmVolumeExtrema::AlgorithmVolumeExtrema(ProgressObject* myProgObj, const VolumeFile* myVolIn, const float& distance, VolumeFile* myVolOut,
                                               const VolumeFile* myRoi, const float& presmooth, const bool& sumSubvols, const bool& consolidateMode,
                                               bool ignoreMinima, bool ignoreMaxima, const int& subvol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ignoreMinima && ignoreMaxima) throw AlgorithmException("AlgorithmVolumeExtrema called with ignoreMinima and ignoreMaxima both true");
    if (myRoi != NULL && !myVolIn->matchesVolumeSpace(myRoi))
    {
        throw AlgorithmException("roi doesn't match input volume space");
    }
    vector<int64_t> myDims;
    myVolIn->getDimensions(myDims);
    if (subvol < -1 || subvol >= myDims[3])
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    int useSubvol = subvol;
    const VolumeFile* toProcess = myVolIn;
    VolumeFile tempVol;
    if (presmooth > 0.0f)
    {
        AlgorithmVolumeSmoothing(NULL, myVolIn, presmooth, &tempVol, myRoi, false, subvol);
        toProcess = &tempVol;
        if (subvol != -1)
        {
            useSubvol = 0;
        }
    }
    if (!consolidateMode)
    {
        precomputeStencil(myVolIn, distance);
    }
    if (subvol == -1)
    {
        vector<VoxelIJK> minima, maxima;
        vector<int64_t> outDims = myDims;
        outDims.resize(4);
        if (sumSubvols)
        {
            outDims[3] = 1;
        }
        myVolOut->reinitialize(outDims, myVolIn->getSform(), myDims[4], SubvolumeAttributes::ANATOMY, myVolIn->m_header);
        if (sumSubvols)
        {
            myVolOut->setMapName(0, "sum of extrema");
        }
        myVolOut->setValueAllVoxels(0.0f);
        for (int s = 0; s < myDims[3]; ++s)
        {
            for (int c = 0; c < myDims[4]; ++c)
            {
                if (consolidateMode)
                {
                    findExtremaConsolidate(toProcess, s, c, myRoi, distance, false, 0.0f, 0.0f, ignoreMinima, ignoreMaxima, minima, maxima);
                } else {
                    findExtremaStencils(toProcess, s, c, myRoi, false, 0.0f, 0.0f, ignoreMinima, ignoreMaxima, minima, maxima);
                }
                if (sumSubvols)
                {
                    int64_t numElems = (int64_t)minima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(myVolOut->getValue(minima[i].m_ijk, 0, c) - 1.0f, minima[i].m_ijk, 0, c);
                    }
                    numElems = (int64_t)maxima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(myVolOut->getValue(maxima[i].m_ijk, 0, c) + 1.0f, maxima[i].m_ijk, 0, c);
                    }
                } else {
                    if (c == 0) myVolOut->setMapName(s, "extrema of " + myVolIn->getMapName(s));
                    int64_t numElems = (int64_t)minima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(-1.0f, minima[i].m_ijk, s, c);
                    }
                    numElems = (int64_t)maxima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(1.0f, maxima[i].m_ijk, s, c);
                    }
                }
            }
        }
    } else {
        vector<VoxelIJK> minima, maxima;
        vector<int64_t> outDims = myDims;
        outDims.resize(3);
        myVolOut->reinitialize(outDims, myVolIn->getSform(), myDims[4], SubvolumeAttributes::ANATOMY, myVolIn->m_header);
        myVolOut->setMapName(0, "extrema of " + myVolIn->getMapName(subvol));
        myVolOut->setValueAllVoxels(0.0f);
        for (int c = 0; c < myDims[4]; ++c)
        {
            if (consolidateMode)
            {
                findExtremaConsolidate(toProcess, useSubvol, c, myRoi, distance, false, 0.0f, 0.0f, ignoreMinima, ignoreMaxima, minima, maxima);
            } else {
                findExtremaStencils(toProcess, useSubvol, c, myRoi, false, 0.0f, 0.0f, ignoreMinima, ignoreMaxima, minima, maxima);
            }
            int64_t numElems = (int64_t)minima.size();
            for (int64_t i = 0; i < numElems; ++i)
            {
                myVolOut->setValue(-1.0f, minima[i].m_ijk, 0, c);
            }
            numElems = (int64_t)maxima.size();
            for (int64_t i = 0; i < numElems; ++i)
            {
                myVolOut->setValue(1.0f, maxima[i].m_ijk, 0, c);
            }
        }
    }
}

AlgorithmVolumeExtrema::AlgorithmVolumeExtrema(ProgressObject* myProgObj, const VolumeFile* myVolIn, const float& distance, VolumeFile* myVolOut,
                                               const float& lowThresh, const float& highThresh, const VolumeFile* myRoi, const float& presmooth,
                                               const bool& sumSubvols, const bool& consolidateMode, bool ignoreMinima, bool ignoreMaxima, const int& subvol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (ignoreMinima && ignoreMaxima) throw AlgorithmException("AlgorithmVolumeExtrema called with ignoreMinima and ignoreMaxima both true");
    if (myRoi != NULL && !myVolIn->matchesVolumeSpace(myRoi))
    {
        throw AlgorithmException("roi doesn't match input volume space");
    }
    vector<int64_t> myDims;
    myVolIn->getDimensions(myDims);
    if (subvol < -1 || subvol >= myDims[3])
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    int useSubvol = subvol;
    const VolumeFile* toProcess = myVolIn;
    VolumeFile tempVol;
    if (presmooth > 0.0f)
    {
        AlgorithmVolumeSmoothing(NULL, myVolIn, presmooth, &tempVol, myRoi, false, subvol);
        toProcess = &tempVol;
        if (subvol != -1)
        {
            useSubvol = 0;
        }
    }
    if (!consolidateMode)
    {
        precomputeStencil(myVolIn, distance);
    }
    if (subvol == -1)
    {
        vector<VoxelIJK> minima, maxima;
        vector<int64_t> outDims = myDims;
        outDims.resize(4);
        if (sumSubvols)
        {
            outDims[3] = 1;
        }
        myVolOut->reinitialize(outDims, myVolIn->getSform(), myDims[4]);
        if (sumSubvols)
        {
            myVolOut->setMapName(0, "sum of extrema");
        }
        myVolOut->setValueAllVoxels(0.0f);
        for (int s = 0; s < myDims[3]; ++s)
        {
            for (int c = 0; c < myDims[4]; ++c)
            {
                if (consolidateMode)
                {
                    findExtremaConsolidate(toProcess, s, c, myRoi, distance, true, lowThresh, highThresh, ignoreMinima, ignoreMaxima, minima, maxima);
                } else {
                    findExtremaStencils(toProcess, s, c, myRoi, true, lowThresh, highThresh, ignoreMinima, ignoreMaxima, minima, maxima);
                }
                if (sumSubvols)
                {
                    int64_t numElems = (int64_t)minima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(myVolOut->getValue(minima[i].m_ijk, 0, c) - 1.0f, minima[i].m_ijk, 0, c);
                    }
                    numElems = (int64_t)maxima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(myVolOut->getValue(maxima[i].m_ijk, 0, c) + 1.0f, maxima[i].m_ijk, 0, c);
                    }
                } else {
                    if (c == 0) myVolOut->setMapName(s, "extrema of " + myVolIn->getMapName(s));
                    int64_t numElems = (int64_t)minima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(-1.0f, minima[i].m_ijk, s, c);
                    }
                    numElems = (int64_t)maxima.size();
                    for (int64_t i = 0; i < numElems; ++i)
                    {
                        myVolOut->setValue(1.0f, maxima[i].m_ijk, s, c);
                    }
                }
            }
        }
    } else {
        vector<VoxelIJK> minima, maxima;
        vector<int64_t> outDims = myDims;
        outDims.resize(3);
        myVolOut->reinitialize(outDims, myVolIn->getSform(), myDims[4]);
        myVolOut->setMapName(0, "extrema of " + myVolIn->getMapName(subvol));
        myVolOut->setValueAllVoxels(0.0f);
        for (int c = 0; c < myDims[4]; ++c)
        {
            if (consolidateMode)
            {
                findExtremaConsolidate(toProcess, useSubvol, c, myRoi, distance, true, lowThresh, highThresh, ignoreMinima, ignoreMaxima, minima, maxima);
            } else {
                findExtremaStencils(toProcess, useSubvol, c, myRoi, true, lowThresh, highThresh, ignoreMinima, ignoreMaxima, minima, maxima);
            }
            int64_t numElems = (int64_t)minima.size();
            for (int64_t i = 0; i < numElems; ++i)
            {
                myVolOut->setValue(-1.0f, minima[i].m_ijk, 0, c);
            }
            numElems = (int64_t)maxima.size();
            for (int64_t i = 0; i < numElems; ++i)
            {
                myVolOut->setValue(1.0f, maxima[i].m_ijk, 0, c);
            }
        }
    }
}

void AlgorithmVolumeExtrema::precomputeStencil(const VolumeFile* myVolIn, const float& distance)
{
    m_stencil.clear();
    const vector<vector<float> >& volSpace = myVolIn->getSform();
    Vector3D ivec, jvec, kvec, origin;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2]; origin[0] = volSpace[0][3];
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2]; origin[1] = volSpace[1][3];
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2]; origin[2] = volSpace[2][3];
    Vector3D ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius kernBox
    Vector3D jkorth = jvec.cross(kvec).normal();
    Vector3D kiorth = kvec.cross(ivec).normal();
    m_irange = (int)floor(abs(distance / ivec.dot(jkorth)));//these are member variables so we can avoid testing for things outside the volume bounding box when the stencil is
    m_jrange = (int)floor(abs(distance / jvec.dot(kiorth)));// guaranteed within the bounding box
    m_krange = (int)floor(abs(distance / kvec.dot(ijorth)));
    if (m_irange < 1) m_irange = 1;//don't underflow
    if (m_jrange < 1) m_jrange = 1;
    if (m_krange < 1) m_krange = 1;
    float dist2 = distance * distance;//don't require square root calls
    VoxelIJK tempVox;
    bool ichange = false, jchange = false, kchange = false;//ensure that stencil is 3D, not degenerate
    for (int k = -m_krange; k <= m_krange; ++k)//generate the stencil list in the same index order as the volume file
    {
        Vector3D kpart = k * kvec;
        tempVox.m_ijk[2] = k;
        for (int j = -m_jrange; j <= m_jrange; ++j)
        {
            Vector3D jpart = (j * jvec) + kpart;//I could do an extra test here, or recompute the range, but we only do this once, so whatever
            tempVox.m_ijk[1] = j;
            for (int i = -m_irange; i <= m_irange; ++i)
            {
                if (k == 0 && j == 0 && i == 0) continue;//skip the center voxel
                if ((jpart + (i * ivec)).lengthsquared() <= dist2)
                {
                    tempVox.m_ijk[0] = i;
                    m_stencil.push_back(tempVox);
                    if (k != 0) kchange = true;
                    if (j != 0) jchange = true;
                    if (i != 0) ichange = true;
                }
            }
        }
    }
    if (!ichange || !jchange || !kchange)
    {
        CaretLogWarning("distance too small, stencil did not use all 3 dimensions, substituting in 6-neighbor stencil");
        m_irange = 1;
        m_jrange = 1;
        m_krange = 1;
        m_stencil.clear();
        m_stencil.push_back(VoxelIJK(-1, 0, 0));
        m_stencil.push_back(VoxelIJK(0, -1, 0));
        m_stencil.push_back(VoxelIJK(0, 0, -1));
        m_stencil.push_back(VoxelIJK(0, 0, 1));
        m_stencil.push_back(VoxelIJK(0, 1, 0));
        m_stencil.push_back(VoxelIJK(1, 0, 0));
    }
}

void AlgorithmVolumeExtrema::findExtremaStencils(const VolumeFile* toProcess, const int& s, const int& c, const VolumeFile* myRoi, const bool& threshMode, const float& lowThresh,
                                                 const float& highThresh, bool ignoreMinima, bool ignoreMaxima, vector<VoxelIJK>& minima, vector<VoxelIJK>& maxima)
{
    minima.clear();
    maxima.clear();
    vector<int64_t> myDims;
    toProcess->getDimensions(myDims);
    int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
    int stencilSize = (int)m_stencil.size();
    vector<int> minPos(frameSize, 1), maxPos(frameSize, 1);//mark things off that fail a comparison, to reduce redundant comparisons
    const float* dataFrame = toProcess->getFrame(s, c);
    const float* roiFrame = NULL;
    if (myRoi != NULL)
    {
        roiFrame = myRoi->getFrame();
    }
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t k = 0; k < myDims[2]; ++k)
    {
        bool ksafe = (k >= m_krange && k < myDims[2] - m_krange);//so we can avoid performing certain bounds checks in the inner loop
        for (int64_t j = 0; j < myDims[1]; ++j)
        {
            bool jsafe = (j >= m_jrange && j < myDims[1] - m_jrange);
            for (int64_t i = 0; i < myDims[0]; ++i)
            {
                int64_t myindex = toProcess->getIndex(i, j, k);
                if (roiFrame == NULL || roiFrame[myindex] > 0.0f)
                {
                    bool canBeMin = minPos[myindex] && !ignoreMinima;
                    bool canBeMax = maxPos[myindex] && !ignoreMaxima;
                    float myval = dataFrame[myindex];
                    if (threshMode)
                    {
                        if (myval > lowThresh) canBeMin = false;//check thresholds
                        if (myval < highThresh) canBeMax = false;
                    }
                    if (canBeMin || canBeMax)
                    {
                        bool isafe = (i >= m_jrange && i < myDims[0] - m_irange);
                        if (isafe && jsafe && ksafe)//special case out the inner loops to avoid branching when possible
                        {
                            if (roiFrame == NULL)
                            {
                                int v = 0;
                                if (canBeMin && canBeMax)//no ROI and safe means we can just use the first element in stencil to break the double test
                                {
                                    VoxelIJK& offset = m_stencil[0];
                                    int64_t testVox[3] = { i + offset.m_ijk[0],
                                                        j + offset.m_ijk[1],
                                                        k + offset.m_ijk[2] };
                                    int64_t otherindex = toProcess->getIndex(testVox);
                                    float otherval = dataFrame[otherindex];
                                    if (myval < otherval)
                                    {
                                        minPos[otherindex] = 0;
                                    } else {
                                        canBeMin = false;
                                    }
                                    if (myval > otherval)
                                    {
                                        maxPos[otherindex] = 0;
                                    } else {
                                        canBeMax = false;
                                    }
                                    v = 1;
                                }
                                if (canBeMin)
                                {
                                    for (; v < stencilSize; ++v)
                                    {
                                        VoxelIJK& offset = m_stencil[v];
                                        int64_t testVox[3] = { i + offset.m_ijk[0],
                                                            j + offset.m_ijk[1],
                                                            k + offset.m_ijk[2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        float otherval = dataFrame[otherindex];
                                        if (myval < otherval)
                                        {
                                            minPos[otherindex] = 0;
                                        } else {
                                            canBeMin = false;
                                            break;
                                        }
                                    }
                                }
                                if (canBeMax)
                                {
                                    for (; v < stencilSize; ++v)
                                    {
                                        VoxelIJK& offset = m_stencil[v];
                                        int64_t testVox[3] = { i + offset.m_ijk[0],
                                                            j + offset.m_ijk[1],
                                                            k + offset.m_ijk[2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        float otherval = dataFrame[otherindex];
                                        if (myval > otherval)
                                        {
                                            maxPos[otherindex] = 0;
                                        } else {
                                            canBeMax = false;
                                            break;
                                        }
                                    }
                                }
                            } else {
                                int v = 0;
                                if (canBeMin && canBeMax)//we have an roi, so we may need to loop before the double test hits a valid neighbor
                                {
                                    for (; v < stencilSize; ++v)
                                    {
                                        VoxelIJK& offset = m_stencil[v];
                                        int64_t testVox[3] = { i + offset.m_ijk[0],
                                                            j + offset.m_ijk[1],
                                                            k + offset.m_ijk[2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        if (roiFrame[otherindex] > 0.0f)
                                        {
                                            float otherval = dataFrame[otherindex];
                                            if (myval < otherval)
                                            {
                                                minPos[otherindex] = 0;
                                            } else {
                                                canBeMin = false;
                                            }
                                            if (myval > otherval)
                                            {
                                                maxPos[otherindex] = 0;
                                            } else {
                                                canBeMax = false;
                                            }
                                            ++v;//don't test the same voxel again
                                            break;//we have eliminated one possibility, so we can move to a loop with fewer tests
                                        } else {
                                            if (abs(offset.m_ijk[0]) + abs(offset.m_ijk[1]) + abs(offset.m_ijk[2]) == 1)
                                            {
                                                canBeMax = false;//if we find a face neighbor outside the roi, don't count this as an extrema
                                                canBeMin = false;
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (canBeMin)
                                {
                                    for (; v < stencilSize; ++v)
                                    {
                                        VoxelIJK& offset = m_stencil[v];
                                        int64_t testVox[3] = { i + offset.m_ijk[0],
                                                            j + offset.m_ijk[1],
                                                            k + offset.m_ijk[2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        if (roiFrame[otherindex] > 0.0f)
                                        {
                                            float otherval = dataFrame[otherindex];
                                            if (myval < otherval)
                                            {
                                                minPos[otherindex] = 0;
                                            } else {
                                                canBeMin = false;
                                                break;
                                            }
                                        } else {
                                            if (abs(offset.m_ijk[0]) + abs(offset.m_ijk[1]) + abs(offset.m_ijk[2]))
                                            {
                                                canBeMin = false;//if we find a face neighbor outside the roi, don't count this as an extrema
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (canBeMax)
                                {
                                    for (; v < stencilSize; ++v)
                                    {
                                        VoxelIJK& offset = m_stencil[v];
                                        int64_t testVox[3] = { i + offset.m_ijk[0],
                                                            j + offset.m_ijk[1],
                                                            k + offset.m_ijk[2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        if (roiFrame[otherindex] > 0.0f)
                                        {
                                            float otherval = dataFrame[otherindex];
                                            if (myval > otherval)
                                            {
                                                maxPos[otherindex] = 0;
                                            } else {
                                                canBeMax = false;
                                                break;
                                            }
                                        } else {
                                            if (abs(offset.m_ijk[0]) + abs(offset.m_ijk[1]) + abs(offset.m_ijk[2]) == 1)
                                            {
                                                canBeMax = false;//if we find a face neighbor outside the roi, don't count this as an extrema
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (canBeMin && canBeMax)//only way for this to happen is if there are no neighbors in the roi
                                {
                                    canBeMax = false;//don't count isolated voxels as extrema
                                    canBeMin = false;
                                }
                            }
                        } else {//if we are near an edge, we have to check all the neighbor indexes
                            if (i < 1 || i >= myDims[0] - 1 ||
                                j < 1 || j >= myDims[1] - 1 ||
                                k < 1 || k >= myDims[2] - 1)
                            {
                                canBeMax = false;//but if we are on the edge of the volume, that is effectively the same as on the edge of the roi
                                canBeMin = false;//so, don't count any extrema here
                            } else {
                                for (int v = 0; v < stencilSize; ++v)
                                {
                                    VoxelIJK& offset = m_stencil[v];
                                    int64_t testVox[3] = { i + offset.m_ijk[0],
                                                        j + offset.m_ijk[1],
                                                        k + offset.m_ijk[2] };
                                    if (!ksafe && (testVox[2] < 0 || testVox[2] >= myDims[2])) continue;
                                    if (!jsafe && (testVox[1] < 0 || testVox[1] >= myDims[1])) continue;
                                    if (!isafe && (testVox[0] < 0 || testVox[0] >= myDims[0])) continue;
                                    int64_t otherindex = toProcess->getIndex(testVox);
                                    if (roiFrame != NULL && roiFrame[otherindex] <= 0.0f)
                                    {
                                        if (abs(offset.m_ijk[0]) + abs(offset.m_ijk[1]) + abs(offset.m_ijk[2]) == 1)
                                        {
                                            canBeMax = false;
                                            canBeMin = false;
                                            break;
                                        } else {
                                            continue;
                                        }
                                    }
                                    float otherval = dataFrame[otherindex];
                                    if (myval < otherval)//since we are checking index bounds anyway, just do the double test to make the code simpler
                                    {
                                        minPos[otherindex] = 0;
                                    } else {
                                        canBeMin = false;
                                        if (!canBeMax) break;
                                    }
                                    if (myval > otherval)
                                    {
                                        maxPos[otherindex] = 0;
                                    } else {
                                        canBeMax = false;
                                        if (!canBeMin) break;
                                    }
                                }
                            }
                            /*if (canBeMin && canBeMax)//this can't happen anymore
                            {
                                canBeMax = false;//don't count isolated voxels as extrema
                                canBeMin = false;
                            }//*/
                        }
                        if (canBeMin)
                        {
#pragma omp critical
                            {
                                minima.push_back(VoxelIJK(i, j, k));
                            }
                        }
                        if (canBeMax)
                        {
#pragma omp critical
                            {
                                maxima.push_back(VoxelIJK(i, j, k));
                            }
                        }
                    }
                }
            }
        }
    }
}

void AlgorithmVolumeExtrema::findExtremaConsolidate(const VolumeFile* toProcess, const int& s, const int& c, const VolumeFile* myRoi, const float& distance,
                                                    const bool& threshMode, const float& lowThresh, const float& highThresh, bool ignoreMinima, bool ignoreMaxima, vector<VoxelIJK>& minima, vector<VoxelIJK>& maxima)
{
    const int stencil[18] = {-1, 0, 0,
                             0, -1, 0,
                             0, 0, -1,
                             0, 0, 1,
                             0, 1, 0,
                             1, 0, 0};
    const int STENCIL_SIZE = 18;
    minima.clear();
    maxima.clear();
    vector<int64_t> myDims;
    toProcess->getDimensions(myDims);
    int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
    vector<int> minPos(frameSize, 1), maxPos(frameSize, 1);//mark things off that fail a comparison, to reduce redundant comparisons
    const float* dataFrame = toProcess->getFrame(s, c);
    const float* roiFrame = NULL;
    vector<pair<Vector3D, int> > tempExtrema[2];
    if (myRoi != NULL)
    {
        roiFrame = myRoi->getFrame();
    }
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t k = 0; k < myDims[2]; ++k)
    {
        bool ksafe = (k > 0 && k < myDims[2] - 1);//so we can avoid performing certain bounds checks in the inner loop
        for (int64_t j = 0; j < myDims[1]; ++j)
        {
            bool jsafe = (j > 0 && j < myDims[1] - 1);
            for (int64_t i = 0; i < myDims[0]; ++i)
            {
                int64_t myindex = toProcess->getIndex(i, j, k);
                if (roiFrame == NULL || roiFrame[myindex] > 0.0f)
                {
                    bool canBeMin = minPos[myindex] && !ignoreMinima;
                    bool canBeMax = maxPos[myindex] && !ignoreMaxima;
                    float myval = dataFrame[myindex];
                    if (threshMode)
                    {
                        if (myval > lowThresh) canBeMin = false;//check thresholds
                        if (myval < highThresh) canBeMax = false;
                    }
                    if (canBeMin || canBeMax)
                    {
                        bool isafe = (i > 0 && i < myDims[0] - 1);
                        if (isafe && jsafe && ksafe)//special case out the inner loops to avoid branching when possible
                        {
                            if (roiFrame == NULL)
                            {
                                int v = 0;
                                if (canBeMin && canBeMax)//no ROI and safe means we can just use the first element in stencil to break the double test
                                {
                                    int64_t testVox[3] = { i + stencil[0],
                                                        j + stencil[1],
                                                        k + stencil[2] };
                                    int64_t otherindex = toProcess->getIndex(testVox);
                                    float otherval = dataFrame[otherindex];
                                    if (myval < otherval)
                                    {
                                        minPos[otherindex] = 0;
                                    } else {
                                        canBeMin = false;
                                    }
                                    if (myval > otherval)
                                    {
                                        maxPos[otherindex] = 0;
                                    } else {
                                        canBeMax = false;
                                    }
                                    v = 3;
                                }
                                if (canBeMin)
                                {
                                    for (; v < STENCIL_SIZE; v += 3)
                                    {
                                        int64_t testVox[3] = { i + stencil[v],
                                                            j + stencil[v + 1],
                                                            k + stencil[v + 2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        float otherval = dataFrame[otherindex];
                                        if (myval < otherval)
                                        {
                                            minPos[otherindex] = 0;
                                        } else {
                                            canBeMin = false;
                                            break;
                                        }
                                    }
                                }
                                if (canBeMax)
                                {
                                    for (; v < STENCIL_SIZE; v += 3)
                                    {
                                        int64_t testVox[3] = { i + stencil[v],
                                                            j + stencil[v + 1],
                                                            k + stencil[v + 2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        float otherval = dataFrame[otherindex];
                                        if (myval > otherval)
                                        {
                                            maxPos[otherindex] = 0;
                                        } else {
                                            canBeMax = false;
                                            break;
                                        }
                                    }
                                }
                            } else {
                                if (canBeMin && canBeMax)//we have an roi, but if a neighbor falls outside it, we are done anyway, we don't count extrema on the edge
                                {
                                    int64_t testVox[3] = { i + stencil[0],
                                                           j + stencil[1],
                                                           k + stencil[2] };
                                    int64_t otherindex = toProcess->getIndex(testVox);
                                    if (roiFrame[otherindex] > 0.0f)
                                    {
                                        float otherval = dataFrame[otherindex];
                                        if (myval < otherval)
                                        {
                                            minPos[otherindex] = 0;
                                        } else {
                                            canBeMin = false;
                                        }
                                        if (myval > otherval)
                                        {
                                            maxPos[otherindex] = 0;
                                        } else {
                                            canBeMax = false;
                                        }
                                    } else {
                                        canBeMax = false;//we are next to an roi edge, do not count this as an extrema
                                        canBeMin = false;
                                    }
                                }
                                int v = 3;
                                if (canBeMin)
                                {
                                    for (; v < STENCIL_SIZE; v += 3)
                                    {
                                        int64_t testVox[3] = { i + stencil[v],
                                                            j + stencil[v + 1],
                                                            k + stencil[v + 2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        if (roiFrame[otherindex] > 0.0f)
                                        {
                                            float otherval = dataFrame[otherindex];
                                            if (myval < otherval)
                                            {
                                                minPos[otherindex] = 0;
                                            } else {
                                                canBeMin = false;
                                                break;
                                            }
                                        } else {
                                            canBeMin = false;//we are next to an roi edge, do not count this as an extrema
                                            break;
                                        }
                                    }
                                }
                                if (canBeMax)
                                {
                                    for (; v < STENCIL_SIZE; v += 3)
                                    {
                                        int64_t testVox[3] = { i + stencil[v],
                                                            j + stencil[v + 1],
                                                            k + stencil[v + 2] };
                                        int64_t otherindex = toProcess->getIndex(testVox);
                                        if (roiFrame[otherindex] > 0.0f)
                                        {
                                            float otherval = dataFrame[otherindex];
                                            if (myval > otherval)
                                            {
                                                maxPos[otherindex] = 0;
                                            } else {
                                                canBeMax = false;
                                                break;
                                            }
                                        } else {
                                            canBeMax = false;//we are next to an roi edge, do not count this as an extrema
                                        }
                                    }
                                }
                                /*if (canBeMin && canBeMax)//this can't happen anymore
                                {
                                    canBeMax = false;//don't count isolated voxels as extrema
                                    canBeMin = false;
                                }//*/
                            }
                        } else {//if we are near an edge, we have to check all the neighbor indexes
                            for (int v = 0; v < STENCIL_SIZE; v += 3)
                            {
                                int64_t testVox[3] = { i + stencil[v],
                                                    j + stencil[v + 1],
                                                    k + stencil[v + 2] };
                                if (!ksafe && (testVox[2] < 0 || testVox[2] >= myDims[2])) continue;
                                if (!jsafe && (testVox[1] < 0 || testVox[1] >= myDims[1])) continue;
                                if (!isafe && (testVox[0] < 0 || testVox[0] >= myDims[0])) continue;
                                int64_t otherindex = toProcess->getIndex(testVox);
                                if (roiFrame != NULL && roiFrame[otherindex] <= 0.0f)
                                {
                                    canBeMax = false;//neighbor outside the roi means on the roi edge, don't count as extrema
                                    canBeMin = false;
                                    break;
                                }
                                float otherval = dataFrame[otherindex];
                                if (myval < otherval)//since we are checking index bounds anyway, just do the double test to make the code simpler
                                {
                                    minPos[otherindex] = 0;
                                } else {
                                    canBeMin = false;
                                    if (!canBeMax) break;
                                }
                                if (myval > otherval)
                                {
                                    maxPos[otherindex] = 0;
                                } else {
                                    canBeMax = false;
                                    if (!canBeMin) break;
                                }
                            }
                            /*if (canBeMin && canBeMax)//this can't happen anymore
                            {
                                canBeMax = false;//don't count isolated voxels as extrema
                                canBeMin = false;
                            }//*/
                        }
                        if (canBeMax)
                        {
                            Vector3D tempvec;
                            toProcess->indexToSpace(i, j, k, tempvec);
#pragma omp critical
                            {
                                tempExtrema[0].push_back(pair<Vector3D, int>(tempvec, 1));
                            }
                        }
                        if (canBeMin)
                        {
                            Vector3D tempvec;
                            toProcess->indexToSpace(i, j, k, tempvec);
#pragma omp critical
                            {
                                tempExtrema[1].push_back(pair<Vector3D, int>(tempvec, 1));
                            }
                        }
                    }
                }
            }
        }
    }
    consolidateStep(toProcess, distance, tempExtrema, minima, maxima);
}

void AlgorithmVolumeExtrema::consolidateStep(const VolumeFile* toProcess, const float& distance, vector<pair<Vector3D, int> > initExtrema[2], vector<VoxelIJK>& minima, vector<VoxelIJK>& maxima)
{
    for (int sign = 0; sign < 2; ++sign)
    {
        int numInitExtrema = (int)initExtrema[sign].size();
        vector<bool> removed(numInitExtrema, false);//track which extrema locations are dropped during consolidation - the one that isn't dropped in a merge has its node number changed
        vector<vector<int64_t> > heapIDmatrix(numInitExtrema, vector<int64_t>(numInitExtrema, -1));
        CaretMinHeap<pair<int, int>, float> myDistHeap;
        for (int i = 0; i < numInitExtrema - 1; ++i)
        {
            for (int j = i + 1; j < numInitExtrema; ++j)
            {
                float tempf = (initExtrema[sign][i].first - initExtrema[sign][j].first).length();
                if (tempf < distance)
                {
                    int64_t tempID = myDistHeap.push(pair<int, int>(i, j), tempf);
                    heapIDmatrix[i][j] = tempID;
                    heapIDmatrix[j][i] = tempID;
                }
            }
        }//initial distance matrix computed, now we iterate
        while (!myDistHeap.isEmpty())
        {
            pair<int, int> toMerge = myDistHeap.pop();//we don't need to know the key
            int extr1 = toMerge.first;
            int extr2 = toMerge.second;
            heapIDmatrix[extr1][extr2] = -1;
            heapIDmatrix[extr2][extr1] = -1;
            int weight1 = initExtrema[sign][extr1].second;
            int weight2 = initExtrema[sign][extr2].second;
            if (weight2 > weight1)//swap so weight1 is always bigger
            {
                int temp = weight2;
                weight2 = weight1;
                weight1 = temp;
                temp = extr2;
                extr2 = extr1;
                extr1 = temp;
            }
            Vector3D point1 = initExtrema[sign][extr1].first;
            Vector3D point2 = initExtrema[sign][extr2].first;
            removed[extr2] = true;//drop the one that has less weight, and modify the one that has more weight
            for (int j = 0; j < numInitExtrema; ++j)
            {
                if (!removed[j])
                {
                    int64_t tempID = heapIDmatrix[extr2][j];
                    if (tempID != -1)
                    {
                        myDistHeap.remove(tempID);
                        heapIDmatrix[extr2][j] = -1;
                        heapIDmatrix[j][extr2] = -1;
                    }
                }
            }
            Vector3D newPoint = (point1 * weight1 + point2 * weight2) / (weight1 + weight2);
            initExtrema[sign][extr1].first = newPoint;
            for (int j = 0; j < numInitExtrema; ++j)
            {
                if (!removed[j])
                {
                    float tempf = (newPoint - initExtrema[sign][j].first).length();
                    int64_t tempID = heapIDmatrix[extr1][j];
                    if (tempf < distance)
                    {
                        if (tempID != -1)
                        {
                            myDistHeap.changekey(tempID, tempf);
                        } else {
                            tempID = myDistHeap.push(pair<int, int>(extr1, j), tempf);
                            heapIDmatrix[extr1][j] = tempID;
                            heapIDmatrix[j][extr1] = tempID;
                        }
                    } else {
                        if (tempID != -1)
                        {
                            myDistHeap.remove(tempID);
                            heapIDmatrix[extr1][j] = -1;
                            heapIDmatrix[j][extr1] = -1;
                        }
                    }
                }
            }
        }
        if (sign == 0)
        {
            for (int i = 0; i < numInitExtrema; ++i)
            {
                if (!removed[i])
                {
                    VoxelIJK tempijk;
                    toProcess->enclosingVoxel(initExtrema[sign][i].first, tempijk.m_ijk);
                    maxima.push_back(tempijk);
                }
            }
        } else {
            for (int i = 0; i < numInitExtrema; ++i)
            {
                if (!removed[i])
                {
                    VoxelIJK tempijk;
                    toProcess->enclosingVoxel(initExtrema[sign][i].first, tempijk.m_ijk);
                    minima.push_back(tempijk);
                }
            }
        }
    }
}

float AlgorithmVolumeExtrema::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeExtrema::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
