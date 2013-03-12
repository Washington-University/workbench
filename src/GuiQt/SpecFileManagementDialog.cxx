
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
#include <QBrush>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFontDatabase>
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
    m_fileReloadOrOpenFileActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileReloadOrOpenFileActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileReloadOrOpenFileActionSelected(int)));
    
    m_fileOptionsActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileOptionsActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileOptionsActionSelected(int)));
    
    m_fileRemoveFileActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileRemoveFileActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileRemoveActionSelected(int)));

    int tableRowCounter = 0;
    
    /*
     * Is there a spec file?  If so, set its table row index
     */
    if (enableManageItems) {
        m_specFileTableRowIndex = tableRowCounter++;
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
    m_COLUMN_FILE_NAME_LABEL             = -1;
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
    m_COLUMN_STRUCTURE = columnCounter++;
    m_COLUMN_FILE_NAME_LABEL = columnCounter++;
    m_COLUMN_COUNT = columnCounter++;

    /*
     * Get the content from the spec file.
     * It is examined when creating some of the toolbars.
     */
    getDataFileContentFromSpecFile();

    /*
     * Create the table
     */
    m_filesTableWidget = new QTableWidget();
    m_filesTableWidget->setSelectionBehavior(QTableWidget::SelectItems);
    m_filesTableWidget->setSelectionMode(QTableWidget::SingleSelection);
    QObject::connect(m_filesTableWidget, SIGNAL(cellChanged(int,int)),
                     this, SLOT(filesTableWidgetCellChanged(int,int)));
    
    /*
     * Widget and layout for files.
     *
     * Two layouts used so widget is pushed to the top left (and not
     * spread out) when groups of files are hidden.
     */
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
    toolbarWidget->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Fixed);

    /*
     * No scrollbars in dialog since the table widget will have scroll bars
     */
    const bool enableScrollBars = false;

    setTopBottomAndCentralWidgets(toolbarWidget,
                                  centralWidget,
                                  NULL,
                                  enableScrollBars);
    
    /*
     * Display the data files.
     */
    updateTableDimensionsToFitFiles();
    loadSpecFileContentIntoDialog();
    
    disableAutoDefaultForAllPushButtons();

//    /*
//     * Table widget has a default size of 640 x 480.
//     * So estimate the size of the dialog with the table fully 
//     * expanded.
//     */
//    QHeaderView* columnHeader = m_filesTableWidget->horizontalHeader();
//    const int columnHeaderHeight = columnHeader->sizeHint().height();
//    
//    int dialogWidth = 10; // start out with a little extra space
//    int dialogHeight = 0;
//    
//    const int numRows = m_filesTableWidget->rowCount();
//    const int numCols = m_filesTableWidget->columnCount();
//    const int cellGap = 1;
//    if ((numRows > 0)
//        && (numCols > 0)) {
//        for (int i = 0; i < numCols; i++) {
//            dialogWidth += (m_filesTableWidget->columnWidth(i) + cellGap);
//            std::cout << "column width " << i << ": " << m_filesTableWidget->columnWidth(i) << std::endl;
//        }
//        
//        for (int i = 0; i < numRows; i++) {
//            dialogHeight += (m_filesTableWidget->rowHeight(i) + cellGap);
//        }
//    }
    
    const QSize tableSize = WuQtUtilities::estimateTableWidgetSize(m_filesTableWidget);
    
    int dialogWidth = std::max(tableSize.width(),
                           toolbarWidget->sizeHint().width());
    int dialogHeight = (toolbarWidget->sizeHint().height()
                     + getDialogButtonBox()->sizeHint().height()
                     + tableSize.height());
    
    WuQtUtilities::resizeWindow(this,
                                dialogWidth,
                                dialogHeight);
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

/**
 * Get the info about the data files from the spec file.
 */
void
SpecFileManagementDialog::getDataFileContentFromSpecFile()
{
    m_tableRowDataFileContent.clear();
    
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
    
    if (m_loadScenesPushButton != NULL) {
        m_loadScenesPushButton->setEnabled(haveSceneFiles);
    }
}

/**
 * Called when the content of a cell changes.
 * Update corresponding item in the spec file.
 *
 * @param rowIndex
 *    The row of the cell that was clicked.
 * @param columnIndex
 *    The columnof the cell that was clicked.
 */
void
SpecFileManagementDialog::filesTableWidgetCellChanged(int rowIndex, int columnIndex)
{
    QTableWidgetItem* item = getTableWidgetItem(rowIndex, columnIndex);
    if (item != NULL) {
        /*
         * Is this the row containing the spec file?
         */
        if (rowIndex == m_specFileTableRowIndex) {
            
        }
        else {
            TableRowDataFileContent* rowContent = getFileContentInRow(rowIndex);
            CaretAssert(rowContent);
            SpecFileDataFile* sfdf = rowContent->m_specFileDataFile;
            
            const bool isSelected = WuQtUtilities::checkStateToBool(item->checkState());
            
            if (columnIndex == m_COLUMN_SAVE_CHECKBOX) {
                sfdf->setSavingSelected(isSelected);
            }
            else if (columnIndex == m_COLUMN_LOAD_CHECKBOX) {
                sfdf->setLoadingSelected(isSelected);
            }
            else if (columnIndex == m_COLUMN_IN_SPEC_FILE_CHECKBOX) {
                sfdf->setSpecFileMember(isSelected);
                updateSpecFileRowInTable();
            }
        }
    }
}

