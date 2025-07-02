
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

#define __BRAIN_OPEN_G_L_HISTOLOGY_SLICE_DRAWING_DECLARE__
#include "BrainOpenGLHistologySliceDrawing.h"
#undef __BRAIN_OPEN_G_L_HISTOLOGY_SLICE_DRAWING_DECLARE__

#include <cmath>
#include <tuple>

#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLFociDrawing.h"
#include "BrainOpenGLIdentificationDrawing.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLVolumeSurfaceOutlineDrawing.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CziImage.h"
#include "CziImageFile.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesLabels.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsOrthographicProjection.h"
#include "GraphicsRegionSelectionBox.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "GraphicsShape.h"
#include "HistologyCoordinate.h"
#include "HistologySlice.h"
#include "HistologySlicesFile.h"
#include "HistologySliceImage.h"
#include "ImageFile.h"
#include "ModelHistology.h"
#include "HistologyOverlaySet.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemHistologyCoordinate.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "TabDrawingInfo.h"
#include "VolumeFile.h"
#include "VolumeMappableInterface.h"

using namespace caret;

/**
 * \class caret::BrainOpenGLHistologySliceDrawing
 * \brief Draw histology slice data
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLHistologySliceDrawing::BrainOpenGLHistologySliceDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLHistologySliceDrawing::~BrainOpenGLHistologySliceDrawing()
{
}

/**
 * Setup the orthographic bounds for the underlay histology file
 * @param orthographicsProjectionOut
 *    Output with orthographic projection
 * @return True if othographic bounds are valid, else false.
 */
