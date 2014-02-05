#ifndef __ALGORITHM_LABEL_MODIFY_KEYS_H__
#define __ALGORITHM_LABEL_MODIFY_KEYS_H__

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

AlgorithmLabelModifyKeys     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_LABEL_MODIFY_KEYS    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-label-modify-keys   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
CHANGE KEY VALUES IN A LABEL FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmLabelModifyKeys.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmLabelModifyKeys/g' | sed 's/-[c]ommand-switch/-label-modify-keys/g' | sed 's/[S]HORT DESCRIPTION/CHANGE KEY VALUES IN A LABEL FILE/g' > AlgorithmLabelModifyKeys.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmLabelModifyKeys/g' | sed 's/-[c]ommand-switch/-label-modify-keys/g' | sed 's/[S]HORT DESCRIPTION/CHANGE KEY VALUES IN A LABEL FILE/g' > Algorithms/AlgorithmLabelModifyKeys.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmLabelModifyKeys/g' | sed 's/-[c]ommand-switch/-label-modify-keys/g' | sed 's|[S]HORT DESCRIPTION|CHANGE KEY VALUES IN A LABEL FILE|g' > src/Algorithms/AlgorithmLabelModifyKeys.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmLabelModifyKeys.h
AlgorithmLabelModifyKeys.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmLabelModifyKeys.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelModifyKeys()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

#include <map>

namespace caret {
    
    class AlgorithmLabelModifyKeys : public AbstractAlgorithm
    {
        AlgorithmLabelModifyKeys();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmLabelModifyKeys(ProgressObject* myProgObj, const LabelFile* labelIn, const std::map<int32_t, int32_t>& remap, LabelFile* labelOut, const int& column = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmLabelModifyKeys> AutoAlgorithmLabelModifyKeys;

}

#endif //__ALGORITHM_LABEL_MODIFY_KEYS_H__
