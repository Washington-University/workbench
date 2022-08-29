
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <cmath>
#include <limits>

#define __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__
#include "BrainOpenGLVolumeSliceDrawing.h"
#undef __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__

#include "AnnotationCoordinate.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "BoundingBox.h"
#include "Brain.h"
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#include "BrainOpenGLIdentificationDrawing.h"
#include "BrainOpenGLPrimitiveDrawing.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainordinateRegionOfInterest.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"
#include "CaretPreferenceDataValue.h"
#include "CaretPreferences.h"
#include "CiftiMappableDataFile.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "ElapsedTimer.h"
#include "FociFile.h"
#include "Focus.h"
#include "GapsAndMargins.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "GraphicsViewport.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentificationManager.h"
#include "IdentificationWithColor.h"
#include "LabelDrawingProperties.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "NodeAndVoxelColoring.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemVoxel.h"
#include "SelectionItemVoxelEditing.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "SpacerTabIndex.h"
#include "SurfacePlaneIntersectionToContour.h"
#include "Surface.h"
#include "VolumeFile.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;

static const bool debugFlag = false;
    
/**
 * \class caret::BrainOpenGLVolumeSliceDrawing 
 * \brief Draws volume slices using OpenGL
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLVolumeSliceDrawing::BrainOpenGLVolumeSliceDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLVolumeSliceDrawing::~BrainOpenGLVolumeSliceDrawing()
{
}

/**
 * Draw Volume Slices or slices for ALL Stuctures View.
 *
 * @param fixedPipelineDrawing
 *    The OpenGL drawing.
 * @param browserTabContent
 *    Content of browser tab that is to be drawn.
 * @param volumeDrawInfo
 *    Info on each volume layers for drawing.
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeSliceDrawing::draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                  BrowserTabContent* browserTabContent,
                                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                  const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                  const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                  const int32_t viewport[4])
{
    if (volumeDrawInfo.empty()) {
        return;
    }
    
    CaretAssert(sliceProjectionType == VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL);
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    m_browserTabContent = browserTabContent;    
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    
    /*
     * No lighting for drawing slices
     */
    m_fixedPipelineDrawing->disableLighting();
    
    /*
     * Initialize class members which help reduce the number of
     * parameters that are passed to methods.
     */
    m_brain           = NULL;
    m_modelVolume     = NULL;
    m_modelWholeBrain = NULL;
    m_modelType       = ModelTypeEnum::MODEL_TYPE_INVALID;
    if (m_browserTabContent->getDisplayedVolumeModel() != NULL) {
        m_modelVolume = m_browserTabContent->getDisplayedVolumeModel();
        m_brain = m_modelVolume->getBrain();
        m_modelType = m_modelVolume->getModelType();
    }
    else if (m_browserTabContent->getDisplayedWholeBrainModel() != NULL) {
        m_modelWholeBrain = m_browserTabContent->getDisplayedWholeBrainModel();
        m_brain = m_modelWholeBrain->getBrain();
        m_modelType = m_modelWholeBrain->getModelType();
    }
    else {
        CaretAssertMessage(0, "Invalid model for volume slice drawing.");
    }
    CaretAssert(m_brain);
    CaretAssert(m_modelType != ModelTypeEnum::MODEL_TYPE_INVALID);
    
    m_volumeDrawInfo = volumeDrawInfo;
    if (m_volumeDrawInfo.empty()) {
        return;
    }
    m_underlayVolume = m_volumeDrawInfo[0].volumeFile;
    
    const DisplayPropertiesLabels* dsl = m_brain->getDisplayPropertiesLabels();
    m_displayGroup = dsl->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    
    m_tabIndex = m_browserTabContent->getTabNumber();
    
    /*
     * Cifti files are slow at getting individual voxels since they
     * provide no access to individual voxels.  The reason is that
     * the data may be on a server (Dense data) and accessing a single
     * voxel would require requesting the entire map.  So, for
     * each Cifti file, get the enter map.  This also, eliminate multiple
     * requests for the same map when drawing an ALL view.
     */
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    for (int32_t i = 0; i < numVolumes; i++) {
        std::vector<float> ciftiMapData;
        m_ciftiMappableFileData.push_back(ciftiMapData);
        
        const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(m_volumeDrawInfo[i].volumeFile);
        if (ciftiMapFile != NULL) {
            ciftiMapFile->getMapData(m_volumeDrawInfo[i].mapIndex,
                                     m_ciftiMappableFileData[i]);
        }
    }
    
    /** END SETUP OF MEMBERS IN PARENT CLASS BrainOpenGLVolumeSliceDrawing */
    
    
    
    if (browserTabContent->getDisplayedVolumeModel() != NULL) {
        drawVolumeSliceViewPlane(sliceDrawingType,
                                 sliceProjectionType,
                                 browserTabContent->getSliceViewPlane(),
                                 browserTabContent->getSlicePlanesAllViewLayout(),
                                 viewport);
    }
    else if (browserTabContent->getDisplayedWholeBrainModel() != NULL) {
        drawVolumeSlicesForAllStructuresView(sliceProjectionType,
                                             viewport);
    }    
}

/**
 * Draw volume view slices for the given view plane.
 *
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param allPlanesLayout
 *    The layout in ALL slices view.
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewPlane(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                              const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                              const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                              const int32_t viewport[4])
{
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            int32_t paraVP[4]    = { 0, 0, 0, 0 };
            int32_t coronalVP[4] = { 0, 0, 0, 0 };
            int32_t axialVP[4]   = { 0, 0, 0, 0 };
            
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                    VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                    allPlanesLayout,
                                    paraVP);
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                    VolumeSliceViewPlaneEnum::CORONAL,
                                    allPlanesLayout,
                                    coronalVP);
            BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                    VolumeSliceViewPlaneEnum::AXIAL,
                                    allPlanesLayout,
                                    axialVP);
            
            /*
             * Draw parasagittal slice
             */
            glPushMatrix();
            drawVolumeSliceViewType(AllSliceViewMode::ALL_YES,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                    paraVP);
            glPopMatrix();
            
            
            /*
             * Draw coronal slice
             */
            glPushMatrix();
            drawVolumeSliceViewType(AllSliceViewMode::ALL_YES,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::CORONAL,
                                    coronalVP);
            glPopMatrix();
            
            
            /*
             * Draw axial slice
             */
            glPushMatrix();
            drawVolumeSliceViewType(AllSliceViewMode::ALL_YES,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::AXIAL,
                                    axialVP);
            glPopMatrix();
            
            if (allPlanesLayout == VolumeSliceViewAllPlanesLayoutEnum::GRID_LAYOUT) {
                /*
                 * 4th quadrant is used for axis showing orientation
                 */
                int32_t allVP[4] = { 0, 0, 0, 0 };
                BrainOpenGLViewportContent::getSliceAllViewViewport(viewport,
                                        VolumeSliceViewPlaneEnum::ALL,
                                        allPlanesLayout,
                                        allVP);
                
                switch (sliceProjectionType) {
                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                        CaretAssert(0);
                        break;
                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                        break;
                    case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                        CaretAssert(0);
                        break;
                }
            }
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            drawVolumeSliceViewType(AllSliceViewMode::ALL_NO,
                                    sliceDrawingType,
                                    sliceProjectionType,
                                    sliceViewPlane,
                                    viewport);
            break;
    }
}

/**
 * Draw slices for the all structures view.
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeSliceDrawing::drawVolumeSlicesForAllStructuresView(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                  const int32_t viewport[4])
{
    m_orthographicBounds[0] = m_fixedPipelineDrawing->orthographicLeft;
    m_orthographicBounds[1] = m_fixedPipelineDrawing->orthographicRight;
    m_orthographicBounds[2] = m_fixedPipelineDrawing->orthographicBottom;
    m_orthographicBounds[3] = m_fixedPipelineDrawing->orthographicTop;
    m_orthographicBounds[4] = m_fixedPipelineDrawing->orthographicNear;
    m_orthographicBounds[5] = m_fixedPipelineDrawing->orthographicFar;
    
    /*
     * Enlarge the region
     */
    {
        const float left   = m_fixedPipelineDrawing->orthographicLeft;
        const float right  = m_fixedPipelineDrawing->orthographicRight;
        const float bottom = m_fixedPipelineDrawing->orthographicBottom;
        const float top    = m_fixedPipelineDrawing->orthographicTop;
        
        const float scale = 2.0;
        
        const float centerX  = (left + right) / 2.0;
        const float dx       = (right - left) / 2.0;
        const float newLeft  = centerX - (dx * scale);
        const float newRight = centerX + (dx * scale);
        
        const float centerY    = (bottom + top) / 2.0;
        const float dy         = (top - bottom) / 2.0;
        const float newBottom  = centerY - (dy * scale);
        const float newTop     = centerY + (dy * scale);
        
        m_orthographicBounds[0] = newLeft;
        m_orthographicBounds[1] = newRight;
        m_orthographicBounds[2] = newBottom;
        m_orthographicBounds[3] = newTop;
    }
    
    const float sliceCoordinates[3] = {
        m_browserTabContent->getSliceCoordinateParasagittal(),
        m_browserTabContent->getSliceCoordinateCoronal(),
        m_browserTabContent->getSliceCoordinateAxial()
    };
    
    if (m_browserTabContent->isSliceAxialEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(AllSliceViewMode::ALL_NO,
                                      VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::AXIAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceCoronalEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(AllSliceViewMode::ALL_NO,
                                      VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::CORONAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceParasagittalEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(AllSliceViewMode::ALL_NO,
                                      VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
}

/**
 * Draw single or montage volume view slices.
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
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
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewType(const AllSliceViewMode allSliceViewMode,
                                                       const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const int32_t viewport[4])
{
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawVolumeSliceViewTypeMontage(allSliceViewMode,
                                           sliceDrawingType,
                                           sliceProjectionType,
                                           sliceViewPlane,
                                           viewport);
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
        {
            const float sliceCoordinates[3] = {
                m_browserTabContent->getSliceCoordinateParasagittal(),
                m_browserTabContent->getSliceCoordinateCoronal(),
                m_browserTabContent->getSliceCoordinateAxial()
            };
            drawVolumeSliceViewProjection(allSliceViewMode,
                                          sliceDrawingType,
                                          sliceProjectionType,
                                          sliceViewPlane,
                                          sliceCoordinates,
                                          viewport);
        }
            break;
    }
    
}

/**
 * Draw montage slices.
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
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
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewTypeMontage(const AllSliceViewMode allSliceViewMode,
                                                              const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                              const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                              const int32_t viewport[4])
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
    BrainOpenGLFixedPipeline::createSubViewportSizeAndGaps(viewport[3],
                                                           gapsAndMargins->getVolumeMontageVerticalGapForWindow(windowIndex),
                                                           -1,
                                                           numRows,
                                                           vpSizeY,
                                                           verticalMargin);
    
    int32_t vpSizeX          = 0;
    int32_t horizontalMargin = 0;
    BrainOpenGLFixedPipeline::createSubViewportSizeAndGaps(viewport[2],
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
    
    float sliceCoordinates[3] = {
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
                        viewport[0] + vpX,
                        viewport[1] + vpY,
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
                    
                    drawVolumeSliceViewProjection(allSliceViewMode,
                                                  sliceDrawingType,
                                                  sliceProjectionType,
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
    
    /*
     * Draw the axes labels for the montage view
     */
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);


    if (m_browserTabContent->isVolumeAxesCrosshairLabelsDisplayed()) {
        drawAxesCrosshairsOrtho(sliceViewPlane,
                                sliceCoordinates,
                                false,
                                true);
    }
}

/**
 * Draw a slice for either projection mode (oblique, orthogonal)
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
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
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewProjection(const AllSliceViewMode allSliceViewMode,
                                                             const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                             const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                             const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                             const float sliceCoordinates[3],
                                                             const int32_t viewport[4])
{
    bool twoDimSliceViewFlag = false;
    if (m_modelVolume != NULL) {
        twoDimSliceViewFlag = true;
    }
    else if (m_modelWholeBrain != NULL) {
        /* nothing */
    }
    else {
        CaretAssertMessage(0, "Invalid model type.");
    }
    
    if (twoDimSliceViewFlag) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glViewport(viewport[0],
                   viewport[1],
                   viewport[2],
                   viewport[3]);

        /*
         * Set the orthographic projection to fit the slice axis
         */
        setOrthographicProjection(allSliceViewMode,
                                  sliceViewPlane,
                                  viewport);
    }
    
    /*
     * Create the plane equation for the slice
     */
    Plane slicePlane;
    createSlicePlaneEquation(sliceProjectionType,
                             sliceViewPlane,
                             sliceCoordinates,
                             slicePlane);
    CaretAssert(slicePlane.isValidPlane());
    if (slicePlane.isValidPlane() == false) {
        return;
    }
    
    
    if (twoDimSliceViewFlag) {
        /*
         * Set the viewing transformation (camera position)
         */
        setVolumeSliceViewingAndModelingTransformations(sliceProjectionType,
                                                        sliceViewPlane,
                                                        slicePlane);
        /*
         * Only set for two-d view, 3D view (VIEW -> ALL) is set when surfaces are drawn
         */
        m_fixedPipelineDrawing->setupScaleBarDrawingInformation(m_browserTabContent,
                                                                m_orthographicBounds[0],
                                                                m_orthographicBounds[1]);
    }

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
    
    resetIdentification();
    
    GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);

    if (drawVolumeSlicesFlag) {
        /*
         * Disable culling so that both sides of the triangles/quads are drawn.
         */
        glDisable(GL_CULL_FACE);
        
        const bool cullingSliceViewFlag = true;
        switch (sliceProjectionType) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                if (m_modelVolume != NULL) {
                    if (cullingSliceViewFlag) {
                        drawOrthogonalSliceWithCulling(sliceViewPlane,
                                                       sliceCoordinates,
                                                       slicePlane);
                    }
                    else {
                        drawOrthogonalSlice(sliceViewPlane,
                                            sliceCoordinates,
                                            slicePlane);
                    }
                }
                else if (m_modelWholeBrain != NULL) {
                    const bool allOrientationsFlag = true;
                    if (allOrientationsFlag) {
                        drawOrthogonalSliceAllView(sliceViewPlane,
                                                   sliceCoordinates,
                                                   slicePlane);
                    }
                    else {
                        drawOrthogonalSlice(sliceViewPlane,
                                            sliceCoordinates,
                                            slicePlane);
                    }
                }
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                CaretAssert(0);
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                CaretAssert(0);
                break;
        }
    }
    
    /*
     * Draw model space annotaitons on the volume slice
     */
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
        drawIdentificationSymbols(m_volumeDrawInfo[0].volumeFile,
                                  m_volumeDrawInfo[0].mapIndex,
                                  slicePlane,
                                  sliceThickness);
    }
    
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
    m_fixedPipelineDrawing->m_annotationDrawing->drawModelSpaceAnnotationsOnVolumeSlice(&inputs,
                                                                                        slicePlane,
                                                                                        sliceThickness);
    
    m_fixedPipelineDrawing->disableClippingPlanes();
    
    if (cullFaceOn) {
        glEnable(GL_CULL_FACE);
    }
}

/**
 * Find the given orientation in the given volume, find its corresponding
 * dimesion index.
 *
 * @param volume
 *     The volume.
 * @param orientation
 *     The requested orientation
 * @return
 *     Index of dimension containing the orientation or 
 *     Negative if not found.
 */
static int32_t
getDimensionContainingOrientation(const VolumeMappableInterface* volume,
                                  const VolumeSpace::OrientTypes orientation)
{
    VolumeSpace::OrientTypes orients[3];
    volume->getVolumeSpace().getOrientation(orients);
    
    for (int32_t i = 0; i < 3; i++) {
        if (orientation == orients[i]) {
            return i;
        }
    }
    
    return -1;
}
/**
 * Find the given orientation in the given volume, find its corresponding
 * dimesion index.
 *
 * @param volume
 *     The volume.
 * @param orientation1
 *     First requested orientation
 * @param orientation2
 *     Second requested orientation
 * @return
 *     Index of dimension containing the orientation or
 *     Negative if not found.
 */
static int32_t
getDimensionContainingSlicePlane(const VolumeMappableInterface* volume,
                                 const VolumeSliceViewPlaneEnum::Enum slicePlane)
{
    int32_t indexOne = -1;
    int32_t indexTwo = -1;
    switch (slicePlane) {
        case caret::VolumeSliceViewPlaneEnum::ALL:
            break;
        case caret::VolumeSliceViewPlaneEnum::AXIAL:
            indexOne = getDimensionContainingOrientation(volume, VolumeSpace::INFERIOR_TO_SUPERIOR);
            indexTwo = getDimensionContainingOrientation(volume, VolumeSpace::SUPERIOR_TO_INFERIOR);
            break;
        case caret::VolumeSliceViewPlaneEnum::CORONAL:
            indexOne = getDimensionContainingOrientation(volume, VolumeSpace::ANTERIOR_TO_POSTERIOR);
            indexTwo = getDimensionContainingOrientation(volume, VolumeSpace::POSTERIOR_TO_ANTERIOR);
            break;
        case caret::VolumeSliceViewPlaneEnum::PARASAGITTAL:
            indexOne = getDimensionContainingOrientation(volume, VolumeSpace::LEFT_TO_RIGHT);
            indexTwo = getDimensionContainingOrientation(volume, VolumeSpace::RIGHT_TO_LEFT);
            break;
    }
    
    if ((indexOne >= 0)
        && (indexTwo >= 0)) {
        CaretLogSevere("Volume contains same axes orientations in more than one dimension");
    }
    else if ((indexOne < 0)
             && (indexTwo < 0)) {
        /* probably occurs if volume is not plumb */
    }
    else if (indexOne >= 0) {
        return indexOne;
    }
    else if (indexTwo >= 0) {
        return indexTwo;
    }
    
    return -1;
}

/**
 * Provides information on an axis in a volume.
 * Used for drawing volumes in any valid orientation.
 */
class AxisInfo {
public:
    VolumeSliceViewPlaneEnum::Enum axis;
    /** Orientation in axis */
    VolumeSpace::OrientTypes orientation;
    /** index for use with slice indices/dimensions and 'this->axis' */
    int64_t indexIntoIJK;
    /** index for use with XYZ coordinates */
    int64_t indexIntoXYZ;
    /** number of voxels in dimension */
    int64_t numberOfVoxels;
    /** step to move to next voxel */
    int64_t voxelIndexStep;
    /** index of first voxel (either 0 or (numberOfVoxels - 1)) */
    int64_t firstVoxelIndex;
    /** absolute spatial size (mm) of voxel */
    float absoluteVoxelSize;
    /** spatial step to next voxel (signed) */
    float voxelStepSize;
    /** first voxel xyz */
    float firstVoxelXYZ[3];
    /** last voxel xyz */
    float lastVoxelXYZ[3];
    /** true if this axis info is valid */
    bool valid;
    
    AxisInfo() { valid = false; }
    
    void print() {
        AString orientationName;
        switch (orientation) {
            case caret::VolumeSpace::INFERIOR_TO_SUPERIOR:
                orientationName = "INFERIOR_TO_SUPERIOR";
                break;
            case caret::VolumeSpace::POSTERIOR_TO_ANTERIOR:
                orientationName = "POSTERIOR_TO_ANTERIOR";
                break;
            case caret::VolumeSpace::LEFT_TO_RIGHT:
                orientationName = "LEFT_TO_RIGHT";
                break;
            case caret::VolumeSpace::RIGHT_TO_LEFT:
                orientationName = "RIGHT_TO_LEFT";
                break;
            case caret::VolumeSpace::ANTERIOR_TO_POSTERIOR:
                orientationName = "ANTERIOR_TO_POSTERIOR";
                break;
            case caret::VolumeSpace::SUPERIOR_TO_INFERIOR:
                orientationName = "SUPERIOR_TO_INFERIOR";
                break;
        }
        const AString s("Axis: " + VolumeSliceViewPlaneEnum::toName(axis)
                        + "\n   Orientation: " + orientationName
                        + "\n   IJK Index: " + AString::number(indexIntoIJK)
                        + "\n   XYZ Index: " + AString::number(indexIntoXYZ)
                        + "\n   Number of voxels: " + AString::number(numberOfVoxels)
                        + "\n   First Voxel Index: " + AString::number(firstVoxelIndex)
                        + "\n   Voxel Index Step: " + AString::number(voxelIndexStep)
                        + "\n   Abs Voxel Size: " + AString::number(absoluteVoxelSize)
                        + "\n   Voxel Step Size: " + AString::number(voxelStepSize)
                        + "\n   First voxel XYZ: " + AString::fromNumbers(firstVoxelXYZ, 3, ", ")
                        + "\n   Last voxel XYZ: " + AString::fromNumbers(lastVoxelXYZ, 3, ", ")
                        + "\n");
        std::cout << qPrintable(s) << std::endl;
        
    }
};

/**
 * Get the info for the given axis in the given volume file
 *
 * @param volumeFile
 *     The volume file.
 * @param axis
 *     The axis
 * @param startWithMinimumCoordFlag
 *     If true, voxel information will start with voxel that has minimum coordinate.
 * @param axisInfoOut
 *     Output containing information for the given axis
 */
