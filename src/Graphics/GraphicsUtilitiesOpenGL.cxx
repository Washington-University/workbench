
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

#define __GRAPHICS_UTILITIES_OPEN_G_L_DECLARE__
#include "GraphicsUtilitiesOpenGL.h"
#undef __GRAPHICS_UTILITIES_OPEN_G_L_DECLARE__

#include <array>
#include <cmath>

#include "CaretAssert.h"
#include "CaretOpenGLInclude.h"
#include "GraphicsOpenGLError.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "MathFunctions.h"
using namespace caret;


    
/**
 * \class caret::GraphicsUtilitiesOpenGL 
 * \brief Utility functions for use with OpenGL.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsUtilitiesOpenGL::GraphicsUtilitiesOpenGL()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
GraphicsUtilitiesOpenGL::~GraphicsUtilitiesOpenGL()
{
}

/**
 * Converts millimeters to a percentage of the viewport height.
 * The current transformations must be for drawing in millimeters.
 *
 * @param millimeters
 *     The value in millimeters.
 * @return
       The percentage of height [0.0, 100.0] for the given number of millimeters.
 */
float
GraphicsUtilitiesOpenGL::convertMillimetersToPercentageOfViewportHeight(const float millimeters)
{
    float percentageOfViewportHeight = -1.0f;
    
    EventOpenGLObjectToWindowTransform xform(EventOpenGLObjectToWindowTransform::SpaceType::VOLUME_SLICE_MODEL);
    EventManager::get()->sendEvent(xform.getPointer());
    if (xform.isValid()) {
        const std::array<int32_t, 4> viewport = xform.getViewport();
        
        const float windowZ = 0.0f;
        float bottomWindowXYZ[3] = { (float)viewport[0], (float)viewport[1], (float)windowZ };
        float topWindowXYZ[3]    = { (float)viewport[0], (float)(viewport[1] + viewport[3]), (float)windowZ };
        
        float bottomModelXYZ[3];
        float topModelXYZ[3];
        
        xform.inverseTransformPoint(bottomWindowXYZ, bottomModelXYZ);
        xform.inverseTransformPoint(topWindowXYZ, topModelXYZ);
        
        const float rangeMillimeters = MathFunctions::distance3D(bottomModelXYZ,
                                                                 topModelXYZ);
        const float rangePixels = viewport[3];
        if ((rangePixels > 0)
            && (rangeMillimeters > 0)) {
            percentageOfViewportHeight = (millimeters / rangeMillimeters) * 100.0f;
        }
    }

    return percentageOfViewportHeight ;

}

/**
 * Convert pixels to millimeters.
 * The current transformations must be for drawing in millimeters.
 *
 * @param pixels
 *    The pixels size
 * @return
 *    Millimeters value.
 */
float
GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(const float pixels)
{
    float mm = 1.0f;
    
    EventOpenGLObjectToWindowTransform xform(EventOpenGLObjectToWindowTransform::SpaceType::VOLUME_SLICE_MODEL);
    EventManager::get()->sendEvent(xform.getPointer());
    if (xform.isValid()) {
        const std::array<int32_t, 4> viewport = xform.getViewport();
        
        const float windowZ = 0.0f;
        float bottomWindowXYZ[3] = { (float)viewport[0], (float)viewport[1], (float)windowZ };
        float topWindowXYZ[3]    = { (float)viewport[0], (float)(viewport[1] + viewport[3]), (float)windowZ };
        
        float bottomModelXYZ[3];
        float topModelXYZ[3];
        
        xform.inverseTransformPoint(bottomWindowXYZ, bottomModelXYZ);
        xform.inverseTransformPoint(topWindowXYZ, topModelXYZ);
        
        const float rangeMillimeters = MathFunctions::distance3D(bottomModelXYZ,
                                                                 topModelXYZ);
        const float rangePixels = viewport[3];
        if ((rangePixels > 0)
            && (rangeMillimeters > 0)) {
            const float ratio = rangeMillimeters / rangePixels;
            mm = pixels * ratio;
        }
    }

    return mm;
}

/**
 * Reset and ignore any OpenGL errors.
 */
void
GraphicsUtilitiesOpenGL::resetOpenGLError()
{
    GLenum errorCode = glGetError();
    while (errorCode != GL_NO_ERROR) {
        errorCode = glGetError();
    }
}

/**
 * @return Pointer to OpenGL Error Information.
 * If the pointer is not valid, there is no error.
 *
 * @param message
 *    Optional message added to the OpenGL error
 */
std::unique_ptr<GraphicsOpenGLError>
GraphicsUtilitiesOpenGL::getOpenGLError(const AString& message)
{
    std::unique_ptr<GraphicsOpenGLError> errorInfo;
    
    GLenum errorCode = glGetError();
    while (errorCode != GL_NO_ERROR) {
        if ( ! errorInfo) {
            errorInfo.reset(new GraphicsOpenGLError(message));
        }
        errorInfo->addError(errorCode);
        
        errorCode = glGetError();
    }
    
    return errorInfo;
}