/**
 * Get the table widget item at the given row and column.  If compiled
 * debug the assertions will fail if the row or column is invalid.
 *
 * @param rowIndex
 *    The row of the desired cell.
 * @param columnIndex
 *    The column of the desired cell.
 * @return
 *    item at row and column.
 */
QTableWidgetItem*
SpecFileManagementDialog::getTableWidgetItem(const int rowIndex,
                                             const int columnIndex)
{
    CaretAssert((rowIndex >= 0) && (rowIndex < m_filesTableWidget->rowCount()));
    CaretAssert((columnIndex >= 0) && (columnIndex < m_filesTableWidget->columnCount()));
    return m_filesTableWidget->item(rowIndex,
                                    columnIndex);
}

/**
 * Set the table widget item at the given row and column.  If compiled
 * debug the assertions will fail if the row or column is invalid.
 *
 * @param rowIndex
 *    The row of the desired cell.
 * @param columnIndex
 *    The column of the desired cell.
 * @param item
 *    The item to add.
 */
void
SpecFileManagementDialog::setTableWidgetItem(const int rowIndex,
                        const int columnIndex,
                        QTableWidgetItem* item)
{
    CaretAssert(item);
    CaretAssert((rowIndex >= 0) && (rowIndex < m_filesTableWidget->rowCount()));
    CaretAssert((columnIndex >= 0) && (columnIndex < m_filesTableWidget->columnCount()));
    m_filesTableWidget->blockSignals(true);
    m_filesTableWidget->setItem(rowIndex,
                                columnIndex,
                                item);
    m_filesTableWidget->blockSignals(false);
}

/**
 * Set the labels for the column names in the table.
 */
void
SpecFileManagementDialog::setTableColumnLabels()
{
//    /*
//     * Set names of table's columns
//     */
//    QStringList tableWidgetColumnLabels;
//    tableWidgetColumnLabels.reserve(m_COLUMN_COUNT);
//    for (int32_t i = 0; i < m_COLUMN_COUNT; i++) {
//        tableWidgetColumnLabels.append("");
//    }
//    if (m_COLUMN_LOAD_CHECKBOX >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_LOAD_CHECKBOX, "Load");
//    }
//    if (m_COLUMN_SAVE_CHECKBOX >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_SAVE_CHECKBOX, "Save");
//    }
//    if (m_COLUMN_STATUS_LABEL >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_STATUS_LABEL, "Modified");
//    }
//    if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_IN_SPEC_FILE_CHECKBOX, "In Spec");
//    }
//    if (m_COLUMN_READ_BUTTON >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_READ_BUTTON, "Read");
//    }
//    if (m_COLUMN_REMOVE_BUTTON >= 0) {
//        tableWidgetColumnLabels.replace(m_COLUMN_REMOVE_BUTTON, "Remove");
//    }
//    if (m_COLUMN_OPTIONS_TOOLBUTTON >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_OPTIONS_TOOLBUTTON, "More");
//    }
//    if (m_COLUMN_DATA_FILE_TYPE_LABEL >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_DATA_FILE_TYPE_LABEL, "Data Type");
//    }
//    if (m_COLUMN_STRUCTURE >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_STRUCTURE, "Structure");
//    }
//    if (m_COLUMN_FILE_NAME_LABEL >= 0){
//        tableWidgetColumnLabels.replace(m_COLUMN_FILE_NAME_LABEL, "Data File Name");
//    }
//    
//    m_filesTableWidget->setHorizontalHeaderLabels(tableWidgetColumnLabels);
    
    /*
     * Set names of table's columns
     */
    if (m_COLUMN_LOAD_CHECKBOX >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_LOAD_CHECKBOX,
                                                    createHeaderTextItem("Load"));
    }
    if (m_COLUMN_SAVE_CHECKBOX >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_SAVE_CHECKBOX,
                                                    createHeaderTextItem("Save"));
    }
    if (m_COLUMN_STATUS_LABEL >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_STATUS_LABEL,
                                                    createHeaderTextItem("Modified"));
    }
    if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_IN_SPEC_FILE_CHECKBOX,
                                                    createHeaderTextItem("In Spec"));
    }
    if (m_COLUMN_READ_BUTTON >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_READ_BUTTON,
                                                    createHeaderTextItem("Read"));
    }
    if (m_COLUMN_REMOVE_BUTTON >= 0) {
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_REMOVE_BUTTON,
                                                    createHeaderTextItem("Remove"));
    }
    if (m_COLUMN_OPTIONS_TOOLBUTTON >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_OPTIONS_TOOLBUTTON,
                                                    createHeaderTextItem("More"));
    }
    if (m_COLUMN_DATA_FILE_TYPE_LABEL >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_DATA_FILE_TYPE_LABEL,
                                                    createHeaderTextItem("Data Type"));
    }
    if (m_COLUMN_STRUCTURE >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_STRUCTURE,
                                                    createHeaderTextItem("Structure"));
    }
    if (m_COLUMN_FILE_NAME_LABEL >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_FILE_NAME_LABEL,
                                                    createHeaderTextItem("Data File Name"));
    }
}

/**
 * @return A table widget item used in the column header.
 *
 * @param text
 *    Text for the item.
 * @return
      Table widget item containing the given text.
 */
QTableWidgetItem*
SpecFileManagementDialog::createHeaderTextItem(const QString& text)
{
    QList<int> sizesList = QFontDatabase::standardSizes();
    std::sort(sizesList.begin(), sizesList.end());
    
    QTableWidgetItem* item = createTextItem();
    item->setText(text);
    QFont font = item->font();
    
    QListIterator<int> sizeIter(sizesList);
    while (sizeIter.hasNext()) {
        const int nextSize = sizeIter.next();
        if (nextSize > font.pointSize()) {
            font.setPointSize(nextSize);
            break;
        }
    }
    
    font.setBold(true);
    item->setFont(font);
    return item;
}

