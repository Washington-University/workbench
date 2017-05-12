
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

#define __EVENT_GRAPHICS_OPEN_G_L_CREATE_TEXTURE_NAME_DECLARE__
#include "EventGraphicsOpenGLCreateTextureName.h"
#undef __EVENT_GRAPHICS_OPEN_G_L_CREATE_TEXTURE_NAME_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventGraphicsOpenGLDeleteTextureName.h"
#include "EventManager.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventGraphicsOpenGLCreateTextureName 
 * \brief Create an OpenGL Texture Name
 * \ingroup Graphics
 *
 * Creates an OpenGL Texture Name for the current context.
 * There MUST BE an active OpenGL context when this event
 * is processed.  Otherwise, a texture name cannot be
 * created and a program failure will likely occur.
 */

/**
 * Constructor.
 */
EventGraphicsOpenGLCreateTextureName::EventGraphicsOpenGLCreateTextureName()
: Event(EventTypeEnum::EVENT_GRAPHICS_OPENGL_CREATE_TEXTURE_NAME)
{
    
}

/**
 * Destructor.
 */
EventGraphicsOpenGLCreateTextureName::~EventGraphicsOpenGLCreateTextureName()
{
    if (m_openglTextureName != NULL) {
        /*
         * If the caller did not get the texture name we need to delete it.
         */
        CaretLogSevere("Create OpenGL Texture Name Event never had texture name retrieved.  Will attempt to delete it.");
        EventGraphicsOpenGLDeleteTextureName deleteEvent(m_openglTextureName);
        EventManager::get()->sendEvent(deleteEvent.getPointer());
    }
}

/**
 * Set the OpenGL Texture Name
 *
 * @param openglTextureName
 *     The OpenGL Texture Name.  This instance will take ownership of the texture so the caller
 *     MUST NOT ever reference the texture name after calling this method.
 */
void
EventGraphicsOpenGLCreateTextureName::setOpenGLTextureName(GraphicsOpenGLTextureName* openglTextureName)
{
    m_openglTextureName = openglTextureName;
}

/**
 * @return OpenGL Texture Name that was created or NULL if
 * unable to create texture name due to no valid context.
 * Caller takes ownership of the texture name.  If this method
 * is called more than once for an instance, NULL will be 
 * returned the second and all subsequent times.
 */
GraphicsOpenGLTextureName*
EventGraphicsOpenGLCreateTextureName::getOpenGLTextureName() const
{
    /*
     * Do not let texture name be retrieved more than once.
     */
    GraphicsOpenGLTextureName* textureNamePointer = m_openglTextureName;
    m_openglTextureName = NULL;
    
    return textureNamePointer;
}

