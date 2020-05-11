#ifndef __EVENT_PALETTE_GROUPS_GET_H__
#define __EVENT_PALETTE_GROUPS_GET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

    class PaletteGroup;
    
    class EventPaletteGroupsGet : public Event {
        
    public:
        EventPaletteGroupsGet();
        
        virtual ~EventPaletteGroupsGet();
        
        EventPaletteGroupsGet(const EventPaletteGroupsGet&) = delete;

        EventPaletteGroupsGet& operator=(const EventPaletteGroupsGet&) = delete;
        
        void addPaletteGroup(std::weak_ptr<PaletteGroup> paletteGroup);
        
        std::vector<std::weak_ptr<PaletteGroup>> getPaletteGroups() const;

        // ADD_NEW_METHODS_HERE

    private:
        std::vector<std::weak_ptr<PaletteGroup>> m_paletteGroups;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_PALETTE_GROUPS_GET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_PALETTE_GROUPS_GET_DECLARE__

} // namespace
#endif  //__EVENT_PALETTE_GROUPS_GET_H__
