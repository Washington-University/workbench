
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __CLASS_AND_NAME_HIERARCHY_GROUP_DECLARE__
#include "GroupAndNameHierarchyGroup.h"
#undef __CLASS_AND_NAME_HIERARCHY_GROUP_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GroupAndNameHierarchyGroup
 * \brief Maintains selection of a class and name in each 'DisplayGroupEnum'.
 */

/**
 * Constructor.
 * @param name
 *    The name.
 * @param key
 *    Key assigned to the name.
 */
GroupAndNameHierarchyGroup::GroupAndNameHierarchyGroup(const AString& name,
                                                                                 const int32_t key)
: GroupAndNameHierarchyName(name,
                                                       key)
{
    this->clearPrivate();
}

/**
 * Destructor.
 */
GroupAndNameHierarchyGroup::~GroupAndNameHierarchyGroup()
{
    this->clearPrivate();
}

/**
 * Copy the selections from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which selections are copied.
 * @param targetTabIndex
 *    Index of tab to which selections are copied.
 */
void
GroupAndNameHierarchyGroup::copySelections(const int32_t sourceTabIndex,
                                                                      const int32_t targetTabIndex)
{
    GroupAndNameHierarchyName::copySelections(sourceTabIndex,
                                             targetTabIndex);
    this->expandedStatusInTab[targetTabIndex] = this->expandedStatusInTab[sourceTabIndex];
    
    for (std::map<AString, GroupAndNameHierarchyName*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        GroupAndNameHierarchyName* ns = iter->second;
        if (ns != NULL) {
            ns->copySelections(sourceTabIndex,
                               targetTabIndex);
        }
    }
}

/**
 * Clear the contents of this class selector.
 */
void
GroupAndNameHierarchyGroup::clear()
{
    clearPrivate();
}

/**
 * Clear the contents of this class selector.
 */
void
GroupAndNameHierarchyGroup::clearPrivate()
{
    for (std::map<AString, GroupAndNameHierarchyName*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        GroupAndNameHierarchyName* ns = iter->second;
        if (ns != NULL) {
            delete ns;
        }
    }
    this->nameToNameSelectorMap.clear();
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        this->expandedStatusInDisplayGroup[i] = false;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->expandedStatusInTab[i] = false;
    }
}

/**
 * Get the selection status for the given display group and tab.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return
 *    The check state.  
 */
