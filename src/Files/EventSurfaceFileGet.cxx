
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __EVENT_SURFACE_FILE_GET_DECLARE__
#include "EventSurfaceFileGet.h"
#undef __EVENT_SURFACE_FILE_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventSurfaceFileGet 
 * \brief Get surface files that have a specific structure and number of vertices
 * \ingroup Files
 */

/**
 * Constructor.
 * @param structure
 *   Find surface files with this structure
 * @param numberOfVertices
 *   Find surface files with this number of vertices
 */
EventSurfaceFileGet::EventSurfaceFileGet(const StructureEnum::Enum structure,
                                         const int32_t numberOfVertices)
: Event(EventTypeEnum::EVENT_SURFACE_FILE_GET),
m_structure(structure),
m_numberOfVertices(numberOfVertices)
{
    
}

/**
 * Destructor.
 */
EventSurfaceFileGet::~EventSurfaceFileGet()
{
}

/**
 * Add a surface file
 * @param surfaceFile
 *    Surface file to add
 */
void
EventSurfaceFileGet::addSurfaceFile(const SurfaceFile* surfaceFile)
{
    CaretAssert(surfaceFile);
    m_surfaceFiles.push_back(surfaceFile);
}

/**
 * @return The structure for surface file
 */
StructureEnum::Enum
EventSurfaceFileGet::getStructure() const
{
    return m_structure;
}

/**
 * @return The number of vertices in the surface file
 */
int32_t
EventSurfaceFileGet::getNumberOfVertices() const
{
    return m_numberOfVertices;
}

/**
 * @return The number of surface files that were found
 */
int32_t
EventSurfaceFileGet::getNumberOfSurfaceFiles() const
{
    return m_surfaceFiles.size();
}

/**
 * @return SurfaceFile at the given index
 * @param index
 *    Index of the surface file
 */
const SurfaceFile*
EventSurfaceFileGet::getSurfaceFile(const int32_t index) const
{
    CaretAssertVectorIndex(m_surfaceFiles, index);
    return m_surfaceFiles[index];
}