static void
getAxisInfo(const VolumeMappableInterface* volumeFile,
            const VolumeSliceViewPlaneEnum::Enum axis,
            const bool startWithMinimumCoordFlag,
            AxisInfo& axisInfoOut)
{
    /*
     * Data for axis may be in any dimension
     */
    axisInfoOut.axis = axis;
    axisInfoOut.indexIntoIJK = getDimensionContainingSlicePlane(volumeFile, axis);
    if (axisInfoOut.indexIntoIJK < 0) {
        axisInfoOut.valid = false;
        return;
    }
    
    /*
     * X, Y, Z is always indices 0, 1, 2
     */
    switch (axis) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            axisInfoOut.indexIntoXYZ = 2;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            axisInfoOut.indexIntoXYZ = 1;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            axisInfoOut.indexIntoXYZ = 0;
            break;
    }
    VolumeSpace::OrientTypes orientations[3];
    volumeFile->getVolumeSpace().getOrientation(orientations);
    axisInfoOut.orientation = orientations[axisInfoOut.indexIntoXYZ];
    
    std::vector<int64_t> dimArray;
    volumeFile->getDimensions(dimArray);
    
    CaretAssertVectorIndex(dimArray, axisInfoOut.indexIntoIJK);
    axisInfoOut.numberOfVoxels = dimArray[axisInfoOut.indexIntoIJK];

    axisInfoOut.firstVoxelIndex = 0;
    
    int64_t zeroIndices[3] = { 0, 0, 0 };
    volumeFile->indexToSpace(zeroIndices, axisInfoOut.firstVoxelXYZ);
    int64_t oneIndices[3] = { 1, 1, 1};
    float xyzOne[3] = { 1.0, 1.0, 1.0 };
    volumeFile->indexToSpace(oneIndices, xyzOne);
    int64_t lastIndices[3] = { dimArray[0] - 1, dimArray[1] - 1, dimArray[2] - 1 };
    volumeFile->indexToSpace(lastIndices, axisInfoOut.lastVoxelXYZ);

    
    axisInfoOut.voxelStepSize = (xyzOne[axisInfoOut.indexIntoXYZ] - axisInfoOut.firstVoxelXYZ[axisInfoOut.indexIntoXYZ]);
    
    axisInfoOut.absoluteVoxelSize = std::fabs(axisInfoOut.voxelStepSize);
    
    axisInfoOut.voxelIndexStep = 1;
    
    if (startWithMinimumCoordFlag) {
        if (axisInfoOut.voxelStepSize < 0.0) {
            axisInfoOut.firstVoxelIndex = axisInfoOut.numberOfVoxels - 1;
            axisInfoOut.voxelStepSize = -axisInfoOut.voxelStepSize;
            axisInfoOut.voxelIndexStep = -1;
        }
    }
    else {
        if (axisInfoOut.voxelStepSize > 0.0) {
            axisInfoOut.firstVoxelIndex = axisInfoOut.numberOfVoxels - 1;
            axisInfoOut.voxelStepSize = -axisInfoOut.voxelStepSize;
            axisInfoOut.voxelIndexStep = -1;
        }
    }
    
    axisInfoOut.valid = true;
}

/**
 * Draw an orthogonal slice.
 *
 * @param sliceViewingPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param plane
 *    Plane equation for the selected slice.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewingPlane,
                                                   const float sliceCoordinates[3],
                                                   const Plane& plane)
{
    CaretAssert(plane.isValidPlane());
    if (plane.isValidPlane() == false) {
        return;
    }
    
    const int32_t browserTabIndex = m_browserTabContent->getTabNumber();
    const DisplayPropertiesLabels* displayPropertiesLabels = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    const bool allowBlendingFlag(true);
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    if (allowBlendingFlag) {
        BrainOpenGLFixedPipeline::setupBlending(BrainOpenGLFixedPipeline::BlendDataType::VOLUME_ORTHOGONAL_SLICES);
    }
    
    /*
     * Flat shading voxels not interpolated
     */
    glShadeModel(GL_FLAT);
    
    /*
     * Compute coordinate of point in center of first slice
     */
    float selectedSliceCoordinate = 0.0;
    float sliceNormalVector[3] = { 0.0, 0.0, 0.0 };
    plane.getNormalVector(sliceNormalVector);
    switch (sliceViewingPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            selectedSliceCoordinate = sliceCoordinates[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            selectedSliceCoordinate = sliceCoordinates[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            selectedSliceCoordinate = sliceCoordinates[0];
            break;
    }
    
    /*
     * Holds colors for voxels in the slice
     * Outside of loop to minimize allocations
     * It is faster to make one call to
     * NodeAndVoxelColoring::colorScalarsWithPalette() with
     * all voxels in the slice than it is to call it
     * separately for each voxel.
     */
    std::vector<uint8_t> sliceVoxelsRgbaVector;
    
    /*
     * Draw each of the volumes separately so that each
     * is drawn with the correct voxel slices.
     */
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(m_volumeDrawInfo.size());
    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
        const BrainOpenGLFixedPipeline::VolumeDrawInfo& volInfo = m_volumeDrawInfo[iVol];
        const VolumeMappableInterface* volumeFile = volInfo.volumeFile;
        
        /*
         * Find axis that correspond to the axis that are on
         * the screen horizontally and vertically.
         */
        AxisInfo drawLeftToRightInfo;
        AxisInfo drawBottomToTopInfo;
        
        int64_t viewPlaneDimIndex = getDimensionContainingSlicePlane(volumeFile, sliceViewingPlane);
        int64_t sliceViewingPlaneIndexIntoXYZ = -1;
        switch (sliceViewingPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                getAxisInfo(volumeFile, VolumeSliceViewPlaneEnum::PARASAGITTAL, true, drawLeftToRightInfo);
                getAxisInfo(volumeFile, VolumeSliceViewPlaneEnum::CORONAL, true, drawBottomToTopInfo);
                sliceViewingPlaneIndexIntoXYZ = 2;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                getAxisInfo(volumeFile, VolumeSliceViewPlaneEnum::PARASAGITTAL, true, drawLeftToRightInfo);
                getAxisInfo(volumeFile, VolumeSliceViewPlaneEnum::AXIAL, true, drawBottomToTopInfo);
                sliceViewingPlaneIndexIntoXYZ = 1;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                getAxisInfo(volumeFile, VolumeSliceViewPlaneEnum::CORONAL, false, drawLeftToRightInfo);
                getAxisInfo(volumeFile, VolumeSliceViewPlaneEnum::AXIAL, true, drawBottomToTopInfo);
                sliceViewingPlaneIndexIntoXYZ = 0;
                break;
        }
        CaretAssert(drawLeftToRightInfo.valid);
        CaretAssert(drawBottomToTopInfo.valid);
        CaretAssert(viewPlaneDimIndex != drawLeftToRightInfo.indexIntoIJK);
        CaretAssert(viewPlaneDimIndex != drawBottomToTopInfo.indexIntoIJK);
        CaretAssert(drawLeftToRightInfo.indexIntoIJK != drawBottomToTopInfo.indexIntoIJK);
        CaretAssert(sliceViewingPlaneIndexIntoXYZ != drawLeftToRightInfo.indexIntoXYZ);
        CaretAssert(sliceViewingPlaneIndexIntoXYZ != drawBottomToTopInfo.indexIntoXYZ);
        CaretAssert(drawLeftToRightInfo.indexIntoXYZ != drawBottomToTopInfo.indexIntoXYZ);

        /*
         * There must be at least two voxels in both dimensions.
         * If a dimension consists of a single voxel, then it is 
         * likely a single slice volume and our viewpoint is
         * "in" the slice.
         *
         * Without this check the user would see a strange looking
         * line that is one voxel in width
         */
        if ((drawLeftToRightInfo.numberOfVoxels <= 1)
            || (drawBottomToTopInfo.numberOfVoxels <= 1)) {
            continue;
        }
        
        /*
         * Spatial amount to move up row.
         */
        float rowStepXYZ[3] = { 0.0, 0.0, 0.0 };
        rowStepXYZ[drawBottomToTopInfo.indexIntoXYZ] = drawBottomToTopInfo.voxelStepSize;
        
        /*
         * Spatial amount to move right one column.
         */
        float columnStepXYZ[3] = { 0.0, 0.0, 0.0 };
        columnStepXYZ[drawLeftToRightInfo.indexIntoXYZ] = drawLeftToRightInfo.voxelStepSize;

        /*
         * Step in voxel dimensions to move right one column
         */
        int64_t columnStepIJK[3] = { 0, 0, 0 };
        columnStepIJK[drawLeftToRightInfo.indexIntoIJK] = drawLeftToRightInfo.voxelIndexStep;
        
        /*
         * Step in voxel dimensions to move up one row
         */
        int64_t rowStepIJK[3] = { 0, 0, 0 };
        rowStepIJK[drawBottomToTopInfo.indexIntoIJK] = drawBottomToTopInfo.voxelIndexStep;
        
        /*
         * XYZ needs to use regular X=0, Y=1, Z=2 indices
         */
        int64_t sliceVoxelIndices[3] = { 0, 0, 0 };
        float   sliceVoxelXYZ[3]     = { 0.0, 0.0, 0.0 };
        sliceVoxelXYZ[sliceViewingPlaneIndexIntoXYZ] = selectedSliceCoordinate;
        volumeFile->enclosingVoxel(sliceVoxelXYZ[0], sliceVoxelXYZ[1], sliceVoxelXYZ[2],
                                   sliceVoxelIndices[0], sliceVoxelIndices[1], sliceVoxelIndices[2]);
        
        /*
         * Find the index of the slice for drawing and verify that
         * it is a valid slice index.
         */
        const int64_t sliceIndexForDrawing = sliceVoxelIndices[viewPlaneDimIndex];
        std::vector<int64_t> volDim;
        volumeFile->getDimensions(volDim);
        const int64_t maximumAxisSliceIndex = volDim[viewPlaneDimIndex];
        if ((sliceIndexForDrawing < 0)
            || (sliceIndexForDrawing >= maximumAxisSliceIndex)) {
            continue;
        }
        
        /*
         * Voxel indices for first voxel that is drawn at bottom left of screen
         */
        int64_t firstVoxelIJK[3] = { -1, -1, -1 };
        firstVoxelIJK[drawBottomToTopInfo.indexIntoIJK] = drawBottomToTopInfo.firstVoxelIndex;
        firstVoxelIJK[drawLeftToRightInfo.indexIntoIJK] = drawLeftToRightInfo.firstVoxelIndex;
        firstVoxelIJK[viewPlaneDimIndex] = sliceIndexForDrawing;
        
        /*
         * Coordinate of first voxel that is drawn at bottom left of screen
         */
        float startCoordinateXYZ[3] = { 0.0, 0.0, 0.0 };
        volumeFile->indexToSpace(firstVoxelIJK, startCoordinateXYZ);
        startCoordinateXYZ[drawLeftToRightInfo.indexIntoXYZ] -= (drawLeftToRightInfo.voxelStepSize / 2.0);
        startCoordinateXYZ[drawBottomToTopInfo.indexIntoXYZ] -= (drawBottomToTopInfo.voxelStepSize / 2.0);
        startCoordinateXYZ[viewPlaneDimIndex] = selectedSliceCoordinate;
        
        /*
         * Stores RGBA values for each voxel.
         * Use a vector for voxel colors so no worries about memory being freed.
         */
        const int64_t numVoxelsInSlice = drawBottomToTopInfo.numberOfVoxels * drawLeftToRightInfo.numberOfVoxels;
        const int64_t numVoxelsInSliceRGBA = numVoxelsInSlice * 4;
        if (numVoxelsInSliceRGBA > static_cast<int64_t>(sliceVoxelsRgbaVector.size())) {
            sliceVoxelsRgbaVector.resize(numVoxelsInSliceRGBA);
        }
        uint8_t* sliceVoxelsRGBA = &sliceVoxelsRgbaVector[0];
        
        /*
         * Get colors for all voxels in the slice.
         */
        const int64_t validVoxelCount = volumeFile->getVoxelColorsForSliceInMap(volInfo.mapIndex,
                                                                        firstVoxelIJK,
                                                                        rowStepIJK,
                                                                        columnStepIJK,
                                                                        drawBottomToTopInfo.numberOfVoxels,
                                                                        drawLeftToRightInfo.numberOfVoxels,
                                                                        displayGroup,
                                                                        browserTabIndex,
                                                                        sliceVoxelsRGBA);

        /*
         * Is label outline mode?
         */
        if (m_volumeDrawInfo[iVol].mapFile->isMappedWithLabelTable()) {
            int64_t xdim = drawLeftToRightInfo.numberOfVoxels;
            int64_t ydim = drawBottomToTopInfo.numberOfVoxels;
            
            LabelDrawingTypeEnum::Enum labelDrawingType = LabelDrawingTypeEnum::DRAW_FILLED;
            CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
            const CaretMappableDataFile* mapFile = dynamic_cast<const CaretMappableDataFile*>(volumeFile);
            if (mapFile != NULL) {
                if (mapFile->isMappedWithLabelTable()) {
                    const LabelDrawingProperties* props = mapFile->getLabelDrawingProperties();
                    labelDrawingType = props->getDrawingType();
                    outlineColor     = props->getOutlineColor();
                }
            }
            NodeAndVoxelColoring::convertSliceColoringToOutlineMode(sliceVoxelsRGBA,
                                                                    labelDrawingType,
                                                                    outlineColor,
                                                                    xdim,
                                                                    ydim);
        }
        if (m_volumeDrawInfo[iVol].mapFile->isMappedWithPalette()) {
            const int32_t mapIndex = m_volumeDrawInfo[iVol].mapIndex;
            const PaletteColorMapping* pcm = m_volumeDrawInfo[iVol].mapFile->getMapPaletteColorMapping(mapIndex);
            int64_t xdim = drawLeftToRightInfo.numberOfVoxels;
            int64_t ydim = drawBottomToTopInfo.numberOfVoxels;
            NodeAndVoxelColoring::convertPaletteSliceColoringToOutlineMode(sliceVoxelsRGBA,
                                                                           pcm->getThresholdOutlineDrawingMode(),
                                                                           pcm->getThresholdOutlineDrawingColor(),
                                                                           xdim,
                                                                           ydim);
        }
    
        const uint8_t volumeDrawingOpacity = static_cast<uint8_t>(volInfo.opacity * 255.0);
        
        if (m_modelWholeBrain != NULL) {
            /*
             * After the a slice is drawn in ALL view, some layers
             * (volume surface outline) may be drawn in lines.  As the
             * view is rotated, lines will partially appear and disappear
             * due to the lines having the same (extremely close) depth
             * values as the voxel polygons.  OpenGL's Polygon Offset
             * only works with polygons and NOT with lines or points.
             * So, polygon offset cannot be used to move the depth
             * values for the lines and points "a little closer" to
             * the user.  Instead, polygon offset is used to push
             * the underlaying slices "a little bit away" from the
             * user.
             *
             * Resolves WB-414
             */
            const float inverseSliceIndex = numberOfVolumesToDraw - iVol;
            const float factor  = inverseSliceIndex * 1.0 + 1.0;
            const float units  = inverseSliceIndex * 1.0 + 1.0;
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(factor, units);
        }
        
        /*
         * Draw the voxels in the slice.
         */
//        const AString drawMsg("NEW:"
//                              "\n   Axis: " + VolumeSliceViewPlaneEnum::toName(sliceViewingPlane)
//                              + "\n   Start XYZ: " + AString::fromNumbers(startCoordinateXYZ, 3, ",")
//                              + "\n   Row Step: " + AString::fromNumbers(rowStepXYZ, 3, ",")
//                              + "\n   Column Step: " + AString::fromNumbers(columnStepXYZ, 3, ",")
//                              + "\n   Num Cols: " + AString::number(drawLeftToRightInfo.numberOfVoxels)
//                              + "\n   Num Rows: " + AString::number(drawBottomToTopInfo.numberOfVoxels)
//                              + "\n");
//        std::cout << qPrintable(drawMsg) << std::endl;
        drawOrthogonalSliceVoxels(sliceNormalVector,
                                  startCoordinateXYZ,
                                  rowStepXYZ,
                                  columnStepXYZ,
                                  drawLeftToRightInfo.numberOfVoxels,
                                  drawBottomToTopInfo.numberOfVoxels,
                                  sliceVoxelsRgbaVector,
                                  validVoxelCount,
                                  volumeFile,
                                  iVol,
                                  volInfo.mapIndex,
                                  volumeDrawingOpacity);
        glDisable(GL_POLYGON_OFFSET_FILL);
        
        if (m_identificationModeFlag) {
            processIdentification(true);
        }
    }
    
    showBrainordinateHighlightRegionOfInterest(sliceViewingPlane,
                                               sliceCoordinates,
                                               sliceNormalVector);
    glPopAttrib();
    glShadeModel(GL_SMOOTH);
}

/**
 * Show brainordinate highlighting region of interest for the volume slice.
 *
 * @param sliceViewPlane
 *     Slice plane viewed.
 * @param sliceCoordinates
 *     Coordinates of the slice.
 * @param sliceNormalVector
 *     Normal vector for the slice.
 */
void
BrainOpenGLVolumeSliceDrawing::showBrainordinateHighlightRegionOfInterest(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                          const float sliceCoordinates[3],
                                                                          const float sliceNormalVector[3])
{
    const BrainordinateRegionOfInterest* roi = m_brain->getBrainordinateHighlightRegionOfInterest();
    if ( ! roi->hasVolumeVoxels()) {
        return;
    }
    if ( ! roi->isBrainordinateHighlightingEnabled()) {
        return;
    }
    
    const std::vector<float>& voxelXYZ = roi->getVolumeVoxelsXYZ();
    const int64_t numVoxels = static_cast<int64_t>(voxelXYZ.size() / 3);
    if (numVoxels <= 0) {
        return;
    }

    float voxelSize[3];
    roi->getVolumeVoxelSize(voxelSize);
    CaretAssert(voxelSize[0] >= 0.0);
    CaretAssert(voxelSize[1] >= 0.0);
    CaretAssert(voxelSize[2] >= 0.0);
    float halfX = voxelSize[0] / 2.0;
    float halfY = voxelSize[1] / 2.0;
    float halfZ = voxelSize[2] / 2.0;
    
    int64_t axisIndex = 0;
    float sliceMinCoord = 0.0;
    float sliceMaxCoord = 0.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceMinCoord = sliceCoordinates[2] - halfZ;
            sliceMaxCoord = sliceCoordinates[2] + halfZ;
            axisIndex = 2;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceMinCoord = sliceCoordinates[1] - halfY;
            sliceMaxCoord = sliceCoordinates[1] + halfZ;
            axisIndex = 1;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceMinCoord = sliceCoordinates[0] - halfX;
            sliceMaxCoord = sliceCoordinates[0] + halfX;
            axisIndex = 0;
            break;
    }
    
    std::vector<float> quadsXYZ;
    
    for (int64_t i = 0; i < numVoxels; i++) {
        const int64_t i3 = i * 3;
        
        if ((voxelXYZ[i3 + axisIndex] >= sliceMinCoord)
            && (voxelXYZ[i3 + axisIndex] <= sliceMaxCoord)) {
            CaretAssertVectorIndex(voxelXYZ, i3+2);
            const float x = voxelXYZ[i3];
            const float y = voxelXYZ[i3+1];
            const float z = voxelXYZ[i3+2];
            
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    quadsXYZ.push_back(x - halfX);
                    quadsXYZ.push_back(y - halfY);
                    quadsXYZ.push_back(sliceCoordinates[2]);
                    quadsXYZ.push_back(x + halfX);
                    quadsXYZ.push_back(y - halfY);
                    quadsXYZ.push_back(sliceCoordinates[2]);
                    quadsXYZ.push_back(x + halfX);
                    quadsXYZ.push_back(y + halfY);
                    quadsXYZ.push_back(sliceCoordinates[2]);
                    quadsXYZ.push_back(x - halfX);
                    quadsXYZ.push_back(y + halfY);
                    quadsXYZ.push_back(sliceCoordinates[2]);
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    quadsXYZ.push_back(x - halfX);
                    quadsXYZ.push_back(sliceCoordinates[1]);
                    quadsXYZ.push_back(z - halfZ);
                    quadsXYZ.push_back(x + halfX);
                    quadsXYZ.push_back(sliceCoordinates[1]);
                    quadsXYZ.push_back(z - halfZ);
                    quadsXYZ.push_back(x + halfX);
                    quadsXYZ.push_back(sliceCoordinates[1]);
                    quadsXYZ.push_back(z + halfZ);
                    quadsXYZ.push_back(x - halfX);
                    quadsXYZ.push_back(sliceCoordinates[1]);
                    quadsXYZ.push_back(z + halfZ);
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    quadsXYZ.push_back(sliceCoordinates[0]);
                    quadsXYZ.push_back(y + halfY);
                    quadsXYZ.push_back(z - halfZ);
                    quadsXYZ.push_back(sliceCoordinates[0]);
                    quadsXYZ.push_back(y - halfY);
                    quadsXYZ.push_back(z - halfZ);
                    quadsXYZ.push_back(sliceCoordinates[0]);
                    quadsXYZ.push_back(y - halfY);
                    quadsXYZ.push_back(z + halfZ);
                    quadsXYZ.push_back(sliceCoordinates[0]);
                    quadsXYZ.push_back(y + halfY);
                    quadsXYZ.push_back(z + halfZ);
                    break;
            }
        }
    }
    
    const int64_t numVoxelsToDraw = (quadsXYZ.size() / 12);
    CaretAssert((numVoxelsToDraw * 12) == static_cast<int64_t>(quadsXYZ.size()));
    
    const int64_t numCoords = (quadsXYZ.size()) / 3;
    std::vector<float> voxelQuadNormals;
    voxelQuadNormals.reserve(numVoxelsToDraw * 12);
    std::vector<uint8_t> voxelQuadRgba;
    voxelQuadRgba.reserve(numVoxelsToDraw * 16);
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t foregroundColor[4];
    prefs->getBackgroundAndForegroundColors()->getColorForegroundVolumeView(foregroundColor);
    for (int32_t iNormalAndColor = 0; iNormalAndColor < numCoords; iNormalAndColor++) {
        voxelQuadRgba.push_back(foregroundColor[0]);
        voxelQuadRgba.push_back(foregroundColor[1]);
        voxelQuadRgba.push_back(foregroundColor[2]);
        voxelQuadRgba.push_back(255);
        
        voxelQuadNormals.push_back(sliceNormalVector[0]);
        voxelQuadNormals.push_back(sliceNormalVector[1]);
        voxelQuadNormals.push_back(sliceNormalVector[2]);
    }
    
    CaretAssert(quadsXYZ.size() == voxelQuadNormals.size());
    CaretAssert((numVoxelsToDraw * 16) == static_cast<int64_t>(voxelQuadRgba.size()));
    
    BrainOpenGLPrimitiveDrawing::drawQuads(quadsXYZ,
                                           voxelQuadNormals,
                                           voxelQuadRgba);
}

