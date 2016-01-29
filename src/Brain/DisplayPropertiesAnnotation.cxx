
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

#define __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__
#include "DisplayPropertiesAnnotation.h"
#undef __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__

#include "Brain.h"
#include "CaretLogger.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
using namespace caret;



/**
 * \class caret::DisplayPropertiesAnnotation
 * \brief Contains display properties for annotations.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
DisplayPropertiesAnnotation::DisplayPropertiesAnnotation(Brain* parentBrain)
: DisplayProperties(),
m_parentBrain(parentBrain)
{
    CaretAssert(parentBrain);
    
    resetPrivate();
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = false;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = false;
    }
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
    m_sceneAssistant->addTabIndexedBooleanArray("m_displayStatusInTab",
                                                m_displayStatusInTab);
    
    m_sceneAssistant->addArray("m_displayStatusInDisplayGroup",
                               m_displayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_displayStatusInDisplayGroup[0]);
    
    
    
    
    
    m_sceneAssistant->addArray("m_displayWindowAnnotationsOnlyInTileTabs",
                               m_displayWindowAnnotationsOnlyInTileTabs,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS,
                               true);
}

/**
 * Destructor.
 */
DisplayPropertiesAnnotation::~DisplayPropertiesAnnotation()
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
DisplayPropertiesAnnotation::copyDisplayProperties(const int32_t sourceTabIndex,
                                                   const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    this->m_displayStatusInTab[targetTabIndex] = this->m_displayStatusInTab[sourceTabIndex];
}

/**
 * Reset all settings to default.
 * NOTE: reset() is virtual so can/should not be called from constructor.
 */
void
DisplayPropertiesAnnotation::resetPrivate()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        m_displayWindowAnnotationsOnlyInTileTabs[i] = false;
    }
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesAnnotation::reset()
{
    resetPrivate();
}

/**
 * Update due to changes in data.
 */
void
DisplayPropertiesAnnotation::update()
{
    
}

/**
 * @return  Display status of borders.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool
DisplayPropertiesAnnotation::isDisplayed(const DisplayGroupEnum::Enum  displayGroup,
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
 * Set the display status for borders for the given display group.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void
DisplayPropertiesAnnotation::setDisplayed(const DisplayGroupEnum::Enum  displayGroup,
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
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum
DisplayPropertiesAnnotation::getDisplayGroupForTab(const int32_t browserTabIndex) const
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
DisplayPropertiesAnnotation::setDisplayGroupForTab(const int32_t browserTabIndex,
                                                const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}


/**
 * Is the window annotation displayed in the given window index?
 *
 * @param windowIndex
 *     Index of the window.
 * @return
 * True if displayed, else false.
 */
bool
DisplayPropertiesAnnotation::isDisplayWindowAnnotationsOnlyInTileTabs(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_displayWindowAnnotationsOnlyInTileTabs, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_displayWindowAnnotationsOnlyInTileTabs[windowIndex];
}

/**
 * Set the window annotation displayed in the given window index.
 *
 * @param windowIndex
 *     Index of the tab.
 * @param status
 *     True if displayed, else false.
 */
void
DisplayPropertiesAnnotation::setDisplayWindowAnnotationsOnlyInTileTabs(const int32_t windowIndex,
                                                         const bool status)
{
    CaretAssertArrayIndex(m_displayWindowAnnotationsOnlyInTileTabs, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_displayWindowAnnotationsOnlyInTileTabs[windowIndex] = status;
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
DisplayPropertiesAnnotation::saveToScene(const SceneAttributes* sceneAttributes,
                                         const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesAnnotation",
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
DisplayPropertiesAnnotation::restoreFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    CaretLogSevere("NEED TO CHECK SCENE VERSION FOR OLD TAB/WINDOW SELECTION");
    
    if (sceneClass->getVersionNumber() == 1) {
        /*
         * Version one scenes did not have class/name hierarchy
         *
         m_sceneAssistant->addTabIndexedBooleanArray("m_displayModelAnnotations",
         m_displayModelAnnotations);
         m_sceneAssistant->addTabIndexedBooleanArray("m_displaySurfaceAnnotations",
         m_displaySurfaceAnnotations);
         m_sceneAssistant->addTabIndexedBooleanArray("m_displayTabAnnotations",
         m_displayTabAnnotations);
         
         m_sceneAssistant->addArray("m_displayWindowAnnotations",
         m_displayWindowAnnotations,
         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS,
         true);
         *
         */
        
        bool displayModelAnnotation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        if (sceneClass->getBooleanArrayValue("m_displayModelAnnotations",
                                             displayModelAnnotation,
                                             BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)
            == BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS) {
            
        }
        
        bool displaySurfaceAnnotation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        if (sceneClass->getBooleanArrayValue("m_displaySurfaceAnnotations",
                                             displaySurfaceAnnotation,
                                             BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)
            == BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS) {
            
        }
        
        bool displayTabAnnotation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        if (sceneClass->getBooleanArrayValue("m_displayTabAnnotations",
                                             displayTabAnnotation,
                                             BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)
            == BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS) {
            
        }
        
        bool displayWindowAnnotation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
        if (sceneClass->getBooleanArrayValue("m_displayWindowAnnotations",
                                             displayWindowAnnotation,
                                             BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)
            == BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS) {
            
        }
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

