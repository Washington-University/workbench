#ifndef __EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX_H__
#define __EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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
#include "StructureEnum.h"


namespace caret {

    class EventBrowserTabRotateSurfaceToShowVertex : public Event {
        
    public:
        EventBrowserTabRotateSurfaceToShowVertex(const int32_t browserTabIndex,
                                                 const StructureEnum::Enum surfaceStructure,
                                                 const std::vector<int32_t>& surfaceVertexIndices);
        
        virtual ~EventBrowserTabRotateSurfaceToShowVertex();
        
        EventBrowserTabRotateSurfaceToShowVertex(const EventBrowserTabRotateSurfaceToShowVertex&) = delete;

        EventBrowserTabRotateSurfaceToShowVertex& operator=(const EventBrowserTabRotateSurfaceToShowVertex&) = delete;
        
        int32_t getBrowserTabIndex() const;
        
        StructureEnum::Enum getSurfaceStructure() const;
        
        const std::vector<int32_t>& getSurfaceVertexIndices() const;

        // ADD_NEW_METHODS_HERE

    private:
        const int32_t m_browserTabIndex;
        
        const StructureEnum::Enum m_surfaceStructure;
        
        const std::vector<int32_t> m_surfaceVertexIndices;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX_DECLARE__

} // namespace
#endif  //__EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX_H__
