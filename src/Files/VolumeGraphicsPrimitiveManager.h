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
#include "VolumeToImageMappingModeEnum.h"
#include "VoxelColorUpdate.h"
#include "VoxelIJK.h"

namespace caret {
    class CaretDataFile;
    class CaretMappableDataFile;
    class GraphicsPrimitive;
    class GraphicsPrimitiveV3fT3f;
    class GraphicsPrimitiveV3fT2f;
    class HistologySlice;
    class ImageFile;
    class MediaFile;
    class TabDrawingInfo;
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
                                                                 const TabDrawingInfo& tabDrawingInfo) const;


        GraphicsPrimitiveV3fT2f* getImageIntersectionDrawingPrimitiveForMap(const MediaFile* mediaFile,
                                                                            const int32_t mapIndex,
                                                                            const TabDrawingInfo& tabDrawingInfo,
                                                                            const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                            const float volumeSliceThickness,
                                                                            AString& errorMessageOut) const;
        
        std::vector<GraphicsPrimitive*> getImageIntersectionDrawingPrimitiveForMap(const HistologySlice* histologySlice,
                                                                                   const int32_t mapIndex,
                                                                                   const TabDrawingInfo& tabDrawingInfo,
                                                                                   const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                                   const float volumeSliceThickness,
                                                                                   AString& errorMessageOut) const;

        void updateVoxelColorsInMapTexture(const VoxelColorUpdate& voxelColorUpdate);

        const VoxelColorUpdate* getVoxelColorUpdate(const int32_t mapIndex) const;
        
        void resetVoxelColorUpdate(const int32_t mapIndex) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class ImageIntersectionKey {
        public:
            ImageIntersectionKey(void* dataPtr,
                                 const int32_t mapIndex,
                                 const int32_t tabIndex,
                                 const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                 const float volumeSliceThickness)
            : m_dataPtr(dataPtr),
            m_mapIndex(mapIndex),
            m_tabIndex(tabIndex),
            m_volumeMappingMode(volumeMappingMode),
            m_volumeSliceThickness(volumeSliceThickness)
            { }
            
            
            bool operator<(const ImageIntersectionKey& rhs) const {
                if (m_dataPtr == rhs.m_dataPtr) {
                    if (m_mapIndex == rhs.m_mapIndex) {
                        if (m_tabIndex == rhs.m_tabIndex) {
                            if ((int)m_volumeMappingMode == (int)rhs.m_volumeMappingMode) {
                                return (m_volumeSliceThickness < rhs.m_volumeSliceThickness);
                            }
                            else {
                                return ((int)m_volumeMappingMode < (int)rhs.m_volumeMappingMode);
                            }
                        }
                        else {
                            return (m_tabIndex < rhs.m_tabIndex);
                        }
                    }
                    else {
                        return (m_mapIndex < rhs.m_mapIndex);
                    }
                }
                return (m_dataPtr < rhs.m_dataPtr);
            }

            
            void* m_dataPtr;
            int32_t m_mapIndex;
            int32_t m_tabIndex;
            VolumeToImageMappingModeEnum::Enum m_volumeMappingMode;
            float m_volumeSliceThickness;
        };
        
        GraphicsPrimitiveV3fT3f* createPrimitive(const PrimitiveShape drawingType,
                                                 const int32_t mapIndex,
                                                 const TabDrawingInfo& tabDrawingInfo,
                                                 AString& errorMessageOut) const;

        void updateNumberOfVoxelColorUpdates(const int32_t mapIndex) const;
        
        void clearIntersectionImagePrimitives();
        
        CaretMappableDataFile* m_mapDataFile;
        
        VolumeMappableInterface* m_volumeInterface;
        
        mutable std::vector<std::unique_ptr<GraphicsPrimitiveV3fT3f>> m_mapGraphicsTriangleFanPrimitives;

        mutable std::vector<std::unique_ptr<GraphicsPrimitiveV3fT3f>> m_mapGraphicsTriangleStripPrimitives;
        
        mutable std::vector<std::unique_ptr<GraphicsPrimitiveV3fT3f>> m_mapGraphicsTrianglesPrimitives;
        
        mutable std::vector<VoxelColorUpdate> m_voxelColorUpdates;
        
//        mutable std::vector<std::unique_ptr<ImageFile>> m_mapIntersectionImageFiles;

        mutable std::map<ImageIntersectionKey, std::vector<ImageFile*>> m_mapIntersectionImageFiles;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_GRAPHICS_PRIMITIVE_MANAGER_DECLARE__

} // namespace
#endif  //__VOLUME_GRAPHICS_PRIMITIVE_MANAGER_H__
