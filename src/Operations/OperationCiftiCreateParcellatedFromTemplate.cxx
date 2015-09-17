/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "OperationCiftiCreateParcellatedFromTemplate.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "CiftiFile.h"
#include "MultiDimIterator.h"

#include <map>

using namespace caret;
using namespace std;

AString OperationCiftiCreateParcellatedFromTemplate::getCommandSwitch()
{
    return "-cifti-create-parcellated-from-template";
}

AString OperationCiftiCreateParcellatedFromTemplate::getShortDescription()
{
    return "MATCH PARCELS TO TEMPLATE BY NAME";
}

OperationParameters* OperationCiftiCreateParcellatedFromTemplate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-template", "a cifti file with the template parcel mapping along column");
    
    ret->addStringParameter(2, "modify-direction", "which dimension of the output file should match the template, ROW or COLUMN");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
    
    OptionalParameter* fillOpt = ret->createOptionalParameter(4, "-fill-value", "specify value to be used in parcels that don't match");
    fillOpt->addDoubleParameter(1, "value", "value to use (default 0)");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(5, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "the input parcellated cifti file");
    
    ret->setHelpText(
        AString("For each parcel name in the template mapping, find that name in an input cifti file and use its data in the output file.  ") +
        "All input cifti files must have a parcels mapping along <modify-direction> and matching mappings along other dimensions."
    );
    return ret;
}

