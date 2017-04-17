
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

#define __EVENT_GRAPHICS_OPEN_G_L_CREATE_BUFFER_OBJECT_DECLARE__
#include "EventGraphicsOpenGLCreateBufferObject.h"
#undef __EVENT_GRAPHICS_OPEN_G_L_CREATE_BUFFER_OBJECT_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventGraphicsOpenGLDeleteBufferObject.h"
#include "EventManager.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGraphicsOpenGLCreateBufferObject 
 * \brief Create an OpenGL Buffer Object
 * \ingroup Graphics
 *
 * Creates an OpenGL Buffer Object for the current context.
 * There MUST BE an active OpenGL context when this event
 * is processed.  Otherwise, a buffer object cannot be
 * created and a program will likely occur.
 */

/**
 * Constructor.
 */
EventGraphicsOpenGLCreateBufferObject::EventGraphicsOpenGLCreateBufferObject()
: Event(EventTypeEnum::EVENT_GRAPHICS_OPENGL_CREATE_BUFFER_OBJECT)
{
    
}

/**
 * Destructor.
 */
EventGraphicsOpenGLCreateBufferObject::~EventGraphicsOpenGLCreateBufferObject()
{
    if (m_openglBufferObject != NULL) {
        /*
         * If the caller did not get the buffer object we need to delete it.
         */
        CaretLogSevere("Create OpenGL Buffer Object Event never had buffer object retrieved.  Will attempt to delete it.");
        EventGraphicsOpenGLDeleteBufferObject deleteEvent(m_openglBufferObject);
        EventManager::get()->sendEvent(deleteEvent.getPointer());
    }
}

/**
 * Set the OpenGL Buffer Object.
 *
 * @param openglBufferObject
 *     The OpenGL Buffer Object.  This instance will take ownership of the buffer so the caller
 *     MUST NOT ever reference the buffer object after calling this method.
 */
void
EventGraphicsOpenGLCreateBufferObject::setOpenGLBufferObject(GraphicsOpenGLBufferObject* openglBufferObject)
{
    m_openglBufferObject = openglBufferObject;
}

/**
 * @return OpenGL Buffer Object that was created or NULL if 
 * unable to create buffer object due to no valid context.
 * Caller takes ownership of the buffer object.  If this method
 * is called more than once for an instance, NULL will be 
 * returned the second and all subsequent times.
 */
GraphicsOpenGLBufferObject*
EventGraphicsOpenGLCreateBufferObject::getOpenGLBufferObject() const
{
    /*
     * Do not let buffer object be retrieved more than once.
     */
    GraphicsOpenGLBufferObject* bufferObjectPointer = m_openglBufferObject;
    m_openglBufferObject = NULL;
    
    return bufferObjectPointer;
}

