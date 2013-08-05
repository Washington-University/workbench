#ifndef __VOLUME_FILE_VOXEL_COLORIZER_H__
#define __VOLUME_FILE_VOXEL_COLORIZER_H__

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class Palette;
    class VolumeFile;
    
    class VolumeFileVoxelColorizer : public CaretObject {
        
    public:
        VolumeFileVoxelColorizer(VolumeFile* volumeFile);
        
        virtual ~VolumeFileVoxelColorizer();
        
        void assignVoxelColorsForMapInBackground(const int32_t mapIndex,
                                                 const Palette* palette,
                                                 const VolumeFile* thresholdVolume,
                                                 const int32_t thresholdVolumeMapIndex);
        
        void assignVoxelColorsForMap(const int32_t mapIndex,
                                     const Palette* palette,
                                     const VolumeFile* thresholdVolume,
                                     const int32_t thresholdVolumeMapIndex);

        void getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                    const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                    const int64_t sliceIndex,
                                         const DisplayGroupEnum::Enum displayGroup,
                                         const int32_t tabIndex,
                                    uint8_t* rgbaOut) const;
        
        void getVoxelColorInMap(const int64_t i,
                                const int64_t j,
                                const int64_t k,
                                const int64_t mapIndex,
                                uint8_t rgbaOut[4]) const;
        
        void clearVoxelColoringForMap(const int64_t mapIndex);
        
//        void setVoxelColorInMap(const int64_t i,
//                                 const int64_t j,
//                                 const int64_t k,
//                                 const int64_t mapIndex,
//                                 const float rgbaFloat[4]);
        
        void invalidateColoring();
        
    private:
        VolumeFileVoxelColorizer(const VolumeFileVoxelColorizer&);

        VolumeFileVoxelColorizer& operator=(const VolumeFileVoxelColorizer&);
        
        // ADD_NEW_MEMBERS_HERE

        VolumeFile* m_volumeFile;
        
        int64_t m_dimI;
        int64_t m_dimJ;
        int64_t m_dimK;
        int64_t m_voxelCountPerMap;
        int64_t m_mapCount;
        int64_t m_mapRGBACount;
        
        std::vector<bool> m_mapColoringValid;
        std::vector<uint8_t*> m_mapRGBA;
    };
    
#ifdef __VOLUME_FILE_VOXEL_COLORIZER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_FILE_VOXEL_COLORIZER_DECLARE__

} // namespace
#endif  //__VOLUME_FILE_VOXEL_COLORIZER_H__
