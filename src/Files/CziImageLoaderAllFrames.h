#ifndef __CZI_IMAGE_LOADER_ALL_FRAMES_H__
#define __CZI_IMAGE_LOADER_ALL_FRAMES_H__

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

#include "CziImageLoaderBase.h"



namespace caret {

    class CziImageLoaderAllFrames : public CziImageLoaderBase {
        
    public:
        CziImageLoaderAllFrames();
        
        virtual ~CziImageLoaderAllFrames();
        
        virtual void initialize(CziImageFile* cziImageFile) override;
        
        virtual CziImage* loadNewData(const CziImage* cziImage,
                                      const int32_t tabIndex,
                                      const int32_t overlayIndex,
                                      const int32_t frameIndex,
                                      const bool allFramesFlag,
                                      const GraphicsObjectToWindowTransform* transform) override;
        
        CziImageLoaderAllFrames(const CziImageLoaderAllFrames&) = delete;

        CziImageLoaderAllFrames& operator=(const CziImageLoaderAllFrames&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        /*
         * Info about a "full resolution" layer
         */
        class FullResolutionLayer {
        public:
            FullResolutionLayer(const int64_t width,
                                const int64_t height,
                                const float pixelScale)
            : m_width(width),
            m_height(height),
            m_pixelScale(pixelScale) { }
            
            /** logical width covered by this layer (lower resolution covers larger region */
            int64_t m_width = 0;
            
            /** logical height coverted by this layer */
            int64_t m_height = 0;
            
            /** size of pixel relative to previous layer */
            float m_pixelScale = 1.0;
        };
        
        int32_t getLayerIndexForHeight(const int32_t logicalHeight) const;
        
        CziImage* loadImageForLayer(const CziImage* oldCziImage,
                                    const int32_t tabIndex,
                                    const int32_t overlayIndex,
                                    const GraphicsObjectToWindowTransform* transform,
                                    const int32_t fullResolutionLayerIndexIn);
        
        void pixelSizeToLogicalSize(const int32_t pyramidLayerIndex,
                                    int32_t& widthInOut,
                                    int32_t& heightInOut) const;

        CziImageFile* m_cziImageFile;
        
        QRectF m_fullResolutionLogicalRect;
        
        std::vector<FullResolutionLayer> m_fullResolutionLayers;
        
        int32_t m_numberOfFullResolutionLayers = 0;
        

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_LOADER_ALL_FRAMES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_LOADER_ALL_FRAMES_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_LOADER_ALL_FRAMES_H__
