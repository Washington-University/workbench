#ifndef __VOLUME_TO_IMAGE_MAPPING_H__
#define __VOLUME_TO_IMAGE_MAPPING_H__

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
#include <map>
#include <memory>

#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "VolumeToImageMappingModeEnum.h"
#include "VoxelIJK.h"

namespace caret {

    class CaretMappableDataFile;
    class CiftiMappableDataFile;
    class HistologySlice;
    class ImageFile;
    class MediaFile;
    class VolumeFile;
    class VolumeMappableInterface;
    
    class VolumeToImageMapping : public CaretObject {
        
    public:
        VolumeToImageMapping(const VolumeMappableInterface* volumeInterface,
                             const int32_t volumeFileMapIndex,
                             const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                             const float volumeSliceThickness,
                             const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const MediaFile* inputMediaFile);
        
        VolumeToImageMapping(const VolumeMappableInterface* volumeInterface,
                             const int32_t volumeFileMapIndex,
                             const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                             const float volumeSliceThickness,
                             const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const HistologySlice* histologySlice);
        
        virtual ~VolumeToImageMapping();
        
        VolumeToImageMapping(const VolumeToImageMapping&) = delete;

        VolumeToImageMapping& operator=(const VolumeToImageMapping&) = delete;
        
        bool runMapping(AString& errorMessageOut);
        
        int32_t getNumberOfOutputImageFiles() const;
        
        ImageFile* takeOutputImageFile(const int32_t index);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        bool performMapping(const MediaFile* mediaFile,
                            AString& errorMessageOut);
        
        bool validateInputs(const MediaFile* mediaFile,
                            AString& errorMessageOut);
        
        bool validateMediaFile(const MediaFile* mediaFile,
                               const AString errorMessagePrefix,
                               AString& errorMessageInOut);
        
        bool performRgbaMapping(ImageFile* outputImageFile,
                                AString& errorMessageOut);
        
        bool performIntensityMapping(const MediaFile* mediaFile,
                                     ImageFile* outputImageFile,
                                     AString& errorMessageOut);
        
        int32_t getImageBestDimension(const MediaFile* mediaFile,
                                      const VolumeMappableInterface* volumeInterface) const;
        
        void printInputOutputImageDimensions(const std::vector<const MediaFile*> inputMediaFiles) const;
        
        AString getResolutionString(const MediaFile* mediaFile) const;
        
        const VolumeMappableInterface* m_volumeInterface;
        
        const int32_t m_volumeFileMapIndex;
        
        const VolumeToImageMappingModeEnum::Enum m_inputVolumeMappingMode;
        
        const float m_volumeSliceThickness;
        
        const CaretMappableDataFile* m_volumeMappableDataFile;
        
        const DisplayGroupEnum::Enum m_displayGroup;
        
        const int32_t m_tabIndex;

        const MediaFile* m_inputMediaFile;
        
        const HistologySlice* m_histologySlice;
        
        const VolumeFile* m_volumeFile;
        
        const CiftiMappableDataFile* m_ciftiMappableDataFile;
        
        std::vector<std::unique_ptr<ImageFile>> m_outputImageFiles;
        
        std::map<VoxelIJK, std::array<uint8_t, 4>> m_ijkRgbaMap;
        
        static bool s_debugFlag;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_TO_IMAGE_MAPPING_DECLARE__
    bool VolumeToImageMapping::s_debugFlag = false;
#endif // __VOLUME_TO_IMAGE_MAPPING_DECLARE__

} // namespace
#endif  //__VOLUME_TO_IMAGE_MAPPING_H__
