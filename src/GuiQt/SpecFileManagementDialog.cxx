
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

#define __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__
#include "SpecFileManagementDialog.h"
#undef __SPEC_FILE_MANAGEMENT_DIALOG_DECLARE__

#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QCheckBox>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSignalMapper>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationFile.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretFileDialog.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretPreferences.h"
#include "CursorDisplayScoped.h"
#include "DataFileException.h"
#include "EventDataFileRead.h"
#include "EventDataFileReload.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "MetaDataEditorDialog.h"
#include "ProgressReportingDialog.h"
#include "SessionManager.h"
#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "UsernamePasswordWidget.h"
#include "WuQImageLabel.h"
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
 * Run a dialog for opening a spec file.
 *
 * @param brain
 *    Brain into which spec file is read.
 * @param specFile
 *    Spec File.
 * @param parent
 *    Parent of dialog.
 * @return
 *    True if user loaded the spec file, else false.
 */
bool
SpecFileManagementDialog::runOpenSpecFileDialog(Brain* brain,
                                                SpecFile* specFile,
                                                QWidget* parent)
{
    CaretAssert(brain);
    CaretAssert(specFile);
    const AString title = ("Open Spec File: "
                           + specFile->getFileNameNoPath());
    SpecFileManagementDialog dialog(MODE_OPEN_SPEC_FILE,
                                    brain,
                                    specFile,
                                    title,
                                    parent);
    if (dialog.exec() == SpecFileManagementDialog::Accepted) {
        return true;
    }
    
    return false;
}

/**
 * Run a dialog for managing files in a brain.
 *
 * DO NOT delete the returned dialog as it will delete itself when closed.
 *
 * @param brain
 *    Brain for which files are managed.
 * @param parent
 *    Parent of dialog.
 */
void
SpecFileManagementDialog::runManageFilesDialog(Brain* brain,
                                               QWidget* parent)
{
    CaretAssert(brain);
    const AString title = ("Manage Data Files");
    
    SpecFileManagementDialog dialog(MODE_MANAGE_FILES,
                                    brain,
                                    brain->getSpecFile(),
                                    title,
                                    parent);
    
    /*
     * Override view files type the first time the dialog is displayed
     * using the value from the user's preferences
     */
    static bool firstTime = true;
    if (firstTime) {
        const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        s_manageFilesViewFilesType = prefs->getManageFilesViewFileType();
        firstTime = false;
    }
    
    dialog.setFilterSelections(s_manageFilesViewFilesType,
                               s_manageFilesFilteredDataFileType,
                               s_manageFilesFilteredStructureType);
    
    if ( ! s_manageFilesGeometry.isEmpty()) {
        dialog.restoreGeometry(s_manageFilesGeometry);
    }
    dialog.loadSpecFileContentIntoDialog();

    dialog.exec();
    
    dialog.getFilterSelections(s_manageFilesViewFilesType,
                               s_manageFilesFilteredDataFileType,
                               s_manageFilesFilteredStructureType);
    
    s_manageFilesGeometry = dialog.saveGeometry();
}

/**
 * Run a dialog for saving files in a brain while exiting workbench.
 *
 * DO NOT delete the returned dialog as it will delete itself when closed.
 *
 * @param brain
 *    Brain for which files are managed.
 * @param parent
 *    Parent of dialog.
 * @return 
 *    true if workbench is allowed to exit, else false.
 */
bool
SpecFileManagementDialog::runSaveFilesDialogWhileQuittingWorkbench(Brain* brain,
                                               QWidget* parent)
{
    CaretAssert(brain);
    const AString title = ("Save Data Files");
    
    SpecFileManagementDialog dialog(MODE_SAVE_FILES_WHILE_QUITTING,
                                    brain,
                                    brain->getSpecFile(),
                                    title,
                                    parent);
    
    if (dialog.exec()) {
        return true;
    }
    return false;
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
    /*
     * Initialize members
     */
    m_filesTableWidget = NULL;
    m_fileSelectionActionGroup = NULL;
    m_manageFilesLoadedNotLoadedActionGroup = NULL;
    m_specFileDataFileCounter = 0;
    m_specFileTableRowIndex = -1;
    m_sceneAnnotationFileRowIndex = -1;
    m_fileSorting = SpecFileManagementDialogRowContent::SORTING_TYPE_STRUCTURE_NAME;

    /*
     * Load icons.
     */
    m_iconOpenFile   = WuQtUtilities::loadIcon(":/SpecFileDialog/load_icon.png");
    m_iconOptions    = WuQtUtilities::loadIcon(":/SpecFileDialog/options_icon.png");
    m_iconReloadFile = WuQtUtilities::loadIcon(":/SpecFileDialog/reload_icon.png");
    m_iconCloseFile = WuQtUtilities::loadIcon(":/SpecFileDialog/delete_icon.png");
    
    /*
     * Open Spec File or Manage Files?
     */
    bool enableManageItems = false;
    bool enableOpenItems   = false;
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
        case SpecFileManagementDialog::MODE_SAVE_FILES_WHILE_QUITTING:
            m_specFile->setModifiedFilesSelectedForSaving();
            enableManageItems = true;
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            enableOpenItems = true;
            break;
    }
    
    /*
     * Signal mappers for buttons
     */
    m_fileReloadOrOpenFileActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileReloadOrOpenFileActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileReloadOrOpenFileActionSelected(int)));
    
    m_fileOptionsActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileOptionsActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileOptionsActionSelected(int)));
    
    m_fileCloseFileActionSignalMapper = new QSignalMapper(this);
    QObject::connect(m_fileCloseFileActionSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(fileRemoveActionSelected(int)));

    int tableRowCounter = 0;
    
    /*
     * Is there a spec file?  If so, set its table row index
     */
    if (enableManageItems) {
        m_specFileTableRowIndex = tableRowCounter++;
        m_sceneAnnotationFileRowIndex = tableRowCounter++;
    }
    
    bool testForDisplayedDataFiles = false;
    m_loadScenesPushButton = NULL;
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
            setOkButtonText("Save Checked Files");
            setCancelButtonText("Close");
            testForDisplayedDataFiles = true;
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            setOkButtonText("Load");
            setCancelButtonText("Cancel");
            m_loadScenesPushButton = addUserPushButton("Load Scenes",
                                                       QDialogButtonBox::AcceptRole);
            break;
        case SpecFileManagementDialog::MODE_SAVE_FILES_WHILE_QUITTING:
            setOkButtonText("Save Selected Files and Exit");
            setCancelButtonText("Cancel");
            testForDisplayedDataFiles = true;
            break;
    }
    
    if (testForDisplayedDataFiles) {
        EventGetDisplayedDataFiles displayedFilesEvent;
        EventManager::get()->sendEvent(displayedFilesEvent.getPointer());
        m_displayedDataFiles = displayedFilesEvent.getDisplayedDataFiles();
    }
    

    /*
     * Set column indices for table's members
     */
    int columnCounter = 0;
    m_COLUMN_LOAD_CHECKBOX               = -1;
    m_COLUMN_SAVE_CHECKBOX               = -1;
    m_COLUMN_STATUS_LABEL                = -1;
    m_COLUMN_DISPLAYED_LABEL             = -1;
    m_COLUMN_IN_SPEC_FILE_CHECKBOX       = -1;
    m_COLUMN_READ_BUTTON                 = -1;
    m_COLUMN_CLOSE_BUTTON                = -1;
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
        m_COLUMN_DISPLAYED_LABEL = columnCounter++;
        m_COLUMN_IN_SPEC_FILE_CHECKBOX = columnCounter++;
        m_COLUMN_READ_BUTTON = columnCounter++;
        m_COLUMN_CLOSE_BUTTON = columnCounter++;
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
    m_filesTableWidget->setAlternatingRowColors(true);
    m_filesTableWidget->setSelectionBehavior(QTableWidget::SelectItems);
    m_filesTableWidget->setSelectionMode(QTableWidget::SingleSelection);
    QObject::connect(m_filesTableWidget, SIGNAL(cellChanged(int,int)),
                     this, SLOT(filesTableWidgetCellChanged(int,int)));
    QObject::connect(m_filesTableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)),
                     this, SLOT(horizontalHeaderSelectedForSorting(int)));
    const QString headerToolTip = ("Click on the column names (of those columns that contain text) to sort.");
    m_filesTableWidget->horizontalHeader()->setToolTip(WuQtUtilities::createWordWrappedToolTipText(headerToolTip));
    
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
    WuQtUtilities::setLayoutSpacingAndMargins(toolbarLayout, 0, 0);
    
    QLabel* fileTypesToolBarLabel = NULL;
    toolbarLayout->addWidget(createFilesTypesToolBar(fileTypesToolBarLabel));
    
    QLabel* fileStructureToolBarLabel = NULL;
    toolbarLayout->addWidget(createStructureToolBar(fileStructureToolBarLabel));
    
    QLabel* fileSelectionToolBarLabel = NULL;
    QLabel* fileLoadedNotLoadedToolBarLabel = NULL;
    if (enableOpenItems) {
        toolbarLayout->addWidget(createFilesSelectionToolBar(fileSelectionToolBarLabel));
    }
    else if (enableManageItems) {
        toolbarLayout->addWidget(createManageFilesLoadedNotLoadedToolBar(fileLoadedNotLoadedToolBarLabel));
    }
    toolbarWidget->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Fixed);

    /*
     * Make all of the toolbar labels have the same width to that
     * they line up nicely.  Note that the match widgets widgets 
     * method will ignore NULL values.
     */
    WuQtUtilities::matchWidgetWidths(fileTypesToolBarLabel,
                                     fileStructureToolBarLabel,
                                     fileSelectionToolBarLabel,
                                     fileLoadedNotLoadedToolBarLabel);
    /*
     * No scrollbars in dialog since the table widget will have scroll bars
     */
