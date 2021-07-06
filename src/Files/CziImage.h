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
#include "CziPixelCoordSpaceEnum.h"
#include "PixelIndex.h"
#include "SceneableInterface.h"

class QImage;

namespace caret {
    class CziImageFile;
    class GraphicsPrimitiveV3fT3f;
    class PixelIndex;
    class RectangleTransform;
    class SceneClassAssistant;

    class CziImage : public CaretObject, public SceneableInterface {
        
    public:
        CziImage(const CziImageFile* parentCziImageFile,
                 QImage* image,
                 const QRectF& fullResolutionLogicalRect,
                 const QRectF& logicalRect);
        
        virtual ~CziImage();
        
        CziImage(const CziImage&) = delete;

        CziImage& operator=(const CziImage&) = delete;
        
        GraphicsPrimitiveV3fT3f* getGraphicsPrimitiveForMediaDrawing() const;
        
        PixelIndex transformPixelIndexToSpace(const PixelIndex& pixelIndex,
                                              const CziPixelCoordSpaceEnum::Enum fromPixelCoordSpace,
                                              const CziPixelCoordSpaceEnum::Enum toPixelCoordSpace) const;

        void getPixelIdentificationText(const AString& filename,
                                        const PixelIndex& pixelIndex,
                                        std::vector<AString>& columnOneTextOut,
                                        std::vector<AString>& columnTwoTextOut,
                                        std::vector<AString>& toolTipTextOut) const;


        bool isPixelIndexValid(const PixelIndex& pixelIndex) const;

        bool getImagePixelRGBA(const PixelIndex& pixelIndex,
                               uint8_t pixelRGBAOut[4]) const;
        
        int32_t getWidth() const;
        
        int32_t getHeight() const;
        
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
        const QRectF m_logicalRect;
        
        /**
         * This rectangle defines the pixels of the full-resolution image
         */
        QRectF m_fullResolutionPixelsRect;
        
        /**
         * This rectangle defines the pixels of this image
         */
        QRectF m_pixelsRect;
        
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
        
        friend class CziImageFile;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CZI_IMAGE_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_H__
