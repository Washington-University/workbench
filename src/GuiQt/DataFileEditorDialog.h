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
    class Border;
    class BorderFile;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class DataFileEditorItem;
    class DataFileEditorModel;
    class FociFile;
    class Focus;

    class DataFileEditorDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class DataType {
            BORDERS,
            FOCI
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
        
        void dataFileSelected(CaretDataFile* caretDataFile);
        
        void copyMoveToFileSelected(CaretDataFile* caretDataFile);
        
        void copyActionTriggered();
        
        void deleteActionTriggered();
        
        void moveActionTriggered();
        
    private:
        void createDialog();
        
        FociFile* getSelectedFociFile();
        
        std::vector<DataFileEditorItem*> getSelectedItems() const;
        
        std::vector<int32_t> getSelectedRowIndicesSorted() const;
        
        std::vector<const Border*> getSelectedBorders() const;
        
        std::vector<const Focus*> getSelectedFoci() const;
        
        void updateGraphicsAndUserInterface();
        
        bool copyBorders();
        
        bool copyFoci();
        
        void updateCopyMoveToActions();
        
        const DataType m_dataType;
        
        std::unique_ptr<DataFileEditorModel> m_model;
        
        DataFileTypeEnum::Enum m_dataFileType = DataFileTypeEnum::UNKNOWN;
        
        QDialogButtonBox* m_buttonBox;
        
        std::unique_ptr<CaretDataFileSelectionModel> m_viewFileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_viewFileSelectionComboBox;
        
        std::unique_ptr<CaretDataFileSelectionModel> m_copyMoveToFileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_copyMoveToFileSelectionComboBox;
        
        QTreeView* m_treeView = NULL;
        
        QAction* m_copyAction = NULL;
        
        QAction* m_deleteAction = NULL;
        
        QAction* m_moveAction = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__DATA_FILE_EDITOR_DIALOG_H__
