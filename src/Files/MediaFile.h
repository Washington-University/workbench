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

#include <memory>

#include <QRectF>

#include "CaretDataFile.h"
#include "MediaDisplayCoordinateModeEnum.h"
#include "NiftiEnums.h"
#include "PixelCoordinate.h"
#include "PixelIndex.h"
#include "PixelLogicalIndex.h"
#include "SceneClassAssistant.h"
#include "Vector3D.h"
#include "VoxelIJK.h"

class QImage;

namespace caret {

    class BoundingBox;
    class GraphicsPrimitiveV3fT2f;
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
         * @return width of media file
         */
        virtual int32_t getWidth() const = 0;
        
        /**
         * @return height of media file
         */
        virtual int32_t getHeight() const = 0;
        
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
        
        virtual bool isPixelIndexInFrameValid(const int32_t frameIndex,
                                              const PixelIndex& pixelIndexOriginAtTopLeft) const = 0;
        
        virtual bool isPixelIndexInFrameValid(const int32_t frameIndex,
                                              const PixelLogicalIndex& pixelLogicalIndex) const = 0;
        
        virtual bool isPixelIndexValid(const PixelIndex& pixelIndexOriginAtTopLeft) const = 0;
        
        virtual bool isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const = 0;
        
        virtual void getPixelIdentificationTextForFrames(const int32_t tabIndex,
                                                         const std::vector<int32_t>& frameIndices,
                                                         const PixelLogicalIndex& pixelLogicalIndex,
                                                         std::vector<AString>& columnOneTextOut,
                                                         std::vector<AString>& columnTwoTextOut,
                                                         std::vector<AString>& toolTipTextOut) const = 0;

        virtual bool getPixelRGBA(const int32_t tabIndex,
                                  const int32_t overlayIndex,
                                  const PixelLogicalIndex& pixelLogicalIndex,
                                  uint8_t pixelRGBAOut[4]) const = 0;

        virtual bool pixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                                const bool includeNonlinearFlag,
                                                Vector3D& xyzOut) const = 0;
        
        virtual bool stereotaxicXyzToPixelIndex(const Vector3D& xyz,
                                                const bool includeNonlinearFlag,
                                                PixelLogicalIndex& pixelLogicalIndexOut) const = 0;
        
        virtual bool findPixelNearestStereotaxicXYZ(const Vector3D& xyz,
                                                    const bool includeNonLinearFlag,
                                                    float& signedDistanceToPixelMillimetersOut,
                                                    PixelLogicalIndex& pixelLogicalIndexOut) const = 0;
        
        virtual bool pixelIndexToPlaneXYZ(const PixelIndex& pixelIndex,
                                          Vector3D& planeXyzOut) const;
        
        virtual bool logicalPixelIndexToPlaneXYZ(const PixelLogicalIndex& pixelLogialIndex,
                                                 Vector3D& planeXyzOut) const;
        
        virtual bool planeXyzToPixelIndex(const Vector3D& planeXyz,
                                          PixelIndex& pixelIndexOut) const;
        
        virtual bool planeXyzToLogicalPixelIndex(const Vector3D& planeXyz,
                                                 PixelLogicalIndex& pixelLogicalIndexOut) const;
        
        virtual QRectF getLogicalBoundsRect() const;
        
        virtual PixelIndex pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const;
        
        virtual PixelLogicalIndex pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const;
        
        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForMediaDrawing(const int32_t tabIndex,
                                                                             const int32_t overlayIndex) const = 0;
        
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
    protected: 
        MediaFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        void initializeMembersMediaFile();
        
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
