
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
#include "BrainOpenGLVolumeSliceDrawing.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GraphicsRegionSelectionBox.h"
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
 *    The content of the viewport
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
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ViewingTransformationToFitRegion::toString() const
{
    return "ViewingTransformationToFitRegion";
}

/**
 * @return True if orthogonal volume successfully fit into region and output values valid
 * @param translationOut
 *    Output with translation
 * @param zoomOut
 *    Output with zooming
 */
bool
ViewingTransformationToFitRegion::applyToOrthogonalVolume(const Vector3D& translationIn,
                                                          Vector3D& translationOut,
                                                          float& zoomOut)
{
    const bool debugFlag(false);
    
    translationOut.set(0.0, 0.0, 0.0);
    zoomOut = 1.0;
    
    if ( ! initializeData()) {
        return false;
    }
    
    if (m_browserTabContent->getVolumeSliceProjectionType() != VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL) {
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

    if (debugFlag) std::cout << "Region: " << m_selectedRegion->toString() << std::endl;
    
    Vector3D volumeCenterXYZ;
    voxelBoundingBox.getCenter(volumeCenterXYZ);
    if (debugFlag) std::cout << "   Volume Center: " << volumeCenterXYZ.toString(5) << std::endl;
    
    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane(m_browserTabContent->getVolumeSliceViewPlane());
    switch (sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            return false;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            break;
    }

    float zoomFactor(1.0);
    const std::array<int32_t, 4> viewportArray(m_viewport.getViewport());
    double orthoBounds[6];
    BrainOpenGLVolumeSliceDrawing::getOrthographicProjection(BrainOpenGLVolumeSliceDrawing::AllSliceViewMode::ALL_NO,
                                                             sliceViewPlane,
                                                             voxelBoundingBox,
                                                             zoomFactor,
                                                             viewportArray.data(),
                                                             orthoBounds);
    if (debugFlag) std::cout << "   Ortho bounds: " << AString::fromNumbers(orthoBounds, 6) << std::endl;
    const Vector3D orthoCenter((orthoBounds[0] + orthoBounds[1]) / 2.0,
                               (orthoBounds[2] + orthoBounds[3]) / 2.0,
                               (orthoBounds[4] + orthoBounds[5]) / 2.0);
    if (debugFlag) std::cout << "   Ortho Center: " << orthoCenter.toString(5) << std::endl;
    
    const float volumeSliceX(m_browserTabContent->getVolumeSliceCoordinateParasagittal());
    const float volumeSliceY(m_browserTabContent->getVolumeSliceCoordinateCoronal());
    const float volumeSliceZ(m_browserTabContent->getVolumeSliceCoordinateAxial());
    const Vector3D volumeSliceXYZ(volumeSliceX,
                                  volumeSliceY,
                                  volumeSliceZ);
    if (debugFlag) std::cout << "   Volume Slice XYZ: " << volumeSliceXYZ.toString(5) << std::endl;
    
    Vector3D regionCenterXYZ;
    m_selectionRegionBounds.getCenter(regionCenterXYZ);
    if (debugFlag) std::cout << "   Region center: " << regionCenterXYZ.toString(5) << std::endl;
    
    float zoom(1.0);
    if ( ! generateZoom(zoom)) {
        return false;
    }
    
    if (debugFlag) std::cout << "   Zoom: " << zoom << std::endl;
    
    /*
     * Center of volume is NOT the origin (0, 0, 0).
     * Origin is at the anterior commissure.
     *
     */
    Vector3D translation;
    float volumeCenterOffsetX(0.0);
    float volumeCenterOffsetY(0.0);
    float volumeCenterOffsetZ(0.0);
    switch (sliceViewPlane) {
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
            translation[1] = (-orthoCenter[0] - regionCenterXYZ[1]);
            translation[2] = (orthoCenter[1] - regionCenterXYZ[2]);
            volumeCenterOffsetY = volumeCenterXYZ[1];
            volumeCenterOffsetZ = volumeCenterXYZ[2];
            break;
    }
    
    if (debugFlag) std::cout << "   Translation: " << translation.toString(5) << std::endl;
    
    Matrix4x4 m1;
    m1.translate(translation);
    m1.scale(zoom, zoom, zoom);
    m1.translate(volumeCenterOffsetX, volumeCenterOffsetY, volumeCenterOffsetZ);
    float t[3];
    m1.getTranslation(t);
    double sx, sy, sz;
    m1.getScale(sx, sy, sz);
    CaretAssert(sx != 0.0);
        
    /*
     * Need to preserve translation along plane
     * being viewed using input translation value
     */
    switch (sliceViewPlane) {
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
    zoomOut = sx;

    if (debugFlag) std::cout << "   Final Trans: " << t[0] << ", " << t[1] << ", " << t[2] << std::endl;
    if (debugFlag) std::cout << "   Final Zoom : " << sx << ", " << sy << std::endl;

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
    m_viewport = GraphicsViewport(viewport);
    if ( ! m_viewport.isValid()) {
        return false;
    }
    m_viewportAspectRatio = (m_viewport.getHeightF()
                             / m_viewport.getWidthF());
    
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
    
    if (m_selectionRegionViewportAspectRatio > m_viewportAspectRatio) {
        zoomOut = m_viewport.getHeightF() / m_selectionRegionViewport.getHeightF();
    }
    else {
        zoomOut = m_viewport.getWidthF() / m_selectionRegionViewport.getWidthF();
    }

    return (zoomOut != 0.0);
}
