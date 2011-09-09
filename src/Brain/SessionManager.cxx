
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __SESSION_MANAGER_DECLARE__
#include "SessionManager.h"
#undef __SESSION_MANAGER_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabNew.h"

using namespace caret;


/**
 * Constructor.
 */
SessionManager::SessionManager()
: CaretObject(), EventListenerInterface()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->browserTabs[i] = NULL;
    }
    
    EventManager::get()->addEventListener(this, Event::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addEventListener(this, Event::EVENT_BROWSER_TAB_NEW);
}

/**
 * Destructor.
 */
SessionManager::~SessionManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get the one and only session manager.
 *
 * @return  Pointer to the session manager.
 */
SessionManager* 
SessionManager::get()
{
    if (SessionManager::singletonSessionManager == NULL) {
        SessionManager::singletonSessionManager = new SessionManager();
    }
    
    return this->singletonSessionManager;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SessionManager::toString() const
{
    return "SessionManager";
}

/**
 * Receive events.
 *
 * @param event
 *    Event that needs to be processed.
 */
void 
SessionManager::receiveEvent(Event* event)
{
    if (event->getEventType() == Event::EVENT_BROWSER_TAB_NEW) {
        EventBrowserTabNew* tabEvent =
            dynamic_cast<EventBrowserTabNew*>(event);
        CaretAssert(tabEvent);
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (this->browserTabs[i] == NULL) {
                BrowserTabContent* tab = new BrowserTabContent(i);
                this->browserTabs[i] = tab;
                tabEvent->setBrowserTab(tab);
                break;
            }
        }
    }
    else if (event->getEventType() == Event::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* tabEvent =
        dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(tabEvent);
        
        BrowserTabContent* tab = tabEvent->getBrowserTab();
        
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            if (this->browserTabs[i] == tab) {
                delete this->browserTabs[i];
                this->browserTabs[i] = NULL;
                break;
            }
        }
    }
}