/**
 * Draw identification symbols on volume slice with the given plane.
 *
 * @param volume
 *    The underlay volume
 * @param mapIndex
 *    Index of map in volume file that is being drawn
 * @param plane
 *   The plane equation.
 * @param sliceThickess
 *   Thickness of the slice
 */
void
BrainOpenGLVolumeSliceDrawing::drawIdentificationSymbols(const VolumeMappableInterface* volume,
                                                         const int32_t mapIndex,
                                                         const Plane& plane,
                                                         const float sliceThickness)
{
    drawIdentificationSymbols(m_fixedPipelineDrawing,
                              m_browserTabContent,
                              volume,
                              mapIndex,
                              plane,
                              sliceThickness);
}

/**
 * Draw identification symbols on volume slice with the given plane.
 * @param fixedPipelineDrawing
 *   The fixed pipeline drawing
 * @param browserTabContent
 *    Tab content containing volume
 * @param volume
 *    The underlay volume
 * @param mapIndex
 *    Index of map in volume file that is being drawn
 * @param plane
 *   The plane equation.
 * @param sliceThickess
 *   Thickness of the slice
 */
void
BrainOpenGLVolumeSliceDrawing::drawIdentificationSymbols(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                         BrowserTabContent* browserTabContent,
                                                         const VolumeMappableInterface* volume,
                                                         const int32_t mapIndex,
                                                         const Plane& plane,
                                                         const float sliceThickness)
{
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(volume);
        
    const GraphicsViewport viewport(GraphicsViewport::newInstanceCurrentViewport());
    
    /*
     * Draw volume identification symbols
     */
    BrainOpenGLIdentificationDrawing idDrawing(fixedPipelineDrawing,
                                               fixedPipelineDrawing->m_brain,
                                               browserTabContent,
                                               fixedPipelineDrawing->mode);
    idDrawing.drawVolumeIdentificationSymbols(volume,
                                              mapIndex,
                                              plane,
                                              sliceThickness,
                                              browserTabContent->getScaling(),
                                              viewport.getHeight());
}

/**
 * Draw an orthogonal slice with culling to avoid drawing
 * voxels not visible in the viewport and reduce drawing time.
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param plane
 *    Plane equation for the selected slice.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSliceWithCulling(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                              const float sliceCoordinates[3],
                                                              const Plane& plane)
{
    const int32_t browserTabIndex = m_browserTabContent->getTabNumber();
    const DisplayPropertiesLabels* displayPropertiesLabels = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);

    /*
     * Flat shading voxels not interpolated
     */
    glShadeModel(GL_FLAT);
    
    CaretAssert(plane.isValidPlane());
    if (plane.isValidPlane() == false) {
        return;
    }
    
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    const bool allowBlendingFlag(true);
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    if (allowBlendingFlag) {
        BrainOpenGLFixedPipeline::setupBlending(BrainOpenGLFixedPipeline::BlendDataType::VOLUME_ORTHOGONAL_SLICES);
    }
    
    /*
     * Compute coordinate of point in center of first slice
     */
    float selectedSliceCoordinate = 0.0;
    float sliceNormalVector[3] = { 0.0, 0.0, 0.0 };
    plane.getNormalVector(sliceNormalVector);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            selectedSliceCoordinate = sliceCoordinates[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            selectedSliceCoordinate = sliceCoordinates[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            selectedSliceCoordinate = sliceCoordinates[0];
            break;
    }
    
    /*
     * Holds colors for voxels in the slice
     * Outside of loop to minimize allocations
     * It is faster to make one call to
     * NodeAndVoxelColoring::colorScalarsWithPalette() with
     * all voxels in the slice than it is to call it
     * separately for each voxel.
     */
    std::vector<uint8_t> sliceVoxelsRgbaVector;
    
    /*
     * Draw each of the volumes separately so that each
     * is drawn with the correct voxel slices.
     */
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(m_volumeDrawInfo.size());
    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
        const BrainOpenGLFixedPipeline::VolumeDrawInfo& volInfo = m_volumeDrawInfo[iVol];
        const VolumeMappableInterface* volumeFile = volInfo.volumeFile;
        
        int64_t culledFirstVoxelIJK[3];
        int64_t culledLastVoxelIJK[3];
        float voxelDeltaXYZ[3];

        if ( ! getVolumeDrawingViewDependentCulling(sliceViewPlane,
                                                    selectedSliceCoordinate,
                                                    volumeFile,
                                                    culledFirstVoxelIJK,
                                                    culledLastVoxelIJK,
                                                    voxelDeltaXYZ)) {
            /* volume does not have slice within the culled region */
            continue;
        }
        
        int64_t numVoxelsX = -1, numVoxelsY = -1, numVoxelsZ = -1;
        int64_t sliceIndexForDrawing = -1;
        int64_t dimIJK[3], numMaps, numComponents;
        volumeFile->getDimensions(dimIJK[0], dimIJK[1], dimIJK[2], numMaps, numComponents);
        
        VolumeSpace::OrientTypes orient[3];
        volumeFile->getVolumeSpace().getOrientation(orient);//use the volume's orientation to get the correct dimension for each axis
        
        bool skipDraw = false;
        for (int whichDim = 0; whichDim < 3; ++whichDim)
        {
            int64_t numVoxelsIndex = std::abs(culledLastVoxelIJK[whichDim] - culledFirstVoxelIJK[whichDim]) + 1;
            switch (orient[whichDim])
            {
                case VolumeSpace::LEFT_TO_RIGHT:
                case VolumeSpace::RIGHT_TO_LEFT:
                    numVoxelsX = numVoxelsIndex;
                    if (sliceViewPlane == VolumeSliceViewPlaneEnum::PARASAGITTAL)
                    {
                        sliceIndexForDrawing = culledFirstVoxelIJK[whichDim];
                        if ((sliceIndexForDrawing < 0) || (sliceIndexForDrawing >= dimIJK[whichDim]))
                        {
                            skipDraw = true;
                        }
                    }
                    break;
                case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                    numVoxelsY = numVoxelsIndex;
                    if (sliceViewPlane == VolumeSliceViewPlaneEnum::CORONAL)
                    {
                        sliceIndexForDrawing = culledFirstVoxelIJK[whichDim];
                        if ((sliceIndexForDrawing < 0) || (sliceIndexForDrawing >= dimIJK[whichDim]))
                        {
                            skipDraw = true;
                        }
                    }
                    break;
                case VolumeSpace::INFERIOR_TO_SUPERIOR:
                case VolumeSpace::SUPERIOR_TO_INFERIOR:
                    numVoxelsZ = numVoxelsIndex;
                    if (sliceViewPlane == VolumeSliceViewPlaneEnum::AXIAL)
                    {
                        sliceIndexForDrawing = culledFirstVoxelIJK[whichDim];
                        if ((sliceIndexForDrawing < 0) || (sliceIndexForDrawing >= dimIJK[whichDim]))
                        {
                            skipDraw = true;
                        }
                    }
                    break;
            }
        }
        if (skipDraw) continue;
        
        const int64_t mapIndex = volInfo.mapIndex;
        
        float firstVoxelXYZ[3];
        volumeFile->indexToSpace(culledFirstVoxelIJK[0], culledFirstVoxelIJK[1], culledFirstVoxelIJK[2],
                                 firstVoxelXYZ[0], firstVoxelXYZ[1], firstVoxelXYZ[2]);
        
        const float voxelStepX = voxelDeltaXYZ[0];
        const float voxelStepY = voxelDeltaXYZ[1];
        const float voxelStepZ = voxelDeltaXYZ[2];
        
        int64_t numVoxelsInSlice = 0;
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                numVoxelsInSlice = numVoxelsX * numVoxelsY;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                numVoxelsInSlice = numVoxelsX * numVoxelsZ;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                numVoxelsInSlice = numVoxelsY * numVoxelsZ;
                break;
        }
        
        /*
         * Stores RGBA values for each voxel.
         * Use a vector for voxel colors so no worries about memory being freed.
         */
        const int64_t numVoxelsInSliceRGBA = numVoxelsInSlice * 4;
        if (numVoxelsInSliceRGBA != static_cast<int64_t>(sliceVoxelsRgbaVector.size())) {
            sliceVoxelsRgbaVector.resize(numVoxelsInSliceRGBA);
        }
        uint8_t* sliceVoxelsRGBA = &sliceVoxelsRgbaVector[0];
        
        /*
         * Get colors for all voxels in the slice.
         */
        const int64_t voxelCountXYZ[3] = {
            numVoxelsX,
            numVoxelsY,
            numVoxelsZ
        };//only used to multiply them all together to get an element count for the presumed array size, so just provide them as XYZ
        
        const int64_t validVoxelCount =
           volumeFile->getVoxelColorsForSubSliceInMap(mapIndex,
                                                   sliceViewPlane,
                                                   sliceIndexForDrawing,
                                                   culledFirstVoxelIJK,
                                                   culledLastVoxelIJK,
                                                   voxelCountXYZ,
                                                   displayGroup,
                                                   browserTabIndex,
                                                   sliceVoxelsRGBA);
        
        /*
         * Is label outline mode?
         */
        if (m_volumeDrawInfo[iVol].mapFile->isMappedWithLabelTable()) {
            int64_t xdim = 0;
            int64_t ydim = 0;
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    xdim = numVoxelsX;
                    ydim = numVoxelsY;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    xdim = numVoxelsX;
                    ydim = numVoxelsZ;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    xdim = numVoxelsY;
                    ydim = numVoxelsZ;
                    break;
            }
            
            LabelDrawingTypeEnum::Enum labelDrawingType = LabelDrawingTypeEnum::DRAW_FILLED;
            CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
            const CaretMappableDataFile* mapFile = dynamic_cast<const CaretMappableDataFile*>(volumeFile);
            if (mapFile != NULL) {
                if (mapFile->isMappedWithLabelTable()) {
                    const LabelDrawingProperties* props = mapFile->getLabelDrawingProperties();
                    labelDrawingType = props->getDrawingType();
                    outlineColor     = props->getOutlineColor();
                }
            }
            NodeAndVoxelColoring::convertSliceColoringToOutlineMode(sliceVoxelsRGBA,
                                                                    labelDrawingType,
                                                                    outlineColor,
                                                                    xdim,
                                                                    ydim);
        }
        
        if (m_volumeDrawInfo[iVol].mapFile->isMappedWithPalette()) {
            const int32_t mapIndex = m_volumeDrawInfo[iVol].mapIndex;
            int64_t xdim = 0;
            int64_t ydim = 0;
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    xdim = numVoxelsX;
                    ydim = numVoxelsY;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    xdim = numVoxelsX;
                    ydim = numVoxelsZ;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    xdim = numVoxelsY;
                    ydim = numVoxelsZ;
                    break;
            }
            const PaletteColorMapping* pcm = m_volumeDrawInfo[iVol].mapFile->getMapPaletteColorMapping(mapIndex);
            NodeAndVoxelColoring::convertPaletteSliceColoringToOutlineMode(sliceVoxelsRGBA,
                                                                           pcm->getThresholdOutlineDrawingMode(),
                                                                           pcm->getThresholdOutlineDrawingColor(),
                                                                           xdim,
                                                                           ydim);
        }
        
        const uint8_t volumeDrawingOpacity = static_cast<uint8_t>(volInfo.opacity * 255.0);
        
        /*
         * Setup for drawing the voxels in the slice.
         */
        float startCoordinate[3] = {
            firstVoxelXYZ[0] - (voxelStepX / 2.0f),
            firstVoxelXYZ[1] - (voxelStepY / 2.0f),
            firstVoxelXYZ[2] - (voxelStepZ / 2.0f)
        };
        
        float rowStep[3] = {
            0.0,
            0.0,
            0.0
        };
        
        float columnStep[3] = {
            0.0,
            0.0,
            0.0
        };
        
        
        int64_t numberOfRows = 0;
        int64_t numberOfColumns = 0;
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                rowStep[1] = voxelStepY;
                columnStep[0] = voxelStepX;
                numberOfRows    = numVoxelsY;//WARNING: this is actually length of row, not number of rows, ditto for the rest
                numberOfColumns = numVoxelsX;//leaving it alone for now...
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                rowStep[2] = voxelStepZ;
                columnStep[0] = voxelStepX;
                numberOfRows    = numVoxelsZ;
                numberOfColumns = numVoxelsX;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                rowStep[2] = voxelStepZ;
                columnStep[1] = voxelStepY;
                numberOfRows    = numVoxelsZ;
                numberOfColumns = numVoxelsY;
                break;
        }
        
        if (m_modelWholeBrain != NULL) {
            /*
             * After the a slice is drawn in ALL view, some layers
             * (volume surface outline) may be drawn in lines.  As the
             * view is rotated, lines will partially appear and disappear
             * due to the lines having the same (extremely close) depth
             * values as the voxel polygons.  OpenGL's Polygon Offset
             * only works with polygons and NOT with lines or points.
             * So, polygon offset cannot be used to move the depth
             * values for the lines and points "a little closer" to
             * the user.  Instead, polygon offset is used to push
             * the underlaying slices "a little bit away" from the
             * user.
             *
             * Resolves WB-414
             */
            const float inverseSliceIndex = numberOfVolumesToDraw - iVol;
            const float factor  = inverseSliceIndex * 1.0 + 1.0;
            const float units  = inverseSliceIndex * 1.0 + 1.0;
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(factor, units);
        }
        else {
            /*
             * A layer may be "under" another layer and not be seen.
             * Draw all layers at the selected slice coordinate.
             */
            switch (sliceViewPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    startCoordinate[2] = selectedSliceCoordinate;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    startCoordinate[1] = selectedSliceCoordinate;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    startCoordinate[0] = selectedSliceCoordinate;
                    break;
            }
        }
        
        /*
         * Draw the voxels in the slice.
         */
        drawOrthogonalSliceVoxels(sliceNormalVector,
                                  startCoordinate,
                                  rowStep,
                                  columnStep,
                                  numberOfColumns,
                                  numberOfRows,
                                  sliceVoxelsRgbaVector,
                                  validVoxelCount,
                                  volumeFile,
                                  iVol,
                                  mapIndex,
                                  volumeDrawingOpacity);
        
        glDisable(GL_POLYGON_OFFSET_FILL);
        
        if (m_identificationModeFlag) {
            processIdentification(true);
        }
    }

    showBrainordinateHighlightRegionOfInterest(sliceViewPlane,
                                               sliceCoordinates,
                                               sliceNormalVector);
    
    glPopAttrib();
    glShadeModel(GL_SMOOTH);
}

/**
 * Create the equation for the slice plane
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param montageSliceIndex
 *    Selected montage slice index
 * @param planeOut
 *    OUTPUT plane of slice after transforms.
 */
void
BrainOpenGLVolumeSliceDrawing::createSlicePlaneEquation(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                      const float sliceCoordinates[3],
                                                      Plane& planeOut)
{
    /*
     * Default the slice normal vector to an orthogonal view
     */
    float sliceNormalVector[3] = { 0.0, 0.0, 0.0 };
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceNormalVector[2] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceNormalVector[1] = -1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceNormalVector[0] = -1.0;
            break;
    }
    
    switch (sliceProjectionType) {
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            CaretAssert(0);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            CaretAssert(0);
            break;
    }
    
    Plane plane(sliceNormalVector,
                sliceCoordinates);
    planeOut = plane;
    
    m_lookAtCenter[0] = sliceCoordinates[0];
    m_lookAtCenter[1] = sliceCoordinates[1];
    m_lookAtCenter[2] = sliceCoordinates[2];
}

/**
 * Set the volume slice viewing transformation.  This sets the position and
 * orientation of the camera.
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param plane
 *    Plane equation of selected slice.
 */
