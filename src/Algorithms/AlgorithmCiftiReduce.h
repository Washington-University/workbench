#ifndef __ALGORITHM_CIFTI_REDUCE_H__
#define __ALGORITHM_CIFTI_REDUCE_H__

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

AlgorithmCiftiReduce     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_CIFTI_REDUCE    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-cifti-reduce   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
PERFORM REDUCTION OPERATION ALONG CIFTI ROWS : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmCiftiReduce.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiReduce/g' | sed 's/-[c]ommand-switch/-cifti-reduce/g' | sed 's/[S]HORT DESCRIPTION/PERFORM REDUCTION OPERATION ALONG CIFTI ROWS/g' > AlgorithmCiftiReduce.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiReduce/g' | sed 's/-[c]ommand-switch/-cifti-reduce/g' | sed 's/[S]HORT DESCRIPTION/PERFORM REDUCTION OPERATION ALONG CIFTI ROWS/g' > Algorithms/AlgorithmCiftiReduce.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiReduce/g' | sed 's/-[c]ommand-switch/-cifti-reduce/g' | sed 's/[S]HORT DESCRIPTION/PERFORM REDUCTION OPERATION ALONG CIFTI ROWS/g' > src/Algorithms/AlgorithmCiftiReduce.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmCiftiReduce.h
AlgorithmCiftiReduce.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmCiftiReduce.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiReduce()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"
#include "ReductionEnum.h"

namespace caret {
    
    class AlgorithmCiftiReduce : public AbstractAlgorithm
    {
        AlgorithmCiftiReduce();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiReduce(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const ReductionEnum::Enum& myReduce, CiftiFile* ciftiOut);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiReduce> AutoAlgorithmCiftiReduce;

}

#endif //__ALGORITHM_CIFTI_REDUCE_H__
