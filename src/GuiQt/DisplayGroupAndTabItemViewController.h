#ifndef __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_H__
#define __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_H__

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

#include <set>

#include <QWidget>

#include "ContextSensitiveMenuItemsEnum.h"
#include "DataFileTypeEnum.h"
#include "DisplayGroupEnum.h"

class QAction;
class QLineEdit;
class QTreeWidgetItem;
class QToolButton;

namespace caret {

    class DisplayGroupAndTabItemInterface;
    class DisplayGroupAndTabItemTreeWidgetItem;
    class WuQTreeWidget;
    
    class DisplayGroupAndTabItemViewController : public QWidget {
        
        Q_OBJECT

    public:
        DisplayGroupAndTabItemViewController(const DataFileTypeEnum::Enum dataFileType,
                                             const int32_t browserWindowIndex,
                                             const QString& objectNameForMacros,
                                             const QString& descriptiveNameForMacros,
                                             QWidget* parent = 0);
        
        virtual ~DisplayGroupAndTabItemViewController();

        void enableContextSensitiveMenu(const std::vector<ContextSensitiveMenuItemsEnum::Enum>& contextMenuItems);
        
        void updateContent(std::vector<DisplayGroupAndTabItemInterface*>& contentItemsIn,
                           const DisplayGroupEnum::Enum displayGroup,
                           const int32_t tabIndex,
                           const bool allowSelectionFlag);

        // ADD_NEW_METHODS_HERE

    signals:
        void contextMenuItemSelected(QList<QTreeWidgetItem*>& itemsSelected,
                                     const ContextSensitiveMenuItemsEnum::Enum contextMenuItem);
        
    private slots:
        void itemWasCollapsed(QTreeWidgetItem* item);
        
        void itemWasExpanded(QTreeWidgetItem* item);
        
        void itemWasChanged(QTreeWidgetItem* item,
                            int column);
        
        void itemsWereSelected();
        
        void displayContextMenu(const QPoint& pos);
        
        void turnOnSelectedItemsTriggered();
        
        void turnOffSelectedItemsTriggered();
        
        
        void collapseAllActionTriggered();
        
        void expandAllActionTriggered();
        
        void allOnActionTriggered();
        
        void allOffActionTriggered();
        
        void infoActionTriggered();
        
        void findActionTriggered();
        
        void nextActionTriggered();
        
        void findTextLineEditTextChanged(const QString& text);
        

    private:
        DisplayGroupAndTabItemViewController(const DisplayGroupAndTabItemViewController&);

        DisplayGroupAndTabItemViewController& operator=(const DisplayGroupAndTabItemViewController&);
        
        QWidget* createButtonsWidget(const QString& objectNameForMacros,
                                     const QString& descriptiveNameForMacros);
        
        void getDisplayGroupAndTabIndex(DisplayGroupEnum::Enum& displayGroupOut,
                                        int32_t& tabIndexOut) const;
        
        void processItemExpanded(QTreeWidgetItem* item,
                                 const bool expandedStatus);
        
        void processAnnotationDataSelection(const std::vector<DisplayGroupAndTabItemInterface*>& interfaceItems);
        
        DisplayGroupAndTabItemInterface* getDataItem(QTreeWidgetItem* item) const;
        
        void updateGraphics();
        
        void updateSelectedAndExpandedCheckboxes(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex);
        
        void updateSelectedAndExpandedCheckboxesInOtherViewControllers();
        
        void setCheckedStatusOfSelectedItems(const bool checkedFlag);
        
        void scrollTreeViewToFindItem();
        
        const DataFileTypeEnum::Enum m_dataFileType;
        
        const int32_t m_browserWindowIndex;
        
        WuQTreeWidget* m_treeWidget;
                
        QAction* m_collapseAllAction;
        
        QAction* m_expandAllAction;
        
        QAction* m_allOnAction;
        
        QAction* m_allOffAction;
        
        QToolButton* m_infoToolButton;
        
        QAction* m_infoAction;
        
        QAction* m_findAction;
        
        QAction* m_nextAction;
        
        QLineEdit* m_findTextLineEdit;
        
        QList<QTreeWidgetItem*> m_findItems;
        
        int32_t m_findItemsCurrentIndex = 0;

        std::set<ContextSensitiveMenuItemsEnum::Enum> m_contextMenuItems;
        
        static std::set<DisplayGroupAndTabItemViewController*> s_allViewControllers;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_DECLARE__
    std::set<DisplayGroupAndTabItemViewController*> DisplayGroupAndTabItemViewController::s_allViewControllers;
#endif // __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_H__
