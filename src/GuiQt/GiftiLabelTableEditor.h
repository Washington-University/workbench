#ifndef __GIFTI_LABEL_TABLE_EDITOR__H_
#define __GIFTI_LABEL_TABLE_EDITOR__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "WuQDialogModal.h"

class QAction;
class QLineEdit;
class QListWidget;
class QListWidgetItem;

namespace caret {

    class BorderFile;
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
            OPTION_UNASSIGNED_LABEL_HIDDEN = 1
        };
        
        GiftiLabelTableEditor(GiftiLabelTable* giftiLableTable,
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

    private:
        void initializeDialog(GiftiLabelTable* giftiLabelTable,
                              const uint32_t options);
        
        void loadLabels(const AString& selectedName,
                        const bool usePreviouslySelectedIndex);
        
        GiftiLabel* getSelectedLabel();
        
        void setWidgetItemIconColor(QListWidgetItem* item,
                                    const float rgba[4]);
        
        QListWidget* m_labelSelectionListWidget;
        
        BorderFile* m_borderFile;
        
        FociFile* m_fociFile;
        
        GiftiLabelTable* m_giftiLableTable;
        
        ColorEditorWidget* m_colorEditorWidget;
        
        QLineEdit* m_labelNameLineEdit;
        
        AString m_lastSelectedLabelName;
        
        GiftiLabel* m_undoGiftiLabel;
        
        bool m_showUnassignedLabelInEditor;
        
        WuQWidgetObjectGroup* m_editingGroup;
    };
    
#ifdef __GIFTI_LABEL_TABLE_EDITOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GIFTI_LABEL_TABLE_EDITOR_DECLARE__

} // namespace
#endif  //__GIFTI_LABEL_TABLE_EDITOR__H_
