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
class QTableView;
class QVBoxLayout;

namespace caret {

    class CaretColorEnumComboBox;
    class CaretDataFileSelectionComboBox;
    class DisplayGroupEnumComboBox;
    class EnumComboBoxTemplate;
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
        
        void featureItemTableViewItemClicked(const QModelIndex& index);
        
        void labelModelComboBoxActivated(int index);
        
        void labelTableViewItemClicked(const QModelIndex& index);
        
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
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex;
        
        CaretDataFileSelectionComboBox* m_featureFileSelectionComboBox;
        
        CaretDataFileSelectionComboBox* m_volumeFileSelectionComboBox;
        
        QCheckBox* m_displayCheckBox;
        
        DisplayGroupEnumComboBox* m_displayGroupComboBox;

        QDoubleSpinBox* m_symbolScaleSpinBox;
        
        WuQTabWidget* m_tabWidget;
        
        static std::set<FeatureSelectionViewController*> allFeatureSelectionViewControllers;
        
        QTableView* m_featureItemTableView;
        
        QComboBox* m_labelModelSelectionComboBox;
        
        QTableView* m_labelsTableView;
        
    };
    
#ifdef __FEATURE_SELECTION_VIEW_CONTROLLER_DECLARE__
    std::set<FeatureSelectionViewController*> FeatureSelectionViewController::allFeatureSelectionViewControllers;
#endif // __FEATURE_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__FEATURE_SELECTION_VIEW_CONTROLLER__H_
