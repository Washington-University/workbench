#ifndef __OPERATION_CIFTI_CREATE_DENSE_FROM_TEMPLATE_H__
#define __OPERATION_CIFTI_CREATE_DENSE_FROM_TEMPLATE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

OperationCiftiCreateDenseFromTemplate     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_CIFTI_CREATE_DENSE_FROM_TEMPLATE    : uppercase of operation name, with underscore between words, used in #ifdef guards

next, make OperationCiftiCreateDenseFromTemplate.cxx from OperationTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiCreateDenseFromTemplate/g' > OperationCiftiCreateDenseFromTemplate.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiCreateDenseFromTemplate/g' > Operations/OperationCiftiCreateDenseFromTemplate.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiCreateDenseFromTemplate/g' > src/Operations/OperationCiftiCreateDenseFromTemplate.cxx

or manually copy and replace

next, implement its functions

add these to Operations/CMakeLists.txt:

OperationCiftiCreateDenseFromTemplate.h
OperationCiftiCreateDenseFromTemplate.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "OperationCiftiCreateDenseFromTemplate.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiCreateDenseFromTemplate()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationCiftiCreateDenseFromTemplate : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationCiftiCreateDenseFromTemplate> AutoOperationCiftiCreateDenseFromTemplate;

}

#endif //__OPERATION_CIFTI_CREATE_DENSE_FROM_TEMPLATE_H__
