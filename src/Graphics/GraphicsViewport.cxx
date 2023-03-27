
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __GRAPHICS_VIEWPORT_DECLARE__
#include "GraphicsViewport.h"
#undef __GRAPHICS_VIEWPORT_DECLARE__

#include "CaretAssert.h"
#include "CaretOpenGLInclude.h"

using namespace caret;
    
/**
 * \class caret::GraphicsViewport 
 * \brief A helper for graphics viewport
 * \ingroup Graphics
 */

/**
 * @return New instance with the current graphics viewport.
 * OpenGL MUST BE "current".
 */
GraphicsViewport
GraphicsViewport::newInstanceCurrentViewport()
{
    GraphicsViewport vp;
    glGetIntegerv(GL_VIEWPORT,
                  vp.m_viewport.data());
    return vp;
}


/**
 * Constructs invalid viewport
 */
GraphicsViewport::GraphicsViewport()
{
    m_viewport.fill(0);
}


/**
 * Constructor.
 * @param viewport
 *    Value for viewport.
 */
GraphicsViewport::GraphicsViewport(const std::array<int32_t, 4>& viewport)
: CaretObject()
{
    for (int32_t i = 0; i < 4; i++) {
        m_viewport[i] = viewport[i];
    }
}

/**
 * Constructor.
 * @param viewport
 *    Value for viewport.
 */
GraphicsViewport::GraphicsViewport(const int32_t viewport[4])
: CaretObject()
{
    for (int32_t i = 0; i < 4; i++) {
        m_viewport[i] = viewport[i];
    }
}

/**
 * Constructor.
 * @param x
 *    X-coordinate of viewport.
 * @param y
 *    Y-coordinate of viewport.
 * @param width
 *    Width of viewport.
 * @param x
 *    Height of viewport.
 */
GraphicsViewport::GraphicsViewport(const int32_t x,
                                   const int32_t y,
                                   const int32_t width,
                                   const int32_t height)
: CaretObject()
{
    m_viewport[0] = x;
    m_viewport[1] = y;
    m_viewport[2] = width;
    m_viewport[3] = height;

}

/**
 * Destructor.
 */
GraphicsViewport::~GraphicsViewport()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsViewport::GraphicsViewport(const GraphicsViewport& obj)
: CaretObject(obj)
{
    this->copyHelperGraphicsViewport(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
GraphicsViewport&
GraphicsViewport::operator=(const GraphicsViewport& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperGraphicsViewport(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsViewport::copyHelperGraphicsViewport(const GraphicsViewport& obj)
{
    m_viewport = obj.m_viewport;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
GraphicsViewport::operator==(const GraphicsViewport& obj) const
{
    if (this == &obj) {
        return true;    
    }

    for (int32_t i = 0; i < 4; i++) {
        if (m_viewport[i] != obj.m_viewport[i]) {
            return false;
        }
    }

    return true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsViewport::toString() const
{
    return ("x=" + AString::number(m_viewport[0])
            + ", y=" + AString::number(m_viewport[1])
            + ", width=" + AString::number(m_viewport[2])
            + ", height=" + AString::number(m_viewport[3]));
}

/**
 * @return Viewport as integer array
 */
std::array<int32_t, 4>
GraphicsViewport::getViewport() const
{
    return m_viewport;
}

/**
 * @return Viewport as float array
 */
std::array<float, 4>
GraphicsViewport::getViewportF() const
{
    std::array<float, 4> floatViewport;
    for (int32_t i = 0; i < 4; i++) {
        floatViewport[i] = static_cast<float>(m_viewport[i]);
    };
    
    return floatViewport;
}

/**
 * @return True if the viewport is valid (width and height greater than zero)
 */
bool
GraphicsViewport::isValid() const
{
    if ((m_viewport[2] > 0)
        && (m_viewport[3] > 0)) {
        return true;
    }
    return false;
}

/**
 * @return True if the viewport contains the given window X & Y
 * @param windowX
 *    Window X coordinate
 * @param windowY
 *    Window Y coordinate
 */
bool
GraphicsViewport::containsWindowXY(const int32_t windowX,
                                   const int32_t windowY) const
{
    if (isValid()) {
        if ((windowX >= m_viewport[0])
            && (windowX < (m_viewport[0] + m_viewport[2]))
            && (windowY >= m_viewport[1])
            && (windowY < (m_viewport[1] + m_viewport[3]))) {
            return true;
        }
    }
    return false;
}

/**
 * @return True if the viewport contains the given window X & Y
 * @param windowXY
 *    Window XY coordinate (Z ignored)
 */
bool
GraphicsViewport::containsWindowXY(const Vector3D& windowXY) const
{
    return containsWindowXY(windowXY[0],
                            windowXY[1]);
}


