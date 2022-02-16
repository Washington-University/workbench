
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

#define __BRAIN_OPEN_G_L_VOLUME_MPR_TWO_DRAWING_DECLARE__
#include "BrainOpenGLVolumeMprTwoDrawing.h"
#undef __BRAIN_OPEN_G_L_VOLUME_MPR_TWO_DRAWING_DECLARE__

#include <cmath>
#include <limits>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AnnotationPercentSizeText.h"
#include "Brain.h"
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#include "BrainOpenGLIdentificationDrawing.h"
#include "BrainOpenGLShapeRing.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
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
#include "SelectionItemVoxel.h"
#include "SelectionItemVoxelEditing.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "VolumeFile.h"

using namespace caret;

static bool debugFlag(false);

/**
 * \class caret::BrainOpenGLVolumeMprTwoDrawing
 * \brief Multiplanar Reconstruction volume drawing
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLVolumeMprTwoDrawing::BrainOpenGLVolumeMprTwoDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLVolumeMprTwoDrawing::~BrainOpenGLVolumeMprTwoDrawing()
{
}

/**
 * Draw the volume slices
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param browserTabContent
 *    Content of the browser tab being drawn
 * @param volumeDrawInfo
 *    Volumes being drawn
 * @param viewport
 *    The viewport
 */
void
BrainOpenGLVolumeMprTwoDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                     BrowserTabContent* browserTabContent,
                                     std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                     const GraphicsViewport& viewport)
{
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    CaretAssert(m_fixedPipelineDrawing);
    
    m_browserTabContent = browserTabContent;
    CaretAssert(m_browserTabContent);
    
    m_volumeDrawInfo = volumeDrawInfo;
    
    m_modelVolume = browserTabContent->getDisplayedVolumeModel();
    if (m_modelVolume == NULL) {
        return;
    }
    m_brain = m_modelVolume->getBrain();
    CaretAssert(m_brain);
    
    const int32_t numberOfVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    if (numberOfVolumes <= 0) {
        return;
    }
    
    m_underlayVolume = m_volumeDrawInfo[0].volumeFile;
    
    const DisplayPropertiesLabels* dsl = m_brain->getDisplayPropertiesLabels();
    m_displayGroup = dsl->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    
    m_tabIndex = m_browserTabContent->getTabNumber();

    /*
     * No lighting for drawing slices
     */
    m_fixedPipelineDrawing->disableLighting();

    const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType = m_browserTabContent->getSliceDrawingType();
    const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType(m_browserTabContent->getSliceProjectionType());
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_NEUROLOGICAL:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_RADIOLOGICAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            CaretAssert(0);
            break;
    }

    m_allSliceViewFlag = false;
    
    VolumeSliceViewPlaneEnum::Enum sliceViewPlane(browserTabContent->getSliceViewPlane());
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            m_allSliceViewFlag = true;
            
            /*
             * Draw parasagittal slice
             */
            std::array<int32_t, 4> axisVP;
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport.getViewport().data(),
                                                                VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                                                browserTabContent->getSlicePlanesAllViewLayout(),
                                                                axisVP.data());
            glPushMatrix();
            drawVolumeSliceViewType(sliceProjectionType,
                                    sliceDrawingType,
                                    VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                    axisVP);
            glPopMatrix();
            
            /*
             * Draw coronal slice
             */
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport.getViewport().data(),
                                                                VolumeSliceViewPlaneEnum::CORONAL,
                                                                browserTabContent->getSlicePlanesAllViewLayout(),
                                                                axisVP.data());
            glPushMatrix();
            drawVolumeSliceViewType(sliceProjectionType,
                                    sliceDrawingType,
                                    VolumeSliceViewPlaneEnum::CORONAL,
                                    axisVP);
            glPopMatrix();
            
            /*
             * Draw axial slice
             */
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport.getViewport().data(),
                                                                VolumeSliceViewPlaneEnum::AXIAL,
                                                                browserTabContent->getSlicePlanesAllViewLayout(),
                                                                axisVP.data());
            glPushMatrix();
            drawVolumeSliceViewType(sliceProjectionType,
                                    sliceDrawingType,
                                    VolumeSliceViewPlaneEnum::AXIAL,
                                    axisVP);
            glPopMatrix();
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            glPushMatrix();
            drawVolumeSliceViewType(sliceProjectionType,
                                    sliceDrawingType,
                                    sliceViewPlane,
                                    viewport);
            glPopMatrix();
            break;
    }
}

