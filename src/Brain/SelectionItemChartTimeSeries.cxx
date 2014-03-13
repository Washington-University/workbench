
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

#define __SELECTION_ITEM_CHART_TIME_SERIES_DECLARE__
#include "SelectionItemChartTimeSeries.h"
#undef __SELECTION_ITEM_CHART_TIME_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTimeSeries 
 * \brief Contains selection of a time-series chart.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTimeSeries::SelectionItemChartTimeSeries()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TIME_SERIES)
{
    m_chartModelTimeSeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemChartTimeSeries::~SelectionItemChartTimeSeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTimeSeries::SelectionItemChartTimeSeries(const SelectionItemChartTimeSeries& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTimeSeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTimeSeries&
SelectionItemChartTimeSeries::operator=(const SelectionItemChartTimeSeries& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTimeSeries(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTimeSeries::copyHelperSelectionItemChartTimeSeries(const SelectionItemChartTimeSeries& obj)
{
    m_chartModelTimeSeries = obj.m_chartModelTimeSeries;
    m_chartDataCartesian  = obj.m_chartDataCartesian;
    m_chartDataPointIndex = obj.m_chartDataPointIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartTimeSeries::isValid() const
{
    if ((m_chartModelTimeSeries != NULL)
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
SelectionItemChartTimeSeries::reset()
{
    m_chartModelTimeSeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * @return
 */
ChartModelTimeSeries*
SelectionItemChartTimeSeries::getChartModelTimeSeries() const
{
    return m_chartModelTimeSeries;
}

/**
 * @return
 */
ChartDataCartesian*
SelectionItemChartTimeSeries::getChartDataCartesian() const
{
    return m_chartDataCartesian;
}

/**
 * @return
 */
int32_t
SelectionItemChartTimeSeries::getChartDataPointIndex() const
{
    return m_chartDataPointIndex;
}

/**
 * Set the selected chart information.
 *
 * @param chartModelTimeSeries
 *     Data series chart model that was selected.
 * @param chartDataCartesian
 *     Cartesian chart data that was selected.
 * @param chartDataPointIndex
 *     Point index of selected chart data.
 */
void
SelectionItemChartTimeSeries::setChart(ChartModelTimeSeries* chartModelTimeSeries,
                                       ChartDataCartesian* chartDataCartesian,
                                       const int32_t chartDataPointIndex)
{
    CaretAssert(chartModelTimeSeries);
    CaretAssert(chartDataCartesian);
    CaretAssert(chartDataPointIndex >= 0);

    m_chartModelTimeSeries = chartModelTimeSeries;
    m_chartDataCartesian   = chartDataCartesian;
    m_chartDataPointIndex  = chartDataPointIndex;
}



