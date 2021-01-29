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

#include "AlgorithmVolumeParcelResampling.h"
#include "AlgorithmException.h"
#include "VolumeFile.h"
#include "AlgorithmVolumeSmoothing.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "Vector3D.h"

#include <vector>
#include <map>
#include <utility>
#include <cmath>

using namespace caret;
using namespace std;

const int FIX_ZEROS_POST_ITERATIONS = 10;//number of times to do the "find remaining zeros and try to fill" code before giving up when -fix-zeros is specified

AString AlgorithmVolumeParcelResampling::getCommandSwitch()
{
    return "-volume-parcel-resampling";
}

AString AlgorithmVolumeParcelResampling::getShortDescription()
{
    return "SMOOTH AND RESAMPLE VOLUME PARCELS";
}

OperationParameters* AlgorithmVolumeParcelResampling::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input data volume");
    
    ret->addVolumeParameter(2, "cur-parcels", "label volume of where the parcels currently are");
    
    ret->addVolumeParameter(3, "new-parcels", "label volume of where the parcels should be");
    
    ret->addDoubleParameter(4, "kernel", "gaussian kernel size in mm to smooth by during resampling, as sigma by default");
    
    ret->addVolumeOutputParameter(5, "volume-out", "output volume");
    
    ret->createOptionalParameter(6, "-fix-zeros", "treat zero values as not being data");
    
    ret->createOptionalParameter(8, "-fwhm", "smoothing kernel size is FWHM, not sigma");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(7, "-subvolume", "select a single subvolume as input");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Smooths and resamples the region inside each label in cur-parcels to the region of the same label name in new-parcels.  ") +
        "Any voxels in the output label region but outside the input label region will be extrapolated from nearby data.  " +
        "The -fix-zeros option causes the smoothing to not use an input value if it is zero, but still write a smoothed value to the voxel, and after smoothing " +
        "is complete, it will check for any remaining values of zero, and fill them in with extrapolated values.\n\nNote: all volumes must have " + 
        "the same dimensions and spacing.  To use a different output space, see -volume-parcel-resampling-generic."
    );
    return ret;
}

void AlgorithmVolumeParcelResampling::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
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
    AlgorithmVolumeParcelResampling(myProgObj, inVol, curLabel, newLabel, kernel, outVol, fixZeros, subvolNum);
}

AlgorithmVolumeParcelResampling::AlgorithmVolumeParcelResampling(ProgressObject* myProgObj, const VolumeFile* inVol, const VolumeFile* curLabel, const VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const bool& fixZeros, const int& subvolNum) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(inVol != NULL);
    CaretAssert(curLabel != NULL);
    CaretAssert(newLabel != NULL);
    CaretAssert(outVol != NULL);
    if (!inVol->matchesVolumeSpace(curLabel) || !inVol->matchesVolumeSpace(newLabel))
    {
        throw AlgorithmException("volume spacing or dimension mismatch");
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
    generateVoxelLists(matchedLabels, curLabel, newLabel, voxelLists);
    /*ProgressObject* subAlgProgress = NULL;
    if (myProgObj != NULL)//TODO: create a vector of progress objects based on number of matched labels, and possibly counts of voxels
    {
        subAlgProgress = myProgObj->addAlgorithm(AlgorithmVolumeParcelSmoothing::getAlgorithmWeight());
    }//*/
    LevelProgress myProgress(myProgObj);
    if (fixZeros)
    {
        resampleFixZeros(myProgress, matchedLabels, voxelLists, inVol, curLabel, newLabel, kernel, outVol, subvolNum);
    } else {
        resample(myProgress, matchedLabels, voxelLists, inVol, curLabel, newLabel, kernel, outVol, subvolNum);
    }
}

void AlgorithmVolumeParcelResampling::matchLabels(const VolumeFile* curLabel, const VolumeFile* newLabel, vector<pair<int, int> >& matchedLabels)
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

