
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

#include <algorithm>

#define __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__
#include "BrainStructureNodeAttributes.h"
#undef __BRAIN_STRUCTURE_NODE_ATTRIBUTE_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::BrainStructureNodeAttributes 
 * \brief Contains attributes for all node in a brain structure.
 *
 * Contains attributes for all nodes in a brain structure.
 * If the number of nodes in the brain structure changes,
 * this class' update() method must be called.
 */

/**
 * Constructor.
 */
BrainStructureNodeAttributes::BrainStructureNodeAttributes()
: CaretObject()
{
    this->update(0);
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
    return ("BrainStructureNodeAttributes");
}

void 
BrainStructureNodeAttributes::update(const int32_t numberOfNodes)
{
    if (numberOfNodes > 0) {
        this->identificationType.resize(numberOfNodes);
        this->setAllIdentificationNone();
    }
    else {
        this->identificationType.clear();
    }
}

void 
BrainStructureNodeAttributes::setAllIdentificationNone()
{
    std::fill(this->identificationType.begin(),
              this->identificationType.end(),
              NodeIdentificationTypeEnum::NONE);
}

/**
 * Get the identification type for the given node.
 * @param nodeIndex
 *     Number of node.
 * @return The identified status of the node.
 */
NodeIdentificationTypeEnum::Enum 
BrainStructureNodeAttributes::getIdentificationType(const int32_t nodeIndex) const
{
    CaretAssertVectorIndex(this->identificationType, nodeIndex);
    return this->identificationType[nodeIndex];
}

/**
 * Set the identification type for the given node.
 * @param nodeIndex
 *     Number of node.
 * @param identifiedStatus
 *    New identified status.
 */
void 
BrainStructureNodeAttributes::setIdentificationType(const int32_t nodeIndex,
                                                    const NodeIdentificationTypeEnum::Enum identificationType)
{
    CaretAssertVectorIndex(this->identificationType, nodeIndex);
    this->identificationType[nodeIndex] = identificationType;    
}
