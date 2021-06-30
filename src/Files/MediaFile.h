#ifndef __MEDIA_FILE_H__
#define __MEDIA_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#include <memory>

#include "CaretDataFile.h"
#include "DefaultViewTransform.h"
#include "NiftiEnums.h"
#include "PixelCoordinate.h"
#include "PixelIndex.h"
#include "SceneClassAssistant.h"
#include "Vector3D.h"
#include "VoxelIJK.h"

class QImage;

namespace caret {

    class BoundingBox;
    class VolumeSpace;

    class MediaFile : public CaretDataFile {
        
    public:
        /**
         * Location of origin in image data.
         */
        enum IMAGE_DATA_ORIGIN_LOCATION {
            /** Origin at bottom (OpenGL has origin at bottom) */
            IMAGE_DATA_ORIGIN_AT_BOTTOM,
            /** Origin at top (most image formats have origin at top) */
            IMAGE_DATA_ORIGIN_AT_TOP
        };
        
        virtual ~MediaFile();
        
        MediaFile(const MediaFile& mediaFile);

        MediaFile& operator=(const MediaFile&) = delete;
        
        /**
         * @return The structure for this file.
         */
        virtual StructureEnum::Enum getStructure() const override;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        virtual void setStructure(const StructureEnum::Enum structure) override;
        
        /**
         * @return Number of frames in the file
         */
        virtual int32_t getNumberOfFrames() const = 0;

        /**
         * @return Name of frame at given index.
         * @param frameIndex Index of the frame
         */
        virtual AString getFrameName(const int32_t frameIndex) const;

        /**
         * @return The units for the 'interval' between two consecutive frames.
         */
        NiftiTimeUnitsEnum::Enum getFrameIntervalUnits() const;

        /**
         * Get the units value for the first frame and the
         * quantity of units between consecutive frames.  If the
         * units for the frame is unknown, value of one (1) are
         * returned for both output values.
         *
         * @param firstFrameUnitsValueOut
         *     Output containing units value for first frame.
         * @param frameIntervalStepValueOut
         *     Output containing number of units between consecutive frame.
         */
        virtual void getFrameIntervalStartAndStep(float& firstFrameUnitsValueOut,
                                                  float& frameIntervalStepValueOut) const;
        

        MediaFile* castToMediaFile();
        
        const MediaFile* castToMediaFile() const;
        
        virtual bool pixelIndexValid(const int32_t tabIndex,
                                const PixelIndex& pixelIndex) const;
        
        virtual PixelIndex spaceToPixelIndex(const int32_t tabIndex,
                                        const PixelCoordinate& coordinate) const;
        
        virtual bool spaceToPixelIndexValid(const int32_t tabIndex,
                                       const PixelCoordinate& coordinate,
                                       PixelIndex& pixelIndexOut) const;
        
        virtual PixelCoordinate pixelIndexToSpace(const int32_t tabIndex,
                                                  const PixelIndex& pixelIndex) const;
        
        virtual DefaultViewTransform getDefaultViewTransform(const int32_t tabIndex) const = 0;
        
        /**
         * Get the identification text for the pixel at the given pixel index with origin at bottom left.
         * @param tabIndex
         *    Index of the tab in which identification took place
         * @param pixelIndex
         *    Index of the pixel.
         * @param columnOneTextOut
         *    Text for column one that is displayed to user.
         * @param columnTwoTextOut
         *    Text for column two that is displayed to user.
         */
        virtual void getPixelIdentificationText(const int32_t tabIndex,
                                                const PixelIndex& pixelIndex,
                                                std::vector<AString>& columnOneTextOut,
                                                std::vector<AString>& columnTwoTextOut) const = 0;
        
        static float getMediaDrawingOrthographicHalfHeight();
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
    protected: 
        enum class SpatialCoordinateOrigin {
            BOTTOM_LEFT,
            CENTER
        };
        
        class SpatialInfo {
        public:
            SpatialInfo(VolumeSpace* volumeSpace,
                        BoundingBox* boundingBox)
            : m_volumeSpace(volumeSpace),
            m_boundingBox(boundingBox)
            { }
            
            VolumeSpace* m_volumeSpace;
            BoundingBox* m_boundingBox;
        };
        
        MediaFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual const VolumeSpace* getPixelToCoordinateTransform(const int32_t tabIndex) const = 0;
        
        virtual const BoundingBox* getSpatialBoundingBox(const int32_t tabIndex) const = 0;
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        void initializeMembersMediaFile();
        
        static SpatialInfo initializeVolumeSpace(const int32_t imageWidth,
                                                 const int32_t imageHeight,
                                                 const std::array<float,3> firstPixelXYZ,
                                                 const std::array<float,3> pixelStepXYZ);

        static SpatialInfo setDefaultSpatialCoordinates(const QImage* qImage,
                                                        const SpatialCoordinateOrigin spatialOrigin);
        
        static void getSpatialValues(const float numPixels,
                                     const float spatialMinimumValue,
                                     const float spatialMaximumValue,
                                     float& firstPixelSpatialValue,
                                     float& lastPixelSpatialValue,
                                     float& pixelSpatialStepValue);
        
        static void getDefaultSpatialValues(const float numPixels,
                                            float& minSpatialValueOut,
                                            float& maxSpatialValueOut,
                                            float& firstPixelSpatialValue,
                                            float& lastPixelSpatialValue,
                                            float& pixelSpatialStepValue);

    private:
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

        friend class CziImage;
    };
    
#ifdef __MEDIA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MEDIA_FILE_DECLARE__

} // namespace
#endif  //__MEDIA_FILE_H__
