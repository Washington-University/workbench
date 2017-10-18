
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __ZIP_SCENE_FILE_DIALOG_DECLARE__
#include "ZipSceneFileDialog.h"
#undef __ZIP_SCENE_FILE_DIALOG_DECLARE__

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>

#include "BalsaDatabaseManager.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretLogger.h"
#include "CursorDisplayScoped.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "SceneBasePathWidget.h"
#include "SceneFile.h"
#include "SystemUtilities.h"
#include "WuQMessageBox.h"

using namespace caret;

/**
 * \class caret::ZipSceneFileDialog 
 * \brief Dialog for zipping a scene file.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param sceneFile
 *     Scene file that will be uploaded.
 * @param parent
 *     Parent of this dialog.
 */
ZipSceneFileDialog::ZipSceneFileDialog(SceneFile* sceneFile,
                                       QWidget* parent)
: WuQDialogModal("Zip Scene File",
                 parent),
m_sceneFile(sceneFile)
{
    
    const int minimumLineEditWidth = 400;
    
    AString defaultExtractToDirectoryName("ext_dir");
    AString zipFileName("file.zip");
    FileInformation fileInfo(sceneFile->getFileName());
    AString sceneFileDirectory = fileInfo.getAbsolutePath();
    if ( ! sceneFileDirectory.isEmpty()) {
        QDir dir(sceneFileDirectory);
        if (dir.exists()) {
            const AString dirName = dir.dirName();
            if ( ! dirName.isEmpty()) {
                if (dirName != ".") {
                    defaultExtractToDirectoryName = dirName;
                }
            }
        }
    }
    else {
        sceneFileDirectory = GuiManager::get()->getBrain()->getCurrentDirectory();
    }
    
    if ( ! sceneFileDirectory.isEmpty()) {
        zipFileName = FileInformation::assembleFileComponents(sceneFileDirectory,
                                                              sceneFile->getFileNameNoPathNoExtension(),
                                                              "zip");
    }

    /*
     * ZIP file
     */
    m_zipFileNameLabel = new QLabel("Zip File Name");
    m_zipFileNameLineEdit = new QLineEdit;
    m_zipFileNameLineEdit->setToolTip("Name of the ZIP file");
    m_zipFileNameLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_zipFileNameLineEdit->setText(zipFileName);
    m_zipFileNameLineEdit->setValidator(createValidator(LabelName::ZIP_FILE));
    QObject::connect(m_zipFileNameLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateData(); });
    
    /*
     * Zip file button
     */
    QPushButton* chooseZipFileButton = new QPushButton("Browse...");
    QObject::connect(chooseZipFileButton, SIGNAL(clicked()),
                     this, SLOT(chooseZipFileButtonClicked()));
    
    /*
     * Extract to directory
     */
    m_extractDirectoryLabel = new QLabel("Extract to Directory");
    m_extractDirectoryNameLineEdit = new QLineEdit();
    m_extractDirectoryNameLineEdit->setToolTip("Directory that is created when user unzips the ZIP file");
    m_extractDirectoryNameLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_extractDirectoryNameLineEdit->setValidator(createValidator(LabelName::EXTRACT_DIRECTORY));
    QObject::connect(m_extractDirectoryNameLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateData(); });
    
    /*
     * Base Directory
     */
    m_basePathWidget = new SceneBasePathWidget();
    
    QGroupBox* zipFileGroupBox = new QGroupBox("Zip File");
    QGridLayout* zipFileLayout = new QGridLayout(zipFileGroupBox);
    zipFileLayout->setSpacing(2);
    zipFileLayout->setColumnStretch(0, 0);
    zipFileLayout->setColumnStretch(1, 100);
    zipFileLayout->setColumnStretch(2, 0);
    int row = 0;
    zipFileLayout->addWidget(m_zipFileNameLabel, row, 0);
    zipFileLayout->addWidget(m_zipFileNameLineEdit, row, 1);
    zipFileLayout->addWidget(chooseZipFileButton, row, 2);
    row++;
    zipFileLayout->addWidget(m_extractDirectoryLabel, row, 0);
    zipFileLayout->addWidget(m_extractDirectoryNameLineEdit, row, 1);
    row++;

    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(zipFileGroupBox);
    dialogLayout->addWidget(m_basePathWidget);
    row++;
    
    AString extractDirectory = m_sceneFile->getBalsaExtractToDirectoryName();
    if (extractDirectory.isEmpty()) {
        extractDirectory = m_sceneFile->getDefaultExtractToDirectoryName();
    }
    m_extractDirectoryNameLineEdit->setText(extractDirectory);
    
    setCentralWidget(dialogWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
    
    m_basePathWidget->updateWithSceneFile(m_sceneFile);
    
    validateData();
}

/**
 * Destructor.
 */
ZipSceneFileDialog::~ZipSceneFileDialog()
{
}