GroupAndNameCheckStateEnum::Enum
GroupAndNameHierarchyGroup::getSelected(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const
{
    GroupAndNameCheckStateEnum::Enum myStatus = GroupAndNameHierarchyName::getSelected(displayGroup,
                                                                                       tabIndex);
    if (myStatus == GroupAndNameCheckStateEnum::UNCHECKED) {
        return myStatus;
    }
    
    int64_t numChildren = 0;
    int64_t numChildrenChecked = 0;
    
    for (std::map<AString, GroupAndNameHierarchyName*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        numChildren++;
        
        const GroupAndNameHierarchyName* nameSelector = iter->second;
        const GroupAndNameCheckStateEnum::Enum childStatus = nameSelector->getSelected(displayGroup,
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
    
    return GroupAndNameCheckStateEnum::UNCHECKED;
}

/**
 * Set class seletion status for the given display group.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param status
 *    New selection status.
 */
void
GroupAndNameHierarchyGroup::setSelected(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const GroupAndNameCheckStateEnum::Enum status)
{
    GroupAndNameHierarchyName::setSelected(displayGroup,
                                           tabIndex,
                                           status);
    
//    /*
//     * Turning class on?
//     */
//    if (status) {
//        /*
//         * If NO children are selected
//         */
//        if (isAnySelected(displayGroup, tabIndex) == false) {
//            for (std::map<AString, GroupAndNameHierarchyName*>::const_iterator iter = this->nameToNameSelectorMap.begin();
//                 iter != this->nameToNameSelectorMap.end();
//                 iter++) {
//                GroupAndNameHierarchyName* nameSelector = iter->second;
//                nameSelector->setSelected(displayGroup,
//                                          tabIndex,
//                                          true);
//            }
//        }
//    }
}


/**
 * Set the selection status for this class and all of its child names
 * and for all display groups.
 *
 * @param status
 *    New selection status.
 */
void
GroupAndNameHierarchyGroup::setAllSelected(const bool status)
{
    GroupAndNameCheckStateEnum::Enum checkState = GroupAndNameCheckStateEnum::UNCHECKED;
    if (status) {
        checkState = GroupAndNameCheckStateEnum::CHECKED;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        DisplayGroupEnum::Enum group = (DisplayGroupEnum::Enum)i;
        if (group == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
            for (int32_t j = 0; j < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; j++) {
                this->setSelected(group, j, checkState);
            }
        }
        else {
            this->setSelected(group, -1, checkState);
        }
    }
    
    for (std::map<AString, GroupAndNameHierarchyName*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        GroupAndNameHierarchyName* ns = iter->second;
        if (ns != NULL) {
            for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
                DisplayGroupEnum::Enum group = (DisplayGroupEnum::Enum)i;
                if (group == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
                    for (int32_t j = 0; j < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; j++) {
                        ns->setSelected(group, j, checkState);
                    }
                }
                else {
                    ns->setSelected(group, -1, checkState);
                }
            }
        }
    }
}

/**
 * Set the selection status of this class and its names for the display group/tab.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param selectionStatus
 *    New selection status for class and its names.
 */
void
GroupAndNameHierarchyGroup::setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                                                                      const int32_t tabIndex,
                                                                      const bool selectionStatus)
{
    GroupAndNameCheckStateEnum::Enum checkState = GroupAndNameCheckStateEnum::UNCHECKED;
    if (selectionStatus) {
        checkState = GroupAndNameCheckStateEnum::CHECKED;
    }
    
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    setSelected(displayGroup,
                tabIndex,
                checkState);
    
    for (std::map<AString, GroupAndNameHierarchyName*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        GroupAndNameHierarchyName* nameSelector = iter->second;
        nameSelector->setSelected(displayGroup,
                                  tabIndex,
                                  checkState);
    }
}

/**
 * Is this class and all of its child names selected?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return
 *    true if everything is selected, else false.
 */
bool
GroupAndNameHierarchyGroup::isAllSelected(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex) const
{
    GroupAndNameCheckStateEnum::Enum checkState = getSelected(displayGroup,
                                                              tabIndex);
    if (checkState == GroupAndNameCheckStateEnum::CHECKED) {
        return true;
    }
    return false;
}

/**
 * Is this class and ANY of its child names selected?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return
 *    true if everything is selected, else false.
 */
bool
GroupAndNameHierarchyGroup::isAnySelected(const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex) const
{
    GroupAndNameCheckStateEnum::Enum checkState = getSelected(displayGroup,
                                                              tabIndex);
    if ((checkState == GroupAndNameCheckStateEnum::CHECKED)
        && (checkState == GroupAndNameCheckStateEnum::CHECKED)) {
        return true;
    }
    return false;
}

/**
 * @return All keys for the names.
 */
std::vector<int32_t>
GroupAndNameHierarchyGroup::getAllNameKeysSortedByName() const
{
    std::vector<int32_t> allKeys;
    
    for (std::map<AString, GroupAndNameHierarchyName*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        GroupAndNameHierarchyName* ns = iter->second;
        allKeys.push_back(ns->getKey());
    }
    
    return allKeys;
}

/**
 * Add a name selector to the map that maps a name to a name selector.
 * @param name
 *     Name of selector.
 * @param nameSelector
 *     The name selector.
 */
void
GroupAndNameHierarchyGroup::addToNameSelectorMap(const AString& name,
                                                                            GroupAndNameHierarchyName* nameSelector)
{
    CaretAssert(nameSelector);
    this->nameToNameSelectorMap.insert(std::make_pair(name, nameSelector));
}

/**
 * Remove a name selector from the map that maps names to name selectors.
 * Caller will delete the name selector so, in here, just remove it from
 * the map.
 *
 * @param nameSelector
 *     Name selector that is to be removed.
 */
void
GroupAndNameHierarchyGroup::removeNameSelector(GroupAndNameHierarchyName* nameSelector)
{
    for (std::map<AString, GroupAndNameHierarchyName*>::iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        if (nameSelector == iter->second) {
            iter->second = NULL;
            this->nameToNameSelectorMap.erase(iter);
            break;
        }
    }
}


/**
 * Get the name selector for the name with the given name.
 * @param name
 *    The name.
 * @return
 *    Name selector for the given name or NULL if there
 *    is no name selector with the given name.
 */
GroupAndNameHierarchyName*
GroupAndNameHierarchyGroup::getNameSelectorWithName(const AString& name)
{
    std::map<AString, GroupAndNameHierarchyName*>::iterator iter = this->nameToNameSelectorMap.find(name);
    if (iter != this->nameToNameSelectorMap.end()) {
        GroupAndNameHierarchyName* ns = iter->second;
        return ns;
    }
    
    return NULL;
}


/**
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return The expanded status.
 */
bool
GroupAndNameHierarchyGroup::isExpanded(const DisplayGroupEnum::Enum displayGroup,
                                                                  const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->expandedStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->expandedStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return this->expandedStatusInTab[tabIndex];
    }
    return this->expandedStatusInDisplayGroup[displayIndex];
}

/**
 * Set the expanded status.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param expanded
 *    New expaned status.
 */
void
GroupAndNameHierarchyGroup::setExpanded(const DisplayGroupEnum::Enum displayGroup,
                                                                   const int32_t tabIndex,
                                                                   const bool expanded)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->expandedStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->expandedStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        this->expandedStatusInTab[tabIndex] = expanded;
    }
    else {
        this->expandedStatusInDisplayGroup[displayIndex] = expanded;
    }
}
