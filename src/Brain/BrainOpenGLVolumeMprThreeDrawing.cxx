
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

#define __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_DECLARE__
#include "BrainOpenGLVolumeMprThreeDrawing.h"
#undef __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_DECLARE__

#include <cmath>
#include <limits>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AnnotationPercentSizeText.h"
#include "Brain.h"
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#include "BrainOpenGLFociDrawing.h"
#include "BrainOpenGLIdentificationDrawing.h"
#include "BrainOpenGLShapeRing.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "BrainOpenGLVolumeSurfaceOutlineDrawing.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"
#include "CaretPreferences.h"
#include "CiftiMappableDataFile.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesVolume.h"
#include "EventBrowserWindowContent.h"
#include "EventDrawingViewportContentAdd.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "GapsAndMargins.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "GraphicsShape.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "GraphicsViewport.h"
#include "MathFunctions.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "SamplesDrawingSettings.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemVoxel.h"
#include "SelectionItemVoxelEditing.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "TabDrawingInfo.h"
#include "VolumeVerticesEdgesFaces.h"
#include "VolumeFile.h"
#include "VolumeMprViewportSlice.h"
#include "VolumePlaneIntersection.h"

using namespace caret;

/**
 * \class caret::BrainOpenGLVolumeMprThreeDrawing
 * \brief Multiplanar Reconstruction volume drawing
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLVolumeMprThreeDrawing::BrainOpenGLVolumeMprThreeDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLVolumeMprThreeDrawing::~BrainOpenGLVolumeMprThreeDrawing()
{
}

/**
 * Draw the volume slices
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param viewportContent
 *    Content of viewport
 * @param browserTabContent
 *    Content of the browser tab
 * @param volumeDrawInfo
 *    Volumes being drawn
 * @param viewport
 *    The viewport
 */
void
BrainOpenGLVolumeMprThreeDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                     const BrainOpenGLViewportContent* viewportContent,
                                     BrowserTabContent* browserTabContent,
                                     const std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                     const GraphicsViewport& viewport)
{
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    CaretAssert(m_fixedPipelineDrawing);
    
    m_browserTabContent = browserTabContent;
    CaretAssert(m_browserTabContent);
    
    m_brainModelMode = BrainModelMode::INVALID;
    ModelVolume* modelVolume(browserTabContent->getDisplayedVolumeModel());
    ModelWholeBrain* modelWholeBrain(browserTabContent->getDisplayedWholeBrainModel());
    if (modelVolume != NULL) {
        m_brain = modelVolume->getBrain();
        m_brainModelMode = BrainModelMode::VOLUME_2D;
    }
    else if (modelWholeBrain != NULL) {
        m_brain = modelWholeBrain->getBrain();
        m_brainModelMode = BrainModelMode::ALL_3D;
    }
    else {
        const AString msg("Neither Volume nor All (Whole Brain) view");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    CaretAssert(m_brain);
    CaretAssert(m_brainModelMode != BrainModelMode::INVALID);

    const DisplayPropertiesLabels* dsl = m_brain->getDisplayPropertiesLabels();
    m_displayGroup = dsl->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    m_tabIndex = m_browserTabContent->getTabNumber();
    m_labelViewMode = dsl->getLabelViewModeForTab(m_tabIndex);

    m_mprViewMode = browserTabContent->getVolumeMprViewMode();
    m_orientationMode = browserTabContent->getVolumeMprOrientationMode();
    
    if (m_browserTabContent->isWholeBrainDisplayed()) {
        /*
         * For now, no intensity modes in 3D as drawing accumulates in the
         * framebuffer and in 3D the slices will overlap causing problems.
         * May be fixable by drawing each slice into an offscreen buffer and
         * then transfering that to the framebuffer.
         */
        m_mprViewMode = VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION;
    }
    
    m_volumeDrawInfo = volumeDrawInfo;

    /*
     * Test if underlay is a single slice volume file
     */
    m_underlayIsSingleSliceVolumeFlag = false;
    if ( ! m_volumeDrawInfo.empty()) {
        CaretAssertVectorIndex(m_volumeDrawInfo, 0);
        if (m_volumeDrawInfo[0].volumeFile->isSingleSlice()) {
            /*
             * Intensity modes will not work with single slice volume file
             */
            m_mprViewMode = VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION;
            m_underlayIsSingleSliceVolumeFlag = true;
        }
    }
    
    /*
     * Filter the volume drawing info
     */
    filterVolumeDrawInfo(m_volumeDrawInfo);
    
    const int32_t numberOfVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    if (numberOfVolumes <= 0) {
        return;
    }
    CaretAssert( ! m_volumeDrawInfo.empty());

    /*
     * No lighting for drawing slices
     */
    m_fixedPipelineDrawing->disableLighting();

    const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType(m_browserTabContent->getVolumeSliceProjectionType());
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            CaretAssert(0);
            break;
    }
    
    VolumeSliceDrawingTypeEnum::Enum sliceDrawingType = m_browserTabContent->getVolumeSliceDrawingType();
    if (m_underlayIsSingleSliceVolumeFlag) {
        /* No montage with underlay single slice volume file */
        sliceDrawingType = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE;
    }

    m_axialCoronalParaSliceViewFlag = false;
    if (m_browserTabContent->isWholeBrainDisplayed()) {
        drawWholeBrainView(viewportContent,
                           sliceDrawingType,
                           sliceProjectionType,
                           viewport);
    }
    else if (m_browserTabContent->isVolumeSlicesDisplayed()) {
        m_axialCoronalParaSliceViewFlag = true;
        drawSliceView(viewportContent,
                      browserTabContent,
                      sliceDrawingType,
                      sliceProjectionType,
                      viewport);
    }
    else {
        CaretAssert(0);
    }
}

/**
 * Draw the volume slice view in Volume mode
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param viewportContent
 *    Content of viewport
 * @param browserTabContent
 *    Content of the browser tab being drawn
 * @param volumeDrawInfo
 *    Volumes being drawn
 * @param viewport
 *    The viewport
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawSliceView(const BrainOpenGLViewportContent* viewportContent,
                                              BrowserTabContent* browserTabContent,
                                              const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                              const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                              const GraphicsViewport& viewport)
{
    CaretAssertVectorIndex(m_volumeDrawInfo, 0);
    const VolumeMappableInterface* underlayVolume(m_volumeDrawInfo[0].volumeFile);

    VolumeSliceViewPlaneEnum::Enum sliceViewPlane(browserTabContent->getVolumeSliceViewPlane());
    if (m_underlayIsSingleSliceVolumeFlag) {
        /* only one view for single slice volume file */
        sliceViewPlane = VolumeSliceViewPlaneEnum::AXIAL;
    }
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout(browserTabContent->getVolumeSlicePlanesAllViewLayout());
            
            /*
             * Draw parasagittal slice
             */
            std::array<int32_t, 4> axisVP;
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport.getViewport().data(),
                                                                VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                                                allPlanesLayout,
                                                                axisVP.data());
            EventDrawingViewportContentAdd addParaViewportEvent;
            addParaViewportEvent.addModel(this->m_fixedPipelineDrawing->m_windowIndex,
                                          m_tabIndex,
                                          GraphicsViewport(axisVP.data()),
                                          ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES);
            EventManager::get()->sendEvent(addParaViewportEvent.getPointer());

            glPushMatrix();
            drawVolumeSliceViewType(viewportContent,
                                    sliceProjectionType,
                                    sliceDrawingType,
                                    VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                    axisVP);
            glPopMatrix();
            
            /*
             * Draw coronal slice
             */
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport.getViewport().data(),
                                                                VolumeSliceViewPlaneEnum::CORONAL,
                                                                allPlanesLayout,
                                                                axisVP.data());
            glPushMatrix();
            EventDrawingViewportContentAdd addCoronalViewportEvent;
            addCoronalViewportEvent.addModel(this->m_fixedPipelineDrawing->m_windowIndex,
                                          m_tabIndex,
                                          GraphicsViewport(axisVP.data()),
                                          ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES);
            EventManager::get()->sendEvent(addCoronalViewportEvent.getPointer());
            drawVolumeSliceViewType(viewportContent,
                                    sliceProjectionType,
                                    sliceDrawingType,
                                    VolumeSliceViewPlaneEnum::CORONAL,
                                    axisVP);
            glPopMatrix();
            
            /*
             * Draw axial slice
             */
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport.getViewport().data(),
                                                                VolumeSliceViewPlaneEnum::AXIAL,
                                                                allPlanesLayout,
                                                                axisVP.data());
            EventDrawingViewportContentAdd addAxialViewportEvent;
            addAxialViewportEvent.addModel(this->m_fixedPipelineDrawing->m_windowIndex,
                                          m_tabIndex,
                                          GraphicsViewport(axisVP.data()),
                                          ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES);
            EventManager::get()->sendEvent(addAxialViewportEvent.getPointer());
            glPushMatrix();
            drawVolumeSliceViewType(viewportContent,
                                    sliceProjectionType,
                                    sliceDrawingType,
                                    VolumeSliceViewPlaneEnum::AXIAL,
                                    axisVP);
            glPopMatrix();
            
            
            switch (allPlanesLayout) {
                case VolumeSliceViewAllPlanesLayoutEnum::COLUMN_LAYOUT:
                    break;
                case VolumeSliceViewAllPlanesLayoutEnum::GRID_LAYOUT:
                {
                    /*
                     * Draw rotation box only in grid layout
                     */
                    BrainOpenGLViewportContent::getSliceAllViewViewport(viewport.getViewport().data(),
                                                                        VolumeSliceViewPlaneEnum::ALL,
                                                                        allPlanesLayout,
                                                                        axisVP.data());
                    glPushMatrix();
                    const bool drawAllThreeAxesFlag(false);
                    const bool drawSingleAxesFlag(false);
                    const bool drawBoxAxesFlag(true);
                    if (drawAllThreeAxesFlag) {
                        drawAllViewRotationThreeAxes(axisVP.data());
                    }
                    if (drawSingleAxesFlag) {
                        drawAllViewRotationAxes(axisVP.data());
                    }
                    if (drawBoxAxesFlag) {
                        drawAllViewRotationBox(underlayVolume,
                                               axisVP.data());
                    }
                    glPopMatrix();
                }
                    break;
                case VolumeSliceViewAllPlanesLayoutEnum::ROW_LAYOUT:
                    break;
            }
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
        {
            glPushMatrix();
            drawVolumeSliceViewType(viewportContent,
                                    sliceProjectionType,
                                    sliceDrawingType,
                                    sliceViewPlane,
                                    viewport);
            glPopMatrix();
        }
            break;
    }
}

/**
 * Draw a box showing the current rotation for each axis
 * @param viewport
 *   The viewport
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawAllViewRotationThreeAxes(const int32_t viewportIn[4])
{
    /*
     * Set the modeling transformation
     */
    Matrix4x4 matrix(m_browserTabContent->getMprThreeRotationMatrix());
    
    switch (VolumeMprVirtualSliceView::getViewTypeForVolumeSliceView()) {
        case VolumeMprVirtualSliceView::ViewType::VOLUME_VIEW_FIXED_CAMERA:
            break;
        case VolumeMprVirtualSliceView::ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            break;
        case VolumeMprVirtualSliceView::ViewType::ALL_VIEW_SLICES:
            break;
    }
    
    const GraphicsViewport fullVP(viewportIn);
    
    const bool drawCylindersFlag(true);
    const bool drawLabelsFlag(true);
    
    for (int32_t i = 0; i < 3; i++) {
        GraphicsViewport viewport;
        
        const int32_t vpHalfWidth(fullVP.getWidth() / 2);
        const int32_t vpHalfHeight(fullVP.getHeight() / 2);
        
        const float eyeOffsetDistance(100);
        Vector3D lookAtUpVector;
        Vector3D lookAtEye;
        switch (i) {
            case 0:
                /*
                 * Parasagittal view
                 */
                viewport = GraphicsViewport(fullVP.getLeft(),
                                            fullVP.getCenterY(),
                                            vpHalfWidth,
                                            vpHalfHeight);
                lookAtEye.set(-eyeOffsetDistance, 0.0, 0.0);
                lookAtUpVector.set(0.0, 0.0, 1.0);
                break;
            case 1:
                /*
                 * Coronal view
                 */
                viewport = GraphicsViewport(fullVP.getCenterX(),
                                            fullVP.getCenterY(),
                                            vpHalfWidth,
                                            vpHalfHeight);
                lookAtEye.set(0.0, -eyeOffsetDistance, 0.0);
                lookAtUpVector.set(0.0, 0.0, 1.0);
                break;
            case 2:
                /*
                 * Axial view
                 */
                viewport = GraphicsViewport(fullVP.getCenterX(),
                                            fullVP.getBottom(),
                                            vpHalfWidth,
                                            vpHalfHeight);
                lookAtEye.set(0.0, 0.0, eyeOffsetDistance);
                lookAtUpVector.set(0.0, 1.0, 0.0);
                break;
        }
        /*
         * Set the viewport
         */
        glViewport(viewport.getX(),
                   viewport.getY(),
                   viewport.getWidth(),
                   viewport.getHeight());
        const double viewportWidth  = viewport.getWidthF();
        const double viewportHeight = viewport.getHeightF();
        
        /*
         * Determine bounds for orthographic projection
         */
        const double maxCoord = 100.0;
        const double minCoord = -maxCoord;
        double left   = 0.0;
        double right  = 0.0;
        double top    = 0.0;
        double bottom = 0.0;
        const double nearDepth = -1000.0;
        const double farDepth  =  1000.0;
        if (viewportHeight > viewportWidth) {
            left  = minCoord;
            right = maxCoord;
            const double aspectRatio = (viewportHeight
                                        / viewportWidth);
            top   = maxCoord * aspectRatio;
            bottom = minCoord * aspectRatio;
        }
        else {
            const double aspectRatio = (viewportWidth
                                        / viewportHeight);
            top   = maxCoord;
            bottom = minCoord;
            left  = minCoord * aspectRatio;
            right = maxCoord * aspectRatio;
        }
        /*
         * Set the orthographic projection
         */
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(left, right,
                bottom, top,
                nearDepth, farDepth);
        
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        {
            /*
             * Set the viewing transformation, places 'eye' so that it looks
             * at the 'model' which is, in this case, the axes
             */
            const double centerX = 0;
            const double centerY = 0;
            const double centerZ = 0;
            gluLookAt(lookAtEye[0], lookAtEye[1], lookAtEye[2],
                      centerX, centerY, centerZ,
                      lookAtUpVector[0], lookAtUpVector[1], lookAtUpVector[2]);
            
            double rotationMatrix[16];
            matrix.getMatrixForOpenGL(rotationMatrix);
            glMultMatrixd(rotationMatrix);
            
            
            /*
             * Disable depth buffer.  Otherwise, when volume slices are drawn
             * black regions of the slices may set depth buffer and the occlude
             * the axes from display.
             */
            GLboolean depthBufferEnabled = false;
            glGetBooleanv(GL_DEPTH_TEST,
                          &depthBufferEnabled);
            glDisable(GL_DEPTH_TEST);
            const float red[4] = {
                1.0, 0.0, 0.0, 1.0
            };
            const float green[4] = {
                0.0, 1.0, 0.0, 1.0
            };
            const float blue[4] = {
                0.0, 0.0, 1.0, 1.0
            };
            
            const double axisMaxCoord = maxCoord * 0.8;
            const double axisMinCoord = -axisMaxCoord;
            const double textMaxCoord = maxCoord * 0.9;
            const double textMinCoord = -textMaxCoord;
            
            const float axialPlaneMin[3] = { 0.0, 0.0, (float)axisMinCoord };
            const float axialPlaneMax[3] = { 0.0, 0.0, (float)axisMaxCoord };
            const double axialTextMin[3]  = { 0.0, 0.0, (float)textMinCoord };
            const double axialTextMax[3]  = { 0.0, 0.0, (float)textMaxCoord };
            
            const float coronalPlaneMin[3] = { (float)axisMinCoord, 0.0, 0.0 };
            const float coronalPlaneMax[3] = { (float)axisMaxCoord, 0.0, 0.0 };
            const double coronalTextMin[3]  = { (float)textMinCoord, 0.0, 0.0 };
            const double coronalTextMax[3]  = { (float)textMaxCoord, 0.0, 0.0 };
            
            const float paraPlaneMin[3] = { 0.0, (float)axisMinCoord, 0.0 };
            const float paraPlaneMax[3] = { 0.0, (float)axisMaxCoord, 0.0 };
            const double paraTextMin[3]  = { 0.0, (float)textMinCoord, 0.0 };
            const double paraTextMax[3]  = { 0.0, (float)textMaxCoord, 0.0 };
            
            /*
             * Set radius as percentage of viewport height
             */
            float axesCrosshairRadius = 1.0;
            if (viewportHeight > 0) {
                const float percentageRadius = 0.005f;
                axesCrosshairRadius = percentageRadius * viewportHeight;
            }
            
            
            const std::array<uint8_t, 4> backgroundRGBA = {
                m_fixedPipelineDrawing->m_backgroundColorByte[0],
                m_fixedPipelineDrawing->m_backgroundColorByte[1],
                m_fixedPipelineDrawing->m_backgroundColorByte[2],
                m_fixedPipelineDrawing->m_backgroundColorByte[3]
            };
            
            AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
            annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
            annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
            annotationText.setFontPercentViewportSize(5.0f);
            annotationText.setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
            annotationText.setTextColor(CaretColorEnum::CUSTOM);
            annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
            annotationText.setCustomBackgroundColor(backgroundRGBA.data());
            
            if (drawCylindersFlag) {
                m_fixedPipelineDrawing->drawCylinder(blue,
                                                     axialPlaneMin,
                                                     axialPlaneMax,
                                                     axesCrosshairRadius * 0.5f);
                m_fixedPipelineDrawing->drawCylinder(green,
                                                     coronalPlaneMin,
                                                     coronalPlaneMax,
                                                     axesCrosshairRadius * 0.5f);
                m_fixedPipelineDrawing->drawCylinder(red,
                                                     paraPlaneMin,
                                                     paraPlaneMax,
                                                     axesCrosshairRadius * 0.5f);
            }
            
            /*
             * Draw axes labels after cyclinder (axes lines) so that
             * the text background will obscure the cyclinders.
             */
            if (drawLabelsFlag) {
                annotationText.setCustomTextColor(blue);
                annotationText.setText("I");
                m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMin,
                                                              annotationText);
                annotationText.setText("S");
                m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMax,
                                                              annotationText);
                
                annotationText.setCustomTextColor(green);
                annotationText.setText("L");
                m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMin,
                                                              annotationText);
                annotationText.setText("R");
                m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMax,
                                                              annotationText);
                
                annotationText.setCustomTextColor(red);
                annotationText.setText("P");
                m_fixedPipelineDrawing->drawTextAtModelCoords(paraTextMin,
                                                              annotationText);
                
                annotationText.setText("A");
                m_fixedPipelineDrawing->drawTextAtModelCoords(paraTextMax,
                                                              annotationText);
            }
        }
        glPopMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
}

