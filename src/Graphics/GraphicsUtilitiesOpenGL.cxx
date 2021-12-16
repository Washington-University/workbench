
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
#include "CaretLogger.h"
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
 * Converts pixels to a percentage of the viewport height.
 *
 * @param pixels
 *     The value in pixels.
 * @return
 The percentage of height [0.0, 100.0] for the given number of millimeters.
 */
float
GraphicsUtilitiesOpenGL::convertPixelsToPercentageOfViewportHeight(const float pixels)
{
    float percentageOfViewportHeight = 1.0f;
    
    EventOpenGLObjectToWindowTransform xform(EventOpenGLObjectToWindowTransform::SpaceType::VOLUME_SLICE_MODEL);
    EventManager::get()->sendEvent(xform.getPointer());
    if (xform.isValid()) {
        const std::array<int32_t, 4> viewport = xform.getViewport();
        
        const float rangePixels = viewport[3];
        if ((rangePixels > 0)
            && (pixels > 0)) {
            percentageOfViewportHeight = (pixels / rangePixels) * 100.0f;
        }
    }
    
    return percentageOfViewportHeight ;
    
}

/**
 * Converts percentage of viewport height to millimeters.
 * The current transformations must be for drawing in millimeters.
 *
 * @param percentOfViewportHeight
 *     The value in percentage of viewport height.
 * @return
 *     Millimeters
 */
float
GraphicsUtilitiesOpenGL::convertPercentageOfViewportHeightToMillimeters(const float percentOfViewportHeight)
{
    float millimeters = -1.0f;
    
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
            millimeters = (percentOfViewportHeight / 100.0) * rangeMillimeters;
        }
    }
    
    return millimeters ;
}

/**
 * Converts percentage of viewport height to pixels.
 * The current transformations must be for drawing in millimeters.
 *
 * @param percentOfViewportHeight
 *     The value in percentage of viewport height.
 * @return
 *     Millimeters
 */
float
GraphicsUtilitiesOpenGL::convertPercentageOfViewportHeightToPixels(const float percentOfViewportHeight)
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    
    float pixels = 0;
    if (vp[3] > 0) {
        pixels = (percentOfViewportHeight / 100.0) * static_cast<float>(vp[3]);
    }
    return pixels;
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
 * Convert millimeters to pixels.
 * The current transformations must be for drawing in millimeters.
 *
 * @param millimeters
 *    The millimeters size
 * @return
 *    Pixels value.
 */
float
GraphicsUtilitiesOpenGL::convertMillimetersToPixels(const float millimeters)
{
    float pixels = 1.0f;
    
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
            const float ratio =  rangePixels / rangeMillimeters;
            pixels = millimeters * ratio;
        }
    }
    
    return pixels;
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

/**
 * Set the OpenGL major and minor version
 * @param majorVersion
 *     Major version
 * @param majorVersion
 *     Major version
 */
void
GraphicsUtilitiesOpenGL::setMajorMinorVersion(const int32_t majorVersion,
                                              const int32_t minorVersion)
{
    s_majorVersion = majorVersion;
    s_minorVersion = minorVersion;
}

/**
 * @return True if the OpenGL major and minor version same or greater
 * @param majorVersion
 *     Major version
 * @param majorVersion
 *     Major version
 */
bool
GraphicsUtilitiesOpenGL::isVersionOrGreater(const int32_t majorVersion,
                                            const int32_t minorVersion)
{
    /*
     * Version unknown, assume valid
     */
    if (s_majorVersion <= 0) {
        return true;
    }
    
    if (s_majorVersion > majorVersion) {
        return true;
    }
    else if (s_majorVersion == majorVersion) {
        if (s_minorVersion >= minorVersion) {
            return true;
        }
    }
    
    return false;
}

/**
 * @return The OpenGL Major Version
 */
int32_t
GraphicsUtilitiesOpenGL::getMajorVersion()
{
    return s_majorVersion;
}

/**
 * @return The OpenGL Minor Version
 */
int32_t
GraphicsUtilitiesOpenGL::getMinorVersion()
{
    return s_minorVersion;
}

/**
 * @return The OpenGL version in form <major>.<minor>
 */
