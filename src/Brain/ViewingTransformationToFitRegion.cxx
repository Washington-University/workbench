
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __VIEWING_TRANSFORMATION_TO_FIT_REGION_DECLARE__
#include "ViewingTransformationToFitRegion.h"
#undef __VIEWING_TRANSFORMATION_TO_FIT_REGION_DECLARE__

#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLVolumeMprTwoDrawing.h"
#include "BrainOpenGLVolumeObliqueSliceDrawing.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsRegionSelectionBox.h"
#include "HistologySlice.h"
#include "OverlaySet.h"
#include "VolumeMappableInterface.h"

using namespace caret;


    
/**
 * \class caret::ViewingTransformationToFitRegion 
 * \brief Generate a transformation to so that a region will fill the vieweport
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param viewportContent
 *    Content of the viewport from which region is fit
 * @param selectionRegion
 *    The selection bounds
 * @param browserTabContent
 *    The content of the browser tab
 */
ViewingTransformationToFitRegion::ViewingTransformationToFitRegion(const BrainOpenGLViewportContent* viewportContent,
                                                                   const GraphicsRegionSelectionBox* selectedRegion,
                                                                   const BrowserTabContent* browserTabContent)
: CaretObject(),
m_viewportContent(viewportContent),
m_selectedRegion(selectedRegion),
m_browserTabContent(browserTabContent)
{
    m_debugFlag = false;
    CaretAssert(m_viewportContent);
    CaretAssert(m_selectedRegion);
    CaretAssert(m_browserTabContent);
}

/**
 * Destructor.
 */
ViewingTransformationToFitRegion::~ViewingTransformationToFitRegion()
{
}

/**
 * Constructor.
 * @param mouseEvent
 *    The mouse event
 * @param selectionRegion
 *    The selection bounds
 * @param selectedRegionPercentageViewportWidth
 *    Percentage of viewport width occupied by selected region
 * @param selectedRegionPercentageViewportHeight
 *    Percentage of viewport height occupied by selected region
 * @param browserTabContent
 *    The content of the browser tab
 */

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ViewingTransformationToFitRegion::toString() const
{
    return "ViewingTransformationToFitRegion";
}

/**
 * @return True if MPR volume successfully fit into region and output values valid
 * @param translationIn
 *    Input with translation
 * @param sliceViewPlaneSelectedInTab
 *    The slice plane selected in the tab. This may be ALL, AXIAL, CORONAL, or PARASAGITTAL
 * @param sliceViewPlaneForFitToRegion
 *    The slice plane that is fit to plane (perhaps the slice plane containing mouse).  This may be
 * AXIAL, CORONAL, or PARASAGITTAL only.  It is NEVER ALL !!!   When ALL is selected for
 * the tab, this is the particular slice plane in the ALL view.
 * @param translationOut
 *    Output with translation
 * @param zoomOut
 *    Output with zooming
 * @return True if successful, else false.
 */
bool
ViewingTransformationToFitRegion::applyToMprVolume(const Vector3D& translationIn,
                                                   const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneSelectedInTab,
                                                   const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneForFitToRegion,
                                                   Vector3D& translationOut,
                                                   float& zoomOut)
{
    m_modeName = "Mode: Volume MPR";
    return applyToVolume(VolumeMode::MPR,
                         translationIn,
                         sliceViewPlaneSelectedInTab,
                         sliceViewPlaneForFitToRegion,
                         translationOut,
                         zoomOut);
}

/**
 * @return True if orthogonal volume successfully fit into region and output values valid
 * @param translationIn
 *    Input with translation
 * @param sliceViewPlaneSelectedInTab
 *    The slice plane selected in the tab. This may be ALL, AXIAL, CORONAL, or PARASAGITTAL
 * @param sliceViewPlaneForFitToRegion
 *    The slice plane that is fit to plane (perhaps the slice plane containing mouse).  This may be
 * AXIAL, CORONAL, or PARASAGITTAL only.  It is NEVER ALL !!!   When ALL is selected for
 * the tab, this is the particular slice plane in the ALL view.
 * @param translationOut
 *    Output with translation
 * @param zoomOut
 *    Output with zooming
 * @return True if successful, else false.
 */
