
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

#define __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__
#include "BrainOpenGLVolumeSliceDrawing.h"
#undef __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__

#include "AnnotationCoordinate.h"
#include "AnnotationText.h"
#include "BoundingBox.h"
#include "Brain.h"
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#include "BrainOpenGLPrimitiveDrawing.h"
#include "BrainordinateRegionOfInterest.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOpenGLInclude.h"
#include "CaretPreferences.h"
#include "CiftiMappableDataFile.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "ElapsedTimer.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentificationManager.h"
#include "IdentificationWithColor.h"
#include "IdentifiedItemVoxel.h"
#include "LabelDrawingProperties.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "NodeAndVoxelColoring.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemVoxel.h"
#include "SelectionItemVoxelEditing.h"
#include "SelectionItemVoxelIdentificationSymbol.h"
#include "SelectionManager.h"
#include "SessionManager.h"
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
    
    CaretAssert(fixedPipelineDrawing);
    CaretAssert(browserTabContent);
    m_browserTabContent = browserTabContent;    
    
    /*
     * Initialize class members which help reduce the number of
     * parameters that are passed to methods.
     */
    m_brain           = NULL;
    m_modelVolume     = NULL;
    m_modelWholeBrain = NULL;
    if (m_browserTabContent->getDisplayedVolumeModel() != NULL) {
        m_modelVolume = m_browserTabContent->getDisplayedVolumeModel();
        m_brain = m_modelVolume->getBrain();
    }
    else if (m_browserTabContent->getDisplayedWholeBrainModel() != NULL) {
        m_modelWholeBrain = m_browserTabContent->getDisplayedWholeBrainModel();
        m_brain = m_modelWholeBrain->getBrain();
    }
    else {
        CaretAssertMessage(0, "Invalid model for volume slice drawing.");
    }
    CaretAssert(m_brain);
    
    m_fixedPipelineDrawing = fixedPipelineDrawing;
    
    m_volumeDrawInfo = volumeDrawInfo;
    if (m_volumeDrawInfo.empty()) {
        return;
    }
    m_underlayVolume = m_volumeDrawInfo[0].volumeFile;
    
    m_paletteFile = m_browserTabContent->getModelForDisplay()->getBrain()->getPaletteFile();
    CaretAssert(m_paletteFile);
    
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
 * @param viewport
 *    The viewport (region of graphics area) for drawing slices.
 */
