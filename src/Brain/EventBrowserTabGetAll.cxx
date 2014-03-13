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

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGetAll.h"

using namespace caret;

/**
 * Constructor.
 */
EventBrowserTabGetAll::EventBrowserTabGetAll()
: Event(EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL)
{
}

/**
 * Destructor.
 */
EventBrowserTabGetAll::~EventBrowserTabGetAll()
{
    
}

/**
 * @return The number of browser tabs.
 */
int32_t 
EventBrowserTabGetAll::getNumberOfBrowserTabs() const
{
    return this->allBrowserTabs.size();
}

/**
 * Get the browser tab at the given index.
 * @param indx
 *    Index of the browser tab.
 * @return Browser tab at the given index.
 */
BrowserTabContent* 
EventBrowserTabGetAll::getBrowserTab(const int32_t indx)
{
    CaretAssertVectorIndex(this->allBrowserTabs, indx);
    return this->allBrowserTabs[indx];
}

/**
 * Add a browser tab.
 * @param browserTab
 *    Tab that is added.
 */
void 
EventBrowserTabGetAll::addBrowserTab(BrowserTabContent* browserTab)
{
    this->allBrowserTabs.push_back(browserTab);
}

/**
 * @return All browser tabs.
 */
std::vector<BrowserTabContent*> 
EventBrowserTabGetAll::getAllBrowserTabs() const
{
    return this->allBrowserTabs;
}

/**
 * @return The indices of all browser tabs.
 */
std::vector<int32_t> 
EventBrowserTabGetAll::getBrowserTabIndices() const
{
    std::vector<int32_t> tabIndices;
    
    for (std::vector<BrowserTabContent*>::const_iterator iter = allBrowserTabs.begin();
         iter != allBrowserTabs.end();
         iter++) {
        const BrowserTabContent* btc = *iter;
        tabIndices.push_back(btc->getTabNumber());
    }
    return tabIndices;
}


