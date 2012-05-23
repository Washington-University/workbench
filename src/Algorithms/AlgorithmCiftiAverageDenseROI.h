#ifndef __ALGORITHM_CIFTI_AVERAGE_DENSE_ROI_H__
#define __ALGORITHM_CIFTI_AVERAGE_DENSE_ROI_H__

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

AlgorithmCiftiAverageDenseROI     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_CIFTI_AVERAGE_DENSE_ROI    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-cifti-average-dense-roi   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
AVERAGE CIFTI ROWS ACROSS SUBJECTS BY ROI : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmCiftiAverageDenseROI.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiAverageDenseROI/g' | sed 's/-[c]ommand-switch/-cifti-average-dense-roi/g' | sed 's/[S]HORT DESCRIPTION/AVERAGE CIFTI ROWS ACROSS SUBJECTS BY ROI/g' > AlgorithmCiftiAverageDenseROI.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiAverageDenseROI/g' | sed 's/-[c]ommand-switch/-cifti-average-dense-roi/g' | sed 's/[S]HORT DESCRIPTION/AVERAGE CIFTI ROWS ACROSS SUBJECTS BY ROI/g' > Algorithms/AlgorithmCiftiAverageDenseROI.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiAverageDenseROI/g' | sed 's/-[c]ommand-switch/-cifti-average-dense-roi/g' | sed 's/[S]HORT DESCRIPTION/AVERAGE CIFTI ROWS ACROSS SUBJECTS BY ROI/g' > src/Algorithms/AlgorithmCiftiAverageDenseROI.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmCiftiAverageDenseROI.h
AlgorithmCiftiAverageDenseROI.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmCiftiAverageDenseROI.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAverageDenseROI()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"
#include "StructureEnum.h"

#include <vector>

namespace caret {
    
    class CiftiInterface;
    
    class AlgorithmCiftiAverageDenseROI : public AbstractAlgorithm
    {
        AlgorithmCiftiAverageDenseROI();
        void verifySurfaceComponent(const int& index, const CiftiInterface* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi);
        void processSurfaceComponent(std::vector<double>& accum, int64_t& accumCount, const CiftiInterface* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi);
        void verifyVolumeComponent(const int& index, const CiftiInterface* myCifti, const VolumeFile* volROI);
        void processVolumeComponent(std::vector<double>& accum, int64_t& accumCount, const CiftiInterface* myCifti, const VolumeFile* volROI);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiAverageDenseROI(ProgressObject* myProgObj, const std::vector<const CiftiInterface*>& ciftiList, CiftiFile* ciftiOut,
                                        const MetricFile* leftROI = NULL, const MetricFile* rightROI = NULL, const MetricFile* cerebROI = NULL, const VolumeFile* volROI = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiAverageDenseROI> AutoAlgorithmCiftiAverageDenseROI;

}

#endif //__ALGORITHM_CIFTI_AVERAGE_DENSE_ROI_H__
