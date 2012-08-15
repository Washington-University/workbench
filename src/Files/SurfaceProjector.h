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
        
        SurfaceProjector(const SurfaceProjector& o);
        
        SurfaceProjector& operator=(const SurfaceProjector& o);

        void initializeMembersSurfaceProjector();
        
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
        
        void computeSurfaceNearestNodeTolerances();
        
        std::vector<const SurfaceFile*> m_surfaceFiles;
        
        SurfaceHintType m_surfaceTypeHint;

        std::vector<bool> m_searchedTriangleFlags;
        
        static float s_triangleAreaTolerance;
        
        std::vector<float> m_surfaceNearestNodeToleranceSquared;
        
        float m_nearestNodeToleranceSquared;
        
        float m_sphericalSurfaceRadius;
        
        float m_surfaceOffset;
        bool m_surfaceOffsetValid;
        
        bool m_allowEdgeProjection;
    };
    
#ifdef __SURFACE_PROJECTOR_DEFINE__
    float SurfaceProjector::s_triangleAreaTolerance = -0.01;
#endif // __SURFACE_PROJECTOR_DEFINE__
} // namespace

#endif // __SURFACE_PROJECTOR_H__
