
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

#define __EVENT_CHART_TWO_ATTRIBUTES_CHANGED_DECLARE__
#include "EventChartTwoAttributesChanged.h"
#undef __EVENT_CHART_TWO_ATTRIBUTES_CHANGED_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartTwoAttributesChanged 
 * \brief Event for change in chart two attributes
 * \ingroup Charting
 */

/**
 * Constructor.
 */
EventChartTwoAttributesChanged::EventChartTwoAttributesChanged()
: Event(EventTypeEnum::EVENT_CHART_TWO_ATTRIBUTES_CHANGED)
{
    
}

/**
 * Destructor.
 */
EventChartTwoAttributesChanged::~EventChartTwoAttributesChanged()
{
}

/**
 * @return The mode.
 */
EventChartTwoAttributesChanged::Mode
EventChartTwoAttributesChanged::getMode() const
{
    return m_mode;
}

/**
 * Set for a change in a cartesian axis.
 *
 * @param yokingGroup
 *     Selected yoking group.
 * @param chartTwoDataType
 *     Type of chart.
 * @param cartesianAxis
 *     Cartesian axis that is changed.
 */
void
EventChartTwoAttributesChanged::setCartesianAxisChanged(const YokingGroupEnum::Enum yokingGroup,
                              const ChartTwoDataTypeEnum::Enum chartTwoDataType,
                              const ChartTwoCartesianAxis* cartesianAxis)
{
    CaretAssert(cartesianAxis);
    m_mode             = Mode::CARTESIAN_AXIS;
    m_yokingGroup      = yokingGroup;
    m_chartTwoDataType = chartTwoDataType;
    m_cartesianAxis    = const_cast<ChartTwoCartesianAxis*>(cartesianAxis);
}

/**
 * Get for a change in a cartesian axis.
 *
 * @param yokingGroupOut
 *     Selected yoking group.
 * @param chartTwoDataTypeOut
 *     Type of chart.
 * @param cartesianAxisOut
 *     Cartesian axis that is changed.
 */
void
EventChartTwoAttributesChanged::getCartesianAxisChanged(YokingGroupEnum::Enum &yokingGroupOut,
                              ChartTwoDataTypeEnum::Enum &chartTwoDataTypeOut,
                              ChartTwoCartesianAxis* &cartesianAxisOut) const
{
    CaretAssert(m_mode == Mode::CARTESIAN_AXIS);
    yokingGroupOut      = m_yokingGroup;
    chartTwoDataTypeOut = m_chartTwoDataType;
    cartesianAxisOut    = m_cartesianAxis;
}

/**
 * Set for a change in line thickness.
 *
 * @param yokingGroup
 *     Selected yoking group.
 * @param chartTwoDataType
 *     Type of chart.
 * @param lineThickness
 *     New line thickness
 */
void
EventChartTwoAttributesChanged::setLineThicknessChanged(const YokingGroupEnum::Enum yokingGroup,
                                                        const ChartTwoDataTypeEnum::Enum chartTwoDataType,
                                                        const float lineThickness)
{
    m_mode             = Mode::LINE_THICKESS;
    m_yokingGroup      = yokingGroup;
    m_chartTwoDataType = chartTwoDataType;
    m_lineThickness    = lineThickness;
}

/**
 * Get for a change in line thickness
 *
 * @param yokingGroupOut
 *     Selected yoking group.
 * @param chartTwoDataTypeOut
 *     Type of chart.
 * @param lineThicknessOut
 *     Output line thickness
 */
void
EventChartTwoAttributesChanged::getLineThicknessChanged(YokingGroupEnum::Enum &yokingGroupOut,
                                                        ChartTwoDataTypeEnum::Enum &chartTwoDataTypeOut,
                                                        float& lineThicknessOut) const
{
    CaretAssert(m_mode == Mode::LINE_THICKESS);
    yokingGroupOut      = m_yokingGroup;
    chartTwoDataTypeOut = m_chartTwoDataType;
    lineThicknessOut       = m_lineThickness;
}

/**
 * Set for a change in a title.
 *
 * @param yokingGroup
 *     Selected yoking group.
 * @param chartTwoDataType
 *     Type of chart.
 * @parm chartTitle
 *     Chart title.
 */
void
EventChartTwoAttributesChanged::setTitleChanged(const YokingGroupEnum::Enum yokingGroup,
                                                const ChartTwoDataTypeEnum::Enum chartTwoDataType,
                                                const ChartTwoTitle* chartTitle)
{
    CaretAssert(chartTitle);
    m_mode             = Mode::TITLE;
    m_yokingGroup      = yokingGroup;
    m_chartTwoDataType = chartTwoDataType;
    m_chartTitle       = const_cast<ChartTwoTitle*>(chartTitle);
}

/**
 * Get for a change in title.
 *
 * @param yokingGroupOut
 *     Selected yoking group.
 * @param chartTwoDataTypeOut
 *     Type of chart.
 * @parm chartTitleOut
 *     Chart title.
 */
void
EventChartTwoAttributesChanged::getTitleChanged(YokingGroupEnum::Enum &yokingGroupOut,
                                                ChartTwoDataTypeEnum::Enum &chartTwoDataTypeOut,
                                                ChartTwoTitle* &chartTitleOut) const
{
    CaretAssert(m_mode == Mode::TITLE);
    yokingGroupOut      = m_yokingGroup;
    chartTwoDataTypeOut = m_chartTwoDataType;
    chartTitleOut       = m_chartTitle;
}