bool
BrainOpenGLHistologySliceDrawing::getOrthoBounds(GraphicsOrthographicProjection& orthographicsProjectionOut)
{
    orthographicsProjectionOut.resetToInvalid();
        
    BoundingBox boundingBox;
    
    bool firstFlag(true);
    const int32_t numberOfFiles(m_mediaFilesAndDataToDraw.size());
    for (int32_t i = 0; i < numberOfFiles; i++) {
        CaretAssertVectorIndex(m_mediaFilesAndDataToDraw, i);
        CaretAssert(m_mediaFilesAndDataToDraw[i].m_selectedFile);
        const MediaFile* mediaFile(m_mediaFilesAndDataToDraw[i].m_mediaFile);
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
    
    const double viewportWidth(m_viewport.getWidthF());
    const double viewportHeight(m_viewport.getHeightF());
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
    
    double orthoLeftOut(0.0);
    double orthoRightOut(0.0);
    double orthoBottomOut(0.0);
    double orthoTopOut(0.0);
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
    
    /*
     * Note that too small will clip foci drawn as spheres
     */
    const double nearValue(-100.0);
    const double farValue(100.0);
    
    if (m_browserTabContent->isHistologyFlipXEnabled()) {
        std::swap(orthoLeftOut, orthoRightOut);
    }
    orthographicsProjectionOut.set(orthoLeftOut,
                                   orthoRightOut,
                                   orthoBottomOut,
                                   orthoTopOut,
                                   nearValue,
                                   farValue);
    return true;
}

/**
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param viewportContent
 *    The viewport content
 * @param browserTabContent
 *    Content of the browser tab
 * @param histologyModel
 *    Histology model for drawing
 * @param viewport
 *    Size of the viewport
 */
void
BrainOpenGLHistologySliceDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                       const BrainOpenGLViewportContent* viewportContent,
                                       BrowserTabContent* browserTabContent,
                                       ModelHistology* histologyModel,
                                       const std::array<int32_t, 4>& viewport)
{
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    CaretAssert(histologyModel);
    
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    m_browserTabContent    = browserTabContent;
    m_viewport             = GraphicsViewport(viewport);
    m_mediaFilesAndDataToDraw.clear();
    
    m_fixedPipelineDrawing->checkForOpenGLError(NULL, "At beginning of BrainOpenGLHistologySliceDrawing::draw()");

    m_identificationStereotaxicXYZValidFlag = false;
    
    HistologyOverlaySet* overlaySet = histologyModel->getHistologyOverlaySet(m_browserTabContent->getTabNumber());
    CaretAssert(overlaySet);
    const int32_t numberOfOverlays = overlaySet->getNumberOfDisplayedOverlays();
    
    /*
     * Find overlays containing files that support coordinates
     */
    bool firstFlag(true);
    Vector3D underlayStereotaxicXYZ;
    HistologyCoordinate underlayHistologyCoordinate;
    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        HistologyOverlay* overlay = overlaySet->getOverlay(iOverlay);
        CaretAssert(overlay);
        
        if (overlay->isEnabled()) {
            const HistologyOverlay::SelectionData selectionData(overlay->getSelectionData());
            const HistologySlicesFile* selectedFile(selectionData.m_selectedFile);
            if (selectedFile != NULL) {
                const HistologyCoordinate histologyCoordinate(browserTabContent->getHistologySelectedCoordinate(selectionData.m_selectedFile));
                int32_t selectedSliceIndex(histologyCoordinate.getSliceIndex());
                if (firstFlag) {
                    if (histologyCoordinate.isValid()) {
                        if (histologyCoordinate.isStereotaxicXYZValid()) {
                            firstFlag = false;
                            underlayHistologyCoordinate = histologyCoordinate;
                            underlayStereotaxicXYZ      = histologyCoordinate.getStereotaxicXYZ();
                        }
                    }
                }
                else {
                    /*
                     * Since this is not the underlay, need to find slice in this file
                     * closest to the underlay's slice
                     */
                    HistologyCoordinate hc(HistologyCoordinate::newInstanceStereotaxicXYZ(const_cast<HistologySlicesFile*>(selectedFile),
                                                                                          underlayStereotaxicXYZ));
                    if (hc.isValid()
                        && hc.isSliceIndexValid()) {
                        selectedSliceIndex = hc.getSliceIndex();
                    }
                    else {
                        CaretLogSevere("Slice index invalid for histology overlay with file: "
                                       + selectedFile->getFileName());
                        continue;
                    }
                }
                std::vector<HistologyOverlay::DrawingData> drawingData(overlay->getDrawingData(selectedSliceIndex));
                m_mediaFilesAndDataToDraw.insert(m_mediaFilesAndDataToDraw.end(),
                                                 drawingData.begin(),
                                                 drawingData.end());
            }
        }
    }
    
    if (m_mediaFilesAndDataToDraw.empty()) {
        const GraphicsViewport vp(GraphicsViewport::newInstanceCurrentViewport());
        glPushAttrib(GL_DEPTH_BITS
                     | GL_TRANSFORM_BIT);
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(vp.getLeft(), vp.getRight(), vp.getBottom(), vp.getTop(), -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        AnnotationPointSizeText text(AnnotationAttributesDefaultTypeEnum::NORMAL);
        text.setFontPointSize(AnnotationTextFontPointSizeEnum::SIZE20);
        text.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        text.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        text.setTextColor(CaretColorEnum::CUSTOM);
        text.setCustomTextColor(m_fixedPipelineDrawing->m_foregroundColorByte);
        text.setBackgroundColor(CaretColorEnum::CUSTOM);
        text.setCustomBackgroundColor(m_fixedPipelineDrawing->m_backgroundColorByte);
        text.setText("No Images");
        m_fixedPipelineDrawing->drawTextAtModelCoords(vp.getCenterX(),
                                                      vp.getCenterY(),
                                                      0.0,
                                                      text);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix(); /* pop attrib below will restore matrix mode */
        glPopAttrib();
        
        return;
    }
    
    GraphicsOrthographicProjection orthographicProjection;
    if ( ! getOrthoBounds(orthographicProjection)) {
        return;
    }

    setupScaleBars(orthographicProjection);
    
    if (s_debugFlag) {
        std::cout << orthographicProjection.toString() << std::endl;
    }

    if ((m_viewport.getWidth() < 1)
        || (m_viewport.getHeight() < 1)) {
        return;
    }
    
    m_fixedPipelineDrawing->checkForOpenGLError(NULL, "In BrainOpenGLHistologySliceDrawing::draw() before glViewport()");

    m_viewport.applyWithOpenGL();
    
    orthographicProjection.applyWithOpenGL();
    
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

    if (underlayHistologyCoordinate.isValid()) {
        const Vector3D planeXYZ(underlayHistologyCoordinate.getPlaneXYZ());
        
        const Vector3D t(planeXYZ[0],
                         planeXYZ[1],
                         0.0);
        
        glTranslated(t[0],
                     t[1],
                     0.0);
        
        const Matrix4x4 flatRotMatrix(m_browserTabContent->getHistologyRotationMatrix());
        float m[16];
        flatRotMatrix.getMatrixForOpenGL(m);
        glMultMatrixf(m);


        glTranslated(-t[0], -t[1], 0.0);
    }
    
    const float scaling = m_browserTabContent->getScaling();
    glScalef(scaling, scaling, 1.0);

    GraphicsObjectToWindowTransform* transform = new GraphicsObjectToWindowTransform();
    fixedPipelineDrawing->loadObjectToWindowTransform(transform, orthographicProjection, 0.0, true);
    viewportContent->setHistologyGraphicsObjectToWindowTransform(transform);
    
    drawModelLayers(orthographicProjection,
                    viewportContent,
                    transform,
                    underlayStereotaxicXYZ);
    
    BrainOpenGLFixedPipeline::drawGraphicsRegionSelectionBox(m_browserTabContent->getRegionSelectionBox(),
                                                             GraphicsRegionSelectionBox::DrawMode::Z_PLANE,
                                                             m_fixedPipelineDrawing->m_foregroundColorFloat);

    
    /*
     * Draw yellow cross at center of viewport
     */
    if (SessionManager::get()->getCaretPreferences()->isCrossAtViewportCenterEnabled()) {
        GraphicsShape::drawYellowCrossAtViewportCenter();
    }

    m_fixedPipelineDrawing->checkForOpenGLError(NULL, "At end of BrainOpenGLHistologySliceDrawing::draw()");
}

