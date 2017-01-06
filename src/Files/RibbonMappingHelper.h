#ifndef __RIBBON_MAPPING_HELPER_H__
#define __RIBBON_MAPPING_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "stdint.h"
#include <cstddef>
#include <vector>

namespace caret
{
    
    class SurfaceFile;
    class VolumeSpace;
    
    struct VoxelWeight
    {//for precomputation in ribbon/myelin style volume to surface mapping
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
    
    class RibbonMappingHelper
    {
    public:
        ///compute per-vertex ribbon mapping weights - surfaces must have vertex correspondence, or an exception is thrown
        static void computeWeightsRibbon(std::vector<std::vector<VoxelWeight> >& myWeightsOut, const VolumeSpace& myVolSpace,
                                         const SurfaceFile* innerSurf, const SurfaceFile* outerSurf,
                                         const float* roiFrame = NULL, const int& numDivisions = 3, const bool& thinColumn = false);
    };

}

#endif //__RIBBON_MAPPING_HELPER_H__
