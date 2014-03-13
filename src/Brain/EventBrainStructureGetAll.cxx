
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

#define __EVENT_BRAIN_STRUCTURE_GET_ALL_DECLARE__
#include "EventBrainStructureGetAll.h"
#undef __EVENT_BRAIN_STRUCTURE_GET_ALL_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrainStructureGetAll 
 * \brief Get all brain structures.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventBrainStructureGetAll::EventBrainStructureGetAll()
: Event(EventTypeEnum::EVENT_BRAIN_STRUCTURE_GET_ALL)
{
    
}

/**
 * Destructor.
 */
EventBrainStructureGetAll::~EventBrainStructureGetAll()
{
}

/**
 * Add a brain structure.
 * 
 * @param brainStructure
 *    Brain structure that is added.
 */
void
EventBrainStructureGetAll::addBrainStructure(BrainStructure* brainStructure)
{
    m_brainStructures.push_back(brainStructure);
}

/**
 * @return Number of brain structures that were found.
 */
int32_t
EventBrainStructureGetAll::getNumberOfBrainStructures() const
{
    return m_brainStructures.size();
}

/**
 * Get the brain structure at the given index.
 *
 * @param indx
 *    Index of the brain structure.
 * @return
 *    Brain structure at the given index.
 */
BrainStructure*
EventBrainStructureGetAll::getBrainStructureByIndex(const int32_t indx)
{
    CaretAssertVectorIndex(m_brainStructures,
                           indx);
    return m_brainStructures[indx];
}

/**
 * Get the brain structure of the specified type.
 *
 * @param structure
 *    Type of structure.
 * @return
 *    Brain structure with the given structure type or NULL if not found.
 */
BrainStructure*
EventBrainStructureGetAll::getBrainStructureByStructure(const StructureEnum::Enum structure)
{
    for (std::vector<BrainStructure*>::iterator iter = m_brainStructures.begin();
         iter != m_brainStructures.end();
         iter++) {
        BrainStructure* bs = *iter;
        if (bs->getStructure() == structure) {
            return bs;
        }
    }
    
    return NULL;
}

