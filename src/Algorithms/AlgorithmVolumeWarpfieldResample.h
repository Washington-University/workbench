#ifndef __ALGORITHM_VOLUME_WARPFIELD_RESAMPLE_H__
#define __ALGORITHM_VOLUME_WARPFIELD_RESAMPLE_H__

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
#include "VolumeFile.h"

namespace caret {
    
    class AlgorithmVolumeWarpfieldResample : public AbstractAlgorithm
    {
        AlgorithmVolumeWarpfieldResample();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeWarpfieldResample(ProgressObject* myProgObj, const VolumeFile* inVol, const VolumeFile* warpfield,
                                         const int64_t refDims[3], const std::vector<std::vector<float> >& refSform, const VolumeFile::InterpType& myMethod, VolumeFile* outVol);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeWarpfieldResample> AutoAlgorithmVolumeWarpfieldResample;

}

#endif //__ALGORITHM_VOLUME_WARPFIELD_RESAMPLE_H__
