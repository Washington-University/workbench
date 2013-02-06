
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
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "EventDataFileRead.h"
#include "EventDataFileReload.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
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
SpecFileManagementDialog::createOpenSpecFileDialog(Brain* brain,
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
SpecFileManagementDialog::createManageFilesDialog(Brain* brain,
                                                  SpecFile* specFile,
                                                  QWidget* parent)
{
    CaretAssert(brain);
    const AString title = ("Manage Data Files");
    
    brain->determineDisplayedDataFiles();
    
    SpecFileManagementDialog* d = new SpecFileManagementDialog(MODE_MANAGE_FILES,
                                                               brain,
                                                               specFile,
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
    setDeleteWhenClosed(true);
    
    /*
     * Mac wheel event causes unintentional selection of combo box
     */
    m_comboBoxWheelEventBlockingFilter = new WuQEventBlockingFilter(this);
#ifdef CARET_OS_MACOSX
    m_comboBoxWheelEventBlockingFilter->setEventBlocked(QEvent::Wheel,
                                                        true);
#endif // CARET_OS_MACOSX
    
    bool haveConnectivityFiles = false;
    bool haveSceneFiles = false;
    
    /*
     * Display each type of data file
     */
    const int32_t numGroups = m_specFile->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0 ; ig < numGroups; ig++) {
        /*
         * File type of group
         */
        SpecFileDataFileTypeGroup* group = m_specFile->getDataFileTypeGroup(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        
        std::vector<SpecFileDataFile*> dataFileInfoVector;
        
        const AString groupName = getEditedGroupName(DataFileTypeEnum::toGuiName(dataFileType));

        GuiSpecFileDataFileTypeGroup* guiSpecGroup = new GuiSpecFileDataFileTypeGroup(m_brain,
                                                                                      m_dialogMode,
                                                                                      group,
                                                                                      groupName,
                                                                                      this);
        m_guiSpecFileDataFileTypeGroups.push_back(guiSpecGroup);
        
        if (dataFileType == DataFileTypeEnum::SCENE) {
            haveSceneFiles = true;
        }
    }

    if (m_dialogMode == MODE_MANAGE_FILES) {
        std::vector<CaretDataFile*> caretDataFiles;
        brain->getAllDataFiles(caretDataFiles);
        
        const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
        for (std::vector<CaretDataFile*>::iterator iter = caretDataFiles.begin();
             iter != caretDataFiles.end();
             iter++) {
            CaretDataFile* cdf = *iter;
            for (int32_t i = 0; i < numGuiGroups; i++) {
                GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[i];
                if (guiSpecGroup->addDataFile(cdf)) {
                    break;
                }
            }
        }
    }
    
    AString loadSaveColumnTitle;
    switch (m_dialogMode) {
        case MODE_MANAGE_FILES:
            loadSaveColumnTitle = "Save";
            break;
        case MODE_OPEN_SPEC_FILE:
            loadSaveColumnTitle = "Load";
            break;
    }
    /*
     * Columns for file layout
     */
    int ctr = 0;
    const int COLUMN_LOAD_SAVE_CHECKBOX          = ctr++;
    const int COLUMN_STATUS_LABEL                = ctr++;
    const int COLUMN_IN_SPEC_FILE_CHECKBOX       = ctr++;
    const int COLUMN_RELOAD_OPEN_BUTTON          = ctr++;
    const int COLUMN_OPTIONS_TOOLBUTTON          = ctr++;
    const int COLUMN_SELECT_FILE_NAME_TOOLBUTTON = ctr++;
    const int COLUMN_SELECT_FILE_NAME_LABEL      = ctr++;
    const int COLUMN_COUNT                       = ctr++;
    
    /*
     * Layout for files
     */
    QWidget* filesGridWidget = new QWidget();
    QGridLayout* filesGridLayout = new QGridLayout(filesGridWidget);
    WuQtUtilities::setLayoutMargins(filesGridLayout, 4, 4);
    for (int i = 0; i < COLUMN_COUNT; i++) {
        filesGridLayout->setColumnStretch(i, 0);
    }
    /*
     * Column Titles
     */
    const int titleRow = filesGridLayout->rowCount();
    filesGridLayout->addWidget(new QLabel(loadSaveColumnTitle),
                               titleRow, COLUMN_LOAD_SAVE_CHECKBOX,
                               Qt::AlignHCenter);
    filesGridLayout->addWidget(new QLabel("In-Spec"),
                               titleRow, COLUMN_IN_SPEC_FILE_CHECKBOX,
                               Qt::AlignHCenter);
    filesGridLayout->addWidget(new QLabel("Read"),
                               titleRow, COLUMN_RELOAD_OPEN_BUTTON,
                               Qt::AlignHCenter);
    filesGridLayout->addWidget(new QLabel(" Status "),
                               titleRow, COLUMN_STATUS_LABEL,
                               Qt::AlignHCenter);
    filesGridLayout->addWidget(new QLabel("More"),
                               titleRow, COLUMN_OPTIONS_TOOLBUTTON,
                               Qt::AlignHCenter);
    filesGridLayout->addWidget(new QLabel("Data File Name"),
                               titleRow, COLUMN_SELECT_FILE_NAME_TOOLBUTTON,
                               1, 2,
                               Qt::AlignLeft);
//    filesGridLayout->addWidget(new QLabel("Name"),
//                               titleRow, COLUMN_SELECT_FILE_NAME_LABEL);
    
    /*
     * Add all files to the dialog.
     */
    const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
    for (int32_t i = 0; i < numGuiGroups; i++) {
        GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[i];
        
        /*
         * Group's title row
         */
        const int row = filesGridLayout->rowCount();
        filesGridLayout->addWidget(guiSpecGroup->m_leftHorizontalLineWidget,
                                   row, COLUMN_LOAD_SAVE_CHECKBOX,
                                   1, 6);
        QHBoxLayout* titleLayout = new QHBoxLayout();
        WuQtUtilities::setLayoutMargins(titleLayout, 2, 0);
        titleLayout->addWidget(guiSpecGroup->m_groupNameLabel, 0);
        titleLayout->addWidget(guiSpecGroup->m_rightHorizontalLineWidget, 100);
        filesGridLayout->addLayout(titleLayout,
                                   row, COLUMN_SELECT_FILE_NAME_LABEL,
                                   Qt::AlignLeft);

        /*
         * Add files in group
         */
        const int numGuiFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
        for (int32_t j = 0; j < numGuiFiles; j++) {
            const int row = filesGridLayout->rowCount();
            
            GuiSpecFileDataFile* guiSpecDataFile = guiSpecGroup->getGuiSpecFileDataFile(j);

            
            filesGridLayout->addWidget(guiSpecDataFile->m_loadOrSaveCheckBox,
                                    row, COLUMN_LOAD_SAVE_CHECKBOX,
                                       Qt::AlignHCenter);
            filesGridLayout->addWidget(guiSpecDataFile->m_inSpecFileCheckBox,
                                       row, COLUMN_IN_SPEC_FILE_CHECKBOX,
                                       Qt::AlignHCenter);
            filesGridLayout->addWidget(guiSpecDataFile->m_reloadOrOpenFileToolButton,
                                       row, COLUMN_RELOAD_OPEN_BUTTON,
                                       Qt::AlignHCenter);
            filesGridLayout->addWidget(guiSpecDataFile->m_loadedModifiedAndDisplayedLabel,
                                       row, COLUMN_STATUS_LABEL,
                                       Qt::AlignLeft);
            filesGridLayout->addWidget(guiSpecDataFile->m_optionsToolButton,
                                       row, COLUMN_OPTIONS_TOOLBUTTON,
                                       Qt::AlignHCenter);
            filesGridLayout->addWidget(guiSpecDataFile->m_selectFileNameToolButton,
                                       row, COLUMN_SELECT_FILE_NAME_TOOLBUTTON,
                                       Qt::AlignHCenter);
            filesGridLayout->addWidget(guiSpecDataFile->m_filenameLabel,
                                       row, COLUMN_SELECT_FILE_NAME_LABEL,
                                       Qt::AlignLeft);
        }
        
        guiSpecGroup->updateContent();
    }
    
    /*
     * Widget and layout for files.
     *
     * Two layouts used so widget is pushed to the top left (and not
     * spread out) when groups of files are hidden.
     */
    QWidget* filesWidgetPushedLeft = new QWidget();
    QHBoxLayout* filesPushedLeftWidgetLayout = new QHBoxLayout(filesWidgetPushedLeft);
    WuQtUtilities::setLayoutMargins(filesPushedLeftWidgetLayout, 0, 0);
    filesPushedLeftWidgetLayout->addWidget(filesGridWidget);
    filesPushedLeftWidgetLayout->addStretch();
    QWidget* filesWidgetPushedTop = new QWidget();
    QVBoxLayout* filesWidgetPushedTopLayout = new QVBoxLayout(filesWidgetPushedTop);
    WuQtUtilities::setLayoutMargins(filesWidgetPushedTopLayout, 0, 0);
    filesWidgetPushedTopLayout->addWidget(filesWidgetPushedLeft);
    filesWidgetPushedTopLayout->addStretch();
    
    QWidget* toolbarWidget = new QWidget();
    QVBoxLayout* toolbarLayout = new QVBoxLayout(toolbarWidget);
    toolbarLayout->addWidget(createFilesTypesToolBar());
    toolbarLayout->addWidget(createStructureToolBar());
    toolbarLayout->addWidget(createFilesSelectionToolBar());
    
    setTopBottomAndCentralWidgets(toolbarWidget,
                                  filesWidgetPushedTop,
                                  NULL,
                                  true);
    
    updateDisplayedFiles();
}

/**
 * Destructor.
 */
SpecFileManagementDialog::~SpecFileManagementDialog()
{
    
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
//    * When managing, ALL LOADED NON-LOADED
    
    m_fileSelectionActionGroup = new QActionGroup(this);
    m_fileSelectionActionGroup->setExclusive(true);
    QObject::connect(m_fileSelectionActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(toolBarSelectFilesActionTriggered(QAction*)));
    
    QAction* allFilesAction = m_fileSelectionActionGroup->addAction("All");
    allFilesAction->setCheckable(true);
    allFilesAction->setData(qVariantFromValue(SHOW_FILES_ALL));
    
    QAction* loadedFilesAction = m_fileSelectionActionGroup->addAction("Loaded");
    loadedFilesAction->setCheckable(true);
    loadedFilesAction->setData(qVariantFromValue(SHOW_FILES_LOADED));
    
    QAction* noneFilesAction = m_fileSelectionActionGroup->addAction("None");
    noneFilesAction->setCheckable(true);
    noneFilesAction->setData(qVariantFromValue(0));
    
    QToolBar* toolbar = createToolBarWithActionGroup("Select Files: ",
                                                     m_fileSelectionActionGroup);    
    return toolbar;
}

/**
 * @return Edit and return the text for a name of a group.
 */
AString
SpecFileManagementDialog::getEditedGroupName(const AString& groupName) const
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
        
        if (dataValue == SHOW_FILES_ALL) {
            std::cout << "Selected ALL files: " << std::endl;
        }
        else if (dataValue == SHOW_FILES_LOADED) {
            std::cout << "Selected LOADED files: " << std::endl;
        }
        else {
            std::cout << "Selected NONE files: " << std::endl;
        }
    }    
}

