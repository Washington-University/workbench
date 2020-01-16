
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __TILE_TABS_BROWSER_TAB_GEOMETRY_DECLARE__
#include "TileTabsBrowserTabGeometry.h"
#undef __TILE_TABS_BROWSER_TAB_GEOMETRY_DECLARE__

#include <algorithm>

#include <QTextStream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"

using namespace caret;

/**
 * \class caret::TileTabsBrowserTabGeometry
 * \brief Information about a tab's position in the window using percentage coordinates
 * \ingroup Common
 *
 * 
 *
 */

/**
 * Constructor.
 */
TileTabsBrowserTabGeometry::TileTabsBrowserTabGeometry(const int32_t tabIndex)
: CaretObject(),
m_tabIndex(tabIndex)
{
    float xy(10.0f + (5.0 * tabIndex));
    const float widthHeight(20.0);
    xy  = MathFunctions::limitRange(xy,  5.0f, 95.0f - widthHeight);
    m_minX = xy;
    m_maxX = xy + widthHeight;
    m_minY = xy;
    m_maxY = xy + widthHeight;
    m_stackingOrder = tabIndex;
}

/**
 * Destructor.
 */
TileTabsBrowserTabGeometry::~TileTabsBrowserTabGeometry()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TileTabsBrowserTabGeometry::TileTabsBrowserTabGeometry(const TileTabsBrowserTabGeometry& obj)
: CaretObject(obj)
{
    this->copyHelperTileTabsBrowserTabGeometry(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TileTabsBrowserTabGeometry&
TileTabsBrowserTabGeometry::operator=(const TileTabsBrowserTabGeometry& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTileTabsBrowserTabGeometry(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsBrowserTabGeometry::copyHelperTileTabsBrowserTabGeometry(const TileTabsBrowserTabGeometry& obj)
{
    m_displayFlag = obj.m_displayFlag;
    m_tabIndex    = obj.m_tabIndex;
    m_minX        = obj.m_minX;
    m_maxX        = obj.m_maxX;
    m_minY        = obj.m_minY;
    m_maxY        = obj.m_maxY;
    m_stackingOrder  = obj.m_stackingOrder;
    m_backgroundType = obj.m_backgroundType;
}

/**
 * Copy the given geometry to 'this'
 *
 * @param geometry
 *     Geometry that is copied
 */
void
TileTabsBrowserTabGeometry::copyGeometry(const TileTabsBrowserTabGeometry& geometry)
{
    copyHelperTileTabsBrowserTabGeometry(geometry);
}

/**
 * @return Index of the tab
 */
int32_t
TileTabsBrowserTabGeometry::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
TileTabsBrowserTabGeometry::toString() const
{
    AString str;
    QTextStream ts(&str);
    
    ts << "TileTabsBrowserTabGeometry: "
    << "m_displayFlag=" << AString::fromBool(m_displayFlag)
    << " m_tabIndex=" << m_tabIndex
    << " m_minX=" << m_minX
    << " m_maxX=" << m_maxX
    << " m_minY=" << m_minY
    << " m_maxY=" << m_maxY
    << " m_stackingOrder=" << m_stackingOrder
    << " m_backgroundType=" << TileTabsLayoutBackgroundTypeEnum::toName(m_backgroundType);

    return str;
}

/**
 * @return The display status
 */
bool
TileTabsBrowserTabGeometry::isDisplayed() const
{
    return m_displayFlag;
}

/**
 * Set the display status
 *
 * @param status
 *     New display status
 */
void
TileTabsBrowserTabGeometry::setDisplayed(const bool status)
{
    m_displayFlag = status;
}

/**
 * Set the bounds
 *
 * @param minX
 *     The minimum X value
 * @param maxX
 *     The maximum X value
 * @param minY
 *     The minimum Y value
 * @param maxY
 *     The maximum Y value
 */
void
TileTabsBrowserTabGeometry::setBounds(const float minX,
                                      const float maxX,
                                      const float minY,
                                      const float maxY)
{
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
}

/**
 * Get the bounds
 *
 * @param minX
 *     The minimum X value
 * @param maxX
 *     The maximum X value
 * @param minY
 *     The minimum Y value
 * @param maxY
 *     The maximum Y value
 */
void
TileTabsBrowserTabGeometry::getBounds(float& minX,
                                      float& maxX,
                                      float& minY,
                                      float& maxY) const
{
    minX = m_minX;
    maxX = m_maxX;
    minY = m_minY;
    maxY = m_maxY;
}

/**
 * @return center x-coordinate as percentage 0% to 100%
 */
float
TileTabsBrowserTabGeometry::getCenterX() const
{
    return ((m_minX + m_maxX) / 2.0);
}

/**
 * Set center x-coordinate as percentage 0% to 100%
 *
 * @param x
 *    New value for center x-coordinate as percentage 0% to 100%
 */
void
TileTabsBrowserTabGeometry::setCenterX(const float x)
{
    const float dx = x - getCenterX();
    m_minX += dx;
    m_maxX += dx;
}

/**
 * @return center y-coordinate as percentage 0% to 100%
 */
float
TileTabsBrowserTabGeometry::getCenterY() const
{
    return ((m_minY + m_maxY) / 2.0);
}

/**
 * Set center y-coordinate as percentage 0% to 100%
 *
 * @param y
 *    New value for center y-coordinate as percentage 0% to 100%
 */
void
TileTabsBrowserTabGeometry::setCenterY(const float y)
{
    const float dy = y - getCenterY();
    m_minY += dy;
    m_maxY += dy;
}

/**
 * @return width as percentage 0% to 100%
 */
float
TileTabsBrowserTabGeometry::getWidth() const
{
    return (m_maxX - m_minX);
}

/**
 * Set width as percentage 0% to 100%
 *
 * @param width
 *    New value for width as percentage 0% to 100%
 */
void
TileTabsBrowserTabGeometry::setWidth(const float width)
{
    const float cx = getCenterX();
    const float half = width / 2.0;
    m_minX = cx - half;
    m_maxX = cx + half;
}

/**
 * @return height as percentage 0% to 100%
 */
float
TileTabsBrowserTabGeometry::getHeight() const
{
    return (m_maxY - m_minY);
}

/**
 * Set height as percentage 0% to 100%
 *
 * @param height
 *    New value for height as percentage 0% to 100%
 */
void
TileTabsBrowserTabGeometry::setHeight(const float height)
{
    const float cy = getCenterY();
    const float half = height / 2.0;
    m_minY = cy - half;
    m_maxY = cy + half;
}

/**
 * @return minimum x percentage 0% to 100% at left
 */
float
TileTabsBrowserTabGeometry::getMinX() const
{
    return m_minX;
}

/**
 * Set minimum x percentage 0% to 100% at left
 *
 * @param minX
 *    New value for minimum x percentage 0% to 100% at left
 */
void
TileTabsBrowserTabGeometry::setMinX(const float minX)
{
    m_minX = minX;
    m_maxX = std::max(m_maxX,
                      m_minX + 1.0f);
}

/**
 * @return maximum x percentage 0% to 100% at rigth
 */
float
TileTabsBrowserTabGeometry::getMaxX() const
{
    return m_maxX;
}

/**
 * Set maximum x percentage 0% to 100% at right
 *
 * @param maxX
 *    New value for maximum x percentage 0% to 100% at right
 */
void
TileTabsBrowserTabGeometry::setMaxX(const float maxX)
{
    m_maxX = maxX;
    m_minX = std::min(m_minX,
                      m_maxX - 1.0f);
}

/**
 * @return minimum y percentage 0% to 100% at bottom
 */
float
TileTabsBrowserTabGeometry::getMinY() const
{
    return m_minY;
}

/**
 * Set minimum y percentage 0% to 100% at bottom
 *
 * @param minY
 *    New value for minimum y percentage 0% to 100% at bottom
 */
void
TileTabsBrowserTabGeometry::setMinY(const float minY)
{
    m_minY = minY;
    m_maxY = std::max(m_maxY,
                      m_minY + 1.0f);
}

/**
 * @return maximum y percentage 0% to 100% at top
 */
float
TileTabsBrowserTabGeometry::getMaxY() const
{
    return m_maxY;
}

/**
 * Set maximum y percentage 0% to 100% at top
 *
 * @param maxY
 *    New value for maximum y percentage 0% to 100% at top
 */
void
TileTabsBrowserTabGeometry::setMaxY(const float maxY)
{
    m_maxY = maxY;
    m_minY = std::min(m_minY,
                      m_maxY - 1.0f);
}

/**
 * Get the window viewport
 *
 * @param windowWidth
 *     Width of window in pixels
 * @param windowHeight
 *     Height of window in pixels
 * @param viewportOut
 *     Output containing lower-left X, Y, Width, and Height in window coordinates
 */
void
TileTabsBrowserTabGeometry::getWindowViewport(const int32_t windowWidth,
                                              const int32_t windowHeight,
                                              int32_t viewportOut[4]) const
{
    viewportOut[0] = static_cast<int32_t>((getMinX()   / 100.0) * windowWidth);
    viewportOut[1] = static_cast<int32_t>((getMinY()   / 100.0) * windowHeight);
    viewportOut[2] = static_cast<int32_t>((getWidth()  / 100.0) * windowWidth);
    viewportOut[3] = static_cast<int32_t>((getHeight() / 100.0) * windowHeight);
}

/**
 * @return Stacking order (depth in screen) of tab, greater value is 'in front'
 */
int32_t
TileTabsBrowserTabGeometry::getStackingOrder() const
{
    return m_stackingOrder;
}

/**
 * Set Stacking order (depth in screen) of tab, greater value is 'in front'
 *
 * @param stackingOrder
 *    New value for Stacking order (depth in screen) of tab, greater value is 'in front'
 */
void
TileTabsBrowserTabGeometry::setStackingOrder(const int32_t stackingOrder)
{
    m_stackingOrder = stackingOrder;
}

/**
 * @return Type of background (opaque / transparent) for tab
 */
TileTabsLayoutBackgroundTypeEnum::Enum
TileTabsBrowserTabGeometry::getBackgroundType() const
{
    return m_backgroundType;
}

/**
 * Set Type of background (opaque / transparent) for tab
 *
 * @param backgroundType
 *    New value for Type of background (opaque / transparent) for tab
 */
void
TileTabsBrowserTabGeometry::setBackgroundType(const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType)
{
    m_backgroundType = backgroundType;
}

/**
 * @return true if this and the given geometry intersect.
 *         NOTE: if 'other' is 'this' true is returned (overlaps self) but this
 *         could change so it is best to avoid testing overlap of self.
 *
 * @param other
 *     Other geometry for intersection test
 */
bool
TileTabsBrowserTabGeometry::intersectionTest(const TileTabsBrowserTabGeometry* other) const
{
    CaretAssert(other);
    
    /*
     * Does self overlap
     */
    if (this == other) {
        return true;
    }
    
    /*
     * Note: Since the geometry is aligned with the X- and Y-axes,
     * we only need to test for one to be above or the the right of the other
     *
     * https://www.geeksforgeeks.org/find-two-rectangles-overlap/
     * https://leetcode.com/articles/rectangle-overlap/
     */
    /* 'this' is on right side of 'other' */
    if (m_minX >= other->m_maxX) {
        return false;
    }
    
    /* 'other' is on right side of 'this' */
    if (other->m_minX >= m_maxX) {
        return false;
    }
    
    /* 'this' is above 'other */
    if (m_minY >= other->m_maxY) {
        return false;
    }
    
    /* 'other' is above 'this' */
    if (other->m_minY >= m_maxY) {
        return false;
    }
    
    return true;
}

