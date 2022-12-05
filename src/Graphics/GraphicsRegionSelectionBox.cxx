
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

#define __GRAPHICS_REGION_SELECTION_BOX_DECLARE__
#include "GraphicsRegionSelectionBox.h"
#undef __GRAPHICS_REGION_SELECTION_BOX_DECLARE__

#include <cmath>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsRegionSelectionBox 
 * \brief Box used for selection in graphics region
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsRegionSelectionBox::GraphicsRegionSelectionBox()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
GraphicsRegionSelectionBox::~GraphicsRegionSelectionBox()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsRegionSelectionBox::GraphicsRegionSelectionBox(const GraphicsRegionSelectionBox& obj)
: CaretObject(obj)
{
    this->copyHelperGraphicsRegionSelectionBox(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
GraphicsRegionSelectionBox&
GraphicsRegionSelectionBox::operator=(const GraphicsRegionSelectionBox& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperGraphicsRegionSelectionBox(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsRegionSelectionBox::copyHelperGraphicsRegionSelectionBox(const GraphicsRegionSelectionBox& obj)
{
    m_status = obj.m_status;
    m_x1     = obj.m_x1;
    m_x2     = obj.m_x2;
    m_y1     = obj.m_y1;
    m_y2     = obj.m_y2;
    m_z1     = obj.m_z1;
    m_z2     = obj.m_z2;
    m_vpX1   = obj.m_vpX1;
    m_vpX2   = obj.m_vpX2;
    m_vpY1   = obj.m_vpY1;
    m_vpY2   = obj.m_vpY2;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsRegionSelectionBox::toString() const
{
    AString statusName;
    switch (m_status) {
        case Status::INVALID:
            statusName = "Invalid";
            break;
        case Status::VALID:
            statusName = "Valid";
            break;
    }
    
    QString s1(QString("x1=%1, x2=%2, y1=%3, y2=%4, z1=%5, z2=%6\n").arg(m_x1).arg(m_x2).arg(m_y1).arg(m_y2).arg(m_z1).arg(m_z2));
    QString s2(QString("vpX1=%1, vpX2=%2, vpY1=%3, vpY2=%4, valid=%5").arg(m_vpX1).arg(m_vpX2).arg(m_vpY1).arg(m_vpY2).arg(statusName));
    return (s1 + s2);
}

/**
 * Initalize the corners to the same X, Y, Z coordinates.
 * Also sets the box status to valid.
 * @param x
 *    The x-coordinate
 * @param y
 *    The y-coordinate
 * @param z
 *    The z-coordinate
 * @param vpX
 *    The viewport X-coordinate
 * @param vpY
 *    The viewport X-coordinate
 */
void
GraphicsRegionSelectionBox::initialize(const float x,
                                       const float y,
                                       const float z,
                                       const float vpX,
                                       const float vpY)
{
    m_x1 = x;
    m_y1 = y;
    m_z1 = z;
    m_x2 = x;
    m_y2 = y;
    m_z2 = z;
    m_vpX1 = vpX;
    m_vpY1 = vpY;
    m_vpX2 = vpX;
    m_vpY2 = vpY;
    m_status = Status::VALID;
}

/**
 * Update the second corner same X, Y, Z coordinates
 * @param x
 *    The x-coordinate
 * @param y
 *    The y-coordinate
 * @param z
 *    The z-coordinate
 * @param vpX
 *    The viewport X-coordinate
 * @param vpY
 *    The viewport X-coordinate
 */
void
GraphicsRegionSelectionBox::update(const float x,
                                   const float y,
                                   const float z,
                                   const float vpX,
                                   const float vpY)
{
    m_x2 = x;
    m_y2 = y;
    m_z2 = z;
    m_vpX2 = vpX;
    m_vpY2 = vpY;
}

/**
 * @return The status
 */
GraphicsRegionSelectionBox::Status
GraphicsRegionSelectionBox::getStatus() const
{
    return m_status;
}

/**
 * Set the status
 * @param status
 *    New status
 */
void
GraphicsRegionSelectionBox::setStatus(const Status status)
{
    m_status = status;
}

/**
 * @return bounds of region in a bounding box
 */
BoundingBox
GraphicsRegionSelectionBox::getBounds() const
{
    BoundingBox box;
    
    float minX, minY, minZ, maxX, maxY, maxZ;
    if (getBounds(minX, minY, minZ, maxX, maxY, maxZ)) {
        box.set(minX, maxX, minY, maxY, minZ, maxZ);
    }
    
    return box;
}

/**
 * Get the box's bounds
 * @param minXYZ
 *    The minimum x/y/z-coordinate
 * @param maxXYZ
 *    The maximum x/y/z-coordinate
 * @return True if the status is valid AND the min and max are NOT coincident (same values)
 */
bool
GraphicsRegionSelectionBox::getBounds(Vector3D& minXYZ,
                                      Vector3D& maxXYZ) const
{
    float minX, minY, minZ, maxX, maxY, maxZ;
    if (getBounds(minX, minY, minZ, maxX, maxY, maxZ)) {
        minXYZ.set(minX, minY, minZ);
        maxXYZ.set(maxX, maxY, maxZ);
        return true;
    }
    return false;
}

/**
 * Get the box's bounds
 * @param minX
 *    The minimum x-coordinate
 * @param minY
 *    The minimum y-coordinate
 * @param minZ
 *    The minimum z-coordinate
 * @param maxX
 *    The maximum x-coordinate
 * @param maxY
 *    The maximum y-coordinate
 * @param maxZ
 *    The maximum z-coordinate
 * @return True if the status is valid AND the min and max are NOT coincident (same values)
 */
bool
GraphicsRegionSelectionBox::getBounds(float& minX,
                                      float& minY,
                                      float& minZ,
                                      float& maxX,
                                      float& maxY,
                                      float& maxZ) const
{
    if (m_x1 > m_x2) {
        minX = m_x2;
        maxX = m_x1;
    }
    else {
        minX = m_x1;
        maxX = m_x2;
    }
    if (m_y1 > m_y2) {
        minY = m_y2;
        maxY = m_y1;
    }
    else {
        minY = m_y1;
        maxY = m_y2;
    }
    if (m_z1 > m_z2) {
        minZ = m_z2;
        maxZ = m_z1;
    }
    else {
        minZ = m_z1;
        maxZ = m_z2;
    }

    return isValidCoords();
}

/**
 * Get the box's bounds
 * @param vpMinX
 *    The viewport minimum x-coordinate
 * @param vpMinY
 *    The viewport minimum y-coordinate
 * @param vpMinZ
 *    The viewport minimum z-coordinate
 * @param vpMaxX
 *    The viewport maximum x-coordinate
 * @return True if the status is valid AND the min and max are NOT coincident (same values)
 */
bool
GraphicsRegionSelectionBox::getViewportBounds(float& vpMinX,
                                              float& vpMinY,
                                              float& vpMaxX,
                                              float& vpMaxY) const
{
    if (m_vpX1 > m_vpX2) {
        vpMinX = m_vpX2;
        vpMaxX = m_vpX1;
    }
    else {
        vpMinX = m_vpX1;
        vpMaxX = m_vpX2;
    }
    if (m_vpY1 > m_vpY2) {
        vpMinY = m_vpY2;
        vpMaxY = m_vpY1;
    }
    else {
        vpMinY = m_vpY1;
        vpMaxY = m_vpY2;
    }
    return isValidViewportCoords();
}

/**
 * @return The region of the viewport that was selected.
 */
GraphicsViewport
GraphicsRegionSelectionBox::getViewport() const
{
    float vpMinX(0.0), vpMaxX(0.0), vpMinY(0.0), vpMaxY(0.0);
    if (getViewportBounds(vpMinX, vpMinY, vpMaxX, vpMaxY)) {
        GraphicsViewport vp(static_cast<int32_t>(vpMinX),
                            static_cast<int32_t>(vpMinY),
                            static_cast<int32_t>(vpMaxX - vpMinX),
                            static_cast<int32_t>(vpMaxY - vpMinY));
        return vp;

    }
    return GraphicsViewport(0, 0, 0, 0);
}

/**
 * Get the box's center.
 * @param centerX
 *    The maximum x-coordinate
 * @param centerY
 *    The maximum y-coordinate
 * @param centerZ
 *    The maximum z-coordinate
 * @return True if the status is valid AND the min and max are NOT coincident (same values)
 */
bool
GraphicsRegionSelectionBox::getCenter(float& centerX,
                                      float& centerY,
                                      float& centerZ) const
{
    centerX = (m_x1 + m_x2) / 2.0f;
    centerY = (m_y1 + m_y2) / 2.0f;
    centerZ = (m_z1 + m_z2) / 2.0f;
    return isValidCoords();
}

/**
 * @return X-Size of the selection box
 */
float
GraphicsRegionSelectionBox::getSizeX() const
{
    return std::fabs(m_x1 - m_x2);
}

/**
 * @return Y-Size of the selection box
 */
float
GraphicsRegionSelectionBox::getSizeY() const
{
    return std::fabs(m_y1 - m_y2);
}

/**
 * @return Z-Size of the selection box
 */
float
GraphicsRegionSelectionBox::getSizeZ() const
{
    return std::fabs(m_z1 - m_z2);
}

/*
 * @return True if the status is valid AND at least two of the X, Y, Z,
 * min and max are NOT coincident (same values)
 */
bool
GraphicsRegionSelectionBox::isValidCoords() const
{
    bool validFlag(false);
    
    switch (m_status) {
        case Status::INVALID:
            break;
        case Status::VALID:
        {
            int32_t validCount(0);
            if (m_x1 != m_x2) ++validCount;
            if (m_y1 != m_y2) ++validCount;
            if (m_z1 != m_z2) ++validCount;
            validFlag = (validCount >= 2);
        }
            break;
    }
    
    return validFlag;
}

/*
 * @return True if the status is valid AND the viewport coordinates
 * for a box with a non-zero area
 */
bool
GraphicsRegionSelectionBox::isValidViewportCoords() const
{
    bool validFlag(false);
    
    switch (m_status) {
        case Status::INVALID:
            break;
        case Status::VALID:
        {
            int32_t validCount(0);
            if (m_vpX1 != m_vpX2) ++validCount;
            if (m_vpY1 != m_vpY2) ++validCount;
            validFlag = (validCount >= 2);
        }
            break;
    }
    
    return validFlag;
}


