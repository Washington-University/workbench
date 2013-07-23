#ifndef __FIBER_TRAJECTORY_MAP_PROPERTIES_H__
#define __FIBER_TRAJECTORY_MAP_PROPERTIES_H__

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
#include "FiberTrajectoryDisplayModeEnum.h"
#include "SceneableInterface.h"

namespace caret {

    class SceneClassAssistant;
    
    class FiberTrajectoryMapProperties : public SceneableInterface {
        
    public:
        FiberTrajectoryMapProperties();
        
        virtual ~FiberTrajectoryMapProperties();

        bool isDisplayed() const;
        
        void setDisplayed(const bool displayStatus);
                
        FiberTrajectoryDisplayModeEnum::Enum getDisplayMode() const;
        
        void setDisplayMode(const FiberTrajectoryDisplayModeEnum::Enum displayMode);
        
        float getProportionStreamline() const;
        
        void setProportionStreamline(const float thresholdStreamline);
        
        float getProportionMaximumOpacity() const;
        
        void setProportionMaximumOpacity(const float maximumProportionOpacity);
        
        float getProportionMinimumOpacity() const;
        
        void setProportionMinimumOpacity(const float minimumProportionOpacity);
        
        
        float getCountStreamline() const;
        
        void setCountStreamline(const float countStreamline);
        
        float getCountMaximumOpacity() const;
        
        void setCountMaximumOpacity(const float countMaximumOpacity);
        
        float getCountMinimumOpacity() const;
        
        void setCountMinimumOpacity(const float countMinimumOpacity);
        

        float getDistanceStreamline() const;
        
        void setDistanceStreamline(const float distanceStreamline);
        
        float getDistanceMaximumOpacity() const;
        
        void setDistanceMaximumOpacity(const float distanceMaximumOpacity);
        
        float getDistanceMinimumOpacity() const;
        
        void setDistanceMinimumOpacity(const float distanceMinimumOpacity);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        FiberTrajectoryMapProperties(const FiberTrajectoryMapProperties&);

        FiberTrajectoryMapProperties& operator=(const FiberTrajectoryMapProperties&);
        
        FiberTrajectoryDisplayModeEnum::Enum m_displayMode;
        
        SceneClassAssistant* m_sceneAssistant;
        
        bool m_displayStatus;
        
        float m_proportionStreamline;
        
        float m_maximumProportionOpacity;
        
        float m_minimumProportionOpacity;

        float m_countStreamline;
        
        float m_countMaximumOpacity;
        
        float m_countMinimumOpacity;

        float m_distanceStreamline;
        
        float m_distanceMaximumOpacity;
        
        float m_distanceMinimumOpacity;
};
    
#ifdef __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__

} // namespace
#endif  //__FIBER_TRAJECTORY_MAP_PROPERTIES_H__
