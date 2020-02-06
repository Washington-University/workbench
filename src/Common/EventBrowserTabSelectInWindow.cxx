
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

#define __EVENT_BROWSER_TAB_SELECT_IN_WINDOW_DECLARE__
#include "EventBrowserTabSelectInWindow.h"
#undef __EVENT_BROWSER_TAB_SELECT_IN_WINDOW_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabSelectInWindow 
 * \brief Request selection of a browser tab in the window that contains the browser tab
 * \ingroup Common
 */

/**
 * Constructor.
 * @param browserTabIndex
 *    Index of the browser tab
 */
EventBrowserTabSelectInWindow::EventBrowserTabSelectInWindow(const int32_t browserTabIndex)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_SELECT_IN_WINDOW),
m_browserTabIndex(browserTabIndex)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabSelectInWindow::~EventBrowserTabSelectInWindow()
{
}

/**
 * @return Index of the browser tab
 */
int32_t
EventBrowserTabSelectInWindow::getBrowserTabIndex() const
{
    return m_browserTabIndex;
}


