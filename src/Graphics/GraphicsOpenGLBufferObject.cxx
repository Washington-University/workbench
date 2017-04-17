
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

#define __GRAPHICS_OPEN_G_L_BUFFER_OBJECT_DECLARE__
#include "GraphicsOpenGLBufferObject.h"
#undef __GRAPHICS_OPEN_G_L_BUFFER_OBJECT_DECLARE__

#include "CaretAssert.h"
#include "EventGraphicsOpenGLDeleteBufferObject.h"
#include "EventManager.h"

using namespace caret;


    
/**
 * \class caret::GraphicsOpenGLBufferObject 
 * \brief Contains the OpenGL Buffer ID and the context in which it was created.
 * \ingroup Graphics
 */

/**
 * Constructor.
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context in which buffer object name was created.
 * @param bufferObjectName
 *     The OpenGL buffer object name.
 */
GraphicsOpenGLBufferObject::GraphicsOpenGLBufferObject(void* openglContextPointer,
                                                       const GLuint bufferObjectName)
: CaretObject(),
m_openglContextPointer(openglContextPointer),
m_bufferObjectName(bufferObjectName)
{
    
}

/**
 * Destructor.
 */
GraphicsOpenGLBufferObject::~GraphicsOpenGLBufferObject()
{
    if (m_bufferObjectName > 0) {
        EventGraphicsOpenGLDeleteBufferObject deleteEvent(this);
        EventManager::get()->sendEvent(deleteEvent.getPointer());
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsOpenGLBufferObject::toString() const
{
    return "GraphicsOpenGLBufferObject";
}

