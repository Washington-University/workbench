
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

#define __MPR_VIRTUAL_SLICE_VIEW_DECLARE__
#include "MprVirtualSliceView.h"
#undef __MPR_VIRTUAL_SLICE_VIEW_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumePlaneIntersection.h"
#include "VolumeTextureCoordinateMapper.h"

using namespace caret;


    
/**
 * \class caret::MprVirtualSliceView
 * \brief Camera for MPR  volume slice viewing
 * \ingroup Brain
 */

MprVirtualSliceView::ViewType
MprVirtualSliceView::getViewType()
{
    return s_viewType;
}


/**
 * Constructor.
 */
MprVirtualSliceView::MprVirtualSliceView()
: CaretObject()
{
}

/**
 * Constructor.
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
 * @param rotationMatrix
 *    The current rotation matrix
 */
MprVirtualSliceView::MprVirtualSliceView(const Vector3D& volumeCenterXYZ,
                                         const Vector3D& selectedSlicesXYZ,
                                         const float sliceWidthHeight,
                                         const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                         const VolumeMprOrientationModeEnum::Enum& mprOrientationMode,
                                         const Matrix4x4& rotationMatrix)
: CaretObject(),
m_volumeCenterXYZ(volumeCenterXYZ),
m_selectedSlicesXYZ(selectedSlicesXYZ),
m_sliceWidthHeight(sliceWidthHeight),
m_sliceViewPlane(sliceViewPlane),
m_mprOrientationMode(mprOrientationMode),
m_rotationMatrix(rotationMatrix)
{
    const float cameraOffsetDistance(sliceWidthHeight * 2.0);
    
    m_cameraXYZ.set(0.0, 0.0, 0.0);
    m_cameraUpVector.set(0.0, 0.0, 0.0);
    
    m_planeUpVector.set(0.0, 0.0, 0.0);
    m_planeRightVector.set(0.0, 0.0, 0.0);
    
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
    
    switch (m_sliceViewPlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            m_cameraXYZ[2] = cameraOffsetDistance;
            m_cameraUpVector[1] = 1.0;
            
            m_planeRightVector[0] = 1.0;
            if (m_radiologicalOrientationFlag) {
                m_planeRightVector[0] = -1.0;
            }
            m_planeUpVector[1] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            m_cameraXYZ[1] = -cameraOffsetDistance;
            m_cameraUpVector[2] = 1.0;
            
            m_planeRightVector[0] = 1.0;
            if (m_radiologicalOrientationFlag) {
                m_planeRightVector[0] = -1.0;
            }
            m_planeUpVector[2] = 1.0;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            m_cameraXYZ[0] = -cameraOffsetDistance;
            m_cameraUpVector[2] = 1.0;
            
            m_planeRightVector[1] = -1.0;
            m_planeUpVector[2] = 1.0;
            break;
    }
    
    switch (s_viewType) {
        case ROTATE_CAMERA:
            m_rotationMatrix.multiplyPoint3(m_cameraXYZ);
            m_rotationMatrix.multiplyPoint3(m_cameraUpVector);
            m_rotationMatrix.multiplyPoint3(m_planeRightVector);
            m_rotationMatrix.multiplyPoint3(m_planeUpVector);
            break;
        case ROTATE_VOLUME:
        {
            
            Vector3D offset(selectedSlicesXYZ);
            
            Matrix4x4 m1;
            m1.translate(-offset);
            
            Matrix4x4 m2(m_rotationMatrix);
            
            Matrix4x4 m3;
            m3.translate(offset);
            
            m_transformationMatrix = m1;
            m_transformationMatrix.postmultiply(m2);
            m_transformationMatrix.postmultiply(m3);
        }
            break;
    }
    
    Vector3D virtualSlicePlaneVector(m_planeRightVector.cross(m_planeUpVector));
    
    /*
     * Virtual slice plane is vector from virtual slice
     * that points to camera and contains selected slices XYZ
     */
    m_virtualSlicePlane = Plane(virtualSlicePlaneVector,
                                selectedSlicesXYZ);
    CaretAssert(m_virtualSlicePlane.isValidPlane());
    
    /*
     * "Look at" is volume center projected to the virtual slice
     */
    m_virtualSlicePlane.projectPointToPlane(volumeCenterXYZ,
                                            m_cameraLookAtXYZ);
    
    /*
     * Does increasing slice coordinate direction face to the
     * user or away from the user
     */
    bool sameDirectionFlag(false);
    switch (sliceViewPlane) {
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
    
    if (m_radiologicalOrientationFlag) {
        /*
         * Need to flip sign of normal vector
         */
        m_montageVirutalSliceIncreasingDirectionPlane = Plane(-virtualSlicePlaneVector,
                                                              selectedSlicesXYZ);
    }
    else {
        m_montageVirutalSliceIncreasingDirectionPlane = Plane(virtualSlicePlaneVector,
                                                              selectedSlicesXYZ);
    }
    
    /*
     * Camera is offset from the 'look at' on the virtual slice
     */
    m_cameraXYZ = (m_cameraLookAtXYZ
                   + virtualSlicePlaneVector * cameraOffsetDistance);
    //m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
}

/**
 * Destructor.
 */
MprVirtualSliceView::~MprVirtualSliceView()
{
}

/**
 * Get coordinates for drawing the volume primitive with a triangle fan
 * @param volume
 *    Volume that will be drawn and has coordinates generated
 * @param stereotaxicXyzOut
 *    Actual stereotaxic coordinates of volume intersection
 * @param primtiveVertexXyzOut
 *    Coordinates for drawing the virtual slice
 * @param primitiveTextureStrOut
 *    Texture coordinates for drawing the virtual slice
 */
bool
MprVirtualSliceView::getTriangleFanCoordinates(const VolumeMappableInterface* volume,
                                        std::vector<Vector3D>& stereotaxicXyzOut,
                                        std::vector<Vector3D>& primtiveVertexXyzOut,
                                        std::vector<Vector3D>& primitiveTextureStrOut) const
{
    CaretAssert(volume);
    stereotaxicXyzOut.clear();
    primtiveVertexXyzOut.clear();
    primitiveTextureStrOut.clear();
    
    
    primtiveVertexXyzOut = createVirtualSliceTriangleFan(volume);
    stereotaxicXyzOut    = mapBackToStereotaxicCoordinates(primtiveVertexXyzOut);
    primitiveTextureStrOut = mapTextureCoordinates(volume,
                                                   stereotaxicXyzOut);
    CaretAssert(primtiveVertexXyzOut.size() == primitiveTextureStrOut.size());
    
    return (primtiveVertexXyzOut.size() >= 3);
}

/**
 * Get coordinates for drawing the volume primitive with triangles
 * @param volume
 *    Volume that will be drawn and has coordinates generated
 * @param stereotaxicXyzOut
 *    Actual stereotaxic coordinates of volume intersection
 * @param primtiveVertexXyzOut
 *    Coordinates for drawing the virtual slice
 * @param primitiveTextureStrOut
 *    Texture coordinates for drawing the virtual slice
 */
bool
MprVirtualSliceView::getTrianglesCoordinates(const VolumeMappableInterface* volume,
                                      std::vector<Vector3D>& stereotaxicXyzOut,
                                      std::vector<Vector3D>& primtiveVertexXyzOut,
                                      std::vector<Vector3D>& primitiveTextureStrOut) const
{
    CaretAssert(volume);
    stereotaxicXyzOut.clear();
    primtiveVertexXyzOut.clear();
    primitiveTextureStrOut.clear();
    
    
    primtiveVertexXyzOut = createVirtualSliceTriangles(volume);
    stereotaxicXyzOut    = mapBackToStereotaxicCoordinates(primtiveVertexXyzOut);
    primitiveTextureStrOut = mapTextureCoordinates(volume,
                                                   stereotaxicXyzOut);
    CaretAssert(primtiveVertexXyzOut.size() == primitiveTextureStrOut.size());
    
    if (m_sliceViewPlane == VolumeSliceViewPlaneEnum::AXIAL) {
        std::cout << this->toString() << std::endl;
    }
    return (primtiveVertexXyzOut.size() >= 3);
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
MprVirtualSliceView::mapTextureCoordinates(const VolumeMappableInterface* volume,
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
 * Map coordinates from intersection viewing back to stereotaxic coordinates
 * @param intersectionXyz
 *    The intersection coordinates
 * @return
 *    Stereotaxic coordinates
 */
std::vector<Vector3D>
MprVirtualSliceView::mapBackToStereotaxicCoordinates(const std::vector<Vector3D>& intersectionXyz) const
{
    std::vector<Vector3D> stereotaxicCoordinatesOut;
    
    Matrix4x4 m(m_transformationMatrix);
    const bool matrixValidFlag(m.invert());
    
    for (auto& volXYZ : intersectionXyz) {
        Vector3D xyz(volXYZ);
        switch (s_viewType) {
            case ROTATE_CAMERA:
                break;
            case ROTATE_VOLUME:
                /*
                 * The XYZ was transformed to we need to
                 * inverse transform the XYZ to so that it is
                 * in the original volume coordinates so that
                 * the XYZ can be mapped to a texture coordinate
                 */
                if (matrixValidFlag) {
                    m.multiplyPoint3(xyz);
                }
                break;
        }
        stereotaxicCoordinatesOut.push_back(xyz);
    }
    
    CaretAssert(intersectionXyz.size() == stereotaxicCoordinatesOut.size());
    return stereotaxicCoordinatesOut;
}

/**
 * @param volume
 *   The volume
 * @return
 *   A triangle fan for drawing the virtual volume slice that intersects with the volume
 */
std::vector<Vector3D>
MprVirtualSliceView::createVirtualSliceTriangleFan(const VolumeMappableInterface* volume) const
{
    const Vector3D cameraLookAtVector((m_cameraLookAtXYZ - m_cameraXYZ).normal());

    const Plane plane(cameraLookAtVector,
                      m_selectedSlicesXYZ);
    
    std::vector<Vector3D> triangleFanVertices;

    Matrix4x4 rotationMatrix;
    switch (s_viewType) {
        case ROTATE_CAMERA:
            break;
        case ROTATE_VOLUME:
            rotationMatrix = m_rotationMatrix;
            rotationMatrix = m_transformationMatrix;
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
        if (numPoints > 6) {
            numPoints = 6;
            CaretAssertMessage(0, "There were more than 6 intersections.  This should never happen");
        }
        
        Vector3D firstPoint;
        if (numPoints > 0) {
            CaretAssertVectorIndex(intersectionPoints, 0);
            firstPoint = intersectionPoints[0];
        }
        
        triangleFanVertices.push_back(intersectionCenterXYZ);
        
        /*
         * Add the intersections
         */
        for (int32_t i = 0; i < numPoints; i++) {
            CaretAssertVectorIndex(intersectionPoints, i);
            triangleFanVertices.push_back(intersectionPoints[i]);
        }
        
        /*
         * Need to add first intersection to create triangle with last intersection
         * since we draw with a triangle fan.
         */
        if (numPoints > 0) {
            CaretAssertVectorIndex(intersectionPoints, 0);
            triangleFanVertices.push_back(intersectionPoints[0]);
        }
        
        /*
         * The maximum number of intersections is 6.
         * We also add the first vertex one more time to
         * create the last triagle so that makes 7.
         * The center is the first fan vertex so that makes 8.
         * When less than 8 fan vertices, add the first vertex
         * to fill in the rest so that there are always
         * 8 fan vertices (the primitive for drawing always
         * uses 8 vertices).
         * Adding the first vertex additional times will create
         * degenerate triangles that OpenGL will cull.
         */
        const int32_t numFanVertices(triangleFanVertices.size());
        for (int32_t i = numFanVertices; i < 8; i++) {
            triangleFanVertices.push_back(firstPoint);
        }
        CaretAssert(triangleFanVertices.size() == 8);
    }
    
    return triangleFanVertices;
}

/**
 * @param volume
 *   The volume
 * @return
 *   Triangle vertices for drawing the virtual volume slice that intersects with the volume
 *   One triangle for each three vertices
 */
std::vector<Vector3D>
MprVirtualSliceView::createVirtualSliceTriangles(const VolumeMappableInterface* volume) const
{
    Vector3D cameraLookAtVector;
    switch (s_viewType) {
        case ROTATE_CAMERA:
            cameraLookAtVector = (m_cameraXYZ - m_cameraLookAtXYZ).normal();
            break;
        case ROTATE_VOLUME:
            cameraLookAtVector = (m_cameraLookAtXYZ - m_cameraXYZ).normal();
            break;
    }

    const Plane plane(cameraLookAtVector,
                      m_selectedSlicesXYZ);
        
    std::vector<Vector3D> trianglesVertices;
    
    Matrix4x4 rotationMatrix;
    switch (s_viewType) {
        case ROTATE_CAMERA:
            break;
        case ROTATE_VOLUME:
            rotationMatrix = m_rotationMatrix;
            rotationMatrix = m_transformationMatrix;
            break;
    }

    const int32_t requiredNumberOfTriangles(6);
    const int32_t requiredNumberOfVertices(requiredNumberOfTriangles * 3);

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
MprVirtualSliceView::MprVirtualSliceView(const MprVirtualSliceView& obj)
: CaretObject(obj)
{
    this->copyHelperMprVirtualSliceView(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
MprVirtualSliceView&
MprVirtualSliceView::operator=(const MprVirtualSliceView& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj); 
        this->copyHelperMprVirtualSliceView(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
MprVirtualSliceView::copyHelperMprVirtualSliceView(const MprVirtualSliceView& obj)
{
    m_volumeCenterXYZ = obj.m_volumeCenterXYZ;
    
    m_selectedSlicesXYZ = obj.m_selectedSlicesXYZ;
    
    m_sliceWidthHeight = obj.m_sliceWidthHeight;
    
    m_sliceViewPlane = obj.m_sliceViewPlane;
    
    m_mprOrientationMode = obj.m_mprOrientationMode;
    
    m_rotationMatrix = obj.m_rotationMatrix;
    
    m_transformationMatrix = obj.m_transformationMatrix;
    
    m_cameraXYZ = obj.m_cameraXYZ;
    
    m_cameraLookAtXYZ = obj.m_cameraLookAtXYZ;
    
    m_cameraUpVector = obj.m_cameraUpVector;
    
    m_planeRightVector = obj.m_planeRightVector;
    
    m_planeUpVector = obj.m_planeUpVector;
    
    m_virtualSlicePlane = obj.m_virtualSlicePlane;
    
    m_montageVirutalSliceIncreasingDirectionPlane = obj.m_montageVirutalSliceIncreasingDirectionPlane;
    
    m_neurologicalOrientationFlag = obj.m_neurologicalOrientationFlag;
    
    m_radiologicalOrientationFlag = obj.m_radiologicalOrientationFlag;
}

/**
 * @return The coordinate of the camera
 */
Vector3D
MprVirtualSliceView::getCameraXYZ() const
{
    return m_cameraXYZ;
}

/**
 * @return The coordinate of what the camera is looking at
 */
Vector3D
MprVirtualSliceView::getCameraLookAtXYZ() const
{
    return m_cameraLookAtXYZ;
}

/**
 * @return The up vector of the camera
 */
Vector3D
MprVirtualSliceView::getCameraUpVector() const
{
    return m_cameraUpVector;
}

/**
 * @return Plane for the virtual slice
 */
Plane
MprVirtualSliceView::getPlane() const
{
    return m_virtualSlicePlane;
}

/**
 * @return Plane for using when increasing slices coordinates
 * in montage view
 */
Plane
MprVirtualSliceView::getMontageIncreasingDirectionPlane() const
{
    return m_montageVirutalSliceIncreasingDirectionPlane;
}

/**
 * @return Normal vector of virtual slice
 */
Vector3D
MprVirtualSliceView::getNormalVector() const
{
    Vector3D n;
    m_virtualSlicePlane.getNormalVector(n);
    return n;
}

/**
 * @return Center of volume
 */
Vector3D
MprVirtualSliceView::getVolumeCenterXYZ() const
{
    return m_volumeCenterXYZ;;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
MprVirtualSliceView::toString() const
{
    AString txt;
    
    txt.appendWithNewLine("m_sliceViewPlane=" + VolumeSliceViewPlaneEnum::toName(m_sliceViewPlane));
    txt.appendWithNewLine("m_cameraXYZ=" + m_cameraXYZ.toString());
    txt.appendWithNewLine("m_cameraLookAtXYZ=" + m_cameraLookAtXYZ.toString());
    txt.appendWithNewLine("m_cameraUpVector=" + m_cameraUpVector.toString());

    return txt;
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
MprVirtualSliceView::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "MprVirtualSliceView",
                                            1);
//    m_sceneAssistant->saveMembers(sceneAttributes,
//                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
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
MprVirtualSliceView::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
//    m_sceneAssistant->restoreMembers(sceneAttributes,
//                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

