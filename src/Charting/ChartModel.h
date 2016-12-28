#ifndef __CHART_MODEL_H__
#define __CHART_MODEL_H__

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

#include <QSharedPointer>

#include <deque>

#include "CaretObject.h"
#include "ChartVersionOneDataTypeEnum.h"
#include "ChartSelectionModeEnum.h"
#include "SceneableInterface.h"


namespace caret {

    class ChartAxis;
    class ChartData;
    class SceneClassAssistant;
    
    class ChartModel : public CaretObject, public SceneableInterface {
        
    public:
        ChartModel(const ChartVersionOneDataTypeEnum::Enum chartDataType,
                   const ChartSelectionModeEnum::Enum chartSelectionMode);
        
        ChartModel(const ChartModel&);
        
        ChartModel& operator=(const ChartModel&);
        
        virtual ~ChartModel();
        
        void removeChartData();
        
        ChartVersionOneDataTypeEnum::Enum getChartDataType() const;
        
        ChartSelectionModeEnum::Enum getChartSelectionMode() const;
        
        void addChartData(const QSharedPointer<ChartData>& chartData);
        
        virtual int32_t getMaximumNumberOfChartDatasToDisplay() const;
        
        void setMaximumNumberOfChartDatasToDisplay(const int32_t numberToDisplay);
        
        bool isEmpty() const;
        
        std::vector<const ChartData*> getAllChartDatas() const;
        
        std::vector<ChartData*> getAllChartDatas();
        
        std::vector<const ChartData*> getAllSelectedChartDatas(const int32_t tabIndex) const;
        
        int32_t getNumberOfChartData() const;
        
        ChartData* getChartDataAtIndex(const int32_t chartDataIndex);
        
        const ChartData* getChartDataAtIndex(const int32_t chartDataIndex) const;
        
        void moveChartDataAtIndexToOneLowerIndex(const int32_t chartDataIndex);
        
        void moveChartDataAtIndexToOneHigherIndex(const int32_t chartDataIndex);
        
        void removeChartAtIndex(const int32_t chartDataIndex);
        
        /**
         * @return Is an average of data supported?
         */
        virtual bool isAverageChartDisplaySupported() const = 0;
        
        /**
         * Get the average for charts in the given tab.
         *
         * @param tabIndex
         *     Index of the tab.
         *
         * @return
         *     The average chart data.  Will return NULL if either
         * no data to average or model does not support an average.
         * Includes only those chart data that are displayed.
         */
        virtual const ChartData* getAverageChartDataForDisplay(const int32_t tabIndex) const = 0;
        
        bool isAverageChartDisplaySelected() const;
        
        void setAverageChartDisplaySelected(const bool selected);
        
        ChartAxis* getLeftAxis();
        
        const ChartAxis* getLeftAxis() const;

        ChartAxis* getRightAxis();
        
        const ChartAxis* getRightAxis() const;
        
        ChartAxis* getBottomAxis();
        
        const ChartAxis* getBottomAxis() const;
        
        ChartAxis* getTopAxis();
        
        const ChartAxis* getTopAxis() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        void restoreChartDataFromScene(const SceneAttributes* sceneAttributes,
                                       std::vector<QSharedPointer<ChartData> >& restoredChartData);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    protected:
        void setBottomAxis(ChartAxis* leftAxis);
        
        void setLeftAxis(ChartAxis* leftAxis);
        
        void setRightAxis(ChartAxis* leftAxis);
        
        void setTopAxis(ChartAxis* leftAxis);
        
        virtual void updateAfterChartDataHasBeenAddedOrRemoved();
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) = 0;
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) = 0;
        
    private:
        void copyHelperChartModel(const ChartModel& obj);
        
        void childChartDataSelectionChanged(ChartData* childChartData);
        
        void removeChartDataPrivate();
        
        void removeAllAxes();
        
        void updateUsingMaximumNumberOfChartDatasToDisplay();
        
        ChartVersionOneDataTypeEnum::Enum m_chartDataType;
        
        ChartSelectionModeEnum::Enum m_chartSelectionMode;
        
        std::deque<QSharedPointer<ChartData> > m_chartDatas;
        
        int32_t m_maximumNumberOfChartDatasToDisplay;
        
        ChartAxis* m_leftAxis;
        
        ChartAxis* m_rightAxis;
        
        ChartAxis* m_bottomAxis;
        
        ChartAxis* m_topAxis;
        
        std::vector<AString> m_chartDataUniqueIDsRestoredFromScene;
        
        bool m_averageChartDisplaySelected;
        
        /** helps with scene save/restore */
        SceneClassAssistant* m_sceneAssistant;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ChartData;
    };
    
#ifdef __CHART_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_MODEL_DECLARE__

} // namespace
#endif  //__CHART_MODEL_H__