/**
 * Draw single or montage volume view slices.
 *
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
BrainOpenGLVolumeMprTwoDrawing::drawVolumeSliceViewType(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                        const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                        const GraphicsViewport& viewport)
{
    glPushAttrib(GL_ENABLE_BIT);
    
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawVolumeSliceViewTypeMontage(sliceDrawingType,
                                           sliceProjectionType,
                                           sliceViewPlane,
                                           viewport);
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
        {
            const Vector3D sliceCoordinates {
                m_browserTabContent->getSliceCoordinateParasagittal(),
                m_browserTabContent->getSliceCoordinateCoronal(),
                m_browserTabContent->getSliceCoordinateAxial()
            };
            drawVolumeSliceViewProjection(sliceProjectionType,
                                          sliceDrawingType,
                                          sliceViewPlane,
                                          sliceCoordinates,
                                          viewport);
        }
            break;
    }
    
    glPopAttrib();
}

/**
 * Draw montage slices.
 *
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
BrainOpenGLVolumeMprTwoDrawing::drawVolumeSliceViewTypeMontage(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                               const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                               const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                               const GraphicsViewport& viewport)
{
    const int32_t numRows = m_browserTabContent->getMontageNumberOfRows();
    CaretAssert(numRows > 0);
    const int32_t numCols = m_browserTabContent->getMontageNumberOfColumns();
    CaretAssert(numCols > 0);
    
    const int32_t montageCoordPrecision = m_browserTabContent->getVolumeMontageCoordinatePrecision();
    
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
    float originX, originY, originZ;
    float x1, y1, z1;
    m_underlayVolume->indexToSpace(0, 0, 0, originX, originY, originZ);
    m_underlayVolume->indexToSpace(1, 1, 1, x1, y1, z1);
    float sliceThickness = 0.0;
    float sliceOrigin    = 0.0;
    
    AString axisLetter = "";
    
    Vector3D sliceCoordinates {
        m_browserTabContent->getSliceCoordinateParasagittal(),
        m_browserTabContent->getSliceCoordinateCoronal(),
        m_browserTabContent->getSliceCoordinateAxial()
    };
    
    int32_t sliceIndex = -1;
    int32_t maximumSliceIndex = -1;
    int64_t dimI, dimJ, dimK, numMaps, numComponents;
    m_underlayVolume->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
    const int32_t sliceStep = m_browserTabContent->getMontageSliceSpacing();
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            sliceIndex = -1;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceIndex = m_browserTabContent->getSliceIndexAxial(m_underlayVolume);
            maximumSliceIndex = dimK;
            sliceThickness = z1 - originZ;
            sliceOrigin = originZ;
            axisLetter = "Z";
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceIndex = m_browserTabContent->getSliceIndexCoronal(m_underlayVolume);
            maximumSliceIndex = dimJ;
            sliceThickness = y1 - originY;
            sliceOrigin = originY;
            axisLetter = "Y";
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceIndex = m_browserTabContent->getSliceIndexParasagittal(m_underlayVolume);
            maximumSliceIndex = dimI;
            sliceThickness = x1 - originX;
            sliceOrigin = originX;
            axisLetter = "X";
            break;
    }
    
    /*
     * Foreground color for slice coordinate text
     */
    const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t foregroundRGBA[4];
    prefs->getBackgroundAndForegroundColors()->getColorForegroundVolumeView(foregroundRGBA);
    foregroundRGBA[3] = 255;
    const bool showCoordinates = m_browserTabContent->isVolumeMontageAxesCoordinatesDisplayed();
    uint8_t backgroundRGBA[4];
    prefs->getBackgroundAndForegroundColors()->getColorBackgroundVolumeView(backgroundRGBA);
    backgroundRGBA[3] = 255;
    
    /*
     * Determine a slice offset to selected slices is in
     * the center of the montage
     */
    const int32_t numSlicesViewed = (numCols * numRows);
    const int32_t sliceOffset = ((numSlicesViewed / 2)
                                 * sliceStep);
    
    sliceIndex += sliceOffset;
    
    /*
     * Find first valid slice for montage
     */
    while (sliceIndex >= 0) {
        if (sliceIndex < maximumSliceIndex) {
            break;
        }
        sliceIndex -= sliceStep;
    }
    
    if (sliceIndex >= 0) {
        for (int32_t i = 0; i < numRows; i++) {
            for (int32_t j = 0; j < numCols; j++) {
                if ((sliceIndex >= 0)
                    && (sliceIndex < maximumSliceIndex)) {
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
                    
                    const float sliceCoord = (sliceOrigin
                                              + sliceThickness * sliceIndex);
                    switch (sliceViewPlane) {
                        case VolumeSliceViewPlaneEnum::ALL:
                            break;
                        case VolumeSliceViewPlaneEnum::AXIAL:
                            sliceCoordinates[2] = sliceCoord;
                            break;
                        case VolumeSliceViewPlaneEnum::CORONAL:
                            sliceCoordinates[1] = sliceCoord;
                            break;
                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                            sliceCoordinates[0] = sliceCoord;
                            break;
                    }
                    
                    drawVolumeSliceViewProjection(sliceProjectionType,
                                                  sliceDrawingType,
                                                  sliceViewPlane,
                                                  sliceCoordinates,
                                                  vp);
                    
                    if (showCoordinates) {
                        const AString coordText = (axisLetter
                                                   + "="
                                                   + AString::number(sliceCoord, 'f', montageCoordPrecision));
                        
                        AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
                        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                        annotationText.setFontPercentViewportSize(10.0f);
                        annotationText.setTextColor(CaretColorEnum::CUSTOM);
                        annotationText.setCustomTextColor(foregroundRGBA);
                        annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
                        annotationText.setCustomBackgroundColor(backgroundRGBA);
                        annotationText.setText(coordText);
                        m_fixedPipelineDrawing->drawTextAtViewportCoords((vpSizeX - 5),
                                                                         5,
                                                                         annotationText);
                    }
                }
                sliceIndex -= sliceStep;
            }
        }
    }
}

