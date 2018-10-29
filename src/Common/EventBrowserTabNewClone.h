#ifndef __EVENT_BROWSER_TAB_NEW_CLONE_H__
#define __EVENT_BROWSER_TAB_NEW_CLONE_H__

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

    class BrowserTabContent;
    
    class EventBrowserTabNewClone : public Event {
        
    public:
        EventBrowserTabNewClone(const int32_t indexOfBrowserTabThatWillBeCloned);
        
        virtual ~EventBrowserTabNewClone();
        
        EventBrowserTabNewClone(const EventBrowserTabNewClone&) = delete;

        EventBrowserTabNewClone& operator=(const EventBrowserTabNewClone&) = delete;
        
        BrowserTabContent* getNewBrowserTab() const;
        
        void setNewBrowserTab(BrowserTabContent* newBrowserTab,
                           const int32_t newBrowserTabIndex);
        
        int32_t getNewBrowserTabIndex() const;
        
        int32_t getIndexOfBrowserTabThatWasCloned() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

        const int32_t m_indexOfBrowserTabThatWasClonded = -1;
        
        BrowserTabContent* m_newBrowserTabContent = NULL;
        
        int32_t m_newBrowserTabIndex = -1;
        
    };
    
#ifdef __EVENT_BROWSER_TAB_NEW_CLONE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_TAB_NEW_CLONE_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_TAB_NEW_CLONE_H__
