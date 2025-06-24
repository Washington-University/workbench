#ifndef __ALGORITHM_METRIC_INTERPOLATE_GAPS_H__
#define __ALGORITHM_METRIC_INTERPOLATE_GAPS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025  Washington University School of Medicine
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

#include "AlgorithmMetricDilate.h"

#include <vector>

namespace caret {
    
    class AlgorithmMetricInterpolateGaps : public AbstractAlgorithm
    {
        AlgorithmMetricInterpolateGaps();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricInterpolateGaps(ProgressObject* myProgObj, const std::vector<const MetricFile*> inputMetrics, const SurfaceFile* mySurf, float distLimit, MetricFile* myMetricOut,
                                                               const std::vector<const MetricFile*> badRois, const MetricFile* dataROI = NULL, const MetricFile* corrAreas = NULL,
                                                               const float dilExponent = 6.0f, const AlgorithmMetricDilate::Method dilMethod = AlgorithmMetricDilate::WEIGHTED);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmMetricInterpolateGaps> AutoAlgorithmMetricInterpolateGaps;

}

#endif //__ALGORITHM_METRIC_INTERPOLATE_GAPS_H__
