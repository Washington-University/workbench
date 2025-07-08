
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

#define __EVENT_SURFACE_NODES_GET_NEAR_X_Y_Z_DECLARE__
#include "EventSurfaceNodesGetNearXYZ.h"
#undef __EVENT_SURFACE_NODES_GET_NEAR_X_Y_Z_DECLARE__

#include <algorithm>

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventSurfaceNodesGetNearXYZ 
 * \brief Find all nodes within a distance from an XYZ for all primary anatomical surfaces
 * \ingroup Common
 */

/**
 * Constructor.
 * @param xyz
 *    The XYZ
 * @param maximumDistanceFromXYZ
 *    Maximum distance surface nodes can be from XYZ
 */
EventSurfaceNodesGetNearXYZ::EventSurfaceNodesGetNearXYZ(const Vector3D& xyz,
                                                         const float maximumDistanceFromXYZ)
: Event(EventTypeEnum::EVENT_SURFACE_NODES_GET_NEAR_XYZ),
m_xyz(xyz),
m_maximumDistanceFromXYZ(maximumDistanceFromXYZ)
{
    
}

/**
 * Destructor.
 */
EventSurfaceNodesGetNearXYZ::~EventSurfaceNodesGetNearXYZ()
{
}

/**
 * @return The XYZ
 */
Vector3D
EventSurfaceNodesGetNearXYZ::getXYZ() const
{
    return m_xyz;
}

/**
 * @return Maximum distance nodes can be from XYZ
 */
float 
EventSurfaceNodesGetNearXYZ::getMaximumDistanceFromXYZ() const
{
    return m_maximumDistanceFromXYZ;
}

/**
 * Add a nearby node
 * @param surfaceFile
 *    SurfaceFile containing the node
 * @param structure
 *    Structure that owns node
 * @param xyz
 *    XYZ of node
 * @param distance
 *    Distance of node from query XYZ
 * @param nodeIndex
 *    Index of node

 */
void
EventSurfaceNodesGetNearXYZ::addNearbyNode(const SurfaceFile* surfaceFile,
                                           const StructureEnum::Enum structure,
                                           const Vector3D& xyz,
                                           const float distance,
                                           const int32_t nodeIndex)
{
    m_nodeInfo.emplace_back(surfaceFile,
                            structure,
                            xyz,
                            distance,
                            nodeIndex);
    m_sortedFlag = false;
}

/**
 * @return Number of nodes that were within 'tolerance' distance of query XYZ
 */
int32_t
EventSurfaceNodesGetNearXYZ::getNumberOfNearbyNodes() const
{
    return m_nodeInfo.size();
}

/**
 * @return Node info at given index
 * @param index
 *    Index of node info
 * Item at index=0 is closest, item at index=1 is next closest, etc.
 */
const EventSurfaceNodesGetNearXYZ::NodeInfo&
EventSurfaceNodesGetNearXYZ::getNearbyNode(const int32_t index) const
{
    if ( ! m_sortedFlag) {
        m_sortedFlag = true;
        std::sort(m_nodeInfo.begin(),
                  m_nodeInfo.end());
    }
    
    CaretAssertVectorIndex(m_nodeInfo, index);
    return m_nodeInfo[index];
}

