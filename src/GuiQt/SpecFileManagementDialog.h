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

#include <QGroupBox>
#include <QSignalMapper>

#include "CaretObject.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QActionGroup;
class QCheckBox;
class QLabel;
class QTableWidget;
class QTableWidgetItem;
class QToolBar;
class QToolButton;

namespace caret {

    class Brain;
    class CaretDataFile;
    class DataFileTableRow;
    class GuiSpecFileDataFile;
    class GuiSpecFileDataFileTypeGroup;
    class SpecFile;
    class SpecFileDataFile;
    class SpecFileDataFileTypeGroup;
    class StructureEnumComboBox;
    class TableRowDataFileContent;
    class WuQEventBlockingFilter;
    class WuQWidgetObjectGroup;
    
    class SpecFileManagementDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static SpecFileManagementDialog* createOpenSpecFileDialogTableLayout(Brain* brain,
                                                                  SpecFile* specFile,
                                                                  QWidget* parent);
        
        static SpecFileManagementDialog* createManageFilesDialogTableLayout(Brain* brain,
                                                                 QWidget* parent);
        
        virtual ~SpecFileManagementDialog();
        
    protected:
        WuQDialogModal::ModalDialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
    private slots:
        void toolBarFileTypeActionTriggered(QAction* action);
        
        void toolBarStructuresActionTriggered(QAction* action);
        
        void toolBarSelectFilesActionTriggered(QAction* action);
        
        void toolBarManageFilesLoadedNotLoadedActionTriggered(QAction* action);
        
        void chooseSpecFileNameActionTriggered();

        void specFileOptionsActionTriggered();
        
        void fileLoadCheckBoxSelected(int indx);
        
        void fileSaveCheckBoxSelected(int indx);
        
        void fileInSpecCheckBoxSelected(int indx);
        
        void fileReloadOrOpenFileActionSelected(int indx);
        
        void fileRemoveActionSelected(int indx);
        
        void fileOptionsActionSelected(int indx);
        
        void fileSelectFileNameActionSelected(int indx);
        
        void fileStructureComboBoxSelected(int);
        
    private:
        friend class GuiSpecFileDataFileTypeGroup;
        friend class GuiSpecFileDataFile;
        
        enum Mode {
            MODE_MANAGE_FILES,
            MODE_OPEN_SPEC_FILE
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
        
        QToolBar* createFilesTypesToolBar();
        
        QToolBar* createFilesSelectionToolBar();
        
        QToolBar* createManageFilesLoadedNotLoadedToolBar();
        
        QToolBar* createStructureToolBar();
        
        QToolBar* createToolBarWithActionGroup(const QString& text,
                                               QActionGroup* actionGroup);
        
        static AString getEditedDataFileTypeName(const DataFileTypeEnum::Enum dataFileType);
        
        GuiSpecFileDataFile* getSpecFileDataFileBySignalMapperIndex(const int signalMapperIndex);
        
        void updateGraphicWindowsAndUserInterface();
        
        void loadSpecFileContentIntoDialog();
        
        void setTableColumnLabels();
        
        virtual void okButtonClicked();
        
        void okButtonClickedOpenSpecFile();
        
        bool okButtonClickedManageFiles();
        
        void changeFileName(QWidget* parent,
                            const int indx);
        
        void createOrUpdateTableDimensions();
        
        QTableWidgetItem* createTextItem();
        
        QTableWidgetItem* createCheckableItem();
        
        AString writeSpecFile(const bool writeOnlyIfModified);
        
        // ADD_NEW_MEMBERS_HERE
        
        const Mode m_dialogMode;

        Brain* m_brain;
        
        SpecFile* m_specFile;
    
        QPushButton* m_loadScenesPushButton;
        
        WuQEventBlockingFilter* m_comboBoxWheelEventBlockingFilter;
        
        QActionGroup* m_fileTypesActionGroup;
        
        QActionGroup* m_fileSelectionActionGroup;
        
        QActionGroup* m_manageFilesLoadedNotLoadedActionGroup;
        
        QActionGroup* m_structureActionGroup;
        
        std::vector<TableRowDataFileContent*> m_tableRowDataFileContent;
        
        int m_specFileTableRowIndex;
        
        std::vector<GuiSpecFileDataFileTypeGroup*> m_guiSpecFileDataFileTypeGroups;
        
        QTableWidgetItem* m_specFileSaveCheckedItem;
        
        QTableWidgetItem* m_specFileStatusItem;
        
        QTableWidgetItem* m_specFileNameItem;
        
        QToolButton* m_specFileOptionsToolButton;
        
        QToolButton* m_chooseSpecFileToolButton;
        
        WuQWidgetObjectGroup* m_specFileWidgetGroup;
        
        QSignalMapper* m_fileLoadCheckBoxSignalMapper;
        
        QSignalMapper* m_fileSaveCheckBoxSignalMapper;
        
        QSignalMapper* m_fileInSpecCheckBoxSignalMapper;
        
        QSignalMapper* m_fileReloadOrOpenFileActionSignalMapper;
        
        QSignalMapper* m_fileRemoveFileActionSignalMapper;
        
        QSignalMapper* m_fileOptionsActionSignalMapper;
        
        QSignalMapper* m_fileSelectFileNameActionSignalMapper;
        
        QSignalMapper* m_fileStructureComboBoxSignalMapper;
        
