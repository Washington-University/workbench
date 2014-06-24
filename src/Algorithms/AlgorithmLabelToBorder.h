#ifndef __ALGORITHM_LABEL_TO_BORDER_H__
#define __ALGORITHM_LABEL_TO_BORDER_H__

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

AlgorithmLabelToBorder     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_LABEL_TO_BORDER    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-label-to-border   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
DRAW BORDERS AROUND LABELS : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmLabelToBorder.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmLabelToBorder/g' | sed 's/-[c]ommand-switch/-label-to-border/g' | sed 's/[S]HORT DESCRIPTION/DRAW BORDERS AROUND LABELS/g' > AlgorithmLabelToBorder.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmLabelToBorder/g' | sed 's/-[c]ommand-switch/-label-to-border/g' | sed 's/[S]HORT DESCRIPTION/DRAW BORDERS AROUND LABELS/g' > Algorithms/AlgorithmLabelToBorder.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmLabelToBorder/g' | sed 's/-[c]ommand-switch/-label-to-border/g' | sed 's|[S]HORT DESCRIPTION|DRAW BORDERS AROUND LABELS|g' > src/Algorithms/AlgorithmLabelToBorder.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmLabelToBorder.h
AlgorithmLabelToBorder.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmLabelToBorder.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelToBorder()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmLabelToBorder : public AbstractAlgorithm
    {
        AlgorithmLabelToBorder();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmLabelToBorder(ProgressObject* myProgObj, const SurfaceFile* mySurf, const LabelFile* myLabel, BorderFile* myBorderOut,
                               const float& placement = 0.33f, const int& columnNum = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmLabelToBorder> AutoAlgorithmLabelToBorder;

}

#endif //__ALGORITHM_LABEL_TO_BORDER_H__
