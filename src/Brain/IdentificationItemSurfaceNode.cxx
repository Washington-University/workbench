
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

#define __IDENTIFICATION_ITEM_SURFACE_NODE_DECLARE__
#include "IdentificationItemSurfaceNode.h"
#undef __IDENTIFICATION_ITEM_SURFACE_NODE_DECLARE__

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
IdentificationItemSurfaceNode::IdentificationItemSurfaceNode()
: IdentificationItem(IdentificationItemDataTypeEnum::SURFACE_NODE)
{
    m_surface = NULL;
    m_contralateralFlag = false;
    m_nodeNumber = -1;
}

/**
 * Destructor.
 */
IdentificationItemSurfaceNode::~IdentificationItemSurfaceNode()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentificationItemSurfaceNode::IdentificationItemSurfaceNode(const IdentificationItemSurfaceNode& obj)
: IdentificationItem(obj)
{
    copyHelperIdentificationItemSurfaceNode(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to m_.
 * @return
 *    Reference to m_ object.
 */
IdentificationItemSurfaceNode&
IdentificationItemSurfaceNode::operator=(const IdentificationItemSurfaceNode& obj)
{
    if (this != &obj) {
        IdentificationItem::operator=(obj);
        copyHelperIdentificationItemSurfaceNode(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of m_ type.
 * @param ff
 *    Object that is copied.
 */
void
IdentificationItemSurfaceNode::copyHelperIdentificationItemSurfaceNode(const IdentificationItemSurfaceNode& idItem)
{
    m_surface = idItem.m_surface;
    m_nodeNumber = idItem.m_nodeNumber;
    m_contralateralFlag = idItem.m_contralateralFlag;
}

/**
 * Reset the selection item. 
 */
void 
IdentificationItemSurfaceNode::reset()
{
    IdentificationItem::reset();
    m_surface = NULL;
    m_nodeNumber = -1;
    m_contralateralFlag = false;
}

/**
 * @return Is m_ identified item valid?
 */
bool 
IdentificationItemSurfaceNode::isValid() const
{
    return (m_nodeNumber >= 0);
}

/**
 * @return Surface containing identified node.
 */
const Surface* 
IdentificationItemSurfaceNode::getSurface() const
{
    return m_surface;
}

/**
 * @return Surface containing identified node.
 */
Surface* 
IdentificationItemSurfaceNode::getSurface()
{
    return m_surface;
}

/**
 * Set the surface containing the identified node.
 * @param surface
 *    New value for surface.
 *
 */
void 
IdentificationItemSurfaceNode::setSurface(Surface* surface)
{
    m_surface = surface;
}

/**
 * return Number of identified node.
 */
int32_t 
IdentificationItemSurfaceNode::getNodeNumber() const
{
    return m_nodeNumber;
}

/**
 * Set node number that was identified.
 * @param nodeNumber
 *    New value for node.
 */
void 
IdentificationItemSurfaceNode::setNodeNumber(const int32_t nodeNumber)
{
    m_nodeNumber = nodeNumber;
}

/**
 * @return Is m_ a contralateral identification?
 */
bool 
IdentificationItemSurfaceNode::isContralateral() const
{
    return m_contralateralFlag;
}

/**
 * Set contralateral identification status.
 * @param status
 *    New status.
 */
void 
IdentificationItemSurfaceNode::setContralateral(const bool status)
{
    m_contralateralFlag = status;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString 
IdentificationItemSurfaceNode::toString() const
{
    AString text = IdentificationItem::toString();
    text += ("Surface: " + ((m_surface != NULL) ? m_surface->getFileNameNoPath() : "INVALID") + "\n");
    text += "Vertex: " + AString::number(m_nodeNumber) + "\n";
    if (isValid() && (m_surface != NULL)) {
        text += "Coordinate: " + AString::fromNumbers(m_surface->getCoordinate(m_nodeNumber), 3, ", ") + "\n";
    }
    text += "Contralateral: " + AString::fromBool(m_contralateralFlag) + "\n";
    return text;
}
