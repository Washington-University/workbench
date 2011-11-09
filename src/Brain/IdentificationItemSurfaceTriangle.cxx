
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __IDENTIFICATION_ITEM_SURFACE_TRIANGLE_DECLARE__
#include "IdentificationItemSurfaceTriangle.h"
#undef __IDENTIFICATION_ITEM_SURFACE_TRIANGLE_DECLARE__

#include <limits>
#include "Surface.h"

using namespace caret;

/**
 * \class IdentificationItemSurfaceNode
 * \brief Identified node.
 *
 * Information about the identified node.
 */


/**
 * Constructor.
 */
IdentificationItemSurfaceTriangle::IdentificationItemSurfaceTriangle()
: IdentificationItem(IdentificationItemDataTypeEnum::SURFACE_TRIANGLE)
{
    this->surface = NULL;
    this->triangleNumber = -1;
    this->nearestNodeNumber = -1;
    this->nearestNodeScreenXYZ[0] = 0.0;
    this->nearestNodeScreenXYZ[1] = 0.0;
    this->nearestNodeScreenXYZ[2] = std::numeric_limits<double>::max();
    this->nearestNodeModelXYZ[0] = 0.0;
    this->nearestNodeModelXYZ[1] = 0.0;
    this->nearestNodeModelXYZ[2] = 0.0;
}

/**
 * Destructor.
 */
IdentificationItemSurfaceTriangle::~IdentificationItemSurfaceTriangle()
{
    
}

/**
 * Reset this selection item. 
 */
void 
IdentificationItemSurfaceTriangle::reset()
{
    IdentificationItem::reset();
    this->surface = NULL;
    this->triangleNumber = -1;
    this->nearestNodeNumber = -1;
    this->nearestNodeScreenXYZ[0] = 0.0;
    this->nearestNodeScreenXYZ[1] = 0.0;
    this->nearestNodeScreenXYZ[2] = std::numeric_limits<double>::max();
    this->nearestNodeModelXYZ[0] = 0.0;
    this->nearestNodeModelXYZ[1] = 0.0;
    this->nearestNodeModelXYZ[2] = 0.0;
}

/**
 * return Is this identified item valid?
 */
bool 
IdentificationItemSurfaceTriangle::isValid() const
{
    return (this->triangleNumber >= 0);
}

/**
 * return Surface containing identified node.
 */
Surface* 
IdentificationItemSurfaceTriangle::getSurface()
{
    return this->surface;
}

/**
 * Set the surface containing the identified node.
 * @param surface
 *    New value for surface.
 *
 */
void 
IdentificationItemSurfaceTriangle::setSurface(Surface* surface)
{
    this->surface = surface;
}

/**
 * return Number of identified triangle.
 */
int32_t 
IdentificationItemSurfaceTriangle::getTriangleNumber() const
{
    return this->triangleNumber;
}

/**
 * Set triangle number that was identified.
 * @param triangleNumber
 *    New value for triangle.
 */
void 
IdentificationItemSurfaceTriangle::setTriangleNumber(const int32_t triangleNumber)
{
    this->triangleNumber = triangleNumber;
}

/**
 * @return Node nearest the mouse click in screen X&Y coordinates.
 * Will return negative if invalid.
 */
int32_t 
IdentificationItemSurfaceTriangle::getNearestNodeNumber() const
{
    return this->nearestNodeNumber;
}

/**
 * Set the node nearest to the mouse click in screen X&Y coordinates.
 * @param nearestNodeNumber
 *    New value for the node.
 */
void 
IdentificationItemSurfaceTriangle::setNearestNode(const int32_t nearestNodeNumber)
{
    this->nearestNodeNumber = nearestNodeNumber;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationItemSurfaceTriangle::toString() const
{
    AString text = "IdentificationItemSurfaceTriangle\n";
    text += IdentificationItem::toString() + "\n";
    text += "Surface: " + surface->getFileNameNoPath() + "\n";
    text += "Triangle: " + AString::number(this->triangleNumber);
    text += "Nearest Node: " + AString::number(this->nearestNodeNumber);
    return text;
}

/**
 * Get the screen XYZ of the nearest node.
 * @param nearestNodeScreenXYZ
 *    XYZ out.
 */
void 
IdentificationItemSurfaceTriangle::getNearestNodeScreenXYZ(double nearestNodeScreenXYZ[3]) const
{
    nearestNodeScreenXYZ[0] = this->nearestNodeScreenXYZ[0];
    nearestNodeScreenXYZ[1] = this->nearestNodeScreenXYZ[1];
    nearestNodeScreenXYZ[2] = this->nearestNodeScreenXYZ[2];
}

/**
 * Set the screen XYZ of the nearest node.
 * @param nearestNodeScreenXYZ
 *    new XYZ.
 */
void 
IdentificationItemSurfaceTriangle::setNearestNodeScreenXYZ(const double nearestNodeScreenXYZ[3])
{
    this->nearestNodeScreenXYZ[0] = nearestNodeScreenXYZ[0];
    this->nearestNodeScreenXYZ[1] = nearestNodeScreenXYZ[1];
    this->nearestNodeScreenXYZ[2] = nearestNodeScreenXYZ[2];
}

/**
 * Get the model XYZ of the nearest node.
 * @param nearestNodeModelXYZ
 *    XYZ out.
 */
void 
IdentificationItemSurfaceTriangle::getNearestNodeModelXYZ(double nearestNodeModelXYZ[3]) const
{
    nearestNodeModelXYZ[0] = this->nearestNodeModelXYZ[0];
    nearestNodeModelXYZ[1] = this->nearestNodeModelXYZ[1];
    nearestNodeModelXYZ[2] = this->nearestNodeModelXYZ[2];
}

/**
 * Set the model XYZ of the nearest node.
 * @param nearestNodeModelXYZ
 *    new XYZ.
 */
void 
IdentificationItemSurfaceTriangle::setNearestNodeModelXYZ(const double nearestNodeModelXYZ[3])
{
    this->nearestNodeModelXYZ[0] = nearestNodeModelXYZ[0];
    this->nearestNodeModelXYZ[1] = nearestNodeModelXYZ[1];
    this->nearestNodeModelXYZ[2] = nearestNodeModelXYZ[2];
}



