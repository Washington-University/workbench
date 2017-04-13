
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

#define __EVENT_GRAPHICS_ENGINE_OPEN_G_L_DELETE_BUFFERS_DECLARE__
#include "EventGraphicsEngineOpenGLDeleteBuffers.h"
#undef __EVENT_GRAPHICS_ENGINE_OPEN_G_L_DELETE_BUFFERS_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGraphicsEngineOpenGLDeleteBuffers 
 * \brief Event for deleting OpenGL buffers created by the OpenGL Graphics Engine
 * \ingroup Graphics
 */

/**
 * Constructor.
 *
 * @param bufferIdentifiers
 *     Identifiers of buffers for deletion.
 */
EventGraphicsEngineOpenGLDeleteBuffers::EventGraphicsEngineOpenGLDeleteBuffers(const std::vector<GLuint>& bufferIdentifiers)
: Event(EventTypeEnum::EVENT_GRAPHICS_ENGINE_OPENGL_DELETE_BUFFERS)
{
    m_bufferIdentifiers = bufferIdentifiers;
}

/**
 * Constructor.
 *
 * @param bufferIdentifier
 *     Identifier of buffer for deletion.
 */
EventGraphicsEngineOpenGLDeleteBuffers::EventGraphicsEngineOpenGLDeleteBuffers(const GLuint bufferIdentifier)
: Event(EventTypeEnum::EVENT_GRAPHICS_ENGINE_OPENGL_DELETE_BUFFERS)
{
    m_bufferIdentifiers.push_back(bufferIdentifier);
}

/**
 * Destructor.
 */
EventGraphicsEngineOpenGLDeleteBuffers::~EventGraphicsEngineOpenGLDeleteBuffers()
{
}

/**
 * @return Identifiers of OpenGL buffers for deletion.
 */
std::vector<GLuint>
EventGraphicsEngineOpenGLDeleteBuffers::getBufferIdentifiers() const
{
    return m_bufferIdentifiers;
}

