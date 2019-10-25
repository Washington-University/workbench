
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

#define __CLIPPING_PLANE_GROUP_DECLARE__
#include "ClippingPlaneGroup.h"
#undef __CLIPPING_PLANE_GROUP_DECLARE__

#include "CaretAssert.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ClippingPlaneGroup 
 * \brief A group of clipping plane for clipping to a rectangular region
 * \ingroup Brain
 */

/**
 * Constructor.
 */
ClippingPlaneGroup::ClippingPlaneGroup()
: CaretObject()
{
    invalidateActiveClippingPlainEquations();
    
    resetToDefaultValues();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_enabledStatus", &m_enabledStatus);
    m_sceneAssistant->add("m_displayClippingBoxStatus", &m_displayClippingBoxStatus);
    m_sceneAssistant->addArray("m_translation", m_translation, 3, 0.0);
    m_sceneAssistant->addArray("m_thickness", m_thickness, 3, 20.0);
    m_sceneAssistant->add("m_xAxisSelectionStatus", &m_xAxisSelectionStatus);
    m_sceneAssistant->add("m_yAxisSelectionStatus", &m_yAxisSelectionStatus);
    m_sceneAssistant->add("m_zAxisSelectionStatus", &m_zAxisSelectionStatus);
    m_sceneAssistant->add("m_surfaceSelectionStatus", &m_surfaceSelectionStatus);
    m_sceneAssistant->add("m_volumeSelectionStatus", &m_volumeSelectionStatus);
    m_sceneAssistant->add("m_featuresSelectionStatus", &m_featuresSelectionStatus);
}

/**
 * Destructor.
 */
