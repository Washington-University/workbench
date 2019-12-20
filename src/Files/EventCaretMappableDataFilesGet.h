#ifndef __EVENT_CARET_MAPPABLE_DATA_FILES_GET_H__
#define __EVENT_CARET_MAPPABLE_DATA_FILES_GET_H__

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

#include "Event.h"

namespace caret {

    class CaretMappableDataFile;
    
    /// Event that gets all caret mappable data files.
    class EventCaretMappableDataFilesGet : public Event {
        
    public:
        EventCaretMappableDataFilesGet();
        
        EventCaretMappableDataFilesGet(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~EventCaretMappableDataFilesGet();
        
        void addFile(CaretMappableDataFile* mapDataFile);        
        
        void getAllFiles(std::vector<CaretMappableDataFile*>& allFilesOut) const;
        
        void getAllFilesSortedByName(std::vector<CaretMappableDataFile*>& allFilesOut) const;
        
    private:
        enum Mode {
            MODE_ANY_DATA_FILE_TYPE,
            MODE_ONE_DATA_FILE_TYPE
        };
        
        EventCaretMappableDataFilesGet(const EventCaretMappableDataFilesGet&);
        
        EventCaretMappableDataFilesGet& operator=(const EventCaretMappableDataFilesGet&);

        const Mode m_mode;
        
        const DataFileTypeEnum::Enum m_oneDataFileType;
        
        std::vector<CaretMappableDataFile*> m_allCaretMappableDataFiles;
        
    };

} // namespace

#endif // __EVENT_CARET_MAPPABLE_DATA_FILES_GET_H__
