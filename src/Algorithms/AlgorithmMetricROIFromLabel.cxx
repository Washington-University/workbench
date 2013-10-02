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

#include "AlgorithmMetricROIFromLabel.h"
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

AString AlgorithmMetricROIFromLabel::getCommandSwitch()
{
    return "-metric-roi-from-label";
}

AString AlgorithmMetricROIFromLabel::getShortDescription()
{
    return "MAKE AN ROI METRIC FROM A GIFTI LABEL FILE";
}

OperationParameters* AlgorithmMetricROIFromLabel::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addLabelParameter(1, "label-in", "the input gifti label file");
    
    ret->addStringParameter(2, "label-name", "the label name that you want an roi of");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric file");
    
    OptionalParameter* mapOpt = ret->createOptionalParameter(4, "-map", "select a single label map to use");
    mapOpt->addStringParameter(1, "map", "the map number or name");
    
    ret->setHelpText(
        AString("For each map in <label-in>, a map is created in <metric-out> where the all locations labeled with <label-name> are given a value of 1, and all other locations are given 0.  ") +
        "Specify -map to use only one map from <label-in>."
    );
    return ret;
}

void AlgorithmMetricROIFromLabel::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* myLabel = myParams->getLabel(1);
    AString labelName = myParams->getString(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    int whichMap = -1;
    OptionalParameter* mapOpt = myParams->getOptionalParameter(4);
    if (mapOpt->m_present)
    {
        AString mapID = mapOpt->getString(1);
        whichMap = myLabel->getMapIndexFromNameOrNumber(mapID);
        if (whichMap == -1)
        {
            throw AlgorithmException("invalid map number or name specified");
        }
    }
    AlgorithmMetricROIFromLabel(myProgObj, myLabel, labelName, myMetricOut, whichMap);
}

AlgorithmMetricROIFromLabel::AlgorithmMetricROIFromLabel(ProgressObject* myProgObj, const LabelFile* myLabel, const AString& labelName, MetricFile* myMetricOut, const int& whichMap) : AbstractAlgorithm(myProgObj)
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
        for (int thisMap = 0; thisMap < numMaps; ++thisMap)
        {
            myMetricOut->setColumnName(thisMap, myLabel->getColumnName(thisMap));
            const int32_t* labelColumn = myLabel->getLabelKeyPointerForColumn(thisMap);
            for (int i = 0; i < numNodes; ++i)
            {
                if (labelColumn[i] == matchKey)
                {
                    scratchCol[i] = 1.0f;
                } else {
                    scratchCol[i] = 0.0f;
                }
            }
            myMetricOut->setValuesForColumn(thisMap, scratchCol.data());
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(myLabel->getStructure());
        myMetricOut->setColumnName(0, myLabel->getColumnName(whichMap));
        const int32_t* labelColumn = myLabel->getLabelKeyPointerForColumn(whichMap);
        for (int i = 0; i < numNodes; ++i)
        {
            if (labelColumn[i] == matchKey)
            {
                scratchCol[i] = 1.0f;
            } else {
                scratchCol[i] = 0.0f;
            }
        }
        myMetricOut->setValuesForColumn(0, scratchCol.data());
    }
}

float AlgorithmMetricROIFromLabel::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricROIFromLabel::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
