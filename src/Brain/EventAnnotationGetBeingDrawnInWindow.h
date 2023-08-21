#ifndef __EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW_H__
#define __EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW_H__

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



namespace caret {

    class Annotation;
    
    class EventAnnotationGetBeingDrawnInWindow : public Event {
        
    public:
        EventAnnotationGetBeingDrawnInWindow(const int32_t browserWindowIndex);
        
        virtual ~EventAnnotationGetBeingDrawnInWindow();
        
        EventAnnotationGetBeingDrawnInWindow(const EventAnnotationGetBeingDrawnInWindow&) = delete;

        EventAnnotationGetBeingDrawnInWindow& operator=(const EventAnnotationGetBeingDrawnInWindow&) = delete;
        
        int32_t getBrowserWindowIndex() const;
        
        Annotation* getAnnotation() const;

        int32_t getDrawingViewportHeight() const;
        
        void setAnnotation(Annotation* annotation,
                           const int32_t drawingViewportHeight);
        
        // ADD_NEW_METHODS_HERE

    private:
        const int32_t m_browserWindowIndex;
        
        Annotation* m_annotation = NULL;
        
        int32_t m_drawingViewportHeight = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_GET_BEING_DRAWN_IN_WINDOW_H__
