#ifndef __ALGORITHM_CIFTI_CREATE_LABEL_H__
#define __ALGORITHM_CIFTI_CREATE_LABEL_H__

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

#include "AbstractAlgorithm.h"

#include "StructureEnum.h"

#include <map>

namespace caret {
    
    class AlgorithmCiftiCreateLabel : public AbstractAlgorithm
    {
        AlgorithmCiftiCreateLabel();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        struct SurfParam
        {
            const LabelFile* data;
            const MetricFile* roi;
            SurfParam() { data = NULL; roi = NULL; }
            SurfParam(const LabelFile* dataIn, const MetricFile* roiIn = NULL) { data = dataIn; roi = roiIn; }
        };
        AlgorithmCiftiCreateLabel(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                  const LabelFile* leftData, const MetricFile* leftRoi = NULL,
                                  const LabelFile* rightData = NULL, const MetricFile* rightRoi = NULL,
                                  const LabelFile* cerebData = NULL, const MetricFile* cerebRoi = NULL);
        AlgorithmCiftiCreateLabel(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol = NULL, const VolumeFile* myVolLabel = NULL,
                                  const std::map<StructureEnum::Enum, SurfParam> surfParams = std::map<StructureEnum::Enum, AlgorithmCiftiCreateLabel::SurfParam>());
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCreateLabel> AutoAlgorithmCiftiCreateLabel;

}

#endif //__ALGORITHM_CIFTI_CREATE_LABEL_H__