void
BrainOpenGLVolumeSliceDrawing::setVolumeSliceViewingAndModelingTransformations(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                             const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                             const Plane& plane)
{
    /*
     * Determine model size in screen Y when viewed
     */
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    const double centerX = boundingBox.getCenterX();
    const double centerY = boundingBox.getCenterY();
    const double centerZ = boundingBox.getCenterZ();
    
    /*
     * Initialize the modelview matrix to the identity matrix
     * This places the camera at the origin, pointing down the
     * negative-Z axis with the up vector set to (0,1,0 =>
     * positive-Y is up).
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    /*
     * Translate to place center of volume at origin
     */
    double moveToCenterX = 0.0;
    double moveToCenterY = 0.0;
    double moveToCenterZ = 0.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssertMessage(0, "Should never get here");
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            moveToCenterX = -centerX;
            moveToCenterY = -centerY;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            moveToCenterX = -centerX;
            moveToCenterY = -centerZ;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            moveToCenterX =  centerY;
            moveToCenterY = -centerZ;
            break;
    }
    
    
    glTranslated(moveToCenterX,
                 moveToCenterY,
                 moveToCenterZ);
    
    /*
     * Set "look at" to origin
     */
    m_lookAtCenter[0] = 0.0;
    m_lookAtCenter[1] = 0.0;
    m_lookAtCenter[2] = 0.0;
    
    /*
     * Since an orthographic projection is used, the camera only needs
     * to be a little bit from the center along the plane's normal vector.
     */
    double planeNormal[3];
    plane.getNormalVector(planeNormal);
    double cameraXYZ[3] = {
        m_lookAtCenter[0] + planeNormal[0] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
        m_lookAtCenter[1] + planeNormal[1] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance,
        m_lookAtCenter[2] + planeNormal[2] * BrainOpenGLFixedPipeline::s_gluLookAtCenterFromEyeOffsetDistance
    };
    
    /*
     * Set the up vector which indices which way is up (screen Y)
     */
    float up[3] = { 0.0, 0.0, 0.0 };
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        case VolumeSliceViewPlaneEnum::AXIAL:
            up[1] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            up[2] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            up[2] = 1.0;
            break;
    }
    
    /*
     * Now set the camera to look at the selected coordinate (center)
     * with the camera offset a little bit from the center.
     * This allows the slice's voxels to be drawn in the actual coordinates.
     */
    gluLookAt(cameraXYZ[0], cameraXYZ[1], cameraXYZ[2],
              m_lookAtCenter[0], m_lookAtCenter[1], m_lookAtCenter[2],
              up[0], up[1], up[2]);
    
    
    const float* userTranslation = m_browserTabContent->getTranslation();
    
    /*
     * Apply user translation
     */
    glTranslatef(userTranslation[0],
                 userTranslation[1],
                 userTranslation[2]);
    
    /*
     * For oblique viewing, the up vector needs to be rotated by the
     * oblique rotation matrix.
     */
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            CaretAssert(0);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            CaretAssert(0);
            break;
    }
    /*
     * Apply user scaling
     */
    const float userScaling = m_browserTabContent->getScaling();
    glScalef(userScaling,
             userScaling,
             userScaling);
    
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 m_viewingMatrix);
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
BrainOpenGLVolumeSliceDrawing::drawLayers(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                        const Plane& slicePlane,
                                        const float sliceCoordinates[3])
{
    bool drawCrosshairsFlag = true;
    bool drawFibersFlag     = true;
    bool drawFociFlag       = true;
    bool drawOutlineFlag    = true;
    
    if (m_modelWholeBrain != NULL) {
        drawCrosshairsFlag = false;
        drawFibersFlag = false;
        drawFociFlag = false;
    }
    
    if ( ! m_identificationModeFlag) {
        if (slicePlane.isValidPlane()) {
            /*
             * Disable culling so that both sides of the triangles/quads are drawn.
             */
            GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
            glDisable(GL_CULL_FACE);
            
            glPushMatrix();
            
            GLboolean depthBufferEnabled = false;
            glGetBooleanv(GL_DEPTH_TEST,
                          &depthBufferEnabled);
            
            /*
             * Use some polygon offset that will adjust the depth values of the
             * layers so that the layers depth values place the layers in front of
             * the volume slice.
             */
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.0, 1.0);
            
            if (drawOutlineFlag) {
                drawSurfaceOutline(m_underlayVolume,
                                   m_modelType,
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
                if (depthBufferEnabled) {
                    glEnable(GL_DEPTH_TEST);
                }
                else {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            if (drawFociFlag) {
                glDisable(GL_DEPTH_TEST);
                drawVolumeSliceFoci(slicePlane);
                if (depthBufferEnabled) {
                    glEnable(GL_DEPTH_TEST);
                }
                else {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            
            glDisable(GL_POLYGON_OFFSET_FILL);
            
            if (drawCrosshairsFlag) {
                glPushMatrix();
                drawAxesCrosshairs(sliceProjectionType,
                                   sliceDrawingType,
                                   sliceViewPlane,
                                   sliceCoordinates);
                glPopMatrix();
                if (depthBufferEnabled) {
                    glEnable(GL_DEPTH_TEST);
                }
                else {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            
            glPopMatrix();
            
            if (cullFaceOn) {
                glEnable(GL_CULL_FACE);
            }
        }
    }
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param underlayVolume
 *    The underlay volume
 * @param modelType
 *    Type of model being drawn.
 * @param sliceProjectionType
 *    Projection type (oblique/orthogonal)
 * @param sliceViewPlane
 *    Slice view plane (axial, coronal, parasagittal)
 * @param sliceXYZ
 *    Coordinates of slices
 * @param plane
 *    Plane of the volume slice on which surface outlines are drawn.
 * @param outlineSet
 *    The surface outline set.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSliceDrawing::drawSurfaceOutline(const VolumeMappableInterface* underlayVolume,
                                                  const ModelTypeEnum::Enum modelType,
                                                  const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                  const float sliceXYZ[3],
                                                  const Plane& plane,
                                                  VolumeSurfaceOutlineSetModel* outlineSet,
                                                  BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                  const bool useNegativePolygonOffsetFlag)
{
    bool drawCachedFlag(true);
    
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            drawCachedFlag = false;
            break;
    }
    /*
     * Code still here to allow comparison with
     * previous algorithm
     */
    if (drawCachedFlag) {
        drawSurfaceOutlineCached(underlayVolume,
                                 modelType,
                                 sliceProjectionType,
                                 sliceViewPlane,
                                 sliceXYZ,
                                 plane,
                                 outlineSet,
                                 fixedPipelineDrawing,
                                 useNegativePolygonOffsetFlag);
    }
    else {
        drawSurfaceOutlineNotCached(modelType,
                                    plane,
                                    outlineSet,
                                    fixedPipelineDrawing,
                                    useNegativePolygonOffsetFlag);
    }
}

/**
 * Draw surface outlines on the volume slices
 *
 * @param underlayVolume
 *    The underlay volume
 * @param modelType
 *    Type of model being drawn.
 * @param sliceProjectionType
      Type of slice projection
 * @param sliceProjectionType
 *    Type of slice projection
 * @param sliceViewPlane
 *    Slice view plane (axial, coronal, parasagittal)
 * @param sliceXYZ
 *    Coordinates of slices
 * @param plane
 *    Plane of the volume slice on which surface outlines are drawn.
 * @param outlineSet
 *    The surface outline set.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSliceDrawing::drawSurfaceOutlineCached(const VolumeMappableInterface* underlayVolume,
                                                        const ModelTypeEnum::Enum modelType,
                                                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                        const float sliceXYZ[3],
                                                        const Plane& plane,
                                                        VolumeSurfaceOutlineSetModel* outlineSet,
                                                        BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                        const bool useNegativePolygonOffsetFlag)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            /*
             * Enable depth so outlines in front or in back
             * of the slices.  Without this the volume surface
             * outlines "behind" the slices are visible and
             * it looks weird
             */
            glEnable(GL_DEPTH_TEST);
            break;
    }
    
    float sliceCoordinate(0.0);
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceCoordinate = sliceXYZ[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceCoordinate = sliceXYZ[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceCoordinate = sliceXYZ[0];
            break;
    }
    
    /*
     * Key for outline cache
     */
    VolumeSurfaceOutlineModelCacheKey outlineCacheKey(underlayVolume,
                                                      sliceViewPlane,
                                                      sliceCoordinate);
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            outlineCacheKey = VolumeSurfaceOutlineModelCacheKey(underlayVolume,
                                                                plane);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            //CaretAssert(0);
            break;
    }
    
    /*
     * Process each surface outline
     * As of 24 May, "zero" is on top so draw in reverse order
     */
    const int32_t numberOfOutlines = outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    for (int32_t io = (numberOfOutlines - 1);
         io >= 0;
         io--) {
        std::vector<GraphicsPrimitive*> contourPrimitives;
        
        VolumeSurfaceOutlineModel* outline = outlineSet->getVolumeSurfaceOutlineModel(io);
        if (outline->isDisplayed()) {
            Surface* surface = outline->getSurface();
            if (surface != NULL) {
                float thicknessPercentage = outline->getThicknessPercentageViewportHeight();
                const float thicknessPixels = outline->getThicknessPixelsObsolete();
                
                /*
                 * Thickness was changed from pixels to percentage viewport height on Feb 02, 2018
                 * If thickness percentage is negative, it was not present in an old
                 * scene so convert pixels to percentage using viewports dimensions
                 */
                if (thicknessPercentage < 0.0f) {
                    thicknessPercentage = GraphicsUtilitiesOpenGL::convertPixelsToPercentageOfViewportHeight(thicknessPixels);
                    if (thicknessPercentage > 0.0f) {
                        outline->setThicknessPercentageViewportHeight(thicknessPercentage);
                    }
                }
                
                if (outline->getOutlineCachePrimitives(underlayVolume,
                                                       outlineCacheKey,
                                                       contourPrimitives)) {
                    /* OK, have cached primitives to draw */
                }
                else {
                    CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
                    int32_t colorSourceBrowserTabIndex = -1;
                    
                    VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = outline->getColorOrTabModel();
                    VolumeSurfaceOutlineColorOrTabModel::Item* selectedColorOrTabItem = colorOrTabModel->getSelectedItem();
                    switch (selectedColorOrTabItem->getItemType()) {
                        case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
                            colorSourceBrowserTabIndex = selectedColorOrTabItem->getBrowserTabIndex();
                            outlineColor = CaretColorEnum::CUSTOM;
                            break;
                        case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
                            outlineColor = selectedColorOrTabItem->getColor();
                            break;
                    }
                    const bool surfaceColorFlag = (colorSourceBrowserTabIndex >= 0);
                    
                    float* nodeColoringRGBA = NULL;
                    if (surfaceColorFlag) {
                        nodeColoringRGBA = fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL,
                                                                                                        surface,
                                                                                                        colorSourceBrowserTabIndex);
                    }
                    
                    SurfacePlaneIntersectionToContour contour(surface,
                                                              plane,
                                                              outlineColor,
                                                              nodeColoringRGBA,
                                                              thicknessPercentage);
                    AString errorMessage;
                    if ( ! contour.createContours(contourPrimitives,
                                                  errorMessage)) {
                        CaretLogSevere(errorMessage);
                    }
                    
                    outline->setOutlineCachePrimitives(underlayVolume,
                                                       outlineCacheKey,
                                                       contourPrimitives);
                }
            }
        }
        
        /**
         * Draw the contours.
         * Note: The primitives are now cached so DO NOT delete them.
         */
        for (auto primitive : contourPrimitives) {
            if (useNegativePolygonOffsetFlag) {
                glPolygonOffset(-1.0, -1.0);
            }
            else {
                glPolygonOffset(1.0, 1.0);
            }
            glEnable(GL_POLYGON_OFFSET_FILL);
            
            GraphicsEngineDataOpenGL::draw(primitive);
            
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
    
    glPopAttrib();
}

/**
 * Draw surface outlines on the volume slices WITHOUT caching
 *
 * @param modelType
 *    Type of model being drawn.
 * @param plane
 *    Plane of the volume slice on which surface outlines are drawn.
 * @param outlineSet
 *    The surface outline set.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing.
 * @param useNegativePolygonOffsetFlag
 *    If true, use a negative offset for polygon offset
 */
void
BrainOpenGLVolumeSliceDrawing::drawSurfaceOutlineNotCached(const ModelTypeEnum::Enum modelType,
                                                           const Plane& plane,
                                                           VolumeSurfaceOutlineSetModel* outlineSet,
                                                           BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                           const bool useNegativePolygonOffsetFlag)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            /*
             * Enable depth so outlines in front or in back
             * of the slices.  Without this the volume surface
             * outlines "behind" the slices are visible and
             * it looks weird
             */
            glEnable(GL_DEPTH_TEST);
            break;
    }
    
    /*
     * Process each surface outline
     * As of 24 May, "zero" is on top so draw in reverse order
     */
    const int32_t numberOfOutlines = outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    for (int32_t io = (numberOfOutlines - 1);
         io >= 0;
         io--) {
        std::vector<GraphicsPrimitive*> contourPrimitives;
        
        VolumeSurfaceOutlineModel* outline = outlineSet->getVolumeSurfaceOutlineModel(io);
        if (outline->isDisplayed()) {
            Surface* surface = outline->getSurface();
            if (surface != NULL) {
                float thicknessPercentage = outline->getThicknessPercentageViewportHeight();
                const float thicknessPixels = outline->getThicknessPixelsObsolete();
                
                /*
                 * Thickness was changed from pixels to percentage viewport height on Feb 02, 2018
                 * If thickness percentage is negative, it was not present in an old
                 * scene so convert pixels to percentage using viewports dimensions
                 */
                if (thicknessPercentage < 0.0f) {
                    thicknessPercentage = GraphicsUtilitiesOpenGL::convertPixelsToPercentageOfViewportHeight(thicknessPixels);
                    if (thicknessPercentage > 0.0f) {
                        outline->setThicknessPercentageViewportHeight(thicknessPercentage);
                    }
                }
                
                CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
                int32_t colorSourceBrowserTabIndex = -1;
                
                VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = outline->getColorOrTabModel();
                VolumeSurfaceOutlineColorOrTabModel::Item* selectedColorOrTabItem = colorOrTabModel->getSelectedItem();
                switch (selectedColorOrTabItem->getItemType()) {
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
                        colorSourceBrowserTabIndex = selectedColorOrTabItem->getBrowserTabIndex();
                        outlineColor = CaretColorEnum::CUSTOM;
                        break;
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
                        outlineColor = selectedColorOrTabItem->getColor();
                        break;
                }
                const bool surfaceColorFlag = (colorSourceBrowserTabIndex >= 0);
                
                float* nodeColoringRGBA = NULL;
                if (surfaceColorFlag) {
                    nodeColoringRGBA = fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL,
                                                                                                    surface,
                                                                                                    colorSourceBrowserTabIndex);
                }
                
                SurfacePlaneIntersectionToContour contour(surface,
                                                          plane,
                                                          outlineColor,
                                                          nodeColoringRGBA,
                                                          thicknessPercentage);
                AString errorMessage;
                if ( ! contour.createContours(contourPrimitives,
                                              errorMessage)) {
                    CaretLogSevere(errorMessage);
                }
            }
        }
        
        /**
         * Draw the contours.
         */
        for (auto primitive : contourPrimitives) {
            if (useNegativePolygonOffsetFlag) {
                glPolygonOffset(-1.0, -1.0);
            }
            else {
                glPolygonOffset(1.0, 1.0);
            }
            glEnable(GL_POLYGON_OFFSET_FILL);
            
            GraphicsEngineDataOpenGL::draw(primitive);
            delete primitive;
            
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }
    
    glPopAttrib();
}


/**
 * Draw foci on volume slice.
 *
 * @param plane
 *   Plane of the volume slice on which surface outlines are drawn.
 */
void
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceFoci(const Plane& plane)
{
    SelectionItemFocusVolume* idFocus = m_brain->getSelectionManager()->getVolumeFocusIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (idFocus->isEnabledForSelection()) {
                isSelect = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    VolumeMappableInterface* underlayVolume = m_volumeDrawInfo[0].volumeFile;
    float minVoxelSpacing;
    float maxVoxelSpacing;
    if ( ! getMinMaxVoxelSpacing(underlayVolume, minVoxelSpacing, maxVoxelSpacing)) {
        return;
    }
    
    const float sliceThickness = maxVoxelSpacing;
    const float halfSliceThickness = sliceThickness * 0.5;
    
    
    const DisplayPropertiesFoci* fociDisplayProperties = m_brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = fociDisplayProperties->getDisplayGroupForTab(m_fixedPipelineDrawing->windowTabIndex);
    
    const CaretColorEnum::Enum caretColor = fociDisplayProperties->getStandardColorType(displayGroup,
                                                                                        m_fixedPipelineDrawing->windowTabIndex);
    float caretColorRGBA[4];
    CaretColorEnum::toRGBAFloat(caretColor, caretColorRGBA);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           m_fixedPipelineDrawing->windowTabIndex) == false) {
        return;
    }
//    const float focusDiameter = fociDisplayProperties->getFociSizeMillimeters(displayGroup,
//                                                                   m_fixedPipelineDrawing->windowTabIndex);
    float focusDiameter(1.0);
    switch (fociDisplayProperties->getFociSymbolSizeType(displayGroup,
                                                         m_fixedPipelineDrawing->windowTabIndex)) {
        case IdentificationSymbolSizeTypeEnum::MILLIMETERS:
            focusDiameter = fociDisplayProperties->getFociSizeMillimeters(displayGroup,
                                                                          m_fixedPipelineDrawing->windowTabIndex);
            break;
        case IdentificationSymbolSizeTypeEnum::PERCENTAGE:
        {
            focusDiameter = fociDisplayProperties->getFociSizePercentage(displayGroup,
                                                                         m_fixedPipelineDrawing->windowTabIndex);
            BoundingBox boundingBox;
            underlayVolume->getVoxelSpaceBoundingBox(boundingBox);
            const float maxDimension = boundingBox.getMaximumDifferenceOfXYZ();
            focusDiameter = maxDimension * (focusDiameter / 100.0);
        }
            break;
    }
    
    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                                  m_fixedPipelineDrawing->windowTabIndex);
    
    bool drawAsSpheres = false;
    switch (fociDisplayProperties->getDrawingType(displayGroup,
                                                  m_fixedPipelineDrawing->windowTabIndex)) {
        case FociDrawingTypeEnum::DRAW_AS_SPHERES:
            drawAsSpheres = true;
            break;
        case FociDrawingTypeEnum::DRAW_AS_SQUARES:
            break;
    }
    
    /*
     * Process each foci file
     */
    const int32_t numberOfFociFiles = m_brain->getNumberOfFociFiles();
    for (int32_t iFile = 0; iFile < numberOfFociFiles; iFile++) {
        FociFile* fociFile = m_brain->getFociFile(iFile);
        
        const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              m_fixedPipelineDrawing->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = fociFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable = fociFile->getNameColorTable();
        
        const int32_t numFoci = fociFile->getNumberOfFoci();
        
        for (int32_t j = 0; j < numFoci; j++) {
            Focus* focus = fociFile->getFocus(j);
            
            const GroupAndNameHierarchyItem* groupNameItem = focus->getGroupNameSelectionItem();
            if (groupNameItem != NULL) {
                if (groupNameItem->isSelected(displayGroup,
                                              m_fixedPipelineDrawing->windowTabIndex) == false) {
                    continue;
                }
            }
            
            float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
            switch (fociColoringType) {
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_CLASS:
                    if (focus->isClassRgbaValid() == false) {
                        const GiftiLabel* colorLabel = classColorTable->getLabelBestMatching(focus->getClassName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            focus->setClassRgba(rgba);
                        }
                        else {
                            focus->setClassRgba(rgba);
                        }
                    }
                    focus->getClassRgba(rgba);
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_STANDARD_COLOR:
                    rgba[0] = caretColorRGBA[0];
                    rgba[1] = caretColorRGBA[1];
                    rgba[2] = caretColorRGBA[2];
                    rgba[3] = caretColorRGBA[3];
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME:
                    if (focus->isNameRgbaValid() == false) {
                        const GiftiLabel* colorLabel = nameColorTable->getLabelBestMatching(focus->getName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgba);
                            focus->setNameRgba(rgba);
                        }
                        else {
                            focus->setNameRgba(rgba);
                        }
                    }
                    focus->getNameRgba(rgba);
                    break;
            }

            /*
             * Some label tables may have alpha at zero, so correct it
             */
            rgba[3] = 1.0;

            const int32_t numProjections = focus->getNumberOfProjections();
            for (int32_t k = 0; k < numProjections; k++) {
                const SurfaceProjectedItem* spi = focus->getProjection(k);
                if (spi->isVolumeXYZValid()) {
                    float xyz[3];
                    spi->getVolumeXYZ(xyz);
                    
                    bool drawIt = false;
                    if (plane.absoluteDistanceToPlane(xyz) < halfSliceThickness) {
                        drawIt = true;
                    }
                    
                    if (drawIt) {
                        glPushMatrix();
                        glTranslatef(xyz[0], xyz[1], xyz[2]);
                        if (isSelect) {
                            uint8_t idRGBA[4];
                            m_fixedPipelineDrawing->colorIdentification->addItem(idRGBA,
                                                                                 SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                                                 iFile, // file index
                                                                                 j, // focus index
                                                                                 k);// projection index
                            idRGBA[3] = 255;
                            if (drawAsSpheres) {
                                m_fixedPipelineDrawing->drawSphereWithDiameter(idRGBA,
                                                                               focusDiameter);
                            }
                            else {
                                glColor4ubv(idRGBA);
                                drawSquare(focusDiameter);
                            }
                        }
                        else {
                            if (drawAsSpheres) {
                                m_fixedPipelineDrawing->drawSphereWithDiameter(rgba,
                                                                               focusDiameter);
                            }
                            else {
                                glColor3fv(rgba);
                                drawSquare(focusDiameter);
                            }
                        }
                        glPopMatrix();
                    }
                }
            }
        }
    }
    
    if (isSelect) {
        int32_t fociFileIndex = -1;
        int32_t focusIndex = -1;
        int32_t focusProjectionIndex = -1;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                           m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           fociFileIndex,
                                                           focusIndex,
                                                           focusProjectionIndex,
                                                           depth);
        if (fociFileIndex >= 0) {
            if (idFocus->isOtherScreenDepthCloserToViewer(depth)) {
                Focus* focus = m_brain->getFociFile(fociFileIndex)->getFocus(focusIndex);
                idFocus->setBrain(m_brain);
                idFocus->setFocus(focus);
                idFocus->setFociFile(m_brain->getFociFile(fociFileIndex));
                idFocus->setFocusIndex(focusIndex);
                idFocus->setFocusProjectionIndex(focusProjectionIndex);
                idFocus->setVolumeFile(underlayVolume);
                idFocus->setScreenDepth(depth);
                float xyz[3];
                const SurfaceProjectedItem* spi = focus->getProjection(focusProjectionIndex);
                spi->getVolumeXYZ(xyz);
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(idFocus, xyz);
                CaretLogFine("Selected Volume Focus Identification Symbol: " + QString::number(focusIndex));
            }
        }
    }
}

/**
 * Draw the axes crosshairs.
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceDrawingType
 *    Type of slice drawing (montage, single)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param sliceCoordinates
 *    Coordinates of the selected slices.
 */
void
BrainOpenGLVolumeSliceDrawing::drawAxesCrosshairs(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                const float sliceCoordinates[3])
{
    const bool drawCrosshairsFlag = m_browserTabContent->isVolumeAxesCrosshairsDisplayed();
    bool drawCrosshairLabelsFlag = m_browserTabContent->isVolumeAxesCrosshairLabelsDisplayed();
    
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawCrosshairLabelsFlag = false;
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
            break;
    }
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            drawAxesCrosshairsOrtho(sliceViewPlane,
                                    sliceCoordinates,
                                    drawCrosshairsFlag,
                                    drawCrosshairLabelsFlag);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
        {
            glPushMatrix();
            glLoadIdentity();
            
            double mv[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, mv);
            Matrix4x4 mvm;
            mvm.setMatrixFromOpenGL(mv);
            
            float trans[3];
            m_browserTabContent->getTranslation(trans);
            glTranslatef(trans[0], trans[1], trans[2]);
            drawAxesCrosshairsOrtho(sliceViewPlane,
                                    sliceCoordinates,
                                    drawCrosshairsFlag,
                                    drawCrosshairLabelsFlag);
            glPopMatrix();
        }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            CaretAssert(0);
            break;
    }
}

/**
 * Draw the axes crosshairs for an orthogonal slice.
 *
 * @param sliceViewPlane
 *    The slice plane view.
 * @param sliceCoordinatesIn
 *    Coordinates of the selected slices.
 * @param drawCrosshairsFlag
 *    If true, draw the crosshairs.
 * @param drawCrosshairLabelsFlag
 *    If true, draw the crosshair labels.
 */
