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

#include "OperationSetMapNames.h"
#include "OperationException.h"
#include "CaretDataFile.h"
#include "CaretDataFileHelper.h"
#include "CaretMappableDataFile.h"
#include "CaretPointer.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationSetMapNames::getCommandSwitch()
{
    return "-set-map-names";
}

AString OperationSetMapNames::getShortDescription()
{
    return "SET THE NAME OF ONE OR MORE MAPS IN A FILE";
}

OperationParameters* OperationSetMapNames::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "data-file", "the file to set the map names of");
    
    ParameterComponent* mapOpt = ret->createRepeatableParameter(2, "-map", "specify a map to set the name of");
    
    mapOpt->addIntegerParameter(1, "index", "the map index to change the name of");
    
    mapOpt->addStringParameter(2, "new-name", "the name to set for the map");
    
    ret->setHelpText(
        AString("Sets the name of one or more maps for metric, shape, label, volume, cifti scalar or cifti label files.  ") +
        "The -map option must be specified at least once."
    );
    return ret;
}

void OperationSetMapNames::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString fileName = myParams->getString(1);
    const vector<ParameterComponent*>& mapOpts = *(myParams->getRepeatableParameterInstances(2));
    CaretPointer<CaretDataFile> caretDataFile(CaretDataFileHelper::readAnyCaretDataFile(fileName));
    CaretMappableDataFile* mappableFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile.getPointer());
    if (mappableFile == NULL) throw OperationException("cannot set map name on this file type");
    for (int i = 0; i < (int)mapOpts.size(); ++i)
    {
        int mapIndex = (int)mapOpts[i]->getInteger(1) - 1;
        if (mapIndex < 0) throw OperationException("invalid map index, indices are 1-based");
        if (mapIndex >= mappableFile->getNumberOfMaps()) throw OperationException("invalid map index, file doesn't have enough maps");
        AString newName = mapOpts[i]->getString(2);
        mappableFile->setMapName(mapIndex, newName);
    }
    mappableFile->writeFile(fileName);
    /*bool ok = false;
    DataFileTypeEnum::Enum myType = DataFileTypeEnum::fromFileExtension(fileName, &ok);
    if (!ok)
    {
        throw OperationException("unrecognized data file type for file '" + fileName + "'");
    }
    if (mapOpts.size() == 0) throw OperationException("the -map option must be specified at least once");
    switch (myType)
    {
        case DataFileTypeEnum::METRIC:
        {
            MetricFile myMetric;
            myMetric.readFile(fileName);
            for (int i = 0; i < (int)mapOpts.size(); ++i)
            {
                int mapIndex = (int)mapOpts[i]->getInteger(1) - 1;
                if (mapIndex < 0) throw OperationException("invalid map index, indices are 1-based");
                if (mapIndex >= myMetric.getNumberOfMaps()) throw OperationException("invalid map index, file doesn't have enough maps");
                AString newName = mapOpts[i]->getString(2);
                myMetric.setMapName(mapIndex, newName);
            }
            myMetric.writeFile(fileName);
            break;
        }
        case DataFileTypeEnum::LABEL:
        {
            LabelFile myLabel;
            myLabel.readFile(fileName);
            for (int i = 0; i < (int)mapOpts.size(); ++i)
            {
                int mapIndex = (int)mapOpts[i]->getInteger(1) - 1;
                if (mapIndex < 0) throw OperationException("invalid map index, indices are 1-based");
                if (mapIndex >= myLabel.getNumberOfMaps()) throw OperationException("invalid map index, file doesn't have enough maps");
                AString newName = mapOpts[i]->getString(2);
                myLabel.setMapName(mapIndex, newName);
            }
            myLabel.writeFile(fileName);
            break;
        }
        case DataFileTypeEnum::VOLUME:
        {
            VolumeFile myVol;
            myVol.readFile(fileName);
            for (int i = 0; i < (int)mapOpts.size(); ++i)
            {
                int mapIndex = (int)mapOpts[i]->getInteger(1) - 1;
                if (mapIndex < 0) throw OperationException("invalid map index, indices are 1-based");
                if (mapIndex >= myVol.getNumberOfMaps()) throw OperationException("invalid map index, file doesn't have enough maps");
                AString newName = mapOpts[i]->getString(2);
                myVol.setMapName(mapIndex, newName);
            }
            myVol.writeFile(fileName);
            break;
        }
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
        {
            CiftiFile myCifti;
            myCifti.openFile(fileName, IN_MEMORY);
            CiftiXMLOld myXML = myCifti.getCiftiXMLOld();
            for (int i = 0; i < (int)mapOpts.size(); ++i)
            {
                int mapIndex = (int)mapOpts[i]->getInteger(1) - 1;
                if (mapIndex < 0) throw OperationException("invalid map index, indices are 1-based");
                if (mapIndex >= myXML.getNumberOfColumns()) throw OperationException("cifti file doesn't have enough columns for specified map index");
                AString newName = mapOpts[i]->getString(2);
                if (!myXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, mapIndex, newName)) throw OperationException("failed to set map name, check the type of the cifti file");
            }
            CiftiFile myOutCifti(ON_DISK);
            myOutCifti.setCiftiCacheFile(fileName);
            myOutCifti.setCiftiXML(myXML);
            int numRows = myXML.getNumberOfRows(), rowSize = myXML.getNumberOfColumns();
            vector<float> scratchRow(rowSize);
            for (int i = 0; i < numRows; ++i)
            {
                myCifti.getRow(scratchRow.data(), i);
                myOutCifti.setRow(scratchRow.data(), i);
            }
            myOutCifti.writeFile(fileName);
            break;
        }
        default:
            throw OperationException("cannot set map name on this file type");
    }//*/
}
