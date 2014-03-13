
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
 * @param yokingGroup
 *     Yoking group that is selected.
 */
EventUpdateYokedWindows::EventUpdateYokedWindows(const int32_t browserWindowIndexThatIssuedEvent,
                                                 const YokingGroupEnum::Enum yokingGroup)
: Event(EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS),
m_browserWindowIndexThatIssuedEvent(browserWindowIndexThatIssuedEvent),
m_yokingGroup(yokingGroup)
{
}

/**
 * Constructor for updating all windows.
 *
 * @param browserWindowIndexThatIssuedEvent
 *     Index of browser window that issued event.
 * @param yokingGroup
 *     Yoking group that is selected.
 */
EventUpdateYokedWindows::EventUpdateYokedWindows(const YokingGroupEnum::Enum yokingGroup)
: Event(EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS),
m_browserWindowIndexThatIssuedEvent(-1),
m_yokingGroup(yokingGroup)
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
 * @return The yoking group that should be updated.
 */
YokingGroupEnum::Enum
EventUpdateYokedWindows::getYokingGroup() const
{
    return m_yokingGroup;
}
