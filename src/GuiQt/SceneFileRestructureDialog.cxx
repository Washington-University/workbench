
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

#define __SCENE_FILE_RESTRUCTURE_DIALOG_DECLARE__
#include "SceneFileRestructureDialog.h"
#undef __SCENE_FILE_RESTRUCTURE_DIALOG_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CursorDisplayScoped.h"
#include "GuiManager.h"
#include "OperationException.h"
#include "OperationSceneFileRestructure.h"
#include "WuQMessageBoxTwo.h"
#include "WuQTextEditorDialog.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::SceneFileRestructureDialog 
 * \brief Dialog for scene file utilitiy operations
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param sceneFileName
 *    Name of scene file
 * @param parent
 *    Parent widget
 */
SceneFileRestructureDialog::SceneFileRestructureDialog(const AString& sceneFileName,
                                               QWidget* parent)
: WuQDialogModal("Scene File Restructure",
                 parent)
{
    QLabel* sceneFileLabel(new QLabel("Input Scene File: "));
    m_sceneFileNameLineEdit = new QLineEdit();
    QPushButton* sceneFilePushButton(new QPushButton("Choose..."));
    QObject::connect(sceneFilePushButton, &QPushButton::clicked,
                     this, &SceneFileRestructureDialog::sceneFilePushButtonClicked);
    
    const QString outputDirectoryToolTip("the name of the folder to which the spec file and data files are copied");
    QLabel* outputDirectoryLabel(new QLabel("Output Directory: "));
    m_outputDirectoryLineEdit = new QLineEdit();
    WuQtUtilities::setWordWrappedToolTip(m_outputDirectoryLineEdit,
                                         outputDirectoryToolTip);
    QPushButton* outputDirectoryPushButton(new QPushButton("Choose..."));
    QObject::connect(outputDirectoryPushButton, &QPushButton::clicked,
                     this, &SceneFileRestructureDialog::outputDirectoryPushButtonClicked);

    QLabel* overwriteLabel(new QLabel("Overwrite Existing Output Files: "));
    m_overwriteFilesComboBox = new QComboBox();
    m_overwriteFilesComboBox->addItem("YES: Overwrites existing files",
                                      (int)OperationSceneFileRestructure::OverwriteFilesMode::OVERWRITE_YES);
    m_overwriteFilesComboBox->addItem("ERROR: Fails if a file would be overwritten (default)",
                                      (int)OperationSceneFileRestructure::OverwriteFilesMode::OVERWRITE_ERROR);
    m_overwriteFilesComboBox->addItem("SKIP: Skips copying if the file already exists",
                                      (int)OperationSceneFileRestructure::OverwriteFilesMode::OVERWRITE_SKIP);

    m_skipMissingFilesCheckBox = new QCheckBox("Skip missing files (Issues warnings for missing files or files on network instead of fatal errors)");

    m_testScenesCheckBox = new QCheckBox("Test Scenes (After copying, tests loading of files for each scene in the output scene file)");
    
    m_previewCheckBox = new QCheckBox("Preview (Lists the files that would be created without performing any copying)");
    
    
    
    QLabel* instructionsLabel(new QLabel());
    instructionsLabel->setWordWrap(true);
    instructionsLabel->setText(OperationSceneFileRestructure::getInstructionsInHtml());
    QScrollArea* instructionsScrollArea(new QScrollArea());
    instructionsScrollArea->setWidget(instructionsLabel);
    
    QWidget* widget(new QWidget());
    QGridLayout* layout(new QGridLayout(widget));
    layout->setColumnStretch(1, 100);
    int row(layout->rowCount());
    layout->addWidget(sceneFileLabel,
                      row, 0);
    layout->addWidget(m_sceneFileNameLineEdit,
                      row, 1);
    layout->addWidget(sceneFilePushButton,
                      row, 2);
    row++;
    layout->addWidget(outputDirectoryLabel,
                      row, 0);
    layout->addWidget(m_outputDirectoryLineEdit,
                      row, 1);
    layout->addWidget(outputDirectoryPushButton,
                      row, 2);
    row++;
    layout->addWidget(overwriteLabel,
                      row, 0);
    layout->addWidget(m_overwriteFilesComboBox,
                      row, 1, 1, 2);
    row++;
    layout->addWidget(m_skipMissingFilesCheckBox,
                      row, 0, 1, 3);
    row++;
    layout->addWidget(m_testScenesCheckBox,
                      row, 0, 1, 3);
    row++;
    layout->addWidget(m_previewCheckBox,
                      row, 0, 1, 3);
    row++;
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                      row, 0, 1, 3);
    row++;
    layout->addWidget(instructionsScrollArea,
                      row, 0, 1, 3);
    row++;

    setCentralWidget(widget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    setMinimumWidth(500);
    
    m_outputDirectoryLineEdit->setText(s_previousOutputDirectory);
    const int32_t ovewriteIndex(m_overwriteFilesComboBox->findData((int)s_previousOvewriteFilesMode));
    if ((ovewriteIndex >= 0)
        && (ovewriteIndex < m_overwriteFilesComboBox->count())) {
        m_overwriteFilesComboBox->setCurrentIndex(ovewriteIndex);
    }
    m_previewCheckBox->setChecked(s_previousPreviewFlag);
    m_skipMissingFilesCheckBox->setChecked(s_previousSkipMissingFilessFlag);
    
    if ( ! sceneFileName.isEmpty()) {
        initializeWithSceneFile(sceneFileName);
    }
    else {
        initializeWithSceneFile(s_previousSceneFileName);
    }
}

