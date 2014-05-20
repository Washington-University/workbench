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

#include "AlgorithmCiftiLabelToROI.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "CiftiFile.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiLabelToROI::getCommandSwitch()
{
    return "-cifti-label-to-roi";
}

AString AlgorithmCiftiLabelToROI::getShortDescription()
{
    return "MAKE A CIFTI LABEL INTO AN ROI";
}

OperationParameters* AlgorithmCiftiLabelToROI::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "label-in", "the input cifti label file");
    
    ret->addCiftiOutputParameter(2, "scalar-out", "the output cifti scalar file");
    
    OptionalParameter* nameOpt = ret->createOptionalParameter(3, "-name", "select label by name");
    nameOpt->addStringParameter(1, "label-name", "the label name that you want an roi of");
    
    OptionalParameter* keyOpt = ret->createOptionalParameter(4, "-key", "select label by key");
    keyOpt->addIntegerParameter(1, "label-key", "the label key that you want an roi of");
    
    OptionalParameter* mapOpt = ret->createOptionalParameter(5, "-map", "select a single label map to use");
    mapOpt->addStringParameter(1, "map", "the map number or name");
    
    ret->setHelpText(
        AString("For each map in <label-in>, a map is created in <scalar-out> where all locations labeled with <label-name> or with a key of <label-key> are given a value of 1, and all other locations are given 0.  ") +
        "Exactly one of -name and -key must be specified.  " +
        "Specify -map to use only one map from <label-in>."
    );
    return ret;
}

void AlgorithmCiftiLabelToROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
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
    int64_t whichMap = -1;
    OptionalParameter* mapOpt = myParams->getOptionalParameter(5);
    if (mapOpt->m_present)
    {
        AString mapID = mapOpt->getString(1);
        whichMap = myCifti->getCiftiXMLOld().getMapIndexFromNameOrNumber(CiftiXMLOld::ALONG_ROW, mapID);
        if (whichMap == -1)
        {
            throw AlgorithmException("invalid map number or name specified");
        }
    }
    if (nameMode)
    {
        AlgorithmCiftiLabelToROI(myProgObj, myCifti, labelName, myCiftiOut, whichMap);
    } else {
        AlgorithmCiftiLabelToROI(myProgObj, myCifti, labelKey, myCiftiOut, whichMap);
    }
}

AlgorithmCiftiLabelToROI::AlgorithmCiftiLabelToROI(ProgressObject* myProgObj, const CiftiFile* myCifti, const AString& labelName, CiftiFile* myCiftiOut, const int64_t& whichMap) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXMLOld& myXml = myCifti->getCiftiXMLOld();
    CiftiXMLOld outXml = myXml;
    int64_t numRows = myXml.getNumberOfRows();
    int64_t numMaps = myXml.getDimensionLength(CiftiXMLOld::ALONG_ROW);
    if (whichMap < -1 || whichMap >= numMaps)
    {
        throw AlgorithmException("invalid map index specified");
    }
    if (myXml.getMappingType(CiftiXMLOld::ALONG_ROW) != CIFTI_INDEX_TYPE_LABELS)
    {
        throw AlgorithmException("input cifti must have labels along rows");
    }
    if (whichMap == -1)
    {
        outXml.resetDirectionToScalars(CiftiXMLOld::ALONG_ROW, numMaps);
        vector<int> matchKey(numMaps, -1);
        vector<bool> haveKey(numMaps, false);//-1 is actually a valid key, track with a second variable
        bool shouldThrow = true;
        for (int64_t i = 0; i < numMaps; ++i)
        {
            outXml.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, i, myXml.getMapName(CiftiXMLOld::ALONG_ROW, i));
            const GiftiLabelTable* myTable = myXml.getLabelTableForRowIndex(i);
            int thisKey = myTable->getLabelKeyFromName(labelName);
            if (thisKey != GiftiLabel::getInvalidLabelKey())
            {
                matchKey[i] = thisKey;
                haveKey[i] = true;
                shouldThrow = false;
            } else {
                CaretLogWarning("label name '" + labelName + "' not found in map #" + AString::number(i + 1));
            }
        }
        if (shouldThrow)
        {
            throw AlgorithmException("label name was not found in any map");
        }
        shouldThrow = true;
        myCiftiOut->setCiftiXML(outXml);
        vector<float> rowScratch(numMaps);
        for (int64_t row = 0; row < numRows; ++row)
        {
            myCifti->getRow(rowScratch.data(), row);
            for (int64_t i = 0; i < numMaps; ++i)
            {
                if (haveKey[i])
                {
                    int input = (int)floor(rowScratch[i] + 0.5f);
                    if (input == matchKey[i])
                    {
                        rowScratch[i] = 1.0f;
                        shouldThrow = false;
                    } else {
                        rowScratch[i] = 0.0f;
                    }
                } else {
                    rowScratch[i] = 0.0f;
                }
            }
            myCiftiOut->setRow(rowScratch.data(), row);
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label name");
        }
    } else {
        float outScratch;
        outXml.resetDirectionToScalars(CiftiXMLOld::ALONG_ROW, 1);
        outXml.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 0, myXml.getMapName(CiftiXMLOld::ALONG_ROW, whichMap));
        myCiftiOut->setCiftiXML(outXml);
        const GiftiLabelTable* myTable = myXml.getLabelTableForRowIndex(whichMap);
        int matchKey = myTable->getLabelKeyFromName(labelName);
        if (matchKey == GiftiLabel::getInvalidLabelKey())
        {
            throw AlgorithmException("label name '" + labelName + "' not found in specified map");
        }
        bool shouldThrow = true;
        vector<float> rowScratch(numMaps);
        for (int64_t row = 0; row < numRows; ++row)
        {
            int input = (int)floor(rowScratch[whichMap] + 0.5f);
            if (input == matchKey)
            {
                outScratch = 1.0f;
                shouldThrow = false;
            } else {
                outScratch = 0.0f;
            }
            myCiftiOut->setRow(&outScratch, row);
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label name in the specified map");
        }
    }
}

