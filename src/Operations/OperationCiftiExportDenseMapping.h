#ifndef __OPERATION_CIFTI_EXPORT_DENSE_MAPPING_H__
#define __OPERATION_CIFTI_EXPORT_DENSE_MAPPING_H__

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

OperationCiftiExportDenseMapping     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_CIFTI_EXPORT_DENSE_MAPPING    : uppercase of operation name, with underscore between words, used in #ifdef guards
-cifti-export-dense-mapping   : switch for the command line to use, often hyphenated version of operation name, lowercase, minus "operation"
WRITE INDEX TO ELEMENT MAPPING AS TEXT : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make OperationCiftiExportDenseMapping.cxx from OperationTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiExportDenseMapping/g' | sed 's/-[c]ommand-switch/-cifti-export-dense-mapping/g' | sed 's/[S]HORT DESCRIPTION/WRITE INDEX TO ELEMENT MAPPING AS TEXT/g' > OperationCiftiExportDenseMapping.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiExportDenseMapping/g' | sed 's/-[c]ommand-switch/-cifti-export-dense-mapping/g' | sed 's/[S]HORT DESCRIPTION/WRITE INDEX TO ELEMENT MAPPING AS TEXT/g' > Operations/OperationCiftiExportDenseMapping.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationCiftiExportDenseMapping/g' | sed 's/-[c]ommand-switch/-cifti-export-dense-mapping/g' | sed 's|[S]HORT DESCRIPTION|WRITE INDEX TO ELEMENT MAPPING AS TEXT|g' > src/Operations/OperationCiftiExportDenseMapping.cxx

or manually copy and replace

next, implement its functions

add these to Operations/CMakeLists.txt:

OperationCiftiExportDenseMapping.h
OperationCiftiExportDenseMapping.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "OperationCiftiExportDenseMapping.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiExportDenseMapping()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationCiftiExportDenseMapping : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationCiftiExportDenseMapping> AutoOperationCiftiExportDenseMapping;

}

#endif //__OPERATION_CIFTI_EXPORT_DENSE_MAPPING_H__
