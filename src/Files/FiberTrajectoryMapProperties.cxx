
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__
#include "FiberTrajectoryMapProperties.h"
#undef __FIBER_TRAJECTORY_MAP_PROPERTIES_DECLARE__

#include "CaretAssert.h"
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
    
}

/**
 * Destructor.
 */
FiberTrajectoryMapProperties::~FiberTrajectoryMapProperties()
{
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