/**
 * Update the displayed files.
 */
void
SpecFileManagementDialog::updateDisplayedFiles()
{
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
    
    const int32_t numGuiGroups = static_cast<int32_t>(m_guiSpecFileDataFileTypeGroups.size());
    for (int32_t i = 0; i < numGuiGroups; i++) {
        GuiSpecFileDataFileTypeGroup* guiSpecGroup = m_guiSpecFileDataFileTypeGroups[i];
        
        const int32_t numFiles = guiSpecGroup->getNumberOfGuiSpecFileDataFiles();
        if (numFiles > 0) {
            guiSpecGroup->setWidgetsVisibleByFileTypeAndStructure(dataFileType, structure);
        }
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
    m_groupNameLabel = new QLabel(groupName);
    m_leftHorizontalLineWidget = WuQtUtilities::createHorizontalLineWidget();
    m_rightHorizontalLineWidget = WuQtUtilities::createHorizontalLineWidget();
    
    const int32_t numFiles = specFileDataFileTypeGroup->getNumberOfFiles();
    for (int32_t i = 0; i < numFiles; i++) {
        SpecFileDataFile* specFileDataFile = specFileDataFileTypeGroup->getFileInformation(i);
        addSpecFileDataFile(specFileDataFile);
    }
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
    m_groupNameLabel->setVisible(visible);
    m_leftHorizontalLineWidget->setVisible(visible);
    m_rightHorizontalLineWidget->setVisible(visible);
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
GuiSpecFileDataFileTypeGroup::setWidgetsVisibleByFileTypeAndStructure(const DataFileTypeEnum::Enum dataFileType,
                                                                      const StructureEnum::Enum structure)
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
        
        gdf->setWidgetsVisible(showFile);
        
        if (showFile) {
            haveFilesDisplayed = true;
        }
    }
    
    m_groupNameLabel->setVisible(haveFilesDisplayed);
    m_leftHorizontalLineWidget->setVisible(haveFilesDisplayed);
    m_rightHorizontalLineWidget->setVisible(haveFilesDisplayed);
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
 * Add a spec file data file to this group.
 *
 * @param specFileDataFile
 *    Added to this group if same type.
 */
void
GuiSpecFileDataFileTypeGroup::addSpecFileDataFile(SpecFileDataFile* specFileDataFile)
{
    GuiSpecFileDataFile* guiSpecDataFile = new GuiSpecFileDataFile(m_brain,
                                                                   m_dialogMode,
                                                                   m_specFileDataFileTypeGroup->getDataFileType(),
                                                                   specFileDataFile,
                                                                   this);
    m_guiDataFiles.push_back(guiSpecDataFile);    
}

/**
 * Add a caret data file to this group.
 *
 * @param caretDataFile
 *    Caret data file added to this group if same type.
 * @return
 *    true if matching type and added, else false.
 */
bool
GuiSpecFileDataFileTypeGroup::addDataFile(CaretDataFile* caretDataFile)
{
    CaretAssert(caretDataFile);
    if (m_specFileDataFileTypeGroup->getDataFileType()
        != caretDataFile->getDataFileType()) {
        return false;
    }
    
    const int32_t numFiles = getNumberOfGuiSpecFileDataFiles();
    for (int32_t i = 0; i < numFiles; i++) {
        GuiSpecFileDataFile* gdf = getGuiSpecFileDataFile(i);
        if (gdf->addMatchingCaretDataFile(caretDataFile)) {
            return true;
        }
    }
    
    GuiSpecFileDataFile* gdf = new GuiSpecFileDataFile(m_brain,
                                                       m_dialogMode,
                                                       caretDataFile,
                                                       this);
    m_guiDataFiles.push_back(gdf);
//    const int numFiles = static_cast<int32_t>(m_specFileDataFileTypeG
//                                              )
//    GuiSpecFileDataFile* guiDataFile = new GuiSpecFileDataFile(caretDataFile,
//                                                               this);
    return true;
}

/* =================================================================== */

/**
 * Data for one CaretDataFile.
 *
 * @param brain
 *    Brain
 * @param dialogMode
 *    Mode of dialog.
 * @param specFileDataFile
 *    Entry in a spec file for a data file.
 * @param caretDataFile
 *    A CaretDataFile instance.
 * @param parent
 *    Parent of this object.
 */
GuiSpecFileDataFile::GuiSpecFileDataFile(Brain* brain,
                                         const SpecFileManagementDialog::Mode dialogMode,
                                         const DataFileTypeEnum::Enum dataFileType,
                                         SpecFileDataFile* specFileDataFile,
                                         QObject* parent)
: QObject(parent),
m_brain(brain),
m_dialogMode(dialogMode)
{
    initialize(specFileDataFile,
               dataFileType,
               NULL);
}

/**
 * Data for one CaretDataFile.
 *
 * @param brain
 *    Brain
 * @param dialogMode
 *    Mode of dialog.
 * @param specFileDataFile
 *    Entry in a spec file for a data file.
 * @param caretDataFile
 *    A CaretDataFile instance.
 * @param parent
 *    Parent of this object.
 */
GuiSpecFileDataFile::GuiSpecFileDataFile(Brain* brain,
                                         const SpecFileManagementDialog::Mode dialogMode,
                                         CaretDataFile* caretDataFile,
                                         QObject* parent)
: QObject(parent),
m_brain(brain),
m_dialogMode(dialogMode)
{
    CaretAssert(caretDataFile);
    
    initialize(NULL,
               caretDataFile->getDataFileType(),
               caretDataFile);
    if (caretDataFile->isModified()) {
        m_loadOrSaveCheckBox->setChecked(true);
    }
}

/**
 * Initialize and instance.
 *
 * @param specFileDataFile
 *     Data from the spec file about the file.
 * @param dataFileType
 *     Type of data file.
 * @param caretDataFile
 *     Caret data file in memory.
 */
void
GuiSpecFileDataFile::initialize(SpecFileDataFile* specFileDataFile,
                                const DataFileTypeEnum::Enum dataFileType,
                                CaretDataFile* caretDataFile)
{
    bool isInSpecFileFlag = false;
    if (specFileDataFile != NULL) {
        m_specFileDataFile = specFileDataFile;
        m_specFileDataFileNeedsToBeDeleted = false;
        isInSpecFileFlag = true;
    }
    else {
        m_specFileDataFile = new SpecFileDataFile(caretDataFile->getFileName(),
                                                  caretDataFile->getStructure());
        m_specFileDataFileNeedsToBeDeleted = true;
    }
    m_caretDataFile = caretDataFile;
    m_dataFileType  = dataFileType;
    m_loadOrSaveCheckBox = new QCheckBox("");
    m_loadOrSaveCheckBox->setChecked(false);
    
    switch (m_dialogMode) {
        case SpecFileManagementDialog::MODE_MANAGE_FILES:
            break;
        case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
            m_loadOrSaveCheckBox->setChecked(true);
            break;
    }
    
    m_inSpecFileCheckBox= new QCheckBox("");
    m_inSpecFileCheckBox->setChecked(isInSpecFileFlag);
    
    m_reloadOrOpenFileAction = WuQtUtilities::createAction("Open",
                                                           "tooltip",
                                                           this,
                                                           this,
                                                           SLOT(reloadOrOpenFileActionTriggered()));
    m_reloadOrOpenFileToolButton = new QToolButton();
    m_reloadOrOpenFileToolButton->setDefaultAction(m_reloadOrOpenFileAction);
    
    m_loadedModifiedAndDisplayedLabel = new QLabel("   ");
    m_loadedModifiedAndDisplayedLabel->setToolTip("L -> file is loaded\n"
                                                  "M -> file is modified\n"
                                                  "D -> file is displayed");
    
    m_optionsButtonAction  = WuQtUtilities::createAction("Options",
                                                                  "tooltip",
                                                                  this,
                                                                  this,
                                                                  SLOT(optionsMenuActionTriggered()));
    m_optionsToolButton = new QToolButton();
    m_optionsToolButton->setDefaultAction(m_optionsButtonAction);
    
    m_selectFileNameButtonAction  = WuQtUtilities::createAction("Name",
                                                                         "tooltip",
                                                                         this,
                                                                         this,
                                                                SLOT(selectFileNameActionTriggered()));
    m_selectFileNameToolButton = new QToolButton();
    m_selectFileNameToolButton->setDefaultAction(m_selectFileNameButtonAction);
    
    m_filenameLabel = new QLabel("");
    
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_loadOrSaveCheckBox);
    m_widgetGroup->add(m_inSpecFileCheckBox);
    m_widgetGroup->add(m_reloadOrOpenFileToolButton);
    m_widgetGroup->add(m_loadedModifiedAndDisplayedLabel);
    m_widgetGroup->add(m_optionsToolButton);
    m_widgetGroup->add(m_selectFileNameToolButton);
    m_widgetGroup->add(m_filenameLabel);
}