/**
 * Draw a slice for either projection mode (oblique, orthogonal)
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprTwoDrawing::drawVolumeSliceViewProjection(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                              const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                              const Vector3D& sliceCoordinates,
                                                              const GraphicsViewport& viewport)
{
    
    glMatrixMode(GL_MODELVIEW);
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
    
    /*
     * Set the orthographic projection to fit the slice axis
     */
    setOrthographicProjection(sliceProjectionType,
                              sliceViewPlane,
                              viewport);

    SliceInfo sliceInfo(createSliceInfo(m_browserTabContent,
                                        m_underlayVolume,
                                        sliceProjectionType,
                                        sliceViewPlane,
                                        sliceCoordinates,
                                        m_allSliceViewFlag));

    if ( ! sliceInfo.m_plane.isValidPlane()) {
        return;
    }

    /*
     * Set the viewing transformation (camera position)
     */
    setViewingTransformation(sliceViewPlane,
                             sliceInfo);

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
            if (voxelID->isEnabledForSelection()
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
        /*
         * Disable culling so that both sides of the triangles/quads are drawn.
         */
        glDisable(GL_CULL_FACE);

        drawSliceWithPrimitive(sliceInfo,
                               sliceProjectionType,
                               sliceViewPlane,
                               sliceCoordinates,
                               viewport);


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
            
            BrainOpenGLVolumeSliceDrawing::drawIdentificationSymbols(m_fixedPipelineDrawing,
                                                                     m_browserTabContent,
                                                                     m_volumeDrawInfo[0].volumeFile,
                                                                     sliceInfo.m_plane,
                                                                     sliceThickness);
        }
        
        const Plane slicePlane(sliceInfo.m_plane);
        
        if ( ! m_identificationModeFlag) {
            if (slicePlane.isValidPlane()) {
                drawLayers(sliceDrawingType,
                           sliceProjectionType,
                           sliceViewPlane,
                           slicePlane,
                           sliceCoordinates);
            }
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
    }

    m_fixedPipelineDrawing->disableClippingPlanes();
}

/**
 * Create information about the slice being drawn
 * @param browserTabContent
 *    Content of the browser tab
 * @param underlayVolume
 *    The underlay volume
 * @param sliceProjectionType
 *    Type of slice projection
 * @param sliceViewPlane
 *    Plane being viewed
 * @param sliceCoordinates
 *    Coordinates of selected slices
 * @param allSliceViewFlag
 *    True if all slice view
 */
