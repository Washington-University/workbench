#ifndef __EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR_H__
#define __EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR_H__

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

    class EventDrawingViewportContentClear : public Event {
        
    public:
        EventDrawingViewportContentClear(const int32_t windowIndex);
        
        virtual ~EventDrawingViewportContentClear();
        
        EventDrawingViewportContentClear(const EventDrawingViewportContentClear&) = delete;

        EventDrawingViewportContentClear& operator=(const EventDrawingViewportContentClear&) = delete;
        
        int32_t getWindowIndex() const;

        // ADD_NEW_METHODS_HERE

    private:
        const int32_t m_windowIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR_DECLARE__

} // namespace
#endif  //__EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR_H__
