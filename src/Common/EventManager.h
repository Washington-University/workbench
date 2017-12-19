#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

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

#include <stdint.h>

#include "CaretObject.h"

#include "EventTypeEnum.h"

//#define CONTAINER_VECTOR 1
//#define CONTAINER_HASH_SET 1
#define CONTAINER_SET 1

#ifdef CONTAINER_VECTOR
#include <vector>
#elif CONTAINER_HASH_SET
#include <functional>
#include "CaretHashSet.h"
#elif CONTAINER_SET
#include <set>
#else
   INTENTIONAL_COMPILER_ERROR_MISSING_CONTAINER_TYPE
#endif

namespace caret {

    class Event;
    class EventListenerInterface;
    
#ifdef CONTAINER_HASH_SET
    class EventListenerCompareHash {
    public:
        bool operator()(const EventListenerInterface* e1,
                        const EventListenerInterface* e2) const {
            return (e1 == e2);
        }
    };
    
    class EventListenerInterfaceHash {
    public:
        size_t operator()(const EventListenerInterface* p) const {
            EventListenerInterface* el = const_cast<EventListenerInterface*>(p);
            return reinterpret_cast<size_t>((void*)el);
        }
    };
#endif // CONTAINER_HASH_SET
    
    class EventManager : public CaretObject {
        
    public:
        static void createEventManager();
        
        static void deleteEventManager();
        
        static EventManager* get();
        
        void addEventListener(EventListenerInterface* eventListener,
                              const EventTypeEnum::Enum listenForEventType);
        
        void addProcessedEventListener(EventListenerInterface* eventListener,
                                       const EventTypeEnum::Enum listenForEventType);
        
        void removeEventFromListener(EventListenerInterface* eventListener,
                                 const EventTypeEnum::Enum listenForEventType);

        void removeAllEventsFromListener(EventListenerInterface* eventListener);
        
        void sendEvent(Event* event);
        
        void sendSimpleEvent(const EventTypeEnum::Enum eventType);
        
        void blockEvent(const EventTypeEnum::Enum eventToBlock,
                        const bool blockStatus);
        
        int64_t getEventIssuedCounter() const;
        
    private:
        EventManager();
        
        virtual ~EventManager();
        
        void verifyAllListenersRemoved(EventListenerInterface* eventListener);
        
        /**
         * Define the container
         */
#ifdef CONTAINER_VECTOR
        typedef std::vector<EventListenerInterface*> EVENT_LISTENER_CONTAINER;
#elif CONTAINER_HASH_SET
        typedef caret::hash_set<EventListenerInterface*,
                                   EventListenerInterfaceHash,
                                   EventListenerCompareHash> EVENT_LISTENER_CONTAINER;
#elif CONTAINER_SET
        typedef std::set<EventListenerInterface*> EVENT_LISTENER_CONTAINER;
#else
        INTENTIONAL_COMPILER_ERROR_MISSING_CONTAINER_TYPE
#endif
        
        /**
         * Iterator for the container 
         */
        typedef EVENT_LISTENER_CONTAINER::iterator EVENT_LISTENER_CONTAINER_ITERATOR;
        
        /**
         * The event listeners
         */
        EVENT_LISTENER_CONTAINER m_eventListeners[EventTypeEnum::EVENT_COUNT];
        
        /**
         * Special listeners that are notified AFTER the eventListeners
         */
        EVENT_LISTENER_CONTAINER m_eventProcessedListeners[EventTypeEnum::EVENT_COUNT];
        
        /** Counter that is incremented each time an event is issued */
        int64_t m_eventIssuedCounter;
        
        /** A counter for blocking events of each type */
        std::vector<int64_t> m_eventBlockingCounter;
        
        static EventManager* s_singletonEventManager;
        
        friend EventListenerInterface;
        
    };
    
#ifdef __EVENT_MANAGER_MAIN__
    EventManager* EventManager::s_singletonEventManager = NULL;
#endif // __EVENT_MANAGER_MAIN__
    
} // namespace

#endif // __EVENT_MANAGER_H__
