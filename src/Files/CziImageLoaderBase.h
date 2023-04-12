#ifndef __CZI_IMAGE_LOADER_BASE_H__
#define __CZI_IMAGE_LOADER_BASE_H__

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

#include "CaretObject.h"
#include "CziImageResolutionChangeModeEnum.h"
#include "MediaDisplayCoordinateModeEnum.h"

class QRectF;

namespace caret {
    
    class CziImage;
    class CziImageFile;
    class GraphicsObjectToWindowTransform;
    
    class CziImageLoaderBase : public CaretObject {
        
    public:
        CziImageLoaderBase();
        
        virtual ~CziImageLoaderBase();
        
        CziImageLoaderBase(const CziImageLoaderBase&) = delete;

        CziImageLoaderBase& operator=(const CziImageLoaderBase&) = delete;
        
        virtual void initialize(const int32_t tabIndex,
                                const int32_t overlayIndex,
                                CziImageFile* cziImageFile) = 0;

        virtual void updateImage(const CziImage* cziImage,
                                 const int32_t frameIndex,
                                 const bool allFramesFlag,
                                 const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                 const MediaDisplayCoordinateModeEnum::Enum coordinateMode,
                                 const int32_t channelIndex,
                                 const int32_t manualPyramidLayerIndex,
                                 const GraphicsObjectToWindowTransform* transform) = 0;

        virtual void forceImageReloading() = 0;
        
        virtual CziImage* getImage() = 0;
        
        virtual const CziImage* getImage() const = 0;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    protected:
        QRectF moveAndClipRectangle(const QRectF& referenceRectangle,
                                    const QRectF& rectangleToClipIn) const;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_LOADER_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_LOADER_BASE_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_LOADER_BASE_H__
