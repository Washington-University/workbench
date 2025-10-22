#ifndef __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER__H_
#define __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER__H_

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

#include <cstdint>
#include <set>
#include <utility>

#include <QWidget>

#include "SceneableInterface.h"

class QAbstractItemModel;
class QComboBox;
class QLineEdit;
class QMenu;
class QModelIndex;
class QStandardItem;
class QStandardItemModel;
class QTreeView;
class QToolButton;

namespace caret {

    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectorObject;
    class LabelSelectionItem;
    class LabelSelectionItemModel;
    class LabelSelectionItemModelProxyFilter;
    
    class LabelSelectionViewHierarchyController : public QWidget, public SceneableInterface {
        
        Q_OBJECT

    public:
        LabelSelectionViewHierarchyController(const int32_t browserWindowIndex,
                                     const QString& parentObjectName,
                                     QWidget* parent = 0);
        
        virtual ~LabelSelectionViewHierarchyController();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        void updateLabelViewController();
        
    private slots:
        LabelSelectionItem* getLabelSelectionItemAtModelIndex(const QModelIndex& modelIndex);
        
        void showTreeViewContextMenu(const QPoint& pos);
        
        void processFileSelectionChanged();
        
        void processSelectionChanges();
        
        void treeItemClicked(const QModelIndex& modelIndex);
        
        void treeItemDoubleClicked(const QModelIndex& modelIndex);
        
        void collapseAllActionTriggered();
        
        void expandAllActionTriggered();
        
        void allOnActionTriggered();
        
        void allOffActionTriggered();
        
        void infoActionTriggered();
        
        void moreActionTriggered();
        
        void findActionTriggered();
        
        void nextActionTriggered();
        
        void findTextLineEditTextChanged(const QString& text);
        
    private:
        LabelSelectionViewHierarchyController(const LabelSelectionViewHierarchyController&);

        LabelSelectionViewHierarchyController& operator=(const LabelSelectionViewHierarchyController&);

        void setCheckedStatusOfAllChildren(QStandardItem* item,
                                           const Qt::CheckState checkState);
        
        std::pair<CaretMappableDataFile*, int32_t> getSelectedFileAndMapIndex();
        
        void scrollTreeViewToFindItem();
        
        void showSelectedItemMenu(const LabelSelectionItem* labelItem,
                                  const QPoint& pos,
                                  const bool infoButtonFlag);
        
        void showNameComboBoxActivated(const AString& text);
        
        LabelSelectionItemModel* getLabelSelectionModel(QAbstractItemModel* abstractItemModel) const;
        
        std::vector<LabelSelectionItem*> getAllItemsInModelTreeView() const;
        
        int32_t getBrowserTabIndex() const;
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex = -1;
        
        QTreeView* m_treeView;
        
        QAction* m_collapseAllAction;
        
        QAction* m_expandAllAction;
        
        QAction* m_allOnAction;
        
        QAction* m_allOffAction;
        
        QToolButton* m_infoToolButton;
        
        QAction* m_infoAction;
        
        QMenu* m_infoMenu;
        
        QToolButton* m_moreToolButton;
        
        QAction* m_findAction;
        
        QAction* m_nextAction;
        
        QLineEdit* m_findTextLineEdit;
        
        LabelSelectionItemModel* m_labelHierarchyModel = NULL;
        
        CaretMappableDataFileAndMapSelectorObject* m_labelFileAndMapSelector;
        
        QComboBox* m_showNameComboBox;
        
        QList<QStandardItem*> m_findItems;
        
        int32_t m_findItemsCurrentIndex = 0;
        
        LabelSelectionItemModelProxyFilter* m_proxyFilter = NULL;
        
    };
    
#ifdef __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__
#endif // __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER__H_
