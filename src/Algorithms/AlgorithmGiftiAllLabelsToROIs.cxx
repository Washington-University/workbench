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

#include "AlgorithmGiftiAllLabelsToROIs.h"
#include "AlgorithmException.h"

#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"

#include <map>

using namespace caret;
using namespace std;

AString AlgorithmGiftiAllLabelsToROIs::getCommandSwitch()
{
    return "-gifti-all-labels-to-rois";
}

AString AlgorithmGiftiAllLabelsToROIs::getShortDescription()
{
    return "MAKE ROIS FROM ALL LABELS IN A GIFTI COLUMN";
}

OperationParameters* AlgorithmGiftiAllLabelsToROIs::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelParameter(1, "label-in", "the input gifti label file");
    
    ret->addStringParameter(2, "map", "the number or name of the label map to use");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric file");
        
    ret->setHelpText(
        AString("The output metric file has a column for each label in the specified input map, other than the ??? label, ") +
        "each of which contains an ROI of all vertices that are set to the corresponding label."
    );
    return ret;
}

void AlgorithmGiftiAllLabelsToROIs::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* myLabel = myParams->getLabel(1);
    AString mapID = myParams->getString(2);
    int whichMap = myLabel->getMapIndexFromNameOrNumber(mapID);
    if (whichMap == -1)
    {
        throw AlgorithmException("invalid map number or name specified");
    }
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    AlgorithmGiftiAllLabelsToROIs(myProgObj, myLabel, whichMap, myMetricOut);
}

AlgorithmGiftiAllLabelsToROIs::AlgorithmGiftiAllLabelsToROIs(ProgressObject* myProgObj, const LabelFile* myLabel, const int& whichMap, MetricFile* myMetricOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (whichMap < 0 || whichMap >= myLabel->getNumberOfMaps())
    {
        throw AlgorithmException("invalid map index specified");
    }
    const GiftiLabelTable* myTable = myLabel->getMapLabelTable(whichMap);
    int32_t unusedKey = myTable->getUnassignedLabelKey();//WARNING: this actually MODIFIES the label table if the ??? key doesn't exist
    set<int32_t> myKeys = myTable->getKeys();
    int numKeys = (int)myKeys.size();
    if (numKeys < 2)
    {
        throw AlgorithmException("label table doesn't contain any keys besides the ??? key");
    }
    int numNodes = myLabel->getNumberOfNodes();
    map<int32_t, int> keyToMap;//lookup from keys to column
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numKeys - 1);//skip the ??? label
    myMetricOut->setStructure(myLabel->getStructure());
    for (int i = 0; i < numKeys - 1; ++i)
    {
        myMetricOut->initializeColumn(i, 0.0f);
    }
    int counter = 0;
    for (set<int32_t>::iterator iter = myKeys.begin(); iter != myKeys.end(); ++iter)
    {
        if (*iter == unusedKey) continue;//skip the ??? key
        keyToMap[*iter] = counter;
        myMetricOut->setMapName(counter, myTable->getLabelName(*iter));
        ++counter;
    }
    const int32_t* myColumn = myLabel->getLabelKeyPointerForColumn(whichMap);
    for (int i = 0; i < numNodes; ++i)
    {
        map<int32_t, int>::iterator iter = keyToMap.find(myColumn[i]);
        if (iter != keyToMap.end())
        {
            myMetricOut->setValue(i, iter->second, 1.0f);
        }
    }
}

float AlgorithmGiftiAllLabelsToROIs::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmGiftiAllLabelsToROIs::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