void AlgorithmVolumeParcelResampling::generateVoxelLists(const vector<pair<int, int> >& matchedLabels, const VolumeFile* curLabel, const VolumeFile* newLabel, vector<vector<int64_t> >& voxelLists)
{
    map<int, int> curLabelReverse, newLabelReverse;
    for (int i = 0; i < (int)matchedLabels.size(); ++i)
    {
        curLabelReverse[matchedLabels[i].first] = i;
        newLabelReverse[matchedLabels[i].second] = i;
    }
    voxelLists.resize(matchedLabels.size());
    vector<int64_t> myDims;
    curLabel->getDimensions(myDims);
    for (int64_t k = 0; k < myDims[2]; ++k)
    {
        for (int64_t j = 0; j < myDims[1]; ++j)
        {
            for (int64_t i = 0; i < myDims[0]; ++i)
            {
                int curValue = (int)floor(curLabel->getValue(i, j, k) + 0.5f);
                int newValue = (int)floor(newLabel->getValue(i, j, k) + 0.5f);
                map<int, int>::iterator curiter = curLabelReverse.find(curValue), newiter = newLabelReverse.find(newValue);
                if (curiter != curLabelReverse.end())
                {
                    voxelLists[curiter->second].push_back(i);
                    voxelLists[curiter->second].push_back(j);
                    voxelLists[curiter->second].push_back(k);
                }
                if (newiter != newLabelReverse.end() && (curiter == curLabelReverse.end() || newiter->second != curiter->second))
                {
                    voxelLists[newiter->second].push_back(i);
                    voxelLists[newiter->second].push_back(j);
                    voxelLists[newiter->second].push_back(k);
                }
            }
        }
    }
}

