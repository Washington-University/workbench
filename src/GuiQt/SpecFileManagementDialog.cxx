
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

#define __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__
#include "SpecFileManagementDialog.h"
#undef __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretFileDialog.h"
#include "CaretMappableDataFile.h"
#include "CursorDisplayScoped.h"
#include "EventDataFileRead.h"
#include "EventDataFileReload.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "MetaDataEditorDialog.h"
#include "ProgressReportingDialog.h"
#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "StructureEnumComboBox.h"
#include "WuQEventBlockingFilter.h"
#include "WuQMessageBox.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::SpecFileManagementDialog
 * \brief Dialog for operations with Spec Files.
 * \ingroup GuiQt
 */

/**
 * Create a dialog for opening a spec file.
 *
 * DO NOT delete the returned dialog as it will delete itself when closed.
 *
 * @param brain
 *    Brain into which spec file is read.
 * @param specFile
 *    Spec File.
 * @param parent
 *    Parent of dialog.
 */
SpecFileManagementDialog*
SpecFileManagementDialog::createOpenSpecFileDialogTableLayout(Brain* brain,
                                                              SpecFile* specFile,
                                                              QWidget* parent)
{
    CaretAssert(brain);
    CaretAssert(specFile);
    const AString title = ("Open Spec File: "
                           + specFile->getFileNameNoPath());
    SpecFileManagementDialog* d = new SpecFileManagementDialog(MODE_OPEN_SPEC_FILE,
                                                               brain,
                                                               specFile,
                                                               title,
                                                               parent);
    return d;
}

/**
 * Create a dialog for managing files in a brain.
 *
 * DO NOT delete the returned dialog as it will delete itself when closed.
 *
 * @param brain
 *    Brain for which files are managed.
 * @param specFile
 *    Spec File.
 * @param parent
 *    Parent of dialog.
 */
SpecFileManagementDialog*
SpecFileManagementDialog::createManageFilesDialogTableLayout(Brain* brain,
                                                             QWidget* parent)
{
    CaretAssert(brain);
    const AString title = ("Manage Data Files");
    
    brain->determineDisplayedDataFiles();
    
    SpecFileManagementDialog* d = new SpecFileManagementDialog(MODE_MANAGE_FILES,
                                                               brain,
                                                               brain->getSpecFile(),
                                                               title,
                                                               parent);
    return d;
}


/**
 * Constructor.
 *
 * @param dialogMode
 *    Mode of dialog.
 * @param brain
 *    Brain
 * @param specFile
 *    Spec File.
 * @param dialogTitle
 *    Title for dialog.
 * @param parent
 *    Parent of dialog.
 */
SpecFileManagementDialog::SpecFileManagementDialog(const Mode dialogMode,
                                                   Brain* brain,
                                                   SpecFile* specFile,
                                                   const AString& dialogTitle,
                                                   QWidget* parent)
: WuQDialogModal(dialogTitle,
                 parent),
m_dialogMode(dialogMode),
m_brain(brain),
m_specFile(specFile)
{
    m_filesTableWidget = NULL;
    m_fileSelectionActionGroup = NULL;
    m_manageFilesLoadedNotLoadedActionGroup = NULL;
    setDeleteWhenClosed(true);
    m_specFileDataFileCounter = 0;
    m_specFileTableRowIndex = -1;

    /*
     * Open Spec File or Manage Files?
     */
    bool enableManageItems = false;
    bool enableOpenItems   = false;
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
            enableManageItems = true;
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            enableOpenItems = true;
            break;
    }
    
    /*
     * Mac wheel event causes unintentional selection of combo box
     */
    m_comboBoxWheelEventBlockingFilter = new WuQEventBlockingFilter(this);
#ifdef CARET_OS_MACOSX
    m_comboBoxWheelEventBlockingFilter->setEventBlocked(QEvent::Wheel,
                                                        true);
#endif // CARET_OS_MACOSX
    
    /*
     * Signal mappers for buttons
     */
    m_fileLoadCheckBoxSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileLoadCheckBoxSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileLoadCheckBoxSelected(int)));
    
    m_fileSaveCheckBoxSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileSaveCheckBoxSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileSaveCheckBoxSelected(int)));
    
    m_fileInSpecCheckBoxSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileInSpecCheckBoxSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileInSpecCheckBoxSelected(int)));
    
    m_fileReloadOrOpenFileActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileReloadOrOpenFileActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileReloadOrOpenFileActionSelected(int)));
    
    m_fileOptionsActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileOptionsActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileOptionsActionSelected(int)));
    
    m_fileRemoveFileActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileRemoveFileActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileRemoveActionSelected(int)));
    
    m_fileSelectFileNameActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileSelectFileNameActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileSelectFileNameActionSelected(int)));
    
    m_fileStructureComboBoxSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileStructureComboBoxSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileStructureComboBoxSelected(int)));

    int tableRowCounter = 0;
    
    /*
     * Is there a spec file?
     */
    if (enableManageItems) {
        m_specFileTableRowIndex = tableRowCounter++;
    }
    
    bool haveSceneFiles = false;
    
    /*
     * Display each type of data file
     */
    const int32_t numGroups = m_specFile->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0 ; ig < numGroups; ig++) {
        /*
         * File type of group
         */
        SpecFileDataFileTypeGroup* group = m_specFile->getDataFileTypeGroupByIndex(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        
        const int32_t numFiles = group->getNumberOfFiles();
        for (int iFile = 0; iFile < numFiles; iFile++) {
            SpecFileDataFile* sfdf = group->getFileInformation(iFile);
            
            TableRowDataFileContent* rowContent = new TableRowDataFileContent(group,
                                                                              sfdf);
            m_tableRowDataFileContent.push_back(rowContent);
            m_specFileDataFileCounter++;
        }
        
        if (dataFileType == DataFileTypeEnum::SCENE) {
            haveSceneFiles = true;
        }
    }
    
    m_loadScenesPushButton = NULL;
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
            setOkButtonText("Save Checked Files");
            setCancelButtonText("Close");
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            setOkButtonText("Load");
            setCancelButtonText("Cancel");
            m_loadScenesPushButton = addUserPushButton("Load Scenes",
                                                       QDialogButtonBox::AcceptRole);
            m_loadScenesPushButton->setEnabled(haveSceneFiles);
            break;
    }
    

    /*
     * Set column indices for table's members
     */
    int columnCounter = 0;
    m_COLUMN_LOAD_CHECKBOX               = -1;
    m_COLUMN_SAVE_CHECKBOX               = -1;
    m_COLUMN_STATUS_LABEL                = -1;
    m_COLUMN_IN_SPEC_FILE_CHECKBOX       = -1;
    m_COLUMN_READ_BUTTON                 = -1;
    m_COLUMN_REMOVE_BUTTON               = -1;
    m_COLUMN_OPTIONS_TOOLBUTTON          = -1;
    m_COLUMN_DATA_FILE_TYPE_LABEL        = -1;
    m_COLUMN_STRUCTURE                   = -1;
    m_COLUMN_FILE_NAME_LABEL      = -1;
    m_COLUMN_COUNT                       = -1;
    if (enableOpenItems) {
        m_COLUMN_LOAD_CHECKBOX = columnCounter++;
    }
    if (enableManageItems) {
        m_COLUMN_SAVE_CHECKBOX = columnCounter++;
        m_COLUMN_STATUS_LABEL = columnCounter++;
        m_COLUMN_IN_SPEC_FILE_CHECKBOX = columnCounter++;
        m_COLUMN_READ_BUTTON = columnCounter++;
        m_COLUMN_REMOVE_BUTTON = columnCounter++;
    }
    m_COLUMN_OPTIONS_TOOLBUTTON = columnCounter++;
    m_COLUMN_DATA_FILE_TYPE_LABEL = columnCounter++;
    if (enableOpenItems) {
        m_COLUMN_STRUCTURE = columnCounter++;
    }
    m_COLUMN_FILE_NAME_LABEL = columnCounter++;
    m_COLUMN_COUNT = columnCounter++;


    /*
     * Number of rows
     */
    int tableRowIndex = 0;
    int numberOfRows = m_specFile->getNumberOfFiles();
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
            numberOfRows += 1;  // add row for listing spec file
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            break;
    }
    
    /*
     * Create the table
     */
    createOrUpdateTableDimensions();
    
    
    
