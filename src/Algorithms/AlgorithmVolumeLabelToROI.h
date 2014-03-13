#ifndef __ALGORITHM_VOLUME_LABEL_TO_ROI_H__
#define __ALGORITHM_VOLUME_LABEL_TO_ROI_H__

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
    
    class AlgorithmVolumeLabelToROI : public AbstractAlgorithm
    {
        AlgorithmVolumeLabelToROI();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeLabelToROI(ProgressObject* myProgObj, const VolumeFile* myLabel, const AString& labelName, VolumeFile* myVolumeOut, const int& whichMap = -1);
        AlgorithmVolumeLabelToROI(ProgressObject* myProgObj, const VolumeFile* myLabel, const int32_t& labelKey, VolumeFile* myVolumeOut, const int& whichMap = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeLabelToROI> AutoAlgorithmVolumeLabelToROI;

}

#endif //__ALGORITHM_VOLUME_LABEL_TO_ROI_H__
