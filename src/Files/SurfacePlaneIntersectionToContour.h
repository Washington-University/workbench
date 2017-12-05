#ifndef __SURFACE_PLANE_INTERSECTION_TO_CONTOUR_H__
#define __SURFACE_PLANE_INTERSECTION_TO_CONTOUR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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


#include <array>
#include <memory>

#include "CaretColorEnum.h"
#include "CaretObject.h"
#include "CaretPointer.h"
#include "TopologyHelper.h"

namespace caret {

    class GraphicsPrimitive;
    class Plane;
    class SurfaceFile;
    
    class SurfacePlaneIntersectionToContour : public CaretObject {
        
    public:
        SurfacePlaneIntersectionToContour(const SurfaceFile* surfaceFile,
                                          const Plane& plane,
                                          const CaretColorEnum::Enum caretColor,
                                          const float* vertexColoringRGBA,
                                          const float contourThicknessMillimeters);
        
        virtual ~SurfacePlaneIntersectionToContour();

        bool createContours(std::vector<GraphicsPrimitive*>& graphicsPrimitivesOut,
                            AString& errorMessageOut);

        // ADD_NEW_METHODS_HERE
    private:
        /**
         * Contains information about each surface vertex
         * and its relationship to the plane
         */
        class Vertex {
        public:
            Vertex(const std::array<float, 3>& xyz,
                   const float signedDistanceAbovePlane)
            : m_xyz(xyz),
            m_signedDistanceAbovePlane(signedDistanceAbovePlane),
            m_abovePlaneFlag(signedDistanceAbovePlane > 0.0f) { }
            
            ~Vertex() { }
            
            const std::array<float, 3> m_xyz;
            const float m_signedDistanceAbovePlane;
            const bool m_abovePlaneFlag;
        };
        
        /*
         * Contains information about an edge that intersects the plane
         */
        class IntersectionEdge {
        public:
            IntersectionEdge(const std::array<float, 3>& intersectionXYZ,
                 int32_t belowPlaneVertexIndex,
                 int32_t abovePlaneVertexIndex,
                 int32_t triangleOneIndex,
                 int32_t triangleTwoIndex)
            : m_intersectionXYZ(intersectionXYZ),
            m_belowPlaneVertexIndex(belowPlaneVertexIndex),
            m_abovePlaneVertexIndex(abovePlaneVertexIndex),
            m_triangleOneIndex(triangleOneIndex),
            m_triangleTwoIndex(triangleTwoIndex) { }
            
            /**
             * True if this and the other edge are connected to the same triangle.
             * @param otherEdge
             *     The other edge
             * @return
             *     True if connected to same triangle, else false.
             */
            bool hasMatchingTriangle(const IntersectionEdge* otherEdge) {
                if ((m_triangleOneIndex == otherEdge->m_triangleOneIndex)
                    || (m_triangleOneIndex == otherEdge->m_triangleTwoIndex)) {
                    return true;
                }
                if (m_triangleTwoIndex >= 0) {
                    if ((m_triangleTwoIndex == otherEdge->m_triangleOneIndex)
                        || (m_triangleTwoIndex == otherEdge->m_triangleTwoIndex)) {
                        return true;
                    }
                }
                return false;
            }
            
            AString toString() const {
                AString msg("Intersection=("
                            + AString::fromNumbers(m_intersectionXYZ.data(), 3, ",")
                            + ") below="
                            + AString::number(m_belowPlaneVertexIndex)
                            + " aboveV="
                            + AString::number(m_abovePlaneVertexIndex)
                            + " T1="
                            + AString::number(m_triangleOneIndex)
                            + " T2="
                            + AString::number(m_triangleTwoIndex));
                return msg;
            }
            /** coordinate of where edge intersects plane */
            std::array<float, 3> m_intersectionXYZ;
            
            /** vertex below the plane  (index for surface vertex, topology helper vertex, and this class' m_vertices */
            int32_t m_belowPlaneVertexIndex;
            
            /** vertex above the plane */
            int32_t m_abovePlaneVertexIndex;
            
            /** first triangle used by edge (index for ToplogyHelper's TopologyTileInfo) */
            int32_t m_triangleOneIndex;
            
            /** second triangle used by edge (index for ToplogyHelper's TopologyTileInfo) */
            int32_t m_triangleTwoIndex;
            
            bool m_processedFlag = false;
        };
        
        SurfacePlaneIntersectionToContour(const SurfacePlaneIntersectionToContour&);

        SurfacePlaneIntersectionToContour& operator=(const SurfacePlaneIntersectionToContour&);
        
        void prepareVertices();
        
        void prepareEdges();
        
        void generateContours(std::vector<GraphicsPrimitive*>& graphicsPrimitivesOut);
        
        GraphicsPrimitive* generateContourFromEdge(IntersectionEdge* edge);
        
        const SurfaceFile* m_surfaceFile;
        
        const Plane& m_plane;
        
        const CaretColorEnum::Enum m_caretColor;
        
        const float* m_vertexColoringRGBA;
        
        const float m_contourThicknessPercentOfViewportHeight;
        
        std::array<float, 4> m_solidRGBA;
        
        CaretPointer<TopologyHelper> m_topologyHelper;
        
        std::vector<std::unique_ptr<Vertex>> m_vertices;
        
        std::vector<std::unique_ptr<IntersectionEdge>> m_intersectingEdges;
        
        /** index with a topo helper edge index to get its intersecting edge (-1) if edge does not intersect */
        std::vector<int32_t> m_topoHelperEdgeToIntersectingEdgeIndices;
        
        bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SURFACE_PLANE_INTERSECTION_TO_CONTOUR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_PLANE_INTERSECTION_TO_CONTOUR_DECLARE__

} // namespace
#endif  //__SURFACE_PLANE_INTERSECTION_TO_CONTOUR_H__
