#ifndef __DISPLAY_PROPERTIES_FIBER_TRAJECTORY__H_
#define __DISPLAY_PROPERTIES_FIBER_TRAJECTORY__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include "BrainConstants.h"
#include "DisplayGroupEnum.h"
#include "DisplayProperties.h"

namespace caret {

    class Brain;
    
    class DisplayPropertiesFiberTrajectory : public DisplayProperties {
        
    public:
        DisplayPropertiesFiberTrajectory(Brain* brain);
        
        virtual ~DisplayPropertiesFiberTrajectory();

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
                             const DisplayGroupEnum::Enum displayGroup);
        
        
        float getThresholdProportion(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex) const;
        
        void setThresholdProportion(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex,
                           const float thresholdProportion);
        
        float getThresholdStreamline(const DisplayGroupEnum::Enum displayGroup,
                                     const int32_t tabIndex) const;
        
        void setThresholdStreamline(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex,
                                    const float thresholdStreamline);
        
        float getMaximumProportionOpacity(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex) const;
        
        void setMaximumProportionOpacity(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex,
                           const float maximumProportionOpacity);
        
        float getMinimumProportionOpacity(const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex) const;
        
        void setMinimumProportionOpacity(const DisplayGroupEnum::Enum displayGroup,
                                         const int32_t tabIndex,
                                         const float minimumProportionOpacity);
        
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        DisplayPropertiesFiberTrajectory(const DisplayPropertiesFiberTrajectory&);

        DisplayPropertiesFiberTrajectory& operator=(const DisplayPropertiesFiberTrajectory&);
        
        DisplayGroupEnum::Enum m_displayGroup[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_displayStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        bool m_displayStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_thresholdProportionInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_thresholdProportionInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_thresholdStreamlineInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_thresholdStreamlineInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_maximimProportionOpacityInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_maximumProportionOpacityInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_minimumProportionOpacityInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        float m_minimumProportionOpacityInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

    };
    
#ifdef __DISPLAY_PROPERTIES_FIBER_TRAJECTORY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_FIBER_TRAJECTORY_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_FIBER_TRAJECTORY__H_
