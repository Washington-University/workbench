#ifndef __DISPLAY_PROPERTIES_FIBER_ORIENTATION__H_
#define __DISPLAY_PROPERTIES_FIBER_ORIENTATION__H_

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
#include "EventListenerInterface.h"
#include "FiberOrientationColoringTypeEnum.h"
#include "FiberOrientationSymbolTypeEnum.h"

namespace caret {

    class Brain;
    class FiberOrientation;
    class VolumeFile;
    
    class DisplayPropertiesFiberOrientation : public DisplayProperties, public EventListenerInterface {
        
    public:
        DisplayPropertiesFiberOrientation(Brain* brain);
        
        virtual ~DisplayPropertiesFiberOrientation();

        void receiveEvent(Event* event);
        
        virtual void reset();
        
        virtual void update();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
        bool isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
        void setDisplayed(const DisplayGroupEnum::Enum displayGroup,
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
        
        struct OrientationVector {
            float vector[3];
            float magnitude;
            float rgb[3];
            
            void setColor() {
                rgb[0] = (vector[0] >= 0.0) ? vector[0] : -vector[0];
                rgb[1] = (vector[1] >= 0.0) ? vector[1] : -vector[1];
                rgb[2] = (vector[2] >= 0.0) ? vector[2] : -vector[2];
            }
        };
        
        bool getSphericalOrientationVectors(std::vector<OrientationVector>& xVectors,
                                            std::vector<OrientationVector>& yVectors,
                                            std::vector<OrientationVector>& zVectors,
                                            FiberOrientation* &fiberOrientationOut,
                                            AString& errorMessageOut);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        bool loadSphericalOrientationVolumes(AString& errorMessageOut);
        
        DisplayPropertiesFiberOrientation(const DisplayPropertiesFiberOrientation&);

        DisplayPropertiesFiberOrientation& operator=(const DisplayPropertiesFiberOrientation&);
        
        void resetPrivate();
        
        Brain* m_brain;
        
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
        
        /** Tried to load sample volumes since last reset (they may or may not be valid) */
        bool m_sampleVolumesLoadAttemptValid;
        
        /** Sample volumes were loaded and are valid */
        bool m_sampleVolumesValid;
        
        /* sample magnitude volumes */
        VolumeFile* m_sampleMagnitudeVolumes[3];
        
        /* sample theta angle volumes */
        VolumeFile* m_sampleThetaVolumes[3];
        
        /* sample phi angle volumes */
        VolumeFile* m_samplePhiVolumes[3];
        
        float m_lastIdentificationXYZ[3];
        
        bool m_lastIdentificationValid;
        
        friend class BrainOpenGLFixedPipeline;
    };
    
#ifdef __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_FIBER_ORIENTATION__H_
