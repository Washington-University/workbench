#ifndef __SPEC_FILE_MANAGEMENT_DIALOG_H__
#define __SPEC_FILE_MANAGEMENT_DIALOG_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#include <QLabel>

#include "CaretObject.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QActionGroup;
class QIcon;
class QLabel;
class QSignalMapper;
class QTableWidget;
class QTableWidgetItem;
class QToolBar;

namespace caret {

    class Brain;
    class CaretDataFile;
    class SpecFile;
    class SpecFileDataFile;
    class SpecFileDataFileTypeGroup;
    class SpecFileManagementDialogRowContent;
    
    /**
     * Data in a row of the table widget
     */
    class SpecFileManagementDialogRowContent : public CaretObject {
    public:
        /**
         * Type of sorting
         */
        enum Sorting {
            SORTING_TYPE_STRUCTURE_NAME,
            SORTING_STRUCTURE_TYPE_NAME,
            SORTING_NAME
        };
        
        SpecFileManagementDialogRowContent(SpecFileDataFileTypeGroup* specFileDataFileTypeGroup,
                                SpecFileDataFile* specFileDataFile);
        
        ~SpecFileManagementDialogRowContent();
        
        void setSortingKey(const Sorting sorting);
        
        int m_tableRowIndex;
        
        SpecFileDataFileTypeGroup* m_specFileDataFileTypeGroup;
        
        SpecFileDataFile* m_specFileDataFile;
        
        QString m_sortingKey;
        
        static bool lessThanForSorting(const SpecFileManagementDialogRowContent* item1,
                                       const SpecFileManagementDialogRowContent* item2);
    };
    
    class SpecFileManagementDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static bool runOpenSpecFileDialog(Brain* brain,
                                          SpecFile* specFile,
                                          QWidget* parent);
        
        static void runManageFilesDialog(Brain* brain,
                                         QWidget* parent);
        
        static bool runSaveFilesDialogWhileQuittingWorkbench(Brain* brain,
                                         QWidget* parent);
        
        virtual ~SpecFileManagementDialog();
        
    protected:
        WuQDialogModal::ModalDialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
    private slots:
        void toolBarFileTypeActionTriggered(QAction* action);
        
        void toolBarStructuresActionTriggered(QAction* action);
        
        void toolBarSelectFilesActionTriggered(QAction* action);
        
        void toolBarManageFilesLoadedNotLoadedActionTriggered(QAction* action);
        
//        void chooseSpecFileNameActionTriggered();

//        void specFileOptionsActionTriggered();
        
        void fileReloadOrOpenFileActionSelected(int indx);
        
        void fileRemoveActionSelected(int indx);
        
        void fileOptionsActionSelected(int indx);
        
        void filesTableWidgetCellChanged(int row, int column);

        void horizontalHeaderSelectedForSorting(int logicalIndex);
        
    private:
        enum Mode {
            MODE_MANAGE_FILES,
            MODE_OPEN_SPEC_FILE,
            MODE_SAVE_FILES_WHILE_QUITTING
        };
        
        enum ManageFilesDisplay {
            MANAGE_FILES_ALL,
            MANAGE_FILES_LOADED,
            MANAGE_FILES_NOT_LOADED
        };
        
        SpecFileManagementDialog(const Mode dialogMode,
                                 Brain* brain,
                                 SpecFile* specFile,
                                 const AString& dialogTitle,
                                 QWidget* parent);
        
        SpecFileManagementDialog(const SpecFileManagementDialog&);

        SpecFileManagementDialog& operator=(const SpecFileManagementDialog&);
        
        QToolBar* createFilesTypesToolBar(QLabel* &labelOut);
        
        QToolBar* createFilesSelectionToolBar(QLabel* &labelOut);
        
        QToolBar* createManageFilesLoadedNotLoadedToolBar(QLabel* &labelOut);
        
        QToolBar* createStructureToolBar(QLabel* &labelOut);
        
        QToolBar* createToolBarWithActionGroup(const QString& text,
                                               QLabel* &labelOut,
                                               QActionGroup* actionGroup);
        
