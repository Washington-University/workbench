
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
#include "CaretLogger.h"
#include "CiftiBrainordinateFile.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyName.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
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
: GroupAndNameHierarchyItem(GroupAndNameHierarchyItem::ITEM_TYPE_MODEL,
                           "",
                           -1)
{
    this->clearModelPrivate();
}

/**
 * Destructor.
 */
GroupAndNameHierarchyModel::~GroupAndNameHierarchyModel()
{
    this->clearModelPrivate();
}

/**
 * Clear the group/name hierarchy.
 */
void
GroupAndNameHierarchyModel::clear()
{
    GroupAndNameHierarchyItem::clear();
    clearModelPrivate();
}

/**
 * Clear the group/name hierarchy.
 */
void 
GroupAndNameHierarchyModel::clearModelPrivate()
{
    setUserInterfaceUpdateNeeded();
}

/**
 * Set the selected status for self and all children.
 * @param status
 *    The selection status.
 */
void 
GroupAndNameHierarchyModel::setAllSelected(const bool status)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        setAllSelected(DisplayGroupEnum::DISPLAY_GROUP_TAB, i, status);
    }
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        const DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::fromIntegerCode(i, NULL);
        if (displayGroup != DisplayGroupEnum::DISPLAY_GROUP_TAB) {
            setAllSelected(displayGroup,
                           -1,
                           status);
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
    setSelfAncestorsAndDescendantsSelected(displayGroup,
                                           tabIndex,
                                           selectionStatus);
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
    
    setName(borderFile->getFileNameNoPath());

    const int32_t numBorders = borderFile->getNumberOfBorders();
    if (needToGenerateKeys == false) {
        for (int32_t i = 0; i < numBorders; i++) {
            const Border* border = borderFile->getBorder(i);
            if (border->getGroupNameSelectionItem() == NULL) {
                needToGenerateKeys = true;
            }
        }
    }
    
    /*
     * ID for groups and names is not used
     */
    const int32_t ID_NOT_USED = 0;
    
    if (needToGenerateKeys) {
        /*
         * Clear the counters
         */
        clearCounters();
        
        /*
         * Names for missing group names or border names.
         */
        const AString missingGroupName = "NoGroup";
        const AString missingBorderName = "NoName";
        
        /*
         * For icons
         */
        const GiftiLabelTable* classLabelTable = borderFile->getClassColorTable();
        const GiftiLabelTable* nameLabelTable  = borderFile->getNameColorTable();
        const float rgbaBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
        
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
             * Class
             */
            GroupAndNameHierarchyItem* groupItem = addChild(GroupAndNameHierarchyItem::ITEM_TYPE_GROUP,
                                                           theGroupName,
                                                           ID_NOT_USED);
            CaretAssert(groupItem);
            
            const GiftiLabel* groupLabel = classLabelTable->getLabelBestMatching(theGroupName);
            if (groupLabel != NULL) {
                groupItem->setIconColorRGBA(groupLabel->getColor());
            }
            else {
                groupItem->setIconColorRGBA(rgbaBlack);
            }
            
            /*
             * Name
             */
            GroupAndNameHierarchyItem* nameItem = groupItem->addChild(GroupAndNameHierarchyItem::ITEM_TYPE_NAME,
                                                                     name,
                                                                     ID_NOT_USED);
            const GiftiLabel* nameLabel = nameLabelTable->getLabelBestMatching(name);
            if (nameLabel != NULL) {
                nameItem->setIconColorRGBA(nameLabel->getColor());
            }
            else {
                nameItem->setIconColorRGBA(rgbaBlack);
            }
            
            /*
             * Place the name selector into the border.
             */
            border->setGroupNameSelectionItem(nameItem);
        }
        
        sortDescendantsByName();
        setUserInterfaceUpdateNeeded();
        
        CaretLogFine("BORDER HIERARCHY:"
                     + toString());
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

    setName(labelFile->getFileNameNoPath());

    /*
     * The label table
     */
    GiftiLabelTable* labelTable = labelFile->getLabelTable();
    
    std::map<int32_t, AString> labelKeysAndNames;
    
    if (needToGenerateKeys == false) {
        /*
         * Check to see if any group (map) names have changed.
         */
        const std::vector<GroupAndNameHierarchyItem*> groups = getChildren();
        const int numGroups = static_cast<int32_t>(groups.size());
        if (numGroups != labelFile->getNumberOfMaps()) {
            needToGenerateKeys = true;
        }
        else {
            for (int32_t i = 0; i < numGroups; i++) {
                if (groups[i]->getName() != labelFile->getMapName(i)) {
                    needToGenerateKeys = true;
                    break;
                }
            }
            
            if (needToGenerateKeys == false) {
                labelTable->getKeysAndNames(labelKeysAndNames);
                
                if (m_previousLabelFileKeysAndNames.size() != labelKeysAndNames.size()) {
                    needToGenerateKeys = true;
                }
                else {
                    std::map<int32_t, AString>::const_iterator prevIter = m_previousLabelFileKeysAndNames.begin();
                    for (std::map<int32_t, AString>::const_iterator labelIter = labelKeysAndNames.begin();
                         labelIter != labelKeysAndNames.end();
                         labelIter++) {
                        if (prevIter->first != labelIter->first) {
                            needToGenerateKeys = true;
                            break;
                        }
                        else if (prevIter->second != labelIter->second) {
                            needToGenerateKeys = true;
                            break;
                        }
                        
                        prevIter++;
                    }
//                    if (std::equal(labelKeysAndNames.begin(),
//                                   labelKeysAndNames.end(),
//                                   m_previousLabelKeysAndNames) == false) {
//                        needToGenerateKeys = true;
//                    }
                }
                
//                if (labelTable->hasLabelsWithInvalidGroupNameHierarchy()) {
//                    needToGenerateKeys = true;
//                }
            }
        }
    }

    if (needToGenerateKeys) {
        //const int32_t ID_NOT_USED = 0;
        
        /*
         * Save keys and names for comparison in next update test
         */
        m_previousLabelFileKeysAndNames = labelKeysAndNames;
        if (m_previousLabelFileKeysAndNames.empty()) {
            labelTable->getKeysAndNames(m_previousLabelFileKeysAndNames);
        }
        
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
             * Find/create group
             */
            GroupAndNameHierarchyItem* groupItem = addChild(GroupAndNameHierarchyItem::ITEM_TYPE_GROUP,
                                                           theGroupName,
                                                           iMap);
            CaretAssert(groupItem);
            
            /*
             * Get indices of labels used in this map
             */
            std::vector<int32_t> labelKeys = labelFile->getUniqueLabelKeysUsedInMap(iMap);

            const int32_t numLabelKeys = static_cast<int32_t>(labelKeys.size());
            for (int32_t iLabel = 0; iLabel < numLabelKeys; iLabel++) {
                const int32_t labelKey = labelKeys[iLabel];
                GiftiLabel* label = labelTable->getLabel(labelKey);
                if (label == NULL) {
                    continue;
                }
                
                AString labelName = label->getName();
                if (labelName.isEmpty()) {
                    labelName = missingName;
                }

                const float* rgba = label->getColor();

                /*
                 * Adding focus to class
                 */
                GroupAndNameHierarchyItem* nameItem = groupItem->addChild(GroupAndNameHierarchyItem::ITEM_TYPE_NAME,
                                                                         labelName,
                                                                         labelKey);
                nameItem->setIconColorRGBA(rgba);
                
                /*
                 * Place the name selector into the label.
                 */
                label->setGroupNameSelectionItem(nameItem);
            }
        }
        
        /*
         * Sort names in each group
         */
        std::vector<GroupAndNameHierarchyItem*> groups = getChildren();
        for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = groups.begin();
             iter != groups.end();
             iter++) {
            GroupAndNameHierarchyItem* nameItem = *iter;
            nameItem->sortDescendantsByName();
        }
        
        setUserInterfaceUpdateNeeded();
        
        CaretLogFine("LABEL HIERARCHY:"
                     + toString());
    }
}

