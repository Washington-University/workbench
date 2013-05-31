#ifndef __ALGORITHM_CIFTI_SEPARATE_H__
#define __ALGORITHM_CIFTI_SEPARATE_H__

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

#include "AbstractAlgorithm.h"
#include "StructureEnum.h"

#include <vector>

namespace caret {
    
    class AlgorithmCiftiSeparate : public AbstractAlgorithm
    {
        AlgorithmCiftiSeparate();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               const StructureEnum::Enum& myStruct, MetricFile* metricOut, MetricFile* roiOut = NULL);
        AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               const StructureEnum::Enum& myStruct, LabelFile* labelOut, MetricFile* roiOut = NULL);
        AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               const StructureEnum::Enum& myStruct, VolumeFile* volOut, int64_t offsetOut[3],
                                               VolumeFile* roiOut = NULL, const bool& cropVol = true);
        AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const int& myDir,
                                               VolumeFile* volOut, int64_t offsetOut[3], VolumeFile* roiOut = NULL, const bool& cropVol = true);
        static void getCroppedVolSpace(const CiftiFile* ciftiIn, const int& myDir, const StructureEnum::Enum& myStruct, int64_t dimsOut[3],
                                       std::vector<std::vector<float> >& sformOut, int64_t offsetOut[3]);
        static void getCroppedVolSpaceAll(const CiftiFile* ciftiIn, const int& myDir, int64_t dimsOut[3],
                                       std::vector<std::vector<float> >& sformOut, int64_t offsetOut[3]);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiSeparate> AutoAlgorithmCiftiSeparate;

}

#endif //__ALGORITHM_CIFTI_SEPARATE_H__
