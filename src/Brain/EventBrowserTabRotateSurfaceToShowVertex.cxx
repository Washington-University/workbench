
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX_DECLARE__
#include "EventBrowserTabRotateSurfaceToShowVertex.h"
#undef __EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventBrowserTabRotateSurfaceToShowVertex 
 * \brief Rotate surface in browser tab so that a vertex faces the user
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param browserTabIndex
 *    Index of browser tab (zero-based)
 * @param surfaceStructure
 *    The surface's structure
 * @param surfaceVertexIndices
 *    Index of the surface vertices (zero-based).  If more than one vertex the vertex nearest the average XYZ of the vertices is used.
 */
EventBrowserTabRotateSurfaceToShowVertex::EventBrowserTabRotateSurfaceToShowVertex(const int32_t browserTabIndex,
                                                                                   const StructureEnum::Enum surfaceStructure,
                                                                                   const std::vector<int32_t>& surfaceVertexIndices)
: Event(EventTypeEnum::EVENT_BROWSER_TAB_ROTATE_SURFACE_TO_SHOW_VERTEX),
m_browserTabIndex(browserTabIndex),
m_surfaceStructure(surfaceStructure),
m_surfaceVertexIndices(surfaceVertexIndices)
{
    
}

/**
 * Destructor.
 */
EventBrowserTabRotateSurfaceToShowVertex::~EventBrowserTabRotateSurfaceToShowVertex()
{
}

/**
 * @return Browser tab index (zero-based)
 */
int32_t
EventBrowserTabRotateSurfaceToShowVertex::getBrowserTabIndex() const
{
    return m_browserTabIndex;
}

/**
 * @return The surface's structure
 */
StructureEnum::Enum
EventBrowserTabRotateSurfaceToShowVertex::getSurfaceStructure() const
{
    return m_surfaceStructure;
}

/**
 * @return The surface vertex indices (zero-based).  If more than one vertex the vertex nearest the average XYZ of the vertices is used.
 */
const std::vector<int32_t>&
EventBrowserTabRotateSurfaceToShowVertex::getSurfaceVertexIndices() const
{
    return m_surfaceVertexIndices;
}