/**
 * Load items into the table widget adding rows as needed.
 */
void
SpecFileManagementDialog::updateTableDimensionsToFitFiles()
{
    
    /*
     * If needed, add a row for the spec file
     */
    m_specFileTableRowIndex = -1;
    int numberOfRows = 0;
    if (m_dialogMode == MODE_MANAGE_FILES) {
        m_specFileTableRowIndex = numberOfRows;
        numberOfRows++;
    }
    
    /*
     * Update rows indices for data files
     */
    const int32_t numFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numFiles; i++) {
        m_tableRowDataFileContent[i]->m_tableRowIndex = numberOfRows;
        numberOfRows++;
    }
    
    /*
     * If the number of rows has not changed, no need to update table dimensions
     */
    bool needToCreateColumnHeaders = false;
    if (numberOfRows == m_filesTableWidget->rowCount()) {
        return;
    }
    else if (m_filesTableWidget->rowCount() == 0) {
        needToCreateColumnHeaders = true;
    }
    
    
    /*
     * Update the dimensions of the table
     */
    const int32_t firstNewRowIndex = m_filesTableWidget->rowCount();
    m_filesTableWidget->setRowCount(numberOfRows);
    m_filesTableWidget->setColumnCount(m_COLUMN_COUNT);
    m_filesTableWidget->verticalHeader()->hide();
    m_filesTableWidget->setGridStyle(Qt::NoPen);
    m_filesTableWidget->setSortingEnabled(false);
    if (needToCreateColumnHeaders) {
        setTableColumnLabels();
    }
    const int32_t lastNewRowIndex  = m_filesTableWidget->rowCount(); // value changed by setRowCount()
    
    /*
     * Add new cells to the table widget
     */
    for (int32_t iRow = firstNewRowIndex; iRow < lastNewRowIndex; iRow++) {
        const bool isDataFileRow = (m_specFileTableRowIndex != iRow);
        
        if (isDataFileRow) {
            if (m_COLUMN_LOAD_CHECKBOX >= 0) {
                QTableWidgetItem* item = createCheckableItem();
                item->setTextAlignment(Qt::AlignHCenter);
                setTableWidgetItem(iRow,
                                   m_COLUMN_LOAD_CHECKBOX,
                                   item);
            }
        }

        if (m_COLUMN_SAVE_CHECKBOX >= 0) {
            QTableWidgetItem* item = createCheckableItem();
            item->setTextAlignment(Qt::AlignHCenter);
            setTableWidgetItem(iRow,
                               m_COLUMN_SAVE_CHECKBOX,
                               item);
        }
        
        if (m_COLUMN_STATUS_LABEL >= 0) {
            QTableWidgetItem* item = createTextItem();
            setTableWidgetItem(iRow,
                               m_COLUMN_STATUS_LABEL,
                               item);
            
            /*
             * Text for modified cell is always red and centered
             */
            item->setTextAlignment(Qt::AlignHCenter);
            QBrush modifiedBrush = item->foreground();
            modifiedBrush.setColor(Qt::red);
            item->setForeground(modifiedBrush);
        }
        
        if (isDataFileRow) {
            if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0) {
                QTableWidgetItem* item = createCheckableItem();
                item->setTextAlignment(Qt::AlignHCenter);
                setTableWidgetItem(iRow,
                                   m_COLUMN_IN_SPEC_FILE_CHECKBOX,
                                   item);
            }
            if (m_COLUMN_READ_BUTTON >= 0) {
                QAction* loadFileAction = WuQtUtilities::createAction("Reload",
                                                                      "Read or reload a file",
                                                                      this);
                QToolButton* loadFileToolButton = new QToolButton();
                loadFileToolButton->setDefaultAction(loadFileAction);
                
                QObject::connect(loadFileAction, SIGNAL(triggered()),
                                 m_fileReloadOrOpenFileActionSignalMapper, SLOT(map()));
                m_fileReloadOrOpenFileActionSignalMapper->setMapping(loadFileAction, iRow);
                
                m_filesTableWidget->setCellWidget(iRow,
                                                  m_COLUMN_READ_BUTTON,
                                                  loadFileToolButton);
            }
            
            if (m_COLUMN_REMOVE_BUTTON >= 0) {
                QAction* removeFileAction = WuQtUtilities::createAction("X",
                                                                        "Remove a file from memory.\n"
                                                                        "Does NOT delete the file on disk.",
                                                                        this);
                QToolButton* removeFileToolButton = new QToolButton();
                removeFileToolButton->setDefaultAction(removeFileAction);
                
                QObject::connect(removeFileAction, SIGNAL(triggered()),
                                 m_fileRemoveFileActionSignalMapper, SLOT(map()));
                m_fileRemoveFileActionSignalMapper->setMapping(removeFileAction, iRow);
                
                m_filesTableWidget->setCellWidget(iRow,
                                                  m_COLUMN_REMOVE_BUTTON,
                                                  removeFileToolButton);
            }
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
            setTableWidgetItem(iRow,
                                        m_COLUMN_DATA_FILE_TYPE_LABEL,
                                        createTextItem());
        }
        if (m_COLUMN_STRUCTURE >= 0) {
            setTableWidgetItem(iRow,
                                        m_COLUMN_STRUCTURE,
                                        createTextItem());
        }
        if (m_COLUMN_FILE_NAME_LABEL >= 0) {
            setTableWidgetItem(iRow,
                                        m_COLUMN_FILE_NAME_LABEL,
                                        createTextItem());
        }
    }
}

