
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

#define __CARET_FIVE_FILE_IMPORT_DIALOG_DECLARE__
#include "CaretFiveFileImportDialog.h"
#undef __CARET_FIVE_FILE_IMPORT_DIALOG_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretFiveFileTypeEnum.h"
#include "CursorDisplayScoped.h"
#include "EventBrowserWindowCreateTabs.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQMessageBoxTwo.h"
#include "WuQTextEditorDialog.h"

using namespace caret;


    
/**
 * \class caret::CaretFiveFileImportDialog 
 * \brief Dialog for importing Caret Five files into Workbench
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
CaretFiveFileImportDialog::CaretFiveFileImportDialog(QWidget* parent)
: WuQDialogModal("Import Caret5 Files",
                    parent)
{
    /*
     * Get all file filters and place them in a combo box
     */
    QLabel* fileFilterLabel(new QLabel("File Type:"));
    m_fileFilterComboBox = new QComboBox();
    m_fileFilterComboBox->addItem("Any File (*)");
    std::vector<CaretFiveFileTypeEnum::Enum> dataFileTypes;
    int32_t defaultComboBoxIndex(0);
    CaretFiveFileTypeEnum::getAllEnums(dataFileTypes);
    for (CaretFiveFileTypeEnum::Enum dt : dataFileTypes) {
        if (dt == CaretFiveFileTypeEnum::SPEC) {
            defaultComboBoxIndex = m_fileFilterComboBox->count();
        }
        m_fileFilterComboBox->addItem(CaretFiveFileTypeEnum::toQFileDialogFilterForReading(dt));
    }
    m_fileFilterComboBox->setCurrentIndex(defaultComboBoxIndex);

    /*
     * File selection button
     */
    QAction* fileSelectionAction(new QAction("Select..."));
    QObject::connect(fileSelectionAction, &QAction::triggered,
                     this, &CaretFiveFileImportDialog::fileSelectionActionTriggered);
    QToolButton* fileSelectionToolButton(new QToolButton());
    fileSelectionToolButton->setDefaultAction(fileSelectionAction);
    
    /*
     * Filename line edit
     */
    QLabel* filenameLabel(new QLabel("File:"));
    m_filenameLineEdit = new QLineEdit();
    m_filenameLineEdit->setMinimumWidth(600);
    m_filenameLineEdit->setReadOnly(true);
    
    /*
     * Filename group box
     */
    QGroupBox* filenameGroupBox(new QGroupBox("Choose Caret5 File for Import"));
    QGridLayout* filenameLayout(new QGridLayout(filenameGroupBox));
    filenameLayout->setColumnStretch(1, 100);
    filenameLayout->addWidget(filenameLabel, 0, 0);
    filenameLayout->addWidget(m_filenameLineEdit, 0, 1);
    filenameLayout->addWidget(fileSelectionToolButton, 0, 2);
    filenameLayout->addWidget(fileFilterLabel, 1, 0);
    filenameLayout->addWidget(m_fileFilterComboBox, 1, 1);

    /*
     * Output directory line edit
     */
    QLabel* outputDirectoryLabel(new QLabel("Directory:"));
    m_outputDirectoryLineEdit = new QLineEdit();
    m_outputDirectoryLineEdit->setMinimumWidth(600);
    m_outputDirectoryLineEdit->setReadOnly(true);

    /*
     * Output directory selection button
     */
    QAction* outputDirectorySelectionAction(new QAction("Select..."));
    QObject::connect(outputDirectorySelectionAction, &QAction::triggered,
                     this, &CaretFiveFileImportDialog::outputDirectorySelectionActionTriggered);
    QToolButton* outputDirectorySelectionToolButton(new QToolButton());
    outputDirectorySelectionToolButton->setDefaultAction(outputDirectorySelectionAction);
    
    /*
     * Output directory group box
     */
    QGroupBox* outputDirectoryGroupBox(new QGroupBox("Directory for Saving Imported Files"));
    QGridLayout* outputDirectoryLayout(new QGridLayout(outputDirectoryGroupBox));
    outputDirectoryLayout->setColumnStretch(1, 100);
    outputDirectoryLayout->addWidget(outputDirectoryLabel, 0, 0);
    outputDirectoryLayout->addWidget(m_outputDirectoryLineEdit, 0, 1);
    outputDirectoryLayout->addWidget(outputDirectorySelectionToolButton, 0, 2);

    /*
     * Converted file name prefix
     */
    QLabel* convertedFileNameLabel = new QLabel("Imported File Name Prefix");
    m_convertedFileNamePrefix = new QLineEdit();
    const AString convToolTipText("Optional (may be empty) prefix added to name of imported files");
    convertedFileNameLabel->setToolTip(convToolTipText);
    m_convertedFileNamePrefix->setToolTip(convToolTipText);
    
    /*
     * Overwrite files checkbox
     */
    m_saveConvertedFilesCheckBox = new QCheckBox("Save Converted Files");
    
    /*
     * Options Group Box
     */
    QGroupBox* optionsGroupBox(new QGroupBox("Options"));
    QGridLayout* optionsLayout(new QGridLayout(optionsGroupBox));
    optionsLayout->addWidget(convertedFileNameLabel, 0, 0);
    optionsLayout->addWidget(m_convertedFileNamePrefix, 0, 1);
    optionsLayout->addWidget(m_saveConvertedFilesCheckBox, 1, 0, 1, 2, Qt::AlignLeft);
    
    QWidget* dialogWidget(new QWidget());
    QVBoxLayout* dialogLayout(new QVBoxLayout(dialogWidget));
    dialogLayout->addWidget(filenameGroupBox);
    dialogLayout->addWidget(outputDirectoryGroupBox);
    dialogLayout->addWidget(optionsGroupBox);
    
    setCentralWidget(dialogWidget, WuQDialog::SCROLL_AREA_NEVER);
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
    
    /*
     * Initialize previous values to defaults
     */
    if (s_firstTimeFlag) {
        s_firstTimeFlag = false;
        s_previousImportFileName.clear();
        s_previousImportFileType = CaretFiveFileTypeEnum::toQFileDialogFilterForReading(CaretFiveFileTypeEnum::SPEC);
        s_previousDirectory.clear();
        s_previousFileNamePrefix = "Converted_";
        s_previousSaveConvertedFiles = true;
    }

    /*
     * Load previous values
     */
    m_filenameLineEdit->setText(s_previousImportFileName);
    m_fileFilterComboBox->setCurrentText(s_previousImportFileType);
    m_outputDirectoryLineEdit->setText(s_previousDirectory);
    m_convertedFileNamePrefix->setText(s_previousFileNamePrefix);
    m_saveConvertedFilesCheckBox->setChecked(s_previousSaveConvertedFiles);
}

