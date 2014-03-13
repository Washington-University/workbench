#ifndef __EVENT_BROWSER_WINDOW_NEW_H__
#define __EVENT_BROWSER_WINDOW_NEW_H__

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

class QWidget;

namespace caret {

    class BrainBrowserWindow;
    class BrowserTabContent;
    /// Create a new browser window
    class EventBrowserWindowNew : public Event {
        
    public:
        EventBrowserWindowNew();
        
        EventBrowserWindowNew(QWidget* parent,
                              BrowserTabContent* browserTabContent);
        
        virtual ~EventBrowserWindowNew();
        
        QWidget* getParent();
        
        BrowserTabContent* getBrowserTabContent() const;

        BrainBrowserWindow* getBrowserWindowCreated() const;
        
        void setBrowserWindowCreated(BrainBrowserWindow* browserWindowCreated);
        
    private:
        EventBrowserWindowNew(const EventBrowserWindowNew&);
        
        EventBrowserWindowNew& operator=(const EventBrowserWindowNew&);
        
        /** Widget used for placement of new window */
        QWidget* parent;
        
        /** If not NULL, contains tab for the new window */
        BrowserTabContent* browserTabContent;
        
        /** Window that was created. */
        BrainBrowserWindow* browserWindowCreated;
    };

} // namespace

#endif // __EVENT_BROWSER_WINDOW_NEW_H__
