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

#include "AlgorithmVolumeFindClusters.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeFindClusters::getCommandSwitch()
{
    return "-volume-find-clusters";
}

AString AlgorithmVolumeFindClusters::getShortDescription()
{
    return "FILTER CLUSTERS BY VOLUME";
}

OperationParameters* AlgorithmVolumeFindClusters::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input volume");
    
    ret->addDoubleParameter(2, "value-threshold", "threshold for data values");
    
    ret->addDoubleParameter(3, "minimum-volume", "threshold for cluster volume, in mm^3");
    
    ret->addVolumeOutputParameter(4, "volume-out", "the output volume");
    
    ret->createOptionalParameter(5, "-less-than", "find values less than <value-threshold>, rather than greater");
    
    OptionalParameter* roiOption = ret->createOptionalParameter(6, "-roi", "select a region of interest");
    roiOption->addMetricParameter(1, "roi-volume", "the roi, as a volume file");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(7, "-subvolume", "select a single subvolume");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    OptionalParameter* startOpt = ret->createOptionalParameter(8, "-start", "start labeling clusters from a value other than 1");
    startOpt->addIntegerParameter(1, "startval", "the value to give the first cluster found");
    
    ret->setHelpText(
        AString("Outputs a volume with cluster labels for all voxels within a large enough cluster, and zeros elsewhere.  ") +
        "By default, values greater than <value-threshold> are considered to be in a cluster, use -less-than to test for values less than the threshold.  " +
        "To apply this as a mask to the data, or to do more complicated thresholding, see -volume-math."
    );
    return ret;
}

void AlgorithmVolumeFindClusters::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volIn = myParams->getVolume(1);
    float threshValue = (float)myParams->getDouble(2);
    float minVolume = (float)myParams->getDouble(3);
    VolumeFile* volOut = myParams->getOutputVolume(4);
    bool lessThan = myParams->getOptionalParameter(5)->m_present;
    VolumeFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(6);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getVolume(1);
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(7);
    int subvolNum = -1;
    if (subvolSelect->m_present)
    {
        subvolNum = (int)volIn->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    OptionalParameter* startOpt = myParams->getOptionalParameter(8);
    int startVal = 1;
    if (startOpt->m_present)
    {
        startVal = (int)startOpt->getInteger(1);
    }
    AlgorithmVolumeFindClusters(myProgObj, volIn, threshValue, minVolume, volOut, lessThan, myRoi, subvolNum, startVal);
}

