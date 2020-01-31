
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __EVENT_BROWSER_TAB_REOPEN_AVAILABLE_DECLARE__
#include "EventBrowserTabReopenAvailable.h"
#undef __EVENT_BROWSER_TAB_REOPEN_AVAILABLE_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;



/**
 * \class caret::EventBrowserTabReopenAvailable
 * \brief Event to get browser tabs that can be reopened
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventBrowserTabReopenAvailable::EventBrowserTabReopenAvailable()
: Event(EventTypeEnum::EVENT_BROWSER_TAB_REOPEN_AVAILBLE)
{
}

/**
 * Destructor.
 */
EventBrowserTabReopenAvailable::~EventBrowserTabReopenAvailable()
{
}

/**
 * @return True if there is a tab available for reopening
 */
bool
EventBrowserTabReopenAvailable::isReopenValid() const
{
    return (m_tabIndex >= 0);
}

/**
 * @return True if there is a tab available for reopening
 */
int32_t
EventBrowserTabReopenAvailable::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * @return True if there is a tab available for reopening
 */
AString
EventBrowserTabReopenAvailable::getTabName() const
{
    return m_tabName;
}

/**
 * Set the index and name of a tab that can be reopened
 * This method is used by the receiver of the event.
 *
 * @param tabIndex
 *     Index of the tab
 * @param tabName
 *     Name of the tab.
 */
void
EventBrowserTabReopenAvailable::setTabIndexAndName(const int32_t tabIndex,
                                                   const AString& tabName)
{
    m_tabIndex = tabIndex;
    m_tabName  = tabName;
}


