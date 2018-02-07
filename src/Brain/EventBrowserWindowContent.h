#ifndef __EVENT_BROWSER_WINDOW_CONTENT_H__
#define __EVENT_BROWSER_WINDOW_CONTENT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

    class BrowserWindowContent;
    
    class EventBrowserWindowContent : public Event {
        
    public:
        enum class Mode {
            DELETE,
            GET,
            NEW
        };
        
        static std::unique_ptr<EventBrowserWindowContent> newWindowContent(const int32_t windowIndex);
        
        static std::unique_ptr<EventBrowserWindowContent> deleteWindowContent(const int32_t windowIndex);
        
        static std::unique_ptr<EventBrowserWindowContent> getWindowContent(const int32_t windowIndex);
        
        virtual ~EventBrowserWindowContent();

        Mode getMode() const;

        BrowserWindowContent* getBrowserWindowContent();
        
        void setBrowserWindowContent(BrowserWindowContent* browserWindowContent);
        
        int32_t getBrowserWindowIndex() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventBrowserWindowContent(const Mode mode,
                                  const int32_t browserWindowIndex);
        
        EventBrowserWindowContent(const EventBrowserWindowContent&);

        EventBrowserWindowContent& operator=(const EventBrowserWindowContent&);
        
        const Mode m_mode;
        
        const int32_t m_browserWindowIndex = -1;
        
        BrowserWindowContent* m_browserWindowContent = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_WINDOW_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_WINDOW_CONTENT_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_WINDOW_CONTENT_H__