/**
 * Update this group hierarchy with the label names
 * and groups.
 *
 * @param ciftiBrainordinateFile
 *    The cifti brainordinate file from which classes and names are from.
 * @parm forceUpdate
 *    If true, force an update.
 */
void
GroupAndNameHierarchyModel::update(CiftiBrainordinateFile* ciftiBrainordinateFile,
                                   const bool forceUpdate)
{
    bool needToGenerateKeys = forceUpdate;
    
    setName(ciftiBrainordinateFile->getFileNameNoPath());
    
    std::vector<std::map<int32_t, AString> > labelMapKeysAndNames;
    
    if (needToGenerateKeys == false) {
        /*
         * Check to see if any group (map) names have changed.
         */
        const std::vector<GroupAndNameHierarchyItem*> groups = getChildren();
        const int numGroups = static_cast<int32_t>(groups.size());
        if (numGroups != ciftiBrainordinateFile->getNumberOfMaps()) {
            /*
             * Number of maps has changed.
             */
            needToGenerateKeys = true;
        }
        else {
            for (int32_t i = 0; i < numGroups; i++) {
                if (groups[i]->getName() != ciftiBrainordinateFile->getMapName(i)) {
                    needToGenerateKeys = true;
                    break;
                }
            }
            
            if (needToGenerateKeys == false) {
                if (static_cast<int32_t>(m_previousCiftiLabelFileMapKeysAndNames.size()) != ciftiBrainordinateFile->getNumberOfMaps()) {
                    needToGenerateKeys = true;
                }
            }
            if (needToGenerateKeys == false) {
                for (int32_t i = 0; i < numGroups; i++) {
                    const GiftiLabelTable* labelTable = ciftiBrainordinateFile->getMapLabelTable(i);
                    std::map<int32_t, AString> labelKeysAndNames;
                    labelTable->getKeysAndNames(labelKeysAndNames);
                    
                    labelMapKeysAndNames.push_back(labelKeysAndNames);
                }
                
                for (int32_t i = 0; i < numGroups; i++) {
                    const std::map<int32_t, AString>& labelKeysAndNames = labelMapKeysAndNames[i];
                
                    const std::map<int32_t, AString>& previousLabelKeysAndNames = m_previousCiftiLabelFileMapKeysAndNames[i];
                    
                    if (previousLabelKeysAndNames.size() != labelKeysAndNames.size()) {
                        needToGenerateKeys = true;
                        break;
                    }
                    else {
                        std::map<int32_t, AString>::const_iterator prevIter = previousLabelKeysAndNames.begin();
                        for (std::map<int32_t, AString>::const_iterator labelIter = labelKeysAndNames.begin();
                             labelIter != labelKeysAndNames.end();
                             labelIter++) {
                            if (prevIter->first != labelIter->first) {
                                needToGenerateKeys = true;
                                break;
                            }
                            else if (prevIter->second != labelIter->second) {
                                needToGenerateKeys = true;
                                break;
                            }
                            
                            prevIter++;
                        }
                    }
                }
            }
            
        }
    }
    
    if (needToGenerateKeys) {
        //const int32_t ID_NOT_USED = 0;
        
        /*
         * Save keys and names for comparison in next update test
         */
        const int numMaps = ciftiBrainordinateFile->getNumberOfMaps();
        m_previousCiftiLabelFileMapKeysAndNames = labelMapKeysAndNames;
        if (m_previousCiftiLabelFileMapKeysAndNames.empty()) {
            for (int32_t i = 0; i < numMaps; i++) {
                const GiftiLabelTable* labelTable = ciftiBrainordinateFile->getMapLabelTable(i);
                std::map<int32_t, AString> labelKeysAndNames;
                labelTable->getKeysAndNames(labelKeysAndNames);
                m_previousCiftiLabelFileMapKeysAndNames.push_back(labelKeysAndNames);
            }
        }
        
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
        for (int32_t iMap = 0; iMap < numMaps; iMap++) {
            /*
             * The label table
             */
            GiftiLabelTable* labelTable = ciftiBrainordinateFile->getMapLabelTable(iMap);
            
            /*
             * Get the group.  If it is empty, use the default name.
             */
            AString theGroupName = ciftiBrainordinateFile->getMapName(iMap);
            if (theGroupName.isEmpty()) {
                theGroupName = missingGroupName;
            }
            
            /*
             * Find/create group
             */
            GroupAndNameHierarchyItem* groupItem = addChild(GroupAndNameHierarchyItem::ITEM_TYPE_GROUP,
                                                            theGroupName,
                                                            iMap);
            CaretAssert(groupItem);
            
            /*
             * Get indices of labels used in this map
             */
            std::vector<int32_t> labelKeys = ciftiBrainordinateFile->getUniqueLabelKeysUsedInMap(iMap);
            
            const int32_t numLabelKeys = static_cast<int32_t>(labelKeys.size());
            for (int32_t iLabel = 0; iLabel < numLabelKeys; iLabel++) {
                const int32_t labelKey = labelKeys[iLabel];
                GiftiLabel* label = labelTable->getLabel(labelKey);
                if (label == NULL) {
                    continue;
                }
                
                AString labelName = label->getName();
                if (labelName.isEmpty()) {
                    labelName = missingName;
                }
                
                const float* rgba = label->getColor();
                
                /*
                 * Adding focus to class
                 */
                GroupAndNameHierarchyItem* nameItem = groupItem->addChild(GroupAndNameHierarchyItem::ITEM_TYPE_NAME,
                                                                          labelName,
                                                                          labelKey);
                nameItem->setIconColorRGBA(rgba);
                
                /*
                 * Place the name selector into the label.
                 */
                label->setGroupNameSelectionItem(nameItem);
            }
        }
        
        /*
         * Sort names in each group
         */
        std::vector<GroupAndNameHierarchyItem*> groups = getChildren();
        for (std::vector<GroupAndNameHierarchyItem*>::iterator iter = groups.begin();
             iter != groups.end();
             iter++) {
            GroupAndNameHierarchyItem* nameItem = *iter;
            nameItem->sortDescendantsByName();
        }
        
        setUserInterfaceUpdateNeeded();
        
        CaretLogFine("LABEL HIERARCHY:"
                     + toString());
    }
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
    
    setName(fociFile->getFileNameNoPath());
    
    const int32_t numFoci = fociFile->getNumberOfFoci();
    if (needToGenerateKeys == false) {
        for (int32_t i = 0; i < numFoci; i++) {
            const Focus* focus = fociFile->getFocus(i);
            if (focus->getGroupNameSelectionItem() == NULL) {
                needToGenerateKeys = true;
            }
        }
    }
    
    /*
     * ID for groups and names is not used
     */
    const int32_t ID_NOT_USED = 0;
    
    if (needToGenerateKeys) {
        /*
         * Names for missing group names or foci names.
         */
        const AString missingGroupName = "NoGroup";
        const AString missingName = "NoName";
        
        /*
         * Reset the counts for all group and children names.
         */
        clearCounters();
        
        /*
         * For icons
         */
        const GiftiLabelTable* classLabelTable = fociFile->getClassColorTable();
        const GiftiLabelTable* nameLabelTable  = fociFile->getNameColorTable();
        const float rgbaBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
                
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
             * Class
             */
            GroupAndNameHierarchyItem* groupItem = addChild(GroupAndNameHierarchyItem::ITEM_TYPE_GROUP,
                                                            theGroupName,
                                                            ID_NOT_USED);
            CaretAssert(groupItem);
            CaretAssert(groupItem);
            
            const GiftiLabel* groupLabel = classLabelTable->getLabelBestMatching(theGroupName);
            if (groupLabel != NULL) {
                groupItem->setIconColorRGBA(groupLabel->getColor());
            }
            else {
                groupItem->setIconColorRGBA(rgbaBlack);
            }

            /*
             * Name
             */
            GroupAndNameHierarchyItem* nameItem = groupItem->addChild(GroupAndNameHierarchyItem::ITEM_TYPE_NAME,
                                                                      name,
                                                                      ID_NOT_USED);
            const GiftiLabel* nameLabel = nameLabelTable->getLabelBestMatching(name);
            if (nameLabel != NULL) {
                nameItem->setIconColorRGBA(nameLabel->getColor());
            }
            else {
                nameItem->setIconColorRGBA(rgbaBlack);
            }
            
            /*
             * Place the name selector into the border.
             */
            focus->setGroupNameSelectionItem(nameItem);
        }
        
        sortDescendantsByName();
        setUserInterfaceUpdateNeeded();

        CaretLogFine("FOCI HIERARCHY:"
                     + toString());
    }
}

/**
 * Is a User-Interface needed in the given display group and tab?
 * This occurs when the hierarchy has changed (group and name).
 * After calling this method, the status for the display group/tab is cleared.
 * 
 * @param displayGroup
 *    Display group.
 * @param tabIndex
 *    Index of tab.
 * @return True if update needed, else false.
 */
bool
GroupAndNameHierarchyModel::needsUserInterfaceUpdate(const DisplayGroupEnum::Enum displayGroup,
                                                     const int32_t tabIndex) const
{
    bool needUpdate = false;
    
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->expandedStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->expandedStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        needUpdate = m_updateNeededInTab[tabIndex];
        m_updateNeededInTab[tabIndex] = false;
    }
    else {
        needUpdate = m_updateNeededInDisplayGroupAndTab[displayIndex][tabIndex];
        m_updateNeededInDisplayGroupAndTab[displayIndex][tabIndex] = false;
    }
    
    return needUpdate;
}

/**
 * Set user interface updates needed.
 */
void
GroupAndNameHierarchyModel::setUserInterfaceUpdateNeeded()
{
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        for (int32_t j = 0; j < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; j++) {
            m_updateNeededInDisplayGroupAndTab[i][j] = true;
        }
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_updateNeededInTab[i] = true;
    }
}