/**
 * Draw axes showing the current rotation
 * @param viewport
 *   The viewport
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawAllViewRotationAxes(const int32_t viewport[4])
{
    /*
     * Set the modeling transformation
     */
    Matrix4x4 matrix(m_browserTabContent->getMprThreeRotationMatrix());
    
    switch (VolumeMprVirtualSliceView::getViewTypeForVolumeSliceView()) {
        case VolumeMprVirtualSliceView::ViewType::VOLUME_VIEW_FIXED_CAMERA:
            break;
        case VolumeMprVirtualSliceView::ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            break;
        case VolumeMprVirtualSliceView::ViewType::ALL_VIEW_SLICES:
            break;
    }

    const bool drawCylindersFlag(true);
    const bool drawLabelsFlag(true);
    
    /*
     * Set the viewport
     */
    glViewport(viewport[0],
               viewport[1],
               viewport[2],
               viewport[3]);
    const double viewportWidth  = viewport[2];
    const double viewportHeight = viewport[3];
    
    /*
     * Determine bounds for orthographic projection
     */
    const double maxCoord = 100.0;
    const double minCoord = -maxCoord;
    double left   = 0.0;
    double right  = 0.0;
    double top    = 0.0;
    double bottom = 0.0;
    const double nearDepth = -1000.0;
    const double farDepth  =  1000.0;
    if (viewportHeight > viewportWidth) {
        left  = minCoord;
        right = maxCoord;
        const double aspectRatio = (viewportHeight
                                    / viewportWidth);
        top   = maxCoord * aspectRatio;
        bottom = minCoord * aspectRatio;
    }
    else {
        const double aspectRatio = (viewportWidth
                                    / viewportHeight);
        top   = maxCoord;
        bottom = minCoord;
        left  = minCoord * aspectRatio;
        right = maxCoord * aspectRatio;
    }
    /*
     * Set the orthographic projection
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(left, right,
            bottom, top,
            nearDepth, farDepth);
    
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    {
        /*
         * Set the viewing transformation, places 'eye' so that it looks
         * at the 'model' which is, in this case, the axes
         */
        double eyeX = 0.0;
        double eyeY = 0.0;
        double eyeZ = BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance;
        const double centerX = 0;
        const double centerY = 0;
        const double centerZ = 0;
        const double upX = 0;
        const double upY = 1;
        const double upZ = 0;
        gluLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ);
        
        double rotationMatrix[16];
        matrix.getMatrixForOpenGL(rotationMatrix);
        glMultMatrixd(rotationMatrix);

        
        /*
         * Disable depth buffer.  Otherwise, when volume slices are drawn
         * black regions of the slices may set depth buffer and the occlude
         * the axes from display.
         */
        GLboolean depthBufferEnabled = false;
        glGetBooleanv(GL_DEPTH_TEST,
                      &depthBufferEnabled);
        glDisable(GL_DEPTH_TEST);
        const float red[4] = {
            1.0, 0.0, 0.0, 1.0
        };
        const float green[4] = {
            0.0, 1.0, 0.0, 1.0
        };
        const float blue[4] = {
            0.0, 0.0, 1.0, 1.0
        };
        
        const double axisMaxCoord = maxCoord * 0.8;
        const double axisMinCoord = -axisMaxCoord;
        const double textMaxCoord = maxCoord * 0.9;
        const double textMinCoord = -textMaxCoord;

        const float axialPlaneMin[3] = { 0.0, 0.0, (float)axisMinCoord };
        const float axialPlaneMax[3] = { 0.0, 0.0, (float)axisMaxCoord };
        const double axialTextMin[3]  = { 0.0, 0.0, (float)textMinCoord };
        const double axialTextMax[3]  = { 0.0, 0.0, (float)textMaxCoord };
        
        const float coronalPlaneMin[3] = { (float)axisMinCoord, 0.0, 0.0 };
        const float coronalPlaneMax[3] = { (float)axisMaxCoord, 0.0, 0.0 };
        const double coronalTextMin[3]  = { (float)textMinCoord, 0.0, 0.0 };
        const double coronalTextMax[3]  = { (float)textMaxCoord, 0.0, 0.0 };
        
        const float paraPlaneMin[3] = { 0.0, (float)axisMinCoord, 0.0 };
        const float paraPlaneMax[3] = { 0.0, (float)axisMaxCoord, 0.0 };
        const double paraTextMin[3]  = { 0.0, (float)textMinCoord, 0.0 };
        const double paraTextMax[3]  = { 0.0, (float)textMaxCoord, 0.0 };
        
        /*
         * Set radius as percentage of viewport height
         */
        float axesCrosshairRadius = 1.0;
        if (viewportHeight > 0) {
            const float percentageRadius = 0.005f;
            axesCrosshairRadius = percentageRadius * viewportHeight;
        }
        
        
        const std::array<uint8_t, 4> backgroundRGBA = {
            m_fixedPipelineDrawing->m_backgroundColorByte[0],
            m_fixedPipelineDrawing->m_backgroundColorByte[1],
            m_fixedPipelineDrawing->m_backgroundColorByte[2],
            m_fixedPipelineDrawing->m_backgroundColorByte[3]
        };

        AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        annotationText.setFontPercentViewportSize(5.0f);
        annotationText.setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
        annotationText.setTextColor(CaretColorEnum::CUSTOM);
        annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
        annotationText.setCustomBackgroundColor(backgroundRGBA.data());
        
        if (drawCylindersFlag) {
            m_fixedPipelineDrawing->drawCylinder(blue,
                                                 axialPlaneMin,
                                                 axialPlaneMax,
                                                 axesCrosshairRadius * 0.5f);
            m_fixedPipelineDrawing->drawCylinder(green,
                                                 coronalPlaneMin,
                                                 coronalPlaneMax,
                                                 axesCrosshairRadius * 0.5f);
            m_fixedPipelineDrawing->drawCylinder(red,
                                                 paraPlaneMin,
                                                 paraPlaneMax,
                                                 axesCrosshairRadius * 0.5f);
        }

        /*
         * Draw axes labels after cyclinder (axes lines) so that
         * the text background will obscure the cyclinders.
         */
        if (drawLabelsFlag) {
            annotationText.setCustomTextColor(blue);
            annotationText.setText("I");
            m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMin,
                                                          annotationText);
            annotationText.setText("S");
            m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMax,
                                                          annotationText);

            annotationText.setCustomTextColor(green);
            annotationText.setText("L");
            m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMin,
                                                          annotationText);
            annotationText.setText("R");
            m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMax,
                                                          annotationText);

            annotationText.setCustomTextColor(red);
            annotationText.setText("P");
            m_fixedPipelineDrawing->drawTextAtModelCoords(paraTextMin,
                                                          annotationText);
            
            annotationText.setText("A");
            m_fixedPipelineDrawing->drawTextAtModelCoords(paraTextMax,
                                                          annotationText);
        }
    }
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Draw box showing the current rotation
 *
 * Rotation box designed copied from: https://castlemountain.dk/mulrecon/sprayMPR.html?folder=MRBrainT1&pixelspacing=0.625&ST=1&SS=1&slices=169&compression=1
 *
 * @param underlayVolume
 *   The underlay volume
 * @param viewportIn
 *   The viewport
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawAllViewRotationBox(const VolumeMappableInterface* underlayVolume,
                                                         const int32_t viewportIn[4])
{
    CaretAssert(underlayVolume);
    
    /*
     * Set the modeling transformation
     */
    const Vector3D selectedXYZ(m_browserTabContent->getVolumeSliceCoordinates());
    Matrix4x4 rotationMatrix(m_browserTabContent->getMprThreeRotationMatrix());
    rotationMatrix.invert();
    Matrix4x4 matrix;
    matrix.translate(-selectedXYZ);
    matrix.postmultiply(rotationMatrix);
    matrix.translate(selectedXYZ);
    
    /*
     * Set the viewport
     */
    glViewport(viewportIn[0],
               viewportIn[1],
               viewportIn[2],
               viewportIn[3]);
    const GraphicsViewport viewport(viewportIn[0],
                                    viewportIn[1],
                                    viewportIn[2],
                                    viewportIn[3]);
    const double viewportWidth  = viewport.getWidth();
    const double viewportHeight = viewport.getHeight();
    
    /*
     * Determine bounds for orthographic projection
     */
    const float marginPercent(1.15);
    BoundingBox boundingBox;
    underlayVolume->getVoxelSpaceBoundingBox(boundingBox);
    const double maxCoord = boundingBox.getMaximumDifferenceOfXYZ() * marginPercent;
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    Vector3D eyeOffsetXYZ(0.0, 0.0, maxCoord);
    
    const float nearToFarDistance(maxCoord * 1.5);
    const float nearHeight(maxCoord);
    const float farHeight(nearHeight * marginPercent);
    const float middleNearFarHeight((nearHeight + farHeight) / 2.0);
    /*
     * Right triangle
     *               ==
     *             =  =
     *           =    =
     *       c =      =  b
     *       =        =
     *     =          =
     *   =  theta     =
     *  ===============
     *        a
     */
    
    const float fov(80.0);
    
    const float theta(fov / 2.0);
    const float b(middleNearFarHeight);
    const float a(b / std::tan(MathFunctions::toRadians(theta)));
    
    const float middleNearFarDistance(a);
    const float nearDistance(middleNearFarDistance - (nearToFarDistance / 2.0));
    const float farDistance(nearDistance + nearToFarDistance);
    
    const double aspectRatio = (viewportWidth
                                / viewportHeight);
    
    gluPerspective(fov, aspectRatio, nearDistance, farDistance);
    
    eyeOffsetXYZ[2] = middleNearFarDistance;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    {
        /*
         * Set the viewing transformation, places 'eye' so that it looks
         * at the 'model' which is, in this case, the box from dorsal view
         */
        Vector3D centerXYZ;
        boundingBox.getCenter(centerXYZ);
        const Vector3D eyeXYZ(centerXYZ + eyeOffsetXYZ);
        const double upX = 0;
        const double upY = 1;
        const double upZ = 0;
        gluLookAt(eyeXYZ[0], eyeXYZ[1], eyeXYZ[2],
                  centerXYZ[0], centerXYZ[1], centerXYZ[2],
                  upX, upY, upZ);
        
        const float gray[4] { 0.7f, 0.7f, 0.7f, 1.0f };
        std::unique_ptr<GraphicsPrimitiveV3f> box(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_LINES, gray));
        const auto verticesEdgesFaces(VolumeVerticesEdgesFaces::newInstance(underlayVolume));
        if (verticesEdgesFaces->isValid()) {
            const auto& edges(verticesEdgesFaces->getEdges());
            for (const auto& e : edges) {
                box->addVertex(e.v1());
                box->addVertex(e.v2());
            }
        }
        
        /*
         * Draw box without roation
         */
        GraphicsEngineDataOpenGL::draw(box.get());
        
        /*
         * Draw box with rotation and slice spheres
         */
        {
            glPushMatrix();
            
            glPushAttrib(GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            
            m_fixedPipelineDrawing->enableLineAntiAliasing();
            
            double rotationMatrix[16];
            matrix.getMatrixForOpenGL(rotationMatrix);
            glMultMatrixd(rotationMatrix);
            
            GraphicsEngineDataOpenGL::draw(box.get());
            
            const float x(selectedXYZ[0]);
            const float y(selectedXYZ[1]);
            const float z(selectedXYZ[2]);
            
            const float minX(boundingBox.getMinX());
            const float minY(boundingBox.getMinY());
            const float maxX(boundingBox.getMaxX());
            const float maxY(boundingBox.getMaxY());
            const float minZ(boundingBox.getMinZ());
            const float maxZ(boundingBox.getMaxZ());
            
            const float sphereSize(maxCoord * 0.05);
            
            /*
             * Axial slice spheres
             */
            const float blue[4] { 0.0, 0.0, 1.0, 1.0 };
            std::unique_ptr<GraphicsPrimitiveV3f> axialSpheres(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::SPHERES, blue));
            axialSpheres->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS, sphereSize);
            axialSpheres->addVertex(minX, minY, z);
            axialSpheres->addVertex(maxX, minY, z);
            axialSpheres->addVertex(maxX, maxY, z);
            axialSpheres->addVertex(minX, maxY, z);
            GraphicsEngineDataOpenGL::draw(axialSpheres.get());
            
            /*
             * Coronal slice spheres
             */
            const float green[4] { 0.0, 1.0, 0.0, 1.0 };
            std::unique_ptr<GraphicsPrimitiveV3f> coronalSpheres(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::SPHERES, green));
            coronalSpheres->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS, sphereSize);
            coronalSpheres->addVertex(minX, y, minZ);
            coronalSpheres->addVertex(maxX, y, minZ);
            coronalSpheres->addVertex(maxX, y, maxZ);
            coronalSpheres->addVertex(minX, y, maxZ);
            GraphicsEngineDataOpenGL::draw(coronalSpheres.get());
            
            /*
             * Coronal slice spheres
             */
            const float red[4] { 1.0, 0.0, 0.0, 1.0 };
            std::unique_ptr<GraphicsPrimitiveV3f> paraSpheres(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::SPHERES, red));
            paraSpheres->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS, sphereSize);
            paraSpheres->addVertex(x, minY, minZ);
            paraSpheres->addVertex(x, maxY, minZ);
            paraSpheres->addVertex(x, minY, maxZ);
            paraSpheres->addVertex(x, maxY, maxZ);
            GraphicsEngineDataOpenGL::draw(paraSpheres.get());
            
            m_fixedPipelineDrawing->disableLineAntiAliasing();
            
            glPopAttrib();
            
            glPopMatrix();
        }
    }
    
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
/**
 * Draw the  slice view in Whole Brain (ALL)  mode
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param volumeDrawInfo
 *    Volumes being drawn
 * @param viewport
 *    The viewport
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawWholeBrainView(const BrainOpenGLViewportContent* viewportContent,
                                                   const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                   const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                   const GraphicsViewport& viewport)
{
    m_orthographicBounds[0] = m_fixedPipelineDrawing->orthographicLeft;
    m_orthographicBounds[1] = m_fixedPipelineDrawing->orthographicRight;
    m_orthographicBounds[2] = m_fixedPipelineDrawing->orthographicBottom;
    m_orthographicBounds[3] = m_fixedPipelineDrawing->orthographicTop;
    m_orthographicBounds[4] = m_fixedPipelineDrawing->orthographicNear;
    m_orthographicBounds[5] = m_fixedPipelineDrawing->orthographicFar;
    
    const Vector3D sliceCoordinates {
        m_browserTabContent->getVolumeSliceCoordinateParasagittal(),
        m_browserTabContent->getVolumeSliceCoordinateCoronal(),
        m_browserTabContent->getVolumeSliceCoordinateAxial()
    };

    const bool updateGraphicsObjectToWindowTransformFlag(false);
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            drawSliceIntensityProjection3D(VolumeSliceViewPlaneEnum::AXIAL,
                                           sliceCoordinates,
                                           viewport);
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            if (m_browserTabContent->isVolumeSliceAxialEnabled()) {
                glPushMatrix();
                drawVolumeSliceViewProjection(viewportContent,
                                              sliceProjectionType,
                                              sliceDrawingType,
                                              VolumeSliceViewPlaneEnum::AXIAL,
                                              sliceCoordinates,
                                              viewport,
                                              GridInfo(),
                                              updateGraphicsObjectToWindowTransformFlag);
                glPopMatrix();
            }
            
            if (m_browserTabContent->isVolumeSliceCoronalEnabled()) {
                glPushMatrix();
                drawVolumeSliceViewProjection(viewportContent,
                                              sliceProjectionType,
                                              sliceDrawingType,
                                              VolumeSliceViewPlaneEnum::CORONAL,
                                              sliceCoordinates,
                                              viewport,
                                              GridInfo(),
                                              updateGraphicsObjectToWindowTransformFlag);
                glPopMatrix();
            }
            
            if (m_browserTabContent->isVolumeSliceParasagittalEnabled()) {
                glPushMatrix();
                drawVolumeSliceViewProjection(viewportContent,
                                              sliceProjectionType,
                                              sliceDrawingType,
                                              VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                              sliceCoordinates,
                                              viewport,
                                              GridInfo(),
                                              updateGraphicsObjectToWindowTransformFlag);
                glPopMatrix();
            }
            break;
    }
}

/**
 * Draw single or montage volume view slices.
 * @param viewportContent
 *    Content of viewport
 * @param sliceProjectionType
 *   Type of slice projection
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawVolumeSliceViewType(const BrainOpenGLViewportContent* viewportContent,
                                                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                        const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                        const GraphicsViewport& viewport)
{
    glPushAttrib(GL_ENABLE_BIT);
    
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawVolumeSliceViewTypeMontage(viewportContent,
                                           sliceDrawingType,
                                           sliceProjectionType,
                                           sliceViewPlane,
                                           viewport);
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
        {
            const Vector3D sliceCoordinates {
                m_browserTabContent->getVolumeSliceCoordinateParasagittal(),
                m_browserTabContent->getVolumeSliceCoordinateCoronal(),
                m_browserTabContent->getVolumeSliceCoordinateAxial()
            };
            const bool updateGraphicsObjectToWindowTransformFlag(true);
            drawVolumeSliceViewProjection(viewportContent,
                                          sliceProjectionType,
                                          sliceDrawingType,
                                          sliceViewPlane,
                                          sliceCoordinates,
                                          viewport,
                                          GridInfo(),
                                          updateGraphicsObjectToWindowTransformFlag);
        }
            break;
    }
    
    glPopAttrib();
}

/**
 * Draw a slice for either projection mode (oblique, orthogonal)
 * @param viewportContent
 *    Content of viewport
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceDrawingType
 *    The slice drawing type (montage / single)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 * @param gridInfo
 *    Grid info if montage slice drawing
 * @param updateGraphicsObjectToWindowTransformFlag
 *    If true, update the graphics opbject to window transform
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawVolumeSliceViewProjection(const BrainOpenGLViewportContent* viewportContent,
                                                                const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                const Vector3D& sliceCoordinates,
                                                                const GraphicsViewport& viewport,
                                                                const GridInfo& gridInfo,
                                                                const bool updateGraphicsObjectToWindowTransformFlag)
{
    SelectionItemAnnotation* annotationID(m_brain->getSelectionManager()->getAnnotationIdentification());
    SelectionItemAnnotation* samplesID(m_brain->getSelectionManager()->getSamplesIdentification());
    SelectionItemVolumeMprCrosshair* crosshairID(m_brain->getSelectionManager()->getVolumeMprCrosshairIdentification());
    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    SelectionItemVoxelEditing* voxelEditingID = m_brain->getSelectionManager()->getVoxelEditingIdentification();

    /*
     * Check for a 'selection' type mode
     */
    bool drawVolumeSlicesFlag = true;
    m_identificationModeFlag = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (annotationID->isEnabledForSelection()
                || samplesID->isEnabledForSelection()
                || voxelID->isEnabledForSelection()
                || crosshairID->isEnabledForSelection()
                || voxelEditingID->isEnabledForSelection()) {
                m_identificationModeFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                /*
                 * Don't return.  Allow other items (such as annotations) to be drawn.
                 */
                drawVolumeSlicesFlag = false;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }

    
    glMatrixMode(GL_MODELVIEW);
    
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::ALL_3D:
            break;
        case BrainModelMode::VOLUME_2D:
        {
            glLoadIdentity();
            glViewport(viewport.getX(),
                       viewport.getY(),
                       viewport.getWidth(),
                       viewport.getHeight());
            
            bool drawViewportBoxFlag(false);
            if (drawViewportBoxFlag) {
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                glOrtho(viewport.getLeftF(), viewport.getRightF(),
                        viewport.getBottomF(), viewport.getTopF(),
                        -100.0, 100.0);
                glColor3f(0.0, 1.0, 0.0);
                glLineWidth(2.0);
                glBegin(GL_LINE_LOOP);
                glVertex2f(viewport.getLeftF() + 1, viewport.getBottomF() + 1);
                glVertex2f(viewport.getRightF() - 1, viewport.getBottomF() + 1);
                glVertex2f(viewport.getRightF() - 1, viewport.getTopF() - 1);
                glVertex2f(viewport.getLeftF() + 1, viewport.getTopF() - 1);
                glEnd();
                
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
            }
        }
            break;
    }
        
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::ALL_3D:
            break;
        case BrainModelMode::VOLUME_2D:
            /*
             * Set the orthographic projection to fit the slice axis
             */
            setOrthographicProjection(viewport);
            break;
    }

    CaretAssertVectorIndex(m_volumeDrawInfo, 0);
    const VolumeMappableInterface* underlayVolume(m_volumeDrawInfo[0].volumeFile);
    CaretAssert(underlayVolume);
    VolumeMprVirtualSliceView mprSliceView(createSliceInfo(underlayVolume,
                                                           sliceViewPlane,
                                                           sliceCoordinates));

    if ( ! mprSliceView.getVirtualPlane().isValidPlane()) {
        return;
    }

    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::ALL_3D:
            break;
        case BrainModelMode::VOLUME_2D:
            /*
             * Set the viewing transformation (camera position)
             */
            setViewingTransformation(sliceViewPlane,
                                     mprSliceView);
            
            /*
             * Only set for two-d view, 3D view (VIEW -> ALL) is set when surfaces are drawn
             */
            m_fixedPipelineDrawing->setupScaleBarDrawingInformation(m_browserTabContent,
                                                                    m_orthographicBounds[0],
                                                                    m_orthographicBounds[1]);
            break;
    }

    m_fixedPipelineDrawing->applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_VOLUME,
                                                StructureEnum::ALL);

    if (drawVolumeSlicesFlag) {
        DrawingViewportContentVolumeSlice vpSlice(gridInfo.m_numberOfRows,
                                                  gridInfo.m_numberOfColumns,
                                                  gridInfo.m_rowIndex,
                                                  gridInfo.m_columnIndex,
                                                  sliceViewPlane,
                                                  mprSliceView.getVirtualPlane(),
                                                  sliceCoordinates);
        EventDrawingViewportContentAdd addViewportEvent;
        addViewportEvent.addVolumeSlice(this->m_fixedPipelineDrawing->m_windowIndex,
                                        m_tabIndex,
                                        viewport,
                                        vpSlice);
        EventManager::get()->sendEvent(addViewportEvent.getPointer());
        
        glPushMatrix();
                
        /*
         * Disable culling so that both sides of the triangles/quads are drawn.
         */
        glDisable(GL_CULL_FACE);
        
        bool intensityModeFlag(false);
        switch (m_brainModelMode) {
            case BrainModelMode::INVALID:
                break;
            case BrainModelMode::ALL_3D:
                switch (m_mprViewMode) {
                    case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
                    case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
                    case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
                        CaretAssertMessage(0, "This function should not be called for ALL view Intensity Modes");
                        break;
                    case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
                        break;
                }
                break;
            case BrainModelMode::VOLUME_2D:
                switch (m_mprViewMode) {
                    case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
                        intensityModeFlag = true;
                        break;
                    case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
                        intensityModeFlag = true;
                        break;
                    case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
                        intensityModeFlag = true;
                        break;
                    case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
                        break;
                }
                break;
        }

        if (m_identificationModeFlag) {
            intensityModeFlag = false;
        }
        
        bool drawIdentificationSymbolsFlag(false);
        if (intensityModeFlag) {
            drawSliceIntensityProjection2D(mprSliceView,
                                           sliceViewPlane,
                                           sliceCoordinates,
                                           viewport);
        }
        else {
            const bool enableBlendingFlag(true);
            const bool drawAttributesFlag(true);
            const bool drawIntensitySliceBackgroundFlag(false);
            drawSliceWithPrimitive(mprSliceView,
                                   sliceViewPlane,
                                   viewport,
                                   enableBlendingFlag,
                                   drawAttributesFlag,
                                   drawIntensitySliceBackgroundFlag);
            drawIdentificationSymbolsFlag = true;
        }

        if (m_brainModelMode == BrainModelMode::VOLUME_2D) {
            if (updateGraphicsObjectToWindowTransformFlag) {
                std::array<float, 4> orthoLRBT {
                    static_cast<float>(viewport.getLeft()),
                    static_cast<float>(viewport.getRight()),
                    static_cast<float>(viewport.getBottom()),
                    static_cast<float>(viewport.getTop())
                };
                GraphicsObjectToWindowTransform* transform = new GraphicsObjectToWindowTransform();
                m_fixedPipelineDrawing->loadObjectToWindowTransform(transform, orthoLRBT, 0.0, true);
                viewportContent->setVolumeMprGraphicsObjectToWindowTransform(sliceViewPlane, transform);
            }
        }

        float sliceThickness = 1.0;
        if ( ! m_volumeDrawInfo.empty()) {
            if (m_volumeDrawInfo[0].volumeFile != NULL) {
                float spaceX = 0.0, spaceY = 0.0, spaceZ = 0.0;
                m_volumeDrawInfo[0].volumeFile->getVoxelSpacing(spaceX, spaceY, spaceZ);
                
                switch (sliceViewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        sliceThickness = spaceZ;
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        sliceThickness = spaceY;
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        sliceThickness = spaceX;
                        break;
                }
                
                if (m_volumeDrawInfo[0].volumeFile->isThin()) {
                    /*
                     * Thin volumes can cause problems with identification symbol drawing
                     * which uses half of the slice thickness.  In this case the
                     * symbol may be just outside of the half slice thickness.  So
                     * increase the slice thickness so that identification symbols
                     * are drawn.
                     */
                    sliceThickness *= 2.0;
                }
            }
            
            CaretAssertVectorIndex(m_volumeDrawInfo, 0);
            
            if (drawIdentificationSymbolsFlag) {
                BrainOpenGLVolumeSliceDrawing::drawIdentificationSymbols(m_fixedPipelineDrawing,
                                                                         m_browserTabContent,
                                                                         m_volumeDrawInfo[0].volumeFile,
                                                                         m_volumeDrawInfo[0].mapIndex,
                                                                         mprSliceView.getVirtualPlane(),
                                                                         sliceThickness);
            }
        }
        
        Plane layersDrawingPlane(mprSliceView.getVirtualPlane());
        
        if (layersDrawingPlane.isValidPlane()) {
            drawLayers(mprSliceView,
                       underlayVolume,
                       sliceProjectionType,
                       sliceViewPlane,
                       layersDrawingPlane,
                       sliceCoordinates,
                       sliceThickness);
        }

        const bool annotationModeFlag = (m_fixedPipelineDrawing->m_windowUserInputMode == UserInputModeEnum::Enum::ANNOTATIONS);
        const bool tileTabsEditModeFlag = (m_fixedPipelineDrawing->m_windowUserInputMode == UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING);
        std::set<AString> emptyMediaFileNames;
        BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                                 m_fixedPipelineDrawing->mode,
                                                                 BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                                 m_fixedPipelineDrawing->m_windowIndex,
                                                                 m_fixedPipelineDrawing->windowTabIndex,
                                                                 SpacerTabIndex(),
                                                                 BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO,
                                                                 emptyMediaFileNames,
                                                                 annotationModeFlag,
                                                                 tileTabsEditModeFlag);
        m_fixedPipelineDrawing->m_annotationDrawing->drawModelSpaceAnnotationsOnVolumeSlice(&inputs,
                                                                                            layersDrawingPlane,
                                                                                            sliceThickness);
        
        {
            const bool samplesModeFlag(m_fixedPipelineDrawing->m_windowUserInputMode == UserInputModeEnum::Enum::SAMPLES_EDITING);
            BrainOpenGLAnnotationDrawingFixedPipeline::Inputs inputs(this->m_brain,
                                                                     m_fixedPipelineDrawing->mode,
                                                                     BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                                                                     m_fixedPipelineDrawing->m_windowIndex,
                                                                     m_fixedPipelineDrawing->windowTabIndex,
                                                                     SpacerTabIndex(),
                                                                     BrainOpenGLAnnotationDrawingFixedPipeline::Inputs::WINDOW_DRAWING_NO,
                                                                     emptyMediaFileNames,
                                                                     samplesModeFlag,
                                                                     tileTabsEditModeFlag);
            m_fixedPipelineDrawing->m_annotationDrawing->drawModelSpaceSamplesOnVolumeSlice(&inputs,
                                                                                            layersDrawingPlane,
                                                                                            sliceThickness);
        }
                
        switch (m_brainModelMode) {
            case BrainModelMode::INVALID:
                CaretAssert(0);
                break;
            case BrainModelMode::ALL_3D:
                break;
            case BrainModelMode::VOLUME_2D:
                drawCrosshairs(underlayVolume,
                               mprSliceView,
                               sliceViewPlane,
                               sliceCoordinates,
                               viewport);
                break;
        }

        bool drawSelectionBoxFlag(false);
        switch (sliceDrawingType) {
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
                break;
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
                drawSelectionBoxFlag = true;
                break;
        }
        if (drawSelectionBoxFlag) {
            /*
             * Draw selection box in viewport coordinates since
             * volume may be rotated
             */
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            GraphicsViewport viewport(GraphicsViewport::newInstanceCurrentViewport());
            glOrtho(viewport.getLeftF(), viewport.getRightF(),
                    viewport.getBottomF(), viewport.getTopF(),
                    -100.0, 100.0);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            BrainOpenGLFixedPipeline::drawGraphicsRegionSelectionBox(m_browserTabContent->getRegionSelectionBox(),
                                                                     GraphicsRegionSelectionBox::DrawMode::VIEWPORT,
                                                                     m_fixedPipelineDrawing->m_foregroundColorFloat);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        }
        
        glPopMatrix();
    }

    m_fixedPipelineDrawing->disableClippingPlanes();
    
    /*
     * Draw yellow cross at center of viewport
     */
    if (SessionManager::get()->getCaretPreferences()->isCrossAtViewportCenterEnabled()) {
        GraphicsShape::drawYellowCrossAtViewportCenter();
    }
}