QString
GraphicsUtilitiesOpenGL::getVersion()
{
    const QString txt(AString::number(s_majorVersion)
                      + "."
                      + AString::number(s_minorVersion));
    return txt;
}

/**
 * Set the OpenGL maximum texture dimensions
 * @param widthHeightMaximumDimension
 *     The maximum texture dimension for width or height (texture s or t)
 * @param depthMaximumDimension
 *     The maximum texture dimension for depth (texture r)
 */
void
GraphicsUtilitiesOpenGL::setMaximumTextureDimension(const int32_t widthHeightMaximumDimension,
                                                    const int32_t depthMaximumDimension)
{
    if (widthHeightMaximumDimension > 0) {
        s_textureWidthHeightMaximumDimension = widthHeightMaximumDimension;
    }
    if (depthMaximumDimension > 0) {
        s_textureDepthMaximumDimension = depthMaximumDimension;
    }
}

/**
 * @return The maximum texture dimension for width or height (texture s or t)
 */
int32_t
GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension()
{
    return s_textureWidthHeightMaximumDimension;
}

/**
 * @return The maximum texture dimension for depth (texture r)
 */
int32_t
GraphicsUtilitiesOpenGL::getTextureDepthMaximumDimension()
{
    return s_textureDepthMaximumDimension;
}

/**
 * Push the current OpenGL matrix.  This function behaves just like
 * "glPushMatrix".  With some graphics systems, the projection stack
 * depth is too small (4 on some nvidia systems) and this may
 * cause an OpenGL stack overflow error.  A call to this function
 * MUST be paired with a call to popMatrix().
 */
void
GraphicsUtilitiesOpenGL::pushMatrix()
{
    GLint matrixMode(-1);
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
    if (matrixMode == GL_PROJECTION) {
        const int32_t stackDepth(s_projectionMatrixStack.size());
        if (stackDepth > 32) {
            const QString msg("Projection matrix overflow.");
            CaretLogSevere(msg);
            CaretAssertMessage(0, msg);
        }
        else {
            std::array<double, 16> matrix;
            glGetDoublev(GL_PROJECTION_MATRIX, matrix.data());
            s_projectionMatrixStack.push(matrix);
        }
    }
    else {
        const QString msg("Matrix mode not supported for push/pop matrix.");
        CaretLogSevere(msg);
        CaretAssertMessage(0, msg);
    }
}

/**
 * Pop the current OpenGL matrix.  MUST be paired with cal to pushMatrix().
 */
void
GraphicsUtilitiesOpenGL::popMatrix()
{
    GLint matrixMode(-1);
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
    if (matrixMode == GL_PROJECTION) {
        const int32_t stackDepth(s_projectionMatrixStack.size());
        if (stackDepth < 1) {
            const QString msg("Projection matrix underflow.");
            CaretLogSevere(msg);
            CaretAssertMessage(0, msg);
        }
        else {
            std::array<double, 16> matrix(s_projectionMatrixStack.top());
            s_projectionMatrixStack.pop();
            glLoadMatrixd(matrix.data());
        }
    }
    else {
        const QString msg("Matrix mode not supported for push/pop matrix.");
        CaretLogSevere(msg);
        CaretAssertMessage(0, msg);
    }
}

/**
 * Transform the given window coordinates to model coordinates.
 * OpenGL MUST BE "current".
 * @param windowX
 *    Window X-coordinate.
 * @param windowY
 *    Window Y-coordinate.
 * @param modelXyzOut
 *    Output with model coordinates.
 * @return True if successful, else false.
 */
bool
GraphicsUtilitiesOpenGL::unproject(const float windowX,
                                   const float windowY,
                                   float modelXyzOut[3])
{
    GLdouble modelviewMatrix[16];
    GLdouble projectionMatrix[16];
    GLint viewport[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    double modelX(0.0), modelY(0.0), modelZ(0.0);
    float windowZ(0.0);
    if (gluUnProject(windowX, windowY, windowZ ,
                     modelviewMatrix, projectionMatrix, viewport,
                     &modelX, &modelY, &modelZ) == GL_TRUE) {
        modelXyzOut[0] = modelX;
        modelXyzOut[1] = modelY;
        modelXyzOut[2] = modelZ;
        
        return true;
    }

    return false;
}

