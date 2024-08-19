
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
#include "OmeZarrImageFile.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemMediaLogicalCoordinate.h"
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
                                        double& orthoLeftOut,
                                        double& orthoRightOut,
                                        double& orthoBottomOut,
                                        double& orthoTopOut)
{
    orthoLeftOut   = -1.0;
    orthoRightOut  =  1.0;
    orthoBottomOut = -1.0;
    orthoTopOut    =  1.0;
        
    QRectF boundingRect;
    
    CaretAssert(mediaOverlaySet);
    
    std::vector<MediaFile*> displayedMediaFiles;
    std::vector<int32_t> displayedverlayIndices;
    mediaOverlaySet->getDisplayedMediaFileAndOverlayIndices(displayedMediaFiles,
                                                             displayedverlayIndices);
    const int32_t numberOfFiles(displayedMediaFiles.size());
    for (int32_t i = 0; i < numberOfFiles; i++) {
        CaretAssertVectorIndex(displayedMediaFiles, i);
        MediaFile* mediaFile(displayedMediaFiles[i]);
        CaretAssertVectorIndex(displayedverlayIndices, i);
        
        const QRectF logicalRect(mediaFile->getLogicalBoundsRect());
        if (i == 0) {
            boundingRect = logicalRect;
        }
        else {
            boundingRect = boundingRect.united(logicalRect);
        }
    }
    
    const double viewportWidth(m_viewport[2]);
    const double viewportHeight(m_viewport[3]);
    const double viewportAspectRatio = (viewportHeight
                                        / viewportWidth);

    const double imageWidth(boundingRect.width());
    const double imageHeight(boundingRect.height());
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
    orthoTopOut = boundingRect.top() - topMargin;
    orthoBottomOut = orthoTopOut + orthoHeight;
    
    const float orthoWidth(orthoRightOut - orthoLeftOut);
    orthoLeftOut = boundingRect.left() - leftMargin;
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
    if ( ! getOrthoBounds(mediaOverlaySet,
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
    viewportContent->setMediaGraphicsObjectToWindowTransform(transform);
    
    drawModelLayers(viewportContent,
                    transform,
                    browserTabContent->getTabNumber(),
                    viewport[3]);
    
    BrainOpenGLFixedPipeline::drawGraphicsRegionSelectionBox(m_browserTabContent->getRegionSelectionBox(),
                                                             GraphicsRegionSelectionBox::DrawMode::Z_PLANE,
                                                             m_fixedPipelineDrawing->m_foregroundColorFloat);
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
    SelectionItemMediaLogicalCoordinate* idMedia = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getMediaLogicalCoordinateIdentification();
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
    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        MediaOverlay* overlay = mediaOverlaySet->getOverlay(iOverlay);
        CaretAssert(overlay);
        
        glPushMatrix();
        
        if (overlay->isEnabled()) {
            const MediaOverlay::SelectionData selectionData(overlay->getSelectionData());
            
            if (selectionData.m_selectedMediaFile != NULL) {
                GraphicsPrimitiveV3fT2f* primitive(NULL);
                CziImageFile* cziImageFile = selectionData.m_selectedCziImageFile;
                ImageFile* imageFile = selectionData.m_selectedMediaFile->castToImageFile();
                OmeZarrImageFile* omeZarrImageFile(selectionData.m_selectedMediaFile->castToOmeZarrImageFile());
                float mediaHeight(-1.0);
                if (imageFile != NULL) {
                    /*
                     * Image is drawn using a primitive in which
                     * the image is a texture
                     */
                    primitive = imageFile->getGraphicsPrimitiveForMediaDrawing(tabIndex,
                                                                               iOverlay);
                    
                    mediaHeight = imageFile->getHeight();
                }
                else  if (cziImageFile != NULL) {
                    cziImageFile->updateImageForDrawingInTab(tabIndex,
                                                             iOverlay,
                                                             selectionData.m_selectedFrameIndex,
                                                             selectionData.m_allFramesSelectedFlag,
                                                             selectionData.m_cziResolutionChangeMode,
                                                             MediaDisplayCoordinateModeEnum::PIXEL,
                                                             selectionData.m_selectedChannelIndex,
                                                             selectionData.m_cziManualPyramidLayerIndex,
                                                             transform);
                    
                    primitive = cziImageFile->getGraphicsPrimitiveForMediaDrawing(tabIndex,
                                                                                  iOverlay);
                    mediaHeight = cziImageFile->getHeight();
                }
                else if (omeZarrImageFile != NULL) {
                    omeZarrImageFile->updateImageForDrawingInTab(tabIndex,
                                                                 iOverlay,
                                                                 selectionData.m_selectedFrameIndex,
                                                                 selectionData.m_selectedFrameIndex,
                                                                 selectionData.m_cziManualPyramidLayerIndex,
                                                                 selectionData.m_cziResolutionChangeMode);
                    primitive = omeZarrImageFile->getGraphicsPrimitiveForMediaDrawing(tabIndex,
                                                                                      iOverlay);
                    mediaHeight = omeZarrImageFile->getHeight();
                }
                else {
                    CaretAssertMessage(0, ("Unrecognized file type "
                                           + DataFileTypeEnum::toName(selectionData.m_selectedMediaFile->getDataFileType())
                                           + " for media drawing."));
                }
                
                if (primitive) {
                    CaretAssert(primitive->isValid());
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
                        /*
                         * Allow blending.  Images may have a border color with alpha of zero
                         * so that background shows through.
                         */
                        BrainOpenGLFixedPipeline::setupBlending(BrainOpenGLFixedPipeline::BlendDataType::FEATURE_IMAGE);
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
                                                  iOverlay,
                                                  selectionData.m_selectedMediaFile,
                                                  selectionData.m_selectedFrameIndex,
                                                  selectionData.m_allFramesSelectedFlag,
                                                  primitive);
                    }
                    
                    
                    /*
                     * Draw identification symbols
                     */
                    BrainOpenGLIdentificationDrawing idDrawing(m_fixedPipelineDrawing,
                                                               m_fixedPipelineDrawing->m_brain,
                                                               m_browserTabContent,
                                                               m_fixedPipelineDrawing->mode);
                    const float mediaThickness(2.0f);
                    Plane plane;
                    idDrawing.drawMediaFileLogicalCoordinateIdentificationSymbols(selectionData.m_selectedMediaFile,
                                                                 plane,
                                                                 mediaThickness,
                                                                 m_browserTabContent->getScaling(),
                                                                 viewportHeight);
                    
                    m_fixedPipelineDrawing->drawMediaSpaceAnnotations(viewportContent);

                }
            }
        }
        
        glPopMatrix();
    }
}

