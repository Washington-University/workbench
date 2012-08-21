#ifndef __SURFACE_PROJECTOR_H__
#define __SURFACE_PROJECTOR_H__

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


#include "CaretObject.h"
#include "SurfaceProjectorException.h"

#include <stdint.h>

#include <set>

namespace caret {
    
    class FociFile;
    class Focus;
    class SurfaceFile;
    class SurfaceProjectedItem;
    class SurfaceProjectionBarycentric;
    class SurfaceProjectionVanEssen;
    class TopologyHelper;
    
    /**
     * Class for projecting items to the surface.
     */
    class SurfaceProjector : public CaretObject {
        
    public:
        
        SurfaceProjector(const SurfaceFile* surfaceFile);
        
        SurfaceProjector(const std::vector<const SurfaceFile*>& surfaceFiles);
        
        virtual ~SurfaceProjector();
        
        void projectItemToTriangle(SurfaceProjectedItem* spi) throw (SurfaceProjectorException);
        
        void projectItemToTriangleOrEdge(SurfaceProjectedItem* spi) throw (SurfaceProjectorException);
        
        void projectFociFile(FociFile* fociFile) throw (SurfaceProjectorException);
        
        void projectFocus(Focus* focus) throw (SurfaceProjectorException);
        
        void setSurfaceOffset(const float surfaceOffset);
        
    private:
        enum SurfaceHintType {
            SURFACE_HINT_FLAT,
            SURFACE_HINT_SPHERE,
            SURFACE_HINT_THREE_DIMENSIONAL
        };
        
        class ProjectionLocation {
        public:
            enum Type {
                EDGE,
                INVALID,
                NODE,
                TRIANGLE
            };
            
            ProjectionLocation();
            
            ~ProjectionLocation();
            
            AString toString(const SurfaceFile* surfaceFile) const;
            
            /** Type of surface item projected to */
            Type m_type;
            /** Coordinate that was projected */
            float m_pointXYZ[3];
            /** Nearest coordinate on surface */
            float m_surfaceXYZ[3];
            /** Nearest triangle(s) indices (closest triangle always first) */
            int32_t* m_triangleIndices;
            /** Number of triangles */
            int32_t m_numberOfTriangles;
            /** Absolute distance to the surface */
            float m_absoluteDistance;
            /** Signed distance to surface (positive=>above, negative=>below) */
            float m_signedDistance;
            /** Nodes of node/edge/triangle (node has 1 element, edge 2, triangle 3) */
            int32_t m_nodes[3];
            /** Weights cooresponding to nodes (node has 1 element, edge 2, triangle 3) */
            float m_weights[3];
            /** Node nearest to the coordinate that was projected */
            int32_t m_nearestNode;
        };
        
        SurfaceProjector(const SurfaceProjector& o);
        
        SurfaceProjector& operator=(const SurfaceProjector& o);

        void initializeMembersSurfaceProjector();
        
        void getProjectionLocation(const SurfaceFile* surfaceFile,
                                   const float xyz[3],
                                   ProjectionLocation& projectionLocation) const throw (SurfaceProjectorException);
        
        void projectItem(SurfaceProjectedItem* spi) throw (SurfaceProjectorException);
        
        //    SurfaceProjectorBarycentricInformation* projectToSurfaceBestTriangle2D(const float xyz[3])
        //            throw (SurfaceProjectorException);
        //
        //    SurfaceProjectorBarycentricInformation* projectToSurfaceForRegistration(const float xyz[3])
        //            throw (SurfaceProjectorException);
        //
        void projectToSurface(const SurfaceFile* surfaceFile,
                              const float xyz[3],
                              SurfaceProjectedItem* spi)
                                  throw (SurfaceProjectorException);
        
        void projectToSurfaceTriangle(const SurfaceFile* surfaceFile,
                              const ProjectionLocation& projectionLocation,
                              SurfaceProjectionBarycentric* baryProj)
                    throw (SurfaceProjectorException);

        int32_t projectToSurfaceTriangleAux(const SurfaceFile* surfaceFile,
                                            const ProjectionLocation& projectionLocation,
                                            SurfaceProjectionBarycentric* baryProj)
                    throw (SurfaceProjectorException);
        
        void findEnclosingTriangle(const SurfaceFile* surfaceFile,
                                   const int32_t nearestNode,
                                   const float xyz[3],
                                   SurfaceProjectionBarycentric* baryProj);
        
        void checkItemInTriangle(const SurfaceFile* surfaceFile,
                                 const int32_t triangleNumber,
                                 const float xyz[3],
                                 const float degenerateTolerance,
                                 SurfaceProjectionBarycentric* baryProj);
        
        int32_t triangleAreas(
                              const float p1[3],
                              const float p2[3],
                              const float p3[3],
                              const float normal[3],
                              const float xyz[3],
                              const float degenerateTolerance,
                              float areasOut[3]);
        
        void convertToTriangleProjection(const SurfaceFile* surfaceFile,
                                         ProjectionLocation& projectionLocation) throw (SurfaceProjectorException);

        
        void projectWithVanEssenAlgorithm(const SurfaceFile* surfaceFile,
                                          const ProjectionLocation& projectionLocation,
                                          SurfaceProjectionVanEssen* spve)
            throw (SurfaceProjectorException);
        
        int32_t findNearestTriangle(const SurfaceFile* surfaceFile,
                                    const float xyz[3]);
        
        void computeSurfaceNearestNodeTolerances();
        
        std::vector<const SurfaceFile*> m_surfaceFiles;
        
        SurfaceHintType m_surfaceTypeHint;

        std::vector<bool> m_searchedTriangleFlags;
        
        std::vector<float> m_surfaceNearestNodeToleranceSquared;
        
        float m_nearestNodeToleranceSquared;
        
        float m_sphericalSurfaceRadius;
        
        float m_surfaceOffset;
        bool m_surfaceOffsetValid;
        
        bool m_allowEdgeProjection;
        
        bool m_validateFlag;
        
        AString m_validateItemName;
        
        /** Point in triangle test tolerance that requires point inside triangle */
        static float s_normalTriangleAreaTolerance;
        
        /** Point in triangle test tolerance that allows point outside triangle (degenerate case) */
        static float s_extremeTriangleAreaTolerance;
        
    };
    
#ifdef __SURFACE_PROJECTOR_DEFINE__
    float SurfaceProjector::s_normalTriangleAreaTolerance = -0.01;
    float SurfaceProjector::s_extremeTriangleAreaTolerance = -10000000.0;
#endif // __SURFACE_PROJECTOR_DEFINE__
} // namespace

#endif // __SURFACE_PROJECTOR_H__