void AlgorithmVolumeParcelResampling::resample(LevelProgress& myProgress, const vector<pair<int, int> >& matchedLabels,
                                               const vector<vector<int64_t> >& voxelLists, const VolumeFile* inVol,
                                               const VolumeFile* curLabel, const VolumeFile* newLabel,
                                               const float& kernel, VolumeFile* outVol, const int& subvolNum)
{
    float kernBox = kernel * 3.0f;
    vector<vector<float> > volSpace = inVol->getSform();//copied from volume smoothing, perhaps this should be in a convenience method in VolumeFile
    Vector3D ivec, jvec, kvec, origin, ijorth, jkorth, kiorth;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2]; origin[0] = volSpace[0][3];//needs to be this verbose because the axis and origin vectors are column vectors
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2]; origin[1] = volSpace[1][3];//while vector<vector<> > is a column of row vectors
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2]; origin[2] = volSpace[2][3];
    ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius kernBox
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    int irange = (int)floor(abs(kernBox / ivec.dot(jkorth)));
    int jrange = (int)floor(abs(kernBox / jvec.dot(kiorth)));
    int krange = (int)floor(abs(kernBox / kvec.dot(ijorth)));
    if (irange < 1) irange = 1;//don't underflow
    if (jrange < 1) jrange = 1;
    if (krange < 1) krange = 1;
    map<int, int> curLabelReverse, newLabelReverse;
    int numLabels = (int)matchedLabels.size();
    for (int i = 0; i < numLabels; ++i)
    {
        curLabelReverse[matchedLabels[i].first] = i;
        newLabelReverse[matchedLabels[i].second] = i;
    }
    vector<int64_t> myDims;
    inVol->getDimensions(myDims);
    if (subvolNum == -1)
    {
        outVol->reinitialize(inVol->getOriginalDimensions(), inVol->getSform(), myDims[4], inVol->getType(), inVol->m_header);
    } else {
        vector<int64_t> newDims = inVol->getOriginalDimensions();
        newDims.resize(3);//discard nonspatial dimentions
        outVol->reinitialize(newDims, inVol->getSform(), myDims[4], inVol->getType(), inVol->m_header);
    }
    outVol->setValueAllVoxels(0.0f);
    const GiftiLabelTable* curTable = curLabel->getMapLabelTable(0);
    for (int whichList = 0; whichList < numLabels; ++whichList)
    {
        int curLabelValue = matchedLabels[whichList].first;
        int newLabelValue = matchedLabels[whichList].second;
        myProgress.reportProgress(((float)whichList) / numLabels);
        myProgress.setTask("Processing parcel " + curTable->getLabelName(curLabelValue));
        const vector<int64_t>& thisList = voxelLists[whichList];
        int64_t listSize = (int64_t)thisList.size();
        if (listSize > 2)//NOTE: this should NEVER be something other than a multiple of 3, but check against what we will actually access anyway
        {
            int64_t extrema[6] = { thisList[0],
                                   thisList[0],
                                   thisList[1],
                                   thisList[1],
                                   thisList[2],
                                   thisList[2] };
            for (int64_t base = 3; base < listSize; base += 3)
            {
                if (thisList[base] < extrema[0]) extrema[0] = thisList[base];
                if (thisList[base] > extrema[1]) extrema[1] = thisList[base];
                if (thisList[base + 1] < extrema[2]) extrema[2] = thisList[base + 1];
                if (thisList[base + 1] > extrema[3]) extrema[3] = thisList[base + 1];
                if (thisList[base + 2] < extrema[4]) extrema[4] = thisList[base + 2];
                if (thisList[base + 2] > extrema[5]) extrema[5] = thisList[base + 2];
            }
            vector<int64_t> boxdims;
            boxdims.push_back(extrema[1] - extrema[0] + 1);
            boxdims.push_back(extrema[3] - extrema[2] + 1);
            boxdims.push_back(extrema[5] - extrema[4] + 1);
            VolumeFile roibox(boxdims, inVol->getSform());
            roibox.setValueAllVoxels(0.0f);
            vector<int64_t> curList, newList;//make the 2 separate lists, so we don't need to test the label volume
            for (int64_t base = 0; base < listSize; base += 3)//could do this when we make the merged list...maybe use member variables to cut the argument clutter
            {
                int curvalue = (int)floor(curLabel->getValue(thisList[base], thisList[base + 1], thisList[base + 2]) + 0.5f);
                int newvalue = (int)floor(newLabel->getValue(thisList[base], thisList[base + 1], thisList[base + 2]) + 0.5f);
                if (curvalue == curLabelValue)
                {
                    curList.push_back(thisList[base]);
                    curList.push_back(thisList[base + 1]);
                    curList.push_back(thisList[base + 2]);
                }
                if (newvalue == newLabelValue)
                {
                    newList.push_back(thisList[base]);
                    newList.push_back(thisList[base + 1]);
                    newList.push_back(thisList[base + 2]);
                }
            }
            int64_t curListSize = (int64_t)curList.size();
            int64_t newListSize = (int64_t)newList.size();
            if (subvolNum == -1)
            {
                boxdims.push_back(myDims[3]);
                VolumeFile inbox(boxdims, inVol->getSform(), myDims[4]), outbox;
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int s = 0; s < myDims[3]; ++s)
                    {
                        for (int64_t base = 0; base < curListSize; base += 3)
                        {
                            if (s == 0 && c == 0)//first, smooth within only the current label
                            {
                                roibox.setValue(1.0f, curList[base] - extrema[0], curList[base + 1] - extrema[2], curList[base + 2] - extrema[4]);
                            }
                            inbox.setValue(inVol->getValue(curList[base], curList[base + 1], curList[base + 2], s, c),
                                curList[base] - extrema[0], curList[base + 1] - extrema[2], curList[base + 2] - extrema[4], s, c);
                        }
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &inbox, kernel, &outbox, &roibox, false);
                float kernelMult = -1.0f / kernel / kernel / 2.0f;//precompute the part of the kernel function that doesn't change
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int s = 0; s < myDims[3]; ++s)
                    {
                        const float* inFrame = inVol->getFrame(s, c);
                        const float* labelFrame = curLabel->getFrame();
                        for (int64_t base = 0; base < newListSize; base += 3)
                        {
                            int myCurVal = (int)floor(curLabel->getValue(newList[base], newList[base + 1], newList[base + 2]) + 0.5f);
                            if (myCurVal == curLabelValue)
                            {
                                outVol->setValue(outbox.getValue(newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], s, c),
                                    newList[base], newList[base + 1], newList[base + 2], s, c);
                            } else {
                                float sum = 0.0f, weightsum = 0.0f;//coded in-place for now, its a special restricted case of volume dilate, copied out of nonorth volume smoothing
                                int i = newList[base], j = newList[base + 1], k = newList[base + 2];//special casing orthogonal would be faster, but harder to follow/debug, and more code
                                int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                                if (imin < 0) imin = 0;
                                if (imax > myDims[0]) imax = myDims[0];
                                int jmin = j - jrange, jmax = j + jrange + 1;
                                if (jmin < 0) jmin = 0;
                                if (jmax > myDims[1]) jmax = myDims[1];
                                int kmin = k - krange, kmax = k + krange + 1;
                                if (kmin < 0) kmin = 0;
                                if (kmax > myDims[2]) kmax = myDims[2];
                                Vector3D kscratch, jscratch, iscratch;
                                for (int kkern = kmin; kkern < kmax; ++kkern)
                                {
                                    kscratch = kvec * (kkern - k);
                                    int64_t kindpart = kkern * myDims[1];
                                    for (int jkern = jmin; jkern < jmax; ++jkern)
                                    {
                                        jscratch = kscratch + jvec * (jkern - j);
                                        int64_t jindpart = (kindpart + jkern) * myDims[0];
                                        for (int ikern = imin; ikern < imax; ++ikern)
                                        {
                                            int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                            int curVal = (int)floor(labelFrame[thisIndex] + 0.5f);
                                            if (curVal == curLabelValue)
                                            {
                                                iscratch = jscratch + ivec * (ikern - i);
                                                float tempf = iscratch.length();
                                                float weight = exp(tempf * tempf * kernelMult);
                                                sum += weight * inFrame[thisIndex];
                                                weightsum += weight;
                                            }
                                        }
                                    }
                                }
                                if (weightsum != 0.0f)
                                {
                                    outVol->setValue(sum / weightsum, newList[base], newList[base + 1], newList[base + 2], s, c);
                                }//should already be 0, so don't need to handle the else
                            }
                        }
                    }
                }
            } else {
                VolumeFile inbox(boxdims, inVol->getSform(), myDims[4]), outbox;
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int64_t base = 0; base < curListSize; base += 3)
                    {
                        int myvalue = (int)floor(curLabel->getValue(curList[base], curList[base + 1], curList[base + 2]) + 0.5f);
                        if (myvalue == matchedLabels[whichList].first)
                        {//first, only smooth within the ROI of the current label, otherwise we would smooth in some zeros that aren't data
                            if (c == 0)
                            {
                                roibox.setValue(1.0f, curList[base] - extrema[0], curList[base + 1] - extrema[2], curList[base + 2] - extrema[4]);
                            }
                            inbox.setValue(inVol->getValue(curList[base], curList[base + 1], curList[base + 2], subvolNum, c),
                                curList[base] - extrema[0], curList[base + 1] - extrema[2], curList[base + 2] - extrema[4], 0, c);
                        }
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &inbox, kernel, &outbox, &roibox, false);
                float kernelMult = -1.0f / kernel / kernel / 2.0f;//precompute the part of the kernel function that doesn't change
                for (int c = 0; c < myDims[4]; ++c)
                {
                    const float* inFrame = inVol->getFrame(subvolNum, c);
                    const float* labelFrame = curLabel->getFrame();
                    for (int64_t base = 0; base < newListSize; base += 3)
                    {
                        int myCurVal = (int)floor(curLabel->getValue(newList[base], newList[base + 1], newList[base + 2]) + 0.5f);
                        if (myCurVal == curLabelValue)
                        {
                            outVol->setValue(outbox.getValue(newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], 0, c),
                                newList[base], newList[base + 1], newList[base + 2], 0, c);
                        } else {
                            float sum = 0.0f, weightsum = 0.0f;//coded in-place for now, its a special restricted case of volume dilate, copied out of nonorth volume smoothing
                            int i = newList[base], j = newList[base + 1], k = newList[base + 2];//special casing orthogonal would be faster, but harder to follow/debug, and more code
                            int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                            if (imin < 0) imin = 0;
                            if (imax > myDims[0]) imax = myDims[0];
                            int jmin = j - jrange, jmax = j + jrange + 1;
                            if (jmin < 0) jmin = 0;
                            if (jmax > myDims[1]) jmax = myDims[1];
                            int kmin = k - krange, kmax = k + krange + 1;
                            if (kmin < 0) kmin = 0;
                            if (kmax > myDims[2]) kmax = myDims[2];
                            Vector3D kscratch, jscratch, iscratch;
                            for (int kkern = kmin; kkern < kmax; ++kkern)
                            {
                                kscratch = kvec * (kkern - k);
                                int64_t kindpart = kkern * myDims[1];
                                for (int jkern = jmin; jkern < jmax; ++jkern)
                                {
                                    jscratch = kscratch + jvec * (jkern - j);
                                    int64_t jindpart = (kindpart + jkern) * myDims[0];
                                    for (int ikern = imin; ikern < imax; ++ikern)
                                    {
                                        int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                        int curVal = (int)floor(labelFrame[thisIndex] + 0.5f);
                                        if (curVal == curLabelValue)
                                        {
                                            iscratch = jscratch + ivec * (ikern - i);
                                            float tempf = iscratch.length();
                                            float weight = exp(tempf * tempf * kernelMult);
                                            sum += weight * inFrame[thisIndex];
                                            weightsum += weight;
                                        }
                                    }
                                }
                            }
                            if (weightsum != 0.0f)
                            {
                                outVol->setValue(sum / weightsum, newList[base], newList[base + 1], newList[base + 2], 0, c);
                            }//should already be 0, so don't need to handle the else
                        }
                    }
                }
            }
        }
    }
}

