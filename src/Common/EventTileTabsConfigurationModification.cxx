
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

#define __EVENT_TILE_TABS_CONFIGURATION_MODIFICATION_DECLARE__
#include "EventTileTabsConfigurationModification.h"
#undef __EVENT_TILE_TABS_CONFIGURATION_MODIFICATION_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventTileTabsConfigurationModification 
 * \brief Event for modifying a tile tabs configuration.
 * \ingroup Common
 */

/**
 * Constructor.
 * 
 * @param tileTabsConfiguration
 *     Tile tabs configuration that will be modified
 * @param rowColumnIndex
 *     Index of the row or column used for modification
 * @param rowColumnType
 *     'Row' or "Column' type
 * @param operation
 *     Enumerated type with type of modification.
 */
EventTileTabsConfigurationModification::EventTileTabsConfigurationModification(TileTabsConfiguration* tileTabsConfiguration,
                                                                               const int32_t rowColumnIndex,
                                                                               const RowColumnType rowColumnType,
                                                                               const Operation operation)
: Event(EventTypeEnum::EVENT_TILE_TABS_MODIFICATION),
m_tileTabsConfiguration(tileTabsConfiguration),
m_rowColumnIndex(rowColumnIndex),
m_rowColumnType(rowColumnType),
m_operation(operation),
m_windowIndex(-1)
{
    
}

/**
 * Destructor.
 */
EventTileTabsConfigurationModification::~EventTileTabsConfigurationModification()
{
}

/**
 * @return Tile tabs configuration that is modified.
 */
TileTabsConfiguration*
EventTileTabsConfigurationModification::getTileTabsConfiguration()
{
    return m_tileTabsConfiguration;
}


/**
 * @return Index of the window.
 */
int32_t
EventTileTabsConfigurationModification::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Set the window index.
 *
 * @param windowIndex
 *     Index of the window.
 */
void
EventTileTabsConfigurationModification::setWindowIndex(const int32_t windowIndex)
{
    m_windowIndex = windowIndex;
}

/**
 * @return The index of the row or column.
 */
int32_t
EventTileTabsConfigurationModification::getRowColumnIndex() const
{
    return m_rowColumnIndex;
}

/**
 * @return The type ROW or COLUMN
 */
EventTileTabsConfigurationModification::RowColumnType
EventTileTabsConfigurationModification::getRowColumnType() const
{
    return m_rowColumnType;
}

/**
 * @return The operation.
 */
EventTileTabsConfigurationModification::Operation
EventTileTabsConfigurationModification::getOperation() const
{
    return m_operation;
}

