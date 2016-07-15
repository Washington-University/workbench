#ifndef __EVENT_BROWSER_TAB_GET_ALL_VIEWED_H__
#define __EVENT_BROWSER_TAB_GET_ALL_VIEWED_H__

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
#include "StructureEnum.h"

namespace caret {
    class BrowserTabContent;
    
    class EventBrowserTabGetAllViewed : public Event {
        
    public:
        EventBrowserTabGetAllViewed();
        
        virtual ~EventBrowserTabGetAllViewed();
        
        void addViewedBrowserTab(BrowserTabContent* browserTabContent);
        
        std::vector<BrowserTabContent*> getViewedBrowserTabs() const;
        
        std::vector<int32_t> getViewdedBrowserTabIndices() const;
        
        std::vector<StructureEnum::Enum> getViewedSurfaceStructures() const;
        
    private:
        EventBrowserTabGetAllViewed(const EventBrowserTabGetAllViewed&);

        EventBrowserTabGetAllViewed& operator=(const EventBrowserTabGetAllViewed&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

        std::vector<BrowserTabContent*> m_viewedBrowserTabs;
    };
    
#ifdef __EVENT_BROWSER_TAB_GET_ALL_VIEWED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_TAB_GET_ALL_VIEWED_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_TAB_GET_ALL_VIEWED_H__
