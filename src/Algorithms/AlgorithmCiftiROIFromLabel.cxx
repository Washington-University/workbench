/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AlgorithmCiftiROIFromLabel.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "CiftiFile.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiROIFromLabel::getCommandSwitch()
{
    return "-cifti-roi-from-label";
}

AString AlgorithmCiftiROIFromLabel::getShortDescription()
{
    return "MAKE A DSCALAR ROI FROM A DLABEL FILE";
}

OperationParameters* AlgorithmCiftiROIFromLabel::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "label-in", "input cifti label file");
    
    ret->addStringParameter(2, "label-name", "the label name that you want an roi of");
    
    ret->addCiftiOutputParameter(3, "scalar-out", "output cifti scalar file");
    
    OptionalParameter* mapOpt = ret->createOptionalParameter(4, "-map", "select a single label map to use");
    mapOpt->addStringParameter(1, "map", "the map number or name");
    
    ret->setHelpText(
        AString("For each map in <label-in>, a map is created in <scalar-out> where the all locations labeled with <label-name> are given a value of 1, and all other locations are given 0.  ") +
        "Specify -map to use only one map from <label-in>."
    );
    return ret;
}

void AlgorithmCiftiROIFromLabel::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    AString labelName = myParams->getString(2);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(3);
    int64_t whichMap = -1;
    OptionalParameter* mapOpt = myParams->getOptionalParameter(4);
    if (mapOpt->m_present)
    {
        AString mapID = mapOpt->getString(1);
        whichMap = myCifti->getCiftiXML().getMapIndexFromNameOrNumber(CiftiXML::ALONG_ROW, mapID);
        if (whichMap == -1)
        {
            throw AlgorithmException("invalid map number or name specified");
        }
    }
    AlgorithmCiftiROIFromLabel(myProgObj, myCifti, labelName, myCiftiOut, whichMap);
}

AlgorithmCiftiROIFromLabel::AlgorithmCiftiROIFromLabel(ProgressObject* myProgObj, const CiftiInterface* myCifti, const AString& labelName, CiftiFile* myCiftiOut, const int64_t& whichMap) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXml = myCifti->getCiftiXML();
    CiftiXML outXml = myXml;
    int64_t numRows = myXml.getNumberOfRows();
    int64_t numMaps = myXml.getDimensionLength(CiftiXML::ALONG_ROW);
    if (whichMap < -1 || whichMap >= numMaps)
    {
        throw AlgorithmException("invalid map index specified");
    }
    if (myXml.getMappingType(CiftiXML::ALONG_ROW) != CIFTI_INDEX_TYPE_LABELS)
    {
        throw AlgorithmException("input cifti must have labels along rows");
    }
    vector<float> rowScratch(numMaps);
    if (whichMap == -1)
    {
        outXml.resetDirectionToScalars(CiftiXML::ALONG_ROW, numMaps);
        vector<int> matchKey(numMaps, -1);
        vector<bool> haveKey(numMaps, false);//-1 is actually a valid key, track with a second variable
        for (int64_t i = 0; i < numMaps; ++i)
        {
            outXml.setMapNameForIndex(CiftiXML::ALONG_ROW, i, myXml.getMapName(CiftiXML::ALONG_ROW, i));
            const GiftiLabelTable* myTable = myXml.getLabelTableForRowIndex(i);
            int thisKey = myTable->getLabelKeyFromName(labelName);
            if (thisKey != GiftiLabel::getInvalidLabelKey())
            {
                matchKey[i] = thisKey;
                haveKey[i] = true;
            } else {
                CaretLogWarning("label name '" + labelName + "' not found in map #" + AString::number(i + 1));
            }
        }
        myCiftiOut->setCiftiXML(outXml);
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
                    } else {
                        rowScratch[i] = 0.0f;
                    }
                } else {
                    rowScratch[i] = 0.0f;
                }
            }
            myCiftiOut->setRow(rowScratch.data(), row);
        }
    } else {
        float outScratch;
        outXml.resetDirectionToScalars(CiftiXML::ALONG_ROW, 1);
        outXml.setMapNameForIndex(CiftiXML::ALONG_ROW, 0, myXml.getMapName(CiftiXML::ALONG_ROW, whichMap));
        myCiftiOut->setCiftiXML(outXml);
        const GiftiLabelTable* myTable = myXml.getLabelTableForRowIndex(whichMap);
        int matchKey = myTable->getLabelKeyFromName(labelName);
        if (matchKey == GiftiLabel::getInvalidLabelKey())
        {
            CaretLogWarning("label name '" + labelName + "' not found in specified map");
            outScratch = 0.0f;
            for (int64_t row = 0; row < numRows; ++row)
            {
                myCiftiOut->setRow(&outScratch, row);
            }
        } else {
            for (int64_t row = 0; row < numRows; ++row)
            {
                int input = (int)floor(rowScratch[whichMap] + 0.5f);
                if (input == matchKey)
                {
                    outScratch = 1.0f;
                } else {
                    outScratch = 0.0f;
                }
                myCiftiOut->setRow(&outScratch, row);
            }
        }
    }
}

float AlgorithmCiftiROIFromLabel::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiROIFromLabel::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
