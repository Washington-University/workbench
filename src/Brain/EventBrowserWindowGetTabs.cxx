
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

#define __EVENT_BROWSER_WINDOW_GET_TABS_DECLARE__
#include "EventBrowserWindowGetTabs.h"
#undef __EVENT_BROWSER_WINDOW_GET_TABS_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserWindowGetTabs 
 * \brief Event to get browser tabs in a window
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param windowIndex
 *    Index of the window
 */
EventBrowserWindowGetTabs::EventBrowserWindowGetTabs(const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_GET_TABS),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
EventBrowserWindowGetTabs::~EventBrowserWindowGetTabs()
{
}

/**
 * @return Index of browser window
 */
int32_t
EventBrowserWindowGetTabs::getBrowserWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Tab content in the window
 */
std::vector<BrowserTabContent*>
EventBrowserWindowGetTabs::getBrowserTabs() const
{
    return m_browserTabs;
}

/**
 * @return Indices of the browser tabs in the window
 */
std::vector<int32_t>
EventBrowserWindowGetTabs::getBrowserTabIndices() const
{
    std::vector<int32_t> indices;
    for (auto bt : m_browserTabs) {
        indices.push_back(bt->getTabNumber());
    }
    return indices;
}

/**
 * Add a browser tab
 *
 * @param browserTabContent
 *     Tab to add
 */
void
EventBrowserWindowGetTabs::addBrowserTab(BrowserTabContent* browserTabContent)
{
    CaretAssert(browserTabContent);
    m_browserTabs.push_back(browserTabContent);
}


