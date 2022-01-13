#ifndef __ALGORITHM_LABEL_RESAMPLE_H__
#define __ALGORITHM_LABEL_RESAMPLE_H__

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
#include "SurfaceResamplingMethodEnum.h"

namespace caret {
    
    class AlgorithmLabelResample : public AbstractAlgorithm
    {
        AlgorithmLabelResample();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmLabelResample(ProgressObject* myProgObj, const LabelFile* labelIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                               const SurfaceResamplingMethodEnum::Enum& myMethod, LabelFile* labelOut, const MetricFile* curAreas = NULL,
                               const MetricFile* newAreas = NULL, const MetricFile* currentRoi = NULL, MetricFile* validRoiOut = NULL,
                               const bool largest = false, const bool allowNonSphere = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmLabelResample> AutoAlgorithmLabelResample;

}

#endif //__ALGORITHM_LABEL_RESAMPLE_H__
