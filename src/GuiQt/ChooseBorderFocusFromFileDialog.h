#ifndef __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_H__
#define __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_H__

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

#include "DataFileTypeEnum.h"
#include "WuQDialogModal.h"

class QAbstractButton;
class QButtonGroup;
class QComboBox;
class QLineEdit;
class QRadioButton;
class QStackedWidget;

namespace caret {

    class BorderFile;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class FociFile;
    
    class ChooseBorderFocusFromFileDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        enum class FileMode {
            BORDER,
            FOCUS
        };
        
        ChooseBorderFocusFromFileDialog(const FileMode fileMode,
                                        QWidget* parent = 0);
        
        virtual ~ChooseBorderFocusFromFileDialog();
        
        ChooseBorderFocusFromFileDialog(const ChooseBorderFocusFromFileDialog&) = delete;

        ChooseBorderFocusFromFileDialog& operator=(const ChooseBorderFocusFromFileDialog&) = delete;
        
        void setSelections(const AString& filename,
                           const AString& className,
                           const AString& name);
        
        AString getSelectedFileName() const;
        
        AString getSelectedName() const;
        
        AString getSelectedClass() const;
        
        virtual void okButtonClicked() override;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void dataFileSelected(CaretDataFile* caretDataFile);
        
        void modeButtonGroupButtonClicked(QAbstractButton* button);
        
        void classWidgetClassComboBoxActivated(int index);
        
        void nameWidgetNameComboBoxActivated(int index);
        
    private:
        enum class SelectionMode {
            CLASS,
            NAME
        };
        
        enum class ToolTip {
            MODE_CLASS_BUTTON,
            MODE_NAME_BUTTON,
            CLASS_SELECTION_CLASS_COMBO_BOX,
            CLASS_SELECTION_NAME_COMBO_BOX,
            NAME_SELECTION_NAME_COMBO_BOX,
            NAME_SELECTION_CLASS_COMBO_BOX
        };
        
        QWidget* createClassAndNameSelectionWidget();
        
        QWidget* createModeSelectionWidget();
        
        void loadComboBox(QComboBox* comboBox,
                          const std::vector<AString>& itemNames);
        
        void setToolTipText(QWidget* widget,
                            const ToolTip toolTip) const;
                
        void setComboBox(QComboBox* comboBox,
                         const QString& text);
        
        const FileMode m_fileMode;
        
        SelectionMode m_selectionMode = SelectionMode::CLASS;
        
        DataFileTypeEnum::Enum m_dataFileType = DataFileTypeEnum::UNKNOWN;
        
        CaretDataFileSelectionModel* m_fileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_fileSelectionComboBox;
        
        QLineEdit* m_classLineEdit;
        
        QLineEdit* m_nameLineEdit;
        
        QButtonGroup* m_modeButtonGroup;
        
        QRadioButton* m_modeNameRadioButton;
        
        QRadioButton* m_modeClassRadioButton;
        
        QStackedWidget* m_classOrNameStackedWidget;
        
        QWidget* m_classWidget;
        
        QComboBox* m_classWidgetClassSelectionComboBox;
        
        QComboBox* m_classWidgetNameSelectionComboBox;
        
        QWidget* m_nameWidget;
        
        QComboBox* m_nameWidgetClassSelectionComboBox;
        
        QComboBox* m_nameWidgetNameSelectionComboBox;
        
        BorderFile* m_borderFile = NULL;
        
        FociFile* m_fociFile = NULL;
        
        std::vector<AString> m_allNames;
        
        std::vector<AString> m_allClassNames;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__CHOOSE_BORDER_FOCUS_FROM_FILE_DIALOG_H__