bool
ViewingTransformationToFitRegion::applyToObliqueVolume(const Vector3D& translationIn,
                                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneSelectedInTab,
                                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneForFitToRegion,
                                                       Vector3D& translationOut,
                                                       float& zoomOut)
{
    const std::vector<MouseLeftDragModeEnum::Enum> mouseModes(m_browserTabContent->getSupportedMouseLeftDragModes());
    if (std::find(mouseModes.begin(),
                  mouseModes.end(),
                  MouseLeftDragModeEnum::REGION_SELECTION) == mouseModes.end()) {
        /*
         * Note: to enable this functionality, enable REGION_SELECTION for oblique slice
         * drawing in BrowserTabContent::getSupportedMouseLeftDragModes().
         */
        CaretLogSevere("Region selection with mouse is not available for Oblique Slice Viewing");
        return false;
    }
    
    m_modeName = "Mode: Volume Oblique";
    return applyToVolume(VolumeMode::Oblique,
                         translationIn,
                         sliceViewPlaneSelectedInTab,
                         sliceViewPlaneForFitToRegion,
                         translationOut,
                         zoomOut);
}

/**
 * @return True if orthogonal volume successfully fit into region and output values valid
 * @param translationIn
 *    Input with translation
 * @param sliceViewPlaneSelectedInTab
 *    The slice plane selected in the tab. This may be ALL, AXIAL, CORONAL, or PARASAGITTAL
 * @param sliceViewPlaneForFitToRegion
 *    The slice plane that is fit to plane (perhaps the slice plane containing mouse).  This may be
 * AXIAL, CORONAL, or PARASAGITTAL only.  It is NEVER ALL !!!   When ALL is selected for
 * the tab, this is the particular slice plane in the ALL view.
 * @param translationOut
 *    Output with translation
 * @param zoomOut
 *    Output with zooming
 * @return True if successful, else false.
 */
bool
ViewingTransformationToFitRegion::applyToOrthogonalVolume(const Vector3D& translationIn,
                                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneSelectedInTab,
                                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneForFitToRegion,
                                                          Vector3D& translationOut,
                                                          float& zoomOut)
{
    m_modeName = "Mode: Volume Orthogonal";
    return applyToVolume(VolumeMode::Orthogonal,
                         translationIn,
                         sliceViewPlaneSelectedInTab,
                         sliceViewPlaneForFitToRegion,
                         translationOut,
                         zoomOut);
}

/**
 *@p
 * @return True if volume successfully fit into region and output values valid
 * @param volumeMode
 *    The volume mode (type of volume view)
 * @param translationIn
 *    Input with translation
 * @param sliceViewPlaneSelectedInTab
 *    The slice plane selected in the tab. This may be ALL, AXIAL, CORONAL, or PARASAGITTAL
 * @param sliceViewPlaneForFitToRegion
 *    The slice plane that is fit to plane (perhaps the slice plane containing mouse).  This may be
 * AXIAL, CORONAL, or PARASAGITTAL only.  It is NEVER ALL !!!   When ALL is selected for
 * the tab, this is the particular slice plane in the ALL view.
 * @param translationOut
 *    Output with translation
 * @param zoomOut
 *    Output with zooming
 * @return True if successful, else false.
 */
