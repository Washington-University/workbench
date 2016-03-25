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
#include "FloatMatrix.h"
#include "Vector3D.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

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
    exponentOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (1 / (distance ^ n)) as the weighting function (default 2)");
    
    OptionalParameter* badRoiOpt = ret->createOptionalParameter(5, "-bad-voxel-roi", "specify an roi of voxels to overwrite, rather than voxels with value zero");
    badRoiOpt->addVolumeParameter(1, "roi-volume", "volume file, positive values denote voxels to have their values replaced");
    
    OptionalParameter* dataRoiOpt = ret->createOptionalParameter(7, "-data-roi", "specify an roi of where there is data");
    dataRoiOpt->addVolumeParameter(1, "roi-volume", "volume file, positive values denote voxels that have data");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to dilate");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("For all voxels that are designated as bad, if they neighbor a non-bad voxel with data or are within the specified distance of such a voxel, ") +
        "replace the value in the bad voxel with a value calculated from nearby non-bad voxels that have data, otherwise set the value to zero.  " +
        "No matter how small <distance> is, dilation will always use at least the face neighbor voxels.\n\n" +
        "By default, voxels that have data with the value 0 are bad, specify -bad-voxel-roi to only count voxels as bad if they are selected by the roi.  " +
        "If -data-roi is not specified, all voxels are assumed to have data.\n\n" +
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
    float exponent = 2.0f;
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
    AlgorithmVolumeDilate(myProgObj, volIn, distance, myMethod, volOut, badRoi, dataRoi, subvol, exponent);
}

AlgorithmVolumeDilate::AlgorithmVolumeDilate(ProgressObject* myProgObj, const VolumeFile* volIn, const float& distance, const Method& myMethod,
                                             VolumeFile* volOut, const VolumeFile* badRoi, const VolumeFile* dataRoi, const int& subvol, const float& exponent) : AbstractAlgorithm(myProgObj)
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
    vector<vector<float> > volSpace = volIn->getSform();
    Vector3D ivec, jvec, kvec, origin, ijorth, jkorth, kiorth;
    FloatMatrix(volSpace).getAffineVectors(ivec, jvec, kvec, origin);
    ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius kernBox
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    int irange = (int)floor(abs(distance / ivec.dot(jkorth)));
    int jrange = (int)floor(abs(distance / jvec.dot(kiorth)));
    int krange = (int)floor(abs(distance / kvec.dot(ijorth)));
    if (irange < 1) irange = 1;//don't underflow
    if (jrange < 1) jrange = 1;
    if (krange < 1) krange = 1;
    Vector3D kscratch, jscratch, iscratch;
    vector<int> stencil;
    vector<float> stenWeights;
    for (int k = -krange; k <= krange; ++k)
    {
        kscratch = kvec * k;
        for (int j = -jrange; j <= jrange; ++j)
        {
            jscratch = kscratch + jvec * j;
            for (int i = -irange; i <= irange; ++i)
            {
                if (k == 0 && j == 0 && i == 0) continue;
                iscratch = jscratch + ivec * i;
                float tempf = iscratch.length();
                if (tempf <= distance || abs(i) + abs(j) + abs(k) == 1)
                {
                    stencil.push_back(i);
                    stencil.push_back(j);
                    stencil.push_back(k);
                    switch (myMethod)
                    {
                        case NEAREST:
                            stenWeights.push_back(tempf);
                            break;
                        case WEIGHTED:
                            if (tempf == 0.0f) throw AlgorithmException("volume space is degenerate, aborting");
                            stenWeights.push_back(1.0f / pow(tempf, exponent));
                            break;
                    }
                }
            }
        }
    }
    if (myMethod == NEAREST)
    {//sort the stencil by distance, so we can stop early
        CaretSimpleMinHeap<VoxelIJK, float> myHeap;
        int stencilSize = (int)stenWeights.size();
        myHeap.reserve(stencilSize);
        for (int i = 0; i < stencilSize; ++i)
        {
            myHeap.push(VoxelIJK(stencil.data() + i * 3), stenWeights[i]);
        }
        stencil.clear();
        stenWeights.clear();
        while (!myHeap.isEmpty())
        {
            float tempf;
            VoxelIJK myTriple = myHeap.pop(&tempf);
            stenWeights.push_back(tempf);
            stencil.push_back(myTriple.m_ijk[0]);
            stencil.push_back(myTriple.m_ijk[1]);
            stencil.push_back(myTriple.m_ijk[2]);
        }
    }
    if (subvol == -1)
    {
        volOut->reinitialize(volIn->getOriginalDimensions(), volIn->getSform(), volIn->getNumberOfComponents(), volIn->getType());
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
        volOut->reinitialize(outDims, volIn->getSform(), volIn->getNumberOfComponents(), volIn->getType());
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
                if (isLabelData)
                {
                    dilateFrameLabel(volIn, s, c, volOut, s, badRoi, dataRoi, myMethod, stencil, stenWeights);
                } else {
                    dilateFrame(volIn, s, c, volOut, s, badRoi, dataRoi, myMethod, stencil, stenWeights);
                }
            }
        }
    } else {
        for (int c = 0; c < myDims[4]; ++c)
        {
            if (isLabelData)
            {
                dilateFrameLabel(volIn, subvol, c, volOut, 0, badRoi, dataRoi, myMethod, stencil, stenWeights);
            } else {
                dilateFrame(volIn, subvol, c, volOut, 0, badRoi, dataRoi, myMethod, stencil, stenWeights);
            }
        }
    }
}

