#ifndef __VOLUME_PADDING_HELPER_H__
#define __VOLUME_PADDING_HELPER_H__

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

#include "stdint.h"
#include <vector>

namespace caret {
    
    class VolumeFile;
    
    class VolumePaddingHelper
    {
        std::vector<std::vector<float> > m_origSform, m_paddedSform;
        std::vector<int64_t> m_origDims, m_paddedDims;
        int64_t m_ipad, m_jpad, m_kpad;//hopefully apple doesn't sue us
    public:
        VolumePaddingHelper() { }
        static VolumePaddingHelper padMM(const VolumeFile* orig, const float& mmpad);
        static VolumePaddingHelper padVoxels(const VolumeFile* orig, const int& ipad, const int& jpad, const int& kpad);
        void doPadding(const VolumeFile* orig, VolumeFile* padded, const float& padval = 0.0f);
        void undoPadding(const VolumeFile* padded, VolumeFile* orig);
    };
    
}

#endif //__VOLUME_PADDING_HELPER_H__
