
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

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "CaretDataFile.h"
#include "CaretDataFileHelper.h"
#include "CaretMappableDataFile.h"
#include "CaretPointer.h"
#include "CiftiMappableDataFile.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "GiftiMetaData.h"
#include "OperationFileInformation.h"
#include "OperationException.h"

using namespace caret;
using namespace std;

/**
 * \class caret::OperationFileInformation 
 * \brief List information about a file's content
 */

/**
 * @return Command line switch
 */
AString
OperationFileInformation::getCommandSwitch()
{
    return "-file-information";
}

/**
 * @return Short description of operation
 */
AString
OperationFileInformation::getShortDescription()
{
    return "LIST INFORMATION ABOUT A FILE'S CONTENT";
}

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationFileInformation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "data-file", "data file");
    
    ret->createOptionalParameter(2, "-no-map-info", "do not show map information for files that support maps");
    
    ret->createOptionalParameter(3, "-only-step-interval", "suppress normal output, print the interval between maps");

    ret->createOptionalParameter(4, "-only-number-of-maps", "suppress normal output, print the number of maps");

    ret->createOptionalParameter(5, "-only-map-names", "suppress normal output, print the names of all maps");
    
    OptionalParameter* metadataOpt = ret->createOptionalParameter(6, "-only-metadata", "suppress normal output, print file metadata");
    OptionalParameter* mdKeyOpt = metadataOpt->createOptionalParameter(1, "-key", "only print the metadata for one key, with no formatting");
    mdKeyOpt->addStringParameter(1, "key", "the metadata key");
    
    AString helpText("List information about the content of a data file.  "
                     "Only one -only option may be specified.  "
                     "The information listed when no -only option is present is dependent upon the type of data file.");
    
    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
void
OperationFileInformation::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    
    const AString dataFileName = myParams->getString(1);
    
    const bool showMapInformationFlag = ! (myParams->getOptionalParameter(2)->m_present);
    
    int countOnlys = 0;
    bool onlyTimestep = myParams->getOptionalParameter(3)->m_present;
    if (onlyTimestep) ++countOnlys;

    bool onlyNumMaps = myParams->getOptionalParameter(4)->m_present;
    if (onlyNumMaps) ++countOnlys;
    
    bool onlyMapNames = myParams->getOptionalParameter(5)->m_present;
    if (onlyMapNames) ++countOnlys;
    
    OptionalParameter* metadataOpt = myParams->getOptionalParameter(6);
    bool onlyMetadata = metadataOpt->m_present;
    AString mdKey = "";
    if (onlyMetadata)
    {
        ++countOnlys;
        OptionalParameter* mdKeyOpt = metadataOpt->getOptionalParameter(1);
        if (mdKeyOpt->m_present)
        {
            mdKey = mdKeyOpt->getString(1);
            if (mdKey == "")
            {
                throw OperationException("<key> must not be empty");
            }
        }
    }
    
    if (countOnlys > 1) throw OperationException("only one -only-* option may be specified");
    
    bool preferOnDisk = (!showMapInformationFlag || countOnlys != 0);

    CaretPointer<CaretDataFile> caretDataFile;
    try {
        caretDataFile.grabNew(CaretDataFileHelper::readAnyCaretDataFile(dataFileName, preferOnDisk));
    }
    catch (const DataFileException& dfe) {
//        No longer needed since CaretDataFileHelper::readAnyCaretDataFile will
//        handle this case
//        /*
//         * If the name ends with ".nii" but was rejected by VolumeFile, it could be
//         * an unsupported CIFTI file type.
//         */
//        if (dataFileName.endsWith(".nii")) {
//            try {
//                DataFileContentInformation dataFileContentInformation;
//                CiftiMappableDataFile::getDataFileContentInformationForGenericCiftiFile(dataFileName,
//                                                                                        dataFileContentInformation);
//                
//                cout << qPrintable(dataFileContentInformation.getInformationInString()) << endl;
//            }
//            catch (const DataFileException& dfe2) {
//                throw DataFileException(dfe.whatString()
//                                        + "\n"
//                                        + "Also unsuccessful trying to read as generic CIFTI file.");
//            }
//            
//            return;
//            
//        }
//        else {
            throw dfe;
//        }
    }
    
    if (onlyTimestep)
    {
        CaretMappableDataFile* mappableFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile.getPointer());
        if (mappableFile == NULL) throw OperationException("file does not support maps");//TODO: also give error on things that it doesn't make sense on
        if (mappableFile->getMapIntervalUnits() == NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN) throw OperationException("file does not support series data");
        float start, step;
        mappableFile->getMapIntervalStartAndStep(start, step);
        cout << step << endl;
    }
    if (onlyNumMaps)
    {
        CaretMappableDataFile* mappableFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile.getPointer());
        if (mappableFile == NULL) throw OperationException("file does not support maps");//TODO: also give error on things that it doesn't make sense on
        int numMaps = mappableFile->getNumberOfMaps();
        if (numMaps < 1) throw OperationException("file does not support maps");
        cout << numMaps << endl;
    }
    if (onlyMapNames)
    {
        CaretMappableDataFile* mappableFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile.getPointer());
        if (mappableFile == NULL) throw OperationException("file does not support maps");//TODO: also give error on things that it doesn't make sense on
        int numMaps = mappableFile->getNumberOfMaps();
        if (numMaps < 1) throw OperationException("file does not support maps");
        for (int i = 0; i < numMaps; ++i)
        {
            cout << mappableFile->getMapName(i) << endl;
        }
    }
    if (onlyMetadata)
    {
        const GiftiMetaData* myMD = caretDataFile->getFileMetaData();
        if (mdKey == "")
        {
            const map<AString, AString> mdMap = myMD->getAsMap();
            for (map<AString, AString>::const_iterator iter = mdMap.begin(); iter != mdMap.end(); ++iter)
            {
                cout << "   " << iter->first << ":" << endl;
                cout << iter->second << endl << endl;
            }
        } else {
            if (!myMD->exists(mdKey))
            {
                throw OperationException("specified metadata key is not present in the file");
            }
            cout << myMD->get(mdKey) << endl;
        }
    }
    if (countOnlys == 0)
    {
        DataFileContentInformation dataFileContentInformation;
        dataFileContentInformation.setOptionFlag(DataFileContentInformation::OPTION_SHOW_MAP_INFORMATION,
                                                showMapInformationFlag);
        
        caretDataFile->addToDataFileContentInformation(dataFileContentInformation);
        
        cout << qPrintable(dataFileContentInformation.getInformationInString()) << endl;
    }
}

