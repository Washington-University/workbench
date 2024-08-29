#ifndef __GROUP_AND_NAME_HIERARCHY_ITEM_H__
#define __GROUP_AND_NAME_HIERARCHY_ITEM_H__

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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "GroupAndNameCheckStateEnum.h"
#include "SceneableInterface.h"


namespace caret {

    class GroupAndNameHierarchyUserInterface;
    class SceneClassAssistant;
    class TabDrawingInfo;
    
    class GroupAndNameHierarchyItem : public CaretObject, SceneableInterface {
        
    public:
        /**
         * Type of item.
         */
        enum ItemType {
            /** Model (eg: file) */
            ITEM_TYPE_MODEL,
            /** Group (eg: class or map) */
            ITEM_TYPE_GROUP,
            /** Name (eg: border, focus, or label) */
            ITEM_TYPE_NAME
        };
        
    protected:
        GroupAndNameHierarchyItem(GroupAndNameHierarchyUserInterface* groupAndNameHierarchyUserInterface,
                                  const ItemType itemType,
                                  const AString& name,
                                  const int32_t idNumber);
    public:
        virtual ~GroupAndNameHierarchyItem();
        
        virtual void clear();
        
        ItemType getItemType() const;
        
        AString getName() const;
        
        GroupAndNameHierarchyItem* getParent();
        
        const GroupAndNameHierarchyItem* getParent() const;
        
        std::vector<GroupAndNameHierarchyItem*> getAncestors() const;
        
        std::vector<GroupAndNameHierarchyItem*> getChildren() const;
        
        void sortDescendantsByName();
        
        GroupAndNameHierarchyItem* getChildWithNameAndIdNumber(const AString& name,
                                                              const int32_t idNumber);
        
        std::vector<GroupAndNameHierarchyItem*> getDescendants() const;
        
        GroupAndNameHierarchyItem* addChild(const ItemType itemType,
                                            const AString& name,
                                            const int32_t idNumber);
        
        void addChild(GroupAndNameHierarchyItem* child);
        
        void removeChild(GroupAndNameHierarchyItem* child);
        
        bool isSelected(const TabDrawingInfo& tabDrawingInfo) const;
        
        bool isSelected(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex) const;
        
        GroupAndNameCheckStateEnum::Enum getCheckState(const TabDrawingInfo& tabDrawingInfo) const;
        
        GroupAndNameCheckStateEnum::Enum getCheckState(const DisplayGroupEnum::Enum displayGroup,
                                                       const int32_t tabIndex) const;
        
        void setSelected(const TabDrawingInfo& tabDrawingInfo,
                         const bool status);
        
        void setSelected(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool status);
        
        void setDescendantsSelected(const TabDrawingInfo& tabDrawingInf,
                                         const bool status);
        
        void setDescendantsSelected(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex,
                                    const bool status);
        
        void setAncestorsSelected(const TabDrawingInfo& tabDrawingInf,
                                  const bool status);
        
        void setAncestorsSelected(const DisplayGroupEnum::Enum displayGroup,
                                  const int32_t tabIndex,
                                  const bool status);
        
        void setSelfAncestorsAndDescendantsSelected(const TabDrawingInfo& tabDrawingInf,
                                                         const bool status);
        
        void setSelfAncestorsAndDescendantsSelected(const DisplayGroupEnum::Enum displayGroup,
                                                    const int32_t tabIndex,
                                                    const bool status);
        
        const float* getIconColorRGBA() const;
        
        void setIconColorRGBA(const float rgba[4]);
        
        bool isExpandedToDisplayChildren(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex) const;
        
        void setExpandedToDisplayChildren(const DisplayGroupEnum::Enum displayGroup,
                                                  const int32_t tabIndex,
                                                  const bool expanded);
        
        virtual void copySelections(const int32_t sourceTabIndex,
                                    const int32_t targetTabIndex);

        int32_t getIdNumber() const;
        
        void clearCounters();
        
        void incrementCounter();
        
        int32_t getCounter() const;
        
        void removeDescendantsWithCountersEqualToZeros();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    protected:
        void setName(const AString& name);
        
    private:
        GroupAndNameHierarchyItem(const GroupAndNameHierarchyItem&);
        
        GroupAndNameHierarchyItem& operator=(const GroupAndNameHierarchyItem&);
        
        void addChildPrivate(GroupAndNameHierarchyItem* child);
        
        void clearPrivate();
        
        class ChildMapKey {
        public:
            ChildMapKey(const int32_t idNumber,
                        const AString& name)
            : m_idNumber(idNumber),
              m_name(name) {  }
            
            const int32_t m_idNumber;
            const AString m_name;
            
            bool operator==(const ChildMapKey& childMapKey) const {
                if (m_idNumber == childMapKey.m_idNumber) {
                    if (m_name == childMapKey.m_name) {
                        return true;
                    }
                }
                return false;
            }
            
            bool operator<(const ChildMapKey& childMapKey) const {
                if (m_idNumber < childMapKey.m_idNumber) {
                    return true;
                }
                else if (m_idNumber == childMapKey.m_idNumber) {
                    if (m_name < childMapKey.m_name) {
                        return true;
                    }
                }
                return false;
            }
        };

        /** Interface for files that use group and name hierarchy */
        GroupAndNameHierarchyUserInterface* m_groupAndNameHierarchyUserInterface = NULL;
        
        /** Type of item */
        const ItemType m_itemType;
        
        /** Name of this item */
        AString m_name;
        
        /** ID Number */
        const int32_t m_idNumber;
        
        /** Parent of this item */
        GroupAndNameHierarchyItem* m_parent;
        
        /** Children of this item */
        std::vector<GroupAndNameHierarchyItem*> m_children;
        
        /** For fast access to children by name and id number of child */
        std::map<ChildMapKey, GroupAndNameHierarchyItem*> m_childrenNameIdMap;
        
        /** Selection for each display group */
        bool m_selectedInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /** Selection for each tab */
        bool m_selectedInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Color for icon, valid when (iconRGBA[3] > 0.0) */
        float m_iconRGBA[4];
        
        /** Expanded (collapsed) status in display group */
        bool m_expandedStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /** Expanded (collapsed) status in tab */
        bool m_expandedStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Counter for tracking usage of item */
        int32_t m_counter;

        /** Assists with scenes */
        SceneClassAssistant* m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __GROUP_AND_NAME_HIERARCHY_ITEM_DECLARE__
#endif // __GROUP_AND_NAME_HIERARCHY_ITEM_DECLARE__

} // namespace
#endif  //__GROUP_AND_NAME_HIERARCHY_ITEM_H__
