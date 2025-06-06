#ifndef __SPEC_FILE_DATA_FILE_TYPE_GROUP_H__
#define __SPEC_FILE_DATA_FILE_TYPE_GROUP_H__

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


#include "CaretObjectTracksModification.h"
#include "DataFileTypeEnum.h"

namespace caret {

    class SpecFileDataFile;
    
    class SpecFileDataFileTypeGroup : public CaretObjectTracksModification {
        
    public:
        SpecFileDataFileTypeGroup(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~SpecFileDataFileTypeGroup();
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        int32_t getNumberOfFiles() const;
        
        int32_t getNumberOfFilesSelectedForLoading() const;
        
        int32_t getNumberOfFilesSelectedForSaving() const;
        
        void addFileInformation(SpecFileDataFile* fileInformation);
        
        void removeFileInformation(const int32_t fileIndex);
        
        void removeAllFileInformation();
        
        void removeFileInformationIfNotInSpecAndNoCaretDataFile();
        
        SpecFileDataFile* getFileInformation(const int32_t fileIndex);
        
        const SpecFileDataFile* getFileInformation(const int32_t fileIndex) const;
        
        void setAllFilesSelectedForLoading(bool selectionStatus);
        
        void setAllFilesSelectedForSaving(bool selectionStatus);
        
        void setAllFilesSpecFileMemberStatus(bool memberStatus);
        
        void setModifiedFilesSelectedForSaving();
        
        virtual void clearModified();
        
        virtual bool isModified() const;
        
    private:
        SpecFileDataFileTypeGroup(const SpecFileDataFileTypeGroup&);

        SpecFileDataFileTypeGroup& operator=(const SpecFileDataFileTypeGroup&);
        
    public:
        virtual AString toString() const;
        
    private:
        std::vector<SpecFileDataFile*> files;
        
        DataFileTypeEnum::Enum dataFileType;
        
        
    };
    
#ifdef __SPEC_FILE_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPEC_FILE_GROUP_DECLARE__

} // namespace

#endif //  __SPEC_FILE_DATA_FILE_TYPE_GROUP_H__

