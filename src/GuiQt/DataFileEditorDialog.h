#ifndef __DATA_FILE_EDITOR_DIALOG_H__
#define __DATA_FILE_EDITOR_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include <vector>

#include <QDialog>

#include "DataFileTypeEnum.h"

class QAbstractButton;
class QAction;
class QDialogButtonBox;
class QTreeView;

namespace caret {
    class Annotation;
    class Border;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class DataFileEditorItem;
    class DataFileEditorModel;
    class Focus;

    class DataFileEditorDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class DataType {
            ANNOTATIONS,
            BORDERS,
            FOCI,
            SAMPLES
        };
        
        enum EditorIndex : int32_t {
            LEFT  = 0,
            RIGHT = 1
        };
        
        DataFileEditorDialog(const DataType dataType,
                             QWidget* parent = 0);
        
        virtual ~DataFileEditorDialog();
        
        DataFileEditorDialog(const DataFileEditorDialog&) = delete;

        DataFileEditorDialog& operator=(const DataFileEditorDialog&) = delete;
        
        virtual void done(int result) override;

        // ADD_NEW_METHODS_HERE

    private slots:
        void dialogButtonClicked(QAbstractButton* buttonClicked);
        
        void fileSelected(const EditorIndex editorIndex,
                          CaretDataFile* caretDataFile);
        
        void copyActionSelected(const EditorIndex editorIndex);
        
        void moveActionSelected(const EditorIndex editorIndex);
        
        void deleteActionSelected(const EditorIndex editorIndex);
        
    private:
        class EditorWidgets {
        public:
            EditorWidgets(CaretDataFileSelectionModel* fileSelectionModel,
                          CaretDataFileSelectionComboBox* fileSelectionComboBox,
                          QTreeView* treeView,
                          QAction* copyAction,
                          QAction* moveAction,
                          QAction* deleteAction);
            
            ~EditorWidgets();
            
            void setModel(DataFileEditorModel* model);
            
            CaretDataFile* getSelectedFile();
            
            std::unique_ptr<DataFileEditorModel> m_model;

            std::unique_ptr<CaretDataFileSelectionModel> m_fileSelectionModel;
            
            CaretDataFileSelectionComboBox* m_fileSelectionComboBox;
            
            QTreeView* m_treeView = NULL;
            
            QAction* m_copyAction = NULL;
            
            QAction* m_deleteAction = NULL;
            
            QAction* m_moveAction = NULL;
        };
        
        void createDialog();
        
        std::pair<QWidget*,EditorWidgets*> createEditor(const EditorIndex editorIndex);
        
        std::vector<int32_t> getSelectedRowIndicesSorted(const EditorIndex editorIndex) const;
        
        std::vector<DataFileEditorItem*> getSelectedItems(const EditorIndex editorIndex) const;

        std::vector<const Annotation*> getSelectedAnnotations(const EditorIndex editorIndex) const;
        
        std::vector<const Border*> getSelectedBorders(const EditorIndex editorIndex) const;
        
        std::vector<const Focus*> getSelectedFoci(const EditorIndex editorIndex) const;
        
        std::vector<const Annotation*> getSelectedSamples(const EditorIndex editorIndex) const;
        
        EditorWidgets* getEditor(const EditorIndex editorIndex) const;
        
        void updateGraphicsAndUserInterface();
        
        bool copyAnnotations(const EditorIndex sourceEditorIndex,
                             const EditorIndex destinationEditorIndex);
        
        bool copyBorders(const EditorIndex sourceEditorIndex,
                         const EditorIndex destinationEditorIndex);
        
        bool copyFoci(const EditorIndex sourceEditorIndex,
                      const EditorIndex destinationEditorIndex);
        
        bool copySamples(const EditorIndex sourceEditorIndex,
                         const EditorIndex destinationEditorIndex);
        
        void updateCopyMoveDeleteActions();
        
        void updateOtherModelView(const EditorIndex editorIndex,
                                  const CaretDataFile* caretDataFile);
        
        const DataType m_dataType;
        
        DataFileTypeEnum::Enum m_dataFileType = DataFileTypeEnum::UNKNOWN;
        
        QDialogButtonBox* m_buttonBox;
        
        std::unique_ptr<EditorWidgets> m_leftEditor = NULL;
        
        std::unique_ptr<EditorWidgets> m_rightEditor = NULL;
                
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__DATA_FILE_EDITOR_DIALOG_H__
