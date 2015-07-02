#ifndef __EVENT_ANNOTATION_CREATE_NEW_TYPE_H__
#define __EVENT_ANNOTATION_CREATE_NEW_TYPE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "AnnotationTypeEnum.h"
#include "Event.h"



namespace caret {

    class EventAnnotationCreateNewType : public Event {
        
    public:
        EventAnnotationCreateNewType(const AnnotationTypeEnum::Enum annotationType);
        
        virtual ~EventAnnotationCreateNewType();
        
        AnnotationTypeEnum::Enum getAnnotationType() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventAnnotationCreateNewType(const EventAnnotationCreateNewType&);

        EventAnnotationCreateNewType& operator=(const EventAnnotationCreateNewType&);
        
        const AnnotationTypeEnum::Enum m_annotationType;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_CREATE_NEW_TYPE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_CREATE_NEW_TYPE_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_CREATE_NEW_TYPE_H__
