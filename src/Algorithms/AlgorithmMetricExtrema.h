#ifndef __ALGORITHM_METRIC_EXTREMA_H__
#define __ALGORITHM_METRIC_EXTREMA_H__

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
#include <utility>
#include <vector>

namespace caret {
    
    class AlgorithmMetricExtrema : public AbstractAlgorithm
    {
        void precomputeNeighborhoods(const SurfaceFile* mySurf, const float& distance, std::vector<std::vector<int32_t> >& neighborhoods);
        void findMinimaConsolidate(const SurfaceFile* mySurf, const float* data, const float& distance, std::vector<int>& minima, std::vector<int>& maxima);
        void findMinimaNeighborhoods(const float* data, const std::vector<std::vector<int32_t> >& neighborhoods, std::vector<int>& minima, std::vector<int>& maxima);
        void findMinimaConsolidate(const SurfaceFile* mySurf, const float* data, const float& distance, const float& lowThresh, const float& highThresh, std::vector<int>& minima, std::vector<int>& maxima);
        void findMinimaNeighborhoods(const float* data, const std::vector<std::vector<int32_t> >& neighborhoods, const float& lowThresh, const float& highThresh, std::vector<int>& minima, std::vector<int>& maxima);
        void consolidateStep(const SurfaceFile* mySurf, const float& distance, std::vector<std::pair<int, int> > initExtrema[2], std::vector<int>& minima, std::vector<int>& maxima);
        AlgorithmMetricExtrema();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricExtrema(ProgressObject* myProgObj, const SurfaceFile* mySurf,const MetricFile* myMetric, const float& distance,
                                               MetricFile* myMetricOut, const float& presmooth = -1.0f, const bool& sumColumns = false,
                                               const bool& consolidateMode = false, const int& columnNum = -1);
        AlgorithmMetricExtrema(ProgressObject* myProgObj, const SurfaceFile* mySurf,const MetricFile* myMetric, const float& distance,
                                               MetricFile* myMetricOut, const float& lowThresh, const float& highThresh, const float& presmooth = -1.0f,
                                               const bool& sumColumns = false, const bool& consolidateMode = false, const int& columnNum = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmMetricExtrema> AutoAlgorithmMetricExtrema;

}

#endif //__ALGORITHM_METRIC_EXTREMA_H__
