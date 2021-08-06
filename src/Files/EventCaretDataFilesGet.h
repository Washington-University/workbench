#ifndef __EVENT_CARET_DATA_FILES_GET_H__
#define __EVENT_CARET_DATA_FILES_GET_H__

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



#include <memory>

#include "DataFileTypeEnum.h"
#include "Event.h"
#include "StructureEnum.h"



namespace caret {
    class BorderFile;
    class CaretDataFile;
    
    class EventCaretDataFilesGet : public Event {
        
    public:
        virtual ~EventCaretDataFilesGet();

        static std::vector<CaretDataFile*> getAllCaretDataFiles();
        
        static std::vector<DataFileTypeEnum::Enum> getAllCaretDataFileTyes();
        
        static std::vector<CaretDataFile*> getCaretDataFilesForType(const DataFileTypeEnum::Enum dataFileType);

        static std::vector<CaretDataFile*> getCaretDataFilesForTypes(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes);
        
        static std::vector<CaretDataFile*> getCaretDataFilesForStructureAndTypes(const StructureEnum::Enum structure,
                                                                                 const std::vector<DataFileTypeEnum::Enum>& dataFileTypes);
        
        void addAllCaretDataFiles(std::vector<CaretDataFile*>& caretDataFiles);
        
        // ADD_NEW_METHODS_HERE

    private:
        EventCaretDataFilesGet(const StructureEnum::Enum structure,
                               const std::vector<DataFileTypeEnum::Enum>& dataFileTypes);
        
        EventCaretDataFilesGet(const EventCaretDataFilesGet&);

        EventCaretDataFilesGet& operator=(const EventCaretDataFilesGet&);
        
        static void run(const StructureEnum::Enum structure,
                        const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                        std::vector<CaretDataFile*>& caretDataFilesOut);
        
        const StructureEnum::Enum m_structure;
        
        const std::vector<DataFileTypeEnum::Enum>& m_dataFileTypes;
        
        std::vector<CaretDataFile*> m_caretDataFiles;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CARET_DATA_FILES_GET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CARET_DATA_FILES_GET_DECLARE__

} // namespace
#endif  //__EVENT_CARET_DATA_FILES_GET_H__
