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

#include "Vector3D.h"
#include "VolumeFile.h"

#include <vector>

namespace caret {
    
    struct VoxelWeight
    {//for precomputation in ribbon/myelin style mapping
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
        Vector3D m_xyz[3];
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
        void precomputeWeightsRibbon(std::vector<std::vector<VoxelWeight> >& myWeights, const VolumeFile* myVol, const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const VolumeFile* roiVol, const int& numDivisions);//surfaces MUST be in node correspondence, otherwise SEVERE strangeness, possible crashes
        float computeVoxelFraction(const VolumeFile* myVolume, const int64_t* ijk, PolyInfo& myPoly, const int divisions, const Vector3D& ivec, const Vector3D& jvec, const Vector3D& kvec);
        void precomputeWeightsMyelin(std::vector<std::vector<VoxelWeight> >& myWeights, const SurfaceFile* mySurface, const VolumeFile* roiVol, const MetricFile* thickness, const float& sigma);
        enum Method
        {
            TRILINEAR,
            ENCLOSING_VOXEL,
            RIBBON_CONSTRAINED,
            CUBIC,
            MYELIN_STYLE
        };
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut, const VolumeFile::InterpType& myMethod,
                                        const int64_t& mySubVol = -1);
        AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut,
                                        const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const VolumeFile* roiVol = NULL, const int32_t& subdivisions = 3, const int64_t& mySubVol = -1);
        AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut,
                                        const VolumeFile* roiVol, const MetricFile* thickness, const float& sigma, const int64_t& mySubVol = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeToSurfaceMapping> AutoAlgorithmVolumeToSurfaceMapping;

}

#endif //__ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__