/**
 * Destructor.
 */
GuiSpecFileDataFile::~GuiSpecFileDataFile()
{
    if (m_specFileDataFileNeedsToBeDeleted) {
        delete m_specFileDataFile;
    }
}

/**
 * Update the content of this item.
 */
void
GuiSpecFileDataFile::updateContent()
{
    QString loadedText = " ";
    QString modifiedText = " ";
    QString displayedText = " ";

    QString filename;
    if (m_caretDataFile != NULL) {
        filename = m_caretDataFile->getFileName();
        
        loadedText = "L";
        if (m_caretDataFile->isModified()) {
            modifiedText = "<font color=\"#ff0000\">M</font>";
        }
        if (m_caretDataFile->isDisplayedInGUI()) {
            displayedText = "D";
        }
    }
    else {
        filename = m_specFileDataFile->getFileName();
        m_reloadOrOpenFileAction->setText("Open");
    }
    
    bool isFileSavable = true;
    switch (m_dataFileType) {
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
        m_selectFileNameButtonAction->setEnabled(false);
        m_selectFileNameButtonAction->setChecked(false);
        m_loadOrSaveCheckBox->setEnabled(false);
        m_loadOrSaveCheckBox->setChecked(false);
        
        modifiedText = " ";
    }
    
    /*
     * Format the file label's text: name (path)
     */
    FileInformation fileInfo(filename);
    const QString path = fileInfo.getAbsolutePath();
    const QString name = fileInfo.getFileName();
    QString fileNameText = name;
    if (path.isEmpty() == false) {
        fileNameText += (" ("
                         + path
                         + ")");
    }
    m_filenameLabel->setText(fileNameText);
    
    
    AString lmdText = ("<html>"
                       + loadedText
                       + modifiedText
                       + displayedText
                       + "</html>");
    
    m_loadedModifiedAndDisplayedLabel->setText(lmdText);
    
}

