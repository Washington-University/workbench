#ifndef __ALGORITHM_VOLUME_PARCEL_RESAMPLING_H__
#define __ALGORITHM_VOLUME_PARCEL_RESAMPLING_H__

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
    
    class AlgorithmVolumeParcelResampling : public AbstractAlgorithm
    {
        AlgorithmVolumeParcelResampling();
        void matchLabels(const caret::VolumeFile* curLabel, const caret::VolumeFile* newLabel, std::vector<std::pair<int, int> >& matchedLabels);
        void generateVoxelLists(const std::vector<std::pair<int, int> >& matchedLabels, const VolumeFile* curLabel, const VolumeFile* newLabel, std::vector<std::vector<int64_t> >& voxelLists);
        void resampleFixZeros(LevelProgress& myProgress, const std::vector<std::pair<int, int> >& matchedLabels, const std::vector<std::vector<int64_t> >& voxelLists, const VolumeFile* inVol, const VolumeFile* curLabel, const caret::VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const int& subvolNum);
        void resample(LevelProgress& myProgress, const std::vector<std::pair<int, int> >& matchedLabels, const std::vector<std::vector<int64_t> >& voxelLists, const VolumeFile* inVol, const VolumeFile* curLabel, const caret::VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const int& subvolNum);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeParcelResampling(ProgressObject* myProgObj, const VolumeFile* inVol, const VolumeFile* curLabel, const VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const bool& fixZeros = false, const int& subvolNum = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeParcelResampling> AutoAlgorithmVolumeParcelResampling;

}

#endif //__ALGORITHM_VOLUME_PARCEL_RESAMPLING_H__