//    int leftHorizLineColumn = 0;
//    int leftHorizLineColumnCount = m_COLUMN_FILE_NAME_LABEL;
//    if (m_COLUMN_LOAD_CHECKBOX >= 0) {
//        leftHorizLineColumn = m_COLUMN_LOAD_CHECKBOX;
//    }
//    else if (m_COLUMN_SAVE_CHECKBOX >= 0) {
//        leftHorizLineColumn = m_COLUMN_SAVE_CHECKBOX;
//    }
    
    m_specFileSaveCheckedItem = NULL;
    m_specFileStatusItem = NULL;
    m_specFileOptionsToolButton = NULL;
    m_chooseSpecFileToolButton = NULL;
    m_specFileNameItem = NULL;
    m_specFileWidgetGroup = NULL;
    
    if (m_dialogMode == MODE_MANAGE_FILES) {
        m_specFileWidgetGroup = new WuQWidgetObjectGroup(this);
        
        m_specFileSaveCheckedItem = new QTableWidgetItem();
        m_specFileSaveCheckedItem->setFlags(Qt::ItemIsEnabled
                                            | Qt::ItemIsUserCheckable);
        m_specFileSaveCheckedItem->setCheckState(Qt::Unchecked);
        
        m_specFileStatusItem = new QTableWidgetItem();
        m_specFileStatusItem->setFlags(Qt::ItemIsEnabled);
        
        QAction* specFileOptionsAction = WuQtUtilities::createAction("Options",
                                                                     "Options for spec file",
                                                                     this,
                                                                     this,
                                                                     SLOT(specFileOptionsActionTriggered()));
        m_specFileOptionsToolButton = new QToolButton();
        m_specFileOptionsToolButton->setDefaultAction(specFileOptionsAction);
        
        m_specFileNameItem = new QTableWidgetItem();
        m_specFileNameItem->setFlags(Qt::ItemIsEnabled);
        
        if (m_chooseSpecFileToolButton != NULL) m_specFileWidgetGroup->add(m_chooseSpecFileToolButton);
        if (m_specFileOptionsToolButton != NULL) m_specFileWidgetGroup->add(m_specFileOptionsToolButton);
        
        m_filesTableWidget->setItem(tableRowIndex,
                                          m_COLUMN_SAVE_CHECKBOX,
                                          m_specFileSaveCheckedItem);
        m_filesTableWidget->setItem(tableRowIndex,
                                          m_COLUMN_STATUS_LABEL,
                                          m_specFileStatusItem);
        QTableWidgetItem* specTypeItem = new QTableWidgetItem();
        specTypeItem->setFlags(Qt::ItemIsEnabled);
        specTypeItem->setText(DataFileTypeEnum::toGuiName(DataFileTypeEnum::SPECIFICATION));
        m_filesTableWidget->setItem(tableRowIndex,
                                          m_COLUMN_DATA_FILE_TYPE_LABEL,
                                          specTypeItem);
        
        m_filesTableWidget->setCellWidget(tableRowIndex,
                                          m_COLUMN_OPTIONS_TOOLBUTTON,
                                          m_specFileOptionsToolButton);
        m_filesTableWidget->setItem(tableRowIndex,
                                          m_COLUMN_FILE_NAME_LABEL,
                                          m_specFileNameItem);
        tableRowIndex++;
    }
    
    /*
     * Add all files to the dialog.
     */
    const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
    for (int32_t i = 0; i < numGuiGroups; i++) {
        GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[i];
        
        /*
         * Add files in group
         */
        const int numGuiFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
        for (int32_t j = 0; j < numGuiFiles; j++) {
            
            GuiSpecFileDataFile* guiSpecDataFile = guiSpecGroup->getGuiSpecFileDataFile(j);
            
            guiSpecDataFile->m_tableRowIndex = tableRowIndex;
            
            if (m_COLUMN_LOAD_CHECKBOX >= 0) {
                m_filesTableWidget->setItem(tableRowIndex,
                                            m_COLUMN_LOAD_CHECKBOX,
                                            guiSpecDataFile->m_loadCheckedItem);
            }
            if (m_COLUMN_SAVE_CHECKBOX >= 0) {
                m_filesTableWidget->setItem(tableRowIndex,
                                                  m_COLUMN_SAVE_CHECKBOX,
                                                  guiSpecDataFile->m_saveCheckedItem);
            }
            if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0) {
                m_filesTableWidget->setItem(tableRowIndex,
                                            m_COLUMN_IN_SPEC_FILE_CHECKBOX,
                                            guiSpecDataFile->m_inSpecFileCheckedItem);
            }
            if (m_COLUMN_READ_BUTTON >= 0) {
                m_filesTableWidget->setCellWidget(tableRowIndex,
                                                  m_COLUMN_READ_BUTTON,
                                                  guiSpecDataFile->m_reloadOrOpenFileToolButton);
            }
            if (m_COLUMN_STATUS_LABEL >= 0) {
                m_filesTableWidget->setItem(tableRowIndex,
                                                  m_COLUMN_STATUS_LABEL,
                                                  guiSpecDataFile->m_statusLabelItem);
            }
            if (m_COLUMN_OPTIONS_TOOLBUTTON >= 0) {
                m_filesTableWidget->setCellWidget(tableRowIndex,
                                                  m_COLUMN_OPTIONS_TOOLBUTTON,
                                                  guiSpecDataFile->m_optionsToolButton);
            }
            if (m_COLUMN_DATA_FILE_TYPE_LABEL >= 0) {
                m_filesTableWidget->setItem(tableRowIndex,
                                                  m_COLUMN_DATA_FILE_TYPE_LABEL,
                                                  guiSpecDataFile->m_dataTypeLabelItem);
            }
            if (m_COLUMN_STRUCTURE >= 0) {
                if (guiSpecDataFile->m_structureLabelItem != NULL) {
                    m_filesTableWidget->setItem(tableRowIndex,
                                                      m_COLUMN_STRUCTURE,
                                                      guiSpecDataFile->m_structureLabelItem);
                }
            }
            if (m_COLUMN_FILE_NAME_LABEL >= 0) {
                m_filesTableWidget->setItem(tableRowIndex,
                                                  m_COLUMN_FILE_NAME_LABEL,
                                                  guiSpecDataFile->m_fileNameLabelItem);
            }
            
            tableRowIndex++;
        }
        
        guiSpecGroup->updateContent();
    }
    
    /*
     * Widget and layout for files.
     *
     * Two layouts used so widget is pushed to the top left (and not
     * spread out) when groups of files are hidden.
     */
    bool enableScrollBars = false;
    QWidget* centralWidget = NULL;
    centralWidget = m_filesTableWidget;
    m_filesTableWidget->resizeColumnsToContents();
    m_filesTableWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding);
    
    QWidget* toolbarWidget = new QWidget();
    QVBoxLayout* toolbarLayout = new QVBoxLayout(toolbarWidget);
    WuQtUtilities::setLayoutMargins(toolbarLayout, 0, 0);
    toolbarLayout->addWidget(createFilesTypesToolBar());
    toolbarLayout->addWidget(createStructureToolBar());
    if (enableOpenItems) {
        toolbarLayout->addWidget(createFilesSelectionToolBar());
    }
    else if (enableManageItems) {
        toolbarLayout->addWidget(createManageFilesLoadedNotLoadedToolBar());
    }
    setTopBottomAndCentralWidgets(toolbarWidget,
                                  centralWidget,
                                  NULL,
                                  enableScrollBars);
    
    updateDisplayedFiles();
    
    disableAutoDefaultForAllPushButtons();
}

/**
 * Destructor.
 */
SpecFileManagementDialog::~SpecFileManagementDialog()
{
    const int32_t numRows = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numRows; i++) {
        delete m_tableRowDataFileContent[i];
    }
    m_tableRowDataFileContent.clear();
}

void
SpecFileManagementDialog::setTableColumnLabels()
{
    /*
     * Set names of table's columns
     */
    QStringList tableWidgetColumnLabels;
    tableWidgetColumnLabels.reserve(m_COLUMN_COUNT);
    for (int32_t i = 0; i < m_COLUMN_COUNT; i++) {
        tableWidgetColumnLabels.append("");
    }
    if (m_COLUMN_LOAD_CHECKBOX >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_LOAD_CHECKBOX, "Load");
    }
    if (m_COLUMN_SAVE_CHECKBOX >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_SAVE_CHECKBOX, "Save");
    }
    if (m_COLUMN_STATUS_LABEL >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_STATUS_LABEL, "Modified");
    }
    if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_IN_SPEC_FILE_CHECKBOX, "In Spec");
    }
    if (m_COLUMN_READ_BUTTON >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_READ_BUTTON, "Read");
    }
    if (m_COLUMN_REMOVE_BUTTON >= 0) {
        tableWidgetColumnLabels.replace(m_COLUMN_REMOVE_BUTTON, "Remove");
    }
    if (m_COLUMN_OPTIONS_TOOLBUTTON >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_OPTIONS_TOOLBUTTON, "More");
    }
    if (m_COLUMN_DATA_FILE_TYPE_LABEL >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_DATA_FILE_TYPE_LABEL, "Data Type");
    }
    if (m_COLUMN_STRUCTURE >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_STRUCTURE, "Structure");
    }
    if (m_COLUMN_FILE_NAME_LABEL >= 0){
        tableWidgetColumnLabels.replace(m_COLUMN_FILE_NAME_LABEL, "Data File Name");
    }
    
    m_filesTableWidget->setHorizontalHeaderLabels(tableWidgetColumnLabels);
}


/**
 * Load items into the table widget adding rows as needed.
 */
