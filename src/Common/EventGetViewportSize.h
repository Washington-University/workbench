#ifndef __EVENT_GET_VIEWPORT_SIZE_H__
#define __EVENT_GET_VIEWPORT_SIZE_H__

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


#include "Event.h"
#include "SpacerTabIndex.h"

namespace caret {

    class EventGetViewportSize : public Event {
        
    public:
        enum Mode {
            MODE_SPACER_TAB_INDEX,
            MODE_SURFACE_MONTAGE,
            MODE_TAB_BEFORE_MARGINS_INDEX,
            MODE_TAB_AFTER_MARGINS_INDEX,
            MODE_VOLUME_MONTAGE,
            MODE_WINDOW_INDEX,
        };
        
        EventGetViewportSize(const Mode mode,
                             const int32_t index);
        
        EventGetViewportSize(const SpacerTabIndex& spacerTabIndex);
        
        virtual ~EventGetViewportSize();
        
        Mode getMode() const;
        
        int32_t getIndex() const;
        
        bool isViewportSizeValid() const;
        
        void getViewportSize(int32_t viewportOut[4]) const;
        
        void setViewportSize(const int32_t viewport[4]);

        // ADD_NEW_METHODS_HERE

    private:
        EventGetViewportSize(const EventGetViewportSize&);

        EventGetViewportSize& operator=(const EventGetViewportSize&);
        
        const Mode m_mode;
        
        const int32_t m_index = -1;
        
        SpacerTabIndex m_spacerTabIndex;
        
        bool m_viewportValid = false;
        
        int32_t m_viewport[4];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_GET_VIEWPORT_SIZE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_GET_VIEWPORT_SIZE_DECLARE__

} // namespace
#endif  //__EVENT_GET_VIEWPORT_SIZE_H__
