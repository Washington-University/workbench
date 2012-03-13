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
    this->eventIssuedCounter = 0;
    this->eventBlockingCounter.resize(EventTypeEnum::EVENT_COUNT, 0);
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
    EventTypeEnum::Enum eventType = event->getEventType();
    const AString eventNumberString = AString::number(this->eventIssuedCounter);
    const AString eventMessagePrefix = ("Event "
                                        + eventNumberString
                                        + ": "
                                        + event->toString() 
                                        + " from thread: " 
                                        + AString::number((uint64_t)QThread::currentThread())
                                        + " ");
    
    const int32_t eventTypeIndex = static_cast<int32_t>(eventType);
    CaretAssertVectorIndex(this->eventBlockingCounter, eventTypeIndex);
    if (this->eventBlockingCounter[eventTypeIndex] > 0) {
        AString msg = (eventMessagePrefix
                       + " is blocked.  Blocking counter="
                       + AString::number(this->eventBlockingCounter[eventTypeIndex]));
        CaretLogFiner(msg);
    }
    else {
        /*
         * Get listeners for event.
         */
        EVENT_LISTENER_CONTAINER listeners = this->eventListeners[eventType];
        
        const AString eventNumberString = AString::number(this->eventIssuedCounter);
        
        AString msg = (eventMessagePrefix + " SENT.");
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
    }    
    
    this->eventIssuedCounter++;
}

/**
 * Block an event.  A counter is used to track blocking of each
 * event type.  Each time a request is made to block an event type,
 * the counter is incremented for that event type.  When a request
 * is made to un-block the event, the counter is decremented.  This
 * allows multiple requests for blocking an event to come from 
 * different sections of the source code.  Thus, anytime the
 * blocking counter is greater than zero for an event, the event
 * is blocked.
 * 
 * @param eventType
 *    Type of event to block.
 * @param blockStatus
 *    Blocking status (true increments blocking counter,
 *    false decrements blocking counter.
 */
void 
EventManager::blockEvent(const EventTypeEnum::Enum eventType,
                         const bool blockStatus)
{
    const int32_t eventTypeIndex = static_cast<int32_t>(eventType);
    CaretAssertVectorIndex(this->eventBlockingCounter, eventTypeIndex);
    
    const AString eventName = EventTypeEnum::toName(eventType);
    
    if (blockStatus) {
        this->eventBlockingCounter[eventTypeIndex]++;
        CaretLogFiner("Blocking event "
                      + eventName
                      + " blocking counter is now "
                      + AString::number(this->eventBlockingCounter[eventTypeIndex]));
    }
    else {
        if (this->eventBlockingCounter[eventTypeIndex] > 0) {
            this->eventBlockingCounter[eventTypeIndex]--;
            CaretLogFiner("Unblocking event "
                          + eventName
                          + " blocking counter is now "
                          + AString::number(this->eventBlockingCounter[eventTypeIndex]));
        }
        else {
            const AString message("Trying to unblock event "
                                  + eventName
                                  + " but it is not blocked");
            CaretAssertMessage(0, message);
            CaretLogWarning(message);
        }
    }
}

