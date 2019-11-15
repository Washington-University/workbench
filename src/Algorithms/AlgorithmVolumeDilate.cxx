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

#include "AlgorithmVolumeDilate.h"

#include "AlgorithmException.h"
#include "CaretHeap.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CaretPointLocator.h"
#include "FloatMatrix.h"
#include "Vector3D.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

#include <algorithm>
#include <cmath>
#include <map>

using namespace caret;
using namespace std;

AString AlgorithmVolumeDilate::getCommandSwitch()
{
    return "-volume-dilate";
}

AString AlgorithmVolumeDilate::getShortDescription()
{
    return "DILATE A VOLUME FILE";
}

OperationParameters* AlgorithmVolumeDilate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume", "the volume to dilate");
    
    ret->addDoubleParameter(2, "distance", "distance in mm to dilate");
    
    ret->addStringParameter(3, "method", "dilation method to use");
    
    ret->addVolumeOutputParameter(4, "volume-out", "the output volume");
    
    OptionalParameter* exponentOpt = ret->createOptionalParameter(8, "-exponent", "use a different exponent in the weighting function");
    exponentOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (1 / (distance ^ n)) as the weighting function (default 7)");
    
    OptionalParameter* badRoiOpt = ret->createOptionalParameter(5, "-bad-voxel-roi", "specify an roi of voxels to overwrite, rather than voxels with value zero");
    badRoiOpt->addVolumeParameter(1, "roi-volume", "volume file, positive values denote voxels to have their values replaced");
    
    OptionalParameter* dataRoiOpt = ret->createOptionalParameter(7, "-data-roi", "specify an roi of where there is data");
    dataRoiOpt->addVolumeParameter(1, "roi-volume", "volume file, positive values denote voxels that have data");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to dilate");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->createOptionalParameter(9, "-legacy-cutoff", "use the old method of excluding voxels further than the dilation distance when calculating the dilated value");
    
    ret->setHelpText(
        AString("For all voxels that are designated as bad, if they neighbor a non-bad voxel with data or are within the specified distance of such a voxel, ") +
        "replace the value in the bad voxel with a value calculated from nearby non-bad voxels that have data, otherwise set the value to zero.  " +
        "No matter how small <distance> is, dilation will always use at least the face neighbor voxels.\n\n" +
        "By default, voxels that have data with the value 0 are bad, specify -bad-voxel-roi to only count voxels as bad if they are selected by the roi.  " +
        "If -data-roi is not specified, all voxels are assumed to have data.\n\n" +
        "To get the behavior of version 1.3.2 or earlier, use '-legacy-cutoff -exponent 2'.\n\n" +
        "Valid values for <method> are:\n\n" +
        "NEAREST - use the value from the nearest good voxel\n" +
        "WEIGHTED - use a weighted average based on distance"
    );
    return ret;
}

void AlgorithmVolumeDilate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volIn = myParams->getVolume(1);
    float distance = (float)myParams->getDouble(2);
    AString methodName = myParams->getString(3);
    Method myMethod = NEAREST;
    if (methodName == "NEAREST")
    {
        myMethod = NEAREST;
    } else if (methodName == "WEIGHTED") {
        myMethod = WEIGHTED;
    } else {
        throw AlgorithmException("invalid method specified, use NEAREST or WEIGHTED");
    }
    VolumeFile* volOut = myParams->getOutputVolume(4);
    OptionalParameter* exponentOpt = myParams->getOptionalParameter(8);
    float exponent = 7.0f;
    if (exponentOpt->m_present)
    {
        exponent = (float)exponentOpt->getDouble(1);
    }
    OptionalParameter* badRoiOpt = myParams->getOptionalParameter(5);
    VolumeFile* badRoi = NULL;
    if (badRoiOpt->m_present)
    {
        badRoi = badRoiOpt->getVolume(1);
    }
    OptionalParameter* dataRoiOpt = myParams->getOptionalParameter(7);
    VolumeFile* dataRoi = NULL;
    if (dataRoiOpt->m_present)
    {
        dataRoi = dataRoiOpt->getVolume(1);
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(6);
    int subvol = -1;
    if (subvolSelect->m_present)
    {
        subvol = volIn->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvol < 0) throw AlgorithmException("invalid subvolume specified");
    }
    bool legacyCutoff = myParams->getOptionalParameter(9)->m_present;
    AlgorithmVolumeDilate(myProgObj, volIn, distance, myMethod, volOut, badRoi, dataRoi, subvol, exponent, legacyCutoff);
}

