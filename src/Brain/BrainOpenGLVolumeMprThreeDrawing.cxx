
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
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"
#include "CaretPreferences.h"
#include "CiftiMappableDataFile.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesVolume.h"
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
#include "SelectionItemAnnotation.h"
#include "SelectionItemVoxel.h"
#include "SelectionItemVoxelEditing.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "VolumeFile.h"

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
                                     std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                     const GraphicsViewport& viewport)
{
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    CaretAssert(m_fixedPipelineDrawing);
    
    m_browserTabContent = browserTabContent;
    CaretAssert(m_browserTabContent);
    
    m_volumeDrawInfo = volumeDrawInfo;
    const int32_t numberOfVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    if (numberOfVolumes <= 0) {
        return;
    }
    CaretAssert( ! m_volumeDrawInfo.empty());

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

    /*
     * No lighting for drawing slices
     */
    m_fixedPipelineDrawing->disableLighting();

    m_mprViewMode = browserTabContent->getVolumeMprViewMode();
    m_orientationMode = browserTabContent->getVolumeMprOrientationMode();
    
    const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType(m_browserTabContent->getVolumeSliceProjectionType());
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            CaretAssert(0);
            break;
    }
    
    const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType = m_browserTabContent->getVolumeSliceDrawingType();

    m_axialCoronalParaSliceViewFlag = false;

    if (browserTabContent->isWholeBrainDisplayed()) {
        drawWholeBrainView(viewportContent,
                           sliceDrawingType,
                           sliceProjectionType,
                           viewport);
    }
    else if (browserTabContent->isVolumeSlicesDisplayed()) {
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
                    drawAllViewRotationAxes(browserTabContent,
                                            underlayVolume,
                                            axisVP.data());
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
 * Draw a box showing the current rotation
 * @param browserTabContent
 *   Content of browser tab
 * @param underlayVolume
 *   The underlay volume
 * @param viewport
 *   The viewport
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawAllViewRotationAxes(const BrowserTabContent* browserTabContent,
                                                          const VolumeMappableInterface* underlayVolume,
                                                          const int32_t viewport[4])
{
    CaretAssert(browserTabContent);
    CaretAssert(underlayVolume);
    
    /*
     * Set the modeling transformation
     */
    Matrix4x4 matrix(m_browserTabContent->getMprThreeRotationMatrix());
    if ( ! matrix.invert()) {
        CaretLogSevere("Failed to invert MPR Rotation Matrix for Axis Drawing");
        return;
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
                                                              const bool updateGraphicsObjectToWindowTransformFlag)
{
    
    glMatrixMode(GL_MODELVIEW);
    
    
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::ALL_3D:
            break;
        case BrainModelMode::VOLUME_2D:
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
    SliceInfo sliceInfo(createSliceInfo(m_browserTabContent,
                                        underlayVolume,
                                        sliceViewPlane,
                                        sliceCoordinates));

    if ( ! sliceInfo.m_mprSliceView.getPlane().isValidPlane()) {
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
            setViewingTransformation(underlayVolume,
                                     sliceViewPlane,
                                     sliceInfo.m_mprSliceView,
                                     sliceCoordinates);
            break;
    }

    SelectionItemAnnotation* annotationID(m_brain->getSelectionManager()->getAnnotationIdentification());
    SelectionItemVolumeMprCrosshair* crosshairID(m_brain->getSelectionManager()->getVolumeMprCrosshairIdentification());
    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    SelectionItemVoxelEditing* voxelEditingID = m_brain->getSelectionManager()->getVoxelEditingIdentification();

    m_fixedPipelineDrawing->applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_VOLUME,
                                                StructureEnum::ALL);

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

    if (drawVolumeSlicesFlag) {
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

        bool drawIdentificationSymbolsFlag(false);
        if (intensityModeFlag) {
            drawSliceIntensityProjection2D(sliceInfo.m_mprSliceView,
                                           sliceViewPlane,
                                           sliceCoordinates,
                                           viewport);
        }
        else {
            const bool enableBlendingFlag(true);
            const bool drawAttributesFlag(true);
            const bool drawIntensitySliceBackgroundFlag(false);
            drawSliceWithPrimitive(sliceInfo.m_mprSliceView,
                                   sliceViewPlane,
                                   sliceCoordinates,
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
            }
            
            CaretAssertVectorIndex(m_volumeDrawInfo, 0);
            
            if (drawIdentificationSymbolsFlag) {
                BrainOpenGLVolumeSliceDrawing::drawIdentificationSymbols(m_fixedPipelineDrawing,
                                                                         m_browserTabContent,
                                                                         m_volumeDrawInfo[0].volumeFile,
                                                                         m_volumeDrawInfo[0].mapIndex,
                                                                         sliceInfo.m_mprSliceView.getPlane(),
                                                                         sliceThickness);
            }
        }
        
        const Plane slicePlane(sliceInfo.m_mprSliceView.getPlane());
        
        if (slicePlane.isValidPlane()) {
            drawLayers(underlayVolume,
                       sliceProjectionType,
                       sliceViewPlane,
                       slicePlane,
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
        const float doubleSliceThickness(sliceThickness * 2.0);
        m_fixedPipelineDrawing->m_annotationDrawing->drawModelSpaceAnnotationsOnVolumeSlice(&inputs,
                                                                                            slicePlane,
                                                                                            doubleSliceThickness);
        
        bool drawSelectionBoxFlag(false);
        switch (sliceDrawingType) {
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
                break;
            case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
                drawSelectionBoxFlag = true;
                break;
        }
        if (drawSelectionBoxFlag) {
            GraphicsRegionSelectionBox::DrawMode drawMode(GraphicsRegionSelectionBox::DrawMode::Z_PLANE);
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    drawMode = GraphicsRegionSelectionBox::DrawMode::Z_PLANE;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    drawMode = GraphicsRegionSelectionBox::DrawMode::Y_PLANE;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    drawMode = GraphicsRegionSelectionBox::DrawMode::X_PLANE;
                    break;
            }
            BrainOpenGLFixedPipeline::drawGraphicsRegionSelectionBox(m_browserTabContent->getRegionSelectionBox(),
                                                                     drawMode,
                                                                     m_fixedPipelineDrawing->m_foregroundColorFloat);
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

static void
multiplyPoint(const Matrix4x4& m,
              const Vector3D& centerXYZ,
              const Vector3D& selectedXYZ,
              Vector3D& xyz)
{
    Vector3D offsetXYZ;
    offsetXYZ = selectedXYZ;

    Vector3D p(xyz);
    
    Matrix4x4 rot;
    rot.translate(offsetXYZ);
    rot.postmultiply(m);
    rot.translate(-offsetXYZ);
    
    rot.multiplyPoint3(p);
    
    xyz = p;
}

/**
 * Create information about the slice being drawn
 * @param browserTabContent
 *    Content of the browser tab
 * @param underlayVolume
 *    The underlay volume
 * @param sliceViewPlane
 *    Plane being viewed
 * @param sliceCoordinates
 *    Coordinates of selected slices
 */
BrainOpenGLVolumeMprThreeDrawing::SliceInfo
BrainOpenGLVolumeMprThreeDrawing::createSliceInfo(const BrowserTabContent* browserTabContent,
                                                const VolumeMappableInterface* underlayVolume,
                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                const Vector3D& sliceCoordinates) const
{
    SliceInfo sliceInfo;
    
    CaretAssert(underlayVolume);
    BoundingBox boundingBox;
    underlayVolume->getVoxelSpaceBoundingBox(boundingBox);
        
    /*
     * Set the modeling transformation
     */
    const Matrix4x4 rotationMatrix(m_browserTabContent->getMprThreeRotationMatrix());
    
    Vector3D volumeCenterXYZ;
    boundingBox.getCenter(volumeCenterXYZ);
       
    sliceInfo.m_mprSliceView = MprVirtualSliceView(volumeCenterXYZ,
                                                   sliceCoordinates,
                                                   boundingBox.getMaximumDifferenceOfXYZ(),
                                                   sliceViewPlane,
                                                   m_orientationMode,
                                                   rotationMatrix);

    return sliceInfo;
}

/**
 * Add a segment to the crosshairs
 * @param primitiveSliceCrosshair
 *    Primitive for drawing crosshair sections for slice selection
 * @param primitiveRotateCrosshair
 *    Primitive for drawing crosshair sections for rotation
 * @param xStart
 *    Starting x-coordinate at the crosshair intersection
 * @param yStart
 *    Starting y-coordinate at the crosshair intersection
 * @param xEnd
 *    Ending y-coordinate at the edge of viewport
 * @param yEnd
 *    Ending  y-coordinate at the edge of viewport
 * @param rgba
 *    Coloring for the lines
 * @param gapLengthPixels
 *    Length of crosshair gap in pixels
 * @param sliceSelectionIndices
 *    Tracks indices of slice selection indices
 * @param rotateSelectionIndices
 *    Tracks indices of slice rotate indices
 * @param sliceAxisID
 *    Identification of slice axis selection
 * @param rotationAxisID
 *    Identification of rotation axis selection
 */
void
BrainOpenGLVolumeMprThreeDrawing::addCrosshairSection(GraphicsPrimitiveV3fC4ub* primitiveSliceCrosshair,
                                                 GraphicsPrimitiveV3fC4ub* primitiveRotateCrosshair,
                                                 const float xStart,
                                                 const float yStart,
                                                 const float xEnd,
                                                 const float yEnd,
                                                 const std::array<uint8_t, 4>& rgba,
                                                 const float gapLengthPixels,
                                                 std::vector<SelectionItemVolumeMprCrosshair::Axis>& sliceSelectionIndices,
                                                 std::vector<SelectionItemVolumeMprCrosshair::Axis>& rotateSelectionIndices,
                                                 const SelectionItemVolumeMprCrosshair::Axis sliceAxisID,
                                                 const SelectionItemVolumeMprCrosshair::Axis rotationAxisID)
{
    /*
     * Note: There are two sections:
     * (1) Starts at intersection (or edge of gap if there is a gap) to the
     * midpoint of the line and is dragged to select slice indices for
     * other slice plane views.
     * (2) Starts at mid point and extends to the end point.  Is used
     * to rotate the other slice plane views.
     */
    float offsetX(0.0);
    float offsetY(0.0);
    const float dx(xEnd - xStart);
    const float dy(yEnd - yStart);
    const float len(std::sqrt(dx*dx + dy*dy));
    const float gapLen(gapLengthPixels / 2.0);
    if (gapLen > 0.0) {
        /*
         * Move the starting x/y so that there is a gap at the crosshair intersection
         */
        if (gapLen < len) {
            const float pct(gapLen / len);
            offsetX = dx * pct;
            offsetY = dy * pct;
        }
        else {
            /*
             * Gaps is longer than line
             */
            return;
        }
    }
    
    const float xMid((xStart + xEnd) / 2.0);
    const float yMid((yStart + yEnd) / 2.0);
    const float dxMid(xMid - xStart);
    const float dyMid(yMid - yStart);
    const float lenMid(std::sqrt(dxMid*dxMid + dyMid*dyMid));
    
    const float z(0.0);
    
    /*
     * If distance from intersetion to gap is less than distance to mid-point
     */
    if (lenMid > gapLen) {
        sliceSelectionIndices.push_back(sliceAxisID);
        primitiveSliceCrosshair->addVertex(xStart + offsetX, yStart + offsetY, z, rgba.data());
        primitiveSliceCrosshair->addVertex(xMid, yMid, z, rgba.data());
        
        rotateSelectionIndices.push_back(rotationAxisID);
        primitiveRotateCrosshair->addVertex(xMid, yMid, z, rgba.data());
        primitiveRotateCrosshair->addVertex(xEnd, yEnd, z, rgba.data());
    }
    else {
        rotateSelectionIndices.push_back(rotationAxisID);
        primitiveRotateCrosshair->addVertex(xStart + offsetX, yStart + offsetY, z, rgba.data());
        primitiveRotateCrosshair->addVertex(xEnd, yEnd, z, rgba.data());
    }
}


/**
 * Draw the panning crosshairs for slice
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param crossHairXYZ
 *    Intersection of crosshairs
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawPanningCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                      const Vector3D& crossHairXYZ,
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

    bool radiologicalFlag(false);
    switch (m_orientationMode) {
        case VolumeMprOrientationModeEnum::NEUROLOGICAL:
            radiologicalFlag = false;
            break;
        case VolumeMprOrientationModeEnum::RADIOLOGICAL:
            radiologicalFlag = true;
            break;
    }
    
    const float percentViewportHeight(0.5);
    const float gapPercentViewportHeight = SessionManager::get()->getCaretPreferences()->getVolumeCrosshairGap();
    const float gapLengthPixels((gapPercentViewportHeight / 100.0)
                                * viewport.getHeightF());
    
    const float crossHairX(0.0);
    const float crossHairY(0.0);
    
    std::unique_ptr<GraphicsPrimitiveV3fC4ub> sliceSelectionPrimitive(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::OPENGL_LINES));
    const float sliceLineWidth(m_identificationModeFlag
                               ? (percentViewportHeight * 5.0)
                               : percentViewportHeight);
    sliceSelectionPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                          sliceLineWidth);
    
    std::unique_ptr<GraphicsPrimitiveV3fC4ub> rotatePrimitive(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::OPENGL_LINES));
    const float rotateThicker(m_axialCoronalParaSliceViewFlag
                              ? 2.0
                              : 1.0);
    const float rotateLineWidth(m_identificationModeFlag
                                ? (percentViewportHeight * 5.0)
                                : percentViewportHeight * rotateThicker);
    rotatePrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                  rotateLineWidth);
    
    const float circleRadius(std::min(viewport.getWidthF(), viewport.getHeightF())/ 2.0);
    const float vpMinX( - circleRadius);
    const float vpMaxX(circleRadius);
    const float vpMinY(- circleRadius);
    const float vpMaxY(circleRadius);

    
    const std::array<uint8_t, 4> axialPlaneRGBA(getAxisColor(VolumeSliceViewPlaneEnum::AXIAL));
    const std::array<uint8_t, 4> coronalPlaneRGBA(getAxisColor(VolumeSliceViewPlaneEnum::CORONAL));
    const std::array<uint8_t, 4> parasagittalPlaneRGBA(getAxisColor(VolumeSliceViewPlaneEnum::PARASAGITTAL));
    
    std::vector<SelectionItemVolumeMprCrosshair::Axis> sliceSelectionIndices;
    std::vector<SelectionItemVolumeMprCrosshair::Axis> rotateSelectionIndices;

    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            /* Bottom edge (-y) to selected coordinate */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                crossHairX, vpMinY,
                                parasagittalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::POSTERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::POSTERIOR_ROTATE);
            
            /* Selected coordinate to top edge (+y) */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                crossHairX, vpMaxY,
                                parasagittalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::ANTERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::ANTERIOR_ROTATE);
            
            /* Left edge (-x) to selected coordinate */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                vpMinX, crossHairY,
                                coronalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::LEFT_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::LEFT_ROTATE);
            
            /* Selected coordinate to right edge (+x) */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                vpMaxX, crossHairY,
                                coronalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::RIGHT_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::RIGHT_ROTATE);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            /* Bottom edge (-z) to selected coordinate */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                crossHairX, vpMinY,
                                parasagittalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::INFERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::INFERIOR_ROTATE);
            
            /* Selected coordinate to top edge (+z) */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                crossHairX, vpMaxY,
                                parasagittalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::SUPERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::SUPERIOR_ROTATE);
            
            /* Left edge (-x) to selected coordinate */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                vpMinX, crossHairY,
                                axialPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::LEFT_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::LEFT_ROTATE);
            
            /* Selected coordinate to right edge (+x) */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                vpMaxX, crossHairY,
                                axialPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::RIGHT_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::RIGHT_ROTATE);
            
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            /* Bottom edge (-z) to selected coordinate */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                crossHairX, vpMinY,
                                coronalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::INFERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::INFERIOR_ROTATE);
            
            /* Selected coordinate to top edge (+z) */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                crossHairX, vpMaxY,
                                coronalPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::SUPERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::SUPERIOR_ROTATE);
            
            /* Left edge (+y) to selected coordinate */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                vpMinX, crossHairY,
                                axialPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::ANTERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::ANTERIOR_ROTATE);
            
            /* Selected coordinate to right edge (-y) */
            addCrosshairSection(sliceSelectionPrimitive.get(),
                                rotatePrimitive.get(),
                                crossHairX, crossHairY,
                                vpMaxX, crossHairY,
                                axialPlaneRGBA,
                                gapLengthPixels,
                                sliceSelectionIndices,
                                rotateSelectionIndices,
                                SelectionItemVolumeMprCrosshair::Axis::POSTERIOR_SLICE,
                                SelectionItemVolumeMprCrosshair::Axis::POSTERIOR_ROTATE);
            break;
    }
    
    glPushMatrix();    
    glTranslatef(crossHairXYZ[0], crossHairXYZ[1], crossHairXYZ[2]);

    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            if (radiologicalFlag) {
                glRotatef(-m_browserTabContent->getMprRotationZ(), 0.0, 0.0, 1.0);
            }
            else {
                glRotatef(m_browserTabContent->getMprRotationZ(), 0.0, 0.0, 1.0);
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            if (radiologicalFlag) {
                glRotatef(-m_browserTabContent->getMprRotationY(), 0.0, 0.0, 1.0);
            }
            else {
                glRotatef(m_browserTabContent->getMprRotationY(), 0.0, 0.0, 1.0);
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            glRotatef(m_browserTabContent->getMprRotationX(), 0.0, 0.0, 1.0);
            break;
    }

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
        GraphicsEngineDataOpenGL::drawWithSelection(rotatePrimitive.get(),
                                                    this->m_fixedPipelineDrawing->mouseX,
                                                    this->m_fixedPipelineDrawing->mouseY,
                                                    primitiveIndex,
                                                    primitiveDepth);
        if ((primitiveIndex >= 0)
            && (primitiveIndex < static_cast<int32_t>(rotateSelectionIndices.size()))) {
            crosshairID->setIdentification(m_brain,
                                           rotateSelectionIndices[primitiveIndex],
                                           primitiveDepth);
        }
    }
    else {
        GraphicsEngineDataOpenGL::draw(sliceSelectionPrimitive.get());
        GraphicsEngineDataOpenGL::draw(rotatePrimitive.get());
    }
    
    glPopMatrix();
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
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
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
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(vpMinX, vpMaxX,
            vpMinY, vpMaxY,
            -100.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* start */
    drawPanningCrosshairs(sliceViewPlane,
                          crossHairXYZ,
                          viewport);
    
    drawAxisLabels(sliceViewPlane,
                   viewport);
    
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

/**
 * Draw the axis labels for slice
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawAxisLabels(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
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
    
    AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
    annotationText.setBoldStyleEnabled(true);
    annotationText.setFontPercentViewportSize(5.0f);
    annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
    annotationText.setTextColor(CaretColorEnum::CUSTOM);
    annotationText.setCustomTextColor(horizontalAxisRGBA.data());
    annotationText.setCustomBackgroundColor(backgroundRGBA.data());
    
    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
    annotationText.setText(westText);
    m_fixedPipelineDrawing->drawTextAtViewportCoords(textLeftWindowXY[0],
                                                     textLeftWindowXY[1],
                                                     annotationText);
    
    annotationText.setText(eastText);
    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
    m_fixedPipelineDrawing->drawTextAtViewportCoords(textRightWindowXY[0],
                                                     textRightWindowXY[1],
                                                     annotationText);
    
    annotationText.setCustomTextColor(verticalAxisRGBA.data());
    
    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
    annotationText.setText(southText);
    m_fixedPipelineDrawing->drawTextAtViewportCoords(textBottomWindowXY[0],
                                                     textBottomWindowXY[1],
                                                     annotationText);
    
    annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
    annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
    annotationText.setText(northText);
    m_fixedPipelineDrawing->drawTextAtViewportCoords(textTopWindowXY[0],
                                                     textTopWindowXY[1],
                                                     annotationText);
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
 * @param volume
 *    Volume being drawn
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param mprSliceView
 *    Info about virtual slice.
 * @param selectedSliceXYZ
 *    Selected slice coordiantes
 */
void
BrainOpenGLVolumeMprThreeDrawing::setViewingTransformation(const VolumeMappableInterface* volume,
                                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                           const MprVirtualSliceView& mprSliceView,
                                                           const Vector3D& selectedSliceXYZ)
{
    /*
     * Initialize the modelview matrix to the identity matrix
     * This places the camera at the origin, pointing down the
     * negative-Z axis with the up vector set to (0,1,0 =>
     * positive-Y is up).
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Vector3D translation;
    m_browserTabContent->getTranslation(translation);
    float tx(0.0), ty(0.0), tz(0.0);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            tx = translation[0];
            ty = translation[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            tx = translation[0];
            ty = translation[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            tx = translation[1];
            ty = translation[2];
            break;
    }
    glTranslatef(tx, ty, tz);

/*
 *
 switch (sliceViewPlane) {
 case VolumeSliceViewPlaneEnum::ALL:
 CaretAssert(0);
 break;
 case VolumeSliceViewPlaneEnum::AXIAL:
 offsetHoriz = leftToRightVector * (-translation[0]);
 offsetVert  = bottomToTopVector * (-translation[1]);
 break;
 case VolumeSliceViewPlaneEnum::CORONAL:
 offsetHoriz = leftToRightVector * (-translation[0]);
 offsetVert  = bottomToTopVector * (-translation[2]);
 break;
 case VolumeSliceViewPlaneEnum::PARASAGITTAL:
 offsetHoriz = leftToRightVector * (-translation[1]);
 offsetVert  = bottomToTopVector * (-translation[2]);
 break;
 }

 */
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
    
//    Vector3D translation;
//    m_browserTabContent->getTranslation(translation);
//    switch (sliceViewPlane) {
//        case VolumeSliceViewPlaneEnum::ALL:
//            break;
//        case VolumeSliceViewPlaneEnum::AXIAL:
//            translation[2] = 0.0;
//            break;
//        case VolumeSliceViewPlaneEnum::CORONAL:
//            translation[1] = 0.0;
//            break;
//        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
//            translation[0] = 0.0;
//            break;
//    }
//    glTranslatef(translation[0], translation[1], translation[2]);
}

/**
 * Does the ray intersect the rectangle (volume side) defined by the four points
 * @param volume
 *    The volume
 * @param aIJK
 *    First coordinate of rectangle
 * @param bIJK
 *    Second coordinate of rectangle
 * @param cIJK
 *    Third coordinate of rectangle
 * @param dIJK
 *    Fourth coordinate of rectangle
 * @param rayOrigin
 *    Origin of the ray
 * @param rayVector
 *    Direction of the ray
 * @param sideName
 *    Name of the side of the volume
 * @param intersectionXYZOut
 *    Output with where ray intersects the side
 * @rerturn
 *    True if the ray intersects the side, else false
 */
bool
BrainOpenGLVolumeMprThreeDrawing::getVolumeSideIntersection(const VolumeMappableInterface* volume,
                                                          const int64_t aIJK[],
                                                          const int64_t bIJK[],
                                                          const int64_t cIJK[],
                                                          const int64_t dIJK[],
                                                          const float rayOrigin[3],
                                                          const float rayVector[3],
                                                          const AString& sideName,
                                                          Vector3D& intersectionXYZOut) const
{
    Vector3D aXYZ, bXYZ, cXYZ, dXYZ;
    volume->indexToSpace(aIJK, aXYZ);
    volume->indexToSpace(bIJK, bXYZ);
    volume->indexToSpace(cIJK, cXYZ);
    volume->indexToSpace(dIJK, dXYZ);
    
    float xyzAndDistance[4];
    Plane plane(aXYZ, bXYZ, cXYZ);
    if (plane.rayIntersection(rayOrigin,
                              rayVector,
                              xyzAndDistance)) {
        intersectionXYZOut[0] = xyzAndDistance[0];
        intersectionXYZOut[1] = xyzAndDistance[1];
        intersectionXYZOut[2] = xyzAndDistance[2];
        
        bool insideFlag(false);
        
        const float degenerateTolerance(0.001);
        {
            /*
             * Is point in triangle (half of side)?
             * Use barycentric areas.  Orientation of triangle is unknown so if point
             * is inside triangle all areas will be either negative or positive.
             */
            const float area1 = MathFunctions::triangleAreaSigned3D(rayVector, aXYZ, bXYZ, intersectionXYZOut);
            const float area2 = MathFunctions::triangleAreaSigned3D(rayVector, bXYZ, cXYZ, intersectionXYZOut);
            const float area3 = MathFunctions::triangleAreaSigned3D(rayVector, cXYZ, aXYZ, intersectionXYZOut);
            if ((area1 > -degenerateTolerance)
                && (area2 > -degenerateTolerance)
                && (area3 > -degenerateTolerance)) {
                insideFlag = true;
            }
            if ((area1 < degenerateTolerance)
                && (area2 < degenerateTolerance)
                && (area3 < degenerateTolerance)) {
                insideFlag = true;
            }
        }

        if ( ! insideFlag) {
            const float area1 = MathFunctions::triangleAreaSigned3D(rayVector, aXYZ, cXYZ, intersectionXYZOut);
            const float area2 = MathFunctions::triangleAreaSigned3D(rayVector, cXYZ, dXYZ, intersectionXYZOut);
            const float area3 = MathFunctions::triangleAreaSigned3D(rayVector, dXYZ, aXYZ, intersectionXYZOut);
            if ((area1 > -degenerateTolerance)
                && (area2 > -degenerateTolerance)
                && (area3 > -degenerateTolerance)) {
                insideFlag = true;
            }
            if ((area1 < degenerateTolerance)
                && (area2 < degenerateTolerance)
                && (area3 < degenerateTolerance)) {
                insideFlag = true;
            }
        }
        
        if (insideFlag) {
            if (m_debugFlag) {
                std::cout << "Intersection " << sideName << ": ("
                << AString::fromNumbers(aXYZ) << ") ("
                << AString::fromNumbers(bXYZ) << ") ("
                << AString::fromNumbers(cXYZ) << ") ("
                << AString::fromNumbers(dXYZ) << ") -> "
                << AString::fromNumbers(intersectionXYZOut) << std::endl;
            }
            return true;
        }
    }
    
    return false;
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
    
    if (volume == NULL) {
        return allIntersections;
    }
    std::vector<int64_t> dims;
    volume->getDimensions(dims);
    if (dims.size() < 3) {
        return allIntersections;
    }
    CaretAssertVectorIndex(dims, 2);
    for (int32_t i = 0; i < 3; i++) {
        if (dims[i] < 1) {
            return allIntersections;
        }
    }
    
    const int64_t iMax(dims[0] - 1);
    const int64_t jMax(dims[1] - 1);
    const int64_t kMax(dims[2] - 1);
    
    
    const int64_t ijk[] { 0,    0,    0 };
    const int64_t Ijk[] { iMax, 0,    0 };
    const int64_t IJk[] { iMax, jMax, 0 };
    const int64_t iJk[] { 0,    jMax, 0 };
    
    const int64_t ijK[] { 0,    0,    kMax };
    const int64_t IjK[] { iMax, 0,    kMax };
    const int64_t IJK[] { iMax, jMax, kMax };
    const int64_t iJK[] { 0,    jMax, kMax };
    
    std::vector<AString> intersectionNames;
    Vector3D intersectionXYZ;
    
    /*
     * Bottom
     */
    if (getVolumeSideIntersection(volume,
                                  ijk, Ijk, IJk, iJk,
                                  rayOrigin,
                                  rayVector,
                                  "Bottom",
                                  intersectionXYZ)) {
        allIntersections.push_back(intersectionXYZ);
        intersectionNames.push_back("Bottom");
    }
    
    /*
     * Near
     */
    if (getVolumeSideIntersection(volume,
                                  ijk, Ijk, IjK, ijK,
                                  rayOrigin,
                                  rayVector,
                                  "Near",
                                  intersectionXYZ)) {
        allIntersections.push_back(intersectionXYZ);
        intersectionNames.push_back("Near");
    }
    
    /*
     * Far
     */
    if (getVolumeSideIntersection(volume,
                                  iJk, IJk, IJK, iJK,
                                  rayOrigin,
                                  rayVector,
                                  "Far",
                                  intersectionXYZ)) {
        allIntersections.push_back(intersectionXYZ);
        intersectionNames.push_back("Far");
    }
    
    /*
     * Right
     */
    if (getVolumeSideIntersection(volume,
                                  Ijk, IJk, IJK, IjK,
                                  rayOrigin,
                                  rayVector,
                                  "Right",
                                  intersectionXYZ)) {
        allIntersections.push_back(intersectionXYZ);
        intersectionNames.push_back("Right");
    }
    
    /*
     * Left
     */
    if (getVolumeSideIntersection(volume,
                                  ijk, iJk, iJK, ijK,
                                  rayOrigin,
                                  rayVector,
                                  "Left",
                                  intersectionXYZ)) {
        allIntersections.push_back(intersectionXYZ);
        intersectionNames.push_back("Left");
    }
    
    /*
     * Top
     */
    if (getVolumeSideIntersection(volume,
                                  ijK, IjK, IJK, iJK,
                                  rayOrigin,
                                  rayVector,
                                  "Top",
                                  intersectionXYZ)) {
        allIntersections.push_back(intersectionXYZ);
        intersectionNames.push_back("Top");
    }
    
    CaretAssert(allIntersections.size() == intersectionNames.size());

    return allIntersections;
}

/**
 * @return The intensity files and map indices for drawing
 */
std::vector<std::pair<VolumeMappableInterface*,int32_t>>
BrainOpenGLVolumeMprThreeDrawing::getIntensityVolumeFilesAndMapIndices() const
{
    std::vector<std::pair<VolumeMappableInterface*,int32_t>> filesAndMapIndices;
    for (auto& vdi : m_volumeDrawInfo) {
        if (vdi.volumeFile != NULL) {
            filesAndMapIndices.push_back(std::make_pair(vdi.volumeFile,
                                                     vdi.mapIndex));
        }
    }
    return filesAndMapIndices;
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
                    Vector3D pointOnLineXYZ;
                    MathFunctions::nearestPointOnLine3D(p1,
                                                        p2,
                                                        sliceCoordinates,
                                                        pointOnLineXYZ,
                                                        distanceToLine);
                    if (m_debugFlag) {
                        std::cout << "Point on line, distance: "
                        << AString::fromNumbers(pointOnLineXYZ)
                        << ",  "
                        << distanceToLine
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
BrainOpenGLVolumeMprThreeDrawing::drawSliceIntensityProjection2D(const MprVirtualSliceView& mprSliceView,
                                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                 const Vector3D& sliceCoordinates,
                                                                 const GraphicsViewport& viewport)
{
    std::vector<std::pair<VolumeMappableInterface*,int32_t>> intensityVolumeFiles(getIntensityVolumeFilesAndMapIndices());
    if (intensityVolumeFiles.empty()) {
        return;
    }
    
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
    
    for (auto& volumeFileAndMapIndex : intensityVolumeFiles) {
        VolumeMappableInterface* volumeFile(volumeFileAndMapIndex.first);
        CaretAssert(volumeFile);
        if (idModeFlag) {
            performIntensityIdentification(mprSliceView,
                                           volumeFile);
            continue;
        }
        
        std::vector<Vector3D> allIntersections(getVolumeRayIntersections(volumeFile,
                                                                         mprSliceView.getVolumeCenterXYZ(),
                                                                         //mprSliceView.getCameraLookAtXYZ(),
                                                                         mprSliceView.getNormalVector()));
        const int32_t numIntersections(allIntersections.size());
        
        if (numIntersections == 2) {
            applySliceThicknessToIntersections(sliceViewPlane,
                                               sliceCoordinates,
                                               allIntersections);
            
            const float voxelSize(getVoxelSize(volumeFile));
            if (voxelSize < 0.01) {
                CaretLogSevere("Voxel size is too small for Intensity Projection: "
                               + AString::number(voxelSize));
                continue;
            }
            
            CaretAssertVectorIndex(allIntersections, 1);
            const Vector3D p1(allIntersections[0]);
            const Vector3D p2(allIntersections[1]);
            const float distance = (p1 - p2).length();
            const Vector3D p1ToP2Vector((p2 - p1).normal());
            const Vector3D stepVector(p1ToP2Vector[0] * voxelSize,
                                      p1ToP2Vector[1] * voxelSize,
                                      p1ToP2Vector[2] * voxelSize);
            const float numSteps = distance / voxelSize;
            if (numSteps < 1) {
                CaretLogSevere("Invalid number of steps="
                               + AString::number(numSteps)
                               + " for intensity projection 2D");
                return;
            }
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
            
            setupIntensityModeBlending(numSteps);
            
            for (int32_t iStep = 0; iStep < numSteps; iStep++) {
                Vector3D sliceCoords(p1 + stepVector * iStep);
                const SliceInfo stepSliceInfo(createSliceInfo(m_browserTabContent,
                                                              volumeFile,
                                                              sliceViewPlane,
                                                              sliceCoords));
                if (m_debugFlag) {
                    if (iStep == 0) {
                        std::cout << "First slice: " << std::endl;
                        std::cout << stepSliceInfo.toString("   ") << std::endl;
                    }
                    else if (iStep == (numSteps - 1)) {
                        std::cout << "Last Slice: " << std::endl;
                        std::cout << stepSliceInfo.toString("   ") << std::endl;
                    }
                }
                                
                const bool enableBlendingFlag(false);
                const bool drawAttributesFlag(false);
                const bool drawIntensitySliceBackgroundFlag(iStep == 0);
                drawSliceWithPrimitive(stepSliceInfo.m_mprSliceView,
                                       sliceViewPlane,
                                       sliceCoords,
                                       viewport,
                                       enableBlendingFlag,
                                       drawAttributesFlag,
                                       drawIntensitySliceBackgroundFlag);
            }
            
            glPopAttrib();
        }
        else if (numIntersections > 0) {
            CaretLogSevere("Possible algorithm failure for Intensity Projection, intersection count="
                           + AString::number(numIntersections));
        }
    }
    
    CaretAssertVectorIndex(intensityVolumeFiles, 0);
    VolumeMappableInterface* volumeFile(intensityVolumeFiles[0].first);
    const int32_t mapIndex(intensityVolumeFiles[0].second);
    CaretAssert(volumeFile);
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
            drawCrosshairs(sliceViewPlane,
                           sliceCoordinates,
                           viewport);
            break;
    }
}

/**
 * Setup blending for the intensity modes (Average, Minimum, Maximum)
 * @param numSteps
 *    Number of "slices" that are drawn (steps)
 */
void
BrainOpenGLVolumeMprThreeDrawing::setupIntensityModeBlending(const int32_t numSlicesDrawn)
{
    bool averageFlag(false);
    bool minMaxFlag(false);
    switch (m_mprViewMode) {
        case VolumeMprViewModeEnum::AVERAGE_INTENSITY_PROJECTION:
            averageFlag = true;
            break;
        case VolumeMprViewModeEnum::MAXIMUM_INTENSITY_PROJECTION:
            glBlendEquationSeparate(GL_MAX, GL_MAX);
            minMaxFlag = true;
            break;
        case VolumeMprViewModeEnum::MINIMUM_INTENSITY_PROJECTION:
            glBlendEquationSeparate(GL_MIN, GL_MIN);
            minMaxFlag = true;
            break;
        case VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION:
            CaretAssert(0);
            break;
    }
    
    if (minMaxFlag) {
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
        
        glAlphaFunc(GL_GEQUAL, 0.95);
        glEnable(GL_ALPHA_TEST);
    }
    else if (averageFlag) {
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
        const float alpha(1.0 / static_cast<float>(numSlicesDrawn));
        glBlendColor(0.0, 0.0, 0.0, alpha);
        
        glBlendFuncSeparate(GL_CONSTANT_ALPHA_EXT,
                            GL_ONE,
                            GL_ZERO,
                            GL_ONE);
        
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GEQUAL, 0.95);
        glAlphaFunc(GL_ALWAYS, 1.0);
        glEnable(GL_ALPHA_TEST);
    }
    else {
        CaretAssertMessage(0, "Neither average nor min/max mode");
    }
}

/**
 * Draw the slice
 * @param mprSliceView
 *    Information for drawing slice
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 * @parram enabledBlendingFlag
 *    If true, enable blending
 * @param drawAttributesFlag
 *    Draw attributes (crosshairs, etc)
 * @param drawIntensitySliceBackgroundFlag
 *    Draw the background for intensity mode
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawSliceWithPrimitive(const MprVirtualSliceView& mprSliceView,
                                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                       const Vector3D& sliceCoordinates,
                                                       const GraphicsViewport& viewport,
                                                       const bool enabledBlendingFlag,
                                                       const bool drawAttributesFlag,
                                                       const bool drawIntensitySliceBackgroundFlag)
{
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
    
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
        
    const DisplayPropertiesVolume* dsv = m_brain->getDisplayPropertiesVolume();
    const bool allowBlendingFlag(dsv->getOpacity() >= 1.0);
    
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    if (allowBlendingFlag
        && enabledBlendingFlag) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
            if (enabledBlendingFlag) {
                if (firstFlag) {
                    /*
                     * Using GL_ONE prevents an edge artifact
                     * (narrow line on texture edges).
                     */
                    if (allowBlendingFlag) {
                        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                    }
                    firstFlag = false;
                }
                else {
                    if (allowBlendingFlag) {
                        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    }
                }
            }
            
            GraphicsPrimitiveV3fT3f* primitive(volumeInterface->getVolumeDrawingTrianglesPrimitive(vdi.mapIndex,
                                                                                                   m_displayGroup,
                                                                                                   m_tabIndex));
            if (primitive != NULL) {
                const Vector3D sliceOffset(0.0, 0.0, 0.0);
                const bool validPrimitiveFlag(setPrimtiveCoordinates(mprSliceView,
                                                                     volumeInterface,
                                                                     sliceOffset,
                                                                     primitive));

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
                            const VolumeFile* vf(dynamic_cast<VolumeFile*>(vdi.mapFile));
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
                                    CaretAssert(0);
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
                        performTriangleIdentification(primitive,
                                                      mprSliceView,
                                                      volumeInterface,
                                                      sliceViewPlane,
                                                      viewport,
                                                      m_fixedPipelineDrawing->mouseX,
                                                      m_fixedPipelineDrawing->mouseY);
                    }
                    else {
                        if (drawIntensitySliceBackgroundFlag) {
                            drawIntensityBackgroundSlice(primitive);
                        }
                        GraphicsEngineDataOpenGL::draw(primitive);
                    }
                }
            }
        }
    }
    
    if (drawAttributesFlag) {
        switch (m_brainModelMode) {
            case BrainModelMode::INVALID:
                CaretAssert(0);
                break;
            case BrainModelMode::ALL_3D:
                break;
            case BrainModelMode::VOLUME_2D:
                drawCrosshairs(sliceViewPlane,
                               sliceCoordinates,
                               viewport);
                break;
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
                                                                const MprVirtualSliceView& mprSliceView,
                                                                VolumeMappableInterface* volume,
                                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                const GraphicsViewport& viewport,
                                                                const float mouseX,
                                                                const float mouseY)
{
    CaretAssert(slicePrimitive);
    if (slicePrimitive->getPrimitiveType() != GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES) {
        CaretAssertMessage(0, "Primitive is not triangles");
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
        std::vector<Vector3D> vertexXYZ;
        std::vector<Vector3D> textureStr;
        mprSliceView.getTrianglesCoordinates(volume,
                                             stereotaxicXYZ,
                                             vertexXYZ,
                                             textureStr);
    }
    const int32_t numStereotaxicCoords(stereotaxicXYZ.size());
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
                                                        selectedPrimitiveDepth);
                        
                        Vector3D xyz;
                        volume->indexToSpace(ijk, xyz);
                        m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                         xyz);
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
                    voxelEditID->setVoxelIdentification(m_brain,
                                                        volume,
                                                        ijk,
                                                        selectedPrimitiveDepth);
                    const float floatDiffXYZ[3] { 0.0, 0.0, 0.0 };
                    voxelEditID->setVoxelDiffXYZ(floatDiffXYZ);
                    
                    Vector3D xyz;
                    volume->indexToSpace(ijk, xyz);
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
BrainOpenGLVolumeMprThreeDrawing::drawLayers(const VolumeMappableInterface* underlayVolume,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const Plane& slicePlane,
                                           const Vector3D& sliceCoordinates,
                                           const float sliceThickness)
{
    bool drawFibersFlag     = true;
    bool drawFociFlag       = true;
    bool drawOutlineFlag    = true;
    
    switch (m_brainModelMode) {
        case BrainModelMode::INVALID:
            break;
        case BrainModelMode::ALL_3D:
            drawFibersFlag = false;
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
        
        glPushMatrix();
        
        /*
         * Use some polygon offset that will adjust the depth values of the
         * layers so that the layers depth values place the layers in front of
         * the volume slice.
         */
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.0, 1.0);
        
        if (drawFociFlag) {
            BrainOpenGLFociDrawing fociDrawing;
            fociDrawing.drawVolumeOrthogonalFoci(m_brain,
                                                 m_fixedPipelineDrawing,
                                                 const_cast<VolumeMappableInterface*>(underlayVolume),
                                                 slicePlane,
                                                 sliceViewPlane,
                                                 sliceThickness);
        }
        if (drawOutlineFlag) {
            BrainOpenGLVolumeSurfaceOutlineDrawing::drawSurfaceOutline(underlayVolume,
                                                                       ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES,
                                                                       sliceProjectionType,
                                                                       sliceViewPlane,
                                                                       sliceCoordinates,
                                                                       slicePlane,
                                                                       m_browserTabContent->getVolumeSurfaceOutlineSet(),
                                                                       m_fixedPipelineDrawing,
                                                                       false);
        }
        
        if (drawFibersFlag) {
            glDisable(GL_DEPTH_TEST);
            m_fixedPipelineDrawing->drawFiberOrientations(&slicePlane,
                                                          StructureEnum::ALL);
            m_fixedPipelineDrawing->drawFiberTrajectories(&slicePlane,
                                                          StructureEnum::ALL);
        }
        
        glDisable(GL_POLYGON_OFFSET_FILL);
        
        glPopMatrix();
        
        glPopAttrib();
    }
}