/**
 * Update the table row containing the spec file.
 */
void
SpecFileManagementDialog::updateSpecFileRowInTable()
{
    /*
     * Update spec file data
     */
    if (m_dialogMode == MODE_MANAGE_FILES) {
        
        if (m_specFileTableRowIndex >= 0) {
            CaretAssert(m_COLUMN_DATA_FILE_TYPE_LABEL >= 0);
            QTableWidgetItem* dataTypeItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                            m_COLUMN_DATA_FILE_TYPE_LABEL);
            dataTypeItem->setText(getEditedDataFileTypeName(DataFileTypeEnum::SPECIFICATION));
            
            CaretAssert(m_COLUMN_SAVE_CHECKBOX >= 0);
            QTableWidgetItem* saveItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                            m_COLUMN_SAVE_CHECKBOX);
            CaretAssert(saveItem);
            // saveItem->setCheckState(WuQtUtilities::boolToCheckState(m_specFile->is()));
            
            CaretAssert(m_COLUMN_FILE_NAME_LABEL >= 0);
            QTableWidgetItem* nameItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                            m_COLUMN_FILE_NAME_LABEL);
            CaretAssert(nameItem);
            
            const AString specFileName = m_specFile->getFileName();
            AString path;
            AString name;
            if (specFileName.isEmpty() == false) {
                FileInformation fileInfo(specFileName);
                path = fileInfo.getAbsolutePath();
                name = fileInfo.getFileName();
            }
            
            nameItem->setText(name);
            nameItem->setToolTip(path);
            
            CaretAssert(m_COLUMN_STATUS_LABEL >= 0);
            QTableWidgetItem* statusItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                              m_COLUMN_STATUS_LABEL);
            CaretAssert(statusItem);
            if (m_specFile->isModified()) {
                statusItem->setText("YES");
            }
            else {
                statusItem->setText("");
            }

//            /*
//             * Hide in Spec Check Box
//             */
//            CaretAssert(m_COLUMN_IN_SPEC_FILE_CHECKBOX);
//            QTableWidgetItem* inSpecItem = getTableWidgetItem(m_specFileTableRowIndex,
//                                                              m_COLUMN_IN_SPEC_FILE_CHECKBOX);
//            inSpecItem->setFlags(inSpecItem->flags()
//                                 & (~Qt::ItemIsEnabled));
//            inSpecItem->setFlags(inSpecItem->flags()
//                                 & (~Qt::ItemIsUserCheckable));
            
            /*
             * Get filtering selections
             */
            ManageFilesDisplay manageFilesDisplay;
            DataFileTypeEnum::Enum filteredDataFileType;
            StructureEnum::Enum filteredStructureType;
            getFilterSelections(manageFilesDisplay,
                                filteredDataFileType,
                                filteredStructureType);
            
            bool hideSpecFileRow = false;
            
            if ((filteredStructureType != StructureEnum::ALL)
                && (filteredStructureType != StructureEnum::OTHER)) {
                hideSpecFileRow = true;
            }
            
            switch (manageFilesDisplay) {
                case MANAGE_FILES_ALL:
                    break;
                case MANAGE_FILES_LOADED:
                    break;
                case MANAGE_FILES_NOT_LOADED:
                    hideSpecFileRow = true;
                    break;
            }
            
            if (filteredDataFileType != DataFileTypeEnum::UNKNOWN) {
                hideSpecFileRow = true;
            }
            
            m_filesTableWidget->setRowHidden(m_specFileTableRowIndex,
                                             hideSpecFileRow);
        }
    }
}

/**
 * Get the file filtering selections
 *
 * @param manageFilesDisplayOut
 *    Manage files loaded/not loaded
 * @param filteredDataFileTypeOut
 *    Data file type
 * @param filteredStructureTypeOut
 *    Structure
 */
void
SpecFileManagementDialog::getFilterSelections(ManageFilesDisplay& manageFilesDisplayOut,
                                              DataFileTypeEnum::Enum& filteredDataFileTypeOut,
                                              StructureEnum::Enum& filteredStructureTypeOut) const
{
    /*
     * All/Loaded/Not-Loaded
     */
    manageFilesDisplayOut = MANAGE_FILES_ALL;
    if (m_manageFilesLoadedNotLoadedActionGroup != NULL) {
        const QAction* manageFileAction = m_manageFilesLoadedNotLoadedActionGroup->checkedAction();
        if (manageFileAction != NULL) {
            manageFilesDisplayOut = static_cast<ManageFilesDisplay>(manageFileAction->data().toInt());
        }
    }
    
    /*
     * Filter for data file type selection
     */
    filteredDataFileTypeOut = DataFileTypeEnum::UNKNOWN;
    const QAction* dataFileTypeAction = m_fileTypesActionGroup->checkedAction();
    if (dataFileTypeAction != NULL) {
        const int dataFileTypeInt = dataFileTypeAction->data().toInt();
        filteredDataFileTypeOut = DataFileTypeEnum::fromIntegerCode(dataFileTypeInt,
                                                                 NULL);
    }
    
    /*
     * Filter for structure
     */
    filteredStructureTypeOut = StructureEnum::ALL;
    const QAction* filteredStructureAction = m_structureActionGroup->checkedAction();
    if (filteredStructureAction != NULL) {
        const int structureTypeInt = filteredStructureAction->data().toInt();
        filteredStructureTypeOut = StructureEnum::fromIntegerCode(structureTypeInt,
                                                               NULL);
    }
}

