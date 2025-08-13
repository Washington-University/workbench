
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __VOLUME_MPR_VIRTUAL_SLICE_VIEW_DECLARE__
#include "VolumeMprVirtualSliceView.h"
#undef __VOLUME_MPR_VIRTUAL_SLICE_VIEW_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DeveloperFlagsEnum.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumeMappableInterface.h"
#include "VolumePlaneIntersection.h"
#include "VolumeTextureCoordinateMapper.h"

using namespace caret;


    
/**
 * \class caret::VolumeMprVirtualSliceView
 * \brief Camera for MPR  volume slice viewing
 * \ingroup Brain
 */

/**
 * @return View type for drawing volume slices in VOLUME view
 */
VolumeMprVirtualSliceView::ViewType
VolumeMprVirtualSliceView::getViewTypeForVolumeSliceView()
{
    return VolumeMprVirtualSliceView::ViewType::VOLUME_VIEW_FIXED_CAMERA;
    
}

/**
 * @return View type for drawing  single slice volume underlay
 */
VolumeMprVirtualSliceView::ViewType
VolumeMprVirtualSliceView::getViewTypeForVolumeSingleSliceView()
{
    return  VolumeMprVirtualSliceView::ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA;
}

/**
 * @return View type for drawing volume slices in ALL view
 */
VolumeMprVirtualSliceView::ViewType
VolumeMprVirtualSliceView::getViewTypeForAllView()
{
    return VolumeMprVirtualSliceView::ViewType::ALL_VIEW_SLICES;
}

/**
 * Constructor.
 */
VolumeMprVirtualSliceView::VolumeMprVirtualSliceView()
: CaretObject()
{
}

/**
 * Constructor.
 * @param underlayVolume
 *    The underlay volume
 * @param viewType
 *    Type of view
 * @param volumeCenterXYZ
 *    Center of the volume
 * @param selectedSlicesXYZ
 *    Coordinates of the selected slices
 * @param sliceWidthHeight
 *    Max coordinates dimension of volume
 * @param sliceViewPlane
 *    The slice plane selected (axial, coronal, parasagittal)
 * @param mprOrientationMode
 *    Orientation (neurological or radiological)
 * @param sliceRotationMatrix
 *    The current rotation matrix for the slice view (threeDimRotationMatrix with possible inverse rotation for slice view))
 * @param singleSliceRotationAngle
 *    Rotation angle for single slice volume
 */
VolumeMprVirtualSliceView::VolumeMprVirtualSliceView(const VolumeMappableInterface* underlayVolume,
                                                     const ViewType viewType,
                                                     const Vector3D& volumeCenterXYZ,
                                                     const Vector3D& selectedSlicesXYZ,
                                                     const float sliceWidthHeight,
                                                     const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                     const VolumeMprOrientationModeEnum::Enum& mprOrientationMode,
                                                     const Matrix4x4& sliceRotationMatrix,
                                                     const float singleSliceRotationAngle)
: CaretObject(),
m_viewType(viewType),
m_volumeCenterXYZ(volumeCenterXYZ),
m_selectedSlicesXYZ(selectedSlicesXYZ),
m_sliceWidthHeight(sliceWidthHeight),
m_sliceViewPlane(sliceViewPlane),
m_mprOrientationMode(mprOrientationMode),
m_sliceRotationMatrix(sliceRotationMatrix),
m_singleSliceRotationAngle(singleSliceRotationAngle)
{
    m_transformationMatrix.identity();
    m_cameraXYZ.set(0.0, 0.0, 1.0);
    m_cameraLookAtXYZ.set(0.0, 0.0, 0.0);
    m_cameraUpVector.set(0.0, 1.0, 0.0);
    m_preLookAtTranslation.fill(0.0);
    m_postLookAtTranslation.fill(0.0);

    switch (m_mprOrientationMode) {
        case VolumeMprOrientationModeEnum::NEUROLOGICAL:
            m_neurologicalOrientationFlag = true;
            m_radiologicalOrientationFlag = false;
            break;
        case VolumeMprOrientationModeEnum::RADIOLOGICAL:
            m_neurologicalOrientationFlag = false;
            m_radiologicalOrientationFlag = true;
            break;
    }

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());

    switch (getViewType()) {
        case ViewType::VOLUME_VIEW_FIXED_CAMERA:
            initializeModeVolumeViewFixedCamera();
            break;
        case ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            initializeModeVolumeSingleSliceFixedCamera(underlayVolume);
            break;
        case ViewType::ALL_VIEW_SLICES:
            CaretAssertMessage(0, "This mode does not work for ALL view but Fixed Camera mode does.");
            initializeModeAllViewSlices();
            break;
    }
}

/**
 * Initialize for ALL view mode slices
 */
