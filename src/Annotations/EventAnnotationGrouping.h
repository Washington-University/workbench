#ifndef __EVENT_ANNOTATION_GROUPING_H__
#define __EVENT_ANNOTATION_GROUPING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include "AnnotationGroupKey.h"
#include "Event.h"



namespace caret {

    class Annotation;
    
    class EventAnnotationGrouping : public Event {
        
    public:
        enum Mode {
            MODE_INVALID,
            MODE_GROUP,
            MODE_REGROUP,
            MODE_UNGROUP
        };
        
        EventAnnotationGrouping();
        
        virtual ~EventAnnotationGrouping();
        
        void setModeGroupAnnotations(const AnnotationGroupKey spaceGroupKey,
                                     std::vector<Annotation*>& annotations);

        void setModeUngroupAnnotations(const AnnotationGroupKey userGroupKey,
                                       std::vector<Annotation*>& annotations);
        
        Mode getMode() const;
        
        AnnotationGroupKey getAnnotationGroupKey() const;
        
        std::vector<Annotation*> getAnnotations() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        
        EventAnnotationGrouping(const EventAnnotationGrouping&);

        EventAnnotationGrouping& operator=(const EventAnnotationGrouping&);
        
        Mode m_mode;
        
        AnnotationGroupKey m_annotationGroupKey;
        
        std::vector<Annotation*> m_annotations;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_GROUPING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_GROUPING_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_GROUPING_H__
