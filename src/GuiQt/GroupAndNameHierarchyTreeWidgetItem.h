#ifndef __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM__H_
#define __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM__H_

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


#include <QTreeWidgetItem>

#include "DisplayGroupEnum.h"
#include "GroupAndNameCheckStateEnum.h"

class QAction;
class QCheckBox;
class QVBoxLayout;

namespace caret {

    class GroupAndNameHierarchyGroup;
    class GroupAndNameHierarchyItem;
    class GroupAndNameHierarchyModel;
    class GroupAndNameHierarchyName;
    
    class GroupAndNameHierarchyTreeWidgetItem : public QTreeWidgetItem {
        
    public:
        /** Type of item within the hierarchy */
        enum ItemType {
            /** The class/name hierarchy model */
            ITEM_TYPE_HIERARCHY_MODEL,
            /** Class in the class/name hierarchy */
            ITEM_TYPE_CLASS,
            /** Name in the class/name hieracrchy */
            ITEM_TYPE_NAME
        };
        
        GroupAndNameHierarchyTreeWidgetItem(const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex,
                                            GroupAndNameHierarchyModel* classAndNameHierarchyModel);
        
        GroupAndNameHierarchyTreeWidgetItem(const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex,
                                          GroupAndNameHierarchyGroup* classDisplayGroupSelector);
        
        GroupAndNameHierarchyTreeWidgetItem(const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex,
                                          GroupAndNameHierarchyName* nameDisplayGroupSelector);
        
        ~GroupAndNameHierarchyTreeWidgetItem();
        
        ItemType getItemType() const;
        
        GroupAndNameHierarchyModel* getClassAndNameHierarchyModel();
        
        GroupAndNameHierarchyGroup* getClassDisplayGroupSelector();
        
        GroupAndNameHierarchyName* getNameDisplayGroupSelector();
        
        void addChildItem(GroupAndNameHierarchyTreeWidgetItem* child);
        
        void updateSelections(const DisplayGroupEnum::Enum displayGroup,
                              const int32_t tabIndex);
        
        void setModelDataExpanded(const bool expanded);
        
        void setModelDataSelected(const bool selected);
        
        void updateIconColorIncludingChildren();
        
    private:
        GroupAndNameHierarchyTreeWidgetItem(const GroupAndNameHierarchyTreeWidgetItem&);
        
        GroupAndNameHierarchyTreeWidgetItem& operator=(const GroupAndNameHierarchyTreeWidgetItem&);
        
        void initialize(GroupAndNameHierarchyItem* groupAndNameHierarchyItem,
                        const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex,
                        const ItemType itemType,
                        const QString text,
                        const float* iconColorRGBA);
        
        static GroupAndNameCheckStateEnum::Enum fromQCheckState(const Qt::CheckState checkState);
        
        static Qt::CheckState toQCheckState(const GroupAndNameCheckStateEnum::Enum checkState);
        
        ItemType m_itemType;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex;
        
        GroupAndNameHierarchyItem* m_groupAndNameHierarchyItem;
        
        GroupAndNameHierarchyModel* m_classAndNameHierarchyModel;
        
        GroupAndNameHierarchyGroup* m_classDisplayGroupSelector;
        
        GroupAndNameHierarchyName* m_nameDisplayGroupSelector;
        
        std::vector<GroupAndNameHierarchyTreeWidgetItem*> m_children;
        
        bool m_displayNamesWithZeroCount;
        
        bool m_hasChildren;
        
        float m_iconBackgroundColorRGBA[4];
        
        float m_iconForegroundColorRGBA[4];
        
        static const int TREE_COLUMN;
        
        friend class GroupAndNameHierarchyViewController;
    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM_DECLARE__
    const int GroupAndNameHierarchyTreeWidgetItem::TREE_COLUMN = 0;
#endif // __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM__H_
