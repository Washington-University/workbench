#ifndef __EVENT_DATA_FILE_READ_H__
#define __EVENT_DATA_FILE_READ_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include "DataFileTypeEnum.h"
#include "Event.h"
#include "StructureEnum.h"

namespace caret {

    class Brain;
    
    /**
     * Event for reading a data file.
     */
    class EventDataFileRead : public Event {
        
    public:
        EventDataFileRead(Brain* loadIntoBrain,
                          const DataFileTypeEnum::Enum dataFileType,
                          const AString& dataFileName);
        
        EventDataFileRead(Brain* loadIntoBrain,
                          const StructureEnum::Enum structure,
                          const DataFileTypeEnum::Enum dataFileType,
                          const AString& dataFileName);
        
        virtual ~EventDataFileRead();
        
        AString getDataFileName() const;
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        Brain* getLoadIntoBrain();
        
        StructureEnum::Enum getStructure() const;
        
        bool isErrorInvalidStructure() const;
        
        void setErrorInvalidStructure(const bool status);
        
        AString getUsername() const;
        
        AString getPassword() const;
        
        void setUsernameAndPassword(const AString& username,
                                    const AString& password);
        
    private:
        EventDataFileRead(const EventDataFileRead&);
        
        EventDataFileRead& operator=(const EventDataFileRead&);
        
        Brain* loadIntoBrain;
        
        AString dataFileName;
        
        DataFileTypeEnum::Enum dataFileType;
        
        StructureEnum::Enum structure;
        
        AString username;
        
        AString password;
        
        bool errorInvalidStructure;
        
    };

} // namespace

#endif // __EVENT_DATA_FILE_READ_H__
