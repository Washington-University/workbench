#ifndef __TILE_TABS_MANUAL_CONFIGURATION_MODIFIER_H__
#define __TILE_TABS_MANUAL_CONFIGURATION_MODIFIER_H__

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



namespace caret {

    class BrowserTabContent;
    
    class TileTabsManualConfigurationModifier : public CaretObject {
        
    public:
        enum class TabOrderOperation {
            BRING_TO_FRONT,
            BRING_FORWARD,
            SEND_TO_BACK,
            SEND_BACKWARD
        };
        
        TileTabsManualConfigurationModifier(const std::vector<BrowserTabContent*>& browserTabContents);
        
        virtual ~TileTabsManualConfigurationModifier();
        
        TileTabsManualConfigurationModifier(const TileTabsManualConfigurationModifier&) = delete;

        TileTabsManualConfigurationModifier& operator=(const TileTabsManualConfigurationModifier&) = delete;
        
        bool runTabOrderOperation(const TabOrderOperation operation,
                                  const int32_t tabIndex,
                                  AString& errorMessageOut);


        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        /**
         * Sort tab content by stack order
         */
        class TabInfo {
        public:
            TabInfo(BrowserTabContent* tabContent,
                    const int32_t stackOrder)
            : m_tabContent(tabContent),
            m_stackOrder(stackOrder) { }

            bool operator<(const TabInfo& tabInfo) const {
                return m_stackOrder < tabInfo.m_stackOrder;
            }
            
            BrowserTabContent* m_tabContent = NULL;
            int32_t m_stackOrder = -1;
        };
        
        std::vector<BrowserTabContent*> m_browserTabs;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TILE_TABS_MANUAL_CONFIGURATION_MODIFIER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_MANUAL_CONFIGURATION_MODIFIER_DECLARE__

} // namespace
#endif  //__TILE_TABS_MANUAL_CONFIGURATION_MODIFIER_H__
