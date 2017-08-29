
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

#define __EVENT_CHART_TWO_AXIS_GET_DATA_RANGE_DECLARE__
#include "EventChartTwoAxisGetDataRange.h"
#undef __EVENT_CHART_TWO_AXIS_GET_DATA_RANGE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartTwoAxisGetDataRange 
 * \brief Event to get the range of data for a chart two axis
 * \ingroup Charting
 */

/**
 * Constructor.
 */
EventChartTwoAxisGetDataRange::EventChartTwoAxisGetDataRange(const ChartTwoOverlaySet* chartOverlaySet,
                                                             const ChartAxisLocationEnum::Enum chartAxisLocation)
: Event(EventTypeEnum::EVENT_CHART_TWO_AXIS_GET_DATA_RANGE),
m_chartOverlaySet(chartOverlaySet),
m_chartAxisLocation(chartAxisLocation)
{
    
}

/**
 * Destructor.
 */
EventChartTwoAxisGetDataRange::~EventChartTwoAxisGetDataRange()
{
}

/**
 * @return The chart overlay set that contains the axis.
 */
const ChartTwoOverlaySet*
EventChartTwoAxisGetDataRange::getChartOverlaySet() const
{
    return m_chartOverlaySet;
}

/**
 * @return The location of the axis.
 */
ChartAxisLocationEnum::Enum
EventChartTwoAxisGetDataRange::getChartAxisLocation() const
{
    return m_chartAxisLocation;
}

/**
 * Get the minimum and maximum values.
 *
 * @param minimumValue
 *    The minimum value.
 * @param maximumValue
 *    The maximum value.
 * @return 
 *    True if the minimum and maximum are valid, else false.
 */
bool
EventChartTwoAxisGetDataRange::getMinimumAndMaximumValues(float& minimumValue,
                                                          float& maximumValue) const
{
    minimumValue = m_minimumValue;
    maximumValue = m_maximumValue;
    
    return m_valuesValidFlag;
}


/**
 * Set the minimum and maximum values.
 *
 * @param minimumValue
 *    The minimum value.
 * @param maximumValue
 *    The maximum value.
 */
void EventChartTwoAxisGetDataRange::setMinimumAndMaximumValues(const float minimumValue,
                                                               const float maximumValue)
{
    m_minimumValue = minimumValue;
    m_maximumValue = maximumValue;
    m_valuesValidFlag = true;
}
