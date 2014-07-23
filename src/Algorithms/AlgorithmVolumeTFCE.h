#ifndef __ALGORITHM_VOLUME_TFCE_H__
#define __ALGORITHM_VOLUME_TFCE_H__

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
    
    class AlgorithmVolumeTFCE : public AbstractAlgorithm
    {
        AlgorithmVolumeTFCE();
        void processFrame(const VolumeFile* inVol, const int64_t& b, const int64_t& c, float* outData, const float* roiData, const float& param_e, const float& param_h);
        void tfce(const VolumeFile* inVol, const int64_t& b, const int64_t& c, double* accumData, const float* roiData, const float& param_e, const float& param_h, const bool& negate);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeTFCE(ProgressObject* myProgObj, const VolumeFile* myVol, VolumeFile* myVolOut, const float& presmooth = 0.0f, const VolumeFile* myRoi = NULL,
                            const float& param_e = 0.5f, const float& param_h = 2.0f, const int64_t& subvolNum = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeTFCE> AutoAlgorithmVolumeTFCE;

}

#endif //__ALGORITHM_VOLUME_TFCE_H__
