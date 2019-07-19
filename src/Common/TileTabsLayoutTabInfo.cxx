
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

#define __TILE_TABS_TAB_INFO_DECLARE__
#include "TileTabsLayoutTabInfo.h"
#undef __TILE_TABS_TAB_INFO_DECLARE__

#include <QTextStream>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::TileTabsLayoutTabInfo
 * \brief Information about a tab's position in the window using percentage coordinates
 * \ingroup Common
 *
 * 
 *
 */

/**
 * Constructor.
 */
TileTabsLayoutTabInfo::TileTabsLayoutTabInfo(const int32_t tabIndex)
: CaretObject(),
m_tabIndex(tabIndex)
{
    
}

/**
 * Destructor.
 */
TileTabsLayoutTabInfo::~TileTabsLayoutTabInfo()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TileTabsLayoutTabInfo::TileTabsLayoutTabInfo(const TileTabsLayoutTabInfo& obj)
: CaretObject(obj)
{
    this->copyHelperTileTabsLayoutTabInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TileTabsLayoutTabInfo&
TileTabsLayoutTabInfo::operator=(const TileTabsLayoutTabInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTileTabsLayoutTabInfo(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsLayoutTabInfo::copyHelperTileTabsLayoutTabInfo(const TileTabsLayoutTabInfo& obj)
{
    m_tabIndex = obj.m_tabIndex;
    m_xCenter  = obj.m_xCenter;
    m_yCenter  = obj.m_yCenter;
    m_width    = obj.m_width;
    m_height   = obj.m_height;
    m_stackingOrder  = obj.m_stackingOrder;
    m_backgroundType = obj.m_backgroundType;
}

/**
 * @return Index of the tab
 */
int32_t
TileTabsLayoutTabInfo::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
TileTabsLayoutTabInfo::toString() const
{
    AString str;
    QTextStream ts(&str);
    
    ts << "TileTabsLayoutTabInfo: "
    << "m_tabIndex=" << m_tabIndex
    << " m_xCenter=" << m_xCenter
    << " m_yCenter=" << m_yCenter
    << " m_width=" << m_width
    << " m_height=" << m_height
    << " m_stackingOrder=" << m_stackingOrder
    << " m_backgroundType=" << TileTabsLayoutBackgroundTypeEnum::toName(m_backgroundType);

    return str;
}

/**
 * @return center x-coordinate as percentage 0% to 100%
 */
float
TileTabsLayoutTabInfo::getCenterX() const
{
    return m_xCenter;
}

/**
 * Set center x-coordinate as percentage 0% to 100%
 *
 * @param x
 *    New value for center x-coordinate as percentage 0% to 100%
 */
void
TileTabsLayoutTabInfo::setCenterX(const float x)
{
    m_xCenter = x;
}

/**
 * @return center y-coordinate as percentage 0% to 100%
 */
float
TileTabsLayoutTabInfo::getCenterY() const
{
    return m_yCenter;
}

/**
 * Set center y-coordinate as percentage 0% to 100%
 *
 * @param y
 *    New value for center y-coordinate as percentage 0% to 100%
 */
void
TileTabsLayoutTabInfo::setCenterY(const float y)
{
    m_yCenter = y;
}

/**
 * @return width as percentage 0% to 100%
 */
float
TileTabsLayoutTabInfo::getWidth() const
{
    return m_width;
}

/**
 * Set width as percentage 0% to 100%
 *
 * @param width
 *    New value for width as percentage 0% to 100%
 */
void
TileTabsLayoutTabInfo::setWidth(const float width)
{
    m_width = width;
}

/**
 * @return height as percentage 0% to 100%
 */
float
TileTabsLayoutTabInfo::getHeight() const
{
    return m_height;
}

/**
 * Set height as percentage 0% to 100%
 *
 * @param height
 *    New value for height as percentage 0% to 100%
 */
void
TileTabsLayoutTabInfo::setHeight(const float height)
{
    m_height = height;
}

/**
 * @return minimum x percentage 0% to 100% at left
 */
float
TileTabsLayoutTabInfo::getMinX() const
{
    return (m_xCenter - (m_width / 2.0));
}

/**
 * Set minimum x percentage 0% to 100% at left
 *
 * @param minX
 *    New value for minimum x percentage 0% to 100% at left
 */
void
TileTabsLayoutTabInfo::setMinX(const float minX)
{
    m_xCenter = minX + (m_width / 2.0);
}

/**
 * @return maximum x percentage 0% to 100% at rigth
 */
float
TileTabsLayoutTabInfo::getMaxX() const
{
    return (m_xCenter + (m_width / 2.0));
}

/**
 * Set maximum x percentage 0% to 100% at right
 *
 * @param maxX
 *    New value for maximum x percentage 0% to 100% at right
 */
void
TileTabsLayoutTabInfo::setMaxX(const float maxX)
{
    m_xCenter = maxX - (m_width / 2.0);
}

/**
 * @return minimum y percentage 0% to 100% at bottom
 */
float
TileTabsLayoutTabInfo::getMinY() const
{
    return (m_yCenter - (m_height / 2.0));
}

/**
 * Set minimum y percentage 0% to 100% at bottom
 *
 * @param minY
 *    New value for minimum y percentage 0% to 100% at bottom
 */
void
TileTabsLayoutTabInfo::setMinY(const float minY)
{
    m_yCenter = minY + (m_height / 2.0);
}

/**
 * @return maximum y percentage 0% to 100% at top
 */
float
TileTabsLayoutTabInfo::getMaxY() const
{
    return (m_xCenter + (m_height / 2.0));
}

/**
 * Set maximum y percentage 0% to 100% at top
 *
 * @param maxY
 *    New value for maximum y percentage 0% to 100% at top
 */
void
TileTabsLayoutTabInfo::setMaxY(const float maxY)
{
    m_xCenter = maxY - (m_height / 2.0);
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
TileTabsLayoutTabInfo::getWindowViewport(const int32_t windowWidth,
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
TileTabsLayoutTabInfo::getStackingOrder() const
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
TileTabsLayoutTabInfo::setStackingOrder(const int32_t stackingOrder)
{
    m_stackingOrder = stackingOrder;
}

/**
 * @return Type of background (opaque / transparent) for tab
 */
TileTabsLayoutBackgroundTypeEnum::Enum
TileTabsLayoutTabInfo::getBackgroundType() const
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
TileTabsLayoutTabInfo::setBackgroundType(const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType)
{
    m_backgroundType = backgroundType;
}
