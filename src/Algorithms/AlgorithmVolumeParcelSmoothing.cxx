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

#include "AlgorithmVolumeParcelSmoothing.h"
#include "AlgorithmException.h"
#include "VolumeFile.h"
#include "AlgorithmVolumeSmoothing.h"
#include <map>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmVolumeParcelSmoothing::getCommandSwitch()
{
    return "-volume-parcel-smoothing";
}

AString AlgorithmVolumeParcelSmoothing::getShortDescription()
{
    return "SMOOTH PARCELS IN A VOLUME SEPARATELY";
}

OperationParameters* AlgorithmVolumeParcelSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "data-volume", "the volume to smooth");
    
    ret->addVolumeParameter(2, "label-volume", "a label volume containing the parcels to smooth");
    
    ret->addDoubleParameter(3, "kernel", "the gaussian smoothing kernel sigma, in mm");
    
    ret->addVolumeOutputParameter(4, "volume-out", "the output volume");
    
    ret->createOptionalParameter(5, "-fix-zeros", "treat zero values as not being data");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to smooth");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("The volume is smoothed within each label in the label volume using data only from within the label.  Equivalent to ") +
        "running volume smoothing with ROIs matching each label separately, then adding the resulting volumes, but faster."
    );
    return ret;
}

void AlgorithmVolumeParcelSmoothing::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVol = myParams->getVolume(1);
    VolumeFile* myLabelVol = myParams->getVolume(2);
    float myKernel = (float)myParams->getDouble(3);
    VolumeFile* myOutVol = myParams->getOutputVolume(4);
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
    AlgorithmVolumeParcelSmoothing(myProgObj, myVol, myLabelVol, myKernel, myOutVol, fixZeros, subvolNum);
}

