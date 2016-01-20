
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __EVENT_OPENGL_TEXTURE_DECLARE__
#include "EventOpenGLTexture.h"
#undef __EVENT_OPENGL_TEXTURE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventOpenGLTexture 
 * \brief Event that assists with management of OpenGL textures.
 * \ingroup Common
 *
 * When an object is destroyed that used texture resources, the
 * texture resources need to be released.  This event can be 
 * sent by the object to the texture manager to free these
 * texture resources.
 */

/**
 * Constructor.
 */
EventOpenGLTexture::EventOpenGLTexture()
: Event(EventTypeEnum::EVENT_OPENGL_TEXTURE),
m_mode(MODE_NONE),
m_windowIndex(-1),
m_textureName(0)
{
}

/**
 * Destructor.
 */
EventOpenGLTexture::~EventOpenGLTexture()
{
}

/**
 * @return The mode.
 */
EventOpenGLTexture::Mode
EventOpenGLTexture::getMode() const
{
    return m_mode;
}

/**
 * Set the mode to delete all textures for a given window.
 * 
 * @param windowIndex
 *     Index of the window.
 */
void
EventOpenGLTexture::setModeDeleteAllTexturesInWindow(const int32_t windowIndex)
{
    m_mode = MODE_DELETE_ALL_TEXTURES_IN_WINDOW;
    m_windowIndex = windowIndex;
}

/**
 * Get the mode to delete all textures for a given window.
 *
 * @param windowIndex
 *     Index of the window.
 */
void
EventOpenGLTexture::getModeDeleteAllTexturesInWindow(int32_t windowIndexOut) const
{
    windowIndexOut = m_windowIndex;
}

/**
 * Set the mode to delete texture name for window.
 *
 * @param windowIndex
 *     Index of window in which texture is used.
 * @param textureName
 *     OpenGL texture name.
 */
void
EventOpenGLTexture::setModeDeleteTexture(const int32_t windowIndex,
                                         const int32_t textureName)
{
    m_mode = MODE_DELETE_TEXTURE;
    m_windowIndex = windowIndex;
    m_textureName = textureName;
}

/**
 * Get the mode to delete texture name for window.
 *
 * @param windowIndex
 *     Index of window in which texture is used.
 * @param textureName
 *     OpenGL texture name.
 */
void
EventOpenGLTexture::getModeDeleteTexture(int32_t& windowIndexOut,
                                         int32_t& textureNameOut) const
{
    windowIndexOut = m_windowIndex;
    textureNameOut = m_textureName;
}

