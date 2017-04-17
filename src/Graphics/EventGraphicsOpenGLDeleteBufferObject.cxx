
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

#define __EVENT_GRAPHICS_OPEN_G_L_DELETE_BUFFER_OBJECT_DECLARE__
#include "EventGraphicsOpenGLDeleteBufferObject.h"
#undef __EVENT_GRAPHICS_OPEN_G_L_DELETE_BUFFER_OBJECT_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"
#include "GraphicsOpenGLBufferObject.h"

using namespace caret;


    
/**
 * \class caret::EventGraphicsOpenGLDeleteBufferObject 
 * \brief Delete an OpenGL Buffer Object
 * \ingroup Graphics
 *
 * Deletes an OpenGL Buffer Object for the current context.
 */
/**
 * Constructor.
 *
 * @param openglBufferObject
 *     The OpenGL Buffer Object.  This instance will take ownership of the buffer so the caller
 *     MUST NOT ever reference the buffer object after calling this method.
 */
EventGraphicsOpenGLDeleteBufferObject::EventGraphicsOpenGLDeleteBufferObject(GraphicsOpenGLBufferObject* openglBufferObject)
: Event(EventTypeEnum::EVENT_GRAPHICS_OPENGL_DELETE_BUFFER_OBJECT),
m_openglBufferObject(openglBufferObject)
{
    
}

/**
 * Destructor.
 */
EventGraphicsOpenGLDeleteBufferObject::~EventGraphicsOpenGLDeleteBufferObject()
{
    if (m_openglBufferObject != NULL) {
        CaretLogSevere("Failure to delete OpenGL Buffer Object Name="
                       + AString::number(m_openglBufferObject->getBufferObjectName())
                       + ", context pointer="
                       + AString::number((qulonglong)m_openglBufferObject->getOpenGLContextPointer()));
    }
}

/**
 * @return OpenGL Buffer Object that was created or NULL if
 * unable to create buffer object due to no valid context.
 * Caller takes ownership of the buffer object.  If this method
 * is called more than once, NULL will be returned the second
 * and all subsequent times.
 */
GraphicsOpenGLBufferObject*
EventGraphicsOpenGLDeleteBufferObject::getOpenGLBufferObject() const
{
    /*
     * Do not let buffer object be retrieved more than once.
     */
    GraphicsOpenGLBufferObject* bufferObjectPointer = m_openglBufferObject;
    m_openglBufferObject = NULL;
    
    return bufferObjectPointer;
}

