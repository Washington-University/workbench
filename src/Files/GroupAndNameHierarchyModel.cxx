
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

#define __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__
#include "GroupAndNameHierarchyModel.h"
#undef __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__

#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyGroupGeneratedKey.h"
#include "GroupAndNameHierarchyGroupUserKey.h"
#include "GroupAndNameHierarchyName.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"

using namespace caret;


    
/**
 * \class caret::GroupAndNameHierarchySelection 
 * \brief Maintains a 'group' and 'name' hierarchy for selection.
 *
 * Each group maps to one or more names.  Identical names
 * may be children of more than one group.  However, each
 * group holds its child names independently from the children
 * of all other groups.  
 *
 * Note: Two containers are used to hold the data.  One,
 * a vector, maps group keys to groups and a second, a map
 * group names to groups.  
 * use 'maps'.  A map is typically constructed using a
 * balanced tree so retrieval can be fast.  However, adding
 * or removing items may be slow due to tree rebalancing.  
 * As a result, unused groups and children names are only 
 * removed when the entire instance is cleared (via clear())
 * or by calling removeUnusedNamesAndGroupes().  
 *
 * Each group or name supports a 'count'.  If the for a group 
 * or name is zero, that indicates that the item is unused.
 * 
 * Attributes are available for every tab and also a 
 * few 'display groups'.  A number of methods in this group accept 
 * both display group and tab index parameters.  When the display 
 * group is set to 'Tab', the tab index is used meaning that the
 * attribute requeted/sent is for use with a specifc tab.  For an
 * other display group value, the attribute is for a display group
 * and the tab index is ignored.
 */

/**
 * Constructor.
 */
GroupAndNameHierarchyModel::GroupAndNameHierarchyModel()
: CaretObject()
{
    this->clear();
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        this->selectionStatusInDisplayGroup[i] = true;
        this->expandedStatusInDisplayGroup[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->selectionStatusInTab[i] = true;
        this->expandedStatusInTab[i] = true;
    }
}

/**
 * Destructor.
 */
GroupAndNameHierarchyModel::~GroupAndNameHierarchyModel()
{
    this->clear();
}

/**
 * Clear the group/name hierarchy.
 */
void 
GroupAndNameHierarchyModel::clear()
{
    /** 
     * While both maps point to NameDisplayGroupSelectors, both maps point
     * to the same selectors so only need to delete them one time.
     */
    for (std::vector<GroupAndNameHierarchyGroup*>::iterator iter = this->keyToGroupNameSelectorVector.begin();
         iter != this->keyToGroupNameSelectorVector.end();
         iter++) {
        GroupAndNameHierarchyGroup* cs = *iter;
        if (cs != NULL) {
            delete cs;
        }
    }
    
    this->keyToGroupNameSelectorVector.clear();
    this->groupNameToGroupSelectorMap.clear();
    
    this->availableGroupKeys.clear();        
}

/**
 * Copy the group and names selections from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which selections are copied.
 * @param targetTabIndex
 *    Index of tab to which selections are copied.
 */
void 
GroupAndNameHierarchyModel::copyGroupNameAndHierarchy(const int32_t sourceTabIndex,
                                                      const int32_t targetTabIndex)
{
    this->selectionStatusInTab[targetTabIndex] = this->selectionStatusInTab[sourceTabIndex];
    this->expandedStatusInTab[targetTabIndex]  = this->expandedStatusInTab[sourceTabIndex];
}

/**
 * Set the selected status for EVERYTHING.
 * @param status
 *    The selection status.
 */
void 
GroupAndNameHierarchyModel::setAllSelected(const bool status)
{
    const int32_t numberOfGroupKeys = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
    for (int32_t groupKey = 0; groupKey < numberOfGroupKeys; groupKey++) {
        GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[groupKey];
        if (cs != NULL) {
            cs->setAllSelected(status);
        }
    }
}

/**
 * Set the selection status of this hierarchy model for the display group/tab.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param selectionStatus
 *    New selection status.
 */
