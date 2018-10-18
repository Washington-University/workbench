#ifndef __SPACER_TAB_CONTENT_H__
#define __SPACER_TAB_CONTENT_H__

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

#include "TabContentBase.h"



namespace caret {

    class SpacerTabContent : public TabContentBase {
        
    public:
        class MapKey {
        public:
            MapKey(const int32_t windowIndex,
                   const int32_t rowIndex,
                   const int32_t columnIndex);
            
            bool operator<(const MapKey& rhs) const;
            
            const int32_t m_windowIndex;
            const int32_t m_rowIndex;
            const int32_t m_columnIndex;
        };
        
        SpacerTabContent(const int32_t windowIndex,
                         const int32_t rowIndex,
                         const int32_t columnIndex);
        
        virtual ~SpacerTabContent();
        
        SpacerTabContent(const SpacerTabContent&) = delete;

        SpacerTabContent& operator=(const SpacerTabContent&) = delete;
        
        virtual AString getTabName() const override;
        
        virtual AString getTabNamePrefix() const override;
        
        int32_t getWindowIndex() const;
        
        int32_t getRowIndex() const;
        
        int32_t getColumnIndex() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        MapKey m_key;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SPACER_TAB_CONTENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPACER_TAB_CONTENT_DECLARE__

} // namespace
#endif  //__SPACER_TAB_CONTENT_H__
