
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __EVENT_CARET_DATA_FILES_GET_DECLARE__
#include "EventCaretDataFilesGet.h"
#undef __EVENT_CARET_DATA_FILES_GET_DECLARE__

#include <set>

#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventTypeEnum.h"
#include "FileIdentificationAttributes.h"
#include "FileInformation.h"

using namespace caret;


    
/**
 * \class caret::EventCaretDataFilesGet 
 * \brief Event to get CaretDataFiles
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param structure
 *     Structure.
 * @param dataFileTypes
 *     Type of data files.
 */
EventCaretDataFilesGet::EventCaretDataFilesGet(const StructureEnum::Enum structure,
                                               const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
: Event(EventTypeEnum::EVENT_CARET_DATA_FILES_GET)
, m_structure(structure),
m_dataFileTypes(dataFileTypes)
{
    
}

/**
 * Destructor.
 */
EventCaretDataFilesGet::~EventCaretDataFilesGet()
{
}

/**
 * @return CaretDataFile with the given name or NULL if no match.
 * @param filename
 *    Name of file.
 */
CaretDataFile*
EventCaretDataFilesGet::getCaretDataFileWithName(const AString& filename)
{
    const std::vector<CaretDataFile*> allFiles(EventCaretDataFilesGet::getAllCaretDataFiles());
    const bool absPathFlag(FileInformation(filename).isAbsolute());
    
    CaretDataFile* caretDataFileOut(NULL);
    if (absPathFlag) {
        for (auto& dataFile : allFiles) {
            CaretAssert(dataFile);
            if (dataFile->getFileName() == filename) {
                if (caretDataFileOut != NULL) {
                    CaretLogWarning("More than one file has same absolute path name: "
                                    + filename);
                }
                else {
                   caretDataFileOut = dataFile;
                }
            }
        }
    }
    else {
        for (auto& dataFile : allFiles) {
            CaretAssert(dataFile);
            if (dataFile->getFileName().endsWith(filename)) {
                if (caretDataFileOut != NULL) {
                    CaretLogWarning("More than one file matches relative path name: "
                                    + filename);
                }
                else {
                    caretDataFileOut = dataFile;
                }
            }
        }
    }

    return caretDataFileOut;
}


/**
 * @return Data file types of all loaded data files
 */
std::vector<DataFileTypeEnum::Enum>
EventCaretDataFilesGet::getAllCaretDataFileTyes()
{
    std::set<DataFileTypeEnum::Enum> uniqueTypes;
    
    std::vector<CaretDataFile*> allFiles(getAllCaretDataFiles());
    for (const auto& file : allFiles) {
        uniqueTypes.insert(file->getDataFileType());
    }
    
    std::vector<DataFileTypeEnum::Enum> typesVector(uniqueTypes.begin(),
                                                    uniqueTypes.end());
    return typesVector;
}

/**
 * @return All loaded caret data files.
 */
std::vector<CaretDataFile*>
EventCaretDataFilesGet::getAllCaretDataFiles()
{
    std::vector<DataFileTypeEnum::Enum> types;
    
    std::vector<CaretDataFile*> files;
    run(StructureEnum::ALL,
        types,
        files);
    
    return files;
}

/**
 * @return All data files of the given data file type.
 *
 * @param dataFileType
 *     Type of data file.
 */
std::vector<CaretDataFile*>
EventCaretDataFilesGet::getCaretDataFilesForType(const DataFileTypeEnum::Enum dataFileType)
{
    std::vector<DataFileTypeEnum::Enum> types;
    types.push_back(dataFileType);
    
    std::vector<CaretDataFile*> files;
    run(StructureEnum::ALL,
        types,
        files);
    
    return files;
}

/**
 * @return All data files of the given multiple data file types.
 *
 * @param dataFileTypes
 *     Types of data files.
 */
std::vector<CaretDataFile*>
EventCaretDataFilesGet::getCaretDataFilesForTypes(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
{
    std::vector<CaretDataFile*> files;
    run(StructureEnum::ALL,
        dataFileTypes,
        files);
    
    return files;
}

/**
 * @return All data files of the given structure and multiple data file types.
 *
 * @param structure
 *     Structure.
 * @param dataFileTypes
 *     Type of data files.
 */
std::vector<CaretDataFile*>
EventCaretDataFilesGet::getCaretDataFilesForStructureAndTypes(const StructureEnum::Enum structure,
                                                              const std::vector<DataFileTypeEnum::Enum>& dataFileTypes)
{
    std::vector<CaretDataFile*> files;
    run(structure,
        dataFileTypes,
        files);
    
    return files;
}

/**
 * Add all caret data files (called by Brain).  This method will filter the
 * data files to meet the selection criteria.
 *
 * @param caretDataFiles
 *     All caret data files.
 */
void
EventCaretDataFilesGet::addAllCaretDataFiles(std::vector<CaretDataFile*>& caretDataFiles)
{
    m_caretDataFiles = caretDataFiles;
}

/**
 * @return All data files that support File Identification Attributes sorted by name
 */
std::vector<CaretDataFile*>
EventCaretDataFilesGet::getIdentifiableFilesSortedByName()
{
    std::vector<CaretDataFile*> allFiles(getAllCaretDataFiles());
    
    std::vector<CaretDataFile*> idFiles;
    for (CaretDataFile* cdf : allFiles) {
        if (cdf->getFileIdentificationAttributes()->isSupported()) {
            idFiles.push_back(cdf);
        }
    }
    
    std::sort(idFiles.begin(),
              idFiles.end(),
              [] (CaretDataFile* lhs, CaretDataFile* rhs) {
        const int result = lhs->getFileNameNoPath().compare(rhs->getFileNameNoPath(), Qt::CaseInsensitive);
        return (result < 0);
    } );

    return idFiles;
}

/**
 * Run the event to get files of the specified criteria.
 *
 * @param structure
 *     Structure.
 * @param dataFileTypes
 *     Type of data files.
 * @param caretDataFilesOut
 *     Output with caret data files matching selection criteria.
 */
void
EventCaretDataFilesGet::run(const StructureEnum::Enum structure,
                            const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                            std::vector<CaretDataFile*>& caretDataFilesOut)
{
    caretDataFilesOut.clear();
    
    EventCaretDataFilesGet instance(structure,
                                    dataFileTypes);
    EventManager::get()->sendEvent(instance.getPointer());
    
    /*
     * If data file types is empty it implies ALL file types
     */
    const bool allDataFileTypesFlag = dataFileTypes.empty();
    
    for (auto dataFile : instance.m_caretDataFiles) {
        /*
         * Match structure
         */
        if (structure != StructureEnum::ALL) {
            if (structure != dataFile->getStructure()) {
                continue;
            }
        }
        
        /*
         * Test data file type
         */
        if (allDataFileTypesFlag) {
            caretDataFilesOut.push_back(dataFile);
        }
        else {
            if (std::find(dataFileTypes.begin(),
                          dataFileTypes.end(),
                          dataFile->getDataFileType()) != dataFileTypes.end()) {
                caretDataFilesOut.push_back(dataFile);
            }
        }
    }
}