void
SpecFileManagementDialog::createOrUpdateTableDimensions()
{
    m_specFileTableRowIndex = -1;
    
    /*
     * If needed, add a row for the spec file
     */
    int rowCounter = 0;
    if (m_dialogMode == MODE_MANAGE_FILES) {
        m_specFileTableRowIndex = rowCounter;
        rowCounter++;
    }
    
    /*
     * Update rows indices for data files
     */
    const int32_t numFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numFiles; i++) {
        m_tableRowDataFileContent[i]->m_tableRowIndex = rowCounter;
        rowCounter++;
    }
    
    /*
     * Create or update the dimensions of the table
     */
    int32_t firstNewRowIndex = -1;
    if (m_filesTableWidget == NULL) {
        /*
         * Create the table widget.
         * No vertical header (contains row numbers)
         * No grid lines.
         */
        m_filesTableWidget = new QTableWidget(rowCounter,
                                              m_COLUMN_COUNT,
                                              this);
        m_filesTableWidget->verticalHeader()->hide();
        m_filesTableWidget->setGridStyle(Qt::NoPen);
        setTableColumnLabels();
        
        firstNewRowIndex = 0;
    }
    else {
        firstNewRowIndex = m_filesTableWidget->rowCount();
        m_filesTableWidget->setRowCount(rowCounter);
    }
    int32_t lastNewRowIndex  = m_filesTableWidget->rowCount();
    
    /*
     * Add new cells to the table widget
     */
    for (int32_t iRow = firstNewRowIndex; iRow < lastNewRowIndex; iRow++) {
        if (m_COLUMN_LOAD_CHECKBOX >= 0) {
            m_filesTableWidget->setItem(iRow,
                                        m_COLUMN_LOAD_CHECKBOX,
                                        createCheckableItem());
        }
        if (m_COLUMN_SAVE_CHECKBOX >= 0) {
            m_filesTableWidget->setItem(iRow,
                                        m_COLUMN_SAVE_CHECKBOX,
                                        createCheckableItem());
        }
        if (m_COLUMN_STATUS_LABEL >= 0) {
            m_filesTableWidget->setItem(iRow,
                                        m_COLUMN_STATUS_LABEL,
                                        createTextItem());
        }
        if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0) {
            m_filesTableWidget->setItem(iRow,
                                        m_COLUMN_IN_SPEC_FILE_CHECKBOX,
                                        createCheckableItem());
        }
        if (m_COLUMN_READ_BUTTON >= 0) {
            QAction* loadFileAction = WuQtUtilities::createAction("Reload",
                                                                 "Read or reload a file",
                                                                 this);
            QToolButton* loadFileToolButton = new QToolButton();
            loadFileToolButton->setDefaultAction(loadFileAction);
            
            QObject::connect(loadFileAction, SIGNAL(triggered()),
                             m_fileReloadOrOpenFileActionSignalMapper, SLOT(map()));
            m_fileOptionsActionSignalMapper->setMapping(loadFileAction, iRow);
            
            m_filesTableWidget->setCellWidget(iRow,
                                              m_COLUMN_OPTIONS_TOOLBUTTON,
                                              loadFileToolButton);
        }
        
        if (m_COLUMN_REMOVE_BUTTON >= 0) {
            QAction* removeFileAction = WuQtUtilities::createAction("Reload",
                                                                  "Read or reload a file",
                                                                  this);
            QToolButton* removeFileToolButton = new QToolButton();
            removeFileToolButton->setDefaultAction(removeFileAction);
            
            QObject::connect(removeFileAction, SIGNAL(triggered()),
                             m_fileRemoveFileActionSignalMapper, SLOT(map()));
            m_fileOptionsActionSignalMapper->setMapping(removeFileAction, iRow);
            
            m_filesTableWidget->setCellWidget(iRow,
                                              m_COLUMN_OPTIONS_TOOLBUTTON,
                                              removeFileToolButton);
        }
        
        if (m_COLUMN_OPTIONS_TOOLBUTTON >= 0) {
            QAction* optionsAction = WuQtUtilities::createAction("Options",
                                                                 "Options for file",
                                                                 this);
            QToolButton* optionsToolButton = new QToolButton();
            optionsToolButton->setDefaultAction(optionsAction);

            QObject::connect(optionsAction, SIGNAL(triggered()),
                             m_fileOptionsActionSignalMapper, SLOT(map()));
            m_fileOptionsActionSignalMapper->setMapping(optionsAction, iRow);
            
            m_filesTableWidget->setCellWidget(iRow,
                                              m_COLUMN_OPTIONS_TOOLBUTTON,
                                              optionsToolButton);            
        }
        if (m_COLUMN_DATA_FILE_TYPE_LABEL >= 0) {
            m_filesTableWidget->setItem(iRow,
                                        m_COLUMN_DATA_FILE_TYPE_LABEL,
                                        createTextItem());
        }
        if (m_COLUMN_STRUCTURE >= 0) {
            m_filesTableWidget->setItem(iRow,
                                        m_COLUMN_STRUCTURE,
                                        createTextItem());
        }
        if (m_COLUMN_FILE_NAME_LABEL >= 0) {
            m_filesTableWidget->setItem(iRow,
                                        m_COLUMN_FILE_NAME_LABEL,
                                        createTextItem());
        }
    }
}

/**
 * @return Create and return a text item for the table.
 */
QTableWidgetItem*
SpecFileManagementDialog::createTextItem()
{
    QTableWidgetItem* item = new QTableWidgetItem();
    item->setFlags(Qt::ItemIsEnabled);
    
    return item;
}

/**
 * @return Create and return a checkable item for the table.
 */
QTableWidgetItem*
SpecFileManagementDialog::createCheckableItem()
{
    QTableWidgetItem* item = new QTableWidgetItem();
    item->setFlags(Qt::ItemIsEnabled
                   | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    
    return item;
}


/**
 * Called when a push button was added using addUserPushButton().
 * Subclasses MUST override this if user push buttons were
 * added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 * @return
 *    The result that indicates action that should be taken
 *    as a result of the button being pressed.
 */
WuQDialogModal::ModalDialogUserButtonResult
SpecFileManagementDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_loadScenesPushButton) {
        /*
         * Load all of the scene files but nothing else
         */
        m_specFile->setAllSceneFilesSelectedAndAllOtherFilesNotSelected();
        
        okButtonClickedOpenSpecFile();
        
        GuiManager::get()->processShowSceneDialog(GuiManager::get()->getActiveBrowserWindow());
        
        return RESULT_ACCEPT;
    }
    else {
        CaretAssert(0);
    }
    
    return RESULT_NONE;
}

/**
 * Gets called when the OK button is pressed.
 */
void
SpecFileManagementDialog::okButtonClicked ()
{
    bool allowDialogToClose = false;
    
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
            allowDialogToClose = okButtonClickedManageFiles();
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            okButtonClickedOpenSpecFile();
            allowDialogToClose = true;
            break;
    }
    
    if (allowDialogToClose) {
        WuQDialogModal::okButtonClicked();
    }
}

/**
 * Perform processing when the Open button is pressed for Open Spec File mode.
 */
void
SpecFileManagementDialog::okButtonClickedOpenSpecFile()
{
    AString specFileErrorMessage = writeSpecFile(true);
    AString errorMessages;
    errorMessages.appendWithNewLine(specFileErrorMessage);
    
    EventSpecFileReadDataFiles readSpecFileEvent(m_brain,
                                                 m_specFile);
    
    ProgressReportingDialog::runEvent(&readSpecFileEvent,
                                      this,
                                      m_specFile->getFileNameNoPath());
    
    errorMessages.appendWithNewLine(readSpecFileEvent.getErrorMessage());
    
    updateGraphicWindowsAndUserInterface();
    
    if (errorMessages.isEmpty() == false) {
        WuQMessageBox::errorOk(this,
                               errorMessages);
    }
}

/**
 * Perform processing when the Open button is pressed for Manage Files mode.
 */
bool
SpecFileManagementDialog::okButtonClickedManageFiles()
{
    /*
     * Wait cursor
     */
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    AString errorMessages;
    
    const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
    for (int32_t iGroup = 0; iGroup < numGuiGroups; iGroup++) {
        GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[iGroup];
        const int32_t numFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            GuiSpecFileDataFile* guiDataFile = guiSpecGroup->getGuiSpecFileDataFile(iFile);
            if (guiDataFile->m_saveCheckedItem->flags() & Qt::ItemIsEnabled) {
                if (guiDataFile->m_saveCheckedItem->checkState() == Qt::Checked) {
                    CaretDataFile* caretDataFile = guiDataFile->m_specFileDataFile->getCaretDataFile();
                    if (caretDataFile != NULL) {
                        try {
                            m_brain->writeDataFile(caretDataFile,
                                                   false);
                            guiDataFile->m_saveCheckedItem->setCheckState(Qt::Unchecked);
                        }
                        catch (const DataFileException& e) {
                            errorMessages.appendWithNewLine(e.whatString());
                        }
                        guiDataFile->updateContent();
                    }
                }
            }
        }
    }
    
    if (m_specFileSaveCheckedItem->checkState() == Qt::Checked) {
        AString specFileName = m_specFile->getFileName();
        if (m_specFile->getFileName().isEmpty()) {
            errorMessages.appendWithNewLine("Spec File name is empty.");
        }
        else {
            AString specFileErrorMessage = writeSpecFile(false);
            if (specFileErrorMessage.isEmpty() == false) {
                errorMessages.appendWithNewLine(specFileErrorMessage);
            }
            m_specFileSaveCheckedItem->setCheckState(Qt::Unchecked);
        }
    }
    
    updateDisplayedFiles();
    
    cursor.restoreCursor();
    
    if (errorMessages.isEmpty() == false) {
        WuQMessageBox::errorOk(this, errorMessages);
        return false;
    }
    
    return false;
}

