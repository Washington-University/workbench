
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

#define __GROUP_AND_NAME_HIERARCHY_ITEM_DECLARE__
#include "GroupAndNameHierarchyItem.h"
#undef __GROUP_AND_NAME_HIERARCHY_ITEM_DECLARE__

#include "AStringNaturalComparison.h"
#include "CaretAssert.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyName.h"
#include "GroupAndNameHierarchyUserInterface.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "TabDrawingInfo.h"

#include <algorithm>

using namespace caret;



/**
 * \class caret::GroupAndNameHierarchyItem
 * \brief Base class for items in a hierarchy tree.
 */

/**
 * Constructor.
 *
 * @param groupAndNameHierarchyUserInterface;
 *    Interface for files that use group and name hierarchy
 * @param itemType
 *    Type of this item.
 * @param name
 *    Name of the item.
 * @param idNumber
 *    Id number for the item.
 */
GroupAndNameHierarchyItem::GroupAndNameHierarchyItem(GroupAndNameHierarchyUserInterface* groupAndNameHierarchyUserInterface,
                                                     const ItemType itemType,
                                                     const AString& name,
                                                     const int32_t idNumber)
: CaretObject(),
m_groupAndNameHierarchyUserInterface(groupAndNameHierarchyUserInterface),
m_itemType(itemType),
m_name(name),
m_idNumber(idNumber),
m_parent(0)
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_iconRGBA[0] = 0.0;
    m_iconRGBA[1] = 0.0;
    m_iconRGBA[2] = 0.0;
    m_iconRGBA[3] = 0.0;

    clearPrivate();
    
    m_sceneAssistant->addTabIndexedBooleanArray("m_selectedInTab",
                                                m_selectedInTab);
    
    m_sceneAssistant->addTabIndexedBooleanArray("m_expandedStatusInTab",
                                                m_expandedStatusInTab);
    
    m_sceneAssistant->addArray("m_selectedInDisplayGroup",
                               m_selectedInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_selectedInDisplayGroup[0]);
    
    m_sceneAssistant->addArray("m_expandedStatusInDisplayGroup",
                               m_expandedStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_expandedStatusInDisplayGroup[0]);
}

/**
 * Destructor.
 */
GroupAndNameHierarchyItem::~GroupAndNameHierarchyItem()
{
    delete m_sceneAssistant;
    clearPrivate();
}

/**
 * Clear the contents of this class selector.
 */
void
GroupAndNameHierarchyItem::clear()
{
    clearPrivate();
}

/**
 * Clear the contents of this class selector.
 */
void
GroupAndNameHierarchyItem::clearPrivate()
{
    for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* item = *iter;
        item->m_parent = NULL;
        delete item;
    }
    m_children.clear();
    m_childrenNameIdMap.clear();
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_selectedInDisplayGroup[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedInTab[i] = true;
    }
    
    bool defaultExpandStatus = false;
    switch (m_itemType) {
        case ITEM_TYPE_NAME:
            break;
        case ITEM_TYPE_GROUP:
            break;
        case ITEM_TYPE_MODEL:
            defaultExpandStatus = true;
            break;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_expandedStatusInDisplayGroup[i] = defaultExpandStatus;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_expandedStatusInTab[i] = defaultExpandStatus;
    }
    
    m_counter = 0;
}

/**
 * @return The type of the item.
 */
GroupAndNameHierarchyItem::ItemType
GroupAndNameHierarchyItem::getItemType() const
{
    return m_itemType;
}

/**
 * @return The name of the item.
 */
AString
GroupAndNameHierarchyItem::getName() const
{
    return m_name;
}

/**
 * Set the name of the item.  This should only be used
 * to set the name of an item without a parent as names
 * are used when creating a hierarchy.
 *
 * @param name
 *     Name of item.
 */
void
GroupAndNameHierarchyItem::setName(const AString& name)
{
    m_name = name;
}

/**
 * @return The parent of this item.
 */
GroupAndNameHierarchyItem*
GroupAndNameHierarchyItem::getParent()
{
    return m_parent;
}

/**
 * @return The parent of this item.
 */
const GroupAndNameHierarchyItem*
GroupAndNameHierarchyItem::getParent() const
{
    return m_parent;
}

/**
 * @return The ancestors of this item.
 */
