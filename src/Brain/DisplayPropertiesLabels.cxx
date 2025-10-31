
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
#undef __DISPLAY_PROPERTIES_LABELS_DECLARE__

#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "EventDisplayPropertiesLabels.h"
#include "EventManager.h"
#include "SceneClassAssistant.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"

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
    m_showUnusedLabelsInHierarchiesFlag = false;
    m_showBranchesWithoutLabelsFlag = false;
    
    std::vector<DataFileTypeEnum::Enum> dataFileTypes {
        DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL,
        DataFileTypeEnum::LABEL,
        DataFileTypeEnum::VOLUME
    };
    std::vector<SubvolumeAttributes::VolumeType> volumeTypes { SubvolumeAttributes::LABEL };

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_fileSelectionModelForBrowserTabs[i].reset(new CaretMappableDataFileAndMapSelectionModel(dataFileTypes,
                                                                                                  volumeTypes));
    }
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_fileSelectionModelsForDisplayGroups[i].reset(new CaretMappableDataFileAndMapSelectionModel(dataFileTypes,
                                                                                                     volumeTypes));
    }
    
    m_sceneAssistant->add("m_showUnusedLabelsInHierarchiesFlag",
                          &m_showUnusedLabelsInHierarchiesFlag);
    m_sceneAssistant->add("m_showBranchesWithoutLabelsFlag",
                          &m_showBranchesWithoutLabelsFlag);
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
    
    CaretMappableDataFileAndMapSelectionModel* sourceModel(getFileSelectionModel(DisplayGroupEnum::DISPLAY_GROUP_TAB, sourceTabIndex));
    CaretMappableDataFileAndMapSelectionModel* targetModel(getFileSelectionModel(DisplayGroupEnum::DISPLAY_GROUP_TAB, targetTabIndex));
    targetModel->setSelectedFile(sourceModel->getSelectedFile());
    targetModel->setSelectedMapIndex(sourceModel->getSelectedMapIndex());
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
 * @return True if unused labels should be shown in label hierarchies
 */
bool
DisplayPropertiesLabels::isShowUnusedLabelsInHierarchies() const
{
    return m_showUnusedLabelsInHierarchiesFlag;
}

/**
 * Set unused labels should be shown in label hierarchies
 * @param status
 *    New status
 */
void
DisplayPropertiesLabels::setShowUnusedLabelsInHierarchies(const bool status)
{
    if (status != m_showUnusedLabelsInHierarchiesFlag) {
        m_showUnusedLabelsInHierarchiesFlag = status;
        
        EventDisplayPropertiesLabels eventDPL(EventDisplayPropertiesLabels::Mode::SEND_SHOW_UNUSED_LABELS_CHANGED);
        eventDPL.setDisplayPropertiesLabels(this);
        EventManager::get()->sendEvent(eventDPL.getPointer());
    }
}

/**
 * @return True if branches without labels should be shown hierarchies
 */
bool
DisplayPropertiesLabels::isShowBranchesWithoutLabelsInHierarchies() const
{
    return m_showBranchesWithoutLabelsFlag;
}

/**
 * Set branches without labels should be shown hierarchies
 * @param status
 *    New status
 */
void
DisplayPropertiesLabels::setShowBranchesWithoutLabelsInHierarchies(const bool status)
{
    if (status != m_showBranchesWithoutLabelsFlag) {
        m_showBranchesWithoutLabelsFlag = status;
        
        EventDisplayPropertiesLabels eventDPL(EventDisplayPropertiesLabels::Mode::SEND_SHOW_UNUSED_LABELS_CHANGED);
        eventDPL.setDisplayPropertiesLabels(this);
        EventManager::get()->sendEvent(eventDPL.getPointer());
    }
}

/**
 * @return The file selection model for the given display group / tab index
 * @param displayGroup
 *    The display group
 * @param tabIndex
 *    Index of the tab
 */
