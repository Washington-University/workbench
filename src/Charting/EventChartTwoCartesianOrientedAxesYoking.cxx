
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

#define __EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING_DECLARE__
#include "EventChartTwoCartesianOrientedAxesYoking.h"
#undef __EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING_DECLARE__

#include <limits>

#include "CaretAssert.h"
#include "ChartTwoCartesianOrientedAxes.h"
#include "EventManager.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartTwoCartesianOrientedAxesYoking 
 * \brief Event for getting/setting min/max values for chart axes yoking
 * \ingroup Charting
 */

/**
 * Constructor.
 * @param mode
 *    The mode for the event
 * @param axisOrientation
 *    The axis orientation
 * @param yokingRangeMode
 *    The yoking range mode
 */
EventChartTwoCartesianOrientedAxesYoking::EventChartTwoCartesianOrientedAxesYoking(const Mode mode,
                                                                                   const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                                                   const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode)
: Event(EventTypeEnum::EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING),
m_mode(mode),
m_axisOrientation(axisOrientation),
m_yokingRangeMode(yokingRangeMode)
{
    
}

/**
 * Destructor.
 */
EventChartTwoCartesianOrientedAxesYoking::~EventChartTwoCartesianOrientedAxesYoking()
{
}

/**
 * @return The mode
 */
EventChartTwoCartesianOrientedAxesYoking::Mode
EventChartTwoCartesianOrientedAxesYoking::getMode() const
{
    return m_mode;
}

/**
 * @return The axes orientation
 */
ChartTwoAxisOrientationTypeEnum::Enum
EventChartTwoCartesianOrientedAxesYoking::getAxisOrientation() const
{
    return m_axisOrientation;
}

/**
 * @return The yoking range mode
 */
ChartTwoAxisScaleRangeModeEnum::Enum
EventChartTwoCartesianOrientedAxesYoking::getYokingRangeMode() const
{
    return m_yokingRangeMode;
}

/**
 * Get the minimum and maximum values for an orientation and yoking group
 * @param minimumValueOut
 *    Output with minimum value
 * @param maximumValueOut
 *    Output with maximum value
 * @return True if the output values are valid; false if no axes are yoked to the yokingRangeMode
 */
bool
EventChartTwoCartesianOrientedAxesYoking::getMinimumAndMaximumValues(float& minimumValueOut,
                                                                     float& maximumValueOut) const
{
    minimumValueOut = m_minimumValue;
    maximumValueOut = m_maximumValue;
    return m_valuesValid;
}

/**
 * @return The minimum value
 */
float
EventChartTwoCartesianOrientedAxesYoking::getMinimumValue() const
{
    return m_minimumValue;
}

/**
 * @return The maximum value
 */
float
EventChartTwoCartesianOrientedAxesYoking::getMaximumValue() const
{
    return m_maximumValue;
}

/**
 * Set the minimum and maximum values for an orientation and yoking group
 * and sets the validty for the minimum and maximum values
 * @param minimumValue
 *    Minimum value
 * @param maximumValue
 *    Maximum value
 */
void
EventChartTwoCartesianOrientedAxesYoking::setMinimumAndMaximumValues(const float minimumValue,
                                                                     const float maximumValue)
{
    m_minimumValue = minimumValue;
    m_maximumValue = maximumValue;
    m_valuesValid  = true;
}

/**
 * Add to the yoked axes
 * @param axes
 *    Axes to add
 */
void
EventChartTwoCartesianOrientedAxesYoking::addYokedAxes(ChartTwoCartesianOrientedAxes* axes)
{
    m_yokedAxes.push_back(axes);
}

/**
 * @return Vector containing the yoked axes
 */
std::vector<ChartTwoCartesianOrientedAxes*>
EventChartTwoCartesianOrientedAxesYoking::getYokedAxes() const
{
    return m_yokedAxes;
}


/**
 * Static method to get the yoked axes
 * @param axisOrientation
 *    The axis orientation
 * @param yokingRangeMode
 *    The yoking range mode
 * @return Vector containing the yoked axes
 *
 */
std::vector<ChartTwoCartesianOrientedAxes*>
EventChartTwoCartesianOrientedAxesYoking::getYokedAxes(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                       const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode)
{
    std::vector<ChartTwoCartesianOrientedAxes*> axesOut;
    
    EventChartTwoCartesianOrientedAxesYoking axesEvent(EventChartTwoCartesianOrientedAxesYoking::Mode::GET_YOKED_AXES,
                                                       axisOrientation,
                                                       yokingRangeMode);
    EventManager::get()->sendEvent(axesEvent.getPointer());
    
    return axesEvent.getYokedAxes();
}

/**
 * Static method to get the minimum and maximum from the data in all yoked charts for an orientation and yoking group
 * @param axisOrientation
 *    The axis orientation
 * @param yokingRangeMode
 *    The yoking range mode
 * @param minimumValueOut
 *    Output with minimum value
 * @param maximumValueOut
 *    Output with maximum value
 * @return True if the output values are valid; false if no axes are yoked to the yokingRangeMode
 */