void
VolumeMprVirtualSliceView::initializeModeAllViewSlices()
{
    Vector3D planeRightVector(0.0, 0.0, 0.0);
    
    Vector3D planeUpVector(0.0, 0.0, 0.0);

    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            if (m_radiologicalOrientationFlag) {
                planeRightVector.set(-1.0, 0.0, 0.0);
            }
            else {
                planeRightVector.set(1.0, 0.0, 0.0);
            }
            planeUpVector.set(0.0, 1.0, 0.0);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            if (m_radiologicalOrientationFlag) {
                planeRightVector.set(-1.0, 0.0, 0.0);
            }
            else {
                planeRightVector.set(1.0, 0.0, 0.0);
            }
            planeUpVector.set(0.0, 0.0, 1.0);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            planeRightVector.set(0.0, -1.0, 0.0);
            planeUpVector.set(0.0, 0.0, 1.0);
            break;
    }
    
    Vector3D v(m_volumeCenterXYZ);
    Vector3D vRight(m_volumeCenterXYZ + planeRightVector);
    Vector3D vUp(m_volumeCenterXYZ + planeUpVector);

    switch (getViewType()) {
        case ViewType::VOLUME_VIEW_FIXED_CAMERA:
            CaretAssert(0);
            break;
        case ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            CaretAssert(0);
            break;
        case ViewType::ALL_VIEW_SLICES:
            m_sliceRotationMatrix.multiplyPoint3(v);
            m_sliceRotationMatrix.multiplyPoint3(vRight);
            m_sliceRotationMatrix.multiplyPoint3(vUp);
            break;
    }
    
    /*
     * Set transformation matrix
     */
    const Vector3D transXYZ(m_selectedSlicesXYZ);
    m_transformationMatrix.translate(-transXYZ);
    m_transformationMatrix.postmultiply(m_sliceRotationMatrix);
    m_transformationMatrix.translate(transXYZ);

    planeRightVector = (vRight - v).normal();
    planeUpVector    = (vUp - v).normal();
    Vector3D originalSlicePlaneVector(planeRightVector.cross(planeUpVector));
    
    /*
     * Create virtual slice plane with camera look at on plane
     */
    m_originalPlane = Plane(originalSlicePlaneVector,
                                m_selectedSlicesXYZ);
    
    /*
     * Does increasing slice coordinate direction face to the
     * user or away from the user
     */
    bool sameDirectionFlag(false);
    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            /*
             * In an axial view, the viewing vector that points to user
             * is inferior to superior and so is increasing Z if in
             * neurological orientation
             */
            sameDirectionFlag = true;
            
            /*
             * Radiological orientation flips viewing vector
             */
            if (m_radiologicalOrientationFlag) {
                sameDirectionFlag = ( ! sameDirectionFlag);
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            /*
             * In coronal view, the viewing vector that points to user
             * is posterior to anterior and so is DECREASING Y if in
             * neurological orientation
             */
            sameDirectionFlag = false;
            
            /*
             * Radiological orientation flips viewing vector
             */
            if (m_radiologicalOrientationFlag) {
                sameDirectionFlag = ( ! sameDirectionFlag);
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            /*
             * In parasagittal view, viewing vector that points to user
             * is right to left and so is INCREASING X if in
             * neurological orientation
             */
            sameDirectionFlag = true;
            break;
    }
    
    computeVirtualSlicePlane();

    Vector3D virtualSliceVector;
    m_virtualPlane.getNormalVector(virtualSliceVector);
    if (m_radiologicalOrientationFlag) {
        /*
         * Need to flip sign of normal vector
         */
        m_montageTopLeftSliceDirectionPlane = Plane(-virtualSliceVector,
                                                              m_selectedSlicesXYZ);
    }
    else {
        m_montageTopLeftSliceDirectionPlane = Plane(virtualSliceVector,
                                                              m_selectedSlicesXYZ);
    }
}

/**
 * Compute the virtual slice plane of slice that is drawn
 */
void
VolumeMprVirtualSliceView::computeVirtualSlicePlane()
{
    
    switch (getViewType()) {
        case ViewType::VOLUME_VIEW_FIXED_CAMERA:
        {
            Vector3D lookFromVec((m_cameraXYZ - m_cameraLookAtXYZ).normal());
            Matrix4x4 invMat(m_sliceRotationMatrix);
            if (invMat.invert()) {
                invMat.multiplyPoint3(lookFromVec);
                m_virtualPlane = Plane(lookFromVec, m_selectedSlicesXYZ);
            }
        }
            break;
        case ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            {
                Vector3D lookFromVec((m_cameraXYZ - m_cameraLookAtXYZ).normal());
                m_virtualPlane = Plane(lookFromVec, m_selectedSlicesXYZ);
            }
            break;
        case ViewType::ALL_VIEW_SLICES:
            m_virtualPlane = m_originalPlane;
            break;
    }
}

/**
 * Initialize for volume view with camera in a fixed position
 */
