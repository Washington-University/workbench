#ifndef __EVENT_BROWSER_WINDOW_CONTENT_GET_H__
#define __EVENT_BROWSER_WINDOW_CONTENT_GET_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "Event.h"

namespace caret {

    class BrowserTabContent;
    class Model;
    class TileTabsConfiguration;
    
    /// Get the content of a browser window
    class EventBrowserWindowContentGet : public Event {
        
    public:
        EventBrowserWindowContentGet(const int32_t browserWindowIndex);
        
        virtual ~EventBrowserWindowContentGet();
        
        int32_t getBrowserWindowIndex() const;
        
        int32_t getNumberOfItemsToDraw() const;
        
        void addTabContentToDraw(BrowserTabContent* browserTabContent);
        
        BrowserTabContent* getTabContentToDraw(const int32_t itemIndex);
        
        void setTabIndexForTileTabsHighlighting(const int32_t tabIndex);
        
        int32_t getTabIndexForTileTabsHighlighting() const;
        
        TileTabsConfiguration* getTileTabsConfiguration() const;
        
        void setTileTabsConfiguration(TileTabsConfiguration* tileTabsConfiguration);
        
    private:
        EventBrowserWindowContentGet(const EventBrowserWindowContentGet&);
        
        EventBrowserWindowContentGet& operator=(const EventBrowserWindowContentGet&);
        
        /** index of browswer window */
        int32_t m_browserWindowIndex;
        
        /** Tab content that are to be drawn in the window */
        std::vector<BrowserTabContent*> browserTabContents;
        
        /** Index of tab that is highlighted in Tile Tabs mode */
        int32_t m_tabIndexForTileTabsHighlighting;
        
        /** Selected tile tabs configuration when more than one item to draw */
        TileTabsConfiguration* m_tileTabsConfiguration;
    };

} // namespace

#endif // __EVENT_BROWSER_WINDOW_CONTENT_GET_H__
