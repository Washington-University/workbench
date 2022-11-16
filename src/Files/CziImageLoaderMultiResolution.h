#ifndef __CZI_IMAGE_LOADER_MULTI_RESOLUTION_H__
#define __CZI_IMAGE_LOADER_MULTI_RESOLUTION_H__

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


#include <QRectF>
#include <memory>

#include "CziImageFile.h"
#include "CziImageLoaderBase.h"
#include "MediaDisplayCoordinateModeEnum.h"

namespace caret {

    class CziImageLoaderMultiResolution : public CziImageLoaderBase {
        
    public:
        CziImageLoaderMultiResolution();
        
        virtual ~CziImageLoaderMultiResolution();
        
        virtual void initialize(const int32_t tabIndex,
                                const int32_t overlayIndex,
                                CziImageFile* cziImageFile) override;
        
        virtual void updateImage(const CziImage* cziImage,
                                 const int32_t frameIndex,
                                 const bool allFramesFlag,
                                 const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                 const MediaDisplayCoordinateModeEnum::Enum coordinateMode,
                                 const int32_t manualPyramidLayerIndex,
                                 const GraphicsObjectToWindowTransform* transform) override;

        virtual void forceImageReloading() override;
        
        virtual CziImage* getImage() override;
        
        virtual const CziImage* getImage() const override;
        
        CziImageLoaderMultiResolution(const CziImageLoaderMultiResolution&) = delete;

        CziImageLoaderMultiResolution& operator=(const CziImageLoaderMultiResolution&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:

        int32_t getLayerIndexForCurrentZoom(const CziImageFile::CziSceneInfo& cziSceneInfo,
                                            const GraphicsObjectToWindowTransform* transform,
                                            const MediaDisplayCoordinateModeEnum::Enum coordinateMode) const;
        
        bool isReloadForPanZoom(const CziImage* cziImage,
                                const GraphicsObjectToWindowTransform* transform,
                                const MediaDisplayCoordinateModeEnum::Enum coordinateMode) const;
        
        CziImage* loadImageForPyrmaidLayer(const CziImage* oldCziImage,
                                           const CziImageFile::CziSceneInfo& cziSceneInfo,
                                           const GraphicsObjectToWindowTransform* transform,
                                           const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                           const MediaDisplayCoordinateModeEnum::Enum coordinateMode,
                                           const int32_t pyramidLayerIndex);

        CziImage* loadImageForPyrmaidLayerForPixelCoords(const CziImage* oldCziImage,
                                                         const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                         const GraphicsObjectToWindowTransform* transform,
                                                         const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                                         const int32_t pyramidLayerIndex);

        CziImage* loadImageForPyrmaidLayerForPlaneCoords(const CziImage* oldCziImage,
                                                         const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                         const GraphicsObjectToWindowTransform* transform,
                                                         const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                                         const int32_t pyramidLayerIndex);
        
        CziImage* loadImageForPyrmaidLayerForStereotaxicCoords(const CziImage* oldCziImage,
                                                               const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                               const GraphicsObjectToWindowTransform* transform,
                                                               const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                                               const int32_t pyramidLayerIndex);

        QRectF getViewportLogicalCoordinates(const GraphicsObjectToWindowTransform* transform,
                                             const MediaDisplayCoordinateModeEnum::Enum coordinateMode) const;
        
        QRectF getViewportLogicalCoordinatesForPixelCoords(const GraphicsObjectToWindowTransform* transform) const;
        
        QRectF getViewportLogicalCoordinatesForPlaneCoords(const GraphicsObjectToWindowTransform* transform) const;
        
        QRectF getViewportLogicalCoordinatesForStereotaxicCoords(const GraphicsObjectToWindowTransform* transform) const;
        
        QRectF getViewportPlaneCoordinates(const GraphicsObjectToWindowTransform* transform) const;
        
        QRectF getViewportStereotaxicCoordinates(const GraphicsObjectToWindowTransform* transform) const;
        
        std::shared_ptr<CziImage> m_cziImage;

        int32_t m_tabIndex;
        
        int32_t m_overlayIndex;
        
        CziImageFile* m_cziImageFile;
        
        int32_t m_previousFrameIndex = -1;
        
        bool m_previousAllFramesFlag = false;

        CziImageResolutionChangeModeEnum::Enum m_previousResolutionChangeMode = CziImageResolutionChangeModeEnum::INVALID;
        
        MediaDisplayCoordinateModeEnum::Enum m_previousCoordinateMode = MediaDisplayCoordinateModeEnum::PLANE;
        
        int32_t m_previousManualPyramidLayerIndex = -1;

        int32_t m_previousZoomLayerIndex = -1;
        
        bool m_forceImageReloadFlag = false;
        
        bool m_reloadImageFlag = false;
        
        bool m_frameChangedFlag = false;
        
        /*
         * Note CZI_BITMAP does not support alpha channel that is needed for distance/masking file alpha values.
         * If QImage is used, gluBuild2DMipmaps() will crash when the Mesa3D library is used.  This may be due to
         * the way QImage stores data; perhaps when number of pixels in a row is not a multiple of 4.
         *
         * If QImage is used, will need to copy image data to contiguous storage.  If CZI_BITMAP is used, will
         * need to copy data so that an alpha channel can be added.
         *
         * When data is loaded, we should load the data so that both the width and height are powers of two.  When
         * OpenGL builds mipmaps, it will accept non-powers-of-two for width and height.  However it will resize the
         * image and it map downsample the data.  From documentation for gluBuild2DMipmaps(): Initially, the
         * width and height of data are checked to see if they are a power of 2. If not, a copy of data (not data),
         * is scaled up or down to the nearest power of 2. This copy will be used for subsequent mipmapping
         * operations described below. (If width or height is exactly between powers of 2, then the copy of data
         * will scale upwards.) For example, if width is 57 and height is 23, then a copy of data will scale up to
         * 64 in width and down to 16 in depth, before mipmapping takes place.
         *
         * See https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluBuild2DMipmaps.xml
         *
         * static const CziImageFile::ImageDataFormat s_imageDataFormatForReading = CziImageFile::ImageDataFormat::Q_IMAGE
         */
        static const CziImageFile::ImageDataFormat s_imageDataFormatForReading = CziImageFile::ImageDataFormat::CZI_BITMAP;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_LOADER_MULTI_RESOLUTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_LOADER_MULTI_RESOLUTION_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_LOADER_MULTI_RESOLUTION_H__