void
VolumeMprVirtualSliceView::initializeModeVolumeViewFixedCamera()
{
    const float cameraOffsetDistance(m_sliceWidthHeight * 2.0);
    
    m_cameraXYZ.set(0.0, 0.0, 0.0);
    m_cameraUpVector.set(0.0, 0.0, 0.0);
    
    Vector3D planeRightVector(0.0, 0.0, 0.0);
    
    Vector3D planeUpVector(0.0, 0.0, 0.0);
    
    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            /*
             * Neurological:
             *   Looking towards negative Z axis from positive Z
             *   Right is +X
             *   Up    is +Y
             * Radiological:
             */
            m_cameraUpVector.set(0.0, 1.0, 0.0);
            
            if (m_radiologicalOrientationFlag) {
                planeRightVector.set(-1.0, 0.0, 0.0);
            }
            else {
                planeRightVector.set(1.0, 0.0, 0.0);
            }
            planeUpVector.set(0.0, 1.0, 0.0);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            /*
             * Neurological:
             *   Looking towards positive Y from negative Y
             *   Right is +X
             *   Up    is +Z
             * Radiological:
             */
            m_cameraUpVector.set(0.0, 0.0, 1.0);
            
            if (m_radiologicalOrientationFlag) {
                planeRightVector.set(-1.0, 0.0, 0.0);
            }
            else {
                planeRightVector.set(1.0, 0.0, 0.0);
            }
            planeUpVector.set(0.0, 0.0, 1.0);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            /*
             * Neurological:
             *   Looking towards positive X from negative X
             *   Right is -Y
             *   Up    is +Z
             * Radiological:
             *   None
             */
            m_cameraUpVector.set(0.0, 0.0, 1.0);
            
            planeRightVector.set(0.0, -1.0, 0.0);
            planeUpVector.set(0.0, 0.0, 1.0);
            break;
    }
    
    
    /*
     * Set transformation matrix
     */
    const Vector3D transXYZ(m_selectedSlicesXYZ);
    m_transformationMatrix.translate(-transXYZ);
    m_transformationMatrix.postmultiply(m_sliceRotationMatrix);
    m_transformationMatrix.translate(transXYZ);

    
    const Vector3D virtualSlicePlaneVector(planeRightVector.cross(planeUpVector));
    
    /*
     * Virtual slice plane is placed at the selected slice coordinates
     */
    m_originalPlane = Plane(virtualSlicePlaneVector,
                                m_selectedSlicesXYZ);
    CaretAssert(m_originalPlane.isValidPlane());
    
    /*
     * Camera "look at" is always center of the volume
     */
    m_cameraLookAtXYZ = m_volumeCenterXYZ;
    
    /*
     * Set camera position
     */
    m_cameraXYZ = (m_cameraLookAtXYZ
                   + (virtualSlicePlaneVector * cameraOffsetDistance));
    
    /*
     * Does increasing slice coordinate direction face to the
     * user or away from the user
     */
    bool sameDirectionFlag(false);
    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            /*
             * In an axial view, the viewing vector that points to user
             * is inferior to superior and so is increasing Z if in
             * neurological orientation
             */
            sameDirectionFlag = true;
            
            /*
             * Radiological orientation flips viewing vector
             */
            if (m_radiologicalOrientationFlag) {
                sameDirectionFlag = ( ! sameDirectionFlag);
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            /*
             * In coronal view, the viewing vector that points to user
             * is posterior to anterior and so is DECREASING Y if in
             * neurological orientation
             */
            sameDirectionFlag = false;
            
            /*
             * Radiological orientation flips viewing vector
             */
            if (m_radiologicalOrientationFlag) {
                sameDirectionFlag = ( ! sameDirectionFlag);
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            /*
             * In parasagittal view, viewing vector that points to user
             * is right to left and so is INCREASING X if in
             * neurological orientation
             */
            sameDirectionFlag = true;
            break;
    }
    
    computeVirtualSlicePlane();
    
    if (m_radiologicalOrientationFlag) {
        /*
         * Need to flip sign of normal vector
         */
        m_montageTopLeftSliceDirectionPlane = Plane(m_virtualPlane.getNormalVector(),
                                                              m_selectedSlicesXYZ);
    }
    else {
        switch (m_sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                m_montageTopLeftSliceDirectionPlane = Plane(m_virtualPlane.getNormalVector(),
                                                            m_selectedSlicesXYZ);
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                m_montageTopLeftSliceDirectionPlane = Plane(-m_virtualPlane.getNormalVector(),
                                                            m_selectedSlicesXYZ);
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                m_montageTopLeftSliceDirectionPlane = Plane(-m_virtualPlane.getNormalVector(),
                                                            m_selectedSlicesXYZ);
                break;
        }
    }
    
    m_preLookAtTranslation.fill(0.0);
    m_postLookAtTranslation.fill(0.0);
    
    /*
     * Prevents slices from jumping if the selected slices are changed
     * and there is non-zero rotation
     */
    if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DEVELOPER_FLAG_MPR_THREE_SLICES_CHANGED_JUMP_FIX)) {
        Vector3D t;
        m_transformationMatrix.getTranslation(t);
        switch (m_sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                m_preLookAtTranslation[0] = -t[0];
                m_preLookAtTranslation[1] = -t[1];
                m_preLookAtTranslation[2] = 0.0;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                m_preLookAtTranslation[0] = -t[0];
                m_preLookAtTranslation[1] = 0.0;
                m_preLookAtTranslation[2] = -t[2];
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                m_preLookAtTranslation[0] = 0.0;
                m_preLookAtTranslation[1] = t[1];
                m_preLookAtTranslation[2] = -t[2];
                break;
        }
    }
}

