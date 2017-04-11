
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

#define __GRAPHICS_FACTORY_DECLARE__
#include "GraphicsFactory.h"
#undef __GRAPHICS_FACTORY_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "GraphicsEngineOpenGL.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"

using namespace caret;


    
/**
 * \class caret::GraphicsFactory 
 * \brief Factory for creating/accessing graphics opertions.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsFactory::GraphicsFactory()
: CaretObject()
{
    m_graphicsEngineOpenGL = std::unique_ptr<GraphicsEngineOpenGL>(new GraphicsEngineOpenGL());
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
GraphicsFactory::~GraphicsFactory()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Create the graphics factory.  
 * This should be called one time, not more, not less.
 */
void
GraphicsFactory::createGraphicsFactory()
{
    CaretAssertMessage((GraphicsFactory::s_singletonGraphicsFactory == NULL),
                       "Graphics Factory has already been created.");
    
    GraphicsFactory::s_singletonGraphicsFactory = new GraphicsFactory();
}

/**
 * Delete the graphics factory.
 *
 * This should be called one time after the graphics factory is created.
 */
void
GraphicsFactory::deleteGraphicsFactory()
{
    CaretAssertMessage((GraphicsFactory::s_singletonGraphicsFactory != NULL),
                       "GraphicsFactory does not exist, cannot delete it.");
    
    delete GraphicsFactory::s_singletonGraphicsFactory;
    GraphicsFactory::s_singletonGraphicsFactory= NULL;
}

/**
 * Get the one and only graphics factory
 *
 * @return  Pointer to the graphics factory.
 */
GraphicsFactory*
GraphicsFactory::get()
{
    CaretAssertMessage((GraphicsFactory::s_singletonGraphicsFactory != NULL),
                       "GraphicsFactory was not created or has been deleted.\n"
                       "It must be created with GraphicsFactory::createGraphicsFactory().");
    
    return GraphicsFactory::s_singletonGraphicsFactory;
}

/**
 * @return The graphics engine for OpenGL.
 */
GraphicsEngineOpenGL*
GraphicsFactory::getGraphicsEngineOpenGL()
{
    CaretAssert(m_graphicsEngineOpenGL.get());
    return m_graphicsEngineOpenGL.get();
}

/**
 * @return A new primitive for XYZ with float RGBA.  Caller is responsible
 * for deleting the returned pointer.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitiveV3fC4f*
GraphicsFactory::newPrimitiveV3fC4f(const GraphicsPrimitive::PrimitiveType primitiveType)
{
    GraphicsPrimitiveV3fC4f* primitive = new GraphicsPrimitiveV3fC4f(primitiveType);
    return primitive;
}

/**
 * @return A new primitive for XYZ with unsigned byte RGBA.  Caller is responsible
 * for deleting the returned pointer.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 */
GraphicsPrimitiveV3fC4ub*
GraphicsFactory::newPrimitiveV3fC4ub(const GraphicsPrimitive::PrimitiveType primitiveType)
{
    GraphicsPrimitiveV3fC4ub* primitive = new GraphicsPrimitiveV3fC4ub(primitiveType);
    return primitive;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsFactory::toString() const
{
    return "GraphicsFactory";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GraphicsFactory::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