/**
 * Create information about the slice being drawn
 * @param underlayVolume
 *    The underlay volume
 * @param sliceViewPlane
 *    Plane being viewed
 * @param sliceCoordinates
 *    Coordinates of selected slices
 */
VolumeMprVirtualSliceView
BrainOpenGLVolumeMprThreeDrawing::createSliceInfo(const VolumeMappableInterface* underlayVolume,
                                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                  const Vector3D& sliceCoordinates)
{
    CaretAssert(underlayVolume);
    BoundingBox boundingBox;
    underlayVolume->getVoxelSpaceBoundingBox(boundingBox);
    
    Vector3D volumeCenterXYZ;
    boundingBox.getCenter(volumeCenterXYZ);
    
    const VolumeMprVirtualSliceView::ViewType viewType(m_underlayIsSingleSliceVolumeFlag
                                                       ? VolumeMprVirtualSliceView::getViewTypeForVolumeSingleSliceView()
                                                       : VolumeMprVirtualSliceView::getViewTypeForVolumeSliceView());
    
    /*
     * Compute the normal vector for each slice
     */
    const VolumeMprVirtualSliceView axialMprSliceView(underlayVolume,
                                                      viewType,
                                                      volumeCenterXYZ,
                                                      sliceCoordinates,
                                                      boundingBox.getMaximumDifferenceOfXYZ(),
                                                      VolumeSliceViewPlaneEnum::AXIAL,
                                                      m_orientationMode,
                                                      m_browserTabContent->getMprThreeRotationMatrixForSlicePlane(VolumeSliceViewPlaneEnum::AXIAL),
                                                      m_browserTabContent->getMprThreeSingleSliceVolumeRotationAngle());
    m_axialSliceNormalVector = axialMprSliceView.getVirtualPlane().getNormalVector();
    
    const VolumeMprVirtualSliceView coronalMprSliceView(underlayVolume,
                                                        viewType,
                                                        volumeCenterXYZ,
                                                        sliceCoordinates,
                                                        boundingBox.getMaximumDifferenceOfXYZ(),
                                                        VolumeSliceViewPlaneEnum::CORONAL,
                                                        m_orientationMode,
                                                        m_browserTabContent->getMprThreeRotationMatrixForSlicePlane(VolumeSliceViewPlaneEnum::CORONAL),
                                                        m_browserTabContent->getMprThreeSingleSliceVolumeRotationAngle());
    m_coronalSliceNormalVector = coronalMprSliceView.getVirtualPlane().getNormalVector();
    
    const VolumeMprVirtualSliceView paraMprSliceView(underlayVolume,
                                                     viewType,
                                                     volumeCenterXYZ,
                                                     sliceCoordinates,
                                                     boundingBox.getMaximumDifferenceOfXYZ(),
                                                     VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                                     m_orientationMode,
                                                     m_browserTabContent->getMprThreeRotationMatrixForSlicePlane(VolumeSliceViewPlaneEnum::PARASAGITTAL),
                                                     m_browserTabContent->getMprThreeSingleSliceVolumeRotationAngle());
    m_parasagittalSliceNormalVector = paraMprSliceView.getVirtualPlane().getNormalVector();

    const Matrix4x4 sliceRotationMatrix(m_browserTabContent->getMprThreeRotationMatrixForSlicePlane(sliceViewPlane));
    
    /*
     * Info for slice that is being drawn
     */
    VolumeMprVirtualSliceView mprSliceView(underlayVolume,
                                           viewType,
                                           volumeCenterXYZ,
                                           sliceCoordinates,
                                           boundingBox.getMaximumDifferenceOfXYZ(),
                                           sliceViewPlane,
                                           m_orientationMode,
                                           sliceRotationMatrix,
                                           m_browserTabContent->getMprThreeSingleSliceVolumeRotationAngle());

    Vector3D rotationVector;
    mprSliceView.getVirtualPlane().getNormalVector(rotationVector);
    m_browserTabContent->setMprThreeRotationVectorForSlicePlane(sliceViewPlane,
                                                                rotationVector);
    
    /*
     * Normal vector for slice being drawn
     */
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            m_axialSliceNormalVector = mprSliceView.getVirtualPlane().getNormalVector();
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            m_coronalSliceNormalVector = mprSliceView.getVirtualPlane().getNormalVector();
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            m_parasagittalSliceNormalVector = mprSliceView.getVirtualPlane().getNormalVector();
            break;
    }
    
    /*
     * Verify that the slices remain orthogonal
     */
    if ( ! underlayVolume->isSingleSlice()) {
        AString orthoMessage;
        if ( ! Plane::arePlanesOrthogonal(m_parasagittalSliceNormalVector,
                                          m_coronalSliceNormalVector,
                                          m_axialSliceNormalVector,
                                          &orthoMessage)) {
            AString rotateMsg("MPR Separate Rotation"
                              + AString(" failed orthogonal planes test (\"Toolbar -> Reset\" will fix this)"));
            rotateMsg.appendWithNewLine(orthoMessage);
            rotateMsg.appendWithNewLine("   Parasagittal Normal: " + m_parasagittalSliceNormalVector.toString());
            rotateMsg.appendWithNewLine("   Coronal Normal: " + m_coronalSliceNormalVector.toString());
            rotateMsg.appendWithNewLine("   Axial Normal: " + m_axialSliceNormalVector.toString());
            CaretLogSevere(rotateMsg);
        }
    }
    
    return mprSliceView;
}

/**
 * Add a segment to the crosshairs
 * @param primitiveSliceSelectionCrosshair
 *    Primitive for drawing crosshair sections for slice selection
 * @param primitiveRotateSliceCrosshair
 *    Primitive for drawing crosshair sections for slice rotation
 * @param primitiveRotateTransformCrosshair
 *    Primitive for drawing crosshair sections for transform rotation
 * @param startXYZ
 *    Starting coordinate of crosshair
 * @param endXYZ
 *    Ending coordinate of crosshair
 * @param rgba
 *    Coloring for the lines
 * @param sliceSelectionIndices
 *    Tracks indices of slice selection indices
 * @param rotateSliceSelectionIndices
 *    Tracks indices of rotate slice indices
 * @param rotateTransformSelectionIndices
 *    Tracks indices of  rotate transform indices
 * @param sliceSelectionAxisID
 *    Identification of slice axis selection
 * @param rotationTransformAxisID
 *    Identification of rotation axis selection
 */
void
BrainOpenGLVolumeMprThreeDrawing::addCrosshairSection(GraphicsPrimitiveV3fC4ub* primitiveSliceSelectionCrosshair,
                                                      GraphicsPrimitiveV3fC4ub* primitiveRotateSliceCrosshair,
                                                      GraphicsPrimitiveV3fC4ub* primitiveRotateTransformCrosshair,
                                                      const Vector3D& startXYZ,
                                                      const Vector3D& endXYZ,
                                                      const std::array<uint8_t, 4>& rgba,
                                                      std::vector<SelectionItemVolumeMprCrosshair::Axis>& sliceSelectionIndices,
                                                      std::vector<SelectionItemVolumeMprCrosshair::Axis>& rotateSliceSelectionIndices,
                                                      std::vector<SelectionItemVolumeMprCrosshair::Axis>& rotateTransformSelectionIndices,
                                                      const SelectionItemVolumeMprCrosshair::Axis sliceSelectionAxisID,
                                                      const SelectionItemVolumeMprCrosshair::Axis rotationSliceAxisID,
                                                      const SelectionItemVolumeMprCrosshair::Axis rotationTransformAxisID)
{
    bool montageFlag(false);
    switch (m_browserTabContent->getVolumeSliceDrawingType()) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            /*montageFlag = true;*/
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
            break;
    }
    
    if (montageFlag) {
        /*
         * No rotation in montage
         */
        const Vector3D sliceStartXYZ(startXYZ);
        const Vector3D sliceEndXYZ(endXYZ);
        sliceSelectionIndices.push_back(sliceSelectionAxisID);
        sliceSelectionIndices.push_back(sliceSelectionAxisID);
        primitiveSliceSelectionCrosshair->addVertex(sliceStartXYZ, rgba.data());
        primitiveSliceSelectionCrosshair->addVertex(sliceEndXYZ, rgba.data());
    }
    else if (m_underlayIsSingleSliceVolumeFlag) {
        /*
         * Note: There are TWO sections:
         * (1) First section is dragged to select slice indices for
         * other slice plane views.
         * (2) Second section is used to rotate only the current axis.
         */
        const Vector3D totalLength(endXYZ - startXYZ);
        const Vector3D sliceStartXYZ(startXYZ);
        const Vector3D sliceEndXYZ(startXYZ + (totalLength * 0.40));
        const Vector3D rotateSliceStartXYZ(startXYZ + (totalLength * 0.45));
        const Vector3D rotateSliceEndXYZ(endXYZ);
        
        sliceSelectionIndices.push_back(sliceSelectionAxisID);
        sliceSelectionIndices.push_back(sliceSelectionAxisID);
        primitiveSliceSelectionCrosshair->addVertex(sliceStartXYZ, rgba.data());
        primitiveSliceSelectionCrosshair->addVertex(sliceEndXYZ, rgba.data());
        
        rotateSliceSelectionIndices.push_back(rotationSliceAxisID);
        rotateSliceSelectionIndices.push_back(rotationSliceAxisID);
        primitiveRotateSliceCrosshair->addVertex(rotateSliceStartXYZ, rgba.data());
        primitiveRotateSliceCrosshair->addVertex(rotateSliceEndXYZ, rgba.data());
    }
    else {
        /*
         * Note: There are three sections:
         * (1) First section is dragged to select slice indices for
         * other slice plane views.
         * (2) Second section is used to rotate only the current axis.
         * (3) Third section rotates the transform.
         */
        const Vector3D totalLength(endXYZ - startXYZ);
        const Vector3D sliceStartXYZ(startXYZ);
        const Vector3D sliceEndXYZ(startXYZ + (totalLength * 0.40));
        const Vector3D rotateSliceStartXYZ(startXYZ + (totalLength * 0.45));
        const Vector3D rotateSliceEndXYZ(startXYZ + (totalLength * 0.70));
        const Vector3D rotateTransformStartXYZ(startXYZ + (totalLength * 0.75));
        const Vector3D rotateTransformEndXYZ(endXYZ);
        
        sliceSelectionIndices.push_back(sliceSelectionAxisID);
        sliceSelectionIndices.push_back(sliceSelectionAxisID);
        primitiveSliceSelectionCrosshair->addVertex(sliceStartXYZ, rgba.data());
        primitiveSliceSelectionCrosshair->addVertex(sliceEndXYZ, rgba.data());
        
        rotateSliceSelectionIndices.push_back(rotationSliceAxisID);
        rotateSliceSelectionIndices.push_back(rotationSliceAxisID);
        primitiveRotateSliceCrosshair->addVertex(rotateSliceStartXYZ, rgba.data());
        primitiveRotateSliceCrosshair->addVertex(rotateSliceEndXYZ, rgba.data());
        
        rotateTransformSelectionIndices.push_back(rotationTransformAxisID);
        rotateTransformSelectionIndices.push_back(rotationTransformAxisID);
        primitiveRotateTransformCrosshair->addVertex(rotateTransformStartXYZ, rgba.data());
        primitiveRotateTransformCrosshair->addVertex(rotateTransformEndXYZ, rgba.data());
    }
}