/**
 * Draw the models layers
 * @param orthographicProjection
 *    Orthographic projection
 * @param viewportContent
 *    The viewport content
 * @param transform
 *   Transforms point from object to window space
 * @param underlayStereotaxicXYZ
 *   Stereotaxic coordinate on the underlay histology slice
 */
void
BrainOpenGLHistologySliceDrawing::drawModelLayers(const GraphicsOrthographicProjection& orthographicProjection,
                                                  const BrainOpenGLViewportContent* viewportContent,
                                                  const GraphicsObjectToWindowTransform* transform,
                                                  const Vector3D& underlayStereotaxicXYZ)
{
    m_fixedPipelineDrawing->checkForOpenGLError(NULL, "At beginning of BrainOpenGLHistologySliceDrawing::drawModelLayers()");
    
    SelectionItemHistologyCoordinate* idHistology = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getHistologyPlaneCoordinateIdentification();
    SelectionItemAnnotation* annotationID = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getAnnotationIdentification();
    SelectionItemAnnotation* sampleID(m_fixedPipelineDrawing->m_brain->getSelectionManager()->getSamplesIdentification());

    /*
     * Check for a 'selection' type mode
     */
    bool selectImageFlag(false);
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (idHistology->isEnabledForSelection()) {
                selectImageFlag = true;
            }
            else if (annotationID->isEnabledForSelection()
                     || sampleID->isEnabledForSelection()) {
                /* continue drawing so annotations and samples get selected */
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    HistologySlice*      underlayHistologySlice(NULL);
    HistologySlicesFile* underlayHistologySlicesFile(NULL);
    AString              underlayHistologySliceName;
    
    glPushMatrix();
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    
    const int32_t numMediaFiles(static_cast<int32_t>(m_mediaFilesAndDataToDraw.size()));
    for (int32_t i = 0; i < numMediaFiles; i++) {
        CaretAssertVectorIndex(m_mediaFilesAndDataToDraw, i);
        HistologyOverlay::DrawingData& drawingData = m_mediaFilesAndDataToDraw[i];
        MediaFile* mediaFile(drawingData.m_mediaFile);
        CaretAssert(mediaFile);
        CziImageFile* cziImageFile(mediaFile->castToCziImageFile());
        ImageFile* imageFile(mediaFile->castToImageFile());

        if (cziImageFile != NULL) {
            const int32_t frameIndex(0);
            const bool allFramesSelectedFlag(true);
            const int32_t channelIndex(0);
            const int32_t manualPyramidLayerIndex(0);
            cziImageFile->updateImageForDrawingInTab(drawingData.m_tabIndex,
                                                     drawingData.m_overlayIndex,
                                                     frameIndex,
                                                     allFramesSelectedFlag,
                                                     CziImageResolutionChangeModeEnum::AUTO2,
                                                     MediaDisplayCoordinateModeEnum::PLANE,
                                                     channelIndex,
                                                     manualPyramidLayerIndex,
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


        glDisable(GL_CULL_FACE);
        GraphicsPrimitiveV3fT2f* primitive(mediaFile->getGraphicsPrimitiveForPlaneXyzDrawing(drawingData.m_tabIndex,
                                                                                             drawingData.m_overlayIndex));
        if (primitive != NULL) {
            
            /*
             * Get stencil masking image.  It contains a mask that limits the
             * region where the image is drawn to prevent overlapping of
             * other images.
             */
            GraphicsPrimitiveV3fT2f* stencilMaskingPrimitive(NULL);
            if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DEVELOPER_FLAG_HISTOLOGY_CORRECT_IMAGE_OVERLAP)) {
                if (drawingData.m_histologySliceImage != NULL) {
                    stencilMaskingPrimitive = drawingData.m_histologySliceImage->getStencilMaskingImagePrimitive();
                }
            }

            glPushMatrix();
            
            glPushAttrib(GL_COLOR_BUFFER_BIT
                         | GL_STENCIL_BUFFER_BIT);
            
            if (stencilMaskingPrimitive != NULL) {
                /*
                 * For debugging
                 * AString errorMessage;
                 * stencilMaskingPrimitive->exportTextureToImageFile("DrawingStencil.png", errorMessage);
                 */
                
                /*
                 * Enable the stencil buffer
                 */
                glEnable(GL_STENCIL_TEST);
                
                /*
                 * Enable drawing INTO the stencil buffer.
                 * Stencil buffer will receive 'stencilValue' for pixels
                 * drawn by the mask
                 */
                const GLint stencilValue(1);
                const GLuint enableStencilBufferUpdateMask(0xff);
                glStencilFunc(GL_ALWAYS, stencilValue, enableStencilBufferUpdateMask);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glStencilMask(enableStencilBufferUpdateMask);
                
                /*
                 * Disable updates to the color buffer
                 */
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                
                /*
                 * Clear the stencil buffer
                 */
                glClearStencil(0x00);
                glClear(GL_STENCIL_BUFFER_BIT);
                
                /*
                 * Use alpha test so that only texels with non-zero
                 * alpha values are placed into the stencil buffer.
                 * Using the alpha test IS ABSOLUTELY NECESSARY !!!
                 */
                glPushAttrib(GL_COLOR_BUFFER_BIT);
                glAlphaFunc(GL_NOTEQUAL, 0.0);
                glEnable(GL_ALPHA_TEST);
                
                /*
                 * Draw the stencil masking primitive to update
                 * the stencil buffer.
                 */
                GraphicsEngineDataOpenGL::draw(stencilMaskingPrimitive);
                
                /*
                 * Restore changes that were made for alpha test
                 */
                glPopAttrib();

                /*
                 * Re-enable updates to color buffer components
                 */
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                
                /*
                 * Only allow drawing image data where the stencil buffer
                 * contains 'stencilValue'
                 */
                glStencilFunc(GL_EQUAL, stencilValue, enableStencilBufferUpdateMask);
                
                /*
                 * GL_KEEP prevents changes to the stencil buffer
                 * as we don't want the image data to change the
                 * stencil buffer
                 */
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
                
                /*
                 * Using a stencil mask of 0 should also prevent
                 * image data from updating the stencil buffer
                 * Probably do not need both this and GL_KEEP.
                 */
                const GLuint disableStencilBufferUpdateMask(0x00);
                glStencilMask(disableStencilBufferUpdateMask);
                
                /*
                 * This code may read the content of the stencil buffer if
                 * needed for debugging.
                 * GLuint mypixels[width*height];
                 * glReadPixels(0, 0, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, mypixels);
                 */
            }
            CaretAssert(primitive->isValid());
            
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
            
            /*
             * Color and stencil
             */
            glPopAttrib();
            
            if (selectImageFlag) {
                processSelection(m_browserTabContent->getTabNumber(),
                                 drawingData,
                                 primitive);
            }
            
            glPopMatrix();
            
            if (underlayHistologySlicesFile == NULL) {
                underlayHistologySlicesFile  = drawingData.m_selectedFile;
                underlayHistologySlice       = drawingData.m_selectedFile->getHistologySliceByIndex(drawingData.m_selectedSliceIndex);
                underlayHistologySliceName   = drawingData.m_selectedSliceName;
            }
        }
    }
    
    glPopAttrib();
    glPopMatrix();
    
    m_fixedPipelineDrawing->checkForOpenGLError(NULL, "After drawing histology slices in BrainOpenGLHistologySliceDrawing::drawModelLayers()");
    
    drawVolumeOverlays();

    /*
     * Slice spacing (thickness) used for drawing features histology slices
     */
    const float sliceSpacing(underlayHistologySlicesFile->getSliceSpacing());
    
    /*
     * Project histology coordinate to slice
     */
    const Plane plane(underlayHistologySlice->getStereotaxicPlane());
    const Vector3D pointOnPlane(plane.projectPointToPlane(underlayStereotaxicXYZ));
    
    /*
     * Draw surface outlines
     */
    const bool useNegativePolygonOffsetFlag(true);
    BrainOpenGLVolumeSurfaceOutlineDrawing outlineDrawing;
    outlineDrawing.drawSurfaceOutline(underlayHistologySlicesFile,
                                      underlayHistologySlice,
                                      pointOnPlane,
                                      m_browserTabContent->getVolumeSurfaceOutlineSet(),
                                      m_fixedPipelineDrawing,
                                      useNegativePolygonOffsetFlag);

    /*
     * Height used for drawing ID symbols
     */
    float planeRangeY(1.0);
    if (underlayHistologySlicesFile != NULL) {
        const BoundingBox bb(underlayHistologySlicesFile->getPlaneXyzBoundingBox());
        planeRangeY = bb.getDifferenceY();
    }

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    
    /*
     * Draw identification symbols
     */
    BrainOpenGLIdentificationDrawing idDrawing(m_fixedPipelineDrawing,
                                               m_fixedPipelineDrawing->m_brain,
                                               m_browserTabContent,
                                               m_fixedPipelineDrawing->mode);
    idDrawing.drawHistologyFilePlaneCoordinateIdentificationSymbols(underlayHistologySlicesFile,
                                                                    underlayHistologySliceName,
                                                                    underlayHistologySlice->getPlaneXyzPlane(),
                                                                    sliceSpacing,
                                                                    m_browserTabContent->getScaling(),
                                                                    planeRangeY);

    /*
     * Draw Foci
     */
    BrainOpenGLFociDrawing fociDrawing;
    fociDrawing.drawHistologyFoci(m_fixedPipelineDrawing->m_brain,
                                  m_fixedPipelineDrawing,
                                  underlayHistologySlicesFile,
                                  underlayHistologySlice,
                                  underlayHistologySlice->getPlaneXyzPlane(),
                                  sliceSpacing);

    /*
     * Draw the crosshairs
     */
    const BrowserTabContent* btc(viewportContent->getBrowserTabContent());
    CaretAssert(btc);
    drawCrosshairs(orthographicProjection,
                   btc->getHistologySelectedCoordinate(underlayHistologySlicesFile));
    
    /*
     * Draw annotation in histology space
     */
    if (underlayHistologySlice != NULL) {
        HistologySpaceKey histologySpaceKey(underlayHistologySlicesFile->getFileName(),
                                            underlayHistologySliceName);
        m_fixedPipelineDrawing->drawHistologySpaceAnnotations(viewportContent,
                                                              histologySpaceKey,
                                                              underlayHistologySlice,
                                                              sliceSpacing);
    }
    
    glPopAttrib();
}

/**
 * Process selection in amedia  file
 * @param tabIndex
 *   Index of the tab
 * @param drawingData
 *   Info about selections in the layer
 * @param primitive
 *    Primitive that draws image file
 */
void
BrainOpenGLHistologySliceDrawing::processSelection(const int32_t tabIndex,
                                                   const HistologyOverlay::DrawingData& drawingData,
                                                   GraphicsPrimitiveV3fT2f* primitive)
{
    SelectionItemHistologyCoordinate* idHistology = m_fixedPipelineDrawing->m_brain->getSelectionManager()->getHistologyPlaneCoordinateIdentification();
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
            
            if (s_debugFlag) {
                int32_t stencilData[100];
                glReadPixels(this->m_fixedPipelineDrawing->mouseX,
                             this->m_fixedPipelineDrawing->mouseY,
                             1, 1,
                             GL_STENCIL_INDEX, GL_INT, static_cast<GLvoid*>(stencilData));
                std::cout << "Stencil: " << stencilData[0] << std::endl;
            }
            
            /*
             * The window Z-coordinate is the value in the depth buffer.
             * Since the histology slices are always drawn with plane-Z equal
             * to zero, the depth value will be 0.5 (provided the near and far
             * values passed to glOrtho are the -X and X).  However, if
             * the user clicks outside of an image, the depth value will
             * be 1 since no image was drawn.  But, using a depth value of 1
             * will cause an incorrect steretaxic coordinate whebn the plane
             * coordinate is converted to a stereotaxic coordinate and this
             * may cause the selected histology slice to change.  So,
             * always use 0.5 for the depth so that the output plane-Z
             * will be zero.
             */
            const float depthValue(0.5);
            Vector3D windowXYZ(mouseX, mouseY, depthValue);
            Vector3D planeXYZ;
            xform.inverseTransformPoint(windowXYZ, planeXYZ);
            
            const float tooSmall(-0.0001);
            const float tooBig(0.0001);
            if ((planeXYZ[2] < tooSmall)
                || (planeXYZ[2] > tooBig)) {
                CaretLogWarning("Transformation of window coord to plane coord has non-zero plane-Z.  "
                                "Window XYZ=" + windowXYZ.toString(5)
                                + " Plane XYZ=" + planeXYZ.toString(5));
            }
            
            MediaFile* mediaFile(drawingData.m_mediaFile);
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
            for (int32_t i = 0; i < mediaFile->getNumberOfFrames(); i++) {
                validFrameIndices.push_back(i);
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
            
            /*
             * Multiple layers/images may be drawn.
             * Always give highest priority to pixel that is within an image.
             */
            bool replaceFlag(false);
            if (validPixelFlag) {
                replaceFlag = true;
            }
            else if ( ! idHistology->isPixelRGBAValid()) {
                replaceFlag = true;
            }
            if (replaceFlag) {
                HistologyCoordinate hc(HistologyCoordinate::newInstancePlaneXYZIdentification(drawingData.m_selectedFile,
                                                                                      drawingData.m_mediaFile,
                                                                                      drawingData.m_selectedSliceIndex,
                                                                                      planeXYZ));
                idHistology->setHistologySlicesFile(drawingData.m_selectedFile);
                idHistology->setCoordinate(hc);
                idHistology->setModelXYZ(hc.getPlaneXYZ());
                idHistology->setTabIndex(tabIndex);
                idHistology->setOverlayIndex(drawingData.m_overlayIndex);
                uint8_t pixelByteRGBA[4] = { 0, 0, 0, 0 };
                idHistology->setScreenXYZ(windowXYZ);
                idHistology->setScreenDepth(0.0);
                if (validPixelFlag) {
                    if (mediaFile->getPixelRGBA(tabIndex,
                                                drawingData.m_overlayIndex,
                                                pixelLogicalIndex,
                                                pixelByteRGBA)) {
                        idHistology->setPixelRGBA(pixelByteRGBA);
                    }
                }
                
                if (drawingData.m_mediaFile->planeXyzToStereotaxicXyz(hc.getPlaneXYZ(),
                                                                      m_identificationStereotaxicXYZ)) {
                    m_identificationStereotaxicXYZValidFlag = true;
                }
            }
        }
    }
}