bool
EventChartTwoCartesianOrientedAxesYoking::getDataRangeMinMaxValues(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                                   const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                                                   float& dataMinimumValueOut,
                                                                   float& dataMaximumValueOut)
{
    dataMinimumValueOut = 0.0;
    dataMaximumValueOut = 0.0;
    
    EventChartTwoCartesianOrientedAxesYoking axesEvent(EventChartTwoCartesianOrientedAxesYoking::Mode::GET_YOKED_AXES,
                                                       axisOrientation,
                                                       yokingRangeMode);
    EventManager::get()->sendEvent(axesEvent.getPointer());
    if (axesEvent.getEventProcessCount() > 0) {
        float minValue(std::numeric_limits<float>::max());
        float maxValue(-minValue);
        
        std::vector<ChartTwoCartesianOrientedAxes*> axes = axesEvent.getYokedAxes();
        for (auto a : axes) {
            float minData(0.0), maxData(0.0);
            a->getDataRange(minData, maxData);
            
            if (minData < minValue) minValue = minData;
            if (maxData > maxValue) maxValue = maxData;
        }
        
        if (maxValue > minValue) {
            dataMinimumValueOut = minValue;
            dataMaximumValueOut = maxValue;
            return true;
        }
    }
    
    return false;
}

/**
 * Static method to get the minimum and maximum values for an orientation and yoking group
 * @param axisOrientation
 *    The axis orientation
 * @param yokingRangeMode
 *    The yoking range mode
 * @param minimumValueOut
 *    Output with minimum value
 * @param maximumValueOut
 *    Output with maximum value
 * @return True if the output values are valid; false if no axes are yoked to the yokingRangeMode
 */
bool
EventChartTwoCartesianOrientedAxesYoking::getMinMaxValues(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                          const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                                          float& minimumValueOut,
                                                          float& maximumValueOut)
{
    minimumValueOut = 0.0;
    maximumValueOut = 0.0;
    
    EventChartTwoCartesianOrientedAxesYoking axesEvent(EventChartTwoCartesianOrientedAxesYoking::Mode::GET_MINIMUM_AND_MAXIMUM_VALUES,
                                                       axisOrientation,
                                                       yokingRangeMode);
    EventManager::get()->sendEvent(axesEvent.getPointer());
    if (axesEvent.getEventProcessCount() > 0) {
        minimumValueOut = axesEvent.getMinimumValue();
        maximumValueOut = axesEvent.getMaximumValue();
        return true;
    }
    
    return false;
}

/**
 * Static method to set the minimum and maximum values for an orientation and yoking group
 * @param axisOrientation
 *    The axis orientation
 * @param yokingRangeMode
 *    The yoking range mode
 * @param minimumValue
 *    New minimum value
 * @param maximumValue
 *    New maximum value
 */
void
EventChartTwoCartesianOrientedAxesYoking::setMinMaxValues(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                          const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                                          const float minimumValue,
                                                          const float maximumValue)
{
    EventChartTwoCartesianOrientedAxesYoking axesEvent(EventChartTwoCartesianOrientedAxesYoking::Mode::SET_MINIMUM_AND_MAXIMUM_VALUES,
                                                       axisOrientation,
                                                       yokingRangeMode);
    axesEvent.setMinimumAndMaximumValues(minimumValue,
                                         maximumValue);
    EventManager::get()->sendEvent(axesEvent.getPointer());
}

/**
 * Static method to set the minimum  value for an orientation and yoking group
 * @param axisOrientation
 *    The axis orientation
 * @param yokingRangeMode
 *    The yoking range mode
 * @param minimumValue
 *    New minimum value
 */
void
EventChartTwoCartesianOrientedAxesYoking::setMinimumValue(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                          const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                                          const float minimumValue)
{
    EventChartTwoCartesianOrientedAxesYoking axesEvent(EventChartTwoCartesianOrientedAxesYoking::Mode::SET_MINIMUM_VALUE,
                                                       axisOrientation,
                                                       yokingRangeMode);
    float dummy(0.0);
    axesEvent.setMinimumAndMaximumValues(minimumValue,
                                         dummy);
    EventManager::get()->sendEvent(axesEvent.getPointer());
}

/**
 * Static method to set the maximum value for an orientation and yoking group
 * @param axisOrientation
 *    The axis orientation
 * @param yokingRangeMode
 *    The yoking range mode
 * @param maximumValue
 *    New maximum value
 */
void
EventChartTwoCartesianOrientedAxesYoking::setMaximumValue(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientation,
                                                          const ChartTwoAxisScaleRangeModeEnum::Enum yokingRangeMode,
                                                          const float maximumValue)
{
    EventChartTwoCartesianOrientedAxesYoking axesEvent(EventChartTwoCartesianOrientedAxesYoking::Mode::SET_MAXIMUM_VALUE,
                                                       axisOrientation,
                                                       yokingRangeMode);
    float dummy(0.0);
    axesEvent.setMinimumAndMaximumValues(dummy,
                                         maximumValue);
    EventManager::get()->sendEvent(axesEvent.getPointer());
}