/**
 * Initialize a view for a single slice volume file
 */
void
VolumeMprVirtualSliceView::initializeModeVolumeSingleSliceFixedCamera(const VolumeMappableInterface* underlayVolume)
{
    const float cameraOffsetDistance(m_sliceWidthHeight * 2.0);
    
    m_cameraXYZ.set(0.0, 0.0, 0.0);
    m_cameraUpVector.set(0.0, 0.0, 0.0);
    
    Vector3D planeRightVector(0.0, 0.0, 0.0);
    
    Vector3D planeUpVector(0.0, 0.0, 0.0);
    
    Vector3D topLeftXYZ, topRightXYZ, bottomLeftXYZ, bottomRightXYZ;
    underlayVolume->getSingleSliceCornersXYZ(bottomLeftXYZ,
                                             bottomRightXYZ,
                                             topRightXYZ,
                                             topLeftXYZ);
    
    m_cameraUpVector = (topLeftXYZ - bottomLeftXYZ).normal();
    planeRightVector = (bottomRightXYZ - bottomLeftXYZ).normal();
    if (m_radiologicalOrientationFlag) {
        planeRightVector = -planeRightVector;
    }
    planeUpVector    = (topLeftXYZ - bottomLeftXYZ).normal();
    Vector3D virtualSlicePlaneVector(planeRightVector.cross(planeUpVector));

    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            virtualSlicePlaneVector = (bottomLeftXYZ - topLeftXYZ).normal();
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            virtualSlicePlaneVector = (bottomLeftXYZ - bottomRightXYZ).normal();
            break;
    }
    
    
    
    
    /*
     * Virtual slice plane is placed at the selected slice coordinates
     */
    m_originalPlane = Plane(virtualSlicePlaneVector,
                            m_selectedSlicesXYZ);
    CaretAssert(m_originalPlane.isValidPlane());
    
    /*
     * Camera "look at" is always center of the volume
     */
    m_cameraLookAtXYZ = m_volumeCenterXYZ;
    
    /*
     * Set camera position
     */
    m_cameraXYZ = (m_cameraLookAtXYZ
                   + (virtualSlicePlaneVector * cameraOffsetDistance));
    
    /*
     * Does increasing slice coordinate direction face to the
     * user or away from the user
     */
    bool sameDirectionFlag(false);
    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            /*
             * In an axial view, the viewing vector that points to user
             * is inferior to superior and so is increasing Z if in
             * neurological orientation
             */
            sameDirectionFlag = true;
            
            /*
             * Radiological orientation flips viewing vector
             */
            if (m_radiologicalOrientationFlag) {
                sameDirectionFlag = ( ! sameDirectionFlag);
            }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            /*
             * In coronal view, the viewing vector that points to user
             * is posterior to anterior and so is DECREASING Y if in
             * neurological orientation
             */
            sameDirectionFlag = false;
            
            /*
             * Radiological orientation flips viewing vector
             */
            if (m_radiologicalOrientationFlag) {
                sameDirectionFlag = ( ! sameDirectionFlag);
            }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            /*
             * In parasagittal view, viewing vector that points to user
             * is right to left and so is INCREASING X if in
             * neurological orientation
             */
            sameDirectionFlag = true;
            break;
    }
    
    computeVirtualSlicePlane();
    
    if (m_radiologicalOrientationFlag) {
        /*
         * Need to flip sign of normal vector
         */
        m_montageTopLeftSliceDirectionPlane = Plane(m_virtualPlane.getNormalVector(),
                                                    m_selectedSlicesXYZ);
    }
    else {
        switch (m_sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                m_montageTopLeftSliceDirectionPlane = Plane(m_virtualPlane.getNormalVector(),
                                                            m_selectedSlicesXYZ);
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                m_montageTopLeftSliceDirectionPlane = Plane(-m_virtualPlane.getNormalVector(),
                                                            m_selectedSlicesXYZ);
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                m_montageTopLeftSliceDirectionPlane = Plane(-m_virtualPlane.getNormalVector(),
                                                            m_selectedSlicesXYZ);
                break;
        }
    }
    
    m_preLookAtTranslation.fill(0.0);
    m_postLookAtTranslation.fill(0.0);
    
    /*
     * Prevents slices from jumping if the selected slices are changed
     * and there is non-zero rotation
     */
    if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DEVELOPER_FLAG_MPR_THREE_SLICES_CHANGED_JUMP_FIX)) {
        Vector3D t;
        m_transformationMatrix.getTranslation(t);
        switch (m_sliceViewPlane) {
            case VolumeSliceViewPlaneEnum::ALL:
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                m_preLookAtTranslation[0] = -t[0];
                m_preLookAtTranslation[1] = -t[1];
                m_preLookAtTranslation[2] = 0.0;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                m_preLookAtTranslation[0] = -t[0];
                m_preLookAtTranslation[1] = 0.0;
                m_preLookAtTranslation[2] = -t[2];
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                m_preLookAtTranslation[0] = 0.0;
                m_preLookAtTranslation[1] = t[1];
                m_preLookAtTranslation[2] = -t[2];
                break;
        }
    }
    
    /*
     * Set transformation matrix
     */
    m_transformationMatrix.identity();
    const Vector3D transXYZ(m_selectedSlicesXYZ);
    m_transformationMatrix.translate(-transXYZ);
    m_transformationMatrix.rotate(m_singleSliceRotationAngle,
                                  virtualSlicePlaneVector[0],
                                  virtualSlicePlaneVector[1],
                                  virtualSlicePlaneVector[2]);
    m_transformationMatrix.translate(transXYZ);
}