AlgorithmVolumeFindClusters::AlgorithmVolumeFindClusters(ProgressObject* myProgObj, const VolumeFile* volIn, const float& threshValue, const float& minVolume, VolumeFile* volOut,
                                                         const bool& lessThan, const VolumeFile* myRoi, const int& subvolNum, const int& startVal, int* endVal) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const VolumeSpace& mySpace = volIn->getVolumeSpace();
    const float* roiFrame = NULL;
    if (myRoi != NULL)
    {
        if (!mySpace.matches(myRoi->getVolumeSpace())) throw AlgorithmException("roi volume space does not match input");
        roiFrame = myRoi->getFrame();
    }
    Vector3D ivec, jvec, kvec, origin;
    mySpace.getSpacingVectors(ivec, jvec, kvec, origin);
    float voxelVolume = abs(ivec.dot(jvec.cross(kvec)));
    int64_t minVoxels = (int64_t)ceil(minVolume / voxelVolume);
    vector<int64_t> dims = volIn->getDimensions();
    int64_t frameSize = dims[0] * dims[1] * dims[2];
    vector<VoxelIJK> neighbors;
    neighbors.push_back(VoxelIJK(1, 0, 0));
    neighbors.push_back(VoxelIJK(-1, 0, 0));
    neighbors.push_back(VoxelIJK(0, 1, 0));
    neighbors.push_back(VoxelIJK(0, -1, 0));
    neighbors.push_back(VoxelIJK(0, 0, 1));
    neighbors.push_back(VoxelIJK(0, 0, -1));
    int markVal = startVal;
    if (subvolNum == -1)
    {
        volOut->reinitialize(volIn->getOriginalDimensions(), volIn->getSform(), dims[4]);
        volOut->setValueAllVoxels(0.0f);
        for (int64_t c = 0; c < dims[4]; ++c)
        {
            for (int64_t s = 0; s < dims[3]; ++s)
            {
                vector<char> marked(frameSize, 0);
                const float* inFrame = volIn->getFrame(s, c);
                if (lessThan)
                {
                    for (int64_t i = 0; i < frameSize; ++i)
                    {
                        if ((roiFrame == NULL || roiFrame[i] > 0.0f) && inFrame[i] < threshValue)
                        {
                            marked[i] = 1;
                        }
                    }
                } else {
                    for (int64_t i = 0; i < frameSize; ++i)
                    {
                        if ((roiFrame == NULL || roiFrame[i] > 0.0f) && inFrame[i] > threshValue)
                        {
                            marked[i] = 1;
                        }
                    }
                }
                for (int64_t k = 0; k < dims[2]; ++k)
                {
                    for (int64_t j = 0; j < dims[1]; ++j)
                    {
                        for (int64_t i = 0; i < dims[0]; ++i)
                        {
                            if (marked[volIn->getIndex(i, j, k)])
                            {
                                vector<VoxelIJK> voxelList;
                                voxelList.push_back(VoxelIJK(i, j, k));
                                marked[volIn->getIndex(i, j, k)] = 0;//don't let voxels get duplicated in the list
                                for (int64_t index = 0; index < (int64_t)voxelList.size(); ++index)//NOTE: vector grows inside loop
                                {
                                    const VoxelIJK& thisVoxel = voxelList[index];
                                    for (int n = 0; n < (int)neighbors.size(); ++n)
                                    {
                                        VoxelIJK neighVox(thisVoxel.m_ijk[0] + neighbors[n].m_ijk[0],
                                                          thisVoxel.m_ijk[1] + neighbors[n].m_ijk[1],
                                                          thisVoxel.m_ijk[2] + neighbors[n].m_ijk[2]);
                                        if (volIn->indexValid(neighVox.m_ijk))
                                        {
                                            int64_t neighIndex = volIn->getIndex(neighVox.m_ijk);
                                            if (marked[neighIndex])
                                            {
                                                voxelList.push_back(neighVox);
                                                marked[neighIndex] = 0;
                                            }
                                        }
                                    }
                                }
                                if ((int64_t)voxelList.size() >= minVoxels)
                                {
                                    if (markVal == 0)
                                    {
                                        CaretLogInfo("skipping 0 for cluster marking");
                                        ++markVal;
                                    }
                                    float tempVal = markVal;
                                    if ((int)tempVal != markVal) throw AlgorithmException("too many clusters, unable to mark them uniquely");
                                    for (int64_t index = 0; index < (int64_t)voxelList.size(); ++index)
                                    {
                                        volOut->setValue(tempVal, voxelList[index].m_ijk, s, c);
                                    }
                                    ++markVal;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        vector<int64_t> outDims = volIn->getOriginalDimensions();
        outDims.resize(3);
        volOut->reinitialize(outDims, volIn->getSform(), dims[4]);
        volOut->setValueAllVoxels(0.0f);
        for (int64_t c = 0; c < dims[4]; ++c)
        {
            vector<char> marked(frameSize, 0);
            const float* inFrame = volIn->getFrame(subvolNum, c);
            if (lessThan)
            {
                for (int64_t i = 0; i < frameSize; ++i)
                {
                    if ((roiFrame == NULL || roiFrame[i] > 0.0f) && inFrame[i] < threshValue)
                    {
                        marked[i] = 1;
                    }
                }
            } else {
                for (int64_t i = 0; i < frameSize; ++i)
                {
                    if ((roiFrame == NULL || roiFrame[i] > 0.0f) && inFrame[i] > threshValue)
                    {
                        marked[i] = 1;
                    }
                }
            }
            for (int64_t k = 0; k < dims[2]; ++k)
            {
                for (int64_t j = 0; j < dims[1]; ++j)
                {
                    for (int64_t i = 0; i < dims[0]; ++i)
                    {
                        if (marked[volIn->getIndex(i, j, k)])
                        {
                            vector<VoxelIJK> voxelList;
                            voxelList.push_back(VoxelIJK(i, j, k));
                            marked[volIn->getIndex(i, j, k)] = 0;//don't let voxels get duplicated in the list
                            for (int64_t index = 0; index < (int64_t)voxelList.size(); ++index)//NOTE: vector grows inside loop
                            {
                                const VoxelIJK& thisVoxel = voxelList[index];
                                for (int n = 0; n < (int)neighbors.size(); ++n)
                                {
                                    VoxelIJK neighVox(thisVoxel.m_ijk[0] + neighbors[n].m_ijk[0],
                                                        thisVoxel.m_ijk[1] + neighbors[n].m_ijk[1],
                                                        thisVoxel.m_ijk[2] + neighbors[n].m_ijk[2]);
                                    if (volIn->indexValid(neighVox.m_ijk))
                                    {
                                        int64_t neighIndex = volIn->getIndex(neighVox.m_ijk);
                                        if (marked[neighIndex])
                                        {
                                            voxelList.push_back(neighVox);
                                            marked[neighIndex] = 0;
                                        }
                                    }
                                }
                            }
                            if ((int64_t)voxelList.size() >= minVoxels)
                            {
                                if (markVal == 0)
                                {
                                    CaretLogInfo("skipping 0 for cluster marking");
                                    ++markVal;
                                }
                                float tempVal = markVal;
                                if ((int)tempVal != markVal) throw AlgorithmException("too many clusters, unable to mark them uniquely");
                                for (int64_t index = 0; index < (int64_t)voxelList.size(); ++index)
                                {
                                    volOut->setValue(tempVal, voxelList[index].m_ijk, 0, c);
                                }
                                ++markVal;
                            }
                        }
                    }
                }
            }
        }
    }
    if (endVal != NULL) *endVal = markVal;
}

float AlgorithmVolumeFindClusters::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeFindClusters::getSubAlgorithmWeight()
{
    return 0.0f;
}
