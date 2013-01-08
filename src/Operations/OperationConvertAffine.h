#ifndef __OPERATION_CONVERT_AFFINE_H__
#define __OPERATION_CONVERT_AFFINE_H__

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

OperationConvertAffine     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_CONVERT_AFFINE    : uppercase of operation name, with underscore between words, used in #ifdef guards
-convert-affine   : switch for the command line to use, often hyphenated version of operation name, lowercase, minus "operation"
CONVERT AN AFFINE FILE BETWEEN CONVENTIONS : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make OperationConvertAffine.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationConvertAffine/g' | sed 's/-[c]ommand-switch/-convert-affine/g' | sed 's/[S]HORT DESCRIPTION/CONVERT AN AFFINE FILE BETWEEN CONVENTIONS/g' > OperationConvertAffine.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationConvertAffine/g' | sed 's/-[c]ommand-switch/-convert-affine/g' | sed 's/[S]HORT DESCRIPTION/CONVERT AN AFFINE FILE BETWEEN CONVENTIONS/g' > Operations/OperationConvertAffine.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationConvertAffine/g' | sed 's/-[c]ommand-switch/-convert-affine/g' | sed 's/[S]HORT DESCRIPTION/CONVERT AN AFFINE FILE BETWEEN CONVENTIONS/g' > src/Operations/OperationConvertAffine.cxx

or manually copy and replace

next, implement its functions

add these to Operations/CMakeLists.txt:

OperationConvertAffine.h
OperationConvertAffine.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "OperationConvertAffine.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertAffine()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationConvertAffine : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationConvertAffine> AutoOperationConvertAffine;

}

#endif //__OPERATION_CONVERT_AFFINE_H__