CaretMappableDataFileAndMapSelectionModel*
DisplayPropertiesLabels::getFileSelectionModel(const DisplayGroupEnum::Enum displayGroup,
                                               const int32_t tabIndex)
{
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertVectorIndex(m_fileSelectionModelForBrowserTabs, tabIndex);
        return m_fileSelectionModelForBrowserTabs[tabIndex].get();
    }
    
    const int32_t displayGroupIndex(DisplayGroupEnum::toIntegerCode(displayGroup));
    return m_fileSelectionModelsForDisplayGroups[displayGroupIndex].get();
}

/**
 * @return The file selection model for the given display group / tab index
 * @param displayGroup
 *    The display group
 * @param tabIndex
 *    Index of the tab
 */
const CaretMappableDataFileAndMapSelectionModel*
DisplayPropertiesLabels::getFileSelectionModel(const DisplayGroupEnum::Enum displayGroup,
                                               const int32_t tabIndex) const
{
    DisplayPropertiesLabels* nonConstThis(const_cast<DisplayPropertiesLabels*>(this));
    CaretAssert(nonConstThis);
    return nonConstThis->getFileSelectionModel(displayGroup,
                                               tabIndex);
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
    
    {
        std::vector<SceneClass*> tabFileModelClasses;
        for (auto& fs : m_fileSelectionModelForBrowserTabs) {
            AString name("m_fileSelectionModelForBrowserTabs"
                         + AString::number(tabFileModelClasses.size()));
            tabFileModelClasses.push_back(fs->saveToScene(sceneAttributes,
                                                          name));
        }
        SceneClassArray* tabModels = new SceneClassArray("tabFileModelClasses",
                                                         tabFileModelClasses);
        sceneClass->addChild(tabModels);
    }

    {
        std::vector<SceneClass*> displayGroupFileModelClasses;
        for (auto& fs : m_fileSelectionModelsForDisplayGroups) {
            AString name("m_fileSelectionModelsForDisplayGroups"
                         + AString::number(displayGroupFileModelClasses.size()));
            displayGroupFileModelClasses.push_back(fs->saveToScene(sceneAttributes,
                                                                   name));
        }
        SceneClassArray* displayGroupModels = new SceneClassArray("displayGroupFileModelClasses",
                                                                  displayGroupFileModelClasses);
        sceneClass->addChild(displayGroupModels);
    }
    
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
    
    const SceneClassArray* displayGroupModels = sceneClass->getClassArray("displayGroupFileModelClasses");
    if (displayGroupModels != NULL) {
        const int32_t numItems = displayGroupModels->getNumberOfArrayElements();
        for (int32_t i = 0; i < numItems; i++) {
            CaretAssertArrayIndex(m_fileSelectionModelsForDisplayGroups, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, i);
            m_fileSelectionModelsForDisplayGroups[i]->restoreFromScene(sceneAttributes,
                                                                       displayGroupModels->getClassAtIndex(i));
        }
    }
    
    const SceneClassArray* fileModels = sceneClass->getClassArray("tabFileModelClasses");
    if (fileModels != NULL) {
        const int32_t numItems = fileModels->getNumberOfArrayElements();
        for (int32_t i = 0; i < numItems; i++) {
            CaretAssertArrayIndex(m_fileSelectionModelForBrowserTabs, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, i);
            m_fileSelectionModelForBrowserTabs[i]->restoreFromScene(sceneAttributes,
                                                                    fileModels->getClassAtIndex(i));
        }
    }

    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    /*
     * Need to toggle status which will cause an event to go out to the listeners of the
     * change in the show unused labels status
     */
    const bool showUnusedLabelsStatus = m_showUnusedLabelsInHierarchiesFlag;
    m_showUnusedLabelsInHierarchiesFlag = ! m_showUnusedLabelsInHierarchiesFlag;
    setShowUnusedLabelsInHierarchies(showUnusedLabelsStatus);
}


