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

/*
file->save as... and enter what you will name the class, plus .h

find and replace these strings, without matching "whole word only":

AlgorithmMetricSmoothing     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_METRIC_SMOOTHING    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-metric-smoothing   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
SMOOTH A METRIC FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmMetricSmoothing.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmMetricSmoothing/g' | sed 's/-[c]ommand-switch/-metric-smoothing/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH A METRIC FILE/g' > AlgorithmMetricSmoothing.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmMetricSmoothing/g' | sed 's/-[c]ommand-switch/-metric-smoothing/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH A METRIC FILE/g' > Algorithms/AlgorithmMetricSmoothing.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmMetricSmoothing/g' | sed 's/-[c]ommand-switch/-metric-smoothing/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH A METRIC FILE/g' > src/Algorithms/AlgorithmMetricSmoothing.cxx

or manually copy and replace, and implement its functions - the algorithm work goes in the CONSTRUCTOR

afterwards, place the following lines into CommandOperations/CommandOperationManager.cxx

#include "AlgorithmMetricSmoothing.h"        //near the top
this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricSmoothing()));          //in CommandOperationManager()

and add these into Algorithms/CMakeLists.txt

AlgorithmMetricSmoothing.h

AlgorithmMetricSmoothing.cxx

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"
#include "stdint.h"
#include <vector>

namespace caret {
    
    class AlgorithmMetricSmoothing : public AbstractAlgorithm
    {
        struct WeightList
        {
            std::vector<int32_t> m_nodes;
            std::vector<float> m_weights;
            float m_weightSum;
        };
        std::vector<WeightList> m_weightLists;
        AlgorithmMetricSmoothing();
        void precomputeWeights(SurfaceFile* mySurf, double myKernel);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricSmoothing(ProgressObject* myProgObj, SurfaceFile* mySurf, MetricFile* myMetric, MetricFile* myMetricOut, double myKernel, int64_t columnNum);
        static AlgorithmParameters* getParameters();
        static void useParameters(AlgorithmParameters*, ProgressObject*);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoAlgorithm<AlgorithmMetricSmoothing> AutoAlgorithmMetricSmoothing;

}

#endif //__ALGORITHM_METRIC_SMOOTHING_H__