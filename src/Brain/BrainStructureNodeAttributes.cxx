
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

#define __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__
#include "BrainStructureNodeAttributes.h"
#undef __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__

using namespace caret;


    
/**
 * \class BrainStructureNodeAttributes 
 * \brief Contains attributes for a node in a brain structure.
 *
 * Contains attributes for a node in a brain structure.
 */

/**
 * Constructor.
 */
BrainStructureNodeAttributes::BrainStructureNodeAttributes()
: CaretObject()
{
    this->reset();
}

/**
 * Destructor.
 */
BrainStructureNodeAttributes::~BrainStructureNodeAttributes()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainStructureNodeAttributes::toString() const
{
    return ("identified=" + AString::fromBool(this->identifiedStatus));
}

/**
 * Reset this node's attributes.
 */
void 
BrainStructureNodeAttributes::reset()
{
    this->identifiedStatus = false;
}

/**
 * @return The identified status of the node.
 */
bool 
BrainStructureNodeAttributes::isIdentified() const
{
    return this->identifiedStatus;
}

/**
 * Set the identified status of the node.
 * @param identifiedStatus
 *    New identified status.
 */
void 
BrainStructureNodeAttributes::setIdentified(const bool identifiedStatus)
{
    this->identifiedStatus = identifiedStatus;
}

