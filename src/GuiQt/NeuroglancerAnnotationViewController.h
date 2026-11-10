#ifndef __NEUROGLANCER_ANNOTATION_VIEW_CONTROLLER__H_
#define __NEUROGLANCER_ANNOTATION_VIEW_CONTROLLER__H_

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

namespace caret {

    class CaretDataFileSelectionComboBox;
    class ComboBoxItemInterface;
    class ComboBoxItemInterfaceComboBox;
    class DisplayGroupEnumComboBox;
    class FeatureLabelModel;
    class NeuroglancerAnnotationFile;
    class WuQTabWidget;
    
    class NeuroglancerAnnotationViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        NeuroglancerAnnotationViewController(const int32_t browserWindowIndex,
                                                       const QString& parentObjectName,
                                                       QWidget* parent = 0);

        virtual ~NeuroglancerAnnotationViewController();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void colorPropertySelected(ComboBoxItemInterface* itemInterface);
        
        void processSelectionChanges();
        
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
        void processAttributesChanges();
        
        void labelModelComboBoxActivated(int index);
        
        void labelTableViewItemClicked(const QModelIndex& index);

        void labelFindActionTriggered();
        
        void labelNextActionTriggered();
        
        void labelFindTextLineEditTextChanged(const QString& text);
        
        void labelScrollTreeViewToFindItem();
        
        void nXpZpYOrientationCheckBoxClicked(bool clicked);
        
    private:
        NeuroglancerAnnotationViewController(const NeuroglancerAnnotationViewController&);

        NeuroglancerAnnotationViewController& operator=(const NeuroglancerAnnotationViewController&);

        void updateViewController();
        
        void updateOtherViewControllers();
        
        void updateLabelWidget();
        
        void updateTransformationWidget();
        
        void labelsAllOnOffButtonClicked(const bool onFlag);
        
        QWidget* createAttributesWidget();
        
        QWidget* createLabelsWidget();
        
        QWidget* createTransformationWidget();
        
        void transformationSpinBoxValueChanged(const int32_t row,
                                               const int32_t column,
                                               const double value);
        
        NeuroglancerAnnotationFile* getSelectedFile();
        
        FeatureLabelModel* getSelectedLabelModel();
        
        void labelResetFindItemsAndFindText();
        
        void labelResetFindItems();
        
        const QString m_objectNamePrefix;
        
        int32_t m_browserWindowIndex;
        
        CaretDataFileSelectionComboBox* m_fileSelectionComboBox;
        
        ComboBoxItemInterfaceComboBox* m_colorPropertySelectionComboBox;
        
        QCheckBox* m_displayCheckBox;
        
        DisplayGroupEnumComboBox* m_displayGroupComboBox;

        QDoubleSpinBox* m_symbolScaleSpinBox;
        
        QDoubleSpinBox* m_distanceToVolumeSliceSpinBox;
        
        WuQTabWidget* m_tabWidget;
        
        static std::set<NeuroglancerAnnotationViewController*> allNeuroglancerAnnotationViewControllers;
        
        QComboBox* m_labelModelSelectionComboBox;
        
        QTableView* m_labelsTableView;
        
        QAction* m_labelFindAction;
        QAction* m_labelNextAction;
        QLineEdit* m_labelFindTextLineEdit;
        /*
         * Find model indices are in proxy model, not the model from the file
         */
        std::vector<QModelIndex> m_labelFindItemModelIndices;
        
        int32_t m_labelFindItemsCurrentIndex = 0;
        
        QDoubleSpinBox* m_matrixSpinBoxes[4][4];
        
        QCheckBox* m_nXpZpYOrientationCheckBox;
    };
    
#ifdef __NEUROGLANCER_ANNOTATION_VIEW_CONTROLLER_DECLARE__
    std::set<NeuroglancerAnnotationViewController*> NeuroglancerAnnotationViewController::allNeuroglancerAnnotationViewControllers;
#endif // __NEUROGLANCER_ANNOTATION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_VIEW_CONTROLLER__H_
