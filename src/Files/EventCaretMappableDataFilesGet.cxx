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

#include <algorithm>
#include <typeinfo>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"

using namespace caret;

/**
 * Constructor for ALL map data files.
 */
EventCaretMappableDataFilesGet::EventCaretMappableDataFilesGet()
: Event(EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET),
m_mode(MODE_ANY_DATA_FILE_TYPE),
m_oneDataFileType(DataFileTypeEnum::UNKNOWN)
{
}

/**
 * Constructor for map data files of the given file type.
 *
 * @param dataFileType
 *    Type of data files requested.
 */
EventCaretMappableDataFilesGet::EventCaretMappableDataFilesGet(const DataFileTypeEnum::Enum dataFileType)
: Event(EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET),
m_mode(MODE_ONE_DATA_FILE_TYPE),
m_oneDataFileType(dataFileType)
{
}



/**
 * Destructor.
 */
EventCaretMappableDataFilesGet::~EventCaretMappableDataFilesGet()
{
    
}

/**
 * Add a map data file.
 * @param mapDataFile
 *    Map data file that is added.
 */
void 
EventCaretMappableDataFilesGet::addFile(CaretMappableDataFile* mapDataFile)
{
    CaretAssert(mapDataFile);
    if (mapDataFile->getNumberOfMaps() <= 0) {
        return;
    }
    
    const DataFileTypeEnum::Enum mapDataFileType = mapDataFile->getDataFileType();
    
    /*
     * No surface files
     */
    if (mapDataFileType == DataFileTypeEnum::SURFACE) {
        return;
    }
    
    /*
     * Based upon mode, perform additional filtering of file data type
     */
    switch (m_mode) {
        case MODE_ONE_DATA_FILE_TYPE:
            if (mapDataFileType != m_oneDataFileType) {
                return;
            }
            break;
        case MODE_ANY_DATA_FILE_TYPE:
            break;
    }

    m_allCaretMappableDataFiles.push_back(mapDataFile);
}

/** 
 * Get all map data files.
 *
 * @param allFilesOut
 *    All map data files output.
 */
void 
EventCaretMappableDataFilesGet::getAllFiles(std::vector<CaretMappableDataFile*>& allFilesOut) const
{
    allFilesOut = m_allCaretMappableDataFiles;
}

/**
 * Get all map data files sorted by name of file
 *
 * @param allFilesOut
 *    All map data files output.
 */
void
EventCaretMappableDataFilesGet::getAllFilesSortedByName(std::vector<CaretMappableDataFile*>& allFilesOut) const
{
    allFilesOut = m_allCaretMappableDataFiles;
    
    std::sort(allFilesOut.begin(),
              allFilesOut.end(),
              [] (CaretMappableDataFile* lhs, CaretMappableDataFile* rhs) {
                  const int result = lhs->getFileNameNoPath().compare(rhs->getFileNameNoPath(), Qt::CaseInsensitive);
                  return (result < 0);
              } );
}



