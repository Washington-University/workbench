#ifndef __SELECTION_ITEM_CHART_TWO_LINE_LAYER_VERTICAL_NEAREST_H__
#define __SELECTION_ITEM_CHART_TWO_LINE_LAYER_VERTICAL_NEAREST_H__

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


#include "SelectionItem.h"



namespace caret {

    class ChartTwoDataCartesian;
    class ChartTwoOverlay;
    class ChartableTwoFileLineLayerChart;
    
    class SelectionItemChartTwoLineLayerVerticalNearest : public SelectionItem {
        
    public:
        SelectionItemChartTwoLineLayerVerticalNearest();
        
        virtual ~SelectionItemChartTwoLineLayerVerticalNearest();
        
        SelectionItemChartTwoLineLayerVerticalNearest(const SelectionItemChartTwoLineLayerVerticalNearest& obj);

        SelectionItemChartTwoLineLayerVerticalNearest& operator=(const SelectionItemChartTwoLineLayerVerticalNearest& obj);
        
        virtual bool isValid() const override;
        
        virtual void reset() override;
        
        const ChartableTwoFileLineLayerChart* getFileLineLayerChart() const;
        
        const ChartTwoDataCartesian* getChartTwoCartesianData() const;
        
        ChartTwoOverlay* getChartTwoOverlay();
        
        int32_t getLineSegmentIndex() const;
        
        float getDistanceToLine() const;
        
        void setLineLayerChart(ChartableTwoFileLineLayerChart* fileLineLayerChart,
                               ChartTwoDataCartesian* chartTwoCartesianData,
                               ChartTwoOverlay* chartOverlay,
                               const float distanceToLine,
                               const int32_t lineSegmentIndex);

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSelectionItemChartTwoLineLayerVerticalNearest(const SelectionItemChartTwoLineLayerVerticalNearest& obj);

        ChartableTwoFileLineLayerChart* m_fileLineLayerChart = NULL;
        
        ChartTwoDataCartesian* m_chartTwoCartesianData = NULL;
        
        ChartTwoOverlay* m_chartOverlay = NULL;
        
        float m_distanceToLine = 0;
        
        int32_t m_lineSegmentIndex = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SELECTION_ITEM_CHART_TWO_LINE_LAYER_VERTICAL_NEAREST_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_CHART_TWO_LINE_LAYER_VERTICAL_NEAREST_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_CHART_TWO_LINE_LAYER_VERTICAL_NEAREST_H__
