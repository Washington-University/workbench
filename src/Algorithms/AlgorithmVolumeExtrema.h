#ifndef __ALGORITHM_VOLUME_EXTREMA_H__
#define __ALGORITHM_VOLUME_EXTREMA_H__

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
#include "Vector3D.h"
#include "VoxelIJK.h"
#include <vector>

namespace caret {
    
    class VolumeFile;
    
    class AlgorithmVolumeExtrema : public AbstractAlgorithm
    {
        std::vector<VoxelIJK> m_stencil;
        int64_t m_irange, m_jrange, m_krange;
        AlgorithmVolumeExtrema();
        void precomputeStencil(const VolumeFile* myVolIn, const float& distance);
        void findExtremaConsolidate(const VolumeFile* toProcess, const int& s, const int& c, const VolumeFile* myRoi, const float& distance, const bool& threshMode, const float& lowThresh, const float& highThresh, bool ignoreMinima, bool ignoreMaxima, std::vector<VoxelIJK>& minima, std::vector<VoxelIJK>& maxima);
        void findExtremaStencils(const VolumeFile* toProcess, const int& s, const int& c, const VolumeFile* myRoi, const bool& threshMode, const float& lowThresh, const float& highThresh, bool ignoreMinima, bool ignoreMaxima, std::vector<VoxelIJK>& minima, std::vector<VoxelIJK>& maxima);
        void consolidateStep(const VolumeFile* toProcess, const float& distance, std::vector<std::pair<Vector3D, int> > tempExtrema[2], std::vector<VoxelIJK>& minima, std::vector<VoxelIJK>& maxima);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeExtrema(ProgressObject* myProgObj, const VolumeFile* myVolIn, const float& distance, VolumeFile* myVolOut, const float& lowThresh,
                               const float& highThresh, const VolumeFile* myRoi = NULL, const float& presmooth = -1.0f, const bool& sumSubvols = false,
                               const bool& consolidateMode = false, bool ignoreMinima = false, bool ignoreMaxima = false, const int& subvol = -1);
        AlgorithmVolumeExtrema(ProgressObject* myProgObj, const VolumeFile* myVolIn, const float& distance, VolumeFile* myVolOut,
                               const VolumeFile* myRoi = NULL, const float& presmooth = -1.0f, const bool& sumSubvols = false,
                               const bool& consolidateMode = false, bool ignoreMinima = false, bool ignoreMaxima = false, const int& subvol = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeExtrema> AutoAlgorithmVolumeExtrema;

}

#endif //__ALGORITHM_VOLUME_EXTREMA_H__
