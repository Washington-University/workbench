
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
    resetToDefaultValues();
    
    m_sceneAssistant = new SceneClassAssistant();
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
    
    m_rotationMatrix = obj.m_rotationMatrix;
    
    m_xAxisSelectionStatus = obj.m_xAxisSelectionStatus;
    m_yAxisSelectionStatus = obj.m_yAxisSelectionStatus;
    m_zAxisSelectionStatus = obj.m_zAxisSelectionStatus;
    
    m_surfaceSelectionStatus  = obj.m_surfaceSelectionStatus;
    m_volumeSelectionStatus   = obj.m_volumeSelectionStatus;
    m_featuresSelectionStatus = obj.m_featuresSelectionStatus;
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
}

/**
 * Create the plane equation for the given plane identifier.
 *
 * @param planeIdentifier
 *    Identifies plane that is created.
 */
Plane
ClippingPlaneGroup::createClippingPlane(const PlaneIdentifier planeIdentifier) const
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
            pointOnPlane[0] = m_translation[0] - (m_thickness[0] / 2.0);
            break;
        case PLANE_MAXIMUM_X:
            /*
             * X Maximum
             */
            normalVector[0] = -1.0;
            pointOnPlane[0] = m_translation[0] + (m_thickness[0] / 2.0);
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
    
    m_rotationMatrix.multiplyPoint3(normalVector);
    m_rotationMatrix.multiplyPoint3(pointOnPlane);
    
    Plane plane(normalVector,
                pointOnPlane);
    return plane;
}

/**
 * @return Planes representing the active clipping planes for the given
 * structure.  
 *
 * @param structure
 *     The structure.  If the structure is a "right side" structure, the
 *     rotations are mirror flipped.
 */
std::vector<Plane>
ClippingPlaneGroup::getActiveClippingPlanesForStructure(const StructureEnum::Enum structure) const
{
    std::vector<Plane> planes;
    
    if (m_xAxisSelectionStatus) {
        planes.push_back(createClippingPlane(PLANE_MINIMUM_X));
        planes.push_back(createClippingPlane(PLANE_MAXIMUM_X));
    }
    
    if (m_yAxisSelectionStatus) {
        planes.push_back(createClippingPlane(PLANE_MINIMUM_Y));
        planes.push_back(createClippingPlane(PLANE_MAXIMUM_Y));
    }
    
    if (m_zAxisSelectionStatus) {
        planes.push_back(createClippingPlane(PLANE_MINIMUM_Z));
        planes.push_back(createClippingPlane(PLANE_MAXIMUM_Z));
    }
    
    return planes;
}

/**
 * @return The rotation matrix.
 */
Matrix4x4
ClippingPlaneGroup::getRotationMatrix() const
{
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
                                            1);
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
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

