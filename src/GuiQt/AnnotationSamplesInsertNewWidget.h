#ifndef __ANNOTATION_SAMPLES_INSERT_NEW_WIDGET_H__
#define __ANNOTATION_SAMPLES_INSERT_NEW_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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
#include <memory>

#include <QWidget>

#include "EventListenerInterface.h"
#include "UserInputModeEnum.h"

class QComboBox;
class QLabel;
class QSpinBox;

namespace caret {
    class SamplesFile;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class EnumComboBoxTemplate;
    

    class AnnotationSamplesInsertNewWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        AnnotationSamplesInsertNewWidget(const UserInputModeEnum::Enum userInputMode,
                                         const int32_t browserWindowIndex,
                                         QWidget* parent = 0);
        
        virtual ~AnnotationSamplesInsertNewWidget();
        
        AnnotationSamplesInsertNewWidget(const AnnotationSamplesInsertNewWidget&) = delete;

        AnnotationSamplesInsertNewWidget& operator=(const AnnotationSamplesInsertNewWidget&) = delete;
        
        void updateContent();
        
        virtual void receiveEvent(Event* event);
        
    private slots:
        void fileSelectionComboBoxFileSelected(CaretDataFile* caretDataFile);

        void newFileActionTriggered();
        
        void saveFileActionTriggered();
        
        void newSampleActionTriggered();
        
        void samplesDrawingModeEnumComboBoxItemActivated();
        
        void lowerSliceOffsetSpinBoxValueChanged(int value);
        
        void upperSliceOffsetSpinBoxValueChanged(int value);
        
        // ADD_NEW_METHODS_HERE

    private:
        std::unique_ptr<CaretDataFileSelectionModel> m_fileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_fileSelectionComboBox = NULL;
        
        SamplesFile* getSelectedSamplesFile() const;
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_browserWindowIndex;
        
        QAction* m_newSampleAction = NULL;
        
        QLabel* m_slicesLabel = NULL;
        
        EnumComboBoxTemplate* m_samplesDrawingModeEnumComboBox = NULL;
        
        QLabel* m_lowerSliceOffsetLabel = NULL;
        QSpinBox* m_lowerSliceOffsetSpinBox = NULL;
        
        QLabel* m_upperSliceOffsetLabel = NULL;
        QSpinBox* m_upperSliceOffsetSpinBox = NULL;
        
        QAction* m_saveFileAction = NULL;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SAMPLES_INSERT_NEW_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_SAMPLES_INSERT_NEW_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_SAMPLES_INSERT_NEW_WIDGET_H__
