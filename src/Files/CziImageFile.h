#ifndef __CZI_IMAGE_FILE_H__
#define __CZI_IMAGE_FILE_H__

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
#include "CziImage.h"
#include "CziImageResolutionChangeModeEnum.h"
#include "EventListenerInterface.h"
#include "SingleChannelPyramidLevelTileAccessor.h"
#include "MediaFile.h"
#include "PixelIndex.h"
#include "Plane.h"
#include "VolumeFile.h"

class QImage;

/*
 * Unable to use: "using namespace libCZI;"
 * Symbols in libCZI namespace clash with Windows symbols.
 * C:\Program Files (x86)\Windows Kits\8.1\include\\um\objidlbase.h(585): error C2872: 'IStream': ambiguous symbol
 * C:\Program Files (x86)\Windows Kits\8.1\include\\um\objidlbase.h(143): note: could be 'libCZI::IStream IStream'
 * C:\Program Files (x86)\Windows Kits\8.1\include\\um\objidlbase.h(143): note: or       'libCZI::IStream'
 */

namespace caret {

    class CziImage;
    class CziImageLoaderBase;
    class CziImageLoaderMultiResolution;
    class GraphicsObjectToWindowTransform;
    class Matrix4x4;
    class RectangleTransform;
    class VolumeSpace;
    
    class CziImageFile : public MediaFile, public EventListenerInterface {
        
    public:        
        CziImageFile();
        
        virtual ~CziImageFile();
        
        CziImageFile(const CziImageFile&) = delete;

        CziImageFile& operator=(const CziImageFile&) = delete;
        
        virtual CziImageFile* castToCziImageFile() override;
        
        virtual const CziImageFile* castToCziImageFile() const override;

        bool isEmpty() const override;
        
        virtual void clearModified() override;
        
        virtual bool isModified() const override;
        
        GiftiMetaData* getFileMetaData() override;
        
        const GiftiMetaData* getFileMetaData() const override;
        
        virtual int32_t getWidth() const override;
        
        virtual int32_t getHeight() const override;

        virtual int32_t getNumberOfFrames() const override;
        
        virtual bool isPixelIndexValid(const int32_t tabIndex, 
                                       const int32_t overlayIndex,
                                       const PixelIndex& pixelIndexOriginAtTopLeft) const override;
        
        virtual bool isPixelIndexValid(const int32_t tabIndex,
                                       const int32_t overlayIndex,
                                       const PixelLogicalIndex& pixelLogicalIndex) const override;
        
        virtual bool isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const;
        
        virtual void getPixelIdentificationTextForFrames(const int32_t tabIndex,
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
                                        const int32_t frameIndex,
                                        const bool allFramesFlag,
                                        const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                        const int32_t manualPyramidLayerIndex,
                                        const GraphicsObjectToWindowTransform* transform);

        virtual GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForMediaDrawing(const int32_t tabIndex,
                                                                             const int32_t overlayIndex) const override;

        virtual bool getPixelRGBA(const int32_t tabIndex,
                                  const int32_t overlayIndex,
                                  const PixelLogicalIndex& pixelLogicalIndex,
                                  uint8_t pixelRGBAOut[4]) const override;
        
        void getPyramidLayerRangeForFrame(const int32_t frameIndex,
                                          const bool allFramesFlag,
                                          int32_t& lowestPyramidLayerIndexOut,
                                          int32_t& highestPyramidLayerIndexOut) const;

        void reloadPyramidLayerInTabOverlay(const int32_t tabIndex,
                                            const int32_t overlayIndex);
        
        virtual void receiveEvent(Event* event) override;
        
        virtual bool pixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                                const bool includeNonlinearFlag,
                                                std::array<float, 3>& xyzOut) const override;
        
        virtual bool stereotaxicXyzToPixelIndex(const std::array<float, 3>& xyz,
                                                const bool includeNonlinearFlag,
                                                PixelLogicalIndex& pixelLogicalIndexOut) const override;
        
