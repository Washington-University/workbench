#ifndef __EVENT_ANNOTATION_H__
#define __EVENT_ANNOTATION_H__

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

    class Annotation;
    
    class EventAnnotation : public Event {
        
    public:
        /**
         * Mode of annotation event
         */
        enum Mode {
            /** Invalid */
            MODE_INVALID,
            /** Begin creation of a new annotation with a specific type MOUSE MODE MUST BE Annotations */
            MODE_CREATE_NEW_ANNOTATION_TYPE,
            /** Delete an annotation */
            MODE_DELETE_ANNOTATION,
            /** Deselect all annotations */
            MODE_DESELECT_ALL_ANNOTATIONS,
            /** Edit an annotation */
            MODE_EDIT_ANNOTATION,
        };
   
        EventAnnotation();
        
        virtual ~EventAnnotation();
        
        EventAnnotation& setModeEditAnnotation(const int32_t browserWindowIndex,
                                   Annotation* annotation);
        
        void getModeEditAnnotation(int32_t& browserWindowIndexOut,
                                   Annotation* & annotationOut) const;
        
        EventAnnotation& setModeDeselectAllAnnotations();
        
        AnnotationTypeEnum::Enum getModeCreateNewAnnotationType() const;
        
        EventAnnotation& setModeCreateNewAnnotationType(const AnnotationTypeEnum::Enum annotationType);
        
        const Annotation* getModeDeleteAnnotation() const;
        
        EventAnnotation& setModeDeleteAnnotation(Annotation* annotation);
        
        Mode getMode() const;

        // ADD_NEW_METHODS_HERE

    private:
        EventAnnotation(const EventAnnotation&);

        EventAnnotation& operator=(const EventAnnotation&);
        
        Mode m_mode;
        
        int32_t m_browserWindowIndex;
        
        Annotation* m_annotation;
        
        AnnotationTypeEnum::Enum m_annotationType;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_H__