void AlgorithmVolumeDilate::dilateFrame(const VolumeFile* volIn, const int& insubvol, const int& component, VolumeFile* volOut, const int& outsubvol,
                                        const VolumeFile* badRoi, const VolumeFile* dataRoi, const Method& myMethod, const vector<int>& stencil, const vector<float>& stenWeights)
{
    vector<int64_t> myDims;
    volIn->getDimensions(myDims);
    int stensize = (int)stenWeights.size();
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int k = 0; k < myDims[2]; ++k)
    {
        for (int j = 0; j < myDims[1]; ++j)
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                bool copy = true;
                if (badRoi == NULL)
                {
                    copy = volIn->getValue(i, j, k, insubvol, component) != 0.0f || (dataRoi != NULL && !(dataRoi->getValue(i, j, k) > 0.0f));
                } else {
                    copy = !(badRoi->getValue(i, j, k) > 0.0f);//in case some clown uses NaNs as bad in an roi
                }
                if (copy)
                {
                    volOut->setValue(volIn->getValue(i, j, k, insubvol, component), i, j, k, outsubvol, component);
                } else {
                    Vector3D voxcoord;
                    volIn->indexToSpace(i, j, k, voxcoord);
                    switch (myMethod)
                    {
                        case NEAREST:
                        {
                            int best = -1;
                            if (badRoi == NULL)
                            {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && volIn->getValue(tempindex, insubvol, component) != 0.0f)
                                        {
                                            best = stenind;
                                            break;
                                        }
                                    }
                                }
                            } else {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && !(badRoi->getValue(tempindex) > 0.0f))
                                        {
                                            best = stenind;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (best == -1)
                            {
                                volOut->setValue(0.0f, i, j, k, outsubvol, component);
                            } else {
                                int base = best * 3;
                                int64_t tempindex[3];
                                tempindex[0] = stencil[base] + i;
                                tempindex[1] = stencil[base + 1] + j;
                                tempindex[2] = stencil[base + 2] + k;
                                volOut->setValue(volIn->getValue(tempindex, insubvol, component), i, j, k, outsubvol, component);
                            }
                            break;
                        }
                        case WEIGHTED:
                        {
                            double sum = 0.0, weightsum = 0.0;
                            if (badRoi == NULL)
                            {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        float tempf = volIn->getValue(tempindex, insubvol, component);
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && tempf != 0.0f)
                                        {
                                            float weight = stenWeights[stenind];
                                            sum += weight * tempf;
                                            weightsum += weight;
                                        }
                                    }
                                }
                            } else {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && !(badRoi->getValue(tempindex) > 0.0f))
                                        {
                                            float tempf = volIn->getValue(tempindex, insubvol, component);
                                            float weight = stenWeights[stenind];
                                            sum += weight * tempf;
                                            weightsum += weight;
                                        }
                                    }
                                }
                            }
                            if (weightsum != 0.0)
                            {
                                volOut->setValue(sum / weightsum, i, j, k, outsubvol, component);
                            } else {
                                volOut->setValue(0.0f, i, j, k, outsubvol, component);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

void AlgorithmVolumeDilate::dilateFrameLabel(const VolumeFile* volIn, const int& insubvol, const int& component, VolumeFile* volOut, const int& outsubvol,
                                             const VolumeFile* badRoi, const VolumeFile* dataRoi, const Method& myMethod, const vector<int>& stencil, const vector<float>& stenWeights)
{
    vector<int64_t> myDims;
    volIn->getDimensions(myDims);
    int stensize = (int)stenWeights.size();
    int32_t unlabeledKey = volIn->getMapLabelTable(insubvol)->getUnassignedLabelKey();
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int k = 0; k < myDims[2]; ++k)
    {
        for (int j = 0; j < myDims[1]; ++j)
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                bool copy = true;
                int32_t keyIn = floor(volIn->getValue(i, j, k, insubvol, component) + 0.5f);//fix non-integers
                if (badRoi == NULL)
                {
                    copy = keyIn != unlabeledKey || (dataRoi != NULL && !(dataRoi->getValue(i, j, k) > 0.0f));
                } else {
                    copy = !(badRoi->getValue(i, j, k) > 0.0f);//in case some clown uses NaNs as bad in an roi
                }
                if (copy)
                {
                    volOut->setValue(keyIn, i, j, k, outsubvol, component);
                } else {
                    Vector3D voxcoord;
                    volIn->indexToSpace(i, j, k, voxcoord);
                    switch (myMethod)
                    {
                        case NEAREST:
                        {
                            int best = -1;
                            if (badRoi == NULL)
                            {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && volIn->getValue(tempindex, insubvol, component) != 0.0f)
                                        {
                                            best = stenind;
                                            break;
                                        }
                                    }
                                }
                            } else {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && !(badRoi->getValue(tempindex) > 0.0f))
                                        {
                                            best = stenind;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (best == -1)
                            {
                                volOut->setValue(unlabeledKey, i, j, k, outsubvol, component);
                            } else {
                                int base = best * 3;
                                int64_t tempindex[3];
                                tempindex[0] = stencil[base] + i;
                                tempindex[1] = stencil[base + 1] + j;
                                tempindex[2] = stencil[base + 2] + k;
                                volOut->setValue((int32_t)floor(volIn->getValue(tempindex, insubvol, component) + 0.5f), i, j, k, outsubvol, component);//fix non-integers
                            }//yes, the cast is undefined for silly things like NaN that shouldn't be in a label file, but so are the other paths - we just want consistency between behavior of old and new values
                            break;
                        }
                        case WEIGHTED:
                        {
                            map<int32_t, float> labelSums;
                            if (badRoi == NULL)
                            {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        int32_t tempKey = floor(volIn->getValue(tempindex, insubvol, component) + 0.5f);//fix non-integers
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && tempKey != unlabeledKey)
                                        {
                                            float weight = stenWeights[stenind];
                                            map<int32_t, float>::iterator iter = labelSums.find(tempKey);
                                            if (iter == labelSums.end())
                                            {
                                                labelSums[tempKey] = weight;
                                            } else {
                                                iter->second += weight;
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (int stenind = 0; stenind < stensize; ++stenind)
                                {
                                    int base = stenind * 3;
                                    int64_t tempindex[3];
                                    tempindex[0] = stencil[base] + i;
                                    tempindex[1] = stencil[base + 1] + j;
                                    tempindex[2] = stencil[base + 2] + k;
                                    if (volIn->indexValid(tempindex))
                                    {
                                        if ((dataRoi == NULL || dataRoi->getValue(tempindex) > 0.0f) && !(badRoi->getValue(tempindex) > 0.0f))
                                        {
                                            int32_t tempKey = floor(volIn->getValue(tempindex, insubvol, component) + 0.5f);//fix non-integers
                                            float weight = stenWeights[stenind];
                                            map<int32_t, float>::iterator iter = labelSums.find(tempKey);
                                            if (iter == labelSums.end())
                                            {
                                                labelSums[tempKey] = weight;
                                            } else {
                                                iter->second += weight;
                                            }
                                        }
                                    }
                                }
                            }
                            int32_t outVal = unlabeledKey;
                            float bestSum = -1.0f;//weights should all be positive, so should the sums
                            for (map<int32_t, float>::iterator iter = labelSums.begin(); iter != labelSums.end(); ++iter)
                            {
                                if (iter->second > bestSum)
                                {
                                    outVal = iter->first;
                                    bestSum = iter->second;
                                }
                            }
                            volOut->setValue(outVal, i, j, k, outsubvol, component);
                            break;
                        }
                    }
                }
            }
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
