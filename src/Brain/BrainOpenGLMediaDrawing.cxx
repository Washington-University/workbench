
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
#include "BrainOpenGLIdentificationDrawing.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziImage.h"
#include "CziImageFile.h"
#include "DisplayPropertiesCziImages.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "ImageFile.h"
#include "ModelMedia.h"
#include "MediaOverlay.h"
#include "MediaOverlaySet.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemMedia.h"
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
 * Setup the orthographic bounds for the underlay media file
 * @param mediaOverlaySet
 *    Media overlay set
 * @param orthoLeftOut
 *    Output with orthographic left
 * @param orthoRightOut
 *    Output with orthographic right
 * @param orthoBottomOut
 *    Output with orthographic bottom
 * @param orthoTopOut
 *    Output with orthographic top
 * @return True if othographic bounds are valid, else false.
 */
bool
BrainOpenGLMediaDrawing::getOrthoBounds(MediaOverlaySet* mediaOverlaySet,
                                        const int32_t tabIndex,
                                        double& orthoLeftOut,
                                        double& orthoRightOut,
                                        double& orthoBottomOut,
                                        double& orthoTopOut)
{
    orthoLeftOut   = -1.0;
    orthoRightOut  =  1.0;
    orthoBottomOut = -1.0;
    orthoTopOut    =  1.0;
        
    BoundingBox boundingBox;
    
    CaretAssert(mediaOverlaySet);
    std::vector<MediaFile*> mediaFiles(mediaOverlaySet->getDisplayedMediaFiles());
    if (mediaFiles.empty()) {
        return false;
    }
    
    for (auto mf : mediaFiles) {
        GraphicsPrimitiveV3fT2f* primitive(NULL);
        CziImageFile* cziImageFile = mf->castToCziImageFile();
        const CziImage* cziImage(NULL);
        ImageFile* imageFile = mf->castToImageFile();
        if (imageFile != NULL) {
            /*
             * Image is drawn using a primitive in which
             * the image is a texture
             */
            primitive = imageFile->getGraphicsPrimitiveForMediaDrawing(tabIndex);
        }
        else  if (cziImageFile != NULL) {
            cziImage = cziImageFile->getDefaultImage();
            if (cziImage == NULL) {
                CaretLogSevere("CZI file has invalid default image: "
                               + mf->getFileNameNoPath());
                return false;
            }
            primitive = cziImage->getGraphicsPrimitiveForMediaDrawing();
        }
        else {
            CaretAssertMessage(0, ("Unrecognized file type "
                                   + DataFileTypeEnum::toName(mf->getDataFileType())
                                   + " for media drawing."));
            return false;
        }
        if (primitive == NULL) {
            CaretLogSevere("Media file has invalid primitive for tab "
                           + AString::number(tabIndex + 1)
                           + mf->getFileNameNoPath());
            return false;
        }
        
        BoundingBox primitiveBoundingBox;
        primitive->getVertexBounds(primitiveBoundingBox);
        boundingBox.unionOperation(primitiveBoundingBox);
    }
    
    const double viewportWidth(m_viewport[2]);
    const double viewportHeight(m_viewport[3]);
    const double viewportAspectRatio = (viewportHeight
                                        / viewportWidth);
    const double imageWidth(boundingBox.getDifferenceX());
    const double imageHeight(boundingBox.getDifferenceY());
    if ((imageWidth < 1.0)
        || (imageHeight < 1.0)) {
        return false;
    }
    const double imageAspectRatio = (imageHeight
                                     / imageWidth);
    
    const double marginPercent(0.02);
    const double marginSizePixels = imageHeight * marginPercent;
    double leftMargin(marginSizePixels);
    double topMargin(marginSizePixels);
    if (imageAspectRatio > viewportAspectRatio) {
        orthoBottomOut = -marginSizePixels;
        orthoTopOut    = imageHeight + marginSizePixels;
        const double orthoHeight(orthoTopOut - orthoBottomOut);
        const double orthoWidth(orthoHeight / viewportAspectRatio);
        const double leftRightMargin((orthoWidth - imageWidth) / 2.0);
        orthoLeftOut  = -leftRightMargin;
        orthoRightOut = imageWidth + leftRightMargin;
        leftMargin = leftRightMargin;
    }
    else {
        orthoLeftOut  = -marginSizePixels;
        orthoRightOut =  imageWidth + marginSizePixels;
        const double orthoWidth(orthoRightOut - orthoLeftOut);
        const double orthoHeight(orthoWidth * viewportAspectRatio);
        const double bottomTopMargin((orthoHeight - imageHeight) / 2.0);
        orthoBottomOut = -bottomTopMargin;
        orthoTopOut    = imageHeight + bottomTopMargin;
        topMargin = bottomTopMargin;
    }

    /*
     * Change ORTHO to fit image with origin at top left
     */
    const float orthoHeight(orthoTopOut - orthoBottomOut);
    orthoTopOut = boundingBox.getMinY() - topMargin;
    orthoBottomOut = orthoTopOut + orthoHeight;
    
    const float orthoWidth(orthoRightOut - orthoLeftOut);
    orthoLeftOut = boundingBox.getMinX() - leftMargin;
    orthoRightOut = orthoLeftOut + orthoWidth;
    
    return true;
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
    
    MediaOverlaySet* mediaOverlaySet = m_mediaModel->getMediaOverlaySet(m_browserTabContent->getTabNumber());
    CaretAssert(mediaOverlaySet);
    const MediaFile* underlayMediaFile = mediaOverlaySet->getBottomMostMediaFile();
    if (underlayMediaFile == NULL) {
        return;
    }
    
    double orthoLeft(-1.0);
    double orthoRight(1.0);
    double orthoBottom(-1.0);
    double orthoTop(1.0);
    if ( ! getOrthoBounds(mediaOverlaySet, browserTabContent->getTabNumber(),
                          orthoLeft, orthoRight, orthoBottom, orthoTop)) {
        return;
    }

    if ((m_viewport[2] < 1)
        || (m_viewport[3] < 1)) {
        return;
    }
    glViewport(m_viewport[0],
               m_viewport[1],
               m_viewport[2],
               m_viewport[3]);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(orthoLeft, orthoRight,
            orthoBottom, orthoTop,
            -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /*
     * Note on translation: Origin is at top left so a positive-Y translation
     * moves the image down.  However, the user may edit the translation values
     * and from the user's perspective, positive-Y is up.  Also, other models
     * types have postive-Y up (surface, volume).  So we use the negative
     * of the Y-translation.
     */
    float translation[3];
    m_browserTabContent->getTranslation(translation);
    glTranslatef(translation[0], -translation[1], 0.0);
    
    const float scaling = m_browserTabContent->getScaling();
    glScalef(scaling, scaling, 1.0);

    std::array<float, 4> orthoLRBT {
        static_cast<float>(orthoLeft),
        static_cast<float>(orthoRight),
        static_cast<float>(orthoBottom),
        static_cast<float>(orthoTop)
    };
    GraphicsObjectToWindowTransform* transform = new GraphicsObjectToWindowTransform();
    fixedPipelineDrawing->loadObjectToWindowTransform(transform, orthoLRBT, 0.0, true);
    viewportContent->setGraphicsObjectToWindowTransform(transform);
    
    drawModelLayers(viewportContent,
                    transform,
                    browserTabContent->getTabNumber(),
                    viewport[3]);
    
    drawSelectionBox();
}

/**
 * Draw the media models layers
 * @param viewportContent
 *    The viewport content
 * @param objectToWindowTransform
 *   Transforms point from object to window space
 * @param tabIndex
 *   Index of the tab
 * @param viewportHeight
 *   Height of viewport
 */
void
BrainOpenGLMediaDrawing::drawModelLayers(const BrainOpenGLViewportContent* viewportContent,
                                         const GraphicsObjectToWindowTransform* transform,
                                         const int32_t tabIndex,
                                         const float viewportHeight)
{
    SelectionItemMedia* idMedia = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getMediaIdentification();
    SelectionItemAnnotation* annotationID = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getAnnotationIdentification();

    
    /*
     * Check for a 'selection' type mode
     */
    bool selectImageFlag(false);
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (idMedia->isEnabledForSelection()) {
                selectImageFlag = true;
            }
            else if (annotationID->isEnabledForSelection()) {
                /* continue drawing so annotations get selected */
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
    
    float underlayMediaHeight(-1.0);
    for (int32_t i = (numberOfOverlays - 1); i >= 0; i--) {
        MediaOverlay* overlay = mediaOverlaySet->getOverlay(i);
        CaretAssert(overlay);
        
        glPushMatrix();
        
        if (overlay->isEnabled()) {
            MediaFile* mediaFile(NULL);
            int32_t selectedIndex(-1);
            overlay->getSelectionData(mediaFile,
                                      selectedIndex);
            
            if (mediaFile != NULL) {
                GraphicsPrimitiveV3fT2f* primitive(NULL);
                CziImageFile* cziImageFile = mediaFile->castToCziImageFile();
                const CziImage* cziImage(NULL);
                ImageFile* imageFile = mediaFile->castToImageFile();
                float mediaHeight(-1.0);
                if (imageFile != NULL) {
                    /*
                     * Image is drawn using a primitive in which
                     * the image is a texture
                     */
                    primitive = imageFile->getGraphicsPrimitiveForMediaDrawing(tabIndex);
                    
                    mediaHeight = imageFile->getHeight();
                }
                else  if (cziImageFile != NULL) {
                    const DisplayPropertiesCziImages* dpc(m_fixedPipelineDrawing->m_brain->getDisplayPropertiesCziImages());
                    cziImage = cziImageFile->getImageForDrawingInTab(tabIndex,
                                                                     transform,
                                                                     dpc->getResolutionChangeMode(tabIndex));
                    primitive = cziImage->getGraphicsPrimitiveForMediaDrawing();
                    
                    BoundingBox boundingBox;
                    primitive->getVertexBounds(boundingBox);
                    
                    mediaHeight = cziImageFile->getHeight();
                }
                else {
                    CaretAssertMessage(0, ("Unrecognized file type "
                                           + DataFileTypeEnum::toName(mediaFile->getDataFileType())
                                           + " for media drawing."));
                }
                
                if (primitive) {
                    if (mediaHeight > 0.0) {
                        /*
                         * OpenGL and media primitives have origin in bottom left
                         * but we want to align images at top left.  So, for any images
                         * on top of bottom most layer, translate the image so that
                         * top left corners align.
                         */
                        if (underlayMediaHeight > 0.0) {
                            const float translateY(underlayMediaHeight - mediaHeight);
                            glTranslatef(0.0, translateY, 0.0);
                        }
                        else {
                            underlayMediaHeight = mediaHeight;
                        }
                    }
                    
                    glPushAttrib(GL_COLOR_BUFFER_BIT);
                    if ( ! selectImageFlag) {
                       // m_fixedPipelineDrawing->setupBlending(BrainOpenGLFixedPipeline::BlendDataType::FEATURE_IMAGE);
                    }
                    
                    /*
                     * Set texture filtering
                     */
                    primitive->setTextureMinificationFilter(s_textureMinificationFilter);
                    primitive->setTextureMagnificationFilter(s_textureMagnificationFilter);
                    
                    GraphicsEngineDataOpenGL::draw(primitive);
                    glPopAttrib();
                    
                    if (selectImageFlag) {
                        processMediaFileSelection(m_browserTabContent->getTabNumber(),
                                                  mediaFile,
                                                  primitive);
                    }
                    
                    /*
                     * Draw volume identification symbols
                     */
                    BrainOpenGLIdentificationDrawing idDrawing(m_fixedPipelineDrawing,
                                                               m_fixedPipelineDrawing->m_brain,
                                                               m_browserTabContent,
                                                               m_fixedPipelineDrawing->mode);
                    const float mediaThickness(2.0f);
                    Plane plane;
                    idDrawing.drawMediaFileIdentificationSymbols(mediaFile,
                                                                 plane,
                                                                 mediaThickness,
                                                                 viewportHeight);
                    
                    m_fixedPipelineDrawing->drawMediaSpaceAnnotations(viewportContent);

                }
            }
        }
        
        glPopMatrix();
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
                std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN,
                                                                                                   m_fixedPipelineDrawing->m_foregroundColorFloat));

                const float z(0.0f);
                primitive->addVertex(minX, minY, z);
                primitive->addVertex(maxX, minY, z);
                primitive->addVertex(maxX, maxY, z);
                primitive->addVertex(minX, maxY, z);
                
                const float lineWidthPercentage(0.5);
                primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                        lineWidthPercentage);
                
                GraphicsEngineDataOpenGL::draw(primitive.get());
            }
        }
            break;
    }
}

