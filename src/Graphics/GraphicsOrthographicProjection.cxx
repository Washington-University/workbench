
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __GRAPHICS_ORTHOGRAPHIC_PROJECTION_DECLARE__
#include "GraphicsOrthographicProjection.h"
#undef __GRAPHICS_ORTHOGRAPHIC_PROJECTION_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "CaretOpenGLInclude.h"

using namespace caret;


    
/**
 * \class caret::GraphicsOrthographicProjection 
 * \brief Data for an OpenGL Orthographic Projection
 * \ingroup Graphics
 */

/**
 * Constructor with an invalid orthographic projection (all values zero)
 */
GraphicsOrthographicProjection::GraphicsOrthographicProjection()
: CaretObject()
{
    
}

/**
 * Constructor.
 * @param left
 *    Left side of projection
 * @param right
 *    Right side of projection
 * @param bottom
 *    Bottom side of projection
 * @param top
 *    Top side of projection
 * @param nearValue
 *    Near side of projection
 * @param farValue
 *    Far side of projection
 */
GraphicsOrthographicProjection::GraphicsOrthographicProjection(const double left,
                                                               const double right,
                                                               const double bottom,
                                                               const double top,
                                                               const double nearValue,
                                                               const double farValue)
: m_left(left),
m_right(right),
m_bottom(bottom),
m_top(top),
m_near(nearValue),
m_far(farValue)
{
}

/**
 * Destructor.
 */
GraphicsOrthographicProjection::~GraphicsOrthographicProjection()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsOrthographicProjection::GraphicsOrthographicProjection(const GraphicsOrthographicProjection& obj)
: CaretObject(obj)
{
    this->copyHelperGraphicsOrthographicProjection(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
GraphicsOrthographicProjection&
GraphicsOrthographicProjection::operator=(const GraphicsOrthographicProjection& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperGraphicsOrthographicProjection(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsOrthographicProjection::copyHelperGraphicsOrthographicProjection(const GraphicsOrthographicProjection& obj)
{
    m_left   = obj.m_left;
    m_right  = obj.m_right;
    m_bottom = obj.m_bottom;
    m_top    = obj.m_top;
    m_near   = obj.m_near;
    m_far    = obj.m_far;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
GraphicsOrthographicProjection::operator==(const GraphicsOrthographicProjection& obj) const
{
    if (this == &obj) {
        return true;    
    }

    /* perform equality testing HERE and return true if equal ! */
    if (   (m_left   == obj.m_left)
        && (m_right  == obj.m_right)
        && (m_bottom == obj.m_bottom)
        && (m_top    == obj.m_top)
        && (m_near   == obj.m_near)
        && (m_far    == obj.m_far)) {
        return true;
    }
    return false;    
}

/**
 * Apply the projection with OpenGL.  This method will save the
 * and restore the projection mode.
 */
void
GraphicsOrthographicProjection::applyWithOpenGL() const
{
    glPushAttrib(GL_TRANSFORM_BIT); /* Saves matrix mode */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_left, m_right, m_bottom, m_top, m_near, m_far);
    glPopAttrib();
}

/**
 * @return True if left is different from right
 * bottom is different from top,
 * and near is different from far
 * else false.
 */
bool
GraphicsOrthographicProjection::isValid() const
{
    if (isValid2D()) {
        if (m_near != m_far) {
            return true;
        }
    }
    return false;
}

/**
 * @return True if left is different from right
 * and bottom is different from top;
 * else false.
 */
bool
GraphicsOrthographicProjection::isValid2D() const
{
    if ((m_left != m_left)
        && (m_bottom != m_top)) {
        return true;
    }
    return false;
}

/**
 * @return Height (absolute difference between bottom and top)
 */
double
GraphicsOrthographicProjection::getHeight() const
{
    const double h(std::fabs(m_bottom - m_top));
    return h;
}

/**
 * @return Width (absolute difference between bottom and top)
 */
double
GraphicsOrthographicProjection::getWidth() const
{
    const double w(std::fabs(m_left - m_right));
    return w;
}

/**
 * @return Orthographic projection (left, right, bottom, top, near, far) in a double array
 */
std::array<double, 6>
GraphicsOrthographicProjection::getAsDoubleArray() const
{
    std::array<double, 6> array {
        m_left,
        m_right,
        m_bottom,
        m_top,
        m_near,
        m_far
    };
    return array;
}

/**
 * @return Orthographic projection (left, right, bottom, top, near, far) in a float array
 */
std::array<float, 6>
GraphicsOrthographicProjection::getAsFloatArray() const
{
    std::array<float, 6> array {
        static_cast<float>(m_left),
        static_cast<float>(m_right),
        static_cast<float>(m_bottom),
        static_cast<float>(m_top),
        static_cast<float>(m_near),
        static_cast<float>(m_far)
    };
    return array;
}

/**
 * Reset to an invalid projection (all values zero)
 */
void
GraphicsOrthographicProjection::resetToInvalid()
{
    m_left = 0.0;
    m_right = 0.0;
    m_bottom = 0.0;
    m_top = 0.0;
    m_near = 0.0;
    m_far = 0.0;
}

/**
 * Set the orthographic projection
 * @param left
 *    Left side of projection
 * @param right
 *    Right side of projection
 * @param bottom
 *    Bottom side of projection
 * @param top
 *    Top side of projection
 * @param nearValue
 *    Near side of projection
 * @param farValue
 *    Far side of projection
 */
void
GraphicsOrthographicProjection::set(const double left,
                                    const double right,
                                    const double bottom,
                                    const double top,
                                    const double nearValue,
                                    const double farValue)
{
    m_left   = left;
    m_right  = right;
    m_bottom = bottom;
    m_top    = top;
    m_near   = nearValue;
    m_far    = farValue;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsOrthographicProjection::toString() const
{
    const int32_t precision(3);
    QString s("Orthographic Projection: "
                "left=" + AString::number(m_left, 'f', precision)
              + ", right=" + AString::number(m_right, 'f', precision)
              + ", bottom=" + AString::number(m_bottom, 'f', precision)
              + ", top=" + AString::number(m_top, 'f', precision)
              + ", near=" + AString::number(m_near, 'f', precision)
              + ", far=" + AString::number(m_far, 'f', precision));
    return s;
}

