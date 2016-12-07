#ifndef __ALGORITHM_CIFTI_LABEL_PROBABILITY_H__
#define __ALGORITHM_CIFTI_LABEL_PROBABILITY_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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
file->save as... and enter what you will name the class, plus .h

find and replace these strings in plain text mode (not "whole word only"):

AlgorithmCiftiLabelProbability     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_CIFTI_LABEL_PROBABILITY    : uppercase of algorithm name, with underscore between words, used in #ifdef guards

next, make AlgorithmCiftiLabelProbability.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiLabelProbability/g' > AlgorithmCiftiLabelProbability.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiLabelProbability/g' > Algorithms/AlgorithmCiftiLabelProbability.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiLabelProbability/g' > src/Algorithms/AlgorithmCiftiLabelProbability.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmCiftiLabelProbability.h
AlgorithmCiftiLabelProbability.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmCiftiLabelProbability.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiLabelProbability()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmCiftiLabelProbability : public AbstractAlgorithm
    {
        AlgorithmCiftiLabelProbability();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiLabelProbability(ProgressObject* myProgObj, const CiftiFile* inputLabel, CiftiFile* outputCifti, const bool& excludeUnlabeled = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiLabelProbability> AutoAlgorithmCiftiLabelProbability;

}

#endif //__ALGORITHM_CIFTI_LABEL_PROBABILITY_H__
