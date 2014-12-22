
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __EVENT_MAP_YOKING_SELECT_MAP_DECLARE__
#include "EventMapYokingSelectMap.h"
#undef __EVENT_MAP_YOKING_SELECT_MAP_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventMapYokingSelectMap 
 * \brief Event sent when a yoked overlay or file selection changes.
 * \ingroup Files
 */

/**
 * Constructor for change in map yoking.
 *
 * @param caretMappableDataFile
 *    Caret mappable file that is causing this event.
 * @param mapYokingGroup
 *    Map yoking group that has a status change (selected map or enabled status)
 */
EventMapYokingSelectMap::EventMapYokingSelectMap(const MapYokingGroupEnum::Enum mapYokingGroup,
                                                 const CaretMappableDataFile* caretMappableDataFile,
                                                 const int32_t mapIndex,
                                                 const bool selectionStatus)
: Event(EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP),
m_mapYokingGroup(mapYokingGroup),
m_caretMappableDataFile(caretMappableDataFile),
m_mapIndex(mapIndex),
m_selectionStatus(selectionStatus)
{
    if (mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        MapYokingGroupEnum::setSelectedMapIndex(mapYokingGroup, mapIndex);
        MapYokingGroupEnum::setEnabled(mapYokingGroup, selectionStatus);
    }
}

/**
 * Destructor.
 */
EventMapYokingSelectMap::~EventMapYokingSelectMap()
{
}

/**
 * @return The map yoking group.
 */
MapYokingGroupEnum::Enum
EventMapYokingSelectMap::getMapYokingGroup() const
{
    return m_mapYokingGroup;
}

/**
 * @return Caret Mappable Data File for which event was issued.
 * Might be NULL.
 */
const CaretMappableDataFile*
EventMapYokingSelectMap::getCaretMappableDataFile() const
{
    return m_caretMappableDataFile;
}

/**
 * @return Map index selected.
 */
int32_t
EventMapYokingSelectMap::getMapIndex() const
{
    return m_mapIndex;
}

/**
 * @return Selection status but ONLY for SAME FILE !
 */
bool
EventMapYokingSelectMap::getSelectionStatus() const
{
    return m_selectionStatus;
}


