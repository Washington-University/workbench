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
                                            const int32_t frameIndex,
                                            const bool allFramesFlag,
                                            const GraphicsObjectToWindowTransform* transform) const;
        
        bool isReloadForPanZoom(const CziImage* cziImage,
                                const GraphicsObjectToWindowTransform* transform) const;
        
        CziImage* loadImageForPyrmaidLayer(const CziImage* oldCziImage,
                                           const CziImageFile::CziSceneInfo& cziSceneInfo,
                                           const GraphicsObjectToWindowTransform* transform,
                                           const int32_t pyramidLayerIndex);

        QRectF getViewportLogicalCoordinates(const GraphicsObjectToWindowTransform* transform) const;
        
        std::unique_ptr<CziImage> m_cziImage;
        
        int32_t m_tabIndex;
        
        int32_t m_overlayIndex;
        
        CziImageFile* m_cziImageFile;
        
        int32_t m_previousFrameIndex = -1;
        
        bool m_previousAllFramesFlag = false;

        CziImageResolutionChangeModeEnum::Enum m_previousResolutionChangeMode = CziImageResolutionChangeModeEnum::INVALID;
        
        int32_t m_previousManualPyramidLayerIndex = -1;

        int32_t m_previousZoomLayerIndex = -1;
        
        bool m_reloadImageFlag = false;
        
        bool m_frameChangedFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_LOADER_MULTI_RESOLUTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_LOADER_MULTI_RESOLUTION_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_LOADER_MULTI_RESOLUTION_H__
