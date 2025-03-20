#ifndef __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER__H_
#define __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER__H_

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

#include <set>
#include <vector>

#include <QWidget>

#include "DataFileTypeEnum.h"
#include "DisplayGroupEnum.h"

class QAction;
class QLineEdit;
class QTreeWidgetItem;
class QToolButton;
class QVBoxLayout;

namespace caret {

    class BorderFile;
    class CiftiBrainordinateLabelFile;
    class FociFile;
    class LabelFile;
    class GroupAndNameHierarchyModel;
    class GroupAndNameHierarchyTreeWidgetItem;
    class VolumeFile;
    class WuQTreeWidget;
    
    class GroupAndNameHierarchyViewController : public QWidget {
        
        Q_OBJECT

    public:
        GroupAndNameHierarchyViewController(const int32_t browserWindowIndex,
                                            const QString& objectNameForMacros,
                                            const QString& descriptiveNameForMacros,
                                            QWidget* parent);
        
        virtual ~GroupAndNameHierarchyViewController();
        
        void updateContents(std::vector<BorderFile*>& borderFiles,
                            const DisplayGroupEnum::Enum displayGroup);
        
        void updateContents(std::vector<FociFile*>& fociFiles,
                            const DisplayGroupEnum::Enum displayGroup);
        
        void updateContents(std::vector<LabelFile*>& labelFiles,
                            std::vector<CiftiBrainordinateLabelFile*>& ciftiLabelFiles,
                            std::vector<VolumeFile*>& volumeLabelFiles,
                            const DisplayGroupEnum::Enum displayGroup);
        
    private slots:
        void itemWasCollapsed(QTreeWidgetItem* item);
        
        void itemWasExpanded(QTreeWidgetItem* item);

        void itemWasChanged(QTreeWidgetItem* item,
                            int column);

        void showTreeViewContextMenu(const QPoint& pos);
        
//        void processFileSelectionChanged();
//        
//        void processSelectionChanges();
        
        void treeItemClicked(QTreeWidgetItem* item, int column);
        
        void treeItemDoubleClicked(QTreeWidgetItem* item, int column);
        
        void collapseAllActionTriggered();
        
        void expandAllActionTriggered();
        
        void allOnActionTriggered();
        
        void allOffActionTriggered();
        
        void infoActionTriggered();
        
        void findActionTriggered();
        
        void nextActionTriggered();
        
        void findTextLineEditTextChanged(const QString& text);
        
    private:
        GroupAndNameHierarchyViewController(const GroupAndNameHierarchyViewController&);

        GroupAndNameHierarchyViewController& operator=(const GroupAndNameHierarchyViewController&);
        
        void updateContents(std::vector<GroupAndNameHierarchyModel*>& modelItems,
                            const DataFileTypeEnum::Enum dataFileType,
                            const bool selectionInvalidatesSurfaceNodeColoring);
        
        std::vector<GroupAndNameHierarchyModel*> getAllModels() const;
        
        void updateGraphics();
        
        void updateSelectedAndExpandedCheckboxes();
        
        void updateSelectedAndExpandedCheckboxesInOtherViewControllers();
        
        void createTreeWidget();
        
        QWidget* createButtonsWidget(const QString& objectNameForMacros,
                                     const QString& descriptiveNameForMacros);
        
        void setAllSelected(bool selected);
        
        void showSelectedItemMenu(const GroupAndNameHierarchyTreeWidgetItem* item,
                                  const QPoint& pos,
                                  const bool infoButtonFlag);
        
        void scrollTreeViewToFindItem();
        
        void setCheckedStatusOfAllChildren(GroupAndNameHierarchyTreeWidgetItem* item,
                                           const Qt::CheckState checkState);

        DataFileTypeEnum::Enum m_dataFileType;
        
        /** Contains pointers to items managed by Qt, so do not delete content */
        std::vector<GroupAndNameHierarchyTreeWidgetItem*> m_treeWidgetItems;
        
        QVBoxLayout* m_modelTreeWidgetLayout;
        
        WuQTreeWidget* m_modelTreeWidget;
        
        int32_t m_browserWindowIndex;
        
        QAction* m_collapseAllAction;
        
        QAction* m_expandAllAction;
        
        QAction* m_allOnAction;
        
        QAction* m_allOffAction;
        
        QToolButton* m_infoToolButton;
        
        QAction* m_infoAction;
        
        QAction* m_findAction;
        
        QAction* m_nextAction;
        
        QLineEdit* m_findTextLineEdit;

        DisplayGroupEnum::Enum m_displayGroup;
        
        DisplayGroupEnum::Enum m_previousDisplayGroup;
        
        int32_t m_previousBrowserTabIndex;
        
        bool m_selectionInvalidatesSurfaceNodeColoring;
        
        QList<QTreeWidgetItem*> m_findItems;
        
        int32_t m_findItemsCurrentIndex = 0;

        static std::set<GroupAndNameHierarchyViewController*> s_allViewControllers;
        
    };
        
#ifdef __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__
    std::set<GroupAndNameHierarchyViewController*> GroupAndNameHierarchyViewController::s_allViewControllers;
#endif // __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER__H_
