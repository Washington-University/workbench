#ifndef __DISPLAY_GROUP_AND_TAB_ITEM_TREE_WIDGET_ITEM_H__
#define __DISPLAY_GROUP_AND_TAB_ITEM_TREE_WIDGET_ITEM_H__

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


#include <QTreeWidgetItem>

#include "DisplayGroupEnum.h"
#include "TriStateSelectionStatusEnum.h"

class QTreeWidget;

namespace caret {

    class DisplayGroupAndTabItemInterface;
    
    class DisplayGroupAndTabItemTreeWidgetItem : public QTreeWidgetItem {
        
    public:
        DisplayGroupAndTabItemTreeWidgetItem(const int32_t browserWindowIndex);

        virtual ~DisplayGroupAndTabItemTreeWidgetItem();

        void updateContent(DisplayGroupAndTabItemInterface *displayGroupAndTabItem,
                               QTreeWidget* treeWidget,
                               const DisplayGroupEnum::Enum displayGroup,
                               const int32_t tabIndex);
        
        DisplayGroupAndTabItemInterface* getDisplayGroupAndTabItem() const;
        
        virtual QTreeWidgetItem* clone() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        
        DisplayGroupAndTabItemTreeWidgetItem(const DisplayGroupAndTabItemTreeWidgetItem&);

        DisplayGroupAndTabItemTreeWidgetItem& operator=(const DisplayGroupAndTabItemTreeWidgetItem&);
        
        static TriStateSelectionStatusEnum::Enum fromQCheckState(const Qt::CheckState checkState);
        
        static Qt::CheckState toQCheckState(const TriStateSelectionStatusEnum::Enum triStateStatus);
        
        void setItemIcon(QTreeWidget* treeWidget,
                         DisplayGroupAndTabItemInterface* myDataItem);
        
        void setDisplayGroupAndTabItem(DisplayGroupAndTabItemInterface* displayGroupAndTabItem);
        
        void updateSelectedAndExpandedCheckboxes(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex);
        
        const int32_t m_browserWindowIndex;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex;
        
        // ADD_NEW_MEMBERS_HERE

        static const int NAME_COLUMN;
        
        friend class DisplayGroupAndTabItemViewController;
    };
    
#ifdef __DISPLAY_GROUP_AND_TAB_ITEM_TREE_WIDGET_ITEM_DECLARE__
    const int DisplayGroupAndTabItemTreeWidgetItem::NAME_COLUMN = 0;
#endif // __DISPLAY_GROUP_AND_TAB_ITEM_TREE_WIDGET_ITEM_DECLARE__

} // namespace
#endif  //__DISPLAY_GROUP_AND_TAB_ITEM_TREE_WIDGET_ITEM_H__
