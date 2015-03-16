
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __EVENT_SURFACE_STRUCTURES_VALID_GET_DECLARE__
#include "EventSurfaceStructuresValidGet.h"
#undef __EVENT_SURFACE_STRUCTURES_VALID_GET_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventSurfaceStructuresValidGet 
 * \brief Get valid surface structures and their number of nodes
 * \ingroup Files
 */

/**
 * Constructor.
 */
EventSurfaceStructuresValidGet::EventSurfaceStructuresValidGet()
: Event(EventTypeEnum::EVENT_SURFACE_STRUCTURES_VALID_GET)
{
    
}

/**
 * Destructor.
 */
EventSurfaceStructuresValidGet::~EventSurfaceStructuresValidGet()
{
}

/**
 * Add a structure and its number of nodes.  If the structure already
 * has been added with the same number of nodes, no action is taken.
 * If a structure already has been added with a DIFFERENT number of nodes,
 * a warning will be logged.
 *
 * @param structure
 *    The structure.
 * @param numberOfNodes
 *    Number of nodes associated with the structure.
 */
void
EventSurfaceStructuresValidGet::addStructure(const StructureEnum::Enum structure,
                  const int32_t numberOfNodes)
{
    std::map<StructureEnum::Enum, int32_t>::iterator iter = m_structureAndNumberOfNodes.find(structure);
    if (iter != m_structureAndNumberOfNodes.end()) {
        const int32_t structNumNodes = iter->second;
        if (structNumNodes != numberOfNodes) {
            const AString message("Structure "
                                  + StructureEnum::toGuiName(structure)
                                  + " has different node counts: "
                                  + AString::number(numberOfNodes)
                                  + " and "
                                  + AString::number(structNumNodes));
            CaretAssertMessage(0, message);
            CaretLogSevere(message);
        }
    }
    else {
        m_structureAndNumberOfNodes.insert(std::make_pair(structure,
                                                          numberOfNodes));
    }
}

/**
 * @return A map containing structures and their number of nodes.
 */
std::map<StructureEnum::Enum, int32_t>
EventSurfaceStructuresValidGet::getStructuresAndNumberOfNodes() const
{
    return m_structureAndNumberOfNodes;
}