std::vector<GroupAndNameHierarchyItem*>
GroupAndNameHierarchyItem::getAncestors() const
{
    std::vector<GroupAndNameHierarchyItem*> ancestors;
    
    if (m_parent != NULL) {
        ancestors.push_back(m_parent);
        
        std::vector<GroupAndNameHierarchyItem*> parentsAncestors = m_parent->getAncestors();
        ancestors.insert(ancestors.end(),
                         parentsAncestors.begin(),
                         parentsAncestors.end());
    }
    
    return ancestors;
}

/**
 * @return The descendants of this item.
 */
std::vector<GroupAndNameHierarchyItem*>
GroupAndNameHierarchyItem::getDescendants() const
{
    std::vector<GroupAndNameHierarchyItem*> descendants;
    
    if ((getItemType() == GroupAndNameHierarchyItem::ITEM_TYPE_MODEL)
        || (m_parent != NULL)) {
        for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
             iter != m_children.end();
             iter++) {
            GroupAndNameHierarchyItem* child = *iter;
            descendants.push_back(child);
            
            std::vector<GroupAndNameHierarchyItem*> childDescendants = child->getDescendants();
            descendants.insert(descendants.end(),
                               childDescendants.begin(),
                               childDescendants.end());
        }
    }
    
    return descendants;
}

/**
 * @return The children of this item.
 */
std::vector<GroupAndNameHierarchyItem*>
GroupAndNameHierarchyItem::getChildren() const
{
    return m_children;
}

static bool
lessName(const GroupAndNameHierarchyItem* itemOne,
                   const GroupAndNameHierarchyItem* itemTwo)
{
    const int32_t result = AStringNaturalComparison::compare(itemOne->getName(),
                                                             itemTwo->getName());
    if (result < 0) {
        return true;
    }
    return false;
    
//    return (itemOne->getName() < itemTwo->getName());
}
/**
 * Sort the descendants by name
 */
void
GroupAndNameHierarchyItem::sortDescendantsByName()
{
    std::sort(m_children.begin(),
              m_children.end(),
              lessName);
    
    for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        child->sortDescendantsByName();
    }
}

/**
 * Get the child with the given name and ID number.
 *
 * @param name
 *     Name of child.
 * @return Child with the given name and ID number or NULL if no 
 *     child with the name and ID number.
 */
GroupAndNameHierarchyItem*
GroupAndNameHierarchyItem::getChildWithNameAndIdNumber(const AString& name,
                                                      const int32_t idNumber)
{
    ChildMapKey childMapKey(idNumber,
                            name);
    
    std::map<ChildMapKey, GroupAndNameHierarchyItem*>::iterator iter;
    iter = m_childrenNameIdMap.find(childMapKey);
    if (iter != m_childrenNameIdMap.end()) {
        GroupAndNameHierarchyItem* item = iter->second;
        return item;
    }
    return NULL;
}

/**
 * Add the given child to this item.
 *
 * @param child
 *    Child to add.
 */
void
GroupAndNameHierarchyItem::addChild(GroupAndNameHierarchyItem* child)
{
    CaretAssertMessage((getChildWithNameAndIdNumber(child->getName(), child->getIdNumber()) != NULL),
                       ("Child with name="
                        + child->getName()
                        + ", idNumber="
                        + AString::number(child->getIdNumber())
                        + " already is in item named="
                        + getName()));
    
    addChildPrivate(child);
}

/**
 * Add the given child to this item.
 *
 * @param child
 *    Child to add.
 */
void
GroupAndNameHierarchyItem::addChildPrivate(GroupAndNameHierarchyItem* child)
{
    child->m_parent = this;
    m_children.push_back(child);
    
    ChildMapKey childMapKey(child->getIdNumber(),
                            child->getName());
    m_childrenNameIdMap.insert(std::make_pair(childMapKey,
                                              child));
}

/**
 * Add a child with the given name and id number.
 * @param itemType
 *     Type of the item.
 * @param name
 *     Name of item.
 * @param idNumber
 *     ID Number for item.
 * @return If a child with the given name and id number exists,
 *     it is returned.  Otherwise, a new child with the given
 *     name and id number is created and returned.
 */
GroupAndNameHierarchyItem*
GroupAndNameHierarchyItem::addChild(const ItemType itemType,
                                    const AString& name,
                                    const int32_t idNumber)
{
    GroupAndNameHierarchyItem* child = getChildWithNameAndIdNumber(name,
                                                                  idNumber);
    if (child != NULL) {
        child->incrementCounter();
        return child;
    }
    
    switch (itemType) {
        case ITEM_TYPE_GROUP:
            child = new GroupAndNameHierarchyGroup(m_groupAndNameHierarchyUserInterface,
                                                   name,
                                                   idNumber);
            break;
        case ITEM_TYPE_NAME:
            child = new GroupAndNameHierarchyName(m_groupAndNameHierarchyUserInterface,
                                                  name,
                                                  idNumber);
            break;
        case ITEM_TYPE_MODEL:
            CaretAssertMessage(0,
                               "Model should never be a child");
            break;
    }
    
    child->incrementCounter();
    addChildPrivate(child);
    
    return child;
}

