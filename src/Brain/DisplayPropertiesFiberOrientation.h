#ifndef __DISPLAY_PROPERTIES_FIBER_ORIENTATION__H_
#define __DISPLAY_PROPERTIES_FIBER_ORIENTATION__H_

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
#include "FiberOrientationColoringTypeEnum.h"
#include "FiberOrientationSymbolTypeEnum.h"

namespace caret {

    class Brain;
    
    class DisplayPropertyDataBoolean;
    class DisplayPropertyDataFloat;
    
    class DisplayPropertiesFiberOrientation : public DisplayProperties {
        
    public:
        DisplayPropertiesFiberOrientation(const Brain* brain);
        
        virtual ~DisplayPropertiesFiberOrientation();

        virtual void reset();
        
        virtual void update();
        
        void setMaximumUncertaintyFromFiles();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
        bool isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        void setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const bool displayStatus);
        
        bool isDrawFiberTrajectoriesInFront(const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex) const;
        
        void setDrawFiberTrajectoriesInFront(const DisplayGroupEnum::Enum displayGroup,
                                             const int32_t tabIndex,
                                             const bool displayStatus);

        bool isDrawWithMagnitude(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex) const;
        
        void setDrawWithMagnitude(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const bool drawWithMagnitude);
        
        DisplayGroupEnum::Enum getDisplayGroupForTab(const int32_t browserTabIndex) const;
        
        void setDisplayGroupForTab(const int32_t browserTabIndex,
                             const DisplayGroupEnum::Enum displayGroup);
        
        float getAboveLimit(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex) const;
        
        void setAboveLimit(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const float aboveLimit);
        
        void setAboveAndBelowLimitsForAll(const float aboveLimit,
                                          const float belowLimit);
        
        float getBelowLimit(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex) const;
        
        void setBelowLimit(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex,
                           const float aboveLimit);
        
        float getMinimumMagnitude(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex) const;
        
        void setMinimumMagnitude(const DisplayGroupEnum::Enum displayGroup,
                          const int32_t tabIndex,
                          const float minimumMagnitude);
        
        float getMaximumUncertainty(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex) const;
        
        void setMaximumUncertainty(const DisplayGroupEnum::Enum displayGroup,
                                   const int32_t tabIndex,
                                   const float maximumUncertainty);
        
        float getLengthMultiplier(const DisplayGroupEnum::Enum displayGroup,
                                  const int32_t tabIndex) const;
        
        void setLengthMultiplier(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const float lengthMultiplier);
        
        float getFanMultiplier(const DisplayGroupEnum::Enum displayGroup,
                                  const int32_t tabIndex) const;
        
        void setFanMultiplier(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const float fanMultiplier);
        
        FiberOrientationColoringTypeEnum::Enum getColoringType(const DisplayGroupEnum::Enum displayGroup,
                                                   const int32_t tabIndex) const;
        
        void setColoringType(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const FiberOrientationColoringTypeEnum::Enum coloringType);        
        
        FiberOrientationSymbolTypeEnum::Enum getSymbolType(const DisplayGroupEnum::Enum displayGroup,
                                                               const int32_t tabIndex) const;
        
        void setSymbolType(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const FiberOrientationSymbolTypeEnum::Enum symbolType);
        
        bool isSphereOrientationsDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex) const;
        
        void setSphereOrientationsDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                  const int32_t tabIndex,
                                  const bool displaySphereOrientations);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        DisplayPropertiesFiberOrientation(const DisplayPropertiesFiberOrientation&);

        DisplayPropertiesFiberOrientation& operator=(const DisplayPropertiesFiberOrientation&);
        
        const Brain* m_brain;
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_displayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_aboveLimitInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_aboveLimitInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_belowLimitInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_belowLimitInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_minimumMagnitudeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_minimumMagnitudeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_drawWithMagnitudeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_drawWithMagnitudeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_lengthMultiplierInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_lengthMultiplierInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_fanMultiplierInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_fanMultiplierInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        FiberOrientationColoringTypeEnum::Enum m_fiberColoringTypeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        FiberOrientationColoringTypeEnum::Enum m_fiberColoringTypeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
                
        FiberOrientationSymbolTypeEnum::Enum m_fiberSymbolTypeInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        FiberOrientationSymbolTypeEnum::Enum m_fiberSymbolTypeInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

        bool m_displaySphereOrientationsInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_displaySphereOrientationsInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        std::unique_ptr<DisplayPropertyDataFloat> m_maximumUncertainty;

        std::unique_ptr<DisplayPropertyDataBoolean> m_drawFiberTrajectoriesInFront;
        
        static constexpr float s_defaultMaximumUncertainty = 10.0f;
    };
    
#ifdef __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_FIBER_ORIENTATION__H_
