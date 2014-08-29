#ifndef __ALGORITHM_METRIC_GRADIENT_H__
#define __ALGORITHM_METRIC_GRADIENT_H__

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

namespace caret {
    
    class AlgorithmMetricGradient : public AbstractAlgorithm
    {
        AlgorithmMetricGradient();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricGradient(ProgressObject* myProgObj,
                                SurfaceFile* mySurf,
                                const MetricFile* myMetricIn,
                                MetricFile* myMetricOut,
                                MetricFile* myVectorsOut = NULL,
                                const float myPresmooth = -1.0f,
                                const MetricFile* myRoi = NULL,
                                const bool myAvgNormals = false,
                                const int32_t myColumn = -1,
                                const MetricFile* corrAreaMetric = NULL,
                                bool matchRoiColumns = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmMetricGradient> AutoAlgorithmMetricGradient;

}

#endif //__ALGORITHM_METRIC_GRADIENT_H__
