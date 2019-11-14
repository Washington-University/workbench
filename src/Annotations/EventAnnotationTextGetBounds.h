#ifndef __EVENT_ANNOTATION_TEXT_GET_BOUNDS_H__
#define __EVENT_ANNOTATION_TEXT_GET_BOUNDS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

    class AnnotationText;
    
    class EventAnnotationTextGetBounds : public Event {
        
    public:
        EventAnnotationTextGetBounds(const AnnotationText& annotationText,
                                     const int32_t viewportWidthForPercentageHeightText,
                                     const int32_t viewportHeightForPercentageHeightText);
        
        const AnnotationText& getAnnotationText() const;
        
        int32_t getViewportWidth() const;
        
        int32_t getViewportHeight() const;
        
        float getTextWidth() const;
        
        float getTextHeight() const;
        
        void setTextWidthHeight(const float textWidth,
                                const float textHeight);
        
        bool isValid() const;
        
        virtual ~EventAnnotationTextGetBounds();
        
        EventAnnotationTextGetBounds(const EventAnnotationTextGetBounds&) = delete;

        EventAnnotationTextGetBounds& operator=(const EventAnnotationTextGetBounds&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        const AnnotationText& m_annotationText;
        
        const int32_t m_viewportWidth;
        
        const int32_t m_viewportHeight;
        
        float m_textWidth = 0.0;
        
        float m_textHeight = 0.0;
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_TEXT_GET_BOUNDS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_TEXT_GET_BOUNDS_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_TEXT_GET_BOUNDS_H__
