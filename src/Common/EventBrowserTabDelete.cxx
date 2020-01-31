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

#include "CaretAssert.h"
#include "EventBrowserTabDelete.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param browserTab
 *    The tab to close
 * @param browserTabIndex
 *    Index of the tab to close
 * @param windowIndex
 *    Index of window containing tab to close
 */
EventBrowserTabDelete::EventBrowserTabDelete(BrowserTabContent* browserTab,
                                             const int32_t browserTabIndex,
                                             const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_DELETE),
m_browserTab(browserTab),
m_browserTabIndex(browserTabIndex),
m_windowIndex(windowIndex)
{
    CaretAssert(browserTab);
}

/**
 * Destructor.
 */
EventBrowserTabDelete::~EventBrowserTabDelete()
{
    
}

/**
 * Get the browser tab that is to be deleted.
 * Note that this may point to a browser tab that 
 * has been deleted and using the pointer in this
 * case could be a disaster.
 *
 * @return
 *    Pointer to browser tab that is to be deleted.
 */
BrowserTabContent* 
EventBrowserTabDelete::getBrowserTab()
{
    return m_browserTab;
}

/**
 * @return Index of browser tab being deleted.
 */
int32_t
EventBrowserTabDelete::getBrowserTabIndex() const
{
    return m_browserTabIndex;
}

/**
 * @return Index of browser window that is deleting tab
 */
int32_t
EventBrowserTabDelete::getWindowIndex() const
{
    return m_windowIndex;
}