/**
 * Draw the histology crosshairs
 * @param orthographicsProjection;
 *    Orthographic projection
 * @param histologyCoordinate
 *    The histology coordinate
 */
void
BrainOpenGLHistologySliceDrawing::drawCrosshairs(const GraphicsOrthographicProjection& orthographicsProjection,
                                                 const HistologyCoordinate& histologyCoordinate)
{
    glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    

    
    const float minX(orthographicsProjection.getLeft());
    const float maxX(orthographicsProjection.getRight());
    const float maxY(orthographicsProjection.getBottom());
    const float minY(orthographicsProjection.getTop());
    
    const float* red(CaretColorEnum::toRGBA(CaretColorEnum::RED));
    const float* green(CaretColorEnum::toRGBA(CaretColorEnum::GREEN));
    
    const Vector3D centerXYZ(histologyCoordinate.getPlaneXYZ());
    const float z(0.0);
    if (s_debugFlag) {
        std::cout << "Selected histology plane: " << AString::fromNumbers(histologyCoordinate.getPlaneXYZ()) << std::endl;
        std::cout << "             stereotaxic: " << AString::fromNumbers(histologyCoordinate.getStereotaxicXYZ()) << std::endl;
    }
    
    if (m_browserTabContent->isHistologyAxesCrosshairsDisplayed()) {
        std::unique_ptr<GraphicsPrimitiveV3fC4f> primitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
        primitive->addVertex(minX, centerXYZ[1], z, green);
        primitive->addVertex(maxX, centerXYZ[1], z, green);
        primitive->addVertex(centerXYZ[0], minY, z, red);
        primitive->addVertex(centerXYZ[0], maxY, z, red);
        
        const float lineWidthPercentage(0.5);
        primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                lineWidthPercentage);
        GraphicsEngineDataOpenGL::draw(primitive.get());
    }
    
    if (m_browserTabContent->isHistologyAxesCrosshairsLabelsDisplayed()) {
        if ( ! m_mediaFilesAndDataToDraw.empty()) {
            const HistologySlice* histologySlice(m_mediaFilesAndDataToDraw[0].m_histologySlice);
            if (histologySlice != NULL) {
                AString westText, eastText, northText, southText;
                histologySlice->getAxisLabels(m_browserTabContent->isHistologyFlipXEnabled(),
                                              westText, eastText, southText, northText);
                
                const std::array<uint8_t, 4> backgroundRGBA = {
                    m_fixedPipelineDrawing->m_backgroundColorByte[0],
                    m_fixedPipelineDrawing->m_backgroundColorByte[1],
                    m_fixedPipelineDrawing->m_backgroundColorByte[2],
                    m_fixedPipelineDrawing->m_backgroundColorByte[3]
                };

                const std::array<uint8_t, 4> foregroundRGBA = {
                    m_fixedPipelineDrawing->m_foregroundColorByte[0],
                    m_fixedPipelineDrawing->m_foregroundColorByte[1],
                    m_fixedPipelineDrawing->m_foregroundColorByte[2],
                    m_fixedPipelineDrawing->m_foregroundColorByte[3]
                };
                
                GraphicsViewport vp(GraphicsViewport::newInstanceCurrentViewport());
                glPushAttrib(GL_DEPTH_BITS
                             | GL_TRANSFORM_BIT);
                glDisable(GL_DEPTH_TEST);
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                glOrtho(vp.getLeft(), vp.getRight(), vp.getBottom(), vp.getTop(), -1.0, 1.0);
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();

                /*
                * Offset text labels be a percentage of viewort width/height
                */
                const float textOffsetX = static_cast<int>(vp.getWidth() * 0.01f);
                const float textOffsetY = static_cast<int>(vp.getHeight() * 0.01f);
                const float orthoWidth(static_cast<int>(vp.getWidth()));
                const float orthoHeight(static_cast<int>(vp.getHeight()));

                const float textLeftWindowXY[2] = {
                    textOffsetX,
                    (orthoHeight / 2.0f)
                };
                const float textRightWindowXY[2] = {
                    (orthoWidth - textOffsetX),
                    (orthoHeight / 2.0f)
                };
                const float textBottomWindowXY[2] = {
                    (orthoWidth / 2.0f),
                    textOffsetY
                };
                const float textTopWindowXY[2] = {
                    (orthoWidth / 2.0f),
                    (orthoHeight - textOffsetY),
                };

                AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
                annotationText.setBoldStyleEnabled(true);
                annotationText.setFontPercentViewportSize(5.0f);
                annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
                annotationText.setTextColor(CaretColorEnum::CUSTOM);
                annotationText.setCustomTextColor(foregroundRGBA.data());
                annotationText.setCustomBackgroundColor(backgroundRGBA.data());
                
                if ( ! westText.isEmpty()) {
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                    annotationText.setText(westText);
                    m_fixedPipelineDrawing->drawTextAtViewportCoords(textLeftWindowXY[0],
                                                                     textLeftWindowXY[1],
                                                                     annotationText);
                }
                
                if ( ! eastText.isEmpty()) {
                    annotationText.setText(eastText);
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
                    m_fixedPipelineDrawing->drawTextAtViewportCoords(textRightWindowXY[0],
                                                                     textRightWindowXY[1],
                                                                     annotationText);
                }
                
                if ( ! southText.isEmpty()) {
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                    annotationText.setText(southText);
                    m_fixedPipelineDrawing->drawTextAtViewportCoords(textBottomWindowXY[0],
                                                                     textBottomWindowXY[1],
                                                                     annotationText);
                }
                
                if ( ! northText.isEmpty()) {
                    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
                    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
                    annotationText.setText(northText);
                    m_fixedPipelineDrawing->drawTextAtViewportCoords(textTopWindowXY[0],
                                                                     textTopWindowXY[1],
                                                                     annotationText);
                }
                glPopMatrix();
                glMatrixMode(GL_PROJECTION);
                glPopMatrix(); /* pop attrib below will restore matrix mode */
                glPopAttrib();
            }
        }
    }
    glPopAttrib();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLHistologySliceDrawing::toString() const
{
    return "BrainOpenGLHistologySliceDrawing";
}

