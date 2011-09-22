/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include <typeinfo>

#include "CaretAssert.h"
#include "EventBrainStructureGet.h"

using namespace caret;

/**
 * Construct an event for finding a brain structure.
 * @param brainStructureIdentifier
 *   Identifier of desired brain structure.
 */
EventBrainStructureGet::EventBrainStructureGet(const int64_t brainStructureIdentifier)
: Event(EventTypeEnum::EVENT_BRAIN_STRUCTURE_GET)
{
    this->brainStructure = NULL;
    this->brainStructureIdentifier = brainStructureIdentifier;
}

/**
 *  Destructore.
 */
EventBrainStructureGet::~EventBrainStructureGet()
{
    
}

/**
 * @return The identifier of desired brain structure.
 */
int64_t 
EventBrainStructureGet::getbrainStructureIdentifier() const
{
    return this->brainStructureIdentifier;
}

/**
 * @return The desired brain structure or NULL if not found.
 */
BrainStructure* 
EventBrainStructureGet::getBrainStructure() const
{
    return this->brainStructure;
}

/**
 * Set the brain structure.
 * @param brainStructure
 *    Brain structure that was found.
 */
void 
EventBrainStructureGet::setBrainStructure(BrainStructure* brainStructure)
{
    this->brainStructure = brainStructure;
}


