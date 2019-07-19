#ifndef __TILE_TABS_LAYOUT_TAB_INFO_H__
#define __TILE_TABS_LAYOUT_TAB_INFO_H__

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

#include "CaretObject.h"
#include "TileTabsLayoutBackgroundTypeEnum.h"


namespace caret {

    class TileTabsLayoutTabInfo : public CaretObject {
        
    public:
        TileTabsLayoutTabInfo(const int32_t tabIndex);
        
        virtual ~TileTabsLayoutTabInfo();
        
        TileTabsLayoutTabInfo(const TileTabsLayoutTabInfo& obj);

        TileTabsLayoutTabInfo& operator=(const TileTabsLayoutTabInfo& obj);

        int32_t getTabIndex() const;
        
        float getCenterX() const;
        
        void setCenterX(const float x);

        float getCenterY() const;
        
        void setCenterY(const float y);

        float getWidth() const;
        
        void setWidth(const float width);
        
        float getHeight() const;
        
        void setHeight(const float height);

        float getMinX() const;
        
        void setMinX(const float minX);

        float getMaxX() const;
        
        void setMaxX(const float maxX);
        
        float getMinY() const;
        
        void setMinY(const float minY);
        
        float getMaxY() const;
        
        void setMaxY(const float maxY);
        
        void getWindowViewport(const int32_t windowWidth,
                               const int32_t windowHeight,
                               int32_t viewportOut[4]) const;
        
        int32_t getStackingOrder() const;
        
        void setStackingOrder(const int32_t stackingOrder);
        
        TileTabsLayoutBackgroundTypeEnum::Enum getBackgroundType() const;
        
        void setBackgroundType(const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperTileTabsLayoutTabInfo(const TileTabsLayoutTabInfo& obj);

        /** index of tab */
        int32_t m_tabIndex;
        
        /** center x-coordinate as percentage*/
        float m_xCenter = 50.0;

        /** center y-coordinate as percentage*/
        float m_yCenter = 50.0;

        /** width of window as percentage*/
        float m_width = 20.0;

        /** height of window as percentage*/
        float m_height = 20.0;

        /** Stacking order (depth in screen) of tab, greater value is 'in front'*/
        int32_t m_stackingOrder = 1;

        /** Type of background (opaque / transparent) for tab*/
        TileTabsLayoutBackgroundTypeEnum::Enum m_backgroundType = TileTabsLayoutBackgroundTypeEnum::OPAQUE;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TILE_TABS_TAB_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_TAB_INFO_DECLARE__

} // namespace
#endif  //__TILE_TABS_LAYOUT_TAB_INFO_H__