ClippingPlaneGroup::~ClippingPlaneGroup()
{
    delete m_sceneAssistant;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ClippingPlaneGroup::ClippingPlaneGroup(const ClippingPlaneGroup& obj)
: CaretObject(obj),
SceneableInterface()
{
    this->copyHelperClippingPlaneGroup(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ClippingPlaneGroup&
ClippingPlaneGroup::operator=(const ClippingPlaneGroup& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperClippingPlaneGroup(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ClippingPlaneGroup::copyHelperClippingPlaneGroup(const ClippingPlaneGroup& obj)
{
    for (int32_t i = 0; i < 3; i++) {
        m_translation[i]     = obj.m_translation[i];
        m_thickness[i]       = obj.m_thickness[i];
    }
    
    /* m_enabledStatus NOT copied */
    
    m_rotationMatrix = obj.m_rotationMatrix;
    
    m_xAxisSelectionStatus = obj.m_xAxisSelectionStatus;
    m_yAxisSelectionStatus = obj.m_yAxisSelectionStatus;
    m_zAxisSelectionStatus = obj.m_zAxisSelectionStatus;
    
    m_surfaceSelectionStatus  = obj.m_surfaceSelectionStatus;
    m_volumeSelectionStatus   = obj.m_volumeSelectionStatus;
    m_featuresSelectionStatus = obj.m_featuresSelectionStatus;

    m_displayClippingBoxStatus = obj.m_displayClippingBoxStatus;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * @return Enabled status
 */
bool
ClippingPlaneGroup::isEnabled() const
{
    return m_enabledStatus;
}

/**
 * Set clipping planes enabled
 * @param status
 *   New enabled status
 */
void
ClippingPlaneGroup::setEnabled(const bool status)
{
    m_enabledStatus = status;
}

/**
 * Reset the transformation.
 */
void
ClippingPlaneGroup::resetTransformation()
{
    for (int32_t i = 0; i < 3; i++) {
        m_translation[i] = 0.0;
    }

    m_thickness[0] = 180.0;
    m_thickness[1] = 250.0;
    m_thickness[2] = 220.0;
    
    m_rotationMatrix.identity();

    invalidateActiveClippingPlainEquations();
}

/**
 * Reset member values.
 */
void
ClippingPlaneGroup::resetToDefaultValues()
{
    resetTransformation();
    
    m_xAxisSelectionStatus = false;
    m_yAxisSelectionStatus = false;
    m_zAxisSelectionStatus = false;
    
    m_surfaceSelectionStatus  = true;
    m_volumeSelectionStatus   = true;
    m_featuresSelectionStatus = true;

    invalidateActiveClippingPlainEquations();
}

/**
 * @return The X-coordinate for the structure.
 *
 * @param structure
 *     The structure.  Note that right and left hemispheres are mirror flipped.
 */
float
ClippingPlaneGroup::getXCoordinateForStructure(const StructureEnum::Enum structure) const
{
    float x = m_translation[0];
    if (StructureEnum::isLeft(structure)) {
        x = -x;
    }
 
    return x;
}

/**
 * Create the plane equation for the given plane identifier.
 *
 * @param planeIdentifier
 *    Identifies plane that is created.
 * @param structure
 *     The structure.  Note that right and left hemispheres are mirror flipped.
 */
Plane*
ClippingPlaneGroup::createClippingPlane(const PlaneIdentifier planeIdentifier,
                                        const StructureEnum::Enum structure) const
{
    float normalVector[3] = { 0.0, 0.0, 0.0 };
    float pointOnPlane[3] = { 0.0, 0.0, 0.0 };
    
    /*
     * Note: the planes form a rectangular cuboid and we want to
     * clip what is OUTSIDE this rectangular cuboid.
     */
    switch (planeIdentifier) {
        case PLANE_MINIMUM_X:
            /*
             * X Minimum
             */
            normalVector[0] = 1.0;
            pointOnPlane[0] = getXCoordinateForStructure(structure) - (m_thickness[0] / 2.0);
            break;
        case PLANE_MAXIMUM_X:
            /*
             * X Maximum
             */
            normalVector[0] = -1.0;
            pointOnPlane[0] = getXCoordinateForStructure(structure) + (m_thickness[0] / 2.0);
            break;
        case PLANE_MINIMUM_Y:
            /*
             * Y Minimum
             */
            normalVector[1] = 1.0;
            pointOnPlane[1] = m_translation[1] - (m_thickness[1] / 2.0);
            break;
        case PLANE_MAXIMUM_Y:
            /*
             * Y Maximum
             */
            normalVector[1] = -1.0;
            pointOnPlane[1] = m_translation[1] + (m_thickness[1] / 2.0);
            break;
        case PLANE_MINIMUM_Z:
            /*
             * Z Minimum
             */
            normalVector[2] = 1.0;
            pointOnPlane[2] = m_translation[2] - (m_thickness[2] / 2.0);
            break;
        case PLANE_MAXIMUM_Z:
            /*
             * Z Maximum
             */
            normalVector[2] = -1.0;
            pointOnPlane[2] = m_translation[2] + (m_thickness[2] / 2.0);
            break;
        default:
            CaretAssert(0);
    }
    
    Matrix4x4 rotMat = getRotationMatrixForStructure(structure);
    rotMat.multiplyPoint3(normalVector);
    rotMat.multiplyPoint3(pointOnPlane);
    
    Plane* plane = new Plane(normalVector,
                             pointOnPlane);
    return plane;
}

/**
 * @return Planes representing the active clipping planes for the given
 * structure.  
 *
 * @param structure
 *     The structure.  Note that right and left hemispheres are mirror flipped.
 */
std::vector<const Plane*>
ClippingPlaneGroup::getActiveClippingPlanesForStructure(const StructureEnum::Enum structure) const
{
    updateActiveClippingPlainEquations();
    
    std::vector<const Plane*> planes;
    
    if (StructureEnum::isRight(structure)) {
        planes.insert(planes.end(),
                      m_rightStructureActiveClippingPlanes.begin(),
                      m_rightStructureActiveClippingPlanes.end());
    }
    else {
        planes.insert(planes.end(),
                      m_activeClippingPlanes.begin(),
                      m_activeClippingPlanes.end());
    }
    
    return planes;
}

/**
 * @return The rotation matrix.
 *
 * @param structure
 *    Structure for the rotation matrix.  A 'right' structure has rotations
 *    'mirror flipped'.
 */
Matrix4x4
ClippingPlaneGroup::getRotationMatrixForStructure(const StructureEnum::Enum structure) const
{
    if (StructureEnum::isRight(structure)) {
        double rotationX, rotationY, rotationZ;
        m_rotationMatrix.getRotation(rotationX,
                                     rotationY,
                                     rotationZ);
        
        const int flipMode = 2;
        switch (flipMode) {
            case 1:
                rotationY = 180.0 - rotationY;
                rotationZ = 180.0 - rotationZ;
                break;
            case 2:
                rotationY = -rotationY;
                rotationZ = -rotationZ;
                break;
            case 3:
                rotationY = 180.0 - rotationY;
                rotationZ = -rotationZ;
                break;
            case 4:
                rotationY = - rotationY;
                rotationZ = 180.0 -rotationZ;
                break;
        }
        
        Matrix4x4 mat;
        mat.setRotation(rotationX,
                        rotationY,
                        rotationZ);
        return mat;
        
        
//        const double rotationFlippedY = 180.0 - rotationY;
//        const double rotationFlippedZ = 180.0 - rotationZ;
//        Matrix4x4 mat;
//        mat.setRotation(rotationX, //rotationFlippedX,
//                        rotationFlippedY,
//                        rotationFlippedZ);
//        return mat;
        
    }
    return m_rotationMatrix;
}

/**
 * Replace the rotation matrix.
 *
 * @param rotationMatrix
 *    New rotation matrix.
 */
void
ClippingPlaneGroup::setRotationMatrix(const Matrix4x4& rotationMatrix)
{
    m_rotationMatrix = rotationMatrix;

    invalidateActiveClippingPlainEquations();
}

/**
 * Get the rotation matrix using the given angles.
 *
 * @param rotationAngles
 *    The X, Y, and Z rotation angles.
 */
void
ClippingPlaneGroup::getRotationAngles(float rotationAngles[3]) const
{
    double x, y, z;
    m_rotationMatrix.getRotation(x, y, z);
    rotationAngles[0] = x;
    rotationAngles[1] = y;
    rotationAngles[2] = z;
}

/**
 * Set the rotation matrix using the given angles.
 *
 * @param rotationAngles
 *    The X, Y, and Z rotation angles.
 */
void
ClippingPlaneGroup::setRotationAngles(const float rotationAngles[3])
{
    m_rotationMatrix.setRotation(rotationAngles[0],
                                 rotationAngles[1],
                                 rotationAngles[2]);
    
    invalidateActiveClippingPlainEquations();
}


/**
 * Get the thickness values
 *
 * @param thickness
 *    The thickness values.
 */
void
ClippingPlaneGroup::getThickness(float thickness[3]) const
{
    thickness[0] = m_thickness[0];
    thickness[1] = m_thickness[1];
    thickness[2] = m_thickness[2];
}

/**
 * Set the translation values.
 *
 * @param translation
 *    The translation values.
 */
void
ClippingPlaneGroup::setTranslation(const float translation[3])
{
    m_translation[0] = translation[0];
    m_translation[1] = translation[1];
    m_translation[2] = translation[2];
    
    invalidateActiveClippingPlainEquations();
}

/**
 * Get the translation values
 *
 * @param translation
 *    The translation values.
 */
void
ClippingPlaneGroup::getTranslation(float translation[3]) const
{
    translation[0] = m_translation[0];
    translation[1] = m_translation[1];
    translation[2] = m_translation[2];
}

/**
 * Get the translation values for the given structure.
 *
 * @param structure
 *     The structure.  Note that right and left hemispheres are mirror flipped.
 * @param translation
 *    The translation values.
 */
void
ClippingPlaneGroup::getTranslationForStructure(const StructureEnum::Enum structure,
                                float translation[3]) const
{
    getTranslation(translation);
    translation[0] = getXCoordinateForStructure(structure);
}

/**
 * Set the thickness values.
 *
 * @param thickness
 *    The thickness values.
 */
void
ClippingPlaneGroup::setThickness(const float thickness[3])
{
    m_thickness[0] = thickness[0];
    m_thickness[1] = thickness[1];
    m_thickness[2] = thickness[2];
    
    invalidateActiveClippingPlainEquations();
}

/**
 * @return Is surface selected for clipping?
 */
bool
ClippingPlaneGroup::isSurfaceSelected() const
{
    return m_surfaceSelectionStatus;
}

/**
 * Set surface selected
 *
 * @param selected
 *    New status.
 */
void
ClippingPlaneGroup::setSurfaceSelected(const bool selected)
{
    m_surfaceSelectionStatus = selected;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * @return Is volume selected for clipping?
 */
bool
ClippingPlaneGroup::isVolumeSelected() const
{
    return m_volumeSelectionStatus;
}

/**
 * Set volume selected
 *
 * @param selected
 *    New status.
 */
void
ClippingPlaneGroup::setVolumeSelected(const bool selected)
{
    m_volumeSelectionStatus = selected;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * @return Is features selected for clipping?
 */
bool
ClippingPlaneGroup::isFeaturesSelected() const
{
    return m_featuresSelectionStatus;
}

/**
 * Set display clipping box selected
 *
 * @param selected
 *    New status.
 */
void
ClippingPlaneGroup::setDisplayClippingBoxSelected(const bool selected)
{
    m_displayClippingBoxStatus = selected;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * @return Is display clipping box selected?
 */
bool
ClippingPlaneGroup::isDisplayClippingBoxSelected() const
{
    return m_displayClippingBoxStatus;
}

/**
 * @return Is features and any one or more axes selected for clipping?
 */
bool
ClippingPlaneGroup::isFeaturesAndAnyAxisSelected() const
{
    if (m_enabledStatus) {
        if (m_featuresSelectionStatus) {
            if (m_xAxisSelectionStatus
                || m_yAxisSelectionStatus
                || m_zAxisSelectionStatus) {
                return true;
            }
        }
    }
    
    return false;
}


/**
 * Set features selected
 *
 * @param selected
 *    New status.
 */
void
ClippingPlaneGroup::setFeaturesSelected(const bool selected)
{
    m_featuresSelectionStatus = selected;
}

/**
 * @return Is the X clipping axis selected?
 */
bool
ClippingPlaneGroup::isXAxisSelected() const
{
    return m_xAxisSelectionStatus;
}

/**
 * @return Is the Y clipping axis selected?
 */
bool
ClippingPlaneGroup::isYAxisSelected() const
{
    return m_yAxisSelectionStatus;
}

/**
 * @return Is the Z clipping axis selected?
 */
bool
ClippingPlaneGroup::isZAxisSelected() const
{
    return m_zAxisSelectionStatus;
}

/**
 * Set the selection status for the X-axis.
 *
 * @param xAxisSelected
 *     New selection status for the X-axis.
 */
void
ClippingPlaneGroup::setXAxisSelected(const bool xAxisSelected)
{
    m_xAxisSelectionStatus = xAxisSelected;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * Set the selection status for the Y-axis.
 *
 * @param yAxisSelected
 *     New selection status for the Y-axis.
 */
void
ClippingPlaneGroup::setYAxisSelected(const bool yAxisSelected)
{
    m_yAxisSelectionStatus = yAxisSelected;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * Set the selection status for the Z-axis.
 *
 * @param zAxisSelected
 *     New selection status for the Z-axis.
 */
void
ClippingPlaneGroup::setZAxisSelected(const bool zAxisSelected)
{
    m_zAxisSelectionStatus = zAxisSelected;
    
    invalidateActiveClippingPlainEquations();
}

/**
 * Is the coordinate inside the clipping planes?
 *
 * If a clipping plane for an axis is off, the coordinate is considered
 * to be inside the clipping plane.
 *
 * @param structure
 *     The structure.  Note that right and left hemispheres are mirror flipped.
 * @param xyz
 *     The coordinate.
 *
 * @return
 *     True if inside the clipping planes, else false.
 */
bool
ClippingPlaneGroup::isCoordinateInsideClippingPlanesForStructure(const StructureEnum::Enum structure,
                                                                 const float xyz[3]) const
{
    updateActiveClippingPlainEquations();
    
    if (StructureEnum::isRight(structure)) {
        for (std::vector<Plane*>::iterator iter = m_rightStructureActiveClippingPlanes.begin();
             iter != m_rightStructureActiveClippingPlanes.end();
             iter++) {
            const Plane* plane = *iter;
            if (plane->signedDistanceToPlane(xyz) < 0.0) {
                return false;
            }
        }
        
        return true;
    }
    
    for (std::vector<Plane*>::iterator iter = m_activeClippingPlanes.begin();
         iter != m_activeClippingPlanes.end();
         iter++) {
        const Plane* plane = *iter;
        if (plane->signedDistanceToPlane(xyz) < 0.0) {
            return false;
        }
    }
    
//    USE THE CLIPPING PLANES EQUATIONS !!!!
    
    
//    if (m_xAxisSelectionStatus) {
//        const float x = getXCoordinateForStructure(structure);
//        const float minX = x - (m_thickness[0] / 2.0);
//        const float maxX = x + (m_thickness[0] / 2.0);
//        if (xyz[0] < minX) return false;
//        if (xyz[0] > maxX) return false;
//    }
//    
//    if (m_yAxisSelectionStatus) {
//        const float minY = m_translation[1] - (m_thickness[1] / 2.0);
//        const float maxY = m_translation[1] + (m_thickness[1] / 2.0);
//        if (xyz[1] < minY) return false;
//        if (xyz[1] > maxY) return false;
//    }
//    
//    if (m_zAxisSelectionStatus) {
//        const float minZ = m_translation[2] - (m_thickness[2] / 2.0);
//        const float maxZ = m_translation[2] + (m_thickness[2] / 2.0);
//        if (xyz[2] < minZ) return false;
//        if (xyz[2] > maxZ) return false;
//    }
    
    return true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ClippingPlaneGroup::toString() const
{
    return "ClippingPlaneGroup";
}

/**
 * Update the active clipping planes.
 */
void
ClippingPlaneGroup::updateActiveClippingPlainEquations() const
{
    if (m_activeClippingPlanesValid) {
        return;
    }
    
    if (m_xAxisSelectionStatus) {
        m_activeClippingPlanes.push_back(createClippingPlane(PLANE_MINIMUM_X,
                                                             StructureEnum::CORTEX_LEFT));
        m_activeClippingPlanes.push_back(createClippingPlane(PLANE_MAXIMUM_X,
                                                             StructureEnum::CORTEX_LEFT));

        m_rightStructureActiveClippingPlanes.push_back(createClippingPlane(PLANE_MINIMUM_X,
                                                             StructureEnum::CORTEX_RIGHT));
        m_rightStructureActiveClippingPlanes.push_back(createClippingPlane(PLANE_MAXIMUM_X,
                                                             StructureEnum::CORTEX_RIGHT));
    }
    
    if (m_yAxisSelectionStatus) {
        m_activeClippingPlanes.push_back(createClippingPlane(PLANE_MINIMUM_Y,
                                                             StructureEnum::CORTEX_LEFT));
        m_activeClippingPlanes.push_back(createClippingPlane(PLANE_MAXIMUM_Y,
                                                             StructureEnum::CORTEX_LEFT));

        m_rightStructureActiveClippingPlanes.push_back(createClippingPlane(PLANE_MINIMUM_Y,
                                                             StructureEnum::CORTEX_RIGHT));
        m_rightStructureActiveClippingPlanes.push_back(createClippingPlane(PLANE_MAXIMUM_Y,
                                                             StructureEnum::CORTEX_RIGHT));
    }
    
    if (m_zAxisSelectionStatus) {
        m_activeClippingPlanes.push_back(createClippingPlane(PLANE_MINIMUM_Z,
                                                             StructureEnum::CORTEX_LEFT));
        m_activeClippingPlanes.push_back(createClippingPlane(PLANE_MAXIMUM_Z,
                                                             StructureEnum::CORTEX_LEFT));

        m_rightStructureActiveClippingPlanes.push_back(createClippingPlane(PLANE_MINIMUM_Z,
                                                             StructureEnum::CORTEX_RIGHT));
        m_rightStructureActiveClippingPlanes.push_back(createClippingPlane(PLANE_MAXIMUM_Z,
                                                             StructureEnum::CORTEX_RIGHT));
    }
    
    m_activeClippingPlanesValid = true;
}

/**
 * Invalidate and remove all of the active clipping planes.
 */
void
ClippingPlaneGroup::invalidateActiveClippingPlainEquations()
{
    m_activeClippingPlanesValid = false;
    
    for (std::vector<Plane*>::iterator iter = m_activeClippingPlanes.begin();
         iter != m_activeClippingPlanes.end();
         iter++) {
        delete *iter;
    }
    m_activeClippingPlanes.clear();
    
    for (std::vector<Plane*>::iterator iter = m_rightStructureActiveClippingPlanes.begin();
         iter != m_rightStructureActiveClippingPlanes.end();
         iter++) {
        delete *iter;
    }
    m_rightStructureActiveClippingPlanes.clear();
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
ClippingPlaneGroup::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ClippingPlaneGroup",
                                            2);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    float m[4][4];
    m_rotationMatrix.getMatrix(m);
    sceneClass->addFloatArray("m_rotationMatrix", &m[0][0], 16);
    
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
ClippingPlaneGroup::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    m_enabledStatus = false;
    resetToDefaultValues();
    
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    float m[4][4];
    const int32_t numElem = sceneClass->getFloatArrayValue("m_rotationMatrix",
                                                           &m[0][0],
                                                           16);
    if (numElem == 16) {
        m_rotationMatrix.setMatrix(m);
    }
    else {
        m_rotationMatrix.identity();
    }

    if (sceneClass->getVersionNumber() < 2) {
        /*
         * Enabled status was added in version 2
         */
        m_enabledStatus = (m_xAxisSelectionStatus
                           || m_yAxisSelectionStatus
                           || m_zAxisSelectionStatus);
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

