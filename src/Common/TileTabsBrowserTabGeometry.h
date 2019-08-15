#ifndef __TILE_TABS_BROWSER_TAB_GEOMETRY_H__
#define __TILE_TABS_BROWSER_TAB_GEOMETRY_H__

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

    class TileTabsBrowserTabGeometry : public CaretObject {
        
    public:
        TileTabsBrowserTabGeometry(const int32_t tabIndex);
        
        virtual ~TileTabsBrowserTabGeometry();
        
        TileTabsBrowserTabGeometry(const TileTabsBrowserTabGeometry& obj);

        TileTabsBrowserTabGeometry& operator=(const TileTabsBrowserTabGeometry& obj);

        void copyGeometry(const TileTabsBrowserTabGeometry& geometry);
        
        int32_t getTabIndex() const;
        
        void setBounds(const float minX,
                       const float maxX,
                       const float minY,
                       const float maxY);
        
        void getBounds(float& minX,
                       float& maxX,
                       float& minY,
                       float& maxY) const;
        
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

        bool intersectionTest(const TileTabsBrowserTabGeometry* other) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperTileTabsBrowserTabGeometry(const TileTabsBrowserTabGeometry& obj);

        /** index of tab */
        int32_t m_tabIndex;
        
        /** left as percentage*/
        float m_minX = 10.0;

        /** right as percentage*/
        float m_maxX = 20.0;

        /** bottom as percentage*/
        float m_minY = 10.0;

        /** topas percentage*/
        float m_maxY = 20.0;

        /** Stacking order (depth in screen) of tab, greater value is 'in front'*/
        int32_t m_stackingOrder = 1;

        /** Type of background (opaque / transparent) for tab*/
        TileTabsLayoutBackgroundTypeEnum::Enum m_backgroundType = TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG;

        // ADD_NEW_MEMBERS_HERE

        /** IF NEW MEMBERS ADDED, MUST UPDATE THIS CLASS */
        friend class TileTabsBrowserTabGeometrySceneHelper;
    };
    
#ifdef __TILE_TABS_BROWSER_TAB_GEOMETRY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_BROWSER_TAB_GEOMETRY_DECLARE__

} // namespace
#endif  //__TILE_TABS_BROWSER_TAB_GEOMETRY_H__