void
BrainOpenGLVolumeSliceDrawing::drawAxesCrosshairsOrtho(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                       const float sliceCoordinatesIn[3],
                                                       const bool drawCrosshairsFlag,
                                                       const bool drawCrosshairLabelsFlag)
{
    const float gapPercentViewportHeight = SessionManager::get()->getCaretPreferences()->getVolumeCrosshairGap();
    const float gapMM = GraphicsUtilitiesOpenGL::convertPercentageOfViewportHeightToMillimeters(gapPercentViewportHeight);
    
    std::array<float, 3> sliceCoordinates { sliceCoordinatesIn[0], sliceCoordinatesIn[1], sliceCoordinatesIn[2] };

    GLboolean depthEnabled = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST,
                  &depthEnabled);
    glDisable(GL_DEPTH_TEST);
    
    const float bigValue = 10000.0 + gapMM;
    
    std::array<float, 3> horizontalAxisPosStartXYZ = sliceCoordinates;
    std::array<float, 3> horizontalAxisPosEndXYZ = sliceCoordinates;
    std::array<float, 3> verticalAxisPosStartXYZ = sliceCoordinates;
    std::array<float, 3> verticalAxisPosEndXYZ = sliceCoordinates;
    
    std::array<float, 3> horizontalAxisNegStartXYZ = horizontalAxisPosStartXYZ;
    std::array<float, 3> horizontalAxisNegEndXYZ = horizontalAxisPosEndXYZ;
    std::array<float, 3> verticalAxisNegStartXYZ = verticalAxisPosStartXYZ;
    std::array<float, 3> verticalAxisNegEndXYZ = verticalAxisPosEndXYZ;

    float axialRGBA[4];
    getAxesColor(VolumeSliceViewPlaneEnum::AXIAL,
                 axialRGBA);
    
    float coronalRGBA[4];
    getAxesColor(VolumeSliceViewPlaneEnum::CORONAL,
                 coronalRGBA);
    
    float paraRGBA[4];
    getAxesColor(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                 paraRGBA);
    
    AString horizontalLeftText  = "";
    AString horizontalRightText = "";
    AString verticalBottomText  = "";
    AString verticalTopText     = "";
    
    float* horizontalAxisRGBA   = axialRGBA;
    float* verticalAxisRGBA     = axialRGBA;
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            horizontalLeftText   = "L";
            horizontalRightText  = "R";
            horizontalAxisRGBA = coronalRGBA;
            horizontalAxisPosStartXYZ[0] += gapMM;
            horizontalAxisPosEndXYZ[0]   += bigValue;
            horizontalAxisNegStartXYZ[0] -= gapMM;
            horizontalAxisNegEndXYZ[0]   -= bigValue;
            
            verticalBottomText = "P";
            verticalTopText    = "A";
            verticalAxisRGBA = paraRGBA;
            verticalAxisPosStartXYZ[1] += gapMM;
            verticalAxisPosEndXYZ[1]   += bigValue;
            verticalAxisNegStartXYZ[1] -= gapMM;
            verticalAxisNegEndXYZ[1]   -= bigValue;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            horizontalLeftText   = "L";
            horizontalRightText  = "R";
            horizontalAxisRGBA = axialRGBA;
            horizontalAxisPosStartXYZ[0] += gapMM;
            horizontalAxisPosEndXYZ[0]   += bigValue;
            horizontalAxisNegStartXYZ[0] -= gapMM;
            horizontalAxisNegEndXYZ[0]   -= bigValue;

            verticalBottomText = "I";
            verticalTopText    = "S";
            verticalAxisRGBA = paraRGBA;
            verticalAxisPosStartXYZ[2] += gapMM;
            verticalAxisPosEndXYZ[2]   += bigValue;
            verticalAxisNegStartXYZ[2] -= gapMM;
            verticalAxisNegEndXYZ[2]   -= bigValue;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            horizontalLeftText   = "A";
            horizontalRightText  = "P";
            horizontalAxisRGBA = axialRGBA;
            horizontalAxisPosStartXYZ[1] += gapMM;
            horizontalAxisPosEndXYZ[1]   += bigValue;
            horizontalAxisNegStartXYZ[1] -= gapMM;
            horizontalAxisNegEndXYZ[1]   -= bigValue;
            
            verticalBottomText = "I";
            verticalTopText    = "S";
            verticalAxisRGBA = coronalRGBA;
            verticalAxisPosStartXYZ[2] += gapMM;
            verticalAxisPosEndXYZ[2]   += bigValue;
            verticalAxisNegStartXYZ[2] -= gapMM;
            verticalAxisNegEndXYZ[2]   -= bigValue;
            break;
    }
    
    /*
     * Offset text labels be a percentage of viewort width/height
     */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    const int textOffsetX = viewport[2] * 0.01f;
    const int textOffsetY = viewport[3] * 0.01f;
    const int textLeftWindowXY[2] = {
        textOffsetX,
        (viewport[3] / 2)
    };
    const int textRightWindowXY[2] = {
        viewport[2] - textOffsetX,
        (viewport[3] / 2)
    };
    const int textBottomWindowXY[2] = {
        viewport[2] / 2,
        textOffsetY
    };
    const int textTopWindowXY[2] = {
        (viewport[2] / 2),
        viewport[3] - textOffsetY
    };
    
    /*
     * Crosshairs
     */
    if (drawCrosshairsFlag) {
        std::unique_ptr<GraphicsPrimitiveV3fC4f> xhairPrimitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
        xhairPrimitive->addVertex(&horizontalAxisPosStartXYZ[0], horizontalAxisRGBA);
        xhairPrimitive->addVertex(&horizontalAxisPosEndXYZ[0], horizontalAxisRGBA);
        xhairPrimitive->addVertex(&horizontalAxisNegStartXYZ[0], horizontalAxisRGBA);
        xhairPrimitive->addVertex(&horizontalAxisNegEndXYZ[0], horizontalAxisRGBA);

        xhairPrimitive->addVertex(&verticalAxisPosStartXYZ[0], verticalAxisRGBA);
        xhairPrimitive->addVertex(&verticalAxisPosEndXYZ[0], verticalAxisRGBA);
        xhairPrimitive->addVertex(&verticalAxisNegStartXYZ[0], verticalAxisRGBA);
        xhairPrimitive->addVertex(&verticalAxisNegEndXYZ[0], verticalAxisRGBA);
        xhairPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);

        GraphicsEngineDataOpenGL::draw(xhairPrimitive.get());
    }
    
    if (drawCrosshairLabelsFlag) {
        uint8_t backgroundRGBA[4] = {
            m_fixedPipelineDrawing->m_backgroundColorByte[0],
            m_fixedPipelineDrawing->m_backgroundColorByte[1],
            m_fixedPipelineDrawing->m_backgroundColorByte[2],
            m_fixedPipelineDrawing->m_backgroundColorByte[3]
        };
        
        AnnotationPercentSizeText annotationText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        annotationText.setBoldStyleEnabled(true);
        annotationText.setFontPercentViewportSize(5.0f);
        annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
        annotationText.setTextColor(CaretColorEnum::CUSTOM);
        annotationText.setCustomTextColor(horizontalAxisRGBA);
        annotationText.setCustomBackgroundColor(backgroundRGBA);
        
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        annotationText.setText(horizontalLeftText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textLeftWindowXY[0],
                                                         textLeftWindowXY[1],
                                                         annotationText);
        
        annotationText.setText(horizontalRightText);
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textRightWindowXY[0],
                                                         textRightWindowXY[1],
                                                         annotationText);
        
        annotationText.setCustomTextColor(verticalAxisRGBA);
        
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
        annotationText.setText(verticalBottomText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textBottomWindowXY[0],
                                                         textBottomWindowXY[1],
                                                         annotationText);
        
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        annotationText.setText(verticalTopText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textTopWindowXY[0],
                                                         textTopWindowXY[1],
                                                         annotationText);
    }
    
    if (depthEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
}

/**
 * Get the RGBA coloring for a slice view plane.
 *
 * @param sliceViewPlane
 *    The slice view plane.
 * @param rgbaOut
 *    Output colors ranging 0.0 to 1.0
 */
void
BrainOpenGLVolumeSliceDrawing::getAxesColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            float rgbaOut[4]) const
{
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 1.0;
            rgbaOut[3] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 1.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            rgbaOut[0] = 1.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 1.0;
            break;
    }
}

/**
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param size
 *     Size of square.
 */
void
BrainOpenGLVolumeSliceDrawing::drawSquare(const float size)
{
    const float length = size * 0.5;
    
    /*
     * Draw both front and back side since in some instances,
     * such as surface montage, we are viweing from the far
     * side (from back of monitor)
     */
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f( length, -length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f( length, -length, 0.0);
    glEnd();
}

/**
 * Get the minimum and maximum distance between adjacent voxels in all
 * slices planes.  Output spacing value are always non-negative even if
 * a right-to-left orientation.
 *
 * @param volume
 *    Volume for which min/max spacing is requested.
 * @param minSpacingOut
 *    Output minimum spacing.
 * @param maxSpacingOut
 *    Output maximum spacing.
 * @return
 *    True if min and max spacing are greater than zero.
 */
bool
BrainOpenGLVolumeSliceDrawing::getMinMaxVoxelSpacing(const VolumeMappableInterface* volume,
                                                     float& minSpacingOut,
                                                     float& maxSpacingOut) const
{
    CaretAssert(volume);
    
    float originX, originY, originZ;
    float x1, y1, z1;
    volume->indexToSpace(0, 0, 0, originX, originY, originZ);
    volume->indexToSpace(1, 1, 1, x1, y1, z1);
    const float dx = std::fabs(x1 - originX);
    const float dy = std::fabs(y1 - originY);
    const float dz = std::fabs(z1 - originZ);
    
    minSpacingOut = std::min(std::min(dx, dy), dz);
    maxSpacingOut = std::max(std::max(dx, dy), dz);
    
    if ((minSpacingOut > 0.0)
        && (maxSpacingOut > 0.0)) {
        return true;
    }
    return false;
}

/**
 * Set the orthographic projection.
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param boundingBox
 *    Bounding box for all volume.
 * @param viewport
 *    The viewport.
 * @param orthographicBoundsOut
 *    Output containing the orthographic bounds used for orthographic projection.
 */
void
BrainOpenGLVolumeSliceDrawing::setOrthographicProjection(const AllSliceViewMode allSliceViewMode,
                                                         const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                         const BoundingBox& boundingBox,
                                                         const float zoomFactor,
                                                         const int viewport[4],
                                                         double orthographicBoundsOut[6])
{
    /*
     * Set top and bottom to the min/max coordinate
     * that runs vertically on the screen
     */
    double modelTop = 200.0;
    double modelBottom = -200.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssertMessage(0, "Should never get here");
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            modelTop = boundingBox.getMaxY();
            modelBottom = boundingBox.getMinY();
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            modelTop = boundingBox.getMaxZ();
            modelBottom = boundingBox.getMinZ();
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            modelTop = boundingBox.getMaxZ();
            modelBottom = boundingBox.getMinZ();
            break;
    }
    
    
    /*
     * WB-677 The zoom of axial, coronal, and sagittal views is not the same
     */
    switch (allSliceViewMode) {
        case AllSliceViewMode::ALL_YES:
        {
            /*
             * Parasagittal and Coronal Views have Brain's Z-axis in Screen Y
             * Axial View has Brain's Y-axis in Screen Y
             * So, use maximum of Brain's Y- and Z-axes for sizing height of slice
             * so that voxels are same size for each slice in each axis view
             */
            const float maxRangeYZ = std::max(boundingBox.getDifferenceY(),
                                              boundingBox.getDifferenceZ());
            const double halfHeight = maxRangeYZ / 2.0;
            const double middle     = (modelBottom + modelTop) / 2.0;
            modelTop    = middle + halfHeight;
            modelBottom = middle - halfHeight;
        }
            break;
        case AllSliceViewMode::ALL_NO:
            break;
    }
    
    /*
     * Scale ratio makes region slightly larger than model
     */
    const double zoom = zoomFactor; 
    double scaleRatio = (1.0 / 0.98);
    if (zoom > 0.0) {
        scaleRatio /= zoom;
    }
    modelTop *= scaleRatio;
    modelBottom *= scaleRatio;
    
    /*
     * Determine aspect ratio of viewport
     */
    const double viewportWidth = viewport[2];
    const double viewportHeight = viewport[3];
    const double aspectRatio = (viewportWidth
                                / viewportHeight);
    
    /*
     * Set bounds of orthographic projection
     */
    const double halfModelY = ((modelTop - modelBottom) / 2.0);
    const double orthoBottom = -halfModelY;
    const double orthoTop    =  halfModelY;
    const double orthoLeft   = -halfModelY * aspectRatio;
    const double orthoRight  =  halfModelY * aspectRatio;
    const double nearDepth = -1000.0;
    const double farDepth  =  1000.0;
    orthographicBoundsOut[0] = orthoLeft;
    orthographicBoundsOut[1] = orthoRight;
    orthographicBoundsOut[2] = orthoBottom;
    orthographicBoundsOut[3] = orthoTop;
    orthographicBoundsOut[4] = nearDepth;
    orthographicBoundsOut[5] = farDepth;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(orthographicBoundsOut[0],
            orthographicBoundsOut[1],
            orthographicBoundsOut[2],
            orthographicBoundsOut[3],
            orthographicBoundsOut[4],
            orthographicBoundsOut[5]);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set the orthographic projection.
 *
 * @param allSliceViewMode
 *    Indicates drawing of ALL slices volume view (axial, coronal, parasagittal in one view)
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeSliceDrawing::setOrthographicProjection(const AllSliceViewMode allSliceViewMode,
                                                         const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                         const int viewport[4])
{
    /*
     * Determine model size in screen Y when viewed
     */
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    
    const float zoomFactor = 1.0f;
    setOrthographicProjection(allSliceViewMode,
                              sliceViewPlane,
                              boundingBox,
                              zoomFactor,
                              viewport,
                              m_orthographicBounds);
}

/**
 * Draw the voxels in an orthogonal slice.
 *
 * @param sliceNormalVector
 *    Normal vector of the slice plane.
 * @param coordinate
 *    Coordinate of first voxel in the slice (bottom left as begin viewed)
 * @param rowStep
 *    Three-dimensional step to next row.
 * @param columnStep
 *    Three-dimensional step to next column.
 * @param numberOfColumns
 *    Number of columns in the slice.
 * @param numberOfRows
 *    Number of rows in the slice.
 * @param sliceRGBA
 *    RGBA coloring for voxels in the slice.
 * @param validVoxelCount
 *    Number of voxels with valid coloring
 * @param volumeInterface
 *    Index of the volume being drawn.
 * @param volumeIndex
 *    Selected map in the volume being drawn.
 * @param mapIndex
 *    Selected map in the volume being drawn.
 * @param sliceOpacity
 *    Opacity from the overlay.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSliceVoxels(const float sliceNormalVector[3],
                                                         const float coordinate[3],
                                                         const float rowStep[3],
                                                         const float columnStep[3],
                                                         const int64_t numberOfColumns,
                                                         const int64_t numberOfRows,
                                                         const std::vector<uint8_t>& sliceRGBA,
                                                         const int64_t validVoxelCount,
                                                         const VolumeMappableInterface* volumeInterface,
                                                         const int32_t volumeIndex,
                                                         const int32_t mapIndex,
                                                         const uint8_t sliceOpacity)
{
    if (validVoxelCount <= 0) {
        if (m_identificationModeFlag) {
            /* voxels are always drawn in identification mode */
        }
        else {
            return;
        }
    }
    
    /*
     * There are two ways to draw the voxels.
     *
     * Quad Indices: This method submits each vertex (coordinate, normal, rgba)
     * one time BUT it submits EVERY vertex in the slice.  Note that the vertex
     * is shared by four voxels (except along an edge).  For each valid voxel, 
     * it submits the indices of the voxel's four vertices.  This method is
     * efficient when many voxels are drawn since each vertex is submitted to
     * OpenGL one time and is shared by up to four voxels.  However, when
     * only a few voxels are drawn, many unused vertices are submitted.
     *
     * Single Quads: For each voxel, this method submits the four vertices
     * (coordinate, normal, rgba) for each voxel drawn.  Even though adjacent
     * voxels share vertices, the vertices are submitted for each voxel.  When
     * only a small number of voxels in the slice are drawn, this method is 
     * efficient since only the needed vertex data is submitted.  However,
     * when many voxels are drawn, many vertices are duplicated making the
     * drawing inefficient.
     *
     * So, estimate the bytes that are passed to OpenGL for each drawing
     * mode and choose the drawing mode that requires the smallest number
     * of bytes.
     */
    
    /*
     * Each vertex requires 28 bytes
     *   (3 float xyz, 3 float normal xyz + 4 bytes color).
     *
     * Single quads uses four vertices per quad.
     *
     * Index quads requires four 4-byte ints for the quad's indices.
     */
    const int64_t bytesPerVertex = 28;
    const int64_t totalVertexBytes = ((numberOfColumns + 1)
                                      * (numberOfRows + 1)
                                      * bytesPerVertex);
    const int64_t numberOfVoxels = numberOfColumns * numberOfRows;
    const int64_t singleQuadBytes = (numberOfVoxels * bytesPerVertex * 4);
    const int64_t indexQuadBytes = (totalVertexBytes
                                    + (16 * numberOfVoxels));
    
    bool drawWithQuadIndicesFlag = false;
    if (indexQuadBytes < singleQuadBytes) {
        drawWithQuadIndicesFlag = true;
    }
    
    GLboolean blendOn = GL_FALSE;
    glGetBooleanv(GL_BLEND, &blendOn);
    
    if (drawWithQuadIndicesFlag) {
        drawOrthogonalSliceVoxelsQuadIndicesAndStrips(sliceNormalVector,
                                                      coordinate,
                                                      rowStep,
                                                      columnStep,
                                                      numberOfColumns,
                                                      numberOfRows,
                                                      sliceRGBA,
                                                      volumeInterface,
                                                      volumeIndex,
                                                      mapIndex,
                                                      sliceOpacity);
    }
    else {
        drawOrthogonalSliceVoxelsSingleQuads(sliceNormalVector,
                                                      coordinate,
                                                      rowStep,
                                                      columnStep,
                                                      numberOfColumns,
                                                      numberOfRows,
                                                      sliceRGBA,
                                                      volumeInterface,
                                                      volumeIndex,
                                                      mapIndex,
                                                      sliceOpacity);
    }
    glGetBooleanv(GL_BLEND, &blendOn);

}

/**
 * Draw the voxels in an orthogonal slice with single quads.
 *
 * Four coordinates, normals, and colors are sent to OpenGL for each 
 * quad that is drawn.  This may be efficent when only a few voxels
 * are drawn but very inefficient when many voxels are drawn.
 *
 * @param sliceNormalVector
 *    Normal vector of the slice plane.
 * @param coordinate
 *    Coordinate of first voxel in the slice (bottom left as begin viewed)
 * @param rowStep
 *    Three-dimensional step to next row.
 * @param columnStep
 *    Three-dimensional step to next column.
 * @param numberOfColumns
 *    Number of columns in the slice.
 * @param numberOfRows
 *    Number of rows in the slice.
 * @param sliceRGBA
 *    RGBA coloring for voxels in the slice.
 * @param volumeInterface
 *    Index of the volume being drawn.
 * @param volumeIndex
 *    Selected map in the volume being drawn.
 * @param mapIndex
 *    Selected map in the volume being drawn.
 * @param sliceOpacity
 *    Opacity from the overlay.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSliceVoxelsSingleQuads(const float sliceNormalVector[3],
                                                         const float coordinate[3],
                                                         const float rowStep[3],
                                                         const float columnStep[3],
                                                         const int64_t numberOfColumns,
                                                         const int64_t numberOfRows,
                                                         const std::vector<uint8_t>& sliceRGBA,
                                                         const VolumeMappableInterface* volumeInterface,
                                                         const int32_t volumeIndex,
                                                         const int32_t mapIndex,
                                                         const uint8_t sliceOpacity)
{
    /*
     * When performing voxel identification for editing voxels,
     * we need to draw EVERY voxel since the user may click
     * regions where the voxels are "off".
     */
    bool volumeEditingDrawAllVoxelsFlag = false;
    if (m_identificationModeFlag) {
        SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
        if (voxelEditID->isEnabledForSelection()) {
            const VolumeFile* vf = dynamic_cast<const VolumeFile*>(volumeInterface);
            if (vf == voxelEditID->getVolumeFileForEditing()) {
                volumeEditingDrawAllVoxelsFlag = true;
            }
        }
    }
    
    const int64_t numVoxelsInSlice = numberOfColumns * numberOfRows;
    
    /*
     * Allocate for quadrilateral drawing
     */
    const int64_t numQuadCoords = numVoxelsInSlice * 12;
    const int64_t numQuadRgba   = numVoxelsInSlice * 16;
    std::vector<float> voxelQuadCoordinates;
    std::vector<float> voxelQuadNormals;
    std::vector<uint8_t> voxelQuadRgba;
    voxelQuadCoordinates.reserve(numQuadCoords);
    voxelQuadNormals.reserve(numQuadCoords);
    voxelQuadRgba.reserve(numQuadRgba);
    
    /*
     * Step to next row or column voxel
     */
    const float rowStepX = rowStep[0];
    const float rowStepY = rowStep[1];
    const float rowStepZ = rowStep[2];
    const float columnStepX = columnStep[0];
    const float columnStepY = columnStep[1];
    const float columnStepZ = columnStep[2];
    
    /*
     * Draw each row
     */
    for (int64_t jRow = 0; jRow < numberOfRows; jRow++) {
        /*
         * Coordinates on left side of row
         */
        float rowBottomLeft[3] = {
            coordinate[0] + (jRow * rowStepX),
            coordinate[1] + (jRow * rowStepY),
            coordinate[2] + (jRow * rowStepZ)
        };
        float rowTopLeft[3] = {
            rowBottomLeft[0] + rowStepX,
            rowBottomLeft[1] + rowStepY,
            rowBottomLeft[2] + rowStepZ
        };
        
        /*
         * Draw each voxel in its column
         */
        for (int64_t iCol = 0; iCol < numberOfColumns; iCol++) {
            
            /*
             * Offset of voxel in coloring.
             */
            const int64_t sliceRgbaOffset = (4 * (iCol
                                                  + (numberOfColumns * jRow)));
            const int64_t alphaOffset = sliceRgbaOffset + 3;
            
            uint8_t rgba[4] = {
                0,
                0,
                0,
                0
            };
            
            /*
             * Negative alpha means do not display
             */
            CaretAssertVectorIndex(sliceRGBA, alphaOffset);
            if (sliceRGBA[alphaOffset] <= 0) {
                if (volumeIndex == 0) {
                    /*
                     * For first drawn volume, use an alpha of 255 so
                     * so that black is drawn
                     */
                    rgba[3] = 255;
                    
                    /*
                     * OVERRIDES BLACK VOXEL ABOVE (255)
                     * For first drawn volume, use an alpha of zero so
                     * that the background shows through
                     */
                    rgba[3] = 0;
                }
            }
            else {
                /*
                 * Use overlay's opacity
                 */
                rgba[0] = sliceRGBA[sliceRgbaOffset];
                rgba[1] = sliceRGBA[sliceRgbaOffset + 1];
                rgba[2] = sliceRGBA[sliceRgbaOffset + 2];
                rgba[3] = sliceOpacity;
            }
            
            /*
             * Set coordinates of voxel corners
             */
            float voxelBottomLeft[3] = {
                rowBottomLeft[0] + (iCol * columnStepX),
                rowBottomLeft[1] + (iCol * columnStepY),
                rowBottomLeft[2] + (iCol * columnStepZ)
            };
            float voxelBottomRight[3] = {
                voxelBottomLeft[0] + columnStepX,
                voxelBottomLeft[1] + columnStepY,
                voxelBottomLeft[2] + columnStepZ
            };
            float voxelTopLeft[3] = {
                rowTopLeft[0] + (iCol * columnStepX),
                rowTopLeft[1] + (iCol * columnStepY),
                rowTopLeft[2] + (iCol * columnStepZ)
            };
            float voxelTopRight[3] = {
                voxelTopLeft[0] + columnStepX,
                voxelTopLeft[1] + columnStepY,
                voxelTopLeft[2] + columnStepZ
            };
            
            /*
             * Need to draw ALL voxels if performing
             * identificaiton for voxel editing.
             */
            if (volumeEditingDrawAllVoxelsFlag) {
                rgba[3] = 255;
            }
            
            /*
             * Draw voxel based upon opacity
             */
            if (rgba[3] > 0) {
                if (m_identificationModeFlag) {
                    /*
                     * Add info about voxel for identication.
                     */
                    int64_t voxelI = 0;
                    int64_t voxelJ = 0;
                    int64_t voxelK = 0;
                    
                    const float voxelCenterX = (voxelBottomLeft[0] + voxelTopRight[0]) / 2.0;
                    const float voxelCenterY = (voxelBottomLeft[1] + voxelTopRight[1]) / 2.0;
                    const float voxelCenterZ = (voxelBottomLeft[2] + voxelTopRight[2]) / 2.0;
                    volumeInterface->enclosingVoxel(voxelCenterX, voxelCenterY, voxelCenterZ,
                                                    voxelI, voxelJ, voxelK);
                    const float voxelDiffXYZ[3] = {
                        voxelTopRight[0] - voxelBottomLeft[0],
                        voxelTopRight[1] - voxelBottomLeft[1],
                        voxelTopRight[2] - voxelBottomLeft[2],
                    };
                    addVoxelToIdentification(volumeIndex,
                                             mapIndex,
                                             voxelI,
                                             voxelJ,
                                             voxelK,
                                             voxelDiffXYZ,
                                             rgba);
                }
                
                /*
                 * Add voxel to quadrilaterals
                 */
                voxelQuadCoordinates.push_back(voxelBottomLeft[0]);
                voxelQuadCoordinates.push_back(voxelBottomLeft[1]);
                voxelQuadCoordinates.push_back(voxelBottomLeft[2]);
                
                voxelQuadCoordinates.push_back(voxelBottomRight[0]);
                voxelQuadCoordinates.push_back(voxelBottomRight[1]);
                voxelQuadCoordinates.push_back(voxelBottomRight[2]);
                
                voxelQuadCoordinates.push_back(voxelTopRight[0]);
                voxelQuadCoordinates.push_back(voxelTopRight[1]);
                voxelQuadCoordinates.push_back(voxelTopRight[2]);
                
                voxelQuadCoordinates.push_back(voxelTopLeft[0]);
                voxelQuadCoordinates.push_back(voxelTopLeft[1]);
                voxelQuadCoordinates.push_back(voxelTopLeft[2]);
                
                
                for (int32_t iNormalAndColor = 0; iNormalAndColor < 4; iNormalAndColor++) {
                    voxelQuadRgba.push_back(rgba[0]);
                    voxelQuadRgba.push_back(rgba[1]);
                    voxelQuadRgba.push_back(rgba[2]);
                    voxelQuadRgba.push_back(rgba[3]);
                    
                    voxelQuadNormals.push_back(sliceNormalVector[0]);
                    voxelQuadNormals.push_back(sliceNormalVector[1]);
                    voxelQuadNormals.push_back(sliceNormalVector[2]);
                }
            }
        }
    }
    
    /*
     * Draw the voxels.
     */
    if (voxelQuadCoordinates.empty() == false) {
        BrainOpenGLPrimitiveDrawing::drawQuads(voxelQuadCoordinates,
                                               voxelQuadNormals,
                                               voxelQuadRgba);
    }
}

