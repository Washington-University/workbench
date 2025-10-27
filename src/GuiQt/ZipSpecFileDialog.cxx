
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

#define __ZIP_SPEC_FILE_DIALOG_DECLARE__
#include "ZipSpecFileDialog.h"
#undef __ZIP_SPEC_FILE_DIALOG_DECLARE__

#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "DataFileTypeEnum.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "OperationException.h"
#include "OperationZipSpecFile.h"
#include "ProgressReportingDialog.h"
#include "WuQMessageBoxTwo.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ZipSpecFileDialog 
 * \brief Zip the spec file dialog
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param specFileName
 *    Name of spec file
 * @param parent
 *    Parent widget
 */
ZipSpecFileDialog::ZipSpecFileDialog(const AString& specFileName,
                                     QWidget* parent)
: WuQDialogModal("Zip Spec File",
                 parent)
{
    QLabel* specFileLabel(new QLabel("Spec File: "));
    m_specFileNameLineEdit = new QLineEdit();
    QPushButton* specFilePushButton(new QPushButton("Choose..."));
    QObject::connect(specFilePushButton, &QPushButton::clicked,
                     this, &ZipSpecFileDialog::specFileNamePushButtonClicked);
    
    const QString extractToolTip("the name of the folder created when the zip file is unzipped");
    QLabel* extractDirectoryLabel(new QLabel("Extract Directory: "));
    m_extractDirectoryLineEdit = new QLineEdit();
    WuQtUtilities::setWordWrappedToolTip(m_extractDirectoryLineEdit,
                                         extractToolTip);
    
    QLabel* zipFileLabel(new QLabel("Zip File: "));
    m_zipFileNameLineEdit = new QLineEdit();
    QPushButton* zipFilePushButton(new QPushButton("Choose..."));
    QObject::connect(zipFilePushButton, &QPushButton::clicked,
                     this, &ZipSpecFileDialog::zipFileNamePushButtonClicked);
    m_zipFileNameLineEdit->setToolTip("the zip file that will be created");
    
    const AString baseDirToolTip("specify a directory that all data files are somewhere "
                                 "within, this will become the root of the zipfile's "
                                 "directory structure");
    m_baseDirectoryCheckBox = new QCheckBox("Base Directory");
    m_baseDirectoryLineEdit = new QLineEdit();
    QPushButton* baseDirectoryPushButton(new QPushButton("Choose..."));
    QObject::connect(baseDirectoryPushButton, &QPushButton::clicked,
                     this, &ZipSpecFileDialog::baseDirectoryNamePushButtonClicked);
    WuQtUtilities::setWordWrappedToolTip(m_baseDirectoryLineEdit,
                                         baseDirToolTip);

    const QString missingToolTip("any missing files will generate only warnings, and the "
                                 "zip file will be created anyway");
    m_skipMissingFilesCheckBox = new QCheckBox("Skip Missing Files");
    WuQtUtilities::setWordWrappedToolTip(m_skipMissingFilesCheckBox,
                                         missingToolTip);
    
    QWidget* widget(new QWidget());
    QGridLayout* layout(new QGridLayout(widget));
    layout->setColumnStretch(1, 100);
    int row(layout->rowCount());
    layout->addWidget(specFileLabel,
                      row, 0);
    layout->addWidget(m_specFileNameLineEdit,
                      row, 1);
    layout->addWidget(specFilePushButton,
                      row, 2);
    row++;
    layout->addWidget(extractDirectoryLabel,
                      row, 0);
    layout->addWidget(m_extractDirectoryLineEdit,
                      row, 1);
    row++;
    layout->addWidget(zipFileLabel,
                      row, 0);
    layout->addWidget(m_zipFileNameLineEdit,
                      row, 1);
    layout->addWidget(zipFilePushButton,
                      row, 2);
    row++;
    
    layout->addWidget(m_baseDirectoryCheckBox,
                      row, 0);
    layout->addWidget(m_baseDirectoryLineEdit,
                      row, 1);
    layout->addWidget(baseDirectoryPushButton,
                      row, 2);
    row++;
    
    layout->addWidget(m_skipMissingFilesCheckBox,
                      row, 0,
                      1, 3, Qt::AlignLeft);
    row++;
    
    setCentralWidget(widget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    setMinimumWidth(500);
    
    initializeWithSpecFile(specFileName);
}

/**
 * Destructor.
 */
ZipSpecFileDialog::~ZipSpecFileDialog()
{
}

void
ZipSpecFileDialog::initializeWithSpecFile(const AString& specFileName)
{
    if (specFileName.isEmpty()) {
        m_directory = GuiManager::get()->getBrain()->getCurrentDirectory();
        if (m_directory.isEmpty()) {
            m_directory = QDir::currentPath();
        }
        return;
    }
    
    const FileInformation fileInfo(specFileName.trimmed());
    const AString parentDirName(fileInfo.getAbsolutePath());
    FileInformation dirInfo(parentDirName);
    
    m_specFileNameLineEdit->setText(specFileName.trimmed());
    m_extractDirectoryLineEdit->setText(dirInfo.getFileName());
    
    const AString zipFileName(FileInformation::assembleFileComponents(parentDirName,
                                                                      fileInfo.getFileNameNoExtension(),
                                                                      ".zip"));
    m_zipFileNameLineEdit->setText(zipFileName);
    
    m_directory = parentDirName;
}

/**
 * Called to select spec file
 */
void
ZipSpecFileDialog::specFileNamePushButtonClicked()
{
    const AString name(CaretFileDialog::getOpenFileNameDialog(this,
                                                              "Choose Spec File",
                                                              m_directory,
                                                              DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::SPECIFICATION)));
    if ( ! name.isEmpty()) {
        initializeWithSpecFile(name);
    }
}

