
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
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "BalsaDatabaseManager.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretLogger.h"
#include "CursorDisplayScoped.h"
#include "FileInformation.h"
#include "GuiManager.h"
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
ZipSceneFileDialog::ZipSceneFileDialog(const SceneFile* sceneFile,
                                       QWidget* parent)
: WuQDialogModal("Zip Scene File",
                 parent),
m_sceneFile(sceneFile)
{
    
    const int minimumLineEditWidth = 400;
    
    AString extractToDirectoryName("ext_dir");
    AString zipFileName("file.zip");
    FileInformation fileInfo(sceneFile->getFileName());
    AString sceneFileDirectory = fileInfo.getAbsolutePath();
    if ( ! sceneFileDirectory.isEmpty()) {
        QDir dir(sceneFileDirectory);
        if (dir.exists()) {
            const AString dirName = dir.dirName();
            if ( ! dirName.isEmpty()) {
                if (dirName != ".") {
                    extractToDirectoryName = dirName;
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

    QLabel* zipFileNameLabel = new QLabel("Zip File Name");
    m_zipFileNameLineEdit = new QLineEdit;
    m_zipFileNameLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_zipFileNameLineEdit->setText(zipFileName);
    
    QPushButton* chooseZipFileButton = new QPushButton("Choose...");
    QObject::connect(chooseZipFileButton, SIGNAL(clicked()),
                     this, SLOT(chooseZipFileButtonClicked()));
    
    QLabel* extractDirectoryLabel = new QLabel("Extract to Directory");
    m_extractDirectoryNameLineEdit = new QLineEdit();
    m_extractDirectoryNameLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_extractDirectoryNameLineEdit->setText(extractToDirectoryName);
    
    
    QWidget* dialogWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(dialogWidget);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    int row = 0;
    gridLayout->addWidget(zipFileNameLabel, row, 0);
    gridLayout->addWidget(m_zipFileNameLineEdit, row, 1);
    gridLayout->addWidget(chooseZipFileButton, row, 2);
    row++;
    gridLayout->addWidget(extractDirectoryLabel, row, 0);
    gridLayout->addWidget(m_extractDirectoryNameLineEdit, row, 1);
    row++;
    
    
    setCentralWidget(dialogWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
ZipSceneFileDialog::~ZipSceneFileDialog()
{
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
    if (extractToDirectoryName.isEmpty()) {
        errorMessage.appendWithNewLine("Extract to directory is missing.");
    }
    
    if (errorMessage.isEmpty()) {
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

