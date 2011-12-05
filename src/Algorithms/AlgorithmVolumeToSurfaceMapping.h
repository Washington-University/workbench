#ifndef __ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__
#define __ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__

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

#include "AbstractAlgorithm.h"
#include <vector>
#include <algorithm>
#include "Vector3d.h"

namespace caret {
    
    struct VoxelWeight
    {//for precomputation in ribbon mapping
        float weight;
        int64_t ijk[3];
        VoxelWeight() { };
        VoxelWeight(const float weightIn, const int64_t* ijkIn)
        {
            weight = weightIn;
            ijk[0] = ijkIn[0];
            ijk[1] = ijkIn[1];
            ijk[2] = ijkIn[2];
        }
    };
    
    struct TriInfo
    {
        Vector3d m_xyz[3];
        float m_planeEq[3];//x coef, y coef, const : z = [0] * x + [1] * y + [2]
        bool vertRayHit(const float* xyz);//true if a +z ray from point hits this triangle
        void initialize(const float* xyz1, const float* xyz2, const float* xyz3);
    };
    
    struct QuadInfo
    {
        TriInfo m_tris[2][2];
        int vertRayHit(const float* xyz);//+z ray intersect: 0 if never, 1 if only 1 of the 2 triangulations, 2 if both
        void initialize(const float* xyz1, const float* xyz2, const float* xyz3, const float* xyz4);
    };
    
    struct PolyInfo
    {
        std::vector<TriInfo> m_tris;
        std::vector<QuadInfo> m_quads;
        PolyInfo(const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int32_t node);//surfaces MUST be in node correspondence, otherwise SEVERE strangeness, possible crashes
        int isInside(const float* xyz);//0 for no, 2 for yes, 1 for if only one of the two triangulations
    private:
        void addTri(const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int32_t root, const int32_t node2, const int32_t node3);//adds the tri for each surface, plus the quad
    };
    
    class AlgorithmVolumeToSurfaceMapping : public AbstractAlgorithm
    {
        AlgorithmVolumeToSurfaceMapping();
        void precomputeWeights(std::vector<std::vector<VoxelWeight> >& myWeights, VolumeFile* myVol, SurfaceFile* innerSurf, SurfaceFile* outerSurf, VolumeFile* roiVol, int numDivisions);//surfaces MUST be in node correspondence, otherwise SEVERE strangeness, possible crashes
        float computeVoxelFraction(const VolumeFile* myVolume, const int64_t* ijk, PolyInfo& myPoly, const int divisions, const Vector3d& ivec, const Vector3d& jvec, const Vector3d& kvec);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        enum Method
        {
            TRILINEAR,
            NEAREST_NEIGHBOR,
            RIBBON_CONSTRAINED
        };
        AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, VolumeFile* myVolume, SurfaceFile* mySurface, MetricFile* myMetricOut, Method myMethod, int64_t mySubVol = -1, SurfaceFile* innerSurf = NULL, SurfaceFile* outerSurf = NULL, VolumeFile* roiVol = NULL, int32_t subdivisions = 3);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters*, ProgressObject*);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeToSurfaceMapping> AutoAlgorithmVolumeToSurfaceMapping;

}

#endif //__ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__
