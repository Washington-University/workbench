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

#include "OperationSetMapName.h"
#include "OperationException.h"
#include "DataFileTypeEnum.h"
#include "CiftiFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationSetMapName::getCommandSwitch()
{
    return "-set-map-name";
}

AString OperationSetMapName::getShortDescription()
{
    return "DEPRECATED: use -set-map-names";
}

OperationParameters* OperationSetMapName::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "data-file", "the file to set a map name of");
    
    ret->addIntegerParameter(2, "index", "the map index to set the name of");
    
    ret->addStringParameter(3, "name", "the name to set for the map");
    
    ret->setHelpText(
        AString("DEPRECATED: this command may be removed in a future release, use -set-map-names.\n\n") +
        "Sets the name of a map for metric, shape, label, volume, cifti scalar or cifti label files."
    );
    return ret;
}

void OperationSetMapName::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString fileName = myParams->getString(1);
    int mapIndex = (int)myParams->getInteger(2) - 1;
    if (mapIndex < 0) throw OperationException("invalid map index specified, indexes are 1-based");
    AString mapName = myParams->getString(3);
    bool ok = false;
    DataFileTypeEnum::Enum myType = DataFileTypeEnum::fromFileExtension(fileName, &ok);
    if (!ok)
    {
        throw OperationException("unrecognized data file type");
    }
    switch (myType)
    {
        case DataFileTypeEnum::META_VOLUME:
            CaretAssertToDoFatal();
            break;
        case DataFileTypeEnum::METRIC:
        {
            MetricFile myMetric;
            myMetric.readFile(fileName);
            if (mapIndex >= myMetric.getNumberOfMaps()) throw OperationException("metric file doesn't have enough columns for specified map index");
            myMetric.setMapName(mapIndex, mapName);
            myMetric.writeFile(fileName);
            break;
        }
        case DataFileTypeEnum::LABEL:
        {
            LabelFile myLabel;
            myLabel.readFile(fileName);
            if (mapIndex >= myLabel.getNumberOfMaps()) throw OperationException("label file doesn't have enough columns for specified map index");
            myLabel.setMapName(mapIndex, mapName);
            myLabel.writeFile(fileName);
            break;
        }
        case DataFileTypeEnum::VOLUME:
        {
            VolumeFile myVol;
            myVol.readFile(fileName);
            if (mapIndex >= myVol.getNumberOfMaps()) throw OperationException("volume file doesn't have enough subvolumes for specified map index");
            myVol.setMapName(mapIndex, mapName);
            myVol.writeFile(fileName);
            break;
        }
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
        {
            CiftiFile myCifti;
            myCifti.openFile(fileName);
            myCifti.convertToInMemory();
            CiftiXMLOld myXML = myCifti.getCiftiXMLOld();
            if (mapIndex >= myXML.getNumberOfColumns()) throw OperationException("cifti file doesn't have enough columns for specified map index");
            if (!myXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, mapIndex, mapName)) throw OperationException("failed to set map name, check the type of the cifti file");
            CiftiFile myOutCifti;
            myOutCifti.setWritingFile(fileName);
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
    }
}
