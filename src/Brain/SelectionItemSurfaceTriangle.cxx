
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

#define __SELECTION_ITEM_SURFACE_TRIANGLE_DECLARE__
#include "SelectionItemSurfaceTriangle.h"
#undef __SELECTION_ITEM_SURFACE_TRIANGLE_DECLARE__

#include <limits>
#include "Surface.h"

using namespace caret;

/**
 * \class SelectionItemSurfaceTriangle
 * \brief Selected node.
 *
 * Information about the selected node.
 */


/**
 * Constructor.
 */
SelectionItemSurfaceTriangle::SelectionItemSurfaceTriangle()
: SelectionItem(SelectionItemDataTypeEnum::SURFACE_TRIANGLE)
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
SelectionItemSurfaceTriangle::~SelectionItemSurfaceTriangle()
{
    
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemSurfaceTriangle::reset()
{
    SelectionItem::reset();
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
 * return Is this selected item valid?
 */
bool 
SelectionItemSurfaceTriangle::isValid() const
{
    return (this->triangleNumber >= 0);
}

/**
 * return Surface containing selected node.
 */
Surface* 
SelectionItemSurfaceTriangle::getSurface()
{
    return this->surface;
}

/**
 * Set the surface containing the selected node.
 * @param surface
 *    New value for surface.
 *
 */
void 
SelectionItemSurfaceTriangle::setSurface(Surface* surface)
{
    this->surface = surface;
}

/**
 * return Number of selected triangle.
 */
int32_t 
SelectionItemSurfaceTriangle::getTriangleNumber() const
{
    return this->triangleNumber;
}

/**
 * Set triangle number that was selected.
 * @param triangleNumber
 *    New value for triangle.
 */
void 
SelectionItemSurfaceTriangle::setTriangleNumber(const int32_t triangleNumber)
{
    this->triangleNumber = triangleNumber;
}

/**
 * @return Node nearest the mouse click in screen X&Y coordinates.
 * Will return negative if invalid.
 */
int32_t 
SelectionItemSurfaceTriangle::getNearestNodeNumber() const
{
    return this->nearestNodeNumber;
}

/**
 * Set the node nearest to the mouse click in screen X&Y coordinates.
 * @param nearestNodeNumber
 *    New value for the node.
 */
void 
SelectionItemSurfaceTriangle::setNearestNode(const int32_t nearestNodeNumber)
{
    this->nearestNodeNumber = nearestNodeNumber;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItemSurfaceTriangle::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
    text += "Triangle: " + AString::number(this->triangleNumber) + "\n";
    text += "Nearest Vertex: " + AString::number(this->nearestNodeNumber) + "\n";
    if (this->isValid() && (surface != NULL)) {
        if (this->nearestNodeNumber >= 0) {
            text += "Coordinate: " + AString::fromNumbers(surface->getCoordinate(this->nearestNodeNumber), 3, ", ") + "\n";
        }
    }
    return text;
}

/**
 * Get the screen XYZ of the nearest node.
 * @param nearestNodeScreenXYZ
 *    XYZ out.
 */
void 
SelectionItemSurfaceTriangle::getNearestNodeScreenXYZ(double nearestNodeScreenXYZ[3]) const
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
SelectionItemSurfaceTriangle::setNearestNodeScreenXYZ(const double nearestNodeScreenXYZ[3])
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
SelectionItemSurfaceTriangle::getNearestNodeModelXYZ(double nearestNodeModelXYZ[3]) const
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
SelectionItemSurfaceTriangle::setNearestNodeModelXYZ(const double nearestNodeModelXYZ[3])
{
    this->nearestNodeModelXYZ[0] = nearestNodeModelXYZ[0];
    this->nearestNodeModelXYZ[1] = nearestNodeModelXYZ[1];
    this->nearestNodeModelXYZ[2] = nearestNodeModelXYZ[2];
}



