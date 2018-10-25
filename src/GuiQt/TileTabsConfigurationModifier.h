#ifndef __TILE_TABS_CONFIGURATION_MODIFIER_H__
#define __TILE_TABS_CONFIGURATION_MODIFIER_H__

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
#include <vector>
#include "CaretObject.h"



namespace caret {

    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class EventTileTabsConfigurationModification;
    class SpacerTabContent;
    class TileTabsConfiguration;
    class TileTabsRowColumnElement;
    
    class TileTabsConfigurationModifier : public CaretObject {
        
    public:
        TileTabsConfigurationModifier(const std::vector<const BrainOpenGLViewportContent*>& existingTabs,
                                      EventTileTabsConfigurationModification* modifyEvent);
        
        virtual ~TileTabsConfigurationModifier();
        
        TileTabsConfigurationModifier(const TileTabsConfigurationModifier&) = delete;

        TileTabsConfigurationModifier& operator=(const TileTabsConfigurationModifier&) = delete;
        
        bool run(AString& errorMessageOut);
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        class Element : public CaretObject {
        public:
            Element(const int32_t rowIndex,
                    const int32_t columnIndex,
                    BrowserTabContent* browserTabContent);
            
            AString toString() const override;
            
            int32_t m_targetRowIndex;
            
            int32_t m_targetColumnIndex;
            
            int32_t m_sourceRowIndex;
            
            int32_t m_sourceColumnIndex;
            
            BrowserTabContent* m_browserTabContent;
            
        };
        
        /**
         * Contains the tabs and stretching for one row or one column
         */
        class RowColumnContent : public CaretObject {
        public:
            RowColumnContent(const std::vector<const BrainOpenGLViewportContent*>& existingTabs,
                             TileTabsConfiguration* tileTabsConfiguration,
                             const int32_t rowColumnIndex,
                             const bool rowFlag);
            
            ~RowColumnContent();
            
            AString toString() const override;
            
            std::vector<Element*> m_tabElements;
            
            TileTabsRowColumnElement* m_stretching;
            
        };
        
    private:
        
        void loadRowColumnsFromTileTabsConfiguration();
        
        bool loadRowColumnsIntoTileTabsConfiguration(AString& errorMessageOut);
        
        bool performModification(AString& errorMessageOut);
        
        const std::vector<const BrainOpenGLViewportContent*>& m_existingTabs;
        
        EventTileTabsConfigurationModification* m_modifyEvent;
        
        std::vector<RowColumnContent*> m_rowColumns;
        
        std::vector<BrowserTabContent*> m_browserTabsToDelete;
        
        /**
         * This is the current tile tabs configuration in the window so DO NOT delete it
         */
        TileTabsConfiguration* m_currentTileTabsConfiguration = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TILE_TABS_CONFIGURATION_MODIFIER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_CONFIGURATION_MODIFIER_DECLARE__

} // namespace
#endif  //__TILE_TABS_CONFIGURATION_MODIFIER_H__