void AlgorithmVolumeParcelResampling::resampleFixZeros(LevelProgress& myProgress, const vector<pair<int, int> >& matchedLabels,
                                                       const vector<vector<int64_t> >& voxelLists, const VolumeFile* inVol,
                                                       const VolumeFile* curLabel, const VolumeFile* newLabel,
                                                       const float& kernel, VolumeFile* outVol, const int& subvolNum)
{
    float kernBox = kernel * 3.0f;
    vector<vector<float> > volSpace = inVol->getSform();//copied from volume smoothing, perhaps this should be in a convenience method in VolumeFile
    Vector3D ivec, jvec, kvec, origin, ijorth, jkorth, kiorth;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2]; origin[0] = volSpace[0][3];//needs to be this verbose because the axis and origin vectors are column vectors
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2]; origin[1] = volSpace[1][3];//while vector<vector<> > is a column of row vectors
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2]; origin[2] = volSpace[2][3];
    ijorth = ivec.cross(jvec).normal();//find the bounding box that encloses a sphere of radius kernBox
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    int irange = (int)floor(abs(kernBox / ivec.dot(jkorth)));
    int jrange = (int)floor(abs(kernBox / jvec.dot(kiorth)));
    int krange = (int)floor(abs(kernBox / kvec.dot(ijorth)));
    if (irange < 1) irange = 1;//don't underflow
    if (jrange < 1) jrange = 1;
    if (krange < 1) krange = 1;
    map<int, int> curLabelReverse, newLabelReverse;
    int numLabels = (int)matchedLabels.size();
    for (int i = 0; i < numLabels; ++i)
    {
        curLabelReverse[matchedLabels[i].first] = i;
        newLabelReverse[matchedLabels[i].second] = i;
    }
    vector<int64_t> myDims;
    inVol->getDimensions(myDims);
    if (subvolNum == -1)
    {
        outVol->reinitialize(inVol->getOriginalDimensions(), inVol->getSform(), myDims[4], inVol->getType(), inVol->m_header);
    } else {
        vector<int64_t> newDims = inVol->getOriginalDimensions();
        newDims.resize(3);//discard nonspatial dimentions
        outVol->reinitialize(newDims, inVol->getSform(), myDims[4], inVol->getType(), inVol->m_header);
    }
    outVol->setValueAllVoxels(0.0f);
    const GiftiLabelTable* curTable = curLabel->getMapLabelTable(0);
    for (int whichList = 0; whichList < numLabels; ++whichList)
    {
        int curLabelValue = matchedLabels[whichList].first;
        int newLabelValue = matchedLabels[whichList].second;
        myProgress.reportProgress(((float)whichList) / numLabels);
        myProgress.setTask("Processing parcel " + curTable->getLabelName(curLabelValue));
        const vector<int64_t>& thisList = voxelLists[whichList];
        int64_t listSize = (int64_t)thisList.size();
        if (listSize > 2)//NOTE: this should NEVER be something other than a multiple of 3, but check against what we will actually access anyway
        {
            int64_t extrema[6] = { thisList[0],
                                   thisList[0],
                                   thisList[1],
                                   thisList[1],
                                   thisList[2],
                                   thisList[2] };
            for (int64_t base = 3; base < listSize; base += 3)
            {
                if (thisList[base] < extrema[0]) extrema[0] = thisList[base];
                if (thisList[base] > extrema[1]) extrema[1] = thisList[base];
                if (thisList[base + 1] < extrema[2]) extrema[2] = thisList[base + 1];
                if (thisList[base + 1] > extrema[3]) extrema[3] = thisList[base + 1];
                if (thisList[base + 2] < extrema[4]) extrema[4] = thisList[base + 2];
                if (thisList[base + 2] > extrema[5]) extrema[5] = thisList[base + 2];
            }
            vector<int64_t> boxdims;
            boxdims.push_back(extrema[1] - extrema[0] + 1);
            boxdims.push_back(extrema[3] - extrema[2] + 1);
            boxdims.push_back(extrema[5] - extrema[4] + 1);
            VolumeFile roibox(boxdims, inVol->getSform());
            roibox.setValueAllVoxels(0.0f);
            vector<int64_t> newList;//make the separate new list, so we don't need to test the label volume
            for (int64_t base = 0; base < listSize; base += 3)//could do this when we make the merged list...maybe use member variables to cut the argument clutter
            {
                int newvalue = (int)floor(newLabel->getValue(thisList[base], thisList[base + 1], thisList[base + 2]) + 0.5f);
                if (newvalue == newLabelValue)
                {
                    newList.push_back(thisList[base]);
                    newList.push_back(thisList[base + 1]);
                    newList.push_back(thisList[base + 2]);
                }
            }
            int64_t newListSize = (int64_t)newList.size();
            if (subvolNum == -1)
            {
                boxdims.push_back(myDims[3]);
                VolumeFile inbox(boxdims, inVol->getSform(), myDims[4]), outbox;
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int s = 0; s < myDims[3]; ++s)
                    {
                        for (int64_t base = 0; base < listSize; base += 3)
                        {
                            if (s == 0 && c == 0)//smooth within BOTH labels, but only copy in from current label
                            {
                                roibox.setValue(1.0f, thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4]);
                            }
                            int curValue = (int)floor(curLabel->getValue(thisList[base], thisList[base + 1], thisList[base + 2]) + 0.5f);
                            if (curValue == curLabelValue)
                            {
                                inbox.setValue(inVol->getValue(thisList[base], thisList[base + 1], thisList[base + 2], s, c),
                                    thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4], s, c);
                            } else {
                                inbox.setValue(0.0f, thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4], s, c);
                            }
                        }
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &inbox, kernel, &outbox, &roibox, true);
                float kernelMult = -1.0f / kernel / kernel / 2.0f;//precompute the part of the kernel function that doesn't change
                VolumeFile* current = &outbox, *next = &inbox, *tempvol;//reuse inbox as scratch space for iterated dilation
                const float* labelFrame = newLabel->getFrame();
                int fixIter;
                for (fixIter = 0; fixIter < FIX_ZEROS_POST_ITERATIONS; ++fixIter)
                {
                    bool again = false;
                    for (int c = 0; c < myDims[4]; ++c)//since fix zeros smoothing doesn't consider components as multidimensional datatypes, neither should this
                    {
                        for (int s = 0; s < myDims[3]; ++s)
                        {
                            for (int64_t base = 0; base < newListSize; base += 3)
                            {
                                float curVal = current->getValue(newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], s, c);
                                if (curVal == 0.0f)
                                {
                                    float sum = 0.0f, weightsum = 0.0f;//coded in-place for now, its a special restricted case of volume dilate, copied out of nonorth volume smoothing
                                    int i = newList[base], j = newList[base + 1], k = newList[base + 2];//special casing orthogonal would be faster, but harder to follow/debug, and more code
                                    int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                                    if (imin < 0) imin = 0;
                                    if (imax > myDims[0]) imax = myDims[0];
                                    int jmin = j - jrange, jmax = j + jrange + 1;
                                    if (jmin < 0) jmin = 0;
                                    if (jmax > myDims[1]) jmax = myDims[1];
                                    int kmin = k - krange, kmax = k + krange + 1;
                                    if (kmin < 0) kmin = 0;
                                    if (kmax > myDims[2]) kmax = myDims[2];
                                    Vector3D kscratch, jscratch, iscratch;
                                    for (int kkern = kmin; kkern < kmax; ++kkern)
                                    {
                                        kscratch = kvec * (kkern - k);
                                        int64_t kindpart = kkern * myDims[1];
                                        for (int jkern = jmin; jkern < jmax; ++jkern)
                                        {
                                            jscratch = kscratch + jvec * (jkern - j);
                                            int64_t jindpart = (kindpart + jkern) * myDims[0];
                                            for (int ikern = imin; ikern < imax; ++ikern)
                                            {
                                                int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                                int tempi = (int)floor(labelFrame[thisIndex] + 0.5f);
                                                if (tempi == curLabelValue)
                                                {
                                                    float dataVal = current->getValue(ikern - extrema[0], jkern - extrema[2], kkern - extrema[4], s, c);
                                                    if (dataVal != 0.0f)
                                                    {
                                                        iscratch = jscratch + ivec * (ikern - i);
                                                        float tempf = iscratch.length();
                                                        float weight = exp(tempf * tempf * kernelMult);
                                                        sum += weight * dataVal;
                                                        weightsum += weight;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if (weightsum != 0.0f)
                                    {
                                        next->setValue(sum / weightsum, newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], s, c);
                                    } else {
                                        again = true;
                                        next->setValue(0.0f, newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], s, c);
                                    }
                                } else {
                                    next->setValue(curVal, newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], s, c);
                                }
                            }
                        }
                    }
                    tempvol = current;
                    current = next;
                    next = tempvol;
                    if (!again) break;
                }
                if (fixIter == FIX_ZEROS_POST_ITERATIONS)
                {
                    const GiftiLabelTable* curLabelTable = curLabel->getMapLabelTable(0);
                    CaretLogWarning("unable to fix all zeros in parcel " + curLabelTable->getLabelName(curLabelValue));
                }
                for (int c = 0; c < myDims[4]; ++c)//copy the final result into the output valume
                {
                    for (int s = 0; s < myDims[3]; ++s)
                    {
                        for (int64_t base = 0; base < newListSize; base += 3)
                        {
                            outVol->setValue(current->getValue(newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], s, c),
                                newList[base], newList[base + 1], newList[base + 2], s, c);
                        }
                    }
                }
            } else {
                VolumeFile inbox(boxdims, inVol->getSform(), myDims[4]), outbox;
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int64_t base = 0; base < listSize; base += 3)
                    {
                        if (c == 0)//smooth within BOTH labels, but only copy in from current label
                        {
                            roibox.setValue(1.0f, thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4]);
                        }
                        int curValue = (int)floor(curLabel->getValue(thisList[base], thisList[base + 1], thisList[base + 2]) + 0.5f);
                        if (curValue == curLabelValue)
                        {
                            inbox.setValue(inVol->getValue(thisList[base], thisList[base + 1], thisList[base + 2], subvolNum, c),
                                thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4], 0, c);
                        }
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &inbox, kernel, &outbox, &roibox, true);
                float kernelMult = -1.0f / kernel / kernel / 2.0f;//precompute the part of the kernel function that doesn't change
                int fixIter;
                VolumeFile* current = &outbox, *next = &inbox, *tempvol;//reuse inbox as scratch space for iterated dilation
                const float* labelFrame = newLabel->getFrame();
                for (fixIter = 0; fixIter < FIX_ZEROS_POST_ITERATIONS; ++fixIter)
                {
                    bool again = false;
                    for (int c = 0; c < myDims[4]; ++c)//since fix zeros smoothing doesn't consider components as multidimensional datatypes, neither should this
                    {
                        for (int64_t base = 0; base < newListSize; base += 3)
                        {
                            float curVal = current->getValue(newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], 0, c);
                            if (curVal == 0.0f)
                            {
                                float sum = 0.0f, weightsum = 0.0f;//coded in-place for now, its a special restricted case of volume dilate, copied out of nonorth volume smoothing
                                int i = newList[base], j = newList[base + 1], k = newList[base + 2];//special casing orthogonal would be faster, but harder to follow/debug, and more code
                                int imin = i - irange, imax = i + irange + 1;//one-after array size convention
                                if (imin < 0) imin = 0;
                                if (imax > myDims[0]) imax = myDims[0];
                                int jmin = j - jrange, jmax = j + jrange + 1;
                                if (jmin < 0) jmin = 0;
                                if (jmax > myDims[1]) jmax = myDims[1];
                                int kmin = k - krange, kmax = k + krange + 1;
                                if (kmin < 0) kmin = 0;
                                if (kmax > myDims[2]) kmax = myDims[2];
                                Vector3D kscratch, jscratch, iscratch;
                                for (int kkern = kmin; kkern < kmax; ++kkern)
                                {
                                    kscratch = kvec * (kkern - k);
                                    int64_t kindpart = kkern * myDims[1];
                                    for (int jkern = jmin; jkern < jmax; ++jkern)
                                    {
                                        jscratch = kscratch + jvec * (jkern - j);
                                        int64_t jindpart = (kindpart + jkern) * myDims[0];
                                        for (int ikern = imin; ikern < imax; ++ikern)
                                        {
                                            int64_t thisIndex = jindpart + ikern;//somewhat optimized index computation, could remove some integer multiplies, but there aren't that many
                                            int tempi = (int)floor(labelFrame[thisIndex] + 0.5f);
                                            if (tempi == curLabelValue)
                                            {
                                                float dataVal = current->getValue(ikern - extrema[0], jkern - extrema[2], kkern - extrema[4], 0, c);
                                                if (dataVal != 0.0f)
                                                {
                                                    iscratch = jscratch + ivec * (ikern - i);
                                                    float tempf = iscratch.length();
                                                    float weight = exp(tempf * tempf * kernelMult);
                                                    sum += weight * dataVal;
                                                    weightsum += weight;
                                                }
                                            }
                                        }
                                    }
                                }
                                if (weightsum != 0.0f)
                                {
                                    next->setValue(sum / weightsum, newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], 0, c);
                                } else {
                                    again = true;
                                    next->setValue(0.0f, newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], 0, c);
                                }
                            } else {
                                next->setValue(curVal, newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], 0, c);
                            }
                        }
                    }
                    tempvol = current;
                    current = next;
                    next = tempvol;
                    if (!again) break;
                }
                if (fixIter == FIX_ZEROS_POST_ITERATIONS)
                {
                    const GiftiLabelTable* curLabelTable = curLabel->getMapLabelTable(0);
                    CaretLogWarning("unable to fix all zeros in parcel " + curLabelTable->getLabelName(curLabelValue));
                }
                for (int c = 0; c < myDims[4]; ++c)//copy the final result into the output valume
                {
                    for (int64_t base = 0; base < newListSize; base += 3)
                    {
                        outVol->setValue(current->getValue(newList[base] - extrema[0], newList[base + 1] - extrema[2], newList[base + 2] - extrema[4], 0, c),
                            newList[base], newList[base + 1], newList[base + 2], 0, c);
                    }
                }
            }
        }
    }
}

float AlgorithmVolumeParcelResampling::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeParcelResampling::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
