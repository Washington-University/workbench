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

#include "AlgorithmGiftiLabelToROI.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmGiftiLabelToROI::getCommandSwitch()
{
    return "-gifti-label-to-roi";
}

AString AlgorithmGiftiLabelToROI::getShortDescription()
{
    return "MAKE A GIFTI LABEL INTO AN ROI METRIC";
}

OperationParameters* AlgorithmGiftiLabelToROI::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addLabelParameter(1, "label-in", "the input gifti label file");
    
    ret->addMetricOutputParameter(2, "metric-out", "the output metric file");
    
    OptionalParameter* nameOpt = ret->createOptionalParameter(3, "-name", "select label by name");
    nameOpt->addStringParameter(1, "label-name", "the label name that you want an roi of");
    
    OptionalParameter* keyOpt = ret->createOptionalParameter(4, "-key", "select label by key");
    keyOpt->addIntegerParameter(1, "label-key", "the label key that you want an roi of");
    
    OptionalParameter* mapOpt = ret->createOptionalParameter(5, "-map", "select a single label map to use");
    mapOpt->addStringParameter(1, "map", "the map number or name");
    
    ret->setHelpText(
        AString("For each map in <label-in>, a map is created in <metric-out> where all locations labeled with <label-name> or with a key of <label-key> are given a value of 1, and all other locations are given 0.  ") +
        "Exactly one of -name and -key must be specified.  " +
        "Specify -map to use only one map from <label-in>."
    );
    return ret;
}

void AlgorithmGiftiLabelToROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* myLabel = myParams->getLabel(1);
    MetricFile* myMetricOut = myParams->getOutputMetric(2);
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
        AlgorithmGiftiLabelToROI(myProgObj, myLabel, labelName, myMetricOut, whichMap);
    } else {
        AlgorithmGiftiLabelToROI(myProgObj, myLabel, labelKey, myMetricOut, whichMap);
    }
}

AlgorithmGiftiLabelToROI::AlgorithmGiftiLabelToROI(ProgressObject* myProgObj, const LabelFile* myLabel, const AString& labelName, MetricFile* myMetricOut, const int& whichMap) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int64_t numNodes = myLabel->getNumberOfNodes();
    int64_t numMaps = myLabel->getNumberOfMaps();
    if (whichMap < -1 || whichMap >= numMaps)
    {
        throw AlgorithmException("invalid map index specified");
    }
    const GiftiLabelTable* myTable = myLabel->getLabelTable();
    int32_t matchKey = myTable->getLabelKeyFromName(labelName);
    if (matchKey == GiftiLabel::getInvalidLabelKey())
    {
        throw AlgorithmException("label name '" + labelName + "' not found in label file");
    }
    vector<float> scratchCol(numNodes);
    if (whichMap == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numMaps);
        myMetricOut->setStructure(myLabel->getStructure());
        bool shouldThrow = true;
        for (int thisMap = 0; thisMap < numMaps; ++thisMap)
        {
            myMetricOut->setColumnName(thisMap, myLabel->getColumnName(thisMap));
            const int32_t* labelColumn = myLabel->getLabelKeyPointerForColumn(thisMap);
            for (int i = 0; i < numNodes; ++i)
            {
                if (labelColumn[i] == matchKey)
                {
                    scratchCol[i] = 1.0f;
                    shouldThrow = false;
                } else {
                    scratchCol[i] = 0.0f;
                }
            }
            myMetricOut->setValuesForColumn(thisMap, scratchCol.data());
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label name");
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(myLabel->getStructure());
        myMetricOut->setColumnName(0, myLabel->getColumnName(whichMap));
        const int32_t* labelColumn = myLabel->getLabelKeyPointerForColumn(whichMap);
        bool shouldThrow = true;
        for (int i = 0; i < numNodes; ++i)
        {
            if (labelColumn[i] == matchKey)
            {
                scratchCol[i] = 1.0f;
                shouldThrow = false;
            } else {
                scratchCol[i] = 0.0f;
            }
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label name in the specified map");
        }
        myMetricOut->setValuesForColumn(0, scratchCol.data());
    }
}

AlgorithmGiftiLabelToROI::AlgorithmGiftiLabelToROI(ProgressObject* myProgObj, const LabelFile* myLabel, const int32_t& labelKey, MetricFile* myMetricOut, const int& whichMap) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int64_t numNodes = myLabel->getNumberOfNodes();
    int64_t numMaps = myLabel->getNumberOfMaps();
    if (whichMap < -1 || whichMap >= numMaps)
    {
        throw AlgorithmException("invalid map index specified");
    }
    const GiftiLabelTable* myTable = myLabel->getLabelTable();
    if (myTable->getLabel(labelKey) == NULL)
    {
        CaretLogWarning("label key " + AString::number(labelKey) + " not found in label file");
    }
    vector<float> scratchCol(numNodes);
    if (whichMap == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numMaps);
        myMetricOut->setStructure(myLabel->getStructure());
        bool shouldThrow = true;
        for (int thisMap = 0; thisMap < numMaps; ++thisMap)
        {
            myMetricOut->setColumnName(thisMap, myLabel->getColumnName(thisMap));
            const int32_t* labelColumn = myLabel->getLabelKeyPointerForColumn(thisMap);
            for (int i = 0; i < numNodes; ++i)
            {
                if (labelColumn[i] == labelKey)
                {
                    scratchCol[i] = 1.0f;
                    shouldThrow = false;
                } else {
                    scratchCol[i] = 0.0f;
                }
            }
            myMetricOut->setValuesForColumn(thisMap, scratchCol.data());
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label key");
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(myLabel->getStructure());
        myMetricOut->setColumnName(0, myLabel->getColumnName(whichMap));
        const int32_t* labelColumn = myLabel->getLabelKeyPointerForColumn(whichMap);
        bool shouldThrow = true;
        for (int i = 0; i < numNodes; ++i)
        {
            if (labelColumn[i] == labelKey)
            {
                scratchCol[i] = 1.0f;
            } else {
                scratchCol[i] = 0.0f;
            }
        }
        if (shouldThrow)
        {
            throw AlgorithmException("no data matched the specified label key in the specified map");
        }
        myMetricOut->setValuesForColumn(0, scratchCol.data());
    }
}

float AlgorithmGiftiLabelToROI::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmGiftiLabelToROI::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