/**
 * Destructor.
 */
SceneFileRestructureDialog::~SceneFileRestructureDialog()
{
}

/**
 * Initialize with the given scene file name
 */
void
SceneFileRestructureDialog::initializeWithSceneFile(const AString& sceneFileName)
{
    m_sceneFileNameLineEdit->setText(sceneFileName);
}

/**
 * Called when scene file push button is clicked
 */
void
SceneFileRestructureDialog::sceneFilePushButtonClicked()
{
    const AString currentSceneFileName(m_sceneFileNameLineEdit->text().trimmed());
    const AString sceneFileName(currentSceneFileName.isEmpty()
                                ? GuiManager::get()->getBrain()->getCurrentDirectory()
                                : currentSceneFileName);

    const AString name(CaretFileDialog::getOpenFileNameDialog(this,
                                                              "Choose Scene File",
                                                              sceneFileName,
                                                              DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::SCENE)));
    if ( ! name.isEmpty()) {
        initializeWithSceneFile(name);
    }

}

/**
 * Called when output directory push button is clicked
 */
void
SceneFileRestructureDialog::outputDirectoryPushButtonClicked()
{
    const AString currentDirName(m_outputDirectoryLineEdit->text().trimmed());
    const AString dirName(currentDirName.isEmpty()
                          ? ""
                          : currentDirName);
    const AString name(CaretFileDialog::getExistingDirectoryDialog(this,
                                                                   "Choose Base Directory",
                                                                   dirName));
    if ( ! name.isEmpty()) {
        m_outputDirectoryLineEdit->setText(name);
    }

}

/**
 * Called when the ok button is clicked
 */
void
SceneFileRestructureDialog::okButtonClicked()
{
    AString errorMessage;
    
    s_previousSceneFileName           = m_sceneFileNameLineEdit->text().trimmed();
    s_previousOutputDirectory         = m_outputDirectoryLineEdit->text().trimmed();
    s_previousPreviewFlag             = m_previewCheckBox->isChecked();
    s_previousSkipMissingFilessFlag   = m_skipMissingFilesCheckBox->isChecked();
    s_previousOvewriteFilesMode       = static_cast<OperationSceneFileRestructure::OverwriteFilesMode>(m_overwriteFilesComboBox->currentData().toInt());
    
    if (s_previousSceneFileName.isEmpty()) {
        errorMessage.appendWithNewLine("Scene File Name is empty.");
    }
    else {
        if ( ! QFile::exists(s_previousSceneFileName)) {
            errorMessage.appendWithNewLine("Scene File with name does not exist.");
        }
    }
    if (s_previousOutputDirectory.isEmpty()) {
        errorMessage.appendWithNewLine("Output Directory is empty.");
    }

    if ( ! errorMessage.isEmpty()) {
        WuQMessageBoxTwo::critical(this,
                                   "Error",
                                   errorMessage);
        return;
    }
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    AString outputMessages;
    try {
        OperationSceneFileRestructure::copySceneFileAndDataFiles(s_previousSceneFileName,
                                                                 s_previousOutputDirectory,
                                                                 OperationSceneFileRestructure::MessageMode::GUI,
                                                                 s_previousOvewriteFilesMode,
                                                                 m_previewCheckBox->isChecked(),
                                                                 m_skipMissingFilesCheckBox->isChecked(),
                                                                 m_testScenesCheckBox->isChecked(),
                                                                 outputMessages);
        m_successFlag = true;
    }
    catch (const OperationException& e) {
        cursor.restoreCursor();
        WuQMessageBoxTwo::critical(this,
                                   "Error",
                                   e.whatString());
    }
    
    cursor.restoreCursor();
    
    if (m_successFlag) {
        WuQTextEditorDialog::runModal("Results",
                                      outputMessages,
                                      WuQTextEditorDialog::TextMode::PLAIN,
                                      WuQTextEditorDialog::WrapMode::YES,
                                      this);
    }
    
    if (m_successFlag) {
        WuQDialogModal::okButtonClicked();
    }
}

/**
 * @return True if reorganization was successful
 */
bool
SceneFileRestructureDialog::isSuccess() const
{
    return m_successFlag;
}


