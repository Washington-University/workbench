#ifndef __EVENT_BROWSER_TAB_GET_VIEWPORT_SIZE_H__
#define __EVENT_BROWSER_TAB_GET_VIEWPORT_SIZE_H__

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



namespace caret {

    class EventBrowserTabGetViewportSize : public Event {
        
    public:
        enum Mode {
            MODE_SURFACE_MONTAGE,
            MODE_TAB_INDEX,
            MODE_VOLUME_MONTAGE
        };
        EventBrowserTabGetViewportSize(const int32_t tabIndex);
        
        EventBrowserTabGetViewportSize(const Mode mode);
        
        virtual ~EventBrowserTabGetViewportSize();
        
        Mode getMode() const;
        
        int32_t getTabIndex() const;
        
        bool isViewportSizeValid() const;
        
        void getViewportSize(int32_t viewportOut[4]) const;
        
        void setViewportSize(const int32_t viewport[4]);

        // ADD_NEW_METHODS_HERE

    private:
        EventBrowserTabGetViewportSize(const EventBrowserTabGetViewportSize&);

        EventBrowserTabGetViewportSize& operator=(const EventBrowserTabGetViewportSize&);
        
        const Mode m_mode;
        
        const int32_t m_tabIndex;
        
        int32_t m_viewport[4];
        
        bool m_viewportValid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_TAB_GET_VIEWPORT_SIZE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_TAB_GET_VIEWPORT_SIZE_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_TAB_GET_VIEWPORT_SIZE_H__
