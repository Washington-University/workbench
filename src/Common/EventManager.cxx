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

#include <QThread>

#include <algorithm>
#include <iostream>
#include <typeinfo>

#define __EVENT_MANAGER_MAIN__
#include "Event.h"
#include "EventManager.h"
#undef __EVENT_MANAGER_MAIN__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventListenerInterface.h"

using namespace caret;

/**
 * Constructor.
 */
EventManager::EventManager()
{
    this->eventCounter = 0;
}

/**
 * Destructor.
 */
EventManager::~EventManager()
{
    /*
     * Verify that all listeners were removed.
     */ 
    for (int32_t i = 0; i < EventTypeEnum::EVENT_COUNT; i++) {
        EVENT_LISTENER_CONTAINER el = this->eventListeners[i];
        if (el.empty() == false) {
            EventTypeEnum::Enum enumValue = static_cast<EventTypeEnum::Enum>(i);
            std::cout 
            << "Not all listeners removed for event "
            << EventTypeEnum::toName(enumValue)
            << ", count is: "
            << el.size()
            << std::endl;
            
/*
 * This will not work because it is likely the object
 * has been deleted and this simply crashes.
            for (EVENT_LISTENER_CONTAINER_ITERATOR iter = el.begin();
                 iter != el.end();
                 iter++) {
                EventListenerInterface* listener = *iter;
                CaretObject* caretObject = dynamic_cast<CaretObject*>(listener);
                if (caretObject != NULL) {
                    std::cout << "   " << caretObject->toString();
                }
            }
 */
        }
    }
    
}

/**
 * Create the event manager.
 */
void 
EventManager::createEventManager()
{
    CaretAssertMessage((EventManager::singletonEventManager == NULL), 
                       "Event manager has already been created.");
    
    EventManager::singletonEventManager = new EventManager();
}

/**
 * Delete the event manager.
 * This may only be called one time after event manager is created.
 */
void 
EventManager::deleteEventManager()
{
    CaretAssertMessage((EventManager::singletonEventManager != NULL), 
                       "Event manager does not exist, cannot delete it.");
    
    delete EventManager::singletonEventManager;
    EventManager::singletonEventManager = NULL;
}

/**
 * Get the one and only event mangers.
 *
 * @return  Pointer to the event manager.
 */
EventManager* 
EventManager::get()
{
    CaretAssertMessage(EventManager::singletonEventManager,
                       "Event manager was not created.\n"
                       "It must be created with EventManager::createEventManager().");
    
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
                               const EventTypeEnum::Enum listenForEventType)
{
    this->eventListeners[listenForEventType].push_back(eventListener);
    
    //std::cout << "Adding listener from class "
    //<< typeid(*eventListener).name()
    //<< " for "
    //<< EventTypeEnum::toName(listenForEventType)
    //<< std::endl;
}

/**
 * Stop listening for an event.
 *
 * @param eventListener
 *     Listener for an event.
 * @param listenForEventType
 *     Type of event that is no longer wanted.
 */
void 
EventManager::removeEventFromListener(EventListenerInterface* eventListener,
                                  const EventTypeEnum::Enum listenForEventType)
{
    EVENT_LISTENER_CONTAINER listeners = this->eventListeners[listenForEventType];
    
    
    /*
     * Remove the listener by creating a new container
     * of non-matching listeners.
     */
    EVENT_LISTENER_CONTAINER updatedListeners;
    for (EVENT_LISTENER_CONTAINER_ITERATOR iter = listeners.begin();
         iter != listeners.end();
         iter++) {
        if (*iter == eventListener) {
            //std::cout << "Removing listener from class "
            //<< typeid(*eventListener).name()
            //<< " for "
            //<< EventTypeEnum::toName(listenForEventType)
            //<< std::endl;            
        }
        else {
            updatedListeners.push_back(*iter);
        }
    }
    
    if (updatedListeners.size() != listeners.size()) {
        this->eventListeners[listenForEventType] = updatedListeners;
    }
}

/**
 * Stop listening for all events.
 * @param eventListener
 *     Listener for all events.
 */ 
void 
EventManager::removeAllEventsFromListener(EventListenerInterface* eventListener)
{
    for (int32_t i = 0; i < EventTypeEnum::EVENT_COUNT; i++) {
        this->removeEventFromListener(eventListener, static_cast<EventTypeEnum::Enum>(i));
    }
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
    /*
     * Get listeners for event.
     */
    EventTypeEnum::Enum eventType = event->getEventType();
    EVENT_LISTENER_CONTAINER listeners = this->eventListeners[eventType];
    
    const AString eventNumberString = AString::number(this->eventCounter);
    
    AString msg = ("Sending event "
                   + eventNumberString
                   + ": "
                   + event->toString() 
                   + " from thread: " 
                   + AString::number((uint64_t)QThread::currentThread()));
    CaretLogFiner(msg);
    //std::cout << msg << std::endl;
    
    /*
     * Send event to each of the listeners.
     */
    for (EVENT_LISTENER_CONTAINER_ITERATOR iter = listeners.begin();
         iter != listeners.end();
         iter++) {
        EventListenerInterface* listener = *iter;
        
        //std::cout << "Sending event from class "
        //<< typeid(*listener).name()
        //<< " for "
        //<< EventTypeEnum::toName(eventType)
        //<< std::endl;

        
        listener->receiveEvent(event);
        
        if (event->isError()) {
            CaretLogWarning("Event " + eventNumberString + " had error: " + event->toString());
            break;
        }
    }
    
    /*
     * Verify event was processed.
     */
    if (event->getEventProcessCount() == 0) {
        CaretLogWarning("Event " + eventNumberString + " not processed: " + event->toString());
    }
    
    this->eventCounter++;
}

