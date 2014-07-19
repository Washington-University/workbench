#ifndef __ALGORITHM_METRIC_TFCE_H__
#define __ALGORITHM_METRIC_TFCE_H__

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
    
    class TopologyHelper;
    
    class AlgorithmMetricTFCE : public AbstractAlgorithm
    {
        AlgorithmMetricTFCE();
        void processColumn(const SurfaceFile* mySurf, const float* colData, float* outData, const float* roiData, const float& param_e, const float& param_h, const float* areaData);
        void tfce_pos(TopologyHelper* myHelper, const float* colData, double* accumData, const float* roiData, const float& param_e, const float& param_h, const float* areaData);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricTFCE(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, MetricFile* myMetricOut, const float& presmooth = 0.0f,
                            const MetricFile* myRoi = NULL, const float& param_e = 1.0f, const float& param_h = 2.0f, const int& columnNum = -1, const MetricFile* corrAreaMetric = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmMetricTFCE> AutoAlgorithmMetricTFCE;

}

#endif //__ALGORITHM_METRIC_TFCE_H__