/**
 * Load the spec file data into the dialog.
 */
void
SpecFileManagementDialog::loadSpecFileContentIntoDialog()
{
    /*
     * Disable signals so cell changed signal not emitted while
     * modifying table content.
     */
    m_filesTableWidget->blockSignals(true);
    
    ManageFilesDisplay manageFilesDisplay;
    DataFileTypeEnum::Enum filteredDataFileType;
    StructureEnum::Enum filteredStructureType;
    getFilterSelections(manageFilesDisplay,
                        filteredDataFileType,
                        filteredStructureType);
    
    updateSpecFileRowInTable();
    
    /*
     * Load all of the data file content.
     */
    const int32_t numDataFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numDataFiles; i++) {
        const int rowIndex = m_tableRowDataFileContent[i]->m_tableRowIndex;
        CaretAssert((rowIndex >= 0) && (rowIndex < m_filesTableWidget->rowCount()));
        
        SpecFileDataFile* specFileDataFile = m_tableRowDataFileContent[i]->m_specFileDataFile;
        
        CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
        const StructureEnum::Enum structure = specFileDataFile->getStructure();
        
        
        bool isFileSavable = true;
        const DataFileTypeEnum::Enum dataFileType = specFileDataFile->getDataFileType();
        switch (dataFileType) {
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
        
        switch (m_dialogMode) {
            case MODE_MANAGE_FILES:
            {
                /*
                 * Save checkbox
                 */
                CaretAssert(m_COLUMN_SAVE_CHECKBOX >= 0);
                QTableWidgetItem* saveItem = getTableWidgetItem(rowIndex,
                                                                      m_COLUMN_SAVE_CHECKBOX);
                CaretAssert(saveItem);
                saveItem->setCheckState(WuQtUtilities::boolToCheckState(specFileDataFile->isSavingSelected()));
                if (isFileSavable) {
                    saveItem->setFlags(saveItem->flags() | Qt::ItemIsSelectable);
                }
                else {
                    saveItem->setFlags(saveItem->flags() & (~ Qt::ItemIsSelectable));
                }
                                
                /*
                 * Status label.
                 */
                CaretAssert(m_COLUMN_STATUS_LABEL >= 0);
                QTableWidgetItem* statusItem = getTableWidgetItem(rowIndex,
                                                                      m_COLUMN_STATUS_LABEL);
                CaretAssert(statusItem);
                statusItem->setText("");
                if (caretDataFile != NULL) {
                    if (caretDataFile->isModified()) {
                        statusItem->setText("YES");
                    }
                }
                
                /*
                 * In-spec checkbox
                 */
                CaretAssert(m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0);
                QTableWidgetItem* inSpecItem = getTableWidgetItem(rowIndex,
                                                                        m_COLUMN_IN_SPEC_FILE_CHECKBOX);
                CaretAssert(inSpecItem);
                inSpecItem->setCheckState(WuQtUtilities::boolToCheckState(specFileDataFile->isSpecFileMember()));
                
                /*
                 * Read button
                 */
                CaretAssert(m_COLUMN_READ_BUTTON >= 0);
                QWidget* readToolButtonWidget = m_filesTableWidget->cellWidget(rowIndex,
                                                                               m_COLUMN_READ_BUTTON);
                CaretAssert(readToolButtonWidget);
                QToolButton* readToolButton = dynamic_cast<QToolButton*>(readToolButtonWidget);
                CaretAssert(readToolButton);
                QList<QAction*> readToolButtonActionList = readToolButton->actions();
                CaretAssert(readToolButtonActionList.size() > 0);
                QAction* readToolButtonAction = readToolButtonActionList.at(0);
                CaretAssert(readToolButtonAction);
                if (caretDataFile != NULL) {
                    readToolButtonAction->setText("Reload");
                }
                else {
                    readToolButtonAction->setText("Open");
                }
            }
                break;
            case MODE_OPEN_SPEC_FILE:
            {
                /*
                 * Load checkbox
                 */
                CaretAssert(m_COLUMN_LOAD_CHECKBOX >= 0);
                QTableWidgetItem* loadItem = getTableWidgetItem(rowIndex,
                                                                      m_COLUMN_LOAD_CHECKBOX);
                CaretAssert(loadItem);
                loadItem->setCheckState(WuQtUtilities::boolToCheckState(specFileDataFile->isLoadingSelected()));
            }
                break;
        }
        
        /*
         * Data file type label
         */
        CaretAssert(m_COLUMN_DATA_FILE_TYPE_LABEL >= 0);
        QTableWidgetItem* dataTypeItem = getTableWidgetItem(rowIndex,
                                                                   m_COLUMN_DATA_FILE_TYPE_LABEL);
        CaretAssert(dataTypeItem);
        dataTypeItem->setText(SpecFileManagementDialog::getEditedDataFileTypeName(dataFileType));
        
        /*
         * Structure label
         */
        CaretAssert(m_COLUMN_STRUCTURE >= 0);
        QTableWidgetItem* structureItem = getTableWidgetItem(rowIndex,
                                                                m_COLUMN_STRUCTURE);
        CaretAssert(structureItem);
        structureItem->setText("");
        if (DataFileTypeEnum::isFileUsedWithOneStructure(dataFileType)) {
            structureItem->setText(StructureEnum::toGuiName(specFileDataFile->getStructure()));
        }
        
        /*
         * File name and path
         */
        CaretAssert(m_COLUMN_FILE_NAME_LABEL >= 0);
        QTableWidgetItem* nameItem = getTableWidgetItem(rowIndex,
                                                              m_COLUMN_FILE_NAME_LABEL);
        CaretAssert(nameItem);
        
        FileInformation fileInfo(specFileDataFile->getFileName());
        const AString path = fileInfo.getAbsolutePath();
        const AString name = fileInfo.getFileName();
        
        nameItem->setText(name);
        nameItem->setToolTip(path);

        /*
         * Should file (row) be hidden?
         */
        bool isFileHidden = false;
        if (filteredDataFileType != DataFileTypeEnum::UNKNOWN) {
            if (dataFileType != filteredDataFileType) {
                isFileHidden = true;
            }
        }
        if (filteredStructureType != StructureEnum::ALL) {
            switch (filteredStructureType) {
                case StructureEnum::CORTEX_LEFT:
                    if (structure != StructureEnum::CORTEX_LEFT) {
                        isFileHidden = true;
                    }
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    if (structure != StructureEnum::CORTEX_RIGHT) {
                        isFileHidden = true;
                    }
                    break;
                case StructureEnum::CEREBELLUM:
                case StructureEnum::CEREBELLUM_LEFT:
                case StructureEnum::CEREBELLUM_RIGHT:
                    if (structure != StructureEnum::CEREBELLUM) {
                        isFileHidden = true;
                    }
                    break;
                case StructureEnum::OTHER:
                {
                    switch (structure) {
                        case StructureEnum::CORTEX_LEFT:
                        case StructureEnum::CORTEX_RIGHT:
                        case StructureEnum::CEREBELLUM:
                        case StructureEnum::CEREBELLUM_LEFT:
                        case StructureEnum::CEREBELLUM_RIGHT:
                            isFileHidden = true;
                            break;
                        default:
                            break;
                    }
                }
                    break;
                default:
                    break;
            }
        }
        switch (manageFilesDisplay) {
            case MANAGE_FILES_ALL:
                break;
            case MANAGE_FILES_LOADED:
                if (caretDataFile == NULL) {
                    isFileHidden = true;
                }
                break;
            case MANAGE_FILES_NOT_LOADED:
                if (caretDataFile != NULL) {
                    isFileHidden = true;
                }
                break;
        }
        
        m_filesTableWidget->setRowHidden(rowIndex, isFileHidden);
    }
    
    /*
     * Enable cell changed signal now that table has been updated.
     */
    m_filesTableWidget->blockSignals(false);
    
    /*
     * Fix table geometry.
     */
    m_filesTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_filesTableWidget->resizeColumnsToContents();
    m_filesTableWidget->resizeRowsToContents();    
}

