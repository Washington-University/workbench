
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

#define __EVENT_GRAPHICS_OPEN_G_L_DELETE_TEXTURE_NAME_DECLARE__
#include "EventGraphicsOpenGLDeleteTextureName.h"
#undef __EVENT_GRAPHICS_OPEN_G_L_DELETE_TEXTURE_NAME_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"
#include "GraphicsOpenGLTextureName.h"

using namespace caret;


    
/**
 * \class caret::EventGraphicsOpenGLDeleteTextureName 
 * \brief Delete an OpenGL Texture Name
 * \ingroup Graphics
 *
 * Deletes an OpenGL Texture Name for the current context.
 */
/**
 * Constructor.
 *
 * @param openglTextureName
 *     The OpenGL Texture Name whose texture name is deleted.
 *     After this event the openglTextureName should be deleted by the caller.
 */
EventGraphicsOpenGLDeleteTextureName::EventGraphicsOpenGLDeleteTextureName(const GraphicsOpenGLTextureName* openglTextureName)
: Event(EventTypeEnum::EVENT_GRAPHICS_OPENGL_DELETE_TEXTURE_NAME),
m_openglTextureName(openglTextureName)
{
    
}

/**
 * Destructor.
 */
EventGraphicsOpenGLDeleteTextureName::~EventGraphicsOpenGLDeleteTextureName()
{
    if (s_disableFailureToDeleteWarningMessages) {
        return;
    }
    
    if (this->getEventProcessCount() <= 0) {
        CaretLogSevere("Deletion of OpenGL Texture Name="
                       + AString::number(m_openglTextureName->getTextureName())
                       + ", context pointer="
                       + AString::number((qulonglong)m_openglTextureName->getOpenGLContextPointer())
                       + " appears to have failed.  The events processed count is zero and that "
                       "indicates that the event was not received in any listener or the "
                       "listener failed to set the event as processed.");
    }
}

/**
 * @return OpenGL Texture Name that was deleted
 */
const GraphicsOpenGLTextureName*
EventGraphicsOpenGLDeleteTextureName::getOpenGLTextureName() const
{
    return m_openglTextureName;
}

/**
 * Disable the warning messages in the destructor when a texture has not been deleted.
 * Sort of a kludge.  There is a bug in the -show-scene command that is not easy to fix that will cause this messge.
 * @param status
 *    New status for disabling message.
 */
void
EventGraphicsOpenGLDeleteTextureName::setDisableFailureToDeleteWarningMessages(const bool status)
{
    s_disableFailureToDeleteWarningMessages = status;
}
