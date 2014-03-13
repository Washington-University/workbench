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

#include "AlgorithmVolumeLabelToROI.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "VolumeFile.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeLabelToROI::getCommandSwitch()
{
    return "-volume-label-to-roi";
}

AString AlgorithmVolumeLabelToROI::getShortDescription()
{
    return "MAKE A VOLUME LABEL INTO AN ROI VOLUME";
}

OperationParameters* AlgorithmVolumeLabelToROI::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "label-in", "the input volume label file");
    
    ret->addVolumeOutputParameter(2, "volume-out", "the output volume file");
    
    OptionalParameter* nameOpt = ret->createOptionalParameter(3, "-name", "select label by name");
    nameOpt->addStringParameter(1, "label-name", "the label name that you want an roi of");
    
    OptionalParameter* keyOpt = ret->createOptionalParameter(4, "-key", "select label by key");
    keyOpt->addIntegerParameter(1, "label-key", "the label key that you want an roi of");
    
    OptionalParameter* mapOpt = ret->createOptionalParameter(5, "-map", "select a single label map to use");
    mapOpt->addStringParameter(1, "map", "the map number or name");
    
    ret->setHelpText(
        AString("For each map in <label-in>, a map is created in <volume-out> where the all locations labeled with <label-name> or with a key of <label-key> are given a value of 1, and all other locations are given 0.  ") +
        "Exactly one of -name and -key must be specified.  " +
        "Specify -map to use only one map from <label-in>."
    );
    return ret;
}

void AlgorithmVolumeLabelToROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myLabel = myParams->getVolume(1);
    VolumeFile* myVolumeOut = myParams->getOutputVolume(2);
    bool nameMode = false;
    AString labelName;
    OptionalParameter* nameOpt = myParams->getOptionalParameter(3);
    if (nameOpt->m_present)
    {
        nameMode = true;
        labelName = nameOpt->getString(1);
    }
    int32_t labelKey;
    OptionalParameter* keyOpt = myParams->getOptionalParameter(4);
    if (keyOpt->m_present)
    {
        if (nameMode) throw AlgorithmException("-name and -key cannot be specified together");
        labelKey = (int32_t)keyOpt->getInteger(1);
    } else {
        if (!nameMode) throw AlgorithmException("you must specify one of -name or -key");
    }
    int whichMap = -1;
    OptionalParameter* mapOpt = myParams->getOptionalParameter(5);
    if (mapOpt->m_present)
    {
        AString mapID = mapOpt->getString(1);
        whichMap = myLabel->getMapIndexFromNameOrNumber(mapID);
        if (whichMap == -1)
        {
            throw AlgorithmException("invalid map number or name specified");
        }
    }
    if (nameMode)
    {
        AlgorithmVolumeLabelToROI(myProgObj, myLabel, labelName, myVolumeOut, whichMap);
    } else {
        AlgorithmVolumeLabelToROI(myProgObj, myLabel, labelKey, myVolumeOut, whichMap);
    }
}