//    const bool enableScrollBars = false;

    setTopBottomAndCentralWidgets(toolbarWidget,
                                  centralWidget,
                                  NULL,
                                  WuQDialog::SCROLL_AREA_NEVER);
    
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
    clearSpecFileManagementDialogRowContent();
    
    if (m_iconOpenFile != NULL) {
        delete m_iconOpenFile;
    }
    if (m_iconOptions != NULL) {
        delete m_iconOptions;
    }
    if (m_iconReloadFile != NULL) {
        delete m_iconReloadFile;
    }
    if (m_iconCloseFile != NULL) {
        delete m_iconCloseFile;
    }
}

/**
 * Clear content of all of the table row.
 */
void
SpecFileManagementDialog::clearSpecFileManagementDialogRowContent()
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
    clearSpecFileManagementDialogRowContent();
    
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
            
            /*
             * If the spec file entry is not a member of the spec file,
             * AND if it does not match a loaded file
             * AND the file does not exist
             * THEN the file likely was a duplicate (more than one copy
             * of a specific file was loaded with the same name) and 
             * the user had removed it from Workbench.
             *
             * So, hide the file from view.
             */
            if ( ! sfdf->isSpecFileMember()) {
                if (sfdf->getCaretDataFile() == NULL) {
                    if (! sfdf->exists()) {
                        continue;
                    }
                }
            }
            SpecFileManagementDialogRowContent* rowContent = new SpecFileManagementDialogRowContent(group,
                                                                              sfdf);
            m_tableRowDataFileContent.push_back(rowContent);
            m_specFileDataFileCounter++;
        }
        
        if (dataFileType == DataFileTypeEnum::SCENE) {
            if (numFiles > 0) {
                haveSceneFiles = true;
            }
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
            /*
             * Nothing to do for spec file
             */
        }
        else if (rowIndex == m_sceneAnnotationFileRowIndex) {
            /* Nothing to do for annotation scene file */
        }
        else {
            SpecFileManagementDialogRowContent* rowContent = getFileContentInRow(rowIndex);
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
                
                /*
                 * Check the Spec File SAVE checkbox since the user has changed the
                 * "in spec" status for a data file
                 */
                if (m_specFileTableRowIndex >= 0) {
                    QTableWidgetItem* saveItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                                    m_COLUMN_SAVE_CHECKBOX);
                    saveItem->setCheckState(Qt::Checked);
                }
            }
        }
    }
    
    enableLoadOrSaveButton();
}

