
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
#include "CaretResult.h"
#include "ChartTwoCartesianAxis.h"
#include "EventManager.h"
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
EventChartTwoCartesianAxisDisplayGroup::EventChartTwoCartesianAxisDisplayGroup(const Mode mode,
                                                                               const DisplayGroupEnum::Enum displayGroup,
                                                                               ChartTwoCartesianAxis* axis)
: Event(EventTypeEnum::EVENT_CHART_TWO_CARTEISAN_AXIS_DISPLAY_GROUP),
m_mode(mode),
m_displayGroup(displayGroup),
m_axis(axis)
{
    
}

/**
 * Destructor.
 */
EventChartTwoCartesianAxisDisplayGroup::~EventChartTwoCartesianAxisDisplayGroup()
{
}

/**
 * @return Axis associated with the given display group.  This axis exists
 * in SessionManager but since the Charting module cannot access
 * SessionManager, this axis is obtained throught this event.
 *
 * @param displayGroup (NEVER use TAB)
 *    The display group
 */
ChartTwoCartesianAxis*
EventChartTwoCartesianAxisDisplayGroup::getAxisForDisplayGroup(const DisplayGroupEnum::Enum displayGroup)
{
    switch (displayGroup) {
        case DisplayGroupEnum::DISPLAY_GROUP_A:
        case DisplayGroupEnum::DISPLAY_GROUP_B:
        case DisplayGroupEnum::DISPLAY_GROUP_C:
        case DisplayGroupEnum::DISPLAY_GROUP_D:
            break;
        case DisplayGroupEnum::DISPLAY_GROUP_TAB:
            CaretAssert(0);
            /** Note event below will function for TAB */
            break;
    }
    
    ChartTwoCartesianAxis* nullAxis(NULL);
    EventChartTwoCartesianAxisDisplayGroup event(Mode::GET_DISPLAY_GROUP_AXIS,
                                                 displayGroup,
                                                 nullAxis);
    EventManager::get()->sendEvent(event.getPointer());
    CaretAssert(event.getEventProcessCount());
    return event.getAxis();
}

/**
 * @return All chart axes yoked to the given display group.  Only those axes that
 * are in displayed charts are included.
 * @param displayGroup (NEVER use TAB)
 *    The display group
 */
std::vector<ChartTwoCartesianAxis*>
EventChartTwoCartesianAxisDisplayGroup::getAxesYokedToDisplayGroup(const DisplayGroupEnum::Enum displayGroup)
{
    std::vector<ChartTwoCartesianAxis*> axes;
    switch (displayGroup) {
        case DisplayGroupEnum::DISPLAY_GROUP_A:
        case DisplayGroupEnum::DISPLAY_GROUP_B:
        case DisplayGroupEnum::DISPLAY_GROUP_C:
        case DisplayGroupEnum::DISPLAY_GROUP_D:
        {
            
            ChartTwoCartesianAxis* nullAxis(NULL);
            EventChartTwoCartesianAxisDisplayGroup event(Mode::GET_ALL_YOKED_AXES,
                                                         displayGroup,
                                                         nullAxis);
            EventManager::get()->sendEvent(event.getPointer());
            axes = event.getYokedAxes();
        }
            break;
        case DisplayGroupEnum::DISPLAY_GROUP_TAB:
            CaretAssert(0);
            return axes;
            break;
    }
    
    return axes;
}

/**
 * Initialize the display group axis if it is not yoked to any axes, otherwise, do nothing.
 * @param displayGroup (NEVER use TAB)
 *    The display group
 * @param axis
 *    Axis that is copied to the display group axis.
 */
void
EventChartTwoCartesianAxisDisplayGroup::initializeUnyokedDisplayGroupAxis(const DisplayGroupEnum::Enum displayGroup,
                                                                          const ChartTwoCartesianAxis* axis)
{
    CaretAssert(axis);
    
    switch (displayGroup) {
        case DisplayGroupEnum::DISPLAY_GROUP_A:
        case DisplayGroupEnum::DISPLAY_GROUP_B:
        case DisplayGroupEnum::DISPLAY_GROUP_C:
        case DisplayGroupEnum::DISPLAY_GROUP_D:
        {
            /*
             * Test for NO axes yoked to display group
             */
            std::vector<ChartTwoCartesianAxis*> axes = EventChartTwoCartesianAxisDisplayGroup::getAxesYokedToDisplayGroup(displayGroup);
            if (axes.empty()) {
                ChartTwoCartesianAxis* displayGroupAxis = EventChartTwoCartesianAxisDisplayGroup::getAxisForDisplayGroup(displayGroup);
                if (displayGroupAxis != NULL) {
                    /*
                     * Since no axes yet yoked to display group, copy the given
                     * axis parameters to the display group's axis
                     */
                    displayGroupAxis->copyAxisParameters(axis);
                }
            }
        }
            break;
        case DisplayGroupEnum::DISPLAY_GROUP_TAB:
            CaretAssert(0);
            return;
            break;
    }
}


/**
 * @return The mode
 */
EventChartTwoCartesianAxisDisplayGroup::Mode
EventChartTwoCartesianAxisDisplayGroup::getMode() const
{
    return m_mode;
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

/**
 * @return All yoked axes
 */
std::vector<ChartTwoCartesianAxis*>
EventChartTwoCartesianAxisDisplayGroup::getYokedAxes() const
{
    return m_yokedAxes;
}

/**
 * Add to the yoked axes
 * @param axes
 *    Axes added to the yoked axes
 */
void
EventChartTwoCartesianAxisDisplayGroup::addToYokedAxes(ChartTwoCartesianAxis* axis)
{
    m_yokedAxes.push_back(axis);
}

