#ifndef __OPERATION_CIFTI_CHANGE_MAPPING_H__
#define __OPERATION_CIFTI_CHANGE_MAPPING_H__

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

OperationCiftiChangeMapping     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_CIFTI_CHANGE_MAPPING    : uppercase of operation name, with underscore between words, used in #ifdef guards

next, make OperationCiftiChangeMapping.cxx from OperationTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiChangeMapping/g' > OperationCiftiChangeMapping.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiChangeMapping/g' > Operations/OperationCiftiChangeMapping.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiChangeMapping/g' > src/Operations/OperationCiftiChangeMapping.cxx

or manually copy and replace

next, implement its functions

add these to Operations/CMakeLists.txt:

OperationCiftiChangeMapping.h
OperationCiftiChangeMapping.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "OperationCiftiChangeMapping.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiChangeMapping()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationCiftiChangeMapping : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationCiftiChangeMapping> AutoOperationCiftiChangeMapping;

}

#endif //__OPERATION_CIFTI_CHANGE_MAPPING_H__
