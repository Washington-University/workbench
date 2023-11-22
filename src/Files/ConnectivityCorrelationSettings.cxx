
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

#define __CONNECTIVITY_CORRELATION_SETTINGS_DECLARE__
#include "ConnectivityCorrelationSettings.h"
#undef __CONNECTIVITY_CORRELATION_SETTINGS_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ConnectivityCorrelationSettings 
 * \brief Settings for ConnectivityCorrelation
 * \ingroup Files
 */

/**
 * Constructor.
 * @param mode
 *    The mode
 * @param enableCorrelationFisherZ
 *    In correltation mode, fisher - Z
 * @param enableCorrelationNoDemean
 *    In correlation mode, no-demean
 */
ConnectivityCorrelationSettings::ConnectivityCorrelationSettings()
: CaretObject()
{
    m_mode = ConnectivityCorrelationModeEnum::CORRELATION;
    m_correlationFisherZEnabled  = false;
    m_correlationNoDemeanEnabled = false;
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add<ConnectivityCorrelationModeEnum, ConnectivityCorrelationModeEnum::Enum>("m_mode",
                                                                                                  &m_mode);
    m_sceneAssistant->add("m_correlationFisherZEnabled",
                          &m_correlationFisherZEnabled);
    m_sceneAssistant->add("m_correlationNoDemeanEnabled",
                          &m_correlationNoDemeanEnabled);
}

/**
 * Destructor.
 */
ConnectivityCorrelationSettings::~ConnectivityCorrelationSettings()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ConnectivityCorrelationSettings::ConnectivityCorrelationSettings(const ConnectivityCorrelationSettings& obj)
: CaretObject(obj)
{
    this->copyHelperConnectivityCorrelationSettings(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ConnectivityCorrelationSettings&
ConnectivityCorrelationSettings::operator=(const ConnectivityCorrelationSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperConnectivityCorrelationSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ConnectivityCorrelationSettings::copyHelperConnectivityCorrelationSettings(const ConnectivityCorrelationSettings& obj)
{
    m_mode                       = obj.m_mode;
    m_correlationFisherZEnabled  = obj.m_correlationFisherZEnabled;
    m_correlationNoDemeanEnabled = obj.m_correlationNoDemeanEnabled;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
ConnectivityCorrelationSettings::operator==(const ConnectivityCorrelationSettings& obj) const
{
    if (this == &obj) {
        return true;    
    }

    if (m_mode                       != obj.m_mode) return false;
    if (m_correlationFisherZEnabled  != obj.m_correlationFisherZEnabled) return false;
    if (m_correlationNoDemeanEnabled != obj.m_correlationNoDemeanEnabled) return false;

    return true;
}

/**
 * Inequality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
ConnectivityCorrelationSettings::operator!=(const ConnectivityCorrelationSettings& obj) const
{
    return ( ! (*this == obj));
}


/**
 * @return The mode
 */
ConnectivityCorrelationModeEnum::Enum
ConnectivityCorrelationSettings::getMode() const
{
    return m_mode;
}

/**
 * Set
 * @param
 */
void
ConnectivityCorrelationSettings::setMode(const ConnectivityCorrelationModeEnum::Enum mode)
{
    m_mode = mode;
}

/**
 * @return Is the Fisher-Z option enabled for correlation
 */
bool
ConnectivityCorrelationSettings::isCorrelationFisherZEnabled() const
{
    return m_correlationFisherZEnabled;
}

/**
 * Set the Fisher-Z option enabled for correlation
 * @param enabled
 *    New status
 */
void
ConnectivityCorrelationSettings::setCorrelationFisherZEnabled(const bool enabled)
{
    m_correlationFisherZEnabled = enabled;
}

/**
 * @return Is the no demean option enabled for correlation
 */
bool
ConnectivityCorrelationSettings::isCorrelationNoDemeanEnabled() const
{
    return m_correlationNoDemeanEnabled;
}

/**
 * Set the No Demean option enabled for correlation
 * @param enabled
 *    New status
 */
void
ConnectivityCorrelationSettings::setCorrelationNoDemeanEnabled(const bool enabled)
{
    m_correlationNoDemeanEnabled = enabled;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ConnectivityCorrelationSettings::toString() const
{
    return "ConnectivityCorrelationSettings";
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
ConnectivityCorrelationSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ConnectivityCorrelationSettings",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
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
ConnectivityCorrelationSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