namespace
{
    
    inline bool badVoxel(const bool labelMode, const int32_t unlabeledKey, const int64_t i, const int64_t j, const int64_t k, const VolumeFile* volIn, const int& insubvol, const int& component,
                         const VolumeFile* badRoi, const VolumeFile* dataRoi)
    {
        if (badRoi == NULL)
        {
            if (labelMode)
            {
                return (dataRoi == NULL || dataRoi->getValue(i, j, k) > 0.0f) && floor(0.5f + volIn->getValue(i, j, k, insubvol, component)) == unlabeledKey;//without bad roi, bad is implicitly "data and not 0/unlabeled"
            } else {
                return (dataRoi == NULL || dataRoi->getValue(i, j, k) > 0.0f) && volIn->getValue(i, j, k, insubvol, component) == 0.0f;
            }
        } else {
            return badRoi->getValue(i, j, k) > 0.0f;
        }
    }
    
    inline bool dataVoxel(const int64_t i, const int64_t j, const int64_t k, const VolumeFile* dataRoi)
    {
        return dataRoi == NULL || dataRoi->getValue(i, j, k) > 0.0f;
    }
    
    void dilateFrame(const bool labelMode, const VolumeFile* volIn, const int& insubvol, const int& component, VolumeFile* volOut, const int& outsubvol, const VolumeFile* badRoi,
                            const VolumeFile* dataRoi, const float& distance, const AlgorithmVolumeDilate::Method& myMethod, const float& exponent, const bool& legacyCutoff)
    {//copy data that is outside the dataROI, replace values where badROI is > 0 (with zero if nothing else), if no badROI, pretend badROI is (data == 0 && dataROI > 0)
        int neighbors[18] = {0, 0, -1,
                           0, -1, 0,
                           -1, 0, 0,
                           1, 0, 0,
                           0, 1, 0,
                           0, 0, 1};//special behavior: when distance is 0, it still dilates by 1 voxel
        Vector3D voxStep[3], origin;
        const VolumeSpace& myVolSpace = volIn->getVolumeSpace();
        myVolSpace.getSpacingVectors(voxStep[0], voxStep[1], voxStep[2], origin);
        //if the distance is within 1% of excluding a neighbor, we need to additionally check the neighbors
        bool checkNeighbors = distance <= voxStep[0].length() * 1.01f || distance <= voxStep[1].length() * 1.01f || distance <= voxStep[2].length() * 1.01f;
        //the single-voxel rule means we can't just base the maximum search distance on the dilation distance
        float cutoffBase = max(2.0f * distance, 2.0f * min(min(voxStep[0].length(), voxStep[1].length()), voxStep[2].length()));
        float minKernel = 1.5f * min(min(voxStep[0].length(), voxStep[1].length()), voxStep[2].length());//small kernels are cheap, so use a minimum of face+edge neighbors in isotropic volumes in weighted mode
        vector<int64_t> myDims = volIn->getDimensions();
        //HACK: unlabeledKey is also used to fill bad voxels that the dilation doesn't reach in non-label mode
        int32_t unlabeledKey = 0;
        if (labelMode) unlabeledKey = volIn->getMapLabelTable(insubvol)->getUnassignedLabelKey();
        vector<float> validPoints;
        vector<VoxelIJK> validIndices, toReplace;
        vector<float> scratchFrame(myDims[0] * myDims[1] * myDims[2]);//uninitialized, we will copy every voxel we don't replace
        for (int64_t k = 0; k < myDims[2]; ++k)
        {
            for (int64_t j = 0; j < myDims[1]; ++j)
            {
                for (int64_t i = 0; i < myDims[0]; ++i)
                {
                    if (badVoxel(labelMode, unlabeledKey, i, j, k, volIn, insubvol, component, badRoi, dataRoi))
                    {
                        toReplace.push_back(VoxelIJK(i, j, k));
                    } else {//copy it, and if it is data, add to usable set
                        if (labelMode)
                        {
                            scratchFrame[myVolSpace.getIndex(i, j, k)] = floor(0.5f + volIn->getValue(i, j, k, insubvol, component));
                        } else {
                            scratchFrame[myVolSpace.getIndex(i, j, k)] = volIn->getValue(i, j, k, insubvol, component);
                        }
                        if (dataVoxel(i, j, k, dataRoi))
                        {
                            VoxelIJK tempVoxel(i, j, k);
                            Vector3D tempCoord = myVolSpace.indexToSpace(tempVoxel);
                            validPoints.push_back(tempCoord[0]);
                            validPoints.push_back(tempCoord[1]);
                            validPoints.push_back(tempCoord[2]);
                            validIndices.push_back(tempVoxel);
                        }
                    }
                }
            }
        }
        if (!toReplace.empty())
        {
            CaretPointLocator locator(validPoints);
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int64_t whichVoxel = 0; whichVoxel < (int64_t)toReplace.size(); ++whichVoxel)
            {
                const VoxelIJK& iter = toReplace[whichVoxel];
                int64_t i = iter.m_ijk[0], j = iter.m_ijk[1], k = iter.m_ijk[2];
                Vector3D voxcoord = myVolSpace.indexToSpace(i, j, k);
                switch (myMethod)
                {
                    case AlgorithmVolumeDilate::NEAREST:
                    {
                        int64_t index = locator.closestPointLimited(voxcoord, distance);
                        float bestVal = unlabeledKey;//HACK: unlabeledKey is 0 when we aren't in label mode, so can double as default value for bad voxel beyond dilate range
                        if (index < 0)
                        {
                            float bestDist = -1.0f;
                            if (checkNeighbors)
                            {
                                for (int n = 0; n < 6; ++n)
                                {
                                    int neighbase = n * 3;
                                    int64_t neighVox[3] = {i + neighbors[neighbase], j + neighbors[neighbase + 1], k + neighbors[neighbase + 2]};
                                    if (myVolSpace.indexValid(neighVox) && ! badVoxel(labelMode, unlabeledKey, neighVox[0], neighVox[1], neighVox[2], volIn, insubvol, component, badRoi, dataRoi))
                                    {
                                        float tempdist = (myVolSpace.indexToSpace(neighbors + neighbase) - myVolSpace.indexToSpace(0, 0, 0)).length();//slightly hacky, but won't have inconsistencies from different rounding per bad voxel
                                        if (tempdist < bestDist || bestDist == -1.0f)
                                        {
                                            bestDist = tempdist;
                                            bestVal = volIn->getValue(neighVox, insubvol, component);
                                        }
                                    }
                                }
                            }
                        } else {
                            bestVal = volIn->getValue(validIndices[index], insubvol, component);
                        }
                        if (labelMode)
                        {
                            bestVal = floor(0.5f + bestVal);
                        }
                        scratchFrame[myVolSpace.getIndex(i, j, k)] = bestVal;
                        break;
                    }
                    case AlgorithmVolumeDilate::WEIGHTED:
                    {
                        vector<LocatorInfo> inRange;
                        if (legacyCutoff)
                        {
                            inRange = locator.pointsInRange(voxcoord, distance);//immediate neighbor special case is handled below
                        } else {
                            float closeDist = -1.0f;
                            LocatorInfo myInfo;
                            int64_t index = locator.closestPointLimited(voxcoord, distance, &myInfo);//only need the distance
                            bool found = false;
                            if (index >= 0)
                            {
                                found = true;
                                closeDist = (myInfo.coords - voxcoord).length();
                            } else {
                                if (checkNeighbors)
                                {//always dilate to neighbor voxels, regardless
                                    for (int n = 0; n < 6; ++n)
                                    {
                                        int neighbase = n * 3;
                                        int64_t neighVox[3] = {i + neighbors[neighbase], j + neighbors[neighbase + 1], k + neighbors[neighbase + 2]};
                                        if (myVolSpace.indexValid(neighVox) && ! badVoxel(labelMode, unlabeledKey, neighVox[0], neighVox[1], neighVox[2], volIn, insubvol, component, badRoi, dataRoi))
                                        {
                                            float tempdist = (myVolSpace.indexToSpace(neighbors + neighbase) - myVolSpace.indexToSpace(0, 0, 0)).length();//slightly hacky, but won't have inconsistencies from different rounding per voxel
                                            if (tempdist < closeDist || !found)
                                            {
                                                found = true;
                                                closeDist = tempdist;
                                            }
                                        }
                                    }
                                }
                            }
                            if (found)
                            {
                                //find what cutoff corresponds to 98% of the total weight being found compared to an infinite kernel
                                //to do this, assume a non-adversarial situation, where farther parts have at most equal angular area to closer ones
                                //49 = 98/(100-98)
                                float cutoffRatio = max(1.1f, pow(49.0f, 1.0f / (exponent - 3.0f))), cutoffDist = cutoffBase;//find what cutoff ratio corresponds to a hudredth of weight
                                if (exponent > 3.0f && cutoffRatio < 100.0f && cutoffRatio > 1.0f)//if the ratio is sane, use it, but never exceed cutoffBase
                                {
                                    cutoffDist = max(min(cutoffRatio * closeDist, cutoffDist), minKernel);//but small kernels are rather cheap anyway, so have a minimum size just in case
                                }
                                inRange = locator.pointsInRange(voxcoord, cutoffDist);
                            }
                        }
                        map<int32_t, float> labelSums;
                        double sum = 0.0, weightsum = 0.0;
                        if (legacyCutoff && checkNeighbors)
                        {//add valid neighbors only if they aren't already in the list, and the non-legacy mode is already handled above...
                            set<VoxelIJK> voxelsToUse;//but looking up the neighbors' indices in validIndices is work we don't need to do, so copy the list and add to it
                            for (auto thisInfo : inRange)
                            {
                                voxelsToUse.insert(validIndices[thisInfo.index]);
                            }
                            for (int n = 0; n < 6; ++n)
                            {
                                int neighbase = n * 3;
                                int64_t neighVox[3] = {i + neighbors[neighbase], j + neighbors[neighbase + 1], k + neighbors[neighbase + 2]};
                                if (myVolSpace.indexValid(neighVox) && ! badVoxel(labelMode, unlabeledKey, neighVox[0], neighVox[1], neighVox[2], volIn, insubvol, component, badRoi, dataRoi))
                                {
                                    voxelsToUse.insert(neighVox);//set eliminates duplicates
                                }
                            }
                            for (auto thisVoxel : voxelsToUse)//unfortunately, this means we need to write a copy of the loop for the common case not to do unneeded work
                            {
                                float thisdist = (myVolSpace.indexToSpace(thisVoxel) - voxcoord).length();
                                float weight = 1.0f / pow(thisdist, exponent);
                                if (labelMode)
                                {
                                    int32_t thisKey = int32_t(floor(0.5f + volIn->getValue(thisVoxel, insubvol, component)));
                                    map<int32_t, float>::iterator iter = labelSums.find(thisKey);
                                    if (iter == labelSums.end())
                                    {
                                        labelSums[thisKey] = weight;
                                    } else {
                                        iter->second += weight;
                                    }
                                } else {
                                    sum += weight * volIn->getValue(thisVoxel, insubvol, component);
                                    weightsum += weight;
                                }
                            }
                        } else {
                            for (auto thisInfo : inRange)
                            {
                                float thisdist = (thisInfo.coords - voxcoord).length();
                                float weight = 1.0f / pow(thisdist, exponent);
                                if (labelMode)
                                {
                                    int32_t thisKey = int32_t(floor(0.5f + volIn->getValue(validIndices[thisInfo.index], insubvol, component)));
                                    map<int32_t, float>::iterator iter = labelSums.find(thisKey);
                                    if (iter == labelSums.end())
                                    {
                                        labelSums[thisKey] = weight;
                                    } else {
                                        iter->second += weight;
                                    }
                                } else {
                                    sum += weight * volIn->getValue(validIndices[thisInfo.index], insubvol, component);
                                    weightsum += weight;
                                }
                            }
                        }
                        if (labelMode)
                        {
                            float bestWeight = -1.0f;//all weights should be positive, so their sums should too
                            int32_t bestKey = unlabeledKey;
                            for (auto iter : labelSums)
                            {
                                if (iter.second > bestWeight)
                                {
                                    bestWeight = iter.second;
                                    bestKey = iter.first;
                                }
                            }
                            scratchFrame[myVolSpace.getIndex(i, j, k)] = bestKey;
                        } else {
                            if (weightsum > 0.0)
                            {
                                scratchFrame[myVolSpace.getIndex(i, j, k)] = sum / weightsum;
                            } else {
                                scratchFrame[myVolSpace.getIndex(i, j, k)] = 0.0f;
                            }
                        }
                        break;
                    }
                }
            }
        }
        volOut->setFrame(scratchFrame.data(), outsubvol, component);
    }
}