/**
 * Draw the panning crosshairs for slice
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Intersection of crosshairs
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawPanningCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                        const Vector3D& sliceCoordinates,
                                                        const GraphicsViewport& viewport)
{
    if ( ! m_browserTabContent->isVolumeAxesCrosshairsDisplayed()) {
        return;
    }
    
    SelectionItemVolumeMprCrosshair* crosshairID(m_brain->getSelectionManager()->getVolumeMprCrosshairIdentification());
    if (m_identificationModeFlag) {
        if ( ! crosshairID->isEnabledForSelection()) {
            return;
        }
        if ( ! m_axialCoronalParaSliceViewFlag) {
            return;
        }
    }
    
    const float pixelSize(std::min(viewport.getWidthF(),
                                   viewport.getHeightF()));
    const float millimeterSize(GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(pixelSize));
    const float crosshairLength(millimeterSize / 2.0);
    if (crosshairLength <= 0.0) {
        return;
    }
    
    std::array<uint8_t, 4> crosshairOneTwoRGBA;
    std::array<uint8_t, 4> crosshairThreeFourRGBA;
    Vector3D crosshairOneTwoVector;
    Vector3D crosshairThreeFourVector;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            crosshairOneTwoVector    = m_parasagittalSliceNormalVector;
            crosshairOneTwoRGBA      = getAxisColor(VolumeSliceViewPlaneEnum::CORONAL);
            crosshairThreeFourVector = m_coronalSliceNormalVector;
            crosshairThreeFourRGBA   = getAxisColor(VolumeSliceViewPlaneEnum::PARASAGITTAL);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            crosshairOneTwoVector    = m_parasagittalSliceNormalVector;
            crosshairOneTwoRGBA      = getAxisColor(VolumeSliceViewPlaneEnum::AXIAL);
            crosshairThreeFourVector = m_axialSliceNormalVector;
            crosshairThreeFourRGBA   = getAxisColor(VolumeSliceViewPlaneEnum::PARASAGITTAL);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            crosshairOneTwoVector    = m_coronalSliceNormalVector;
            crosshairOneTwoRGBA      = getAxisColor(VolumeSliceViewPlaneEnum::AXIAL);
            crosshairThreeFourVector = m_axialSliceNormalVector;
            crosshairThreeFourRGBA   = getAxisColor(VolumeSliceViewPlaneEnum::CORONAL);
            break;
    }
    float gapPercentage = SessionManager::get()->getCaretPreferences()->getVolumeCrosshairGap();
    gapPercentage /= 100.0;
    if (gapPercentage > 0.5) {
        gapPercentage = 0.5;
    }
    
    Vector3D crosshairOneStartXYZ(sliceCoordinates
                                  + ((gapPercentage * crosshairLength) * crosshairOneTwoVector));
    Vector3D crosshairOneEndXYZ(sliceCoordinates
                                + (crosshairLength * crosshairOneTwoVector));
    Vector3D crosshairTwoStartXYZ(sliceCoordinates
                                  - ((gapPercentage * crosshairLength) * crosshairOneTwoVector));
    Vector3D crosshairTwoEndXYZ(sliceCoordinates
                                - (crosshairLength * crosshairOneTwoVector));
    Vector3D crosshairThreeStartXYZ(sliceCoordinates
                                    + ((gapPercentage * crosshairLength) * crosshairThreeFourVector));
    Vector3D crosshairThreeEndXYZ(sliceCoordinates
                                  + (crosshairLength * crosshairThreeFourVector));
    Vector3D crosshairFourStartXYZ(sliceCoordinates
                                   - ((gapPercentage * crosshairLength) * crosshairThreeFourVector));
    Vector3D crosshairFourEndXYZ(sliceCoordinates
                                 - (crosshairLength * crosshairThreeFourVector));


    if (m_debugFlag) {
        std::cout << VolumeSliceViewPlaneEnum::toName(sliceViewPlane) << std::endl;
        std::cout << "   P: " << m_parasagittalSliceNormalVector.toString() << std::endl;
        std::cout << "   C: " << m_coronalSliceNormalVector.toString() << std::endl;
        std::cout << "   A: " << m_axialSliceNormalVector.toString() << std::endl;
    }

    const float percentViewportHeight(0.95);

    std::unique_ptr<GraphicsPrimitiveV3fC4ub> sliceSelectionPrimitive(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
    const float sliceLineWidth(m_identificationModeFlag
                               ? (percentViewportHeight * 5.0)
                               : percentViewportHeight);
    sliceSelectionPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                          sliceLineWidth);
    
    const float rotateThicker(2.0);
    const float rotateTransformLineWidth(m_identificationModeFlag
                                         ? (percentViewportHeight * 5.0)
                                         : percentViewportHeight * rotateThicker);
    std::unique_ptr<GraphicsPrimitiveV3fC4ub> rotateSlicePrimitive(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
    rotateSlicePrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                           rotateTransformLineWidth);

    std::unique_ptr<GraphicsPrimitiveV3fC4ub> rotateTransformPrimitive(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
    rotateTransformPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                           rotateTransformLineWidth * 1.5);
    
    std::vector<SelectionItemVolumeMprCrosshair::Axis> sliceSelectionIndices;
    std::vector<SelectionItemVolumeMprCrosshair::Axis> rotateSliceSelectionIndices;
    std::vector<SelectionItemVolumeMprCrosshair::Axis> rotateTransformSelectionIndices;

    addCrosshairSection(sliceSelectionPrimitive.get(),
                        rotateSlicePrimitive.get(),
                        rotateTransformPrimitive.get(),
                        crosshairOneStartXYZ,
                        crosshairOneEndXYZ,
                        crosshairOneTwoRGBA,
                        sliceSelectionIndices,
                        rotateSliceSelectionIndices,
                        rotateTransformSelectionIndices,
                        SelectionItemVolumeMprCrosshair::Axis::SELECT_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_TRANSFORM);
    
    addCrosshairSection(sliceSelectionPrimitive.get(),
                        rotateSlicePrimitive.get(),
                        rotateTransformPrimitive.get(),
                        crosshairTwoStartXYZ,
                        crosshairTwoEndXYZ,
                        crosshairOneTwoRGBA,
                        sliceSelectionIndices,
                        rotateSliceSelectionIndices,
                        rotateTransformSelectionIndices,
                        SelectionItemVolumeMprCrosshair::Axis::SELECT_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_TRANSFORM);
    
    addCrosshairSection(sliceSelectionPrimitive.get(),
                        rotateSlicePrimitive.get(),
                        rotateTransformPrimitive.get(),
                        crosshairThreeStartXYZ,
                        crosshairThreeEndXYZ,
                        crosshairThreeFourRGBA,
                        sliceSelectionIndices,
                        rotateSliceSelectionIndices,
                        rotateTransformSelectionIndices,
                        SelectionItemVolumeMprCrosshair::Axis::SELECT_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_TRANSFORM);
    
    addCrosshairSection(sliceSelectionPrimitive.get(),
                        rotateSlicePrimitive.get(),
                        rotateTransformPrimitive.get(),
                        crosshairFourStartXYZ,
                        crosshairFourEndXYZ,
                        crosshairThreeFourRGBA,
                        sliceSelectionIndices,
                        rotateSliceSelectionIndices,
                        rotateTransformSelectionIndices,
                        SelectionItemVolumeMprCrosshair::Axis::SELECT_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_SLICE,
                        SelectionItemVolumeMprCrosshair::Axis::ROTATE_TRANSFORM);

    if (m_identificationModeFlag) {
        float mouseX(0);
        float mouseY(0);
        getMouseViewportXY(viewport,
                           this->m_fixedPipelineDrawing->mouseX,
                           this->m_fixedPipelineDrawing->mouseY,
                           mouseX,
                           mouseY);
        int32_t primitiveIndex(-1);
        float primitiveDepth(0.0);
        GraphicsEngineDataOpenGL::drawWithSelection(sliceSelectionPrimitive.get(),
                                                    this->m_fixedPipelineDrawing->mouseX,
                                                    this->m_fixedPipelineDrawing->mouseY,
                                                    primitiveIndex,
                                                    primitiveDepth);
        if ((primitiveIndex >= 0)
            && (primitiveIndex < static_cast<int32_t>(sliceSelectionIndices.size()))) {
            crosshairID->setIdentification(m_brain,
                                           sliceSelectionIndices[primitiveIndex],
                                           primitiveDepth);
        }
        
        primitiveIndex = -1;
        primitiveDepth = 0.0;
        GraphicsEngineDataOpenGL::drawWithSelection(rotateSlicePrimitive.get(),
                                                    this->m_fixedPipelineDrawing->mouseX,
                                                    this->m_fixedPipelineDrawing->mouseY,
                                                    primitiveIndex,
                                                    primitiveDepth);
        if ((primitiveIndex >= 0)
            && (primitiveIndex < static_cast<int32_t>(rotateSliceSelectionIndices.size()))) {
            crosshairID->setIdentification(m_brain,
                                           rotateSliceSelectionIndices[primitiveIndex],
                                           primitiveDepth);
        }
        
        if ( ! m_underlayIsSingleSliceVolumeFlag) {
            primitiveIndex = -1;
            primitiveDepth = 0.0;
            GraphicsEngineDataOpenGL::drawWithSelection(rotateTransformPrimitive.get(),
                                                        this->m_fixedPipelineDrawing->mouseX,
                                                        this->m_fixedPipelineDrawing->mouseY,
                                                        primitiveIndex,
                                                        primitiveDepth);
            if ((primitiveIndex >= 0)
                && (primitiveIndex < static_cast<int32_t>(rotateTransformSelectionIndices.size()))) {
                crosshairID->setIdentification(m_brain,
                                               rotateTransformSelectionIndices[primitiveIndex],
                                               primitiveDepth);
            }
        }
    }
    else {
        glPushAttrib(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        
        m_fixedPipelineDrawing->enableLineAntiAliasing();
        
        GraphicsEngineDataOpenGL::draw(sliceSelectionPrimitive.get());
        GraphicsEngineDataOpenGL::draw(rotateSlicePrimitive.get());
        if ( ! m_underlayIsSingleSliceVolumeFlag) {
            GraphicsEngineDataOpenGL::draw(rotateTransformPrimitive.get());
        }

        m_fixedPipelineDrawing->disableLineAntiAliasing();
        
        glPopAttrib();
    }
}

/**
 * @return The color for drawing an axis representing the given slice plane
 * @param sliceViewPlane
 *    The plane for slice drawing.
 */
std::array<uint8_t, 4>
BrainOpenGLVolumeMprThreeDrawing::getAxisColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const
{
    std::array<uint8_t, 4> rgbaOut { 0, 0, 0, 255 };
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            rgbaOut[2] = 255;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            rgbaOut[1] = 255;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            rgbaOut[0] = 255;
            break;
    }
    return rgbaOut;
}

/**
 * Draw crosshairs for slice
 * @param underlayVolume
 *    The underlay volume
 * @param mprSliceView
 *    The virtual slice info
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawCrosshairs(const VolumeMappableInterface* /*underlayVolume*/,
                                                 const VolumeMprVirtualSliceView& mprSliceView,
                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const Vector3D& sliceCoordinates,
                                                 const GraphicsViewport& viewport)
{
    /*
     * Transform selected coordinates to viewport X/Y
     */
    EventOpenGLObjectToWindowTransform transformEvent(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
    EventManager::get()->sendEvent(transformEvent.getPointer());
        
    Vector3D crossHairXYZ;
    transformEvent.transformPoint(sliceCoordinates,
                                  crossHairXYZ);

    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            break;
    }
    
    const float vpMinX(viewport.getLeftF());
    const float vpMaxX(viewport.getRightF());
    const float vpMinY(viewport.getBottomF());
    const float vpMaxY(viewport.getTopF());
    
    drawPanningCrosshairs(sliceViewPlane,
                          sliceCoordinates,
                          viewport);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(vpMinX, vpMaxX,
            vpMinY, vpMaxY,
            -100.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    if ( ! m_underlayIsSingleSliceVolumeFlag) {
        drawAxisLabels(mprSliceView,
                       sliceViewPlane,
                       viewport);
    }
    
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

/**
 * Draw the axis labels for slice
 *
 * @param mprSliceView
 *  The virtual slice info
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawAxisLabels(const VolumeMprVirtualSliceView& mprSliceView,
                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const GraphicsViewport& viewport)
{
    if ( ! m_browserTabContent->isVolumeAxesCrosshairLabelsDisplayed()) {
        return;
    }

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
    
    std::array<uint8_t, 4> horizontalAxisRGBA = {
        0, 0, 255, 255
    };
    std::array<uint8_t, 4> verticalAxisRGBA(horizontalAxisRGBA);
    
    /*
     * Offset text labels be a percentage of viewort width/height
     */
    const int textOffsetX = viewport.getWidthF() * 0.01f;
    const int textOffsetY = viewport.getHeightF() * 0.01f;
    const int textLeftWindowXY[2] = {
        textOffsetX,
        (viewport.getHeight() / 2)
    };
    const int textRightWindowXY[2] = {
        viewport.getWidth() - textOffsetX,
        (viewport.getHeight() / 2)
    };
    const int textBottomWindowXY[2] = {
        viewport.getWidth() / 2,
        textOffsetY
    };
    const int textTopWindowXY[2] = {
        (viewport.getWidth() / 2),
        viewport.getHeight() - textOffsetY
    };

    const std::array<uint8_t, 4> axialPlaneRGBA(getAxisColor(VolumeSliceViewPlaneEnum::AXIAL));
    const std::array<uint8_t, 4> coronalPlaneRGBA(getAxisColor(VolumeSliceViewPlaneEnum::CORONAL));
    const std::array<uint8_t, 4> parasagittalPlaneRGBA(getAxisColor(VolumeSliceViewPlaneEnum::PARASAGITTAL));

    bool radiologicalFlag(false);
    switch (m_orientationMode) {
        case VolumeMprOrientationModeEnum::NEUROLOGICAL:
            radiologicalFlag = false;
            break;
        case VolumeMprOrientationModeEnum::RADIOLOGICAL:
            radiologicalFlag = true;
            break;
    }

    /*
     * Text for sides of viewport
     */
    AString northText;
    AString eastText;
    AString southText;
    AString westText;

    const bool dynamicLabelsFlag(true);
    if (dynamicLabelsFlag) {
        mprSliceView.getAxisLabels(westText, eastText, southText, northText);

        horizontalAxisRGBA = foregroundRGBA;
        verticalAxisRGBA   = foregroundRGBA;
    }
    else {
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                northText = "A";
                eastText  = "R";
                southText = "P";
                westText  = "L";
                horizontalAxisRGBA = coronalPlaneRGBA;
                verticalAxisRGBA   = parasagittalPlaneRGBA;
                if (radiologicalFlag) {
                    std::swap(eastText, westText);
                }
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                northText = "S";
                eastText  = "P";
                southText = "I";
                westText  = "A";
                horizontalAxisRGBA = axialPlaneRGBA;
                verticalAxisRGBA   = coronalPlaneRGBA;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                northText = "S";
                eastText  = "R";
                southText = "I";
                westText  = "L";
                horizontalAxisRGBA = axialPlaneRGBA;
                verticalAxisRGBA   = parasagittalPlaneRGBA;
                if (radiologicalFlag) {
                    std::swap(eastText, westText);
                }
                break;
        }
    }
    
    AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
    annotationText.setBoldStyleEnabled(true);
    annotationText.setFontPercentViewportSize(5.0f);
    annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
    annotationText.setTextColor(CaretColorEnum::CUSTOM);
    annotationText.setCustomTextColor(horizontalAxisRGBA.data());
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
        annotationText.setCustomTextColor(verticalAxisRGBA.data());
        
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
}

/**
 * Get the orthographic projection.
 *
 * @param boundingBox
 *    Bounding box for all volume.
 * @param zoomFactor
 *    Zooming
 * @param viewport
 *    The viewport.
 * @param orthographicBoundsOut
 *    Output containing the orthographic bounds used for orthographic projection.
 */
void
BrainOpenGLVolumeMprThreeDrawing::getOrthographicProjection(const BoundingBox& boundingBox,
                                                          const float zoomFactor,
                                                          const GraphicsViewport& viewport,
                                                          double orthographicBoundsOut[6])
{
    /*
     * Determine aspect ratio of viewport
     */
    const double viewportWidth = viewport.getWidthF();
    const double viewportHeight = viewport.getHeightF();
    CaretAssert(viewportWidth > 0.0);
    CaretAssert(viewportHeight > 0.0);
    const double viewportAspectRatio = (viewportHeight
                                        / viewportWidth);
    
    
    /*
     * Bounds of volume
     */
    const float lengthX(boundingBox.getDifferenceX());
    const float lengthY(boundingBox.getDifferenceY());
    const float lengthZ(boundingBox.getDifferenceZ());
    
    /*
     * Note: Horizontal axis is always X (axial, coronal) or Y (parasagittal).
     *       Vertical axis is always Y (axial) or Z (coronal or parasagittal).
     */
    float defaultOrthoWidth = std::max(lengthX,
                                       lengthY);
    float defaultOrthoHeight = std::max(lengthY,
                                        lengthZ);
    
    if (viewportAspectRatio > 1.0) {
        defaultOrthoHeight = (defaultOrthoWidth * viewportAspectRatio);
    }
    else {
        defaultOrthoWidth = (defaultOrthoHeight / viewportAspectRatio);
    }
    
    const float scaling(1.0 / zoomFactor);
    
    const double halfOrthoHeight((defaultOrthoHeight / 2.0) * scaling);
    const double halfOrthoWidth((defaultOrthoWidth / 2.0) * scaling);
    
    /*
     * Set bounds of orthographic projection
     */
    double orthoBottom(-halfOrthoHeight);
    double orthoTop(halfOrthoHeight);
    double orthoRight(halfOrthoWidth);
    double orthoLeft(-halfOrthoWidth);
    const double nearDepth = -1000.0;
    const double farDepth  =  1000.0;
    
    orthographicBoundsOut[0] = orthoLeft;
    orthographicBoundsOut[1] = orthoRight;
    orthographicBoundsOut[2] = orthoBottom;
    orthographicBoundsOut[3] = orthoTop;
    orthographicBoundsOut[4] = nearDepth;
    orthographicBoundsOut[5] = farDepth;
}

/**
 * Set the orthographic projection.
 *
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeMprThreeDrawing::setOrthographicProjection(const GraphicsViewport& viewport)
{
    /*
     * Bounds of volume
     */
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    const float zoomFactor(m_browserTabContent->getScaling());
    double orthographicBounds[6];
    getOrthographicProjection(boundingBox,
                              zoomFactor,
                              viewport,
                              orthographicBounds);
    
    m_orthographicBounds[0] = orthographicBounds[0];
    m_orthographicBounds[1] = orthographicBounds[1];
    m_orthographicBounds[2] = orthographicBounds[2];
    m_orthographicBounds[3] = orthographicBounds[3];
    m_orthographicBounds[4] = orthographicBounds[4];
    m_orthographicBounds[5] = orthographicBounds[5];
    

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(m_orthographicBounds[0],
            m_orthographicBounds[1],
            m_orthographicBounds[2],
            m_orthographicBounds[3],
            m_orthographicBounds[4],
            m_orthographicBounds[5]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    bool drawOrthoBoxFlag(false);
    if (drawOrthoBoxFlag) {
        const float orthoLeft(m_orthographicBounds[0]);
        const float orthoRight(m_orthographicBounds[1]);
        const float orthoBottom(m_orthographicBounds[2]);
        const float orthoTop(m_orthographicBounds[3]);
        
        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(5.0);
        glBegin(GL_LINE_LOOP);
        glVertex2f(orthoLeft + 1, orthoBottom + 1);
        glVertex2f(orthoRight - 1, orthoBottom + 1);
        glVertex2f(orthoRight - 1, orthoTop - 1);
        glVertex2f(orthoLeft + 1, orthoTop - 1);
        glEnd();
    }
}

/**
 * Set the viewing transformation
 *
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param mprSliceView
 *    Info about virtual slice.
 */
