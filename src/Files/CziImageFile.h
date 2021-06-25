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



#include <memory>

#include <QRectF>

#include "MediaFile.h"
#include "libCZI.h"

using namespace libCZI;

class QImage;

namespace caret {

    class GraphicsPrimitiveV3fT3f;
    class RectangleTransform;
    class VolumeSpace;
    
    class CziImageFile : public MediaFile {
        
    public:
        /**
         * Data for an Region of Interest image read from a CZI file
         */
        class CziImageROI {
        public:
            CziImageROI(const AString& filename,
                        QImage* image,
                        const QRectF& sourceImageRect,
                        const QRectF& roiRect,
                        MediaFile::SpatialInfo& spatialInfo);
            
            ~CziImageROI();
            
            CziImageROI(const CziImageROI&) = delete;
            
            CziImageROI& operator=(const CziImageROI&) = delete;
            
            GraphicsPrimitiveV3fT3f* getGraphicsPrimitiveForMediaDrawing() const;
            
        private:
            const AString m_filename;
            
            std::unique_ptr<QImage> m_image;
            
            /**
             * This rectangle defines the original, full-resolution source image
             * Coordinates are in the 'space' from SubBlockStatistics::boundingBox.
             * For example: boundingBox = (x = -61920, y = 5391, w = 59229, h = 45513).
             * Do not know where this origin comes from.
             * The origin is at the top left, X increases to the right, Y increases downward.
             */
            const QRectF m_sourceImageRect;
            
            /**
             * This rectangle defines the region from the source image for this image.
             * Coordinates are in the 'space' from SubBlockStatistics::boundingBox.
             * For example: boundingBox = (x = -61920, y = 5391, w = 59229, h = 45513).
             * Do not know where this origin comes from.
             * The origin is at the top left, X increases to the right, Y increases downward.
             */
            const QRectF m_roiRect;
            
            /**
             * Converts from pixel index to pixel coordinate
             */
            std::unique_ptr<VolumeSpace> m_pixelToCoordinateTransform;
            
            /**
             * Bounds of the image
             */
            std::unique_ptr<BoundingBox> m_spatialBoundingBox;
            
            /**
             * Converts between the region of interest coordinates in the image (m_image) to
             * pixel coordinates in the image.
             * The ROI coordinates have the origin in the top left (origin is rarely 0, 0)
             * The pixel coordinate have the origin (0,0) at the top left and range to the
             * size of the image (pixel width - 1, pixel height - 1)
             */
            std::unique_ptr<RectangleTransform> m_roiCoordsToRoiPixelTopLeftTransform;
            
            /**
             * Converts a pixel index in the ROI Image (m_image) with origin at the top left and size
             * (width - 1, height - 1) to the pixel index in the full image with origin at the top left
             */
            std::unique_ptr<RectangleTransform> m_roiPixelTopLeftToFullImagePixelTopLeftTransform;
            
            mutable std::unique_ptr<GraphicsPrimitiveV3fT3f> m_graphicsPrimitiveForMediaDrawing;
            
            /*
             * Maps from pixels
             */
            //std::unique_ptr<VolumeSpace> m_pixelsToCziBoundingBox;
            
            friend class CziImageFile;
        };
        
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
        
        virtual int32_t getNumberOfFrames() const override;
        
        virtual DefaultViewTransform getDefaultViewTransform(const int32_t tabIndex) const override;

        virtual const BoundingBox* getSpatialBoundingBox(const int32_t tabIndex) const override;
        
        virtual const VolumeSpace* getPixelToCoordinateTransform(const int32_t tabIndex) const override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        virtual bool supportsWriting() const override;
        
        CziImageROI* getImageForTab(const int32_t tabIndex);
        
        const CziImageROI* getImageForTab(const int32_t tabIndex) const;

        bool getImagePixelRGBA(const int32_t tabIndex,
                               const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                               const PixelIndex& pixelIndex,
                               uint8_t pixelRGBAOut[4]) const;
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        enum class Status {
            CLOSED,
            OPEN,
            ERROR
        };

        class PyramidLayer {
        public:
            PyramidLayer(const ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo layerInfo,
                         const int64_t width,
                         const int64_t height)
            : m_layerInfo(layerInfo),
            m_width(width),
            m_height(height) { }
            
            const ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo m_layerInfo;
            
            const int64_t m_width;
            
            const int64_t m_height;
        };
        CziImageROI* getDefaultImage();
        
        const CziImageROI* getDefaultImage() const;
        
        void closeFile();
        
        CziImageROI* readPyramidLevelFromCziImageFile(const int32_t pyramidLevel,
                                                      AString& errorMessageOut);
        
        CziImageROI* readFromCziImageFile(const QRectF& regionOfInterest,
                                          const int64_t outputImageWidthHeightMaximum,
                                          AString& errorMessageOut);
        
        QImage* createQImageFromBitmapData(IBitmapData* bitmapData,
                                           AString& errorMessageOut);
        
        void addToMetadataIfNotEmpty(const AString& name,
                                     const AString& text);
        
        void readMetaData();
        
        void readPyramidInfo(const int64_t imageWidth,
                             const int64_t imageHeight);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        Status m_status = Status::CLOSED;
        
        AString m_errorMessage;
        
        std::shared_ptr<IStream> m_stream;
        
        std::shared_ptr<ICZIReader> m_reader;

        std::shared_ptr<ISingleChannelScalingTileAccessor> m_scalingTileAccessor;
        
        std::shared_ptr<ISingleChannelPyramidLayerTileAccessor> m_pyramidLayerTileAccessor;
        
        int32_t m_numberOfPyramidLayers = 0;
        
        float m_pixelSizeMmX = 1.0f;
        
        float m_pixelSizeMmY = 1.0f;
        
        float m_pixelSizeMmZ = 1.0f;
        
        mutable DefaultViewTransform m_defaultViewTransform;
        
        mutable bool m_defaultViewTransformValidFlag = false;

        mutable std::unique_ptr<GiftiMetaData> m_fileMetaData;
        
        std::unique_ptr<CziImageROI> m_defaultImage;
        
        /*
         * Bounds as reported by CZI statistics.
         * Origin is in the upper left.
         */
        QRectF m_sourceImageRect;
        
        std::vector<PyramidLayer> m_pyramidLayers;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_FILE_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_FILE_H__