/**
 * @return Create and return a text item for the table.
 */
QTableWidgetItem*
SpecFileManagementDialog::createTextItem()
{
    QTableWidgetItem* item = new QTableWidgetItem();
    Qt::ItemFlags flags(Qt::ItemIsEnabled);
    item->setFlags(flags);
    
    return item;
}

/**
 * @return Create and return a checkable item for the table.
 */
QTableWidgetItem*
SpecFileManagementDialog::createCheckableItem()
{
    QTableWidgetItem* item = new QTableWidgetItem();
    Qt::ItemFlags flags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    item->setFlags(flags);
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
        m_specFile->setAllSceneFilesSelectedForLoadingAndAllOtherFilesNotSelected();
        
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
    
//    const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
//    for (int32_t iGroup = 0; iGroup < numGuiGroups; iGroup++) {
//        GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[iGroup];
//        const int32_t numFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
//        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
//            GuiSpecFileDataFile* guiDataFile = guiSpecGroup->getGuiSpecFileDataFile(iFile);
//            if (guiDataFile->m_saveCheckedItem->flags() & Qt::ItemIsEnabled) {
//                if (guiDataFile->m_saveCheckedItem->checkState() == Qt::Checked) {
//                    CaretDataFile* caretDataFile = guiDataFile->m_specFileDataFile->getCaretDataFile();
//                    if (caretDataFile != NULL) {
//                        try {
//                            m_brain->writeDataFile(caretDataFile,
//                                                   false);
//                            guiDataFile->m_saveCheckedItem->setCheckState(Qt::Unchecked);
//                        }
//                        catch (const DataFileException& e) {
//                            errorMessages.appendWithNewLine(e.whatString());
//                        }
//                        guiDataFile->updateContent();
//                    }
//                }
//            }
//        }
//    }
    
    CaretAssert(m_COLUMN_SAVE_CHECKBOX >= 0);
    QTableWidgetItem* saveItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                    m_COLUMN_SAVE_CHECKBOX);
    if (saveItem->checkState() == Qt::Checked) {
        AString specFileName = m_specFile->getFileName();
        if (m_specFile->getFileName().isEmpty()) {
            errorMessages.appendWithNewLine("Spec File name is empty.");
        }
        else {
            AString specFileErrorMessage = writeSpecFile(false);
            if (specFileErrorMessage.isEmpty() == false) {
                errorMessages.appendWithNewLine(specFileErrorMessage);
            }
            saveItem->setCheckState(Qt::Unchecked);
        }
    }
    
    loadSpecFileContentIntoDialog();
    
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
 * Called when a file remove button is clicked.
 *
 * @param rowIndex
 *    Row index of the item selected.
 */
