
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

#define __SELECTION_ITEM_CHART_TWO_LINE_LAYER_DECLARE__
#include "SelectionItemChartTwoLineLayer.h"
#undef __SELECTION_ITEM_CHART_TWO_LINE_LAYER_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTwoLineLayer
 * \brief Selection of chart version two line layer
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTwoLineLayer::SelectionItemChartTwoLineLayer()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TWO_LINE_LAYER)
{
    m_fileLineLayerChart = NULL;
    m_chartTwoCartesianData = NULL;
    m_lineSegmentIndex    = -1;
}

/**
 * Destructor.
 */
SelectionItemChartTwoLineLayer::~SelectionItemChartTwoLineLayer()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTwoLineLayer::SelectionItemChartTwoLineLayer(const SelectionItemChartTwoLineLayer& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTwoLineLayer(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTwoLineLayer&
SelectionItemChartTwoLineLayer::operator=(const SelectionItemChartTwoLineLayer& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTwoLineLayer(obj);
    }
    return *this;    
}

/**
 * @return The File line layer chart.
 */
const ChartableTwoFileLineLayerChart*
SelectionItemChartTwoLineLayer::getFileLineLayerChart() const
{
    return m_fileLineLayerChart;
}

/**
 * @return The cartesian data.
 */
const ChartTwoDataCartesian*
SelectionItemChartTwoLineLayer::getChartTwoCartesianData() const
{
    return m_chartTwoCartesianData;
}

/**
 * @return The line segment index.
 */
int32_t
SelectionItemChartTwoLineLayer::getLineSegmentIndex() const
{
    return m_lineSegmentIndex;
}

/**
 * Set selection.
 *
 * @param fileLineLayerChart
 *     The line layer chart.
 * @param chartTwoCartesianData
 *     The cartesian data.
 * @param lineSegmentIndex
 *     Index of the line segment.
 */
void
SelectionItemChartTwoLineLayer::setLineLayerChart(ChartableTwoFileLineLayerChart* fileLineLayerChart,
                                                    ChartTwoDataCartesian* chartTwoCartesianData,
                                                    const int32_t lineSegmentIndex)
{
    m_fileLineLayerChart = fileLineLayerChart;
    m_chartTwoCartesianData = chartTwoCartesianData;
    m_lineSegmentIndex    = lineSegmentIndex;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTwoLineLayer::copyHelperSelectionItemChartTwoLineLayer(const SelectionItemChartTwoLineLayer& obj)
{
    m_fileLineLayerChart = obj.m_fileLineLayerChart;
    m_chartTwoCartesianData = obj.m_chartTwoCartesianData;
    m_lineSegmentIndex    = obj.m_lineSegmentIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartTwoLineLayer::isValid() const
{
    return ((m_fileLineLayerChart != NULL)
            && (m_chartTwoCartesianData != NULL)
            && (m_lineSegmentIndex >= 0));
}

/**
 * Reset the selections.
 */
void
SelectionItemChartTwoLineLayer::reset()
{
    m_fileLineLayerChart = NULL;
    m_chartTwoCartesianData = NULL;
    m_lineSegmentIndex    = -1;
}

