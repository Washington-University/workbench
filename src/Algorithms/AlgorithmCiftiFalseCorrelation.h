#ifndef __ALGORITHM_CIFTI_FALSE_CORRELATION_H__
#define __ALGORITHM_CIFTI_FALSE_CORRELATION_H__

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

AlgorithmCiftiFalseCorrelation     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_CIFTI_FALSE_CORRELATION    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-cifti-false-correlation   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
COMPARE CORRELATION LOCALLY AND ACROSS/THROUGH SULCI/GYRI : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmCiftiFalseCorrelation.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiFalseCorrelation/g' | sed 's/-[c]ommand-switch/-cifti-false-correlation/g' | sed 's/[S]HORT DESCRIPTION/COMPARE CORRELATION LOCALLY AND ACROSS/THROUGH SULCI/GYRI/g' > AlgorithmCiftiFalseCorrelation.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiFalseCorrelation/g' | sed 's/-[c]ommand-switch/-cifti-false-correlation/g' | sed 's/[S]HORT DESCRIPTION/COMPARE CORRELATION LOCALLY AND ACROSS/THROUGH SULCI/GYRI/g' > Algorithms/AlgorithmCiftiFalseCorrelation.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiFalseCorrelation/g' | sed 's/-[c]ommand-switch/-cifti-false-correlation/g' | sed 's|[S]HORT DESCRIPTION|COMPARE CORRELATION LOCALLY AND ACROSS/THROUGH SULCI/GYRI|g' > src/Algorithms/AlgorithmCiftiFalseCorrelation.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmCiftiFalseCorrelation.h
AlgorithmCiftiFalseCorrelation.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmCiftiFalseCorrelation.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiFalseCorrelation()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class CiftiInterface;
    
    class AlgorithmCiftiFalseCorrelation : public AbstractAlgorithm
    {
        AlgorithmCiftiFalseCorrelation();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiFalseCorrelation(ProgressObject* myProgObj, const CiftiInterface* myCiftiIn, const float& max3D, const float& maxgeo, const float& mingeo,
                                       CiftiFile* myCiftiOut, const SurfaceFile* myLeftSurf, const AString& leftDumpName,
                                       const SurfaceFile* myRightSurf, const AString& rightDumpName, const SurfaceFile* myCerebSurf, const AString& cerebDumpName);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiFalseCorrelation> AutoAlgorithmCiftiFalseCorrelation;

}

#endif //__ALGORITHM_CIFTI_FALSE_CORRELATION_H__
