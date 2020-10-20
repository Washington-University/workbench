#ifndef __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_LABEL_H__
#define __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_LABEL_H__

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
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ChartTwoCartesianCustomSubdivisionsLabel : public CaretObject, public SceneableInterface {
        
    public:
        ChartTwoCartesianCustomSubdivisionsLabel();
        
        virtual ~ChartTwoCartesianCustomSubdivisionsLabel();
        
        ChartTwoCartesianCustomSubdivisionsLabel(const ChartTwoCartesianCustomSubdivisionsLabel& obj);

        ChartTwoCartesianCustomSubdivisionsLabel& operator=(const ChartTwoCartesianCustomSubdivisionsLabel& obj);

        bool operator<(const ChartTwoCartesianCustomSubdivisionsLabel& customAxisLabel) const;
        
        float getNumericValue() const;
        
        void setNumericValue(const float numericValue);
        
        AString getCustomText() const;
        
        void setCustomText(const AString& customText);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
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
        void copyHelperChartTwoCartesianCustomSubdivisionsLabel(const ChartTwoCartesianCustomSubdivisionsLabel& obj);

        void initializeInstance();
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        float m_numericValue = 0.0;
        
        AString m_customLabelText;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_LABEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_LABEL_DECLARE__

} // namespace
#endif  //__CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_LABEL_H__