/**
 * Process selection in amedia  file
 * @param tabIndex
 *   Index of the tab
 * @param overlayIndex
 *   Index of the overlay
 * @param mediaFile
 *    The medai file
 * @param selectedFrameIndex
 *    Index of selected frame
 * @param allFramesSelectedFlag
 *    True if all frames are selected
 * @param primitive
 *    Primitive that draws image file
 */
void
BrainOpenGLMediaDrawing::processMediaFileSelection(const int32_t tabIndex,
                                                   const int32_t overlayIndex,
                                                   MediaFile* mediaFile,
                                                   const int32_t selectedFrameIndex,
                                                   const bool allFramesSelectedFlag,
                                                   GraphicsPrimitiveV3fT2f* primitive)
{
    SelectionItemMediaLogicalCoordinate* idMedia = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getMediaLogicalCoordinateIdentification();
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
            Vector3D logicalXYZ;
            xform.inverseTransformPoint(windowXYZ, logicalXYZ);
            
            logicalXYZ[2] = 0.0;
            PixelLogicalIndex pixelLogicalIndex(logicalXYZ);
            
            /*
             * Frame indices to test
             */
            std::vector<int32_t> validFrameIndices;
            if (allFramesSelectedFlag) {
                for (int32_t i = 0; i < mediaFile->getNumberOfFrames(); i++) {
                    validFrameIndices.push_back(i);
                }
            }
            else {
                validFrameIndices.push_back(selectedFrameIndex);
            }
            
            /*
             * Ensure that the pixel is within a displayed frame (Scene
             * in CZI terminology).  Otherwise empty space pixels may
             * be identified.  Also, if RGBA is NOT valid, then
             * there is no CZI data for the pixel.
             */
            bool validPixelFlag(false);
            for (int32_t frameIndex : validFrameIndices) {
                if (mediaFile->isPixelIndexInFrameValid(frameIndex,
                                                        pixelLogicalIndex)) {
                    uint8_t rgba[4];
                    validPixelFlag = mediaFile->getPixelRGBA(tabIndex,
                                                             frameIndex,
                                                             pixelLogicalIndex,
                                                             rgba);
                    if (validPixelFlag) {
                        break;
                    }
                }
            }
            
            if (validPixelFlag) {
                idMedia->setMediaFile(mediaFile);
                idMedia->setTabIndex(tabIndex);
                idMedia->setOverlayIndex(overlayIndex);
                idMedia->setPixelLogicalIndex(pixelLogicalIndex);
                idMedia->setMediaFile(mediaFile);
                uint8_t pixelByteRGBA[4] = { 0, 0, 0, 0 };
                idMedia->setModelXYZ(logicalXYZ);
                idMedia->setScreenXYZ(windowXYZ);
                idMedia->setScreenDepth(0.0);
                if (idMedia->isIncludePixelRGBA()) {
                    if (mediaFile->getPixelRGBA(tabIndex,
                                                overlayIndex,
                                                pixelLogicalIndex,
                                                pixelByteRGBA)) {
                        idMedia->setPixelRGBA(pixelByteRGBA);
                    }
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