        int m_specFileDataFileCounter;
        
        QTableWidget* m_filesTableWidget;
        
        static const int SHOW_FILES_ALL;
        static const int SHOW_FILES_NONE;

        int m_COLUMN_LOAD_CHECKBOX;
        int m_COLUMN_SAVE_CHECKBOX;
        int m_COLUMN_STATUS_LABEL;
        int m_COLUMN_IN_SPEC_FILE_CHECKBOX;
        int m_COLUMN_READ_BUTTON;
        int m_COLUMN_REMOVE_BUTTON;
        int m_COLUMN_OPTIONS_TOOLBUTTON;
        int m_COLUMN_DATA_FILE_TYPE_LABEL;
        int m_COLUMN_STRUCTURE;
        int m_COLUMN_FILE_NAME_LABEL;
        int m_COLUMN_COUNT;
    };
    
    /**
     * Data in a row of the table widget
     */
    class TableRowDataFileContent : public CaretObject {
    public:
        TableRowDataFileContent(SpecFileDataFileTypeGroup* specFileDataFileTypeGroup,
                                SpecFileDataFile* specFileDataFile) {
            m_tableRowIndex             = -1;
            m_specFileDataFileTypeGroup = specFileDataFileTypeGroup;
            m_specFileDataFile          = specFileDataFile;
        }
        
        int m_tableRowIndex;
        
        SpecFileDataFileTypeGroup* m_specFileDataFileTypeGroup;
     
        SpecFileDataFile* m_specFileDataFile;
    };
    
    class GuiSpecFileDataFileTypeGroup : public QObject {
        Q_OBJECT
        
    public:
        GuiSpecFileDataFileTypeGroup(Brain* brain,
                                     const SpecFileManagementDialog::Mode dialogMode,
                                     SpecFileDataFileTypeGroup* specFileDataFileTypeGroup,
                                     const AString& groupName,
                                     QObject* parent);
        
        ~GuiSpecFileDataFileTypeGroup();
        
        void addGuiSpecFileDataFile(GuiSpecFileDataFile* guiSpecFileDataFile);
        
        int32_t getNumberOfGuiSpecFileDataFiles() const;
        
        GuiSpecFileDataFile* getGuiSpecFileDataFile(const int32_t indx);
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        void updateContent();
        
        void setWidgetsVisible(bool visible);
        
        void setWidgetsVisibleByFiltering(const DataFileTypeEnum::Enum dataFileType,
                                                     const StructureEnum::Enum structure,
                                                     const SpecFileManagementDialog::ManageFilesDisplay manageFilesDisplay);
        
    private:
        friend class SpecFileManagementDialog;
        
        Brain* m_brain;
        
        const SpecFileManagementDialog::Mode m_dialogMode;

        SpecFileDataFileTypeGroup* m_specFileDataFileTypeGroup;
        
        std::vector<GuiSpecFileDataFile*> m_guiDataFiles;
    };
    
    
    class GuiSpecFileDataFile : public QObject {
        Q_OBJECT
        
    public:
        GuiSpecFileDataFile(const int signalMapperIndex,
                            Brain* brain,
                            const SpecFileManagementDialog::Mode dialogMode,
                            SpecFileDataFileTypeGroup* specFileDataFileTypeGroup,
                            SpecFileDataFile* specFileDataFile,
                            QObject* parent);
        
        ~GuiSpecFileDataFile();
        
        void setSpecFileDataFile(SpecFileDataFile* specFileDataFile);
        
        void setWidgetsEnabled(const bool enabled);
        
        void setWidgetsVisible(const bool visible);
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        void updateContent();
        
        int getSignalMapperIndex() const;
        
        bool isVisibleByFileTypeAndStructure(const DataFileTypeEnum::Enum dataFileType,
                                             const StructureEnum::Enum structure);
    
    public:
        const int m_signalMapperIndex;
        
        Brain* m_brain;
        
        const SpecFileManagementDialog::Mode m_dialogMode;
        
        SpecFileDataFileTypeGroup* m_specFileDataFileTypeGroup;
        
        SpecFileDataFile* m_specFileDataFile;

        QTableWidgetItem* m_loadCheckedItem;
        
        QTableWidgetItem* m_saveCheckedItem;
        
        QTableWidgetItem* m_inSpecFileCheckedItem;
        
        QTableWidgetItem* m_dataTypeLabelItem;
        
        QAction* m_reloadOrOpenFileAction;
        
        QToolButton* m_reloadOrOpenFileToolButton;
        
        QTableWidgetItem* m_statusLabelItem;
        
        QAction* m_optionsButtonAction;
        
        QToolButton* m_optionsToolButton;
        
        QTableWidgetItem* m_structureLabelItem;
        
        QAction* m_selectFileNameButtonAction;
        
        QToolButton* m_selectFileNameToolButton;
        
        QTableWidgetItem* m_fileNameLabelItem;
        
        WuQWidgetObjectGroup* m_widgetGroup;
        
        int m_tableRowIndex;
    };
    
#ifdef __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__
    const int SpecFileManagementDialog::SHOW_FILES_ALL = -1;
    const int SpecFileManagementDialog::SHOW_FILES_NONE = -2;
#endif // __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__

} // namespace
#endif  //__SPEC_FILE_MANAGEMENT_DIALOG_H__
