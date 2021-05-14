
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
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsRegionSelectionBox::toString() const
{
    return "GraphicsRegionSelectionBox";
}

/**
 * Initalize the corners to the same X, Y coordinates.
 * Also sets the box status to valid.
 * @param x
 *    The x-coordinate
 * @param y
 *    The y-coordinate
 */
void
GraphicsRegionSelectionBox::initialize(const float x,
                                       const float y)
{
    m_x1 = x;
    m_y1 = y;
    m_x2 = x;
    m_y2 = y;
    m_status = Status::VALID;
}

/**
 * Update the second corner same X, Y coordinates
 * @param x
 *    The x-coordinate
 * @param y
 *    The y-coordinate
 */
void
GraphicsRegionSelectionBox::update(const float x,
                                   const float y)
{
    m_x2 = x;
    m_y2 = y;
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
 * Get the box's bounds
 * @param minX
 *    The minimum x-coordinate
 * @param minY
 *    The minimum y-coordinate
 * @param maxX
 *    The maximum x-coordinate
 * @param maxY
 *    The maximum y-coordinate
 * @return True if the status is valid AND the min and max are NOT cooincident (same values)
 */
bool
GraphicsRegionSelectionBox::getBounds(float& minX,
                                      float& minY,
                                      float& maxX,
                                      float& maxY) const
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
    
    bool validFlag(false);
    
    switch (m_status) {
        case Status::INVALID:
            break;
        case Status::VALID:
            if ((m_x1 != m_x2)
                && (m_y1 != m_y2)) {
                validFlag = true;
            }
            break;
    }
    
    return validFlag;
}

