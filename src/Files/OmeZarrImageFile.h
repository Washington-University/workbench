#ifndef __OME_ZARR_IMAGE_FILE_H__
#define __OME_ZARR_IMAGE_FILE_H__

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


#include <array>
#include <memory>

#include <QRectF>

#include "BrainConstants.h"
#include "FunctionResult.h"
#include "CziImageResolutionChangeModeEnum.h"
#include "EventListenerInterface.h"
#include "FunctionResult.h"
#include "Matrix4x4.h"
#include "MediaFile.h"
#include "MediaFileChannelInfo.h"

#include "PixelIndex.h"
#include "Plane.h"

class QImage;

/*
 * Unable to use: "using namespace libCZI;"
 * Symbols in libCZI namespace clash with Windows symbols.
 * C:\Program Files (x86)\Windows Kits\8.1\include\\um\objidlbase.h(585): error C2872: 'IStream': ambiguous symbol
 * C:\Program Files (x86)\Windows Kits\8.1\include\\um\objidlbase.h(143): note: could be 'libCZI::IStream IStream'
 * C:\Program Files (x86)\Windows Kits\8.1\include\\um\objidlbase.h(143): note: or       'libCZI::IStream'
 */

namespace caret {

    class GraphicsObjectToWindowTransform;
    class GraphicsPrimitiveV3fT2f;
    class Matrix4x4;
    class OmeFileReader;
    class OmeDataSet;
    class OmeImage;
    class RectangleTransform;
    class VolumeFile;
    class VolumeSpace;
    
    class OmeZarrImageFile : public MediaFile, public EventListenerInterface {
        
    public:        
        OmeZarrImageFile();
        
        virtual ~OmeZarrImageFile();
        
        OmeZarrImageFile(const OmeZarrImageFile&) = delete;

        OmeZarrImageFile& operator=(const OmeZarrImageFile&) = delete;
        
        virtual OmeZarrImageFile* castToOmeZarrImageFile() override;
        
        virtual const OmeZarrImageFile* castToOmeZarrImageFile() const override;

        bool isEmpty() const override;
        
        virtual void clearModified() override;
        
        virtual bool isModified() const override;
        
        GiftiMetaData* getFileMetaData() override;
        
        const GiftiMetaData* getFileMetaData() const override;
        
        virtual int32_t getWidth() const override;
        
        virtual int32_t getHeight() const override;

        virtual int32_t getNumberOfFrames() const override;
        
        virtual bool isPixelIndexInFrameValid(const int32_t frameIndex,
                                              const PixelIndex& pixelIndexOriginAtTopLeft) const override;
        
        virtual bool isPixelIndexInFrameValid(const int32_t frameIndex,
                                              const PixelLogicalIndex& pixelLogicalIndex) const override;

        virtual bool isPixelIndexValid(const PixelIndex& pixelIndexOriginAtTopLeft) const override;
        
        virtual bool isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const override;
        
        virtual void getPixelLogicalIdentificationTextForFrames(const int32_t tabIndex,
                                                         const std::vector<int32_t>& frameIndices,
                                                         const PixelLogicalIndex& pixelLogicalIndex,
                                                         std::vector<AString>& columnOneTextOut,
                                                         std::vector<AString>& columnTwoTextOut,
                                                         std::vector<AString>& toolTipTextOut) const override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        virtual bool supportsWriting() const override;
        
        int32_t getNumberOfScenes() const;
        
        void updateImageForDrawingInTab(const int32_t tabIndex,
                                        const int32_t overlayIndex,
                                        const int32_t sliceIndex,
                                        const int32_t frameIndex,
                                        const int32_t pyramidLevel,
                                        const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode);

        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForMediaDrawing(const int32_t tabIndex,
                                                                             const int32_t overlayIndex) const override;

        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForPlaneXyzDrawing(const int32_t tabIndex,
                                                                                const int32_t overlayIndex) const override;
        
        virtual bool getPixelRGBA(const int32_t tabIndex,
                                  const int32_t overlayIndex,
                                  const PixelLogicalIndex& pixelLogicalIndex,
                                  uint8_t pixelRGBAOut[4]) const override;
        
        int32_t getNumberOfPyramidLevels() const;
        
        virtual bool getPyrimidLevelDimensions(const int32_t pyramidLevel,
                                               std::vector<int64_t>& dimensionsOut) const override;
        
        virtual void getPyramidLayerRangeForFrame(const int32_t frameIndex,
                                                  const bool allFramesFlag,
                                                  int32_t& lowestPyramidLayerIndexOut,
                                                  int32_t& highestPyramidLayerIndexOut) const override;

        FunctionResultValue<Matrix4x4> getPyramidLevelTransformationMatrix(const int32_t pyramidLevel) const;
        
        virtual void reloadPyramidLayerInTabOverlay(const int32_t tabIndex,
                                                    const int32_t overlayIndex) override;
        
        virtual void receiveEvent(Event* event) override;
        
        virtual bool findPixelNearestStereotaxicXYZ(const Vector3D& xyz,
                                                    float& signedDistanceToPixelMillimetersOut,
                                                    PixelLogicalIndex& pixelLogicalIndexOut) const override;
        
        void testPixelTransforms(const int32_t pixelIndexStep,
                                 const bool nonLinearFlag,
                                 const bool verboseFlag,
                                 AString& resultsMessageOut,
                                 QImage& imageOut) const;
        
        virtual PixelIndex pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const override;
        
        virtual PixelLogicalIndex pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const override;
        
        virtual QRectF getLogicalBoundsRect() const override;
        
        Vector3D getModelviewScaling(const int32_t tabIndex,
                                     const int32_t overlayIndex) const;