void 
GroupAndNameHierarchyModel::setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const bool selectionStatus)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectionStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        this->selectionStatusInTab[tabIndex] = selectionStatus;
    }
    else {
        this->selectionStatusInDisplayGroup[displayIndex] = selectionStatus;
    }
    
    for (std::vector<GroupAndNameHierarchyGroup*>::iterator groupIterator = keyToGroupNameSelectorVector.begin();
         groupIterator != keyToGroupNameSelectorVector.end();
         groupIterator++) {
        GroupAndNameHierarchyGroup* groupSelector = *groupIterator;
        groupSelector->setAllSelected(displayGroup,
                                      tabIndex,
                                      selectionStatus);
    }
}

/**
 * Update this group hierarchy with the border names
 * and classes.
 *
 * @param borderFile
 *    The border file from which classes and names are from.
 * @parm forceUpdate
 *    If true, force an update.
 */
void
GroupAndNameHierarchyModel::update(BorderFile* borderFile,
                                   const bool forceUpdate)
{
    bool needToGenerateKeys = forceUpdate;
    
    this->name = borderFile->getFileNameNoPath();

    const int32_t numBorders = borderFile->getNumberOfBorders();
    if (needToGenerateKeys == false) {
        for (int32_t i = 0; i < numBorders; i++) {
            const Border* border = borderFile->getBorder(i);
            if (border->isSelectionClassOrNameModified()) {
                needToGenerateKeys = true;
            }
        }
    }
    
    if (needToGenerateKeys) {
        /*
         * Names for missing group names or border names.
         */
        const AString missingGroupName = "NoGroup";
        const AString missingBorderName = "NoName";
        
        /*
         * Reset the counts for all group and children names.
         */
        const int32_t numberOfGroupKeys = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
        for (int32_t groupKey = 0; groupKey < numberOfGroupKeys; groupKey++) {
            GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[groupKey];
            if (cs != NULL) {
                cs->clearAllNameCounters();
            }
        }
                
        /*
         * Update with all borders.
         */
        for (int32_t i = 0; i < numBorders; i++) {
            Border* border = borderFile->getBorder(i);
            
            /*
             * Get the group.  If it is empty, use the default name.
             */
            AString theGroupName = border->getClassName();
            if (theGroupName.isEmpty()) {
                theGroupName = missingGroupName;
            }
            
            /*
             * Get the name.
             */
            AString name = border->getName();
            if (name.isEmpty()) {
                name = missingBorderName;
            }
            
            /*
             * Adding border class and name will set the group name keys.
             */
            int32_t groupKey = -1;
            int32_t nameKey = -1;
            this->addName(theGroupName,
                          name,
                          groupKey,
                          nameKey);
            
            /*
             * Update keys used by the border.
             */
            border->setSelectionClassAndNameKeys(groupKey,
                                                 nameKey);
        }
    }
}

/**
 * Remove any unused names and groups.
 * @param borderFile
 *    Border file that contains names and groups.
 */
void 
GroupAndNameHierarchyModel::removeUnusedNamesAndGroups(BorderFile* borderFile)
{
    /*
     * Update with latest data.
     */ 
    this->update(borderFile,
                 true);
    
    /*
     * Remove unused groups.
     */
    const int32_t numberOfGroupes = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
    for (int32_t groupKey = 0; groupKey < numberOfGroupes; groupKey++) {
        GroupAndNameHierarchyGroup* groupSelector = this->keyToGroupNameSelectorVector[groupKey];
        if (groupSelector != NULL) {
            groupSelector->removeNamesWithCountersEqualZero();
            if (groupSelector->getNumberOfNamesWithCountersGreaterThanZero() <= 0) {
                for (std::map<AString, GroupAndNameHierarchyGroup*>::iterator iter = this->groupNameToGroupSelectorMap.begin();
                     iter != this->groupNameToGroupSelectorMap.end();
                     iter++) {
                    if (groupSelector == iter->second) {
                        this->groupNameToGroupSelectorMap.erase(iter);
                        break;
                    }
                }
                
                this->availableGroupKeys.push_front(groupKey);
                delete groupSelector;
                this->keyToGroupNameSelectorVector[groupKey] = NULL;
            }
        }
    }
}

