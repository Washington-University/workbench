#ifndef __SPEC_FILE_DATA_FILE_H__
#define __SPEC_FILE_DATA_FILE_H__

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
        
        bool isLoadingSelected() const;
        
        void setLoadingSelected(const bool selected);
        
        bool isSavingSelected() const;
        
        void setSavingSelected(const bool selected);
        
        bool isSpecFileMember() const;
        
        void setSpecFileMember(const bool status);
        
        bool isRemoveWhenSavingToScene() const;
        
        void setRemoveWhenSavingToScene(const bool status);
        
        bool exists() const;
        
    public:
        virtual AString toString() const;
        
    private:
        SpecFileDataFile(); // not implemented
        
        void copyHelper(const SpecFileDataFile& sfdf);
        
        mutable AString m_filename;
        
        CaretDataFile* m_caretDataFile;
        
        mutable StructureEnum::Enum m_structure;
        
        DataFileTypeEnum::Enum m_dataFileType;
        
        bool m_loadingSelected;
        
        bool m_savingSelected;
        
        bool m_specFileMember;
        
        bool m_removeWhenSavingToSceneFlag;
    };
    
#ifdef __SPEC_FILE_GROUP_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPEC_FILE_GROUP_FILE_DECLARE__

} // namespace

#endif // __SPEC_FILE_DATA_FILE_H__

