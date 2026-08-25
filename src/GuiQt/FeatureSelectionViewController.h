#ifndef __FEATURE_SELECTION_VIEW_CONTROLLER__H_
#define __FEATURE_SELECTION_VIEW_CONTROLLER__H_

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

#include <stdint.h>
#include <set>

#include <QWidget>

#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QTableView;
class QToolButton;
class QTreeView;
class QVBoxLayout;

namespace caret {

    class CaretColorEnumComboBox;
    class CaretDataFileSelectionComboBox;
    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
    class FeatureItemModel;
    class FeatureLabelModel;
    class WuQTabWidget;
    
    class FeatureSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        FeatureSelectionViewController(const int32_t browserWindowIndex,
                                                       const QString& parentObjectName,
                                                       QWidget* parent = 0);

        virtual ~FeatureSelectionViewController();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void processSelectionChanges();
        
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
        void labelModelComboBoxActivated(int index);
        
        void labelTableViewItemClicked(const QModelIndex& index);

        void featureTreeItemClicked(const QModelIndex& modelIndex);
        
        void featureTreeItemDoubleClicked(const QModelIndex& modelIndex);
        
        void featureCollapseAllActionTriggered();
        
        void featureExpandAllActionTriggered();
        
        void featureAllOnActionTriggered();
        
        void featureAllOffActionTriggered();
        
        void featureInfoActionTriggered();
        
        void featureMoreActionTriggered();
        
        void featureFindActionTriggered();
        
        void featureNextActionTriggered();
        
        void featureFindTextLineEditTextChanged(const QString& text);
        
        void featureScrollTreeViewToFindItem();
        
        void labelFindActionTriggered();
        
        void labelNextActionTriggered();
        
        void labelFindTextLineEditTextChanged(const QString& text);
        
        void labelScrollTreeViewToFindItem();
        
    private:
        FeatureSelectionViewController(const FeatureSelectionViewController&);

        FeatureSelectionViewController& operator=(const FeatureSelectionViewController&);

        void updateFeatureViewController();
        
        void updateOtherFeatureViewControllers();
        
        void updateFeatureItemsWidget();
        
        void updateLabelWidget();
        
        QWidget* createFeatureItemsWidget();
        
        void featureItemsAllOnOffButtonClicked(const bool onFlag);
        
        void labelsAllOnOffButtonClicked(const bool onFlag);
        
        QWidget* createAttributesWidget();
        
        QWidget* createLabelsWidget();
        
        FeatureLabelModel* getSelectedLabelModel();
        
        void featureResetFindItemsAndFindText();
        
        void featureResetFindItems();
        
        void labelResetFindItemsAndFindText();
        
        void labelResetFindItems();
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex;
        
        CaretDataFileSelectionComboBox* m_featureFileSelectionComboBox;
        
        CaretDataFileSelectionComboBox* m_volumeFileSelectionComboBox;
        
        QCheckBox* m_displayCheckBox;
        
        DisplayGroupEnumComboBox* m_displayGroupComboBox;

        QDoubleSpinBox* m_symbolScaleSpinBox;
        
        QDoubleSpinBox* m_distanceToVolumeSliceSpinBox;
        
        WuQTabWidget* m_tabWidget;
        
        static std::set<FeatureSelectionViewController*> allFeatureSelectionViewControllers;
        
        QTreeView* m_featureItemTreeView;
        
        QComboBox* m_labelModelSelectionComboBox;
        
        QTableView* m_labelsTableView;
        
        
        QAction* m_featureCollapseAllAction;
        QAction* m_featureExpandAllAction;
        QAction* m_featureAllOnAction;
        QAction* m_featureAllOffAction;
        QAction* m_featureMoreAction;
        QToolButton* m_featureMoreToolButton;
        QAction* m_featureInfoAction;
        QToolButton* m_featureInfoToolButton;
        QAction* m_featureFindAction;
        QAction* m_featureNextAction;
        QLineEdit* m_featureFindTextLineEdit;
        
        /*
         * Find model indices are in proxy model, not the model from the file
         */
        std::vector<QModelIndex> m_featureFindItemModelIndices;
        
        int32_t m_featureFindItemsCurrentIndex = 0;
        

        QAction* m_labelFindAction;
        QAction* m_labelNextAction;
        QLineEdit* m_labelFindTextLineEdit;
        /*
         * Find model indices are in proxy model, not the model from the file
         */
        std::vector<QModelIndex> m_labelFindItemModelIndices;
        
        int32_t m_labelFindItemsCurrentIndex = 0;
        



        FeatureItemModel* m_selectedFeatureItemModel = NULL;
        
        
    };
    
#ifdef __FEATURE_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<FeatureSelectionViewController*> FeatureSelectionViewController::allFeatureSelectionViewControllers;
#endif // __FEATURE_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__FEATURE_SELECTION_VIEW_CONTROLLER__H_
