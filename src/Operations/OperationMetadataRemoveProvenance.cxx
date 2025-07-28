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

#include "OperationMetadataRemoveProvenance.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "DataFileTypeEnum.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "FileInformation.h"

using namespace caret;
using namespace std;

AString OperationMetadataRemoveProvenance::getCommandSwitch()
{
    return "-metadata-remove-provenance";
}

AString OperationMetadataRemoveProvenance::getShortDescription()
{
    return "REMOVE PROVENANCE INFORMATION FROM FILE METADATA";
}

OperationParameters* OperationMetadataRemoveProvenance::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "input-file", "the file to remove provenance information from");
    
    ret->addStringParameter(2, "output-file", "output - the name to save the modified file as");//HACK: fake the output format, since it doesn't know what kind of file it is
    
    ret->setHelpText(
        AString("Removes the provenance metadata fields added by workbench during processing.")
    );
    return ret;
}

void OperationMetadataRemoveProvenance::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString inFileName = myParams->getString(1);
    AString outFileName = myParams->getString(2);
    bool ok = false;
    DataFileTypeEnum::Enum myType = DataFileTypeEnum::fromFileExtension(inFileName, &ok);
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
            myMetric.readFile(inFileName);
            removeProvenance(myMetric.getFileMetaData());
            myMetric.writeFile(outFileName);
            break;
        }
        case DataFileTypeEnum::LABEL:
        {
            LabelFile myLabel;
            myLabel.readFile(inFileName);
            removeProvenance(myLabel.getFileMetaData());
            myLabel.writeFile(outFileName);
            break;
        }
        case DataFileTypeEnum::VOLUME:
        {
            VolumeFile myVol;
            myVol.readFile(inFileName);
            removeProvenance(myVol.getFileMetaData());
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
            removeProvenance(myXML.getFileMetaData());
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
            throw OperationException("file type not supported in metadata remove provenance");
    }
}

void OperationMetadataRemoveProvenance::removeProvenance(GiftiMetaData* toModify)
{
    if (toModify == NULL) return;
    set<AString> provenanceNames = getProvenanceKeys();
    for (set<AString>::iterator iter = provenanceNames.begin(); iter != provenanceNames.end(); ++iter)
    {
        toModify->remove(*iter);
    }
}

void OperationMetadataRemoveProvenance::removeProvenance(map<AString, AString>* toModify)
{
    if (toModify == NULL) return;
    set<AString> provenanceNames = getProvenanceKeys();
    for (set<AString>::iterator iter = provenanceNames.begin(); iter != provenanceNames.end(); ++iter)
    {
        toModify->erase(*iter);
    }
}

set<AString> OperationMetadataRemoveProvenance::getProvenanceKeys()
{
    set<AString> ret;
    ret.insert("Provenance");//these should really be in a common location somewhere
    ret.insert("ParentProvenance");
    ret.insert("ProgramProvenance");
    ret.insert("WorkingDirectory");
    return ret;
}
