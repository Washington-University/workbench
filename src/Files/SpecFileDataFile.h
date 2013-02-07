#ifndef __SPEC_FILE_DATA_FILE_H__
#define __SPEC_FILE_DATA_FILE_H__

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "CaretDataFile.h"
#include "CaretObjectTracksModification.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"

namespace caret {

    
    class SpecFileDataFile : public CaretObjectTracksModification {
        
    public:
        SpecFileDataFile(const AString& filename,
                         const DataFileTypeEnum::Enum dataFileType,
                         const StructureEnum::Enum structure,
                         const bool specFileMember);
        
        virtual ~SpecFileDataFile();
        
        SpecFileDataFile(const SpecFileDataFile& sfdf);
        
        SpecFileDataFile& operator=(const SpecFileDataFile& sfdf);
        
        AString getFileName() const;
        
        void setFileName(const AString& fileName);
        
        CaretDataFile* getCaretDataFile();
        
        void setCaretDataFile(CaretDataFile* caretDataFile);
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        bool isSelected() const;
        
        void setSelected(const bool selected);
        
        bool isRemovedFromSpecFileWhenWritten() const;
        
        void setRemovedFromSpecFileWhenWritten(const bool removeIt);
        
        bool hasBeenEdited() const;
        
        bool isSpecFileMember() const;
        
        void setSpecFileMember(const bool status);
        
    public:
        virtual AString toString() const;
        
    private:
        SpecFileDataFile(); // not implemented
        
        void copyHelper(const SpecFileDataFile& sfdf);
        
        mutable AString m_filename;
        
        CaretDataFile* m_caretDataFile;
        
        mutable StructureEnum::Enum m_structure;
        
        StructureEnum::Enum m_originalStructure;
        
        DataFileTypeEnum::Enum m_dataFileType;
        
        bool m_selected;
        
        bool m_removeFromSpecFileWhenWritten;
        
        bool m_specFileMember;
    };
    
#ifdef __SPEC_FILE_GROUP_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPEC_FILE_GROUP_FILE_DECLARE__

} // namespace

#endif // __SPEC_FILE_DATA_FILE_H__

