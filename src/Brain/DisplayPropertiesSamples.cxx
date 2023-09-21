
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __DISPLAY_PROPERTIES_SAMPLES_DECLARE__
#include "DisplayPropertiesSamples.h"
#undef __DISPLAY_PROPERTIES_SAMPLES_DECLARE__

#include "Annotation.h"
#include "AnnotationManager.h"
#include "Brain.h"
#include "CaretLogger.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"

using namespace caret;



/**
 * \class caret::DisplayPropertiesSamples
 * \brief Contains display properties for samples.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
DisplayPropertiesSamples::DisplayPropertiesSamples(Brain* parentBrain)
: DisplayProperties(),
m_parentBrain(parentBrain)
{
    CaretAssert(parentBrain);
    
    resetPrivate();
    
    m_sceneAssistant->add("m_displaySamples",
                          &m_displaySamples);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
}

/**
 * Destructor.
 */
DisplayPropertiesSamples::~DisplayPropertiesSamples()
{
}

/**
 * Update the properties for a new/modified sample.
 *
 * @param sample
 *     The new/updated sample.
 */
void
DisplayPropertiesSamples::updateForNewSample(const Annotation* sample)
{
    CaretAssert(sample);
    
    setDisplaySamples(true);
}

/**
 * Update the properties for a new/modified sample.
 *
 * @param sample
 *     The new/updated sample.
 */
void
DisplayPropertiesSamples::updateForNewSamples(const std::vector<Annotation*>& samples)
{
    for (auto& ann : samples) {
        CaretAssert(ann);
        updateForNewSample(ann);
    }
    
    setDisplaySamples(true);
}


/**
 * Copy the border display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void
DisplayPropertiesSamples::copyDisplayProperties(const int32_t sourceTabIndex,
                                                   const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
}

/**
 * Reset all settings to default.
 * NOTE: reset() is virtual so can/should not be called from constructor.
 */
void
DisplayPropertiesSamples::resetPrivate()
{
    m_displaySamples = true;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    }
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesSamples::reset()
{
    resetPrivate();
}

/**
 * Update due to changes in data.
 */
void
DisplayPropertiesSamples::update()
{
    
}

/**
 * @return Status for displaying samples
 */
bool
DisplayPropertiesSamples::isDisplaySamples() const
{
    return m_displaySamples;
}

/**
 * Set the display status for samples
 *
 * @param status
 *     New display status.
 */
void
DisplayPropertiesSamples::setDisplaySamples(const bool status)
{
    m_displaySamples = status;
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum
DisplayPropertiesSamples::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(this->displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return m_displayGroup[browserTabIndex];
}

/**
 * Set the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayGroup
 *    New value for display group.
 */
void
DisplayPropertiesSamples::setDisplayGroupForTab(const int32_t browserTabIndex,
                                                const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
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
DisplayPropertiesSamples::saveToScene(const SceneAttributes* sceneAttributes,
                                         const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesSamples",
                                            2);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
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
DisplayPropertiesSamples::restoreFromScene(const SceneAttributes* sceneAttributes,
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

