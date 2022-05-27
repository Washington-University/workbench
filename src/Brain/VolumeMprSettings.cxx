
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

#define __VOLUME_MPR_SETTINGS_DECLARE__
#include "VolumeMprSettings.h"
#undef __VOLUME_MPR_SETTINGS_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::VolumeMprSettings 
 * \brief Settings for MPR
 * \ingroup Brain
 */

/**
 * Constructor.
 */
VolumeMprSettings::VolumeMprSettings()
: CaretObject()
{
    reset();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_sceneAssistant->add<VolumeMprOrientationModeEnum, VolumeMprOrientationModeEnum::Enum>("m_orientationMode", &m_orientationMode);
    m_sceneAssistant->add<VolumeMprViewModeEnum, VolumeMprViewModeEnum::Enum>("m_viewMode", &m_viewMode);
    m_sceneAssistant->add("m_sliceThickness", &m_sliceThickness);
    m_sceneAssistant->add("m_axialSliceThicknessEnabled", &m_axialSliceThicknessEnabled);
    m_sceneAssistant->add("m_coronalSliceThicknessEnabled", &m_coronalSliceThicknessEnabled);
    m_sceneAssistant->add("m_parasagittalSliceThicknessEnabled", &m_parasagittalSliceThicknessEnabled);
}

/**
 * Destructor.
 */
VolumeMprSettings::~VolumeMprSettings()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
VolumeMprSettings::VolumeMprSettings(const VolumeMprSettings& obj)
: CaretObject(obj)
{
    this->copyHelperVolumeMprSettings(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
VolumeMprSettings&
VolumeMprSettings::operator=(const VolumeMprSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperVolumeMprSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
VolumeMprSettings::copyHelperVolumeMprSettings(const VolumeMprSettings& obj)
{
    m_orientationMode = obj.m_orientationMode;
    m_viewMode        = obj.m_viewMode;
    m_sliceThickness  = obj.m_sliceThickness;
    m_axialSliceThicknessEnabled        = obj.m_axialSliceThicknessEnabled;
    m_coronalSliceThicknessEnabled      = obj.m_coronalSliceThicknessEnabled;
    m_parasagittalSliceThicknessEnabled = obj.m_parasagittalSliceThicknessEnabled;
}

/**
 * Reset to default settings
 */
void
VolumeMprSettings::reset()
{
    m_orientationMode = VolumeMprOrientationModeEnum::NEUROLOGICAL;
    m_viewMode        = VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION;
    m_sliceThickness  = 20.0;
    m_axialSliceThicknessEnabled        = false;
    m_coronalSliceThicknessEnabled      = false;
    m_parasagittalSliceThicknessEnabled = false;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeMprSettings::toString() const
{
    return "VolumeMprSettings";
}

/**
 * @return Orientation (Neurological / Radiological )
 */
VolumeMprOrientationModeEnum::Enum
VolumeMprSettings::getOrientationMode() const
{
    return m_orientationMode;
}

/**
 * Set the orientation (Neurological / Radiological )
 * @param orientation
 *    New orientation
 */
void
VolumeMprSettings::setOrientationMode(const VolumeMprOrientationModeEnum::Enum orientationMode)
{
    m_orientationMode = orientationMode;
}

/**
 * @return The MPR view type
 */
VolumeMprViewModeEnum::Enum
VolumeMprSettings::getViewMode() const
{
    return m_viewMode;
}

/**
 * @return Slice thickness
 */
float
VolumeMprSettings::getSliceThickness() const
{
    return m_sliceThickness;
}

/**
 * Set the slice thickness
 * @param sliceThickness
 *    New slice thickness
 */
void
VolumeMprSettings::setSliceThickness(const float sliceThickness)
{
    m_sliceThickness = sliceThickness;
}

/**
 * Set the MPR view mode
 * @param viewMode
 *    New MPR view mode
 */
void
VolumeMprSettings::setViewMode(const VolumeMprViewModeEnum::Enum viewMode)
{
    m_viewMode = viewMode;
}

/**
 * @return Is axial slice thickness enabled
 */
bool
VolumeMprSettings::isAxialSliceThicknessEnabled() const
{
    return m_axialSliceThicknessEnabled;
}

/**
 * Set the slice axial thickness enabled
 * @param enabled
 *    New status
 */
void
VolumeMprSettings::setAxialSliceThicknessEnabled(const bool enabled)
{
    m_axialSliceThicknessEnabled = enabled;
}

/**
 * @return Is coronal slice thickness enabled
 */
bool
VolumeMprSettings::isCoronalSliceThicknessEnabled() const
{
    return m_axialSliceThicknessEnabled;
}

/**
 * Set the slice coronal thickness enabled
 * @param enabled
 *    New status
 */
void
VolumeMprSettings::setCoronalSliceThicknessEnabled(const bool enabled)
{
    m_axialSliceThicknessEnabled = enabled;
}

/**
 * @return Is parasagittal slice thickness enabled
 */
bool
VolumeMprSettings::isParasagittalSliceThicknessEnabled() const
{
    return m_parasagittalSliceThicknessEnabled;
}

/**
 * Set the parasagittal slice thickness enabled
 * @param enabled
 *    New status
 */
void
VolumeMprSettings::setParasagittalSliceThicknessEnabled(const bool enabled)
{
    m_parasagittalSliceThicknessEnabled = enabled;
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
VolumeMprSettings::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeMprSettings",
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
VolumeMprSettings::restoreFromScene(const SceneAttributes* sceneAttributes,
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

