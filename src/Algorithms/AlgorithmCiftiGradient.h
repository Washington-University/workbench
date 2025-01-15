#ifndef __ALGORITHM_CIFTI_GRADIENT_H__
#define __ALGORITHM_CIFTI_GRADIENT_H__

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

namespace caret {
    
    class AlgorithmCiftiGradient : public AbstractAlgorithm
    {
        AlgorithmCiftiGradient();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        struct SurfParam
        {
            SurfaceFile* surface;
            const MetricFile* correctedAreas;
            SurfParam() { surface = NULL; correctedAreas = NULL; }
            SurfParam(SurfaceFile* surfIn, const MetricFile* areasIn = NULL) { surface = surfIn; correctedAreas = areasIn; }
        };

        AlgorithmCiftiGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir, CiftiFile* myCiftiOut,
                               const float& surfKern = -1.0f, const float& volKern = -1.0f,
                               const std::map<StructureEnum::Enum, AlgorithmCiftiGradient::SurfParam> surfParams = std::map<StructureEnum::Enum, AlgorithmCiftiGradient::SurfParam>(),
                               bool outputAverage = false,
                               CiftiFile* ciftiVectorsOut = NULL);
        AlgorithmCiftiGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir, CiftiFile* myCiftiOut,
                               const float& surfKern, const float& volKern,
                               SurfaceFile* myLeftSurf, SurfaceFile* myRightSurf = NULL, SurfaceFile* myCerebSurf = NULL,
                               bool outputAverage = false,
                               const MetricFile* myLeftAreas = NULL, const MetricFile* myRightAreas = NULL, const MetricFile* myCerebAreas = NULL,
                               CiftiFile* ciftiVectorsOut = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiGradient> AutoAlgorithmCiftiGradient;

}

#endif //__ALGORITHM_CIFTI_GRADIENT_H__