AlgorithmVolumeParcelSmoothing::AlgorithmVolumeParcelSmoothing(ProgressObject* myProgObj, const VolumeFile* myVol, const VolumeFile* myLabelVol, const float& myKernel, VolumeFile* myOutVol, const bool& fixZeros, const int& subvolNum) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(myVol != NULL);
    CaretAssert(myOutVol != NULL);
    CaretAssert(myLabelVol != NULL);
    if (myLabelVol->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("label volume doesn't contain label data");
    }
    if (!myLabelVol->matchesVolumeSpace(myVol))
    {
        throw AlgorithmException("input volume and label volume have different spacing");
    }
    const GiftiLabelTable* myLabels = myLabelVol->getMapLabelTable(0);
    vector<int32_t> myKeys;
    myLabels->getKeys(myKeys);
    int numLabels = (int)myKeys.size();
    map<int32_t, int> labelLookup;//reverse lookup, used to compact labels into the list of voxel lists
    for (int i = 0; i < numLabels; ++i)
    {
        labelLookup[myKeys[i]] = i;
    }
    int32_t unusedLabel = myLabels->getUnassignedLabelKey();
    /*ProgressObject* subAlgProgress1 = NULL;//uncomment these if you use another algorithm inside here
    if (myProgObj != NULL)
    {
        subAlgProgress1 = myProgObj->addAlgorithm(AlgorithmInsertNameHere::getAlgorithmWeight());//TODO: set a vector of objects up via number of labels
    }//*/
    LevelProgress myProgress(myProgObj);//this line sets the algorithm up to use the progress object, and will finish the progress object automatically when the algorithm terminates
    vector<int64_t> myDims;
    myVol->getDimensions(myDims);
    if (subvolNum < -1 || subvolNum >= myDims[3])
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    vector<vector<int> > voxelLists;//build all lists in a single pass, allows some short circuits and less conversion to label integers
    voxelLists.resize(numLabels);
    for (int k = 0; k < myDims[2]; ++k)
    {
        for (int j = 0; j < myDims[1]; ++j)
        {
            for (int i = 0; i < myDims[0]; ++i)
            {
                int myValue = (int)floor(myLabelVol->getValue(i, j, k) + 0.5f);
                if (myValue == unusedLabel) continue;//skip the ??? label, whether it is included in the keys or not
                map<int32_t, int>::iterator search = labelLookup.find(myValue);
                if (search != labelLookup.end())
                {
                    int keyIndex = search->second;
                    voxelLists[keyIndex].push_back(i);
                    voxelLists[keyIndex].push_back(j);
                    voxelLists[keyIndex].push_back(k);
                }
            }
        }
    }
    if (subvolNum == -1)
    {
        myOutVol->reinitialize(myVol->getOriginalDimensions(), myVol->getSform(), myDims[4], myVol->getType(), myVol->m_header);
        myOutVol->setValueAllVoxels(0.0f);
        for (int whichList = 0; whichList < numLabels; ++whichList)
        {
            const vector<int>& thisList = voxelLists[whichList];
            int64_t listSize = (int64_t)thisList.size();
            if (listSize > 2)//NOTE: this should NEVER be something other than a multiple of 3, but check against what we will actually access anyway
            {
                int extrema[6] = { thisList[0],
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
                VolumeFile roibox(boxdims, myVol->getSform());
                boxdims.push_back(myDims[3]);
                VolumeFile inbox(boxdims, myVol->getSform(), myDims[4]), outbox;
                roibox.setValueAllVoxels(0.0f);
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int s = 0; s < myDims[3]; ++s)
                    {
                        for (int64_t base = 0; base < listSize; base += 3)
                        {
                            if (s == 0 && c == 0) roibox.setValue(1.0f, thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4]);
                            inbox.setValue(myVol->getValue(thisList[base], thisList[base + 1], thisList[base + 2], s, c),
                                thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4], s, c);
                        }
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &inbox, myKernel, &outbox, &roibox, fixZeros);
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int s = 0; s < myDims[3]; ++s)
                    {
                        for (int64_t base = 0; base < listSize; base += 3)
                        {
                            myOutVol->setValue(outbox.getValue(thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4], s, c),
                                            thisList[base], thisList[base + 1], thisList[base + 2], s, c);
                        }
                    }
                }
            }
        }
        for (int s = 0; s < myDims[3]; ++s)
        {
            myOutVol->setMapName(s, myVol->getMapName(s) + ", parcel smoothed " + AString::number(myKernel));
        }
    } else {
        vector<int64_t> newDims = myVol->getOriginalDimensions();
        newDims.resize(3);//discard non-spatial extra dimensions
        myOutVol->reinitialize(newDims, myVol->getSform(), myDims[4], myVol->getType(), myVol->m_header);//keep components
        myOutVol->setValueAllVoxels(0.0f);
        for (int whichList = 0; whichList < numLabels; ++whichList)
        {
            const vector<int>& thisList = voxelLists[whichList];
            int64_t listSize = (int64_t)thisList.size();
            if (listSize > 2)//NOTE: this should NEVER be something other than a multiple of 3, but check against what we will actually access anyway
            {
                int extrema[6] = { thisList[0],
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
                VolumeFile inbox(boxdims, myVol->getSform(), myDims[4]), roibox(boxdims, myVol->getSform()), outbox;
                roibox.setValueAllVoxels(0.0f);
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int64_t base = 0; base < listSize; base += 3)
                    {
                        if (c == 0) roibox.setValue(1.0f, thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4]);
                        inbox.setValue(myVol->getValue(thisList[base], thisList[base + 1], thisList[base + 2], subvolNum, c),
                            thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4], 0, c);
                    }
                }
                AlgorithmVolumeSmoothing(NULL, &inbox, myKernel, &outbox, &roibox, fixZeros);
                for (int c = 0; c < myDims[4]; ++c)
                {
                    for (int64_t base = 0; base < listSize; base += 3)
                    {
                        myOutVol->setValue(outbox.getValue(thisList[base] - extrema[0], thisList[base + 1] - extrema[2], thisList[base + 2] - extrema[4], 0, c),
                                        thisList[base], thisList[base + 1], thisList[base + 2], 0, c);
                    }
                }
            }
        }
        myOutVol->setMapName(0, myVol->getMapName(subvolNum) + ", parcel smoothed " + AString::number(myKernel));
    }
}

float AlgorithmVolumeParcelSmoothing::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeParcelSmoothing::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