/**
 * Update this group hierarchy with the label names
 * and maps (as group).  Groupes and names are done
 * differently for LabelFiles.  Use the map index as
 * the group key and the label index as the name key.
 *
 * @param labelFile
 *    The label file from which groups (map) and names are from.
 * @parm forceUpdate
 *    If true, force an update.
 */
void
GroupAndNameHierarchyModel::update(LabelFile* labelFile,
                                   const bool forceUpdate)
{
    bool needToGenerateKeys = forceUpdate;
    
    this->name = labelFile->getFileNameNoPath();

    if (needToGenerateKeys == false) {
        /*
         * Check to see if any group (map) names have changed.
         */
        const int32_t numGroupes = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
        if (labelFile->getNumberOfMaps()
            != numGroupes) {
            needToGenerateKeys = true;
        }
        else {
            for (int32_t i = 0; i < numGroupes; i++) {
                GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[i];
                if (cs != NULL) {
                    if (labelFile->getMapName(i) != cs->getName()) {
                        needToGenerateKeys = true;
                        break;
                    }
                }
                else {
                    needToGenerateKeys = true;
                    break;
                }
            }
        }
    }
    
    if (needToGenerateKeys) {
        /*
         * Clear everything
         */
        this->clear();
        
        /*
         * Names for missing group names or foci names.
         */
        const AString missingGroupName = "NoGroup";
        const AString missingName = "NoName";
        
        /*
         * Update with labels from maps
         */
        const int32_t numMaps = labelFile->getNumberOfMaps();
        for (int32_t iMap = 0; iMap < numMaps; iMap++) {
            /*
             * Get the group.  If it is empty, use the default name.
             */
            AString theGroupName = labelFile->getMapName(iMap);
            if (theGroupName.isEmpty()) {
                theGroupName = missingGroupName;
            }
            
            /*
             * Create the group
             */
            GroupAndNameHierarchyGroupUserKey* groupSelector = new GroupAndNameHierarchyGroupUserKey(theGroupName,
                                                                                                   iMap);
//            const int32_t numGroupes = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
//            CaretAssert(numGroupes == iMap);
//            this->keyToGroupNameSelectorVector.push_back(groupSelector);

            /*
             * Get indices of labels used in this map
             */
            std::vector<int32_t> labelKeys = labelFile->getUniqueLabelKeysUsedInMap(iMap);
            
            const int32_t numLabelKeys = static_cast<int32_t>(labelKeys.size());
            for (int32_t iLabel = 0; iLabel < numLabelKeys; iLabel++) {
                const int32_t labelKey = labelKeys[iLabel];
                AString labelName = labelFile->getLabelTable()->getLabelName(labelKey);
                if (labelName.isEmpty()) {
                    labelName = missingName;
                }
                
                groupSelector->addNameWithKey(labelName,
                                              labelKey);
            }
            
            this->addGroup(groupSelector);
        }
    }
    
    
//    std::cout << "LABELS: " << qPrintable(this->toString()) << std::endl;
}

/**
 * Update this group hierarchy with the foci names
 * and groups.
 *
 * @param fociFile
 *    The foci file from which classes and names are from.
 * @parm forceUpdate
 *    If true, force an update.
 */
