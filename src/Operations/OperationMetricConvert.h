#ifndef __OPERATION_METRIC_CONVERT_H__
#define __OPERATION_METRIC_CONVERT_H__

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

OperationMetricNiftiConvert     : operation name, in CamelCase, with initial capital, same as what you saved the header file to
OPERATION_METRIC_NIFTI_CONVERT    : uppercase of operation name, with underscore between words, used in #ifdef guards
-metric-nifti-convert   : switch for the command line to use, often hyphenated version of operation name, lowercase, minus "operation"
CONVERT METRIC FILE TO NIFTI FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make OperationMetricNiftiConvert.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationMetricNiftiConvert/g' | sed 's/-[c]ommand-switch/-metric-nifti-convert/g' | sed 's/[S]HORT DESCRIPTION/CONVERT METRIC FILE TO NIFTI FILE/g' > OperationMetricNiftiConvert.cxx
cat Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationMetricNiftiConvert/g' | sed 's/-[c]ommand-switch/-metric-nifti-convert/g' | sed 's/[S]HORT DESCRIPTION/CONVERT METRIC FILE TO NIFTI FILE/g' > Operations/OperationMetricNiftiConvert.cxx
cat src/Operations/OperationTemplate.cxx.txt | sed 's/[O]perationName/OperationMetricNiftiConvert/g' | sed 's/-[c]ommand-switch/-metric-nifti-convert/g' | sed 's/[S]HORT DESCRIPTION/CONVERT METRIC FILE TO NIFTI FILE/g' > src/Operations/OperationMetricNiftiConvert.cxx

or manually copy and replace

next, implement its functions

add these to Operations/CMakeLists.txt:

OperationMetricConvert.h
OperationMetricConvert.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "OperationMetricConvert.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricConvert()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractOperation.h"

namespace caret {
    
    class OperationMetricConvert : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<OperationMetricConvert> AutoOperationMetricConvert;

}

#endif //__OPERATION_METRIC_CONVERT_H__
