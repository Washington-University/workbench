
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

#define __DRAWN_WITH_OPENGL_TEXTURE_INFO_DECLARE__
#include "DrawnWithOpenGLTextureInfo.h"
#undef __DRAWN_WITH_OPENGL_TEXTURE_INFO_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventOpenGLTexture.h"

using namespace caret;


    
/**
 * \class caret::DrawnWithOpenGLTextureInfo 
 * \brief Assists with managing texture names for items drawn as textures.
 * \ingroup Common
 *
 * OpenGL textures use resources in the graphics system.  Classes
 * implementing this interface must contain an instance of
 * DrawnWithOpenGLTextureInfo which contains information about
 * the OpenGL texture.  When an instance of DrawnWithOpenGLTextureInfo
 * goes out of scope (deleted), it will send a message to
 * OpenGL rendering to free up any associated resources with the
 * OpenGL texture.
 */

/**
 * Constructor.
 */
DrawnWithOpenGLTextureInfo::DrawnWithOpenGLTextureInfo()
: CaretObject()
{
    resetTextureNames();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_OPENGL_TEXTURE);
}

/**
 * Destructor.
 */
DrawnWithOpenGLTextureInfo::~DrawnWithOpenGLTextureInfo()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    /*
     * Need to release the texture names since no longer needed.
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        CaretAssertArrayIndex(m_textureNamesForWindow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, i);
        if (m_textureNamesForWindow[i] > 0) {
            EventOpenGLTexture textureEvent;
            textureEvent.setModeDeleteTexture(i, m_textureNamesForWindow[i]);
            EventManager::get()->sendEvent(textureEvent.getPointer());
        }
    }
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DrawnWithOpenGLTextureInfo::DrawnWithOpenGLTextureInfo(const DrawnWithOpenGLTextureInfo& obj)
: CaretObject(obj)
{
    this->copyHelperDrawnWithOpenGLTextureInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DrawnWithOpenGLTextureInfo&
DrawnWithOpenGLTextureInfo::operator=(const DrawnWithOpenGLTextureInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDrawnWithOpenGLTextureInfo(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DrawnWithOpenGLTextureInfo::copyHelperDrawnWithOpenGLTextureInfo(const DrawnWithOpenGLTextureInfo& /*obj*/)
{
    /*
     * Do not copy the texture identfiers.
     */
    resetTextureNames();
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
DrawnWithOpenGLTextureInfo::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_OPENGL_TEXTURE) {
        EventOpenGLTexture* textureEvent = dynamic_cast<EventOpenGLTexture*>(event);
        CaretAssert(textureEvent);
        
        switch (textureEvent->getMode()) {
            case EventOpenGLTexture::MODE_NONE:
                break;
            case EventOpenGLTexture::MODE_DELETE_ALL_TEXTURES_IN_WINDOW:
            {
                int32_t windowIndex = 0;
                textureEvent->getModeDeleteAllTexturesInWindow(windowIndex);
                if ((windowIndex >= 0)
                    && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)) {
                    m_textureNamesForWindow[windowIndex] = 0;
                    textureEvent->setEventProcessed();
                }
            }
                break;
            case EventOpenGLTexture::MODE_DELETE_TEXTURE:
                break;
        }
    }
}

/**
 * Reset the texture names.
 */
void
DrawnWithOpenGLTextureInfo::resetTextureNames()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        CaretAssertArrayIndex(m_textureNamesForWindow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, i);
        m_textureNamesForWindow[i] = -1;
    }
}


/**
 * Get the texture name for the given window index.
 *
 * @param windowIndex
 *     Index of the window.
 * @return
 *     The texture name.  Valid texture names are always
 *     greater than zero.
 */
int32_t
DrawnWithOpenGLTextureInfo::getTextureNameForWindow(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_textureNamesForWindow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_textureNamesForWindow[windowIndex];
}

/**
 * Set the texture name for the given window index.
 *
 * @param windowIndex
 *     Index of the window.
 * @param textureName
 *     The texture name.  Valid texture name are always
 *     greater than zero.
 */
void
DrawnWithOpenGLTextureInfo::setTextureNameForWindow(const int32_t windowIndex,
                                           const int32_t textureName)
{
    CaretAssertArrayIndex(m_textureNamesForWindow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_textureNamesForWindow[windowIndex] = textureName;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
DrawnWithOpenGLTextureInfo::toString() const
{
    return "DrawnWithOpenGLTextureInfo";
}

