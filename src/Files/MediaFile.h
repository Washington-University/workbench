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

#include "BoundingBox.h"
#include "CaretDataFile.h"
#include "Matrix4x4.h"
#include "MediaDisplayCoordinateModeEnum.h"
#include "MediaFileTransforms.h"
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
    class CziNonLinearTransform;
    class GraphicsPrimitiveV3fT2f;
    class ImageFile;
    class MediaFileChannelInfo;
    class Plane;
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
        
        ImageFile* cloneAsImageFile(AString& errorMessageOut) const;
        
        ImageFile* cloneAsImageFileMaximumWidthHeight(const int32_t maximimumWidthHeight,
                                                      AString& errorMessageOut) const;
        
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
        
        const MediaFileChannelInfo* getMediaFileChannelInfo() const;
        
        MediaFile* castToMediaFile();
        
        const MediaFile* castToMediaFile() const;
        
        virtual bool isPixelIndexInFrameValid(const int32_t frameIndex,
                                              const PixelIndex& pixelIndexOriginAtTopLeft) const = 0;
        
        virtual bool isPixelIndexInFrameValid(const int32_t frameIndex,
                                              const PixelLogicalIndex& pixelLogicalIndex) const = 0;
        
        virtual bool isPixelIndexValid(const PixelIndex& pixelIndexOriginAtTopLeft) const = 0;
        
        virtual bool isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const = 0;
        
        virtual void getPixelLogicalIdentificationTextForFrames(const int32_t tabIndex,
                                                                const std::vector<int32_t>& frameIndices,
                                                                const PixelLogicalIndex& pixelLogicalIndex,
                                                                std::vector<AString>& columnOneTextOut,
                                                                std::vector<AString>& columnTwoTextOut,
                                                                std::vector<AString>& toolTipTextOut) const = 0;

        virtual void getPixelPlaneIdentificationTextForFrames(const int32_t tabIndex,
                                                              const std::vector<int32_t>& frameIndices,
                                                              const Vector3D& planeCoordinate,
                                                              const bool histologyIdFlag,
                                                              std::vector<AString>& columnOneTextOut,
                                                              std::vector<AString>& columnTwoTextOut,
                                                              std::vector<AString>& toolTipTextOut) const;
        
        virtual void getPixelPlaneIdentificationTextForHistology(const int32_t tabIndex,
                                                              const std::vector<int32_t>& frameIndices,
                                                              const Vector3D& planeCoordinate,
                                                              std::vector<AString>& columnOneTextOut,
                                                              std::vector<AString>& columnTwoTextOut,
                                                              std::vector<AString>& toolTipTextOut) const;

        virtual bool getPixelRGBA(const int32_t tabIndex,
                                  const int32_t overlayIndex,
                                  const PixelLogicalIndex& pixelLogicalIndex,
                                  uint8_t pixelRGBAOut[4]) const = 0;

        virtual bool pixelIndexToStereotaxicXYZ(const PixelIndex& pixelIndex,
                                                Vector3D& xyzOut) const;
        
        virtual bool stereotaxicXyzToPixelIndex(const Vector3D& xyz,
                                                PixelIndex& pixelIndexOut) const;
        
        virtual bool stereotaxicXyzToLogicalPixelIndex(const Vector3D& xyz,
                                                       PixelLogicalIndex& pixelLogicalIndexOut) const;
        
        virtual bool findPixelNearestStereotaxicXYZ(const Vector3D& xyz,
                                                    float& signedDistanceToPixelMillimetersOut,
                                                    PixelLogicalIndex& pixelLogicalIndexOut) const;
        
        virtual bool findPlaneCoordinateNearestStereotaxicXYZ(const Vector3D& xyz,
                                                              float& signedDistanceToPlaneMillimetersOut,
                                                              Vector3D& planeXyzOut) const;

        QRectF getPlaneXyzRect() const;
        
        virtual bool pixelIndexToPlaneXYZ(const PixelIndex& pixelIndex,
                                          Vector3D& planeXyzOut) const;
        
        virtual bool logicalPixelIndexToPlaneXYZ(const PixelLogicalIndex& pixelLogialIndex,
                                                 Vector3D& planeXyzOut) const;
        
        virtual bool logicalPixelIndexToPlaneXYZ(const float logicalX,
                                                 const float logicalY,
                                                 Vector3D& planeXyzOut) const;
        
        virtual bool planeXyzToPixelIndex(const Vector3D& planeXyz,
                                          PixelIndex& pixelIndexOut) const;
        
        virtual bool planeXyzToLogicalPixelIndex(const Vector3D& planeXyz,
                                                 PixelLogicalIndex& pixelLogicalIndexOut) const;
        
        virtual bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                              Vector3D& stereotaxicXyzOut) const;
        
        virtual bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                              Vector3D& stereotaxicNoNonLinearXyzOut,
                                              Vector3D& stereotaxicXyzOut) const;
        
        virtual bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeXyzOut) const;
        
        virtual bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeNoNonLinearXyzOut,
                                              Vector3D& planeXyzOut) const;
        
        virtual bool logicalPixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogialIndex,
                                                       Vector3D& stereotaxicXyzOut) const;
        
        virtual bool logicalPixelIndexToStereotaxicXYZ(const float logicalX,
                                                       const float logicalY,
                                                       Vector3D& stereotaxicXyzOut) const;

        virtual void setTransformMatrices(const Matrix4x4& scaledToPlaneMatrix,
                                          const bool scaledToPlaneMatrixValidFlag,
                                          const Matrix4x4& planeToMillimetersMatrix,
                                          const bool planeToMillimetersMatrixValidFlag,
                                          std::shared_ptr<CziNonLinearTransform>& toStereotaxicNonLinearTransform,
                                          std::shared_ptr<CziNonLinearTransform>& fromStereotaxicNonLinearTransform);

        virtual const Plane* getStereotaxicImagePlane() const;
        
        virtual const Plane* getPlaneCoordinatesPlane() const;
        
        virtual QRectF getLogicalBoundsRect() const;
        
        virtual PixelIndex pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const;
        
        virtual PixelLogicalIndex pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const;
        
        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForMediaDrawing(const int32_t tabIndex,
                                                                             const int32_t overlayIndex) const = 0;
        
        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForPlaneXyzDrawing(const int32_t tabIndex,
                                                                                const int32_t overlayIndex) const = 0;
        
        QRectF planeRectToLogicalRect(const QRectF& planeRect) const;
        
        QRectF logicalRectToPlaneRect(const QRectF& logicalRect) const;
        
        QRectF stereotaxicRectToLogicalRect(const QRectF& planeRect) const;
        
        QRectF logicalRectToStereotaxicRect(const QRectF& logicalRect) const;
        
        virtual bool isPlaneXyzSupported() const;
        
        virtual Vector3D getPlaneXyzBottomLeft() const;
        
        virtual Vector3D getPlaneXyzBottomRight() const;
        
        virtual Vector3D getPlaneXyzTopRight() const;
        
        virtual Vector3D getPlaneXyzTopLeft() const;

        virtual BoundingBox getPlaneXyzBoundingBox() const;
        
        virtual BoundingBox getStereotaxicXyzBoundingBox() const;
        
        virtual Vector3D getStereotaxicXyzBottomLeft() const;
        
        virtual Vector3D getStereotaxicXyzBottomRight() const;
        
        virtual Vector3D getStereotaxicXyzTopRight() const;
        
        virtual Vector3D getStereotaxicXyzTopLeft() const;
        
        bool isScaledToPlaneMatrixValid() const;
        
        Matrix4x4 getScaledToPlaneMatrix(bool* validFlagOut = NULL) const;
        
        bool isPlaneToMillimetersMatrixValid() const;
        
        Matrix4x4 getPlaneToMillimetersMatrix(bool* validFlagOut = NULL) const;
        
        void addPlaneCoordsToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        // ADD_NEW_METHODS_HERE
          
    protected: 
        MediaFile(const DataFileTypeEnum::Enum dataFileType);
        
        void copyMediaFileMembers(const MediaFile& mediaFile);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        void initializeMembersMediaFile();
        
        virtual void resetMatrices();
        
        void setMediaFileTransforms(const MediaFileTransforms& mediaFileTransforms);
        
        MediaFileTransforms m_mediaFileTransforms = MediaFileTransforms();
        /**
         * @return Metadata name for scaled to plane matrix
         */
        static QString getMetaDataNameScaledToPlaneMatrix() { return "WorkbenchScaledToPlaneMatrix"; }
        
        /**
         * @return Metadata name of coordinates in image metadata
         */
        static QString getMetaDataNamePlaneToMillimetersMatrix() { return "WorkbenchPlaneToMillimetersMatrix"; }

        MediaFileChannelInfo* getMediaFileChannelInfo();
        
        void replaceMediaFileChannelInfo(const MediaFileChannelInfo& mediaFileChannelInfo);

    private:
        void indexToPlaneTest(const Matrix4x4& scaledToPlane,
                              const Matrix4x4& shiftMat,
                              const Matrix4x4& scaleMat,
                              const Matrix4x4& planeToMM,
                              const int32_t i,
                              const int32_t j,
                              const AString& name);
        
        void lengthsTest(const Matrix4x4& indexToPlane,
                         const int32_t i1,
                         const int32_t j1,
                         const int32_t i2,
                         const int32_t j2,
                         const AString& name);
        
        void indexToPlaneTest(const Matrix4x4& indexToPlane,
                              const Matrix4x4& planeToMM,
                              const int32_t i,
                              const int32_t j,
                              const AString& name);

        void resetMatricesPrivate();
        
        /**
         * Swap the X & Y components of a vector
         * @param vector
         *    Vector whose X & Y components are swapped
         */
        static inline void swapVectorXY(Vector3D& vector) {
            const float temp(vector[0]);
            vector[0] = vector[1];
            vector[1] = temp;
        }
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

        Matrix4x4 m_scaledToPlaneMatrix;
        
        BoundingBox m_planeBoundingBox;
        
        BoundingBox m_stereotaxicXyzBoundingBox;
        
        QRectF m_planeXyzRect;
        
        Vector3D m_planeXyzBottomLeft;
        
        Vector3D m_planeXyzBottomRight;
        
        Vector3D m_planeXyzTopRight;
        
        Vector3D m_planeXyzTopLeft;

        bool m_planeXyzValidFlag = false;
        
        Vector3D m_stereotaxicXyzBottomLeft;
        
        Vector3D m_stereotaxicXyzBottomRight;
        
        Vector3D m_stereotaxicXyzTopRight;
        
        Vector3D m_stereotaxicXyzTopLeft;
        
        mutable std::unique_ptr<Plane> m_stereotaxicPlane;
        
        mutable bool m_stereotaxicPlaneInvalidFlag = false;
        
        mutable std::unique_ptr<Plane> m_planeCoordinatesPlane;
        
        mutable bool m_planeCoordinatesPlaneInvalidFlag = false;
        
        bool m_scaledToPlaneMatrixValidFlag     = false;
        
        mutable std::unique_ptr<MediaFileChannelInfo> m_mediaFileChannelInfo;
        
        friend class CziImage;
    };
    
#ifdef __MEDIA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MEDIA_FILE_DECLARE__

} // namespace
#endif  //__MEDIA_FILE_H__