/**
 * Draw the volume overlays
 */
void
BrainOpenGLHistologySliceDrawing::drawVolumeOverlays()
{
    if (m_mediaFilesAndDataToDraw.empty()) {
        return;
    }
    
    /*
     * Find any volumes to draw
     */
    std::vector<VolumeDrawingInfo> volumeDrawingInfo;
    OverlaySet* overlaySet(m_browserTabContent->getOverlaySet());
    CaretAssert(overlaySet);
    const int32_t numberOfOverlays(overlaySet->getNumberOfDisplayedOverlays());
    for (int32_t iOverlay = (numberOfOverlays - 1); iOverlay >= 0; iOverlay--) {
        Overlay* overlay(overlaySet->getOverlay(iOverlay));
        CaretAssert(overlay);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* mapFile(NULL);
            int32_t mapIndex(-1);
            overlay->getSelectionData(mapFile,
                                      mapIndex);
            
            if (mapFile != NULL) {
                VolumeMappableInterface* vmi(dynamic_cast<VolumeMappableInterface*>(mapFile));
                if (vmi != NULL) {
                    volumeDrawingInfo.emplace_back(overlay,
                                                   vmi,
                                                   mapIndex);
                }
                else {
                    CaretLogSevere("File for drawing volume is not a volume mappable file: "
                                   + mapFile->getFileName());
                }
            }
        }
    }
    
    if (volumeDrawingInfo.empty()) {
        /*
         * No volumes to draw as overlays
         */
        return;
    }
    
    const int32_t numMediaFiles(m_mediaFilesAndDataToDraw.size());
    if (numMediaFiles > 0) {
        const MediaFile* underlayMediaFile(m_mediaFilesAndDataToDraw[0].m_mediaFile);
        if (underlayMediaFile->castToCziImageFile() != NULL) {
            /* ok */
        }
        else if (underlayMediaFile->castToImageFile()) {
            /* ok */
        }
        else {
            /*
             * Neither CZI nor Image File
             */
            static std::set<const MediaFile*> invalidMediaFiles;
            if (invalidMediaFiles.find(underlayMediaFile) == invalidMediaFiles.end()) {
                CaretLogSevere(underlayMediaFile->getFileName()
                               + " is neither CZI nor Image File for mapping volume to histology.");
                invalidMediaFiles.insert(underlayMediaFile);
            }
        }
    }

    drawVolumeOverlaysOnCziImageFile(volumeDrawingInfo);
}