/**
 * Destructor.
 */
VolumeMprVirtualSliceView::~VolumeMprVirtualSliceView()
{
}

/**
 * @return The viewtype of this instance
 */
VolumeMprVirtualSliceView::ViewType
VolumeMprVirtualSliceView::getViewType() const
{
    return m_viewType;
}

/**
 * @return The slice view plane
 */
VolumeSliceViewPlaneEnum::Enum
VolumeMprVirtualSliceView::getSliceViewPlane() const
{
    return m_sliceViewPlane;
}

/**
 * Get coordinates for drawing the volume primitive with triangles
 * @param volume
 *    Volume that will be drawn and has coordinates generated
 * @param stereotaxicXyzOut
 *    Output stereotaxic coordinates for drawing the virtual slice
 * @param textureStrOut
 *    Output texture coordinates for drawing the virtual slice
 * @param layersDrawingPlaneOut
 *    Output plane for drawing layers
 */
bool
VolumeMprVirtualSliceView::getTrianglesCoordinates(const VolumeMappableInterface* volume,
                                                   std::vector<Vector3D>& stereotaxicXyzOut,
                                                   std::vector<Vector3D>& textureStrOut,
                                                   Plane& layersDrawingPlaneOut) const
{
    CaretAssert(volume);
    stereotaxicXyzOut.clear();
    textureStrOut.clear();
    layersDrawingPlaneOut = Plane();
    
    
    stereotaxicXyzOut = createVirtualSliceTriangles(volume);
    textureStrOut     = mapTextureCoordinates(volume,
                                              stereotaxicXyzOut);
    CaretAssert(stereotaxicXyzOut.size() == textureStrOut.size());

    switch (m_viewType) {
        case ViewType::ALL_VIEW_SLICES:
            break;
        case ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            break;
        case ViewType::VOLUME_VIEW_FIXED_CAMERA:
            break;
    }
    if (stereotaxicXyzOut.size() >= 3) {
        layersDrawingPlaneOut = Plane(stereotaxicXyzOut[0],
                                      stereotaxicXyzOut[1],
                                      stereotaxicXyzOut[2]);
    }
    
    return (stereotaxicXyzOut.size() >= 3);
}

/**
 * Map the volume stereotaxic coordinates to texture coordinates
 * @param volume
 *    The volume
 * @param stereotaxicXYZ
 *    Stereotaxic coordiantes that are mapped to texture coordinates
 * @return
 *    The texture coordinates
 */
std::vector<Vector3D>
VolumeMprVirtualSliceView::mapTextureCoordinates(const VolumeMappableInterface* volume,
                                    const std::vector<Vector3D>& stereotaxicXyz) const
{
    std::vector<Vector3D> textureStrOut;

    VolumeTextureCoordinateMapper mapper(volume);
    
    for (auto& volXYZ : stereotaxicXyz) {
        Vector3D xyz(volXYZ);
        textureStrOut.push_back(mapper.mapXyzToStr(xyz));
    }
    
    CaretAssert(textureStrOut.size() == stereotaxicXyz.size());
    return textureStrOut;
}

/**
 * @param volume
 *   The volume
 * @return
 *   Triangle vertices for drawing the virtual volume slice that intersects with the volume
 *   One triangle for each three vertices
 */
