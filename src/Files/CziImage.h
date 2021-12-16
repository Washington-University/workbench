#ifndef __CZI_IMAGE_H__
#define __CZI_IMAGE_H__

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

#include "CaretObject.h"
#include "CziImageResolutionChangeModeEnum.h"
#include "CziPixelCoordSpaceEnum.h"
#include "PixelIndex.h"
#include "PixelLogicalIndex.h"
#include "SceneableInterface.h"

class QImage;

namespace caret {
    class CziImageFile;
    class GraphicsPrimitiveV3fT2f;
    class PixelIndex;
    class RectangleTransform;
    class SceneClassAssistant;

    class CziImage : public CaretObject, public SceneableInterface {
        
    public:
        CziImage(const CziImageFile* parentCziImageFile,
                 QImage* image,
                 const QRectF& fullResolutionLogicalRect,
                 const QRectF& imageDataLogicalRect,
                 const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                 const int32_t resolutionChangeModeLevel);
        
        virtual ~CziImage();
        
        CziImage(const CziImage&) = delete;

        CziImage& operator=(const CziImage&) = delete;
        
        GraphicsPrimitiveV3fT2f* getGraphicsPrimitiveForMediaDrawing() const;
        
        PixelIndex transformPixelIndexToSpace(const PixelIndex& pixelIndex,
                                              const CziPixelCoordSpaceEnum::Enum fromPixelCoordSpace,
                                              const CziPixelCoordSpaceEnum::Enum toPixelCoordSpace) const;

        void getPixelIdentificationText(const AString& filename,
                                        const PixelLogicalIndex& pixelLogicalIndex,
                                        std::vector<AString>& columnOneTextOut,
                                        std::vector<AString>& columnTwoTextOut,
                                        std::vector<AString>& toolTipTextOut) const;

        bool isPixelIndexValid(const PixelIndex& pixelIndex) const;

        bool isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const;
        
        bool getPixelRGBA(const PixelLogicalIndex& pixelLogicalIndex,
                          uint8_t pixelRGBAOut[4]) const;

        int32_t getWidth() const;
        
        int32_t getHeight() const;
        
        virtual PixelIndex pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const;
        
        virtual PixelLogicalIndex pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        const CziImageFile* m_parentCziImageFile;
        
        std::unique_ptr<QImage> m_image;
        
        /**
         * This rectangle defines the original, full-resolution source image logcial rectangle
         * Coordinates are in the 'space' from SubBlockStatistics::boundingBox.
         * For example: boundingBox = (x = -61920, y = 5391, w = 59229, h = 45513).
         * Do not know where this origin comes from.
         * The origin is at the top left, X increases to the right, Y increases downward.
         */
        const QRectF m_fullResolutionLogicalRect;
        
        /**
         * This "logical rectangle" defines the region of this image in the source file.
         * Coordinates are in the 'space' from SubBlockStatistics::boundingBox.
         * For example: boundingBox = (x = -61920, y = 5391, w = 59229, h = 45513).
         * The bounds are accumulated from all sub blocks
         * The origin is at the top left, X increases to the right, Y increases downward.
         */
        const QRectF m_imageDataLogicalRect;
        
        /**
         * This rectangle defines the pixels of the full-resolution image with (0, 0) in corner
         * and w/h is same as full resolution logical w/h
         */
        QRectF m_fullResolutionPixelsRect;
        
        /**
         * This rectangle defines the pixels of this image with (0, 0) in corner
         */
        QRectF m_imagePixelsRect;
        
        /**
         * Resolution change mode that created this image (INVALID is default mode)
         */
        const CziImageResolutionChangeModeEnum::Enum m_resolutionChangeMode;
        
        /**
         * Resolution chnage mode level for this image
         */
        const int32_t m_resolutionChangeModeLevel;
        
        /**
         * This rectangle defines the region of this image in m_fullResolutionPixelsRect
         */
        QRectF m_pixelsRegionOfInterestRect;
        
        mutable std::unique_ptr<GraphicsPrimitiveV3fT2f> m_graphicsPrimitiveForMediaDrawing;
        
        friend class CziImageFile;
        friend class CziImageLoaderAllFrames;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_H__