/**
 * Draw the voxels in an orthogonal slice using quad indices or strips.
 *
 * Each vertex (coordinate, its normal vector, and its color) is sent to OpenGL
 * one time.  Index arrays are used to specify the vertices when drawing the
 * quads.
 *
 * This is efficient when many voxels are drawn but may be inefficent
 * when only a few voxels are drawn.
 *
 * @param sliceNormalVector
 *    Normal vector of the slice plane.
 * @param firstVoxelCoordinate
 *    Coordinate of first voxel in the slice (bottom left as begin viewed)
 * @param rowStep
 *    Three-dimensional step to next row.
 * @param columnStep
 *    Three-dimensional step to next column.
 * @param numberOfColumns
 *    Number of columns in the slice.
 * @param numberOfRows
 *    Number of rows in the slice.
 * @param sliceRGBA
 *    RGBA coloring for voxels in the slice.
 * @param volumeInterface
 *    Index of the volume being drawn.
 * @param volumeIndex
 *    Selected map in the volume being drawn.
 * @param mapIndex
 *    Selected map in the volume being drawn.
 * @param sliceOpacity
 *    Opacity from the overlay.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSliceVoxelsQuadIndicesAndStrips(const float sliceNormalVector[3],
                                                                             const float firstVoxelCoordinate[3],
                                                                             const float rowStep[3],
                                                                             const float columnStep[3],
                                                                             const int64_t numberOfColumns,
                                                                             const int64_t numberOfRows,
                                                                             const std::vector<uint8_t>& sliceRGBA,
                                                                             const VolumeMappableInterface* volumeInterface,
                                                                             const int32_t volumeIndex,
                                                                             const int32_t mapIndex,
                                                                             const uint8_t sliceOpacity)
{
    const bool debugFlag = false;
    
    enum DrawType {
        DRAW_QUADS,
        DRAW_QUAD_STRIPS
    };
    
    const DrawType drawType = DRAW_QUADS;
    
    /*
     * When performing voxel identification for editing voxels,
     * we need to draw EVERY voxel since the user may click
     * regions where the voxels are "off".
     */
    bool volumeEditingDrawAllVoxelsFlag = false;
    if (m_identificationModeFlag) {
        SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
        if (voxelEditID->isEnabledForSelection()) {
            const VolumeFile* vf = dynamic_cast<const VolumeFile*>(volumeInterface);
            if (vf == voxelEditID->getVolumeFileForEditing()) {
                volumeEditingDrawAllVoxelsFlag = true;
            }
        }
    }
    
    /*
     * Allocate vectors for quadrilateral drawing
     */
    const int64_t totalCoordElements = (numberOfColumns + 1) * (numberOfRows + 1);
    const int64_t numQuadStripCoords = totalCoordElements * 3;
    const int64_t numQuadStripRGBA   = totalCoordElements * 4;
    std::vector<float> voxelQuadCoordinates;
    std::vector<float> voxelQuadNormals;
    std::vector<uint8_t> voxelQuadRgba;
    voxelQuadCoordinates.reserve(numQuadStripCoords);
    voxelQuadNormals.reserve(numQuadStripCoords);
    voxelQuadRgba.reserve(numQuadStripRGBA);
    
    /*
     * Step to next row or column voxel
     */
    const float rowStepX = rowStep[0];
    const float rowStepY = rowStep[1];
    const float rowStepZ = rowStep[2];
    const float columnStepX = columnStep[0];
    const float columnStepY = columnStep[1];
    const float columnStepZ = columnStep[2];
    
    const float voxelStepX = rowStepX + columnStepX;
    const float voxelStepY = rowStepY + columnStepY;
    const float voxelStepZ = rowStepZ + columnStepZ;
    const float voxelStepXYZ[3] = {
        voxelStepX,
        voxelStepY,
        voxelStepZ
    };
    
    const float halfVoxelStepX = (voxelStepX / 2.0);
    const float halfVoxelStepY = (voxelStepY / 2.0);
    const float halfVoxelStepZ = (voxelStepZ / 2.0);
    
    int64_t numberOfVoxelsToDraw = 0;
    
    int64_t voxelJMin = 10000000;
    int64_t voxelJMax = -10000000;
    
    /*
     * Loop through column COORDINATES
     */
    float columnBottomCoord[3] = {
        firstVoxelCoordinate[0],
        firstVoxelCoordinate[1],
        firstVoxelCoordinate[2]
    };
    for (int64_t iCol = 0; iCol <= numberOfColumns; iCol++) {
        if (iCol > 0) {
            columnBottomCoord[0] += columnStepX;
            columnBottomCoord[1] += columnStepY;
            columnBottomCoord[2] += columnStepZ;
        }
        
        /*
         * Loop through the row COORDINATES
         */
        float rowCoord[3] = {
            columnBottomCoord[0],
            columnBottomCoord[1],
            columnBottomCoord[2]
        };
        for (int64_t jRow = 0; jRow <= numberOfRows; jRow++) {
            if (jRow > 0) {
                rowCoord[0] += rowStepX;
                rowCoord[1] += rowStepY;
                rowCoord[2] += rowStepZ;
            }
            voxelQuadCoordinates.push_back(rowCoord[0]);
            voxelQuadCoordinates.push_back(rowCoord[1]);
            voxelQuadCoordinates.push_back(rowCoord[2]);
            
            voxelQuadNormals.push_back(sliceNormalVector[0]);
            voxelQuadNormals.push_back(sliceNormalVector[1]);
            voxelQuadNormals.push_back(sliceNormalVector[2]);
            
            uint8_t rgba[4] = {
                0,
                0,
                0,
                0
            };
            
            /*
             * With FLAT shading:
             *    Quads: Uses top left coordinate for quad coloring
             *    Quad Strip: Uses top right coordinate for quad coloring
             * So, the color is only set for this coordinate
             */
            int64_t iColRGBA = iCol;
            int64_t jRowRGBA = jRow;
            switch (drawType) {
                case DRAW_QUADS:
                    if (iColRGBA >= numberOfColumns) {
                        iColRGBA = numberOfColumns - 1;
                    }
                    jRowRGBA = jRow - 1;
                    break;
                case DRAW_QUAD_STRIPS:
                    iColRGBA = iCol - 1;
                    jRowRGBA = jRow - 1;
                    break;
            }
            if ((iColRGBA >= 0)
                && (jRowRGBA >= 0)) {
                const int64_t voxelOffset = (iColRGBA
                                             + (numberOfColumns * jRowRGBA));
                if (debugFlag) {
                    std::cout << "col=" << iCol << " row=" << jRow << " voxel-offset=" << voxelOffset << std::endl;
                }
                
                /*
                 * Offset of voxel in coloring.
                 * Note that colors are stored in rows
                 */
                int64_t sliceRgbaOffset = (4 * voxelOffset);
                
                /*
                 * An alpha greater than zero means the voxel is displayed
                 */
                const int64_t alphaOffset = sliceRgbaOffset + 3;
                CaretAssertVectorIndex(sliceRGBA, alphaOffset);
                if (sliceRGBA[alphaOffset] > 0) {
                    /*
                     * Use overlay's opacity for the voxel
                     */
                    rgba[0] = sliceRGBA[sliceRgbaOffset];
                    rgba[1] = sliceRGBA[sliceRgbaOffset + 1];
                    rgba[2] = sliceRGBA[sliceRgbaOffset + 2];
                    rgba[3] = sliceOpacity;
                }
            }
            
            /*
             * Voxel editing requires drawing of all voxels so that
             * "off" voxels can be turned "on".
             */
            if (volumeEditingDrawAllVoxelsFlag) {
                rgba[3] = 255;
            }
            
            /*
             * Draw voxel if non-zero opacity
             */
            if (rgba[3] > 0) {
                
                ++numberOfVoxelsToDraw;
                
                if (m_identificationModeFlag) {
                    /*
                     * Identification information is encoded in the
                     * RGBA coloring.
                     */
                    const float voxelCenterX = rowCoord[0] -rowStep[0] + halfVoxelStepX;
                    const float voxelCenterY = rowCoord[1] -rowStep[1] + halfVoxelStepY;
                    const float voxelCenterZ = rowCoord[2] -rowStep[2] + halfVoxelStepZ;
                    int64_t voxelI = 0;
                    int64_t voxelJ = 0;
                    int64_t voxelK = 0;
                    volumeInterface->enclosingVoxel(voxelCenterX, voxelCenterY, voxelCenterZ,
                                                    voxelI, voxelJ, voxelK);
                    
                    voxelJMin = std::min(voxelJMin, voxelJ);
                    voxelJMax = std::max(voxelJMax, voxelJ);
                    
                    addVoxelToIdentification(volumeIndex,
                                             mapIndex,
                                             voxelI,
                                             voxelJ,
                                             voxelK,
                                             voxelStepXYZ,
                                             rgba);
                }
            }
            
            voxelQuadRgba.push_back(rgba[0]);
            voxelQuadRgba.push_back(rgba[1]);
            voxelQuadRgba.push_back(rgba[2]);
            voxelQuadRgba.push_back(rgba[3]);
        }
    }
    
    const int64_t numberOfCoordinates = voxelQuadCoordinates.size() / 3;
    if (debugFlag) {
        std::cout << "Num rows/cols: " << numberOfRows << ", " << numberOfColumns << std::endl;
        std::cout << "Total, 3, 4 " << totalCoordElements << ", " << numQuadStripCoords << ", " << numQuadStripRGBA << std::endl;
        std::cout << "Size coords: " << voxelQuadCoordinates.size() << std::endl;
        std::cout << "Size normals: " << voxelQuadNormals.size() << std::endl;
        std::cout << "Size rgba: " << voxelQuadRgba.size() << std::endl;
        std::cout << "Valid voxels: " << numberOfVoxelsToDraw << std::endl;
        
        for (int64_t i = 0; i < numberOfCoordinates; i++) {
            std::cout << i << ": ";
            CaretAssertVectorIndex(voxelQuadCoordinates, i*3 + 2);
            std::cout << qPrintable(AString::fromNumbers(&voxelQuadCoordinates[i*3], 3, ",")) << "    ";
            CaretAssertVectorIndex(voxelQuadRgba, i*4 + 3);
            std::cout << qPrintable(AString::fromNumbers(&voxelQuadRgba[i*4], 4, ",")) << std::endl;
        }
    }
    
    /*
     * Setup indices into coordinates/normals/coloring to draw the quads
     */
    switch (drawType) {
        case DRAW_QUADS:
        {
            std::vector<uint32_t> quadIndices;
            quadIndices.reserve(numberOfVoxelsToDraw * 4);
            
            for (int64_t iCol = 0; iCol < numberOfColumns; iCol++) {
                const int64_t columnOffset = (iCol * (numberOfRows + 1));
                for (int64_t jRow = 0; jRow < numberOfRows; jRow++) {
                    const int64_t coordBottomLeftIndex = columnOffset + jRow; //(iCol * (numberOfRows + 1) + jRow);
                    const int64_t coordTopLeftIndex = coordBottomLeftIndex + 1;
                    const int64_t rgbaIndex = coordTopLeftIndex * 4;
                    
                    CaretAssert(coordBottomLeftIndex < numberOfCoordinates);
                    CaretAssert(coordTopLeftIndex < numberOfCoordinates);
                    CaretAssertVectorIndex(voxelQuadRgba, rgbaIndex + 3);
                    
                    if (voxelQuadRgba[rgbaIndex + 3] > 0) {
                        /*
                         * For quads: (bottom left, bottom right, top right, top left)
                         * Color with flat shading comes from the top left coordinate
                         */
                        const int32_t coordBottomRightIndex = coordBottomLeftIndex + (numberOfRows + 1);
                        const int32_t coordTopRightIndex    = coordBottomRightIndex + 1;
                        CaretAssert(coordBottomRightIndex < numberOfCoordinates);
                        CaretAssert(coordTopRightIndex < numberOfCoordinates);
                        
                        quadIndices.push_back(coordBottomLeftIndex);
                        quadIndices.push_back(coordBottomRightIndex);
                        quadIndices.push_back(coordTopRightIndex);
                        quadIndices.push_back(coordTopLeftIndex);
                    }
                }
                
                if (debugFlag) {
                    std::cout << "Quad Indices: " << quadIndices.size() << std::endl;
                    for (uint32_t i = 0; i < quadIndices.size(); i++) {
                        std::cout << quadIndices[i] << " (";
                        const int32_t coordOffset = quadIndices[i] * 3;
                        CaretAssertVectorIndex(voxelQuadCoordinates, coordOffset + 2);
                        std::cout << qPrintable(AString::fromNumbers(&voxelQuadCoordinates[coordOffset], 3, ",")) << ")   (";
                        
                        const int32_t rgbaOffset = quadIndices[i] * 4;
                        CaretAssertVectorIndex(voxelQuadRgba, rgbaOffset + 3);
                        std::cout << qPrintable(AString::fromNumbers(&voxelQuadRgba[rgbaOffset], 4, ",")) << " " << std::endl;
                    }
                    std::cout << std::endl;
                }
            }
            
            if (debugFlag) {
                std::cout << "Drawing " << quadIndices.size() / 4 << " quads." << std::endl;
            }
            BrainOpenGLPrimitiveDrawing::drawQuadIndices(voxelQuadCoordinates,
                                                         voxelQuadNormals,
                                                         voxelQuadRgba,
                                                         quadIndices);
        }
            break;
        case DRAW_QUAD_STRIPS:
        {
            int64_t stripCount = 0;
            
            const int64_t maxCoordsPerStrip = numberOfRows * 2 + 2;
            
            for (int64_t iCol = 0; iCol < numberOfColumns; iCol++) {
                std::vector<uint32_t> quadIndices;
                quadIndices.reserve(maxCoordsPerStrip);
                
                for (int64_t jRow = 0; jRow < numberOfRows; jRow++) {
                    const int32_t coordBottomLeftIndex = (iCol * (numberOfRows + 1) + jRow);
                    const int32_t coordTopLeftIndex = coordBottomLeftIndex + 1;
                    const int32_t coordBottomRightIndex = coordBottomLeftIndex + (numberOfRows + 1);
                    const int32_t coordTopRightIndex    = coordBottomRightIndex + 1;
                    const int64_t rgbaIndex = coordTopRightIndex * 4;
                    
                    CaretAssert(coordBottomLeftIndex < numberOfCoordinates);
                    CaretAssert(coordTopLeftIndex < numberOfCoordinates);
                    CaretAssert(coordBottomRightIndex < numberOfCoordinates);
                    CaretAssert(coordTopRightIndex < numberOfCoordinates);
                    CaretAssertVectorIndex(voxelQuadRgba, rgbaIndex + 3);
                    
                    if (voxelQuadRgba[rgbaIndex + 3] > 0) {
                        /*
                         * For quad strips (bottom left, bottom right, top left, top right)
                         */
                        if (quadIndices.empty()) {
                            quadIndices.push_back(coordBottomLeftIndex);
                            quadIndices.push_back(coordBottomRightIndex);
                        }
                        quadIndices.push_back(coordTopLeftIndex);
                        quadIndices.push_back(coordTopRightIndex);
                    }
                    else {
                        if ( ! quadIndices.empty()) {
                            if (debugFlag) {
                                std::cout << "Quad Indices: " << quadIndices.size() << std::endl;
                                for (uint32_t i = 0; i < quadIndices.size(); i++) {
                                    std::cout << quadIndices[i] << " (";
                                    const int32_t coordOffset = quadIndices[i] * 3;
                                    CaretAssertVectorIndex(voxelQuadCoordinates, coordOffset + 2);
                                    std::cout << qPrintable(AString::fromNumbers(&voxelQuadCoordinates[coordOffset], 3, ",")) << ")   (";
                                    
                                    const int32_t rgbaOffset = quadIndices[i] * 4;
                                    CaretAssertVectorIndex(voxelQuadRgba, rgbaOffset + 3);
                                    std::cout << qPrintable(AString::fromNumbers(&voxelQuadRgba[rgbaOffset], 4, ",")) << " " << std::endl;
                                }
                                std::cout << std::endl;
                            }
                            BrainOpenGLPrimitiveDrawing::drawQuadStrips(voxelQuadCoordinates,
                                                                        voxelQuadNormals,
                                                                        voxelQuadRgba,
                                                                        quadIndices);
                            quadIndices.clear();
                            stripCount++;
                        }
                    }
                    
                }
                if ( ! quadIndices.empty()) {
                    if (debugFlag) {
                        std::cout << "Quad Indices: " << quadIndices.size() << std::endl;
                        for (uint32_t i = 0; i < quadIndices.size(); i++) {
                            std::cout << quadIndices[i] << " (";
                            const int32_t coordOffset = quadIndices[i] * 3;
                            CaretAssertVectorIndex(voxelQuadCoordinates, coordOffset + 2);
                            std::cout << qPrintable(AString::fromNumbers(&voxelQuadCoordinates[coordOffset], 3, ",")) << ")   (";
                            
                            const int32_t rgbaOffset = quadIndices[i] * 4;
                            CaretAssertVectorIndex(voxelQuadRgba, rgbaOffset + 3);
                            std::cout << qPrintable(AString::fromNumbers(&voxelQuadRgba[rgbaOffset], 4, ",")) << " " << std::endl;
                        }
                        std::cout << std::endl;
                    }
                    BrainOpenGLPrimitiveDrawing::drawQuadStrips(voxelQuadCoordinates,
                                                                voxelQuadNormals,
                                                                voxelQuadRgba,
                                                                quadIndices);
                    quadIndices.clear();
                    stripCount++;
                }
            }
            if (debugFlag) {
                std::cout << "Strips drawn: " << stripCount << std::endl;
            }
        }
            break;
    }
}

/**
 * Reset for volume identification.
 *
 * Clear identification indices and if identification is enabled,
 * reserve a reasonable amount of space for the indices.
 */
void
BrainOpenGLVolumeSliceDrawing::resetIdentification()
{
    m_identificationIndices.clear();
    
    if (m_identificationModeFlag) {
        int64_t estimatedNumberOfItems = 0;
        
        std::vector<int64_t> volumeDims;
        m_volumeDrawInfo[0].volumeFile->getDimensions(volumeDims);
        if (volumeDims.size() >= 3) {
            const int64_t maxDim = std::max(volumeDims[0],
                                            std::max(volumeDims[1], volumeDims[2]));
            estimatedNumberOfItems = maxDim * maxDim * IDENTIFICATION_INDICES_PER_VOXEL;
        }
        
        m_identificationIndices.reserve(estimatedNumberOfItems);
    }
}

/**
 * Add a voxel to the identification indices.
 *
 * @param volumeIndex
 *    Index of the volume.
 * @param mapIndex
 *    Index of the volume map.
 * @param voxelI
 *    Voxel Index I
 * @param voxelJ
 *    Voxel Index J
 * @param voxelK
 *    Voxel Index K
 * @param voxelDiffXYZ
 *    Change in XYZ from voxel bottom left to top right
 * @param rgbaForColorIdentificationOut
 *    Encoded identification in RGBA color OUTPUT
 */
