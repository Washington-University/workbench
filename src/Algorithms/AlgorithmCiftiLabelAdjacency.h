#ifndef __ALGORITHM_CIFTI_LABEL_ADJACENCY_H__
#define __ALGORITHM_CIFTI_LABEL_ADJACENCY_H__

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

/*
file->save as... and enter what you will name the class, plus .h

find and replace these strings, without matching "whole word only" (plain text mode):

AlgorithmCiftiLabelAdjacency     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_CIFTI_LABEL_ADJACENCY    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-cifti-label-adjacency   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
MAKE ADJACENCY MATRIX OF A CIFTI LABEL FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmCiftiLabelAdjacency.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiLabelAdjacency/g' | sed 's/-[c]ommand-switch/-cifti-label-adjacency/g' | sed 's/[S]HORT DESCRIPTION/MAKE ADJACENCY MATRIX OF A CIFTI LABEL FILE/g' > AlgorithmCiftiLabelAdjacency.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiLabelAdjacency/g' | sed 's/-[c]ommand-switch/-cifti-label-adjacency/g' | sed 's/[S]HORT DESCRIPTION/MAKE ADJACENCY MATRIX OF A CIFTI LABEL FILE/g' > Algorithms/AlgorithmCiftiLabelAdjacency.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiLabelAdjacency/g' | sed 's/-[c]ommand-switch/-cifti-label-adjacency/g' | sed 's|[S]HORT DESCRIPTION|MAKE ADJACENCY MATRIX OF A CIFTI LABEL FILE|g' > src/Algorithms/AlgorithmCiftiLabelAdjacency.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmCiftiLabelAdjacency.h
AlgorithmCiftiLabelAdjacency.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmCiftiLabelAdjacency.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiLabelAdjacency()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class CiftiInterface;
    
    class AlgorithmCiftiLabelAdjacency : public AbstractAlgorithm
    {
        AlgorithmCiftiLabelAdjacency();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiLabelAdjacency(ProgressObject* myProgObj, const CiftiInterface* myLabelIn, CiftiFile* myAdjOut,
                                     const SurfaceFile* myLeftSurf = NULL, const SurfaceFile* myRightSurf = NULL, const SurfaceFile* myCerebSurf = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiLabelAdjacency> AutoAlgorithmCiftiLabelAdjacency;

}

#endif //__ALGORITHM_CIFTI_LABEL_ADJACENCY_H__