void 
GroupAndNameHierarchyModel::update(FociFile* fociFile,
                                   const bool forceUpdate)
{
    bool needToGenerateKeys = forceUpdate;
    
    this->name = fociFile->getFileNameNoPath();
    
    const int32_t numFoci = fociFile->getNumberOfFoci();
    if (needToGenerateKeys == false) {
        for (int32_t i = 0; i < numFoci; i++) {
            const Focus* focus = fociFile->getFocus(i);
            if (focus->isSelectionClassOrNameModified()) {
                needToGenerateKeys = true;
            }
        }
    }
    
    if (needToGenerateKeys) {
        /*
         * Names for missing group names or foci names.
         */
        const AString missingGroupName = "NoGroup";
        const AString missingName = "NoName";
        
        /*
         * Reset the counts for all group and children names.
         */
        const int32_t numberOfGroupKeys = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
        for (int32_t groupKey = 0; groupKey < numberOfGroupKeys; groupKey++) {
            GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[groupKey];
            if (cs != NULL) {
                cs->clearAllNameCounters();
            }
        }
        
        /*
         * Update with all foci.
         */
        for (int32_t i = 0; i < numFoci; i++) {
            Focus* focus = fociFile->getFocus(i);
            
            /*
             * Get the group.  If it is empty, use the default name.
             */
            AString theGroupName = focus->getClassName();
            if (theGroupName.isEmpty()) {
                theGroupName = missingGroupName;
            }
            
            /*
             * Get the name.
             */
            AString name = focus->getName();
            if (name.isEmpty()) {
                name = missingName;
            }
            
            /*
             * Adding focus group and name will set the group name keys.
             */
            int32_t groupKey = -1;
            int32_t nameKey = -1;
            this->addName(theGroupName,
                          name,
                          groupKey,
                          nameKey);
            
            /*
             * Update keys used by the focus.
             */
            focus->setSelectionClassAndNameKeys(groupKey,
                                                 nameKey);
        }
    }
}

/**
 * Remove any unused names and groups.
 * @param fociFile
 *    Foci file that contains names and groups.
 */
void 
GroupAndNameHierarchyModel::removeUnusedNamesAndGroups(FociFile* fociFile)
{
    /*
     * Update with latest data.
     */ 
    this->update(fociFile,
                 true);
    
    /*
     * Remove unused groups.
     */
    const int32_t numberOfGroupes = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
    for (int32_t groupKey = 0; groupKey < numberOfGroupes; groupKey++) {
        GroupAndNameHierarchyGroup* groupSelector = this->keyToGroupNameSelectorVector[groupKey];
        if (groupSelector != NULL) {
            groupSelector->removeNamesWithCountersEqualZero();
            if (groupSelector->getNumberOfNamesWithCountersGreaterThanZero() <= 0) {
                for (std::map<AString, GroupAndNameHierarchyGroup*>::iterator iter = this->groupNameToGroupSelectorMap.begin();
                     iter != this->groupNameToGroupSelectorMap.end();
                     iter++) {
                    if (groupSelector == iter->second) {
                        this->groupNameToGroupSelectorMap.erase(iter);
                        break;
                    }
                }
                
                this->availableGroupKeys.push_front(groupKey);
                delete groupSelector;
                this->keyToGroupNameSelectorVector[groupKey] = NULL;
            }
        }
    }
}

/**
 * Is the group valid?  Valid if group has at least one child
 * with a count attribute greater than zero (it is used).
 * @return true if group is valid, else false.
 */
bool 
GroupAndNameHierarchyModel::isGroupValid(const int32_t groupKey) const
{
    if ((groupKey >= 0) 
        && (groupKey < static_cast<int32_t>(this->keyToGroupNameSelectorVector.size()))) {
        GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[groupKey];
        if (cs != NULL) {
            return true;
        }
    }
    
    return false;
}

/**
 * @return  A vector with all group keys.
 */
std::vector<int32_t> 
GroupAndNameHierarchyModel::getAllGroupKeysSortedByName() const
{
    std::vector<int32_t> groupKeys;
    
    for (std::map<AString, GroupAndNameHierarchyGroup*>::const_iterator iter = this->groupNameToGroupSelectorMap.begin();
         iter != this->groupNameToGroupSelectorMap.end();
         iter++) {
        const GroupAndNameHierarchyGroup* cs = iter->second;
        if (cs != NULL) {
            groupKeys.push_back(cs->getKey());
        }
    }
    
    return groupKeys;
}

/**
 * @return A string containing a description
 * of the contents of this group and name
 * hierarchy.
 */
