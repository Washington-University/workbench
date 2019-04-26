#ifndef __VOLUME_PADDING_HELPER_H__
#define __VOLUME_PADDING_HELPER_H__

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
        void getPadding(int& ipad, int& jpad, int& kpad) const { ipad = m_ipad; jpad = m_jpad; kpad = m_kpad; }
        void doPadding(const VolumeFile* orig, VolumeFile* padded, const float& padval = 0.0f) const;
        void undoPadding(const VolumeFile* padded, VolumeFile* orig) const;
    };
    
}

#endif //__VOLUME_PADDING_HELPER_H__
