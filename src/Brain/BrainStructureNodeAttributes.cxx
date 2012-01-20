
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
        this->identificationRGBA.resize(numberOfNodes * 4);
        this->setAllIdentificationNone();
    }
    else {
        this->identificationType.clear();
        this->identificationRGBA.clear();
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
    
    const int32_t rgbaIndex = nodeIndex * 4;
    CaretAssertVectorIndex(this->identificationRGBA, (rgbaIndex + 3));
    switch (identificationType) {
        case NodeIdentificationTypeEnum::NONE:
            break;
        case NodeIdentificationTypeEnum::NORMAL:
            this->identificationRGBA[rgbaIndex]   = 0.0;
            this->identificationRGBA[rgbaIndex+1] = 1.0;
            this->identificationRGBA[rgbaIndex+2] = 0.0;
            this->identificationRGBA[rgbaIndex+3] = 1.0;
            break;
        case NodeIdentificationTypeEnum::CONTRALATERAL:
            this->identificationRGBA[rgbaIndex]   = 0.0;
            this->identificationRGBA[rgbaIndex+1] = 0.0;
            this->identificationRGBA[rgbaIndex+2] = 1.0;
            this->identificationRGBA[rgbaIndex+3] = 1.0;
            break;
    }
}

/**
 * Get the RGBA color for node's identification symbol.
 * @param nodeIndex
 *    Index of the node.
 * @return
 *    RGBA color for symbol.
 */
const float* 
BrainStructureNodeAttributes::getIdentificationRGBA(const int32_t nodeIndex) const
{
    const int32_t rgbaIndex = nodeIndex * 4;
    CaretAssertVectorIndex(this->identificationRGBA, (rgbaIndex + 3));
    return &this->identificationRGBA[rgbaIndex];
}

/**
 * Set the RGBA color for node's identification symbol.
 * NOTE: setIdentificationType() will set the color for the symbol
 * so this method must be called AFTER setIdentificationType() to
 * override the symbol's color.
 *
 * @param nodeIndex
 *    Index of the node.
 * @param rgba
 *    RGB color for symbol.
 */
void 
BrainStructureNodeAttributes::setIdentificationRGBA(const int32_t nodeIndex,
                                                   const float rgba[3])
{
    const int32_t rgbaIndex = nodeIndex * 4;
    CaretAssertVectorIndex(this->identificationRGBA, (rgbaIndex + 3));
    this->identificationRGBA[rgbaIndex]   = rgba[0];
    this->identificationRGBA[rgbaIndex+1] = rgba[1];
    this->identificationRGBA[rgbaIndex+2] = rgba[2];
    this->identificationRGBA[rgbaIndex+3] = rgba[3];
}