bool
ViewingTransformationToFitRegion::applyToVolume(const VolumeMode volumeMode,
                                                const Vector3D& translationIn,
                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneSelectedInTab,
                                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlaneForFitToRegion,
                                                Vector3D& translationOut,
                                                float& zoomOut)
{
    translationOut.set(0.0, 0.0, 0.0);
    zoomOut = 1.0;
    
    if ( ! initializeData()) {
        return false;
    }
    
    const OverlaySet* overlaySet(m_browserTabContent->getOverlaySet());
    const VolumeMappableInterface* volume(overlaySet->getUnderlayVolume());
    if (volume == NULL) {
        return false;
    }
    BoundingBox voxelBoundingBox;
    volume->getVoxelSpaceBoundingBox(voxelBoundingBox);
    if ( ! voxelBoundingBox.isValid()) {
        return false;
    }
    
    if (m_debugFlag) std::cout << m_modeName << std::endl;
    if (m_debugFlag) std::cout << "   Region: " << m_selectedRegion->toString() << std::endl;
    
    Vector3D volumeCenterXYZ;
    voxelBoundingBox.getCenter(volumeCenterXYZ);
    if (m_debugFlag) std::cout << "   Volume Center: " << volumeCenterXYZ.toString(5) << std::endl;

    BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceMode(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO);
    switch (sliceViewPlaneSelectedInTab) {
        case VolumeSliceViewPlaneEnum::ALL:
            allSliceMode = BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_YES;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            allSliceMode = BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO;
            break;
    }
    
    switch (sliceViewPlaneForFitToRegion) {
        case VolumeSliceViewPlaneEnum::ALL:
        {
            const AString msg("sliceViewPlaneForFitToRegion must NEVER be ALL.  "
                              "It must be one of AXIAL, CORONAL, or PARASAGITTAL");
            CaretLogSevere(msg);
            CaretAssertMessage(0, msg);
            return false;
        }
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        case VolumeSliceViewPlaneEnum::CORONAL:
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            break;
    }
        
    std::array<int32_t, 4> viewportArray(m_viewport.getViewport());
    if (sliceViewPlaneSelectedInTab == VolumeSliceViewPlaneEnum::ALL) {
        /*
         * In ALL view, the viewport in tab is for all three of the slices.
         * Must update viewport to that for the single slice plane
         */
        int32_t newViewport[4] { 0, 0, 0, 0};
        BrainOpenGLViewportContent::getSliceAllViewViewport(viewportArray.data(),
                                                            sliceViewPlaneForFitToRegion,
                                                            m_browserTabContent->getVolumeSlicePlanesAllViewLayout(),
                                                            newViewport);
        setupViewport(newViewport);
        viewportArray = m_viewport.getViewport();
    }
    
    if (m_debugFlag) std::cout << "Viewport: " << AString::fromNumbers(viewportArray.data(), 4) << std::endl;
    {
        const GraphicsObjectToWindowTransform* xform(m_viewportContent->getVolumeGraphicsObjectToWindowTransform(sliceViewPlaneForFitToRegion));
        if (xform != NULL) {
            const std::array<float, 4> vpFloat(xform->getViewport());
            if (m_debugFlag) std::cout << "Replace viewport with XFORM: " << AString::fromNumbers(vpFloat.data(), 4) << std::endl;
            const int32_t vp[4] {
                static_cast<int32_t>(vpFloat[0]),
                static_cast<int32_t>(vpFloat[1]),
                static_cast<int32_t>(vpFloat[2]),
                static_cast<int32_t>(vpFloat[3]),
            };
            setupViewport(vp);
        }
    }
    
    float zoomFactor(1.0);
    double orthoBounds[6];
    switch (volumeMode) {
        case VolumeMode::Oblique:
            BrainOpenGLVolumeObliqueSliceDrawing::getOrthographicProjection(voxelBoundingBox,
                                                                            zoomFactor,
                                                                            allSliceMode,
                                                                            sliceViewPlaneForFitToRegion,
                                                                            viewportArray.data(),
                                                                            orthoBounds);
            break;
        case VolumeMode::MPR:
            BrainOpenGLVolumeMprTwoDrawing::getOrthographicProjection(voxelBoundingBox,
                                                                      zoomFactor,
                                                                      viewportArray,
                                                                      orthoBounds);
            break;
        case VolumeMode::Orthogonal:
            BrainOpenGLVolumeSliceDrawing::getOrthographicProjection(allSliceMode,
                                                                     sliceViewPlaneForFitToRegion,
                                                                     voxelBoundingBox,
                                                                     zoomFactor,
                                                                     viewportArray.data(),
                                                                     orthoBounds);
            break;
    }
    if (m_debugFlag) std::cout << "   Ortho bounds: " << AString::fromNumbers(orthoBounds, 6) << std::endl;
    const Vector3D orthoCenter((orthoBounds[0] + orthoBounds[1]) / 2.0,
                               (orthoBounds[2] + orthoBounds[3]) / 2.0,
                               (orthoBounds[4] + orthoBounds[5]) / 2.0);
    if (m_debugFlag) std::cout << "   Ortho Center: " << orthoCenter.toString(5) << std::endl;
    
    const float volumeSliceX(m_browserTabContent->getVolumeSliceCoordinateParasagittal());
    const float volumeSliceY(m_browserTabContent->getVolumeSliceCoordinateCoronal());
    const float volumeSliceZ(m_browserTabContent->getVolumeSliceCoordinateAxial());
    const Vector3D volumeSliceXYZ(volumeSliceX,
                                  volumeSliceY,
                                  volumeSliceZ);
    if (m_debugFlag) std::cout << "   Volume Slice XYZ: " << volumeSliceXYZ.toString(5) << std::endl;
    
    Vector3D regionCenterXYZ;
    m_selectionRegionBounds.getCenter(regionCenterXYZ);
    if (m_debugFlag) std::cout << "   Region center: " << regionCenterXYZ.toString(5) << std::endl;
    
    float zoom(1.0);
    if ( ! generateZoom(zoom)) {
        return false;
    }
    
    if (m_debugFlag) std::cout << "   Zoom: " << zoom << std::endl;
    
    
    /*
     * Center of volume is NOT the origin (0, 0, 0).
     * Origin is at the anterior commissure.
     *
     */
    Vector3D translation;
    float volumeCenterOffsetX(0.0);
    float volumeCenterOffsetY(0.0);
    float volumeCenterOffsetZ(0.0);
    switch (sliceViewPlaneForFitToRegion) {
        case VolumeSliceViewPlaneEnum::ALL:
            return false;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            translation[0] = (orthoCenter[0] - regionCenterXYZ[0]);
            translation[1] = (orthoCenter[1] - regionCenterXYZ[1]);
            volumeCenterOffsetX = volumeCenterXYZ[0];
            volumeCenterOffsetY = volumeCenterXYZ[1];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            translation[0] = (orthoCenter[0] - regionCenterXYZ[0]);
            translation[2] = (orthoCenter[1] - regionCenterXYZ[2]);
            volumeCenterOffsetX = volumeCenterXYZ[0];
            volumeCenterOffsetZ = volumeCenterXYZ[2];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            switch (volumeMode) {
                case VolumeMode::MPR:
                    translation[1] = -(-orthoCenter[0] - regionCenterXYZ[1]);
                    translation[2] = (orthoCenter[1] - regionCenterXYZ[2]);
                    volumeCenterOffsetY = -volumeCenterXYZ[1];
                    volumeCenterOffsetZ = volumeCenterXYZ[2];
                    break;
                case VolumeMode::Oblique:
                case VolumeMode::Orthogonal:
                    translation[1] = (-orthoCenter[0] - regionCenterXYZ[1]);
                    translation[2] = (orthoCenter[1] - regionCenterXYZ[2]);
                    volumeCenterOffsetY = volumeCenterXYZ[1];
                    volumeCenterOffsetZ = volumeCenterXYZ[2];
                    break;
            }
            break;
    }
    
    if (m_debugFlag) std::cout << "   Translation: " << translation.toString(5) << std::endl;
    
    Matrix4x4 m1;
    m1.translate(translation);
    switch (volumeMode) {
        case VolumeMode::Oblique:
        case VolumeMode::Orthogonal:
            m1.scale(zoom, zoom, zoom);
            break;
        case VolumeMode::MPR:
            break;
    }
    m1.translate(volumeCenterOffsetX, volumeCenterOffsetY, volumeCenterOffsetZ);
    float t[3];
    m1.getTranslation(t);
    double matrixZoom, sy, sz;
    m1.getScale(matrixZoom, sy, sz);
    CaretAssert(matrixZoom != 0.0);
    
    /*
     * Need to preserve translation along plane
     * being viewed using input translation value
     */
    switch (sliceViewPlaneForFitToRegion) {
        case VolumeSliceViewPlaneEnum::ALL:
            return false;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            t[2] = translationIn[2];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            t[1] = translationIn[1];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            t[0] = translation[0];
            break;
    }
    
    translationOut = t;

    switch (volumeMode) {
        case VolumeMode::Oblique:
        case VolumeMode::Orthogonal:
            zoomOut = matrixZoom;
            break;
        case VolumeMode::MPR:
            zoomOut = zoom;
            break;
    }
    
    if (m_debugFlag) std::cout << "   Final Trans: " << t[0] << ", " << t[1] << ", " << t[2] << std::endl;
    if (m_debugFlag) std::cout << "   Matrix Zoom: " << matrixZoom << ", " << sy << std::endl;
    if (m_debugFlag) std::cout << "          Zoom: " << zoom << std::endl;
    
    return true;
}