        VolumeFile* getImagesAsRgbaVolumeFile() const;
        
        FunctionResultValue<VolumeFile*> exportToVolumeFile(const int32_t pyramidLevel) const;
        
        bool exportToImageFile(const QString& imageFileName,
                               const int32_t maximumWidthHeight,
                               AString& errorMessageOut);
        
        virtual AString toString() const override;
        
        // ADD_NEW_METHODS_HERE

          
          
    protected: 
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        enum class Status {
            CLOSED,
            OPEN,
            ERRORED  /* Note: "ERROR" fails to compile on Windows */
        };
        
        class PyramidLevelInfo {
        public:
            PyramidLevelInfo(const int64_t pixelWidth,
                             const int64_t pixelHeight,
                             const int64_t pixelSlices,
                             const Vector3D spatialOriginXYZ,
                             const Vector3D spatialSizeXYZ)
            : m_pixelWidth(pixelWidth),
            m_pixelHeight(pixelHeight),
            m_pixelSlices(pixelSlices),
            m_spatialOriginXYZ(spatialOriginXYZ),
            m_spatialSizeXYZ(spatialSizeXYZ),
            m_logicalRectangle(0, 0, pixelWidth, pixelHeight) { }
            
            const int64_t m_pixelWidth;
            const int64_t m_pixelHeight;
            const int64_t m_pixelSlices;
            const Vector3D m_spatialOriginXYZ;
            const Vector3D m_spatialSizeXYZ;
            const QRectF m_logicalRectangle;
        };
        
    class TestTransformResult {
        public:
            TestTransformResult(const PixelLogicalIndex& pixel,
                                const PixelLogicalIndex& pixelTwo,
                                const Vector3D& xyz,
                                const int64_t dI,
                                const int64_t dJ,
                                const float dIJK)
            : m_pixel(pixel),
            m_pixelTwo(pixelTwo),
            m_xyz(xyz),
            m_dI(dI),
            m_dJ(dJ),
            m_dIJK(dIJK) { }

            const PixelLogicalIndex m_pixel;
            const PixelLogicalIndex m_pixelTwo;
            const Vector3D m_xyz;
            const int64_t m_dI;
            const int64_t m_dJ;
            const float m_dIJK;
        };

        class TabOverlayInfo {
        public:
            
            TabOverlayInfo(OmeZarrImageFile* omeZarrImageFile,
                           const int32_t tabIndex,
                           const int32_t overlayIndex);
            
            ~TabOverlayInfo();
            
            void cloneFromOtherTabOverlayInfo(TabOverlayInfo* otherTabOverlayInfo);
            
            void resetContent();
            
            OmeZarrImageFile* m_omeZarrImageFile;
            
            const int32_t m_tabIndex;
            
            const int32_t m_overlayIndex;
            
            int32_t m_sliceIndex = 0;
            
            int32_t m_frameIndex = 0;
            
            int32_t m_pyramidLevel = 0;
            
            CziImageResolutionChangeModeEnum::Enum m_resolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO2;
            
            std::unique_ptr<GraphicsPrimitiveV3fT2f> m_graphicsPrimitive;
        };
        
        GraphicsPrimitiveV3fT2f* createGraphicsPrimitive(const OmeImage* omeImage) const;
        
        GraphicsPrimitiveV3fT2f* createGraphicsPrimitive(const OmeDataSet* dataSet,
                                                         const int64_t sliceIndex) const;
        
        PixelCoordinate getPixelSizeInMillimeters() const;
        
        void closeFile();
        
        void addToMetadataIfNotEmpty(const AString& name,
                                     const AString& text);
        
        const Plane* getImagePlane() const;
        
        void resetPrivate();
        
        int32_t getPreferencesImageDimension() const;
        
        std::array<float, 3> getPreferencesImageBackgroundFloatRGB() const;
        
        std::array<uint8_t, 3> getPreferencesImageBackgroundByteRGB() const;
        
        void zoomToMatchPixelDimension(const QRectF& regionOfInterestToRead,
                                       const QRectF& fullRegionOfInterest,
                                       const float maximumPixelWidthOrHeight,
                                       QRectF& regionToReadOut,
                                       float& zoomOut) const;
        
        std::vector<PyramidLevelInfo> m_pyramidLevels;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        mutable Status m_status = Status::CLOSED;
        
        AString m_errorMessage;
        
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
        std::unique_ptr<OmeFileReader> m_omeFileReader;
#endif
        
        float m_pixelSizeMmX = 1.0f;
        
        float m_pixelSizeMmY = 1.0f;
        
        float m_pixelSizeMmZ = 1.0f;
        
        mutable std::unique_ptr<GiftiMetaData> m_fileMetaData;
        
        mutable std::unique_ptr<TabOverlayInfo> m_tabOverlayInfo[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS]
                                                                [BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS];
        
        /*
         * Logical rectangle of full-resolution image
         */
        QRectF m_fullResolutionLogicalRect;
        
        mutable std::unique_ptr<Plane> m_imagePlane;
        
        mutable bool m_imagePlaneInvalid = false;
        
        int32_t m_maximumImageDimension = 2048;
        
        mutable std::unique_ptr<VolumeFile> m_imagesAsVolumeFile;
        
        mutable bool m_triedToCreateImagesAsVolumeFileFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_ZARR_IMAGE_FILE_DECLARE__
//    const int32_t OmeZarrImageFile::s_allFramesIndex = -1;
#endif // __OME_ZARR_IMAGE_FILE_DECLARE__

} // namespace
#endif  //__OME_ZARR_IMAGE_FILE_H__
