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
#include "EventBrowserTabCloseInToolBar.h"

using namespace caret;

/**
 * Constructor.
 */
EventBrowserTabCloseInToolBar::EventBrowserTabCloseInToolBar(BrowserTabContent* browserTab,
                                             const int32_t browserTabIndex)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_CLOSE_IN_TOOL_BAR),
m_browserTab(browserTab),
m_browserTabIndex(browserTabIndex)
{
    CaretAssert(browserTab);
}

/**
 * Destructor.
 */
EventBrowserTabCloseInToolBar::~EventBrowserTabCloseInToolBar()
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
EventBrowserTabCloseInToolBar::getBrowserTab()
{
    return m_browserTab;
}

/**
 * @return Index of browser tab being deleted.
 */
int32_t
EventBrowserTabCloseInToolBar::getBrowserTabIndex() const
{
    return m_browserTabIndex;
}

