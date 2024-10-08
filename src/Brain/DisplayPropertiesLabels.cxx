
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

#define __DISPLAY_PROPERTIES_LABELS_DECLARE__
#include "DisplayPropertiesLabels.h"
#include "SceneClassAssistant.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#undef __DISPLAY_PROPERTIES_LABELS_DECLARE__

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesLabels 
 * \brief Display properties for labels
 */

/**
 * Constructor.
 */
DisplayPropertiesLabels::DisplayPropertiesLabels()
: DisplayProperties()
{
    m_displayGroup.fill(DisplayGroupEnum::getDefaultValue());
    m_labelViewMode.fill(LabelViewModeEnum::LIST);

    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup.data());
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<LabelViewModeEnum, LabelViewModeEnum::Enum>("m_labelViewMode",
                                                                                                m_labelViewMode.data());
    
}

/**
 * Destructor.
 */
DisplayPropertiesLabels::~DisplayPropertiesLabels()
{
    
}

/**
 * Copy the border display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void
DisplayPropertiesLabels::copyDisplayProperties(const int32_t sourceTabIndex,
                                             const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    this->setLabelViewModeForTab(targetTabIndex, 
                                 this->getLabelViewModeForTab(sourceTabIndex));
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesLabels::reset()
{
    m_displayGroup.fill(DisplayGroupEnum::getDefaultValue());
    m_labelViewMode.fill(LabelViewModeEnum::LIST);
}

/**
 * Update due to changes in data.
 */
void
DisplayPropertiesLabels::update()
{
    
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum
DisplayPropertiesLabels::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(m_displayGroup,
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
DisplayPropertiesLabels::setDisplayGroupForTab(const int32_t browserTabIndex,
                                             const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(m_displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The label view mode for the given tab
 * @param browserTabIndex
 *    Index of browser tab.
 */
LabelViewModeEnum::Enum
DisplayPropertiesLabels::getLabelViewModeForTab(const int32_t browserTabIndex) const
{
    CaretAssertVectorIndex(m_labelViewMode, browserTabIndex);
    return m_labelViewMode[browserTabIndex];
}

/**
 * Set the label view mode for the given tab
 * @param browserTabIndex
 *    Index of browser tab.
 * @param labelViewMode
 *    New label view mode
 */
void
DisplayPropertiesLabels::setLabelViewModeForTab(const int32_t browserTabIndex,
                                                const LabelViewModeEnum::Enum labelViewMode)
{
    CaretAssertVectorIndex(m_labelViewMode, browserTabIndex);
    m_labelViewMode[browserTabIndex] = labelViewMode;
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
DisplayPropertiesLabels::saveToScene(const SceneAttributes* sceneAttributes,
                                   const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesLabels",
                                            1);
    
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
DisplayPropertiesLabels::restoreFromScene(const SceneAttributes* sceneAttributes,
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