void
BrainOpenGLVolumeSliceDrawing::addVoxelToIdentification(const int32_t volumeIndex,
                                                        const int32_t mapIndex,
                                                        const int32_t voxelI,
                                                        const int32_t voxelJ,
                                                        const int32_t voxelK,
                                                        const float voxelDiffXYZ[3],
                                                        uint8_t rgbaForColorIdentificationOut[4])
{
    const int32_t idIndex = m_identificationIndices.size() / IDENTIFICATION_INDICES_PER_VOXEL;
    
    m_fixedPipelineDrawing->colorIdentification->addItem(rgbaForColorIdentificationOut,
                                                         SelectionItemDataTypeEnum::VOXEL,
                                                         idIndex);
    rgbaForColorIdentificationOut[3] = 255;
    
    /*
     * ID stack requires integers to 
     * use an integer pointer to the float values
     */
    CaretAssert(sizeof(float) == sizeof(int32_t));
    const int32_t* intPointerDiffXYZ = (int32_t*)voxelDiffXYZ;
    
    /*
     * If these items change, need to update reset and
     * processing of identification.
     */
    m_identificationIndices.push_back(volumeIndex);
    m_identificationIndices.push_back(mapIndex);
    m_identificationIndices.push_back(voxelI);
    m_identificationIndices.push_back(voxelJ);
    m_identificationIndices.push_back(voxelK);
    m_identificationIndices.push_back(intPointerDiffXYZ[0]);
    m_identificationIndices.push_back(intPointerDiffXYZ[1]);
    m_identificationIndices.push_back(intPointerDiffXYZ[2]);
}

/**
 * Process voxel identification.
 *
 * @param doNotReplaceUnderlayFlag
 * If true, do not replace identification from a lower layer
 */
void
BrainOpenGLVolumeSliceDrawing::processIdentification(const bool doNotReplaceUnderlayFlag)
{
    int32_t identifiedItemIndex;
    float depth = -1.0;
    m_fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL,
                                                       m_fixedPipelineDrawing->mouseX,
                                                       m_fixedPipelineDrawing->mouseY,
                                                       identifiedItemIndex,
                                                       depth);
    if (identifiedItemIndex >= 0) {
        const int32_t idIndex = identifiedItemIndex * IDENTIFICATION_INDICES_PER_VOXEL;
        const int32_t volDrawInfoIndex = m_identificationIndices[idIndex];
        CaretAssertVectorIndex(m_volumeDrawInfo, volDrawInfoIndex);
        VolumeMappableInterface* vf = m_volumeDrawInfo[volDrawInfoIndex].volumeFile;
        const int64_t voxelIndices[3] = {
            m_identificationIndices[idIndex + 2],
            m_identificationIndices[idIndex + 3],
            m_identificationIndices[idIndex + 4]
        };
        
        const float* floatDiffXYZ = (float*)&m_identificationIndices[idIndex + 5];
        
        SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
        if (voxelID->isEnabledForSelection()) {
            const bool idVoxelValid(doNotReplaceUnderlayFlag
                                    && voxelID->isValid());
            if (idVoxelValid) {
                /* do not replace the identified voxel */
            }
            else if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
                voxelID->setVoxelIdentification(m_brain,
                                                vf,
                                                voxelIndices,
                                                depth);
                
                float voxelCoordinates[3];
                vf->indexToSpace(voxelIndices[0], voxelIndices[1], voxelIndices[2],
                                 voxelCoordinates[0], voxelCoordinates[1], voxelCoordinates[2]);
                
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelID,
                                                                 voxelCoordinates);
                CaretLogFinest("Selected Voxel (3D): " + AString::fromNumbers(voxelIndices, 3, ","));
            }
        }
        
        SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
        if (voxelEditID->isEnabledForSelection()) {
            if (voxelEditID->getVolumeFileForEditing() == vf) {
                if (voxelEditID->isOtherScreenDepthCloserToViewer(depth)) {
                    voxelEditID->setVoxelIdentification(m_brain,
                                                        vf,
                                                        voxelIndices,
                                                        depth);
                    voxelEditID->setVoxelDiffXYZ(floatDiffXYZ);
                    
                    float voxelCoordinates[3];
                    vf->indexToSpace(voxelIndices[0], voxelIndices[1], voxelIndices[2],
                                     voxelCoordinates[0], voxelCoordinates[1], voxelCoordinates[2]);
                    
                    m_fixedPipelineDrawing->setSelectedItemScreenXYZ(voxelEditID,
                                                                     voxelCoordinates);
                    CaretLogFinest("Selected Voxel Editing (3D): Indices ("
                                   + AString::fromNumbers(voxelIndices, 3, ",")
                                   + ") Diff XYZ ("
                                   + AString::fromNumbers(floatDiffXYZ, 3, ",")
                                   + ")");
                }
            }
        }
    }
}

/**
 * Get the maximum bounds that enclose the volumes and the minimum
 * voxel spacing from the volumes.
 *
 * @param boundsOut
 *    Bounds of the volumes.
 * @param spacingOut
 *    Minimum voxel spacing from the volumes.  Always positive values (even if
 *    volumes is oriented right to left).
 *
 */
bool
BrainOpenGLVolumeSliceDrawing::getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
                                                                float spacingOut[3])
{
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(m_volumeDrawInfo.size());
    if (numberOfVolumesToDraw <= 0) {
        return false;
    }
    
    /*
     * Find maximum extent of all voxels and smallest voxel
     * size in each dimension.
     */
    float minVoxelX = std::numeric_limits<float>::max();
    float maxVoxelX = -std::numeric_limits<float>::max();
    float minVoxelY = std::numeric_limits<float>::max();
    float maxVoxelY = -std::numeric_limits<float>::max();
    float minVoxelZ = std::numeric_limits<float>::max();
    float maxVoxelZ = -std::numeric_limits<float>::max();
    float voxelStepX = std::numeric_limits<float>::max();
    float voxelStepY = std::numeric_limits<float>::max();
    float voxelStepZ = std::numeric_limits<float>::max();
    for (int32_t i = 0; i < numberOfVolumesToDraw; i++) {
        const VolumeMappableInterface* volumeFile = m_volumeDrawInfo[i].volumeFile;
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        
        float originX, originY, originZ;
        float x1, y1, z1;
        float lastX, lastY, lastZ;
        volumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
        volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
        volumeFile->indexToSpace(dimI - 1, dimJ - 1, dimK - 1, lastX, lastY, lastZ);
        const float dx = x1 - originX;
        const float dy = y1 - originY;
        const float dz = z1 - originZ;
        voxelStepX = std::min(voxelStepX, std::fabs(dx));
        voxelStepY = std::min(voxelStepY, std::fabs(dy));
        voxelStepZ = std::min(voxelStepZ, std::fabs(dz));
        
        minVoxelX = std::min(minVoxelX, std::min(originX, lastX));
        maxVoxelX = std::max(maxVoxelX, std::max(originX, lastX));
        minVoxelY = std::min(minVoxelY, std::min(originY, lastY));
        maxVoxelY = std::max(maxVoxelY, std::max(originY, lastY));
        minVoxelZ = std::min(minVoxelZ, std::min(originZ, lastZ));
        maxVoxelZ = std::max(maxVoxelZ, std::max(originZ, lastZ));
    }
    
    boundsOut[0] = minVoxelX;
    boundsOut[1] = maxVoxelX;
    boundsOut[2] = minVoxelY;
    boundsOut[3] = maxVoxelY;
    boundsOut[4] = minVoxelZ;
    boundsOut[5] = maxVoxelZ;
    
    spacingOut[0] = voxelStepX;
    spacingOut[1] = voxelStepY;
    spacingOut[2] = voxelStepZ;
    
    bool valid = false;
    
    if ((maxVoxelX > minVoxelX)
        && (maxVoxelY > minVoxelY)
        && (maxVoxelZ > minVoxelZ)
        && (voxelStepX > 0.0)
        && (voxelStepY > 0.0)
        && (voxelStepZ > 0.0)) {
        valid = true;
    }
    
    return valid;
}

/**
 * Find portion (or all) of slice that fits inside the graphics window.
 *
 * @param sliceViewPlane
 *    The orthogonal plane being viewed.
 * @param selectedSliceCoordinate
 *    Coordinate of the slice being drawn.
 * @param volumeFile
 *    Volume file that is to be drawn.
 * @param culledFirstVoxelIJKOut
 *    First (bottom left) voxel that will be drawn for the volume file.
 * @param culledLastVoxelIJKOut
 *    Last (top right) voxel that will be drawn for the volume file.
 * @param voxelDeltaXYZOut
 *    Voxel sizes for the volume file.  The element corresponding to the 
 *    slice plane being drawn will be zero (axial => [2]=0)
 */
bool
BrainOpenGLVolumeSliceDrawing::getVolumeDrawingViewDependentCulling(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                    const float selectedSliceCoordinate,
                                                                    const VolumeMappableInterface* volumeFile,
                                                                    int64_t culledFirstVoxelIJKOut[3],
                                                                    int64_t culledLastVoxelIJKOut[3],
                                                                    float voxelDeltaXYZOut[3])
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    GLdouble projectionMatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX,
                 projectionMatrix);
    GLdouble modelMatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 modelMatrix);
    
    const double vpMinX = viewport[0];
    const double vpMaxX = viewport[0] + viewport[2];
    const double vpMinY = viewport[1];
    const double vpMaxY = viewport[1] + viewport[3];
    
    GLdouble bottomLeftWin[3] = {
        vpMinX,
        vpMinY,
        0.0
    };
    GLdouble bottomRightWin[3] = {
        vpMaxX,
        vpMinY,
        0.0
    };
    GLdouble topRightWin[3] = {
        vpMaxX,
        vpMaxY,
        0.0
    };
    GLdouble topLeftWin[3] = {
        vpMinX,
        vpMaxY,
        0.0
    };
    
    GLdouble bottomLeftCoord[3];
    int32_t cornersValidCount = 0;
    if (gluUnProject(bottomLeftWin[0], bottomLeftWin[1], bottomLeftWin[2],
                     modelMatrix, projectionMatrix, viewport,
                     &bottomLeftCoord[0], &bottomLeftCoord[1], &bottomLeftCoord[2])) {
        cornersValidCount++;
    }
    
    GLdouble bottomRightCoord[3];
    if (gluUnProject(bottomRightWin[0], bottomRightWin[1], bottomRightWin[2],
                     modelMatrix, projectionMatrix, viewport,
                     &bottomRightCoord[0], &bottomRightCoord[1], &bottomRightCoord[2])) {
        cornersValidCount++;
    }
    
    GLdouble topRightCoord[3];
    if (gluUnProject(topRightWin[0], topRightWin[1], topRightWin[2],
                     modelMatrix, projectionMatrix, viewport,
                     &topRightCoord[0], &topRightCoord[1], &topRightCoord[2])) {
        cornersValidCount++;
    }
    
    GLdouble topLeftCoord[3];
    if (gluUnProject(topLeftWin[0], topLeftWin[1], topLeftWin[2],
                     modelMatrix, projectionMatrix, viewport,
                     &topLeftCoord[0], &topLeftCoord[1], &topLeftCoord[2])) {
        cornersValidCount++;
    }
    
    
    if (cornersValidCount != 4) {
        return false;
    }
    
    
    /*
     * Limit the corner coordinates to the volume's bounding box
     */
    BoundingBox boundingBox;
    volumeFile->getVoxelSpaceBoundingBox(boundingBox);

    {
        float spaceX, spaceY, spaceZ;
        volumeFile->getVoxelSpacing(spaceX,
                                    spaceY,
                                    spaceZ);
        const float halfX = std::fabs(spaceX / 2.0);
        const float halfY = std::fabs(spaceY / 2.0);
        const float halfZ = std::fabs(spaceZ / 2.0);
        
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
            {
                const float maxZ = boundingBox.getMaxZ() + halfZ;
                const float minZ = boundingBox.getMinZ() - halfZ;
                if ((selectedSliceCoordinate < minZ)
                    || (selectedSliceCoordinate > maxZ)) {
                    return false;
                }
            }
                    break;
            case VolumeSliceViewPlaneEnum::CORONAL:
            {
                const float maxY = boundingBox.getMaxY() + halfY;
                const float minY = boundingBox.getMinY() - halfY;
                if ((selectedSliceCoordinate < minY)
                    || (selectedSliceCoordinate > maxY)) {
                    return false;
                }
            }
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            {
                const float maxX = boundingBox.getMaxX() + halfX;
                const float minX = boundingBox.getMinX() - halfX;
                if ((selectedSliceCoordinate < minX)
                    || (selectedSliceCoordinate > maxX)) {
                    return false;
                }
            }
                break;
        }
    }
    
    /*
     * Limit the corner coordinates to the volume's bounding box
     */
    boundingBox.limitCoordinateToBoundingBox(bottomLeftCoord);
    boundingBox.limitCoordinateToBoundingBox(bottomRightCoord);
    boundingBox.limitCoordinateToBoundingBox(topRightCoord);
    boundingBox.limitCoordinateToBoundingBox(topLeftCoord);
    
    /*
     * The unproject functions will return the "in plane" coordinate (Z in axial view)
     * that is not correct for the slice being viewed.  So, override the
     * Z-coordinate with the coordinate of the current slice plane.
     * 
     * This must be done AFTER limiting coordinates to the volume's bounding box.
     * Otherwise, the culled voxels will always be the first or last slice when
     * the slice coordinate is NOT within the volume file.
     */
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            bottomLeftCoord[2]  = selectedSliceCoordinate;
            bottomRightCoord[2] = selectedSliceCoordinate;
            topRightCoord[2]    = selectedSliceCoordinate;
            topLeftCoord[2]     = selectedSliceCoordinate;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            bottomLeftCoord[1]  = selectedSliceCoordinate;
            bottomRightCoord[1] = selectedSliceCoordinate;
            topRightCoord[1]    = selectedSliceCoordinate;
            topLeftCoord[1]     = selectedSliceCoordinate;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            bottomLeftCoord[0]  = selectedSliceCoordinate;
            bottomRightCoord[0] = selectedSliceCoordinate;
            topRightCoord[0]    = selectedSliceCoordinate;
            topLeftCoord[0]     = selectedSliceCoordinate;
            break;
    }
    
    /*
     * Note: Spacing may be negative for some orientations
     * and positive may be on left or bottom
     */
    float voxelDeltaX, voxelDeltaY, voxelDeltaZ;
    volumeFile->getVoxelSpacing(voxelDeltaX,
                                voxelDeltaY,
                                voxelDeltaZ);
    voxelDeltaX = std::fabs(voxelDeltaX);
    voxelDeltaY = std::fabs(voxelDeltaY);
    voxelDeltaZ = std::fabs(voxelDeltaZ);
    if (bottomLeftCoord[0] > topRightCoord[0]) {
        voxelDeltaX = -voxelDeltaX;
    }
    if (bottomLeftCoord[1] > topRightCoord[1]) {
        voxelDeltaY = -voxelDeltaY;
    }
    if (bottomLeftCoord[2] > topRightCoord[2]) {
        voxelDeltaZ = -voxelDeltaZ;
    }
    bool adjustX = false;
    bool adjustY = false;
    bool adjustZ = false;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            adjustX = true;
            adjustY = true;
            voxelDeltaZ = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            adjustX = true;
            adjustZ = true;
            voxelDeltaY = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            adjustY = true;
            adjustZ = true;
            voxelDeltaX = 0.0;
            break;
    }
    
    /*
     * Adjust by one voxel to ensure full coverage
     */
    if (adjustX) {
        bottomLeftCoord[0] -= voxelDeltaX;
        topRightCoord[0]   += voxelDeltaX;
    }
    if (adjustY) {
        bottomLeftCoord[1] -= voxelDeltaY;
        topRightCoord[1]   += voxelDeltaY;
    }
    if (adjustZ) {
        bottomLeftCoord[2] -= voxelDeltaZ;
        topRightCoord[2]   += voxelDeltaZ;
    }
    
    int64_t bottomLeftIJK[3];
    volumeFile->enclosingVoxel(bottomLeftCoord[0],
                               bottomLeftCoord[1],
                               bottomLeftCoord[2],
                               bottomLeftIJK[0],
                               bottomLeftIJK[1],
                               bottomLeftIJK[2]);
    
    int64_t topRightIJK[3];
    volumeFile->enclosingVoxel(topRightCoord[0],
                               topRightCoord[1],
                               topRightCoord[2],
                               topRightIJK[0],
                               topRightIJK[1],
                               topRightIJK[2]);
    
    volumeFile->limitIndicesToValidIndices(bottomLeftIJK[0], bottomLeftIJK[1], bottomLeftIJK[2]);
    volumeFile->limitIndicesToValidIndices(topRightIJK[0], topRightIJK[1], topRightIJK[2]);
    
    culledFirstVoxelIJKOut[0] = bottomLeftIJK[0];
    culledFirstVoxelIJKOut[1] = bottomLeftIJK[1];
    culledFirstVoxelIJKOut[2] = bottomLeftIJK[2];
    culledLastVoxelIJKOut[0]  = topRightIJK[0];
    culledLastVoxelIJKOut[1]  = topRightIJK[1];
    culledLastVoxelIJKOut[2]  = topRightIJK[2];
    
    voxelDeltaXYZOut[0] = voxelDeltaX;
    voxelDeltaXYZOut[1] = voxelDeltaY;
    voxelDeltaXYZOut[2] = voxelDeltaZ;
    
    return true;
}

/* ======================================================================= */

/**
 * Constructor
 *
 * @param volumeMappableInterface
 *   Volume that contains the data values.
 * @param mapIndex
 *   Index of selected map.
 */
BrainOpenGLVolumeSliceDrawing::VolumeSlice::VolumeSlice(VolumeMappableInterface* volumeMappableInterface,
                                                        const int32_t mapIndex)
{
    m_volumeMappableInterface = volumeMappableInterface;
    m_volumeFile = dynamic_cast<VolumeFile*>(m_volumeMappableInterface);
    m_ciftiMappableDataFile = dynamic_cast<CiftiMappableDataFile*>(m_volumeMappableInterface);
    
    CaretAssert(m_volumeMappableInterface);
    m_mapIndex = mapIndex;
    CaretAssert(m_mapIndex >= 0);
    
    const int64_t sliceDim = 300;
    const int64_t numVoxels = sliceDim * sliceDim;
    m_values.reserve(numVoxels);
}

/**
 * Add a value and return its index.
 *
 * @param value
 *     Value that is added.
 * @return
 *     The index for the value.
 */
int64_t
BrainOpenGLVolumeSliceDrawing::VolumeSlice::addValue(const float value)
{
    const int64_t indx = static_cast<int64_t>(m_values.size());
    m_values.push_back(value);
    return indx;
}

/**
 * Return RGBA colors for value using the value's index
 * returned by addValue().
 *
 * @param indx
 *    Index of the value.
 * @return
 *    RGBA coloring for value.
 */
uint8_t*
BrainOpenGLVolumeSliceDrawing::VolumeSlice::getRgbaForValueByIndex(const int64_t indx)
{
    CaretAssertVectorIndex(m_rgba, indx * 4);
    return &m_rgba[indx*4];
}

/**
 * Allocate colors for the voxel values
 */
void
BrainOpenGLVolumeSliceDrawing::VolumeSlice::allocateColors()
{
    m_rgba.resize(m_values.size() * 4);
}

/* ======================================================================= */
/**
 * Create a voxel for drawing.
 *
 * @param center
 *    Center of voxel.
 * @param leftBottom
 *    Left bottom coordinate of voxel.
 * @param rightBottom
 *    Right bottom coordinate of voxel.
 * @param rightTop
 *    Right top coordinate of voxel.
 * @param leftTop
 *    Left top coordinate of voxel.
 */
BrainOpenGLVolumeSliceDrawing::VoxelToDraw::VoxelToDraw(const float center[3],
                                                        const double leftBottom[3],
                                                        const double rightBottom[3],
                                                        const double rightTop[3],
                                                        const double leftTop[3])
{
    m_center[0] = center[0];
    m_center[1] = center[1];
    m_center[2] = center[2];
    
    m_coordinates[0]  = leftBottom[0];
    m_coordinates[1]  = leftBottom[1];
    m_coordinates[2]  = leftBottom[2];
    m_coordinates[3]  = rightBottom[0];
    m_coordinates[4]  = rightBottom[1];
    m_coordinates[5]  = rightBottom[2];
    m_coordinates[6]  = rightTop[0];
    m_coordinates[7]  = rightTop[1];
    m_coordinates[8]  = rightTop[2];
    m_coordinates[9]  = leftTop[0];
    m_coordinates[10] = leftTop[1];
    m_coordinates[11] = leftTop[2];
    
    const int64_t numSlices = 5;
    m_sliceIndices.reserve(numSlices);
    m_sliceOffsets.reserve(numSlices);
}

/**
 * Get the change in XYZ for the voxel ([top right] minus [bottom left])
 *
 * @param dxyzOut
 *     Change in XYZ from bottom left to top right
 */
void
BrainOpenGLVolumeSliceDrawing::VoxelToDraw::getDiffXYZ(float dxyzOut[3]) const
{
    dxyzOut[0] = m_coordinates[6] - m_coordinates[0];
    dxyzOut[1] = m_coordinates[7] - m_coordinates[1];
    dxyzOut[2] = m_coordinates[8] - m_coordinates[2];
}

/**
 * Add a value from a volume slice.
 *
 * @param sliceIndex
 *    Index of the slice.
 * @param sliceOffset
 *    Offset of value in the slice.
 */
void
BrainOpenGLVolumeSliceDrawing::VoxelToDraw::addVolumeValue(const int64_t sliceIndex,
                                                           const int64_t sliceOffset)
{
    CaretAssert(sliceIndex >= 0);
    CaretAssert(sliceOffset >= 0);
    m_sliceIndices.push_back(sliceIndex);
    m_sliceOffsets.push_back(sliceOffset);
}



/**
 * Get axis information for the given viewing plane and volume.
 *
 * @param volumeInterface
 *    The volume interface.
 * @param sliceViewingPlane
 *    The slice viewing plane.
 * @param axisIndexOut
 *    Output with index into voxel IJK for axis.
 * @param orientationOut
 *    Orientation of the axis.
 */
