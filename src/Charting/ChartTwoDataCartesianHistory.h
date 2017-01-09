#ifndef __CHART_TWO_DATA_CARTESIAN_HISTORY_H__
#define __CHART_TWO_DATA_CARTESIAN_HISTORY_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include <deque>

#include "BrainConstants.h"
#include "CaretColorEnum.h"
#include "CaretObjectTracksModification.h"

#include "SceneableInterface.h"


namespace caret {
    class ChartTwoDataCartesian;
    class SceneClassAssistant;

    class ChartTwoDataCartesianHistory : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        ChartTwoDataCartesianHistory();
        
        virtual ~ChartTwoDataCartesianHistory();
        
        ChartTwoDataCartesianHistory(const ChartTwoDataCartesianHistory& obj);

        ChartTwoDataCartesianHistory& operator=(const ChartTwoDataCartesianHistory& obj);

        CaretColorEnum::Enum getDefaultColor() const;
        
        void setDefaultColor(const CaretColorEnum::Enum defaultColor);
        
        int32_t getDisplayCountInTab(const int32_t tabIndex) const;
        
        void setDisplayCountInTab(const int32_t tabIndex,
                                  const int count);
        
        int32_t getHistoryCount();

        void addHistoryItem(ChartTwoDataCartesian* historyItem);
        
        ChartTwoDataCartesian* getHistoryItem(const int32_t index);
        
        const ChartTwoDataCartesian* getHistoryItem(const int32_t index) const;
        
        void clearHistory();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const override;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) override;

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) override;

        virtual bool isModified() const override;
        
        virtual void clearModified() override;
          
          
          
          
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
        void copyHelperChartTwoDataCartesianHistory(const ChartTwoDataCartesianHistory& obj);

        void initializeInstance();
        
        SceneClassAssistant* m_sceneAssistant = NULL;

        std::deque<ChartTwoDataCartesian*> m_chartHistory;
        
        CaretColorEnum::Enum m_defaultColor = CaretColorEnum::BLUE;
        
        int32_t m_displayCountInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_DATA_CARTESIAN_HISTORY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_DATA_CARTESIAN_HISTORY_DECLARE__

} // namespace
#endif  //__CHART_TWO_DATA_CARTESIAN_HISTORY_H__
