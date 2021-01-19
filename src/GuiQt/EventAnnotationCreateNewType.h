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

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationTypeEnum.h"
#include "Event.h"



namespace caret {

    class AnnotationFile;
    
    class EventAnnotationCreateNewType : public Event {
        
    public:
        enum PolyLineDrawingMode {
            CONTINUOUS,
            DISCRETE
        };
        
        EventAnnotationCreateNewType(AnnotationFile* annotationFile,
                                     const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                     const AnnotationTypeEnum::Enum annotationType,
                                     const PolyLineDrawingMode polyLineDrawingMode);
        
        virtual ~EventAnnotationCreateNewType();
        
        AnnotationFile* getAnnotationFile() const;
        
        AnnotationCoordinateSpaceEnum::Enum getAnnotationSpace() const;
        
        AnnotationTypeEnum::Enum getAnnotationType() const;
        
        PolyLineDrawingMode getPolyLineDrawingMode() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventAnnotationCreateNewType(const EventAnnotationCreateNewType&);

        EventAnnotationCreateNewType& operator=(const EventAnnotationCreateNewType&);
        
        AnnotationFile* m_annotationFile;
        
        const AnnotationCoordinateSpaceEnum::Enum m_annotationSpace;
        
        const AnnotationTypeEnum::Enum m_annotationType;

        const PolyLineDrawingMode m_polyLineDrawingMode;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_CREATE_NEW_TYPE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_CREATE_NEW_TYPE_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_CREATE_NEW_TYPE_H__
