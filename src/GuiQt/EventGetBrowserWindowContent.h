#ifndef __EVENT_GET_BROWSER_WINDOW_CONTENT_H__
#define __EVENT_GET_BROWSER_WINDOW_CONTENT_H__

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
    class ModelDisplayController;
    
    /// Get the content of a browser window
    class EventGetBrowserWindowContent : public Event {
        
    public:
        EventGetBrowserWindowContent(const int32_t browserWindowIndex);
        
        virtual ~EventGetBrowserWindowContent();
        
        int32_t getBrowserWindowIndex() const;
        
        int32_t getWindowTabNumber() const;
        
        ModelDisplayController* getModelDisplayController();
        
        void setWindowTabNumber(const int32_t windowTabNumber);
        
        void setModelDisplayController(ModelDisplayController* modelDisplayController);
        
        BrowserTabContent* getBrowserTabContent();
        
        void setBrowserTabContent(BrowserTabContent* browserTabContent);
        
    private:
        EventGetBrowserWindowContent(const EventGetBrowserWindowContent&);
        
        EventGetBrowserWindowContent& operator=(const EventGetBrowserWindowContent&);
        
        /** index of browswer window */
        int32_t browserWindowIndex;
        
        /** Model Display Controller for drawing */
        ModelDisplayController* modelDisplayController;
        
        /** Window tab number */
        int32_t windowTabNumber;
        
        /** Tab content active in window */
        BrowserTabContent* browserTabContent;
    };

} // namespace

#endif // __EVENT_GET_BROWSER_WINDOW_CONTENT_H__
