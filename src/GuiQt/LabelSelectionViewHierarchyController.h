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

class QModelIndex;
class QStandardItem;
class QStandardItemModel;
class QTreeView;

namespace caret {

    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectorObject;
    class LabelSelectionItemModel;
    
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
        void processFileSelectionChanged();
        
        void processSelectionChanges();
        
        void treeItemClicked(const QModelIndex& modelIndex);
        
        void collapseAllActionTriggered();
        
        void expandAllActionTriggered();
        
        void allOnActionTriggered();
        
        void allOffActionTriggered();
        
    private:
        LabelSelectionViewHierarchyController(const LabelSelectionViewHierarchyController&);

        LabelSelectionViewHierarchyController& operator=(const LabelSelectionViewHierarchyController&);

        void setCheckedStatusOfAllChildren(QStandardItem* item,
                                           const Qt::CheckState checkState);
        
        std::pair<CaretMappableDataFile*, int32_t> getSelectedFileAndMapIndex();
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex;
        
        QTreeView* m_treeView;
        
        QAction* m_collapseAllAction;
        
        QAction* m_expandAllAction;
        
        QAction* m_allOnAction;
        
        QAction* m_allOffAction;
        
        LabelSelectionItemModel* m_labelHierarchyModel = NULL;
        
        CaretMappableDataFileAndMapSelectorObject* m_labelFileAndMapSelector;
    };
    
#ifdef __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__
#endif // __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER__H_