void
BrainOpenGLVolumeMprThreeDrawing::setViewingTransformation(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                           const VolumeMprVirtualSliceView& mprSliceView)
{
    /*
     * Initialize the modelview matrix to the identity matrix
     * This places the camera at the origin, pointing down the
     * negative-Z axis with the up vector set to (0,1,0 =>
     * positive-Y is up).
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*
     * Panning performed by user.  Apply before settting view.
     */
    Vector3D translation;
    m_browserTabContent->getTranslation(translation);
    float userTransX(0.0), userTransY(0.0), userTransZ(0.0);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            userTransX = translation[0];
            userTransY = translation[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            userTransX = translation[0];
            userTransY = translation[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            /*
             * Note: Use negative for horizontal since parasagittal
             * has positive on left, negative on right
             */
            userTransX = -translation[1];
            userTransY =  translation[2];
            break;
    }
    glTranslatef(userTransX, userTransY, userTransZ);

    /*
     * Permits rotation around selected coordinate
     */
    const Vector3D prelt(mprSliceView.getPreLookAtTranslation());
    float preltX(0.0), preltY(0.0), preltZ(0.0);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            preltX = prelt[0];
            preltY = prelt[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            preltX = prelt[0];
            preltY = prelt[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            preltX = prelt[1];
            preltY = prelt[2];
            break;
    }
    glTranslatef(preltX, preltY, preltZ);

    const Vector3D cameraXYZ(mprSliceView.getCameraXYZ());
    const Vector3D cameraLookAtXYZ(mprSliceView.getCameraLookAtXYZ());
    const Vector3D cameraUpVector(mprSliceView.getCameraUpVector());
    gluLookAt(cameraXYZ[0],
              cameraXYZ[1],
              cameraXYZ[2],
              cameraLookAtXYZ[0],
              cameraLookAtXYZ[1],
              cameraLookAtXYZ[2],
              cameraUpVector[0],
              cameraUpVector[1],
              cameraUpVector[2]);
    
    /*
     * Permits rotation around selected coordinate
     */
    const Vector3D plt(mprSliceView.getPostLookAtTranslation());
    glTranslatef(plt[0], plt[1], plt[2]);
    
    const Matrix4x4 mat(mprSliceView.getTransformationMatrix());
    float m16[16];
    mat.getMatrixForOpenGL(m16);
    glMultMatrixf(m16);
    
    const bool printViewingTransformFlag(false);
    if (printViewingTransformFlag) {
        if (mprSliceView.getSliceViewPlane() == VolumeSliceViewPlaneEnum::PARASAGITTAL) {
            static int32_t ctr(0);
            std::cout << "iter=" << ctr++ << std::endl;
            std::cout << mprSliceView.toString() << std::endl;
            
            EventOpenGLObjectToWindowTransform wt(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
            EventManager::get()->sendEvent(wt.getPointer());
            if (wt.isValid()) {
                /*
                 * Used to debug slices 'jumping' when user changes selected
                 * slices after a rotation of the slices
                 */
                Vector3D originXYZ(0.0, 0.0, 0.0);
                Vector3D windowXYZ(0.0, 0.0, 0.0);
                wt.transformPoint(originXYZ, windowXYZ);
                std::cout << "   Origin: " << originXYZ.toString() << std::endl;
                std::cout << "   Window: " << windowXYZ.toString() << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

/**
 * @return All intersections of ray with the the volume
 * @param volume
 *    The volume file
 * @param rayOrigin
 *    The ray's origin
 * @param rayVector
 *    The ray's direction
 */
std::vector<Vector3D>
BrainOpenGLVolumeMprThreeDrawing::getVolumeRayIntersections(VolumeMappableInterface* volume,
                                                            const Vector3D& rayOrigin,
                                                            const Vector3D& rayVector) const
{
    std::vector<Vector3D> allIntersections;
    
    VolumePlaneIntersection volPlan(volume);
    AString errorMessage;
    if (volPlan.intersectWithRay(rayOrigin,
                                 rayVector,
                                 allIntersections,
                                 errorMessage)) {
        if (m_debugFlag) {
            std::cout << "Volume Intersections: " << std::endl;
            for (auto& v : allIntersections) {
                std::cout << "   " << v.toString() << std::endl;
            }
        }
    }
    else {
        CaretLogSevere("Volume Ray Intersection Failure: "
                       + errorMessage);
    }
    
    return allIntersections;
}

/**
 * Filter the volume drawing info based upon modes
 * @param volumeDrawInfoInOut
 *    Volume draw info that is filtered based upon intensity mode enabled
 */
void
BrainOpenGLVolumeMprThreeDrawing::filterVolumeDrawInfo(std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfoInOut) const
{
    bool intensityFlag(false);
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
            intensityFlag = true;
            break;
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
            intensityFlag = true;
            break;
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            intensityFlag = true;
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            intensityFlag = false;
            break;
    }
    
    std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo> validDrawInfo;
    
    for (auto& vdi: volumeDrawInfoInOut) {
        if (intensityFlag) {
            if (vdi.volumeFile != NULL) {
                validDrawInfo.push_back(vdi);
                
                /*
                 * Only allow one volume for intensity drawing
                 * as drawing of the volume is "averaged / minimized
                 * or maximized" in the frame buffer by drawing
                 * many slices.
                 */
                break;
            }
        }
        else {
            validDrawInfo.push_back(vdi);
        }
    }
    
    volumeDrawInfoInOut = validDrawInfo;
}


/**
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param rayVolumeIntersections
 *    Intersections of ray with volume.
 */
void
BrainOpenGLVolumeMprThreeDrawing::applySliceThicknessToIntersections(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                     const Vector3D& sliceCoordinates,
                                                                     std::vector<Vector3D>& rayVolumeIntersections) const
{
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
            break;
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
            break;
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            return;
            break;
    }

    float minX(-std::numeric_limits<float>::max());
    float maxX(std::numeric_limits<float>::max());
    float minY(-std::numeric_limits<float>::max());
    float maxY(std::numeric_limits<float>::max());
    float minZ(-std::numeric_limits<float>::max());
    float maxZ(std::numeric_limits<float>::max());
    
    CaretAssert(m_browserTabContent);
    const float halfThickness(m_browserTabContent->getVolumeMprSliceThickness() / 2.0);
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            if (m_browserTabContent->isVolumeMprAllViewThicknessEnabled()) {
                if (rayVolumeIntersections.size() == 2) {
                    CaretAssertVectorIndex(rayVolumeIntersections, 1);
                    const Vector3D p1(rayVolumeIntersections[0]);
                    const Vector3D p2(rayVolumeIntersections[1]);
                    if (m_debugFlag) {
                        std::cout << "P1, P2: "
                        << AString::fromNumbers(p1)
                        << "   " << AString::fromNumbers(p2)
                        << std::endl;
                    }
                    float distanceToLine(-1.0);
                    float firstPointToPointOnLineNormalizedDistance(-1.0);
                    Vector3D pointOnLineXYZ;
                    MathFunctions::nearestPointOnLine3D(p1,
                                                        p2,
                                                        sliceCoordinates,
                                                        pointOnLineXYZ,
                                                        firstPointToPointOnLineNormalizedDistance,
                                                        distanceToLine);
                    if (m_debugFlag) {
                        std::cout << "Point on line, first point normalized distance: "
                        << AString::fromNumbers(pointOnLineXYZ)
                        << ",  "
                        << firstPointToPointOnLineNormalizedDistance
                        << std::endl;
                    }
                    const Vector3D rayDirection((p2 - p1).normal());
                    const Vector3D rayOffset(rayDirection * halfThickness);
                    const Vector3D newP1(pointOnLineXYZ - rayOffset);
                    const Vector3D newP2(pointOnLineXYZ + rayOffset);
                    if (m_debugFlag) {
                        std::cout << "P1, P2: "
                        << AString::fromNumbers(newP1)
                        << "   " << AString::fromNumbers(newP2)
                        << std::endl << std::endl;
                    }
                    rayVolumeIntersections.clear();
                    rayVolumeIntersections.push_back(newP1);
                    rayVolumeIntersections.push_back(newP2);
                    return;
                }
            }
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            if (m_browserTabContent->isVolumeMprAxialSliceThicknessEnabled()) {
                minZ = sliceCoordinates[2] - halfThickness;
                maxZ = sliceCoordinates[2] + halfThickness;
            }
            else {
                return;
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            if (m_browserTabContent->isVolumeMprCoronalSliceThicknessEnabled()) {
                minY = sliceCoordinates[1] - halfThickness;
                maxY = sliceCoordinates[1] + halfThickness;
            }
            else {
                return;
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            if (m_browserTabContent->isVolumeMprParasagittalSliceThicknessEnabled()) {
                minX = sliceCoordinates[0] - halfThickness;
                maxX = sliceCoordinates[0] + halfThickness;
            }
            else {
                return;
            }
            break;
    }
    
    for (auto& vec : rayVolumeIntersections) {
        if (vec[0] > maxX) vec[0] = maxX;
        if (vec[0] < minX) vec[0] = minX;
        if (vec[1] > maxY) vec[1] = maxY;
        if (vec[1] < minY) vec[1] = minY;
        if (vec[2] > maxZ) vec[2] = maxZ;
        if (vec[2] < minZ) vec[2] = minZ;
    }
}

/**
 * Draw the intensity slices
 * @param mprSliceView
 *    Information for drawing slice
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawSliceIntensityProjection2D(const VolumeMprVirtualSliceView& mprSliceView,
                                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                 const Vector3D& sliceCoordinates,
                                                                 const GraphicsViewport& viewport)
{
    /*
     * Intensity drawing can only be performed on a single volume
     */
    CaretAssert(m_volumeDrawInfo.size() == 1);
    if (m_volumeDrawInfo.empty()) {
        return;
    }
    CaretAssertVectorIndex(m_volumeDrawInfo, 0);
    const BrainOpenGLFixedPipeline::VolumeDrawInfo& intensityVolumeDrawInfo(m_volumeDrawInfo[0]);
    
    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    SelectionItemVoxelEditing* voxelEditingID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
    bool idModeFlag(false);
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()
                || voxelEditingID->isEnabledForSelection()) {
                idModeFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    m_fixedPipelineDrawing->applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_VOLUME,
                                                StructureEnum::ALL);
    
    float sliceThickness(-1.0);
    bool sliceThicknessValidFlag(false);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            if (m_browserTabContent->isVolumeMprAxialSliceThicknessEnabled()) {
                sliceThickness = m_browserTabContent->getVolumeMprSliceThickness();
                sliceThicknessValidFlag = true;
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            if (m_browserTabContent->isVolumeMprCoronalSliceThicknessEnabled()) {
                sliceThickness = m_browserTabContent->getVolumeMprSliceThickness();
                sliceThicknessValidFlag = true;
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            if (m_browserTabContent->isVolumeMprParasagittalSliceThicknessEnabled()) {
                sliceThickness = m_browserTabContent->getVolumeMprSliceThickness();
                sliceThicknessValidFlag = true;
            }
            break;
    }
    
    VolumeMappableInterface* volumeFile(intensityVolumeDrawInfo.volumeFile);
    const int32_t mapIndex(intensityVolumeDrawInfo.mapIndex);
    CaretAssert(volumeFile);
    if (idModeFlag) {
        CaretAssertToDoFatal(); /* should be using normal slice ID */
        performIntensityIdentification(mprSliceView,
                                       sliceViewPlane,
                                       volumeFile,
                                       mapIndex);
        return;
    }
    
    BoundingBox boundingBox;
    volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    const float farAway(boundingBox.getMaximumDifferenceOfXYZ()
                        * 20.0);
    const Vector3D sliceCenterXYZ(sliceCoordinates);
    const Vector3D rayVector((mprSliceView.getCameraLookAtXYZ()
                              - mprSliceView.getCameraXYZ()).normal());
    const Vector3D rayOrigin(sliceCenterXYZ
                             - (rayVector * farAway));
    if (m_debugFlag) {
        std::cout << "Ray origin: " << rayOrigin.toString() << std::endl;
        std::cout << "    vector: " << rayVector.toString() << std::endl;
    }
    
    std::vector<Vector3D> allIntersections(getVolumeRayIntersections(volumeFile,
                                                                     rayOrigin,
                                                                     rayVector));
    const int32_t numIntersections(allIntersections.size());
    
    if (numIntersections == 2) {
        CaretAssertVectorIndex(allIntersections, 1);
        const Vector3D nearestIntersectionXYZ(allIntersections[0]);
        const Vector3D furthestIntersectionXYZ(allIntersections[1]);
        const Vector3D farToNearVector((nearestIntersectionXYZ - furthestIntersectionXYZ).normal());
        const Vector3D nearToFarVector(-farToNearVector);
        
        Vector3D startXYZ(furthestIntersectionXYZ);
        Vector3D endXYZ(nearestIntersectionXYZ);
        
        if (sliceThicknessValidFlag) {
            if (sliceThickness > 0.0) {
                const float halfThickness(sliceThickness / 2.0);
                
                /*
                 * If the 'half thickness' distance is within the volume
                 * adjust the start to the 'half thickness'
                 */
                if ((sliceCenterXYZ - startXYZ).length() > halfThickness) {
                    startXYZ = (sliceCenterXYZ
                                + (nearToFarVector * halfThickness));
                }
                
                /*
                 * If the 'half thickness' distance is within the volume
                 * adjust the end to the 'half thickness'
                 */
                if ((sliceCenterXYZ - endXYZ).length() > halfThickness) {
                    endXYZ = (sliceCenterXYZ
                              - (nearToFarVector * halfThickness));
                }
            }
            else {
                /*
                 * For zero thickness, just use the single slice
                 */
                startXYZ = sliceCenterXYZ;
                endXYZ   = sliceCenterXYZ;
            }
        }
        
        if (m_debugFlag) {
            std::cout << VolumeSliceViewPlaneEnum::toName(sliceViewPlane)
            << " Start XYZ: " << startXYZ.toString() << " End XYZ: " << endXYZ.toString() << std::endl;
        }
        
        const float voxelSize(getVoxelSize(volumeFile));
        if (voxelSize < 0.01) {
            CaretLogSevere("Voxel size is too small for Intensity Projection: "
                           + AString::number(voxelSize));
            return;
        }
        
        const float distance = (endXYZ - startXYZ).length();
        const Vector3D stepXYZ(farToNearVector * voxelSize);
        int32_t numSteps = static_cast<int32_t>(distance / voxelSize);
        if (numSteps < 1) {
            numSteps = 1;
        }
        if (numSteps == 1) {
            startXYZ = sliceCenterXYZ;
        }
        if (m_debugFlag) {
            std::cout << "Num Steps: " << numSteps << " Step XYZ: " << AString::fromNumbers(stepXYZ) << std::endl;
        }
        
        /*
         * Save state to preserve blending setup
         */
        glPushAttrib(GL_COLOR_BUFFER_BIT
                     | GL_ENABLE_BIT);
        
        /*
         * Disable culling so that both sides of the triangles/quads are drawn.
         */
        glDisable(GL_CULL_FACE);
        
        switch (m_mprViewMode) {
            case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
                setupMprBlending(BlendingMode::AVERAGE,
                                 s_INVALID_ALPHA_VALUE,
                                 numSteps);
                break;
            case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
                setupMprBlending(BlendingMode::INTENSITY_MAXIMUM,
                                 s_INVALID_ALPHA_VALUE,
                                 s_INVALID_NUMBER_OF_SLICES);
                break;
            case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
                setupMprBlending(BlendingMode::INTENSITY_MINIMUM,
                                 s_INVALID_ALPHA_VALUE,
                                 s_INVALID_NUMBER_OF_SLICES);
                break;
            case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
                CaretAssert(0);
                break;
        }
        
        Vector3D xyz(startXYZ);
        
        for (int32_t iStep = 0; iStep < numSteps; iStep++) {
            const VolumeMprVirtualSliceView mprSliceView(createSliceInfo(volumeFile,
                                                                         sliceViewPlane,
                                                                         xyz));
            if (m_debugFlag) {
                if (iStep == 0) {
                    std::cout << "First slice: " << xyz.toString() << std::endl;
                }
                else if (iStep == (numSteps - 1)) {
                    std::cout << "Last Slice: " << xyz.toString() << std::endl;
                }
            }
            
            const bool enableBlendingFlag(false);
            const bool drawAttributesFlag(false);
            const bool drawIntensitySliceBackgroundFlag(iStep == 0);
            drawSliceWithPrimitive(mprSliceView,
                                   sliceViewPlane,
                                   viewport,
                                   enableBlendingFlag,
                                   drawAttributesFlag,
                                   drawIntensitySliceBackgroundFlag);
            xyz += stepXYZ;
        }
        
        glPopAttrib();
    }
    else if (numIntersections > 0) {
        AString txt;
        txt.appendWithNewLine("Ray origin: " + AString::fromNumbers(rayOrigin));
        txt.appendWithNewLine("Ray Vector: " + AString::fromNumbers(rayVector));
        txt.appendWithNewLine("Intersections: ");
        for (int32_t i = 0; i < numIntersections; i++) {
            CaretAssertVectorIndex(allIntersections, i);
            txt.appendWithNewLine("   " + AString::fromNumbers(allIntersections[i]));
        }
        CaretLogSevere("Possible vector volume algorithm failure for Intensity Projection.  "
                       + txt);
    }
    
    BrainOpenGLIdentificationDrawing idDrawing(m_fixedPipelineDrawing,
                                               m_brain,
                                               m_browserTabContent,
                                               m_fixedPipelineDrawing->mode);
    idDrawing.drawVolumeIntensity2dIdentificationSymbols(volumeFile,
                                                         mapIndex,
                                                         m_browserTabContent->getScaling(),
                                                         viewport.getHeight());
    
    
    m_fixedPipelineDrawing->disableClippingPlanes();
    
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            CaretAssert(0);
            break;
        case BrainModelMode::ALL_3D:
            break;
        case BrainModelMode::VOLUME_2D:
            drawCrosshairs(volumeFile,
                           mprSliceView,
                           sliceViewPlane,
                           sliceCoordinates,
                           viewport);
            break;
    }
}

/**
 * Draw the slice
 * @param mprSliceView
 *    Information for drawing slice
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 * @param enabledBlendingFlag
 *    If true, enable blending
 * @param drawAttributesFlag
 *    Draw attributes (crosshairs, etc)
 * @param drawIntensitySliceBackgroundFlag
 *    Draw the background for intensity mode
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawSliceWithPrimitive(const VolumeMprVirtualSliceView& mprSliceView,
                                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                       const GraphicsViewport& viewport,
                                                       const bool enabledBlendingFlag,
                                                       const bool drawAttributesFlag,
                                                       const bool drawIntensitySliceBackgroundFlag)
{
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    if (numVolumes < 1) {
        return;
    }
    
    std::unique_ptr<VolumeMprViewportSlice> mprViewportSlice;
    bool useTriangleIdentFlag(true);
    switch (mprSliceView.getViewType()) {
        case VolumeMprVirtualSliceView::ViewType::VOLUME_VIEW_FIXED_CAMERA:
            useTriangleIdentFlag = true;
            break;
        case VolumeMprVirtualSliceView::ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            mprViewportSlice.reset(new VolumeMprViewportSlice(viewport,
                                                              mprSliceView.getVirtualPlane()));
            break;
        case VolumeMprVirtualSliceView::ViewType::ALL_VIEW_SLICES:
            mprViewportSlice.reset(new VolumeMprViewportSlice(viewport,
                                                              mprSliceView.getVirtualPlane()));
            break;
    }

    /*
     * When performing voxel identification for editing voxels,
     * we need to draw EVERY voxel since the user may click
     * regions where the voxels are "off".
     */
    VolumeFile* voxelEditingVolumeFile = NULL;
    if (m_identificationModeFlag) {
        SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
        if (voxelEditID->isEnabledForSelection()) {
            voxelEditingVolumeFile = voxelEditID->getVolumeFileForEditing();
            if (voxelEditingVolumeFile != NULL) {
                if (voxelEditingVolumeFile->isMappedWithLabelTable()) {
                    if (voxelEditingVolumeFile->getNumberOfMaps() > 0) {
                        /*voxelEditingValue = voxelEditingVolumeFile->getMapLabelTable(0)->getUnassignedLabelKey();*/
                    }
                }
            }
        }
    }
    
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            /*
             * Intensity use buffer to average/min/max so drawing
             * is limited to one volume
             */
            CaretAssert(numVolumes == 1);
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            break;
    }
    
    float allViewOpacity(1.0);
    bool allViewBlendingFlag(false);
    bool allViewFlag(false);
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::ALL_3D:
        {
            allViewFlag = true;
            
            const DisplayPropertiesVolume* dsv = m_brain->getDisplayPropertiesVolume();
            allViewOpacity = dsv->getOpacity();
            if ((allViewOpacity >= 0.0)
                && (allViewOpacity < 1.0)) {
                allViewBlendingFlag = true;
            }
        }
            break;
        case BrainModelMode::VOLUME_2D:
            break;
    }
    
    if (allViewFlag) {
        glPushAttrib(GL_COLOR_BUFFER_BIT
                     | GL_POLYGON_BIT);
    }
    else {
        glPushAttrib(GL_COLOR_BUFFER_BIT);
    }
    
    glDisable(GL_DEPTH_TEST);
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::VOLUME_2D:
            break;
        case BrainModelMode::ALL_3D:
            glAlphaFunc(GL_GEQUAL, 0.95);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_DEPTH_TEST);
            break;
    }
    
    bool firstFlag(true);
    for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
        const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = m_volumeDrawInfo[iVol];
        VolumeMappableInterface* volumeInterface = vdi.volumeFile;
        if (volumeInterface != NULL) {
            if (allViewBlendingFlag) {
                setupMprBlending(BlendingMode::ALL_VIEW,
                                 allViewOpacity,
                                 s_INVALID_NUMBER_OF_SLICES);
            }
            else if (enabledBlendingFlag) {
                if (firstFlag) {
                    firstFlag = false;
                    
                    /*
                     * Added 01nov2025.  Seems to allow background
                     * to show through voxels with alpha equal to
                     * zero.
                     *
                     * Note: enabling this might cause a white line
                     * around the edge of the volume slice (texture).
                     */
                    const bool allowBackgroundToShowThroughFlag(true);
                    if (allowBackgroundToShowThroughFlag) {
                        setupMprBlending(BlendingMode::MPR_OVERLAY_SLICE,
                                         vdi.opacity,
                                         s_INVALID_NUMBER_OF_SLICES);
                    }
                    else {
                        setupMprBlending(BlendingMode::MPR_UNDERLAY_SLICE,
                                         s_INVALID_ALPHA_VALUE,
                                         s_INVALID_NUMBER_OF_SLICES);
                        /*
                         * May fix labels on/off when only one layer
                         * setupMprBlending(BlendingMode::MPR_OVERLAY_SLICE, //JWH 27aug2024
                         *                1.0,
                         *                s_INVALID_NUMBER_OF_SLICES);
                         */
                    }
                }
                else {
                    setupMprBlending(BlendingMode::MPR_OVERLAY_SLICE,
                                     vdi.opacity,
                                     s_INVALID_NUMBER_OF_SLICES);
                }
            }
            
            if (allViewFlag) {
                if (iVol > 0) {
                    /*
                     * All view needs polygon offset when there is
                     * more than one layer to ensure layer 'is above'
                     * and prevents flashing
                     */
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    glPolygonOffset(-2.0, 2.0);
                }
            }
            const TabDrawingInfo tabDrawingInfo(vdi.mapFile,
                                                vdi.mapIndex,
                                                m_displayGroup,
                                                m_labelViewMode,
                                                m_tabIndex);
            
            GraphicsPrimitive* primitive(NULL);
            GraphicsPrimitiveV3fT3f* primitive3(NULL);
            GraphicsPrimitiveV3fT2f* primitive2(NULL);
            if (volumeInterface->isSingleSlice()) {
                primitive2 = volumeInterface->getSingleSliceVolumeDrawingPrimitive(vdi.mapIndex,
                                                                                   tabDrawingInfo);
                primitive = primitive2;
            }
            else {
                primitive3 = volumeInterface->getVolumeDrawingTrianglesPrimitive(vdi.mapIndex,
                                                                                 tabDrawingInfo);
                primitive = primitive3;
            }
            CaretAssert(primitive);
            
            if (primitive != NULL) {
                const Vector3D sliceOffset(0.0, 0.0, 0.0);

                bool validPrimitiveFlag(false);
                if (primitive3 != NULL) {
                    validPrimitiveFlag = setPrimitiveCoordinates(mprSliceView,
                                                                 volumeInterface,
                                                                 primitive3);
                }
                else if (primitive2 != NULL) {
                    validPrimitiveFlag = true;
                }

                if (validPrimitiveFlag) {
                    bool discreteFlag(false);
                    bool magNearestFlag(false);
                    bool magSmoothFlag(false);
                    if (vdi.mapFile->isMappedWithLabelTable()
                        || (vdi.mapFile->isMappedWithRGBA())) {
                        /*
                         * Use "discrete" with Labels or RGBA since no interpolation
                         */
                        discreteFlag = true;
                    }
                    else if (vdi.mapFile->isMappedWithPalette()) {
                        bool anatomyFlag(false);
                        bool functionalFlag(false);
                        if (DataFileTypeEnum::isConnectivityDataType(vdi.mapFile->getDataFileType())) {
                            /*
                             * CIFTI is always a functional type
                             */
                            functionalFlag = true;
                        }
                        else {
                            /*
                             * Must be a volume file
                             */
                            const VolumeFile* vf(dynamic_cast<VolumeFile*>(vdi.volumeFile));
                            CaretAssert(vf);
                            switch (vf->getType()) {
                                case SubvolumeAttributes::ANATOMY:
                                    anatomyFlag = true;
                                    break;
                                case SubvolumeAttributes::FUNCTIONAL:
                                    functionalFlag = true;
                                    break;
                                case SubvolumeAttributes::LABEL:
                                    CaretAssert(0);
                                    break;
                                case SubvolumeAttributes::RGB:
                                    CaretAssert(0);
                                    break;
                                case SubvolumeAttributes::RGB_WORKBENCH:
                                    /*
                                     * Use "discrete" with RGB since no interpolation
                                     */
                                    discreteFlag = true;
                                    break;
                                case SubvolumeAttributes::SEGMENTATION:
                                    anatomyFlag = true;
                                    break;
                                case SubvolumeAttributes::UNKNOWN:
                                    anatomyFlag = true;
                                    break;
                                case SubvolumeAttributes::VECTOR:
                                    CaretAssert(0);
                                    break;
                            }
                        }
                        
                        if (anatomyFlag) {
                            if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DELELOPER_FLAG_TEXTURE_ANATOMY_VOLUME_SMOOTH)) {
                                magSmoothFlag = true;
                            }
                            else {
                                magNearestFlag = true;
                            }
                        }
                        else if (functionalFlag) {
                            if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DELELOPER_FLAG_TEXTURE_FUNCTIONAL_VOLUME_SMOOTH)) {
                                magSmoothFlag = true;
                            }
                            else {
                                magNearestFlag = true;
                            }
                        }
                    }
                    
                    /**
                     * Using 'smooth' (averaging of neighboring voxels) and blending (alpha)
                     * causes problems (streaks) during image capture operations that
                     * render to an offscreen buffer.
                     */
                    if (BrainOpenGLFixedPipeline::isImageCaptureInProgress()) {
                        if (magSmoothFlag) {
                            magSmoothFlag  = false;
                            magNearestFlag = true;
                        }
                    }
                    
                    if (discreteFlag) {
                        primitive->setTextureMinificationFilter(GraphicsTextureMinificationFilterEnum::NEAREST);
                        primitive->setTextureMagnificationFilter(GraphicsTextureMagnificationFilterEnum::NEAREST);
                    }
                    else if (magNearestFlag) {
                        /* Use Linear for Minification, Nearest for Magnification */
                        primitive->setTextureMinificationFilter(GraphicsTextureMinificationFilterEnum::LINEAR);
                        primitive->setTextureMagnificationFilter(GraphicsTextureMagnificationFilterEnum::NEAREST);
                    }
                    else if (magSmoothFlag) {
                        /* Use Linear for both Minification and Magnification */
                        primitive->setTextureMinificationFilter(GraphicsTextureMinificationFilterEnum::LINEAR);
                        primitive->setTextureMagnificationFilter(GraphicsTextureMagnificationFilterEnum::LINEAR);
                    }
                    else {
                        CaretAssert(0);
                    }
                    
                    if (drawAttributesFlag
                        && m_identificationModeFlag) {
                        /*
                         * To get correct depth values, slice must be drawn with
                         * depth testing enabled.
                         */
                        glPushAttrib(GL_DEPTH_BUFFER_BIT);
                        glEnable(GL_DEPTH_TEST);
                        if (drawIntensitySliceBackgroundFlag) {
                            drawIntensityBackgroundSlice(primitive);
                        }
                        GraphicsEngineDataOpenGL::draw(primitive);
                        
                        if (useTriangleIdentFlag) {
                            if (primitive2 != NULL) {
                                performSingleSliceIdentification(primitive2,
                                                                 mprSliceView,
                                                                 volumeInterface,
                                                                 sliceViewPlane,
                                                                 viewport,
                                                                 m_fixedPipelineDrawing->mouseX,
                                                                 m_fixedPipelineDrawing->mouseY);
                            }
                            else if (primitive3 != NULL) {
                                performTriangleIdentification(primitive3,
                                                              mprSliceView,
                                                              volumeInterface,
                                                              sliceViewPlane,
                                                              viewport,
                                                              m_fixedPipelineDrawing->mouseX,
                                                              m_fixedPipelineDrawing->mouseY);
                            }
                            else {
                                CaretAssert(0);
                            }
                        }
                        else {
                            CaretAssert(mprViewportSlice);
                            performViewportSliceIdentification(mprSliceView,
                                                               *mprViewportSlice.get(),
                                                               volumeInterface,
                                                               m_fixedPipelineDrawing->mouseX,
                                                               m_fixedPipelineDrawing->mouseY);
                        }
                        
                        glPopAttrib();
                    }
                    else {
                        if (drawIntensitySliceBackgroundFlag) {
                            CaretAssert(primitive3);
                            drawIntensityBackgroundSlice(primitive3);
                        }
                        
                        /*
                         * Enabling blending prevents a while line around several
                         * sides of the texture
                         */
                        GraphicsEngineDataOpenGL::draw(primitive);
                    }
                }
            }
        }
    }
    
    glPopAttrib();

    bool drawViewportBoxFlag(false);
    if (m_debugFlag
        || drawViewportBoxFlag) {
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        
        /*
         * Draw a box around the bounds of the volume
         * (assuming no panning/rotatinig)
         */
        BoundingBox boundingBox;
        m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
        const float minX(boundingBox.getMinX());
        const float maxX(boundingBox.getMaxX());
        const float minY(boundingBox.getMinY());
        const float maxY(boundingBox.getMaxY());
        const float minZ(boundingBox.getMinZ());
        const float maxZ(boundingBox.getMaxZ());
        
        const float d(4.0);
        Vector3D bottomLeft;
        Vector3D bottomRight;
        Vector3D topRight;
        Vector3D topLeft;
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                bottomLeft[0] = minX + d;
                bottomLeft[1] = minY + d;
                bottomLeft[2] = 0.0;
                bottomRight[0] = maxX - d;
                bottomRight[1] = minY + d;
                bottomRight[2] = 0.0;
                topRight[0] = maxX - d;
                topRight[1] = maxY - d;
                topRight[2] = 0.0;
                topLeft[0] = minX + d;
                topLeft[1] = maxY - d;
                topLeft[2] = 0.0;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                bottomLeft[0] = minX + d;
                bottomLeft[1] = 0.0;
                bottomLeft[2] = minZ + d;
                bottomRight[0] = maxX - d;
                bottomRight[1] = 0.0;
                bottomRight[2] = minZ + d;
                topRight[0] = maxX - d;
                topRight[1] = 0.0;
                topRight[2] = maxZ - d;
                topLeft[0] = minX + d;
                topLeft[1] = 0.0;
                topLeft[2] = maxZ - d;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                bottomLeft[0] = 0.0;
                bottomLeft[1] = maxY - d;
                bottomLeft[2] = minZ + d;
                bottomRight[0] = 0.0;
                bottomRight[1] = minY + d;
                bottomRight[2] = minZ + d;
                topRight[0] = 0.0;
                topRight[1] = minY + d;
                topRight[2] = maxZ - d;
                topLeft[0] = 0.0;
                topLeft[1] = maxY - d;
                topLeft[2] = maxZ - d;
                break;
        }
        
        glPopAttrib();
    }
}

