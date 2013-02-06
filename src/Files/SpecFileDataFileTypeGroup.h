#ifndef __SPEC_FILE_DATA_FILE_TYPE_GROUP_H__
#define __SPEC_FILE_DATA_FILE_TYPE_GROUP_H__

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
        
        int32_t getNumberOfFilesSelected() const;
        
        void addFileInformation(SpecFileDataFile* fileInformation);
        
        void removeFileInformation(const int32_t fileIndex);
        
        void removeAllFileInformation();
        
        SpecFileDataFile* getFileInformation(const int32_t fileIndex);
        
        const SpecFileDataFile* getFileInformation(const int32_t fileIndex) const;
        
        void setAllFilesSelected(bool selectionStatus);
        
        void removeFilesTaggedForRemoval();
    
        bool hasBeenEdited() const;
        
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

