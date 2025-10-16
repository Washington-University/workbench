#ifndef __DISPLAY_PROPERTIES_LABELS__H_
#define __DISPLAY_PROPERTIES_LABELS__H_

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

#include "BrainConstants.h"
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"
#include "LabelViewModeEnum.h"

namespace caret {

    class DisplayPropertiesLabels : public DisplayProperties {
        
    public:
        DisplayPropertiesLabels();
        
        virtual ~DisplayPropertiesLabels();
        
        virtual void reset();
        
        virtual void update();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        DisplayGroupEnum::Enum getDisplayGroupForTab(const int32_t browserTabIndex) const;
        
        void setDisplayGroupForTab(const int32_t browserTabIndex,
                                   const DisplayGroupEnum::Enum  displayGroup);
        
        LabelViewModeEnum::Enum getLabelViewModeForTab(const int32_t browserTabIndex) const;
        
        void setLabelViewModeForTab(const int32_t browserTabIndex,
                                    const LabelViewModeEnum::Enum labelViewMode);
        
        bool isShowUnusedLabelsInHierarchies() const;
        
        void setShowUnusedLabelsInHierarchies(const bool status);
        

    private:
        DisplayPropertiesLabels(const DisplayPropertiesLabels&);

        DisplayPropertiesLabels& operator=(const DisplayPropertiesLabels&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE
        
        std::array<DisplayGroupEnum::Enum, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS> m_displayGroup;
        
        std::array<LabelViewModeEnum::Enum, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS> m_labelViewMode;

        bool m_showUnusedLabelsInHierarchiesFlag = false;
    };
    
#ifdef __DISPLAY_PROPERTIES_LABELS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_LABELS_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_LABELS__H_
