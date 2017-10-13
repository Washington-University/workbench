#ifndef __EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION_H__
#define __EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

class QWidget;

namespace caret {

    class EventBrowserWindowTileTabOperation : public Event {
        
    public:
        static void selectTabInWindow(QWidget* parentWidget,
                                      const int32_t windowIndex,
                                      const int32_t browserTabIndex);
        enum Operation {
            OPERATION_NEW_TAB_AFTER,
            OPERATION_NEW_TAB_BEFORE,
            OPERATION_SELECT_TAB
        };
        
        EventBrowserWindowTileTabOperation(const Operation operation,
                                           QWidget* parentWidget,
                                           const int32_t windowIndex,
                                           const int32_t browserTabIndex);
        
        virtual ~EventBrowserWindowTileTabOperation();

        Operation getOperation() const;

        int32_t getWindowIndex() const;
        
        int32_t getBrowserTabIndex() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventBrowserWindowTileTabOperation(const EventBrowserWindowTileTabOperation&);

        EventBrowserWindowTileTabOperation& operator=(const EventBrowserWindowTileTabOperation&);
        
        const Operation m_operation;
        
        QWidget* m_parentWidget;
        
        const int32_t m_windowIndex;
        
        const int32_t m_browserTabIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION_H__
