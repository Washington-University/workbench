#ifndef __TILE_TABS_GRID_ROW_COLUMN_ELEMENT_H__
#define __TILE_TABS_GRID_ROW_COLUMN_ELEMENT_H__

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

#include "CaretObject.h"

#include "TileTabsGridRowColumnContentTypeEnum.h"
#include "TileTabsGridRowColumnStretchTypeEnum.h"

namespace caret {

    class TileTabsGridRowColumnElement : public CaretObject {
        
    public:
        TileTabsGridRowColumnElement();
        
        virtual ~TileTabsGridRowColumnElement();
        
        TileTabsGridRowColumnElement(const TileTabsGridRowColumnElement& obj);

        TileTabsGridRowColumnElement& operator=(const TileTabsGridRowColumnElement& obj);
        
        void clear();
        
        TileTabsGridRowColumnContentTypeEnum::Enum getContentType() const;
        
        void setContentType(const TileTabsGridRowColumnContentTypeEnum::Enum contentType);

        TileTabsGridRowColumnStretchTypeEnum::Enum getStretchType() const;
        
        void setStretchType(const TileTabsGridRowColumnStretchTypeEnum::Enum stretchType);
        
        float getPercentStretch() const;
        
        void setPercentStretch(const float percentStretch);
        
        float getWeightStretch() const;
        
        void setWeightStretch(const float weightStretch);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperTileTabsGridRowColumnElement(const TileTabsGridRowColumnElement& obj);

        TileTabsGridRowColumnContentTypeEnum::Enum m_contentType = TileTabsGridRowColumnContentTypeEnum::TAB;
        
        TileTabsGridRowColumnStretchTypeEnum::Enum m_stretchType = TileTabsGridRowColumnStretchTypeEnum::WEIGHT;
        
        float m_percentStretch = 20.0;
        
        float m_weightStretch = 1.0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TILE_TABS_GRID_ROW_COLUMN_ELEMENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_GRID_ROW_COLUMN_ELEMENT_DECLARE__

} // namespace
#endif  //__TILE_TABS_GRID_ROW_COLUMN_ELEMENT_H__
