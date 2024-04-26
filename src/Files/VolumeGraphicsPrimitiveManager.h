#ifndef __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_H__
#define __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include <map>
#include <memory>

#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "VoxelColorUpdate.h"
#include "VoxelIJK.h"

namespace caret {
    class CaretMappableDataFile;
    class GraphicsPrimitiveV3fT3f;
    class GraphicsPrimitiveV3fT2f;
    class ImageFile;
    class MediaFile;
    class VolumeMappableInterface;

    class VolumeGraphicsPrimitiveManager : public CaretObject {
        
    public:
        enum class PrimitiveShape {
            TRIANGLE_FAN,
            TRIANGLE_STRIP,
            TRIANGLES
        };
        
        VolumeGraphicsPrimitiveManager(CaretMappableDataFile* mapDataFile,
                                       VolumeMappableInterface* volumeInterface);
        

        virtual ~VolumeGraphicsPrimitiveManager();
        
        VolumeGraphicsPrimitiveManager(const VolumeGraphicsPrimitiveManager&) = delete;

        VolumeGraphicsPrimitiveManager& operator=(const VolumeGraphicsPrimitiveManager&) = delete;
        
        void clear();
        
        void invalidateAllColoring();
        
        void invalidateColoringForMap(const int32_t mapIndex);
        
        GraphicsPrimitiveV3fT3f* getVolumeDrawingPrimitiveForMap(const PrimitiveShape drawingType,
                                                                 const int32_t mapIndex,
                                                                 const DisplayGroupEnum::Enum displayGroup,
                                                                 const int32_t tabIndex) const;


        GraphicsPrimitiveV3fT2f* getImageIntersectionDrawingPrimtiveForMap(const MediaFile* mediaFile,
                                                                           const int32_t mapIndex,
                                                                           const DisplayGroupEnum::Enum displayGroup,
                                                                           const int32_t tabIndex,
                                                                           AString& errorMessageOut) const;
        
        void updateVoxelColorsInMapTexture(const VoxelColorUpdate& voxelColorUpdate);

        const VoxelColorUpdate* getVoxelColorUpdate(const int32_t mapIndex) const;
        
        void resetVoxelColorUpdate(const int32_t mapIndex) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class ImageIntersectionKey {
        public:
            ImageIntersectionKey(MediaFile* mediaFile,
                                 const int32_t mapIndex,
                                 const int32_t tabIndex)
            : m_mediaFile(mediaFile),
            m_mapIndex(mapIndex),
            m_tabIndex(tabIndex)
            { }
            
//            bool operator==(const ImageIntersectionKey& rhs) const {
//                if ((m_mediaFile == rhs.m_mediaFile)
//                    && (m_mapIndex == rhs.m_mapIndex)
//                    && (m_tabIndex == rhs.m_tabIndex)) {
//                    return true;
//                }
//                return false;
//            }
            
            bool operator<(const ImageIntersectionKey& rhs) const {
                if (m_mediaFile == rhs.m_mediaFile) {
                    if (m_mapIndex == rhs.m_mapIndex) {
                        return (m_tabIndex < rhs.m_tabIndex);
                    }
                    else {
                        return (m_mapIndex < rhs.m_mapIndex);
                    }
                }
                return (m_mediaFile < rhs.m_mediaFile);
            }

            
            MediaFile* m_mediaFile;
            int32_t m_mapIndex;
            int32_t m_tabIndex;
        };
        
        GraphicsPrimitiveV3fT3f* createPrimitive(const PrimitiveShape drawingType,
                                                 const int32_t mapIndex,
                                                 const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 AString& errorMessageOut) const;

        void updateNumberOfVoxelColorUpdates(const int32_t mapIndex) const;
        
        CaretMappableDataFile* m_mapDataFile;
        
        VolumeMappableInterface* m_volumeInterface;
        
        mutable std::vector<std::unique_ptr<GraphicsPrimitiveV3fT3f>> m_mapGraphicsTriangleFanPrimitives;

        mutable std::vector<std::unique_ptr<GraphicsPrimitiveV3fT3f>> m_mapGraphicsTriangleStripPrimitives;
        
        mutable std::vector<std::unique_ptr<GraphicsPrimitiveV3fT3f>> m_mapGraphicsTrianglesPrimitives;
        
        mutable std::vector<VoxelColorUpdate> m_voxelColorUpdates;
        
//        mutable std::vector<std::unique_ptr<ImageFile>> m_mapIntersectionImageFiles;

        mutable std::map<ImageIntersectionKey, std::unique_ptr<ImageFile>> m_mapIntersectionImageFiles;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_DECLARE__

} // namespace
#endif  //__VOLUME_GRAPHICS_PRIMITIVE_MANAGER_H__
