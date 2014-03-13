
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

#define __SELECTION_ITEM_CHART_DATA_SERIES_DECLARE__
#include "SelectionItemChartDataSeries.h"
#undef __SELECTION_ITEM_CHART_DATA_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartDataSeries 
 * \brief Contains selection of a data-series chart.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartDataSeries::SelectionItemChartDataSeries()
: SelectionItem(SelectionItemDataTypeEnum::CHART_DATA_SERIES)
{
    m_chartModelDataSeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemChartDataSeries::~SelectionItemChartDataSeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartDataSeries::SelectionItemChartDataSeries(const SelectionItemChartDataSeries& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartDataSeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartDataSeries&
SelectionItemChartDataSeries::operator=(const SelectionItemChartDataSeries& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartDataSeries(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartDataSeries::copyHelperSelectionItemChartDataSeries(const SelectionItemChartDataSeries& obj)
{
    m_chartModelDataSeries = obj.m_chartModelDataSeries;
    m_chartDataCartesian  = obj.m_chartDataCartesian;
    m_chartDataPointIndex = obj.m_chartDataPointIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartDataSeries::isValid() const
{
    if ((m_chartModelDataSeries != NULL)
        && (m_chartDataCartesian != NULL)
        && (m_chartDataPointIndex >= 0)) {
        return true;
    }
    
    return false;
}

/**
 * Reset the selections.
 */
void
SelectionItemChartDataSeries::reset()
{
    m_chartModelDataSeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * @return
 */
ChartModelDataSeries*
SelectionItemChartDataSeries::getChartModelDataSeries() const
{
    return m_chartModelDataSeries;
}

/**
 * @return
 */
ChartDataCartesian*
SelectionItemChartDataSeries::getChartDataCartesian() const
{
    return m_chartDataCartesian;
}

/**
 * @return
 */
int32_t
SelectionItemChartDataSeries::getChartDataPointIndex() const
{
    return m_chartDataPointIndex;
}

/**
 * Set the selected chart information.
 *
 * @param chartModelDataSeries
 *     Data series chart model that was selected.
 * @param chartDataCartesian
 *     Cartesian chart data that was selected.
 * @param chartDataPointIndex
 *     Point index of selected chart data.
 */
void
SelectionItemChartDataSeries::setChart(ChartModelDataSeries* chartModelDataSeries,
                                       ChartDataCartesian* chartDataCartesian,
                                       const int32_t chartDataPointIndex)
{
    CaretAssert(chartModelDataSeries);
    CaretAssert(chartDataCartesian);
    CaretAssert(chartDataPointIndex >= 0);

    m_chartModelDataSeries = chartModelDataSeries;
    m_chartDataCartesian   = chartDataCartesian;
    m_chartDataPointIndex  = chartDataPointIndex;
}