/**
 * Create information about the slice being drawn for ALL view Intensity Mode
 */
BrainOpenGLVolumeMprThreeDrawing::SliceInfo
BrainOpenGLVolumeMprThreeDrawing::createSliceInfo3D() const
{
    
    EventOpenGLObjectToWindowTransform transformEvent(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
    EventManager::get()->sendEvent(transformEvent.getPointer());
    
    GraphicsViewport viewport(GraphicsViewport::newInstanceCurrentViewport());
    
    CaretAssertToDoFatal();
    SliceInfo sliceInfo;
    
    return sliceInfo;
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
    GraphicsPrimitive::PrimitiveType primitiveType(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES);
    CaretAssert(volumePrimitive->getPrimitiveType() == primitiveType);
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
    glAlphaFunc(GL_ALWAYS, 0.0);
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    GraphicsEngineDataOpenGL::draw(backgroundPrimitive.get());
    glPopAttrib();
}

/**
 * Draw the slice
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param zooming
 *    Zooming by user
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprThreeDrawing::drawSliceIntensityProjection3D(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                               const Vector3D& sliceCoordinates,
                                                               const GraphicsViewport& viewport)
{
    std::vector<std::pair<VolumeMappableInterface*,int32_t>> intensityVolumeFiles(getIntensityVolumeFilesAndMapIndices());
    if (intensityVolumeFiles.empty()) {
        return;
    }
    
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
    for (auto& volumeFileAndMapIndex : intensityVolumeFiles) {
        VolumeMappableInterface* volumeFile(volumeFileAndMapIndex.first);
        const int32_t mapIndex(volumeFileAndMapIndex.second);
        
        CaretAssertToDoWarning(); // Does 2D slice info work for 3D ?
        const SliceInfo sliceInfo(createSliceInfo(m_browserTabContent,
                                                  volumeFile,
                                                  sliceViewPlane,
                                                  sliceCoordinates));
//        const SliceInfo sliceInfo(createSliceInfo3D());
        if (idModeFlag) {
            performIntensityIdentification(sliceInfo.m_mprSliceView,
                                           volumeFile);
            continue;
        }
        std::vector<Vector3D> allIntersections(getVolumeRayIntersections(volumeFile,
                                                                         sliceInfo.m_mprSliceView.getVolumeCenterXYZ(),
                                                                         sliceInfo.m_mprSliceView.getNormalVector()));
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
            
            setupIntensityModeBlending(numSteps);

            for (int32_t iStep = 0; iStep < numSteps; iStep++) {
                const Vector3D sliceCoords(p1 + stepVector * iStep);
                const Vector3D sliceOffset(sliceCoords - sliceInfo.m_mprSliceView.getVolumeCenterXYZ());
                
                GraphicsPrimitiveV3fT3f* primitive(volumeFile->getVolumeDrawingTriangleStripPrimitive(mapIndex,
                                                                                                 m_displayGroup,
                                                                                                 m_tabIndex));
                
                if (primitive != NULL) {
                    setPrimtiveCoordinates(sliceInfo.m_mprSliceView,
                                           volumeFile,
                                           sliceOffset,
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
                        drawIntensityBackgroundSlice(primitive);
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
    }
    
    CaretAssertVectorIndex(intensityVolumeFiles, 0);
    VolumeMappableInterface* volumeFile(intensityVolumeFiles[0].first);
    const int32_t mapIndex(intensityVolumeFiles[0].second);
    CaretAssert(volumeFile);
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
            drawCrosshairs(sliceViewPlane,
                           sliceCoordinates,
                           viewport);
            break;
    }
}


AString
BrainOpenGLVolumeMprThreeDrawing::SliceInfo::toString(const AString& indentation) const
{
    AString txt;
        
    return txt;
}

/**
 * @return Size of voxel (smallest of any dimension)
 */
float
BrainOpenGLVolumeMprThreeDrawing::getVoxelSize(const VolumeMappableInterface* volume) const
{
    float dx, dy, dz;
    volume->getVoxelSpacing(dx, dy, dz);
    const float voxelSize(std::fabs(std::min(dx, std::min(dy, dz))));
    return voxelSize;
}

/**
 * Perform identification operation on 2D or 3D Maximum or Minimum Intensity Projection
 * @param mprSliceView
 *    Info for drawing slices
 * @param volume
 *    Volume being drawn
 */
void
BrainOpenGLVolumeMprThreeDrawing::performIntensityIdentification(const MprVirtualSliceView& mprSliceView,
                                                                 VolumeMappableInterface* volume)
{
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
    const std::vector<Vector3D> allIntersections(getVolumeRayIntersections(volume,
                                                                           modelXYZ,
                                                                           mprSliceView.getNormalVector()));
    const int32_t numIntersections(allIntersections.size());
    
    if (numIntersections == 2) {
        CaretAssertVectorIndex(allIntersections, 1);
        const Vector3D p1(allIntersections[0]);
        const Vector3D p2(allIntersections[1]);
        
        const float distance(MathFunctions::distance3D(p1, p2));
        if (distance > 1.0) {
            float minMaxIntensity(idMaxIntensityFlag ? 0.0 : 256.0);
            
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
                                               m_displayGroup, m_tabIndex, rgba);
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
                volume->indexToSpace(minMaxIJK, xyz);
                voxelID->setModelXYZ(xyz);
                
                float primitiveDepth(1.0);
                voxelID->setVoxelIdentification(m_brain,
                                                volume,
                                                minMaxIJK,
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
    
    const SliceInfo sliceInfo(createSliceInfo(m_browserTabContent,
                                              underlayVolume,
                                              sliceViewPlane,
                                              selectedXYZ));
    
    /*
     * coordinate step to move between adjacent slices
     */
    Vector3D sliceCoordIncreaseDirectionVector;
    sliceInfo.m_mprSliceView.getMontageIncreasingDirectionPlane().getNormalVector(sliceCoordIncreaseDirectionVector);
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
    
    /*
     * XYZ offset of first slice (top left) in montage grid from the selected
     * slice coordinates (XYZ in toolbar)
     */
    const Vector3D firstSliceOffsetXYZ(montageSliceCoordStepXYZ * firstSliceIndexOffset);
    
    /*
     * XYZ of first slice (top left) in montage grid
     */
    Vector3D sliceXYZ(selectedXYZ + firstSliceOffsetXYZ);
    
    /*
     * When "middle/center" slice is drawn, need to update
     * the graphics object to window transform
     */
    const int32_t numSlices(numRows * numCols);
    const int32_t midSliceNumber((numSlices > 1)
                                 ? (numSlices / 2)
                                 : 1);
    int32_t sliceCounter(1);
    for (int32_t i = 0; i < numRows; i++) {
        for (int32_t j = 0; j < numCols; j++) {
            const int32_t vpX = (j * (vpSizeX + horizontalMargin));
            const int32_t vpY = ((numRows - i - 1) * (vpSizeY + verticalMargin));
            int32_t vp[4] = {
                viewport.getX() + vpX,
                viewport.getY() + vpY,
                vpSizeX,
                vpSizeY
            };
            
            if ((vp[2] <= 0)
                || (vp[3] <= 0)) {
                continue;
            }
            
            bool viewAngleFixFlag(true);
            if (viewAngleFixFlag) {
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
                                                  updateGraphicsObjectToWindowTransformFlag);
                }
            }
            
            /*
             * Draw coordinates on slice
             */
            const float offsetDistance(sliceInfo.m_mprSliceView.getMontageIncreasingDirectionPlane().signedDistanceToPlane(sliceXYZ));
            BrainOpenGLVolumeSliceDrawing::drawMontageSliceCoordinates(m_fixedPipelineDrawing,
                                                                       m_browserTabContent,
                                                                       sliceViewPlane,
                                                                       vp,
                                                                       sliceXYZ,
                                                                       offsetDistance);

            /*
             * Move 'down' along axis
             */
            sliceXYZ -= montageSliceCoordStepXYZ;
            
            ++sliceCounter;
        }
    }
}

/**
 * @return Signed thickness of slice for the given slice plane
 */
float
BrainOpenGLVolumeMprThreeDrawing::getSignedSliceThickness(const VolumeMappableInterface* volumeInterface,
                                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const
{
    CaretAssert(volumeInterface);
    float thickness(1.0);
    
    /*
     * Voxel sizes for underlay volume
     */
    float x1, y1, z1;
    float x2, y2, z2;
    volumeInterface->indexToSpace(0, 0, 0, x1, y1, z1);
    volumeInterface->indexToSpace(1, 1, 1, x2, y2, z2);
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            thickness = z2 - z1;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            thickness = y2 - y1;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            thickness = x2 - x1;
            break;
    }

    return thickness;
}

/**
 * Set the stereotaxic and texture coordinates for the given primitive
 * @param mprSliceView
 *    Info about the slice
 * @param volume
 *    The volume
 * @param sliceOffset
 *    Offset of the slice from the coordinates
 * @param primtive
 *    The graphics primitive
 * @return
 *    True if the primitive is valid for drawing
 */
bool
BrainOpenGLVolumeMprThreeDrawing::setPrimtiveCoordinates(const MprVirtualSliceView& mprSliceView,
                                                         const VolumeMappableInterface* volume,
                                                         const Vector3D& sliceOffset,
                                                         GraphicsPrimitiveV3fT3f* primitive)
{
    bool validFlag(false);
    if (primitive != NULL) {
        if ((primitive->getNumberOfVertices() == 8)
            || (primitive->getNumberOfVertices() == 18)) {
            /*
             * When 8 vertices, a plane has been interesected with
             * the volume producing a triangle fan
             */
            CaretAssert(volume);
            
            std::vector<Vector3D> stereotaxicXYZ;
            std::vector<Vector3D> vertexXYZ;
            std::vector<Vector3D> textureStr;
            if (primitive->getNumberOfVertices() == 8) {
                validFlag = mprSliceView.getTriangleFanCoordinates(volume,
                                                                   stereotaxicXYZ,
                                                                   vertexXYZ,
                                                                   textureStr);
            }
            else if (primitive->getNumberOfVertices() == 18) {
                validFlag = mprSliceView.getTrianglesCoordinates(volume,
                                                                 stereotaxicXYZ,
                                                                 vertexXYZ,
                                                                 textureStr);
            }
            else {
                CaretAssert(0);
            }
            
            if (validFlag) {
                const int32_t numVertices(vertexXYZ.size());
                if (numVertices == primitive->getNumberOfVertices()) {
                    for (int32_t i = 0; i < numVertices; i++) {
                        primitive->replaceVertexFloatXYZ(i, vertexXYZ[i]);
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
                 * Create degenerate triangles
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

