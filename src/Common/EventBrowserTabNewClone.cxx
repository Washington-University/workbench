
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __EVENT_BROWSER_TAB_NEW_CLONE_DECLARE__
#include "EventBrowserTabNewClone.h"
#undef __EVENT_BROWSER_TAB_NEW_CLONE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabNewClone 
 * \brief Get a browser tab that is cloned from another browser tab
 * \ingroup Common
 *
 * Get a browser tab that is cloned from another browser tab
 */

/**
 * Constructor.
 *
 * @param indexOfBrowserTabThatWillBeCloned
 *     Index of browser tab that is cloned.
 */
EventBrowserTabNewClone::EventBrowserTabNewClone(const int32_t indexOfBrowserTabThatWillBeCloned)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_NEW_CLONE),
m_indexOfBrowserTabThatWasClonded(indexOfBrowserTabThatWillBeCloned)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabNewClone::~EventBrowserTabNewClone()
{
}

/**
 * @return The new browser tab.
 */
BrowserTabContent*
EventBrowserTabNewClone::getNewBrowserTab() const
{
    return m_newBrowserTabContent;
}

/**
 * Set the new browser tab and its index.
 *
 * @param newBrowserTab
 *     The new browser tab.
 * @param newBrowserTabIndex
 *     Index of the new browser tab.
 */
void
EventBrowserTabNewClone::setNewBrowserTab(BrowserTabContent* newBrowserTab,
                   const int32_t newBrowserTabIndex)
{
    m_newBrowserTabContent = newBrowserTab;
    m_newBrowserTabIndex   = newBrowserTabIndex;
}

/**
 * @return Index of the new browser index.
 */
int32_t
EventBrowserTabNewClone::getNewBrowserTabIndex() const
{
    return m_newBrowserTabIndex;
}

/**
 * @return Index of the browser tab that was cloned
 */
int32_t
EventBrowserTabNewClone::getIndexOfBrowserTabThatWasCloned() const
{
    return m_indexOfBrowserTabThatWasClonded;
}
