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

#include "EventBrowserWindowContentGet.h"

using namespace caret;

#include "CaretAssert.h"

/**
 * Constructor.
 * @param browserWindowIndex
 *    Index of browser window.
 */
EventBrowserWindowContentGet::EventBrowserWindowContentGet(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT_GET)
{
    this->browserWindowIndex = browserWindowIndex;
}

/*
 * Destructor.
 */
EventBrowserWindowContentGet::~EventBrowserWindowContentGet()
{
    
}

/**
 * @return Get the browser window index.
 */
int32_t 
EventBrowserWindowContentGet::getBrowserWindowIndex() const 
{ 
    return this->browserWindowIndex; 
}

/**
 * @return The number of items to draw.
 */
int32_t 
EventBrowserWindowContentGet::getNumberOfItemsToDraw() const
{
    return this->browserTabContents.size();
}

/**
 * Add tab content for drawing in a window.
 */
void 
EventBrowserWindowContentGet::addTabContentToDraw(BrowserTabContent* browserTabContent)
{
    this->browserTabContents.push_back(browserTabContent);
}

/**
 * Get the tab content for drawing in a window.
 * 
 * @param itemIndex
 *    Index of the item to draw.
 * @return
 *    Pointer to tab contents for the item index.
 */
BrowserTabContent*
EventBrowserWindowContentGet::getTabContentToDraw(const int32_t itemIndex)
{
    CaretAssertVectorIndex(this->browserTabContents, itemIndex);
    return this->browserTabContents[itemIndex];
}

