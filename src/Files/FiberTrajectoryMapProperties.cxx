
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

#define __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__
#include "FiberTrajectoryMapProperties.h"
#undef __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__

#include "CaretAssert.h"
#include "FiberTrajectoryColorModel.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;



/**
 * \class caret::FiberTrajectoryMapProperties
 * \brief Contains display properties for a fiber trajectory file.
 */

/**
 * Constructor.
 */
FiberTrajectoryMapProperties::FiberTrajectoryMapProperties()
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_fiberTrajectoryColoringModel = new FiberTrajectoryColorModel();
    
    const float thresholdStreamline = 5;
    const float maximumProportionOpacity = 0.80;
    const float minimumProportionOpacity = 0.05;
    const FiberTrajectoryDisplayModeEnum::Enum displayMode = FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_ABSOLUTE;
    const float countMaximum = 50;
    const float countMinimum =  5;
    
    m_displayStatus = false;
    
    m_displayMode = displayMode;
    m_proportionStreamline = thresholdStreamline;
    m_maximumProportionOpacity = maximumProportionOpacity;
    m_minimumProportionOpacity = minimumProportionOpacity;
    
    m_countStreamline = thresholdStreamline;
    m_countMaximumOpacity = countMaximum;
    m_countMinimumOpacity = countMinimum;
    
    m_distanceStreamline = thresholdStreamline;
    m_distanceMaximumOpacity = countMaximum;
    m_distanceMinimumOpacity = countMinimum;
    
    m_sceneAssistant->add("m_displayStatus",
                          &m_displayStatus);
    
    m_sceneAssistant->add<FiberTrajectoryDisplayModeEnum, FiberTrajectoryDisplayModeEnum::Enum>("m_displayMode",
                                                                                                &m_displayMode);
    m_sceneAssistant->add("m_proportionStreamline",
                          &m_proportionStreamline);
    m_sceneAssistant->add("m_maximumProportionOpacity",
                          &m_maximumProportionOpacity);
    m_sceneAssistant->add("m_minimumProportionOpacity",
                          &m_minimumProportionOpacity);
    
    m_sceneAssistant->add("m_countStreamline",
                          &m_countStreamline);
    m_sceneAssistant->add("m_countMaximumOpacity",
                          &m_countMaximumOpacity);
    m_sceneAssistant->add("m_countMinimumOpacity",
                          &m_countMinimumOpacity);
    
    m_sceneAssistant->add("m_distanceStreamline",
                          &m_distanceStreamline);
    m_sceneAssistant->add("m_distanceMaximumOpacity",
                          &m_distanceMaximumOpacity);
    m_sceneAssistant->add("m_distanceMinimumOpacity",
                          &m_distanceMinimumOpacity);
    m_sceneAssistant->add("m_fiberTrajectoryColoringModel",
                          "FiberTrajectoryColorModel",
                          m_fiberTrajectoryColoringModel);
    
}

/**
 * Destructor.
 */
FiberTrajectoryMapProperties::~FiberTrajectoryMapProperties()
{
    delete m_fiberTrajectoryColoringModel;
    delete m_sceneAssistant;
}

/**
 * @return  Display status of trajectory.
 */
bool
FiberTrajectoryMapProperties::isDisplayed() const
{
    return m_displayStatus;
}

/**
 * Set the display status for trajectory for the given display group.
 * @param displayStatus
 *    New status.
 */
void
FiberTrajectoryMapProperties::setDisplayed(const bool displayStatus)
{
    m_displayStatus = displayStatus;
}


/**
 * @return The display mode.
 */
FiberTrajectoryDisplayModeEnum::Enum
FiberTrajectoryMapProperties::getDisplayMode() const
{
    return m_displayMode;
}

/**
 * Set the display mode to the given value.
 * @param displayMode
 *     New value for display mode.
 */
void
FiberTrajectoryMapProperties::setDisplayMode(const FiberTrajectoryDisplayModeEnum::Enum displayMode)
{
    m_displayMode = displayMode;
}

/**
 * @return The proportion streamline count
 */
float
FiberTrajectoryMapProperties::getProportionStreamline() const
{
    return m_proportionStreamline;
}

/**
 * Set the proportion streamline count.
 * @param pointSize
 *     New value for below limit.
 */
void
FiberTrajectoryMapProperties::setProportionStreamline(const float proportionStreamline)
{
    m_proportionStreamline = proportionStreamline;
}

/**
 * @return The proporation maximum opacity.
 */
float
FiberTrajectoryMapProperties::getProportionMaximumOpacity() const
{
    return m_maximumProportionOpacity;
}

/**
 * Set the proporation maximum opacity.
 * @param minimumMagnitude
 *     New value for minimum magnitude.
 */
void
FiberTrajectoryMapProperties::setProportionMaximumOpacity(const float maximumMagnitude)
{
    m_maximumProportionOpacity = maximumMagnitude;
}

/**
 * @return The proporation minimum opacity.
 */
float
FiberTrajectoryMapProperties::getProportionMinimumOpacity() const
{
    return m_minimumProportionOpacity;
}