/**
 * Write the spec file if it is modified.
 *
 * @param writeOnlyIfModified
 *     Write only if the spec file is modified.
 * @return Non-empty string if there was an error writing the spec file.
 */
AString
SpecFileManagementDialog::writeSpecFile(const bool writeOnlyIfModified)
{
    if (writeOnlyIfModified) {
        if (m_specFile->isModified() == false) {
            return "";
        }
    }
    
    AString errorMessage;
    
    try {
        m_specFile->writeFile(m_specFile->getFileName());
    }
    catch (const DataFileException& e) {
        errorMessage = e.whatString();
    }
    
    return errorMessage;
}

/**
 * Called when a file load check box is toggled.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileLoadCheckBoxSelected(int indx)
{
//    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
//    SpecFileDataFile* specFileDataFile = guiSpecFileDataFile->m_specFileDataFile;
//    specFileDataFile->setSelected(guiSpecFileDataFile->m_loadCheckBox->isChecked());
}

/**
 * Called when a file save check box is toggled.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileSaveCheckBoxSelected(int /*indx*/)
{
    //    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
    //    SpecFileDataFile* specFileDataFile = guiSpecFileDataFile->m_specFileDataFile;
}

/**
 * Called when a file in spec check box is toggled.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileInSpecCheckBoxSelected(int indx)
{
//    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
//    SpecFileDataFile* specFileDataFile = guiSpecFileDataFile->m_specFileDataFile;
//    specFileDataFile->setSpecFileMember(guiSpecFileDataFile->m_inSpecFileCheckBox->isChecked());
}

/**
 * Called when a file remove button is clicked.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileRemoveActionSelected(int indx)
{
}

/**
 * Called when a file reload or open button is clicked.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileReloadOrOpenFileActionSelected(int indx)
{
    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
    SpecFileDataFile* specFileDataFile = guiSpecFileDataFile->m_specFileDataFile;
    
    CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
    if (caretDataFile != NULL) {
        EventDataFileReload reloadEvent(m_brain,
                                        caretDataFile);
        EventManager::get()->sendEvent(reloadEvent.getPointer());
        
        if (reloadEvent.isError()) {
            WuQMessageBox::errorOk(guiSpecFileDataFile->m_reloadOrOpenFileToolButton,
                                   reloadEvent.getErrorMessage());
            guiSpecFileDataFile->setWidgetsEnabled(false);
        }
        if (guiSpecFileDataFile->m_saveCheckedItem->flags() & Qt::ItemIsEnabled) {
            guiSpecFileDataFile->m_saveCheckedItem->setCheckState(Qt::Unchecked);
        }
    }
    else {
        EventDataFileRead readEvent(m_brain,
                                    false);
        readEvent.addDataFile(specFileDataFile->getStructure(),
                              specFileDataFile->getDataFileType(),
                              specFileDataFile->getFileName());
        
        EventManager::get()->sendEvent(readEvent.getPointer());
        
        if (readEvent.isError()) {
            WuQMessageBox::errorOk(guiSpecFileDataFile->m_reloadOrOpenFileToolButton,
                                   readEvent.getErrorMessage());
        }
        
        if (readEvent.getAddToSpecFileErrorMessages().isEmpty() == false) {
            WuQMessageBox::errorOk(guiSpecFileDataFile->m_reloadOrOpenFileToolButton,
                                   readEvent.getAddToSpecFileErrorMessages());
        }
    }
    guiSpecFileDataFile->m_saveCheckedItem->setCheckState(Qt::Unchecked);
    
    guiSpecFileDataFile->updateContent();
    updateDisplayedFiles();
    
    updateGraphicWindowsAndUserInterface();
}

/**
 * Updates graphics windows and user interface
 */
void
SpecFileManagementDialog::updateGraphicWindowsAndUserInterface()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Called when a file options button is clicked.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileOptionsActionSelected(int indx)
{
    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
    SpecFileDataFile* specFileDataFile = guiSpecFileDataFile->m_specFileDataFile;
    CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
    CaretMappableDataFile* caretMappableDataFile = NULL;
    if (caretDataFile != NULL) {
        caretMappableDataFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile);
    }
    
    QAction* editMetaDataAction = NULL;
    QAction* setFileNameAction = NULL;
    QAction* unloadFileAction = NULL;
    QAction* unloadFileMapsAction = NULL;
    QAction* viewMetaDataAction = NULL;
    
    QMenu menu;
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
            if (caretDataFile != NULL) {
                editMetaDataAction = menu.addAction("Edit Metadata...");
                setFileNameAction = menu.addAction("Set File Name...");
                unloadFileAction = menu.addAction("Unload File");
                if (caretMappableDataFile != NULL) {
                    unloadFileMapsAction = menu.addAction("Unload Map(s) from File");
                    unloadFileMapsAction->setEnabled(false);
                }
            }
            else {
                viewMetaDataAction = menu.addAction("View Metadata...");
                viewMetaDataAction->setEnabled(false);
            }
            break;
        case MODE_OPEN_SPEC_FILE:
            viewMetaDataAction = menu.addAction("View Metadata...");
            viewMetaDataAction->setEnabled(false);
            break;
    }
    
    QAction* selectedAction = menu.exec(QCursor::pos());
    
    if (selectedAction == NULL) {
        /*
         * If the selected action is NULL, it indicates that the user did
         * not make a selection.  This test is needed as some of the actions
         * (such as setFileNameAction) may be NULL and with out this test,
         * those NULL actions would match.
         */
    }
    else if (selectedAction == setFileNameAction) {
        changeFileName(&menu,
                       indx);
    }
    else if (selectedAction == unloadFileAction) {
        CaretDataFile* cdf = specFileDataFile->getCaretDataFile();
        GuiManager::get()->getBrain()->removeDataFile(cdf);
        guiSpecFileDataFile->updateContent();
        updateDisplayedFiles();
        updateGraphicWindowsAndUserInterface();
    }
    else if (selectedAction == unloadFileMapsAction) {
        
    }
    else if (selectedAction == editMetaDataAction) {
        if (caretDataFile != NULL) {
            MetaDataEditorDialog mded(caretDataFile,
                                      &menu);
            mded.exec();
            guiSpecFileDataFile->updateContent();
        }
    }
    else if (selectedAction == viewMetaDataAction) {
        
    }
    else if (selectedAction != NULL) {
        CaretAssertMessage(0,
                           ("Unhandled Menu Action: " + selectedAction->text()));
    }
}

/**
 * Called when a file name button is clicked.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileSelectFileNameActionSelected(int indx)
{
    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
    changeFileName(guiSpecFileDataFile->m_selectFileNameToolButton,
                   indx);
}

/**
 * Change the name of a file.
 *
 * @param parent
 *   Widget on which file dialog is displayed.
 * @param indx
 *   Index of item selected.
 */
void
SpecFileManagementDialog::changeFileName(QWidget* parent,
                                         const int indx)
{
    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
    SpecFileDataFile* specFileDataFile = guiSpecFileDataFile->m_specFileDataFile;
    
    QStringList filenameFilterList;
    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(guiSpecFileDataFile->getDataFileType()));
    CaretFileDialog fd(parent);
    fd.setAcceptMode(CaretFileDialog::AcceptSave);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.selectFile(specFileDataFile->getFileName());
    fd.setLabelText(CaretFileDialog::Accept, "Choose");
    fd.setWindowTitle("Choose File Name");
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList files = fd.selectedFiles();
        if (files.isEmpty() == false) {
            AString newFileName = files.at(0);
            if (newFileName != specFileDataFile->getFileName()) {
                /*
                 * Clone current item, remove file from it,
                 * and create new item.
                 */
                SpecFileDataFile* sfdf = m_specFile->changeFileName(specFileDataFile,
                                                                    newFileName);
                if (sfdf != NULL) {
                    //                m_dialog->addSpecFileDataFile(sfdf);
                }
            }
        }
    }
}

/**
 * Called when a file structure combo box is selected.
 *
 * @param indx
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileStructureComboBoxSelected(int indx)
{
//    GuiSpecFileDataFile* guiSpecFileDataFile = getSpecFileDataFileBySignalMapperIndex(indx);
//    SpecFileDataFile* specFileDataFile = guiSpecFileDataFile->m_specFileDataFile;
//    specFileDataFile->setStructure(guiSpecFileDataFile->m_structureLabelItem->getSelectedStructure());
}

/**
 * Find the GuiSpecFileDataFile with the given signal mapper index.
 *
 * @param signalMapperIndex
 *    The signal mapper index requested.
 * @return
 *    GuiSpecFileDataFile using the given signal mapper index.
 */
GuiSpecFileDataFile*
SpecFileManagementDialog::getSpecFileDataFileBySignalMapperIndex(const int signalMapperIndex)
{
    const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
    for (int32_t iGroup = 0; iGroup < numGuiGroups; iGroup++) {
        GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[iGroup];
        const int32_t numFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            GuiSpecFileDataFile* guiDataFile = guiSpecGroup->getGuiSpecFileDataFile(iFile);
            if (guiDataFile->getSignalMapperIndex() == signalMapperIndex) {
                return guiDataFile;
            }
        }
    }
    
    CaretAssert(0);
    return NULL;
}

/**
 * Called when spec file options tool button is triggered.
 */
