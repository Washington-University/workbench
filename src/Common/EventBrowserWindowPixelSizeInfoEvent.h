#ifndef __EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO_EVENT_H__
#define __EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO_EVENT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

    class EventBrowserWindowPixelSizeInfoEvent : public Event {
        
    public:
        enum class Mode {
            WIDGET_POINTER,
            WINDOW_INDEX
        };
        
        EventBrowserWindowPixelSizeInfoEvent(QWidget* widget);
        
        EventBrowserWindowPixelSizeInfoEvent(const int32_t windowIndex);

        virtual ~EventBrowserWindowPixelSizeInfoEvent();
        
        EventBrowserWindowPixelSizeInfoEvent(const EventBrowserWindowPixelSizeInfoEvent&) = delete;

        EventBrowserWindowPixelSizeInfoEvent& operator=(const EventBrowserWindowPixelSizeInfoEvent&) = delete;
        
        Mode getMode() const;
        
        QWidget* getWidget() const;
        
        int32_t getWindowIndex() const;

        float getLogicalDotsPerInch() const;
        
        void setLogicalDotsPerInch(const float logicalDotsPerInch);
        
        float getPhysicalDotsPerInch() const;
        
        void setPhysicalDotsPerInch(const float physicalDotsPerInch);
        
        // ADD_NEW_METHODS_HERE

    private:
        const Mode m_mode;
        
        QWidget* m_widget = NULL;
        
        int32_t m_windowIndex = -1;
        
        float m_logicalDotsPerInch = -1.0;
        
        float m_physicalDotsPerInch = -1.0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO_EVENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO_EVENT_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO_EVENT_H__