        virtual bool findPixelNearestStereotaxicXYZ(const std::array<float, 3>& xyz,
                                                    const bool includeNonLinearFlag,
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
        
        bool exportToImageFile(const QString& imageFileName,
                               const int32_t maximumWidthHeight,
                               const bool includeAlphaFlag,
                               AString& errorMessageOut);
        
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

        class PyramidLayer {
        public:
            PyramidLayer(const int32_t sceneIndex,
                         const libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo layerInfo,
                         const int64_t pixelWidth,
                         const int64_t pixelHeight)
            : m_sceneIndex(sceneIndex),
            m_layerInfo(layerInfo),
            m_pixelWidth(pixelWidth),
            m_pixelHeight(pixelHeight) { }
            
            int32_t m_sceneIndex;
            
            libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo m_layerInfo;
            
            int64_t m_pixelWidth = 0;
            
            int64_t m_pixelHeight = 0;
            
            float m_zoomLevelFromLowestResolutionImage = 1.0;
            
            int64_t m_logicalWidthForImageReading = 0.0;
            
            int64_t m_logicalHeightForImageReading = 0.0;
            
        };
        
        class CziSceneInfo {
        public:
            CziSceneInfo() {
                m_logicalCenter[0] = 0.0;
                m_logicalCenter[1] = 0.0;
                m_logicalCenter[2] = 0.0;
            }
            
            CziSceneInfo(CziImageFile* cziImageFile,
                         const int32_t sceneIndex,
                         const QRectF& logicalRectange,
                         const AString& name)
            : m_parentCziImageFile(cziImageFile),
            m_sceneIndex(sceneIndex),
            m_logicalRectangle(logicalRectange),
            m_name(name) {
                const QPointF center(m_logicalRectangle.center());
                m_logicalCenter[0] = center.x();
                m_logicalCenter[1] = center.y();
                m_logicalCenter[2] = 0.0;
            }
            
            void addPyramidLayer(const PyramidLayer& pyramidLayer) {
                m_pyramidLayers.push_back(pyramidLayer);
            }
            
            int32_t getNumberOfPyramidLayers() const {
                return m_pyramidLayers.size();
            }
            
            void finishSetup(const bool fixMinFactorFlag);
            
            void setLayersZoomFactors();
            
            std::array<int32_t, 2> getPyramidLayerIndexRange() const;
            
            void setPyramidLayerIndexRange();
            
            void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation,
                                                 const AString& sceneInfoName) const;
            
            AString getName() const { return m_name; }
            
            CziImageFile* m_parentCziImageFile = NULL;
            
            int32_t m_sceneIndex = -1;
            
            QRectF m_logicalRectangle;
            
            AString m_name;
            
            float m_logicalCenter[3];
            
            std::vector<PyramidLayer> m_pyramidLayers;
            
            int32_t m_minimumPyramidLayerIndex = 0;
            
            int32_t m_maximumPyramidLayerIndex = 0;
            
            std::shared_ptr<CziImage> m_defaultImage;

            bool m_defaultImageErrorFlag = false;
        };
        
        class NiftiTransform {
        public:
            mutable std::unique_ptr<VolumeFile> m_niftiFile;
            
            mutable std::unique_ptr<Matrix4x4> m_sformMatrix;
            
            mutable bool m_triedToLoadFileFlag = false;
            
            /** Scales pixel index from full resolution to layer used by NIFTI transform */
            mutable float m_pixelScaleI = -1.0f;
            
            /** Scales pixel index from full resolution to layer used by NIFTI transform */
            mutable float m_pixelScaleJ = -1.0f;
            
            /** Volume is oriented left-to-right for first axis (same as CZI image) */
            bool m_xLeftToRightFlag = false;
            
            /** Volume is oriented top-to-bottom for second axis (same as CZI image) */
            bool m_yTopToBottomFlag = false;
        };
        
        class TestTransformResult {
        public:
            TestTransformResult(const PixelLogicalIndex& pixel,
                                const PixelLogicalIndex& pixelTwo,
                                const std::array<float, 3>& xyz,
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
            const std::array<float, 3> m_xyz;
            const int64_t m_dI;
            const int64_t m_dJ;
            const float m_dIJK;
        };

        class TabOverlayInfo {
        public:
            
            TabOverlayInfo(CziImageFile* cziImageFile,
                           const int32_t tabIndex,
                           const int32_t overlayIndex);
            
            ~TabOverlayInfo();
            
            CziImageLoaderBase* getMultiResolutionImageLoader();
            
            const CziImageLoaderBase* getMultiResolutionImageLoader() const;

            void cloneFromOtherTabOverlayInfo(TabOverlayInfo* otherTabOverlayInfo);
            
            void resetContent();
            
            CziImageFile* m_cziImageFile;
            
            const int32_t m_tabIndex;
            
            const int32_t m_overlayIndex;
            
            CziImageResolutionChangeModeEnum::Enum m_imageResolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO2;
            
