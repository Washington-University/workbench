#ifndef __EVENT_BROWSER_WINDOW_DRAWING_CONTENT_GET_H__
#define __EVENT_BROWSER_WINDOW_DRAWING_CONTENT_GET_H__

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

    class BrowserTabContent;
    class BrowserWindowContent;
    class Model;
    class TileTabsConfiguration;
    
    /// Get the content of a browser window
    class EventBrowserWindowDrawingContent : public Event {
        
    public:
        EventBrowserWindowDrawingContent(const int32_t browserWindowIndex);
        
        virtual ~EventBrowserWindowDrawingContent();
        
        int32_t getBrowserWindowIndex() const;
        
        int32_t getNumberOfBrowserTabs() const;
        
        void addBrowserTab(BrowserTabContent* browserTabContent);
        
        BrowserTabContent* getBrowserTab(const int32_t itemIndex);
        
        void setTabIndexForTileTabsHighlighting(const int32_t tabIndex);
        
        int32_t getTabIndexForTileTabsHighlighting() const;
        
        BrowserWindowContent* getBrowserWindowContent();
        
        const BrowserWindowContent* getBrowserWindowContent() const;
        
        void setBrowserWindowContent(BrowserWindowContent* browserWindowContent);
        
        BrowserTabContent* getSelectedBrowserTabContent();
        
        void setSelectedBrowserTabContent(BrowserTabContent* browserTabContent);
        
    private:
        EventBrowserWindowDrawingContent(const EventBrowserWindowDrawingContent&);
        
        EventBrowserWindowDrawingContent& operator=(const EventBrowserWindowDrawingContent&);
        
        BrowserTabContent* m_selectedBrowserTabContent;
        
        /** index of browswer window */
        int32_t m_browserWindowIndex;
        
        /** all browser tabs in the window */
        std::vector<BrowserTabContent*> browserTabContents;
        
        /** content of browser window */
        BrowserWindowContent* m_browserWindowContent;
        
        /** Index of tab that is highlighted in Tile Tabs mode */
        int32_t m_tabIndexForTileTabsHighlighting;
    };

} // namespace

#endif // __EVENT_BROWSER_WINDOW_DRAWING_CONTENT_GET_H__
