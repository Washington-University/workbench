#ifndef __TAB_DRAWING_INFO_H__
#define __TAB_DRAWING_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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
#include "DisplayGroupEnum.h"
#include "LabelViewModeEnum.h"


namespace caret {

    class TabDrawingInfo : public CaretObject {
        
    public:
        TabDrawingInfo(const DisplayGroupEnum::Enum displayGroup,
                       const LabelViewModeEnum::Enum labelViewMode,
                       const int32_t tabIndex);
        
        virtual ~TabDrawingInfo();
        
        TabDrawingInfo(const TabDrawingInfo& obj);

        TabDrawingInfo& operator=(const TabDrawingInfo& obj);
        
        DisplayGroupEnum::Enum getDisplayGroup() const;
        
        LabelViewModeEnum::Enum getLabelViewMode() const;
        
        int32_t getTabIndex() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperTabDrawingInfo(const TabDrawingInfo& obj);

        DisplayGroupEnum::Enum m_displayGroup;
        
        LabelViewModeEnum::Enum m_labelViewMode;
        
        int32_t m_tabIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TAB_DRAWING_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TAB_DRAWING_INFO_DECLARE__

} // namespace
#endif  //__TAB_DRAWING_INFO_H__
