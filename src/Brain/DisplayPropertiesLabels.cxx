
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
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_drawingTypeInTab[i] = LabelDrawingTypeEnum::DRAW_FILLED;
        m_outlineColorInTab[i] = CaretColorEnum::BLACK;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_drawingTypeInDisplayGroup[i] = LabelDrawingTypeEnum::DRAW_FILLED;
        m_outlineColorInDisplayGroup[i] = CaretColorEnum::BLACK;
    }
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<LabelDrawingTypeEnum,LabelDrawingTypeEnum::Enum>("m_drawingTypeInTab",
                                                                                                      m_drawingTypeInTab);
    
    
    m_sceneAssistant->addArray<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>("m_drawingTypeInDisplayGroup",
                                                                               m_drawingTypeInDisplayGroup,
                                                                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                                 LabelDrawingTypeEnum::DRAW_FILLED);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<CaretColorEnum,CaretColorEnum::Enum>("m_outlineColorInTab",
                                                                                            m_outlineColorInTab);
    
    
    m_sceneAssistant->addArray<CaretColorEnum, CaretColorEnum::Enum>("m_outlineColorInDisplayGroup",
                                                                     m_outlineColorInDisplayGroup,
                                                                     DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                     CaretColorEnum::BLACK);
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
    
    m_drawingTypeInTab[targetTabIndex]      = m_drawingTypeInTab[sourceTabIndex];
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesLabels::reset()
{
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
 * @param displayGroup
 *     Display group.
 * @return The drawing type.
 */
LabelDrawingTypeEnum::Enum
DisplayPropertiesLabels::getDrawingType(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_drawingTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_drawingTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_drawingTypeInTab[tabIndex];
    }
    return m_drawingTypeInDisplayGroup[displayGroup];
}

/**
 * Set the drawing type to the given value.
 * @param displayGroup
 *     Display group.
 * @param drawingType
 *     New value for drawing type.
 */
void
DisplayPropertiesLabels::setDrawingType(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex,
                                      const LabelDrawingTypeEnum::Enum drawingType)
{
    CaretAssertArrayIndex(m_drawingTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_drawingTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_drawingTypeInTab[tabIndex] = drawingType;
    }
    else {
        m_drawingTypeInDisplayGroup[displayGroup] = drawingType;
    }
}

/**
 * @param displayGroup
 *     Display group.
 * @return The outline color.
 */
CaretColorEnum::Enum
DisplayPropertiesLabels::getOutlineColor(const DisplayGroupEnum::Enum displayGroup,
                                         const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_outlineColorInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_outlineColorInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_outlineColorInTab[tabIndex];
    }
    return m_outlineColorInDisplayGroup[displayGroup];
}

/**
 * Set the outline color to the given value.
 * @param displayGroup
 *     Display group.
 * @param outlineColor
 *     New value for outline color.
 */
void
DisplayPropertiesLabels::setOutlineColor(const DisplayGroupEnum::Enum displayGroup,
                                         const int32_t tabIndex,
                                         const CaretColorEnum::Enum outlineColor)
{
    CaretAssertArrayIndex(m_outlineColorInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_outlineColorInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_outlineColorInTab[tabIndex] = outlineColor;
    }
    else {
        m_outlineColorInDisplayGroup[displayGroup] = outlineColor;
    }
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


