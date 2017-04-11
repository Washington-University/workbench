
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __GRAPHICS_ENGINE_DECLARE__
#include "GraphicsEngine.h"
#undef __GRAPHICS_ENGINE_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
using namespace caret;


    
/**
 * \class caret::GraphicsEngine 
 * \brief Abstract class for rending graphics objects.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsEngine::GraphicsEngine()
: CaretObject()
{
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
GraphicsEngine::~GraphicsEngine()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsEngine::toString() const
{
    return "GraphicsEngine";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GraphicsEngine::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

