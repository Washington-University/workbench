
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

#define __SELECTION_ITEM_CHART_TWO_LINE_SERIES_DECLARE__
#include "SelectionItemChartTwoLineSeries.h"
#undef __SELECTION_ITEM_CHART_TWO_LINE_SERIES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTwoLineSeries 
 * \brief Selection of chart version two line series
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTwoLineSeries::SelectionItemChartTwoLineSeries()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TWO_LINE_SERIES)
{
    m_fileLineSeriesChart = NULL;
    m_lineSegmentIndex    = -1;
}

/**
 * Destructor.
 */
SelectionItemChartTwoLineSeries::~SelectionItemChartTwoLineSeries()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTwoLineSeries::SelectionItemChartTwoLineSeries(const SelectionItemChartTwoLineSeries& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTwoLineSeries(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTwoLineSeries&
SelectionItemChartTwoLineSeries::operator=(const SelectionItemChartTwoLineSeries& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTwoLineSeries(obj);
    }
    return *this;    
}

/**
 * @return The File line series chart.
 */
ChartableTwoFileLineSeriesChart*
SelectionItemChartTwoLineSeries::getFileLineSeriesChart() const
{
    return m_fileLineSeriesChart;
}

/**
 * @return The line segment index.
 */
int32_t
SelectionItemChartTwoLineSeries::getLineSegmentIndex() const
{
    return m_lineSegmentIndex;
}

/**
 * Set selection.
 *
 * @param fileLineSeriesChart
 *     The line series chart.
 * @param lineSegmentIndex
 *     Index of the line segment.
 */
void
SelectionItemChartTwoLineSeries::setLineSeriesChart(ChartableTwoFileLineSeriesChart* fileLineSeriesChart,
                                                    const int32_t lineSegmentIndex)
{
    m_fileLineSeriesChart = fileLineSeriesChart;
    m_lineSegmentIndex    = lineSegmentIndex;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTwoLineSeries::copyHelperSelectionItemChartTwoLineSeries(const SelectionItemChartTwoLineSeries& obj)
{
    m_fileLineSeriesChart = obj.m_fileLineSeriesChart;
    m_lineSegmentIndex    = obj.m_lineSegmentIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartTwoLineSeries::isValid() const
{
    return ((m_fileLineSeriesChart != NULL)
            && (m_lineSegmentIndex >= 0));
}

/**
 * Reset the selections.
 */
void
SelectionItemChartTwoLineSeries::reset()
{
    m_fileLineSeriesChart = NULL;
    m_lineSegmentIndex    = -1;
}

