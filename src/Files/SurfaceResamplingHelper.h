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
        static bool checkSphere(const SurfaceFile* surface);
        static void changeRadius(const float& radius, const SurfaceFile* input, SurfaceFile* output);
        void computeWeightsAdapBaryArea(const SurfaceFile* currentSphere, const SurfaceFile* newSphere, const SurfaceFile* currentAreaSurf, const SurfaceFile* newAreaSurf, const float* currentRoi);
        void computeWeightsBarycentric(const SurfaceFile* currentSphere, const SurfaceFile* newSphere, const float* currentRoi);
        static void makeBarycentricWeights(const SurfaceFile* from, const SurfaceFile* to, std::vector<std::map<int, float> >& weights, const float* currentRoi);
        void compactWeights(const std::vector<std::map<int, float> >& weights);
    public:
        SurfaceResamplingHelper() { }
        SurfaceResamplingHelper(const SurfaceResamplingMethodEnum::Enum& myMethod, const SurfaceFile* currentSphere, const SurfaceFile* newSphere,
                                const SurfaceFile* currentAreaSurf = NULL, const SurfaceFile* newAreaSurf = NULL, const float* currentRoi = NULL);
        ///resample real-valued data by means of weights
        void resampleNormal(const float* input, float* output, const float& invalidVal = 0.0f) const;
        ///resample 3D coordinate data by means of weights
        void resample3DCoord(const float* input, float* output) const;
        ///resample label-like data according to which value gets the largest weight sum
        void resamplePopular(const int32_t* input, int32_t* output, const int32_t& invalidVal = 0) const;
        ///resample float data according to what weight is largest
        void resampleLargest(const float* input, float* output, const float& invalidVal = 0.0f) const;
        ///resample int data according to what weight is largest
        void resampleLargest(const int32_t* input, int32_t* output, const int32_t& invalidVal = 0) const;
        ///get the ROI of nodes that have data within the input ROI
        void getResampleValidROI(float* output) const;
        
        ///resample a cut surface - not something you will apply multiple times, so static method
        static void resampleCutSurface(const SurfaceFile* cutSurfaceIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere, SurfaceFile* surfaceOut);
    };

}

#endif //__SURFACE_RESAMPLING_HELPER_H__
