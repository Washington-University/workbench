
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
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"
#include "GraphicsPrimitiveV3f.h"
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
 * @param viewportContent
 *    The viewport content
 * @param browserTabContent
 *    Content of the browser tab
 * @param mediaModel
 *    Media model for drawing
 * @param viewport
 *    Size of the viewport
 */
void
BrainOpenGLMediaDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                              const BrainOpenGLViewportContent* viewportContent,
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
    
    const float drawingHalfHeight(MediaFile::getMediaDrawingOrthographicHalfHeight());

    MediaOverlaySet* mediaOverlaySet = m_mediaModel->getMediaOverlaySet(m_browserTabContent->getTabNumber());
    CaretAssert(mediaOverlaySet);
    
    /*
     * Default scaling fits image to the viewport in the default view
     */
    const DefaultViewTransform defaultViewTransform = mediaOverlaySet->getDefaultViewTransform();
    const std::array<float, 3> defaultTranslation(defaultViewTransform.getTranslation());
    const float defaultScaling(defaultViewTransform.getScaling());
    
    if ((m_viewport[2] < 1)
        || (m_viewport[3] < 1)) {
        return;
    }
    glViewport(m_viewport[0],
               m_viewport[1],
               m_viewport[2],
               m_viewport[3]);

    const float aspectRatio = (static_cast<float>(m_viewport[3])
                               / static_cast<float>(m_viewport[2]));
    const float halfWidth(drawingHalfHeight / aspectRatio);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-halfWidth,  halfWidth,
            -drawingHalfHeight, drawingHalfHeight,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    float translation[3];
    m_browserTabContent->getTranslation(translation);
    const float scaling = m_browserTabContent->getScaling();
    
    glTranslatef(defaultTranslation[0], defaultTranslation[1], 0.0);
    glTranslatef(translation[0], translation[1], 0.0);
    glScalef(defaultScaling, defaultScaling, 1.0);
    glScalef(scaling, scaling, 1.0);
    
    std::array<float, 4> orthoLRBT { -halfWidth, halfWidth, -drawingHalfHeight, drawingHalfHeight };
    GraphicsObjectToWindowTransform* transform = new GraphicsObjectToWindowTransform();
    fixedPipelineDrawing->loadObjectToWindowTransform(transform, orthoLRBT, 0.0, true);
    viewportContent->setGraphicsObjectToWindowTransform(transform);
    

    drawModelLayers();
    
    drawSelectionBox();
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
                        glPushAttrib(GL_COLOR_BUFFER_BIT);
                        if ( ! selectImageFlag) {
                            m_fixedPipelineDrawing->setupBlending(BrainOpenGLFixedPipeline::BlendDataType::FEATURE_IMAGE);
                        }
                        
                        /*
                         * Set texture filtering
                         */
                        primitive->setTextureMinificationFilter(s_textureMinificationFilter);
                        primitive->setTextureMagnificationFilter(s_textureMagnificationFilter);
                        
                        GraphicsEngineDataOpenGL::draw(primitive);
                        glPopAttrib();
                        
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
 * Draw the selection box
 */
void
BrainOpenGLMediaDrawing::drawSelectionBox()
{
    const GraphicsRegionSelectionBox* selectionBox = m_browserTabContent->getMediaRegionSelectionBox();

    switch (selectionBox->getStatus()) {
        case GraphicsRegionSelectionBox::Status::INVALID:
            break;
        case GraphicsRegionSelectionBox::Status::VALID:
        {
            float minX, maxX, minY, maxY;
            if (selectionBox->getBounds(minX, minY, maxX, maxY)) {
                auto primitive = GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN,
                                                                    m_fixedPipelineDrawing->m_foregroundColorFloat);

                const float z(0.0f);
                primitive->addVertex(minX, minY, z);
                primitive->addVertex(maxX, minY, z);
                primitive->addVertex(maxX, maxY, z);
                primitive->addVertex(minX, maxY, z);
                
                const float lineWidthPercentage(0.5);
                primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                        lineWidthPercentage);
                
                GraphicsEngineDataOpenGL::draw(primitive);
            }
        }
            break;
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
            && (drawnImageHeight > 0.0)
            && (windowMaxXYZ[0] > windowMinXYZ[0])
            && (windowMaxXYZ[1] - windowMinXYZ[1])) {
            
            const float mouseX(this->m_fixedPipelineDrawing->mouseX);
            const float mouseY(this->m_fixedPipelineDrawing->mouseY);
            
            float windowXYZ[3] { mouseX, mouseY, 0.0 };
            std::array<float, 3> modelXYZ;
            xform.inverseTransformPoint(windowXYZ, modelXYZ.data());
            
            /*
             * Ensure Z is zero.
             * In inverseTransformPoint(), setting Z to
             * "(2.0f * windowXYZ[2]) - 1.0f" may be incorrect.
             */
            modelXYZ[2] = 0.0;
            
            MediaFile::PixelCoordinate pixelCoordinate(modelXYZ);
            MediaFile::PixelIndex pixelIndex;
            const bool validIndexFlag(imageFile->spaceToIndexValid(pixelCoordinate,
                                                                   pixelIndex));
            /*
             * Verify clicked location is inside image
             */
            if (validIndexFlag) {
                idImage->setImageFile(imageFile);
                idImage->setPixelI(pixelIndex.getI());
                idImage->setPixelJ(pixelIndex.getJ());
                
                uint8_t pixelByteRGBA[4];
                if (imageFile->getImagePixelRGBA(ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                                 pixelIndex,
                                                 pixelByteRGBA)) {
                    idImage->setImageFile(imageFile);
                    idImage->setPixelRGBA(pixelByteRGBA);
                    idImage->setModelXYZ(modelXYZ.data());
                    idImage->setScreenXYZ(windowXYZ);
                    idImage->setScreenDepth(0.0);
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

/**
 * @return The texture magnification filter for image drawing
 */
GraphicsTextureMagnificationFilterEnum::Enum
BrainOpenGLMediaDrawing::getTextureMagnificationFilter()
{
    return s_textureMagnificationFilter;
}

/**
 * @return The texture minification filter for image drawing
 */
GraphicsTextureMinificationFilterEnum::Enum
BrainOpenGLMediaDrawing::getTextureMinificationFilter()
{
    return s_textureMinificationFilter;
}

/**
 * Set the texture magnification filter for image drawing
 * @param magFilter
 *    New value for magnification filter
 */
void
BrainOpenGLMediaDrawing::setTextureMagnificationFilter(const GraphicsTextureMagnificationFilterEnum::Enum magFilter)
{
    s_textureMagnificationFilter = magFilter;
}

/**
 * Set the texture minification filter for image drawing
 * @param minFilter
 *    New value for minnification filter
 */
void
BrainOpenGLMediaDrawing::setTextureMinificationFilter(const GraphicsTextureMinificationFilterEnum::Enum minFilter)
{
    s_textureMinificationFilter = minFilter;
}
