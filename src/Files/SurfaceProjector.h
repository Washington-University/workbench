#ifndef __SURFACE_PROJECTOR_H__
#define __SURFACE_PROJECTOR_H__

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
        
        SurfaceProjector(const SurfaceFile* leftSurfaceFile,
                         const SurfaceFile* rightSurfaceFile,
                         const SurfaceFile* cerebellumSurfaceFile);
        
        virtual ~SurfaceProjector();
        
        void projectItemToTriangle(SurfaceProjectedItem* spi);
        
        void projectItemToTriangleOrEdge(SurfaceProjectedItem* spi);
        
        void projectFociFile(FociFile* fociFile);
        
        void projectFocus(const int32_t focusIndex,
                          Focus* focus);
        
        void setSurfaceOffset(const float surfaceOffset);
        
    private:
        enum SurfaceHintType {
            SURFACE_HINT_FLAT,
            SURFACE_HINT_SPHERE,
            SURFACE_HINT_THREE_DIMENSIONAL
        };
        
        enum Mode {
            MODE_LEFT_RIGHT_CEREBELLUM,
            MODE_SURFACES
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
                                   ProjectionLocation& projectionLocation) const;
        
        void projectItem(SurfaceProjectedItem* spi,
                         SurfaceProjectedItem* secondSpi);
        
        void projectItemToSurfaceFile(const SurfaceFile* surfaceFile,
                                  SurfaceProjectedItem* spi);
        
        void projectToSurface(const SurfaceFile* surfaceFile,
                              const float xyz[3],
                              SurfaceProjectedItem* spi)
                                 ;
        
        void projectToSurfaceTriangle(const SurfaceFile* surfaceFile,
                              const ProjectionLocation& projectionLocation,
                              SurfaceProjectionBarycentric* baryProj)
                   ;

        int32_t projectToSurfaceTriangleAux(const SurfaceFile* surfaceFile,
                                            const ProjectionLocation& projectionLocation,
                                            SurfaceProjectionBarycentric* baryProj)
                   ;
        
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
                                         ProjectionLocation& projectionLocation);

        
        void projectWithVanEssenAlgorithm(const SurfaceFile* surfaceFile,
                                          const ProjectionLocation& projectionLocation,
                                          SurfaceProjectionVanEssen* spve)
           ;
        
        std::vector<const SurfaceFile*> m_surfaceFiles;
        
        const SurfaceFile* m_surfaceFileLeft;
        
        const SurfaceFile* m_surfaceFileRight;
        
        const SurfaceFile* m_surfaceFileCerebellum;
        
        const Mode m_mode;
        
        SurfaceHintType m_surfaceTypeHint;

        float m_sphericalSurfaceRadius;
        
        float m_surfaceOffset;
        
        bool m_surfaceOffsetValid;
        
        bool m_allowEdgeProjection;
        
        bool m_validateFlag;
        
        AString m_validateItemName;
        
        AString m_projectionWarning;
        
        /** Point in triangle test tolerance that requires point inside triangle */
        static float s_normalTriangleAreaTolerance;
        
        /** Point in triangle test tolerance that allows point outside triangle (degenerate case) */
        static float s_extremeTriangleAreaTolerance;
        
        /** Projection distance error used to compare original and after projection/unprojection */
        static float s_projectionDistanceError;
        
        /** Cutoff for item that that is projected to cortex and not cerebellum */
        static float s_corticalSurfaceCutoff;
        
        /** Cutoff for item that that is projected to cerebellum and not cortex */
        static float s_cerebellumSurfaceCutoff;
        
    };
    
#ifdef __SURFACE_PROJECTOR_DEFINE__
    float SurfaceProjector::s_normalTriangleAreaTolerance = -0.01;
    float SurfaceProjector::s_extremeTriangleAreaTolerance = -10000000.0;
    float SurfaceProjector::s_projectionDistanceError = 0.5;
    float SurfaceProjector::s_corticalSurfaceCutoff = 2.0;
    float SurfaceProjector::s_cerebellumSurfaceCutoff = 4.0;
#endif // __SURFACE_PROJECTOR_DEFINE__
} // namespace

#endif // __SURFACE_PROJECTOR_H__