/**
 * @return True if media successfully fit into region and output values valid
 * @param translationOut
 *    Output with translation
 * @param zoomOut
 *    Output with zooming
 */
bool
ViewingTransformationToFitRegion::applyToMediaImage(const GraphicsObjectToWindowTransform* transform,
                                                    Vector3D& translationOut,
                                                    float& zoomOut)
{
    m_modeName = "Mode: Media Image";
    CaretAssert(transform);
    
    translationOut.set(0.0, 0.0, 0.0);
    zoomOut = 1.0;
    
    float selectionBoxCenterX(0.0), selectionBoxCenterY(0.0), selectionBoxCenterZ(0.0);
    m_selectedRegion->getCenter(selectionBoxCenterX,
                                selectionBoxCenterY,
                                selectionBoxCenterZ);
    
    /*
     * Ortho is in plane coordinates
     */
    const std::array<float, 4> orthoLRBT(transform->getOrthoLRBT());
    const float windowWidth(orthoLRBT[1] - orthoLRBT[0]);
    const float windowHeight(orthoLRBT[2] - orthoLRBT[3]); /* for images positive Y is down */
    
    const float selectionWidth(m_selectedRegion->getSizeX());
    const float selectionHeight(m_selectedRegion->getSizeY());
    
    bool resultValidFlag(false);
    
    /*
     * Ensure window and selection region are valid
     */
    if ((windowWidth > 0.0)
        && (selectionWidth > 0.0)
        && (windowHeight > 0.0)
        && (selectionHeight > 0.0)) {
        
        /*
         * Scale using width or height to best fit region into window.
         */
        const float widthScale(windowWidth / selectionWidth);
        const float heightScale(windowHeight / selectionHeight);
        const float scale(std::min(widthScale, heightScale));
        zoomOut = scale;
        
        const Vector3D orthoCenterXYZ((orthoLRBT[0] + orthoLRBT[1]) / 2.0f,
                                      (orthoLRBT[2] + orthoLRBT[3]) / 2.0f,
                                      0.0);
        
        /*
         * Translate so that center of selection box is moved
         * to the center of the screen
         * NOTE: Origin is at top so invert Y-translation
         */
        float tx =  (orthoCenterXYZ[0] - (selectionBoxCenterX * scale));
        float ty = -(orthoCenterXYZ[1] - (selectionBoxCenterY * scale));
        translationOut.set(tx, ty, 0.0);
        
        resultValidFlag = true;
    }
    
    return resultValidFlag;
}

