#ifndef __SELECTION_ITEM_CHART_TWO_LABEL_H__
#define __SELECTION_ITEM_CHART_TWO_LABEL_H__

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

    class ChartTwoCartesianAxis;
    class ChartTwoOverlaySet;
    
    class SelectionItemChartTwoLabel : public SelectionItem {
        
    public:
        SelectionItemChartTwoLabel();
        
        virtual ~SelectionItemChartTwoLabel();
        
        SelectionItemChartTwoLabel(const SelectionItemChartTwoLabel& obj);

        SelectionItemChartTwoLabel& operator=(const SelectionItemChartTwoLabel& obj);
        
        virtual bool isValid() const override;
        
        virtual void reset() override;
        
        const ChartTwoCartesianAxis* getChartTwoCartesianAxis() const;
        
        ChartTwoCartesianAxis* getChartTwoCartesianAxis();
        
        const ChartTwoOverlaySet* getChartOverlaySet() const;
        
        ChartTwoOverlaySet* getChartOverlaySet();
        
        void setChartTwoCartesianAxis(ChartTwoCartesianAxis* ChartTwoCartesianAxis,
                                      ChartTwoOverlaySet* chartOverlaySet);

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSelectionItemChartTwoLabel(const SelectionItemChartTwoLabel& obj);

        ChartTwoCartesianAxis* m_chartTwoCartesianAxis;
        
        ChartTwoOverlaySet* m_chartTwoOverlaySet;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SELECTION_ITEM_CHART_TWO_LABEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_CHART_TWO_LABEL_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_CHART_TWO_LABEL_H__
