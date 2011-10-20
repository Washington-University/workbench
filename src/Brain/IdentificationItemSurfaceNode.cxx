
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
    this->surface = NULL;
    this->nodeNumber = -1;
}

/**
 * Destructor.
 */
IdentificationItemSurfaceNode::~IdentificationItemSurfaceNode()
{
    
}

/**
 * Reset this selection item. 
 */
void 
IdentificationItemSurfaceNode::reset()
{
    IdentificationItem::reset();
    this->surface = NULL;
    this->nodeNumber = -1;
}

/**
 * return Is this identified item valid?
 */
bool 
IdentificationItemSurfaceNode::isValid() const
{
    return (this->nodeNumber >= 0);
}

/**
 * return Surface containing identified node.
 */
const Surface* 
IdentificationItemSurfaceNode::getSurface() const
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
IdentificationItemSurfaceNode::setSurface(Surface* surface)
{
    this->surface = surface;
}

/**
 * return Number of identified node.
 */
int32_t 
IdentificationItemSurfaceNode::getNodeNumber() const
{
    return this->nodeNumber;
}

/**
 * Set node number that was identified.
 * @param nodeNumber
 *    New value for node.
 */
void 
IdentificationItemSurfaceNode::setNodeNumber(const int32_t nodeNumber)
{
    this->nodeNumber = nodeNumber;
}

