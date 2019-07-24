#ifndef __EVENT_TILE_TABS_GRID_CONFIGURATION_MODIFICATION_H__
#define __EVENT_TILE_TABS_GRID_CONFIGURATION_MODIFICATION_H__

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

    class TileTabsLayoutGridConfiguration;
    
    class EventTileTabsGridConfigurationModification : public Event {
        
    public:
        /**
         * Row or Column
         */
        enum class RowColumnType {
            COLUMN,
            ROW
        };
        
        /**
         * Operation
         */
        enum class Operation {
            DELETE_IT,  /* DELETE is reserved word on Windows */
            DUPLICATE_AFTER,
            DUPLICATE_BEFORE,
            INSERT_SPACER_BEFORE,
            INSERT_SPACER_AFTER,
            MOVE_AFTER,
            MOVE_BEFORE
        };
        
        EventTileTabsGridConfigurationModification(TileTabsLayoutGridConfiguration* tileTabsConfiguration,
                                               const int32_t rowColumnIndex,
                                               const RowColumnType rowColumnType,
                                               const Operation operation);
        
        virtual ~EventTileTabsGridConfigurationModification();
        
        EventTileTabsGridConfigurationModification(const EventTileTabsGridConfigurationModification&) = delete;

        EventTileTabsGridConfigurationModification& operator=(const EventTileTabsGridConfigurationModification&) = delete;
        
        TileTabsLayoutGridConfiguration* getTileTabsConfiguration();
        
        int32_t getRowColumnIndex() const;
        
        RowColumnType getRowColumnType() const;
        
        Operation getOperation() const;

        int32_t getWindowIndex() const;
        
        void setWindowIndex(const int32_t windowIndex);

        // ADD_NEW_METHODS_HERE

    private:
        TileTabsLayoutGridConfiguration* m_tileTabsConfiguration;
        
        const int32_t m_rowColumnIndex;
        
        const RowColumnType m_rowColumnType;
        
        const Operation m_operation;
        
        int32_t m_windowIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_TILE_TABS_GRID_CONFIGURATION_MODIFICATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_TILE_TABS_GRID_CONFIGURATION_MODIFICATION_DECLARE__

} // namespace
#endif  //__EVENT_TILE_TABS_GRID_CONFIGURATION_MODIFICATION_H__