            std::unique_ptr<CziImageLoaderMultiResolution> m_multiResolutionImageLoader;
        };
        
        bool pixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                        const bool includeNonlinearFlag,
                                        std::array<float, 3>& xyzOut,
                                        std::array<float, 3>& debugPixelIndexOut) const;

        bool stereotaxicXyzToPixelIndex(const std::array<float, 3>& xyz,
                                        const bool includeNonlinearFlag,
                                        PixelLogicalIndex& pixelLogicalIndex,
                                        const std::array<float, 3>& debugPixelIndex) const;
        
        PixelCoordinate getPixelSizeInMillimeters() const;
        
        CziImage* getImageForTabOverlay(const int32_t tabIndex,
                                        const int32_t overlayIndex);
        
        const CziImage* getImageForTabOverlay(const int32_t tabIndex,
                                              const int32_t overlayIndex) const;
        
        void closeFile();
        
        CziImageLoaderBase* getImageLoaderForTabOverlay(const int32_t tabIndex,
                                                        const int32_t overlayIndex);

        const CziImageLoaderBase* getImageLoaderForTabOverlay(const int32_t tabIndex,
                                                              const int32_t overlayIndex) const;
        
        CziImage* readFramePyramidLayerFromCziImageFile(const AString& imageName,
                                                        const int32_t frameIndex,
                                                        const int32_t pyramidLayer,
                                                        const QRectF& rectangleForReadingRect,
                                                        AString& errorMessageOut);

        CziImage* readFromCziImageFile(const AString& imageName,
                                       const QRectF& regionOfInterest,
                                       const QRectF& frameRegionOfInterest,
                                       const int64_t outputImageWidthHeightMaximum,
                                       AString& errorMessageOut);
        
        enum class QImagePixelFormat {
            RGB,
            RGBA
        };
        QImage* createQImageFromBitmapData(const QImagePixelFormat imagePixelFormat,
                                           libCZI::IBitmapData* bitmapData,
                                           AString& errorMessageOut);
        
        void addToMetadataIfNotEmpty(const AString& name,
                                     const AString& text);
        
        void readMetaData();
        
        void createAllFramesPyramidInfo(const libCZI::SubBlockStatistics& subBlockStatistics);
        
        void readPyramidInfo(const libCZI::SubBlockStatistics& subBlockStatistics);
        
        void loadNiftiTransformFile(const AString& filename,
                                    NiftiTransform& transform) const;
        
        void setLayersZoomFactors(CziSceneInfo& cziSceneInfo);
        
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
        
        bool testReadingSmallImage(AString& errorMessageOut);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        Status m_status = Status::CLOSED;
        
        AString m_errorMessage;
        
        std::shared_ptr<libCZI::IStream> m_stream;
        
        std::shared_ptr<libCZI::ICZIReader> m_reader;

        std::shared_ptr<libCZI::ISingleChannelScalingTileAccessor> m_scalingTileAccessor;
        
        std::shared_ptr<libCZI::ISingleChannelPyramidLayerTileAccessor> m_pyramidLayerTileAccessor;
        
        CziSceneInfo m_allFramesPyramidInfo;
        
        std::vector<CziSceneInfo> m_cziScenePyramidInfos;
        
        float m_pixelSizeMmX = 1.0f;
        
        float m_pixelSizeMmY = 1.0f;
        
        float m_pixelSizeMmZ = 1.0f;
        
        mutable std::unique_ptr<GiftiMetaData> m_fileMetaData;
        
        std::unique_ptr<TabOverlayInfo> m_tabOverlayInfo[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS]
                                                        [BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS];
        
        /*
         * Logical rectangle of full-resolution image
         */
        QRectF m_fullResolutionLogicalRect;
        
        mutable NiftiTransform m_pixelToStereotaxicTransform;
        
        mutable NiftiTransform m_stereotaxicToPixelTransform;
        
        mutable std::unique_ptr<Plane> m_imagePlane;
        
        mutable bool m_imagePlaneInvalid = false;
        
        int32_t m_maximumImageDimension = 2048;
        
        static const int32_t s_allFramesIndex;
        
        // ADD_NEW_MEMBERS_HERE

        friend class CziImage;
        friend class CziImageLoaderMultiResolution;
        
    };
    
#ifdef __CZI_IMAGE_FILE_DECLARE__
    const int32_t CziImageFile::s_allFramesIndex = -1;
#endif // __CZI_IMAGE_FILE_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_FILE_H__
