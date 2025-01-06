
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __WHOLE_BRAIN_SURFACE_SETTINGS_DECLARE__
#include "WholeBrainSurfaceSettings.h"
#undef __WHOLE_BRAIN_SURFACE_SETTINGS_DECLARE__

#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::WholeBrainSurfaceSettings 
 * \brief Surface settings for whole brain.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
WholeBrainSurfaceSettings::WholeBrainSurfaceSettings()
: CaretObject()
{
    m_cerebellumEnabled    = true;
    m_hippocampusEnabled   = true;
    m_hippocampusLeftEnabled = true;
    m_hippocampusRightEnabled = true;
    m_dentateHippocampusLeftEnabled = true;
    m_dentateHippocampusRightEnabled = true;
    m_leftEnabled          = true;
    m_rightEnabled         = true;
    m_leftRightSeparation  = 0.0;
    m_cerebellumSeparation = 0.0;

    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_leftEnabled", &m_leftEnabled);
    m_sceneAssistant->add("m_rightEnabled", &m_rightEnabled);
    m_sceneAssistant->add("m_cerebellumEnabled", &m_cerebellumEnabled);
    m_sceneAssistant->add("m_hippocampusEnabled", &m_hippocampusEnabled);
    m_sceneAssistant->add("m_hippocampusLeftEnabled", &m_hippocampusLeftEnabled);
    m_sceneAssistant->add("m_hippocampusRightEnabled", &m_hippocampusRightEnabled);
    m_sceneAssistant->add("m_dentateHippocampusLeftEnabled", &m_dentateHippocampusLeftEnabled);
    m_sceneAssistant->add("m_dentateHippocampusRightEnabled", &m_dentateHippocampusRightEnabled);
    m_sceneAssistant->add("m_leftRightSeparation", &m_leftRightSeparation);
    m_sceneAssistant->add("m_cerebellumSeparation", &m_cerebellumSeparation);
}

/**
 * Destructor.
 */
WholeBrainSurfaceSettings::~WholeBrainSurfaceSettings()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WholeBrainSurfaceSettings::WholeBrainSurfaceSettings(const WholeBrainSurfaceSettings& obj)
: CaretObject(obj), SceneableInterface(obj)
{
    this->copyHelperWholeBrainSurfaceSettings(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
WholeBrainSurfaceSettings&
WholeBrainSurfaceSettings::operator=(const WholeBrainSurfaceSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperWholeBrainSurfaceSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
WholeBrainSurfaceSettings::copyHelperWholeBrainSurfaceSettings(const WholeBrainSurfaceSettings& obj)
{
    m_cerebellumEnabled    = obj.m_cerebellumEnabled;
    m_hippocampusEnabled   = obj.m_hippocampusEnabled;
    m_hippocampusLeftEnabled = obj.m_hippocampusLeftEnabled;
    m_hippocampusRightEnabled = obj.m_hippocampusRightEnabled;
    m_dentateHippocampusLeftEnabled = obj.m_dentateHippocampusLeftEnabled;
    m_dentateHippocampusRightEnabled = obj.m_dentateHippocampusRightEnabled;
    m_leftEnabled          = obj.m_leftEnabled;
    m_rightEnabled         = obj.m_rightEnabled;
    m_leftRightSeparation  = obj.m_leftRightSeparation;
    m_cerebellumSeparation = obj.m_cerebellumSeparation;

}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WholeBrainSurfaceSettings::toString() const
{
    return "WholeBrainSurfaceSettings";
}

/**
 * @return Enabled status for left cerebral cortex.
 */
bool
WholeBrainSurfaceSettings::isLeftEnabled() const
{
    return m_leftEnabled;
}

/**
 * Set the enabled status for the left hemisphere.
 * @param windowTabNumber
 *    Index of window tab.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setLeftEnabled(const bool enabled)
{
    m_leftEnabled = enabled;
}

/**
 * @return Enabled status for right cerebral cortex.
 */
bool
WholeBrainSurfaceSettings::isRightEnabled() const
{
    return m_rightEnabled;
}

/**
 * Set the enabled status for the right hemisphere.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setRightEnabled(const bool enabled)
{
    m_rightEnabled = enabled;
}

/**
 * @return Enabled status for cerebellum.
 */
bool
WholeBrainSurfaceSettings::isCerebellumEnabled() const
{
    return m_cerebellumEnabled;
}

/**
 * Set the enabled status for the cerebellum.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setCerebellumEnabled(const bool enabled)
{
    m_cerebellumEnabled = enabled;
}

/**
 * @return Enabled status for hippocampus.
 */
bool
WholeBrainSurfaceSettings::isHippocampusEnabled() const
{
    return m_hippocampusEnabled;
}

/**
 * Set the enabled status for the hippocampus.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setHippocampusEnabled(const bool enabled)
{
    m_hippocampusEnabled = enabled;
}

/**
 * @return Enabled status for left hippocampus.
 */
bool
WholeBrainSurfaceSettings::isHippocampusLeftEnabled() const
{
    return m_hippocampusLeftEnabled;
}

/**
 * Set the enabled status for the left hippocampus.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setHippocampusLeftEnabled(const bool enabled)
{
    m_hippocampusLeftEnabled = enabled;
}
/**
 * @return Enabled status for right hippocampus.
 */
bool
WholeBrainSurfaceSettings::isHippocampusRightEnabled() const
{
    return m_hippocampusRightEnabled;
}

/**
 * Set the enabled status for the right hippocampus.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setHippocampusRightEnabled(const bool enabled)
{
    m_hippocampusRightEnabled = enabled;
}
/**
 * @return Enabled status for dentate left hippocampus.
 */
bool
WholeBrainSurfaceSettings::isDentateHippocampusLeftEnabled() const
{
    return m_dentateHippocampusLeftEnabled;
}

/**
 * Set the enabled status for the dentate left hippocampus.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setDentateHippocampusLeftEnabled(const bool enabled)
{
    m_dentateHippocampusLeftEnabled = enabled;
}
/**
 * @return Enabled status for dentate right hippocampus.
 */
bool
WholeBrainSurfaceSettings::isDentateHippocampusRightEnabled() const
{
    return m_dentateHippocampusRightEnabled;
}

/**
 * Set the enabled status for the hippocampus.
 * @param enabled
 *    New enabled status.
 */
void
WholeBrainSurfaceSettings::setDentateHippocampusRightEnabled(const bool enabled)
{
    m_dentateHippocampusRightEnabled = enabled;
}

/**
 * @return The separation between the left and right surfaces.
 */
float
WholeBrainSurfaceSettings::getLeftRightSeparation() const
{
    return m_leftRightSeparation;
}

/**
 * Set the separation between the cerebellum and the left/right surfaces.
 * @param separation
 *     New value for separation.
 */
void
WholeBrainSurfaceSettings::setLeftRightSeparation(const float separation)
{
    m_leftRightSeparation = separation;
}

/**
 * @return The separation between the left/right surfaces.
 */
float
WholeBrainSurfaceSettings::getCerebellumSeparation() const
{
    return m_cerebellumSeparation;
}

/**
 * Set the separation between the cerebellum and the eft and right surfaces.
 * @param separation
 *     New value for separation.
 */
void
WholeBrainSurfaceSettings::setCerebellumSeparation(const float separation)
{
    m_cerebellumSeparation = separation;
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
WholeBrainSurfaceSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "WholeBrainSurfaceSettings",
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
WholeBrainSurfaceSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
}