void
SpecFileManagementDialog::enableLoadOrSaveButton()
{
    bool isAnyFileSelected = false;
    
    if (m_specFile != NULL) {
        switch (m_dialogMode) {
            case MODE_MANAGE_FILES:
            case MODE_SAVE_FILES_WHILE_QUITTING:
            {
                if (m_specFile->getNumberOfFilesSelectedForSaving() > 0) {
                    isAnyFileSelected = true;
                }
                QTableWidgetItem* specCheckItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                                     m_COLUMN_SAVE_CHECKBOX);
                if (WuQtUtilities::checkStateToBool(specCheckItem->checkState())) {
                    isAnyFileSelected = true;
                }
            }
                break;
            case MODE_OPEN_SPEC_FILE:
                if (m_specFile->getNumberOfFilesSelectedForLoading() > 0) {
                    isAnyFileSelected = true;
                }
                break;
        }
    }
    
    QAbstractButton* okButton = getDialogButtonBox()->button(QDialogButtonBox::Ok);
    CaretAssert(okButton);
    okButton->setEnabled(isAnyFileSelected);
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
    if (m_COLUMN_DISPLAYED_LABEL >= 0) {
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_DISPLAYED_LABEL,
                                                    createHeaderTextItem("Displayed"));
    }
    if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_IN_SPEC_FILE_CHECKBOX,
                                                    createHeaderTextItem("In Spec"));
    }
    if (m_COLUMN_READ_BUTTON >= 0){
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_READ_BUTTON,
                                                    createHeaderTextItem("Read"));
    }
    if (m_COLUMN_CLOSE_BUTTON >= 0) {
        m_filesTableWidget->setHorizontalHeaderItem(m_COLUMN_CLOSE_BUTTON,
                                                    createHeaderTextItem("Close"));
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
    m_sceneAnnotationFileRowIndex = -1;
    int numberOfRows = 0;
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
        case MODE_SAVE_FILES_WHILE_QUITTING:
            m_specFileTableRowIndex = numberOfRows;
            numberOfRows++;
            if ( ! m_brain->getSceneAnnotationFile()->isEmpty()) {
                m_sceneAnnotationFileRowIndex = numberOfRows;
                numberOfRows++;
            }
            break;
        case MODE_OPEN_SPEC_FILE:
            break;
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
        bool hasCloseCheckBoxFlag  = true;
        bool hasDisplayedLabelFlag = true;
        bool hasInSpecCheckBoxFlag = true;
        bool hasLoadCheckBoxFlag   = true;
        bool hasOptionsButtonFlag  = true;
        bool hasReadCheckBoxFlag   = true;
        bool hasSaveCheckBoxFlag   = true;
        bool hasStatusLabelFlag    = true;
        if (iRow == m_specFileTableRowIndex) {
            hasCloseCheckBoxFlag  = false;
            hasDisplayedLabelFlag = false;
            hasInSpecCheckBoxFlag = false;
            hasLoadCheckBoxFlag   = false;
            hasReadCheckBoxFlag   = false;
        }
        else if (iRow == m_sceneAnnotationFileRowIndex) {
            hasDisplayedLabelFlag = false;
            hasInSpecCheckBoxFlag = false;
            hasLoadCheckBoxFlag   = false;
            hasOptionsButtonFlag  = false;
            hasReadCheckBoxFlag   = false;
            hasSaveCheckBoxFlag   = false;
        }
        
        if (hasLoadCheckBoxFlag) {
            if (m_COLUMN_LOAD_CHECKBOX >= 0) {
                QTableWidgetItem* item = createCheckableItem();
                item->setTextAlignment(Qt::AlignHCenter);
                setTableWidgetItem(iRow,
                                   m_COLUMN_LOAD_CHECKBOX,
                                   item);
            }
        }

        if (hasSaveCheckBoxFlag) {
            if (m_COLUMN_SAVE_CHECKBOX >= 0) {
                QTableWidgetItem* item = createCheckableItem();
                item->setTextAlignment(Qt::AlignHCenter);
                setTableWidgetItem(iRow,
                                   m_COLUMN_SAVE_CHECKBOX,
                                   item);
            }
        }
        
        if (hasStatusLabelFlag) {
            if (m_COLUMN_STATUS_LABEL >= 0) {
                QTableWidgetItem* item = createTextItem();
                setTableWidgetItem(iRow,
                                   m_COLUMN_STATUS_LABEL,
                                   item);
                
                /*
                 * Text for modified cell is always red and centered
                 */
                item->setTextAlignment(Qt::AlignCenter);
                QBrush modifiedBrush = item->foreground();
                modifiedBrush.setColor(Qt::red);
                item->setForeground(modifiedBrush);
            }
        }
        
        if (hasDisplayedLabelFlag) {
            if (m_COLUMN_DISPLAYED_LABEL >= 0) {
                QTableWidgetItem* item = createTextItem();
                setTableWidgetItem(iRow,
                                   m_COLUMN_DISPLAYED_LABEL,
                                   item);
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
        
        if (hasInSpecCheckBoxFlag) {
            if (m_COLUMN_IN_SPEC_FILE_CHECKBOX >= 0) {
                QTableWidgetItem* item = createCheckableItem();
                item->setTextAlignment(Qt::AlignHCenter);
                setTableWidgetItem(iRow,
                                   m_COLUMN_IN_SPEC_FILE_CHECKBOX,
                                   item);
            }
        }
        if (hasReadCheckBoxFlag) {
            if (m_COLUMN_READ_BUTTON >= 0) {
                WuQImageLabel* loadImageLabel = new WuQImageLabel(m_iconReloadFile,
                                                                  "Reload");
                QObject::connect(loadImageLabel, SIGNAL(clicked()),
                                 m_fileReloadOrOpenFileActionSignalMapper, SLOT(map()));
                m_fileReloadOrOpenFileActionSignalMapper->setMapping(loadImageLabel, iRow);
                m_filesTableWidget->setCellWidget(iRow,
                                                  m_COLUMN_READ_BUTTON,
                                                  loadImageLabel);
            }
        }
        
        if (hasCloseCheckBoxFlag) {
            if (m_COLUMN_CLOSE_BUTTON >= 0) {
                WuQImageLabel* closeImageLabel = new WuQImageLabel(m_iconCloseFile,
                                                                   "Close");
                QObject::connect(closeImageLabel, SIGNAL(clicked()),
                                 m_fileCloseFileActionSignalMapper, SLOT(map()));
                m_fileCloseFileActionSignalMapper->setMapping(closeImageLabel, iRow);
                m_filesTableWidget->setCellWidget(iRow,
                                                  m_COLUMN_CLOSE_BUTTON,
                                                  closeImageLabel);
            }
        }
    
        if (hasOptionsButtonFlag) {
            if (m_COLUMN_OPTIONS_TOOLBUTTON >= 0) {
                WuQImageLabel* optionsImageLabel = new WuQImageLabel(m_iconOptions,
                                                                     "Options");
                QObject::connect(optionsImageLabel, SIGNAL(clicked()),
                                 m_fileOptionsActionSignalMapper, SLOT(map()));
                m_fileOptionsActionSignalMapper->setMapping(optionsImageLabel, iRow);
                m_filesTableWidget->setCellWidget(iRow,
                                                  m_COLUMN_OPTIONS_TOOLBUTTON,
                                                  optionsImageLabel);
            }
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
    bool isUpdateRow = false;
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
        case MODE_SAVE_FILES_WHILE_QUITTING:
            isUpdateRow = true;
            break;
        case MODE_OPEN_SPEC_FILE:
            break;
    }
    
    /*
     * Update spec file data
     */
    if (isUpdateRow) {
        if (m_specFileTableRowIndex >= 0) {
            CaretAssert(m_COLUMN_DATA_FILE_TYPE_LABEL >= 0);
            QTableWidgetItem* dataTypeItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                            m_COLUMN_DATA_FILE_TYPE_LABEL);
            dataTypeItem->setText(getEditedDataFileTypeName(DataFileTypeEnum::SPECIFICATION));
            
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

            /*
             * Get filtering selections
             */
            SpecFileDialogViewFilesTypeEnum::Enum viewFilesType;
            DataFileTypeEnum::Enum filteredDataFileType;
            StructureEnum::Enum filteredStructureType;
            getFilterSelections(viewFilesType,
                                filteredDataFileType,
                                filteredStructureType);
            
            bool hideSpecFileRow = false;
            
            if ((filteredStructureType != StructureEnum::ALL)
                && (filteredStructureType != StructureEnum::OTHER)) {
                hideSpecFileRow = true;
            }
            
            switch (viewFilesType) {
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_ALL:
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED:
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED_MODIFIED:
                    if (! m_specFile->isModified()) {
                        hideSpecFileRow = true;
                    }
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED_NOT_MODIFIED:
                    if (m_specFile->isModified()) {
                        hideSpecFileRow = true;
                    }
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_NOT_LOADED:
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
 * Update the table row containing the annotation scene.
 */
void
SpecFileManagementDialog::updateAnnotationSceneFileRowInTable()
{
    bool isUpdateRow = false;
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
        case MODE_SAVE_FILES_WHILE_QUITTING:
            isUpdateRow = true;
            break;
        case MODE_OPEN_SPEC_FILE:
            break;
    }
    
    /*
     * Update spec file data
     */
    if (isUpdateRow) {
        if (m_sceneAnnotationFileRowIndex >= 0) {
            const AnnotationFile* sceneAnnotationFile = m_brain->getSceneAnnotationFile();
            
            CaretAssert(m_COLUMN_DATA_FILE_TYPE_LABEL >= 0);
            QTableWidgetItem* dataTypeItem = getTableWidgetItem(m_sceneAnnotationFileRowIndex,
                                                                m_COLUMN_DATA_FILE_TYPE_LABEL);
            dataTypeItem->setText("Scene Annotations");
            
            CaretAssert(m_COLUMN_FILE_NAME_LABEL >= 0);
            QTableWidgetItem* nameItem = getTableWidgetItem(m_sceneAnnotationFileRowIndex,
                                                            m_COLUMN_FILE_NAME_LABEL);
            CaretAssert(nameItem);
            nameItem->setText("");
            nameItem->setToolTip("");
            
            CaretAssert(m_COLUMN_STATUS_LABEL >= 0);
            QTableWidgetItem* statusItem = getTableWidgetItem(m_sceneAnnotationFileRowIndex,
                                                              m_COLUMN_STATUS_LABEL);
            
            /*
             * Scene annotation file is considered modified if 
             * it is NOT empty.
             */
            const bool modifiedStatusFlag = sceneAnnotationFile->isModified();
            CaretAssert(statusItem);
            if (modifiedStatusFlag) {
                statusItem->setText("YES");
            }
            else {
                statusItem->setText("");
            }
            
            /*
             * Get filtering selections
             */
            SpecFileDialogViewFilesTypeEnum::Enum viewFilesType;
            DataFileTypeEnum::Enum filteredDataFileType;
            StructureEnum::Enum filteredStructureType;
            getFilterSelections(viewFilesType,
                                filteredDataFileType,
                                filteredStructureType);
            
            bool hideSceneAnnontationFileRow = false;
            
            if ((filteredStructureType != StructureEnum::ALL)
                && (filteredStructureType != StructureEnum::OTHER)) {
                hideSceneAnnontationFileRow = true;
            }
            
            switch (viewFilesType) {
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_ALL:
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED:
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED_MODIFIED:
                    if (! modifiedStatusFlag) {
                        hideSceneAnnontationFileRow = true;
                    }
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED_NOT_MODIFIED:
                    if (modifiedStatusFlag) {
                        hideSceneAnnontationFileRow = true;
                    }
                    break;
                case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_NOT_LOADED:
                    hideSceneAnnontationFileRow = true;
                    break;
                    
            }
            
            if (filteredDataFileType != DataFileTypeEnum::UNKNOWN) {
                hideSceneAnnontationFileRow = true;
            }
            
            m_filesTableWidget->setRowHidden(m_sceneAnnotationFileRowIndex,
                                             hideSceneAnnontationFileRow);
        }
    }
}


/**
 * Gets called when a column header is selected.
 * @param logicalIndex
 *    Index of item that was selected.
 */
void
SpecFileManagementDialog::horizontalHeaderSelectedForSorting(int logicalIndex)
{
    if (logicalIndex == m_COLUMN_DATA_FILE_TYPE_LABEL) {
        m_fileSorting = SpecFileManagementDialogRowContent::SORTING_TYPE_STRUCTURE_NAME;
        loadSpecFileContentIntoDialog();
    }
    else if (logicalIndex == m_COLUMN_FILE_NAME_LABEL) {
        m_fileSorting = SpecFileManagementDialogRowContent::SORTING_NAME;
        loadSpecFileContentIntoDialog();
    }
    else if (logicalIndex == m_COLUMN_STRUCTURE) {
        m_fileSorting = SpecFileManagementDialogRowContent::SORTING_STRUCTURE_TYPE_NAME;
        loadSpecFileContentIntoDialog();
    }
}

/**
 * Get the file filtering selections
 *
 * @param viewFilesTypeOut
 *    view files types
 * @param filteredDataFileTypeOut
 *    Data file type
 * @param filteredStructureTypeOut
 *    Structure
 */
void
SpecFileManagementDialog::getFilterSelections(SpecFileDialogViewFilesTypeEnum::Enum& viewFilesTypeOut,
                                              DataFileTypeEnum::Enum& filteredDataFileTypeOut,
                                              StructureEnum::Enum& filteredStructureTypeOut) const
{
    /*
     * All/Loaded/Not-Loaded
     */
    viewFilesTypeOut = SpecFileDialogViewFilesTypeEnum::VIEW_FILES_ALL;
    if (m_manageFilesLoadedNotLoadedActionGroup != NULL) {
        const QAction* manageFileAction = m_manageFilesLoadedNotLoadedActionGroup->checkedAction();
        if (manageFileAction != NULL) {
            viewFilesTypeOut = SpecFileDialogViewFilesTypeEnum::fromIntegerCode(manageFileAction->data().toInt(),
                                                                                NULL);
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
 * Set the file filtering selections
 *
 * @param viewFilesType
 *    View files type
 * @param filteredDataFileType
 *    Data file type
 * @param filteredStructureType
 *    Structure
 */
void
SpecFileManagementDialog::setFilterSelections(const SpecFileDialogViewFilesTypeEnum::Enum viewFilesType,
                                              const DataFileTypeEnum::Enum filteredDataFileType,
                                              const StructureEnum::Enum filteredStructureType)
{
    const int32_t viewFilesTypeInt = SpecFileDialogViewFilesTypeEnum::toIntegerCode(viewFilesType);
    QList<QAction*> manageActions = m_manageFilesLoadedNotLoadedActionGroup->actions();
    QListIterator<QAction*> manageIterator(manageActions);
    while (manageIterator.hasNext()) {
        QAction* action = manageIterator.next();
        if (action->data().toInt() == viewFilesTypeInt) {
            action->setChecked(true);
            break;
        }
    }
    
    const int32_t dataFileTypeInt = DataFileTypeEnum::toIntegerCode(filteredDataFileType);
    QList<QAction*> fileTypeActions = m_fileTypesActionGroup->actions();
    QListIterator<QAction*> fileTypeIterator(fileTypeActions);
    while (fileTypeIterator.hasNext()) {
        QAction* action = fileTypeIterator.next();
        if (action->data() == dataFileTypeInt) {
            action->setChecked(true);
            break;
        }
    }
    
    const int32_t structureTypeInt = StructureEnum::toIntegerCode(filteredStructureType);
    QList<QAction*> structureTypeActions = m_structureActionGroup->actions();
    QListIterator<QAction*> structureTypeIterator(structureTypeActions);
    while (structureTypeIterator.hasNext()) {
        QAction* action = structureTypeIterator.next();
        if (action->data() == structureTypeInt) {
            action->setChecked(true);
            break;
        }
    }
}


///**
// * Less than method for sorting using the sorting key.
// *
// * @param item1
// *    Tested for less than the item2
// * @param item2
// *    Tested for greater than the item1
// * @return
// *    True if item1 is less than item2, else false.
// */
//bool
//lessThanForSorting(const SpecFileManagementDialogRowContent *item1,
//                                                       const SpecFileManagementDialogRowContent* item2)
//{
//    return (item1->m_sortingKey < item2->m_sortingKey);
//}


/**
 * Sort the file content.
 */
void
SpecFileManagementDialog::sortFileContent()
{
    /*
     * Update key used for sorting
     */
    const int32_t numDataFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numDataFiles; i++) {
        m_tableRowDataFileContent[i]->setSortingKey(m_fileSorting);
    }
    
    /*
     * Sort
     */
    std::sort(m_tableRowDataFileContent.begin(),
              m_tableRowDataFileContent.end(),
              SpecFileManagementDialogRowContent::lessThanForSorting);
    
    /*
     * Update row indices in table
     */
    int rowCounter = 0;
    if (m_specFileTableRowIndex >= 0) {
        rowCounter++;
    }
    if (m_sceneAnnotationFileRowIndex >= 0) {
        rowCounter++;
    }
    for (int32_t i = 0; i < numDataFiles; i++) {
        m_tableRowDataFileContent[i]->m_tableRowIndex= rowCounter;
        rowCounter++;
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
    
    /*
     * Sort the rows
     */
    sortFileContent();
    
    SpecFileDialogViewFilesTypeEnum::Enum filteredViewFilesType;
    DataFileTypeEnum::Enum filteredDataFileType;
    StructureEnum::Enum filteredStructureType;
    getFilterSelections(filteredViewFilesType,
                        filteredDataFileType,
                        filteredStructureType);
    
    updateSpecFileRowInTable();
    updateAnnotationSceneFileRowInTable();
    
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
        
        
        bool isFileSavable = false;
        if (caretDataFile != NULL) {
            if (caretDataFile->supportsWriting()) {
                isFileSavable = true;
            }
        }
        const DataFileTypeEnum::Enum dataFileType = specFileDataFile->getDataFileType();
        
        switch (m_dialogMode) {
            case MODE_MANAGE_FILES:
            case MODE_SAVE_FILES_WHILE_QUITTING:
            {
                /*
                 * Save checkbox
                 */
                CaretAssert(m_COLUMN_SAVE_CHECKBOX >= 0);
                QTableWidgetItem* saveItem = getTableWidgetItem(rowIndex,
                                                                      m_COLUMN_SAVE_CHECKBOX);
                CaretAssert(saveItem);
                if (isFileSavable) {
                    saveItem->setFlags(saveItem->flags() | Qt::ItemIsEnabled);
                    saveItem->setCheckState(WuQtUtilities::boolToCheckState(specFileDataFile->isSavingSelected()));
                }
                else {
                    saveItem->setFlags(saveItem->flags() & (~ Qt::ItemIsEnabled));
                    saveItem->setCheckState(Qt::Unchecked);
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
                        if (isFileSavable) {
                            statusItem->setText("YES");
                            
                            /*
                             * Is this a Caret Mappable Data file and is the modification
                             * only in the palette color mapping?
                             */
                            CaretMappableDataFile* mapFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile);
                            if (mapFile != NULL) {
                                /*
                                 * Is modification just the palette color mapping?
                                 */
                                if (mapFile->isModifiedPaletteColorMapping()) {
                                    if ( ! mapFile->isModifiedExcludingPaletteColorMapping()) {
                                        statusItem->setText("PALETTE");
                                    }
                                }
                            }
                        }
                    }
                }
                
                /*
                 * Displayed label.
                 */
                CaretAssert(m_COLUMN_DISPLAYED_LABEL >= 0);
                QTableWidgetItem* displayedItem = getTableWidgetItem(rowIndex,
                                                                     m_COLUMN_DISPLAYED_LABEL);
                
                CaretAssert(displayedItem);
                displayedItem->setText("");
                if (caretDataFile != NULL) {
                    if (m_displayedDataFiles.find(caretDataFile) != m_displayedDataFiles.end()) {
                        displayedItem->setText("YES");
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
                QWidget* readCellWidget = m_filesTableWidget->cellWidget(rowIndex,
                                                                               m_COLUMN_READ_BUTTON);
                CaretAssert(readCellWidget);
                WuQImageLabel* readImageLabel = dynamic_cast<WuQImageLabel*>(readCellWidget);
                CaretAssert(readImageLabel);
                
                if (caretDataFile != NULL) {
                    readImageLabel->updateIconText(m_iconReloadFile,
                                                   "Reload");
                    readImageLabel->setToolTip("Reload this file");
                }
                else {
                    readImageLabel->updateIconText(m_iconOpenFile,
                                                   "Load");
                    readImageLabel->setToolTip("Load this file");
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
        
        AString path;
        AString name;
        const QString fileName(specFileDataFile->getFileName());
        if (fileName.isEmpty()) {
            CaretLogSevere("While loading spec file dialog, file of type "
                           + DataFileTypeEnum::toGuiName(dataFileType)
                           + " has empty file name.");
        }
        else {
            FileInformation fileInfo(specFileDataFile->getFileName());
            path = fileInfo.getAbsolutePath();
            name = fileInfo.getFileName();
        }
        
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
        switch (filteredViewFilesType) {
            case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_ALL:
                break;
            case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED:
                if (caretDataFile == NULL) {
                    isFileHidden = true;
                }
                break;
            case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED_MODIFIED:
                if (caretDataFile != NULL) {
                    if ( ! caretDataFile->isModified()) {
                        isFileHidden = true;
                    }
                }
                else {
                    isFileHidden = true;
                }
                break;
            case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_LOADED_NOT_MODIFIED:
                if (caretDataFile != NULL) {
                    if (caretDataFile->isModified()) {
                        isFileHidden = true;
                    }
                }
                else {
                    isFileHidden = true;
                }
                break;
            case SpecFileDialogViewFilesTypeEnum::VIEW_FILES_NOT_LOADED:
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
    
    enableLoadOrSaveButton();
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

#include "EventDataFileDelete.h"

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
WuQDialogModal::DialogUserButtonResult
SpecFileManagementDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_loadScenesPushButton) {
        /*
         * Load all of the scene files but nothing else
         */
        m_specFile->setAllSceneFilesSelectedForLoadingAndAllOtherFilesNotSelected();
        
        okButtonClickedOpenSpecFile();
        
        GuiManager::get()->processShowSceneDialog(GuiManager::get()->getActiveBrowserWindow());
        
        return RESULT_MODAL_ACCEPT;
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
        case MODE_MANAGE_FILES:
            allowDialogToClose = okButtonClickedManageAndSaveFiles();
            break;
        case MODE_OPEN_SPEC_FILE:
            okButtonClickedOpenSpecFile();
            allowDialogToClose = true;
            break;
        case MODE_SAVE_FILES_WHILE_QUITTING:
            allowDialogToClose = okButtonClickedManageAndSaveFiles();
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
    AString username;
    AString password;
    
    if (m_specFile->hasFilesWithRemotePathSelectedForLoading()) {
        const QString msg("This spec file contains files that are on the network.  "
                          "If accessing the files requires a username and "
                          "password, enter it here.  Otherwise, remove any "
                          "text from the username and password fields.");
        
        
        if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(this,
                                                                   "Username and Password",
                                                                   msg,
                                                                   username,
                                                                   password)) {
            /* nothing */
        }
        else {
            return;
        }
    }
    
    AString specFileErrorMessage = writeSpecFile(true);
    AString errorMessages;
    errorMessages.appendWithNewLine(specFileErrorMessage);
    
    EventSpecFileReadDataFiles readSpecFileEvent(m_brain,
                                                 m_specFile);
    readSpecFileEvent.setUsernameAndPassword(username,
                                             password);
    
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
 * Perform processing when the Open button is pressed for Manage Files 
 * or Save Files mode.
 */
bool
SpecFileManagementDialog::okButtonClickedManageAndSaveFiles()
{
    /*
     * Wait cursor
     */
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    AString errorMessages;
    
    const int32_t numDataFiles = static_cast<int32_t>(m_tableRowDataFileContent.size());
    for (int32_t i = 0; i < numDataFiles; i++) {
        SpecFileDataFile* specFileDataFile = m_tableRowDataFileContent[i]->m_specFileDataFile;
        CaretAssert(specFileDataFile);
        if (specFileDataFile->isSavingSelected()) {
            CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
            if (caretDataFile != NULL) {
                if (caretDataFile->supportsWriting()) {
                    try {
                        m_brain->writeDataFile(caretDataFile);
                        specFileDataFile->setSavingSelected(false);
                    }
                    catch (const DataFileException& e) {
                        errorMessages.appendWithNewLine(e.whatString());
                    }
                }
            }
        }
        
    }
    
    CaretAssert(m_COLUMN_SAVE_CHECKBOX >= 0);
    QTableWidgetItem* saveItem = getTableWidgetItem(m_specFileTableRowIndex,
                                                    m_COLUMN_SAVE_CHECKBOX);
    if (saveItem->checkState() == Qt::Checked) {
        AString specFileName = m_specFile->getFileName();
        if (m_specFile->getFileName().isEmpty()) {
            errorMessages.appendWithNewLine("Spec File name is empty.");
        }
        else {
            m_specFile->removeAnyFileInformationIfNotInSpecAndNoCaretDataFile();
            AString specFileErrorMessage = writeSpecFile(false);
            if (specFileErrorMessage.isEmpty() == false) {
                errorMessages.appendWithNewLine(specFileErrorMessage);
            }
            else {
            }
            saveItem->setCheckState(Qt::Unchecked);
        }
    }
    
    /*
     * Spec file may have changed by SpecFile::removeAnyFileInformationIfNotInSpecAndNoCaretDataFile().
     */
    getDataFileContentFromSpecFile();
    updateTableDimensionsToFitFiles();
    loadSpecFileContentIntoDialog();
    
    cursor.restoreCursor();
    
    if (errorMessages.isEmpty() == false) {
        WuQMessageBox::errorOk(this, errorMessages);
        return false;
    }

    bool allowDialogToClose = false;
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
            break;
        case MODE_OPEN_SPEC_FILE:
            break;
        case MODE_SAVE_FILES_WHILE_QUITTING:
            allowDialogToClose = true;
            break;
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    return allowDialogToClose;
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
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->addToPreviousSpecFiles(m_specFile->getFileName());
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
    QWidget* removeButtonWidget = m_filesTableWidget->cellWidget(rowIndex,
                                                                 m_COLUMN_CLOSE_BUTTON);
    CaretAssert(removeButtonWidget);
    
    bool updateFlag = false;
    
    if (rowIndex == m_sceneAnnotationFileRowIndex) {
        AnnotationFile* sceneAnnotationFile = m_brain->getSceneAnnotationFile();
        CaretAssert(sceneAnnotationFile);
        
        if ( ! sceneAnnotationFile->isEmpty()) {
            const QString msg = ("Remove all scene annotations?");
            if ( ! WuQMessageBox::warningOkCancel(removeButtonWidget, msg)) {
                return;
            }
            
            sceneAnnotationFile->clear();
            updateFlag = true;
        }
    }
    else {
        SpecFileManagementDialogRowContent* rowContent = getFileContentInRow(rowIndex);
        SpecFileDataFile* specFileDataFile = rowContent->m_specFileDataFile;
        
        CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
        if (caretDataFile != NULL) {
            if (caretDataFile->isModified()) {
                const QString msg = (caretDataFile->getFileNameNoPath()
                                     + " is modified.  Close without saving changes?");
                if ( ! WuQMessageBox::warningOkCancel(removeButtonWidget, msg)) {
                    return;
                }
            }
            EventManager::get()->sendEvent(EventDataFileDelete(caretDataFile).getPointer());
            updateFlag = true;
        }
    }

    if (updateFlag) {
        getDataFileContentFromSpecFile();
        loadSpecFileContentIntoDialog();
        updateGraphicWindowsAndUserInterface();
    }
}

/**
 * Get the content for the given row.
 *
 * @param rowIndex
 *    Index of the row.
 * @return 
 *    Content for the given row.
 */
SpecFileManagementDialogRowContent*
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
    SpecFileManagementDialogRowContent* rowContent = getFileContentInRow(rowIndex);
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
        
        AString username;
        AString password;
        
        if (DataFile::isFileOnNetwork(caretDataFile->getFileName())) {
            const QString msg("This file is on the network.  "
                              "If accessing the file requires a username and "
                              "password, enter it here.  Otherwise, remove any "
                              "text from the username and password fields.");
            
            
            if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(this,
                                                                       "Username and Password",
                                                                       msg,
                                                                       username,
                                                                       password)) {
                /* nothing */
            }
            else {
                return;
            }
        }
        
        specFileDataFile->setSavingSelected(false);
        
        EventDataFileReload reloadEvent(m_brain,
                                        caretDataFile);
        reloadEvent.setUsernameAndPassword(username,
                                           password);
        EventManager::get()->sendEvent(reloadEvent.getPointer());
        
        if (reloadEvent.isError()) {
            errorMessage.appendWithNewLine(reloadEvent.getErrorMessage());
        }
    }
    else {
        AString username;
        AString password;
        if (DataFile::isFileOnNetwork(specFileDataFile->getFileName())) {
            const QString msg("This file is on the network.  "
                              "If accessing the file requires a username and "
                              "password, enter it here.  Otherwise, remove any "
                              "text from the username and password fields.");
            
            
            if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(this,
                                                                       "Username and Password",
                                                                       msg,
                                                                       username,
                                                                       password)) {
                /* nothing */
            }
            else {
                return;
            }
        }
        
        EventDataFileRead readEvent(m_brain);
        readEvent.setUsernameAndPassword(username,
                                         password);
        readEvent.addDataFile(specFileDataFile->getStructure(),
                              specFileDataFile->getDataFileType(),
                              specFileDataFile->getFileName());
        
        EventManager::get()->sendEvent(readEvent.getPointer());
        
        if (readEvent.isError()) {
            errorMessage.appendWithNewLine(readEvent.getErrorMessage());
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
    m_filesTableWidget->setRangeSelected(QTableWidgetSelectionRange(rowIndex, m_COLUMN_OPTIONS_TOOLBUTTON,
                                                                    rowIndex, m_COLUMN_OPTIONS_TOOLBUTTON), false);
    
    const AString copyPathText("Copy Path and File Name to Clipboard");
    
    if (rowIndex == m_specFileTableRowIndex) {
        QMenu menu;
        QAction* copyFilePathToClipboardAction = NULL;
        if (m_specFile != NULL) {
            if ( ! m_specFile->getFileName().trimmed().isEmpty()) {
                copyFilePathToClipboardAction = menu.addAction(copyPathText);
            }
        }
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
            loadSpecFileContentIntoDialog();
        }
        else if (selectedAction == copyFilePathToClipboardAction) {
            QApplication::clipboard()->setText(m_specFile->getFileName().trimmed(),
                                               QClipboard::Clipboard);
        }
        else if (selectedAction != NULL) {
            CaretAssertMessage(0,
                               ("Unhandled Menu Action: " + selectedAction->text()));
        }
    }
    else if (rowIndex == m_sceneAnnotationFileRowIndex) {
        /* 
         * No file options for scene annotation file
         */
    }
    else {
        SpecFileManagementDialogRowContent* rowContent = getFileContentInRow(rowIndex);
        SpecFileDataFile* specFileDataFile = rowContent->m_specFileDataFile;
        CaretDataFile* caretDataFile = specFileDataFile->getCaretDataFile();
        QAction* copyFilePathToClipboardAction = NULL;
        QAction* editMetaDataAction = NULL;
        QAction* setFileNameAction = NULL;
        QAction* setStructureAction = NULL;
        //QAction* unloadFileAction = NULL;
        QAction* unloadFileMapsAction = NULL;
        QAction* viewMetaDataAction = NULL;
                
        QMenu menu;
        switch (m_dialogMode) {
            case MODE_MANAGE_FILES:
            case MODE_SAVE_FILES_WHILE_QUITTING:
                if (caretDataFile != NULL) {
                    copyFilePathToClipboardAction = menu.addAction(copyPathText);
                    editMetaDataAction = menu.addAction("Edit Metadata...");
                    setFileNameAction = menu.addAction("Set File Name...");
                    //                unloadFileAction = menu.addAction("Unload File");
                    //if (caretMappableDataFile != NULL) {
                    //    unloadFileMapsAction = menu.addAction("Unload Map(s) from File");
                    //    unloadFileMapsAction->setEnabled(false);
                    //}
                }
                else {
                    copyFilePathToClipboardAction = menu.addAction(copyPathText);
                    //viewMetaDataAction = menu.addAction("View Metadata...");
                    //viewMetaDataAction->setEnabled(false);
                }
                break;
            case MODE_OPEN_SPEC_FILE:
                copyFilePathToClipboardAction = menu.addAction(copyPathText);
                //setStructureAction = menu.addAction("Set Structure...");
                //setStructureAction->setEnabled(false);
                //viewMetaDataAction = menu.addAction("View Metadata...");
                //viewMetaDataAction->setEnabled(false);
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
        else if (selectedAction == copyFilePathToClipboardAction) {
            copyFilePathToClipboard(specFileDataFile,
                                    caretDataFile);
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
//            if (caretDataFile->isModified()) {
//                specFileDataFile->setSavingSelected(true);
//            }
        }
        else if (selectedAction == editMetaDataAction) {
            if (caretDataFile != NULL) {
                MetaDataEditorDialog mded(caretDataFile,
                                          &menu);
                mded.exec();
                
                if (caretDataFile->isModified()) {
                    specFileDataFile->setSavingSelected(true);
                }
                loadSpecFileContentIntoDialog();
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
 * Copy the loaded file's path to the clipboard.
 *
 * @param caretDataFile
 *    The data file.
 */
void
SpecFileManagementDialog::copyFilePathToClipboard(const SpecFileDataFile* specFileDataFile,
                                                  const CaretDataFile* caretDataFile)
{
    if (caretDataFile != NULL) {
        QApplication::clipboard()->setText(caretDataFile->getFileName().trimmed(),
                                           QClipboard::Clipboard);
    }
    else if (specFileDataFile != NULL) {
        QApplication::clipboard()->setText(specFileDataFile->getFileName().trimmed(),
                                           QClipboard::Clipboard);
    }
    else {
        CaretAssert(0);
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
    
    const AString newFileName = CaretFileDialog::getChooseFileNameDialog(caretDataFile->getDataFileType(),
                                                                      caretDataFile->getFileName(),
                                                                      parent);
    if (newFileName.isEmpty()) {
        return;
    }
    
//    QStringList filenameFilterList;
//    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(caretDataFile->getDataFileType()));
//    CaretFileDialog fd(parent);
//    fd.setAcceptMode(CaretFileDialog::AcceptSave);
//    fd.setNameFilters(filenameFilterList);
//    fd.setFileMode(CaretFileDialog::AnyFile);
//    fd.setViewMode(CaretFileDialog::List);
//    fd.selectFile(caretDataFile->getFileName());
//    fd.setLabelText(CaretFileDialog::Accept, "Choose");
//    fd.setWindowTitle("Choose File Name");
//    if (fd.exec() == CaretFileDialog::Accepted) {
//        QStringList files = fd.selectedFiles();
//        if (files.isEmpty() == false) {
//            AString newFileName = files.at(0);
            if (newFileName != caretDataFile->getFileName()) {
                /*
                 * Clone current item, remove file from it,
                 * and create new item.
                 *
                 * Note: specFileDataFile is NULL when changing the name
                 * of the spec file.
                 */
                if (specFileDataFile != NULL) {
                    SpecFileDataFile* sfdf = m_specFile->changeFileName(specFileDataFile,
                                                                        newFileName);
                    caretDataFile = sfdf->getCaretDataFile();
                    CaretAssert(caretDataFile);
                    if (caretDataFile->isModified()) {
                        sfdf->setSavingSelected(true);
                    }
                }
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
//        }
//    }
}

///**
// * Called when spec file options tool button is triggered.
// */
//void
//SpecFileManagementDialog::specFileOptionsActionTriggered()
//{
//    QAction* setFileNameAction = NULL;
//    
//    QMenu menu;
//    QAction* metadataAction = menu.addAction("Edit Metadata...");
//    metadataAction->setEnabled(false);
//    switch (m_dialogMode) {
//        case MODE_MANAGE_FILES:
//        case MODE_SAVE_FILES_WHILE_QUITTING:
//            setFileNameAction = menu.addAction("Set File Name...");
//            break;
//        case MODE_OPEN_SPEC_FILE:
//            break;
//    }
//    
//    QAction* selectedAction = menu.exec(QCursor::pos());
//    
//    if (selectedAction == setFileNameAction) {
//        QStringList filenameFilterList;
//        filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
//        CaretFileDialog fd(&menu);
//        fd.setAcceptMode(CaretFileDialog::AcceptSave);
//        fd.setNameFilters(filenameFilterList);
//        fd.setFileMode(CaretFileDialog::AnyFile);
//        fd.setViewMode(CaretFileDialog::List);
//        fd.selectFile(m_specFile->getFileName());
//        fd.setLabelText(CaretFileDialog::Accept, "Choose");
//        fd.setWindowTitle("Choose Spec File Name");
//        if (fd.exec() == CaretFileDialog::Accepted) {
//            QStringList files = fd.selectedFiles();
//            if (files.isEmpty() == false) {
//                AString newFileName = files.at(0);
//                m_specFile->setFileName(newFileName);
//                loadSpecFileContentIntoDialog();
//            }
//        }
//    }
//    else if (selectedAction == metadataAction) {
//        
//    }
//    else if (selectedAction != NULL) {
//        CaretAssertMessage(0,
//                           ("Unhandled Menu Action: " + selectedAction->text()));
//    }
//}

///**
// * Called to choose the name of the spec file.
// */
//void
//SpecFileManagementDialog::chooseSpecFileNameActionTriggered()
//{
//    QWidget* toolButtonWidget = m_filesTableWidget->cellWidget(m_specFileTableRowIndex,
//                                                               m_COLUMN_OPTIONS_TOOLBUTTON);
//    CaretAssert(toolButtonWidget);
//    
//    QStringList filenameFilterList;
//    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
//    CaretFileDialog fd(toolButtonWidget);
//    fd.setAcceptMode(CaretFileDialog::AcceptSave);
//    fd.setNameFilters(filenameFilterList);
//    fd.setFileMode(CaretFileDialog::AnyFile);
//    fd.setViewMode(CaretFileDialog::List);
//    fd.selectFile(m_specFile->getFileName());
//    fd.setLabelText(CaretFileDialog::Accept, "Choose");
//    fd.setWindowTitle("Choose Spec File Name");
//    if (fd.exec() == CaretFileDialog::Accepted) {
//        QStringList files = fd.selectedFiles();
//        if (files.isEmpty() == false) {
//            AString newFileName = files.at(0);
//            m_specFile->setFileName(newFileName);
//            loadSpecFileContentIntoDialog();
//        }
//    }
//}


/**
 * @return Create and return a toolbar for viewing files by type of file.
 * @param labelOut
 *    If text was not empty, this parameter will be set to the QLabel that
 *    contains the text.  Otherwise, it will be NULL upon exit.
 */
QToolBar*
SpecFileManagementDialog::createFilesTypesToolBar(QLabel* &labelOut)
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
                                                     labelOut,
                                                     m_fileTypesActionGroup);
    return toolbar;
}

/**
 * @return Create and return a toolbar for selecting all or no files.
 * @param labelOut
 *    If text was not empty, this parameter will be set to the QLabel that
 *    contains the text.  Otherwise, it will be NULL upon exit.
 */
QToolBar*
SpecFileManagementDialog::createFilesSelectionToolBar(QLabel* &labelOut)
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
                                                     labelOut,
                                                     m_fileSelectionActionGroup);
    return toolbar;
}

/**
 * @return Create and return a toolbar for selecting loaded or not loaded files.
 * @param labelOut
 *    If text was not empty, this parameter will be set to the QLabel that
 *    contains the text.  Otherwise, it will be NULL upon exit.
 */
QToolBar*
SpecFileManagementDialog::createManageFilesLoadedNotLoadedToolBar(QLabel* &labelOut)
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
    
    QAction* loadedFilesModifiedAction = m_manageFilesLoadedNotLoadedActionGroup->addAction("Loaded:Modified");
    loadedFilesModifiedAction->setData(qVariantFromValue((int)MANAGE_FILES_LOADED_MODIFIED));
    loadedFilesModifiedAction->setCheckable(true);
    
    QAction* loadedFilesNotModifiedAction = m_manageFilesLoadedNotLoadedActionGroup->addAction("Loaded:Not Modified");
    loadedFilesNotModifiedAction->setData(qVariantFromValue((int)MANAGE_FILES_LOADED_NOT_MODIFIED));
    loadedFilesNotModifiedAction->setCheckable(true);
    
    QAction* notLoadedFilesAction = m_manageFilesLoadedNotLoadedActionGroup->addAction("Not Loaded");
    notLoadedFilesAction->setData(qVariantFromValue((int)MANAGE_FILES_NOT_LOADED));
    notLoadedFilesAction->setCheckable(true);
    
    m_manageFilesLoadedNotLoadedActionGroup->blockSignals(true);
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
            allFilesAction->setChecked(true);
            break;
        case MODE_OPEN_SPEC_FILE:
            allFilesAction->setChecked(true);
            break;
        case MODE_SAVE_FILES_WHILE_QUITTING:
            loadedFilesModifiedAction->setChecked(true);
            break;
    }
    m_manageFilesLoadedNotLoadedActionGroup->blockSignals(false);
    
    QToolBar* toolbar = createToolBarWithActionGroup("View Files: ",
                                                     labelOut,
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
SpecFileManagementDialog::createStructureToolBar(QLabel* &labelOut)
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
                                                     labelOut,
                                                     m_structureActionGroup);
    return toolbar;
}

/**
 * Create a toolbar with the given label containing all items
 * in the given action group.
 *
 * @param text
 *    If not empty, this text is inserted into the left side of the toolbar.
 * @param labelOut
 *    If text was not empty, this parameter will be set to the QLabel that
 *    contains the text.  Otherwise, it will be NULL upon exit.
 * @param actionGroup
 *    All actions from this action group are added to the toolbar.
 * @return
 *    The toolbar.
 */
QToolBar*
SpecFileManagementDialog::createToolBarWithActionGroup(const QString& text,
                                                       QLabel* &labelOut,
                                                       QActionGroup* actionGroup)
{
    QToolBar* toolbar = new QToolBar();
    if (text.isEmpty() == false) {
        labelOut = new QLabel(text);
        labelOut->setAlignment(Qt::AlignLeft);
        toolbar->addWidget(labelOut);
    }
    else {
        labelOut = NULL;
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
SpecFileManagementDialog::toolBarFileTypeActionTriggered(QAction* /*action*/)
{
//    if (action != NULL) {
//        const int dataValue = action->data().toInt();
//        bool isValid = false;
//        const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromIntegerCode(dataValue,
//                                                                                      &isValid);
//    }
    
    loadSpecFileContentIntoDialog();
}

/**
 * Called when tool bar's structure button is selected.
 *
 * @param action
 *    QAction of item selected.
 */
void
SpecFileManagementDialog::toolBarStructuresActionTriggered(QAction* /*action*/)
{
//    if (action != NULL) {
//        const int dataValue = action->data().toInt();
//        bool isValid = false;
//        const StructureEnum::Enum structure = StructureEnum::fromIntegerCode(dataValue,
//                                                                             &isValid);
//    }
    
    loadSpecFileContentIntoDialog();
}

/**
 * Show loaded/not loaded files when manage files mode.
 *
 * @param action
 *   QAction of item selected.
 */
void
SpecFileManagementDialog::toolBarManageFilesLoadedNotLoadedActionTriggered(QAction* /*action*/)
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

/* ======================================================================= */
/**
 * \class caret::SpecFileManagementDialogRowContent
 * \brief Content of a row in the SpecFileManagementDialog.
 * \ingroup GuiQt
 */

SpecFileManagementDialogRowContent::SpecFileManagementDialogRowContent(SpecFileDataFileTypeGroup* specFileDataFileTypeGroup,
                        SpecFileDataFile* specFileDataFile)
{
    m_tableRowIndex             = -1;
    m_specFileDataFileTypeGroup = specFileDataFileTypeGroup;
    m_specFileDataFile          = specFileDataFile;
}

SpecFileManagementDialogRowContent::~SpecFileManagementDialogRowContent()
{
    
}

/**
 * Less than method for sorting using the sorting key.
 *
 * @param item1
 *    Tested for less than the item2
 * @param item2
 *    Tested for greater than the item1
 * @return
 *    True if item1 is less than item2, else false.
 */
bool
SpecFileManagementDialogRowContent::lessThanForSorting(const SpecFileManagementDialogRowContent* item1,
                                            const SpecFileManagementDialogRowContent* item2)
{
    return (item1->m_sortingKey < item2->m_sortingKey);
}

/**
 * Set the sorting key for the given sorting type prior to sorting.
 * Creates a text string that is used for sorting that is used by
 * the static sorting method.
 *
 * @param sorting
 *    Type of sorting.
 */
void
SpecFileManagementDialogRowContent::setSortingKey(const Sorting sorting)
{
    FileInformation fileInfo(m_specFileDataFile->getFileName());
    const QString filename = fileInfo.getFileName().toUpper();
    
    const DataFileTypeEnum::Enum dataFileType = m_specFileDataFile->getDataFileType();
    QString typeName = SpecFileManagementDialog::getEditedDataFileTypeName(dataFileType);

    /*
     * Push surface files to the top???
     */
//    if (dataFileType == DataFileTypeEnum::SURFACE) {
//        typeName = "AAAAAA";
//    }

    /*
     * Push non-specific structures to the bottom
     */
    const StructureEnum::Enum structure = m_specFileDataFile->getStructure();
    QString structureName = StructureEnum::toGuiName(structure);
    switch (structure) {
        case StructureEnum::ALL:
        case StructureEnum::ALL_GREY_MATTER:
        case StructureEnum::ALL_WHITE_MATTER:
        case StructureEnum::INVALID:
        case StructureEnum::OTHER:
            structureName = "zzzzzz";
            break;
        default:
            break;
    }
    
    m_sortingKey = "";
    
    switch (sorting) {
        case SORTING_TYPE_STRUCTURE_NAME:
            m_sortingKey = (typeName
                            + structureName
                            + filename);
            break;
        case SORTING_NAME:
            m_sortingKey = filename;
            break;
        case SORTING_STRUCTURE_TYPE_NAME:
            m_sortingKey = (structureName
                            + typeName
                            + filename);
            break;
    }
}

