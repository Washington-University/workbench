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


#include "WuQDialogModal.h"

class QAction;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

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
            OPTION_ADD_APPLY_BUTTON = 2
        };
        
        GiftiLabelTableEditor(GiftiLabelTable* giftiLableTable,
                              const AString& dialogTitle,
                              const uint32_t options,
                              QWidget* parent);
        
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
        
        void listWidgetLabelSelected(int row);
        
        void listWidgetLabelSelected(QListWidgetItem* item);
        
        void colorEditorColorChanged(const float*);
        
        void labelNameLineEditTextEdited(const QString&);
        
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
        
        QListWidget* m_labelSelectionListWidget;
        
        BorderFile* m_borderFile;
        
        FociFile* m_fociFile;
        
        CaretMappableDataFile* m_caretMappableDataFile;
        
        int32_t m_caretMappableDataFileMapIndex;
        
        GiftiLabelTable* m_giftiLableTable;
        
        ColorEditorWidget* m_colorEditorWidget;
        
        QLineEdit* m_labelNameLineEdit;
        
        AString m_lastSelectedLabelName;
        
        GiftiLabel* m_undoGiftiLabel;
        
        bool m_showUnassignedLabelInEditor;
        
        WuQWidgetObjectGroup* m_editingGroup;
        
        QPushButton* m_applyPushButton;
    };
    
#ifdef __GIFTI_LABEL_TABLE_EDITOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GIFTI_LABEL_TABLE_EDITOR_DECLARE__

} // namespace
#endif  //__GIFTI_LABEL_TABLE_EDITOR__H_
