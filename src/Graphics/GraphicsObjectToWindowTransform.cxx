
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

#define __GRAPHICS_OBJECT_TO_WINDOW_TRANSFORM_DECLARE__
#include "GraphicsObjectToWindowTransform.h"
#undef __GRAPHICS_OBJECT_TO_WINDOW_TRANSFORM_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::GraphicsObjectToWindowTransform
 * \brief Get a transform for converting OpenGL Object to Window Coordinates
 * \ingroup Graphics
 *
 * Creates a transform (matrix) that is used to convert
 * object coordinates to window coordinates.
 * There must be a valid OpenGL context that is "current".
 */

/**
 * Constructor.
 *
 */
GraphicsObjectToWindowTransform::GraphicsObjectToWindowTransform()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
GraphicsObjectToWindowTransform::~GraphicsObjectToWindowTransform()
{
}

GraphicsObjectToWindowTransform::GraphicsObjectToWindowTransform(const GraphicsObjectToWindowTransform& transform)
: CaretObject(transform)
{
    copyHelper(transform);
}

GraphicsObjectToWindowTransform&
GraphicsObjectToWindowTransform::operator=(const GraphicsObjectToWindowTransform& transform)
{
    if (this != &transform) {
        copyHelper(transform);
    }
    
    return *this;
}

/**
 * Copy instance
 * @param transform
 *    Instance that is copied to this
 */
void
GraphicsObjectToWindowTransform::copyHelper(const GraphicsObjectToWindowTransform& transform)
{
    m_spaceType       = transform.m_spaceType;
    m_transformMatrix = transform.m_transformMatrix;
    m_inverseTransformMatrix.reset();
    if (transform.m_inverseTransformMatrix) {
        m_inverseTransformMatrix.reset(new Matrix4x4(*transform.m_inverseTransformMatrix));
    }
    m_projectionMatrixArray = transform.m_projectionMatrixArray;
    m_viewport              = transform.m_viewport;
    m_depthRange            = transform.m_depthRange;
    m_centerToEyeDistance   = transform.m_centerToEyeDistance;
    m_orthoLRBT             = transform.m_orthoLRBT;
    m_validFlag             = transform.m_validFlag;
}

/**
 * @return The object to window matrix.
 */
Matrix4x4
GraphicsObjectToWindowTransform::getMatrix() const
{
    return m_transformMatrix;
}

/**
 * @return The modelview matrix.
 */
Matrix4x4
GraphicsObjectToWindowTransform::getModelviewMatrix() const
{
    return m_modelviewMatrix;
}

/**
 * @return Is the data for performing transformations valid?
 */
bool
GraphicsObjectToWindowTransform::isValid() const
{
    return m_validFlag;
}

void
GraphicsObjectToWindowTransform::setValid(const bool validFlag)
{
    m_validFlag = validFlag;
}

/**
 * Transform a window coordinate to an object coordinate.
 *
 * @param windowXYZ
 *     The window coordinate.
 * @param objectXYZOut
 *     Output containing the computed object coordinate.
 * @return
 *     True if output coordinate is valid, else false.
 */
bool
GraphicsObjectToWindowTransform::inverseTransformPoint(const float windowXYZ[3],
                           float objectXYZOut[3]) const
{
    if ( ! m_validFlag) {
        CaretAssert(0);
        CaretLogSevere("Program Error: GraphicsObjectToWindowTransform is not valid");
        return false;
    }
    
    /*
     * If needed, create the inverse matrix
     */
    if ( ! m_inverseTransformMatrix) {
        m_inverseTransformMatrix.reset(new Matrix4x4(m_transformMatrix));
        
        if ( ! m_inverseTransformMatrix->invert()) {
            CaretAssert(0);
            CaretLogSevere("Program Error: GraphicsObjectToWindowTransform unable to invert matrix");
            return false;
        }
    }
    
    float xyzw[4] {
        (2.0f * (windowXYZ[0] - m_viewport[0]) / m_viewport[2]) - 1.0f,
        (2.0f * (windowXYZ[1] - m_viewport[1]) / m_viewport[3]) - 1.0f,
        (2.0f * windowXYZ[2]) - 1.0f,
        1.0f
    };
    
    m_inverseTransformMatrix->multiplyPoint4(xyzw);
    
    objectXYZOut[0] = xyzw[0];
    objectXYZOut[1] = xyzw[1];
    objectXYZOut[2] = xyzw[2];
    
    return true;
}