AlgorithmVolumeDilate::AlgorithmVolumeDilate(ProgressObject* myProgObj, const VolumeFile* volIn, const float& distance, const Method& myMethod, VolumeFile* volOut,
                                             const VolumeFile* badRoi, const VolumeFile* dataRoi, const int& subvol, const float& exponent, const bool legacyCutoff) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> myDims;
    volIn->getDimensions(myDims);
    if (subvol < -1 || subvol >= myDims[3])
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    if (distance < 0.0f)
    {
        throw AlgorithmException("distance cannot be negative");
    }
    if (badRoi != NULL && !volIn->matchesVolumeSpace(badRoi))
    {
        throw AlgorithmException("bad voxel roi volume space does not match input volume");
    }
    if (dataRoi != NULL && !volIn->matchesVolumeSpace(dataRoi))
    {
        throw AlgorithmException("data roi volume space does not match input volume");
    }
    bool isLabelData = false;
    if (volIn->getType() == SubvolumeAttributes::LABEL)
    {
        isLabelData = true;
    }
    if (subvol == -1)
    {
        volOut->reinitialize(volIn->getOriginalDimensions(), volIn->getSform(), volIn->getNumberOfComponents(), volIn->getType(), volIn->m_header);
        for (int i = 0; i < myDims[3]; ++i)
        {
            if (volIn->getType() == SubvolumeAttributes::LABEL)
            {
                *(volOut->getMapLabelTable(i)) = *(volIn->getMapLabelTable(i));
            } else {
                *(volOut->getMapPaletteColorMapping(i)) = *(volIn->getMapPaletteColorMapping(i));
            }
            volOut->setMapName(i, volIn->getMapName(i) + " dilate " + AString::number(distance));
        }
    } else {
        vector<int64_t> outDims = myDims;
        outDims.resize(3);
        volOut->reinitialize(outDims, volIn->getSform(), volIn->getNumberOfComponents(), volIn->getType(), volIn->m_header);
        if (volIn->getType() == SubvolumeAttributes::LABEL)
        {
            *(volOut->getMapLabelTable(0)) = *(volIn->getMapLabelTable(subvol));
        } else {
            *(volOut->getMapPaletteColorMapping(0)) = *(volIn->getMapPaletteColorMapping(subvol));
        }
        volOut->setMapName(0, volIn->getMapName(subvol) + " dilate " + AString::number(distance));
    }
    if (subvol == -1)
    {
        for (int s = 0; s < myDims[3]; ++s)
        {
            for (int c = 0; c < myDims[4]; ++c)
            {
                dilateFrame(isLabelData, volIn, s, c, volOut, s, badRoi, dataRoi, distance, myMethod, exponent, legacyCutoff);
            }
        }
    } else {
        for (int c = 0; c < myDims[4]; ++c)
        {
            dilateFrame(isLabelData, volIn, subvol, c, volOut, 0, badRoi, dataRoi, distance, myMethod, exponent, legacyCutoff);
        }
    }
}

float AlgorithmVolumeDilate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeDilate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
