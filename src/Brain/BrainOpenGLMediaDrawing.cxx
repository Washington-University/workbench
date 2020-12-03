
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

#define __BRAIN_OPEN_G_L_MEDIA_DRAWING_DECLARE__
#include "BrainOpenGLMediaDrawing.h"
#undef __BRAIN_OPEN_G_L_MEDIA_DRAWING_DECLARE__

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "ImageFile.h"
#include "ModelMedia.h"
#include "MediaOverlay.h"
#include "MediaOverlaySet.h"
#include "SelectionItemImage.h"
#include "SelectionManager.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLMediaDrawing
 * \brief Draw media data
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLMediaDrawing::BrainOpenGLMediaDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLMediaDrawing::~BrainOpenGLMediaDrawing()
{
}

/**
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param browserTabContent
 *    Content of the browser tab
 * @param mediaModel
 *    Media model for drawing
 * @param viewport
 *    Size of the viewport
 */
void
BrainOpenGLMediaDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                   BrowserTabContent* browserTabContent,
                                   ModelMedia* mediaModel,
                                   const std::array<int32_t, 4>& viewport)
{
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    CaretAssert(mediaModel);
    
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_browserTabContent    = browserTabContent;
    m_mediaModel           = mediaModel;
    m_viewport             = viewport;
    
    if ((m_viewport[2] < 1)
        || (m_viewport[3] < 1)) {
        return;
    }
    glViewport(m_viewport[0],
               m_viewport[1],
               m_viewport[2],
               m_viewport[3]);
    

    const float halfHeight(500);
    const float aspectRatio = (static_cast<float>(m_viewport[3])
                               / static_cast<float>(m_viewport[2]));
    const float halfWidth(halfHeight / aspectRatio);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-halfWidth,  halfWidth,
            -halfHeight, halfHeight,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    float translation[3];
    m_browserTabContent->getTranslation(translation);
    const float scaling = m_browserTabContent->getScaling();
    
    glTranslatef(translation[0], translation[1], 0.0);
    glScalef(scaling, scaling, 1.0);
    
    drawModelLayers();
}

/**
 * Draw the media models layers
 */
void
BrainOpenGLMediaDrawing::drawModelLayers()
{
    SelectionItemImage* idImage = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getImageIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool selectImageFlag(false);
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (idImage->isEnabledForSelection()) {
                selectImageFlag = true;
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    MediaOverlaySet* mediaOverlaySet = m_mediaModel->getMediaOverlaySet(m_browserTabContent->getTabNumber());
    CaretAssert(mediaOverlaySet);
    const int32_t numberOfOverlays = mediaOverlaySet->getNumberOfDisplayedOverlays();
    
    for (int32_t i = (numberOfOverlays - 1); i >= 0; i--) {
        MediaOverlay* overlay = mediaOverlaySet->getOverlay(i);
        CaretAssert(overlay);
        
        if (overlay->isEnabled()) {
            MediaFile* selectedFile(NULL);
            int32_t selectedIndex(-1);
            overlay->getSelectionData(selectedFile,
                                      selectedIndex);
            
            if (selectedFile != NULL) {
                ImageFile* imageFile = selectedFile->castToImageFile();
                if (imageFile != NULL) {
                    /*
                     * Image is drawn using a primitive in which
                     * the image is a texture
                     */
                    GraphicsPrimitiveV3fT3f* primitive = imageFile->getGraphicsPrimitiveForMediaDrawing();
                    if (primitive) {
                        GraphicsEngineDataOpenGL::draw(primitive);
                        
                        if (selectImageFlag) {
                            processImageFileSelection(imageFile,
                                                      primitive);
                        }
                    }
                }
                else {
                    CaretAssertMessage(0, ("Unrecogized file type "
                                           + DataFileTypeEnum::toName(selectedFile->getDataFileType())
                                           + " for media drawing."));
                }
            }
        }
    }
}

/**
 * Process selection in an image file
 * @param imageFile
 *    The image file
 * @param primitive
 *    Primitive that draws image file
 */
void
BrainOpenGLMediaDrawing::processImageFileSelection(ImageFile* imageFile,
                                                   GraphicsPrimitiveV3fT3f* primitive)
{
    SelectionItemImage* idImage = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getImageIdentification();
    EventOpenGLObjectToWindowTransform xform(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
    EventManager::get()->sendEvent(xform.getPointer());
    if (xform.isValid()) {
        BoundingBox bounds;
        primitive->getVertexBounds(bounds);
        
        /*
         * Transform bottom left and top right coordinates of
         * primitive containing image to window coordinates
         */
        const auto minXYZ = bounds.getMinXYZ();
        float windowMinXYZ[3];
        xform.transformPoint(minXYZ.data(),
                             windowMinXYZ);
        
        const auto maxXYZ = bounds.getMaxXYZ();
        float windowMaxXYZ[3];
        xform.transformPoint(maxXYZ.data(),
                             windowMaxXYZ);
        
        const float drawnImageWidth(bounds.getDifferenceX());
        const float drawnImageHeight(bounds.getDifferenceY());
        if ((drawnImageWidth > 0.0)
            && (drawnImageHeight > 0.0)) {
            const float imageWidth(imageFile->getWidth());
            const float imageHeight(imageFile->getHeight());
            
            const float mouseX(this->m_fixedPipelineDrawing->mouseX);
            const float mouseY(this->m_fixedPipelineDrawing->mouseY);
            
            /*
             * Offset of mouse from bottom left of image in window coordinates
             */
            const float relativeX = mouseX - windowMinXYZ[0];
            const float relativeY = mouseY - windowMinXYZ[1];
            
            /*
             * Normalized coordinate in image (range is 0 to 1 if inside image)
             */
            const float normalizedX = relativeX / static_cast<float>(windowMaxXYZ[0] - windowMinXYZ[0]);
            const float normalizedY = relativeY / static_cast<float>(windowMaxXYZ[1] - windowMinXYZ[1]);
            
            /*
             * Pixel X&Y in image
             */
            const int32_t pixelX = static_cast<int32_t>(normalizedX *
                                                        static_cast<float>(imageWidth));
            const int32_t pixelY = static_cast<int32_t>(normalizedY *
                                                        static_cast<float>(imageHeight));
            
            /*
             * Verify clicked location is inside image
             */
            if ((pixelX    >= 0)
                && (pixelX <  imageWidth)
                && (pixelY >= 0)
                && (pixelY <  imageHeight)) {
                idImage->setImageFile(imageFile);
                idImage->setPixelI(pixelX);
                idImage->setPixelJ(pixelY);
                
                uint8_t pixelByteRGBA[4];
                if (imageFile->getImagePixelRGBA(ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                                 pixelX,
                                                 pixelY,
                                                 pixelByteRGBA)) {
                    idImage->setPixelRGBA(pixelByteRGBA);
                }
            }
        }
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLMediaDrawing::toString() const
{
    return "BrainOpenGLMediaDrawing";
}

