#ifndef __VOXEL_IJK_H__
#define __VOXEL_IJK_H__

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

namespace caret {
    
    struct VoxelIJK
    {
        int64_t m_ijk[3];
        VoxelIJK() { }
        VoxelIJK(int64_t i, int64_t j, int64_t k) { m_ijk[0] = i; m_ijk[1] = j; m_ijk[2] = k; }
        template<typename T>
        VoxelIJK(const T ijk[3]) {
            m_ijk[0] = ijk[0];
            m_ijk[1] = ijk[1];
            m_ijk[2] = ijk[2];
        }
        bool operator<(const VoxelIJK& rhs) const//so it can be the key of a map
        {
            if (m_ijk[2] < rhs.m_ijk[2]) return true;//compare such that when sorted, m_ijk[0] moves fastest
            if (m_ijk[2] > rhs.m_ijk[2]) return false;
            if (m_ijk[1] < rhs.m_ijk[1]) return true;
            if (m_ijk[1] > rhs.m_ijk[1]) return false;
            return (m_ijk[0] < rhs.m_ijk[0]);
        }
        bool operator==(const VoxelIJK& rhs) const
        {
            return (m_ijk[0] == rhs.m_ijk[0] &&
                    m_ijk[1] == rhs.m_ijk[1] &&
                    m_ijk[2] == rhs.m_ijk[2]);
        }
        bool operator!=(const VoxelIJK& rhs) const { return !((*this) == rhs); }
        inline operator int64_t*() { return m_ijk; }
    };
    
}

#endif //__VOXEL_IJK_H__
