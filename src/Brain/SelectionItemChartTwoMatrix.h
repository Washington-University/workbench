#ifndef __SELECTION_ITEM_CHART_TWO_MATRIX_H__
#define __SELECTION_ITEM_CHART_TWO_MATRIX_H__

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

    class ChartableTwoFileMatrixChart;
    
    class SelectionItemChartTwoMatrix : public SelectionItem {
        
    public:
        SelectionItemChartTwoMatrix();
        
        virtual ~SelectionItemChartTwoMatrix();
        
        SelectionItemChartTwoMatrix(const SelectionItemChartTwoMatrix& obj);

        SelectionItemChartTwoMatrix& operator=(const SelectionItemChartTwoMatrix& obj);
        
        virtual bool isValid() const override;
        
        virtual void reset() override;
        
        ChartableTwoFileMatrixChart* getFileMatrixChart() const;
        
        int32_t getRowIndex() const;
        
        int32_t getColumnIndex() const;
        
        void setMatrixChart(ChartableTwoFileMatrixChart* fileMatrixChart,
                            const int32_t rowIndex,
                            const int32_t columnIndex);

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSelectionItemChartTwoMatrix(const SelectionItemChartTwoMatrix& obj);

        ChartableTwoFileMatrixChart* m_fileMatrixChart;
        
        int32_t m_rowIndex;
        
        int32_t m_columnIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SELECTION_ITEM_CHART_TWO_MATRIX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_CHART_TWO_MATRIX_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_CHART_TWO_MATRIX_H__
