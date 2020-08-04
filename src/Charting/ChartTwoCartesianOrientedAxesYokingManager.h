#ifndef __CHART_TWO_AXES_YOKING_MANAGER_H__
#define __CHART_TWO_AXES_YOKING_MANAGER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#include "EventListenerInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ChartTwoCartesianOrientedAxesYokingManager : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        ChartTwoCartesianOrientedAxesYokingManager();
        
        virtual ~ChartTwoCartesianOrientedAxesYokingManager();
        
        ChartTwoCartesianOrientedAxesYokingManager(const ChartTwoCartesianOrientedAxesYokingManager&) = delete;

        ChartTwoCartesianOrientedAxesYokingManager& operator=(const ChartTwoCartesianOrientedAxesYokingManager&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::vector<float> m_horizontalMinimum;
        
        std::vector<float> m_horizontalMaximum;
        
        std::vector<float> m_verticalMinimum;
        
        std::vector<float> m_verticalMaximum;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_AXES_YOKING_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_AXES_YOKING_MANAGER_DECLARE__

} // namespace
#endif  //__CHART_TWO_AXES_YOKING_MANAGER_H__