/**
 * Remove the given child from this item.  The child IS NOT deleted.
 *
 * @param child
 *    Child to remove.
 */
void
GroupAndNameHierarchyItem::removeChild(GroupAndNameHierarchyItem* child)
{
    std::vector<GroupAndNameHierarchyItem*>::iterator iter = std::find(m_children.begin(),
                                                                      m_children.end(),
                                                                      child);
    if (iter != m_children.end()) {
        child->m_parent = NULL;
        m_children.erase(iter);
    }
    
    for (std::map<ChildMapKey, GroupAndNameHierarchyItem*>::iterator mapIter = m_childrenNameIdMap.begin();
         mapIter != m_childrenNameIdMap.end();
         mapIter++) {
        GroupAndNameHierarchyItem* item = mapIter->second;
        if (item == child) {
            m_childrenNameIdMap.erase(mapIter);
            break;
        }
    }
}

/**
 * @return True if selected
 * @param tabDrawingInfo
 *    Info for drawing tab
 */
bool
GroupAndNameHierarchyItem::isSelected(const TabDrawingInfo& tabDrawingInfo) const
{
    return isSelected(tabDrawingInfo.getDisplayGroup(),
                      tabDrawingInfo.getTabIndex());
}

/**
 * Is this item selected?
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @return
 *    True if item is selected, else false.
 */
bool
GroupAndNameHierarchyItem::isSelected(const DisplayGroupEnum::Enum displayGroup,
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
 * Get the "check state" of an item.
 * @param tabDrawingInfo
 *    Info for drawing tab
 * @return
 *    CHECKED if this item and ALL of its children are selected.
 *    PARTIALLY_CHECKED if this item is selected and any of its
 *       children, but not all of its children, are selected.
 *    UNCHECKED if this item is not selected.
 */
GroupAndNameCheckStateEnum::Enum
GroupAndNameHierarchyItem::getCheckState(const TabDrawingInfo& tabDrawingInfo) const
{
    return getCheckState(tabDrawingInfo.getDisplayGroup(),
                         tabDrawingInfo.getTabIndex());
}

/**
 * Get the "check state" of an item.
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @return
 *    CHECKED if this item and ALL of its children are selected.
 *    PARTIALLY_CHECKED if this item is selected and any of its
 *       children, but not all of its children, are selected.
 *    UNCHECKED if this item is not selected.
 */
GroupAndNameCheckStateEnum::Enum
GroupAndNameHierarchyItem::getCheckState(const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex) const
{
    if (isSelected(displayGroup, tabIndex)) {
        int64_t numChildren = 0;
        int64_t numChildrenChecked = 0;
        
        for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
             iter != m_children.end();
             iter++) {
            numChildren++;
            
            GroupAndNameHierarchyItem* child = *iter;
            const GroupAndNameCheckStateEnum::Enum childStatus = child->getCheckState(displayGroup,
                                                                                             tabIndex);
            switch (childStatus) {
                case GroupAndNameCheckStateEnum::CHECKED:
                    numChildrenChecked++;
                    break;
                case GroupAndNameCheckStateEnum::PARTIALLY_CHECKED:
                    return GroupAndNameCheckStateEnum::PARTIALLY_CHECKED;
                    break;
                case GroupAndNameCheckStateEnum::UNCHECKED:
                    break;
            }
        }
        
        if (numChildrenChecked == numChildren) {
            return GroupAndNameCheckStateEnum::CHECKED;
        }
        else if (numChildrenChecked > 0) {
            return GroupAndNameCheckStateEnum::PARTIALLY_CHECKED;
        }
    }
    
    return GroupAndNameCheckStateEnum::UNCHECKED;
}

