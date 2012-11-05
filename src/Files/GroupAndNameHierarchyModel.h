#ifndef __CLASS_AND_NAME_HIERARCHY_MODEL_H_
#define __CLASS_AND_NAME_HIERARCHY_MODEL_H_

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

#include <deque>
#include <map>

#include "BrainConstants.h"
#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "GroupAndNameCheckStateEnum.h"

class QIcon;

namespace caret {

    class BorderFile;
    class GroupAndNameHierarchyGroup;
    class GroupAndNameHierarchyName;
    class FociFile;
    class LabelFile;
    
    class GroupAndNameHierarchyModel : public CaretObject {
    public:
        GroupAndNameHierarchyModel();
        
        virtual ~GroupAndNameHierarchyModel();
        
        void clear();
        
        void copyGroupNameAndHierarchy(const int32_t sourceTabIndex,
                                       const int32_t targetTabIndex);
        
//        void removeUnusedNamesAndGroups(BorderFile* borderFile);
//        
//        void removeUnusedNamesAndGroups(FociFile* fociFile);
        
        bool isGroupValid(const int32_t groupKey) const;
        
        void setAllSelected(const bool status);
        
        void setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const bool status);
        
        void update(BorderFile* borderFile,
                    const bool forceUpdate);
        
        void update(FociFile* fociFile,
                    const bool forceUpdate);
        
        void update(LabelFile* labelFile,
                    const bool forceUpdate);
        
        AString toString() const;
        
        AString getName() const;
        
        bool isSelected(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        void setSelected(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool status);
        
        GroupAndNameCheckStateEnum::Enum getCheckState(const DisplayGroupEnum::Enum displayGroup,
                                                             const int32_t tabIndex) const;
        
        std::vector<int32_t> getAllGroupKeysSortedByName() const;
        
        GroupAndNameHierarchyGroup* getGroupSelectorForGroupName(const AString& groupName);
        
        const GroupAndNameHierarchyGroup* getGroupSelectorForGroupName(const AString& groupName) const;
        
        GroupAndNameHierarchyGroup* getGroupSelectorForGroupKey(const int32_t groupKey);
        
        const GroupAndNameHierarchyGroup* getGroupSelectorForGroupKey(const int32_t groupKey) const;
        
        void addName(const AString& parentGroupName,
                     const AString& name,
                     int32_t& parentGroupKeyOut,
                     int32_t& nameKeyOut);
        
        void addGroup(GroupAndNameHierarchyGroup* group);
        
        bool isGroupSelected(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const int32_t groupKey) const;
        
        void setGroupSelected(const DisplayGroupEnum::Enum displayGroup,
                              const int32_t tabIndex,
                              const int32_t groupKey,
                              const bool selected);
        
        bool isNameSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const int32_t parentGroupKey,
                            const int32_t nameKey) const;

        void setNameSelected(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const int32_t parentGroupKey,
                             const int32_t nameKey,
                             const bool selected);
        
        bool isExpanded(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        void setExpanded(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool expanded);
        
        bool needsUserInterfaceUpdate(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
    private:
        GroupAndNameHierarchyModel(const GroupAndNameHierarchyModel&);

        GroupAndNameHierarchyModel& operator=(const GroupAndNameHierarchyModel&);
        
        void setUserInterfaceUpdateNeeded();
        
        /** Name of model, does NOT get cleared. */
        AString name;
        
        /* overlay selection status in display group */
        bool selectionStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /* overlay selection status in tab */
        bool selectionStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Expanded (collapsed) status in display group */        
        bool expandedStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];

        /** Expanded (collapsed) status in tab */        
        bool expandedStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** If keys are removed, they are stored here for future reuse. */
        std::deque<int32_t> availableGroupKeys;
        
        /** Holds group and its names, indexed by group key.  Vector provides fast access by key. */
        std::vector<GroupAndNameHierarchyGroup*> keyToGroupNameSelectorVector;
        
        /** Maps a group name to its group selector.  Map is fastest way to search by name.  */
        std::map<AString, GroupAndNameHierarchyGroup*> groupNameToGroupSelectorMap;
        
        /** 
         * Update needed status of DISPLAY GROUP in EACH TAB.
         * Used when user has set to a display group.
         * Indicates that an update is needed for the given display group in the given tab.
         */
        mutable bool updateNeededInDisplayGroupAndTab[DisplayGroupEnum::NUMBER_OF_GROUPS][BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /**
         * Update needed in TAB.
         */
        mutable bool updatedNeededInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_MODEL_H_