/**
 * Called to select zip file
 */
void
ZipSpecFileDialog::zipFileNamePushButtonClicked()
{
    const AString name(CaretFileDialog::getOpenFileNameDialog(this,
                                                              "Choose Zip File",
                                                              m_directory,
                                                              "Zip File (*.zip)"));
    if ( ! name.isEmpty()) {
        m_zipFileNameLineEdit->setText(name);
    }
}

/**
 * Called to select base directory
 */
void
ZipSpecFileDialog::baseDirectoryNamePushButtonClicked()
{
    const AString dirName(m_baseDirectoryLineEdit->text().trimmed().isEmpty()
                          ? m_directory
                          : m_baseDirectoryLineEdit->text().trimmed());
    const AString name(CaretFileDialog::getExistingDirectoryDialog(this,
                                                                   "Choose Base Directory",
                                                                   dirName));
    if ( ! name.isEmpty()) {
        m_baseDirectoryLineEdit->setText(name);
    }
}

/**
 * Called when the ok button is clicked
 */
void
ZipSpecFileDialog::okButtonClicked()
{
    AString errorMessage;
    
    const AString specFileName(m_specFileNameLineEdit->text().trimmed());
    const AString zipFileName(m_zipFileNameLineEdit->text().trimmed());
    const AString extractDirectory(m_extractDirectoryLineEdit->text().trimmed());
    const AString baseDirText(m_baseDirectoryLineEdit->text().trimmed());
    
    if (specFileName.isEmpty()) {
        errorMessage.appendWithNewLine("Spec File Name is empty.");
    }
    else {
        if ( ! QFile::exists(specFileName)) {
            errorMessage.appendWithNewLine("Spec File with name does not exist.");
        }
    }
    
    if (extractDirectory.isEmpty()) {
        errorMessage.appendWithNewLine("Extract Directory is empty.");
    }
    
    if (m_baseDirectoryCheckBox->isChecked()
        && (baseDirText.isEmpty())) {
        errorMessage.appendWithNewLine("Base directory option is checked but base directory is empty.");
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBoxTwo::critical(this,
                                   "Error",
                                   errorMessage);
        return;
    }
    
    if (QFile::exists(zipFileName)) {
        const WuQMessageBoxTwo::StandardButton button =
        WuQMessageBoxTwo::warning(this,
                                  "Warning",
                                  "Overwrite existing Zip File?",
                                  (static_cast<int32_t>(WuQMessageBoxTwo::StandardButton::Ok)
                                   | static_cast<int32_t>(WuQMessageBoxTwo::StandardButton::Cancel)),
                                  WuQMessageBoxTwo::StandardButton::Cancel);
        if (button == WuQMessageBoxTwo::StandardButton::Cancel) {
            return;
        }
    }
    
    AString myBaseDir;
    if (m_baseDirectoryCheckBox->isChecked()) {
        myBaseDir = QDir::cleanPath(QDir(baseDirText).absolutePath());
    } else {
        FileInformation specFileInfo(specFileName);
        myBaseDir = QDir::cleanPath(specFileInfo.getAbsolutePath());
    }

    const bool skipMissingFilesFlag(m_skipMissingFilesCheckBox->isChecked());
    
    ProgressReportingDialog prd("Zip Spec File",
                                "Zipping started",
                                this);
    //prd.show();
    ProgressObject* myProgObj(NULL);
    try {
        OperationZipSpecFile::createZipFile(myProgObj,
                                            specFileName,
                                            FileInformation(specFileName).getAbsoluteFilePath(),
                                            extractDirectory,
                                            FileInformation(zipFileName).getAbsoluteFilePath(),
                                            QDir::cleanPath(QDir(myBaseDir).absolutePath()),
                                            OperationZipSpecFile::PROGRESS_GUI_EVENT,
                                            skipMissingFilesFlag);
    }
    catch (const OperationException& e) {
        prd.close();
        WuQMessageBoxTwo::critical(this,
                                   "Error",
                                   e.whatString());
    }
    WuQDialogModal::okButtonClicked();
}