AString 
GroupAndNameHierarchyModel::toString() const
{
    AString text;
    text.reserve(10000);
    
    text += ("Hierarchy Name: "
             + this->name
             + "\n");
    for (std::map<AString, GroupAndNameHierarchyGroup*>::const_iterator iter = this->groupNameToGroupSelectorMap.begin();
         iter != this->groupNameToGroupSelectorMap.end();
         iter++) {
        const GroupAndNameHierarchyGroup* cs = iter->second;
        if (cs != NULL) {
            const AString groupName = cs->getName();
            const int32_t groupKey = cs->getKey();
            const int32_t groupCount = cs->getCounter();
            
            text += ("   Group Key/Count/Name for "
                     + AString::number(groupKey)
                     + ", "
                     + AString::number(groupCount)
                     + ": "
                     + groupName
                     + "\n");
            
            const std::vector<int32_t> allNameKeys = cs->getAllNameKeysSortedByName();
            for (std::vector<int32_t>::const_iterator nameIter = allNameKeys.begin();
                 nameIter != allNameKeys.end();
                 nameIter++) {
                const GroupAndNameHierarchyName* ns = cs->getNameSelectorWithKey(*nameIter);
                const AString name = ns->getName();
                const int32_t nameKey = ns->getKey();
                const int32_t nameCount = ns->getCounter();
                text += ("      Key/Count/Name: " 
                         + AString::number(nameKey)
                         + " "
                         + AString::number(nameCount)
                         + " "
                         + name
                         + "\n");
            }
        }
    }
    
    return text;
}

/**
 * @return  Name of this model.
 */
AString 
GroupAndNameHierarchyModel::getName() const
{
    return this->name;
}

/**
 * @return Is this hierarchy selected?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 */
bool 
GroupAndNameHierarchyModel::isSelected(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectionStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        return this->selectionStatusInTab[tabIndex];
    }
    return this->selectionStatusInDisplayGroup[displayIndex];
}

/**
 * Set the selection status of this hierarchy model.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param selectionStatus
 *    New selection status.
 */
void 
GroupAndNameHierarchyModel::setSelected(const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        const bool selectionStatus)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectionStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        this->selectionStatusInTab[tabIndex] = selectionStatus;
    }
    else {
        this->selectionStatusInDisplayGroup[displayIndex] = selectionStatus;
    }
}

/**
 * Get the group selector for the given group name.
 * @param groupName
 *    The key of the desired group selector.
 * @return The group selector for the key or NULL if no group
 *         selector with the given key.
 */
GroupAndNameHierarchyGroup* 
GroupAndNameHierarchyModel::getGroupSelectorForGroupName(const AString& groupName)
{
    GroupAndNameHierarchyGroup* groupSelector = NULL;

    std::map<AString, GroupAndNameHierarchyGroup*>::iterator iter = this->groupNameToGroupSelectorMap.find(groupName);
    if (iter != this->groupNameToGroupSelectorMap.end()) {
        groupSelector = iter->second;
    }
    
    return groupSelector;
}

/**
 * Get the group selector for the given group name.
 * @param groupName
 *    The key of the desired group selector.
 * @return The group selector for the key or NULL if no group
 *         selector with the given key.
 */
const GroupAndNameHierarchyGroup* 
GroupAndNameHierarchyModel::getGroupSelectorForGroupName(const AString& groupName) const
{
    GroupAndNameHierarchyGroup* groupSelector = NULL;
    
    std::map<AString, GroupAndNameHierarchyGroup*>::const_iterator iter = this->groupNameToGroupSelectorMap.find(groupName);
    if (iter != this->groupNameToGroupSelectorMap.end()) {
        groupSelector = iter->second;
    }
    
    return groupSelector;
}

/**
 * Get the group selector for the given group key.
 * @param groupKey
 *    The key of the desired group selector.
 * @return The group selector for the key or NULL if no group
 *         selector with the given key.
 */
GroupAndNameHierarchyGroup* 
GroupAndNameHierarchyModel::getGroupSelectorForGroupKey(const int32_t groupKey)
{
    GroupAndNameHierarchyGroup* groupSelector = NULL;
    
    if ((groupKey >= 0) 
        && (groupKey < static_cast<int32_t>(this->keyToGroupNameSelectorVector.size()))) {
        groupSelector = this->keyToGroupNameSelectorVector[groupKey];
    }
    
    return groupSelector;
}

/**
 * Get the group selector for the given group key.
 * @param groupKey
 *    The key of the desired group selector.
 * @return The group selector for the key or NULL if no group
 *         selector with the given key.
 */