void
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewPlane(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                              const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                              const int32_t viewport[4])
{
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            const int32_t gap = 2;
            
            const int32_t vpHalfX = viewport[2] / 2;
            const int32_t vpHalfY = viewport[3] / 2;
            
            /*
             * Draw parasagittal slice
             */
            const int32_t paraVP[4] = {
                viewport[0],
                viewport[1] + vpHalfY + gap,
                vpHalfX - gap,
                vpHalfY - gap
            };
            glPushMatrix();
            drawVolumeSliceViewType(sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                    paraVP);
            glPopMatrix();
            
            
            /*
             * Draw coronal slice
             */
            const int32_t coronalVP[4] = {
                viewport[0] + vpHalfX + gap,
                viewport[1] + vpHalfY + gap,
                vpHalfX - gap,
                vpHalfY - gap
            };
            glPushMatrix();
            drawVolumeSliceViewType(sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::CORONAL,
                                    coronalVP);
            glPopMatrix();
            
            
            /*
             * Draw axial slice
             */
            const int32_t axialVP[4] = {
                viewport[0] + vpHalfX + gap,
                viewport[1],
                vpHalfX - gap,
                vpHalfY - gap
            };
            glPushMatrix();
            drawVolumeSliceViewType(sliceDrawingType,
                                    sliceProjectionType,
                                    VolumeSliceViewPlaneEnum::AXIAL,
                                    axialVP);
            glPopMatrix();
            
            /*
             * 4th quadrant is used for axis showing orientation
             */
            const int32_t allVP[4] = {
                viewport[0],
                viewport[1],
                vpHalfX - gap,
                vpHalfY - gap
            };
            
            switch (sliceProjectionType) {
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                    drawOrientationAxes(allVP);
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                    break;
            }
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            drawVolumeSliceViewType(sliceDrawingType,
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
        drawVolumeSliceViewProjection(VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::AXIAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceCoronalEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
                                      sliceProjectionType,
                                      VolumeSliceViewPlaneEnum::CORONAL,
                                      sliceCoordinates,
                                      viewport);
        glPopMatrix();
    }
    
    if (m_browserTabContent->isSliceParasagittalEnabled()) {
        glPushMatrix();
        drawVolumeSliceViewProjection(VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE,
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
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewType(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const int32_t viewport[4])
{
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawVolumeSliceViewTypeMontage(sliceDrawingType,
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
            drawVolumeSliceViewProjection(sliceDrawingType,
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
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewTypeMontage(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                                            const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                  const int32_t viewport[4])
{
    const int32_t numRows = m_browserTabContent->getMontageNumberOfRows();
    CaretAssert(numRows > 0);
    const int32_t numCols = m_browserTabContent->getMontageNumberOfColumns();
    CaretAssert(numCols > 0);
    
    const CaretPreferences* caretPreferences = SessionManager::get()->getCaretPreferences();
    const int32_t montageMargin = caretPreferences->getVolumeMontageGap();
    const int32_t montageCoordPrecision = caretPreferences->getVolumeMontageCoordinatePrecision();
    
    const int32_t totalGapX = montageMargin * (numCols - 1);
    const int32_t vpSizeX = (viewport[2] - totalGapX) / numCols;
    const int32_t totalGapY = montageMargin * (numRows - 1);
    const int32_t vpSizeY = (viewport[3] - totalGapY) / numRows;
    
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
    prefs->getColorForegroundVolumeView(foregroundRGBA);
    foregroundRGBA[3] = 255;
    const bool showCoordinates = prefs->isVolumeMontageAxesCoordinatesDisplayed();
    uint8_t backgroundRGBA[4];
    prefs->getColorBackgroundVolumeView(backgroundRGBA);
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
                    const int32_t vpX = (j * (vpSizeX + montageMargin));
                    const int32_t vpY = ((numRows - i - 1) * (vpSizeY + montageMargin));
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
                    
                    drawVolumeSliceViewProjection(sliceDrawingType,
                                                  sliceProjectionType,
                                                  sliceViewPlane,
                                                  sliceCoordinates,
                                                  vp);
                    
                    if (showCoordinates) {
                        const AString coordText = (axisLetter
                                                   + "="
                                                   + AString::number(sliceCoord, 'f', montageCoordPrecision)
                                                   + "mm");
                        
                        AnnotationText annotationText;
                        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
                        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
                        annotationText.setFontSize(AnnotationFontSizeEnum::SIZE12);
                        annotationText.setForegroundColor(CaretColorEnum::CUSTOM);
                        annotationText.setCustomForegroundColor(foregroundRGBA);
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


    if (prefs->isVolumeAxesLabelsDisplayed()) {
        drawAxesCrosshairsOrthoAndOblique(sliceProjectionType,
                                          sliceViewPlane,
                                          sliceCoordinates,
                                          false,
                                          true);
    }
}

/**
 * Draw a slice for either projection mode (oblique, orthogonal)
 *
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
BrainOpenGLVolumeSliceDrawing::drawVolumeSliceViewProjection(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
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
        setOrthographicProjection(sliceViewPlane,
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
    }
    
    SelectionItemVoxel* voxelID = m_brain->getSelectionManager()->getVoxelIdentification();
    SelectionItemVoxelEditing* voxelEditingID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
    
    m_fixedPipelineDrawing->applyClippingPlanes(BrainOpenGLFixedPipeline::CLIPPING_DATA_TYPE_VOLUME,
                                                StructureEnum::ALL);
    
    /*
     * Check for a 'selection' type mode
     */
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
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    resetIdentification();
    
    /*
     * Disable culling so that both sides of the triangles/quads are drawn.
     */
    GLboolean cullFaceOn = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            if (m_modelVolume != NULL) {
                const bool cullingFlag = true;
                if (cullingFlag) {
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
                drawOrthogonalSlice(sliceViewPlane,
                                sliceCoordinates,
                                slicePlane);
            }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
        {
            /*
             * Create the oblique slice transformation matrix
             */
            Matrix4x4 obliqueTransformationMatrix;
            createObliqueTransformationMatrix(sliceCoordinates,
                                              obliqueTransformationMatrix);
            
            drawObliqueSlice(sliceViewPlane,
                             obliqueTransformationMatrix,
                             slicePlane);
        }
            break;
    }
    
    /*
     * Process selection
     */
    if (m_identificationModeFlag) {
        processIdentification();
    }
    
    drawIdentificationSymbols(slicePlane);
    
    if ( ! m_identificationModeFlag) {
        if (slicePlane.isValidPlane()) {
            drawLayers(sliceDrawingType,
                       sliceProjectionType,
                       sliceViewPlane,
                       slicePlane,
                       sliceCoordinates);
        }
    }
    
    /*
     * Draw model space annotaitons on the volume slice
     */
    m_fixedPipelineDrawing->m_annotationDrawing->drawModelSpaceAnnotationsOnVolumeSlice(slicePlane,
                                                                                        m_fixedPipelineDrawing->m_tabViewport);
    
    m_fixedPipelineDrawing->disableClippingPlanes();
    
    if (cullFaceOn) {
        glEnable(GL_CULL_FACE);
    }
}

/**
 * Draw an oblique slice.
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param transformationMatrix
 *    The for oblique viewing.
 * @param plane
 *    Plane equation for the selected slice.
 */
void
BrainOpenGLVolumeSliceDrawing::drawObliqueSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                              Matrix4x4& transformationMatrix,
                                              const Plane& plane)
{
    /*
     * When performing voxel identification for editing voxels,
     * we need to draw EVERY voxel since the user may click
     * regions where the voxels are "off".
     */
    float voxelEditingValue = 1.0;
    VolumeFile* voxelEditingVolumeFile = NULL;
    bool volumeEditingDrawAllVoxelsFlag = false;
    if (m_identificationModeFlag) {
        SelectionItemVoxelEditing* voxelEditID = m_brain->getSelectionManager()->getVoxelEditingIdentification();
        if (voxelEditID->isEnabledForSelection()) {
            voxelEditingVolumeFile = voxelEditID->getVolumeFileForEditing();
            if (voxelEditingVolumeFile != NULL) {
                volumeEditingDrawAllVoxelsFlag = true;
                if (voxelEditingVolumeFile->isMappedWithLabelTable()) {
                    if (voxelEditingVolumeFile->getNumberOfMaps() > 0) {
                        voxelEditingValue = voxelEditingVolumeFile->getMapLabelTable(0)->getUnassignedLabelKey();
                    }
                }
            }
        }
    }
    
    const bool obliqueSliceModeThreeDimFlag = false;
    
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    Matrix4x4 tm;
    tm.setMatrixFromOpenGL(m);
    
    const int32_t numVolumes = static_cast<int32_t>(m_volumeDrawInfo.size());
    
    /*
     * Get the maximum bounds of the voxels from all slices
     * and the smallest voxel spacing
     */
    float voxelBounds[6];
    float voxelSpacing[3];
    if (false == getVoxelCoordinateBoundsAndSpacing(voxelBounds,
                                                    voxelSpacing)) {
        return;
    }
    float voxelSize = std::min(voxelSpacing[0],
                               std::min(voxelSpacing[1],
                                        voxelSpacing[2]));
    
    /*
     * Use a larger voxel size for the 3D view in volume slice viewing
     * since it draws all three slices and this takes time
     */
    if (obliqueSliceModeThreeDimFlag) {
        voxelSize *= 3.0;
    }
    
    /*
     * Look at point is in center of volume
     */
    float translation[3];
    m_browserTabContent->getTranslation(translation);
    float viewOffsetX = 0.0;
    float viewOffsetY = 0.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            viewOffsetX = (m_lookAtCenter[0] + translation[0]);
            viewOffsetY = (m_lookAtCenter[1] + translation[1]);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            viewOffsetX = (m_lookAtCenter[0] + translation[0]);
            viewOffsetY = (m_lookAtCenter[2] + translation[2]);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            viewOffsetX = (m_lookAtCenter[1] + translation[1]);
            viewOffsetY = (m_lookAtCenter[2] + translation[2]);
            break;
    }
    
    float minScreenX = m_orthographicBounds[0] - viewOffsetX;
    float maxScreenX = m_orthographicBounds[1] - viewOffsetX;
    float minScreenY = m_orthographicBounds[2] - viewOffsetY;
    float maxScreenY = m_orthographicBounds[3] - viewOffsetY;
    
    
    /*
     * Get origin voxel IJK
     */
    const float zeroXYZ[3] = { 0.0, 0.0, 0.0 };
    int64_t originIJK[3];
    m_volumeDrawInfo[0].volumeFile->enclosingVoxel(zeroXYZ[0], zeroXYZ[1], zeroXYZ[2],
                                                   originIJK[0], originIJK[1], originIJK[2]);
    
    
    /*
     * Get XYZ center of origin Voxel
     */
    float originVoxelXYZ[3];
    m_volumeDrawInfo[0].volumeFile->indexToSpace(originIJK, originVoxelXYZ);
    float actualOrigin[3];
    m_volumeDrawInfo[0].volumeFile->indexToSpace(originIJK, actualOrigin);
    
    float screenOffsetX = 0.0;
    float screenOffsetY = 0.0;
    float originOffsetX = 0.0;
    float originOffsetY = 0.0;
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            screenOffsetX = m_lookAtCenter[0];
            screenOffsetY = m_lookAtCenter[1];
            originOffsetX = actualOrigin[0];
            originOffsetY = actualOrigin[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            screenOffsetX = m_lookAtCenter[0];
            screenOffsetY = m_lookAtCenter[2];
            originOffsetX = actualOrigin[0];
            originOffsetY = actualOrigin[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            screenOffsetX = m_lookAtCenter[1];
            screenOffsetY = m_lookAtCenter[2];
            originOffsetX = actualOrigin[1];
            originOffsetY = actualOrigin[2];
            break;
    }
    
    const int32_t alignVoxelsFlag = 1;
    if (alignVoxelsFlag == 1) {
        /*
         * Adjust for when selected slices are not at the origin
         */
        const float xOffset = MathFunctions::remainder(screenOffsetX, voxelSize);
        const float yOffset = MathFunctions::remainder(screenOffsetY, voxelSize);
        originOffsetX -= xOffset;
        originOffsetY -= yOffset;
        
        const int64_t numVoxelsToLeft = static_cast<int64_t>(MathFunctions::round(minScreenX + originOffsetX) / voxelSize);
        const int64_t numVoxelsToRight = static_cast<int64_t>(MathFunctions::round(maxScreenX + originOffsetX) / voxelSize);
        const int64_t numVoxelsToBottom = static_cast<int64_t>(MathFunctions::round(minScreenY + originOffsetY) / voxelSize);
        const int64_t numVoxelsToTop = static_cast<int64_t>(MathFunctions::round(maxScreenY + originOffsetY)/ voxelSize);
        
        const float halfVoxel = voxelSize / 2.0;
        
        const float firstVoxelCenterX = (numVoxelsToLeft * voxelSize) + originOffsetX;
        const float lastVoxelCenterX = (numVoxelsToRight * voxelSize) + originOffsetX;
        
        const float firstVoxelCenterY = (numVoxelsToBottom * voxelSize) + originOffsetY;
        const float lastVoxelCenterY = (numVoxelsToTop * voxelSize) + originOffsetY;
        
        float newMinScreenX = firstVoxelCenterX - halfVoxel;
        float newMaxScreenX = lastVoxelCenterX + halfVoxel;
        float newMinScreenY = firstVoxelCenterY - halfVoxel;
        float newMaxScreenY = lastVoxelCenterY + halfVoxel;
        
        if (debugFlag) {
            const AString msg2 = ("Origin Voxel Coordinate: ("
                                  + AString::fromNumbers(actualOrigin, 3, ",")
                                  + "\n   Oblique Screen X: ("
                                  + AString::number(minScreenX)
                                  + ","
                                  + AString::number(maxScreenX)
                                  + ") Y: ("
                                  + AString::number(minScreenY)
                                  + ","
                                  + AString::number(maxScreenY)
                                  + ")\nNew X: ("
                                  + AString::number(newMinScreenX)
                                  + ","
                                  + AString::number(newMaxScreenX)
                                  + ") Y: ("
                                  + AString::number(newMinScreenY)
                                  + ","
                                  + AString::number(newMaxScreenY)
                                  + ") Diff: ("
                                  + AString::number((newMaxScreenX - newMinScreenX) / voxelSize)
                                  + ","
                                  + AString::number((newMaxScreenY - newMinScreenY) / voxelSize)
                                  + ")");
            std::cout << qPrintable(msg2) << std::endl;
        }
        
        minScreenX = newMinScreenX;
        maxScreenX = newMaxScreenX;
        minScreenY = newMinScreenY;
        maxScreenY = newMaxScreenY;
    }
    
    
    if (alignVoxelsFlag == 2) {
        const float quarterVoxelSize = voxelSize / 4.0;
        float newMinScreenX = (static_cast<int64_t>(minScreenX / voxelSize) * voxelSize) + quarterVoxelSize;
        float newMaxScreenX = (static_cast<int64_t>(maxScreenX / voxelSize) * voxelSize) - quarterVoxelSize;
        float newMinScreenY = (static_cast<int64_t>(minScreenY / voxelSize) * voxelSize) + quarterVoxelSize;
        float newMaxScreenY = (static_cast<int64_t>(maxScreenY / voxelSize) * voxelSize) - quarterVoxelSize;
        
        minScreenX = newMinScreenX;
        maxScreenX = newMaxScreenX;
        minScreenY = newMinScreenY;
        maxScreenY = newMaxScreenY;
    }
    
    /*
     * Set the corners of the screen for the respective view
     */
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            bottomLeft[0] = minScreenX;
            bottomLeft[1] = minScreenY;
            bottomLeft[2] = 0.0;
            bottomRight[0] = maxScreenX;
            bottomRight[1] = minScreenY;
            bottomRight[2] = 0.0;
            topRight[0] = maxScreenX;
            topRight[1] = maxScreenY;
            topRight[2] = 0.0;
            topLeft[0] = minScreenX;
            topLeft[1] = maxScreenY;
            topLeft[2] = 0.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            bottomLeft[0] = minScreenX;
            bottomLeft[1] = 0.0;
            bottomLeft[2] = minScreenY;
            bottomRight[0] = maxScreenX;
            bottomRight[1] = 0.0;
            bottomRight[2] = minScreenY;
            topRight[0] = maxScreenX;
            topRight[1] = 0.0;
            topRight[2] = maxScreenY;
            topLeft[0] = minScreenX;
            topLeft[1] = 0.0;
            topLeft[2] = maxScreenY;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            bottomLeft[0] = 0.0;
            bottomLeft[1] = minScreenX;
            bottomLeft[2] = minScreenY;
            bottomRight[0] = 0.0;
            bottomRight[1] = maxScreenX;
            bottomRight[2] = minScreenY;
            topRight[0] = 0.0;
            topRight[1] = maxScreenX;
            topRight[2] = maxScreenY;
            topLeft[0] = 0.0;
            topLeft[1] = minScreenX;
            topLeft[2] = maxScreenY;
            break;
    }
    
    
    /*
     * Transform the corners of the screen into model coordinates
     */
    transformationMatrix.multiplyPoint3(bottomLeft);
    transformationMatrix.multiplyPoint3(bottomRight);
    transformationMatrix.multiplyPoint3(topRight);
    transformationMatrix.multiplyPoint3(topLeft);
    
    if (debugFlag) {
        const double bottomDist = MathFunctions::distance3D(bottomLeft, bottomRight);
        const double topDist = MathFunctions::distance3D(topLeft, topRight);
        const double bottomVoxels = bottomDist / voxelSize;
        const double topVoxels = topDist / voxelSize;
        const AString msg = ("Bottom Dist: "
                             + AString::number(bottomDist)
                             + " voxel size: "
                             + AString::number(bottomVoxels)
                             + " Top Dist: "
                             + AString::number(bottomDist)
                             + " voxel size: "
                             + AString::number(topVoxels));
        std::cout << qPrintable(msg) << std::endl;
    }
    
    if (debugFlag) {
        m_fixedPipelineDrawing->setLineWidth(3.0);
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        glVertex3fv(bottomLeft);
        glVertex3fv(bottomRight);
        glVertex3fv(topRight);
        glVertex3fv(topLeft);
        glEnd();
    }
    
    /*
     * Unit vector and distance in model coords along left side of screen
     */
    double bottomLeftToTopLeftUnitVector[3] = {
        topLeft[0] - bottomLeft[0],
        topLeft[1] - bottomLeft[1],
        topLeft[2] - bottomLeft[2],
    };
    MathFunctions::normalizeVector(bottomLeftToTopLeftUnitVector);
    const double bottomLeftToTopLeftDistance = MathFunctions::distance3D(bottomLeft,
                                                                         topLeft);
    
    /*
     * Unit vector and distance in model coords along right side of screen
     */
    double bottomRightToTopRightUnitVector[3] = {
        topRight[0] - bottomRight[0],
        topRight[1] - bottomRight[1],
        topRight[2] - bottomRight[2]
    };
    MathFunctions::normalizeVector(bottomRightToTopRightUnitVector);
    const double bottomRightToTopRightDistance = MathFunctions::distance3D(bottomRight,
                                                                           topRight);
    /*
     * For fastest coloring, need to color data values as a group
     */
    std::vector<VolumeSlice> volumeSlices;
    for (int32_t i = 0; i < numVolumes; i++) {
        volumeSlices.push_back(VolumeSlice(m_volumeDrawInfo[i].volumeFile,
                                           m_volumeDrawInfo[i].mapIndex));
        
    }
    bool showFirstVoxelCoordFlag = debugFlag;
    
    /*
     * Track voxels that will be drawn
     */
    std::vector<VoxelToDraw*> voxelsToDraw;
    
    if ((bottomLeftToTopLeftDistance > 0)
        && (bottomRightToTopRightDistance > 0)) {
        
        const double bottomLeftToTopLeftStep = voxelSize;
        const double numLeftSteps = (bottomLeftToTopLeftDistance / bottomLeftToTopLeftStep);
        
        const double bottomRightToTopRightStep = (bottomRightToTopRightDistance
                                                  / numLeftSteps);
        
        const double dtVertical = bottomLeftToTopLeftStep / bottomLeftToTopLeftDistance;
        
        /*
         * Voxels are drawn in rows, left to right, across the screen,
         * starting at the bottom.
         */
        double leftEdgeBottomCoord[3];
        double leftEdgeTopCoord[3];
        double rightEdgeBottomCoord[3];
        double rightEdgeTopCoord[3];
        for (double tVertical = 0.0, dLeft = 0.0, dRight = 0.0;
             tVertical < 1.0;
             tVertical += dtVertical, dLeft += bottomLeftToTopLeftStep, dRight += bottomRightToTopRightStep) {
            /*
             * Coordinate on left edge at BOTTOM of current row
             */
            leftEdgeBottomCoord[0] = bottomLeft[0] + (dLeft * bottomLeftToTopLeftUnitVector[0]);
            leftEdgeBottomCoord[1] = bottomLeft[1] + (dLeft * bottomLeftToTopLeftUnitVector[1]);
            leftEdgeBottomCoord[2] = bottomLeft[2] + (dLeft * bottomLeftToTopLeftUnitVector[2]);
            
            /*
             * Coordinate on right edge at BOTTOM of current row
             */
            rightEdgeBottomCoord[0] = bottomRight[0] + (dRight * bottomRightToTopRightUnitVector[0]);
            rightEdgeBottomCoord[1] = bottomRight[1] + (dRight * bottomRightToTopRightUnitVector[1]);
            rightEdgeBottomCoord[2] = bottomRight[2] + (dRight * bottomRightToTopRightUnitVector[2]);
            
            /*
             * Coordinate on left edge at TOP of current row
             */
            leftEdgeTopCoord[0] = bottomLeft[0] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[0]);
            leftEdgeTopCoord[1] = bottomLeft[1] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[1]);
            leftEdgeTopCoord[2] = bottomLeft[2] + ((dLeft + bottomLeftToTopLeftStep) * bottomLeftToTopLeftUnitVector[2]);
            
            /*
             * Coordinate on right edge at TOP of current row
             */
            rightEdgeTopCoord[0] = bottomRight[0] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[0]);
            rightEdgeTopCoord[1] = bottomRight[1] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[1]);
            rightEdgeTopCoord[2] = bottomRight[2] + ((dRight + bottomRightToTopRightStep) * bottomRightToTopRightUnitVector[2]);
            
            /*
             * Determine change in XYZ per voxel along the bottom of the current row
             */
            const double bottomVoxelEdgeDistance = MathFunctions::distance3D(leftEdgeBottomCoord,
                                                                             rightEdgeBottomCoord);
            double bottomEdgeUnitVector[3];
            MathFunctions::createUnitVector(leftEdgeBottomCoord, rightEdgeBottomCoord, bottomEdgeUnitVector);
            const double numVoxelsInRowFloat = bottomVoxelEdgeDistance / voxelSize;
            const int64_t numVoxelsInRow = MathFunctions::round(numVoxelsInRowFloat);
            const double bottomEdgeVoxelSize = bottomVoxelEdgeDistance / numVoxelsInRow;
            const double bottomVoxelEdgeDX = bottomEdgeVoxelSize * bottomEdgeUnitVector[0];
            const double bottomVoxelEdgeDY = bottomEdgeVoxelSize * bottomEdgeUnitVector[1];
            const double bottomVoxelEdgeDZ = bottomEdgeVoxelSize * bottomEdgeUnitVector[2];
            
            /*
             * Determine change in XYZ per voxel along top of the current row
             */
            const double topVoxelEdgeDistance = MathFunctions::distance3D(leftEdgeTopCoord,
                                                                          rightEdgeTopCoord);
            double topEdgeUnitVector[3];
            MathFunctions::createUnitVector(leftEdgeTopCoord, rightEdgeTopCoord, topEdgeUnitVector);
            const double topEdgeVoxelSize = topVoxelEdgeDistance / numVoxelsInRow;
            const double topVoxelEdgeDX = topEdgeVoxelSize * topEdgeUnitVector[0];
            const double topVoxelEdgeDY = topEdgeVoxelSize * topEdgeUnitVector[1];
            const double topVoxelEdgeDZ = topEdgeVoxelSize * topEdgeUnitVector[2];
            
            /*
             * Initialize bottom and top left coordinate of first voxel in row
             */
            double bottomLeftVoxelCoord[3] = {
                leftEdgeBottomCoord[0],
                leftEdgeBottomCoord[1],
                leftEdgeBottomCoord[2]
            };
            double topLeftVoxelCoord[3] = {
                leftEdgeTopCoord[0],
                leftEdgeTopCoord[1],
                leftEdgeTopCoord[2]
            };
            
            const bool useInterpolatedVoxel = true;
            
            /*
             * Draw the voxels in the row
             */
            for (int64_t i = 0; i < numVoxelsInRow; i++) {
                /*
                 * Top right corner of voxel
                 */
                const double topRightVoxelCoord[3] = {
                    topLeftVoxelCoord[0] + topVoxelEdgeDX,
                    topLeftVoxelCoord[1] + topVoxelEdgeDY,
                    topLeftVoxelCoord[2] + topVoxelEdgeDZ
                };
                
                const float voxelCenter[3] = {
                    (bottomLeftVoxelCoord[0] + topRightVoxelCoord[0]) * 0.5,
                    (bottomLeftVoxelCoord[1] + topRightVoxelCoord[1]) * 0.5,
                    (bottomLeftVoxelCoord[2] + topRightVoxelCoord[2]) * 0.5
                };
                
                
                bool printOriginVoxelInfo = false;
                if (debugFlag) {
                    switch (sliceViewPlane) {
                        case VolumeSliceViewPlaneEnum::ALL:
                            break;
                        case VolumeSliceViewPlaneEnum::AXIAL:
                            if (showFirstVoxelCoordFlag) {
                                const float dist = voxelCenter[0] - actualOrigin[0];
                                const AString msg = ("First Voxel Center: "
                                                     + AString::fromNumbers(voxelCenter, 3, ",")
                                                     + " Dist from origin voxel in X: "
                                                     + AString::number(dist)
                                                     + " Number of voxels between: "
                                                     + AString::number(dist / voxelSize));
                                std::cout << qPrintable(msg) << std::endl;
                                showFirstVoxelCoordFlag = false;
                            }
                            if ((bottomLeftVoxelCoord[0] < actualOrigin[0])
                                && (topRightVoxelCoord[0] > actualOrigin[0])) {
                                if ((bottomLeftVoxelCoord[1] < actualOrigin[1])
                                    && (topRightVoxelCoord[1] > actualOrigin[1])) {
                                    printOriginVoxelInfo = true;
                                }
                            }
                            break;
                        case VolumeSliceViewPlaneEnum::CORONAL:
                            if (showFirstVoxelCoordFlag) {
                                const float dist = voxelCenter[0] - actualOrigin[0];
                                const AString msg = ("First Voxel Center: "
                                                     + AString::fromNumbers(voxelCenter, 3, ",")
                                                     + " Dist from origin voxel in X: "
                                                     + AString::number(dist)
                                                     + " Number of voxels between: "
                                                     + AString::number(dist / voxelSize));
                                std::cout << qPrintable(msg) << std::endl;
                                showFirstVoxelCoordFlag = false;
                            }
                            if ((bottomLeftVoxelCoord[0] < actualOrigin[0])
                                && (topRightVoxelCoord[0] > actualOrigin[0])) {
                                if ((bottomLeftVoxelCoord[2] < actualOrigin[2])
                                    && (topRightVoxelCoord[2] > actualOrigin[2])) {
                                    printOriginVoxelInfo = true;
                                }
                            }
                            break;
                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                            if (showFirstVoxelCoordFlag) {
                                const float dist = voxelCenter[1] - actualOrigin[1];
                                const AString msg = ("First Voxel Center: "
                                                     + AString::fromNumbers(voxelCenter, 3, ",")
                                                     + " Dist from origin voxel in Y: "
                                                     + AString::number(dist)
                                                     + " Number of voxels between: "
                                                     + AString::number(dist / voxelSize));
                                std::cout << qPrintable(msg) << std::endl;
                                showFirstVoxelCoordFlag = false;
                            }
                            if ((bottomLeftVoxelCoord[1] < actualOrigin[1])
                                && (topRightVoxelCoord[1] > actualOrigin[1])) {
                                if ((bottomLeftVoxelCoord[2] < actualOrigin[2])
                                    && (topRightVoxelCoord[2] > actualOrigin[2])) {
                                    printOriginVoxelInfo = true;
                                }
                            }
                            break;
                    }
                }
                if (printOriginVoxelInfo) {
                    const AString msg = ("Origin voxel center when drawn is "
                                         + AString::fromNumbers(voxelCenter, 3, ",")
                                         + " but should be "
                                         + AString::fromNumbers(actualOrigin, 3, ",")
                                         + " Voxel Corners: ("
                                         + AString::fromNumbers(bottomLeftVoxelCoord, 3, ",")
                                         + ") ("
                                         + AString::fromNumbers(topRightVoxelCoord, 3, ",")
                                         + ")");
                    std::cout << qPrintable(msg) << std::endl;
                }
                
                /*
                 * Loop through the volumes selected as overlays.
                 */
                VoxelToDraw* voxelDrawingInfo = NULL;
                
                for (int32_t iVol = 0; iVol < numVolumes; iVol++) {
                    const BrainOpenGLFixedPipeline::VolumeDrawInfo& vdi = m_volumeDrawInfo[iVol];
                    const VolumeMappableInterface* volInter = vdi.volumeFile;
                    const VolumeFile* volumeFile = volumeSlices[iVol].m_volumeFile;
                    
                    float value = 0;
                    bool valueValidFlag = false;
                    
                    bool isPaletteMappedVolumeFile = false;
                    if (volumeFile != NULL) {
                        if (volumeFile->isMappedWithPalette()) {
                            isPaletteMappedVolumeFile = true;
                        }
                    }
                    const CiftiMappableDataFile* ciftiMappableFile = volumeSlices[iVol].m_ciftiMappableDataFile;
                    
                    if (useInterpolatedVoxel
                        && isPaletteMappedVolumeFile) {
                        value = volumeFile->interpolateValue(voxelCenter,
                                                             VolumeFile::CUBIC,
                                                             &valueValidFlag,
                                                             vdi.mapIndex);
                    }
                    else if (ciftiMappableFile != NULL) {
                        const int64_t voxelOffset = ciftiMappableFile->getMapDataOffsetForVoxelAtCoordinate(voxelCenter,
                                                                                                            vdi.mapIndex);
                        if (voxelOffset >= 0) {
                            CaretAssertVectorIndex(m_ciftiMappableFileData, iVol);
                            const std::vector<float>& data = m_ciftiMappableFileData[iVol];
                            CaretAssertVectorIndex(data, voxelOffset);
                            value = data[voxelOffset];
                            valueValidFlag = true;
                        }
                    }
                    else {
                        value = volInter->getVoxelValue(voxelCenter,
                                                        &valueValidFlag,
                                                        vdi.mapIndex);
                    }
                    
                    /*
                     * Need to draw all voxels when editing
                     */
                    if (volumeEditingDrawAllVoxelsFlag) {
                        if (! valueValidFlag) {
                            if (volumeFile != NULL) {
                                if (volumeFile == voxelEditingVolumeFile) {
                                    value = voxelEditingValue;
                                    valueValidFlag = true;
                                }
                            }
                        }
                    }
                    
                    if (valueValidFlag) {
                        if (voxelDrawingInfo == NULL) {
                            /*
                             * Bottom right corner of voxel
                             */
                            const double bottomRightVoxelCoord[3] = {
                                bottomLeftVoxelCoord[0] + bottomVoxelEdgeDX,
                                bottomLeftVoxelCoord[1] + bottomVoxelEdgeDY,
                                bottomLeftVoxelCoord[2] + bottomVoxelEdgeDZ
                            };
                            
                            /*
                             * Top right corner of voxel
                             */
                            const double topRightVoxelCoord[3] = {
                                topLeftVoxelCoord[0] + topVoxelEdgeDX,
                                topLeftVoxelCoord[1] + topVoxelEdgeDY,
                                topLeftVoxelCoord[2] + topVoxelEdgeDZ
                            };
                            
                            voxelDrawingInfo = new VoxelToDraw(voxelCenter,
                                                               bottomLeftVoxelCoord,
                                                               bottomRightVoxelCoord,
                                                               topRightVoxelCoord,
                                                               topLeftVoxelCoord);
                            voxelsToDraw.push_back(voxelDrawingInfo);
                        }
                        
                        const int64_t offset = volumeSlices[iVol].addValue(value);
                        voxelDrawingInfo->addVolumeValue(iVol, offset);
                    }
                }
                
                /*
                 * Move to the next voxel in the row
                 */
                bottomLeftVoxelCoord[0] += bottomVoxelEdgeDX;
                bottomLeftVoxelCoord[1] += bottomVoxelEdgeDY;
                bottomLeftVoxelCoord[2] += bottomVoxelEdgeDZ;
                topLeftVoxelCoord[0] += topVoxelEdgeDX;
                topLeftVoxelCoord[1] += topVoxelEdgeDY;
                topLeftVoxelCoord[2] += topVoxelEdgeDZ;
            }
        }
    }
    
    const int32_t browserTabIndex = m_browserTabContent->getTabNumber();
    const DisplayPropertiesLabels* displayPropertiesLabels = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    
    /*
     * Color voxel values
     */
    for (int32_t i = 0; i < numVolumes; i++) {
        const int64_t numValues = static_cast<int64_t>(volumeSlices[i].m_values.size());
        if (numValues > 0) {
            volumeSlices[i].allocateColors();
            
            VolumeMappableInterface* volume = volumeSlices[i].m_volumeMappableInterface;
            CaretMappableDataFile* mappableFile = dynamic_cast<CaretMappableDataFile*>(volume);
            VolumeFile* volumeFile = volumeSlices[i].m_volumeFile;
            
            CaretAssert(mappableFile);
            const int32_t mapIndex = volumeSlices[i].m_mapIndex;
            const float* values = &volumeSlices[i].m_values[0];
            uint8_t* rgba = &volumeSlices[i].m_rgba[0];
            
            if (volumeEditingDrawAllVoxelsFlag
                && (volumeFile == voxelEditingVolumeFile)) {
                for (int64_t i = 0; i < numValues; i++) {
                    const int64_t i4 = i * 4;
                    rgba[i4] = 255;
                    rgba[i4+1] = 255;
                    rgba[i4+2] = 255;
                    rgba[i4+3] = 255;
                }
            }
            else if (mappableFile->isMappedWithPalette()) {
                const PaletteColorMapping* paletteColorMapping = mappableFile->getMapPaletteColorMapping(mapIndex);
                const AString paletteName = paletteColorMapping->getSelectedPaletteName();
                const Palette* palette = m_paletteFile->getPaletteByName(paletteName);
                if (palette != NULL) {
                    CaretAssertVectorIndex(m_volumeDrawInfo, i);
                    NodeAndVoxelColoring::colorScalarsWithPalette(m_volumeDrawInfo[i].statistics,
                                                                  paletteColorMapping,
                                                                  palette,
                                                                  values,
                                                                  values,
                                                                  numValues,
                                                                  rgba);
                }
                else {
                    CaretLogWarning("Missing palette named: "
                                    + paletteName);
                }
            }
            else if (mappableFile->isMappedWithLabelTable()) {
                GiftiLabelTable* labelTable = mappableFile->getMapLabelTable(mapIndex);
                NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTab(labelTable,
                                                                                   values,
                                                                                   numValues,
                                                                                   displayGroup,
                                                                                   browserTabIndex,
                                                                                   rgba);
            }
            else {
                CaretAssert(0);
            }
        }
    }
    
    const int64_t numVoxelsToDraw = static_cast<int64_t>(voxelsToDraw.size());
    
    /*
     * quadCoords is the coordinates for all four corners of a 'quad'
     * that is used to draw a voxel.  quadRGBA is the colors for each
     * voxel drawn as a 'quad'.
     */
    std::vector<float> quadCoordsVector;
    std::vector<float> quadNormalsVector;
    std::vector<uint8_t> quadRGBAsVector;
    
    /*
     * Reserve space to avoid reallocations
     */
    const int64_t coordinatesPerQuad = 4;
    const int64_t componentsPerCoordinate = 3;
    const int64_t colorComponentsPerCoordinate = 4;
    quadCoordsVector.resize(numVoxelsToDraw
                            * coordinatesPerQuad
                            * componentsPerCoordinate);
    quadNormalsVector.resize(quadCoordsVector.size());
    quadRGBAsVector.resize(numVoxelsToDraw *
                           coordinatesPerQuad *
                           colorComponentsPerCoordinate);
    
    int64_t coordOffset = 0;
    int64_t normalOffset = 0;
    int64_t rgbaOffset = 0;
    
    float*   quadCoords  = &quadCoordsVector[0];
    float*   quadNormals = &quadNormalsVector[0];
    uint8_t* quadRGBAs   = &quadRGBAsVector[0];
    
    for (int64_t iVox = 0; iVox < numVoxelsToDraw; iVox++) {
        CaretAssertVectorIndex(voxelsToDraw, iVox);
        VoxelToDraw* vtd = voxelsToDraw[iVox];
        CaretAssert(vtd);
        
        uint8_t voxelRGBA[4] = { 0, 0, 0, 0 };
        
        const int32_t numSlicesForVoxel = static_cast<int32_t>(vtd->m_sliceIndices.size());
        for (int32_t iSlice = 0; iSlice < numSlicesForVoxel; iSlice++) {
            CaretAssertVectorIndex(vtd->m_sliceIndices, iSlice);
            CaretAssertVectorIndex(vtd->m_sliceOffsets, iSlice);
            const int32_t sliceIndex = vtd->m_sliceIndices[iSlice];
            const int64_t voxelOffset = vtd->m_sliceOffsets[iSlice];
            
            const uint8_t* rgba = volumeSlices[sliceIndex].getRgbaForValueByIndex(voxelOffset);
            if (rgba[3] > 0) {
                voxelRGBA[0] = rgba[0];
                voxelRGBA[1] = rgba[1];
                voxelRGBA[2] = rgba[2];
                voxelRGBA[3] = rgba[3];
                
                if (m_identificationModeFlag) {
                    VolumeMappableInterface* volMap = volumeSlices[sliceIndex].m_volumeMappableInterface;
                    int64_t voxelI, voxelJ, voxelK;
                    volMap->enclosingVoxel(vtd->m_center[0], vtd->m_center[1], vtd->m_center[2],
                                           voxelI, voxelJ, voxelK);
                    
                    if (volMap->indexValid(voxelI, voxelJ, voxelK)) {
                        float diffXYZ[3];
                        vtd->getDiffXYZ(diffXYZ);
                        addVoxelToIdentification(sliceIndex,
                                                 volumeSlices[sliceIndex].m_mapIndex,
                                                 voxelI,
                                                 voxelJ,
                                                 voxelK,
                                                 diffXYZ,
                                                 voxelRGBA);
                    }
                }
            }
        }
        
        if (voxelRGBA[3] > 0) {
            float sliceNormalVector[3];
            plane.getNormalVector(sliceNormalVector);
            
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadRGBAsVector, rgbaOffset + 3);
            quadRGBAs[rgbaOffset]   = voxelRGBA[0];
            quadRGBAs[rgbaOffset+1] = voxelRGBA[1];
            quadRGBAs[rgbaOffset+2] = voxelRGBA[2];
            quadRGBAs[rgbaOffset+3] = voxelRGBA[3];
            rgbaOffset += 4;
            
            CaretAssertVectorIndex(quadNormalsVector, normalOffset + 2);
            quadNormals[normalOffset]   = sliceNormalVector[0];
            quadNormals[normalOffset+1] = sliceNormalVector[1];
            quadNormals[normalOffset+2] = sliceNormalVector[2];
            normalOffset += 3;
            
            CaretAssertVectorIndex(quadCoordsVector, coordOffset + 11);
            for (int32_t iq = 0; iq < 12; iq++) {
                quadCoords[coordOffset + iq] = vtd->m_coordinates[iq];
            }
            coordOffset += 12;
        }
    }
    
    quadCoordsVector.resize(coordOffset);
    quadNormalsVector.resize(normalOffset);
    quadRGBAsVector.resize(rgbaOffset);
    
    for (std::vector<VoxelToDraw*>::iterator iter = voxelsToDraw.begin();
         iter != voxelsToDraw.end();
         iter++) {
        VoxelToDraw* vtd = *iter;
        delete vtd;
    }
    voxelsToDraw.clear();
    
    if ( ! quadCoordsVector.empty()) {
        glPushMatrix();
        BrainOpenGLPrimitiveDrawing::drawQuads(quadCoordsVector,
                                               quadNormalsVector,
                                               quadRGBAsVector);
        glPopMatrix();
    }
}

