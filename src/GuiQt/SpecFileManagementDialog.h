#ifndef __SPEC_FILE_MANAGEMENT_DIALOG_H__
#define __SPEC_FILE_MANAGEMENT_DIALOG_H__

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

#include <set>

#include <QLabel>

#include "CaretObject.h"
#include "DataFileTypeEnum.h"
#include "SpecFileDialogViewFilesTypeEnum.h"
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
    class BrainBrowserWindow;
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
                                          BrainBrowserWindow* parent);
        
        static void runManageFilesDialog(Brain* brain,
                                         BrainBrowserWindow* parent);
        
        static bool runSaveFilesDialogWhileQuittingWorkbench(Brain* brain,
                                         BrainBrowserWindow* parent);
        
        virtual ~SpecFileManagementDialog();
        
    protected:
        WuQDialogModal::DialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
    private slots:
        void toolBarFileTypeActionTriggered(QAction* action);
        
        void toolBarStructuresActionTriggered(QAction* action);
        
        void toolBarSelectFilesActionTriggered(QAction* action);
        
        void toolBarManageFilesLoadedNotLoadedActionTriggered(QAction* action);
        
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
            MANAGE_FILES_LOADED_MODIFIED,
            MANAGE_FILES_LOADED_NOT_MODIFIED,
            MANAGE_FILES_NOT_LOADED
        };
        
        SpecFileManagementDialog(const Mode dialogMode,
                                 Brain* brain,
                                 SpecFile* specFile,
                                 const AString& dialogTitle,
                                 BrainBrowserWindow* parent);
        
        SpecFileManagementDialog(const SpecFileManagementDialog&);

        SpecFileManagementDialog& operator=(const SpecFileManagementDialog&);
        
        BrainBrowserWindow* m_parentBrainBrowserWindow;
        
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
        
        void updateAnnotationSceneFileRowInTable();
        
        void sortFileContent();
        
        void setTableColumnLabels();
        
        virtual void okButtonClicked();
        
        void okButtonClickedOpenSpecFile();
        
        bool okButtonClickedManageAndSaveFiles();
        
        void changeFileName(QWidget* parent,
                            SpecFileDataFile* specFileDataFile,
                            CaretDataFile* caretDataFile);
        
        void showFileInformation(CaretDataFile* caretDataFile);
        
        void copyMoveFileContent(QWidget* parent,
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
        
        void getFilterSelections(SpecFileDialogViewFilesTypeEnum::Enum& viewFilesTypeOut,
                                 DataFileTypeEnum::Enum& filteredDataFileTypeOut,
                                 StructureEnum::Enum& filteredStructureTypeOut) const;
        
        void setFilterSelections(const SpecFileDialogViewFilesTypeEnum::Enum viewFilesType,
                                 const DataFileTypeEnum::Enum filteredDataFileType,
                                 const StructureEnum::Enum filteredStructureType);
        
        void clearSpecFileManagementDialogRowContent();
        
        void enableLoadOrSaveButton();
        
        void copyFilePathToClipboard(const SpecFileDataFile* specFileDataFile,
                                     const CaretDataFile* caretDataFile);
        
        // ADD_NEW_MEMBERS_HERE
        
        const Mode m_dialogMode;

        Brain* m_brain;
        
        SpecFile* m_specFile;
    
        QPushButton* m_reloadAllDataFilesPushButton = NULL;
        
        QPushButton* m_loadScenesPushButton = NULL;
        
        QActionGroup* m_fileTypesActionGroup;
        
        QActionGroup* m_fileSelectionActionGroup;
        
        QActionGroup* m_manageFilesLoadedNotLoadedActionGroup;
        
        QActionGroup* m_structureActionGroup;
        
        std::vector<SpecFileManagementDialogRowContent*> m_tableRowDataFileContent;
        
        int m_specFileTableRowIndex;
        
        int m_sceneAnnotationFileRowIndex;
        
        QSignalMapper* m_fileReloadOrOpenFileActionSignalMapper;
        
        QSignalMapper* m_fileCloseFileActionSignalMapper;
        
        QSignalMapper* m_fileOptionsActionSignalMapper;
        
        int m_specFileDataFileCounter;
        
        QTableWidget* m_filesTableWidget;
        
        SpecFileManagementDialogRowContent::Sorting m_fileSorting;
        
        QIcon* m_iconOptions;
        
        QIcon* m_iconOpenFile;
        
        QIcon* m_iconReloadFile;
        
        QIcon* m_iconCloseFile;
        
        std::set<const CaretDataFile*> m_displayedDataFiles;
        
        static QByteArray s_manageFilesGeometry;
        static SpecFileDialogViewFilesTypeEnum::Enum s_manageFilesViewFilesType;
        static DataFileTypeEnum::Enum s_manageFilesFilteredDataFileType;
        static StructureEnum::Enum s_manageFilesFilteredStructureType;
        
        static const int SHOW_FILES_ALL;
        static const int SHOW_FILES_NONE;

        int m_COLUMN_LOAD_CHECKBOX;
        int m_COLUMN_SAVE_CHECKBOX;
        int m_COLUMN_STATUS_LABEL;
        int m_COLUMN_DISPLAYED_LABEL;
        int m_COLUMN_IN_SPEC_FILE_CHECKBOX;
        int m_COLUMN_READ_BUTTON;
        int m_COLUMN_CLOSE_BUTTON;
        int m_COLUMN_OPTIONS_TOOLBUTTON;
        int m_COLUMN_DATA_FILE_TYPE_LABEL;
        int m_COLUMN_STRUCTURE;
        int m_COLUMN_FILE_NAME_LABEL;
        int m_COLUMN_COUNT;
        
        friend class SpecFileManagementDialogRowContent;
    };
    
#ifdef __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__
    QByteArray SpecFileManagementDialog::s_manageFilesGeometry;
    SpecFileDialogViewFilesTypeEnum::Enum SpecFileManagementDialog::s_manageFilesViewFilesType = SpecFileDialogViewFilesTypeEnum::VIEW_FILES_ALL;
    DataFileTypeEnum::Enum SpecFileManagementDialog::s_manageFilesFilteredDataFileType = DataFileTypeEnum::UNKNOWN;
    StructureEnum::Enum SpecFileManagementDialog::s_manageFilesFilteredStructureType = StructureEnum::ALL;
    
    const int SpecFileManagementDialog::SHOW_FILES_ALL = -1;
    const int SpecFileManagementDialog::SHOW_FILES_NONE = -2;
#endif // __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__

} // namespace
#endif  //__SPEC_FILE_MANAGEMENT_DIALOG_H__
