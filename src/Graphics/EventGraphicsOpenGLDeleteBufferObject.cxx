
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

#include "ApplicationInformation.h"
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
 *     The OpenGL Buffer Object whose buffer name is deleted.
 *     After this event the openglBufferObject should be deleted by the caller.
 */
EventGraphicsOpenGLDeleteBufferObject::EventGraphicsOpenGLDeleteBufferObject(const GraphicsOpenGLBufferObject* openglBufferObject)
: Event(EventTypeEnum::EVENT_GRAPHICS_OPENGL_DELETE_BUFFER_OBJECT),
m_openglBufferObject(openglBufferObject)
{
    
}

/**
 * Destructor.
 */
EventGraphicsOpenGLDeleteBufferObject::~EventGraphicsOpenGLDeleteBufferObject()
{
    switch (ApplicationInformation::getApplicationType()) {
        case ApplicationTypeEnum::APPLICATION_TYPE_COMMAND_LINE:
            /*
             * Do not test for deletion of OpenGL buffers with
             * the command line program as will require redesign
             * of the -show-scene command and would likely require
             * loading of the scene for each window.
             */
            break;
        case ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE:
            /*
             * Only perform this test for the GUI application.
             */
            if (this->getEventProcessCount() <= 0) {
                CaretLogSevere("Deletion of OpenGL Buffer Object Name="
                               + AString::number(m_openglBufferObject->getBufferObjectName())
                               + ", context pointer="
                               + AString::number((qulonglong)m_openglBufferObject->getOpenGLContextPointer())
                               + " appears to have failed.  The events processed count is zero and that "
                               "indicates that the event was not received in any listener or the "
                               "listener failed to set the event as processed.");
            }
            break;
        case ApplicationTypeEnum::APPLICATION_TYPE_INVALID:
            break;
    }
}

/**
 * @return OpenGL Buffer Object that was deleted
 */
const GraphicsOpenGLBufferObject*
EventGraphicsOpenGLDeleteBufferObject::getOpenGLBufferObject() const
{
    return m_openglBufferObject;
}

