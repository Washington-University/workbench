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

#include "AlgorithmCiftiLabelProbability.h"
#include "AlgorithmException.h"

#include "CiftiFile.h"

#include <cmath>
#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiLabelProbability::getCommandSwitch()
{
    return "-cifti-label-probability";
}

AString AlgorithmCiftiLabelProbability::getShortDescription()
{
    return "FIND FREQUENCY OF CIFTI LABELS";
}

OperationParameters* AlgorithmCiftiLabelProbability::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "label-maps", "cifti dlabel file containing individual label maps from many subjects");
    
    ret->addCiftiOutputParameter(2, "probability-dscalar-out", "the relative frequencies of each label at each vertex/voxel");
    
    ret->createOptionalParameter(3, "-exclude-unlabeled", "don't make a probability map of the unlabeled key");
    
    ret->setHelpText(
        AString("This command outputs a set of soft ROIs, one for each label in the input, ") +
        "where the value is how many of the input maps had that label at that vertex/voxel, divided by the number of input maps."
    );
    return ret;
}

void AlgorithmCiftiLabelProbability::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    const CiftiFile* inputLabel = myParams->getCifti(1);
    CiftiFile* outputCifti = myParams->getOutputCifti(2);
    bool excludeUnlabeled = myParams->getOptionalParameter(3)->m_present;
    AlgorithmCiftiLabelProbability(myProgObj, inputLabel, outputCifti, excludeUnlabeled);
}

AlgorithmCiftiLabelProbability::AlgorithmCiftiLabelProbability(ProgressObject* myProgObj, const CiftiFile* inputLabel, CiftiFile* outputCifti, const bool& excludeUnlabeled) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& inputXML = inputLabel->getCiftiXML();
    if (inputXML.getNumberOfDimensions() != 2 ||
        inputXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS ||
        inputXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS)
    {
        throw AlgorithmException("input cifti file does not have the mapping types of a dlabel file");
    }
    const CiftiLabelsMap& inputLabelMap = inputXML.getLabelsMap(CiftiXML::ALONG_ROW);
    int64_t numInMaps = inputLabelMap.getLength();
    vector<map<int, int> > keyToOutMapLookup(numInMaps);//we match labels by name, not by key - since cifti files are more memory efficient than volume files, do it the fast way
    map<AString, int> nameToOutMap;
    for (int64_t m = 0; m < numInMaps; ++m)
    {
        const GiftiLabelTable* mapTable = inputLabelMap.getMapLabelTable(m);
        set<int32_t> mapKeys = mapTable->getKeys();
        int32_t unlabeledKey = -1;//don't request it from the table if we aren't going to skip it, because requesting it can add it to the table
        if (excludeUnlabeled)
        {
            unlabeledKey = mapTable->getUnassignedLabelKey();
        }
        for (set<int32_t>::iterator iter = mapKeys.begin(); iter != mapKeys.end(); ++iter)//order by key value
        {
            if (excludeUnlabeled && *iter == unlabeledKey) continue;//skip to next key
            const AString thisName = mapTable->getLabelName(*iter);
            map<AString, int>::iterator search = nameToOutMap.find(thisName);
            int outMap = -1;
            if (search == nameToOutMap.end())
            {
                outMap = nameToOutMap.size();//sequential integers starting from 0
                nameToOutMap[thisName] = outMap;
            } else {
                outMap = search->second;
            }
            keyToOutMapLookup[m][*iter] = outMap;
        }
    }//for simplicity, do the actual processing in a second loop
    int64_t numOutMaps = nameToOutMap.size(), colSize = inputXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
    vector<vector<int> > counts(numOutMaps, vector<int>(colSize, 0));
    for (int64_t row = 0; row < colSize; ++row)
    {
        vector<float> scratchRow(numInMaps);
        inputLabel->getRow(scratchRow.data(), row);
        for (int64_t m = 0; m < numInMaps; ++m)
        {
            int thiskey = (int)floor(scratchRow[m] + 0.5f);
            map<int, int>::iterator search = keyToOutMapLookup[m].find(thiskey);
            if (search != keyToOutMapLookup[m].end())
            {
                ++counts[search->second][row];
            }
        }
    }
    CiftiXML outXML;
    outXML.setNumberOfDimensions(2);
    outXML.setMap(CiftiXML::ALONG_COLUMN, inputXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN));
    CiftiScalarsMap outRowMap;
    outRowMap.setLength(numOutMaps);
    for (map<AString, int>::iterator iter = nameToOutMap.begin(); iter != nameToOutMap.end(); ++iter)
    {
        outRowMap.setMapName(iter->second, iter->first);
    }
    outXML.setMap(CiftiXML::ALONG_ROW, outRowMap);
    outputCifti->setCiftiXML(outXML);
    for (int64_t row = 0; row < colSize; ++row)
    {
        vector<float> scratchRow(numOutMaps);
        for (int64_t m = 0; m < numOutMaps; ++m)
        {
            scratchRow[m] = ((float)counts[m][row]) / numInMaps;
        }
        outputCifti->setRow(scratchRow.data(), row);
    }
}

float AlgorithmCiftiLabelProbability::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiLabelProbability::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
