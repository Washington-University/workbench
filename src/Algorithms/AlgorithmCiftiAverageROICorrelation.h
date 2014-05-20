#ifndef __ALGORITHM_CIFTI_AVERAGE_ROI_CORRELATION_H__
#define __ALGORITHM_CIFTI_AVERAGE_ROI_CORRELATION_H__

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
    
    class AlgorithmCiftiAverageROICorrelation : public AbstractAlgorithm
    {
        AlgorithmCiftiAverageROICorrelation();
        void verifySurfaceComponent(const int& index, const CiftiFile* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi);
        void verifyVolumeComponent(const int& index, const CiftiFile* myCifti, const VolumeFile* volROI);
        void processCifti(const CiftiFile* myCifti, std::vector<std::vector<float> >& output, const MetricFile* leftROI, const MetricFile* rightROI, const MetricFile* cerebROI, const VolumeFile* volROI,
                          const int& numMaps, const float* leftAreas, const float* rightAreas, const float* cerebAreas);
        void processCifti(const CiftiFile* myCifti, std::vector<std::vector<float> >& output, const CiftiFile* ciftiROI,
                          const int& numMaps, const float* leftAreas, const float* rightAreas, const float* cerebAreas);
        void addSurface(const CiftiFile* myCifti, StructureEnum::Enum myStruct, std::vector<double>& accum, const MetricFile* myRoi, const int& myMap, const float* myAreas);
        void addVolume(const CiftiFile* myCifti, std::vector<double>& accum, const VolumeFile* myRoi, const int& myMap);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiAverageROICorrelation(ProgressObject* myProgObj, const std::vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut,
                                        const MetricFile* leftROI = NULL, const MetricFile* rightROI = NULL, const MetricFile* cerebROI = NULL, const VolumeFile* volROI = NULL,
                                        const SurfaceFile* leftAreaSurf = NULL, const SurfaceFile* rightAreaSurf = NULL, const SurfaceFile* cerebAreaSurf = NULL);
        AlgorithmCiftiAverageROICorrelation(ProgressObject* myProgObj, const std::vector<const CiftiFile*>& ciftiList, CiftiFile* ciftiOut, const CiftiFile* ciftiROI,
                                            const SurfaceFile* leftAreaSurf = NULL, const SurfaceFile* rightAreaSurf = NULL, const SurfaceFile* cerebAreaSurf = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiAverageROICorrelation> AutoAlgorithmCiftiAverageROICorrelation;

}

#endif //__ALGORITHM_CIFTI_AVERAGE_ROI_CORRELATION_H__
