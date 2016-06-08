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

#include "OperationCiftiChangeMapping.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "CiftiFile.h"
#include "FileInformation.h"
#include "MultiDimIterator.h"

#include <fstream>

using namespace caret;
using namespace std;

AString OperationCiftiChangeMapping::getCommandSwitch()
{
    return "-cifti-change-mapping";
}

AString OperationCiftiChangeMapping::getShortDescription()
{
    return "CONVERT TO SCALAR, COPY MAPPING, ETC";
}

OperationParameters* OperationCiftiChangeMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "data-cifti", "the cifti file to use the data from");
    
    ret->addStringParameter(2, "direction", "which direction on <data-cifti> to replace the mapping");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
    
    OptionalParameter* seriesOpt = ret->createOptionalParameter(4, "-series", "set the mapping to series");
    seriesOpt->addDoubleParameter(1, "step", "increment between series points");//this is the order in -cifti-create-from-template, roll with it
    seriesOpt->addDoubleParameter(2, "start", "start value of the series");
    OptionalParameter* seriesUnitOpt = seriesOpt->createOptionalParameter(3, "-unit", "select unit for series (default SECOND)");
    seriesUnitOpt->addStringParameter(1, "unit", "unit identifier");
    
    OptionalParameter* scalarOpt = ret->createOptionalParameter(5, "-scalar", "set the mapping to scalar");
    OptionalParameter* scalarNameFileOpt = scalarOpt->createOptionalParameter(1, "-name-file", "specify names for the maps");
    scalarNameFileOpt->addStringParameter(1, "file", "text file containing map names, one per line");
    
    OptionalParameter* fromCiftiOpt = ret->createOptionalParameter(6, "-from-cifti", "copy mapping from another cifti file");
    fromCiftiOpt->addCiftiParameter(1, "template-cifti", "a cifti file containing the desired mapping");
    fromCiftiOpt->addStringParameter(2, "direction", "which direction to copy the mapping from");
    
    AString helpText =
        AString("Take an existing cifti file and change one of the mappings.  ") +
        "Exactly one of -series, -scalar, or -from-cifti must be specified.  " +
        CiftiXML::directionFromStringExplanation();
    helpText += "\n\nThe argument to -unit must be one of the following:\n";
    vector<CiftiSeriesMap::Unit> unitList = CiftiSeriesMap::getAllUnits();
    for (int i = 0; i < (int)unitList.size(); ++i)
    {
        helpText += "\n" + CiftiSeriesMap::unitToString(unitList[i]);
    }
    ret->setHelpText(helpText);
    return ret;
}

void OperationCiftiChangeMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiFile* inputCifti = myParams->getCifti(1);
    AString dirString = myParams->getString(2);
    CiftiFile* outputCifti = myParams->getOutputCifti(3);
    int direction = CiftiXML::directionFromString(dirString);
    CiftiXML outXML = inputCifti->getCiftiXML();
    if (direction >= outXML.getNumberOfDimensions())
    {
        throw OperationException("specified direction does not exist in <data-cifti> file");
    }
    bool haveOption = false;
    OptionalParameter* seriesOpt = myParams->getOptionalParameter(4);
    if (seriesOpt->m_present)
    {
        haveOption = true;
        CiftiSeriesMap newMap(outXML.getDimensionLength(direction), seriesOpt->getDouble(2), seriesOpt->getDouble(1));
        OptionalParameter* seriesUnitOpt = seriesOpt->getOptionalParameter(3);
        if (seriesUnitOpt->m_present)
        {
            bool ok = false;
            CiftiSeriesMap::Unit myUnit = CiftiSeriesMap::stringToUnit(seriesUnitOpt->getString(1), ok);
            if (!ok)
            {
                throw OperationException("unrecognized string for unit: '" + seriesUnitOpt->getString(1) + "'");
            }
            newMap.setUnit(myUnit);
        }
        outXML.setMap(direction, newMap);
    }
    OptionalParameter* scalarOpt = myParams->getOptionalParameter(5);
    if (scalarOpt->m_present)
    {
        if (haveOption)
        {
            throw OperationException("only one of -series, -scalar, or -from-cifti may be specified");
        }
        haveOption = true;
        CiftiScalarsMap newMap(outXML.getDimensionLength(direction));
        OptionalParameter* scalarNameFileOpt = scalarOpt->getOptionalParameter(1);
        if (scalarNameFileOpt->m_present)
        {
            AString listfileName = scalarNameFileOpt->getString(1);
            FileInformation textFileInfo(listfileName);
            if (!textFileInfo.exists())
            {
                throw OperationException("name list file doesn't exist");
            }
            fstream nameListFile(listfileName.toLocal8Bit().constData(), fstream::in);
            if (!nameListFile.good())
            {
                throw OperationException("error reading name list file");
            }
            string mapName;
            for (int i = 0; i < newMap.getLength(); ++i)
            {
                getline(nameListFile, mapName);
                if (!nameListFile)//no, seriously, that is how you check if your input was good
                {
                    CaretLogWarning("name file contained " + AString::number(i) + " names, expected " + AString::number(newMap.getLength()));
                    break;
                }
                newMap.setMapName(i, mapName.c_str());
            }
        }
        outXML.setMap(direction, newMap);
    }
    OptionalParameter* fromCiftiOpt = myParams->getOptionalParameter(6);
    if (fromCiftiOpt->m_present)
    {
        if (haveOption)
        {
            throw OperationException("only one of -series, -scalar, or -from-cifti may be specified");
        }
        haveOption = true;
        const CiftiFile* templateCifti = fromCiftiOpt->getCifti(1);
        const CiftiXML& templateXML = templateCifti->getCiftiXML();
        AString tempDirString = fromCiftiOpt->getString(2);
        int templateDir = CiftiXML::directionFromString(tempDirString);
        if (templateDir >= templateXML.getNumberOfDimensions())
        {
            throw OperationException("specified direction does not exist in <template-cifti> file");
        }
        if (templateXML.getDimensionLength(templateDir) != outXML.getDimensionLength(direction))
        {
            throw OperationException("selected direction on <template-cifti> has different length than selected direction on <data-cifti>");
        }
        outXML.setMap(direction, *(templateXML.getMap(templateDir)));
    }
    if (!haveOption)
    {
        throw OperationException("you must specify one of -series, -scalar, or -from-cifti");
    }
    outputCifti->setCiftiXML(outXML);
    vector<int64_t> outDims = outXML.getDimensions();
    vector<float> scratchrow(outDims[0]);
    for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
    {//drop the first dimension, row length
        inputCifti->getRow(scratchrow.data(), *iter);
        outputCifti->setRow(scratchrow.data(), *iter);
    }
}
