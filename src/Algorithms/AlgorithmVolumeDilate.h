#ifndef __ALGORITHM_VOLUME_DILATE_H__
#define __ALGORITHM_VOLUME_DILATE_H__

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
    
    class AlgorithmVolumeDilate : public AbstractAlgorithm
    {
        AlgorithmVolumeDilate();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        enum Method
        {
            NEAREST,
            WEIGHTED
        };
        AlgorithmVolumeDilate(ProgressObject* myProgObj, const VolumeFile* volIn, const float& distance, const Method& myMethod, VolumeFile* volOut,
                              const VolumeFile* badRoi = NULL, const VolumeFile* dataRoi = NULL, const int& subvol = -1, const float& exponent = 7.0f,
                              const bool legacyCutoff = false, const bool extrapolate = false, const float extrapPresmooth = -1.0f);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeDilate> AutoAlgorithmVolumeDilate;

}

#endif //__ALGORITHM_VOLUME_DILATE_H__
