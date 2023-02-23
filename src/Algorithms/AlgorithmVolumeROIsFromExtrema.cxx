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

#include "AlgorithmVolumeROIsFromExtrema.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "FloatMatrix.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmVolumeROIsFromExtrema::getCommandSwitch()
{
    return "-volume-rois-from-extrema";
}

AString AlgorithmVolumeROIsFromExtrema::getShortDescription()
{
    return "CREATE VOLUME ROI MAPS FROM EXTREMA MAPS";
}

OperationParameters* AlgorithmVolumeROIsFromExtrema::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input volume");
    
    ret->addDoubleParameter(2, "limit", "distance limit from voxel center, in mm");
    
    ret->addVolumeOutputParameter(3, "volume-out", "the output volume");
    
    OptionalParameter* gaussOpt = ret->createOptionalParameter(4, "-gaussian", "generate a gaussian kernel instead of a flat ROI");
    gaussOpt->addDoubleParameter(1, "sigma", "the sigma for the gaussian kernel, in mm");
    
    OptionalParameter* roiOption = ret->createOptionalParameter(5, "-roi", "select a region of interest to use");
    roiOption->addVolumeParameter(1, "roi-volume", "the region to use");
    
    OptionalParameter* overlapOpt = ret->createOptionalParameter(6, "-overlap-logic", "how to handle overlapping ROIs, default ALLOW");
    overlapOpt->addStringParameter(1, "method", "the method of resolving overlaps");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(7, "-subvolume", "select a single subvolume to take the gradient of");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("For each nonzero value in each map, make a map with an ROI around that location.  ") +
        "If the -gaussian option is specified, then normalized gaussian kernels are output instead of ROIs.  " +
        "The <method> argument to -overlap-logic must be one of ALLOW, CLOSEST, or EXCLUDE.  " +
        "ALLOW is the default, and means that ROIs are treated independently and may overlap.  " +
        "CLOSEST means that ROIs may not overlap, and that no ROI contains vertices that are closer to a different seed vertex.  " +
        "EXCLUDE means that ROIs may not overlap, and that any vertex within range of more than one ROI does not belong to any ROI."
    );
    return ret;
}