/**
 * Transform a window coordinate to an object coordinate.
 *
 * @param windowX
 *     The window X-coordinate.
 * @param windowY
 *     The window Y-coordinate.
 * @param windowZ
 *     The window Z-coordinate.
 * @param objectXYZOut
 *     Output containing the computed object coordinate.
 * @return
 *     True if output coordinate is valid, else false.
 */
bool
GraphicsObjectToWindowTransform::inverseTransformPoint(const float windowX,
                                                       const float windowY,
                                                       const float windowZ,
                                                       float objectXYZOut[3]) const
{
    const float windowXYZ[3] { windowX, windowY, windowZ };
    return inverseTransformPoint(windowXYZ,
                                 objectXYZOut);
}


/**
 * Transform an object coordinate to a window coordinate.
 *
 * @param objectXYZ
 *     The object's coordinate.
 * @param windowXYZOut
 *     Output containing the computed window coordinate.
 * @return
 *     True if output coordinate is valid, else false.
 */
bool
GraphicsObjectToWindowTransform::transformPoint(const float objectXYZ[3],
                                                   float windowXYZOut[3]) const
{
    if ( ! m_validFlag) {
        CaretAssert(0);
        CaretLogSevere("Program Error: GraphicsObjectToWindowTransform is not valid");
        return false;
    }
    
    float xyzw[4] = { objectXYZ[0], objectXYZ[1], objectXYZ[2], 1.0f };
    m_transformMatrix.multiplyPoint4(xyzw);
    windowXYZOut[0] = m_viewport[0] + (m_viewport[2] * ((xyzw[0] + 1.0f) / 2.0f));
    windowXYZOut[1] = m_viewport[1] + (m_viewport[3] * ((xyzw[1] + 1.0f) / 2.0f));
    windowXYZOut[2] = (xyzw[2] + 1.0f) / 2.0f;
    
    /*
     * From OpenGL Programming Guide 3rd Ed, p 133:
     *
     * If the near value is 1.0 and the far value is 3.0,
     * objects must have z-coordinates between -1.0 and -3.0 in order to be visible.
     * So, negate the Z-value to be negative.
     */
    const float windowZ = windowXYZOut[2];
    windowXYZOut[2] = -windowXYZOut[2];
    
    bool modelSpaceFlag  = false;
    bool volumeSpaceFlag = false;
    switch (m_spaceType) {
        case SpaceType::INVALID:
            CaretAssert(0);
            break;
        case SpaceType::MODEL:
            modelSpaceFlag = true;
            break;
        case SpaceType::VOLUME_SLICE_MODEL:
            modelSpaceFlag  = true;
            volumeSpaceFlag = true;
            break;
        case SpaceType::WINDOW:
            break;
    }
    /*
     * If window space, we DO NOT need to offset for the 'eye' location
     * Even when we do, the offset for the eye is very small
     */
    if (modelSpaceFlag) {
        if ((m_projectionMatrixArray[0] != 0.0)
            && (m_projectionMatrixArray[5] != 0.0)
            && (m_projectionMatrixArray[10] != 0.0)) {
            /*
             * From http://lektiondestages.blogspot.com/2013/11/decompose-opengl-projection-matrix.html
             */
            float nearValue   =  (1.0f + m_projectionMatrixArray[14]) / m_projectionMatrixArray[10];
            float farValue    = -(1.0f - m_projectionMatrixArray[14]) / m_projectionMatrixArray[10];
            float left   = -(1.0f + m_projectionMatrixArray[12]) / m_projectionMatrixArray[0];
            float right  =  (1.0f - m_projectionMatrixArray[12]) / m_projectionMatrixArray[0];
            
            /*
             * Depending upon view, near may be positive and far negative
             */
            const float farNearRange = std::fabs(farValue - nearValue);
            if ((m_centerToEyeDistance > 0.0)
                && (farNearRange > 0.0)) {
                /*
                 * Using gluLookAt moves the eye away from the center which
                 * causes the window Z to also move.  Thus, we need to remove
                 * this amount from the window's Z-coordinate.
                 */
                const float eyeAdjustment = (m_centerToEyeDistance / farNearRange);
                if (volumeSpaceFlag) {
                    windowXYZOut[2] = -(windowZ - eyeAdjustment);
                }
                else if (left > right) {
                    windowXYZOut[2] = -(windowZ - eyeAdjustment);
                }
                else {
                    windowXYZOut[2] = -(windowZ + eyeAdjustment);
                }
            }
        }
    }
    
    return true;
}

void
GraphicsObjectToWindowTransform::replaceModelviewMatrix(const std::array<double, 16>& modelviewMatrixArray)
{
    m_inverseTransformMatrix.reset();
    setup(m_spaceType,
          modelviewMatrixArray,
          m_projectionMatrixArray,
          m_viewport,
          m_depthRange,
          m_orthoLRBT,
          m_centerToEyeDistance);
}

