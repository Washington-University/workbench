#ifndef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_VIEW_CONTROLLER_H__
#define __ANNOTATION_TEXT_SUBSTITUTION_LAYER_VIEW_CONTROLLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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



#include <memory>

#include <QObject>

#include "AString.h"

class QAction;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QMenu;
class QSpinBox;
class QToolButton;

namespace caret {

    class AnnotationTextSubstitutionLayer;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class MapYokingGroupComboBox;
    class WuQGridLayoutGroup;
    
    class AnnotationTextSubstitutionLayerViewController : public QObject {
        
        Q_OBJECT

    public:
        AnnotationTextSubstitutionLayerViewController(QGridLayout* gridLayout,
                                                      const int32_t layerIndex,
                                                      const QString& parentObjectName,
                                                      QObject* parent);
        
        virtual ~AnnotationTextSubstitutionLayerViewController();
        
        AnnotationTextSubstitutionLayerViewController(const AnnotationTextSubstitutionLayerViewController&) = delete;

        AnnotationTextSubstitutionLayerViewController& operator=(const AnnotationTextSubstitutionLayerViewController&) = delete;
        
        void updateViewController(AnnotationTextSubstitutionLayer* layer);
        
        void setVisible(bool visible);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void enabledCheckBoxClicked(bool checked);
        
        void groupIdComboBoxActivated(const int index);
        
        void rowIndexSpinBoxValueChanged(const int index);
        
        void yokingGroupComboBoxActivated();
        
        void fileComboBoxSelected(CaretDataFile* caretDataFile);
        
    private:
        QMenu* createConstructionMenu(QWidget* parent,
                                      const AString& descriptivePrefix,
                                      const AString& menuActionNamePrefix);
        
        void updateGraphics();
        
        void updateUserInterface();
        
        const int32_t m_layerIndex;
        
        AnnotationTextSubstitutionLayer* m_layer = NULL;
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
        QCheckBox* m_enabledCheckBox;
        
        QToolButton* m_constructionToolButton;
        
        QAction* m_constructionAction;
        
        QComboBox* m_groupIdComboBox;
        
        MapYokingGroupComboBox* m_yokingComboBox;
        
        QSpinBox* m_rowIndexSpinBox;
        
        CaretDataFileSelectionComboBox* m_fileSelectionComboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_SUBSTITUTION_LAYER_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_SUBSTITUTION_LAYER_VIEW_CONTROLLER_H__
