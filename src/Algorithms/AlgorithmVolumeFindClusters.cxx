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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CaretPointLocator.h"
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
    
    OptionalParameter* sizeRatioOpt = ret->createOptionalParameter(9, "-size-ratio", "ignore clusters smaller than a given fraction of the largest cluster in map");
    sizeRatioOpt->addDoubleParameter(1, "ratio", "fraction of the largest cluster's volume");
    
    OptionalParameter* distanceOpt = ret->createOptionalParameter(10, "-distance", "ignore clusters further than a given distance from the largest cluster");
    distanceOpt->addDoubleParameter(1, "distance", "how far from the largest cluster a cluster can be, edge to edge, in mm");
    
    OptionalParameter* startOpt = ret->createOptionalParameter(8, "-start", "start labeling clusters from a value other than 1");
    startOpt->addIntegerParameter(1, "startval", "the value to give the first cluster found");
    
    ret->setHelpText(
        AString("Outputs a volume with nonzero integers for all voxels within a large enough cluster, and zeros elsewhere.  ") +
        "The integers denote cluster membership (by default, first cluster found will use value 1, second cluster 2, etc).  " +
        "Cluster values are not reused across frames of the output, but instead keep counting up.  " +
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
    OptionalParameter* sizeRatioOpt = myParams->getOptionalParameter(9);
    float sizeRatio = -1.0f;
    if (sizeRatioOpt->m_present)
    {
        sizeRatio = sizeRatioOpt->getDouble(1);
        if (sizeRatio <= 0.0f)
        {
            throw AlgorithmException("volume ratio must be positive");
        }
    }
    OptionalParameter* distanceOpt = myParams->getOptionalParameter(10);
    float distaceCutoff = -1.0f;
    if (distanceOpt->m_present)
    {
        distaceCutoff = distanceOpt->getDouble(1);
        if (distaceCutoff <= 0.0f)
        {
            throw AlgorithmException("distance cutoff must be positive");
        }
    }
    AlgorithmVolumeFindClusters(myProgObj, volIn, threshValue, minVolume, volOut, lessThan, myRoi, subvolNum, startVal, NULL, sizeRatio, distaceCutoff);
}