/**
 * Set the selected status of this item only.  It does not alter
 * the status of ancestors and children.
 *
 * @param tabDrawingInfo
 *    Info for drawing tab
 * @param status
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setSelected(const TabDrawingInfo& tabDrawingInfo,
                                       const bool status)
{
    setSelected(tabDrawingInfo.getDisplayGroup(),
                tabDrawingInfo.getTabIndex(),
                status);
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
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setSelected(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex,
                                      const bool status)
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
    
    if (m_groupAndNameHierarchyUserInterface != NULL) {
        m_groupAndNameHierarchyUserInterface->groupAndNameHierarchyItemStatusChanged();
    }
}

/**
 * Set the selected status for all of this item's descendants.
 *
 * @param tabDrawingInfo
 *    Info for drawing tab
 * @param status
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setDescendantsSelected(const TabDrawingInfo& tabDrawingInfo,
                                                  const bool status)
{
    setDescendantsSelected(tabDrawingInfo.getDisplayGroup(),
                           tabDrawingInfo.getTabIndex(),
                           status);
}

/**
 * Set the selected status for all of this item's descendants.
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @param status
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setDescendantsSelected(const DisplayGroupEnum::Enum displayGroup,
                                              const int32_t tabIndex,
                                              const bool status)
{
    for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        child->setSelected(displayGroup,
                           tabIndex,
                           status);
        child->setDescendantsSelected(displayGroup,
                                      tabIndex,
                                      status);
    }
}

/**
 * Set the selected status of this item's ancestor's (parent,
 * its parent, etc).
 *
 * @param tabDrawingInfo
 *    Info for drawing tab
 * @param status
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setAncestorsSelected(const TabDrawingInfo& tabDrawingInfo,
                                                const bool status)
{
    setAncestorsSelected(tabDrawingInfo.getDisplayGroup(),
                         tabDrawingInfo.getTabIndex(),
                         status);
}

/**
 * Set the selected status of this item's ancestor's (parent,
 * its parent, etc).
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @param status
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setAncestorsSelected(const DisplayGroupEnum::Enum displayGroup,
                                               const int32_t tabIndex,
                                               const bool status)
{
    if (m_parent != NULL) {
        m_parent->setSelected(displayGroup,
                              tabIndex,
                              status);
        m_parent->setAncestorsSelected(displayGroup,
                                       tabIndex,
                                       status);
    }
}

/**
 * Set the selected status of this item, its ancestors, and all
 * of its children.
 *
 * @param tabDrawingInfo
 *    Info for drawing tab
 * @param status
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setSelfAncestorsAndDescendantsSelected(const TabDrawingInfo& tabDrawingInfo,
                                                                  const bool status)
{
    setSelfAncestorsAndDescendantsSelected(tabDrawingInfo.getDisplayGroup(),
                                           tabDrawingInfo.getTabIndex(),
                                           status);
}

/**
 * Set the selected status of this item, its ancestors, and all
 * of its children.
 *
 * @param displayGroup
 *    The display group in which the item is controlled/viewed.
 * @param tabIndex
 *    Index of browser tab in which item is controlled/viewed.
 * @param status
 *    True if item is selected, else false.
 */
void
GroupAndNameHierarchyItem::setSelfAncestorsAndDescendantsSelected(const DisplayGroupEnum::Enum displayGroup,
                                                              const int32_t tabIndex,
                                                              const bool status)
{
    setSelected(displayGroup,
                tabIndex,
                status);
    
    setAncestorsSelected(displayGroup,
                         tabIndex,
                         status);
    
    setDescendantsSelected(displayGroup,
                           tabIndex,
                           status);
}

/**
 * Get the RGBA color for an Icon that is displayed
 * in the selection control for this item.
 *
 * @return
 *     Pointer to the Red, Green, Blue, and Alpha
 *         color components for this item's icon.
 *     If no icon is to be displayed, the alpha component is zero.
 */
const float*
GroupAndNameHierarchyItem::getIconColorRGBA() const
{
    return m_iconRGBA;
}

/**
 * Set the RGBA color components for an icon displayed in the
 * selection control for this item.
 *
 * @param rgba
 *     The Red, Green, Blue, and Alpha color components.
 *     If no icon is to be displayed, the alpha component is zero.
 */
