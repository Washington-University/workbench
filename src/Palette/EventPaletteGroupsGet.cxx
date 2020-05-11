
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

#define __EVENT_PALETTE_GROUPS_GET_DECLARE__
#include "EventPaletteGroupsGet.h"
#undef __EVENT_PALETTE_GROUPS_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventPaletteGroupsGet 
 * \brief Event that gets palette groups
 * \ingroup Palette
 */

/**
 * Constructor.
 */
EventPaletteGroupsGet::EventPaletteGroupsGet()
: Event(EventTypeEnum::EVENT_PALETTE_GROUPS_GET)
{
    
}

/**
 * Destructor.
 */
EventPaletteGroupsGet::~EventPaletteGroupsGet()
{
}

/**
 * Add a palette group
 * @param paletteGroup
 *    Group that is added.
 *    Note: Pass by values so that a shared_ptr is automatically converted to a weak_ptr
 */
void
EventPaletteGroupsGet::addPaletteGroup(std::weak_ptr<PaletteGroup> paletteGroup)
{
    m_paletteGroups.push_back(paletteGroup);
}

/**
 * @return The palette groups
 */
std::vector<std::weak_ptr<PaletteGroup>>
EventPaletteGroupsGet::getPaletteGroups() const
{
    return m_paletteGroups;
}

