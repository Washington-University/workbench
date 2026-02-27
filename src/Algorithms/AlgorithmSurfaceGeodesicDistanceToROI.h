#ifndef __ALGORITHM_SURCACE_GEODESIC_DISTANCE_TO_ROI_H__
#define __ALGORITHM_SURCACE_GEODESIC_DISTANCE_TO_ROI_H__

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

/*
INSTRUCTIONS:
file->save as... and enter what you will name the class, plus .h

find and replace these strings in plain text mode (not "whole word only"):

AlgorithmSurfaceGeodesicDistanceToROI     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_SURCACE_GEODESIC_DISTANCE_TO_ROI    : uppercase of algorithm name, with underscore between words, used in #ifdef guards

next, make AlgorithmSurfaceGeodesicDistanceToROI.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmSurfaceGeodesicDistanceToROI/g' > AlgorithmSurfaceGeodesicDistanceToROI.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmSurfaceGeodesicDistanceToROI/g' > Algorithms/AlgorithmSurfaceGeodesicDistanceToROI.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmSurfaceGeodesicDistanceToROI/g' > src/Algorithms/AlgorithmSurfaceGeodesicDistanceToROI.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmSurfaceGeodesicDistanceToROI.h
AlgorithmSurfaceGeodesicDistanceToROI.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmSurfaceGeodesicDistanceToROI.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceGeodesicDistanceToROI()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmSurfaceGeodesicDistanceToROI : public AbstractAlgorithm
    {
        AlgorithmSurfaceGeodesicDistanceToROI();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmSurfaceGeodesicDistanceToROI(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myROI, MetricFile* myMetricOut,
                                              const int32_t column = -1, const bool negatives = false, const bool naive = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmSurfaceGeodesicDistanceToROI> AutoAlgorithmSurfaceGeodesicDistanceToROI;

}

#endif //__ALGORITHM_SURCACE_GEODESIC_DISTANCE_TO_ROI_H__