AlgorithmCiftiLabelToROI::AlgorithmCiftiLabelToROI(ProgressObject* myProgObj, const CiftiFile* myCifti, const int32_t& labelKey, CiftiFile* myCiftiOut, const int64_t& whichMap) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXMLOld& myXml = myCifti->getCiftiXMLOld();
    CiftiXMLOld outXml = myXml;
    int64_t numRows = myXml.getNumberOfRows();
    int64_t numMaps = myXml.getDimensionLength(CiftiXMLOld::ALONG_ROW);
    if (whichMap < -1 || whichMap >= numMaps)
    {
        throw AlgorithmException("invalid map index specified");
    }
    if (myXml.getMappingType(CiftiXMLOld::ALONG_ROW) != CIFTI_INDEX_TYPE_LABELS)
    {
        throw AlgorithmException("input cifti must have labels along rows");
    }
    if (whichMap == -1)
    {
        outXml.resetDirectionToScalars(CiftiXMLOld::ALONG_ROW, numMaps);
        for (int64_t i = 0; i < numMaps; ++i)
        {
            outXml.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, i, myXml.getMapName(CiftiXMLOld::ALONG_ROW, i));
            const GiftiLabelTable* myTable = myXml.getLabelTableForRowIndex(i);
            if (myTable->getLabel(labelKey) == NULL)
            {
                CaretLogWarning("label key " + AString::number(labelKey) + " not found in map #" + AString::number(i + 1));
            }
        }
        bool shouldThrow = true;
        myCiftiOut->setCiftiXML(outXml);
        vector<float> rowScratch(numMaps);
        for (int64_t row = 0; row < numRows; ++row)
        {
            myCifti->getRow(rowScratch.data(), row);
            for (int64_t i = 0; i < numMaps; ++i)
            {
                int input = (int)floor(rowScratch[i] + 0.5f);
                if (input == labelKey)
                {
                    rowScratch[i] = 1.0f;
                    shouldThrow = false;
                } else {
                    rowScratch[i] = 0.0f;
                }
            }
            myCiftiOut->setRow(rowScratch.data(), row);
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label key");
        }
    } else {
        float outScratch;
        outXml.resetDirectionToScalars(CiftiXMLOld::ALONG_ROW, 1);
        outXml.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 0, myXml.getMapName(CiftiXMLOld::ALONG_ROW, whichMap));
        myCiftiOut->setCiftiXML(outXml);
        const GiftiLabelTable* myTable = myXml.getLabelTableForRowIndex(whichMap);
        if (myTable->getLabel(labelKey) == NULL)
        {
            CaretLogWarning("label key " + AString::number(labelKey) + " not found in specified map");
        }
        bool shouldThrow = true;
        vector<float> rowScratch(numMaps);
        for (int64_t row = 0; row < numRows; ++row)//try anyway, in case label table is incomplete
        {
            int input = (int)floor(rowScratch[whichMap] + 0.5f);
            if (input == labelKey)
            {
                outScratch = 1.0f;
                shouldThrow = false;
            } else {
                outScratch = 0.0f;
            }
            myCiftiOut->setRow(&outScratch, row);
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label key in the specified map");
        }
    }
}

float AlgorithmCiftiLabelToROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiLabelToROI::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
