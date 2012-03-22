/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

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


