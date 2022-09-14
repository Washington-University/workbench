#ifndef __SELECTION_ITEM_HISTOLOGY_COORDINATE_H_
#define __SELECTION_ITEM_HISTOLOGY_COORDINATE_H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <array>

#include "HistologyCoordinate.h"
#include "PixelIndex.h"
#include "PixelLogicalIndex.h"
#include "SelectionItem.h"

namespace caret {

    class HistologySlicesFile;
    class MediaFile;
    
    class SelectionItemHistologyCoordinate : public SelectionItem {
        
    public:
        SelectionItemHistologyCoordinate();
        
        virtual ~SelectionItemHistologyCoordinate();
        
        virtual bool isValid() const;
        
        HistologyCoordinate getCoordinate() const;
        
        void setCoordinate(const HistologyCoordinate& histologyCoordinate);
        
        int32_t getOverlayIndex() const;
        
        int32_t getTabIndex() const;
        
        void setOverlayIndex(const int32_t tabIndex);
        
        void setTabIndex(const int32_t tabIndex);
        
        bool getPixelRGBA(uint8_t pixelRGBAOut[4]) const;
        
        bool isPixelRGBAValid() const;
        
        void setPixelRGBA(const uint8_t pixelRGBA[4]);

        virtual void reset();
        
        virtual AString toString() const;
        
    private:
        SelectionItemHistologyCoordinate(const SelectionItemHistologyCoordinate&);

        SelectionItemHistologyCoordinate& operator=(const SelectionItemHistologyCoordinate&);

        void resetPrivate();
        
        HistologyCoordinate m_histologyCoordinate = HistologyCoordinate();
        
        int32_t m_tabIndex = -1;
        
        int32_t m_overlayIndex = -1;
        
        uint8_t m_pixelRGBA[4];
        
        bool m_pixelRGBAValidFlag = false;
    };
    
#ifdef __SELECTION_ITEM_HISTOLOGY_COORDINATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_HISTOLOGY_COORDINATE_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_HISTOLOGY_COORDINATE_H_