void
ZipSceneFileDialog::validateData()
{
    setLabelText(LabelName::EXTRACT_DIRECTORY);
    setLabelText(LabelName::ZIP_FILE);
}

/**
 * Choose the zip file name with a file browser dialog.
 */
void
ZipSceneFileDialog::chooseZipFileButtonClicked()
{
    /*
     * Let user choose a different path/name
     */
    AString newZipFileName = CaretFileDialog::getSaveFileNameDialog(this,
                                                                    "Choose Zip File Name",
                                                                    m_zipFileNameLineEdit->text().trimmed(),
                                                                    "Zip File (*.zip)");
    /*
     * If user cancels, delete the new scene file and return
     */
    if (newZipFileName.isEmpty()) {
        return;
    }

    m_zipFileNameLineEdit->setText(newZipFileName);
}

/**
 * Gets called when the OK button is clicked.
 */
void
ZipSceneFileDialog::okButtonClicked()
{
    const AString zipFileName = m_zipFileNameLineEdit->text().trimmed();
    const AString extractToDirectoryName = m_extractDirectoryNameLineEdit->text().trimmed();

    AString errorMessage;
    
    if (zipFileName.isEmpty()) {
        errorMessage.appendWithNewLine("Zip file name is missing");
    }
    if ( ! m_extractDirectoryNameLineEdit->hasAcceptableInput()) {
        errorMessage.appendWithNewLine("Extract to Directory is invalid.<p>");
    }
    AString basePathErrorMessage;
    if ( ! m_basePathWidget->isValid(basePathErrorMessage)) {
        errorMessage.appendWithNewLine(basePathErrorMessage);
    }
    
    if (errorMessage.isEmpty()) {
        m_sceneFile->setBalsaExtractToDirectoryName(extractToDirectoryName);
        
        if (m_sceneFile->isModified()) {
            const QString msg("The scene file is modified and must be saved before continuing.  Would you like "
                              "to save the scene file using its current name and continue?");
            if (WuQMessageBox::warningYesNo(this, msg)) {
                try {
                    Brain* brain = GuiManager::get()->getBrain();
                    brain->writeDataFile(m_sceneFile);
                }
                catch (const DataFileException& e) {
                    WuQMessageBox::errorOk(this, e.whatString());
                    return;
                }
            }
            else {
                return;
            }
        }

        
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        const bool successFlag = BalsaDatabaseManager::zipSceneAndDataFiles(m_sceneFile,
                                                                            extractToDirectoryName,
                                                                            zipFileName,
                                                                            errorMessage);
        
        if ( ! successFlag) {
            if (errorMessage.isEmpty()) {
                errorMessage = "Zipping scene file failed with unknown error.";
            }
        }
        
        cursor.restoreCursor();
    }
    
    if (errorMessage.isEmpty()) {
        WuQMessageBox::informationOk(this, "Zip file was successfully created");
    }
    else {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    WuQDialogModal::okButtonClicked();    
}

/**
 * Create a regular expression validatory for the give label/data.
 *
 * @param labelName
 *     'Name' of label.
 */
QRegularExpressionValidator*
ZipSceneFileDialog::createValidator(const LabelName labelName)
{
    QRegularExpression regEx;
    
    switch (labelName) {
        case LabelName::EXTRACT_DIRECTORY:
            regEx.setPattern(".+");
            break;
        case LabelName::ZIP_FILE:
            regEx.setPattern(".+\\.zip$");
            break;
    }
    CaretAssert(regEx.isValid());
    
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(regEx,
                                                                             this);
    return validator;
}

/**
 * Set the label's text.
 *
 * @param label
 *     The label
 * @param labelName
 *     'Name' of the label
 */
void
ZipSceneFileDialog::setLabelText(const LabelName labelName)
{
    QLabel* label = NULL;
    AString labelText;
    bool validFlag = false;
    switch (labelName) {
        case LabelName::EXTRACT_DIRECTORY:
            label = m_extractDirectoryLabel;
            labelText = "Extract to Directory";
            validFlag = m_extractDirectoryNameLineEdit->hasAcceptableInput();
            break;
        case LabelName::ZIP_FILE:
            label = m_zipFileNameLabel;
            labelText = "Zip File Name";
            validFlag = m_zipFileNameLineEdit->hasAcceptableInput();
            break;
    }
    
    const bool textRedIfInvalid = true;
    
    AString coloredText;
    if (validFlag) {
        if (textRedIfInvalid) {
            coloredText = (labelText
                           + ": ");
        }
        else {
            coloredText = (" "
                           + labelText
                           + ": ");
        }
    }
    else {
        if (textRedIfInvalid) {
            coloredText = ("<html><font color=red>"
                           + labelText
                           + "</font>: </html>");
        }
        else {
            coloredText = ("<html>"
                           + labelText
                           + "<font color=red>*</font>"
                           ": </html>");
        }
    }
    
    CaretAssert(label);
    label->setText(coloredText);
}