void
SpecFileManagementDialog::specFileOptionsActionTriggered()
{
    QAction* setFileNameAction = NULL;
    
    QMenu menu;
    QAction* metadataAction = menu.addAction("Edit Metadata...");
    metadataAction->setEnabled(false);
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
            setFileNameAction = menu.addAction("Set File Name...");
            break;
        case MODE_OPEN_SPEC_FILE:
            break;
    }
    
    QAction* selectedAction = menu.exec(QCursor::pos());
    
    if (selectedAction == setFileNameAction) {
        QStringList filenameFilterList;
        filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
        CaretFileDialog fd(&menu);
        fd.setAcceptMode(CaretFileDialog::AcceptSave);
        fd.setNameFilters(filenameFilterList);
        fd.setFileMode(CaretFileDialog::AnyFile);
        fd.setViewMode(CaretFileDialog::List);
        fd.selectFile(m_specFile->getFileName());
        fd.setLabelText(CaretFileDialog::Accept, "Choose");
        fd.setWindowTitle("Choose Spec File Name");
        if (fd.exec() == CaretFileDialog::Accepted) {
            QStringList files = fd.selectedFiles();
            if (files.isEmpty() == false) {
                AString newFileName = files.at(0);
                m_specFile->setFileName(newFileName);
                updateDisplayedFiles();
            }
        }
    }
    else if (selectedAction == metadataAction) {
        
    }
    else if (selectedAction != NULL) {
        CaretAssertMessage(0,
                           ("Unhandled Menu Action: " + selectedAction->text()));
    }
}

/**
 * Called to choose the name of the spec file.
 */
void
SpecFileManagementDialog::chooseSpecFileNameActionTriggered()
{
    QStringList filenameFilterList;
    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
    CaretFileDialog fd(m_chooseSpecFileToolButton);
    fd.setAcceptMode(CaretFileDialog::AcceptSave);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.selectFile(m_specFile->getFileName());
    fd.setLabelText(CaretFileDialog::Accept, "Choose");
    fd.setWindowTitle("Choose Spec File Name");
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList files = fd.selectedFiles();
        if (files.isEmpty() == false) {
            AString newFileName = files.at(0);
            m_specFile->setFileName(newFileName);
            updateDisplayedFiles();
        }
    }
}


/**
 * @return Create and return a toolbar for viewing files by type of file.
 */
QToolBar*
SpecFileManagementDialog::createFilesTypesToolBar()
{
    m_fileTypesActionGroup = new QActionGroup(this);
    m_fileTypesActionGroup->setExclusive(true);
    QObject::connect(m_fileTypesActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(toolBarFileTypeActionTriggered(QAction*)));
    
    QAction* fileTypeAllAction = m_fileTypesActionGroup->addAction("All");
    fileTypeAllAction->setCheckable(true);
    fileTypeAllAction->setData(qVariantFromValue(DataFileTypeEnum::toIntegerCode(DataFileTypeEnum::UNKNOWN)));
    
    /*
     * All types of files
     */
    std::vector<DataFileTypeEnum::Enum> allDataFileTypes;
    DataFileTypeEnum::getAllEnums(allDataFileTypes);
    
    /*
     * Get data types of files that are listed in the dialog
     */
    std::vector<DataFileTypeEnum::Enum> loadedDataFileTypes;
    const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
    for (int32_t i = 0; i < numGuiGroups; i++) {
        GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[i];
        if (guiSpecGroup->getNumberOfGuiSpecFileDataFiles() > 0) {
            loadedDataFileTypes.push_back(guiSpecGroup->getDataFileType());
        }
    }
    
    for (std::vector<DataFileTypeEnum::Enum>::iterator iter = allDataFileTypes.begin();
         iter != allDataFileTypes.end();
         iter++) {
        DataFileTypeEnum::Enum dataFileType = *iter;
        
        /*
         * Only list file types if listed in dialog
         */
        if (dataFileType == DataFileTypeEnum::SPECIFICATION) {
            continue;
        }
        if (std::find(loadedDataFileTypes.begin(),
                      loadedDataFileTypes.end(),
                      dataFileType) == loadedDataFileTypes.end()) {
            continue;
        }
        
        AString text = DataFileTypeEnum::toGuiName(dataFileType);
        text = getEditedGroupName(text);
        
        QAction* action = m_fileTypesActionGroup->addAction(text);
        action->setCheckable(true);
        action->setData(qVariantFromValue(DataFileTypeEnum::toIntegerCode(dataFileType)));
    }
    
    if (m_fileTypesActionGroup->actions().isEmpty() == false) {
        m_fileTypesActionGroup->blockSignals(true);
        m_fileTypesActionGroup->actions().at(0)->setChecked(true);
        m_fileTypesActionGroup->blockSignals(false);
    }
    
    QToolBar* toolbar = createToolBarWithActionGroup("View File Types: ",
                                                     m_fileTypesActionGroup);
    return toolbar;
}

/**
 * @return Create and return a toolbar for selecting all or no files.
 */
QToolBar*
SpecFileManagementDialog::createFilesSelectionToolBar()
{
    //    * When loading, ALL or NONE but only ones that are visibleRegion()
    m_fileSelectionActionGroup = new QActionGroup(this);
    QObject::connect(m_fileSelectionActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(toolBarSelectFilesActionTriggered(QAction*)));
    
    QAction* allFilesAction = m_fileSelectionActionGroup->addAction("All");
    allFilesAction->setData(qVariantFromValue(SHOW_FILES_ALL));
    
    QAction* noneFilesAction = m_fileSelectionActionGroup->addAction("None");
    noneFilesAction->setData(qVariantFromValue(SHOW_FILES_NONE));
    
    QToolBar* toolbar = createToolBarWithActionGroup("Select Files: ",
                                                     m_fileSelectionActionGroup);
    return toolbar;
}

QToolBar*
SpecFileManagementDialog::createManageFilesLoadedNotLoadedToolBar()
{
    m_manageFilesLoadedNotLoadedActionGroup = new QActionGroup(this);
    
    QObject::connect(m_manageFilesLoadedNotLoadedActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(toolBarManageFilesLoadedNotLoadedActionTriggered(QAction*)));
    
    QAction* allFilesAction = m_manageFilesLoadedNotLoadedActionGroup->addAction("All");
    allFilesAction->setData(qVariantFromValue((int)MANAGE_FILES_ALL));
    allFilesAction->setCheckable(true);
    
    QAction* loadedFilesAction = m_manageFilesLoadedNotLoadedActionGroup->addAction("Loaded");
    loadedFilesAction->setData(qVariantFromValue((int)MANAGE_FILES_LOADED));
    loadedFilesAction->setCheckable(true);
    
    QAction* notLoadedFilesAction = m_manageFilesLoadedNotLoadedActionGroup->addAction("Not-Loaded");
    notLoadedFilesAction->setData(qVariantFromValue((int)MANAGE_FILES_NOT_LOADED));
    notLoadedFilesAction->setCheckable(true);
    
    m_manageFilesLoadedNotLoadedActionGroup->blockSignals(true);
    allFilesAction->setChecked(true);
    m_manageFilesLoadedNotLoadedActionGroup->blockSignals(false);
    
    QToolBar* toolbar = createToolBarWithActionGroup("Show Files: ",
                                                     m_manageFilesLoadedNotLoadedActionGroup);
    return toolbar;
}

/**
 * @return Edit and return the text for a name of a group.
 */
AString
SpecFileManagementDialog::getEditedGroupName(const AString& groupName)
{
    const AString connectivityPrefix("Connectivity - ");
    const int connectivityPrefixLength = connectivityPrefix.length();
    
    const AString temporarySuffix(" TEMPORARY");
    const int temporarySuffixLength = temporarySuffix.length();
    
    AString text = groupName;
    if (text.startsWith(connectivityPrefix)) {
        text = text.mid(connectivityPrefixLength);
    }
    
    if (text.endsWith(temporarySuffix)) {
        text = text.left(text.length() - temporarySuffixLength);
    }
    
    return text;
}


/**
 * @return Create and return a toolbar for viewing files by structure.
 */
QToolBar*
SpecFileManagementDialog::createStructureToolBar()
{
    
    std::vector<StructureEnum::Enum> structureTypes;
    structureTypes.push_back(StructureEnum::ALL);
    structureTypes.push_back(StructureEnum::CORTEX_LEFT);
    structureTypes.push_back(StructureEnum::CORTEX_RIGHT);
    structureTypes.push_back(StructureEnum::CEREBELLUM);
    structureTypes.push_back(StructureEnum::OTHER);
    
    m_structureActionGroup = new QActionGroup(this);
    m_structureActionGroup->setExclusive(true);
    QObject::connect(m_structureActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(toolBarStructuresActionTriggered(QAction*)));
    
    for (std::vector<StructureEnum::Enum>::iterator iter = structureTypes.begin();
         iter != structureTypes.end();
         iter++) {
        StructureEnum::Enum structure = *iter;
        QAction* action = m_structureActionGroup->addAction(StructureEnum::toGuiName(structure));
        action->setCheckable(true);
        action->setData(qVariantFromValue(StructureEnum::toIntegerCode(structure)));
    }
    
    if (m_structureActionGroup->actions().isEmpty() == false) {
        m_structureActionGroup->blockSignals(true);
        m_structureActionGroup->actions().at(0)->setChecked(true);
        m_structureActionGroup->blockSignals(false);
    }
    
    QToolBar* toolbar = createToolBarWithActionGroup("View Structures: ",
                                                     m_structureActionGroup);
    return toolbar;
}

