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

#include "EventIdentificationSymbolRemoval.h"

#include "StructureEnum.h"

using namespace caret;

/**
 * Constructor for removal of ALL surface node ID symbols
 */
EventIdentificationSymbolRemoval::EventIdentificationSymbolRemoval()
: Event(EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL)
{
    this->structure  = StructureEnum::INVALID;
    this->nodeNumber = -1;
}

/**
 * Constructor for removal of specific node ID Symbol.
 * @param structure
 *   Structure for node.
 * @param nodeNumber
 *   Number of the node.
 */
EventIdentificationSymbolRemoval::EventIdentificationSymbolRemoval(const StructureEnum::Enum structure,
                                                                   const int32_t nodeNumber)
: Event(EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL)
{
    this->structure  = structure;
    this->nodeNumber = nodeNumber;
}

/**
 *  Destructor.
 */
EventIdentificationSymbolRemoval::~EventIdentificationSymbolRemoval()
{
    
}

/**
 * @return Are all surface ID symbols to be removed?
 */
bool 
EventIdentificationSymbolRemoval::isRemoveAllSurfaceSymbols() const
{
    return (this->nodeNumber < 0);
}

/**
 * @return Is a surface ID symbol for a specific node to be removed?
 * If so, use "getSurfaceNodeNumber()" to get the node number.
 */
bool 
EventIdentificationSymbolRemoval::isRemoveSurfaceNodeSymbol() const
{
    return (this->nodeNumber >= 0);
}

/**
 * @return Structure for removal of a specific node's ID symbol
 */
StructureEnum::Enum 
EventIdentificationSymbolRemoval::getSurfaceStructure() const
{
    return this->structure;
}

/**
 * @return Node number for removal of a specific node's ID symbol
 */
int32_t 
EventIdentificationSymbolRemoval::getSurfaceNodeNumber() const
{
    return this->nodeNumber;
}
