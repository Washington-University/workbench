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


#include "CaretObject.h"

#include "StructureEnum.h"

namespace caret {

    
    class SpecFileDataFile : public CaretObject {
        
    public:
        SpecFileDataFile(const AString& filename,
                          const StructureEnum::Enum structure);
        
        virtual ~SpecFileDataFile();
        
        SpecFileDataFile(const SpecFileDataFile& sfdf);
        
        SpecFileDataFile& operator=(const SpecFileDataFile& sfdf);
        
        AString getFileName() const;
        
        StructureEnum::Enum getStructure() const;
        
        bool isSelected() const;
        
        void setSelected(const bool selected);
        
        bool isRemovedFromSpecFileWhenWritten() const;
        
        void setRemovedFromSpecFileWhenWritten(const bool removeIt);
        
    public:
        virtual AString toString() const;
        
    private:
        void copyHelper(const SpecFileDataFile& sfdf);
        
        AString filename;
        
        StructureEnum::Enum structure;
        
        bool selected;
        
        bool removeFromSpecFileWhenWritten;
    };
    
#ifdef __SPEC_FILE_GROUP_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPEC_FILE_GROUP_FILE_DECLARE__

} // namespace

#endif // __SPEC_FILE_DATA_FILE_H__