AlgorithmVolumeLabelToROI::AlgorithmVolumeLabelToROI(ProgressObject* myProgObj, const VolumeFile* myLabel, const AString& labelName, VolumeFile* myVolumeOut, const int& whichMap) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numMaps = myLabel->getNumberOfMaps();
    if (whichMap < -1 || whichMap >= numMaps)
    {
        throw AlgorithmException("invalid map index specified");
    }
    if (myLabel->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("input volume is not a label volume");
    }
    vector<int64_t> dims, origDims = myLabel->getOriginalDimensions();
    myLabel->getDimensions(dims);
    if (dims[4] != 1)
    {
        throw AlgorithmException("input label volume has multi-component type");
    }
    int64_t frameSize = dims[0] * dims[1] * dims[2];
    vector<float> scratchFrame(frameSize);
    if (whichMap == -1)
    {
        myVolumeOut->reinitialize(origDims, myLabel->getSform());
        bool shouldThrow = true;
        for (int thisMap = 0; thisMap < numMaps; ++thisMap)
        {
            const GiftiLabelTable* myTable = myLabel->getMapLabelTable(thisMap);
            int matchKey = myTable->getLabelKeyFromName(labelName);
            if (matchKey == GiftiLabel::getInvalidLabelKey())
            {
                CaretLogWarning("label name '" + labelName + "' not found in map #" + AString::number(thisMap + 1));
                for (int64_t i = 0; i < frameSize; ++i)
                {
                    scratchFrame[i] = 0.0f;
                }
            } else {
                const float* labelFrame = myLabel->getFrame(thisMap);
                for (int64_t i = 0; i < frameSize; ++i)
                {
                    int thisKey = (int)floor(labelFrame[i] + 0.5f);
                    if (thisKey == matchKey)
                    {
                        scratchFrame[i] = 1.0f;
                        shouldThrow = false;
                    } else {
                        scratchFrame[i] = 0.0f;
                    }
                }
            }
            myVolumeOut->setFrame(scratchFrame.data(), thisMap);
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label name");
        }
    } else {
        origDims.resize(3);
        myVolumeOut->reinitialize(origDims, myLabel->getSform());
        const GiftiLabelTable* myTable = myLabel->getMapLabelTable(whichMap);
        int matchKey = myTable->getLabelKeyFromName(labelName);
        if (matchKey == GiftiLabel::getInvalidLabelKey())
        {
            throw AlgorithmException("label name '" + labelName + "' not found in specified map");
        }
        const float* labelFrame = myLabel->getFrame(whichMap);
        bool shouldThrow = true;
        for (int64_t i = 0; i < frameSize; ++i)
        {
            int thisKey = (int)floor(labelFrame[i] + 0.5f);
            if (thisKey == matchKey)
            {
                scratchFrame[i] = 1.0f;
                shouldThrow = false;
            } else {
                scratchFrame[i] = 0.0f;
            }
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label name in the specified map");
        }
        myVolumeOut->setFrame(scratchFrame.data(), 0);
    }
}

AlgorithmVolumeLabelToROI::AlgorithmVolumeLabelToROI(ProgressObject* myProgObj, const VolumeFile* myLabel, const int32_t& labelKey, VolumeFile* myVolumeOut, const int& whichMap) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numMaps = myLabel->getNumberOfMaps();
    if (whichMap < -1 || whichMap >= numMaps)
    {
        throw AlgorithmException("invalid map index specified");
    }
    if (myLabel->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("input volume is not a label volume");
    }
    vector<int64_t> dims, origDims = myLabel->getOriginalDimensions();
    myLabel->getDimensions(dims);
    if (dims[4] != 1)
    {
        throw AlgorithmException("input label volume has multi-component type");
    }
    int64_t frameSize = dims[0] * dims[1] * dims[2];
    vector<float> scratchFrame(frameSize);
    if (whichMap == -1)
    {
        myVolumeOut->reinitialize(origDims, myLabel->getSform());
        bool shouldThrow = true;
        for (int thisMap = 0; thisMap < numMaps; ++thisMap)
        {
            const GiftiLabelTable* myTable = myLabel->getMapLabelTable(thisMap);
            if (myTable->getLabel(labelKey) == NULL)
            {
                CaretLogWarning("label key " + AString::number(labelKey) + " not found in map #" + AString::number(thisMap + 1));
            }
            const float* labelFrame = myLabel->getFrame(thisMap);//try anyway, in case label table is incomplete
            for (int64_t i = 0; i < frameSize; ++i)
            {
                int thisKey = (int)floor(labelFrame[i] + 0.5f);
                if (thisKey == labelKey)
                {
                    scratchFrame[i] = 1.0f;
                    shouldThrow = false;
                } else {
                    scratchFrame[i] = 0.0f;
                }
            }
            myVolumeOut->setFrame(scratchFrame.data(), thisMap);
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label key");
        }
    } else {
        origDims.resize(3);
        myVolumeOut->reinitialize(origDims, myLabel->getSform());
        const GiftiLabelTable* myTable = myLabel->getMapLabelTable(whichMap);
        if (myTable->getLabel(labelKey) == NULL)
        {
            CaretLogWarning("label key " + AString::number(labelKey) + " not found in specified map");
        }
        const float* labelFrame = myLabel->getFrame(whichMap);
        bool shouldThrow = true;
        for (int64_t i = 0; i < frameSize; ++i)
        {
            int thisKey = (int)floor(labelFrame[i] + 0.5f);
            if (thisKey == labelKey)
            {
                scratchFrame[i] = 1.0f;
                shouldThrow = false;
            } else {
                scratchFrame[i] = 0.0f;
            }
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label key in the specified map");
        }
        myVolumeOut->setFrame(scratchFrame.data(), 0);
    }
}

float AlgorithmVolumeLabelToROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeLabelToROI::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
