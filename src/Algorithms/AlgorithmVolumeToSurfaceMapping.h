#ifndef __ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__
#define __ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__

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

#include "AbstractAlgorithm.h"

#include "RibbonMappingHelper.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <vector>

namespace caret {
    
    class AlgorithmVolumeToSurfaceMapping : public AbstractAlgorithm
    {
        AlgorithmVolumeToSurfaceMapping();
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
                                        const SurfaceFile* innerSurf, const SurfaceFile* outerSurf,
                                        const VolumeFile* roiVol = NULL, const int32_t& subdivisions = 3, const bool& thinColumns = false,
                                        const int64_t& mySubVol = -1,
                                        const int& weightsOutVertex = -1, VolumeFile* weightsOut = NULL);
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
