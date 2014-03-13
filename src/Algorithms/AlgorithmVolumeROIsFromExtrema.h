#ifndef __ALGORITHM_VOLUME_ROIS_FROM_EXTREMA_H__
#define __ALGORITHM_VOLUME_ROIS_FROM_EXTREMA_H__

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

#include "OverlapLogicEnum.h"
#include "VolumeSpace.h"

#include <map>
#include <vector>

namespace caret {
    
    class AlgorithmVolumeROIsFromExtrema : public AbstractAlgorithm
    {
        AlgorithmVolumeROIsFromExtrema();
        void processFrame(const float* data, std::vector<float>& excludeDists, std::vector<int64_t>& excludeSouces, std::vector<std::map<int64_t, float> >& roiLists,
                          int64_t& mapCounter, const std::vector<int>& stencil, const std::vector<float>& stencildist, const float* roiFrame,
                          const OverlapLogicEnum::Enum& overlapType, const VolumeSpace& myDims);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeROIsFromExtrema(ProgressObject* myProgObj, const VolumeFile* myVol, const float& limit, VolumeFile* myVolOut, const float& sigma = -1.0f,
                                       const VolumeFile* myRoi = NULL, const OverlapLogicEnum::Enum& overlapType = OverlapLogicEnum::ALLOW, const int& subvolNum = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeROIsFromExtrema> AutoAlgorithmVolumeROIsFromExtrema;

}

#endif //__ALGORITHM_VOLUME_ROIS_FROM_EXTREMA_H__