/**
 * Destructor.
 */
CaretFiveFileImportDialog::~CaretFiveFileImportDialog()
{
}

/**
 * Called when select file action triggered
 */
void
CaretFiveFileImportDialog::fileSelectionActionTriggered()
{
    /*
     * Setup file selection dialog.
     */
    CaretFileDialog fd(CaretFileDialog::Mode::MODE_OPEN,
                       this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    QStringList filtersList;
    filtersList.push_back(m_fileFilterComboBox->currentText());
    fd.setNameFilters(filtersList);
    fd.selectNameFilter(m_fileFilterComboBox->currentText());
    fd.setFileMode(CaretFileDialog::ExistingFile);
    AString fileName;
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = fd.selectedFiles();
        if ( ! selectedFiles.isEmpty()) {
            m_filenameLineEdit->setText(selectedFiles.at(0));
        }
    }
}

/**
 * Called when select output directory action triggered
 */
void
CaretFiveFileImportDialog::outputDirectorySelectionActionTriggered()
{
    QFileDialog fd(this);
    fd.setOption(QFileDialog::DontUseNativeDialog, true);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setFileMode(QFileDialog::Directory);
    fd.setOption(QFileDialog::ShowDirsOnly, true);
    if (fd.exec() == QFileDialog::Accepted) {
        const QStringList fileList(fd.selectedFiles());
        if (fileList.size() == 1) {
            const QString directoryName(fileList.at(0));
            m_outputDirectoryLineEdit->setText(directoryName);
        }
    }
}

