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

#include <algorithm>

#define __EVENT_MANAGER_MAIN__
#include "Event.h"
#include "EventManager.h"
#undef __EVENT_MANAGER_MAIN__

#include "CaretAssert.h"
#include "EventListenerInterface.h"

using namespace caret;

/**
 * Constructor.
 */
EventManager::EventManager()
{
    
}

/**
 * Destructor.
 */
EventManager::~EventManager()
{
    
}

/**
 * Get the one and only event mangers.
 *
 * @return  Pointer to the event manager.
 */
EventManager* 
EventManager::get()
{
    if (EventManager::singletonEventManager == NULL) {
        EventManager::singletonEventManager = new EventManager();
    }    
    return EventManager::singletonEventManager;
}

/**
 * Add a listener for a specific event.
 *
 * @param eventListener
 *     Listener for an event.
 * @param listenForEventType
 *     Type of event that is wanted.
 */
void 
EventManager::addEventListener(EventListenerInterface* eventListener,
                               const Event::EventType listenForEventType)
{
    this->eventListeners[listenForEventType].push_back(eventListener);
    
    std::cout << "Adding listener from class "
    << typeid(*eventListener).name()
    << " for event="
    << listenForEventType
    << std::endl;
}

/**
 * Remove a listener for a specific event.
 *
 * @param eventListener
 *     Listener for an event.
 * @param listenForEventType
 *     Type of event that is no longer wanted.
 */
void 
EventManager::removeEventListener(EventListenerInterface* eventListener,
                                  const Event::EventType listenForEventType)
{
    EVENT_LISTENER_CONTAINER listeners = this->eventListeners[listenForEventType];
    
    std::cout << "Removing listener from class "
    << typeid(*eventListener).name()
    << " for event="
    << listenForEventType
    << std::endl;

    listeners.erase(std::remove(listeners.begin(),
                                listeners.end(),
                                eventListener),
                    listeners.end());
}

/**
 * Send an event.
 * 
 * @param event
 *    Event that is sent.
 */
void 
EventManager::sendEvent(Event* event)
{   
    Event::EventType eventType = event->getEventType();
    EVENT_LISTENER_CONTAINER listeners = this->eventListeners[eventType];
    
    for (EVENT_LISTENER_CONTAINER_ITERATOR iter = listeners.begin();
         iter != listeners.end();
         iter++) {
        EventListenerInterface* listener = *iter;
        
        std::cout << "Sending listener from class "
        << typeid(*listener).name()
        << " the event="
        << eventType
        << std::endl;

        listener->receiveEvent(event);
        
        if (event->isError()) {
            break;
        }
    }
}

