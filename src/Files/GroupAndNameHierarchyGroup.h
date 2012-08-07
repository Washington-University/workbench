#ifndef __CLASS_AND_NAME_HIERARCHY_GROUP__H_
#define __CLASS_AND_NAME_HIERARCHY_GROUP__H_

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


#include "GroupAndNameHierarchyName.h"
#include "DisplayGroupEnum.h"

namespace caret {

    class GroupAndNameHierarchyGroup : public GroupAndNameHierarchyName {
    protected:
        GroupAndNameHierarchyGroup(const AString& name,
                                   const int32_t key);
        
    public:
        virtual ~GroupAndNameHierarchyGroup();
        
        virtual void clear();
        
        void copySelections(const int32_t sourceTabIndex,
                            const int32_t targetTabIndex);
        
        void setAllSelected(const bool status);
        
        void setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const bool status);
        
        bool isAllSelected(const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex) const;
        
        std::vector<int32_t> getAllNameKeysSortedByName() const;
        
        virtual GroupAndNameHierarchyName* getNameSelectorWithKey(const int32_t nameKey) = 0;
        
        virtual const GroupAndNameHierarchyName* getNameSelectorWithKey(const int32_t nameKey) const = 0;
        
        GroupAndNameHierarchyName* getNameSelectorWithName(const AString& name);
        
        bool isExpanded(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        void setExpanded(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool expanded);
        
        virtual int32_t getNumberOfNamesWithCountersGreaterThanZero() const = 0;
        
        virtual void clearAllNameCounters() = 0;
        
        virtual void removeNamesWithCountersEqualZero() = 0;
        
        // ADD_NEW_METHODS_HERE
        
    protected:
        void addToNameSelectorMap(const AString& name,
                                  GroupAndNameHierarchyName* nameSelector);
        
        void removeNameSelector(GroupAndNameHierarchyName* nameSelector);
        
    private:
        GroupAndNameHierarchyGroup(const GroupAndNameHierarchyGroup&);
        
        GroupAndNameHierarchyGroup& operator=(const GroupAndNameHierarchyGroup&);
        
        void clearPrivate();
        
        /** Maps a name to its name information.  Map is fastest way to search by name.   */
        std::map<AString, GroupAndNameHierarchyName*> nameToNameSelectorMap;
        
        /** Expanded (collapsed) status in display group */
        bool expandedStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /** Expanded (collapsed) status in tab */
        bool expandedStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_GROUP_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_GROUP__H_
