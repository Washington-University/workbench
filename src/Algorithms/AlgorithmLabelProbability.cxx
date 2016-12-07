/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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

#include "AlgorithmLabelProbability.h"
#include "AlgorithmException.h"

#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"

#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmLabelProbability::getCommandSwitch()
{
    return "-label-probability";
}

AString AlgorithmLabelProbability::getShortDescription()
{
    return "FIND FREQUENCY OF SURFACE LABELS";
}

OperationParameters* AlgorithmLabelProbability::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addLabelParameter(1, "label-maps", "label file containing individual label maps from many subjects");
    
    ret->addMetricOutputParameter(2, "probability-metric-out", "the relative frequencies of each label at each vertex");
    
    ret->createOptionalParameter(3, "-exclude-unlabeled", "don't make a probability map of the unlabeled key");
    
    ret->setHelpText(
        AString("This command outputs a set of soft ROIs, one for each label in the input, ") +
        "where the value is how many of the input maps had that label at that vertex, divided by the number of input maps."
    );
    return ret;
}

void AlgorithmLabelProbability::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    const LabelFile* inputLabel = myParams->getLabel(1);
    MetricFile* outputMetric = myParams->getOutputMetric(2);
    bool excludeUnlabeled = myParams->getOptionalParameter(3)->m_present;
    AlgorithmLabelProbability(myProgObj, inputLabel, outputMetric, excludeUnlabeled);
}

AlgorithmLabelProbability::AlgorithmLabelProbability(ProgressObject* myProgObj, const LabelFile* inputLabel, MetricFile* outputMetric, const bool& excludeUnlabeled) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = inputLabel->getNumberOfNodes();
    int numInMaps = inputLabel->getNumberOfMaps();//note: label files have only one label table that covers the entire file, and should never have duplicate names
    map<int, int> keyToOutMapLookup;
    vector<AString> outMapNames;
    const GiftiLabelTable* fileTable = inputLabel->getLabelTable();
    set<int32_t> keys = fileTable->getKeys();
    int32_t unlabeledKey = -1;//don't request it from label table if we aren't going to exclude it, as that could create the unassigned key
    if (excludeUnlabeled)
    {
        unlabeledKey = fileTable->getUnassignedLabelKey();
    }
    for (set<int32_t>::iterator iter = keys.begin(); iter != keys.end(); ++iter)
    {
        if (excludeUnlabeled && *iter == unlabeledKey) continue;
        int newOutMap = (int)keyToOutMapLookup.size();//get size before [] allocates a new member
        keyToOutMapLookup[*iter] = newOutMap;
        outMapNames.push_back(fileTable->getLabelName(*iter));
    }
    int numOutMaps = keyToOutMapLookup.size();
    vector<vector<int32_t> > counts(numOutMaps, vector<int32_t>(numNodes, 0));
    for (int m = 0; m < numInMaps; ++m)
    {
        const int32_t* data = inputLabel->getLabelKeyPointerForColumn(m);
        for (int i = 0; i < numNodes; ++i)
        {
            map<int, int>::iterator search = keyToOutMapLookup.find(data[i]);
            if (search != keyToOutMapLookup.end())
            {
                ++counts[search->second][i];
            }
        }
    }
    vector<float> scratch(numNodes);
    outputMetric->setNumberOfNodesAndColumns(numNodes, numOutMaps);
    outputMetric->setStructure(inputLabel->getStructure());
    for (int m = 0; m < numOutMaps; ++m)
    {
        outputMetric->setMapName(m, outMapNames[m]);
        for (int i = 0; i < numNodes; ++i)
        {
            scratch[i] = ((float)counts[m][i]) / numInMaps;
        }
        outputMetric->setValuesForColumn(m, scratch.data());
    }
}

float AlgorithmLabelProbability::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelProbability::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
