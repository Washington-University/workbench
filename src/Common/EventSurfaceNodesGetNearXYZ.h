#ifndef __EVENT_SURFACE_NODES_GET_NEAR_X_Y_Z_H__
#define __EVENT_SURFACE_NODES_GET_NEAR_X_Y_Z_H__

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



#include <memory>
#include <vector>

#include "Event.h"
#include "StructureEnum.h"
#include "Vector3D.h"

namespace caret {

    class SurfaceFile;
    
    class EventSurfaceNodesGetNearXYZ : public Event {
        
    public:
        class NodeInfo {
        public:
            /**
             * Constructor
             * @param structure
             *    Structure that owns node
             * @param xyz
             *    XYZ of node
             * @param distance
             *    Distance of node from query XYZ
             * @param nodeIndex
             *    Index of node
             */
            NodeInfo(const SurfaceFile* surfaceFile,
                     const StructureEnum::Enum structure,
                     const Vector3D& xyz,
                     const float distance,
                     const int32_t nodeIndex)
            : m_surfaceFile(const_cast<SurfaceFile*>(surfaceFile)),
            m_structure(structure),
            m_xyz(xyz),
            m_distance(distance),
            m_nodeIndex(nodeIndex) { }
            
            /** @return True if this instance is closer to query XYZ than the given instance */
            bool operator<(const NodeInfo& nodeInfo) const { return (m_distance < nodeInfo.m_distance); }
            
            /** @return Pointer to surface file containing node */
            const SurfaceFile* getSurfaceFile() const { return m_surfaceFile; }
            
            /** @return Structure that owns node */
            StructureEnum::Enum getStructure() const { return m_structure; }
            
            /** @return XYZ of node */
            Vector3D getXYZ() const { return m_xyz; }
            
            /** @return Index of node */
            int32_t getNodeIndex() const { return m_nodeIndex; }
            
        private:
            SurfaceFile* m_surfaceFile;
            StructureEnum::Enum m_structure;
            Vector3D  m_xyz;
            float m_distance;
            int32_t m_nodeIndex;
        };
        
        EventSurfaceNodesGetNearXYZ(const Vector3D& xyz,
                                    const float maximumDistanceFromXYZ);
        
        virtual ~EventSurfaceNodesGetNearXYZ();
        
        EventSurfaceNodesGetNearXYZ(const EventSurfaceNodesGetNearXYZ&) = delete;

        EventSurfaceNodesGetNearXYZ& operator=(const EventSurfaceNodesGetNearXYZ&) = delete;
        
        Vector3D getXYZ() const;
        
        float getMaximumDistanceFromXYZ() const;

        void addNearbyNode(const SurfaceFile* surfaceFile,
                           const StructureEnum::Enum structure,
                           const Vector3D& xyz,
                           const float distance,
                           const int32_t nodeInde);
        
        int32_t getNumberOfNearbyNodes() const;
        
        const NodeInfo& getNearbyNode(const int32_t index) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        const Vector3D m_xyz;
        
        const float m_maximumDistanceFromXYZ;
        
        mutable std::vector<NodeInfo> m_nodeInfo;
        
        mutable bool m_sortedFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_SURFACE_NODES_GET_NEAR_X_Y_Z_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_SURFACE_NODES_GET_NEAR_X_Y_Z_DECLARE__

} // namespace
#endif  //__EVENT_SURFACE_NODES_GET_NEAR_X_Y_Z_H__