static void
getAxisInfoForOrthogonalPlane(const VolumeMappableInterface* volumeInterface,
                               const VolumeSliceViewPlaneEnum::Enum sliceViewingPlane,
                               int32_t& axisIndexOut,
                               VolumeSpace::OrientTypes& orientationOut)
{
    CaretAssert(volumeInterface);
    
    axisIndexOut = -1;
    orientationOut = VolumeSpace::OrientTypes::LEFT_TO_RIGHT;
    
    const VolumeSpace volumeSpace = volumeInterface->getVolumeSpace();
    VolumeSpace::OrientTypes volumeOrientations[3];
    volumeSpace.getOrientation(volumeOrientations);
    
    for (int32_t i = 0; i < 3; i++) {
        CaretAssertArrayIndex(orientations, 3, i);
        const VolumeSpace::OrientTypes& ot = volumeOrientations[i];
        
        bool matchFlag = false;
        switch (sliceViewingPlane) {
            case caret::VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case caret::VolumeSliceViewPlaneEnum::PARASAGITTAL:
                if ((ot == VolumeSpace::OrientTypes::LEFT_TO_RIGHT)
                    || (ot == VolumeSpace::OrientTypes::RIGHT_TO_LEFT)) {
                    matchFlag = true;
                }
                break;
            case caret::VolumeSliceViewPlaneEnum::CORONAL:
                if ((ot == VolumeSpace::OrientTypes::ANTERIOR_TO_POSTERIOR)
                    || (ot == VolumeSpace::OrientTypes::POSTERIOR_TO_ANTERIOR)) {
                    matchFlag = true;
                }
                break;
            case caret::VolumeSliceViewPlaneEnum::AXIAL:
                if ((ot == VolumeSpace::OrientTypes::INFERIOR_TO_SUPERIOR)
                    || (ot == VolumeSpace::OrientTypes::SUPERIOR_TO_INFERIOR)) {
                    matchFlag = true;
                }
                break;
        }
        
        if (matchFlag) {
            axisIndexOut   = i;
            orientationOut = ot;
            break;
        }
    }
    
    CaretAssert((axisIndexOut >= 0) && (axisIndexOut < 3));
}

/**
 * Get the info for the given axis in the given volume file
 *
 * @param volumeInterface
 *     The volume interface.
 * @param axis
 *     The axis
 * @param startWithMinimumCoordFlag
 *     If true, voxel information will start with voxel that has minimum coordinate.
 * @param axisInfoOut
 *     Output containing information for the given axis
 */
static void
getAxisInfoAllView(const VolumeMappableInterface* volumeInterface,
                   const VolumeSliceViewPlaneEnum::Enum axis,
                   const bool startWithMinimumCoordFlag,
                   AxisInfo& axisInfoOut)
{
    /*
     * Data for axis may be in any dimension
     */
    int32_t axisIndex = -1;
    VolumeSpace::OrientTypes axisOrientation = VolumeSpace::OrientTypes::LEFT_TO_RIGHT;
    getAxisInfoForOrthogonalPlane(volumeInterface,
                                   axis,
                                   axisIndex,
                                   axisOrientation);
    axisInfoOut.axis = axis;
    axisInfoOut.indexIntoIJK = axisIndex;
    if (axisInfoOut.indexIntoIJK < 0) {
        axisInfoOut.valid = false;
        return;
    }
    
    std::vector<int64_t> dimArray;
    volumeInterface->getDimensions(dimArray);
    
    CaretAssertVectorIndex(dimArray, axisInfoOut.indexIntoIJK);
    axisInfoOut.numberOfVoxels = dimArray[axisInfoOut.indexIntoIJK];
    
    axisInfoOut.firstVoxelIndex = 0;
    
    int64_t zeroIndices[3] = { 0, 0, 0 };
    volumeInterface->indexToSpace(zeroIndices, axisInfoOut.firstVoxelXYZ);
    int64_t oneIndices[3] = { 1, 1, 1};
    float xyzOne[3] = { 1.0, 1.0, 1.0 };
    volumeInterface->indexToSpace(oneIndices, xyzOne);
    int64_t lastIndices[3] = { dimArray[0] - 1, dimArray[1] - 1, dimArray[2] - 1 };
    volumeInterface->indexToSpace(lastIndices, axisInfoOut.lastVoxelXYZ);
    
    axisInfoOut.indexIntoXYZ = axisInfoOut.indexIntoIJK;
    switch (axisOrientation) {
        case caret::VolumeSpace::LEFT_TO_RIGHT:
        case caret::VolumeSpace::RIGHT_TO_LEFT:
            axisInfoOut.indexIntoXYZ    = 0;  // X-axis
            break;
        case caret::VolumeSpace::POSTERIOR_TO_ANTERIOR:
        case caret::VolumeSpace::ANTERIOR_TO_POSTERIOR:
            axisInfoOut.indexIntoXYZ    = 1; // Y-axis
            break;
        case caret::VolumeSpace::INFERIOR_TO_SUPERIOR:
        case caret::VolumeSpace::SUPERIOR_TO_INFERIOR:
            axisInfoOut.indexIntoXYZ    = 2;  // Z-axis
            break;
    }
    
    const float voxelStepSize = (xyzOne[axisInfoOut.indexIntoXYZ] - axisInfoOut.firstVoxelXYZ[axisInfoOut.indexIntoXYZ]);
    axisInfoOut.firstVoxelIndex = 0;
    axisInfoOut.voxelIndexStep  = 1;
    axisInfoOut.voxelStepSize   = voxelStepSize;
    axisInfoOut.orientation     = axisOrientation;
    
    axisInfoOut.absoluteVoxelSize = std::fabs(axisInfoOut.voxelStepSize);
    
    bool flipFlag = false;
    if (startWithMinimumCoordFlag) {
        if (axisInfoOut.voxelStepSize < 0.0) {
            flipFlag = true;
        }
    }
    else {
        if (axisInfoOut.voxelStepSize > 0.0) {
            flipFlag = true;
        }
    }
    
    if (flipFlag) {
        axisInfoOut.firstVoxelIndex = axisInfoOut.numberOfVoxels - 1;
        axisInfoOut.voxelStepSize = -axisInfoOut.voxelStepSize;
        axisInfoOut.voxelIndexStep = -1;
    }
    
    axisInfoOut.valid = true;
}

/**
 * Draw an orthogonal slice for all view that works for any volume orientation.
 *
 * @param sliceViewingPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param plane
 *    Plane equation for the selected slice.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSliceAllView(const VolumeSliceViewPlaneEnum::Enum sliceViewingPlane,
                                                          const float sliceCoordinates[3],
                                                          const Plane& plane)
{
    CaretAssert(plane.isValidPlane());
    if (plane.isValidPlane() == false) {
        return;
    }
    
    const int32_t browserTabIndex = m_browserTabContent->getTabNumber();
    const DisplayPropertiesLabels* displayPropertiesLabels = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    const bool allowBlendingFlag(true);
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    
    /*
     * Flat shading voxels not interpolated
     */
    glShadeModel(GL_FLAT);
    
    /*
     * Compute coordinate of point in center of first slice
     */
    float selectedSliceCoordinate = 0.0;
    float sliceNormalVector[3] = { 0.0, 0.0, 0.0 };
    plane.getNormalVector(sliceNormalVector);
    switch (sliceViewingPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            selectedSliceCoordinate = sliceCoordinates[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            selectedSliceCoordinate = sliceCoordinates[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            selectedSliceCoordinate = sliceCoordinates[0];
            break;
    }
    
    /*
     * Holds colors for voxels in the slice
     * Outside of loop to minimize allocations
     * It is faster to make one call to
     * NodeAndVoxelColoring::colorScalarsWithPalette() with
     * all voxels in the slice than it is to call it
     * separately for each voxel.
     */
    std::vector<uint8_t> sliceVoxelsRgbaVector;
    
    /*
     * Draw each of the volumes separately so that each
     * is drawn with the correct voxel slices.
     */
    const int32_t numberOfVolumesToDraw = static_cast<int32_t>(m_volumeDrawInfo.size());
    for (int32_t iVol = 0; iVol < numberOfVolumesToDraw; iVol++) {
        const BrainOpenGLFixedPipeline::VolumeDrawInfo& volInfo = m_volumeDrawInfo[iVol];
        const VolumeMappableInterface* volumeInterface = volInfo.volumeFile;
        
        int32_t viewingAxisIndex = -1;
        VolumeSpace::OrientTypes orientation = VolumeSpace::OrientTypes::LEFT_TO_RIGHT;
        getAxisInfoForOrthogonalPlane(volumeInterface,
                                       sliceViewingPlane,
                                       viewingAxisIndex,
                                       orientation);
        
        /*
         * Coordinate of slice is ALWAYS from the bottom layer
         */
        if (iVol == 0) {
            switch (sliceViewingPlane) {
                case VolumeSliceViewPlaneEnum::ALL:
                    CaretAssert(0);
                    break;
                case VolumeSliceViewPlaneEnum::AXIAL:
                    selectedSliceCoordinate = sliceCoordinates[2];
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    selectedSliceCoordinate = sliceCoordinates[1];
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    selectedSliceCoordinate = sliceCoordinates[0];
                    break;
            }
        }
        
        /*
         * Find axis that correspond to the axis that are on
         * the screen horizontally and vertically.
         */
        AxisInfo drawLeftToRightInfo;
        AxisInfo drawBottomToTopInfo;
        
        int64_t viewPlaneDimIndex = viewingAxisIndex;
        int64_t sliceViewingPlaneIndexIntoXYZ = viewingAxisIndex;
        switch (sliceViewingPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                getAxisInfoAllView(volumeInterface, VolumeSliceViewPlaneEnum::PARASAGITTAL, true, drawLeftToRightInfo);
                getAxisInfoAllView(volumeInterface, VolumeSliceViewPlaneEnum::CORONAL, true, drawBottomToTopInfo);
                sliceViewingPlaneIndexIntoXYZ = 2;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                getAxisInfoAllView(volumeInterface, VolumeSliceViewPlaneEnum::PARASAGITTAL, true, drawLeftToRightInfo);
                getAxisInfoAllView(volumeInterface, VolumeSliceViewPlaneEnum::AXIAL, true, drawBottomToTopInfo);
                sliceViewingPlaneIndexIntoXYZ = 1;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                getAxisInfoAllView(volumeInterface, VolumeSliceViewPlaneEnum::CORONAL, false, drawLeftToRightInfo);
                getAxisInfoAllView(volumeInterface, VolumeSliceViewPlaneEnum::AXIAL, true, drawBottomToTopInfo);
                sliceViewingPlaneIndexIntoXYZ = 0;
                break;
        }
        CaretAssert(drawLeftToRightInfo.valid);
        CaretAssert(drawBottomToTopInfo.valid);
        CaretAssert(viewPlaneDimIndex != drawLeftToRightInfo.indexIntoIJK);
        CaretAssert(viewPlaneDimIndex != drawBottomToTopInfo.indexIntoIJK);
        CaretAssert(drawLeftToRightInfo.indexIntoIJK != drawBottomToTopInfo.indexIntoIJK);
        CaretAssert(sliceViewingPlaneIndexIntoXYZ != drawLeftToRightInfo.indexIntoXYZ);
        CaretAssert(sliceViewingPlaneIndexIntoXYZ != drawBottomToTopInfo.indexIntoXYZ);
        CaretAssert(drawLeftToRightInfo.indexIntoXYZ != drawBottomToTopInfo.indexIntoXYZ);
        
        if (debugFlag) {
            std::cout << "Viewing Axis Index: " << viewingAxisIndex << " for " << VolumeSliceViewPlaneEnum::toGuiName(sliceViewingPlane) << std::endl;
            std::cout << "Left to Right ";
            drawLeftToRightInfo.print();
            std::cout << "Bottom to Top: ";
            drawBottomToTopInfo.print();
        }
        
        
        /*
         * There must be at least two voxels in both dimensions.
         * If a dimension consists of a single voxel, then it is
         * likely a single slice volume and our viewpoint is
         * "in" the slice.
         *
         * Without this check the user would see a strange looking
         * line that is one voxel in width
         */
        if ((drawLeftToRightInfo.numberOfVoxels <= 1)
            || (drawBottomToTopInfo.numberOfVoxels <= 1)) {
            continue;
        }
        
        /*
         * Spatial amount to move up row.
         */
        float rowStepXYZ[3] = { 0.0, 0.0, 0.0 };
        rowStepXYZ[drawBottomToTopInfo.indexIntoXYZ] = drawBottomToTopInfo.voxelStepSize;
        
        /*
         * Spatial amount to move right one column.
         */
        float columnStepXYZ[3] = { 0.0, 0.0, 0.0 };
        columnStepXYZ[drawLeftToRightInfo.indexIntoXYZ] = drawLeftToRightInfo.voxelStepSize;
        
        /*
         * Step in voxel dimensions to move right one column
         */
        int64_t columnStepIJK[3] = { 0, 0, 0 };
        columnStepIJK[drawLeftToRightInfo.indexIntoIJK] = drawLeftToRightInfo.voxelIndexStep;
        
        /*
         * Step in voxel dimensions to move up one row
         */
        int64_t rowStepIJK[3] = { 0, 0, 0 };
        rowStepIJK[drawBottomToTopInfo.indexIntoIJK] = drawBottomToTopInfo.voxelIndexStep;
        
        /*
         * XYZ needs to use regular X=0, Y=1, Z=2 indices
         */
        int64_t sliceVoxelIndices[3] = { 0, 0, 0 };
        float   sliceVoxelXYZ[3]     = { 0.0, 0.0, 0.0 };
        sliceVoxelXYZ[sliceViewingPlaneIndexIntoXYZ] = selectedSliceCoordinate;
        volumeInterface->enclosingVoxel(sliceVoxelXYZ[0], sliceVoxelXYZ[1], sliceVoxelXYZ[2],
                                        sliceVoxelIndices[0], sliceVoxelIndices[1], sliceVoxelIndices[2]);
        
        /*
         * Find the index of the slice for drawing and verify that
         * it is a valid slice index.
         */
        const int64_t sliceIndexForDrawing = sliceVoxelIndices[viewPlaneDimIndex];
        std::vector<int64_t> volDim;
        volumeInterface->getDimensions(volDim);
        const int64_t maximumAxisSliceIndex = volDim[viewPlaneDimIndex];
        if ((sliceIndexForDrawing < 0)
            || (sliceIndexForDrawing >= maximumAxisSliceIndex)) {
            continue;
        }
        
        /*
         * Voxel indices for first voxel that is drawn at bottom left of screen
         */
        int64_t firstVoxelIJK[3] = { -1, -1, -1 };
        firstVoxelIJK[drawBottomToTopInfo.indexIntoIJK] = drawBottomToTopInfo.firstVoxelIndex;   // BACKWARDS ??????
        firstVoxelIJK[drawLeftToRightInfo.indexIntoIJK] = drawLeftToRightInfo.firstVoxelIndex;
        firstVoxelIJK[viewPlaneDimIndex] = sliceIndexForDrawing;
        
        /*
         * Coordinate of first voxel that is drawn at bottom left of screen
         */
        float startCoordinateXYZ[3] = { 0.0, 0.0, 0.0 };
        volumeInterface->indexToSpace(firstVoxelIJK, startCoordinateXYZ);
        startCoordinateXYZ[sliceViewingPlaneIndexIntoXYZ] = selectedSliceCoordinate;
        startCoordinateXYZ[drawLeftToRightInfo.indexIntoXYZ] -= (drawLeftToRightInfo.voxelStepSize / 2.0);
        startCoordinateXYZ[drawBottomToTopInfo.indexIntoXYZ] -= (drawBottomToTopInfo.voxelStepSize / 2.0);
        
        /*
         * Stores RGBA values for each voxel.
         * Use a vector for voxel colors so no worries about memory being freed.
         */
        const int64_t numVoxelsInSlice = drawBottomToTopInfo.numberOfVoxels * drawLeftToRightInfo.numberOfVoxels;
        const int64_t numVoxelsInSliceRGBA = numVoxelsInSlice * 4;
        if (numVoxelsInSliceRGBA > static_cast<int64_t>(sliceVoxelsRgbaVector.size())) {
            sliceVoxelsRgbaVector.resize(numVoxelsInSliceRGBA);
        }
        uint8_t* sliceVoxelsRGBA = &sliceVoxelsRgbaVector[0];
        
        /*
         * Get colors for all voxels in the slice.
         */
        if (debugFlag) {
            std::cout << "Slice Axis " << VolumeSliceViewPlaneEnum::toGuiName(sliceViewingPlane)
            << " start IJK: " << AString::fromNumbers(firstVoxelIJK, 3, ",")
            << " rowstep IJK: " << AString::fromNumbers(rowStepIJK, 3, ",")
            << " colstep IJK: " << AString::fromNumbers(columnStepIJK, 3, ",") << std::endl;
        }
        const int64_t validVoxelCount = volumeInterface->getVoxelColorsForSliceInMap(volInfo.mapIndex,
                                                                                firstVoxelIJK,
                                                                                rowStepIJK,
                                                                                columnStepIJK,
                                                                                drawBottomToTopInfo.numberOfVoxels,
                                                                                drawLeftToRightInfo.numberOfVoxels,
                                                                                displayGroup,
                                                                                browserTabIndex,
                                                                                sliceVoxelsRGBA);
        
        /*
         * Is label outline mode?
         */
        if (m_volumeDrawInfo[iVol].mapFile->isMappedWithLabelTable()) {
            int64_t xdim = drawLeftToRightInfo.numberOfVoxels;
            int64_t ydim = drawBottomToTopInfo.numberOfVoxels;
            
            LabelDrawingTypeEnum::Enum labelDrawingType = LabelDrawingTypeEnum::DRAW_FILLED;
            CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
            const CaretMappableDataFile* mapFile = dynamic_cast<const CaretMappableDataFile*>(volumeInterface);
            if (mapFile != NULL) {
                if (mapFile->isMappedWithLabelTable()) {
                    const LabelDrawingProperties* props = mapFile->getLabelDrawingProperties();
                    labelDrawingType = props->getDrawingType();
                    outlineColor     = props->getOutlineColor();
                }
            }
            NodeAndVoxelColoring::convertSliceColoringToOutlineMode(sliceVoxelsRGBA,
                                                                    labelDrawingType,
                                                                    outlineColor,
                                                                    xdim,
                                                                    ydim);
        }
        
        if (m_volumeDrawInfo[iVol].mapFile->isMappedWithPalette()) {
            const int32_t mapIndex = m_volumeDrawInfo[iVol].mapIndex;
            int64_t xdim = drawLeftToRightInfo.numberOfVoxels;
            int64_t ydim = drawBottomToTopInfo.numberOfVoxels;
            const PaletteColorMapping* pcm = m_volumeDrawInfo[iVol].mapFile->getMapPaletteColorMapping(mapIndex);
            NodeAndVoxelColoring::convertPaletteSliceColoringToOutlineMode(sliceVoxelsRGBA,
                                                                           pcm->getThresholdOutlineDrawingMode(),
                                                                           pcm->getThresholdOutlineDrawingColor(),
                                                                           xdim,
                                                                           ydim);
        }
        
        const uint8_t volumeDrawingOpacity = static_cast<uint8_t>(volInfo.opacity * 255.0);
        
        if (m_modelWholeBrain != NULL) {
            /*
             * After the a slice is drawn in ALL view, some layers
             * (volume surface outline) may be drawn in lines.  As the
             * view is rotated, lines will partially appear and disappear
             * due to the lines having the same (extremely close) depth
             * values as the voxel polygons.  OpenGL's Polygon Offset
             * only works with polygons and NOT with lines or points.
             * So, polygon offset cannot be used to move the depth
             * values for the lines and points "a little closer" to
             * the user.  Instead, polygon offset is used to push
             * the underlaying slices "a little bit away" from the
             * user.
             *
             * Resolves WB-414
             */
            const float inverseSliceIndex = numberOfVolumesToDraw - iVol;
            const float factor  = inverseSliceIndex * 1.0 + 1.0;
            const float units  = inverseSliceIndex * 1.0 + 1.0;
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(factor, units);
        }
        
        /*
         * Draw the voxels in the slice.
         */
        //        const AString drawMsg("NEW:"
        //                              "\n   Axis: " + VolumeSliceViewPlaneEnum::toName(sliceViewingPlane)
        //                              + "\n   Start XYZ: " + AString::fromNumbers(startCoordinateXYZ, 3, ",")
        //                              + "\n   Row Step: " + AString::fromNumbers(rowStepXYZ, 3, ",")
        //                              + "\n   Column Step: " + AString::fromNumbers(columnStepXYZ, 3, ",")
        //                              + "\n   Num Cols: " + AString::number(drawLeftToRightInfo.numberOfVoxels)
        //                              + "\n   Num Rows: " + AString::number(drawBottomToTopInfo.numberOfVoxels)
        //                              + "\n");
        //        std::cout << qPrintable(drawMsg) << std::endl;
        
        if (debugFlag) {
            std::cout << "Slice Axis " << VolumeSliceViewPlaneEnum::toGuiName(sliceViewingPlane)
            << " start XYZ: " << AString::fromNumbers(startCoordinateXYZ, 3, ",")
            << " rowstep XYZ: " << AString::fromNumbers(rowStepXYZ, 3, ",")
            << " colstep XYZ: " << AString::fromNumbers(columnStepXYZ, 3, ",") << std::endl;
        }
        
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        if (volInfo.opacity < 1.0) {
            if (allowBlendingFlag) {
                if ( ! m_identificationModeFlag) {
                    BrainOpenGLFixedPipeline::setupBlending(BrainOpenGLFixedPipeline::BlendDataType::VOLUME_ALL_VIEW_SLICES);
                }
            }
        }
        
        drawOrthogonalSliceVoxels(sliceNormalVector,
                                  startCoordinateXYZ,
                                  rowStepXYZ,
                                  columnStepXYZ,
                                  drawLeftToRightInfo.numberOfVoxels,
                                  drawBottomToTopInfo.numberOfVoxels,
                                  sliceVoxelsRgbaVector,
                                  validVoxelCount,
                                  volumeInterface,
                                  iVol,
                                  volInfo.mapIndex,
                                  volumeDrawingOpacity);
        
        glDisable(GL_POLYGON_OFFSET_FILL);
        
        glPopAttrib();
        
        if (m_identificationModeFlag) {
            processIdentification(false);
        }
    }
    
    showBrainordinateHighlightRegionOfInterest(sliceViewingPlane,
                                               sliceCoordinates,
                                               sliceNormalVector);
    
    glPopAttrib();
    glShadeModel(GL_SMOOTH);
}

