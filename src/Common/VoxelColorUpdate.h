#ifndef __VOXEL_COLOR_UPDATE_H__
#define __VOXEL_COLOR_UPDATE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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


#include <array>
#include <cstdint>
#include <memory>

#include "CaretObject.h"
#include "VoxelIJK.h"

namespace caret {

    class VoxelColorUpdate : public CaretObject {
        
    public:
        VoxelColorUpdate();
        
        virtual ~VoxelColorUpdate();
        
        VoxelColorUpdate(const VoxelColorUpdate& obj);

        VoxelColorUpdate& operator=(const VoxelColorUpdate& obj);
        
        void addVoxel(const VoxelIJK& voxelIJK);
        
        void addVoxels(const std::vector<VoxelIJK>& voxelsIJK);
        
        void addVoxel(const int64_t i,
                      const int64_t j,
                      const int64_t k);
        
        void clear();
        
        bool isValid() const;
        
        int32_t getMapIndex() const;
        
        void setMapIndex(const int32_t mapIndex);
        
        const std::array<uint8_t, 4>& getRGBA() const;
        
        int32_t getNumberOfVoxels() const;
        
        const VoxelIJK& getVoxel(const int32_t index) const;
        
        void setRGBA(const std::array<uint8_t, 4>& rgba);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperVoxelColorUpdate(const VoxelColorUpdate& obj);

        std::vector<VoxelIJK> m_voxels;
        
        int32_t m_mapIndex = -1;
        
        std::array<uint8_t, 4> m_rgba;
        
        bool m_rgbaValid = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOXEL_COLOR_UPDATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOXEL_COLOR_UPDATE_DECLARE__

} // namespace
#endif  //__VOXEL_COLOR_UPDATE_H__