void
SpecFileManagementDialog::fileRemoveActionSelected(int rowIndex)
{
    TableRowDataFileContent* rowContent = getFileContentInRow(rowIndex);
    SpecFileDataFile* specFileDataFile = rowContent->m_specFileDataFile;
    
    QWidget* removeButtonWidget = m_filesTableWidget->cellWidget(rowIndex,
                                                               m_COLUMN_REMOVE_BUTTON);
    CaretAssert(removeButtonWidget);
    CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
    if (caretDataFile->isModified()) {
        const QString msg = (caretDataFile->getFileNameNoPath()
                             + " is modified.  Remove without saving changes?");
        if (WuQMessageBox::warningOkCancel(removeButtonWidget, msg) == false) {
            return;
        }
    }
    GuiManager::get()->getBrain()->removeDataFile(caretDataFile);
    loadSpecFileContentIntoDialog();
    updateGraphicWindowsAndUserInterface();
}

/**
 * Get the content for the given row.
 *
 * @param rowIndex
 *    Index of the row.
 * @return 
 *    Content for the given row.
 */
TableRowDataFileContent*
SpecFileManagementDialog::getFileContentInRow(const int rowIndex)
{
    const int numDataFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numDataFiles; i++) {
        if (m_tableRowDataFileContent[i]->m_tableRowIndex == rowIndex) {
            return m_tableRowDataFileContent[i];
        }
    }

    CaretAssertMessage(0,
                       ("Invalid data file rowIndex (0 is spec file!!!): "
                        + AString::number(rowIndex)));
    return NULL;
}


/**
 * Called when a file reload or open button is clicked.
 *
 * @param rowIndex
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileReloadOrOpenFileActionSelected(int rowIndex)
{
    TableRowDataFileContent* rowContent = getFileContentInRow(rowIndex);
    SpecFileDataFile* specFileDataFile = rowContent->m_specFileDataFile;
    
    QWidget* toolButtonWidget = m_filesTableWidget->cellWidget(rowIndex,
                                                               m_COLUMN_READ_BUTTON);
    CaretAssert(toolButtonWidget);
    
    AString errorMessage;
    
    CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
    if (caretDataFile != NULL) {
        if (caretDataFile->isModified()) {
            const QString msg = (caretDataFile->getFileNameNoPath()
                                 + " is modified.  Reopen without saving changes?");
            if (WuQMessageBox::warningOkCancel(toolButtonWidget, msg) == false) {
                return;
            }
        }
        
        specFileDataFile->setSavingSelected(false);
        
        EventDataFileReload reloadEvent(m_brain,
                                        caretDataFile);
        EventManager::get()->sendEvent(reloadEvent.getPointer());
        
        if (reloadEvent.isError()) {
            errorMessage.appendWithNewLine(reloadEvent.getErrorMessage());
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
            errorMessage.appendWithNewLine(readEvent.getErrorMessage());
        }
        
        if (readEvent.getAddToSpecFileErrorMessages().isEmpty() == false) {
            errorMessage.appendWithNewLine(readEvent.getAddToSpecFileErrorMessages());
        }
    }
    
    updateGraphicWindowsAndUserInterface();

    loadSpecFileContentIntoDialog();
    
    if (errorMessage.isEmpty() == false) {
        WuQMessageBox::errorOk(toolButtonWidget,
                               errorMessage);
    }
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
 * @param rowIndex
 *    Index of the SpecFileDataFile item.
 */
void
SpecFileManagementDialog::fileOptionsActionSelected(int rowIndex)
{
    if (rowIndex == m_specFileTableRowIndex) {
        QMenu menu;
        QAction* editMetaDataAction = menu.addAction("Edit Metadata...");;
        QAction* setFileNameAction = menu.addAction("Set File Name...");

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
                           NULL,
                           m_specFile);
        }
        else if (selectedAction == editMetaDataAction) {
            MetaDataEditorDialog mded(m_specFile,
                                      &menu);
            mded.exec();
        }
        else if (selectedAction != NULL) {
            CaretAssertMessage(0,
                               ("Unhandled Menu Action: " + selectedAction->text()));
        }
    }
    else {
        TableRowDataFileContent* rowContent = getFileContentInRow(rowIndex);
        SpecFileDataFile* specFileDataFile = rowContent->m_specFileDataFile;
        CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
        CaretMappableDataFile* caretMappableDataFile = NULL;
        if (caretDataFile != NULL) {
            caretMappableDataFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile);
        }
        
        QAction* editMetaDataAction = NULL;
        QAction* setFileNameAction = NULL;
        QAction* setStructureAction = NULL;
        //QAction* unloadFileAction = NULL;
        QAction* unloadFileMapsAction = NULL;
        QAction* viewMetaDataAction = NULL;
        
        QMenu menu;
        switch (m_dialogMode) {
            case MODE_MANAGE_FILES:
                if (caretDataFile != NULL) {
                    editMetaDataAction = menu.addAction("Edit Metadata...");
                    setFileNameAction = menu.addAction("Set File Name...");
                    //                unloadFileAction = menu.addAction("Unload File");
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
                setStructureAction = menu.addAction("Set Structure...");
                setStructureAction->setEnabled(false);
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
                           specFileDataFile,
                           caretDataFile);
        }
        else if (selectedAction == setStructureAction) {
            CaretAssert(0);
        }
        //    else if (selectedAction == unloadFileAction) {
        //        CaretDataFile* cdf = specFileDataFile->getCaretDataFile();
        //        GuiManager::get()->getBrain()->removeDataFile(cdf);
        //        loadSpecFileContentIntoDialog();
        //        updateGraphicWindowsAndUserInterface();
        //    }
        else if (selectedAction == unloadFileMapsAction) {
            
        }
        else if (selectedAction == editMetaDataAction) {
            if (caretDataFile != NULL) {
                MetaDataEditorDialog mded(caretDataFile,
                                          &menu);
                mded.exec();
            }
        }
        else if (selectedAction == viewMetaDataAction) {
            
        }
        else if (selectedAction != NULL) {
            CaretAssertMessage(0,
                               ("Unhandled Menu Action: " + selectedAction->text()));
        }
    }
}

