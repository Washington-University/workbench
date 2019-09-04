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

#include "AlgorithmVolumeTFCE.h"
#include "AlgorithmException.h"

#include "AlgorithmVolumeSmoothing.h"
#include "CaretAssert.h"
#include "CaretHeap.h"
#include "CaretOMP.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

#include <cmath>
#include <set>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeTFCE::getCommandSwitch()
{
    return "-volume-tfce";
}

AString AlgorithmVolumeTFCE::getShortDescription()
{
    return "DO TFCE ON A VOLUME FILE";
}

OperationParameters* AlgorithmVolumeTFCE::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume-in", "the volume to run TFCE on");
    
    ret->addVolumeOutputParameter(2, "volume-out", "the output volume");
    
    OptionalParameter* presmoothOpt = ret->createOptionalParameter(3, "-presmooth", "smooth the volume before running TFCE");
    presmoothOpt->addDoubleParameter(1, "kernel", "the sigma for the gaussian smoothing kernel, in mm");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(4, "-roi", "select a region of interest to run TFCE on");
    roiOpt->addVolumeParameter(1, "roi-volume", "the area to run TFCE on, as a volume");

    OptionalParameter* paramsOpt = ret->createOptionalParameter(5, "-parameters", "set parameters for TFCE integral");
    paramsOpt->addDoubleParameter(1, "E", "exponent for cluster volume (default 0.5)");
    paramsOpt->addDoubleParameter(2, "H", "exponent for threshold value (default 2.0)");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume");
    subvolSelect->addStringParameter(1, "subvolume", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Threshold-free cluster enhancement is a method to increase the relative value of regions that would form clusters in a standard thresholding test.  ") +
        "This is accomplished by evaluating the integral of:\n\n" +
        "e(h, p)^E * h^H * dh\n\n" +
        "at each vertex p, where h ranges from 0 to the maximum value in the data, and e(h, p) is the extent of the cluster containing vertex p at threshold h.  " +
        "Negative values are similarly enhanced by negating the data, running the same process, and negating the result.\n\n" +
        "This method is explained in: Smith SM, Nichols TE., \"Threshold-free cluster enhancement: addressing problems of smoothing, threshold dependence and localisation in cluster inference.\" Neuroimage. 2009 Jan 1;44(1):83-98. PMID: 18501637"
    );
    return ret;
}