/**
 * Convert mouse window X, Y to viewport X, Y
 * @param viewport
 *    The graphics viewport
 * @param mouseX
 *    Mouse window X
 * @param mouseY
 *    Mouse window Y
 * @param outViewportMouseX
 *    Output mouse Viewport X
 * @param outViewportMouseY
 *    Output mouse Viewport Y
 */
void
BrainOpenGLVolumeMprThreeDrawing::getMouseViewportXY(const GraphicsViewport& viewport,
                                                   const float mouseX,
                                                   const float mouseY,
                                                   float& outViewportMouseX,
                                                   float& outViewportMouseY) const
{
    outViewportMouseX = (mouseX - viewport.getXF());
    outViewportMouseY = (mouseY - viewport.getYF());
}

/**
 * Convert mouse window X, Y to viewport X, Y
 * @param viewport
 *    The viewport
 * @param mouseX
 *    Mouse window X
 * @param mouseY
 *    Mouse window Y
 * @param outViewportMouseX
 *    Output mouse Viewport X
 * @param outViewportMouseY
 *    Output mouse Viewport Y
 *
 */
void
BrainOpenGLVolumeMprThreeDrawing::getMouseViewportNormalizedXY(const GraphicsViewport& viewport,
                                                             const float mouseX,
                                                             const float mouseY,
                                                             float& outViewportNormalizedMouseX,
                                                             float& outViewportNormalizedMouseY) const
{
    float viewportMouseX(0.0);
    float viewportMouseY(0.0);
    getMouseViewportXY(viewport,
                       mouseX,
                       mouseY,
                       viewportMouseX,
                       viewportMouseY);
    outViewportNormalizedMouseX = ((mouseX - viewport.getXF()) / viewport.getWidthF());
    outViewportNormalizedMouseY = ((mouseY - viewport.getYF()) / viewport.getHeightF());
}

/**
 * Perform voxel identification using triangles in the primitive
 * @param mprViewportSlice
 *    The viewport slice
 * @param slicePrimitive
 *    Primitve used to draw virtual slice
 * @param mouseX
 *    X location of mouse click
 * @param mouseY
 *    Y location of mouse click
 */
void
BrainOpenGLVolumeMprThreeDrawing::performViewportSliceIdentification(const VolumeMprVirtualSliceView& mprSliceView,
                                                                     const VolumeMprViewportSlice& mprViewportSlice,
                                                                     VolumeMappableInterface* volume,
                                                                     const float mouseX,
                                                                     const float mouseY)
{
    if ( ! mprViewportSlice.containsWindowXY(mouseX, mouseY)) {
        return;
    }
        
    float depthValue(0.0);
    float rgbaValues[4];
    float mouseZ(0.0);
    if (m_fixedPipelineDrawing->getPixelDepthAndRGBA(mouseX, mouseY,
                                                     depthValue, rgbaValues)) {
        mouseZ = depthValue;
    }

    /*
     * Project mouse coordinate to slice plane
     */
    const Vector3D mouseXYZ(mouseX,
                            mouseY,
                            mouseZ);
    const Vector3D& slicePlaneXYZ(mprViewportSlice.mapWindowXyzToSliceXYZ(mouseXYZ));
    
    int64_t ijk[3];
    volume->enclosingVoxel(slicePlaneXYZ[0], slicePlaneXYZ[1], slicePlaneXYZ[2],
                           ijk[0], ijk[1], ijk[2]);
    if (volume->indexValid(ijk[0], ijk[1], ijk[2])) {
        SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
        if (voxelID->isEnabledForSelection()) {
            if ( ! voxelID->isValid()) {
                /*
                 * Get depth from depth buffer
                 */
                glPixelStorei(GL_PACK_ALIGNMENT, 4); /* float */
                float selectedPrimitiveDepth(0.0);
                glReadPixels(mouseX,
                             mouseY,
                             1,
                             1,
                             GL_DEPTH_COMPONENT,
                             GL_FLOAT,
                             &selectedPrimitiveDepth);
                
                /*
                 * Voxel identification
                 */
                if (voxelID->isOtherScreenDepthCloserToViewer(selectedPrimitiveDepth)) {
                    voxelID->setVoxelIdentification(m_brain,
                                                    volume,
                                                    ijk,
                                                    slicePlaneXYZ,
                                                    mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                    selectedPrimitiveDepth);
                    
                    m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                     slicePlaneXYZ);
                    CaretLogFinest("Selected Voxel (3D): " + AString::fromNumbers(ijk, 3, ","));
                }
            }
        }
        
        /*
         * Voxel editing identification
         */
        SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
        if (voxelEditID->isEnabledForSelection()) {
            if (voxelEditID->getVolumeFileForEditing() == volume) {
                /*
                 * Get depth from depth buffer
                 */
                glPixelStorei(GL_PACK_ALIGNMENT, 4); /* float */
                float selectedPrimitiveDepth(0.0);
                glReadPixels(mouseX,
                             mouseY,
                             1,
                             1,
                             GL_DEPTH_COMPONENT,
                             GL_FLOAT,
                             &selectedPrimitiveDepth);
                Vector3D xyz;
                volume->indexToSpace(ijk, xyz);
                voxelEditID->setVoxelIdentification(m_brain,
                                                    volume,
                                                    ijk,
                                                    xyz,
                                                    mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                    selectedPrimitiveDepth);
                const float floatDiffXYZ[3] { 0.0, 0.0, 0.0 };
                voxelEditID->setVoxelDiffXYZ(floatDiffXYZ);
                
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelEditID,
                                                                 xyz);
                CaretLogFinest("Selected Voxel Editing (3D): Indices ("
                               + AString::fromNumbers(ijk, 3, ",")
                               + ") Diff XYZ ("
                               + AString::fromNumbers(floatDiffXYZ, 3, ",")
                               + ")");
            }
        }
    }
}

/**
 * Perform voxel identification using triangles in the primitive
 * @param slicePrimitive
 *    Primitve used to draw virtual slice
 * @param mprSliceView
 *    Info about the slice being drawan
 * @param volumeInterface
 *    The volume
 * @param sliceViewPlane
 *    Plane being viewed
 * @param viewport
 *    Viewport of drawing region
 * @param mouseX
 *    X location of mouse click
 * @param mouseY
 *    Y location of mouse click
 */
void
BrainOpenGLVolumeMprThreeDrawing::performTriangleIdentification(const GraphicsPrimitive* slicePrimitive,
                                                                const VolumeMprVirtualSliceView& mprSliceView,
                                                                VolumeMappableInterface* volume,
                                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                const GraphicsViewport& viewport,
                                                                const float mouseX,
                                                                const float mouseY)
{
    CaretAssert(slicePrimitive);
    if (slicePrimitive->getPrimitiveType() != GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES) {
        return;
    }
    const int32_t numTriangles(slicePrimitive->getNumberOfVertices() / 3);
    if (numTriangles <= 0) {
        CaretLogSevere("Primitive for identification contains no triangles");
        return;
    }
    
    float mouseNormX(0.0);
    float mouseNormY(0.0);
    getMouseViewportNormalizedXY(viewport,
                                 mouseX,
                                 mouseY,
                                 mouseNormX,
                                 mouseNormY);
    
    bool swapNormXValueFlag(false);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            swapNormXValueFlag = true;
            break;
    }
    mouseNormX = (swapNormXValueFlag
                  ? (1.0 - mouseNormX)
                  : mouseNormX);
    if (m_debugFlag) {
        std::cout << "   Mouse Norm X/Y: " << mouseNormX << ", " << mouseNormY << std::endl << std::flush;
    }
    
    if ((mouseNormX >= 0.0)
        && (mouseNormX <= 1.0)
        && (mouseNormY >= 0.0)
        && (mouseNormY <= 1.0)) {
        /* Mouse in viewport */
    }
    else {
        /* Mouse NOT in viewport */
        return;
    }
    
    /*
     * Get the stereotaxic coordinates (primitive are ???)
     */
    std::vector<Vector3D> stereotaxicXYZ;
    {
        std::vector<Vector3D> textureStr;
        Plane layersDrawingPlaneUnused;
        mprSliceView.getTrianglesCoordinates(volume,
                                             stereotaxicXYZ,
                                             textureStr,
                                             layersDrawingPlaneUnused);
    }
    const int32_t numStereotaxicTriangles(stereotaxicXYZ.size() / 3);
    if (numTriangles != numStereotaxicTriangles) {
        CaretLogSevere("Stereotaxic coords count != primitive count");
        return;
    }
    
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);  /* Pixel storage */
    glPushAttrib(GL_COLOR_BUFFER_BIT     /* clear color */
                 | GL_LIGHTING_BIT       /* lighting and shade model */
                 | GL_PIXEL_MODE_BIT); /* glReadBuffer */
    
    glShadeModel(GL_SMOOTH);  /* need shading so that colors are interpolated */
    glDisable(GL_LIGHTING);
    
    /*
     * Must clear color and depth buffers
     */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT);

    Vector3D selectedXYZ;
    bool selectedXYZValid(false);
    
    /*
     * Colors for each triangle vertex
     */
    const float v1RGBA[4] { 1.0, 0.0, 0.0, 1.0 };
    const float v2RGBA[4] { 0.0, 1.0, 0.0, 1.0 };
    const float v3RGBA[4] { 0.0, 0.0, 1.0, 1.0 };
    
    for (int32_t iTriangle = 0; iTriangle < numTriangles; iTriangle++) {
        /*
         * Draw each of the triangles one at a time with red, green, and blue
         * colors at the triangle's vertices.  After drawing the triangle
         * get the color buffer's RGBA values at the location of the cursor.
         * If the alpha value is not zero, the RGB components (that are floats)
         * represent the percentage of each vertex contributing to the pixel's
         * color and allows computation of the stereotaxic coordinate at
         * the cursor.
         */
        std::unique_ptr<GraphicsPrimitiveV3fC4f> primitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES));
        
        Vector3D v1XYZ, v2XYZ, v3XYZ;
        const int32_t vertexIndex(iTriangle * 3);
        slicePrimitive->getVertexFloatXYZ(vertexIndex,     v1XYZ);
        slicePrimitive->getVertexFloatXYZ(vertexIndex + 1, v2XYZ);
        slicePrimitive->getVertexFloatXYZ(vertexIndex + 2, v3XYZ);


        primitive->addVertex(v1XYZ, v1RGBA);
        primitive->addVertex(v2XYZ, v2RGBA);
        primitive->addVertex(v3XYZ, v3RGBA);
        
        /*
         * Must clear color and depth buffers for each triangle.
         * Otherwise, colors from previous triangle remain in buffer
         */
        glClear(GL_COLOR_BUFFER_BIT
                | GL_DEPTH_BUFFER_BIT);
        
        GraphicsEngineDataOpenGL::draw(primitive.get());
        
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1); /* bytes */
        
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        /* do not call glReadBuffer() */
#else
        glReadBuffer(GL_BACK);
#endif
        
        /*
         * Read the RGB color at location of mouse
         */
        float pixelRGBA[4];
        glReadPixels(m_fixedPipelineDrawing->mouseX,
                     m_fixedPipelineDrawing->mouseY,
                     1,
                     1,
                     GL_RGBA,
                     GL_FLOAT,
                     pixelRGBA);
        
        /*
         * Red ranges 0 (bottom) to 1 (top)
         * Green ranges 0 (bottom) to 1 (top)
         */
        const float redPct(pixelRGBA[0]);
        const float greenPct(pixelRGBA[1]);
        const float bluePct(pixelRGBA[2]);
        const float alphaPct(pixelRGBA[3]);
        
        if (alphaPct >= 1.0) {
            /*
             * Use the stereotaxic coordinates
             */
            selectedXYZ = ((stereotaxicXYZ[vertexIndex] * redPct)
                           + (stereotaxicXYZ[vertexIndex + 1] * greenPct)
                           + (stereotaxicXYZ[vertexIndex + 2] * bluePct));
            selectedXYZValid = true;
        }
    }
    
    if (selectedXYZValid) {
        int64_t ijk[3];
        volume->enclosingVoxel(selectedXYZ[0], selectedXYZ[1], selectedXYZ[2],
                               ijk[0], ijk[1], ijk[2]);
        if (volume->indexValid(ijk[0], ijk[1], ijk[2])) {
            SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
            if (voxelID->isEnabledForSelection()) {
                if ( ! voxelID->isValid()) {
                    /*
                     * Get depth from depth buffer
                     */
                    glPixelStorei(GL_PACK_ALIGNMENT, 4); /* float */
                    float selectedPrimitiveDepth(0.0);
                    glReadPixels(mouseX,
                                 mouseY,
                                 1,
                                 1,
                                 GL_DEPTH_COMPONENT,
                                 GL_FLOAT,
                                 &selectedPrimitiveDepth);
                    
                    /*
                     * Voxel identification
                     */
                    if (voxelID->isOtherScreenDepthCloserToViewer(selectedPrimitiveDepth)) {
                        voxelID->setVoxelIdentification(m_brain,
                                                        volume,
                                                        ijk,
                                                        selectedXYZ,
                                                        mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                        selectedPrimitiveDepth);
                        
                        m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                         selectedXYZ);
                        CaretLogFinest("Selected Voxel (3D): " + AString::fromNumbers(ijk, 3, ","));
                    }
                }
            }
            
            /*
             * Voxel editing identification
             */
            SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
            if (voxelEditID->isEnabledForSelection()) {
                if (voxelEditID->getVolumeFileForEditing() == volume) {
                    /*
                     * Get depth from depth buffer
                     */
                    glPixelStorei(GL_PACK_ALIGNMENT, 4); /* float */
                    float selectedPrimitiveDepth(0.0);
                    glReadPixels(mouseX,
                                 mouseY,
                                 1,
                                 1,
                                 GL_DEPTH_COMPONENT,
                                 GL_FLOAT,
                                 &selectedPrimitiveDepth);
                    Vector3D xyz;
                    volume->indexToSpace(ijk, xyz);
                    voxelEditID->setVoxelIdentification(m_brain,
                                                        volume,
                                                        ijk,
                                                        xyz,
                                                        mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                        selectedPrimitiveDepth);
                    const float floatDiffXYZ[3] { 0.0, 0.0, 0.0 };
                    voxelEditID->setVoxelDiffXYZ(floatDiffXYZ);
                    
                    m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelEditID,
                                                                     xyz);
                    CaretLogFinest("Selected Voxel Editing (3D): Indices ("
                                   + AString::fromNumbers(ijk, 3, ",")
                                   + ") Diff XYZ ("
                                   + AString::fromNumbers(floatDiffXYZ, 3, ",")
                                   + ")");
                }
            }
        }
    }
    
    /*
     * Restore OpenGL settings
     */
    glPopAttrib();
    glPopClientAttrib();
}

/**
 * Perform voxel identification for single slice volume
 * @param slicePrimitive
 *    Primitve used to draw virtual slice
 * @param mprSliceView
 *    Info about the slice being drawan
 * @param volumeInterface
 *    The volume
 * @param sliceViewPlane
 *    Plane being viewed
 * @param viewport
 *    Viewport of drawing region
 * @param mouseX
 *    X location of mouse click
 * @param mouseY
 *    Y location of mouse click
 */