/**
 * Change the name of a file.
 *
 * @param parent
 *   Widget on which file dialog is displayed.
 * @param specFileDataFile
 *   Entry in spec file (NULL if caretDataFile is spec file).
 * @param caretDataFileIn
 *   Caret Data File that is having name changed.
 */
void
SpecFileManagementDialog::changeFileName(QWidget* parent,
                                         SpecFileDataFile* specFileDataFile,
                                         CaretDataFile* caretDataFileIn)
{
    CaretDataFile* caretDataFile = caretDataFileIn;
    CaretAssert(caretDataFile);
    if (caretDataFile != m_specFile) {
        CaretAssert(m_specFile);
    }
    
    QStringList filenameFilterList;
    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(caretDataFile->getDataFileType()));
    CaretFileDialog fd(parent);
    fd.setAcceptMode(CaretFileDialog::AcceptSave);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.selectFile(caretDataFile->getFileName());
    fd.setLabelText(CaretFileDialog::Accept, "Choose");
    fd.setWindowTitle("Choose File Name");
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList files = fd.selectedFiles();
        if (files.isEmpty() == false) {
            AString newFileName = files.at(0);
            if (newFileName != caretDataFile->getFileName()) {
                /*
                 * Clone current item, remove file from it,
                 * and create new item.
                 */
                if (specFileDataFile != NULL) {
                    SpecFileDataFile* sfdf = m_specFile->changeFileName(specFileDataFile,
                                                                        newFileName);
                    caretDataFile = sfdf->getCaretDataFile();
                }
                CaretAssert(caretDataFile);
                caretDataFile->setFileName(newFileName);

                /*
                 * Spec file has changed
                 */
                getDataFileContentFromSpecFile();
                
                /*
                 * Table may need to add/remove rows
                 */
                updateTableDimensionsToFitFiles();
                
                /*
                 * Update the table rows with data
                 */
                loadSpecFileContentIntoDialog();
            
                /*
                 * Files have changed
                 */
                updateGraphicWindowsAndUserInterface();
            }
        }
    }
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
                loadSpecFileContentIntoDialog();
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
    QWidget* toolButtonWidget = m_filesTableWidget->cellWidget(m_specFileTableRowIndex,
                                                               m_COLUMN_OPTIONS_TOOLBUTTON);
    CaretAssert(toolButtonWidget);
    
    QStringList filenameFilterList;
    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
    CaretFileDialog fd(toolButtonWidget);
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
            loadSpecFileContentIntoDialog();
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
    std::set<DataFileTypeEnum::Enum> loadedDataFileTypes;
    const int32_t numFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numFiles; i++) {
        SpecFileDataFile* sfdf = m_tableRowDataFileContent[i]->m_specFileDataFile;
        loadedDataFileTypes.insert(sfdf->getDataFileType());
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
        
        const AString text = getEditedDataFileTypeName(dataFileType);
        
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
    
    QToolBar* toolbar = createToolBarWithActionGroup("View Files:       ",
                                                     m_manageFilesLoadedNotLoadedActionGroup);
    return toolbar;
}

/**
 * @return Edit and return the text for a data file type.
 */
AString
SpecFileManagementDialog::getEditedDataFileTypeName(const DataFileTypeEnum::Enum dataFileType)
{
    const AString typeName = DataFileTypeEnum::toGuiName(dataFileType);
    
    const AString connectivityPrefix("Connectivity - ");
    const int connectivityPrefixLength = connectivityPrefix.length();
    
    const AString temporarySuffix(" TEMPORARY");
    const int temporarySuffixLength = temporarySuffix.length();
    
    AString text = typeName;
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
    }
    
    loadSpecFileContentIntoDialog();
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
    }
    
    loadSpecFileContentIntoDialog();
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
    loadSpecFileContentIntoDialog();
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
//    m_filesTableWidget->blockSignals(true);

    if (action != NULL) {
        const int dataValue = action->data().toInt();
        
        bool newStatus = false;
        if (dataValue == SHOW_FILES_ALL) {
            newStatus = true;
        }
        else if (dataValue == SHOW_FILES_NONE) {
            newStatus = false;
        }
        
        const int32_t numFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
        for (int32_t i = 0; i < numFiles; i++) {
            const int32_t rowIndex = m_tableRowDataFileContent[i]->m_tableRowIndex;
            if (m_filesTableWidget->isRowHidden(rowIndex) == false) {
                if (m_COLUMN_LOAD_CHECKBOX >= 0) {
                    QTableWidgetItem* loadItem = getTableWidgetItem(rowIndex,
                                                                          m_COLUMN_LOAD_CHECKBOX);
                    CaretAssert(loadItem);
                    loadItem->setCheckState(WuQtUtilities::boolToCheckState(newStatus));
                }
                
                if (m_COLUMN_SAVE_CHECKBOX >= 0) {
                    QTableWidgetItem* saveItem = getTableWidgetItem(rowIndex,
                                                                      m_COLUMN_SAVE_CHECKBOX);
                    CaretAssert(saveItem);
                    saveItem->setCheckState(WuQtUtilities::boolToCheckState(newStatus));
                }
            }
        }
    }

//    m_filesTableWidget->blockSignals(false);
}

