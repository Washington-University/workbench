#ifndef __ALGORITHM_SURFACE_PAIRS_INTERIOR_DISTANCE_H__
#define __ALGORITHM_SURFACE_PAIRS_INTERIOR_DISTANCE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026  Washington University School of Medicine
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

#include "VolumeSpace.h"

#include <cmath>
#include <map>

namespace caret {
    
    class CaretSparseFileWriter;
    
    class AlgorithmSurfacePairsInteriorDistance : public AbstractAlgorithm
    {
        AlgorithmSurfacePairsInteriorDistance();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmSurfacePairsInteriorDistance(ProgressObject* myProgObj, const SurfaceFile* mySurf, const std::map<int32_t, int32_t> pairs,
                                              const VolumeSpace refVol, MetricFile* myMetricOut,
                                              const float failVal = INFINITY, const int32_t numNeigh = 2, const float offset = 0.001f,
                                              CaretSparseFileWriter* pathsOut = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmSurfacePairsInteriorDistance> AutoAlgorithmSurfacePairsInteriorDistance;

}

#endif //__ALGORITHM_SURFACE_PAIRS_INTERIOR_DISTANCE_H__
