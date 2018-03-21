#ifndef __METRIC_SMOOTHING_OBJECT_H__
#define __METRIC_SMOOTHING_OBJECT_H__

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

//NOTE: this is largely for special-purpose smoothing by providing greater efficiency and flexibility, since it can be reused with different rois or on different metric objects
//      after being constructed once (constructor takes a while).  If you just want to smooth one metric object with one (or no) ROI, you probably want AlgorithmMetricSmoothing.
//
//NOTE: this object contains no mutable members, multiple threads can call the same function on the same instance and expect consistent behavior, while running concurrently,
//      as long as they don't call it with output arguments that overlap (same instance, same row, or one row plus full metric, etc)
//
//NOTE: for a static ROI, it is (sometimes much) more efficient to use it in the constructor, and provide no ROI (NULL) to the functions, using both an ROI in constructor and in method
//      will result in the effective ROI being the logical AND of the two (intersection).

#include "stdint.h"
#include "stddef.h"
#include <vector>

namespace caret {
    
    class SurfaceFile;
    class MetricFile;
    
    class MetricSmoothingObject
    {
    public:
        enum Method
        {
            GEO_GAUSS_AREA,
            GEO_GAUSS_EQUAL,
            GEO_GAUSS
        };
        MetricSmoothingObject(const SurfaceFile* mySurf, const float& kernel, const MetricFile* myRoi = NULL, Method myMethod = GEO_GAUSS_AREA, const float* nodeAreas = NULL);
        void smoothColumn(const MetricFile* metricIn, const int& whichColumn, MetricFile* columnOut, const MetricFile* roi = NULL, const bool& fixZeros = false) const;
        void smoothColumn(const MetricFile* metricIn, const int& whichColumn, MetricFile* metricOut, const int& whichOutColumn, const MetricFile* roi = NULL, const int& whichRoiColumn = 0, const bool& fixZeros = false) const;
        void smoothMetric(const MetricFile* metricIn, MetricFile* metricOut, const MetricFile* roi = NULL, const bool& fixZeros = false) const;
    private:
        struct WeightList
        {
            std::vector<int32_t> m_nodes;
            std::vector<float> m_weights;
            float m_weightSum;
        };
        std::vector<WeightList> m_weightLists;
        void smoothColumnInternal(float* scratch, const MetricFile* metricIn, const int& whichColumn, MetricFile* metricOut, const int& whichOutColumn, const bool& fixZeros) const;
        void smoothColumnInternal(float* scratch, const MetricFile* metricIn, const int& whichColumn, MetricFile* metricOut, const int& whichOutColumn, const MetricFile* roi, const int& whichRoiColumn, const bool& fixZeros) const;
        void precomputeWeights(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, Method myMethod, const float* nodeAreas);
        void precomputeWeightsGeoGauss(const SurfaceFile* mySurf, float myKernel, const float* nodeAreas);
        void precomputeWeightsROIGeoGauss(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, const float* nodeAreas);
        void precomputeWeightsGeoGaussArea(const SurfaceFile* mySurf, float myKernel, const float* nodeAreas);
        void precomputeWeightsROIGeoGaussArea(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, const float* nodeAreas);
        void precomputeWeightsGeoGaussEqual(const SurfaceFile* mySurf, float myKernel, const float* nodeAreas);
        void precomputeWeightsROIGeoGaussEqual(const SurfaceFile* mySurf, float myKernel, const MetricFile* theRoi, const float* nodeAreas);
        MetricSmoothingObject();
    };
    
}

#endif //__METRIC_SMOOTHING_OBJECT_H__