/**
 * Create a toolbar with the given label containing all items
 * in the given action group.
 *
 * @param text
 *    If not empty, this text is inserted into the left side of the toolbar.
 * @param actionGroup
 *    All actions from this action group are added to the toolbar.
 * @return
 *    The toolbar.
 */
QToolBar*
SpecFileManagementDialog::createToolBarWithActionGroup(const QString& text,
                                                       QActionGroup* actionGroup)
{
    QToolBar* toolbar = new QToolBar();
    if (text.isEmpty() == false) {
        toolbar->addWidget(new QLabel(text));
    }
    
    QList<QAction*> actions = actionGroup->actions();
    QListIterator<QAction*> iterator(actions);
    while (iterator.hasNext()) {
        toolbar->addAction(iterator.next());
    }
    
    return toolbar;
}

/**
 * Called when a tool bar's file type button is selected.
 *
 * @param action
 *    QAction of item selected.
 */
void
SpecFileManagementDialog::toolBarFileTypeActionTriggered(QAction* action)
{
    if (action != NULL) {
        const int dataValue = action->data().toInt();
        bool isValid = false;
        const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromIntegerCode(dataValue,
                                                                                      &isValid);
        if (isValid) {
            std::cout << "Selected file type: " << qPrintable(DataFileTypeEnum::toGuiName(dataFileType)) << std::endl;
        }
        else {
            std::cout << "Invalid file type index: " << dataValue << std::endl;
        }
    }
    
    updateDisplayedFiles();
}

/**
 * Called when tool bar's structure button is selected.
 *
 * @param action
 *    QAction of item selected.
 */
void
SpecFileManagementDialog::toolBarStructuresActionTriggered(QAction* action)
{
    if (action != NULL) {
        const int dataValue = action->data().toInt();
        bool isValid = false;
        const StructureEnum::Enum structure = StructureEnum::fromIntegerCode(dataValue,
                                                                             &isValid);
        if (isValid) {
            std::cout << "Selected structure: " << qPrintable(StructureEnum::toGuiName(structure)) << std::endl;
        }
        else {
            std::cout << "Invalid structure index: " << dataValue << std::endl;
        }
    }
    
    updateDisplayedFiles();
}

/**
 * Show loaded/not loaded files when manage files mode.
 *
 * @param action
 *   QAction of item selected.
 */
void
SpecFileManagementDialog::toolBarManageFilesLoadedNotLoadedActionTriggered(QAction* action)
{
    updateDisplayedFiles();
}

/**
 * Set all files as selected.
 *
 * @param action
 *    QAction of item selected.
 */
