#ifndef __SELECTION_ITEM_CHART_MATRIX_H__
#define __SELECTION_ITEM_CHART_MATRIX_H__

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


#include "SelectionItem.h"



namespace caret {
    class ChartableMatrixInterface;
    
    class SelectionItemChartMatrix : public SelectionItem {
        
    public:
        SelectionItemChartMatrix();
        
        virtual ~SelectionItemChartMatrix();
        
        SelectionItemChartMatrix(const SelectionItemChartMatrix& obj);

        SelectionItemChartMatrix& operator=(const SelectionItemChartMatrix& obj);
        
        virtual bool isValid() const;
        
        virtual void reset();
        
        ChartableMatrixInterface* getChartableMatrixInterface() const;
        
        int32_t getMatrixRowIndex() const;
        
        int32_t getMatrixColumnIndex() const;
        
        void setChartMatrix(ChartableMatrixInterface* chartableMatrixInterface,
                            const int32_t matrixRowIndex,
                            const int32_t matrixColumnIndex);
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSelectionItemChartMatrix(const SelectionItemChartMatrix& obj);

        ChartableMatrixInterface* m_chartableMatrixInterface;
        
        int32_t m_matrixRowIndex;
        
        int32_t m_matrixColumnIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SELECTION_ITEM_CHART_MATRIX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_CHART_MATRIX_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_CHART_MATRIX_H__
