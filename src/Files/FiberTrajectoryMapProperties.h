#ifndef __FIBER_TRAJECTORY_MAP_PROPERTIES_H__
#define __FIBER_TRAJECTORY_MAP_PROPERTIES_H__

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

#include "BrainConstants.h"
#include "FiberTrajectoryDisplayModeEnum.h"
#include "SceneableInterface.h"

namespace caret {

    class FiberTrajectoryColorModel;
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
        
        FiberTrajectoryColorModel* getFiberTrajectoryColorModel();
        
        const FiberTrajectoryColorModel* getFiberTrajectoryColorModel() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        void copy(const FiberTrajectoryMapProperties& other);
        
    private:
        FiberTrajectoryMapProperties(const FiberTrajectoryMapProperties&);

        FiberTrajectoryMapProperties& operator=(const FiberTrajectoryMapProperties&);
        
        SceneClassAssistant* m_sceneAssistant;
        
        FiberTrajectoryDisplayModeEnum::Enum m_displayMode;
        
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
        
        FiberTrajectoryColorModel* m_fiberTrajectoryColoringModel;
};
    
#ifdef __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__

} // namespace
#endif  //__FIBER_TRAJECTORY_MAP_PROPERTIES_H__
