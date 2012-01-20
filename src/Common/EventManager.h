#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

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

#include <vector>

#include <CaretObject.h>

#include "Event.h"

#include <stdint.h>

namespace caret {

    class EventListenerInterface;
    
    /**
     * \brief  The event manager.
     *
     * The event manager processes events
     * from senders to receivers.
     *
     * Objects that wish to receive events must implement
     * the EventListenerInterface and call
     * addEventListener() to receive events and call
     * removeEventListener() to no longer receive events.
     *
     * Events are sent by calling this class' sendEvent()
     * method.
     */
    class EventManager : public CaretObject {
        
    public:
        static void createEventManager();
        
        static void deleteEventManager();
        
        static EventManager* get();
        
        void addEventListener(EventListenerInterface* eventListener,
                              const EventTypeEnum::Enum listenForEventType);
        
        void removeEventFromListener(EventListenerInterface* eventListener,
                                 const EventTypeEnum::Enum listenForEventType);

        void removeAllEventsFromListener(EventListenerInterface* eventListener);
        
        void sendEvent(Event* event);
        
    private:
        EventManager();
        
        virtual ~EventManager();
        
        typedef std::vector<EventListenerInterface*> EVENT_LISTENER_CONTAINER;
        typedef std::vector<EventListenerInterface*>::iterator EVENT_LISTENER_CONTAINER_ITERATOR;
        
        EVENT_LISTENER_CONTAINER eventListeners[EventTypeEnum::EVENT_COUNT];
        
        int64_t eventCounter;
        
        static EventManager* singletonEventManager;
        
    };
    
#ifdef __EVENT_MANAGER_MAIN__
    EventManager* EventManager::singletonEventManager = NULL;
#endif // __EVENT_MANAGER_MAIN__
    
} // namespace

#endif // __EVENT_MANAGER_H__
