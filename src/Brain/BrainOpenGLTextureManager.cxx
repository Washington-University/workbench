
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

#include <limits>

#define __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__
#include "BrainOpenGLTextureManager.h"
#undef __BRAIN_OPEN_G_L_TEXTURE_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DrawnWithOpenGLTextureInfo.h"
#include "EventManager.h"
#include "EventOpenGLTexture.h"
using namespace caret;


    
/**
 * \class caret::BrainOpenGLTextureManager 
 * \brief Manages allocated OpenGL Textures.
 * \ingroup Brain
 *
 * This texture manager helps out with tracking OpenGL
 * textures.  There are instance in which textures need
 * to be deleted such as when QGLWidget::renderPixmap()
 * is used to capture an image of the graphics region.
 *
 * Note that the FTGL text rendering code also uses
 * texture and this texture manager does not interact
 * in any way with FTGL.
 */

/**
 * Constructor.
 *
 * @param windowIndex
 *    Index of window for which textures are managed.
 */
BrainOpenGLTextureManager::BrainOpenGLTextureManager(const int32_t windowIndex)
: CaretObject(),
m_windowIndex(windowIndex)
{
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_OPENGL_TEXTURE);
}

/**
 * Destructor.
 */
BrainOpenGLTextureManager::~BrainOpenGLTextureManager()
{
    /*
     * We do not need to worry about deleting texture names
     * since when a instance of this class is deleted, the
     * OpenGL context is also deleted which deletes all
     * of the texture names.
     */
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
BrainOpenGLTextureManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_OPENGL_TEXTURE) {
        EventOpenGLTexture* textureEvent = dynamic_cast<EventOpenGLTexture*>(event);
        CaretAssert(textureEvent);
        
        switch (textureEvent->getMode()) {
            case EventOpenGLTexture::MODE_NONE:
                break;
            case EventOpenGLTexture::MODE_DELETE_ALL_TEXTURES_IN_WINDOW:
                break;
            case EventOpenGLTexture::MODE_DELETE_TEXTURE:
            {
                int32_t windowIndex = -1;
                int32_t textureName = 0;
                textureEvent->getModeDeleteTexture(windowIndex,
                                                   textureName);
                if (m_windowIndex == windowIndex) {
                    deleteTextureName(textureName);
                    
                    textureEvent->setEventProcessed();
                }
            }
                break;
        }
    }
}

/**
 * Delete all textures for a given window index.
 *
 * @param windowIndex
 *     Index of the window.
 */
void
BrainOpenGLTextureManager::deleteAllTexturesForWindow(const int32_t windowIndex)
{
    EventOpenGLTexture textureEvent;
    textureEvent.setModeDeleteAllTexturesInWindow(windowIndex);
    EventManager::get()->sendEvent(textureEvent.getPointer());
}


/**
 * Get the texture name for the given texture info.
 *
 * @param textureInfo
 *     The texture information.
 * @param textureNameOut
 *     Output containing OpenGL texture name for use with glBindTexture.
 * @param newTextureNameFlagOut
 *     If true, the texture name is new and the caller must create
 *     or recreate the texture image.  Recreation may be necessary
 *     after an image capture operation due to it creating a new
 *     OpenGL context.
 */
void
BrainOpenGLTextureManager::getTextureName(DrawnWithOpenGLTextureInfo* textureInfo,
                                          GLuint& textureNameOut,
                                          bool& newTextureNameFlagOut)
{
    textureNameOut        = 0;
    newTextureNameFlagOut = false;
    
    CaretAssert(m_windowIndex >= 0);
    textureNameOut = textureInfo->getTextureNameForWindow(m_windowIndex);
    if (textureNameOut > 0) {
        /*
         * Verify texture is still valid (could be deleted during image capture
         * which recreates the OpenGL context).
         */
        if (glIsTexture(textureNameOut)) {
            return;
        }
    }
    
    textureNameOut = createNewTextureName();
    textureInfo->setTextureNameForWindow(m_windowIndex,
                                               textureNameOut);
    newTextureNameFlagOut = true;
}

/**
 * @return A new texture name.
 */
GLuint
BrainOpenGLTextureManager::createNewTextureName()
{
    /*
     * Each workbench window has its own, unique OpenGL context.
     * Identical OpenGL texture names may exist in each OpenGL
     * context.  To simplify management of textures, we do not
     * want the same texture name in more than one OpenGL 
     * context.
     *
     * So, we do not use glGenTexture() to create texture
     * names.  Instead, we use our own generator that
     * ensures each name is never used in more than one
     * OpenGL context.  This simplifies management of textures
     * and especially the removal of textures when an
     * object drawn using a texture is deleted.
     *
     * In addition, other modules (such as FTGL font
     * rendering) may use textures, so we need to 
     * ensure that a new name is not used elsewhere.
     */
    s_textureNameGenerator++;
    if (s_textureNameGenerator == std::numeric_limits<int32_t>::max()) {
        s_textureNameGenerator = 1;
    }
    while (glIsTexture(s_textureNameGenerator) == GL_TRUE) {
        s_textureNameGenerator++;
        if (s_textureNameGenerator == std::numeric_limits<int32_t>::max()) {
            s_textureNameGenerator = 1;
        }
    }
    
    return s_textureNameGenerator;
}


/**
 * Delete a texture name.
 *
 * @param textureName
 *     Texture name that is deleted.
 */
void
BrainOpenGLTextureManager::deleteTextureName(GLuint textureName)
{
    if (glIsTexture(textureName) == GL_TRUE) {
        glDeleteTextures(1, &textureName);
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLTextureManager::toString() const
{
    return "BrainOpenGLTextureManager";
}

