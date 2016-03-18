
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
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
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
    m_displayModelAnnotations[targetTabIndex] = m_displayModelAnnotations[sourceTabIndex];
    m_displaySurfaceAnnotations[targetTabIndex] = m_displaySurfaceAnnotations[sourceTabIndex];
    m_displayTabAnnotations[targetTabIndex] = m_displayTabAnnotations[sourceTabIndex];
}

/**
 * Reset all settings to default.
 * NOTE: reset() is virtual so can/should not be called from constructor.
 */
void
DisplayPropertiesAnnotation::resetPrivate()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayModelAnnotations[i] = true;
        m_displaySurfaceAnnotations[i] = true;
        m_displayTabAnnotations[i] = true;
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        m_displayWindowAnnotations[i] = true;
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
 * Is the model annotation displayed in the given tab index?
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 * True if displayed, else false.
 */
bool
DisplayPropertiesAnnotation::isDisplayModelAnnotationsInTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayModelAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_displayModelAnnotations[tabIndex];
}

/**
 * Is the model annotation displayed in any of the given tab indices?
 *
 * @param tabIndices
 *     Index of the tab.
 * @return
 * True if displayed, else false.
 */
bool
DisplayPropertiesAnnotation::isDisplayModelAnnotationsInTabs(const std::vector<int32_t>& tabIndices) const
{
    for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
         iter != tabIndices.end();
         iter++) {
        const int32_t tabIndex = *iter;
        CaretAssertArrayIndex(m_displayModelAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
        if (m_displayModelAnnotations[tabIndex]) {
            return true;
        }
    }
    
    return false;
}

/**
 * Set the model annotation displayed in the given tab index.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     True if displayed, else false.
 */
void
DisplayPropertiesAnnotation::setDisplayModelAnnotationsInTab(const int32_t tabIndex,
                                                        const bool status)
{
    CaretAssertArrayIndex(m_displayModelAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_displayModelAnnotations[tabIndex] = status;
}

/**
 * Is the surface annotation displayed in the given tab index?
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 * True if displayed, else false.
 */
bool
DisplayPropertiesAnnotation::isDisplaySurfaceAnnotationsInTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displaySurfaceAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_displaySurfaceAnnotations[tabIndex];
}

/**
 * Is the surface annotation displayed in any of the given tab indices?
 *
 * @param tabIndices
 *     Index of the tab.
 * @return
 * True if displayed, else false.
 */
bool
DisplayPropertiesAnnotation::isDisplaySurfaceAnnotationsInTabs(const std::vector<int32_t>& tabIndices) const
{
    for (std::vector<int32_t>::const_iterator iter = tabIndices.begin();
         iter != tabIndices.end();
         iter++) {
        const int32_t tabIndex = *iter;
        CaretAssertArrayIndex(m_displaySurfaceAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
        if (m_displaySurfaceAnnotations[tabIndex]) {
            return true;
        }
    }
    
    return false;
}

/**
 * Set the surface annotation displayed in the given tab index.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     True if displayed, else false.
 */
void
DisplayPropertiesAnnotation::setDisplaySurfaceAnnotationsInTab(const int32_t tabIndex,
                                                          const bool status)
{
    CaretAssertArrayIndex(m_displaySurfaceAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_displaySurfaceAnnotations[tabIndex] = status;
}

/**
 * Is the tab annotation displayed in the given tab index?
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 * True if displayed, else false.
 */
bool
DisplayPropertiesAnnotation::isDisplayTabAnnotationsInTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayTabAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_displayTabAnnotations[tabIndex];
}

/**
 * Set the tab annotation displayed in the given tab index.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     True if displayed, else false.
 */
void
DisplayPropertiesAnnotation::setDisplayTabAnnotationsInTab(const int32_t tabIndex,
                                                      const bool status)
{
    CaretAssertArrayIndex(m_displayTabAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_displayTabAnnotations[tabIndex] = status;
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
DisplayPropertiesAnnotation::isDisplayWindowAnnotationsInTab(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_displayWindowAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_displayWindowAnnotations[windowIndex];
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
DisplayPropertiesAnnotation::setDisplayWindowAnnotationsInTab(const int32_t windowIndex,
                                                         const bool status)
{
    CaretAssertArrayIndex(m_displayWindowAnnotations, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_displayWindowAnnotations[windowIndex] = status;
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
DisplayPropertiesAnnotation::restoreFromScene(const SceneAttributes* sceneAttributes,
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