/**
 * Draw an orthogonal slice.
 *
 * @param sliceViewPlane
 *    The plane for slice drawing.
 * @param sliceCoordinates
 *    Coordinates of the selected slice.
 * @param plane
 *    Plane equation for the selected slice.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const float sliceCoordinates[3],
                                                 const Plane& plane)
{
    const int32_t browserTabIndex = m_browserTabContent->getTabNumber();
    const DisplayPropertiesLabels* displayPropertiesLabels = m_brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    
    /*
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /*
     * Flat shading voxels not interpolated
     */
    glShadeModel(GL_FLAT);
    
    CaretAssert(plane.isValidPlane());
    if (plane.isValidPlane() == false) {
        return;
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
        int64_t dimI, dimJ, dimK, numMaps, numComponents;
        volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
        const int64_t mapIndex = volInfo.mapIndex;
        
        float originX, originY, originZ;
        volumeFile->indexToSpace(0, 0, 0, originX, originY, originZ);
        
        float x1, y1, z1;
        volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
        const float voxelStepX = x1 - originX;
        const float voxelStepY = y1 - originY;
        const float voxelStepZ = z1 - originZ;
        
 
        /*
         * Determine index of slice being viewed for the volume
         */
        float coordinateOnSlice[3] = {
            originX,
            originY,
            originZ
        };
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                coordinateOnSlice[2] = selectedSliceCoordinate;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                coordinateOnSlice[1] = selectedSliceCoordinate;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                coordinateOnSlice[0] = selectedSliceCoordinate;
                break;
        }
        
        int64_t sliceIndicesForCoordinateOnSlice[3];
        volumeFile->enclosingVoxel(coordinateOnSlice[0],
                                   coordinateOnSlice[1],
                                   coordinateOnSlice[2],
                                   sliceIndicesForCoordinateOnSlice[0],
                                   sliceIndicesForCoordinateOnSlice[1],
                                   sliceIndicesForCoordinateOnSlice[2]);
        
        int64_t sliceIndexForDrawing = -1;
        int64_t numVoxelsInSlice = 0;
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                sliceIndexForDrawing = sliceIndicesForCoordinateOnSlice[2];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimK)) {
                    continue;
                }
                numVoxelsInSlice = dimI * dimJ;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                sliceIndexForDrawing = sliceIndicesForCoordinateOnSlice[1];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimJ)) {
                    continue;
                }
                numVoxelsInSlice = dimI * dimK;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                sliceIndexForDrawing = sliceIndicesForCoordinateOnSlice[0];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimI)) {
                    continue;
                }
                numVoxelsInSlice = dimJ * dimK;
                break;
        }
        
        /*
         * Stores RGBA values for each voxel.
         * Use a vector for voxel colors so no worries about memory being freed.
         */
        const int64_t numVoxelsInSliceRGBA = numVoxelsInSlice * 4;
        if (numVoxelsInSliceRGBA > static_cast<int64_t>(sliceVoxelsRgbaVector.size())) {
            sliceVoxelsRgbaVector.resize(numVoxelsInSliceRGBA);
        }
        uint8_t* sliceVoxelsRGBA = &sliceVoxelsRgbaVector[0];
        
        /*
         * Get colors for all voxels in the slice.
         */
        const int64_t validVoxelCount =
           volumeFile->getVoxelColorsForSliceInMap(m_brain->getPaletteFile(),
                                                mapIndex,
                                                sliceViewPlane,
                                                sliceIndexForDrawing,
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
                    xdim = dimI;
                    ydim = dimJ;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    xdim = dimI;
                    ydim = dimK;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    xdim = dimJ;
                    ydim = dimK;
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

        int64_t selectedSliceIndices[3];
        volumeFile->enclosingVoxel(sliceCoordinates[0],
                                   sliceCoordinates[1],
                                   sliceCoordinates[2],
                                   selectedSliceIndices[0],
                                   selectedSliceIndices[1],
                                   selectedSliceIndices[2]);
        
        const uint8_t volumeDrawingOpacity = static_cast<int8_t>(volInfo.opacity * 255.0);
        
        /*
         * Setup for drawing the voxels in the slice.
         */
        float startCoordinate[3] = {
            originX - (voxelStepX / 2.0),
            originY - (voxelStepY / 2.0),
            originZ - (voxelStepZ / 2.0)
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
                startCoordinate[2] = selectedSliceCoordinate;
                rowStep[1] = voxelStepY;
                columnStep[0] = voxelStepX;
                numberOfRows    = dimJ;
                numberOfColumns = dimI;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                startCoordinate[1] = selectedSliceCoordinate;
                rowStep[2] = voxelStepZ;
                columnStep[0] = voxelStepX;
                numberOfRows    = dimK;
                numberOfColumns = dimI;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                startCoordinate[0] = selectedSliceCoordinate;
                rowStep[2] = voxelStepZ;
                columnStep[1] = voxelStepY;
                numberOfRows    = dimK;
                numberOfColumns = dimJ;
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
    }
    
    showBrainordinateHighlightRegionOfInterest(sliceViewPlane,
                                               sliceCoordinates,
                                               sliceNormalVector);
    
    glDisable(GL_BLEND);
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
    prefs->getColorForegroundVolumeView(foregroundColor);
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
 * @param plane
 *      The plane equation.
 */
void
BrainOpenGLVolumeSliceDrawing::drawIdentificationSymbols(const Plane& plane)
{
    IdentificationManager* idManager = m_brain->getIdentificationManager();
    
    SelectionItemVoxelIdentificationSymbol* symbolID = m_brain->getSelectionManager()->getVoxelIdentificationSymbol();

    const std::vector<IdentifiedItemVoxel> voxelIDs = idManager->getIdentifiedItemsForVolume();
    
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (m_fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (symbolID->isEnabledForSelection()) {
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
    
    uint8_t rgba[4];
    const int32_t numVoxelIdSymbols = static_cast<int32_t>(voxelIDs.size());
    for (int32_t iVoxel = 0; iVoxel < numVoxelIdSymbols; iVoxel++) {
        CaretAssertVectorIndex(voxelIDs, iVoxel);
        const IdentifiedItemVoxel& voxel = voxelIDs[iVoxel];
        
        /*
         * Show symbol for node ID?
         */
        if ( ! voxel.isShowIdentificationSymbol()) {
            continue;
        }
        
        
        float xyz[3];
        voxel.getXYZ(xyz);
        
        const float symbolDiameter = voxel.getSymbolSize();
        const float halfSymbolSize = symbolDiameter / 2.0;
        
        const float dist = plane.signedDistanceToPlane(xyz);
        if (dist < halfSymbolSize) {
            if (isSelect) {
                m_fixedPipelineDrawing->colorIdentification->addItem(rgba,
                                                   SelectionItemDataTypeEnum::VOXEL_IDENTIFICATION_SYMBOL,
                                                   iVoxel);
                rgba[3] = 255;
            }
            else {
                voxel.getSymbolRGBA(rgba);
            }
            
            glPushMatrix();
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            m_fixedPipelineDrawing->drawSphereWithDiameter(rgba,
                                                           symbolDiameter);
            glPopMatrix();
        }
    }
    
    if (isSelect) {
        int voxelIdIndex = -1;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::VOXEL_IDENTIFICATION_SYMBOL,
                                         m_fixedPipelineDrawing->mouseX,
                                         m_fixedPipelineDrawing->mouseY,
                                         voxelIdIndex,
                                         depth);
        if (voxelIdIndex >= 0) {
            if (symbolID->isOtherScreenDepthCloserToViewer(depth)) {
                CaretAssertVectorIndex(voxelIDs, voxelIdIndex);
                const IdentifiedItemVoxel& voxel = voxelIDs[voxelIdIndex];
                float xyz[3];
                voxel.getXYZ(xyz);
                symbolID->setVoxelXYZ(xyz);
                symbolID->setBrain(m_brain);
                symbolID->setScreenDepth(depth);
                m_fixedPipelineDrawing->setSelectedItemScreenXYZ(symbolID, xyz);
                CaretLogFine("Selected Vertex Identification Symbol: " + QString::number(voxelIdIndex));
            }
        }
    }
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
     * Enable alpha blending so voxels that are not drawn from higher layers
     * allow voxels from lower layers to be seen.
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /*
     * Flat shading voxels not interpolated
     */
    glShadeModel(GL_FLAT);
    
    CaretAssert(plane.isValidPlane());
    if (plane.isValidPlane() == false) {
        return;
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
            CaretLogSevere("BrainOpenGLVolumeSliceDrawing::getVolumeDrawingViewDependentCulling() failed.");
            continue;
        }
        const int64_t numVoxelsI = std::abs(culledLastVoxelIJK[0] - culledFirstVoxelIJK[0]) + 1;
        const int64_t numVoxelsJ = std::abs(culledLastVoxelIJK[1] - culledFirstVoxelIJK[1]) + 1;
        const int64_t numVoxelsK = std::abs(culledLastVoxelIJK[2] - culledFirstVoxelIJK[2]) + 1;
        
        int64_t dimIJK[3], numMaps, numComponents;
        volumeFile->getDimensions(dimIJK[0], dimIJK[1], dimIJK[2], numMaps, numComponents);
        const int64_t mapIndex = volInfo.mapIndex;
        
        float firstVoxelXYZ[3];
        volumeFile->indexToSpace(culledFirstVoxelIJK[0], culledFirstVoxelIJK[1], culledFirstVoxelIJK[2],
                                 firstVoxelXYZ[0], firstVoxelXYZ[1], firstVoxelXYZ[2]);
        
        const float voxelStepX = voxelDeltaXYZ[0];
        const float voxelStepY = voxelDeltaXYZ[1];
        const float voxelStepZ = voxelDeltaXYZ[2];
        
        int64_t sliceIndexForDrawing = -1;
        int64_t numVoxelsInSlice = 0;
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                CaretAssert(0);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                sliceIndexForDrawing = culledFirstVoxelIJK[2];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimIJK[2])) {
                    continue;
                }
                numVoxelsInSlice = numVoxelsI * numVoxelsJ;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                sliceIndexForDrawing = culledFirstVoxelIJK[1];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimIJK[1])) {
                    continue;
                }
                numVoxelsInSlice = numVoxelsI * numVoxelsK;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                sliceIndexForDrawing = culledFirstVoxelIJK[0];
                if ((sliceIndexForDrawing < 0)
                    || (sliceIndexForDrawing >= dimIJK[0])) {
                    continue;
                }
                numVoxelsInSlice = numVoxelsJ * numVoxelsK;
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
        const int64_t voxelCountIJK[3] = {
            numVoxelsI,
            numVoxelsJ,
            numVoxelsK
        };
        
        const int64_t validVoxelCount =
           volumeFile->getVoxelColorsForSubSliceInMap(m_brain->getPaletteFile(),
                                                   mapIndex,
                                                   sliceViewPlane,
                                                   sliceIndexForDrawing,
                                                   culledFirstVoxelIJK,
                                                   culledLastVoxelIJK,
                                                   voxelCountIJK,
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
                    xdim = numVoxelsI;
                    ydim = numVoxelsJ;
                    break;
                case VolumeSliceViewPlaneEnum::CORONAL:
                    xdim = numVoxelsI;
                    ydim = numVoxelsK;
                    break;
                case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    xdim = numVoxelsJ;
                    ydim = numVoxelsK;
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
        
        const uint8_t volumeDrawingOpacity = static_cast<int8_t>(volInfo.opacity * 255.0);
        
        /*
         * Setup for drawing the voxels in the slice.
         */
        float startCoordinate[3] = {
            firstVoxelXYZ[0] - (voxelStepX / 2.0),
            firstVoxelXYZ[1] - (voxelStepY / 2.0),
            firstVoxelXYZ[2] - (voxelStepZ / 2.0)
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
                numberOfRows    = numVoxelsJ;
                numberOfColumns = numVoxelsI;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                rowStep[2] = voxelStepZ;
                columnStep[0] = voxelStepX;
                numberOfRows    = numVoxelsK;
                numberOfColumns = numVoxelsI;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                rowStep[2] = voxelStepZ;
                columnStep[1] = voxelStepY;
                numberOfRows    = numVoxelsK;
                numberOfColumns = numVoxelsJ;
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
    }

    showBrainordinateHighlightRegionOfInterest(sliceViewPlane,
                                               sliceCoordinates,
                                               sliceNormalVector);
    
    glDisable(GL_BLEND);
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
        {
            /*
             * Transform the slice normal vector by the oblique rotation
             * matrix so that the normal vector points out of the slice
             */
            const Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
            obliqueRotationMatrix.multiplyPoint3(sliceNormalVector);
            MathFunctions::normalizeVector(sliceNormalVector);
        }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
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
        m_lookAtCenter[0] + planeNormal[0] * 1.0,
        m_lookAtCenter[1] + planeNormal[1] * 1.0,
        m_lookAtCenter[2] + planeNormal[2] * 1.0
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
        {
            Matrix4x4 m = m_browserTabContent->getObliqueVolumeRotationMatrix();
            m.invert();
            double mat4[16];
            m.getMatrixForOpenGL(mat4);
            glMultMatrixd(mat4);
        }
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
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
                drawSurfaceOutline(slicePlane);
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
 * @param plane
 *   Plane of the volume slice on which surface outlines are drawn.
 */
void
BrainOpenGLVolumeSliceDrawing::drawSurfaceOutline(const Plane& plane)
{
    if ( ! plane.isValidPlane()) {
        return;
    }
    
    float intersectionPoint1[3];
    float intersectionPoint2[3];
    
    m_fixedPipelineDrawing->enableLineAntiAliasing();
    
    VolumeSurfaceOutlineSetModel* outlineSet = m_browserTabContent->getVolumeSurfaceOutlineSet();
    
    /*
     * Process each surface outline
     */
    const int32_t numberOfOutlines = outlineSet->getNumberOfDislayedVolumeSurfaceOutlines();
    for (int io = 0;
         io < numberOfOutlines;
         io++) {
        VolumeSurfaceOutlineModel* outline = outlineSet->getVolumeSurfaceOutlineModel(io);
        if (outline->isDisplayed()) {
            Surface* surface = outline->getSurface();
            if (surface != NULL) {
                const float thickness = outline->getThickness();
                
                int numTriangles = surface->getNumberOfTriangles();
                
                CaretColorEnum::Enum outlineColor = CaretColorEnum::BLACK;
                int32_t colorSourceBrowserTabIndex = -1;
                
                VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel = outline->getColorOrTabModel();
                VolumeSurfaceOutlineColorOrTabModel::Item* selectedColorOrTabItem = colorOrTabModel->getSelectedItem();
                switch (selectedColorOrTabItem->getItemType()) {
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
                        colorSourceBrowserTabIndex = selectedColorOrTabItem->getBrowserTabIndex();
                        break;
                    case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
                        outlineColor = selectedColorOrTabItem->getColor();
                        break;
                }
                const bool surfaceColorFlag = (colorSourceBrowserTabIndex >= 0);
                
                float* nodeColoringRGBA = NULL;
                if (surfaceColorFlag) {
                    nodeColoringRGBA = m_fixedPipelineDrawing->surfaceNodeColoring->colorSurfaceNodes(NULL,
                                                                                                      surface,
                                                                                                      colorSourceBrowserTabIndex);
                }
                
                glColor3fv(CaretColorEnum::toRGB(outlineColor));
                m_fixedPipelineDrawing->setLineWidth(thickness);
                
                /*
                 * Examine each triangle to see if it intersects the Plane
                 * in which the slice exists.
                 */
                glBegin(GL_LINES);
                for (int it = 0; it < numTriangles; it++) {
                    const int32_t* triangleNodes = surface->getTriangle(it);
                    const float* c1 = surface->getCoordinate(triangleNodes[0]);
                    const float* c2 = surface->getCoordinate(triangleNodes[1]);
                    const float* c3 = surface->getCoordinate(triangleNodes[2]);
                    
                    if (plane.triangleIntersectPlane(c1, c2, c3,
                                                     intersectionPoint1,
                                                     intersectionPoint2)) {
                        if (surfaceColorFlag) {
                            /*
                             * Use coloring assigned to the first node in the triangle
                             * but only if Alpha is valid (greater than zero).
                             */
                            const int64_t colorIndex = triangleNodes[0] * 4;
                            if (nodeColoringRGBA[colorIndex + 3] > 0.0) {
                                glColor3fv(&nodeColoringRGBA[triangleNodes[0] * 4]);
                            }
                            else {
                                continue;
                            }
                        }
                        
                        /*
                         * Draw the line where the triangle intersections the slice
                         */
                        glVertex3fv(intersectionPoint1);
                        glVertex3fv(intersectionPoint2);
                    }
                }
                glEnd();
            }
        }
    }
    
    m_fixedPipelineDrawing->disableLineAntiAliasing();
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
    CaretColorEnum::toRGBFloat(caretColor, caretColorRGBA);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           m_fixedPipelineDrawing->windowTabIndex) == false) {
        return;
    }
    const float focusDiameter = fociDisplayProperties->getFociSize(displayGroup,
                                                                   m_fixedPipelineDrawing->windowTabIndex);
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
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const bool drawCrosshairsFlag = prefs->isVolumeAxesCrosshairsDisplayed();
    bool drawCrosshairLabelsFlag = prefs->isVolumeAxesLabelsDisplayed();
    
    switch (sliceDrawingType) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            drawCrosshairLabelsFlag = false;
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
            break;
    }
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            drawAxesCrosshairsOrthoAndOblique(sliceProjectionType,
                                              sliceViewPlane,
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
            drawAxesCrosshairsOrthoAndOblique(sliceProjectionType,
                                              sliceViewPlane,
                                              sliceCoordinates,
                                              drawCrosshairsFlag,
                                              drawCrosshairLabelsFlag);
            glPopMatrix();
        }
            break;
    }
}