namespace
{
    void processSubvol(const float* inFrame, VolumeFile* volOut, const int64_t& outSubvol, const int64_t& outComponent, const float& threshValue, const float& minVolume,
                       const bool& lessThan, const float* roiFrame, const float& sizeRatio, const float& distanceCutoff, int& markVal)
    {
        vector<int64_t> dims = volOut->getDimensions();
        int64_t frameSize = dims[0] * dims[1] * dims[2];
        const VolumeSpace& mySpace = volOut->getVolumeSpace();
        Vector3D ivec, jvec, kvec, origin;
        mySpace.getSpacingVectors(ivec, jvec, kvec, origin);
        float voxelVolume = abs(ivec.dot(jvec.cross(kvec)));
        int64_t minVoxels = (int64_t)ceil(minVolume / voxelVolume);
        vector<VoxelIJK> neighbors;
        neighbors.push_back(VoxelIJK(1, 0, 0));
        neighbors.push_back(VoxelIJK(-1, 0, 0));
        neighbors.push_back(VoxelIJK(0, 1, 0));
        neighbors.push_back(VoxelIJK(0, -1, 0));
        neighbors.push_back(VoxelIJK(0, 0, 1));
        neighbors.push_back(VoxelIJK(0, 0, -1));
        vector<char> marked(frameSize, 0);
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
        vector<vector<VoxelIJK> > clusters;
        size_t biggestCount = 0;
        int64_t biggestCluster = -1;
        for (int64_t k = 0; k < dims[2]; ++k)
        {
            for (int64_t j = 0; j < dims[1]; ++j)
            {
                for (int64_t i = 0; i < dims[0]; ++i)
                {
                    int64_t myIndex = mySpace.getIndex(i, j, k);
                    if (marked[myIndex])
                    {
                        vector<VoxelIJK> voxelList;
                        voxelList.push_back(VoxelIJK(i, j, k));
                        marked[myIndex] = 0;//don't let voxels get duplicated in the list
                        for (int64_t index = 0; index < (int64_t)voxelList.size(); ++index)//NOTE: vector grows inside loop
                        {
                            const VoxelIJK& thisVoxel = voxelList[index];
                            for (int n = 0; n < (int)neighbors.size(); ++n)
                            {
                                VoxelIJK neighVox(thisVoxel.m_ijk[0] + neighbors[n].m_ijk[0],
                                                    thisVoxel.m_ijk[1] + neighbors[n].m_ijk[1],
                                                    thisVoxel.m_ijk[2] + neighbors[n].m_ijk[2]);
                                if (mySpace.indexValid(neighVox.m_ijk))
                                {
                                    int64_t neighIndex = mySpace.getIndex(neighVox.m_ijk);
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
                            if (voxelList.size() > biggestCount)
                            {
                                biggestCount = voxelList.size();
                                biggestCluster = (int64_t)clusters.size();
                            }
                            clusters.push_back(voxelList);
                        }
                    }
                }
            }
        }
        if (!clusters.empty()) CaretAssert(biggestCluster != -1);
        if (biggestCluster != -1 && (distanceCutoff > 0.0f || sizeRatio > 0.0f))
        {
            CaretPointer<CaretPointLocator> myLocator;
            if (distanceCutoff > 0.0f)
            {
                vector<float> biggestCoords;//gather coordinates of biggest cluster voxels
                biggestCoords.reserve(biggestCount * 3);
                for (size_t i = 0; i < clusters[biggestCluster].size(); ++i)
                {
                    float thisCoord[3];
                    mySpace.indexToSpace(clusters[biggestCluster][i].m_ijk, thisCoord);
                    biggestCoords.push_back(thisCoord[0]);
                    biggestCoords.push_back(thisCoord[1]);
                    biggestCoords.push_back(thisCoord[2]);
                }
                myLocator.grabNew(new CaretPointLocator(biggestCoords.data(), biggestCoords.size()));
            }
            for (size_t i = 0; i < clusters.size(); ++i)
            {
                if ((int64_t)i != biggestCluster)
                {
                    bool erase = false;
                    if (sizeRatio > 0.0f && ((float)clusters[i].size()) / biggestCount < sizeRatio)
                    {
                        erase = true;
                    }
                    if (!erase && distanceCutoff > 0.0f)
                    {
                        erase = true;//erase unless we find a point close enough to the biggest cluster
                        for (size_t j = 0; j < clusters[i].size(); ++j)
                        {
                            float thisCoord[3];
                            mySpace.indexToSpace(clusters[i][j].m_ijk, thisCoord);
                            int32_t ret = myLocator->closestPointLimited(thisCoord, distanceCutoff);
                            if (ret == -1)
                            {
                                erase = false;
                                break;
                            }
                        }
                    }
                    if (erase)
                    {
                        clusters.erase(clusters.begin() + i);//remove it
                        --i;//don't skip a cluster
                        if (biggestCluster > (int64_t)i) --biggestCluster;//don't lose track of the biggest cluster
                    }
                }
            }
        }
        for (size_t i = 0; i < clusters.size(); ++i)
        {
            if (markVal == 0)
            {
                CaretLogInfo("skipping 0 for cluster marking");
                ++markVal;
            }
            float tempVal = markVal;
            if ((int)tempVal != markVal) throw AlgorithmException("too many clusters, unable to mark them uniquely");
            for (size_t index = 0; index < clusters[i].size(); ++index)
            {
                volOut->setValue(tempVal, clusters[i][index].m_ijk, outSubvol, outComponent);
            }
            ++markVal;
        }
    }
}

AlgorithmVolumeFindClusters::AlgorithmVolumeFindClusters(ProgressObject* myProgObj, const VolumeFile* volIn, const float& threshValue, const float& minVolume, VolumeFile* volOut,
                                                         const bool& lessThan, const VolumeFile* myRoi, const int& subvolNum, const int& startVal, int* endVal,
                                                         const float& sizeRatio, const float& distanceCutoff) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (startVal == 0)
    {
        throw AlgorithmException("0 is not a valid cluster marking start value");
    }
    const VolumeSpace& mySpace = volIn->getVolumeSpace();
    const float* roiFrame = NULL;
    if (myRoi != NULL)
    {
        if (!mySpace.matches(myRoi->getVolumeSpace())) throw AlgorithmException("roi volume space does not match input");
        roiFrame = myRoi->getFrame();
    }
    vector<int64_t> dims = volIn->getDimensions();
    int markVal = startVal;
    if (subvolNum == -1)
    {
        volOut->reinitialize(volIn->getOriginalDimensions(), volIn->getSform(), dims[4], SubvolumeAttributes::ANATOMY, volIn->m_header);
        volOut->setValueAllVoxels(0.0f);
        for (int64_t c = 0; c < dims[4]; ++c)
        {
            for (int64_t s = 0; s < dims[3]; ++s)
            {
                const float* inFrame = volIn->getFrame(s, c);
                processSubvol(inFrame, volOut, s, c, threshValue, minVolume, lessThan, roiFrame, sizeRatio, distanceCutoff, markVal);
            }
        }
    } else {
        vector<int64_t> outDims = volIn->getOriginalDimensions();
        outDims.resize(3);
        volOut->reinitialize(outDims, volIn->getSform(), dims[4], SubvolumeAttributes::ANATOMY, volIn->m_header);
        volOut->setValueAllVoxels(0.0f);
        for (int64_t c = 0; c < dims[4]; ++c)
        {
            const float* inFrame = volIn->getFrame(subvolNum, c);
            processSubvol(inFrame, volOut, 0, c, threshValue, minVolume, lessThan, roiFrame, sizeRatio, distanceCutoff, markVal);
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