std::vector<Vector3D>
VolumeMprVirtualSliceView::createVirtualSliceTriangles(const VolumeMappableInterface* volume) const
{
    std::vector<Vector3D> trianglesVertices;
    
    Plane plane;
    Matrix4x4 rotationMatrix;
    switch (getViewType()) {
        case ViewType::VOLUME_VIEW_FIXED_CAMERA:
            plane = m_originalPlane;
            rotationMatrix = m_transformationMatrix;
            break;
        case ViewType::VOLUME_SINGLE_SLICE_FIXED_CAMERA:
            plane = m_virtualPlane;
            break;
        case ViewType::ALL_VIEW_SLICES:
            plane = m_virtualPlane;
            rotationMatrix.identity();
            break;
    }

    VolumePlaneIntersection vpi(volume,
                                rotationMatrix);
    Vector3D intersectionCenterXYZ;
    std::vector<Vector3D> intersectionPoints;
    AString errorMessage;
    if (vpi.intersectWithPlane(plane,
                               intersectionCenterXYZ,
                               intersectionPoints,
                               errorMessage)) {
        
        int32_t numPoints(static_cast<int32_t>(intersectionPoints.size()));
        
        if (numPoints == 0) {
            /*
             * No intersection, OK.
             */
            return trianglesVertices;
        }
        else if (numPoints < 3) {
            /*
             * Edge (2) or vertex (1) intersection
             */
            const AString msg("VolumePlaneIntersection produced "
                              + AString::number(numPoints)
                              + " intersections.  Intersection ignored.");
            CaretLogWarning(msg);
            return trianglesVertices;
        }
        
        if (numPoints > 6) {
            const AString msg("VolumePlaneIntersection produced "
                              + AString::number(numPoints)
                              + " intersections.  There should never be more than 6 intersections");
            CaretLogSevere(msg);
            
            /*
             * Limit to 6 but continue
             */
            numPoints = 6;
        }
        
        if (numPoints >= 3) {
            if (m_sliceViewPlane == VolumeSliceViewPlaneEnum::AXIAL) {
                const bool printIntersectionFlag(false);
                if (printIntersectionFlag) {
                    bool firstFlag(true);
                    for (auto& p : intersectionPoints) {
                        if ( ! firstFlag) std::cout << "   ";
                        std::cout << p.toString() << std::endl;
                        if (firstFlag) firstFlag = false;
                    }
                }
            }
            Vector3D firstPoint;
            if (numPoints > 0) {
                CaretAssertVectorIndex(intersectionPoints, 0);
                firstPoint = intersectionPoints[0];
            }
            
            /*
             * Create the triangles
             */
            for (int32_t i = 0; i < numPoints; i++) {
                /*
                 * Each triangle consists of center and two
                 * consecutive intersection points
                 */
                trianglesVertices.push_back(intersectionCenterXYZ);
                
                CaretAssertVectorIndex(intersectionPoints, i);
                trianglesVertices.push_back(intersectionPoints[i]);
                
                const int32_t nextIndex((i == numPoints - 1)
                                        ? 0
                                        : i + 1);
                CaretAssertVectorIndex(intersectionPoints, nextIndex);
                trianglesVertices.push_back(intersectionPoints[nextIndex]);
            }
        }
        
        /*
         * Maximum number of triangle formed by intersection is 6
         * But we must always have 6 triangles.  So, if needed
         * create degenerate triangles (OpenGL will not draw them).
         */
        const int32_t numIntersectionTriangles(trianglesVertices.size() / 3);
        const int32_t requiredNumberOfTriangles(6);
        for (int32_t i = numIntersectionTriangles; i < requiredNumberOfTriangles; i++) {
            Vector3D zeros(0.0, 0.0, 0.0);
            trianglesVertices.push_back(zeros);
            trianglesVertices.push_back(zeros);
            trianglesVertices.push_back(zeros);
        }
        CaretAssert( (trianglesVertices.size() / 3) == requiredNumberOfTriangles);
    }
    
    return trianglesVertices;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
VolumeMprVirtualSliceView::VolumeMprVirtualSliceView(const VolumeMprVirtualSliceView& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    this->copyHelperVolumeMprVirtualSliceView(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
VolumeMprVirtualSliceView&
VolumeMprVirtualSliceView::operator=(const VolumeMprVirtualSliceView& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj); 
        this->copyHelperVolumeMprVirtualSliceView(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
VolumeMprVirtualSliceView::copyHelperVolumeMprVirtualSliceView(const VolumeMprVirtualSliceView& obj)
{
    m_viewType = obj.m_viewType;
    
    m_volumeCenterXYZ = obj.m_volumeCenterXYZ;
    
    m_selectedSlicesXYZ = obj.m_selectedSlicesXYZ;
    
    m_sliceWidthHeight = obj.m_sliceWidthHeight;
    
    m_sliceViewPlane = obj.m_sliceViewPlane;
    
    m_mprOrientationMode = obj.m_mprOrientationMode;
    
    m_sliceRotationMatrix = obj.m_sliceRotationMatrix;
    
    m_transformationMatrix = obj.m_transformationMatrix;
    
    m_cameraXYZ = obj.m_cameraXYZ;
    
    m_cameraLookAtXYZ = obj.m_cameraLookAtXYZ;
    
    m_cameraUpVector = obj.m_cameraUpVector;
    
    m_preLookAtTranslation = obj.m_preLookAtTranslation;
    
    m_postLookAtTranslation = obj.m_postLookAtTranslation;
    
    m_originalPlane = obj.m_originalPlane;
    
    m_virtualPlane = obj.m_virtualPlane;
    
    m_montageTopLeftSliceDirectionPlane = obj.m_montageTopLeftSliceDirectionPlane;
    
    m_neurologicalOrientationFlag = obj.m_neurologicalOrientationFlag;
    
    m_radiologicalOrientationFlag = obj.m_radiologicalOrientationFlag;
}

/**
 * @return The coordinate of the camera
 */
Vector3D
VolumeMprVirtualSliceView::getCameraXYZ() const
{
    return m_cameraXYZ;
}

/**
 * @return The coordinate of what the camera is looking at
 */
Vector3D
VolumeMprVirtualSliceView::getCameraLookAtXYZ() const
{
    return m_cameraLookAtXYZ;
}

/**
 * @return The up vector of the camera
 */
Vector3D
VolumeMprVirtualSliceView::getCameraUpVector() const
{
    return m_cameraUpVector;
}

/**
 * @return Plane after any transformation/rotation for slice that is drawn
 */
Plane
VolumeMprVirtualSliceView::getVirtualPlane() const
{
    return m_virtualPlane;
}

/**
 * @return Plane before any transformation/rotation (an orthogonal vector)
 */
Plane
VolumeMprVirtualSliceView::getOriginalUtransformedPlane() const
{
    return m_originalPlane;
}

/**
 * @return Plane that contains coordinate direction from center slice to the top left slice in the montage
 * in montage view
 */
Plane
VolumeMprVirtualSliceView::getMontageTopLeftSliceDirectionPlane() const
{
    return m_montageTopLeftSliceDirectionPlane;
}

/**
 * @return Center of volume
 */
Vector3D
VolumeMprVirtualSliceView::getVolumeCenterXYZ() const
{
    return m_volumeCenterXYZ;;
}

/**
 * @return Translation after look at view is set
 */
Vector3D
VolumeMprVirtualSliceView::getPostLookAtTranslation() const
{
    return m_postLookAtTranslation;
}

/**
 * @return The transformation matrix
 */
Matrix4x4
VolumeMprVirtualSliceView::getTransformationMatrix() const
{
    return m_transformationMatrix;
}

/**
 * @return Translation before look at view is set
 */
Vector3D
VolumeMprVirtualSliceView::getPreLookAtTranslation() const
{
    return m_preLookAtTranslation;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeMprVirtualSliceView::toString() const
{
    AString txt;
    
    double rotX(0.0), rotY(0.0), rotZ(0.0);
    m_sliceRotationMatrix.getRotation(rotX, rotY, rotZ);
    const Vector3D rotAngles(rotX, rotY, rotZ);
    
    const AString transMat(m_transformationMatrix.toFormattedString("   "));
    
    txt.appendWithNewLine("m_sliceViewPlane=" + VolumeSliceViewPlaneEnum::toName(m_sliceViewPlane));
    txt.appendWithNewLine("   m_cameraXYZ=" + m_cameraXYZ.toString());
    txt.appendWithNewLine("   m_cameraLookAtXYZ=" + m_cameraLookAtXYZ.toString());
    txt.appendWithNewLine("   m_cameraUpVector=" + m_cameraUpVector.toString());
    txt.appendWithNewLine("   m_volumeCenterXYZ=" + m_volumeCenterXYZ.toString());
    txt.appendWithNewLine("   m_selectedSlicesXYZ=" + m_selectedSlicesXYZ.toString());
    txt.appendWithNewLine("   m_preLookAtTranslation=" + m_preLookAtTranslation.toString());
    txt.appendWithNewLine("   m_postLookAtTranslation=" + m_postLookAtTranslation.toString());
    txt.appendWithNewLine("   m_sliceRotationMatrix=" + rotAngles.toString());
    txt.appendWithNewLine("   m_transformationMatrix=" + transMat);

    Matrix4x4 m2(m_transformationMatrix);
    const Vector3D sliceDiffXYZ(m_selectedSlicesXYZ);
    Vector3D diffXYZ(sliceDiffXYZ);
    m2.multiplyPoint3(diffXYZ);
    txt.appendWithNewLine("   Selected Slice X Transform=" + diffXYZ.toString());
    diffXYZ = sliceDiffXYZ;
    m2.setTranslation(0.0,0.0,0.0);
    m2.multiplyPoint3(diffXYZ);
    txt.appendWithNewLine("   Selected Slice X Rotate Only=" + diffXYZ.toString());
    return txt;
}

/**
 * Get the orientation labels for the slice view.  The rotation of the slice's axis are compared
 * to the standard orientations to determine the text (or no text) that is displayed.
 * @param leftScreenLabelTextOut
 *    Label for left side
 * @param rightScreenLabelTextOut
 *    Label for right side
 * @param bottomScreenLabelTextOut
 *    Label for bottom side
 * @param topScreenLabelTextOut
 *    Label for top side
 */
void
VolumeMprVirtualSliceView::getAxisLabels(AString& leftScreenLabelTextOut,
                                         AString& rightScreenLabelTextOut,
                                         AString& bottomScreenLabelTextOut,
                                         AString& topScreenLabelTextOut) const
{
    leftScreenLabelTextOut   = "";
    rightScreenLabelTextOut  = "";
    bottomScreenLabelTextOut = "";
    topScreenLabelTextOut    = "";
    
    bool radiologicalFlag(false);
    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            radiologicalFlag = m_radiologicalOrientationFlag;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            radiologicalFlag = m_radiologicalOrientationFlag;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            break;
    }

    std::vector<VectorAndLabel> vectorsAndLabels;
    if (radiologicalFlag) {
        vectorsAndLabels.emplace_back("L",  1.0,  0.0,  0.0);
        vectorsAndLabels.emplace_back("R", -1.0,  0.0,  0.0);
    }
    else {
        vectorsAndLabels.emplace_back("L", -1.0,  0.0,  0.0);
        vectorsAndLabels.emplace_back("R",  1.0,  0.0,  0.0);
    }
    vectorsAndLabels.emplace_back("A",  0.0,  1.0,  0.0);
    vectorsAndLabels.emplace_back("P",  0.0, -1.0,  0.0);
    vectorsAndLabels.emplace_back("I",  0.0,  0.0, -1.0);
    vectorsAndLabels.emplace_back("S",  0.0,  0.0,  1.0);
    
    std::vector<VectorAndLabel> screenVectorsAndLabels;
    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            screenVectorsAndLabels.emplace_back("L", -1.0,  0.0, 0.0);
            screenVectorsAndLabels.emplace_back("R",  1.0,  0.0, 0.0);
            screenVectorsAndLabels.emplace_back("T",  0.0,  1.0, 0.0);
            screenVectorsAndLabels.emplace_back("B",  0.0, -1.0, 0.0);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            screenVectorsAndLabels.emplace_back("L", -1.0,  0.0,  0.0);
            screenVectorsAndLabels.emplace_back("R",  1.0,  0.0,  0.0);
            screenVectorsAndLabels.emplace_back("T",  0.0,  0.0,  1.0);
            screenVectorsAndLabels.emplace_back("B",  0.0,  0.0, -1.0);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            screenVectorsAndLabels.emplace_back("L",  0.0,  1.0,  0.0);
            screenVectorsAndLabels.emplace_back("R",  0.0, -1.0,  0.0);
            screenVectorsAndLabels.emplace_back("T",  0.0,  0.0,  1.0);
            screenVectorsAndLabels.emplace_back("B",  0.0,  0.0, -1.0);
            break;
    }
    
    const int32_t numAxes(vectorsAndLabels.size());
    const int32_t numScreenAxes(screenVectorsAndLabels.size());
    
    for (int32_t i = 0; i < numScreenAxes; i++) {
        CaretAssertVectorIndex(screenVectorsAndLabels, i);
        for (int32_t j = 0; j < numAxes; j++) {
            CaretAssertVectorIndex(vectorsAndLabels, j);
            
            Vector3D rotatedAxisVector(vectorsAndLabels[j].m_vector);
            m_sliceRotationMatrix.multiplyPoint3(rotatedAxisVector);
            
            const float dotValue(screenVectorsAndLabels[i].m_vector.dot(rotatedAxisVector));
            
            const AString screenTemp(screenVectorsAndLabels[i].m_label);
            AString labelTemp;
            if (dotValue > 0.95) {
                const AString& label(vectorsAndLabels[j].m_label);
                
                labelTemp = label;
                
                if (screenVectorsAndLabels[i].m_label == "L") {
                    leftScreenLabelTextOut = label;
                }
                else if (screenVectorsAndLabels[i].m_label == "R") {
                    rightScreenLabelTextOut = label;
                }
                else if (screenVectorsAndLabels[i].m_label == "T") {
                    topScreenLabelTextOut = label;
                }
                else if (screenVectorsAndLabels[i].m_label == "B") {
                    bottomScreenLabelTextOut = label;
                }
                else {
                    CaretAssert(0);
                }
            }
        }
    }
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
VolumeMprVirtualSliceView::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeMprVirtualSliceView",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
VolumeMprVirtualSliceView::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

