#ifndef __ALGORITHM_CIFTI_CREATE_DENSE_SCALAR_H__
#define __ALGORITHM_CIFTI_CREATE_DENSE_SCALAR_H__

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

#include "AlgorithmCiftiCreateDenseTimeseries.h"

namespace caret {
    
    class AlgorithmCiftiCreateDenseScalar : public AbstractAlgorithm
    {
        AlgorithmCiftiCreateDenseScalar();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        typedef AlgorithmCiftiCreateDenseTimeseries::SurfParam SurfParam; //the "make dense map" code is in that algorithm at the moment, so typedef this for convenience
        AlgorithmCiftiCreateDenseScalar(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol, const VolumeFile* myVolLabel,
                                        const MetricFile* leftData, const MetricFile* leftRoi = NULL,
                                        const MetricFile* rightData = NULL, const MetricFile* rightRoi = NULL,
                                        const MetricFile* cerebData = NULL, const MetricFile* cerebRoi = NULL,
                                        const std::vector<AString>* namePtr = NULL);
        AlgorithmCiftiCreateDenseScalar(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol = NULL, const VolumeFile* myVolLabel = NULL,
                                        const std::map<StructureEnum::Enum, SurfParam> surfParams = std::map<StructureEnum::Enum, SurfParam>(),
                                        const std::vector<AString>* namePtr = NULL);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCreateDenseScalar> AutoAlgorithmCiftiCreateDenseScalar;

}

#endif //__ALGORITHM_CIFTI_CREATE_DENSE_SCALAR_H__
