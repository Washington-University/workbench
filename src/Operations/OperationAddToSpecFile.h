#ifndef __OPERATION_ADD_TO_SPEC_FILE_H__
#define __OPERATION_ADD_TO_SPEC_FILE_H__

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

OperationAddToSpecFile     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_ADD_TO_SPEC_FILE    : uppercase of operation name, with underscore between words, used in #ifdef guards
-add-to-spec-file   : switch for the command line to use, often hyphenated version of operation name, lowercase, minus "operation"
ADD A FILE TO A SPECIFICATION FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

if the operation takes no parameters, uncomment the line below for takesParameters(), otherwise delete it

next, make OperationAddToSpecFile.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationAddToSpecFile/g' | sed 's/-[c]ommand-switch/-add-to-spec-file/g' | sed 's/[S]HORT DESCRIPTION/ADD A FILE TO A SPECIFICATION FILE/g' > OperationAddToSpecFile.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationAddToSpecFile/g' | sed 's/-[c]ommand-switch/-add-to-spec-file/g' | sed 's/[S]HORT DESCRIPTION/ADD A FILE TO A SPECIFICATION FILE/g' > Operations/OperationAddToSpecFile.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationAddToSpecFile/g' | sed 's/-[c]ommand-switch/-add-to-spec-file/g' | sed 's/[S]HORT DESCRIPTION/ADD A FILE TO A SPECIFICATION FILE/g' > src/Operations/OperationAddToSpecFile.cxx

or manually copy and replace, and implement its functions

afterwards, place the following lines into Commands/CommandOperationManager.cxx

#include "OperationAddToSpecFile.h"        //near the top
this->commandOperations.push_back(new CommandParser(new AutoOperationAddToSpecFile()));          //in CommandOperationManager()

and add these into Operations/CMakeLists.txt

OperationAddToSpecFile.h

OperationAddToSpecFile.cxx

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationAddToSpecFile : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters*, ProgressObject*);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationAddToSpecFile> AutoOperationAddToSpecFile;

}

#endif //__OPERATION_ADD_TO_SPEC_FILE_H__