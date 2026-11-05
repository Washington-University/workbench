
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

#define __DISPLAY_PROPERTIES_NERUOGLANCER_ANNOTATION_DECLARE__
#include "DisplayPropertiesNeuroglancerAnnotation.h"
#undef __DISPLAY_PROPERTIES_NERUOGLANCER_ANNOTATION_DECLARE__

#include "CaretAssert.h"
#include "CaretDataFileSelectionModel.h"
#include "DisplayPropertyDataFloat.h"
#include "NeuroglancerAnnotationFile.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesNeuroglancerAnnotation 
 * \brief Contains display properties for neuroglancer annotations.
 */

/**
 * Constructor.
 */
DisplayPropertiesNeuroglancerAnnotation::DisplayPropertiesNeuroglancerAnnotation()
: DisplayProperties()
{
    resetPrivate();
    
    m_fileSelectionModel.reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileType(DataFileTypeEnum::NEUROGLANCER_ANNOTATION));
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
    m_sceneAssistant->addTabIndexedBooleanArray("m_displayStatusInTab",
                               m_displayStatusInTab);
    
    m_sceneAssistant->addArray("m_displayStatusInDisplayGroup",
                               m_displayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_displayStatusInDisplayGroup[0]);
    m_sceneAssistant->add("m_symbolScale",
                          &m_symbolScale);
    m_sceneAssistant->add("m_distanceFromSliceTolerance",
                          &m_distanceFromSliceTolerance);
    m_sceneAssistant->add("m_fileSelectionModel",
                          "CaretDataFileSelectionModel",
                          m_fileSelectionModel.get());
}

/**
 * Destructor.
 */
DisplayPropertiesNeuroglancerAnnotation::~DisplayPropertiesNeuroglancerAnnotation()
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
DisplayPropertiesNeuroglancerAnnotation::copyDisplayProperties(const int32_t sourceTabIndex,
                                                 const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    
    m_displayStatusInTab[targetTabIndex]    = m_displayStatusInTab[sourceTabIndex];
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesNeuroglancerAnnotation::reset()
{
    resetPrivate();
}

void
DisplayPropertiesNeuroglancerAnnotation::resetPrivate()
{
    const bool defaultDisplayStatusFlag(true);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = defaultDisplayStatusFlag;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = defaultDisplayStatusFlag;
    }
    m_symbolScale = 5.0;
    m_distanceFromSliceTolerance = 2.0;
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesNeuroglancerAnnotation::update()
{
    
}

/**
 * @return  Display status of neuroglancer annotations
 * @param displayGroup
 *     Display group.
 */
bool 
DisplayPropertiesNeuroglancerAnnotation::isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                   const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        return m_displayStatusInTab[tabIndex];
    }
    return m_displayStatusInDisplayGroup[displayGroup];
}

/**
 * Set the display status for neuroglancer annotations.
 * @param displayGroup
 *     Display group.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesNeuroglancerAnnotation::setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex,
                                       const bool displayStatus)
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex); 
        m_displayStatusInTab[tabIndex] = displayStatus;
    }
    else {
        m_displayStatusInDisplayGroup[displayGroup] = displayStatus;
    }
}

/**
 * @return Scaling for symbols
 */
float
DisplayPropertiesNeuroglancerAnnotation::getSymbolScale() const
{
    return m_symbolScale;
}

/**
 * Set scaling for symbols
 * @param symbolScale
 *    New scaling value
 */
void
DisplayPropertiesNeuroglancerAnnotation::setSymbolScale(const float symbolScale)
{
    m_symbolScale = symbolScale;
}

/**
 * @return Distance a neuroglancer annotation can be from slice to be displayed
 */
float
DisplayPropertiesNeuroglancerAnnotation::getDistanceFromVolumeSliceTolerance() const
{
    return m_distanceFromSliceTolerance;
}

/**
 * Set distance a neuroglancer annotation can be from slice to be displayed
 * @param tolerance
 */
void
DisplayPropertiesNeuroglancerAnnotation::setDistanceFromVolumeSliceTolerance(const float distanceFromSlice)
{
    m_distanceFromSliceTolerance = distanceFromSlice;
}


/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum 
DisplayPropertiesNeuroglancerAnnotation::getDisplayGroupForTab(const int32_t browserTabIndex) const
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
DisplayPropertiesNeuroglancerAnnotation::setDisplayGroupForTab(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The file selection model (selects the file)
 */
CaretDataFileSelectionModel*
DisplayPropertiesNeuroglancerAnnotation::getFileSelectionModel()
{
    return m_fileSelectionModel.get();
}

/**
 * @return The file selection model (const method) (selects the file)
 */
const CaretDataFileSelectionModel*
DisplayPropertiesNeuroglancerAnnotation::getFileSelectionModel() const
{
    return m_fileSelectionModel.get();
}

/**
 * @return The selected neuroglancer annotation file
 */
NeuroglancerAnnotationFile*
DisplayPropertiesNeuroglancerAnnotation::getSelectedFile()
{
    NeuroglancerAnnotationFile* file(NULL);
    
    CaretDataFile* cdf(getFileSelectionModel()->getSelectedFile());
    if (cdf != NULL) {
        file = cdf->castToNeuroglancerAnnotationFile();
    }
    return file;
}

/**
 * @return The selected neuroglancer annotations file
 */
const NeuroglancerAnnotationFile*
DisplayPropertiesNeuroglancerAnnotation::getSelectedFile() const
{
    const NeuroglancerAnnotationFile* file(NULL);
    
    const CaretDataFile* cdf(getFileSelectionModel()->getSelectedFile());
    if (cdf != NULL) {
        file = cdf->castToNeuroglancerAnnotationFile();
    }
    return file;
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
DisplayPropertiesNeuroglancerAnnotation::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesNeuroglancerAnnotation",
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
DisplayPropertiesNeuroglancerAnnotation::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    resetPrivate();
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
}

