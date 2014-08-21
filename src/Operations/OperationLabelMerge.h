#ifndef __OPERATION_LABEL_MERGE_H__
#define __OPERATION_LABEL_MERGE_H__

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

OperationLabelMerge     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_LABEL_MERGE    : uppercase of operation name, with underscore between words, used in #ifdef guards

next, make OperationLabelMerge.cxx from OperationTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationLabelMerge/g' > OperationLabelMerge.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationLabelMerge/g' > Operations/OperationLabelMerge.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationLabelMerge/g' > src/Operations/OperationLabelMerge.cxx

or manually copy and replace

next, implement its functions

add these to Operations/CMakeLists.txt:

OperationLabelMerge.h
OperationLabelMerge.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "OperationLabelMerge.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoOperationLabelMerge()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationLabelMerge : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationLabelMerge> AutoOperationLabelMerge;

}

#endif //__OPERATION_LABEL_MERGE_H__