void AlgorithmVolumeTFCE::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVol = myParams->getVolume(1);
    VolumeFile* myVolOut = myParams->getOutputVolume(2);
    float presmooth = 0.0f;
    OptionalParameter* presmoothOpt = myParams->getOptionalParameter(3);
    if (presmoothOpt->m_present)
    {
        presmooth = (float)presmoothOpt->getDouble(1);
        if (presmooth <= 0.0f) throw AlgorithmException("presmooth kernel size must be positive");
    }
    VolumeFile* myRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(4);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getVolume(1);
    }
    float param_e = 0.5f, param_h = 2.0;
    OptionalParameter* paramsOpt = myParams->getOptionalParameter(5);
    if (paramsOpt->m_present)
    {
        param_e = (float)paramsOpt->getDouble(1);
        param_h = (float)paramsOpt->getDouble(2);
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(6);
    int64_t subvolNum = -1;
    if (subvolSelect->m_present)
    {//set up to use the single column
        subvolNum = myVol->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    AlgorithmVolumeTFCE(myProgObj, myVol, myVolOut, presmooth, myRoi, param_e, param_h, subvolNum);
}

AlgorithmVolumeTFCE::AlgorithmVolumeTFCE(ProgressObject* myProgObj, const VolumeFile* myVol, VolumeFile* myVolOut, const float& presmooth, const VolumeFile* myRoi,
                                         const float& param_e, const float& param_h, const int64_t& subvolNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myRoi != NULL && !myVol->getVolumeSpace().matches(myRoi->getVolumeSpace())) throw AlgorithmException("roi volume has different volume space than input");
    if (subvolNum < -1 || subvolNum >= myVol->getNumberOfMaps()) throw AlgorithmException("invalid subvolume specified");
    vector<int64_t> dims = myVol->getDimensions();
    const float* roiFrame = NULL;
    if (myRoi != NULL) roiFrame = myRoi->getFrame();
    if (subvolNum == -1)
    {
        myVolOut->reinitialize(myVol->getOriginalDimensions(), myVol->getSform(), dims[4], myVol->getType(), myVol->m_header);
        const VolumeFile* toUse = myVol;
        VolumeFile smoothed;
        if (presmooth > 0.0f)
        {
            AlgorithmVolumeSmoothing(NULL, myVol, presmooth, &smoothed, myRoi);
            toUse = &smoothed;
        }
#pragma omp CARET_PAR
        {
            vector<float> outframe(dims[0] * dims[1] * dims[2]);
#pragma omp CARET_FOR
            for (int64_t b = 0; b < dims[3]; ++b)
            {
                for (int64_t c = 0; c < dims[4]; ++c)
                {
                    processFrame(toUse, b, c, outframe.data(), roiFrame, param_e, param_h);
                    myVolOut->setFrame(outframe.data(), b, c);
                }
            }
        }
    } else {
        vector<int64_t> outDims = dims;
        outDims.resize(3);
        myVolOut->reinitialize(outDims, myVol->getSform(), dims[4], myVol->getType(), myVol->m_header);
        const VolumeFile* toUse = myVol;
        int useFrame = subvolNum;
        VolumeFile smoothed;
        if (presmooth > 0.0f)
        {
            AlgorithmVolumeSmoothing(NULL, myVol, presmooth, &smoothed, myRoi, false, subvolNum);
            toUse = &smoothed;
            useFrame = 0;
        }
        vector<float> outframe(dims[0] * dims[1] * dims[2]);
        for (int64_t c = 0; c < dims[4]; ++c)
        {
            processFrame(toUse, useFrame, c, outframe.data(), roiFrame, param_e, param_h);
            myVolOut->setFrame(outframe.data(), 0, c);
        }
    }
}

void AlgorithmVolumeTFCE::processFrame(const VolumeFile* inVol, const int64_t& b, const int64_t& c, float* outData, const float* roiData, const float& param_e, const float& param_h)
{
    vector<int64_t> dims = inVol->getDimensions();
    int64_t frameSize = dims[0] * dims[1] * dims[2];
    vector<double> accum(frameSize, 0.0);
    tfce(inVol, b, c, accum.data(), roiData, param_e, param_h, false);//don't negate - positives
    tfce(inVol, b, c, accum.data(), roiData, param_e, param_h, true);//negate - negatives - NOTE: output is still positive!!!
    const float* inData = inVol->getFrame(b, c);
    for (int64_t i = 0; i < frameSize; ++i)
    {
        if (inData[i] > 0.0f)//negate the results from negative inputs
        {
            outData[i] = accum[i];
        } else {//the areas outside the roi will have zeros, so we don't have to worry about them
            outData[i] = -accum[i];
        }
    }
}

namespace
{//hidden namespace just to make sure things don't collide
    struct Cluster
    {
        double accumVal, totalVolume;
        vector<VoxelIJK> members;
        float lastVal;
        bool first;
        Cluster()
        {
            first = true;
            accumVal = 0.0;
            totalVolume = 0.0;
        }
        void addMember(const VoxelIJK& voxel, const float& val, const float& voxel_volume, const float& param_e, const float& param_h)
        {
            update(val, param_e, param_h);
            members.push_back(voxel);
            totalVolume += voxel_volume;
        }
        void update(const float& bottomVal, const float& param_e, const float& param_h)
        {
            if (first)
            {
                lastVal = bottomVal;
                first = false;
            } else {
                if (bottomVal != lastVal)//skip computing if there is no difference
                {
                    CaretAssert(bottomVal < lastVal);
                    double integrated_h = param_h + 1.0f;//integral(x^h) = (x^(h + 1))/(h + 1) + C
                    double newSlice = pow(totalVolume, (double)param_e) * (pow((double)lastVal, integrated_h) - pow((double)bottomVal, integrated_h)) / integrated_h;
                    accumVal += newSlice;
                    lastVal = bottomVal;//computing in double precision, with float for inputs, puts the smallest difference between values far greater than the instability of the computation
                }
            }
        }
    };
    