void
SpecFileManagementDialog::toolBarSelectFilesActionTriggered(QAction* action)
{
    if (action != NULL) {
        const int dataValue = action->data().toInt();
        
        bool newStatus = false;
        if (dataValue == SHOW_FILES_ALL) {
            newStatus = true;
            std::cout << "Select ALL files: " << std::endl;
        }
        else if (dataValue == SHOW_FILES_NONE) {
            newStatus = false;
        }
        
        const int rowCount = m_filesTableWidget->rowCount();
        for (int iRow = 0; iRow < rowCount; iRow++) {
            if (m_filesTableWidget->isRowHidden(iRow) == false) {
                const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
                for (int32_t iGroup = 0; iGroup < numGuiGroups; iGroup++) {
                    GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[iGroup];
                    const int numFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
                    for (int32_t iFile = 0; iFile < numFiles; iFile++) {
                        GuiSpecFileDataFile* guiFile = guiSpecGroup->getGuiSpecFileDataFile(iFile);
                        if (guiFile->m_tableRowIndex == iRow) {
                            if (guiFile->m_loadCheckedItem != NULL) {
                                guiFile->m_loadCheckedItem->setCheckState(newStatus
                                                                   ? Qt::Checked
                                                                   : Qt::Unchecked);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * Update the displayed files.
 */
void
SpecFileManagementDialog::updateDisplayedFiles()
{
    /*
     * Update spec file data
     */
    if (m_dialogMode == MODE_MANAGE_FILES) {
        if (m_specFileTableRowIndex >= 0) {
            QTableWidgetItem* saveItem = m_filesTableWidget->item(m_specFileTableRowIndex,
                                                                  m_COLUMN_SAVE_CHECKBOX);
            CaretAssert(saveItem);
            saveItem->setCheckState(WuQtUtilities::boolToCheckState(m_specFile->isModified()));
            
            QTableWidgetItem* nameItem = m_filesTableWidget->item(m_specFileTableRowIndex,
                                                                  m_COLUMN_FILE_NAME_LABEL);
            CaretAssert(nameItem);
            
            FileInformation fileInfo(m_specFile->getFileName());
            const AString path = fileInfo.getAbsolutePath();
            const AString name = fileInfo.getFileName();

            nameItem->setText(name);
            nameItem->setToolTip(path);
            
            QTableWidgetItem* statusItem = m_filesTableWidget->item(m_specFileTableRowIndex,
                                                                    m_COLUMN_STATUS_LABEL);
            CaretAssert(statusItem);
            if (m_specFile->isModified()) {
                statusItem->setText("YES");
            }
            else {
                statusItem->setText("");
            }
        }
    }
    
    const int32_t numDataFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numDataFiles; i++) {
        const int rowIndex = m_tableRowDataFileContent[i]->m_tableRowIndex;
        CaretAssert((rowIndex >= 0) && (rowIndex < m_filesTableWidget->rowCount()));
        
//TODO        update tables cells
    }
    
    
    if (m_specFileStatusItem != NULL) {
        QString statusText = "   ";
        if (m_specFile->isModified()) {
            statusText = "  <font color=\"#ff0000\">YES</font> ";
        }
        m_specFileStatusItem->setText(statusText);
    }
    
    if (m_specFileNameItem != NULL) {
        QString path;
        QString name;
        if (m_specFile->getFileName().isEmpty() == false) {
            FileInformation fileInfo(m_specFile->getFileName());
            path = fileInfo.getAbsolutePath();
            name = fileInfo.getFileName();
            //        QString fileNameText = name;
            //        if (path.isEmpty() == false) {
            //            fileNameText += (" ("
            //                             + path
            //                             + ")");
            //        }
            //        m_specFileNameLabel->setText(fileNameText);
        }
        m_specFileNameItem->setText(name);
        m_specFileNameItem->setToolTip("Path: "
                                            + path);
    }
    
    ManageFilesDisplay manageFilesSelection = MANAGE_FILES_ALL;
    if (m_manageFilesLoadedNotLoadedActionGroup != NULL) {
        QAction* manageFilesAction = m_manageFilesLoadedNotLoadedActionGroup->checkedAction();
        if (manageFilesAction != NULL) {
            const int dataValue = manageFilesAction->data().toInt();
            manageFilesSelection = (ManageFilesDisplay)dataValue;
        }
    }
    
    /*
     * Update list of displayed files using filters
     */
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::UNKNOWN;
    QAction* fileTypeAction = m_fileTypesActionGroup->checkedAction();
    if (fileTypeAction != NULL) {
        const int dataValue = fileTypeAction->data().toInt();
        bool isValid = false;
        const DataFileTypeEnum::Enum dft = DataFileTypeEnum::fromIntegerCode(dataValue,
                                                                             &isValid);
        if (isValid) {
            dataFileType = dft;
        }
    }
    
    StructureEnum::Enum structure = StructureEnum::ALL;
    QAction* structureAction = m_structureActionGroup->checkedAction();
    if (structureAction != NULL) {
        const int dataValue = structureAction->data().toInt();
        bool isValid = false;
        const StructureEnum::Enum s = StructureEnum::fromIntegerCode(dataValue,
                                                                     &isValid);
        if (isValid) {
            structure = s;
        }
    }
    
    if (m_filesTableWidget != NULL) {
        const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
        for (int32_t iGroup = 0; iGroup < numGuiGroups; iGroup++) {
            GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[iGroup];
            const int numFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
            for (int32_t iFile = 0; iFile < numFiles; iFile++) {
                GuiSpecFileDataFile* guiFile = guiSpecGroup->getGuiSpecFileDataFile(iFile);
                bool showFile = guiFile->isVisibleByFileTypeAndStructure(dataFileType,
                                                                         structure);
                switch (m_dialogMode) {
                    case MODE_MANAGE_FILES:
                    {
                        switch (manageFilesSelection) {
                            case MANAGE_FILES_ALL:
                                break;
                            case MANAGE_FILES_LOADED:
                                if (guiFile->m_specFileDataFile->getCaretDataFile() == NULL) {
                                    showFile = false;
                                }
                                break;
                            case MANAGE_FILES_NOT_LOADED:
                                if (guiFile->m_specFileDataFile->getCaretDataFile() != NULL) {
                                    showFile = false;
                                }
                                break;
                        }
                    }
                        break;
                    case MODE_OPEN_SPEC_FILE:
                        break;
                }
                if (guiFile->m_tableRowIndex >= 0) {
                    m_filesTableWidget->setRowHidden(guiFile->m_tableRowIndex,
                                                     (showFile == false));
                }
            }
        }
    }
    else {
        const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
        for (int32_t i = 0; i < numGuiGroups; i++) {
            GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[i];
            
            const int32_t numFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
            if (numFiles > 0) {
                guiSpecGroup->setWidgetsVisibleByFiltering(dataFileType,
                                                           structure,
                                                           manageFilesSelection);
            }
        }
    }
    
    bool showSpecFileWidgets = false;
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
            if ((dataFileType == DataFileTypeEnum::UNKNOWN)
                && (structure == StructureEnum::ALL)
                && (manageFilesSelection != MANAGE_FILES_NOT_LOADED)) {
                showSpecFileWidgets = true;
            }
            break;
        case MODE_OPEN_SPEC_FILE:
            break;
    }
    
    if (m_filesTableWidget != NULL) {
        if (m_specFileTableRowIndex >= 0) {
            m_filesTableWidget->setRowHidden(m_specFileTableRowIndex,
                                             (showSpecFileWidgets == false));
        }
    }
    else if (m_specFileWidgetGroup != NULL) {
        m_specFileWidgetGroup->setVisible(showSpecFileWidgets);
    }
}

/* =================================================================== */

/**
 * Constructor for group of files with same data file type.
 *
 * @param brain
 *    Brain
 * @param dialogMode
 *    Mode of the dialog.
 * @param specFileDataFileTypeGroup
 *   Group of files from spec  file.
 * @param groupName
 *   Name of group.
 * @param parent
 *   Parent for this widget.
 */
GuiSpecFileDataFileTypeGroup::GuiSpecFileDataFileTypeGroup(Brain* brain,
                                                           const SpecFileManagementDialog::Mode dialogMode,
                                                           SpecFileDataFileTypeGroup* specFileDataFileTypeGroup,
                                                           const AString& groupName,
                                                           QObject* parent)
: QObject(parent),
m_brain(brain),
m_dialogMode(dialogMode),
m_specFileDataFileTypeGroup(specFileDataFileTypeGroup)
{
}

/**
 * Destructor.
 */
GuiSpecFileDataFileTypeGroup::~GuiSpecFileDataFileTypeGroup()
{
    
}

/**
 * Update the content of this group.
 */
void
GuiSpecFileDataFileTypeGroup::updateContent()
{
    bool showWidgets = false;
    
    if (m_guiDataFiles.empty() == false) {
        showWidgets = true;
    }
    
    const int32_t numFiles = getNumberOfGuiSpecFileDataFiles();
    for (int32_t i = 0; i < numFiles; i++) {
        getGuiSpecFileDataFile(i)->updateContent();
    }
    
    setWidgetsVisible(showWidgets);
}

/**
 * Set the visibility status for widgets in this group.
 *
 * @param visible
 *    New visibility status.
 */
void
GuiSpecFileDataFileTypeGroup::setWidgetsVisible(bool visible)
{
    const int32_t numFiles = getNumberOfGuiSpecFileDataFiles();
    for (int32_t i = 0; i < numFiles; i++) {
        getGuiSpecFileDataFile(i)->setWidgetsVisible(visible);
    }
}

/**
 * Set the visibility of widgets based upon selected structure
 * and data file type.
 *
 * @param dataFileType
 *   Type of data file.
 * @param structure
 *   The structure.
 */
void
GuiSpecFileDataFileTypeGroup::setWidgetsVisibleByFiltering(const DataFileTypeEnum::Enum dataFileType,
                                                           const StructureEnum::Enum structure,
                                                           const SpecFileManagementDialog::ManageFilesDisplay manageFilesDisplay)
{
    bool showGroup = true;
    
    /*
     * Note: UNKNOWN means show all files
     */
    if (dataFileType != DataFileTypeEnum::UNKNOWN) {
        if (dataFileType != getDataFileType()) {
            showGroup = false;
        }
    }
    
    bool haveFilesDisplayed = false;
    
    const int32_t numFiles = getNumberOfGuiSpecFileDataFiles();
    for (int32_t i = 0; i < numFiles; i++) {
        bool showFile = showGroup;
        
        GuiSpecFileDataFile* gdf = getGuiSpecFileDataFile(i);
        StructureEnum::Enum fileStructure = gdf->getStructure();
        
        switch (structure) {
            case StructureEnum::ALL:
                break;
            case StructureEnum::CEREBELLUM:
            case StructureEnum::CORTEX_LEFT:
            case StructureEnum::CORTEX_RIGHT:
                if (fileStructure != structure) {
                    showFile = false;
                }
                break;
            default:
                if ((fileStructure == StructureEnum::CEREBELLUM)
                    || (fileStructure == StructureEnum::CORTEX_LEFT)
                    || (fileStructure == StructureEnum::CORTEX_RIGHT)) {
                    showFile = false;
                }
                break;
        }
        
        switch (m_dialogMode) {
            case SpecFileManagementDialog::MODE_MANAGE_FILES:
            {
                switch (manageFilesDisplay) {
                    case SpecFileManagementDialog::MANAGE_FILES_ALL:
                        break;
                    case SpecFileManagementDialog::MANAGE_FILES_LOADED:
                        if (gdf->m_specFileDataFile->getCaretDataFile() == NULL) {
                            showFile = false;
                        }
                        break;
                    case SpecFileManagementDialog::MANAGE_FILES_NOT_LOADED:
                        if (gdf->m_specFileDataFile->getCaretDataFile() != NULL) {
                            showFile = false;
                        }
                        break;
                }
            }
                break;
            case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
                break;
        }
        
        gdf->setWidgetsVisible(showFile);
        
        if (showFile) {
            haveFilesDisplayed = true;
        }
    }    
}

/**
 * @return Type of data file in this group.
 */
DataFileTypeEnum::Enum
GuiSpecFileDataFileTypeGroup::getDataFileType() const
{
    return m_specFileDataFileTypeGroup->getDataFileType();
}

int32_t
GuiSpecFileDataFileTypeGroup::getNumberOfGuiSpecFileDataFiles() const
{
    return m_guiDataFiles.size();
}

GuiSpecFileDataFile*
GuiSpecFileDataFileTypeGroup::getGuiSpecFileDataFile(const int32_t indx)
{
    CaretAssertVectorIndex(m_guiDataFiles, indx);
    return m_guiDataFiles[indx];
}

/**
 * Add a gui spec file data file to this group.
 *
 * @param guiSpecFileDataFile
 *    Added to this group if same type.
 */
void
GuiSpecFileDataFileTypeGroup::addGuiSpecFileDataFile(GuiSpecFileDataFile* guiSpecFileDataFile)
{
    m_guiDataFiles.push_back(guiSpecFileDataFile);
}

/* =================================================================== */

/**
 * Data for one CaretDataFile.
 *
 * @param brain
 *    Brain
 * @param dialogMode
 *    Mode of dialog.
 * @param specFileDataFileTypeGroup
 *    Group
 * @param specFileDataFile
 *    Entry in a spec file for a data file.
 * @param caretDataFile
 *    A CaretDataFile instance.
 * @param parent
 *    Parent of this object.
 */
GuiSpecFileDataFile::GuiSpecFileDataFile(const int signalMapperIndex,
                                         Brain* brain,
                                         const SpecFileManagementDialog::Mode dialogMode,
                                         SpecFileDataFileTypeGroup* specFileDataFileTypeGroup,
                                         SpecFileDataFile* specFileDataFile,
                                         QObject* parent)
: QObject(parent),
m_signalMapperIndex(signalMapperIndex),
m_brain(brain),
m_dialogMode(dialogMode),
m_specFileDataFileTypeGroup(specFileDataFileTypeGroup),
m_specFileDataFile(specFileDataFile),
m_tableRowIndex(-1)
{
    CaretDataFile* caretDataFile = m_specFileDataFile->getCaretDataFile();
    
    m_dataTypeLabelItem = NULL;
    m_inSpecFileCheckedItem = NULL;
    m_saveCheckedItem = NULL;
    m_loadCheckedItem = NULL;
    m_statusLabelItem = NULL;
    m_reloadOrOpenFileAction = NULL;
    m_reloadOrOpenFileToolButton = NULL;
    m_structureLabelItem = NULL;
    m_selectFileNameButtonAction = NULL;
    m_selectFileNameToolButton = NULL;
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
            /*
             * Checkbox for saving file
             */
            m_saveCheckedItem = new QTableWidgetItem();
            m_saveCheckedItem->setFlags(Qt::ItemIsEnabled
                                 | Qt::ItemIsUserCheckable);
            m_saveCheckedItem->setCheckState(Qt::Unchecked);
            if (caretDataFile != NULL) {
                if (caretDataFile->isModified()) {
                    m_saveCheckedItem->setCheckState(Qt::Checked);
                }
            }
            
            m_inSpecFileCheckedItem = new QTableWidgetItem();
            m_inSpecFileCheckedItem->setFlags(Qt::ItemIsEnabled
                                 | Qt::ItemIsUserCheckable);
            m_inSpecFileCheckedItem->setCheckState(Qt::Unchecked);
            
            m_reloadOrOpenFileAction = WuQtUtilities::createAction("Open",
                                                                   "tooltip",
                                                                   this);
            m_reloadOrOpenFileToolButton = new QToolButton();
            m_reloadOrOpenFileToolButton->setDefaultAction(m_reloadOrOpenFileAction);
            
            m_statusLabelItem = new QTableWidgetItem();
            m_statusLabelItem->setFlags(Qt::ItemIsEnabled);
            m_statusLabelItem->setToolTip("L -> file is loaded\n"
                                      "M -> file is modified\n"
                                      "D -> file is displayed");
            
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            /*
             * Checkbox for loading file
             */
            m_loadCheckedItem = new QTableWidgetItem();
            m_loadCheckedItem->setFlags(Qt::ItemIsEnabled
                                 | Qt::ItemIsUserCheckable);
            m_loadCheckedItem->setCheckState(Qt::Unchecked);
            
            if (DataFileTypeEnum::isFileUsedWithOneStructure(m_specFileDataFile->getDataFileType())) {
                m_structureLabelItem = new QTableWidgetItem();
                m_structureLabelItem->setFlags(Qt::ItemIsEnabled);
            }
            break;
    }
    
    m_dataTypeLabelItem =  new QTableWidgetItem();
    m_dataTypeLabelItem->setFlags(Qt::ItemIsEnabled);
    
    m_optionsButtonAction  = WuQtUtilities::createAction("Options",
                                                         "tooltip",
                                                         this);
    m_optionsToolButton = new QToolButton();
    m_optionsToolButton->setDefaultAction(m_optionsButtonAction);
    
    m_fileNameLabelItem = new QTableWidgetItem();
    m_fileNameLabelItem->setFlags(Qt::ItemIsEnabled);

    m_widgetGroup = new WuQWidgetObjectGroup(this);
    if (m_reloadOrOpenFileToolButton != NULL) m_widgetGroup->add(m_reloadOrOpenFileToolButton);
    if (m_optionsToolButton != NULL) m_widgetGroup->add(m_optionsToolButton);
    if (m_selectFileNameToolButton != NULL) m_widgetGroup->add(m_selectFileNameToolButton);
}

/**
 * Destructor.
 */
GuiSpecFileDataFile::~GuiSpecFileDataFile()
{
}

/**
 * @return The signal mapper index.
 */
int
GuiSpecFileDataFile::getSignalMapperIndex() const
{
    return m_signalMapperIndex;
}

/**
 * Update the content of this item.
 */
void
GuiSpecFileDataFile::updateContent()
{
    m_widgetGroup->blockAllSignals(true);
    
    //QString loadedText = " ";
    QString modifiedText = " ";
    //QString displayedText = " ";
    
    QString filename = m_specFileDataFile->getFileName();
    CaretDataFile* caretDataFile = m_specFileDataFile->getCaretDataFile();
    if (caretDataFile != NULL) {
        filename = caretDataFile->getFileName();
        
        //loadedText = "L";
        if (caretDataFile->isModified()) {
            modifiedText = "<font color=\"#ff0000\">YES</font>";
        }
        if (caretDataFile->isDisplayedInGUI()) {
            //displayedText = "D";
        }
        
        if (m_reloadOrOpenFileAction != NULL) {
            m_reloadOrOpenFileAction->setText("Reload");
        }
    }
    else {
        filename = m_specFileDataFile->getFileName();
        if (m_reloadOrOpenFileAction != NULL) {
            m_reloadOrOpenFileAction->setText("Open");
        }
    }
    
    bool isFileSavable = true;
    switch (m_specFileDataFile->getDataFileType()) {
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            isFileSavable = false;
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
        default:
            break;
    }
    
    if (isFileSavable == false) {
        if (m_saveCheckedItem != NULL) {
            m_saveCheckedItem->setFlags(m_saveCheckedItem->flags() & (~Qt::ItemIsEnabled));
            m_saveCheckedItem->setCheckState(Qt::Unchecked);
        }
        
        modifiedText = " ";
    }
    
    if (m_selectFileNameButtonAction != NULL) {
        m_selectFileNameButtonAction->setEnabled(false);
        if (m_specFileDataFile->getCaretDataFile() != NULL) {
            if (isFileSavable) {
                m_selectFileNameButtonAction->setEnabled(true);
            }
        }
    }
    
    if (m_loadCheckedItem != NULL) {
        m_loadCheckedItem->setCheckState(m_specFileDataFile->isSelected()
                                  ? Qt::Checked
                                  : Qt::Unchecked);
    }
    if (m_inSpecFileCheckedItem != NULL) {
        m_inSpecFileCheckedItem->setCheckState(m_specFileDataFile->isSpecFileMember()
                                               ? Qt::Checked
                                               : Qt::Unchecked);
    }
    if (m_structureLabelItem != NULL) {
        m_structureLabelItem->setText(StructureEnum::toName(m_specFileDataFile->getStructure()));
    }
    
    QString typeName = DataFileTypeEnum::toGuiName(m_specFileDataFile->getDataFileType());
    typeName = SpecFileManagementDialog::getEditedGroupName(typeName);
    m_dataTypeLabelItem->setText(typeName);
    
    /*
     * Format the file label's text: name (path)
     */
    if (filename.isEmpty()) {
        m_fileNameLabelItem->setText("");
    }
    else {
        FileInformation fileInfo(filename);
        const QString path = fileInfo.getAbsolutePath();
        const QString name = fileInfo.getFileName();
        m_fileNameLabelItem->setText(name);
        m_fileNameLabelItem->setToolTip("Path: "
                                        + path);
        //        QString fileNameText = name;
        //        if (path.isEmpty() == false) {
        //            fileNameText += (" ("
        //                             + path
        //                             + ")");
        //            m_fileNameLabelItem->setText(fileNameText);
        //        }
    }
    
    if (m_statusLabelItem != NULL) {
//        AString lmdText = ("<html>"
//                           //+ loadedText
//                           + modifiedText
//                           //+ displayedText
//                           + "</html>");
        AString lmdText = modifiedText;
        
        m_statusLabelItem->setText(lmdText);
    }
    
    m_widgetGroup->blockAllSignals(false);
}

/**
 * Set the spec file data file for this instance.  This typically occurs
 * when a data file is saved AND added to the spec file.
 *
 * @param specFileDataFile
 *     Spec file data file that is added.
 */
void
GuiSpecFileDataFile::setSpecFileDataFile(SpecFileDataFile* specFileDataFile)
{
    m_specFileDataFile = specFileDataFile;
    
    updateContent();
}

/**
 * @return The data file type.
 */
DataFileTypeEnum::Enum
GuiSpecFileDataFile::getDataFileType() const
{
    CaretDataFile* caretDataFile = m_specFileDataFile->getCaretDataFile();
    if (caretDataFile != NULL) {
        return caretDataFile->getDataFileType();
    }
    return m_specFileDataFile->getDataFileType();
}

/**
 * @return The structure.
 */
StructureEnum::Enum
GuiSpecFileDataFile::getStructure() const
{
    CaretDataFile* caretDataFile = m_specFileDataFile->getCaretDataFile();
    if (caretDataFile != NULL) {
        return caretDataFile->getStructure();
    }
    
    return m_specFileDataFile->getStructure();
}

/**
 * Set the structure.
 *
 * @param structure
 *    New value for structure.
 */
void
GuiSpecFileDataFile::setStructure(const StructureEnum::Enum structure)
{
    CaretDataFile* caretDataFile = m_specFileDataFile->getCaretDataFile();
    if (caretDataFile != NULL) {
        caretDataFile->setStructure(structure);
    }
    m_specFileDataFile->setStructure(structure);
    if (m_structureLabelItem != NULL) {
        m_structureLabelItem->setText(StructureEnum::toName(structure));
    }
}


/**
 * Set the widgets enabled status.
 *
 * @param enabled
 *    New enabled status.
 */
void
GuiSpecFileDataFile::setWidgetsEnabled(const bool enabled)
{
    m_widgetGroup->setEnabled(enabled);
}

/**
 * Set the widgets visible status.
 *
 * @param visible
 *    New visible status.
 */
void
GuiSpecFileDataFile::setWidgetsVisible(const bool visible)
{
    m_widgetGroup->setVisible(visible);
}

/**
 * Deterimine if a file should  be displayed based upon the filters
 * for data file type and structure.
 *
 * @param dataFileType
 *    Type of data file.
 * @param structure
 *    The structure.
 */
bool
GuiSpecFileDataFile::isVisibleByFileTypeAndStructure(const DataFileTypeEnum::Enum dataFileType,
                                                     const StructureEnum::Enum structure)
{
    /*
     * Note: UNKNOWN means show all files
     */
    if (dataFileType != DataFileTypeEnum::UNKNOWN) {
        if (dataFileType != getDataFileType()) {
            return false;
        }
    }
    
    StructureEnum::Enum fileStructure = getStructure();
    
    switch (structure) {
        case StructureEnum::ALL:
            break;
        case StructureEnum::CEREBELLUM:
        case StructureEnum::CORTEX_LEFT:
        case StructureEnum::CORTEX_RIGHT:
            if (fileStructure != structure) {
                return false;
            }
            break;
        default:
            if ((fileStructure == StructureEnum::CEREBELLUM)
                || (fileStructure == StructureEnum::CORTEX_LEFT)
                || (fileStructure == StructureEnum::CORTEX_RIGHT)) {
                return false;
            }
            break;
    }
    
    return true;
}


