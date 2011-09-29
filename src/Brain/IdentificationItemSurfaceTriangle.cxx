
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
    this->triangleNumber = -1;
    this->surface = NULL;
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
IdentificationItemSurfaceTriangle::getTriangleNumber()
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