    int64_t allocCluster(vector<Cluster>& clusterList, set<int64_t>& deadClusters)
    {
        if (deadClusters.empty())
        {
            clusterList.push_back(Cluster());
            return (int64_t)(clusterList.size() - 1);
        } else {
            set<int64_t>::iterator iter = deadClusters.begin();
            int64_t ret = *iter;
            deadClusters.erase(iter);
            clusterList[ret] = Cluster();//reinitialize
            return ret;
        }
    }
}

void AlgorithmVolumeTFCE::tfce(const VolumeFile* inVol, const int64_t& b, const int64_t& c, double* accumData, const float* roiData, const float& param_e, const float& param_h, const bool& negate)
{
    vector<int64_t> dims = inVol->getDimensions();
    Vector3D ivec, jvec, kvec, origin;//compute the volume of a voxel so different resolutions have comparable values - as if it matters, but hey
    inVol->getVolumeSpace().getSpacingVectors(ivec, jvec, kvec, origin);//who knows, maybe we'll have distortion correction in volume someday
    float voxelVolume = abs(ivec.dot(jvec.cross(kvec)));
    const int64_t frameSize = dims[0] * dims[1] * dims[2];
    const float* frameData = inVol->getFrame(b, c);
    vector<int64_t> membership(frameSize, -1);//use int64_t just in case we get an absurd number of clusters
    vector<Cluster> clusterList;
    set<int64_t> deadClusters;//to allow reallocation without changing indices
    CaretSimpleMaxHeap<VoxelIJK, float> voxelHeap;
    for (int64_t i = 0; i < dims[0]; ++i)
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t k = 0; k < dims[2]; ++k)
            {
                int64_t index = inVol->getIndex(i, j, k);
                if ((roiData == NULL || roiData[index] > 0.0f))
                {
                    if (negate)
                    {
                        if (frameData[index] < 0.0f)
                        {
                            voxelHeap.push(VoxelIJK(i, j, k), -frameData[index]);
                        }
                    } else {
                        if (frameData[index] > 0.0f)
                        {
                            voxelHeap.push(VoxelIJK(i, j, k), frameData[index]);
                        }
                    }
                }
            }
        }
    }
    const int STENCIL_SIZE = 18;
    int64_t stencil[STENCIL_SIZE] = { 0, 0, -1,
                                      0, -1, 0,
                                      -1, 0, 0,
                                      1, 0, 0,
                                      0, 1, 0,
                                      0, 0, 1 };
    while (!voxelHeap.isEmpty())
    {
        float value;
        VoxelIJK voxel = voxelHeap.pop(&value);
        int64_t voxelIndex = inVol->getIndex(voxel.m_ijk);
        set<int64_t> touchingClusters;
        for (int i = 0; i < STENCIL_SIZE; i += 3)
        {
            VoxelIJK neighVoxel(voxel.m_ijk[0] + stencil[i], voxel.m_ijk[1] + stencil[i + 1], voxel.m_ijk[2] + stencil[i + 2]);
            if (inVol->indexValid(neighVoxel.m_ijk))
            {
                int64_t neighIndex = inVol->getIndex(neighVoxel.m_ijk);
                if (membership[neighIndex] != -1)
                {
                    touchingClusters.insert(membership[neighIndex]);
                }
            }
        }
        int numTouching = (int)touchingClusters.size();
        switch (numTouching)
        {
            case 0://make new cluster
            {
                int64_t newCluster = allocCluster(clusterList, deadClusters);
                clusterList[newCluster].addMember(voxel, value, voxelVolume, param_e, param_h);
                membership[voxelIndex] = newCluster;
                break;
            }
            case 1://add to cluster
            {
                int64_t whichCluster = *(touchingClusters.begin());
                clusterList[whichCluster].addMember(voxel, value, voxelVolume, param_e, param_h);
                membership[voxelIndex] = whichCluster;
                accumData[voxelIndex] -= clusterList[whichCluster].accumVal;//the accum value is the current amount less than the peak value that the edge of the cluster has (this node is on the edge)
                break;//so, when the cluster merges or reaches 0, we add the accum value to every member and zero the accum value of the merged cluster, and we get the correct value in the end (with far fewer flops than integrating everywhere)
            }
            default://merge all touching clusters
            {
                int64_t mergedIndex = -1, biggestSize = 0;//find the biggest cluster (in number of members) and use as merged cluster, for optimization purposes
                for (set<int64_t>::iterator iter = touchingClusters.begin(); iter != touchingClusters.end(); ++iter)
                {
                    if ((int64_t)clusterList[*iter].members.size() > biggestSize)
                    {
                        mergedIndex = *iter;
                        biggestSize = (int64_t)clusterList[*iter].members.size();
                    }
                }
                CaretAssertVectorIndex(clusterList, mergedIndex);
                Cluster& mergedCluster = clusterList[mergedIndex];
                mergedCluster.update(value, param_e, param_h);//recalculate to align cluster bottoms
                for (set<int64_t>::iterator iter = touchingClusters.begin(); iter != touchingClusters.end(); ++iter)
                {
                    if (*iter != mergedIndex)//if we are the largest cluster, don't modify the per-voxel accum for members, so merges between small and large clusters are cheap
                    {
                        Cluster& thisCluster = clusterList[*iter];
                        thisCluster.update(value, param_e, param_h);
                        int64_t numMembers = (int64_t)thisCluster.members.size();
                        double correctionVal = thisCluster.accumVal - mergedCluster.accumVal;//fix the accum values in the side cluster so we can add the merged cluster's accum to everything at the end
                        for (int64_t j = 0; j < numMembers; ++j)//add the correction value to every member so that we have the current integrated values correct
                        {
                            int64_t memberIndex = inVol->getIndex(thisCluster.members[j].m_ijk);
                            accumData[memberIndex] += correctionVal;//apply the correction
                            membership[memberIndex] = mergedIndex;//and update membership
                        }
                        mergedCluster.members.insert(mergedCluster.members.end(), thisCluster.members.begin(), thisCluster.members.end());//copy all members
                        mergedCluster.totalVolume += thisCluster.totalVolume;
                        deadClusters.insert(*iter);//kill it
                        vector<VoxelIJK>().swap(clusterList[*iter].members);//also try to deallocate member list
                    }
                }
                mergedCluster.addMember(voxel, value, voxelVolume, param_e, param_h);//will not trigger recomputation, we already recomputed at this value
                accumData[voxelIndex] -= mergedCluster.accumVal;//the voxel they merge on must not get the peak value of the cluster, obviously, so again, record its difference from peak
                membership[voxelIndex] = mergedIndex;
                break;//NOTE: do not reset the accum value of the merged cluster, we specifically avoided modifying the per-voxel accum for its members, so the cluster accum is still in play
            }
        }
    }
    int listSize = (int)clusterList.size();//final cleanup of accum values
    for (int i = 0; i < listSize; ++i)
    {
        if (deadClusters.find(i) != deadClusters.end()) continue;//ignore clusters that don't exist
        Cluster& thisCluster = clusterList[i];
        thisCluster.update(0.0f, param_e, param_h);//update to include the to-zero slice
        int numMembers = (int)thisCluster.members.size();
        for (int j = 0; j < numMembers; ++j)
        {
            accumData[inVol->getIndex(thisCluster.members[j].m_ijk)] += thisCluster.accumVal;//add the resulting slice to all members - their stored data contains the offset between the cluster peak and their corect value
        }
    }
}

float AlgorithmVolumeTFCE::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeTFCE::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