/**
 * Try to add a CaretDataFile by matching the file's name.
 *
 * @param caretDataFile
 *    Data file to add by matching name.
 * @return
 *    True if the file matched and was added, else false.
 */
bool
GuiSpecFileDataFile::addMatchingCaretDataFile(CaretDataFile* caretDataFile)
{
    if (m_specFileDataFile->getFileName() == caretDataFile->getFileName()) {
        m_caretDataFile = caretDataFile;
        updateContent();
        
        switch (m_dialogMode) {
            case SpecFileManagementDialog::MODE_MANAGE_FILES:
                m_reloadOrOpenFileAction->setText("Reload");
                if (m_caretDataFile->isModified()) {
                    m_loadOrSaveCheckBox->setChecked(true);
                }
                break;
            case SpecFileManagementDialog::MODE_OPEN_SPEC_FILE:
                break;
        }
        return true;
    }
    
    return false;
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
 * @return The structure.
 */
StructureEnum::Enum
GuiSpecFileDataFile::getStructure() const
{
    if (m_caretDataFile != NULL) {
        return m_caretDataFile->getStructure();
    }
    else {
        return m_specFileDataFile->getStructure();
    }
    
    return StructureEnum::INVALID;
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
    if (m_caretDataFile != NULL) {
        m_caretDataFile->setStructure(structure);
    }
    m_specFileDataFile->setStructure(structure);
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
 * Called when Reload/Open button is clicked.
 */
void
GuiSpecFileDataFile::reloadOrOpenFileActionTriggered()
{
    if (m_caretDataFile != NULL) {
        EventDataFileReload reloadEvent(m_brain,
                                        m_caretDataFile);
        EventManager::get()->sendEvent(reloadEvent.getPointer());
        
        if (reloadEvent.isError()) {
            WuQMessageBox::errorOk(m_reloadOrOpenFileToolButton,
                                         reloadEvent.getErrorMessage());
        }
    }
    else {
        EventDataFileRead readEvent(m_brain,
                                      false);
        readEvent.addDataFile(m_specFileDataFile->getStructure(),
                              m_dataFileType,
                              m_specFileDataFile->getFileName());
        
        EventManager::get()->sendEvent(readEvent.getPointer());
        
        if (readEvent.isError()) {
            WuQMessageBox::errorOk(m_reloadOrOpenFileToolButton,
                                         readEvent.getErrorMessage());
        }
        else {
            addMatchingCaretDataFile(readEvent.getDataFileRead(0));
        }
        
        if (readEvent.getAddToSpecFileErrorMessages().isEmpty() == false) {
            WuQMessageBox::errorOk(m_reloadOrOpenFileToolButton,
                                   readEvent.getAddToSpecFileErrorMessages());
        }
    }
    m_selectFileNameButtonAction->setChecked(false);
    
    this->updateContent();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when Options Menu action is triggered.
 */
void
GuiSpecFileDataFile::optionsMenuActionTriggered()
{
    
}

/**
 * Called when Filename action is triggered.
 */
void
GuiSpecFileDataFile::selectFileNameActionTriggered()
{
    
}