/**
 * Setup viewport and related data
 * @param viewport
 *    Viewport dimensions
 * @return True if viewport is valid
 */
bool
ViewingTransformationToFitRegion::setupViewport(const int32_t viewport[4])
{
    m_viewport = GraphicsViewport(viewport);
    if ( ! m_viewport.isValid()) {
        return false;
    }

    m_viewportAspectRatio = (m_viewport.getHeightF()
                             / m_viewport.getWidthF());
    if (m_viewportAspectRatio <= 0.0) {
        return false;
    }

    return true;
}
/**
 * Initialize and validate input data
 * @return True if initialization was successful, else false.
 */
bool
ViewingTransformationToFitRegion::initializeData()
{
    if (m_viewportContent == NULL) {
        return false;
    }
    if (m_selectedRegion == NULL) {
        return false;
    }
    if (m_browserTabContent == NULL) {
        return false;
    }

    m_selectionRegionBounds = m_selectedRegion->getBounds();
    if ( ! m_selectionRegionBounds.isValidTwoAxis()) {
        return false;
    }

    /*
     * Viewport in which model is drawn
     */
    int32_t viewport[4];
    m_viewportContent->getModelViewport(viewport);
    if ( ! setupViewport(viewport)) {
        return false;
    }
    
    /*
     * Viewport of selected region
     */
    m_selectionRegionViewport = m_selectedRegion->getViewport();
    if ( ! m_selectionRegionViewport.isValid()) {
        return false;
    }
    m_selectionRegionViewportAspectRatio = (m_selectionRegionViewport.getHeightF()
                                            / m_selectionRegionViewport.getWidthF());
    

    return true;
}

/**
 * Generate zooming to fit region to viewport
 * @param zoomOut
 *    Output with zooming
 * @return True if zooming is valid
 */
bool
ViewingTransformationToFitRegion::generateZoom(float& zoomOut)
{
    zoomOut = 1.0;
    
    if (m_debugFlag) std::cout << "   Viewport:     " << m_viewport.toString() << std::endl;
    if (m_debugFlag) std::cout << "   Region VP: " << m_selectionRegionViewport.toString() << std::endl;
    
    if (m_selectionRegionViewportAspectRatio > m_viewportAspectRatio) {
        zoomOut = m_viewport.getHeightF() / m_selectionRegionViewport.getHeightF();
    }
    else {
        zoomOut = m_viewport.getWidthF() / m_selectionRegionViewport.getWidthF();
    }

    /*
     * User may be zoomed in/out and need to include it
     */
    zoomOut *= m_browserTabContent->getScaling();
    
    return (zoomOut != 0.0);
}