void OperationCiftiCreateParcellatedFromTemplate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* templateCifti = myParams->getCifti(1);
    AString dirString = myParams->getString(2);
    int direction = -1;
    if (dirString == "ROW")
    {
        direction = CiftiXML::ALONG_ROW;
    } else {
        if (dirString == "COLUMN")
        {
            direction = CiftiXML::ALONG_COLUMN;
        } else {
            throw OperationException("unrecognized direction string, use ROW or COLUMN");
        }
    }
    CiftiFile* ciftiOut = myParams->getOutputCifti(3);
    OptionalParameter* fillOpt = myParams->getOptionalParameter(4);
    float fillValue = 0.0f;
    if (fillOpt->m_present)
    {
        fillValue = (float)fillOpt->getDouble(1);
    }
    const vector<ParameterComponent*>& inputInstances = *(myParams->getRepeatableParameterInstances(5));
    const CiftiXML& templateXML = templateCifti->getCiftiXML();
    if (templateXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::PARCELS)
    {
        throw OperationException("template file does not have a parcel mapping along column");
    }
    int numInstances = (int)inputInstances.size();
    if (numInstances < 1)
    {
        throw OperationException("at least one input cifti file must be specified");
    }
    const CiftiXML& firstXML = inputInstances[0]->getCifti(1)->getCiftiXML();
    int numDims = firstXML.getNumberOfDimensions();
    for (int i = 0; i < numInstances; ++i)
    {
        const CiftiFile* thisInput = inputInstances[i]->getCifti(1);
        const CiftiXML& thisXML = thisInput->getCiftiXML();
        if (numDims != thisXML.getNumberOfDimensions())
        {
            throw OperationException("input '" + thisInput->getFileName() + "' has different number of dimensions");
        }
        for (int j = 0; j < numDims; ++j)
        {
            if (j == direction)
            {
                if (thisXML.getMappingType(j) != CiftiMappingType::PARCELS)
                {
                    throw OperationException("input '" + thisInput->getFileName() + "' does not contain parcels mapping along specified direction");
                }
            } else {
                if (!thisXML.getMap(j)->approximateMatch(*(firstXML.getMap(j))))
                {
                    throw OperationException("input '" + thisInput->getFileName() + "' has non-matching mapping along other direction(s)");
                }
            }
        }
    }
    const CiftiParcelsMap& myTemplateMap = templateXML.getParcelsMap(CiftiXML::ALONG_COLUMN);
    int numOutParcels = myTemplateMap.getLength();
    map<AString, int> nameMatcher;
    for (int i = 0; i < numOutParcels; ++i)
    {
        if (nameMatcher.find(myTemplateMap.getIndexName(i)) != nameMatcher.end())
        {
            throw OperationException("template parcel mapping reuses a parcel name");
        }
        nameMatcher[myTemplateMap.getIndexName(i)] = i;
    }
    vector<int> sourceInput(numOutParcels, -1), sourceParcel(numOutParcels);
    for (int i = 0; i < numInstances; ++i)
    {
        const CiftiFile* thisInput = inputInstances[i]->getCifti(1);
        const CiftiXML& thisXML = thisInput->getCiftiXML();
        const CiftiParcelsMap& thisParcelMap = thisXML.getParcelsMap(direction);
        int thisNumParcels = thisParcelMap.getLength();
        bool match = false;
        for (int j = 0; j < thisNumParcels; ++j)
        {
            map<AString, int>::const_iterator result = nameMatcher.find(thisParcelMap.getIndexName(j));
            if (result != nameMatcher.end())
            {
                match = true;
                if (sourceInput[result->second] != -1)
                {
                    throw OperationException("multiple input parcels match output parcel '" + result->first + "'");
                }
                sourceInput[result->second] = i;
                sourceParcel[result->second] = j;
            }
        }
        if (!match)
        {
            CaretLogWarning("input '" + thisInput->getFileName() + "' does not match any template parcels");
        }
    }
    CiftiXML outXML = firstXML;
    outXML.setMap(direction, myTemplateMap);
    ciftiOut->setCiftiXML(outXML);
    if (direction == CiftiXML::ALONG_ROW)
    {
        vector<float> outRow(numOutParcels, fillValue);
        vector<vector<float> > inputRows(numInstances);
        for (int i = 0; i < numInstances; ++i)
        {
            const CiftiFile* thisInput = inputInstances[i]->getCifti(1);
            const CiftiXML& thisXML = thisInput->getCiftiXML();
            inputRows[i].resize(thisXML.getDimensionLength(CiftiXML::ALONG_ROW));
        }
        vector<int64_t> iterDims = firstXML.getDimensions();
        iterDims.erase(iterDims.begin());//remove row dimension
        for (MultiDimIterator<int64_t> iter(iterDims); !iter.atEnd(); ++iter)
        {
            for (int i = 0; i < numInstances; ++i)
            {
                const CiftiFile* thisInput = inputInstances[i]->getCifti(1);
                thisInput->getRow(inputRows[i].data(), *iter);
            }
            for (int i = 0; i < numOutParcels; ++i)
            {
                if (sourceInput[i] != -1)
                {
                    outRow[i] = inputRows[sourceInput[i]][sourceParcel[i]];
                }
            }
            ciftiOut->setRow(outRow.data(), *iter);
        }
    } else {
        vector<float> fillRow(firstXML.getDimensionLength(CiftiXML::ALONG_ROW), fillValue);
        vector<float> scratchRow = fillRow;
        vector<int64_t> iterDims = firstXML.getDimensions();
        iterDims.erase(iterDims.begin() + direction);//remove remapped dimension
        iterDims.erase(iterDims.begin());//remove row dimension
        for (MultiDimIterator<int64_t> iter(iterDims); !iter.atEnd(); ++iter)
        {
            vector<int64_t> restoreDims = *iter;
            restoreDims.insert(restoreDims.begin() + (direction - 1), -1);//deliberately invalid placeholder
            for (int i = 0; i < numOutParcels; ++i)
            {
                if (sourceInput[i] == -1)
                {
                    restoreDims[direction - 1] = i;
                    ciftiOut->setRow(fillRow.data(), restoreDims);
                } else {
                    restoreDims[direction - 1] = sourceParcel[i];
                    const CiftiFile* thisInput = inputInstances[sourceInput[i]]->getCifti(1);
                    thisInput->getRow(scratchRow.data(), restoreDims);
                    restoreDims[direction - 1] = i;
                    ciftiOut->setRow(scratchRow.data(), restoreDims);
                }
            }
        }
    }
}
