#ifndef __ALGORITHM_LABEL_TO_VOLUME_MAPPING_H__
#define __ALGORITHM_LABEL_TO_VOLUME_MAPPING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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
    
    class VolumeSpace;
    
    class AlgorithmLabelToVolumeMapping : public AbstractAlgorithm
    {
        AlgorithmLabelToVolumeMapping();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmLabelToVolumeMapping(ProgressObject* myProgObj, const LabelFile* myLabel, const SurfaceFile* mySurf, const VolumeSpace& myVolSpace,
                                      VolumeFile* myVolOut, const float& nearDist);
        AlgorithmLabelToVolumeMapping(ProgressObject* myProgObj, const LabelFile* myLabel, const SurfaceFile* mySurf, const VolumeSpace& myVolSpace,
                                      VolumeFile* myVolOut, const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int& subDivs = 3);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmLabelToVolumeMapping> AutoAlgorithmLabelToVolumeMapping;

}

#endif //__ALGORITHM_LABEL_TO_VOLUME_MAPPING_H__