/**
 * Draw the axes crosshairs for an orthogonal slice.
 *
 * @param sliceProjectionType
 *    Type of projection for the slice drawing (oblique, orthogonal)
 * @param sliceViewPlane
 *    The slice plane view.
 * @param sliceCoordinates
 *    Coordinates of the selected slices.
 * @param drawCrosshairsFlag
 *    If true, draw the crosshairs.
 * @param drawCrosshairLabelsFlag
 *    If true, draw the crosshair labels.
 */
void
BrainOpenGLVolumeSliceDrawing::drawAxesCrosshairsOrthoAndOblique(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                               const float sliceCoordinates[3],
                                                                 const bool drawCrosshairsFlag,
                                                                 const bool drawCrosshairLabelsFlag)
{
    bool obliqueModeFlag = false;
    switch (sliceProjectionType) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            obliqueModeFlag = true;
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            break;
    }
    
    GLboolean depthEnabled = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST,
                  &depthEnabled);
    glDisable(GL_DEPTH_TEST);
    
    const float bigValue = 10000.0;
    
    float horizontalAxisStartXYZ[3] = {
        sliceCoordinates[0],
        sliceCoordinates[1],
        sliceCoordinates[2]
    };
    float horizontalAxisEndXYZ[3] = {
        sliceCoordinates[0],
        sliceCoordinates[1],
        sliceCoordinates[2]
    };
    
    float verticalAxisStartXYZ[3] = {
        sliceCoordinates[0],
        sliceCoordinates[1],
        sliceCoordinates[2]
    };
    float verticalAxisEndXYZ[3] = {
        sliceCoordinates[0],
        sliceCoordinates[1],
        sliceCoordinates[2]
    };
    
    if (obliqueModeFlag) {
        switch (sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                horizontalAxisStartXYZ[0] = sliceCoordinates[0];
                horizontalAxisStartXYZ[1] = sliceCoordinates[2];
                horizontalAxisStartXYZ[2] = sliceCoordinates[1];
                horizontalAxisEndXYZ[0]   = sliceCoordinates[0];
                horizontalAxisEndXYZ[1]   = sliceCoordinates[2];
                horizontalAxisEndXYZ[2]   = sliceCoordinates[1];
                
                verticalAxisStartXYZ[0] = sliceCoordinates[0];
                verticalAxisStartXYZ[1] = sliceCoordinates[1];
                verticalAxisStartXYZ[2] = sliceCoordinates[2];
                verticalAxisEndXYZ[0]   = sliceCoordinates[0];
                verticalAxisEndXYZ[1]   = sliceCoordinates[1];
                verticalAxisEndXYZ[2]   = sliceCoordinates[2];
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                horizontalAxisStartXYZ[0] = sliceCoordinates[1];
                horizontalAxisStartXYZ[1] = sliceCoordinates[2];
                horizontalAxisStartXYZ[2] = sliceCoordinates[0];
                horizontalAxisEndXYZ[0]   = sliceCoordinates[1];
                horizontalAxisEndXYZ[1]   = sliceCoordinates[2];
                horizontalAxisEndXYZ[2]   = sliceCoordinates[0];
                
                verticalAxisStartXYZ[0] = -sliceCoordinates[1];
                verticalAxisStartXYZ[1] = sliceCoordinates[0];
                verticalAxisStartXYZ[2] = sliceCoordinates[2];
                verticalAxisEndXYZ[0]   = -sliceCoordinates[1];
                verticalAxisEndXYZ[1]   = sliceCoordinates[0];
                verticalAxisEndXYZ[2]   = sliceCoordinates[2];
                break;
        }
    }
    
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
            horizontalAxisStartXYZ[0] -= bigValue;
            horizontalAxisEndXYZ[0]   += bigValue;
            
            verticalBottomText = "P";
            verticalTopText    = "A";
            verticalAxisRGBA = paraRGBA;
            verticalAxisStartXYZ[1] -= bigValue;
            verticalAxisEndXYZ[1]   += bigValue;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            horizontalLeftText   = "L";
            horizontalRightText  = "R";
            horizontalAxisRGBA = axialRGBA;
            if (obliqueModeFlag) {
                horizontalAxisStartXYZ[0] -= bigValue;
                horizontalAxisEndXYZ[0]   += bigValue;
            }
            else {
                horizontalAxisStartXYZ[0] -= bigValue;
                horizontalAxisEndXYZ[0]   += bigValue;
            }
            
            verticalBottomText = "I";
            verticalTopText    = "S";
            verticalAxisRGBA = paraRGBA;
            if (obliqueModeFlag) {
                verticalAxisStartXYZ[1] -= bigValue;
                verticalAxisEndXYZ[1]   += bigValue;
            }
            else {
                verticalAxisStartXYZ[2] -= bigValue;
                verticalAxisEndXYZ[2]   += bigValue;
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            horizontalLeftText   = "A";
            horizontalRightText  = "P";
            horizontalAxisRGBA = axialRGBA;
            if (obliqueModeFlag) {
                horizontalAxisStartXYZ[0] -= bigValue;
                horizontalAxisEndXYZ[0]   += bigValue;
            }
            else {
                horizontalAxisStartXYZ[1] -= bigValue;
                horizontalAxisEndXYZ[1]   += bigValue;
            }
            
            verticalBottomText = "I";
            verticalTopText    = "S";
            verticalAxisRGBA = coronalRGBA;
            if (obliqueModeFlag) {
                verticalAxisStartXYZ[1] -= bigValue;
                verticalAxisEndXYZ[1]   += bigValue;
            }
            else {
                verticalAxisStartXYZ[2] -= bigValue;
                verticalAxisEndXYZ[2]   += bigValue;
            }
            break;
    }
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    const int textOffset = 15;
    const int textLeftWindowXY[2] = {
        textOffset,
        (viewport[3] / 2)
    };
    const int textRightWindowXY[2] = {
        viewport[2] - textOffset,
        (viewport[3] / 2)
    };
    const int textBottomWindowXY[2] = {
        viewport[2] / 2,
        textOffset
    };
    const int textTopWindowXY[2] = {
        (viewport[2] / 2),
        viewport[3] - textOffset
    };
    
    /*
     * Crosshairs
     */
    if (drawCrosshairsFlag) {
        glLineWidth(1.0);
        glColor3fv(horizontalAxisRGBA);
        glBegin(GL_LINES);
        glVertex3fv(horizontalAxisStartXYZ);
        glVertex3fv(horizontalAxisEndXYZ);
        glEnd();
        
        glLineWidth(1.0);
        glColor3fv(verticalAxisRGBA);
        glBegin(GL_LINES);
        glVertex3fv(verticalAxisStartXYZ);
        glVertex3fv(verticalAxisEndXYZ);
        glEnd();
    }
    
    if (drawCrosshairLabelsFlag) {
        const AnnotationFontSizeEnum::Enum fontSize = AnnotationFontSizeEnum::SIZE18;
        const int32_t fontSizeInt = AnnotationFontSizeEnum::toSizeNumeric(fontSize);
        
        const int textCenter[2] = {
            textLeftWindowXY[0],
            textLeftWindowXY[1]
        };
        const int halfFontSize = fontSizeInt / 2;
        
        uint8_t backgroundRGBA[4] = {
            m_fixedPipelineDrawing->m_backgroundColorByte[0],
            m_fixedPipelineDrawing->m_backgroundColorByte[1],
            m_fixedPipelineDrawing->m_backgroundColorByte[2],
            m_fixedPipelineDrawing->m_backgroundColorByte[3]
        };
        
        GLint savedViewport[4];
        glGetIntegerv(GL_VIEWPORT, savedViewport);
        
        int vpLeftX   = savedViewport[0] + textCenter[0] - halfFontSize;
        int vpRightX  = savedViewport[0] + textCenter[0] + halfFontSize;
        int vpBottomY = savedViewport[1] + textCenter[1] - halfFontSize;
        int vpTopY    = savedViewport[1] + textCenter[1] + halfFontSize;
        MathFunctions::limitRange(vpLeftX,
                                  savedViewport[0],
                                  savedViewport[0] + savedViewport[2]);
        MathFunctions::limitRange(vpRightX,
                                  savedViewport[0],
                                  savedViewport[0] + savedViewport[2]);
        MathFunctions::limitRange(vpBottomY,
                                  savedViewport[1],
                                  savedViewport[1] + savedViewport[3]);
        MathFunctions::limitRange(vpTopY,
                                  savedViewport[1],
                                  savedViewport[1] + savedViewport[3]);
        
        const int vpSizeX = vpRightX - vpLeftX;
        const int vpSizeY = vpTopY - vpBottomY;
        glViewport(vpLeftX, vpBottomY, vpSizeX, vpSizeY);
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        std::vector<uint8_t> rgba;
        std::vector<float> coords, normals;
        
        coords.push_back(-1.0);
        coords.push_back(-1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back( 1.0);
        coords.push_back(-1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back( 1.0);
        coords.push_back( 1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        coords.push_back(-1.0);
        coords.push_back( 1.0);
        coords.push_back( 0.0);
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
        rgba.push_back(backgroundRGBA[0]);
        rgba.push_back(backgroundRGBA[1]);
        rgba.push_back(backgroundRGBA[2]);
        rgba.push_back(backgroundRGBA[3]);
        
        
        BrainOpenGLPrimitiveDrawing::drawQuads(coords,
                                               normals,
                                               rgba);
        
        glPopMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        glViewport(savedViewport[0],
                   savedViewport[1],
                   savedViewport[2],
                   savedViewport[3]);
        
        AnnotationText annotationText;
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        annotationText.setBoldEnabled(true);
        annotationText.setFontSize(fontSize);
        annotationText.setBackgroundColor(CaretColorEnum::CUSTOM);
        annotationText.setForegroundColor(CaretColorEnum::CUSTOM);
        annotationText.setCustomForegroundColor(horizontalAxisRGBA);
        annotationText.setCustomBackgroundColor(backgroundRGBA);
        annotationText.setText(horizontalLeftText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textLeftWindowXY[0],
                                                         textLeftWindowXY[1],
                                                         annotationText);
        
        annotationText.setText(horizontalRightText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textRightWindowXY[0],
                                                         textRightWindowXY[1],
                                                         annotationText);
        
        annotationText.setCustomForegroundColor(verticalAxisRGBA);
        annotationText.setText(verticalBottomText);
        m_fixedPipelineDrawing->drawTextAtViewportCoords(textBottomWindowXY[0],
                                                         textBottomWindowXY[1],
                                                         annotationText);
        
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
 * Draw orientation axes
 *
 * @param viewport
 *    The viewport region for the orientation axes.
 */
void
BrainOpenGLVolumeSliceDrawing::drawOrientationAxes(const int viewport[4])
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const bool drawCylindersFlag = prefs->isVolumeAxesCrosshairsDisplayed();
    const bool drawLabelsFlag = prefs->isVolumeAxesLabelsDisplayed();
    
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
        double eyeZ = 100.0;
        const double centerX = 0;
        const double centerY = 0;
        const double centerZ = 0;
        const double upX = 0;
        const double upY = 1;
        const double upZ = 0;
        gluLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ);
        
        /*
         * Set the modeling transformation
         */
        const Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
        double rotationMatrix[16];
        obliqueRotationMatrix.getMatrixForOpenGL(rotationMatrix);
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
        
        
        const float axialPlaneMin[3] = { 0.0, 0.0, axisMinCoord };
        const float axialPlaneMax[3] = { 0.0, 0.0, axisMaxCoord };
        const double axialTextMin[3]  = { 0.0, 0.0, textMinCoord };
        const double axialTextMax[3]  = { 0.0, 0.0, textMaxCoord };
        
        const float coronalPlaneMin[3] = { axisMinCoord, 0.0, 0.0 };
        const float coronalPlaneMax[3] = { axisMaxCoord, 0.0, 0.0 };
        const double coronalTextMin[3]  = { textMinCoord, 0.0, 0.0 };
        const double coronalTextMax[3]  = { textMaxCoord, 0.0, 0.0 };
        
        const float paraPlaneMin[3] = { 0.0, axisMinCoord, 0.0 };
        const float paraPlaneMax[3] = { 0.0, axisMaxCoord, 0.0 };
        const double paraTextMin[3]  = { 0.0, textMinCoord, 0.0 };
        const double paraTextMax[3]  = { 0.0, textMaxCoord, 0.0 };
        
        const float axesCrosshairRadius = 1.0;
        
        if (drawCylindersFlag) {
            m_fixedPipelineDrawing->drawCylinder(blue,
                                                 axialPlaneMin,
                                                 axialPlaneMax,
                                                 axesCrosshairRadius);
        }
        
        AnnotationText annotationText;
        annotationText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        annotationText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        annotationText.setFontSize(AnnotationFontSizeEnum::SIZE14);
        annotationText.setCoordinateSpace(AnnotationCoordinateSpaceEnum::MODEL);
        annotationText.setForegroundColor(CaretColorEnum::CUSTOM);
        
        if (drawLabelsFlag) {
            annotationText.setCustomForegroundColor(blue);
            annotationText.setText("I");
            m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMin,
                                                          annotationText);
            annotationText.setText("S");
            m_fixedPipelineDrawing->drawTextAtModelCoords(axialTextMax,
                                                          annotationText);
        }
        
        
        if (drawCylindersFlag) {
            m_fixedPipelineDrawing->drawCylinder(green,
                                                 coronalPlaneMin,
                                                 coronalPlaneMax,
                                                 axesCrosshairRadius);
        }
        
        if (drawLabelsFlag) {
            annotationText.setCustomForegroundColor(green);
            annotationText.setText("L");
            m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMin,
                                                          annotationText);
            annotationText.setText("R");
            m_fixedPipelineDrawing->drawTextAtModelCoords(coronalTextMax,
                                                          annotationText);
        }
        
        
        if (drawCylindersFlag) {
            m_fixedPipelineDrawing->drawCylinder(red,
                                                 paraPlaneMin,
                                                 paraPlaneMax,
                                                 axesCrosshairRadius);
        }
        
        if (drawLabelsFlag) {
            annotationText.setCustomForegroundColor(red);
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
 * Set the orthographic projection.
 *
 * @param sliceViewPlane
 *    View plane that is displayed.
 * @param viewport
 *    The viewport.
 */
void
BrainOpenGLVolumeSliceDrawing::setOrthographicProjection(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                         const int viewport[4])
{
    
    /*
     * Determine model size in screen Y when viewed
     */
    BoundingBox boundingBox;
    m_volumeDrawInfo[0].volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    
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
     * Scale ratio makes region slightly larger than model
     */
    const double zoom =  1.0;
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
    const int64_t singleQuadBytes = (validVoxelCount * bytesPerVertex * 4);
    const int64_t indexQuadBytes = (totalVertexBytes
                                    + (16 * validVoxelCount));
    
    bool drawWithQuadIndicesFlag = false;
    if (indexQuadBytes < singleQuadBytes) {
        drawWithQuadIndicesFlag = true;
    }
    
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
                    const float voxelCenterX = rowCoord[0] + halfVoxelStepX;
                    const float voxelCenterY = rowCoord[1] + halfVoxelStepY;
                    const float voxelCenterZ = rowCoord[2] + halfVoxelStepZ;
                    int64_t voxelI = 0;
                    int64_t voxelJ = 0;
                    int64_t voxelK = 0;
                    volumeInterface->enclosingVoxel(voxelCenterX, voxelCenterY, voxelCenterZ,
                                                    voxelI, voxelJ, voxelK);
                    
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
 */
void
BrainOpenGLVolumeSliceDrawing::processIdentification()
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
            if (voxelID->isOtherScreenDepthCloserToViewer(depth)) {
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
 * Create the oblique transformation matrix.
 *
 * @param sliceCoordinates
 *    Slice that is being drawn.
 * @param obliqueTransformationMatrixOut
 *    OUTPUT transformation matrix for oblique viewing.
 */
void
BrainOpenGLVolumeSliceDrawing::createObliqueTransformationMatrix(const float sliceCoordinates[3],
                                                                 Matrix4x4& obliqueTransformationMatrixOut)
{
    /*
     * Initialize the oblique transformation matrix
     */
    obliqueTransformationMatrixOut.identity();
    
    /*
     * Get the oblique rotation matrix
     */
    Matrix4x4 obliqueRotationMatrix = m_browserTabContent->getObliqueVolumeRotationMatrix();
    
    /*
     * Create the transformation matrix
     */
    obliqueTransformationMatrixOut.postmultiply(obliqueRotationMatrix);
    
    /*
     * Translate to selected coordinate
     */
    obliqueTransformationMatrixOut.translate(sliceCoordinates[0],
                                             sliceCoordinates[1],
                                             sliceCoordinates[2]);
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
    
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            bottomLeftCoord[2] = selectedSliceCoordinate;
            bottomRightCoord[2] = selectedSliceCoordinate;
            topRightCoord[2] = selectedSliceCoordinate;
            topLeftCoord[2] = selectedSliceCoordinate;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            bottomLeftCoord[1] = selectedSliceCoordinate;
            bottomRightCoord[1] = selectedSliceCoordinate;
            topRightCoord[1] = selectedSliceCoordinate;
            topLeftCoord[1] = selectedSliceCoordinate;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            bottomLeftCoord[0] = selectedSliceCoordinate;
            bottomRightCoord[0] = selectedSliceCoordinate;
            topRightCoord[0] = selectedSliceCoordinate;
            topLeftCoord[0] = selectedSliceCoordinate;
            break;
    }
    
    BoundingBox boundingBox;
    volumeFile->getVoxelSpaceBoundingBox(boundingBox);
    
    boundingBox.limitCoordinateToBoundingBox(bottomLeftCoord);
    boundingBox.limitCoordinateToBoundingBox(bottomRightCoord);
    boundingBox.limitCoordinateToBoundingBox(topRightCoord);
    boundingBox.limitCoordinateToBoundingBox(topLeftCoord);
    
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