/**
 * Draw volume overlays on a CziImage
 * @param volumeDrawingInfo
 *    Info on volumes for drawing
 */
void
BrainOpenGLHistologySliceDrawing::drawVolumeOverlaysOnCziImageFile(std::vector<VolumeDrawingInfo>& volumeDrawingInfo)
{
    const DisplayPropertiesLabels* dsl = m_fixedPipelineDrawing->m_brain->getDisplayPropertiesLabels();
    const int32_t tabIndex(m_browserTabContent->getTabNumber());
    const DisplayGroupEnum::Enum displayGroup = dsl->getDisplayGroupForTab(tabIndex);
    const LabelViewModeEnum::Enum labelViewMode(dsl->getLabelViewModeForTab(tabIndex));
    
    glPushAttrib(GL_DEPTH_BUFFER_BIT
                 | GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    CaretAssertVectorIndex(m_mediaFilesAndDataToDraw, 0);
    MediaFile* mediaFile(m_mediaFilesAndDataToDraw[0].m_mediaFile);
    CaretAssert(mediaFile);
    const HistologySlice* histologySlice(m_mediaFilesAndDataToDraw[0].m_histologySlice);
    CaretAssert(histologySlice);
    
    
    for (auto& vdi : volumeDrawingInfo) {
        Overlay* overlay(vdi.m_overlay);
        VolumeMappableInterface* vmi(vdi.m_volumeMappableInterface);
        const int32_t mapIndex(vdi.m_mapIndex);
        CaretAssert(overlay);
        CaretAssert(vmi);
        CaretAssert(mapIndex >= 0);
        AString errorMessage;
        const TabDrawingInfo tabDrawingInfo(dynamic_cast<CaretMappableDataFile*>(vmi),
                                            mapIndex,
                                            displayGroup,
                                            labelViewMode,
                                            tabIndex);
        std::vector<GraphicsPrimitive*> primitives(vmi->getHistologySliceIntersectionPrimitive(mapIndex,
                                                                                               tabDrawingInfo,
                                                                                               histologySlice,
                                                                                               overlay->getVolumeToImageMappingMode(),
                                                                                               overlay->getVolumeToImageMappingThickness(),
                                                                                               errorMessage));
        if ( ! primitives.empty()) {
            const float alphaValue(overlay->getOpacity());
            glPushAttrib(GL_COLOR_BUFFER_BIT);
            if (alphaValue < 1.0) {
                CaretAssert((alphaValue >= 0.0)
                            && (alphaValue <= 1.0));
                
                /*
                 * The constant alpha comes from the overlay.
                 * The layer being drawn gets (RGB * alphaValue)
                 * and current frame buffer gets (FrameRGB * (1 - alphaValue)
                 */
                glBlendColor(alphaValue, alphaValue, alphaValue, 0.0);
                glBlendFuncSeparate(GL_CONSTANT_COLOR,           /* source (incoming) RGB blending factor */
                                    GL_ONE_MINUS_CONSTANT_COLOR, /* destination (frame buffer) RGB blending factor */
                                    GL_ONE,                /* source (incoming) Alpha blending factor */
                                    GL_ZERO);                /* destination (frame buffer) Alpha blending factor */
                glEnable(GL_BLEND);
                
                /*
                 * Only allow framebuffer update if the incoming alpha is greater than
                 * zero.  For a label volume, voxels have alpha equal to zero
                 * where there is no label.  This prevents an drawing of these
                 * zero alpha voxels while allowing blending.
                 */
                glAlphaFunc(GL_GREATER, 0.0);
                glEnable(GL_ALPHA_TEST);
            }
            else {
                m_fixedPipelineDrawing->setupBlending(BrainOpenGLFixedPipeline::BlendDataType::FEATURE_IMAGE);
                
            }
            for (GraphicsPrimitive* p : primitives) {
                GraphicsEngineDataOpenGL::draw(p);
            }
            glPopAttrib();
            
            if (m_identificationStereotaxicXYZValidFlag) {
                const Plane* plane(mediaFile->getStereotaxicImagePlane());
                int64_t voxelIJK[3];
                vmi->enclosingVoxel(m_identificationStereotaxicXYZ, voxelIJK);
                const float screenDepth(0.0);
                SelectionItemVoxel* voxelSelection(m_fixedPipelineDrawing->m_brain->getSelectionManager()->getVoxelIdentification());
                voxelSelection->setVoxelIdentification(m_fixedPipelineDrawing->m_brain,
                                                       vmi,
                                                       voxelIJK,
                                                       m_identificationStereotaxicXYZ,
                                                       *plane,
                                                       screenDepth);
            }
        }
        else {
            CaretLogSevere(errorMessage);
        }
    }
    
    glPopAttrib();
}

/**
 * Setup the scale bars for histology slice drawing
 * @param orthographicProjection
 *    The orthographic projection
 */
void
BrainOpenGLHistologySliceDrawing::setupScaleBars(const GraphicsOrthographicProjection& orthographicProjection)
{
    /*
     * Must have histology slice to set the stereotaxic coordinates at the left and right sides
     * of the viewport
     */
    for (const HistologyOverlay::DrawingData& dd : m_mediaFilesAndDataToDraw) {
        const HistologySlice* slice(dd.m_histologySlice);
        if (slice != NULL) {
            const Vector3D pLeft(orthographicProjection.getLeft(), 0.0, 0.0);
            const Vector3D pRight(orthographicProjection.getRight(), 0.0, 0.0);
            Vector3D xyzLeft;
            Vector3D xyzRight;
            if (slice->planeXyzToStereotaxicXyz(pLeft, xyzLeft)
                && slice->planeXyzToStereotaxicXyz(pRight, xyzRight)) {
                /*
                 * Needed for scale bar drawing
                 */
                m_fixedPipelineDrawing->setupScaleBarDrawingInformation(m_browserTabContent,
                                                                        xyzLeft[0],
                                                                        xyzRight[0]);
                return;
            }
        }
    }
}
