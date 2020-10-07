
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __EVENT_CHART_TWO_CARTESIAN_AXIS_DISPLAY_GROUP_DECLARE__
#include "EventChartTwoCartesianAxisDisplayGroup.h"
#undef __EVENT_CHART_TWO_CARTESIAN_AXIS_DISPLAY_GROUP_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartTwoCartesianAxisDisplayGroup 
 * \brief Event to get an axis associated with a display group
 * \ingroup Charting
 */

/**
 * Constructor.
 * @param displayGroup
 *    Display group for desired axis
 */
EventChartTwoCartesianAxisDisplayGroup::EventChartTwoCartesianAxisDisplayGroup(const DisplayGroupEnum::Enum displayGroup)
: Event(EventTypeEnum::EVENT_CHART_TWO_CARTEISAN_AXIS_DISPLAY_GROUP),
m_displayGroup(displayGroup)
{
    
}

/**
 * Destructor.
 */
EventChartTwoCartesianAxisDisplayGroup::~EventChartTwoCartesianAxisDisplayGroup()
{
}

/**
 * @return The display group
 */
DisplayGroupEnum::Enum
EventChartTwoCartesianAxisDisplayGroup::getDisplayGroup() const
{
    return m_displayGroup;
}

/**
 * @return the axis
 */
ChartTwoCartesianAxis*
EventChartTwoCartesianAxisDisplayGroup::getAxis()
{
    return m_axis;
}

/**
 * @return the axis (const method)
 */
const ChartTwoCartesianAxis*
EventChartTwoCartesianAxisDisplayGroup::getAxis() const
{
    return m_axis;
}

/*
 * Set the axis
 * @param axis
 *   New value for axis
 */
void
EventChartTwoCartesianAxisDisplayGroup::setAxis(ChartTwoCartesianAxis* axis)
{
    m_axis = axis;
}
