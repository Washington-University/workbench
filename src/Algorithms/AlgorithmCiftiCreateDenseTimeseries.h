#ifndef __ALGORITHM_CIFTI_CREATE_DENSE_TIMESERIES_H__
#define __ALGORITHM_CIFTI_CREATE_DENSE_TIMESERIES_H__

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

find and replace these strings, without matching "whole word only" (plain text mode):

AlgorithmCiftiCreateDenseTimeseries     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_CIFTI_CREATE_DENSE_TIMESERIES    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-cifti-create-dense-timeseries   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
CREATE A CIFTI DENSE TIMESERIES : short description of the command, uppercase, three to five words, often just command switch with more verbosity

if the algorithm takes no parameters (???) uncomment the line below for takesParameters(), otherwise delete it

next, make AlgorithmCiftiCreateDenseTimeseries.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiCreateDenseTimeseries/g' | sed 's/-[c]ommand-switch/-cifti-create-dense-timeseries/g' | sed 's/[S]HORT DESCRIPTION/CREATE A CIFTI DENSE TIMESERIES/g' > AlgorithmCiftiCreateDenseTimeseries.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiCreateDenseTimeseries/g' | sed 's/-[c]ommand-switch/-cifti-create-dense-timeseries/g' | sed 's/[S]HORT DESCRIPTION/CREATE A CIFTI DENSE TIMESERIES/g' > Algorithms/AlgorithmCiftiCreateDenseTimeseries.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiCreateDenseTimeseries/g' | sed 's/-[c]ommand-switch/-cifti-create-dense-timeseries/g' | sed 's/[S]HORT DESCRIPTION/CREATE A CIFTI DENSE TIMESERIES/g' > src/Algorithms/AlgorithmCiftiCreateDenseTimeseries.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmCiftiCreateDenseTimeseries.h
AlgorithmCiftiCreateDenseTimeseries.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmCiftiCreateDenseTimeseries.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCreateDenseTimeseries()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmCiftiCreateDenseTimeseries : public AbstractAlgorithm
    {
        AlgorithmCiftiCreateDenseTimeseries();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiCreateDenseTimeseries(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol = NULL,
                                                                         const VolumeFile* myVolLabel = NULL, const MetricFile* leftData = NULL, const MetricFile* leftRoi = NULL,
                                                                         const MetricFile* rightData = NULL, const MetricFile* rightRoi = NULL, const MetricFile* cerebData = NULL,
                                                                         const MetricFile* cerebRoi = NULL, const float& timestep = 1.0f);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCreateDenseTimeseries> AutoAlgorithmCiftiCreateDenseTimeseries;

}

#endif //__ALGORITHM_CIFTI_CREATE_DENSE_TIMESERIES_H__
