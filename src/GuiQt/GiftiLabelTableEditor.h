#ifndef __GIFTI_LABEL_TABLE_EDITOR__H_
#define __GIFTI_LABEL_TABLE_EDITOR__H_

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

#include <map>

#include "CaretPointer.h"
#include "WuQDialogModal.h"

class QAction;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QToolButton;

namespace caret {

    class BorderFile;
    class CaretMappableDataFile;
    class ColorEditorWidget;
    class FociFile;
    class GiftiLabel;
    class GiftiLabelTable;
    class WuQWidgetObjectGroup;
    
    class GiftiLabelTableEditor : public WuQDialogModal {
        Q_OBJECT
        
    public:
        enum Options {
            /** 
             * No options 
             */
            OPTION_NONE = 0,
            /** 
             * Hide the unassigned label so that it is not shown in editor.
             * May be bitwise OR'ed with other options.
             */
            OPTION_UNASSIGNED_LABEL_HIDDEN = 1,
            /**
             * Add an apply button so that the graphics windows can be
             * updated without having to close the dialog.
             */
            OPTION_ADD_APPLY_BUTTON = 2,
            /** 
             * Add GUI components that allow the user to edit the
             * key assigned to the selected label.
             */
            OPTION_ADD_KEY_EDITING = 4
        };
        
//        GiftiLabelTableEditor(GiftiLabelTable* giftiLableTable,
//                              const AString& dialogTitle,
//                              const uint32_t options,
//                              QWidget* parent);
        
        GiftiLabelTableEditor(CaretMappableDataFile* caretMappableDataFile,
                              const int32_t mapIndex,
                              const AString& dialogTitle,
                              const uint32_t options,
                              QWidget* parent);
        
        GiftiLabelTableEditor(FociFile* fociFile,
                              GiftiLabelTable* giftiLableTable,
                              const AString& dialogTitle,
                              const uint32_t options,
                              QWidget* parent);
        
        GiftiLabelTableEditor(BorderFile* borderFile,
                              GiftiLabelTable* giftiLableTable,
                              const AString& dialogTitle,
                              const uint32_t options,
                              QWidget* parent);
        
        virtual ~GiftiLabelTableEditor();
        
        AString getLastSelectedLabelName() const;
        
        void selectLabelWithName(const AString& labelName);
        
    private:
        GiftiLabelTableEditor(const GiftiLabelTableEditor&);

        GiftiLabelTableEditor& operator=(const GiftiLabelTableEditor&);
        
    private slots:
        void newButtonClicked();
        void deleteButtonClicked();
        void undoButtonClicked();
        void changeLabelKeyLockButtonClicked();
        
//        void listWidgetLabelSelected(int row);
        
        void listWidgetLabelSelected();
        
        void colorEditorColorChanged(const float*);
        
        void labelNameLineEditTextEdited(const QString&);
        
        void sortingLabelsActivated();
        
    protected:
        virtual void okButtonClicked();

        DialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
    private:
        
        void initializeDialog(GiftiLabelTable* giftiLabelTable,
                              const uint32_t options);
        
        void loadLabels(const AString& selectedName,
                        const bool usePreviouslySelectedIndex);
        
        GiftiLabel* getSelectedLabel();
        
        void setWidgetItemIconColor(QListWidgetItem* item,
                                    const float rgba[4]);
        
        void processApplyButton();
        
        void allowLabelDataEditing(const bool allowEditingFlag);
        
        QListWidget* m_labelSelectionListWidget;
        
        BorderFile* m_borderFile;
        
        FociFile* m_fociFile;
        
        CaretMappableDataFile* m_caretMappableDataFile;
        
        int32_t m_caretMappableDataFileMapIndex;
        
        GiftiLabelTable* m_giftiLableTable;
        
        ColorEditorWidget* m_colorEditorWidget;
        
        QLineEdit* m_labelNameLineEdit;
        
        QLineEdit* m_keyValueLineEdit;
        
        QToolButton* m_changeKeyValueToolButton;
        
        AString m_lastSelectedLabelName;
        
        GiftiLabel* m_undoGiftiLabel;
        
        bool m_showUnassignedLabelInEditor;
        
        WuQWidgetObjectGroup* m_editingGroup;
        
        QPushButton* m_applyPushButton;
        
        QComboBox* m_sortLabelsByComboBox;
        
        struct PreviousSelections {
            AString m_sortingName;
            AString m_selectedLabelName;
        };
        
        static std::map<GiftiLabelTable*, PreviousSelections> s_previousSelections;
        
        static const AString s_SORT_COMBO_BOX_NAME_BY_KEY;
        
        static const AString s_SORT_COMBO_BOX_NAME_BY_NAME;
        
        static bool s_displayKeyEditingWarningFlag;
        
        
    };
    
    class ChangeLabelKeyDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        ChangeLabelKeyDialog(GiftiLabelTable* giftiLabelTable,
                             const GiftiLabel* giftiLabel,
                             QWidget* parent);
        
        ~ChangeLabelKeyDialog();
        
//        int32_t getNewKeyValue() const;
        
    protected:
        virtual void okButtonClicked();
        
    private:
        GiftiLabelTable*  m_giftiLabelTable;
        const GiftiLabel* m_giftiLabel;
        QLineEdit* m_labelKeyLineEdit;
    };
    
#ifdef __GIFTI_LABEL_TABLE_EDITOR_DECLARE__
    std::map<GiftiLabelTable*, GiftiLabelTableEditor::PreviousSelections> GiftiLabelTableEditor::s_previousSelections;
    const AString GiftiLabelTableEditor::s_SORT_COMBO_BOX_NAME_BY_KEY  = "Key";
    const AString GiftiLabelTableEditor::s_SORT_COMBO_BOX_NAME_BY_NAME = "Name";
    bool GiftiLabelTableEditor::s_displayKeyEditingWarningFlag = true;
#endif // __GIFTI_LABEL_TABLE_EDITOR_DECLARE__

} // namespace
#endif  //__GIFTI_LABEL_TABLE_EDITOR__H_
