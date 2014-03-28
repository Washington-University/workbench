
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
    m_sceneAssistant->addArray("m_selectionStatus", m_selectionStatus, 3, false);
    
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
: CaretObject(obj)
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
        m_selectionStatus[i] = obj.m_selectionStatus[i];
    }
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            m_rotation[i][j] = obj.m_rotation[i][j];
        }
    }
}

/***
 * Set the rotation matrix to identity.
 */
void
ClippingPlaneGroup::setRotationToIdentity()
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            if (i == j) {
                m_rotation[i][j] = 1.0;
            }
            else {
                m_rotation[i][j] = 0.0;
            }
        }
    }
}

/**
 * Reset member values.
 */
void
ClippingPlaneGroup::resetToDefaultValues()
{
    for (int32_t i = 0; i < 3; i++) {
        m_translation[i]     = 0.0;
        m_thickness[i]       = 20.0;
        m_selectionStatus[i] = false;
    }
    
    setRotationToIdentity();
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
 * Get the rotation values
 *
 * @param rotation
 *    The rotation values.
 */
void
ClippingPlaneGroup::getRotation(float rotation[4][4]) const
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            rotation[i][j] = m_rotation[i][j];
        }
    }
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
 * Get the selection status values
 *
 * @param selectionStatus
 *    The selection status values.
 */
void
ClippingPlaneGroup::getSelectionStatus(bool selectionStatus[3]) const
{
    selectionStatus[0] = m_selectionStatus[0];
    selectionStatus[1] = m_selectionStatus[1];
    selectionStatus[2] = m_selectionStatus[2];
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
 * Set the rotation values.
 *
 * @param rotation
 *    The rotation values.
 */
void
ClippingPlaneGroup::setRotation(const float rotation[4][4])
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            m_rotation[i][j] = rotation[i][j];
        }
    }
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
 * Set the selection status values.
 *
 * @param selectionStatus
 *    The selection status values.
 */
void
ClippingPlaneGroup::setSelectionStatus(const bool selectionStatus[3])
{
    m_selectionStatus[0] = selectionStatus[0];
    m_selectionStatus[1] = selectionStatus[1];
    m_selectionStatus[2] = selectionStatus[2];
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
    
    float* rotationPointer = &m_rotation[0][0];
    sceneClass->addFloatArray("m_rotation", rotationPointer, 16);
    
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
    
    float* rotationPointer = &m_rotation[0][0];
    const int32_t numElem = sceneClass->getFloatArrayValue("m_rotation",
                                                           rotationPointer,
                                                           16);
    if (numElem != 16) {
        setRotationToIdentity();
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