/**
 * Save values entered by user for next time
 */
void
CaretFiveFileImportDialog::saveValuesForNextTime()
{
    /*
     * Load previous values
     */
    s_previousImportFileName     = m_filenameLineEdit->text().trimmed();
    s_previousImportFileType     = m_fileFilterComboBox->currentText();
    s_previousDirectory          = m_outputDirectoryLineEdit->text().trimmed();
    s_previousFileNamePrefix     = m_convertedFileNamePrefix->text().trimmed();
    s_previousSaveConvertedFiles = m_saveConvertedFilesCheckBox->isChecked();
}

/**
 * Called when Cancel button clicked
 */
void
CaretFiveFileImportDialog::cancelButtonClicked()
{
    saveValuesForNextTime();
    WuQDialogModal::cancelButtonClicked();
}

/**
 * Called when OK button clicked
 */
void
CaretFiveFileImportDialog::okButtonClicked()
{
    saveValuesForNextTime();
    
    const AString filename(m_filenameLineEdit->text().trimmed());
    if (filename.isEmpty()) {
        WuQMessageBoxTwo::critical(this, "Error", "Name of file for importing is invalid.");
        return;
    }
    
    const AString directory(m_outputDirectoryLineEdit->text().trimmed());
    if (directory.isEmpty()) {
        WuQMessageBoxTwo::critical(this, "Error", "Directory for writing files is invalid.");
        return;
    }
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    AString errorMessages;
    AString warningMessages;
    Brain* brain(GuiManager::get()->getBrain());
    const bool successFlag(brain->importCaretFiveDataFile(filename,
                                                          m_outputDirectoryLineEdit->text().trimmed(),
                                                          m_convertedFileNamePrefix->text().trimmed(),
                                                          m_saveConvertedFilesCheckBox->isChecked(),
                                                          errorMessages,
                                                          warningMessages));
    
    cursor.restoreCursor();
    if ( ! successFlag) {
        WuQMessageBoxTwo::criticalOk(this,
                                     "Error",
                                     errorMessages);
        return;
    }
    
    AString messages;
    if ( ! errorMessages.isEmpty()) {
        messages.appendWithNewLine("ERRORS:");
        messages.appendWithNewLine(errorMessages);
    }
    if ( ! warningMessages.isEmpty()) {
        messages.appendWithNewLine("WARNINGS:");
        messages.appendWithNewLine(warningMessages);
    }
    
    
    /*
     * If there are no models loaded, will want to create default tabs.
     */
    bool createDefaultTabsFlag(false);
    EventModelGetAll modelGetAllEvent;
    EventManager::get()->sendEvent(modelGetAllEvent.getPointer());
    const int32_t numberOfModels = static_cast<int32_t>(modelGetAllEvent.getModels().size());
    if (numberOfModels <= 0) {
        createDefaultTabsFlag = true;
    }
    
    const EventBrowserWindowCreateTabs::Mode tabMode = (createDefaultTabsFlag ?
                                                        EventBrowserWindowCreateTabs::MODE_LOADED_SPEC_FILE :
                                                        EventBrowserWindowCreateTabs::MODE_LOADED_DATA_FILE);
    EventBrowserWindowCreateTabs createTabsEvent(tabMode);
    EventManager::get()->sendEvent(createTabsEvent.getPointer());
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
    cursor.restoreCursor();
        
    if ( ! messages.isEmpty()) {
        WuQTextEditorDialog::runModal("Import",
                                      messages,
                                      WuQTextEditorDialog::TextMode::PLAIN,
                                      WuQTextEditorDialog::WrapMode::YES,
                                      this);
    }
    
    WuQDialogModal::okButtonClicked();
}
