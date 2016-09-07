
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __DISPLAY_PROPERTY_DATA_FLOAT_DECLARE__
#include "DisplayPropertyDataFloat.h"
#undef __DISPLAY_PROPERTY_DATA_FLOAT_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;
    
/**
 * \class caret::DisplayPropertyDataFloat 
 * \brief Float data for display property tab/display group
 * \ingroup Brain
 */

/**
 * Constructor that initializes with the given default value.
 *
 * @param defaultValue
 *    Default data value.
 */
DisplayPropertyDataFloat::DisplayPropertyDataFloat(const float defaultValue)
: CaretObject()
{
    setAllValues(defaultValue);
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->addTabIndexedFloatArray("m_tabValues",
                                              m_tabValues);
    m_sceneAssistant->addArray("m_displayGroupValues",
                               m_displayGroupValues,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               defaultValue);
}

/**
 * Destructor.
 */
DisplayPropertyDataFloat::~DisplayPropertyDataFloat()
{
    delete m_sceneAssistant;
}

/**
 * Copy the values from one tab to another.
 *
 * @param sourceTabIndex
 *    Index of tab from which values are copied.
 * @param targetTabIndex
 *    Index of tab to which values are copied.
 */
void
DisplayPropertyDataFloat::copyValues(const int32_t sourceTabIndex,
                                     const int32_t targetTabIndex)
{
    CaretAssertArrayIndex(m_tabValues,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          sourceTabIndex);
    CaretAssertArrayIndex(m_tabValues,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          targetTabIndex);
    
    m_tabValues[targetTabIndex] = m_tabValues[sourceTabIndex];
}


/**
 * @return The data value.
 *
 * @param displayGroup
 *     Display group.
 * @param tabIndex
 *     Tab index.
 */
float
DisplayPropertyDataFloat::getValue(const DisplayGroupEnum::Enum displayGroup,
               const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayGroupValues,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_tabValues,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_tabValues[tabIndex];
    }
    return m_displayGroupValues[displayGroup];
    
}

/**
 * Set all display groups and tabs to the given value.
 *
 * @param value
 *     The value.
 */
void
DisplayPropertyDataFloat::setAllValues(const float value)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_tabValues[i] = value;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayGroupValues[i] = value;
    }
}

/**
 * Set the data value.
 *
 * @param displayGroup
 *     Display group.
 * @param tabIndex
 *     Tab index.
 * @param value
 *     New value.
 */
void
DisplayPropertyDataFloat::setValue(const DisplayGroupEnum::Enum displayGroup,
              const int32_t tabIndex,
              const float value)
{
    CaretAssertArrayIndex(m_displayGroupValues,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_tabValues,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_tabValues[tabIndex] = value;
    }
    else {
        m_displayGroupValues[displayGroup] = value;
    }
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
DisplayPropertyDataFloat::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertyDataFloat",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
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
DisplayPropertyDataFloat::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
}

