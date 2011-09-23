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

#include "EventBrowserWindowNew.h"

using namespace caret;

/**
 * Constructor.
 * @param parent
 *   Widget used for window placement (may be NULL).
 * @param browserTabContent
 *   Initial content for window (may be NULL).
 */
EventBrowserWindowNew::EventBrowserWindowNew(QWidget* parent,
                                             BrowserTabContent* browserTabContent)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_NEW)
{
    this->parent = parent;
    this->browserTabContent = browserTabContent;
    this->browserWindowCreated = NULL;
}

/**
 * Constructor.
 */
EventBrowserWindowNew::EventBrowserWindowNew()
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_NEW)
{
    this->browserTabContent = NULL;
}

/*
 * Destructor.
 */
EventBrowserWindowNew::~EventBrowserWindowNew()
{
    
}

/**
 * @return Parent for placement of new window (my be NULL).
 */
QWidget* 
EventBrowserWindowNew::getParent()
{
    return this->parent;
}

/**
 * Returns the tab content for the window.  If the
 * returned value is NULL, then a new tab should be
 * created.
 *
 * @return Returns the browser tab content.
 */
BrowserTabContent* 
EventBrowserWindowNew::getBrowserTabContent() const
{
    return this->browserTabContent;
}

/**
 * @return The browser window that was created.
 */
BrainBrowserWindow* 
EventBrowserWindowNew::getBrowserWindowCreated() const
{
    return this->browserWindowCreated;
}

/**
 * Set the browser window that was created.
 * param browserWindowCreated
 *   Browser window that was created.
 */
void 
EventBrowserWindowNew::setBrowserWindowCreated(BrainBrowserWindow* browserWindowCreated)
{
    this->browserWindowCreated = browserWindowCreated;
}