const GroupAndNameHierarchyGroup* 
GroupAndNameHierarchyModel::getGroupSelectorForGroupKey(const int32_t groupKey) const
{
    GroupAndNameHierarchyGroup* groupSelector = NULL;
    
    if ((groupKey >= 0) 
        && (groupKey < static_cast<int32_t>(this->keyToGroupNameSelectorVector.size()))) {
        groupSelector = this->keyToGroupNameSelectorVector[groupKey];
    }
    
    return groupSelector;
}

/**
 * Add a name to its parent group.
 * If the group does not exist for the parent group name, the group is created.
 * If the name is not present in the parent group, the name is added to the group.
 * Keys are returned for both the parent group and the name.
 *
 * @param parentGroupName
 *    Name of parent group.
 * @param name
 *    Name to add as child of parent group.
 * @param groupKeyOut
 *    OUTPUT containing key for group.
 * @param nameKeyOut
 *    OUTPUT containing key for name in parent group.
 */
void 
GroupAndNameHierarchyModel::addName(const AString& parentGroupName,
                                    const AString& name,
                                    int32_t& groupKeyOut,
                                    int32_t& nameKeyOut)
{
    GroupAndNameHierarchyGroupGeneratedKey* groupSelector = NULL;
    std::map<AString, GroupAndNameHierarchyGroup*>::iterator iter = this->groupNameToGroupSelectorMap.find(parentGroupName);
    if (iter != this->groupNameToGroupSelectorMap.end()) {
        GroupAndNameHierarchyGroupGeneratedKey* cdgk = dynamic_cast<GroupAndNameHierarchyGroupGeneratedKey*>(iter->second);
        CaretAssert(cdgk);
        groupSelector = cdgk;
    }
    else {
        if (this->availableGroupKeys.empty()) {
            const int32_t groupKey = static_cast<int32_t>(this->keyToGroupNameSelectorVector.size());
            groupSelector = new GroupAndNameHierarchyGroupGeneratedKey(parentGroupName,
                                                          groupKey);
            this->keyToGroupNameSelectorVector.push_back(groupSelector);
        }
        else {
            const int32_t groupKey = this->availableGroupKeys.front();
            this->availableGroupKeys.pop_front();
            CaretAssert(this->keyToGroupNameSelectorVector[groupKey] == NULL);
            groupSelector = new GroupAndNameHierarchyGroupGeneratedKey(parentGroupName, groupKey);
            this->keyToGroupNameSelectorVector[groupKey] = groupSelector;
        }
        this->groupNameToGroupSelectorMap.insert(std::make_pair(parentGroupName, groupSelector));
    }
    
    CaretAssert(groupSelector);
    groupKeyOut = groupSelector->getKey();
    CaretAssert(groupKeyOut >= 0);
    
    nameKeyOut = groupSelector->addName(name);
}

/**
 * Add a group using the key contained in the given group group.
 * @param group
 *    Group display group that is added using its key.
 */
void
GroupAndNameHierarchyModel::addGroup(GroupAndNameHierarchyGroup* group)
{
    CaretAssert(group);
    
    const int32_t key = group->getKey();
    CaretAssert(key >= 0);
    
    const AString groupName = group->getName();
    
    const int32_t numberOfGroupes = this->keyToGroupNameSelectorVector.size();
    
    if (key < numberOfGroupes) {
        if (this->keyToGroupNameSelectorVector[key] != NULL) {
            delete this->keyToGroupNameSelectorVector[key];
            this->keyToGroupNameSelectorVector[key] = NULL;
            this->groupNameToGroupSelectorMap.erase(groupName);
        }
    }
    else {
        this->keyToGroupNameSelectorVector.resize(key + 1, NULL);
    }
    
    this->keyToGroupNameSelectorVector[key] = group;
    this->groupNameToGroupSelectorMap.insert(std::make_pair(groupName,
                                                            group));
}

/**
 * Is a group selected in the given display group?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param groupKey
 *    Key for group for which selection statis is desired.
 * @return
 *    true if group is selected for the given display group.
 *    false if not selected or key is invalid.
 */
