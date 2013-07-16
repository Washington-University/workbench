#ifndef __OPERATION_WBSPARSE_MERGE_DENSE_H__
#define __OPERATION_WBSPARSE_MERGE_DENSE_H__

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

if the operation takes no mandatory parameters, uncomment the line below for takesParameters(), otherwise delete it

find and replace these strings, without matching "whole word only" (plain text mode):

OperationWbsparseMergeDense     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_WBSPARSE_MERGE_DENSE    : uppercase of operation name, with underscore between words, used in #ifdef guards
-wbsparse-merge-dense   : switch for the command line to use, often hyphenated version of operation name, lowercase, minus "operation"
MERGE WBSPARSE FILES ALONG DENSE DIMENSION : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make OperationWbsparseMergeDense.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationWbsparseMergeDense/g' | sed 's/-[c]ommand-switch/-wbsparse-merge-dense/g' | sed 's/[S]HORT DESCRIPTION/MERGE WBSPARSE FILES ALONG DENSE DIMENSION/g' > OperationWbsparseMergeDense.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationWbsparseMergeDense/g' | sed 's/-[c]ommand-switch/-wbsparse-merge-dense/g' | sed 's/[S]HORT DESCRIPTION/MERGE WBSPARSE FILES ALONG DENSE DIMENSION/g' > Operations/OperationWbsparseMergeDense.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationWbsparseMergeDense/g' | sed 's/-[c]ommand-switch/-wbsparse-merge-dense/g' | sed 's/[S]HORT DESCRIPTION/MERGE WBSPARSE FILES ALONG DENSE DIMENSION/g' > src/Operations/OperationWbsparseMergeDense.cxx

or manually copy and replace

next, implement its functions

add these to Operations/CMakeLists.txt:

OperationWbsparseMergeDense.h
OperationWbsparseMergeDense.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "OperationWbsparseMergeDense.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoOperationWbsparseMergeDense()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationWbsparseMergeDense : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
        //static bool takesParameters() { return false; };
    };

    typedef TemplateAutoOperation<OperationWbsparseMergeDense> AutoOperationWbsparseMergeDense;

}

#endif //__OPERATION_WBSPARSE_MERGE_DENSE_H__
