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

#include "Event.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param eventType
 *    The type of the event.
 */
Event::Event(const EventTypeEnum::Enum eventType)
{
    this->eventType = eventType;
    this->errorMessage = "";
    this->errorStatus  = false;
    this->eventProcessedCount = 0;
}

/**
 * Destructor.
 */
Event::~Event()
{
    
}

/**
 * Get the type of event.
 *
 * @return 
 *    The event type.
 */
EventTypeEnum::Enum 
Event::getEventType() const
{
    return this->eventType;
}

/**
 * Get a pointer to this event.  The intention of this 
 * method is to allow the sender of the event to create
 * this event ?statically? (not with new) and then send
 * "event"->getPointer() to the event manager since 
 * &eventObject will not cast automatically to an
 * Event object pointer.
 * 
 * @return
 *    Pointer to this object with the base Event class.
 */
Event* 
Event::getPointer() 
{ 
    return this; 
}

/**
 * Was there an error processing this event?
 *
 * @return
 *    True if there was an error processing the
 *    event else false.
 */
bool 
Event::isError() const 
{ 
    return this->errorStatus; 
}

/**
 * Get the error message which is only
 * valid if isError() returns true.
 *
 * @return 
 *    A message describing the error.
 */
AString 
Event::getErrorMessage() const
{ 
    return this->errorMessage; 
}

/** 
 * Consumer of an event can set the error message
 * to indicate that there was an error processing 
 * the event. 
 *
 * Calling this method will result in the error
 * status being set and the event manager will not
 * send this event to any other receivers.
 *
 * @param errorMessage
 *    The error message.
 */
void 
Event::setErrorMessage(const AString& errorMessage) 
{
    this->errorMessage = errorMessage;
    this->errorStatus = true;
}

/**
 * Set the this event was processed by a listener.
 */
void 
Event::setEventProcessed()
{
    this->eventProcessedCount++;
}

/**
 * Get the number of times the event was processed.
 *
 * @preturn Count of receivers that processed this event.
 */
int32_t 
Event::getEventProcessCount() const
{
    return this->eventProcessedCount;
}

/**
 * Get String representation of caret object.
 * @return String containing caret object.
 *
 */
AString 
Event::toString() const
{
    AString s = EventTypeEnum::toName(this->eventType);
    return s;
}


