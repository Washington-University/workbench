#ifndef __EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED_H__
#define __EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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
#include <vector>

#include "Event.h"



namespace caret {

    class EventBrowserTabIndicesGetAllViewed : public Event {
        
    public:
        EventBrowserTabIndicesGetAllViewed();
        
        virtual ~EventBrowserTabIndicesGetAllViewed();
        
        EventBrowserTabIndicesGetAllViewed(const EventBrowserTabIndicesGetAllViewed&) = delete;

        EventBrowserTabIndicesGetAllViewed& operator=(const EventBrowserTabIndicesGetAllViewed&) = delete;
        
        void addBrowserTabIndex(const int32_t browserTabIndex);
        
        std::vector<int32_t> getAllBrowserTabIndices() const;
        
        bool isValidBrowserTabIndex(const int32_t browserTabIndex);

        // ADD_NEW_METHODS_HERE

    private:
        std::vector<int32_t> m_browserTabIndices;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED_H__
