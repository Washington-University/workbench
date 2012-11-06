#ifndef __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM__H_
#define __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM__H_

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


#include <QTreeWidgetItem>

#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "GroupAndNameCheckStateEnum.h"

class QAction;
class QCheckBox;
class QVBoxLayout;

namespace caret {

    class GroupAndNameHierarchyGroup;
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
        
        void updateSelections();
        
        void setModelDataExpanded(const bool expanded);
        
        void setModelDataSelected(const bool selected);
        
    private:
        GroupAndNameHierarchyTreeWidgetItem(const GroupAndNameHierarchyTreeWidgetItem&);
        
        GroupAndNameHierarchyTreeWidgetItem& operator=(const GroupAndNameHierarchyTreeWidgetItem&);
        
        void initialize(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex,
                        const ItemType itemType,
                        const QString text,
                        const float* iconColorRGBA);
        
        static GroupAndNameCheckStateEnum::Enum fromQCheckState(const Qt::CheckState checkState);
        
        static Qt::CheckState toQCheckState(const GroupAndNameCheckStateEnum::Enum checkState);
        
        ItemType itemType;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex;
        
        GroupAndNameHierarchyModel* classAndNameHierarchyModel;
        
        GroupAndNameHierarchyGroup* classDisplayGroupSelector;
        
        GroupAndNameHierarchyName* nameDisplayGroupSelector;
        
        std::vector<GroupAndNameHierarchyTreeWidgetItem*> m_children;
        
        bool m_displayNamesWithZeroCount;
        
        bool m_hasChildren;
        
        static const int TREE_COLUMN;
        
        friend class GroupAndNameHierarchyViewController;
    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM_DECLARE__
    const int GroupAndNameHierarchyTreeWidgetItem::TREE_COLUMN = 0;
#endif // __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM__H_
