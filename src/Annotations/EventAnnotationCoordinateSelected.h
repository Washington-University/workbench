#ifndef __EVENT_ANNOTATION_COORDINATE_SELECTED_H__
#define __EVENT_ANNOTATION_COORDINATE_SELECTED_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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



namespace caret {

    class Annotation;
    
    class EventAnnotationCoordinateSelected : public Event {
        
    public:
        EventAnnotationCoordinateSelected(const Annotation* annotation,
                                          const int32_t coordinateIndex);
        
        virtual ~EventAnnotationCoordinateSelected();
        
        EventAnnotationCoordinateSelected(const EventAnnotationCoordinateSelected&) = delete;

        EventAnnotationCoordinateSelected& operator=(const EventAnnotationCoordinateSelected&) = delete;
        
        const Annotation* getAnnotation() const;
        
        int32_t getCoordinateIndex() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        const Annotation* m_annotation;

        const int32_t m_coordinateIndex;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_COORDINATE_SELECTED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_COORDINATE_SELECTED_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_COORDINATE_SELECTED_H__
