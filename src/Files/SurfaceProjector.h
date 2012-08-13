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
        
        enum SurfaceHintType {
            SURFACE_HINT_FLAT,
            SURFACE_HINT_SPHERE,
            SURFACE_HINT_THREE_DIMENSIONAL,
            SURFACE_HINT_UNKNOWN
        };
        
        enum ProjectionsAllowedType {
            PROJECTION_ALLOW_BARYCENTRIC,
            PROJECTION_ALLOW_BARYCENTRIC_AND_VAN_ESSEN
        };
        
        enum ProjectionPositionSourceType {
            PROJECTION_POSITION_SOURCE_PROJECTED_XYZ,
            PROJECTION_POSITION_SOURCE_STEREOTAXIC_XYZ
        };
        
        SurfaceProjector(const SurfaceFile* surface,
                         const SurfaceHintType surfaceTypeHint,
                         const ProjectionsAllowedType projectionsAllowed,
                         const bool surfaceMayGetModifiedFlag);
        
        virtual ~SurfaceProjector();
        
    private:
        SurfaceProjector(const SurfaceProjector& o);
        
        SurfaceProjector& operator=(const SurfaceProjector& o);

        void setSurfaceOffset(const float surfaceOffset);
        
        void projectToSurface(SurfaceProjectedItem* spi,
                              const ProjectionPositionSourceType positionSource)
                                                    throw (SurfaceProjectorException);
        
        //    SurfaceProjectorBarycentricInformation* projectToSurfaceBestTriangle2D(const float xyz[3])
        //            throw (SurfaceProjectorException);
        //
        //    SurfaceProjectorBarycentricInformation* projectToSurfaceForRegistration(const float xyz[3])
        //            throw (SurfaceProjectorException);
        //
        void projectToSurface(const float xyz[3],
                              SurfaceProjectionBarycentric* baryProj)
                    throw (SurfaceProjectorException);

        int32_t projectToSurfaceAux(const float xyzIn[3],
                                  SurfaceProjectionBarycentric* baryProj)
                    throw (SurfaceProjectorException);
        
        void findEnclosingTriangle(
                                   const int32_t nearestNode,
                                   const float xyz[3],
                                   SurfaceProjectionBarycentric* baryProj);
        
        void checkItemInTriangle(
                                 const int32_t triangleNumber,
                                 const float xyz[3],
                                 SurfaceProjectionBarycentric* baryProj);
        
        int32_t triangleAreas(
                              const float p1[3],
                              const float p2[3],
                              const float p3[3],
                              const float normal[3],
                              const float xyz[3],
                              float areasOut[3]);
        
        void projectWithVanEssenAlgorithm(const int32_t nearestTriangleIn,
                                          const float xyzIn[3],
                                          SurfaceProjectionVanEssen* spve)
            throw (SurfaceProjectorException);
        
        int32_t findNearestTriangle(const float xyz[3]);
        
        const SurfaceFile* m_surface;
        
        SurfaceHintType m_surfaceTypeHint;
        
        const ProjectionsAllowedType m_projectionsAllowed;
        
        const bool m_surfaceMayGetModifiedFlag;
        
        int32_t m_numberOfNodes;
        
        TopologyHelper* m_topologyHelper;
        
        std::set<int32_t> m_searchedTriangleIndices;
        
        static float s_triangleAreaTolerance;
        
        float m_nearestNodeToleranceSquared;
        
        float m_sphericalSurfaceRadius;
        
        float m_surfaceOffset;
        
        bool m_surfaceOffsetValid;
        
    };
    
#ifdef __SURFACE_PROJECTOR_DEFINE__
    float SurfaceProjector::s_triangleAreaTolerance = -0.01;
#endif // __SURFACE_PROJECTOR_DEFINE__
} // namespace

#endif // __SURFACE_PROJECTOR_H__
