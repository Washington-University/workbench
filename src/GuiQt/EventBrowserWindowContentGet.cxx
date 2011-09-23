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

/**
 * Constructor.
 * @param browserWindowIndex
 *    Index of browser window.
 */
EventBrowserWindowContentGet::EventBrowserWindowContentGet(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT_GET)
{
    this->browserWindowIndex = browserWindowIndex;
    this->windowTabNumber = -1;
    this->modelDisplayController = NULL;
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
 * @return Get window tab number
 */
int32_t 
EventBrowserWindowContentGet::getWindowTabNumber() const 
{ 
    return this->windowTabNumber; 
}

/**
 * Get the model controller being drawn
 */
ModelDisplayController* 
EventBrowserWindowContentGet::getModelDisplayController()  
{ 
    return this->modelDisplayController; 
}

/**
 * Set the window tab number.
 *
 * @param windowTabNumber
 *     Number of tab window.
 */
void 
EventBrowserWindowContentGet::setWindowTabNumber(const int32_t windowTabNumber)
{
    this->windowTabNumber = windowTabNumber;
}

/**
 * Set the model display controller.
 *
 * @param modelDisplayController
 *    The model display controller.
 */
void 
EventBrowserWindowContentGet::setModelDisplayController(ModelDisplayController* modelDisplayController)
{
    this->modelDisplayController = modelDisplayController;   
}

/**
 * @return Returns the browser tab content.
 */
BrowserTabContent* 
EventBrowserWindowContentGet::getBrowserTabContent()
{
    return this->browserTabContent;
}

/**
 * Sets the browser tab content.
 * @param browserTabContent
 *    New value for browser tab content.
 */
void 
EventBrowserWindowContentGet::setBrowserTabContent(BrowserTabContent* browserTabContent)
{
    this->browserTabContent = browserTabContent;
}

