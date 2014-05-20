#ifndef __ALGORITHM_CIFTI_AVERAGE_DENSE_ROI_H__
#define __ALGORITHM_CIFTI_AVERAGE_DENSE_ROI_H__

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

#include <vector>

namespace caret {
    
    class AlgorithmCiftiAverageDenseROI : public AbstractAlgorithm
    {
        AlgorithmCiftiAverageDenseROI();
        void verifySurfaceComponent(const CiftiFile* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi);
        void processSurfaceComponent(std::vector<std::vector<double> >& accum, std::vector<double>& denom, const CiftiFile* myCifti,
                                     const StructureEnum::Enum& myStruct, const MetricFile* myRoi, const float* myAreas);
        void verifyVolumeComponent(const CiftiFile* myCifti, const VolumeFile* volROI);
        void processVolumeComponent(std::vector<std::vector<double> >& accum, std::vector<double>& denom, const CiftiFile* myCifti, const VolumeFile* volROI);
        void processCifti(std::vector<std::vector<double> >& accum, std::vector<double>& denom, const CiftiFile* myCifti, const CiftiFile* ciftiROI,
                                    const float* leftAreas, const float* rightAreas, const float* cerebAreas);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiAverageDenseROI(ProgressObject* myProgObj, const std::vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut, const CiftiFile* ciftiROI,
                                      const SurfaceFile* leftAreaSurf = NULL, const SurfaceFile* rightAreaSurf = NULL, const SurfaceFile* cerebAreaSurf = NULL);
        AlgorithmCiftiAverageDenseROI(ProgressObject* myProgObj, const std::vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut,
                                      const MetricFile* leftROI = NULL, const MetricFile* rightROI = NULL, const MetricFile* cerebROI = NULL, const VolumeFile* volROI = NULL,
                                      const SurfaceFile* leftAreaSurf = NULL, const SurfaceFile* rightAreaSurf = NULL, const SurfaceFile* cerebAreaSurf = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiAverageDenseROI> AutoAlgorithmCiftiAverageDenseROI;

}

#endif //__ALGORITHM_CIFTI_AVERAGE_DENSE_ROI_H__
