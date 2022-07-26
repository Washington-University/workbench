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
        
        virtual void getPixelLogicalIdentificationTextForFrames(const int32_t tabIndex,
                                                                const std::vector<int32_t>& frameIndices,
                                                                const PixelLogicalIndex& pixelLogicalIndex,
                                                                std::vector<AString>& columnOneTextOut,
                                                                std::vector<AString>& columnTwoTextOut,
                                                                std::vector<AString>& toolTipTextOut) const = 0;

        virtual void getPixelPlaneIdentificationTextForFrames(const int32_t tabIndex,
                                                                const std::vector<int32_t>& frameIndices,
                                                                const Vector3D& planeCoordinate,
                                                                std::vector<AString>& columnOneTextOut,
                                                                std::vector<AString>& columnTwoTextOut,
                                                                std::vector<AString>& toolTipTextOut) const;
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
        
        QRectF getPlaneXyzRect() const;
        
        virtual bool pixelIndexToPlaneXYZ(const PixelIndex& pixelIndex,
                                          Vector3D& planeXyzOut) const;
        
        virtual bool logicalPixelIndexToPlaneXYZ(const PixelLogicalIndex& pixelLogialIndex,
                                                 Vector3D& planeXyzOut) const;
        
        virtual bool planeXyzToPixelIndex(const Vector3D& planeXyz,
                                          PixelIndex& pixelIndexOut) const;
        
        virtual bool planeXyzToLogicalPixelIndex(const Vector3D& planeXyz,
                                                 PixelLogicalIndex& pixelLogicalIndexOut) const;
        
        virtual bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                              Vector3D& stereotaxicXyzOut) const;
        
        virtual bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeXyzOut) const;
        
        virtual void setScaledToPlaneMatrix(const Matrix4x4& scaledToPlaneMatrix,
                                            const Matrix4x4& planeToMillimetersMatrix,
                                            const bool matixValidFlag);

        virtual QRectF getLogicalBoundsRect() const;
        
        virtual PixelIndex pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const;
        
        virtual PixelLogicalIndex pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const;
        
        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForMediaDrawing(const int32_t tabIndex,
                                                                             const int32_t overlayIndex) const = 0;
        
        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForPlaneXyzDrawing(const int32_t tabIndex,
                                                                                const int32_t overlayIndex) const = 0;
        
        virtual bool isPlaneXyzSupported() const;
        
        virtual Vector3D getPlaneXyzBottomLeft() const;
        
        virtual Vector3D getPlaneXyzBottomRight() const;
        
        virtual Vector3D getPlaneXyzTopRight() const;
        
        virtual Vector3D getPlaneXyzTopLeft() const;

        virtual BoundingBox getPlaneXyzBoundingBox() const;
        
        Matrix4x4 getScaledToPlaneMatrix() const;
        
        Matrix4x4 getPixelIndexToPlaneMatrix() const;
        
        Matrix4x4 getPlaneToPixelIndexMatrix() const;
        
        Matrix4x4 getPlaneToMillimetersMatrix() const;
        
        Matrix4x4 getMillimetersToPlaneMatrix() const;
        
        // ADD_NEW_METHODS_HERE
          
    protected: 
        MediaFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        void initializeMembersMediaFile();
        
        virtual void resetMatrices();
        
        /**
         * @return Metadata name for scaled to plane matrix
         */
        static QString getMetaDataNameScaledToPlaneMatrix() { return "WorkbenchScaledToPlaneMatrix"; }
        
        /**
         * @return Metadata name of coordinates in image metadata
         */
        static QString getMetaDataNamePlaneToMillimetersMatrix() { return "WorkbenchPlaneToMillimetersMatrix"; }

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
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

        friend class CziImage;
        
        Matrix4x4 m_scaledToPlaneMatrix;
        
        Matrix4x4 m_pixelIndexToPlaneMatrix;
        
        Matrix4x4 m_planeToPixelIndexMatrix;
        
        Matrix4x4 m_planeToMillimetersMatrix;
        
        Matrix4x4 m_millimetersToPlaneMatrix;
        
        BoundingBox m_planeBoundingBox;
        
        QRectF m_planeXyzRect;
        
        Vector3D m_planeXyzBottomLeft;
        
        Vector3D m_planeXyzBottomRight;
        
        Vector3D m_planeXyzTopRight;
        
        Vector3D m_planeXyzTopLeft;

        bool m_scaledToPlaneMatrixValidFlag     = false;
        
        bool m_pixelIndexToPlaneMatrixValidFlag = false;
        
        bool m_planeToPixelIndexMatrixValidFlag = false;
        
        bool m_planeToMillimetersMatrixValidFlag = false;
        
        bool m_millimetersToPlaneMatrixValidFlag = false;
    };
    
#ifdef __MEDIA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MEDIA_FILE_DECLARE__

} // namespace
#endif  //__MEDIA_FILE_H__
