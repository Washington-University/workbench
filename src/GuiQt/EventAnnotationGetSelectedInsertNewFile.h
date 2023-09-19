#ifndef __EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE_H__
#define __EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include "Event.h"
#include "UserInputModeEnum.h"


namespace caret {

    class AnnotationFile;
    
    class EventAnnotationGetSelectedInsertNewFile : public Event {
        
    public:
        EventAnnotationGetSelectedInsertNewFile(const UserInputModeEnum::Enum userInputMode);
        
        virtual ~EventAnnotationGetSelectedInsertNewFile();
        
        UserInputModeEnum::Enum getUserInputMode() const;
        
        AnnotationFile* getAnnotationFile() const;
        
        void setAnnotationFile(AnnotationFile* annotationFile);
        
        EventAnnotationGetSelectedInsertNewFile(const EventAnnotationGetSelectedInsertNewFile&) = delete;

        EventAnnotationGetSelectedInsertNewFile& operator=(const EventAnnotationGetSelectedInsertNewFile&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        const UserInputModeEnum::Enum m_userInputMode;
        
        AnnotationFile* m_annotationFile = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE_H__