void
GraphicsObjectToWindowTransform::modelViewTransformPoint(const float xyz[3],
                                                         float xyzOut[3]) const
{
    xyzOut[0] = xyz[0];
    xyzOut[1] = xyz[1];
    xyzOut[2] = xyz[2];
    m_modelviewMatrix.multiplyPoint3(xyzOut);
}

bool
GraphicsObjectToWindowTransform::modelViewInverseTransformPoint(const float xyz[3],
                                                                float xyzOut[3]) const
{
    Matrix4x4 m(m_modelviewMatrix);
    if ( ! m.invert()) {
        return false;
    }
    xyzOut[0] = xyz[0];
    xyzOut[1] = xyz[1];
    xyzOut[2] = xyz[2];
    m.multiplyPoint3(xyzOut);
    return true;
}

/**
 * @return a clone of this instance but replace the modelview matrix
 * @param modelviewMatrix
 *   New modelview matrix
 */
std::unique_ptr<GraphicsObjectToWindowTransform>
GraphicsObjectToWindowTransform::cloneWithNewModelViewMatrix(const Matrix4x4& modelviewMatrix) const
{
    std::unique_ptr<GraphicsObjectToWindowTransform> ptr(new GraphicsObjectToWindowTransform());
    
    std::array<double,16> modelviewMatrixArray;
    modelviewMatrix.getMatrixForOpenGL(modelviewMatrixArray.data());
    
    ptr->setup(m_spaceType,
               modelviewMatrixArray,
               m_projectionMatrixArray,
               m_viewport,
               m_depthRange,
               m_orthoLRBT,
               m_centerToEyeDistance);
    
    return ptr;
}


/**
 * Setup the transformation's data.
 *
 * @param modelviewMatrixArray
 *     The OpenGL modelview matrix as an array.
 * @param projectionMatrixArray
 *     The OpenGL projection matrix as an array.
 * @param viewport
 *     The OpenGL Viewport (x, y, w, h)
 * @param depthRange
 *     The depth range (near, far)
 * @param orthoLRBT
 *     Orthographic left, right, bottom, top
 * @param centerToEyeDistance
 *     Distance from object space center to the viewer's eye along Z-axis.
 */
void
GraphicsObjectToWindowTransform::setup(const SpaceType spaceType,
                                       const std::array<double, 16>& modelviewMatrixArray,
                                       const std::array<double, 16>& projectionMatrixArray,
                                       const std::array<int32_t, 4>& viewport,
                                       const std::array<double, 2>& depthRange,
                                       const std::array<float, 4>& orthoLRBT,
                                       const double centerToEyeDistance)
{
    CaretAssert(spaceType != SpaceType::INVALID);
    m_spaceType             = spaceType;
    m_projectionMatrixArray = projectionMatrixArray;
    m_viewport              = viewport;
    m_depthRange            = depthRange;
    m_centerToEyeDistance   = centerToEyeDistance;
    m_orthoLRBT             = orthoLRBT;
    
    m_modelviewMatrix.setMatrixFromOpenGL(modelviewMatrixArray.data());
    
    m_transformMatrix.setMatrixFromOpenGL(m_projectionMatrixArray.data());
    //m_projectionMatrix.postmultiply(modelviewMatrix);
    m_transformMatrix.premultiply(m_modelviewMatrix);
    
    const bool autoDetectWindowSpaceFlag = false;
    if (autoDetectWindowSpaceFlag) {
        /*
         * Far/near from http://lektiondestages.blogspot.com/2013/11/decompose-opengl-projection-matrix.html
         */
        const float nearValue   =  (1.0f + m_projectionMatrixArray[14]) / m_projectionMatrixArray[10];
        const float farValue    = -(1.0f - m_projectionMatrixArray[14]) / m_projectionMatrixArray[10];
        
        /*
         * Window space when near is zero, far is one
         */
        const float smallValue = 0.005;
        if ((std::fabs(nearValue) < smallValue)
            && (std::fabs(farValue - 1.0) < smallValue)) {
            //m_windowDepthFlag = true;
        }
    }
    
    m_validFlag = true;
}

/**
 * @return The viewport.
 */
std::array<int32_t, 4>
GraphicsObjectToWindowTransform::getViewport() const
{
    return m_viewport;
}

/**
 * @return Orthographic left, right, bottom top
 */
std::array<float, 4>
GraphicsObjectToWindowTransform::getOrthoLRBT() const
{
    return m_orthoLRBT;
}


