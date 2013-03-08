#ifndef __SURFACE_RESAMPLING_HELPER_H__
#define __SURFACE_RESAMPLING_HELPER_H__

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

#include "CaretPointer.h"
#include "SurfaceResamplingMethodEnum.h"

#include <map>
#include <vector>

namespace caret {

    class SurfaceFile;
    
    class SurfaceResamplingHelper
    {
        struct WeightElem
        {
            int node;
            float weight;
            WeightElem() { }
            WeightElem(const int& nodeIn, const float& weightIn) : node(nodeIn), weight(weightIn) { }
        };
        CaretArray<WeightElem> m_storagechunk;
        CaretArray<WeightElem*> m_weights;
        bool checkSphere(const SurfaceFile* surface);
        void changeRadius(const float& radius, const SurfaceFile* input, SurfaceFile* output);
        void computeWeightsAdapBaryArea(const SurfaceFile* currentSphere, const SurfaceFile* newSphere, const SurfaceFile* currentAreaSurf, const SurfaceFile* newAreaSurf, const float* currentRoi);
        void computeWeightsBarycentric(const SurfaceFile* currentSphere, const SurfaceFile* newSphere, const float* currentRoi);
        void makeBarycentricWeights(const SurfaceFile* from, const SurfaceFile* to, std::vector<std::map<int, float> >& weights, const float* currentRoi);
        void compactWeights(const std::vector<std::map<int, float> >& weights);
    public:
        SurfaceResamplingHelper() { }
        SurfaceResamplingHelper(const SurfaceResamplingMethodEnum::Enum& myMethod, const SurfaceFile* currentSphere, const SurfaceFile* newSphere,
                                const SurfaceFile* currentAreaSurf = NULL, const SurfaceFile* newAreaSurf = NULL, const float* currentRoi = NULL);
        ///resample real-valued data by means of weights, optional roi in SOURCE mesh
        void resampleNormal(const float* input, float* output);
        ///resample 3D coordinate data by means of weights, optional roi in SOURCE mesh
        void resample3DCoord(const float* input, float* output);
        ///resample label-like data according to which value gets the largest weight sum, optional roi in SOURCE mesh
        void resamplePopular(const int32_t* input, int32_t* output);
        ///resample float data according to what weight is largest, optional roi in SOURCE mesh
        void resampleLargest(const float* input, float* output);
        ///resample int data according to what weight is largest, optional roi in SOURCE mesh
        void resampleLargest(const int32_t* input, int32_t* output);
    };

}

#endif //__SURFACE_RESAMPLING_HELPER_H__
