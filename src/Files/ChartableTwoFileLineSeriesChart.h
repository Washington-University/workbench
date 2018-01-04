#ifndef __CHARTABLE_TWO_FILE_LINE_SERIES_CHART_H__
#define __CHARTABLE_TWO_FILE_LINE_SERIES_CHART_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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
#include "ChartableTwoFileBaseChart.h"
#include "ChartTwoLineSeriesContentTypeEnum.h"


namespace caret {

    class ChartTwoLineSeriesHistory;
    class EventChartTwoLoadLineSeriesData;
    
    class ChartableTwoFileLineSeriesChart : public ChartableTwoFileBaseChart {
        
    public:
        ChartableTwoFileLineSeriesChart(const ChartTwoLineSeriesContentTypeEnum::Enum lineSeriesContentType,
                                                CaretMappableDataFile* parentCaretMappableDataFile);
        
        virtual ~ChartableTwoFileLineSeriesChart();
        
        ChartTwoLineSeriesContentTypeEnum::Enum getLineSeriesContentType() const;
        
        ChartTwoLineSeriesHistory* getHistory();
        
        const ChartTwoLineSeriesHistory* getHistory() const;
        
        virtual bool isValid() const override;
        
        virtual bool isEmpty() const override;
        
        virtual void receiveEvent(Event* event) override;
        
        void loadDataForRowOrColumn(const int32_t tabIndex,
                                    const int32_t rowOrColumnIndex);

        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) override;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) override;

    private:
        void loadLineCharts(const EventChartTwoLoadLineSeriesData* lineSeriesDataEvent);
        
        ChartableTwoFileLineSeriesChart(const ChartableTwoFileLineSeriesChart&);

        ChartableTwoFileLineSeriesChart& operator=(const ChartableTwoFileLineSeriesChart&);
        
        std::vector<int32_t> getTabIndicesForLoadingData(const std::vector<int32_t>& validTabIndices) const;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        ChartTwoLineSeriesContentTypeEnum::Enum m_lineSeriesContentType;
        
        std::unique_ptr<ChartTwoLineSeriesHistory> m_lineChartHistory;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_FILE_LINE_SERIES_CHART_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_FILE_LINE_SERIES_CHART_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_LINE_SERIES_CHART_H__
