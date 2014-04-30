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

#include "OperationMetadataStringReplace.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "DataFileTypeEnum.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "FileInformation.h"

using namespace caret;
using namespace std;

AString OperationMetadataStringReplace::getCommandSwitch()
{
    return "-metadata-string-replace";
}

AString OperationMetadataStringReplace::getShortDescription()
{
    return "REPLACE A STRING IN ALL METADATA OF A FILE";
}

OperationParameters* OperationMetadataStringReplace::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "input-file", "the file to replace metadata in");
    
    ret->addStringParameter(2, "find-string", "the string to find");
    
    ret->addStringParameter(3, "replace-string", "the string to replace <find-string> with");
    
    ret->addStringParameter(4, "output-file", "output - the name to save the modified file as");//HACK: fake the output format, since it doesn't know what kind of file it is
    
    ret->createOptionalParameter(5, "-case-insensitive", "match with case variation also");
    
    ret->setHelpText(
        AString("Replaces all occurrences of <find-string> in the metadata and map names of <input-file> with <replace-string>.")
    );
    return ret;
}

void OperationMetadataStringReplace::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString inFileName = myParams->getString(1);
    AString findString = myParams->getString(2);
    AString replString = myParams->getString(3);
    AString outFileName = myParams->getString(4);
    Qt::CaseSensitivity myCS = Qt::CaseSensitive;
    if (myParams->getOptionalParameter(5)->m_present)
    {
        myCS = Qt::CaseInsensitive;
    }
    bool ok = false;
    DataFileTypeEnum::Enum myType = DataFileTypeEnum::fromFileExtension(inFileName, &ok);
    if (!ok)
    {
        throw OperationException("unrecognized data file type");
    }
    switch (myType)
    {
        case DataFileTypeEnum::METRIC:
        {
            MetricFile myMetric;
            myMetric.readFile(inFileName);
            replaceInMetaData(myMetric.getFileMetaData(), findString, replString, myCS);
            int32_t numMaps = myMetric.getNumberOfMaps();
            for (int32_t map = 0; map < numMaps; ++map)
            {
                replaceInMetaData(myMetric.getMapMetaData(map), findString, replString, myCS);
                AString mapName = myMetric.getMapName(map);
                myMetric.setMapName(map, mapName.replace(findString, replString, myCS));
            }
            myMetric.writeFile(outFileName);
            break;
        }
        case DataFileTypeEnum::LABEL:
        {
            LabelFile myLabel;
            myLabel.readFile(inFileName);
            replaceInMetaData(myLabel.getFileMetaData(), findString, replString, myCS);
            int32_t numMaps = myLabel.getNumberOfMaps();
            for (int32_t map = 0; map < numMaps; ++map)
            {
                replaceInMetaData(myLabel.getMapMetaData(map), findString, replString, myCS);
                AString mapName = myLabel.getMapName(map);
                myLabel.setMapName(map, mapName.replace(findString, replString, myCS));
            }
            myLabel.writeFile(outFileName);
            break;
        }
        case DataFileTypeEnum::VOLUME:
        {
            VolumeFile myVol;
            myVol.readFile(inFileName);
            replaceInMetaData(myVol.getFileMetaData(), findString, replString, myCS);
            int32_t numMaps = myVol.getNumberOfMaps();
            for (int32_t map = 0; map < numMaps; ++map)
            {
                replaceInMetaData(myVol.getMapMetaData(map), findString, replString, myCS);
                AString mapName = myVol.getMapName(map);
                myVol.setMapName(map, mapName.replace(findString, replString, myCS));
            }
            myVol.writeFile(outFileName);
            break;
        }
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
        {
            CiftiFile myCifti;
            FileInformation myInfo1(inFileName), myInfo2(outFileName);
            myCifti.openFile(inFileName);
            if (myInfo1.getCanonicalFilePath() == myInfo2.getCanonicalFilePath())
            {
                myCifti.convertToInMemory();
            }
            CiftiXMLOld myXML = myCifti.getCiftiXMLOld();
            CiftiFile myOutCifti;
            myOutCifti.setWritingFile(outFileName);
            int numRows = myXML.getNumberOfRows(), rowSize = myXML.getNumberOfColumns();
            replaceInMetaData(myXML.getFileMetaData(), findString, replString, myCS);
            if (myXML.getMappingType(CiftiXMLOld::ALONG_ROW) == CIFTI_INDEX_TYPE_SCALARS || myXML.getMappingType(CiftiXMLOld::ALONG_ROW) == CIFTI_INDEX_TYPE_LABELS)
            {
                for (int row = 0; row < numRows; ++row)
                {
                    AString mapName = myXML.getMapName(CiftiXMLOld::ALONG_ROW, row);
                    myXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, row, mapName.replace(findString, replString, myCS));
                    replaceInMetaData(myXML.getMapMetadata(CiftiXMLOld::ALONG_ROW, row), findString, replString, myCS);
                }
            }
            if (myXML.getMappingType(CiftiXMLOld::ALONG_COLUMN) == CIFTI_INDEX_TYPE_SCALARS || myXML.getMappingType(CiftiXMLOld::ALONG_COLUMN) == CIFTI_INDEX_TYPE_LABELS)
            {
                for (int col = 0; col < rowSize; ++col)
                {
                    AString mapName = myXML.getMapName(CiftiXMLOld::ALONG_COLUMN, col);
                    myXML.setMapNameForIndex(CiftiXMLOld::ALONG_COLUMN, col, mapName.replace(findString, replString, myCS));
                    replaceInMetaData(myXML.getMapMetadata(CiftiXMLOld::ALONG_COLUMN, col), findString, replString, myCS);
                }
            }
            myOutCifti.setCiftiXML(myXML, false);
            vector<float> scratchRow(rowSize);
            for (int i = 0; i < numRows; ++i)
            {
                myCifti.getRow(scratchRow.data(), i);
                myOutCifti.setRow(scratchRow.data(), i);
            }
            myOutCifti.writeFile(outFileName);
            break;
        }
        default:
            throw OperationException("file type not supported in metadata string replace");
    }
}

void OperationMetadataStringReplace::replaceInMetaData(GiftiMetaData* toModify, const AString& findStr, const AString& replStr, const Qt::CaseSensitivity& myCS)
{
    if (toModify == NULL) return;
    vector<AString> metaNames = toModify->getAllMetaDataNames();
    int64_t numNames = (int64_t)metaNames.size();
    for (int64_t i = 0; i < numNames; ++i)
    {
        AString value = toModify->get(metaNames[i]);
        toModify->set(metaNames[i], value.replace(findStr, replStr, myCS));
    }
}

void OperationMetadataStringReplace::replaceInMetaData(map<AString, AString>* toModify, const AString& findStr, const AString& replStr, const Qt::CaseSensitivity& myCS)
{
    if (toModify == NULL) return;
    for (map<AString, AString>::iterator iter = toModify->begin(); iter != toModify->end(); ++iter)
    {
        iter->second.replace(findStr, replStr, myCS);//replace actually modifies the string itself
    }
}
