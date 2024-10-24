
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
    m_barycentricAreas[0] = 0.0;
    m_barycentricAreas[1] = 0.0;
    m_barycentricAreas[2] = 0.0;
    m_barycentricVertices[0] = -1;
    m_barycentricVertices[1] = -1;
    m_barycentricVertices[2] = -1;
    m_barycentricProjectionValidFlag = false;
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
    m_barycentricAreas[0] = 0.0;
    m_barycentricAreas[1] = 0.0;
    m_barycentricAreas[2] = 0.0;
    m_barycentricVertices[0] = -1;
    m_barycentricVertices[1] = -1;
    m_barycentricVertices[2] = -1;
    m_barycentricProjectionValidFlag = false;
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

/**
 * @return True if the barycentric projection is valid
 */
bool
SelectionItemSurfaceTriangle::isBarycentricProjectionValid() const
{
    return m_barycentricProjectionValidFlag;
}

/**
 * Set the barycentric projection valid
 * @param validFlag
 *    New status
 */
void
SelectionItemSurfaceTriangle::setBarycentricProjectionValid(const bool validFlag)
{
    m_barycentricProjectionValidFlag = validFlag;
}

/**
 * Get the barycentric areas
 * @param areasOut
 *    Output with areas
 */
void
SelectionItemSurfaceTriangle::getBarycentricAreas(float areasOut[3]) const
{
    areasOut[0] = m_barycentricAreas[0];
    areasOut[1] = m_barycentricAreas[1];
    areasOut[2] = m_barycentricAreas[2];
}

/**
 * Get the barycentric vertices
 * @param verticesOut
 *    Vertices out
 */
void
SelectionItemSurfaceTriangle::getBarycentricVertices(int32_t verticesOut[3]) const
{
    verticesOut[0] = m_barycentricVertices[0];
    verticesOut[1] = m_barycentricVertices[1];
    verticesOut[2] = m_barycentricVertices[2];
}

/**
 * Set the barycentric areas
 * @param areas
 *    The areas
 */
void
SelectionItemSurfaceTriangle::setBarycentricAreas(const float areas[3])
{
    m_barycentricAreas[0] = areas[0];
    m_barycentricAreas[1] = areas[1];
    m_barycentricAreas[2] = areas[2];
}

/**
 * Set the barycentric vertices
 * @param vertices
 *    The vertices
 */
void
SelectionItemSurfaceTriangle::setBarycentricVertices(const int32_t vertices[3])
{
    m_barycentricVertices[0] = vertices[0];
    m_barycentricVertices[1] = vertices[1];
    m_barycentricVertices[2] = vertices[2];
}