void AlgorithmVolumeROIsFromExtrema::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVol = myParams->getVolume(1);
    float limit = (float)myParams->getDouble(2);
    VolumeFile* myVolOut = myParams->getOutputVolume(3);
    float sigma = -1.0f;
    OptionalParameter* gaussOpt = myParams->getOptionalParameter(4);
    if (gaussOpt->m_present)
    {
        sigma = (float)gaussOpt->getDouble(1);
        if (sigma <= 0.0f)
        {
            throw AlgorithmException("invalid sigma specified");
        }
    }
    VolumeFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(5);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getVolume(1);
    }
    OverlapLogicEnum::Enum overlapType = OverlapLogicEnum::ALLOW;
    OptionalParameter* overlapOpt = myParams->getOptionalParameter(6);
    if (overlapOpt->m_present)
    {
        bool ok = false;
        overlapType = OverlapLogicEnum::fromName(overlapOpt->getString(1), &ok);
        if (!ok) throw AlgorithmException("unrecognized overlap method: " + overlapOpt->getString(1));
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(7);
    int subvolNum = -1;
    if (subvolSelect->m_present)
    {
        subvolNum = (int)myVol->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    AlgorithmVolumeROIsFromExtrema(myProgObj, myVol, limit, myVolOut, sigma, myRoi, overlapType, subvolNum);
}

AlgorithmVolumeROIsFromExtrema::AlgorithmVolumeROIsFromExtrema(ProgressObject* myProgObj, const VolumeFile* myVol, const float& limit, VolumeFile* myVolOut, const float& sigma,
                                                               const VolumeFile* myRoi, const OverlapLogicEnum::Enum& overlapType, const int& subvolNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const float* roiFrame = NULL;
    if (myRoi != NULL)
    {
        if (!myRoi->matchesVolumeSpace(myVol)) throw AlgorithmException("roi volume has a different volume space");
        roiFrame = myRoi->getFrame();
    }
    int64_t extremaCount = 0;
    vector<int64_t> myDims;
    myVol->getDimensions(myDims);
    int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
    if (subvolNum == -1)
    {
        if (roiFrame == NULL)
        {
            for (int c = 0; c < myDims[4]; ++c)
            {
                for (int b = 0; b < myDims[3]; ++b)
                {
                    const float* frame = myVol->getFrame(b, c);
                    for (int64_t index = 0; index < frameSize; ++index)
                    {
                        if (frame[index] != 0.0f) ++extremaCount;
                    }
                }
            }
        } else {
            for (int c = 0; c < myDims[4]; ++c)
            {
                for (int b = 0; b < myDims[3]; ++b)
                {
                    const float* frame = myVol->getFrame(b, c);
                    for (int64_t index = 0; index < frameSize; ++index)
                    {
                        if (roiFrame[index] > 0.0f && frame[index] != 0.0f) ++extremaCount;
                    }
                }
            }
        }
    } else {
        if (roiFrame == NULL)
        {
            for (int c = 0; c < myDims[4]; ++c)
            {
                const float* frame = myVol->getFrame(subvolNum, c);
                for (int64_t index = 0; index < frameSize; ++index)
                {
                    if (frame[index] != 0.0f) ++extremaCount;
                }
            }
        } else {
            for (int c = 0; c < myDims[4]; ++c)
            {
                const float* frame = myVol->getFrame(subvolNum, c);
                for (int64_t index = 0; index < frameSize; ++index)
                {
                    if (roiFrame[index] > 0.0f && frame[index] != 0.0f) ++extremaCount;
                }
            }
        }
    }
    if (extremaCount == 0) CaretLogWarning("no nonzero values in input volume file, output file will be empty");
    Vector3D ivec, jvec, kvec, offset, ijorth, jkorth, kiorth;
    FloatMatrix(myVol->getVolumeSpace().getSform()).getAffineVectors(ivec, jvec, kvec, offset);//this should probably be made more accessible
    ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius limit
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    int irange = (int)floor(abs(limit / ivec.dot(jkorth)));
    int jrange = (int)floor(abs(limit / jvec.dot(kiorth)));
    int krange = (int)floor(abs(limit / kvec.dot(ijorth)));
    Vector3D iscratch, jscratch, kscratch;
    vector<int> stencil;
    vector<float> stencildist;
    for (int k = -krange; k <= krange; ++k)
    {
        kscratch = kvec * k;
        for (int j = -jrange; j <= jrange; ++j)
        {
            jscratch = kscratch + jvec * j;
            for (int i = -irange; i <= irange; ++i)
            {
                iscratch = jscratch + ivec * i;
                float tempf = iscratch.length();
                if (tempf <= limit)
                {
                    stencil.push_back(i);
                    stencil.push_back(j);
                    stencil.push_back(k);
                    stencildist.push_back(tempf);
                }
            }
        }
    }
    vector<float> excludeDists(frameSize, -1.0f);
    vector<int64_t> excludeSources(frameSize, -1);
    vector<map<int64_t, float> > roiLists(extremaCount);
    int64_t mapCounter = 0;
    if (subvolNum == -1)
    {
        for (int c = 0; c < myDims[4]; ++c)
        {
            for (int b = 0; b < myDims[3]; ++b)
            {
                const float* data = myVol->getFrame(b, c);
                processFrame(data, excludeDists, excludeSources, roiLists, mapCounter, stencil, stencildist, roiFrame, overlapType, myVol->getVolumeSpace());
            }
        }
    } else {
        for (int c = 0; c < myDims[4]; ++c)
        {
            const float* data = myVol->getFrame(subvolNum, c);
            processFrame(data, excludeDists, excludeSources, roiLists, mapCounter, stencil, stencildist, roiFrame, overlapType, myVol->getVolumeSpace());
        }
    }
    CaretAssert(mapCounter == extremaCount);
    vector<int64_t> outDims = myDims;
    outDims.resize(4);
    outDims[3] = extremaCount;
    myVolOut->reinitialize(outDims, myVol->getSform(), 1, SubvolumeAttributes::ANATOMY, myVol->m_header);
    vector<float> tempFrame(frameSize, 0.0f);
    if (sigma > 0.0f)
    {
        float gaussDenom = -0.5f / sigma / sigma;
        for (int64_t i = 0; i < mapCounter; ++i)
        {
            double accum = 0.0;
            map<int64_t, float>::iterator myEnd = roiLists[i].end();
            for (map<int64_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                float gaussVal = exp(iter->second * iter->second * gaussDenom);
                accum += gaussVal;
                tempFrame[iter->first] = gaussVal;//initial kernel value
            }
            for (map<int64_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempFrame[iter->first] /= accum;//normalize
            }
            myVolOut->setFrame(tempFrame.data(), i);
            for (map<int64_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempFrame[iter->first] = 0.0f;//rezero changed values for next map
            }
        }
    } else {
        for (int64_t i = 0; i < mapCounter; ++i)
        {
            map<int64_t, float>::iterator myEnd = roiLists[i].end();
            for (map<int64_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempFrame[iter->first] = 1.0f;//make roi
            }
            myVolOut->setFrame(tempFrame.data(), i);
            for (map<int64_t, float>::iterator iter = roiLists[i].begin(); iter != myEnd; ++iter)
            {
                tempFrame[iter->first] = 0.0f;//rezero changed values for next map
            }
        }
    }
}

void AlgorithmVolumeROIsFromExtrema::processFrame(const float* data, vector<float>& excludeDists, vector<int64_t>& excludeSources, vector<map<int64_t, float> >& roiLists,
                                                  int64_t& mapCounter, const vector<int>& stencil, const vector<float>& stencildist, const float* roiFrame,
                                                  const OverlapLogicEnum::Enum& overlapType, const VolumeSpace& mySpace)
{
    const int64_t* myDims = mySpace.getDims();
    int stencilSize = (int)stencildist.size();
    for (int k = 0; k < myDims[2]; ++k)
    {
        for (int j = 0; j < myDims[1]; ++j)
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                int64_t myIndex = mySpace.getIndex(i, j, k);
                if ((roiFrame == NULL || roiFrame[myIndex] > 0.0f) && data[myIndex] != 0.0f)
                {
                    switch (overlapType)
                    {
                        case OverlapLogicEnum::ALLOW:
                            if (roiFrame == NULL)
                            {
                                for (int s = 0; s < stencilSize; ++s)
                                {
                                    int s3 = s * 3;
                                    const int thisVoxel[3] = { i + stencil[s3], j + stencil[s3 + 1], k + stencil[s3 + 2] };
                                    if (mySpace.indexValid(thisVoxel))
                                    {
                                        roiLists[mapCounter][mySpace.getIndex(thisVoxel)] = stencildist[s];
                                    }
                                }
                            } else {
                                for (int s = 0; s < stencilSize; ++s)
                                {
                                    int s3 = s * 3;
                                    const int thisVoxel[3] = { i + stencil[s3], j + stencil[s3 + 1], k + stencil[s3 + 2] };
                                    if (mySpace.indexValid(thisVoxel) && roiFrame[mySpace.getIndex(thisVoxel)] > 0.0f)
                                    {
                                        roiLists[mapCounter][mySpace.getIndex(thisVoxel)] = stencildist[s];
                                    }
                                }
                            }
                            break;
                        case OverlapLogicEnum::CLOSEST:
                            for (int s = 0; s < stencilSize; ++s)
                            {
                                int s3 = s * 3;
                                const int thisVoxel[3] = { i + stencil[s3], j + stencil[s3 + 1], k + stencil[s3 + 2] };
                                if (mySpace.indexValid(thisVoxel) && (roiFrame == NULL || roiFrame[mySpace.getIndex(thisVoxel)] > 0.0f))
                                {
                                    int64_t thisIndex = mySpace.getIndex(thisVoxel);
                                    const float& thisDist = stencildist[s];
                                    if (excludeDists[thisIndex] < 0.0f)
                                    {
                                        excludeDists[thisIndex] = thisDist;
                                        excludeSources[thisIndex] = mapCounter;
                                        roiLists[mapCounter][thisIndex] = thisDist;
                                    } else {
                                        if (excludeDists[thisIndex] > thisDist)
                                        {
                                            roiLists[excludeSources[thisIndex]].erase(thisIndex);
                                            excludeDists[thisIndex] = thisDist;
                                            excludeSources[thisIndex] = mapCounter;
                                            roiLists[mapCounter][thisIndex] = thisDist;
                                        }
                                    }
                                }
                            }
                            break;
                        case OverlapLogicEnum::EXCLUDE:
                            for (int s = 0; s < stencilSize; ++s)
                            {
                                int s3 = s * 3;
                                const int thisVoxel[3] = { i + stencil[s3], j + stencil[s3 + 1], k + stencil[s3 + 2] };
                                if (mySpace.indexValid(thisVoxel) && (roiFrame == NULL || roiFrame[mySpace.getIndex(thisVoxel)] > 0.0f))
                                {
                                    int64_t thisIndex = mySpace.getIndex(thisVoxel);
                                    const float& thisDist = stencildist[s];
                                    if (excludeDists[thisIndex] < 0.0f)
                                    {
                                        excludeDists[thisIndex] = thisDist;
                                        excludeSources[thisIndex] = mapCounter;
                                        roiLists[mapCounter][thisIndex] = thisDist;
                                    } else {
                                        if (excludeSources[thisIndex] != -1)
                                        {
                                            roiLists[excludeSources[thisIndex]].erase(thisIndex);
                                            excludeSources[thisIndex] = -1;
                                        }
                                    }
                                }
                            }
                            break;
                    }
                    ++mapCounter;
                }
            }
        }
    }
}

float AlgorithmVolumeROIsFromExtrema::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeROIsFromExtrema::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