        static AString getEditedDataFileTypeName(const DataFileTypeEnum::Enum dataFileType);
        
        SpecFileManagementDialogRowContent* getFileContentInRow(const int rowIndex);
        
        void updateGraphicWindowsAndUserInterface();
        
        void getDataFileContentFromSpecFile();
        
        void loadSpecFileContentIntoDialog();
        
        void loadDialogContentIntoSpecFile();
        
        void updateSpecFileRowInTable();
        
        void sortFileContent();
        
        void setTableColumnLabels();
        
        virtual void okButtonClicked();
        
        void okButtonClickedOpenSpecFile();
        
        bool okButtonClickedManageAndSaveFiles();
        
        void changeFileName(QWidget* parent,
                            SpecFileDataFile* specFileDataFile,
                            CaretDataFile* caretDataFile);
        
        void updateTableDimensionsToFitFiles();
        
        QTableWidgetItem* createHeaderTextItem(const QString& text);
        
        QTableWidgetItem* createTextItem();
        
        QTableWidgetItem* createCheckableItem();
        
        AString writeSpecFile(const bool writeOnlyIfModified);
        
        QTableWidgetItem* getTableWidgetItem(const int rowIndex,
                                             const int columnIndex);
        
        void setTableWidgetItem(const int rowIndex,
                                const int columnIndex,
                                QTableWidgetItem* item);
        
        void getFilterSelections(ManageFilesDisplay& manageFilesDisplayOut,
                                 DataFileTypeEnum::Enum& filteredDataFileTypeOut,
                                 StructureEnum::Enum& filteredStructureTypeOut) const;
        
        void clearSpecFileManagementDialogRowContent();
        
        void enableLoadOrSaveButton();
        
        // ADD_NEW_MEMBERS_HERE
        
        const Mode m_dialogMode;

        Brain* m_brain;
        
        SpecFile* m_specFile;
    
        QPushButton* m_loadScenesPushButton;
        
        QActionGroup* m_fileTypesActionGroup;
        
        QActionGroup* m_fileSelectionActionGroup;
        
        QActionGroup* m_manageFilesLoadedNotLoadedActionGroup;
        
        QActionGroup* m_structureActionGroup;
        
        std::vector<SpecFileManagementDialogRowContent*> m_tableRowDataFileContent;
        
        int m_specFileTableRowIndex;
                
        QSignalMapper* m_fileReloadOrOpenFileActionSignalMapper;
        
        QSignalMapper* m_fileRemoveFileActionSignalMapper;
        
        QSignalMapper* m_fileOptionsActionSignalMapper;
        
        int m_specFileDataFileCounter;
        
        QTableWidget* m_filesTableWidget;
        
        SpecFileManagementDialogRowContent::Sorting m_fileSorting;
        
        QIcon* m_iconOptions;
        
        QIcon* m_iconOpenFile;
        
        QIcon* m_iconReloadFile;
        
        QIcon* m_iconRemoveFile;
        
        static const int SHOW_FILES_ALL;
        static const int SHOW_FILES_NONE;

        int m_COLUMN_LOAD_CHECKBOX;
        int m_COLUMN_SAVE_CHECKBOX;
        int m_COLUMN_STATUS_LABEL;
        int m_COLUMN_DISPLAYED_LABEL;
        int m_COLUMN_IN_SPEC_FILE_CHECKBOX;
        int m_COLUMN_READ_BUTTON;
        int m_COLUMN_REMOVE_BUTTON;
        int m_COLUMN_OPTIONS_TOOLBUTTON;
        int m_COLUMN_DATA_FILE_TYPE_LABEL;
        int m_COLUMN_STRUCTURE;
        int m_COLUMN_FILE_NAME_LABEL;
        int m_COLUMN_COUNT;
        
        friend class SpecFileManagementDialogRowContent;
    };
    
#ifdef __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__
    const int SpecFileManagementDialog::SHOW_FILES_ALL = -1;
    const int SpecFileManagementDialog::SHOW_FILES_NONE = -2;
#endif // __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__

} // namespace
#endif  //__SPEC_FILE_MANAGEMENT_DIALOG_H__