BrainOpenGLVolumeMprTwoDrawing::SliceInfo
BrainOpenGLVolumeMprTwoDrawing::createSliceInfo(const BrowserTabContent* browserTabContent,
                                                const VolumeMappableInterface* underlayVolume,
                                                const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                const Vector3D& sliceCoordinates,
                                                const bool allSliceViewFlag) const
{
    SliceInfo sliceInfo;
    
    /*
     * Normal vector of the plane
     */
    Vector3D planeNormalVector;
    
    bool radioFlag(false);
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_RADIOLOGICAL:
            radioFlag = true;
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_NEUROLOGICAL:
            break;
    }
    /*
     * Vector orthogonal to normal and up vectors.
     * It is in the plane
     */
    Vector3D orthogonalVector;
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            planeNormalVector[2] = 1.0;
            sliceInfo.m_upVector[1] = 1.0;
            orthogonalVector[0]  = 1.0;
            
            if (radioFlag) {
                planeNormalVector[2] = -1.0;
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            planeNormalVector[1] = -1.0;
            sliceInfo.m_upVector[2] =  1.0;
            orthogonalVector[0]  =  1.0;
            
            if (radioFlag) {
                planeNormalVector[1] = 1.0;
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            planeNormalVector[0] = -1.0;
            sliceInfo.m_upVector[2] =  1.0;
            orthogonalVector[1]  = -1.0;
            break;
    }
    
    CaretAssert(underlayVolume);
    BoundingBox boundingBox;
    underlayVolume->getVoxelSpaceBoundingBox(boundingBox);
    
    /*
     * Might want to expand these so that slice is extra big and does not get clipped
     */
    const float posX(boundingBox.getMaxX());
    const float negX(boundingBox.getMinX());
    const float posY(boundingBox.getMaxY());
    const float negY(boundingBox.getMinY());
    const float posZ(boundingBox.getMaxZ());
    const float negZ(boundingBox.getMinZ());
    
    const float sliceX(sliceCoordinates[0]);
    const float sliceY(sliceCoordinates[1]);
    const float sliceZ(sliceCoordinates[2]);
    
    boundingBox.getCenter(sliceInfo.m_centerXYZ);
    
    const float leftX(radioFlag
                      ? posX
                      : negX);
    const float rightX(radioFlag
                      ? negX
                      : posX);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            /*
             * Axial slice is in plane orthogonal to Z-axis
             */
            sliceInfo.m_bottomLeftXYZ[0] = leftX;
            sliceInfo.m_bottomLeftXYZ[1] = negY;
            sliceInfo.m_bottomLeftXYZ[2] = sliceZ;
            
            sliceInfo.m_bottomRightXYZ[0] = rightX;
            sliceInfo.m_bottomRightXYZ[1] = negY;
            sliceInfo.m_bottomRightXYZ[2] = sliceZ;
            
            sliceInfo.m_topLeftXYZ[0]  = leftX;
            sliceInfo.m_topLeftXYZ[1]  = posY;
            sliceInfo.m_topLeftXYZ[2] = sliceZ;
            
            sliceInfo.m_topRightXYZ[0] = rightX;
            sliceInfo.m_topRightXYZ[1] = posY;
            sliceInfo.m_topRightXYZ[2] = sliceZ;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            /*
             * Coronal slice is in plane orthogonal to Y-axis
             */
            sliceInfo.m_bottomLeftXYZ[0] = leftX;
            sliceInfo.m_bottomLeftXYZ[1] = sliceY;
            sliceInfo.m_bottomLeftXYZ[2] = negZ;
            
            sliceInfo.m_bottomRightXYZ[0] = rightX;
            sliceInfo.m_bottomRightXYZ[1] = sliceY;
            sliceInfo.m_bottomRightXYZ[2] = negZ;
            
            sliceInfo.m_topLeftXYZ[0]  = leftX;
            sliceInfo.m_topLeftXYZ[1]  = sliceY;
            sliceInfo.m_topLeftXYZ[2] = posZ;
            
            sliceInfo.m_topRightXYZ[0] = rightX;
            sliceInfo.m_topRightXYZ[1] = sliceY;
            sliceInfo.m_topRightXYZ[2] = posZ;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            /*
             * Parasgittal slice is in plane orthogonal to X-axis
             */
            sliceInfo.m_bottomLeftXYZ[0] = sliceX;
            sliceInfo.m_bottomLeftXYZ[1] = posY;
            sliceInfo.m_bottomLeftXYZ[2] = negZ;
            
            sliceInfo.m_bottomRightXYZ[0] = sliceX;
            sliceInfo.m_bottomRightXYZ[1] = negY;
            sliceInfo.m_bottomRightXYZ[2] = negZ;
            
            sliceInfo.m_topLeftXYZ[0]  = sliceX;
            sliceInfo.m_topLeftXYZ[1]  = posY;
            sliceInfo.m_topLeftXYZ[2] = posZ;
            
            sliceInfo.m_topRightXYZ[0] = sliceX;
            sliceInfo.m_topRightXYZ[1] = negY;
            sliceInfo.m_topRightXYZ[2] = posZ;
            break;
    }
    
    const float expandPercentage(2.0); /* 2.0 = 200% */
    MathFunctions::expandBoxPercentage3D(sliceInfo.m_bottomLeftXYZ,
                                         sliceInfo.m_bottomRightXYZ,
                                         sliceInfo.m_topRightXYZ,
                                         sliceInfo.m_topLeftXYZ,
                                         expandPercentage);
    
    Vector3D translation;
    browserTabContent->getTranslation(translation);
    
    Matrix4x4 viewRotationMatrix;
    
    /*
     * We want to rotate around the coordinate of the selected slices.
     * 1 - Translate the coordinate to the origin
     * 2 - Rotate
     * 3 - Scale (zoom)
     * 4 - Translate back to the coordinate
     */
    viewRotationMatrix.translate(-sliceCoordinates[0], -sliceCoordinates[1], -sliceCoordinates[2]);

    const Matrix4x4 rotationMatrix = browserTabContent->getMprRotationMatrix4x4ForSlicePlane(sliceViewPlane);
    viewRotationMatrix.postmultiply(rotationMatrix);
    
    viewRotationMatrix.translate(sliceCoordinates[0], sliceCoordinates[1], sliceCoordinates[2]);
    
    viewRotationMatrix.multiplyPoint3(sliceInfo.m_bottomLeftXYZ);
    viewRotationMatrix.multiplyPoint3(sliceInfo.m_bottomRightXYZ);
    viewRotationMatrix.multiplyPoint3(sliceInfo.m_topRightXYZ);
    viewRotationMatrix.multiplyPoint3(sliceInfo.m_topLeftXYZ);
    
    sliceInfo.m_plane = Plane(sliceInfo.m_topLeftXYZ,
                              sliceInfo.m_bottomLeftXYZ,
                              sliceInfo.m_bottomRightXYZ);
    CaretAssert(sliceInfo.m_plane.isValidPlane());
    
    sliceInfo.m_plane.getNormalVector(sliceInfo.m_normalVector);

    /*
     * "Up" vector points from bottom of slice to top
     */
    const Vector3D bottomToTop(sliceInfo.m_topLeftXYZ - sliceInfo.m_bottomLeftXYZ);
    sliceInfo.m_upVector = bottomToTop.normal();
    
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
BrainOpenGLVolumeMprTwoDrawing::addCrosshairSection(GraphicsPrimitiveV3fC4ub* primitiveSliceCrosshair,
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
BrainOpenGLVolumeMprTwoDrawing::drawPanningCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                      const Vector3D& crossHairXYZ,
                                                      const GraphicsViewport& viewport)
{
    SelectionItemVolumeMprCrosshair* crosshairID(m_brain->getSelectionManager()->getVolumeMprCrosshairIdentification());
    if (m_identificationModeFlag) {
        if ( ! crosshairID->isEnabledForSelection()) {
            return;
        }
        if ( ! m_allSliceViewFlag) {
            return;
        }
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
    const float rotateThicker(m_allSliceViewFlag
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
            glRotatef(-m_browserTabContent->getMprRotationZ(), 0.0, 0.0, 1.0);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            glRotatef(m_browserTabContent->getMprRotationY(), 0.0, 0.0, 1.0);
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
BrainOpenGLVolumeMprTwoDrawing::getAxisColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const
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
 * @param sliceProjectionType
 *    Type of slice projection
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprTwoDrawing::drawCrosshairs(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
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
    
    drawAxisLabels(sliceProjectionType,
                   sliceViewPlane,
                   viewport);
    
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

/**
 * Draw the axis labels for slice
 *
 * @param sliceProjectionType
 *    Type of slice projection
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprTwoDrawing::drawAxisLabels(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const GraphicsViewport& viewport)
{
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

    bool radioFlag(false);
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_NEUROLOGICAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_RADIOLOGICAL:
            radioFlag = true;
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            CaretAssert(0);
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
            if (radioFlag) {
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
            if (radioFlag) {
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
 * Set the orthographic projection.
 *
 * @param sliceProjectionType
 *    Type of slice projection
 * @param sliceViewPlane
 *    The slice plane being drawn
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeMprTwoDrawing::setOrthographicProjection(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                          const GraphicsViewport& viewport)
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
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
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
    
    const float scaling(1.0 / m_browserTabContent->getScaling());
    
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
    
    const bool centerizeFlag(false);
    if (centerizeFlag) {
        /*
         * Center the volume in the orthographic region
         */
        bool xHorizontalFlag(false);
        bool yHorozontalFlag(false);
        bool yVerticalFlag(false);
        bool zVerticalFlag(false);
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                xHorizontalFlag = true;
                yVerticalFlag   = true;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                xHorizontalFlag = true;
                zVerticalFlag   = true;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                yHorozontalFlag = true;
                zVerticalFlag       = true;
                break;
        }
        
        if (xHorizontalFlag) {
            const float halfExtra((defaultOrthoWidth - lengthX) / 2.0);
            orthoRight = boundingBox.getMaxX() + halfExtra;
            orthoLeft  = boundingBox.getMinX() - halfExtra;
            
            switch (sliceProjectionType) {
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_NEUROLOGICAL:
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_RADIOLOGICAL:
                    std::swap(orthoLeft,
                              orthoRight);
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                    CaretAssert(0);
                    break;
            }
        }
        else if (yHorozontalFlag) {
            const float halfExtra((defaultOrthoWidth - lengthY) / 2.0);
            orthoLeft  += halfExtra;
            orthoRight += halfExtra;
        }
        else {
            CaretAssert(0);
        }
        if (yVerticalFlag) {
            const float halfExtra((defaultOrthoHeight - lengthY) / 2.0);
            orthoTop    = boundingBox.getMaxY() + halfExtra;
            orthoBottom = boundingBox.getMinY() - halfExtra;
        }
        else if (zVerticalFlag) {
            const float halfExtra((defaultOrthoHeight - lengthZ) / 2.0);
            orthoTop    = boundingBox.getMaxZ() + halfExtra;
            orthoBottom = boundingBox.getMinZ() - halfExtra;
        }
        else {
            CaretAssert(0);
        }
    }

    m_orthographicBounds[0] = orthoLeft;
    m_orthographicBounds[1] = orthoRight;
    m_orthographicBounds[2] = orthoBottom;
    m_orthographicBounds[3] = orthoTop;
    m_orthographicBounds[4] = nearDepth;
    m_orthographicBounds[5] = farDepth;
    
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
 * @param plane
 *    Plane equation of selected slice.
 */
void
BrainOpenGLVolumeMprTwoDrawing::setViewingTransformation(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                         const SliceInfo& sliceInfo)
{
    /*
     * Look at center of volume
     */
    m_lookAtCenterXYZ = sliceInfo.m_centerXYZ;
    
    Vector3D translation;
    m_browserTabContent->getTranslation(translation);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            m_lookAtCenterXYZ[0] -= translation[0];
            m_lookAtCenterXYZ[1] -= translation[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            m_lookAtCenterXYZ[0] -= translation[0];
            m_lookAtCenterXYZ[2] -= translation[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            m_lookAtCenterXYZ[1] += translation[1];
            m_lookAtCenterXYZ[2] -= translation[2];
            break;
    }

    /*
     * Since an orthographic projection is used, the eye only needs
     * to be a little bit from the center along the plane's normal vector.
     */
    glm::vec3 eye(m_lookAtCenterXYZ[0] + sliceInfo.m_normalVector[0] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                  m_lookAtCenterXYZ[1] + sliceInfo.m_normalVector[1] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
                  m_lookAtCenterXYZ[2] + sliceInfo.m_normalVector[2] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance);
        
    /*
     * Now set the camera to look at the center.
     * with the camera offset a little bit from the center.
     * This allows the slice's voxels to be drawn in the actual coordinates.
     */
    glm::vec3 up(sliceInfo.m_upVector[0], sliceInfo.m_upVector[1], sliceInfo.m_upVector[2]);
    glm::vec3 lookAt(m_lookAtCenterXYZ[0], m_lookAtCenterXYZ[1], m_lookAtCenterXYZ[2]);
    glm::mat4 lookAtMatrix = glm::lookAt(eye, lookAt, up);
    
    if (debugFlag) {
        if (sliceViewPlane == VolumeSliceViewPlaneEnum::PARASAGITTAL) {
            glm::vec3 vec = lookAt - eye;
            glm::vec3 n = glm::normalize(vec);
            std::cout << "Eye : " << eye[0] << ", " << eye[1] << ", " << eye[2] << std::endl;
            std::cout << "   Look at: "
            << AString::fromNumbers(m_lookAtCenterXYZ, 3, ", ") << std::endl;
            std::cout << "   Up: "
            << AString::fromNumbers(sliceInfo.m_upVector, 3, ", ") << std::endl;
            std::cout << "   Eye to Look At Vector: " << n[0] << ", " << n[1] << ", " << n[2] << std::endl;
        }
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(lookAtMatrix));
    
    if (debugFlag) {
        float m16[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m16);
        Matrix4x4 m;
        m.setMatrixFromOpenGL(m16);
        std::cout << "ModelView: " << m.toString() << std::endl << std::flush;
    }
}

/**
 * Draw the slice
 * @param sliceInfo
 *    Information for drawing slice
 * @param sliceProjectionType
 *    Type of slice projection
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeMprTwoDrawing::drawSliceWithPrimitive(const SliceInfo& sliceInfo,
                                                       const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                       const Vector3D& sliceCoordinates,
                                                       const GraphicsViewport& viewport)
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
        
    /*
     * Unit vector and distance in model coords along left side of screen
     */
    double bottomLeftToTopLeftUnitVector[3] = {
        sliceInfo.m_topLeftXYZ[0] - sliceInfo.m_bottomLeftXYZ[0],
        sliceInfo.m_topLeftXYZ[1] - sliceInfo.m_bottomLeftXYZ[1],
        sliceInfo.m_topLeftXYZ[2] - sliceInfo.m_bottomLeftXYZ[2],
    };
    MathFunctions::normalizeVector(bottomLeftToTopLeftUnitVector);
    const double bottomLeftToTopLeftDistance = MathFunctions::distance3D(sliceInfo.m_bottomLeftXYZ,
                                                                         sliceInfo.m_topLeftXYZ);
    
    /*
     * Unit vector and distance in model coords along right side of screen
     */
    double bottomRightToTopRightUnitVector[3] = {
        sliceInfo.m_topRightXYZ[0] - sliceInfo.m_bottomRightXYZ[0],
        sliceInfo.m_topRightXYZ[1] - sliceInfo.m_bottomRightXYZ[1],
        sliceInfo.m_topRightXYZ[2] - sliceInfo.m_bottomRightXYZ[2]
    };
    MathFunctions::normalizeVector(bottomRightToTopRightUnitVector);
    const double bottomRightToTopRightDistance = MathFunctions::distance3D(sliceInfo.m_bottomRightXYZ,
                                                                           sliceInfo.m_topRightXYZ);
    
    if ((bottomLeftToTopLeftDistance > 0)
        && (bottomRightToTopRightDistance > 0)) {
        const DisplayPropertiesVolume* dsv = m_brain->getDisplayPropertiesVolume();
        const bool allowBlendingFlag(dsv->getOpacity() >= 1.0);
        
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        if (allowBlendingFlag) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        glDisable(GL_DEPTH_TEST);
        
        bool firstFlag(true);
        for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
            const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = m_volumeDrawInfo[iVol];
            VolumeMappableInterface* volumeInterface = vdi.volumeFile;
            if (volumeInterface != NULL) {
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
                
                GraphicsPrimitiveV3fT3f* primitive(volumeInterface->getVolumeDrawingPrimitive(vdi.mapIndex,
                                                                                              DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                                                              m_tabIndex));
                
                if (primitive != NULL) {
                    Vector3D maxStr = { 1.0, 1.0, 1.0 };
                    Vector3D textureBottomLeft;
                    getTextureCoordinates(volumeInterface, sliceInfo.m_bottomLeftXYZ, maxStr, textureBottomLeft);
                    Vector3D textureBottomRight;
                    getTextureCoordinates(volumeInterface, sliceInfo.m_bottomRightXYZ, maxStr, textureBottomRight);
                    Vector3D textureTopLeft;
                    getTextureCoordinates(volumeInterface, sliceInfo.m_topLeftXYZ, maxStr, textureTopLeft);
                    Vector3D textureTopRight;
                    getTextureCoordinates(volumeInterface, sliceInfo.m_topRightXYZ, maxStr, textureTopRight);
                    
                    primitive->replaceVertexFloatXYZ(0, sliceInfo.m_bottomLeftXYZ);
                    primitive->replaceVertexFloatXYZ(1, sliceInfo.m_bottomRightXYZ);
                    primitive->replaceVertexFloatXYZ(2, sliceInfo.m_topLeftXYZ);
                    primitive->replaceVertexFloatXYZ(3, sliceInfo.m_topRightXYZ);
                    
                    primitive->replaceVertexTextureSTR(0, textureBottomLeft);
                    primitive->replaceVertexTextureSTR(1, textureBottomRight);
                    primitive->replaceVertexTextureSTR(2, textureTopLeft);
                    primitive->replaceVertexTextureSTR(3, textureTopRight);
                    
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
                    
                    if (m_identificationModeFlag) {
                        performPlaneIdentification(sliceInfo,
                                                   volumeInterface,
                                                   sliceViewPlane,
                                                   viewport,
                                                   m_fixedPipelineDrawing->mouseX,
                                                   m_fixedPipelineDrawing->mouseY);
                    }
                    else {
                        GraphicsEngineDataOpenGL::draw(primitive);
                    }
                }
            }
        }
        
        drawCrosshairs(sliceProjectionType,
                       sliceViewPlane,
                       sliceCoordinates,
                       viewport);

        glPopAttrib();
    }

    bool drawViewportBoxFlag(false);
    if (debugFlag
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
 * Get the texture coordinates for an XYZ-coordinate
 *
 * @param volumeMappableInterface
 *     The volume file
 * @param xyz
 *     The XYZ coordinate
 * @param maxStr
 *     The maximum texture str coordinate
 * @param strOut
 *     Output texture str coordinate
 * @return
 *     True if output coordinate is valid, else false.
 */
bool
BrainOpenGLVolumeMprTwoDrawing::getTextureCoordinates(const VolumeMappableInterface* volumeMappableInterface,
                                                      const Vector3D& xyz,
                                                      const Vector3D& maxStr,
                                                      Vector3D& strOut)
{
    std::vector<int64_t> dims(5);
    volumeMappableInterface->getDimensions(dims);
    
    
    {
        const VolumeSpace& volumeSpace = volumeMappableInterface->getVolumeSpace();
        Vector3D ijk;
        volumeSpace.spaceToIndex(xyz, ijk);
        
        const Vector3D normalizedIJK {
            (ijk[0] / dims[0]),
            (ijk[1] / dims[1]),
            (ijk[2] / dims[2])
        };
        Vector3D str {
            (normalizedIJK[0] * maxStr[0]),
            (normalizedIJK[1] * maxStr[1]),
            (normalizedIJK[2] * maxStr[2])
        };
        
        strOut = str;
    }
    
    return true;
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
 *
 */
void
BrainOpenGLVolumeMprTwoDrawing::getMouseViewportXY(const GraphicsViewport& viewport,
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
BrainOpenGLVolumeMprTwoDrawing::getMouseViewportNormalizedXY(const GraphicsViewport& viewport,
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
 * Perform voxel identification using a rectangle drawn in the plane of the slice
 * @param sliceInfo
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
BrainOpenGLVolumeMprTwoDrawing::performPlaneIdentification(const SliceInfo& sliceInfo,
                                                           VolumeMappableInterface* volumeInterface,
                                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                           const GraphicsViewport& viewport,
                                                           const float mouseX,
                                                           const float mouseY)
{
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
    if (debugFlag) {
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
     * Must clear color and depth buffers
     */
    glClear(GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT);
    
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);  /* Pixel storage */
    glPushAttrib(GL_LIGHTING_BIT       /* lighting and shade model */
                 | GL_PIXEL_MODE_BIT); /* glReadBuffer */
    
    glShadeModel(GL_SMOOTH);  /* need shading so that colors are interpolated */
    glDisable(GL_LIGHTING);
    
    /*
     * Rectangle with no red on left to full red on right
     * no green and bottom to full green at top
     */
    const float bottomLeftRGBA[4] { 0.0, 0.0, 1.0, 1.0 };   /* no red or green*/
    const float bottomRightRGBA[4] { 1.0, 0.0, 1.0, 1.0 };  /* red */
    const float topRightRGBA[4] { 1.0, 1.0, 1.0, 1.0 };     /* red and green */
    const float topLeftRGBA[4] { 0.0, 1.0, 1.0, 1.0 };      /* green */
    
    std::unique_ptr<GraphicsPrimitiveV3fC4f> primitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN));
    primitive->addVertex(sliceInfo.m_bottomLeftXYZ, bottomLeftRGBA);
    primitive->addVertex(sliceInfo.m_bottomRightXYZ, bottomRightRGBA);
    primitive->addVertex(sliceInfo.m_topRightXYZ, topRightRGBA);
    primitive->addVertex(sliceInfo.m_topLeftXYZ, topLeftRGBA);
    GraphicsEngineDataOpenGL::draw(primitive.get());
        
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1); /* bytes */
    float pixelRGBA[4];

#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /* do not call glReadBuffer() */
#else
    glReadBuffer(GL_BACK);
#endif

    /*
     * Read the RGB color at location of mouse
     */
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
    
    /*
     * Interpolate to get XYZ at location of mouse
     */
    const Vector3D topDiffXYZ = sliceInfo.m_topRightXYZ - sliceInfo.m_topLeftXYZ;
    const Vector3D bottomDiffXYZ = sliceInfo.m_bottomRightXYZ - sliceInfo.m_bottomLeftXYZ;
    const Vector3D topXYZ = sliceInfo.m_topLeftXYZ + topDiffXYZ * redPct;
    const Vector3D bottomXYZ = sliceInfo.m_bottomLeftXYZ + bottomDiffXYZ * redPct;
    const Vector3D diffXYZ = topXYZ - bottomXYZ;
    const Vector3D xyz = bottomXYZ + diffXYZ * greenPct;
    
    CaretLogFine("Selected Red/Green RGBA: "
                 + AString::fromNumbers(pixelRGBA, 4, ", ")
                 + "\nInterpolated XYZ: "
                 + AString::fromNumbers(xyz, 3, ", "));

    int64_t ijk[3];
    volumeInterface->enclosingVoxel(xyz[0], xyz[1], xyz[2], ijk[0], ijk[1], ijk[2]);
    if (volumeInterface->indexValid(ijk[0], ijk[0], ijk[2])) {
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
                
                if (voxelID->isOtherScreenDepthCloserToViewer(selectedPrimitiveDepth)) {
                    voxelID->setVoxelIdentification(m_brain,
                                                    volumeInterface,
                                                    ijk,
                                                    selectedPrimitiveDepth);
                    
                    Vector3D xyz;
                    volumeInterface->indexToSpace(ijk, xyz);
                    m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                     xyz);
                    CaretLogFinest("Selected Voxel (3D): " + AString::fromNumbers(ijk, 3, ","));
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
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param slicePlane
 *    Plane of the slice.
 * @param sliceCoordinates
 *    Coordinates of the selected slices.
 */
void
BrainOpenGLVolumeMprTwoDrawing::drawLayers(const VolumeSliceDrawingTypeEnum::Enum /*sliceDrawingType*/,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const Plane& slicePlane,
                                           const Vector3D& sliceCoordinates)
{
    bool drawFibersFlag     = true;
    bool drawOutlineFlag    = true;
    
    if ( ! m_identificationModeFlag) {
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
            
            if (drawOutlineFlag) {
                BrainOpenGLVolumeSliceDrawing::drawSurfaceOutline(m_underlayVolume,
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
}
