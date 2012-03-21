#ifndef __ALGORITHM_VOLUME_SMOOTHING_H__
#define __ALGORITHM_VOLUME_SMOOTHING_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmVolumeSmoothing : public AbstractAlgorithm
    {
        AlgorithmVolumeSmoothing();
        static bool haveWarned;
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
        void smoothFrame(const float* inFrame, std::vector<int64_t> myDims, CaretArray<float> scratchFrame, CaretArray<float> scratchFrame2, CaretArray<float> scratchWeights,
                         CaretArray<float> scratchWeights2, const VolumeFile* inVol, CaretArray<float> iweights, CaretArray<float> jweights, CaretArray<float> kweights,
                         int irange, int jrange, int krange, const bool& fixZeros);
        void smoothFrameROI(const float* inFrame, std::vector<int64_t> myDims, CaretArray<float> scratchFrame, CaretArray<float> scratchFrame2, CaretArray<float> scratchFrame3,
                                              CaretArray<float> scratchWeights, CaretArray<float> scratchWeights2, std::vector<int> lists[3],
                                              const VolumeFile* inVol, const VolumeFile* roiVol, CaretArray<float> iweights, CaretArray<float> jweights, CaretArray<float> kweights,
                                              int irange, int jrange, int krange, const bool& fixZeros);
        void smoothFrameNonOrth(const float* inFrame, const std::vector<int64_t>& myDims, CaretArray<float>& scratchFrame, const VolumeFile* inVol, const VolumeFile* roiVol, const CaretArray<float**>& weights, const int& irange, const int& jrange, const int& krange, const bool& fixZeros);
    public:
        AlgorithmVolumeSmoothing(ProgressObject* myProgObj, const VolumeFile* inVol, const float& kernel, VolumeFile* outVol, const VolumeFile* roiVol = NULL, const bool& fixZeros = false, const int& subvol = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeSmoothing> AutoAlgorithmVolumeSmoothing;

}

#endif //__ALGORITHM_VOLUME_SMOOTHING_H__
