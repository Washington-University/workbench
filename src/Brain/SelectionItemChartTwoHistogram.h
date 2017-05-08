#ifndef __SELECTION_ITEM_CHART_TWO_HISTOGRAM_H__
#define __SELECTION_ITEM_CHART_TWO_HISTOGRAM_H__

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

    class ChartableTwoFileHistogramChart;
    
    class SelectionItemChartTwoHistogram : public SelectionItem {
        
    public:
        SelectionItemChartTwoHistogram();
        
        virtual ~SelectionItemChartTwoHistogram();
        
        SelectionItemChartTwoHistogram(const SelectionItemChartTwoHistogram& obj);

        SelectionItemChartTwoHistogram& operator=(const SelectionItemChartTwoHistogram& obj);
        
        virtual bool isValid() const override;
        
        virtual void reset() override;
        
        ChartableTwoFileHistogramChart* getFileHistogramChart() const;
        
        int32_t getBucketIndex() const;
        
        int32_t getMapIndex() const;
        
        bool isAllMapsSelected() const;
        
        void setHistogramChart(ChartableTwoFileHistogramChart* fileHistogramChart,
                               const int32_t mapIndex,
                               const int32_t bucketIndex,
                               const bool allMapsSelected);

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSelectionItemChartTwoHistogram(const SelectionItemChartTwoHistogram& obj);

        void resetPrivate();
        
        ChartableTwoFileHistogramChart* m_fileHistogramChart;
        
        int32_t m_mapIndex;
        int32_t m_bucketIndex;
        bool m_allMapsSelectedFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SELECTION_ITEM_CHART_TWO_HISTOGRAM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_CHART_TWO_HISTOGRAM_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_CHART_TWO_HISTOGRAM_H__
