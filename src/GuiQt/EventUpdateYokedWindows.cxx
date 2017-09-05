
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

#define __EVENT_UPDATE_YOKED_WINDOWS_DECLARE__
#include "EventUpdateYokedWindows.h"
#undef __EVENT_UPDATE_YOKED_WINDOWS_DECLARE__

#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventUpdateYokedWindows 
 * \brief Updates windows that are yoked.
 * \ingroup GuiQt
 */

/**
 * Constructor for a window.  This given window will NOT be updated.
 *
 * @param browserWindowIndexThatIssuedEvent
 *     Index of browser window that issued event.
 * @param brainModelYokingGroup
 *     brain model yoking group that is selected.
 * @param chartModelYokingGroup
 *     chart model yoking group that is selected.
 */
EventUpdateYokedWindows::EventUpdateYokedWindows(const int32_t browserWindowIndexThatIssuedEvent,
                                                 const YokingGroupEnum::Enum brainModelYokingGroup,
                                                 const YokingGroupEnum::Enum chartModelYokingGroup)
: Event(EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS),
m_browserWindowIndexThatIssuedEvent(browserWindowIndexThatIssuedEvent),
m_brainModelYokingGroup(brainModelYokingGroup),
m_chartModelYokingGroup(chartModelYokingGroup)
{
}

/**
 * Constructor for updating all windows.
 *
 * @param browserWindowIndexThatIssuedEvent
 *     Index of browser window that issued event.
 * @param brainModelYokingGroup
 *     brain model yoking group that is selected.
 * @param chartModelYokingGroup
 *     chart model yoking group that is selected.
 */
EventUpdateYokedWindows::EventUpdateYokedWindows(const YokingGroupEnum::Enum brainModelYokingGroup,
                                                 const YokingGroupEnum::Enum chartModelYokingGroup)
: Event(EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS),
m_browserWindowIndexThatIssuedEvent(-1),
m_brainModelYokingGroup(brainModelYokingGroup),
m_chartModelYokingGroup(chartModelYokingGroup)
{
}

/**
 * Destructor.
 */
EventUpdateYokedWindows::~EventUpdateYokedWindows()
{
    
}

/**
 * @return Index of browser window that issued the event.
 */
int32_t
EventUpdateYokedWindows::getBrowserWindowIndexThatIssuedEvent() const
{
    return m_browserWindowIndexThatIssuedEvent;
}

/**
 * @return Is the given brain model yoking group needing an update?
 *
 * @param brainModelYokingGroup
 *     The brain model yoking group.
 */
bool
EventUpdateYokedWindows::isBrainModelYoking(const YokingGroupEnum::Enum brainModelYokingGroup) const
{
    if (m_brainModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return false;
    }
    
    return (m_brainModelYokingGroup == brainModelYokingGroup);
}

/**
 * @return Is the given chart model yoking group needing an update?
 *
 * @param chartModelYokingGroup
 *     The chart model yoking group.
 */
bool
EventUpdateYokedWindows::isChartModelYoking(const YokingGroupEnum::Enum chartModelYokingGroup) const
{
    if (m_chartModelYokingGroup == YokingGroupEnum::YOKING_GROUP_OFF) {
        return false;
    }
    
    return (m_chartModelYokingGroup == chartModelYokingGroup);
}


/**
 * @return Is the given brain OR chart model yoking group needing an update?
 *
 * @param brainModelYokingGroup
 *     The brain model yoking group.
 * @param chartModelYokingGroup
 *     The chart model yoking group.
 */
bool
EventUpdateYokedWindows::isBrainOrChartModelYoking(const YokingGroupEnum::Enum brainModelYokingGroup,
                                                   const YokingGroupEnum::Enum chartModelYokingGroup) const
{
    return (isBrainModelYoking(brainModelYokingGroup)
            || isChartModelYoking(chartModelYokingGroup));
}

