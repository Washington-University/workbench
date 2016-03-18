
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

#define __DISPLAY_GROUP_AND_TAB_ITEM_HELPER_DECLARE__
#include "DisplayGroupAndTabItemHelper.h"
#undef __DISPLAY_GROUP_AND_TAB_ITEM_HELPER_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::DisplayGroupAndTabItemHelper 
 * \brief Helper for display group and tab selection items
 * \ingroup Scenes
 */

/**
 * Constructor.
 */
DisplayGroupAndTabItemHelper::DisplayGroupAndTabItemHelper()
: CaretObject()
{
    initializeNewInstance();
}

/**
 * Destructor.
 */
DisplayGroupAndTabItemHelper::~DisplayGroupAndTabItemHelper()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DisplayGroupAndTabItemHelper::DisplayGroupAndTabItemHelper(const DisplayGroupAndTabItemHelper& obj)
: CaretObject(obj)
{
    initializeNewInstance();
    
    this->copyHelperDisplayGroupAndTabItemHelper(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DisplayGroupAndTabItemHelper&
DisplayGroupAndTabItemHelper::operator=(const DisplayGroupAndTabItemHelper& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDisplayGroupAndTabItemHelper(obj);
    }
    return *this;    
}

/**
 * @return Parent.
 */
DisplayGroupAndTabItemInterface*
DisplayGroupAndTabItemHelper::getParent() const
{
    return m_parent;
}

/**
 * Set the parent.
 *
 * @param itemParent
 *     Parent.
 */
void
DisplayGroupAndTabItemHelper::setParent(DisplayGroupAndTabItemInterface* parent)
{
    m_parent = parent;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DisplayGroupAndTabItemHelper::copyHelperDisplayGroupAndTabItemHelper(const DisplayGroupAndTabItemHelper& obj)
{
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_selectedInDisplayGroup[i] = obj.m_selectedInDisplayGroup[i];
        m_expandedStatusInDisplayGroup[i] = obj.m_expandedStatusInDisplayGroup[i];
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedInTab[i] = m_selectedInTab[i];
        m_expandedStatusInTab[i] = m_expandedStatusInTab[i];
    }
}

/**
 * Clear members.
 */
void
DisplayGroupAndTabItemHelper::clearPrivate()
{
    m_parent = NULL;
    bool defaultExpandStatus = true;
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_selectedInDisplayGroup[i] = TriStateSelectionStatusEnum::Enum::SELECTED;
        m_expandedStatusInDisplayGroup[i] = defaultExpandStatus;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedInTab[i] = TriStateSelectionStatusEnum::SELECTED;
        m_expandedStatusInTab[i] = defaultExpandStatus;
    }
    
}

/**
 * Initialize new instance.
 */
void
DisplayGroupAndTabItemHelper::initializeNewInstance()
{
    clearPrivate();
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<TriStateSelectionStatusEnum,
                                 TriStateSelectionStatusEnum::Enum>("m_selectedInTab",
                                                m_selectedInTab);
    
    m_sceneAssistant->addTabIndexedBooleanArray("m_expandedStatusInTab",
                                                m_expandedStatusInTab);
    
    m_sceneAssistant->addArray<TriStateSelectionStatusEnum,
                               TriStateSelectionStatusEnum::Enum>("m_selectedInDisplayGroup",
                               m_selectedInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_selectedInDisplayGroup[0]);
    
    m_sceneAssistant->addArray("m_expandedStatusInDisplayGroup",
                               m_expandedStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_expandedStatusInDisplayGroup[0]);
}

/**
 * Get selection status.
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @return
 *    Selection status.
 */
TriStateSelectionStatusEnum::Enum
DisplayGroupAndTabItemHelper::getSelected(const DisplayGroupEnum::Enum displayGroup,
                                              const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(m_selectedInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_selectedInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_selectedInTab[tabIndex];
    }
    
    return m_selectedInDisplayGroup[displayIndex];
}

/**
 * Set the selected status of this item only.  It does not alter
 * the status of ancestors and children.
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @param status
 *    New selection status.
 */
void
DisplayGroupAndTabItemHelper::setSelected(const DisplayGroupEnum::Enum displayGroup,
                                               const int32_t tabIndex,
                                          const TriStateSelectionStatusEnum::Enum status)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(m_selectedInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_selectedInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_selectedInTab[tabIndex] = status;
    }
    else {
        m_selectedInDisplayGroup[displayIndex] = status;
    }
}

/**
 * Is this item expanded to display its children in the
 * selection controls?
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @return
 *    True if children should be visible, else false.
 */
bool
DisplayGroupAndTabItemHelper::isExpanded(const DisplayGroupEnum::Enum displayGroup,
                                              const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(m_expandedStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_expandedStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_expandedStatusInTab[tabIndex];
    }
    return m_expandedStatusInDisplayGroup[displayIndex];
}

/**
 * Set this item expanded to display its children in the
 * selection controls.
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @param expanded
 *    True if children should be visible, else false.
 */
void
DisplayGroupAndTabItemHelper::setExpanded(const DisplayGroupEnum::Enum displayGroup,
                                               const int32_t tabIndex,
                                               const bool expanded)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(m_expandedStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_expandedStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_expandedStatusInTab[tabIndex] = expanded;
    }
    else {
        m_expandedStatusInDisplayGroup[displayIndex] = expanded;
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
DisplayGroupAndTabItemHelper::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayGroupAndTabItemHelper",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
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
DisplayGroupAndTabItemHelper::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

