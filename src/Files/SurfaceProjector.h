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
        
        enum ProjectionsAllowedType {
            PROJECTION_ALLOW_BARYCENTRIC,
            PROJECTION_ALLOW_BARYCENTRIC_AND_VAN_ESSEN
        };
        
        SurfaceProjector(const SurfaceFile* surfaceFile,
                         const ProjectionsAllowedType projectionsAllowed);
        
        SurfaceProjector(const std::vector<const SurfaceFile*>& surfaceFiles,
                         const ProjectionsAllowedType projectionsAllowed);
        
        virtual ~SurfaceProjector();
        
        void projectItem(SurfaceProjectedItem* spi)
                              throw (SurfaceProjectorException);
        
        void setSurfaceOffset(const float surfaceOffset);
        
    private:
        enum SurfaceHintType {
            SURFACE_HINT_FLAT,
            SURFACE_HINT_SPHERE,
            SURFACE_HINT_THREE_DIMENSIONAL
        };
        
        SurfaceProjector(const SurfaceProjector& o);
        
        SurfaceProjector& operator=(const SurfaceProjector& o);

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
                              const float xyz[3],
                              SurfaceProjectionBarycentric* baryProj)
                    throw (SurfaceProjectorException);

        int32_t projectToSurfaceAux(const SurfaceFile* surfaceFile,
                                    const float xyzIn[3],
                                  SurfaceProjectionBarycentric* baryProj)
                    throw (SurfaceProjectorException);
        
        void findEnclosingTriangle(const SurfaceFile* surfaceFile,
                                   const int32_t nearestNode,
                                   const float xyz[3],
                                   SurfaceProjectionBarycentric* baryProj);
        
        void checkItemInTriangle(const SurfaceFile* surfaceFile,
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
        
        void projectWithVanEssenAlgorithm(const SurfaceFile* surfaceFile,
                                          const int32_t nearestTriangleIn,
                                          const float xyzIn[3],
                                          SurfaceProjectionVanEssen* spve)
            throw (SurfaceProjectorException);
        
        int32_t findNearestTriangle(const SurfaceFile* surfaceFile,
                                    const float xyz[3]);
        
        std::vector<const SurfaceFile*> m_surfaceFiles;
        
        SurfaceHintType m_surfaceTypeHint;
        
        const ProjectionsAllowedType m_projectionsAllowed;
        
        std::vector<bool> m_searchedTriangleFlags;
        
        static const float s_triangleAreaTolerance;
        
        float m_nearestNodeToleranceSquared;
        
        float m_sphericalSurfaceRadius;
        
        float m_surfaceOffset;
    };
    
#ifdef __SURFACE_PROJECTOR_DEFINE__
    const float SurfaceProjector::s_triangleAreaTolerance = -0.01;
#endif // __SURFACE_PROJECTOR_DEFINE__
} // namespace

#endif // __SURFACE_PROJECTOR_H__