void
BrainOpenGLVolumeMprThreeDrawing::performSingleSliceIdentification(const GraphicsPrimitiveV3fT2f* slicePrimitive,
                                                                   const VolumeMprVirtualSliceView& mprSliceView,
                                                                   VolumeMappableInterface* volume,
                                                                   const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                   const GraphicsViewport& viewport,
                                                                   const float mouseX,
                                                                   const float mouseY)
{
    CaretAssert(slicePrimitive);
    if (slicePrimitive->getPrimitiveType() != GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES) {
        return;
    }
    const int32_t numTriangles(slicePrimitive->getNumberOfVertices() / 3);
    if (numTriangles <= 0) {
        CaretLogSevere("Primitive for identification contains no triangles");
        return;
    }
    
    float mouseNormX(0.0);
    float mouseNormY(0.0);
    getMouseViewportNormalizedXY(viewport,
                                 mouseX,
                                 mouseY,
                                 mouseNormX,
                                 mouseNormY);
    
    bool swapNormXValueFlag(false);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            swapNormXValueFlag = true;
            break;
    }
    mouseNormX = (swapNormXValueFlag
                  ? (1.0 - mouseNormX)
                  : mouseNormX);
    if (m_debugFlag) {
        std::cout << "   Mouse Norm X/Y: " << mouseNormX << ", " << mouseNormY << std::endl << std::flush;
    }
    
    if ((mouseNormX >= 0.0)
        && (mouseNormX <= 1.0)
        && (mouseNormY >= 0.0)
        && (mouseNormY <= 1.0)) {
        /* Mouse in viewport */
    }
    else {
        /* Mouse NOT in viewport */
        return;
    }
    
    /*
     * Get the stereotaxic coordinates (primitive are ???)
     */
    std::vector<Vector3D> stereotaxicXYZ;
    const std::vector<float>& xyz = slicePrimitive->getFloatXYZ();
    const int32_t numVertices(xyz.size() / 3);
    for (int32_t i = 0; i < numVertices; i++) {
        const int32_t i3(i * 3);
        CaretAssertVectorIndex(xyz, i3 + 2);
        stereotaxicXYZ.emplace_back(xyz[i3], xyz[i3+1], xyz[i3+2]);
    }
    const int32_t numStereotaxicTriangles(stereotaxicXYZ.size() / 3);
    if (numTriangles != numStereotaxicTriangles) {
        CaretLogSevere("Stereotaxic coords count != primitive count");
        return;
    }
    
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);  /* Pixel storage */
    glPushAttrib(GL_COLOR_BUFFER_BIT     /* clear color */
                 | GL_LIGHTING_BIT       /* lighting and shade model */
                 | GL_PIXEL_MODE_BIT); /* glReadBuffer */
    
    glShadeModel(GL_SMOOTH);  /* need shading so that colors are interpolated */
    glDisable(GL_LIGHTING);
    
    /*
     * Must clear color and depth buffers
     */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT);
    
    Vector3D selectedXYZ;
    bool selectedXYZValid(false);
    
    /*
     * Colors for each triangle vertex
     */
    const float v1RGBA[4] { 1.0, 0.0, 0.0, 1.0 };
    const float v2RGBA[4] { 0.0, 1.0, 0.0, 1.0 };
    const float v3RGBA[4] { 0.0, 0.0, 1.0, 1.0 };
    
    for (int32_t iTriangle = 0; iTriangle < numTriangles; iTriangle++) {
        /*
         * Draw each of the triangles one at a time with red, green, and blue
         * colors at the triangle's vertices.  After drawing the triangle
         * get the color buffer's RGBA values at the location of the cursor.
         * If the alpha value is not zero, the RGB components (that are floats)
         * represent the percentage of each vertex contributing to the pixel's
         * color and allows computation of the stereotaxic coordinate at
         * the cursor.
         */
        std::unique_ptr<GraphicsPrimitiveV3fC4f> primitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES));
        
        const int32_t vertexIndex(iTriangle * 3);
        CaretAssertVectorIndex(stereotaxicXYZ, vertexIndex + 2);
        primitive->addVertex(stereotaxicXYZ[vertexIndex],     v1RGBA);
        primitive->addVertex(stereotaxicXYZ[vertexIndex + 1], v2RGBA);
        primitive->addVertex(stereotaxicXYZ[vertexIndex + 2], v3RGBA);

        /*
         * Must clear color and depth buffers for each triangle.
         * Otherwise, colors from previous triangle remain in buffer
         */
        glClear(GL_COLOR_BUFFER_BIT
                | GL_DEPTH_BUFFER_BIT);
        
        GraphicsEngineDataOpenGL::draw(primitive.get());
        
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1); /* bytes */
        
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        /* do not call glReadBuffer() */
#else
        glReadBuffer(GL_BACK);
#endif
        
        /*
         * Read the RGB color at location of mouse
         */
        float pixelRGBA[4];
        glReadPixels(m_fixedPipelineDrawing->mouseX,
                     m_fixedPipelineDrawing->mouseY,
                     1,
                     1,
                     GL_RGBA,
                     GL_FLOAT,
                     pixelRGBA);
        
        /*
         * Red ranges 0 (bottom) to 1 (top)
         * Green ranges 0 (bottom) to 1 (top)
         */
        const float redPct(pixelRGBA[0]);
        const float greenPct(pixelRGBA[1]);
        const float bluePct(pixelRGBA[2]);
        const float alphaPct(pixelRGBA[3]);
        
        if (alphaPct >= 1.0) {
            /*
             * Use the stereotaxic coordinates
             */
            selectedXYZ = ((stereotaxicXYZ[vertexIndex] * redPct)
                           + (stereotaxicXYZ[vertexIndex + 1] * greenPct)
                           + (stereotaxicXYZ[vertexIndex + 2] * bluePct));
            selectedXYZValid = true;
        }
    }
    
    if (selectedXYZValid) {
        int64_t ijk[3];
        volume->enclosingVoxel(selectedXYZ[0], selectedXYZ[1], selectedXYZ[2],
                               ijk[0], ijk[1], ijk[2]);
        if (volume->indexValid(ijk[0], ijk[1], ijk[2])) {
            SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
            if (voxelID->isEnabledForSelection()) {
                if ( ! voxelID->isValid()) {
                    /*
                     * Get depth from depth buffer
                     */
                    glPixelStorei(GL_PACK_ALIGNMENT, 4); /* float */
                    float selectedPrimitiveDepth(0.0);
                    glReadPixels(mouseX,
                                 mouseY,
                                 1,
                                 1,
                                 GL_DEPTH_COMPONENT,
                                 GL_FLOAT,
                                 &selectedPrimitiveDepth);
                    
                    /*
                     * Voxel identification
                     */
                    if (voxelID->isOtherScreenDepthCloserToViewer(selectedPrimitiveDepth)) {
                        voxelID->setVoxelIdentification(m_brain,
                                                        volume,
                                                        ijk,
                                                        selectedXYZ,
                                                        mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                        selectedPrimitiveDepth);
                        
                        m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                         selectedXYZ);
                        CaretLogFinest("Selected Voxel (3D): " + AString::fromNumbers(ijk, 3, ","));
                    }
                }
            }
            
            /*
             * Voxel editing identification
             */
            SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
            if (voxelEditID->isEnabledForSelection()) {
                if (voxelEditID->getVolumeFileForEditing() == volume) {
                    /*
                     * Get depth from depth buffer
                     */
                    glPixelStorei(GL_PACK_ALIGNMENT, 4); /* float */
                    float selectedPrimitiveDepth(0.0);
                    glReadPixels(mouseX,
                                 mouseY,
                                 1,
                                 1,
                                 GL_DEPTH_COMPONENT,
                                 GL_FLOAT,
                                 &selectedPrimitiveDepth);
                    Vector3D xyz;
                    volume->indexToSpace(ijk, xyz);
                    voxelEditID->setVoxelIdentification(m_brain,
                                                        volume,
                                                        ijk,
                                                        xyz,
                                                        mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                        selectedPrimitiveDepth);
                    const float floatDiffXYZ[3] { 0.0, 0.0, 0.0 };
                    voxelEditID->setVoxelDiffXYZ(floatDiffXYZ);
                    
                    m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelEditID,
                                                                     xyz);
                    CaretLogFinest("Selected Voxel Editing (3D): Indices ("
                                   + AString::fromNumbers(ijk, 3, ",")
                                   + ") Diff XYZ ("
                                   + AString::fromNumbers(floatDiffXYZ, 3, ",")
                                   + ")");
                }
            }
        }
    }
    
    /*
     * Restore OpenGL settings
     */
    glPopAttrib();
    glPopClientAttrib();
}

/**
 * Draw the layers type data.
 *
 * @param mprSliceView
 *    The virtual slice view
 * @param underlayVolume
 *   The underlay volume
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param slicePlane
 *    Plane of the slice.
 * @param sliceCoordinates
 *    Coordinates of the selected slices.
 * @param sliceThickness
 *    Thickness of slices
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawLayers(const VolumeMprVirtualSliceView& mprSliceView,
                                             const VolumeMappableInterface* underlayVolume,
                                             const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                             const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                             const Plane& slicePlane,
                                             const Vector3D& sliceCoordinates,
                                             const float sliceThickness)
{
    bool drawFibersFlag     = true;
    bool drawFociFlag       = true;
    bool drawOutlineFlag    = true;
    
    if (m_identificationModeFlag) {
        drawOutlineFlag = false;
    }
    
    ModelTypeEnum::Enum modelType(ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES);
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::ALL_3D:
            drawFibersFlag = false;
            modelType = ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN;
            break;
        case BrainModelMode::VOLUME_2D:
            break;
    }

    if (slicePlane.isValidPlane()) {
        /*
         * Save cull and depth test status
         */
        glPushAttrib(GL_ENABLE_BIT);
        
        /*
         * Disable culling so that both sides of the triangles/quads are drawn.
         */
        glDisable(GL_CULL_FACE);
        
        GLboolean depthBufferEnabled = false;
        glGetBooleanv(GL_DEPTH_TEST,
                      &depthBufferEnabled);

        glPushMatrix();
        
        /*
         * Use some polygon offset that will adjust the depth values of the
         * layers so that the layers depth values place the layers in front of
         * the volume slice.
         */
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.0, 2.0);
        
        if (drawOutlineFlag) {
            Matrix4x4 displayTransformMatrix;
            bool displayTransformMatrixValidFlag(false);
            switch (mprSliceView.getViewType()) {
                case VolumeMprVirtualSliceView::ViewType::ALL_VIEW_SLICES:
                    break;
                case VolumeMprVirtualSliceView::ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
                    break;
                case VolumeMprVirtualSliceView::ViewType::VOLUME_VIEW_FIXED_CAMERA:
                    break;
            }
            bool useNegativePolygonOffsetFlag(true);
            BrainOpenGLVolumeSurfaceOutlineDrawing outlineDrawing;
            outlineDrawing.drawSurfaceOutline(underlayVolume,
                                              modelType,
                                              sliceProjectionType,
                                              sliceViewPlane,
                                              sliceCoordinates,
                                              slicePlane,
                                              displayTransformMatrix,
                                              displayTransformMatrixValidFlag,
                                              m_browserTabContent->getVolumeSurfaceOutlineSet(),
                                              m_fixedPipelineDrawing,
                                              useNegativePolygonOffsetFlag);
        }
        
        if (drawFibersFlag) {
            glDisable(GL_DEPTH_TEST);
            m_fixedPipelineDrawing->drawFiberOrientations(&slicePlane,
                                                          StructureEnum::ALL);
            m_fixedPipelineDrawing->drawFiberTrajectories(&slicePlane,
                                                          StructureEnum::ALL);
        }
        
        if (drawFociFlag) {
            BrainOpenGLFociDrawing fociDrawing;
            fociDrawing.drawVolumeOrthogonalFoci(m_brain,
                                                 m_fixedPipelineDrawing,
                                                 const_cast<VolumeMappableInterface*>(underlayVolume),
                                                 slicePlane,
                                                 sliceViewPlane,
                                                 sliceThickness);
        }
        
        glDisable(GL_POLYGON_OFFSET_FILL);
        
        glPopMatrix();
        
        glPopAttrib();
    }
}

/**
 * The intensity modes set a replace a pixel when the new pixel value is greater than (MAXIMUM mode)
 * or less than the existing pixel (MINIMUM mode).  For this to work, the background must be black (for MAXIMUM)
 * or white (for MINIIMUM).  So prior to drawing any pixels from the volume, draw a slice in either black or white.
 * @param volumePrimitive
 *    Primitive from the volume used to draw voxels.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawIntensityBackgroundSlice(const GraphicsPrimitive* volumePrimitive) const
{
    CaretAssert(volumePrimitive);
    const int32_t numVertices(volumePrimitive->getNumberOfVertices());
    if (numVertices < 3) {
        return;
    }
    
    float backgroundRGBA[4] { 0.0, 0.0, 0.0, 1.0 };
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
            /*
             * Draw black background
             */
            backgroundRGBA[0] = 0.0;
            backgroundRGBA[1] = 0.0;
            backgroundRGBA[2] = 0.0;
            break;
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
            /*
             * Draw black background
             */
            backgroundRGBA[0] = 0.0;
            backgroundRGBA[1] = 0.0;
            backgroundRGBA[2] = 0.0;
            break;
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            /*
             * Draw white background
             */
            backgroundRGBA[0] = 1.0;
            backgroundRGBA[1] = 1.0;
            backgroundRGBA[2] = 1.0;
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            CaretAssert(0);
            break;
    }
    
    /*
     * New primitive that copies vertices from volume primitive and uses constant color
     */
    const GraphicsPrimitive::PrimitiveType primitiveType(volumePrimitive->getPrimitiveType());
    std::unique_ptr<GraphicsPrimitiveV3f> backgroundPrimitive(GraphicsPrimitive::newPrimitiveV3f(primitiveType,
                                                                                                 backgroundRGBA));
    const std::vector<float>& verticesXYZ(volumePrimitive->getFloatXYZ());
    CaretAssert(static_cast<int32_t>(verticesXYZ.size()) == (numVertices * 3));
    backgroundPrimitive->addVertices(&verticesXYZ[0],
                                     numVertices);
    
    /*
     * Disable blending and use GL_ALWAYS for alpha so the that pixels are always updated
     */
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
            setupMprBlending(BlendingMode::BACKGROUND_INTENSITY_AVERAGE,
                             s_INVALID_ALPHA_VALUE,
                             s_INVALID_NUMBER_OF_SLICES);
            break;
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
            setupMprBlending(BlendingMode::BACKGROUND_MAXIMUM_INTENSITY_SLICE,
                             s_INVALID_ALPHA_VALUE,
                             s_INVALID_NUMBER_OF_SLICES);
            break;
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            setupMprBlending(BlendingMode::BACKGROUND_MINIMUM_INTENSITY_SLICE,
                             s_INVALID_ALPHA_VALUE,
                             s_INVALID_NUMBER_OF_SLICES);
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            CaretAssert(0);
            break;
    }
    GraphicsEngineDataOpenGL::draw(backgroundPrimitive.get());
    glPopAttrib();
}

/**
 * Draw the slice
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawSliceIntensityProjection3D(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                 const Vector3D& sliceCoordinates,
                                                                 const GraphicsViewport& viewport)
{
    /*
     * Intensity drawing can only be performed on a single volume
     */
    CaretAssert(m_volumeDrawInfo.size() == 1);
    if (m_volumeDrawInfo.empty()) {
        return;
    }
    CaretAssertVectorIndex(m_volumeDrawInfo, 0);
    const BrainOpenGLFixedPipeline::VolumeDrawInfo& intensityVolumeDrawInfo(m_volumeDrawInfo[0]);
    
    VolumeMappableInterface* volumeFile(intensityVolumeDrawInfo.volumeFile);
    const int32_t mapIndex(intensityVolumeDrawInfo.mapIndex);
    
    /*
     * For 3D would need to draw in an offscreen buffer
     * due to the way drawing is done.  Alpha testing
     * and blending are used by drawing repeatedly into
     * the frame buffer.
     */
    CaretAssertToDoFatal(); /* Does 2D slice info work for 3D ? */
    
    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    bool idModeFlag(false);
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (voxelID->isEnabledForSelection()) {
                idModeFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    m_fixedPipelineDrawing->applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_VOLUME,
                                                StructureEnum::ALL);
    
    bool drawBackgroundSliceFlag(true);
    
    const VolumeMprVirtualSliceView mprSliceView(createSliceInfo(volumeFile,
                                                                 sliceViewPlane,
                                                                 sliceCoordinates));
    if (idModeFlag) {
        performIntensityIdentification(mprSliceView,
                                       sliceViewPlane,
                                       volumeFile,
                                       mapIndex);
        return;
    }
    
    Vector3D sliceNormalVector;
    mprSliceView.getVirtualPlane().getNormalVector(sliceNormalVector);
    std::vector<Vector3D> allIntersections(getVolumeRayIntersections(volumeFile,
                                                                     mprSliceView.getVolumeCenterXYZ(),
                                                                     sliceNormalVector));
    const int32_t numIntersections(allIntersections.size());
    
    if (numIntersections == 2) {
        applySliceThicknessToIntersections(VolumeSliceViewPlaneEnum::ALL,
                                           sliceCoordinates,
                                           allIntersections);
        
        const float voxelSize(getVoxelSize(volumeFile));
        if (voxelSize < 0.01) {
            CaretLogSevere("Voxel size is too small for Intensity Projection: "
                           + AString::number(voxelSize));
            return;
        }
        CaretAssertVectorIndex(allIntersections, 1);
        const Vector3D p1(allIntersections[0]);
        const Vector3D p2(allIntersections[1]);
        const float distance = (p1 - p2).length();
        const Vector3D p1ToP2Vector((p2 - p1).normal());
        const Vector3D stepVector(p1ToP2Vector[0] * voxelSize,
                                  p1ToP2Vector[1] * voxelSize,
                                  p1ToP2Vector[2] * voxelSize);
        const float stepSize(voxelSize / 2.0);
        const float numSteps = distance / stepSize;
        if (m_debugFlag) {
            std::cout << "Num Steps: " << numSteps << " Step Vector: " << AString::fromNumbers(stepVector) << std::endl;
        }
        
        /*
         * Save state to preserve blending setup
         */
        glPushAttrib(GL_COLOR_BUFFER_BIT
                     | GL_ENABLE_BIT);
        
        /*
         * Disable culling so that both sides of the triangles/quads are drawn.
         */
        glDisable(GL_CULL_FACE);
        
        switch (m_mprViewMode) {
            case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
                setupMprBlending(BlendingMode::BACKGROUND_INTENSITY_AVERAGE,
                                 s_INVALID_ALPHA_VALUE,
                                 s_INVALID_NUMBER_OF_SLICES);
                break;
            case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
                setupMprBlending(BlendingMode::BACKGROUND_MAXIMUM_INTENSITY_SLICE,
                                 s_INVALID_ALPHA_VALUE,
                                 s_INVALID_NUMBER_OF_SLICES);
                break;
            case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
                setupMprBlending(BlendingMode::BACKGROUND_MINIMUM_INTENSITY_SLICE,
                                 s_INVALID_ALPHA_VALUE,
                                 s_INVALID_NUMBER_OF_SLICES);
                break;
            case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
                CaretAssert(0);
                break;
        }

        for (int32_t iStep = 0; iStep < numSteps; iStep++) {
            const TabDrawingInfo tabDrawingInfo(dynamic_cast<CaretMappableDataFile*>(volumeFile),
                                                mapIndex,
                                                m_displayGroup,
                                                m_labelViewMode,
                                                m_tabIndex);
            GraphicsPrimitiveV3fT3f* primitive(volumeFile->getVolumeDrawingTrianglesPrimitive(mapIndex,
                                                                                              tabDrawingInfo));
            
            if (primitive != NULL) {
                setPrimitiveCoordinates(mprSliceView,
                                        volumeFile,
                                        primitive);
                
                primitive->setTextureMinificationFilter(GraphicsTextureMinificationFilterEnum::LINEAR);
                primitive->setTextureMagnificationFilter(GraphicsTextureMagnificationFilterEnum::LINEAR);
                
                /*
                 * Disable depth testing, otherwise, the backfacing polygons
                 * may be removed by the depth testing and nothing will be seen
                 */
                glDisable(GL_DEPTH_TEST);
                
                if (drawBackgroundSliceFlag) {
                    drawBackgroundSliceFlag = false;
                    /*
                     * Necessary for Min/Max blending to function
                     */
                    drawIntensityBackgroundSlice(primitive); /* 3D */
                }
                GraphicsEngineDataOpenGL::draw(primitive);
            }
        }
        
        glPopAttrib();
        
    }
    else if (numIntersections > 0) {
        CaretLogSevere("Possible algorithm failure for Intensity Projection, intersection count="
                       + AString::number(numIntersections));
    }
    BrainOpenGLIdentificationDrawing idDrawing(m_fixedPipelineDrawing,
                                               m_brain,
                                               m_browserTabContent,
                                               m_fixedPipelineDrawing->mode);
    idDrawing.drawVolumeIntensity3dIdentificationSymbols(volumeFile,
                                                         mapIndex,
                                                         m_browserTabContent->getScaling(),
                                                         viewport.getHeight());
    
    m_fixedPipelineDrawing->disableClippingPlanes();
    
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            CaretAssert(0);
            break;
        case BrainModelMode::ALL_3D:
            break;
        case BrainModelMode::VOLUME_2D:
            CaretAssertToDoFatal();
            /* Finish
             drawCrosshairs(mprSliceView,
             sliceViewPlane,
             sliceCoordinates,
             viewport);
             */
            break;
    }
}

/**
 * @return Size of voxel (smallest of any dimension)
 * @param volume
 *    The volume
 */
float
BrainOpenGLVolumeMprThreeDrawing::getVoxelSize(const VolumeMappableInterface* volume) const
{
    CaretAssert(volume);
    float dx, dy, dz;
    volume->getVoxelSpacing(dx, dy, dz);
    const float voxelSize(std::fabs(std::min(dx, std::min(dy, dz))));
    return voxelSize;
}

/**
 * Perform identification operation on 2D or 3D Maximum or Minimum Intensity Projection
 * @param mprSliceView
 *    Info for drawing slices
 * @param sliceViewPlane
 *    Slice view plane being drawn
 * @param volume
 *    Volume being drawn
 * @param mapIndex
 *    Map file index
 */