bool 
GroupAndNameHierarchyModel::isGroupSelected(const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex,
                                            const int32_t groupKey) const
{
    bool status = false;
    
    if ((groupKey >= 0) 
        && (groupKey < static_cast<int32_t>(this->keyToGroupNameSelectorVector.size()))) {
        GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[groupKey];
        if (cs != NULL) {
            status = cs->isSelected(displayGroup, tabIndex);
        }
        else {
            CaretAssertMessage(0, "No group group for group key="
                               + AString::number(groupKey));
        }
    }
    
    return status;
}

/**
 * Set a group selected in the given display group
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param groupKey
 *    Key for group for which selection status is set.
 * @param selected
 *    New selection status.
 */
void 
GroupAndNameHierarchyModel::setGroupSelected(const DisplayGroupEnum::Enum displayGroup,
                                             const int32_t tabIndex,
                                             const int32_t groupKey,
                                             const bool selected)
{
    if ((groupKey >= 0) 
        && (groupKey < static_cast<int32_t>(this->keyToGroupNameSelectorVector.size()))) {
        GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[groupKey];
        if (cs != NULL) {
            cs->setSelected(displayGroup, tabIndex, selected);
        }
        else {
            CaretAssertMessage(0, "No group group for group key="
                               + AString::number(groupKey));
        }
    }
}

/**
 * Is a name selected in the given parent group and display group?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param parentGroupKey
 *    Key for parent group for which name selection status is desired.
 * @param nameKey
 *    Key for name in parent group for which selection status is desired.
 * @return
 *    true if name is selected for the given group and display group.
 *    false if not selected or either group or name key is invalid.
 */
bool 
GroupAndNameHierarchyModel::isNameSelected(const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const int32_t parentGroupKey,
                                           const int32_t nameKey) const
{
    bool status = false;
    
    if ((parentGroupKey >= 0) 
        && (parentGroupKey < static_cast<int32_t>(this->keyToGroupNameSelectorVector.size()))) {
        GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[parentGroupKey];
        if (cs != NULL) {
            GroupAndNameHierarchyName* ns = cs->getNameSelectorWithKey(nameKey);
            if (ns != NULL) {
                status = ns->isSelected(displayGroup, tabIndex);
            }
            else {
                CaretAssertMessage(0, "No name group for name key="
                                   + AString::number(nameKey)
                                   + " for group="
                                   + cs->getName());
            }
        }
        else {
            CaretAssertMessage(0, "No group group for group key="
                               + AString::number(parentGroupKey));
        }
    }
    
    return status;
}

/**
 * Set a name selected in the given parent group and display group
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param parentGroupKey
 *    Key for parent group for which selection status is set.
 * @param nameKey
 *    Key for name for which selection status is set.
 * @param selected
 *    New selection status.
 */
void GroupAndNameHierarchyModel::setNameSelected(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 const int32_t parentGroupKey,
                                                 const int32_t nameKey,
                                                 const bool selected)
{
    if ((parentGroupKey >= 0) 
        && (parentGroupKey < static_cast<int32_t>(this->keyToGroupNameSelectorVector.size()))) {
        GroupAndNameHierarchyGroup* cs = this->keyToGroupNameSelectorVector[parentGroupKey];
        if (cs != NULL) {
            GroupAndNameHierarchyName* ns = cs->getNameSelectorWithKey(nameKey);
            if (ns != NULL) {
                ns->setSelected(displayGroup, tabIndex, selected);
            }
            else {
                CaretAssertMessage(0, "No name group for name key="
                                   + AString::number(nameKey)
                                   + " for group="
                                   + cs->getName());
            }
        }
        else {
            CaretAssertMessage(0, "No group group for group key="
                               + AString::number(parentGroupKey));
        }
    }
    
}

/**
 * @return The expanded status.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 */
bool 
GroupAndNameHierarchyModel::isExpanded(const DisplayGroupEnum::Enum displayGroup,
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
GroupAndNameHierarchyModel::setExpanded(const DisplayGroupEnum::Enum displayGroup,
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

