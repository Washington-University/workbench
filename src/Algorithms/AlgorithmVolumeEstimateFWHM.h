#ifndef __ALGORITHM_VOLUME_ESTIMATE_FWHM_H__
#define __ALGORITHM_VOLUME_ESTIMATE_FWHM_H__

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

#include "Vector3D.h"
#include "VolumeFile.h"

namespace caret {
    
    class AlgorithmVolumeEstimateFWHM : public AbstractAlgorithm
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
        
        static Vector3D estimateFWHM(const VolumeFile* input, const VolumeFile* roi = NULL, const int64_t& brickIndex = 0, const int64_t& component = 0);
        static Vector3D estimateFWHMAllFrames(const VolumeFile* input, const VolumeFile* roi = NULL, bool demean = false);
    };

    typedef TemplateAutoOperation<AlgorithmVolumeEstimateFWHM> AutoAlgorithmVolumeEstimateFWHM;

}

#endif //__ALGORITHM_VOLUME_ESTIMATE_FWHM_H__