/**
 * Process selection in amedia  file
 * @param tabIndex
 *   Index of the tab
 * @param mediaFile
 *    The medai file
 * @param primitive
 *    Primitive that draws image file
 */
void
BrainOpenGLMediaDrawing::processMediaFileSelection(const int32_t tabIndex,
                                                   MediaFile* mediaFile,
                                                   GraphicsPrimitiveV3fT2f* primitive)
{
    SelectionItemMedia* idMedia = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getMediaIdentification();
    EventOpenGLObjectToWindowTransform xform(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
    EventManager::get()->sendEvent(xform.getPointer());
    if (xform.isValid()) {
        BoundingBox bounds;
        primitive->getVertexBounds(bounds);
        
        const float drawnImageWidth(bounds.getDifferenceX());
        const float drawnImageHeight(bounds.getDifferenceY());
        if ((drawnImageWidth > 0.0)
            && (drawnImageHeight > 0.0)) {
            
            const float mouseX(this->m_fixedPipelineDrawing->mouseX);
            const float mouseY(this->m_fixedPipelineDrawing->mouseY);
            
            float windowXYZ[3] { mouseX, mouseY, 0.0 };
            std::array<float, 3> logicalXYZ;
            xform.inverseTransformPoint(windowXYZ, logicalXYZ.data());
            
            PixelIndex pixelIndexOriginAtTop;
            if (mediaFile->imageLogicalXYZToPixelIndex(logicalXYZ,
                                                       pixelIndexOriginAtTop)) {
                idMedia->setMediaFile(mediaFile);
                PixelIndex pixelIndexOriginAtBottom(pixelIndexOriginAtTop);
                pixelIndexOriginAtBottom.setJ(mediaFile->getHeight() - pixelIndexOriginAtTop.getJ() - 1);
                idMedia->setPixelIndex(pixelIndexOriginAtBottom,
                                          pixelIndexOriginAtTop);
                idMedia->setTabIndex(tabIndex);
                
                idMedia->setMediaFile(mediaFile);
                uint8_t pixelByteRGBA[4] = { 0, 0, 0, 0 };
                idMedia->setModelXYZ(logicalXYZ.data());
                idMedia->setScreenXYZ(windowXYZ);
                idMedia->setScreenDepth(0.0);
                if (mediaFile->getImagePixelRGBA(tabIndex,
                                                    ImageFile::IMAGE_DATA_ORIGIN_AT_TOP,
                                                 pixelIndexOriginAtTop,
                                                    pixelByteRGBA)) {
                    idMedia->setPixelRGBA(pixelByteRGBA);
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
