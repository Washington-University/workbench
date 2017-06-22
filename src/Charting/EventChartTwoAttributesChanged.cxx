
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
                              ChartTwoCartesianAxis* &cartesianAxisOut)
{
    CaretAssert(m_mode == Mode::CARTESIAN_AXIS);
    yokingGroupOut      = m_yokingGroup;
    chartTwoDataTypeOut = m_chartTwoDataType;
    cartesianAxisOut    = m_cartesianAxis;
}


/**
 * Set for a change in matrix properties.
 *
 * @param yokingGroup
 *     Selected yoking group.
 * @param matrixDisplayProperties
 *     The matrix properties.
 */
void
EventChartTwoAttributesChanged::setMatrixPropertiesChanged(const YokingGroupEnum::Enum yokingGroup,
                                const ChartTwoMatrixDisplayProperties* matrixDisplayProperties)
{
    m_mode        = Mode::MATRIX_PROPERTIES;
    m_yokingGroup = yokingGroup;
    m_matrixDisplayProperties = const_cast<ChartTwoMatrixDisplayProperties*>(matrixDisplayProperties);
}

/**
 * Get for a change in matrix properties.
 *
 * @param yokingGroupOut
 *     Selected yoking group.
 * @param matrixDisplayPropertiesOut
 *     The matrix properties.
 */
void
EventChartTwoAttributesChanged::getMatrixPropertiesChanged(YokingGroupEnum::Enum &yokingGroupOut,
                                ChartTwoMatrixDisplayProperties* &matrixDisplayPropertiesOut)
{
    CaretAssert(m_mode == Mode::MATRIX_PROPERTIES);
    yokingGroupOut = m_yokingGroup;
    matrixDisplayPropertiesOut = m_matrixDisplayProperties;
}


