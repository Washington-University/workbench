
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <algorithm>

#define __EVENT_BROWSER_TAB_INDICES_GET_ALL_DECLARE__
#include "EventBrowserTabIndicesGetAll.h"
#undef __EVENT_BROWSER_TAB_INDICES_GET_ALL_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabIndicesGetAll 
 * \brief Event to get indices of all valid browser tabs.
 * \ingroup Common
 */

/**
 * Constructor.
 */
EventBrowserTabIndicesGetAll::EventBrowserTabIndicesGetAll()
: Event(EventTypeEnum::EVENT_BROWSER_TAB_INDICES_GET_ALL)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabIndicesGetAll::~EventBrowserTabIndicesGetAll()
{
}

/**
 * Add the tab index of a valid browser tab.
 *
 * @parm browserTabIndex
 *     Index of the browser tab.
 */
void
EventBrowserTabIndicesGetAll::addBrowserTabIndex(const int32_t browserTabIndex)
{
    m_browserTabIndices.push_back(browserTabIndex);
}

/**
 * @return Indices of all valid browser tabs.
 */
std::vector<int32_t>
EventBrowserTabIndicesGetAll::getAllBrowserTabIndices() const
{
    return m_browserTabIndices;
}

/**
 * Get the validity of a browser tab.
 *
 * @parm browserTabIndex
 *     Index of the browser tab.
 * @return
 *     True if browser tab index is valid, else false.
 */
bool
EventBrowserTabIndicesGetAll::isValidBrowserTabIndex(const int32_t browserTabIndex)
{
    if (std::find(m_browserTabIndices.begin(),
                  m_browserTabIndices.end(),
                  browserTabIndex) != m_browserTabIndices.end()) {
        return true;
    }
    
    return false;
}

