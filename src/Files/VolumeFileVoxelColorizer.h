#ifndef __VOLUME_FILE_VOXEL_COLORIZER_H__
#define __VOLUME_FILE_VOXEL_COLORIZER_H__

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


#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VoxelIJK.h"

namespace caret {

    class VolumeFile;
    
    class VolumeFileVoxelColorizer : public CaretObject {
        
    public:
        VolumeFileVoxelColorizer(VolumeFile* volumeFile);
        
        virtual ~VolumeFileVoxelColorizer();
        
        void assignVoxelColorsForMap(const int32_t mapIndex) const;
        
        int64_t getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                            const int64_t firstVoxelIJK[3],
                                            const int64_t rowStepIJK[3],
                                            const int64_t columnStepIJK[3],
                                            const int64_t numberOfRows,
                                            const int64_t numberOfColumns,
                                            const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex,
                                            uint8_t* rgbaOut) const;
        
        int64_t getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                         const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                         const int64_t sliceIndex,
                                         const DisplayGroupEnum::Enum displayGroup,
                                         const int32_t tabIndex,
                                         uint8_t* rgbaOut) const;
        
        int64_t getVoxelColorsForSubSliceInMap(const int32_t mapIndex,
                                            const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                            const int64_t sliceIndex,
                                            const int64_t firstCornerVoxelIndex[3],
                                            const int64_t lastCornerVoxelIndex[3],
                                            const int64_t* voxelCountIJK,
                                            const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex,
                                            uint8_t* rgbaOut) const;
        
        void getVoxelColorInMap(const int64_t i,
                                const int64_t j,
                                const int64_t k,
                                const int64_t mapIndex,
                                const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex,
                                uint8_t rgbaOut[4]) const;
        
        void updateVoxelColorsInMap(const int32_t mapIndex,
                                    const std::vector<VoxelIJK>& voxelsIJK,
                                    const uint8_t rgba[4]);
        
        void clearVoxelColoringForMap(const int64_t mapIndex) const;
        
        void invalidateColoring();
        
    private:
        VolumeFileVoxelColorizer(const VolumeFileVoxelColorizer&);

        VolumeFileVoxelColorizer& operator=(const VolumeFileVoxelColorizer&);
        
        /**
         * Get theRGBA offset for a voxel index
         */
        inline int64_t getRgbaOffsetForVoxelIndex(const int64_t i,
                                           const int64_t j,
                                           const int64_t k) const {
            return (4 * (i
                         + (j * m_dimI)
                         + ((k * m_dimI * m_dimJ))));
        }

        /**
         * Get theRGBA offset for a voxel index
         */
        inline int64_t getRgbaOffsetForVoxelIndex(const int64_t ijk[3]) const {
            return (4 * (ijk[0]
                         + (ijk[1] * m_dimI)
                         + ((ijk[2] * m_dimI * m_dimJ))));
        }
        
        // ADD_NEW_MEMBERS_HERE

        VolumeFile* m_volumeFile;
        
        int64_t m_dimI;
        int64_t m_dimJ;
        int64_t m_dimK;
        int64_t m_voxelCountPerMap;
        int64_t m_mapCount;
        int64_t m_mapRGBACount;
        
        mutable std::vector<bool> m_mapColoringValid;
        mutable std::vector<uint8_t*> m_mapRGBA;
    };
    
#ifdef __VOLUME_FILE_VOXEL_COLORIZER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_FILE_VOXEL_COLORIZER_DECLARE__

} // namespace
#endif  //__VOLUME_FILE_VOXEL_COLORIZER_H__
