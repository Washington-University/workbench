#ifndef __ALGORITHM_FOCI_RESAMPLE_H__
#define __ALGORITHM_FOCI_RESAMPLE_H__

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
    
    class AlgorithmFociResample : public AbstractAlgorithm
    {
        AlgorithmFociResample();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmFociResample(ProgressObject* myProgObj, const FociFile* fociIn, FociFile* fociOut,
                              const SurfaceFile* leftCurSurf, const SurfaceFile* leftNewSurf,
                              const SurfaceFile* rightCurSurf = NULL, const SurfaceFile* rightNewSurf = NULL,
                              const SurfaceFile* cerebCurSurf = NULL, const SurfaceFile* cerebNewSurf = NULL,
                              const bool& discardNormDist = false, const bool& restoryXyz = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmFociResample> AutoAlgorithmFociResample;

}

#endif //__ALGORITHM_FOCI_RESAMPLE_H__
