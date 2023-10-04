
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __EVENT_BROWSER_TAB_INDEX_GET_WINDOW_INDEX_DECLARE__
#include "EventBrowserTabIndexGetWindowIndex.h"
#undef __EVENT_BROWSER_TAB_INDEX_GET_WINDOW_INDEX_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabIndexGetWindowIndex 
 * \brief Get the window index for a tab index
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param tabIndex
 *    Index of tab
 */
EventBrowserTabIndexGetWindowIndex::EventBrowserTabIndexGetWindowIndex(const int32_t tabIndex)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_INDEX_GET_WINDOW_INDEX),
m_tabIndex(tabIndex)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabIndexGetWindowIndex::~EventBrowserTabIndexGetWindowIndex()
{
}

/**
 * @return Index of tab
 */
int32_t
EventBrowserTabIndexGetWindowIndex::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * @return Index of window
 */
int32_t
EventBrowserTabIndexGetWindowIndex::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Set the window index
 * @param windowIndex
 *    Index of window.
 */
void
EventBrowserTabIndexGetWindowIndex::setWindowIndex(const int32_t windowIndex)
{
    m_windowIndex = windowIndex;
}