void
GroupAndNameHierarchyItem::setIconColorRGBA(const float rgba[4])
{
    m_iconRGBA[0] = rgba[0];
    m_iconRGBA[1] = rgba[1];
    m_iconRGBA[2] = rgba[2];
    m_iconRGBA[3] = rgba[3];

    if (m_groupAndNameHierarchyUserInterface != NULL) {
        m_groupAndNameHierarchyUserInterface->groupAndNameHierarchyItemStatusChanged();
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
GroupAndNameHierarchyItem::isExpandedToDisplayChildren(const DisplayGroupEnum::Enum displayGroup,
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
GroupAndNameHierarchyItem::setExpandedToDisplayChildren(const DisplayGroupEnum::Enum displayGroup,
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
 * Copy the selections from one tab to another tab.
 * Also copies selections in all descendants.
 *
 * @param sourceTabIndex
 *     Index of source tab (copy "from")
 * @param targetTabIndex
 *     Index of target tab (copy "to")
 */
void
GroupAndNameHierarchyItem::copySelections(const int32_t sourceTabIndex,
                                         const int32_t targetTabIndex)
{
    m_selectedInTab[targetTabIndex] = m_selectedInTab[sourceTabIndex];
    m_expandedStatusInTab[targetTabIndex] = m_expandedStatusInTab[sourceTabIndex];

    AString indent;
    switch (m_itemType) {
        case ITEM_TYPE_GROUP:
            indent = "   ";
            break;
        case ITEM_TYPE_MODEL:
            break;
        case ITEM_TYPE_NAME:
            indent = "      ";
            break;
    }
    
    for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        child->copySelections(sourceTabIndex,
                              targetTabIndex);
    }
}

/**
 * @return The Id Number.
 */
int32_t
GroupAndNameHierarchyItem::getIdNumber() const
{
    return m_idNumber;
}

/**
 * Clear the counter.  Also clears counters in descendants.
 */
void
GroupAndNameHierarchyItem::clearCounters()
{
    m_counter = 0;
    
    for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        child->clearCounters();
    }
}

/**
 * Increment the counter.
 */
void
GroupAndNameHierarchyItem::incrementCounter()
{
    m_counter++;
}

/**
 * @return The value of the counter.
 */
int32_t
GroupAndNameHierarchyItem::getCounter() const
{
    return m_counter;
}

/**
 * Remove all descendants with counters equal to zero.
 */
void
GroupAndNameHierarchyItem::removeDescendantsWithCountersEqualToZeros()
{
    /*
     * Process all descendants.
     */
    for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        child->removeDescendantsWithCountersEqualToZeros();
    }
    
    /*
     * Find children with zero counters indicating item not used.
     */
    std::vector<GroupAndNameHierarchyItem*> childrenWithCountGreaterThanZero;
    std::vector<GroupAndNameHierarchyItem*> childrenWithCountEqualToZero;
    for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        if (child->getCounter() > 0) {
            childrenWithCountGreaterThanZero.push_back(child);
        }
        else {
            childrenWithCountEqualToZero.push_back(child);
        }
    }
    
    /*
     * If no children with zero counter, return.
     */
    if (childrenWithCountEqualToZero.empty()) {
        return;
    }
    
    /*
     * Remove children with zero counters
     */
    for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = childrenWithCountEqualToZero.begin();
         iter != childrenWithCountEqualToZero.end();
         iter++) {
        GroupAndNameHierarchyItem* item = *iter;
        item->m_parent = NULL;
        delete item;
    }
    
    /*
     * Clear children
     */
    m_children.clear();
    m_childrenNameIdMap.clear();
    
    /*
     * Read children so maps properly created.
     */
    for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = childrenWithCountGreaterThanZero.begin();
         iter != childrenWithCountGreaterThanZero.end();
         iter++) {
        addChildPrivate(*iter);
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
GroupAndNameHierarchyItem::toString() const
{
    AString info = (CaretObject::toString()
                    + "\n   name=" + m_name
                    + ", type" + AString::number(m_itemType)
                    + ", idNumber=" + AString::number(m_idNumber)
                    + ", counter=" + AString::number(m_counter)
                    + "\n");
    
    AString childInfo;
    for (std::vector<GroupAndNameHierarchyItem*>::const_iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        childInfo += child->toString();
    }
    if (childInfo.isEmpty() == false) {
        childInfo = childInfo.replace("\n", "\n   ");
    }
    info += childInfo;
    
    return info;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
GroupAndNameHierarchyItem::saveToScene(const SceneAttributes* sceneAttributes,
                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "GroupAndNameHierarchyItem",
                                            1);
    
    sceneClass->addString("m_name", m_name);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        sceneClass->addClass(child->saveToScene(sceneAttributes,
                                                child->getName()));
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
GroupAndNameHierarchyItem::restoreFromScene(const SceneAttributes* sceneAttributes,
                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    sceneClass->getStringValue("m_name"); // prevents "failed to restore"
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyItem* child = *iter;
        child->restoreFromScene(sceneAttributes,
                                sceneClass->getClass(child->getName()));
    }
}

