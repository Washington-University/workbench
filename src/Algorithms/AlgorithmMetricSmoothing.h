#ifndef __ALGORITHM_METRIC_SMOOTHING_H__
#define __ALGORITHM_METRIC_SMOOTHING_H__

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
#include "stdint.h"
#include <vector>

namespace caret {
    
    class AlgorithmMetricSmoothing : public AbstractAlgorithm
    {
    public:
        enum Method
        {
            GEO_GAUSS_AREA,
            GEO_GAUSS
        };
    private:
        struct WeightList
        {
            std::vector<int32_t> m_nodes;
            std::vector<float> m_weights;
            float m_weightSum;
        };
        std::vector<WeightList> m_weightLists;
        AlgorithmMetricSmoothing();
        void precomputeWeights(SurfaceFile* mySurf, double myKernel, MetricFile* theRoi, Method myMethod);
        void precomputeWeightsGeoGauss(SurfaceFile* mySurf, double myKernel);
        void precomputeWeightsROIGeoGauss(SurfaceFile* mySurf, double myKernel, MetricFile* theRoi);
        void precomputeWeightsGeoGaussArea(SurfaceFile* mySurf, double myKernel);
        void precomputeWeightsROIGeoGaussArea(SurfaceFile* mySurf, double myKernel, MetricFile* theRoi);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricSmoothing(ProgressObject* myProgObj, SurfaceFile* mySurf, MetricFile* myMetric, double myKernel, MetricFile* myMetricOut, MetricFile* myRoi = NULL, int64_t columnNum = -1, Method myMethod = GEO_GAUSS_AREA);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmMetricSmoothing> AutoAlgorithmMetricSmoothing;

}

#endif //__ALGORITHM_METRIC_SMOOTHING_H__
