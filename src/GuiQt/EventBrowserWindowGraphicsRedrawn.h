#ifndef __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_H__
#define __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

    class BrainBrowserWindow;
    
    class EventBrowserWindowGraphicsRedrawn : public Event {
        
    public:
        EventBrowserWindowGraphicsRedrawn(BrainBrowserWindow* brainBrowserWindow);
        
        virtual ~EventBrowserWindowGraphicsRedrawn();
        
        BrainBrowserWindow* getBrowserWindow() const;
        
        int32_t getBrowserWindowIndex() const;
        
    private:
        EventBrowserWindowGraphicsRedrawn(const EventBrowserWindowGraphicsRedrawn&);

        EventBrowserWindowGraphicsRedrawn& operator=(const EventBrowserWindowGraphicsRedrawn&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        BrainBrowserWindow* m_brainBrowserWindow;
        
        int32_t m_brainBrowserWindowIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_WINDOW_GRAPHICS_REDRAWN_H__