/**
 * Set the proporation minimum opacity.
 * @param minimumOpacity
 *     New value for minimum opacity
 */
void
FiberTrajectoryMapProperties::setProportionMinimumOpacity(const float minimumOpacity)
{
    m_minimumProportionOpacity = minimumOpacity;
}

/**
 * @return The count streamline threshold.
 */
float
FiberTrajectoryMapProperties::getCountStreamline() const
{
    return m_countStreamline;
}

/**
 * Set the count streamline threshold.
 * @param countStreamline
 *     New value for count streamline threshold
 */
void
FiberTrajectoryMapProperties::setCountStreamline(const float countStreamline)
{
    m_countStreamline = countStreamline;
}

/**
 * @return The count value mapped to maximum opacity.
 */
float
FiberTrajectoryMapProperties::getCountMaximumOpacity() const
{
    return m_countMaximumOpacity;
}

/**
 * Set the count maximum opacity.
 * @param countMaximumOpacity
 *     New value for count mapped to maximum opacity
 */
void
FiberTrajectoryMapProperties::setCountMaximumOpacity(const float countMaximumOpacity)
{
    m_countMaximumOpacity = countMaximumOpacity;
}

/**
 * @return The count value mapped to minimum opacity.
 */
float
FiberTrajectoryMapProperties::getCountMinimumOpacity() const
{
    return m_countMinimumOpacity;
}

/**
 * Set the count minimum opacity.
 * @param countMinimumOpacity
 *     New value for count mapped to minimum opacity
 */
void
FiberTrajectoryMapProperties::setCountMinimumOpacity(const float countMinimumOpacity)
{
    m_countMinimumOpacity = countMinimumOpacity;
}



/**
 * @return The distance streamline threshold.
 */
float
FiberTrajectoryMapProperties::getDistanceStreamline() const
{
    return m_distanceStreamline;
}

/**
 * Set the distance streamline threshold.
 * @param distanceStreamline
 *     New value for distance streamline threshold
 */
void
FiberTrajectoryMapProperties::setDistanceStreamline(const float distanceStreamline)
{
    m_distanceStreamline = distanceStreamline;
}

/**
 * @return The distance value mapped to maximum opacity.
 */
float
FiberTrajectoryMapProperties::getDistanceMaximumOpacity() const
{
    return m_distanceMaximumOpacity;
}

/**
 * Set the distance maximum opacity.
 * @param distanceMaximumOpacity
 *     New value for distance mapped to maximum opacity
 */
void
FiberTrajectoryMapProperties::setDistanceMaximumOpacity(const float distanceMaximumOpacity)
{
    m_distanceMaximumOpacity = distanceMaximumOpacity;
}

/**
 * @return The distance value mapped to minimum opacity.
 */
float
FiberTrajectoryMapProperties::getDistanceMinimumOpacity() const
{
    return m_distanceMinimumOpacity;
}

/**
 * Set the distance minimum opacity.
 * @param distanceMinimumOpacity
 *     New value for distance mapped to minimum opacity
 */
void
FiberTrajectoryMapProperties::setDistanceMinimumOpacity(const float distanceMinimumOpacity)
{
    m_distanceMinimumOpacity = distanceMinimumOpacity;
}

/**
 * @return the fiber trajectory coloring model.
 */
FiberTrajectoryColorModel*
FiberTrajectoryMapProperties::getFiberTrajectoryColorModel()
{
    return m_fiberTrajectoryColoringModel;
}

/**
 * @return the fiber trajectory coloring model.
 */
const FiberTrajectoryColorModel*
FiberTrajectoryMapProperties::getFiberTrajectoryColorModel() const
{
    return m_fiberTrajectoryColoringModel;
}

/**
 * Copy the other map properties.
 *
 * @param other
 *   The map properties that are copied.
 */
void
FiberTrajectoryMapProperties::copy(const FiberTrajectoryMapProperties& other)
{
    m_displayMode              = other.m_displayMode;
    m_displayStatus            = other.m_displayStatus;
    m_proportionStreamline     = other.m_proportionStreamline;
    m_maximumProportionOpacity = other.m_maximumProportionOpacity;
    m_minimumProportionOpacity = other.m_minimumProportionOpacity;
    m_countStreamline          = other.m_countStreamline;
    m_countMaximumOpacity      = other.m_countMaximumOpacity;
    m_countMinimumOpacity      = other.m_countMinimumOpacity;
    m_distanceStreamline       = other.m_distanceStreamline;
    m_distanceMaximumOpacity   = other.m_distanceMaximumOpacity;
    m_distanceMinimumOpacity   = other.m_distanceMinimumOpacity;
    m_fiberTrajectoryColoringModel->copy(*other.getFiberTrajectoryColorModel());
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
FiberTrajectoryMapProperties::saveToScene(const SceneAttributes* sceneAttributes,
                                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FiberTrajectoryMapProperties",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
FiberTrajectoryMapProperties::restoreFromScene(const SceneAttributes* sceneAttributes,
                                               const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
}

