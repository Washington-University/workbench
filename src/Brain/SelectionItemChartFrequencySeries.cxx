
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

#define __SELECTION_ITEM_CHART_FREQUENCY_SERIES_DECLARE__
#include "SelectionItemChartFrequencySeries.h"
#undef __SELECTION_ITEM_CHART_FREQUENCY_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartFrequencySeries 
 * \brief Contains selection of a data-series chart.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartFrequencySeries::SelectionItemChartFrequencySeries()
: SelectionItem(SelectionItemDataTypeEnum::CHART_FREQUENCY_SERIES)
{
    m_chartModelFrequencySeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemChartFrequencySeries::~SelectionItemChartFrequencySeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartFrequencySeries::SelectionItemChartFrequencySeries(const SelectionItemChartFrequencySeries& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartFrequencySeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartFrequencySeries&
SelectionItemChartFrequencySeries::operator=(const SelectionItemChartFrequencySeries& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartFrequencySeries(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartFrequencySeries::copyHelperSelectionItemChartFrequencySeries(const SelectionItemChartFrequencySeries& obj)
{
    m_chartModelFrequencySeries = obj.m_chartModelFrequencySeries;
    m_chartDataCartesian  = obj.m_chartDataCartesian;
    m_chartDataPointIndex = obj.m_chartDataPointIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartFrequencySeries::isValid() const
{
    if ((m_chartModelFrequencySeries != NULL)
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
SelectionItemChartFrequencySeries::reset()
{
    m_chartModelFrequencySeries = NULL;
    m_chartDataCartesian  = NULL;
    m_chartDataPointIndex = -1;
}

/**
 * @return
 */
ChartModelFrequencySeries*
SelectionItemChartFrequencySeries::getChartModelFrequencySeries() const
{
    return m_chartModelFrequencySeries;
}

/**
 * @return
 */
ChartDataCartesian*
SelectionItemChartFrequencySeries::getChartDataCartesian() const
{
    return m_chartDataCartesian;
}

/**
 * @return
 */
int32_t
SelectionItemChartFrequencySeries::getChartDataPointIndex() const
{
    return m_chartDataPointIndex;
}

/**
 * Set the selected chart information.
 *
 * @param chartModelFrequencySeries
 *     Frequency series chart model that was selected.
 * @param chartDataCartesian
 *     Cartesian chart data that was selected.
 * @param chartDataPointIndex
 *     Point index of selected chart data.
 */
void
SelectionItemChartFrequencySeries::setChart(ChartModelFrequencySeries* chartModelFrequencySeries,
                                       ChartDataCartesian* chartDataCartesian,
                                       const int32_t chartDataPointIndex)
{
    CaretAssert(chartModelFrequencySeries);
    CaretAssert(chartDataCartesian);
    CaretAssert(chartDataPointIndex >= 0);

    m_chartModelFrequencySeries = chartModelFrequencySeries;
    m_chartDataCartesian   = chartDataCartesian;
    m_chartDataPointIndex  = chartDataPointIndex;
}



