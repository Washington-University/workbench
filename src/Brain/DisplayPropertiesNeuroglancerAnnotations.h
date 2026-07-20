#ifndef __DISPLAY_PROPERTIES_NEUROGLANCER_ANNOTATIONS__H_
#define __DISPLAY_PROPERTIES_NEUROGLANCER_ANNOTATIONS__H_

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

#include <memory>

#include "BrainConstants.h"
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"

namespace caret {
    class DisplayPropertyDataFloat;
    
    class DisplayPropertiesNeuroglancerAnnotations : public DisplayProperties {
        
    public:
        DisplayPropertiesNeuroglancerAnnotations();
        
        virtual ~DisplayPropertiesNeuroglancerAnnotations();

        virtual void reset();
        
        virtual void update();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                        const int32_t targetTabIndex);
        
        bool isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        void setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const bool displayStatus);
        
        DisplayGroupEnum::Enum getDisplayGroupForTab(const int32_t browserTabIndex) const;
        
        void setDisplayGroupForTab(const int32_t browserTabIndex,
                             const DisplayGroupEnum::Enum  displayGroup);
        
        float getSymbolScale() const;
        
        void setSymbolScale(const float symbolScale);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        void resetPrivate();
        
        DisplayPropertiesNeuroglancerAnnotations(const DisplayPropertiesNeuroglancerAnnotations&);

        DisplayPropertiesNeuroglancerAnnotations& operator=(const DisplayPropertiesNeuroglancerAnnotations&);
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_displayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_symbolScale = 1.0;
        
    };
    
#ifdef __DISPLAY_PROPERTIES_NEUROGLANCER_ANNOTATIONS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_NEUROGLANCER_ANNOTATIONS_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_NEUROGLANCER_ANNOTATIONS__H_
