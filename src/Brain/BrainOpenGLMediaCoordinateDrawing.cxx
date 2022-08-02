
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

#define __BRAIN_OPEN_G_L_MEDIA_COORDINATE_DRAWING_DECLARE__
#include "BrainOpenGLMediaCoordinateDrawing.h"
#undef __BRAIN_OPEN_G_L_MEDIA_COORDINATE_DRAWING_DECLARE__

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
#include "SelectionItemMediaPlaneCoordinate.h"
#include "SelectionManager.h"

using namespace caret;

const bool debugFlag(false);
    
/**
 * \class caret::BrainOpenGLMediaCoordinateDrawing
 * \brief Draw media data
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLMediaCoordinateDrawing::BrainOpenGLMediaCoordinateDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLMediaCoordinateDrawing::~BrainOpenGLMediaCoordinateDrawing()
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
BrainOpenGLMediaCoordinateDrawing::getOrthoBounds(MediaOverlaySet* /*mediaOverlaySet*/,
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
    
    bool firstFlag(true);
    const int32_t numberOfFiles(m_selectionDataToDraw.size());
    for (int32_t i = 0; i < numberOfFiles; i++) {
        CaretAssertVectorIndex(m_selectionDataToDraw, i);
        MediaFile* mediaFile(m_selectionDataToDraw[i].m_selectedMediaFile);
        CaretAssert(mediaFile);
        
        if (mediaFile->isPlaneXyzSupported()) {
            const BoundingBox bb(mediaFile->getPlaneXyzBoundingBox());
            if (firstFlag) {
                firstFlag   = false;
                boundingBox = bb;
            }
            else {
                boundingBox.unionOperation(bb);
            }
        }
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
    
    const bool originTopLeftFlag(true);
    
    const double marginPercent(0.02);
    const double marginSizePixels = imageHeight * marginPercent;
    if (imageAspectRatio > viewportAspectRatio) {
        orthoBottomOut = boundingBox.getMinY() - marginSizePixels;
        orthoTopOut    = boundingBox.getMaxY() + marginSizePixels;
        if (originTopLeftFlag) {
            std::swap(orthoBottomOut, orthoTopOut);
        }
        const double orthoHeight(std::fabs(orthoTopOut - orthoBottomOut));
        const double orthoWidth(orthoHeight / viewportAspectRatio);
        const double halfOrthoWidth(orthoWidth / 2.0);
        const double centerX(boundingBox.getCenterX());
        orthoLeftOut  = centerX - halfOrthoWidth;
        orthoRightOut = centerX + halfOrthoWidth;
    }
    else {
        orthoLeftOut  = boundingBox.getMinX() - marginSizePixels;
        orthoRightOut = boundingBox.getMaxX() + marginSizePixels;
        const double orthoWidth(orthoRightOut - orthoLeftOut);
        const double orthoHeight(orthoWidth * viewportAspectRatio);
        const double halfOrthoHeight(orthoHeight / 2.0);
        const double centerY(boundingBox.getCenterY());
        orthoBottomOut = centerY - halfOrthoHeight;
        orthoTopOut    = centerY + halfOrthoHeight;
        if (originTopLeftFlag) {
            std::swap(orthoBottomOut, orthoTopOut);
        }
    }

    CaretAssert(orthoRightOut > orthoLeftOut);
    if (originTopLeftFlag) {
        CaretAssert(orthoBottomOut > orthoTopOut);
    }
    else {
        CaretAssert(orthoTopOut > orthoBottomOut);
    }
    
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
BrainOpenGLMediaCoordinateDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
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
    const int32_t numberOfOverlays = mediaOverlaySet->getNumberOfDisplayedOverlays();
    
    /*
     * Find overlays containing files that support coordinates
     */
    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        MediaOverlay* overlay = mediaOverlaySet->getOverlay(iOverlay);
        CaretAssert(overlay);
        
        if (overlay->isEnabled()) {
            const MediaOverlay::SelectionData selectionData(overlay->getSelectionData());
            CaretAssert(selectionData.m_selectedMediaFile);
            if (selectionData.m_selectedMediaFile->isPlaneXyzSupported()) {
                m_selectionDataToDraw.push_back(selectionData);
            }
        }
    }
    
    if (m_selectionDataToDraw.empty()) {
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
    if (debugFlag) {
        std::cout << "Ortho L=" << orthoLeft << ", R=" << orthoRight
        << ", B=" << orthoBottom << ", T=" << orthoTop << std::endl;
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
    
    const float orthoHeight(std::fabs(orthoBottom - orthoTop));
    drawModelLayers(viewportContent,
                    transform,
                    browserTabContent->getTabNumber(),
                    orthoHeight,
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
BrainOpenGLMediaCoordinateDrawing::drawModelLayers(const BrainOpenGLViewportContent* viewportContent,
                                                   const GraphicsObjectToWindowTransform* transform,
                                                   const int32_t /*tabIndex*/,
                                                   const float orthoHeight,
                                                   const float viewportHeight)
{
    SelectionItemMediaPlaneCoordinate* idMedia = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getMediaPlaneCoordinateIdentification();
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
    
    const int32_t numMediaFiles(static_cast<int32_t>(m_selectionDataToDraw.size()));
    for (int32_t i = 0; i < numMediaFiles; i++) {
        CaretAssertVectorIndex(m_selectionDataToDraw, i);
        MediaOverlay::SelectionData& selectionData = m_selectionDataToDraw[i];
        MediaFile* mediaFile(selectionData.m_selectedMediaFile);
        CaretAssert(mediaFile);
        CziImageFile* cziImageFile(mediaFile->castToCziImageFile());
        ImageFile* imageFile(mediaFile->castToImageFile());

        if (cziImageFile != NULL) {
            cziImageFile->updateImageForDrawingInTab(selectionData.m_tabIndex,
                                                     selectionData.m_overlayIndex,
                                                     selectionData.m_selectedFrameIndex,
                                                     selectionData.m_allFramesSelectedFlag,
                                                     selectionData.m_cziResolutionChangeMode,
                                                     MediaDisplayCoordinateModeEnum::PLANE,
                                                     selectionData.m_cziManualPyramidLayerIndex,
                                                     transform);
        }
        else if (imageFile != NULL) {
            /* nothing */
        }
        else {
            CaretAssertMessage(0, ("Unrecognized file type "
                                   + DataFileTypeEnum::toName(mediaFile->getDataFileType())
                                   + " for media drawing."));
        }


        GraphicsPrimitiveV3fT2f* primitive(mediaFile->getGraphicsPrimitiveForPlaneXyzDrawing(selectionData.m_tabIndex,
                                                                                             selectionData.m_overlayIndex));
        if (primitive == NULL) {
            /*
             * If a CZI image is completely offscreen there may be no data to load for it
             */
            continue;
        }

        glPushMatrix();
        
        CaretAssert(primitive->isValid());

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
                                      selectionData,
                                      primitive);
        }
        
        /*
         * Height used for drawing ID symbols
         */
        const float symbolIdHeight(viewportHeight / (orthoHeight /  m_browserTabContent->getScaling()));
        
        /*
         * Draw identification symbols
         */
        BrainOpenGLIdentificationDrawing idDrawing(m_fixedPipelineDrawing,
                                                   m_fixedPipelineDrawing->m_brain,
                                                   m_browserTabContent,
                                                   m_fixedPipelineDrawing->mode);
        const float mediaThickness(2.0f);
        Plane plane;
        idDrawing.drawMediaFilePlaneCoordinateIdentificationSymbols(mediaFile,
                                                                    plane,
                                                                    mediaThickness,
                                                                    symbolIdHeight,
                                                                    viewportHeight);

        m_fixedPipelineDrawing->drawMediaSpaceAnnotations(viewportContent);

        glPopMatrix();
    }
}

/**
 * Draw the selection box
 */
void
BrainOpenGLMediaCoordinateDrawing::drawSelectionBox()
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
 * @param selectionData
 *   Info about selections in the layer
 * @param primitive
 *    Primitive that draws image file
 */
void
BrainOpenGLMediaCoordinateDrawing::processMediaFileSelection(const int32_t tabIndex,
                                                             const MediaOverlay::SelectionData& selectionData,
                                                             GraphicsPrimitiveV3fT2f* primitive)
{
    SelectionItemMediaPlaneCoordinate* idMedia = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getMediaPlaneCoordinateIdentification();
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
            Vector3D planeXYZ;
            xform.inverseTransformPoint(windowXYZ, planeXYZ);
            
            MediaFile* mediaFile(selectionData.m_selectedMediaFile);
            CaretAssert(mediaFile);

            PixelLogicalIndex pixelLogicalIndex;
            if ( ! mediaFile->planeXyzToLogicalPixelIndex(planeXYZ,
                                                          pixelLogicalIndex)) {
                return;
            }
            
            /*
             * Frame indices to test
             */
            std::vector<int32_t> validFrameIndices;
            if (selectionData.m_allFramesSelectedFlag) {
                for (int32_t i = 0; i < mediaFile->getNumberOfFrames(); i++) {
                    validFrameIndices.push_back(i);
                }
            }
            else {
                validFrameIndices.push_back(selectionData.m_selectedFrameIndex);
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
                idMedia->setOverlayIndex(selectionData.m_overlayIndex);
                idMedia->setPlaneCoordinate(planeXYZ);
                idMedia->setMediaFile(mediaFile);
                uint8_t pixelByteRGBA[4] = { 0, 0, 0, 0 };
                idMedia->setModelXYZ(planeXYZ);
                idMedia->setScreenXYZ(windowXYZ);
                idMedia->setScreenDepth(0.0);
                if (idMedia->isIncludePixelRGBA()) {
                    if (mediaFile->getPixelRGBA(tabIndex,
                                                selectionData.m_overlayIndex,
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
BrainOpenGLMediaCoordinateDrawing::toString() const
{
    return "BrainOpenGLMediaCoordinateDrawing";
}

