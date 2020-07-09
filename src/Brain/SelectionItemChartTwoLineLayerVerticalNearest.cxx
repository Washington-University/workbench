
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

#define __SELECTION_ITEM_CHART_TWO_LINE_LAYER_VERTICAL_NEAREST_DECLARE__
#include "SelectionItemChartTwoLineLayerVerticalNearest.h"
#undef __SELECTION_ITEM_CHART_TWO_LINE_LAYER_VERTICAL_NEAREST_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTwoLineLayerVerticalNearest
 * \brief Selection of chart version two line layer for nearest vertical distance
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTwoLineLayerVerticalNearest::SelectionItemChartTwoLineLayerVerticalNearest()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TWO_LINE_LAYER_VERTICAL_NEAREST)
{
    m_fileLineLayerChart     = NULL;
    m_chartTwoCartesianData  = NULL;
    m_chartOverlay           = NULL;
    m_distanceToLine         = std::numeric_limits<float>::max();
    m_lineSegmentIndex       = -1;
    m_outsideChartBoundsFlag = false;
}

/**
 * Destructor.
 */
SelectionItemChartTwoLineLayerVerticalNearest::~SelectionItemChartTwoLineLayerVerticalNearest()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTwoLineLayerVerticalNearest::SelectionItemChartTwoLineLayerVerticalNearest(const SelectionItemChartTwoLineLayerVerticalNearest& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTwoLineLayerVerticalNearest(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTwoLineLayerVerticalNearest&
SelectionItemChartTwoLineLayerVerticalNearest::operator=(const SelectionItemChartTwoLineLayerVerticalNearest& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTwoLineLayerVerticalNearest(obj);
    }
    return *this;    
}

/**
 * @return The File line layer chart.
 */
const ChartableTwoFileLineLayerChart*
SelectionItemChartTwoLineLayerVerticalNearest::getFileLineLayerChart() const
{
    return m_fileLineLayerChart;
}

/**
 * @return The cartesian data.
 */
const ChartTwoDataCartesian*
SelectionItemChartTwoLineLayerVerticalNearest::getChartTwoCartesianData() const
{
    return m_chartTwoCartesianData;
}

ChartTwoOverlay*
SelectionItemChartTwoLineLayerVerticalNearest::getChartTwoOverlay()
{
    return m_chartOverlay;
}

/**
 * @return Distance to line segment
 */
float
SelectionItemChartTwoLineLayerVerticalNearest::getDistanceToLine() const
{
    return m_distanceToLine;
}

/**
 * @return The line segment index.
 */
int32_t
SelectionItemChartTwoLineLayerVerticalNearest::getLineSegmentIndex() const
{
    return m_lineSegmentIndex;
}

/**
 * Set selection.  Replaces current if distance is less than current distance.
 *
 * @param fileLineLayerChart
 *     The line layer chart.
 * @param chartTwoCartesianData
 *     The cartesian data.
 * @param chartOverlay
 *     The chart overlay
 * @param distanceToLine
 *     Distance to the line
 * @param lineSegmentIndex
 *     Index of the line segment.
 */
void
SelectionItemChartTwoLineLayerVerticalNearest::setLineLayerChart(ChartableTwoFileLineLayerChart* fileLineLayerChart,
                                                                 ChartTwoDataCartesian* chartTwoCartesianData,
                                                                 ChartTwoOverlay* chartOverlay,
                                                                 const float distanceToLine,
                                                                 const int32_t lineSegmentIndex)
{
    if (distanceToLine < m_distanceToLine) {
        m_fileLineLayerChart    = fileLineLayerChart;
        m_chartTwoCartesianData = chartTwoCartesianData;
        m_chartOverlay          = chartOverlay;
        m_distanceToLine        = distanceToLine;
        m_lineSegmentIndex      = lineSegmentIndex;
    }
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTwoLineLayerVerticalNearest::copyHelperSelectionItemChartTwoLineLayerVerticalNearest(const SelectionItemChartTwoLineLayerVerticalNearest& obj)
{
    m_fileLineLayerChart     = obj.m_fileLineLayerChart;
    m_chartTwoCartesianData  = obj.m_chartTwoCartesianData;
    m_chartOverlay           = obj.m_chartOverlay;
    m_distanceToLine         = obj.m_distanceToLine;
    m_lineSegmentIndex       = obj.m_lineSegmentIndex;
    m_outsideChartBoundsFlag = obj.m_outsideChartBoundsFlag;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartTwoLineLayerVerticalNearest::isValid() const
{
    return ((m_fileLineLayerChart != NULL)
            && (m_chartTwoCartesianData != NULL)
            && (m_chartOverlay != NULL)
            && (m_lineSegmentIndex >= 0));
}

/**
 * Reset the selections.
 */
void
SelectionItemChartTwoLineLayerVerticalNearest::reset()
{
    m_fileLineLayerChart     = NULL;
    m_chartTwoCartesianData  = NULL;
    m_chartOverlay           = NULL;
    m_distanceToLine         = std::numeric_limits<float>::max();
    m_lineSegmentIndex       = -1;
    m_outsideChartBoundsFlag = false;
}

/**
 * @return True if mouse was in the chart viewport but outside the bounds of the chart coordinates.
 * Note isValid() will return false since mouse was not in chart bounds.
 */
bool
SelectionItemChartTwoLineLayerVerticalNearest::isOutsideChartBounds() const
{
    return m_outsideChartBoundsFlag;
}

/**
 * Set mouse was in the chart viewport but outside the bounds of the chart coordinates.
 * @param status
 *     New status
 */
void
SelectionItemChartTwoLineLayerVerticalNearest::setOutsideChartBound(const bool status)
{
    m_outsideChartBoundsFlag = status;
}