void
BrainOpenGLVolumeMprThreeDrawing::performIntensityIdentification(const VolumeMprVirtualSliceView& mprSliceView,
                                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                 VolumeMappableInterface* volume,
                                                                 const int32_t mapIndex)
{
    CaretAssertToDoFatal(); /* should be using normal slice ID for identification */

    
    GraphicsViewport viewport(GraphicsViewport::newInstanceCurrentViewport());
    const int32_t mouseVpX(m_fixedPipelineDrawing->mouseX - viewport.getX());
    const int32_t mouseVpY(m_fixedPipelineDrawing->mouseY - viewport.getY());
    if ((mouseVpX <= 0)
        || (mouseVpY <= 0)
        || (mouseVpX >= viewport.getWidth())
        || (mouseVpY >= viewport.getHeight())) {
        return;
    }

    const float voxelSize(std::max(getVoxelSize(volume), 0.1f));
    
    bool idMaxIntensityFlag(false);
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
            CaretLogWarning("No identification in Average Intensity Projection");
            return;
            break;
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
            idMaxIntensityFlag = true;
            break;
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            idMaxIntensityFlag = false;
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            CaretAssert(0);
            break;
    }

    EventOpenGLObjectToWindowTransform transformEvent(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
    EventManager::get()->sendEvent(transformEvent.getPointer());
    
    float modelXYZ[3];
    transformEvent.inverseTransformPoint(m_fixedPipelineDrawing->mouseX, m_fixedPipelineDrawing->mouseY, 0.0,
                                         modelXYZ);

    /*
     * Create a vector at the location of the mouse (converted from screen to model coords) and
     * find the intersections of the vector with the volume.  Then iterate along the vector
     * finding the voxel with the greatest/least intensity and use this voxel as the
     * identified voxel.
     */
    Vector3D sliceNormalVector;
    mprSliceView.getVirtualPlane().getNormalVector(sliceNormalVector);
    const std::vector<Vector3D> allIntersections(getVolumeRayIntersections(volume,
                                                                           modelXYZ,
                                                                           sliceNormalVector));
    const int32_t numIntersections(allIntersections.size());
    
    if (numIntersections == 2) {
        CaretAssertVectorIndex(allIntersections, 1);
        const Vector3D p1(allIntersections[0]);
        const Vector3D p2(allIntersections[1]);
        
        const float distance(MathFunctions::distance3D(p1, p2));
        if (distance > 1.0) {
            float minMaxIntensity(idMaxIntensityFlag ? 0.0 : 256.0);
            
            const bool limitToThicknessRange(true);
            if (limitToThicknessRange) {
                bool limitFlag(false);
                switch (sliceViewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        if (m_browserTabContent->isVolumeMprAllViewThicknessEnabled()) {
                            limitFlag = true;
                        }
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        if (m_browserTabContent->isVolumeMprAxialSliceThicknessEnabled()) {
                            limitFlag = true;
                        }
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        if (m_browserTabContent->isVolumeMprCoronalSliceThicknessEnabled()) {
                            limitFlag = true;
                        }
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        if (m_browserTabContent->isVolumeMprParasagittalSliceThicknessEnabled()) {
                            limitFlag = true;
                        }
                        break;
                }
                
                if (limitFlag) {
                    std::cout << "Need to limit step start/end for intensity ID" << std::endl;
                }
            }
            
            const TabDrawingInfo tabDrawingInfo(dynamic_cast<CaretMappableDataFile*>(volume),
                                                mapIndex,
                                                m_displayGroup,
                                                m_labelViewMode,
                                                m_tabIndex);
            int64_t minMaxIJK[3] { -1, -1, -1 };
            const Vector3D p1toP2Vector((p2 - p1).normal());
            const float stepDistance(voxelSize);
            const int32_t numSteps(distance / stepDistance);
            for (int64_t iStep = 0; iStep < numSteps; iStep++) {
                const Vector3D xyz(p1 + (p1toP2Vector * (iStep * stepDistance)));
                
                int64_t voxelI(-1), voxelJ(-1), voxelK(-1);
                volume->enclosingVoxel(xyz[0], xyz[1], xyz[2], voxelI, voxelJ, voxelK);
                if (volume->indexValid(voxelI, voxelJ, voxelK)) {
                    const int32_t brickIndex(0);
                    uint8_t rgba[4];
                    volume->getVoxelColorInMap(voxelI, voxelJ, voxelK, brickIndex,
                                               tabDrawingInfo, rgba);
                    if (rgba[3] > 0) {
                        const float intensity((rgba[0] * 0.30)
                                              + (rgba[1] * 0.59)
                                              + (rgba[2] * 0.11));
                        if (idMaxIntensityFlag) {
                            if (intensity > minMaxIntensity) {
                                minMaxIntensity = intensity;
                                minMaxIJK[0] = voxelI;
                                minMaxIJK[1] = voxelJ;
                                minMaxIJK[2] = voxelK;
                            }
                        }
                        else {
                            if (intensity < minMaxIntensity) {
                                minMaxIntensity = intensity;
                                minMaxIJK[0] = voxelI;
                                minMaxIJK[1] = voxelJ;
                                minMaxIJK[2] = voxelK;
                            }
                        }
                    }
                }
            }
            if (minMaxIJK[0] >= 0) {
                /*
                 * Voxel Identification
                 */
                SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
                float xyz[3];
                CaretAssertMessage(0, "Should not use index to space, use actual XYZ");
                volume->indexToSpace(minMaxIJK, xyz);
                voxelID->setModelXYZ(xyz);
                
                float primitiveDepth(1.0);
                voxelID->setVoxelIdentification(m_brain,
                                                volume,
                                                minMaxIJK,
                                                xyz,
                                                mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                primitiveDepth);
                
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                 xyz);
                
                /*
                 * Voxel editing identification
                 */
                SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
                if (voxelEditID->isEnabledForSelection()) {
                    if (voxelEditID->getVolumeFileForEditing() == volume) {
                        voxelEditID->setVoxelIdentification(m_brain,
                                                            volume,
                                                            minMaxIJK,
                                                            xyz,
                                                            mprSliceView.getMontageTopLeftSliceDirectionPlane(),
                                                            primitiveDepth);
                        const float floatDiffXYZ[3] { 0.0, 0.0, 0.0 };
                        voxelEditID->setVoxelDiffXYZ(floatDiffXYZ);
                                                
                        m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelEditID,
                                                                         xyz);
                        CaretLogFinest("Selected Voxel Editing (3D): Indices ("
                                       + AString::fromNumbers(minMaxIJK, 3, ",")
                                       + ") Diff XYZ ("
                                       + AString::fromNumbers(floatDiffXYZ, 3, ",")
                                       + ")");
                    }
                }
            }
        }
    }
}


/**
 * Draw montage slices.
 *
 * @param viewportContent
 *   Content of viewport
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawVolumeSliceViewTypeMontage(const BrainOpenGLViewportContent* viewportContent,
                                                               const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                               const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                               const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                               const GraphicsViewport& viewport)
{
    const int32_t numRows = m_browserTabContent->getVolumeMontageNumberOfRows();
    CaretAssert(numRows > 0);
    const int32_t numCols = m_browserTabContent->getVolumeMontageNumberOfColumns();
    CaretAssert(numCols > 0);
    
    const GapsAndMargins* gapsAndMargins = m_brain->getGapsAndMargins();
    
    const int32_t windowIndex = m_fixedPipelineDrawing->m_windowIndex;
    
    int32_t vpSizeY        = 0;
    int32_t verticalMargin = 0;
    BrainOpenGLFixedPipeline::createSubViewportSizeAndGaps(viewport.getHeight(),
                                                           gapsAndMargins->getVolumeMontageVerticalGapForWindow(windowIndex),
                                                           -1,
                                                           numRows,
                                                           vpSizeY,
                                                           verticalMargin);
    
    int32_t vpSizeX          = 0;
    int32_t horizontalMargin = 0;
    BrainOpenGLFixedPipeline::createSubViewportSizeAndGaps(viewport.getWidth(),
                                                           gapsAndMargins->getVolumeMontageHorizontalGapForWindow(windowIndex),
                                                           -1,
                                                           numCols,
                                                           vpSizeX,
                                                           horizontalMargin);
    
    /*
     * Voxel sizes for underlay volume
     */
    CaretAssertVectorIndex(m_volumeDrawInfo, 0);
    const VolumeMappableInterface* underlayVolume(m_volumeDrawInfo[0].volumeFile);
    CaretAssert(underlayVolume);
    float spacingX(0.0), spacingY(0.0), spacingZ(0.0);
    /* Returned spacing always positive */
    underlayVolume->getVoxelSpacing(spacingX, spacingY, spacingZ);
    float sliceThickness = std::min(spacingX, std::min(spacingY, spacingZ));
    if (sliceThickness <= 0.0) {
        CaretLogSevere("Invalid spacing for underlay volume in MPR drawing");
        return;
    }
    
    const int32_t sliceIndexStep = m_browserTabContent->getVolumeMontageSliceSpacing();
    
    /*
     * Determine a slice offset to selected slices is in
     * the center of the montage
     */
    const int32_t numberOfSlicesInMontageGrid = (numCols * numRows);
    
    const Vector3D selectedXYZ {
        m_browserTabContent->getVolumeSliceCoordinateParasagittal(),
        m_browserTabContent->getVolumeSliceCoordinateCoronal(),
        m_browserTabContent->getVolumeSliceCoordinateAxial()
    };
    
    const VolumeMprVirtualSliceView mprSliceView(createSliceInfo(underlayVolume,
                                                                 sliceViewPlane,
                                                                 selectedXYZ));

    /*
     * coordinate step to move between adjacent slices
     */
    Vector3D sliceCoordIncreaseDirectionVector;
    mprSliceView.getMontageTopLeftSliceDirectionPlane().getNormalVector(sliceCoordIncreaseDirectionVector);
    const Vector3D singleSliceCoordStepXYZ(sliceCoordIncreaseDirectionVector * sliceThickness);
    if (m_debugFlag) std::cout << "Single slice step XYZ: " << singleSliceCoordStepXYZ.toString() << std::endl;
    
    /*
     * Coordinate step to move between adjacent MONTAGE slices
     */
    const Vector3D montageSliceCoordStepXYZ(singleSliceCoordStepXYZ * sliceIndexStep);
    if (m_debugFlag) std::cout << "   Montage slice step XYZ: " << montageSliceCoordStepXYZ.toString() << std::endl;
    
    /*
     * Offset (in slice indices) of first slice (top left) in montage grid
     * from the selected slice indices
     */
    const float firstSliceIndexOffset((numberOfSlicesInMontageGrid - 1.0) / 2.0);
    if (m_debugFlag) std::cout << "   First slice index: " << firstSliceIndexOffset << std::endl;
    
    /*
     * XYZ offset of first slice (top left) in montage grid from the selected
     * slice coordinates (XYZ in toolbar)
     */
    const Vector3D firstSliceOffsetXYZ(montageSliceCoordStepXYZ * firstSliceIndexOffset);
    if (m_debugFlag) std::cout << "   First slice offset: " << firstSliceOffsetXYZ.toString() << std::endl;
    
    /*
     * XYZ of first slice (top left) in montage grid
     */
    Vector3D sliceXYZ(selectedXYZ + firstSliceOffsetXYZ);
    
    EventDrawingViewportContentAdd addModelViewportEvent;
    addModelViewportEvent.addModelVolumeGrid(windowIndex,
                                            m_tabIndex,
                                            GraphicsViewport(viewport),
                                            numRows,
                                            numCols);
    EventManager::get()->sendEvent(addModelViewportEvent.getPointer());

    std::unique_ptr<EventBrowserWindowContent> windowContentEvent = EventBrowserWindowContent::getWindowContent(m_fixedPipelineDrawing->m_windowIndex);
    EventManager::get()->sendEvent(windowContentEvent->getPointer());
    const BrowserWindowContent* windowContent = windowContentEvent->getBrowserWindowContent();
    const SamplesDrawingSettings* samplesSettings(windowContent->getSamplesDrawingSettings());
    
    const bool flipFlag = m_browserTabContent->isVolumeMontageSliceOrderFlippedForSliceViewPlane(sliceViewPlane);

    /*
     * When "middle/center" slice is drawn, need to update
     * the graphics object to window transform
     */
    const int32_t numSlices(numRows * numCols);
    const int32_t midSliceNumber((numSlices > 1)
                                 ? (numSlices / 2)
                                 : 1);
    int32_t sliceCounter(1);
    for (int32_t i1 = 0; i1 < numRows; i1++) {
        int32_t i(i1);
        if (flipFlag) {
            i = numRows - 1 - i;
        }
        for (int32_t j1 = 0; j1 < numCols; j1++) {
            int32_t j(j1);
            if (flipFlag) {
                j = numCols - 1 - j;
            }
            const int32_t vpX = (j * (vpSizeX + horizontalMargin));
            const int32_t vpY = ((numRows - i - 1) * (vpSizeY + verticalMargin));
            
            const GraphicsViewport vp(viewport.getX() + vpX,
                                      viewport.getY() + vpY,
                                      vpSizeX,
                                      vpSizeY);
            if ( ! vp.isValid()) {
                continue;
            }
            
            if (m_debugFlag) std::cout << "   First montage slice coords: " << sliceXYZ.toString() << std::endl;
            int64_t voxelIJK[3];
            underlayVolume->enclosingVoxel(sliceXYZ[0], sliceXYZ[1], sliceXYZ[2],
                                           voxelIJK[0], voxelIJK[1], voxelIJK[2]);
            if (m_debugFlag) std::cout << "   Slice indices: " << voxelIJK[0] << ", "
                << voxelIJK[1] << ", " << voxelIJK[2] << std::endl;
            if (underlayVolume->indexValid(voxelIJK[0], voxelIJK[1], voxelIJK[2])) {
                const bool updateGraphicsObjectToWindowTransformFlag(sliceCounter == midSliceNumber);
                drawVolumeSliceViewProjection(viewportContent,
                                              sliceProjectionType,
                                              sliceDrawingType,
                                              sliceViewPlane,
                                              sliceXYZ,
                                              vp,
                                              GridInfo(numRows, numCols, i, j),
                                              updateGraphicsObjectToWindowTransformFlag);
                
                /*
                 * Draw coordinates on slice
                 */
                const float offsetDistance(mprSliceView.getMontageTopLeftSliceDirectionPlane().signedDistanceToPlane(sliceXYZ));
                BrainOpenGLVolumeSliceDrawing::drawMontageSliceCoordinates(m_fixedPipelineDrawing,
                                                                           m_browserTabContent,
                                                                           sliceViewPlane,
                                                                           vp.toArrayInt4(),
                                                                           sliceXYZ,
                                                                           offsetDistance);
                CaretAssert(m_browserTabContent);
                m_browserTabContent->addMprThreeMontageViewport(GraphicsViewport(vp));
            }

            /*
             * Move 'down' along axis
             */
            sliceXYZ -= montageSliceCoordStepXYZ;
            
            ++sliceCounter;
            
            if (m_fixedPipelineDrawing->m_windowUserInputMode == UserInputModeEnum::Enum::SAMPLES_EDITING) {
                if ( ! samplesSettings->isSliceInLowerUpperOffsetRange(m_tabIndex,
                                                                       i, j)) {
                    const uint8_t rgba[4] { 255, 0, 0, 255 };
                    const float percentageThickness(3.0);
                    GraphicsShape::drawViewportCrossPercentageLineWidth(rgba,
                                                                        percentageThickness);
                }
            }
        }
    }
}

/**
 * Set the stereotaxic and texture coordinates for the given primitive
 * @param mprSliceView
 *    Info about the slice
 * @param volume
 *    The volume
 * @param primtive
 *    The graphics primitive
 * @return
 *    True if the primitive is valid for drawing
 */
bool
BrainOpenGLVolumeMprThreeDrawing::setPrimitiveCoordinates(const VolumeMprVirtualSliceView& mprSliceView,
                                                          const VolumeMappableInterface* volume,
                                                          GraphicsPrimitiveV3fT3f* primitive)
{
    bool validFlag(false);
    if (primitive != NULL) {
        if (primitive->getNumberOfVertices() == 18) {
            /*
             * Intersection produces 6 triangles (18 vertices)
             * Some may be degenerate
             */
            CaretAssert(volume);
            
            std::vector<Vector3D> stereotaxicXYZ;
            std::vector<Vector3D> textureStr;
            Plane layersDrawingPlaneUnused;
            validFlag = mprSliceView.getTrianglesCoordinates(volume,
                                                             stereotaxicXYZ,
                                                             textureStr,
                                                             layersDrawingPlaneUnused);
            
            if (validFlag) {
                const int32_t numVertices(stereotaxicXYZ.size());
                if (numVertices == primitive->getNumberOfVertices()) {
                    for (int32_t i = 0; i < numVertices; i++) {
                        primitive->replaceVertexFloatXYZ(i, stereotaxicXYZ[i]);
                        primitive->replaceVertexTextureSTR(i, textureStr[i]);
                    }
                }
                else {
                    validFlag = false;
                    CaretAssertMessage(0, ("MPR slice has incorrect number of vertices="
                                           + AString::number(numVertices)));
                }
            }
            else {
                /*
                 * No intersection, create degenerate triangles
                 */
                for (int32_t i = 0; i < primitive->getNumberOfVertices(); i++) {
                    Vector3D zeros(0.0, 0.0, 0.0);
                    primitive->replaceVertexFloatXYZ(i, zeros);
                    primitive->replaceVertexTextureSTR(i, zeros);
                }
            }
        }
        else {
            CaretAssertMessage(0, ("Primitive vertices should be 18 (triangles) or 8 (fan) but is "
                                   + AString::number(primitive->getNumberOfVertices())));
        }
    }
    return validFlag;
}

/**
 * Setup blending for the given blending mode and enables blending.  Alpha test
 * may also be enabled.
 *
 * Note: It best to call     glPushAttrib(GL_COLOR_BUFFER_BIT) prior to calling this method
 * and glPopAttrib() afterwards.
 *
 * @param blendingMode
 *    The blending mode
 * @param alphaValue
 *    The alpha value.
 * @param averageNumberOfSlices
 *    Number of slices for average mode
 */
void
BrainOpenGLVolumeMprThreeDrawing::setupMprBlending(const BlendingMode blendingMode,
                                                   const float alphaValue,
                                                   const int32_t averageNumberOfSlices) const
{
    switch (blendingMode) {
        case BlendingMode::ALL_VIEW:
        {
            CaretAssert((alphaValue >= 0.0)
                        && (alphaValue <= 1.0));
            
            /*
             * The constant alpha comes from the overlay.
             * The layer being drawn gets (RGB * alphaValue)
             * and current frame buffer gets (FrameRGB * (1 - alphaValue)
             */
            glBlendColor(alphaValue, alphaValue, alphaValue, 1.0);
            glBlendFuncSeparate(GL_CONSTANT_COLOR,           /* source (incoming) RGB blending factor */
                                GL_ONE_MINUS_CONSTANT_COLOR, /* destination (frame buffer) RGB blending factor */
                                GL_ZERO,                /* source (incoming) Alpha blending factor */
                                GL_ONE);                /* destination (frame buffer) Alpha blending factor */
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
            break;
        case BlendingMode::AVERAGE:
        {
            CaretAssert(averageNumberOfSlices > 0);
            
            /*
             * Each slice contributes equally
             * From OpenGL RedBook, section "Sample Uses of Blending" bullet:
             *
             * To blend three different images equally, set the destination factor to GL_ONE and
             * the source factor to GL_SRC_ALPHA.  Draw each of the images with alpha equal to
             * 0.3333.  With this technique, each image is only one third of its original brightness,
             * which is noticeable where the images don't overlap.
             *
             * Using (alpha == (1/numSlices) allows each slice to contribute
             * equally and thus approximates "an average".
             */
            const float alpha(1.0 / static_cast<float>(averageNumberOfSlices));
            glBlendColor(0.0, 0.0, 0.0, alpha);
            
            glBlendFuncSeparate(GL_CONSTANT_ALPHA_EXT,
                                GL_ONE,
                                GL_ZERO,
                                GL_ONE);
            glEnable(GL_BLEND);
            
            glDisable(GL_ALPHA_TEST);
            glAlphaFunc(GL_ALWAYS, 1.0);
            glEnable(GL_ALPHA_TEST);
        }
            break;
        case BlendingMode::BACKGROUND_INTENSITY_AVERAGE:
        {
            glAlphaFunc(GL_ALWAYS, 0.0);
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_BLEND);
        }
            break;
        case BlendingMode::BACKGROUND_MINIMUM_INTENSITY_SLICE:
        {
            glAlphaFunc(GL_ALWAYS, 0.0);
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_BLEND);
        }
            break;
        case BlendingMode::BACKGROUND_MAXIMUM_INTENSITY_SLICE:
        {
            glAlphaFunc(GL_ALWAYS, 0.0);
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_BLEND);
        }
            break;
        case BlendingMode::INTENSITY_MAXIMUM:
        {
            glBlendEquationSeparate(GL_MAX, GL_MAX);
            
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_BLEND);
            
            glAlphaFunc(GL_GEQUAL, 0.95);
            glEnable(GL_ALPHA_TEST);
        }
            break;
        case BlendingMode::INTENSITY_MINIMUM:
        {
            glBlendEquationSeparate(GL_MIN, GL_MIN);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_BLEND);
            
            glAlphaFunc(GL_GEQUAL, 0.95);
            glEnable(GL_ALPHA_TEST);
            
        }
            break;
        case BlendingMode::MPR_OVERLAY_SLICE:
        {
            CaretAssert((alphaValue >= 0.0)
                        && (alphaValue <= 1.0));
            
            /*
             * The constant alpha comes from the overlay.
             * The layer being drawn gets (RGB * alphaValue)
             * and current frame buffer gets (FrameRGB * (1 - alphaValue)
             */
            glBlendColor(alphaValue, alphaValue, alphaValue, 1.0);
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
            break;
        case BlendingMode::MPR_UNDERLAY_SLICE:
        {
            /*
             * Prevents "white line" along some edges
             * while still allowing blending from additional layers
             */
            glBlendFuncSeparate(GL_ONE,   /* source (incoming) RGB blending factor */
                                GL_ZERO,  /* destination (frame buffer) RGB blending factor */
                                GL_ONE,   /* source (incoming) Alpha blending factor */
                                GL_ONE); /* destination (frame buffer) Alpha blending factor */
            glEnable(GL_BLEND);
        }
            break;
    }
}
