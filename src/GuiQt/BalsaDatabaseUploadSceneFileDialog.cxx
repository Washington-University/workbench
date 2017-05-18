
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

#define __BALSA_DATABASE_UPLOAD_SCENE_FILE_DIALOG_DECLARE__
#include "BalsaDatabaseUploadSceneFileDialog.h"
#undef __BALSA_DATABASE_UPLOAD_SCENE_FILE_DIALOG_DECLARE__

#include <QComboBox>
#include <QDesktopServices>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "BalsaDatabaseManager.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CursorDisplayScoped.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ProgressReportingDialog.h"
#include "SceneFile.h"
#include "SystemUtilities.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BalsaDatabaseUploadSceneFileDialog 
 * \brief Dialog for uploading a scene file to the BALSA Database.
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
BalsaDatabaseUploadSceneFileDialog::BalsaDatabaseUploadSceneFileDialog(SceneFile* sceneFile,
                                                                       QWidget* parent)
: WuQDialogModal("Upload Scene File to BALSA",
                 parent),
m_sceneFile(sceneFile)
{
    CaretAssert(m_sceneFile);
    
    QString defaultUserName = "balsaTest";
    QString defaultPassword = "@2password";

#ifdef NDEBUG
    defaultUserName = "";
    defaultPassword = "";
#endif
    
    QLabel* databaseNameLabel = new QLabel("DataBase: ");
    m_databaseComboBox = new QComboBox();
    m_databaseComboBox->setEditable(true);
    m_databaseComboBox->addItem("https://balsa.wustl.edu");
    m_databaseComboBox->addItem("http://johnsdev.wustl.edu:8080");
    m_databaseComboBox->addItem("https://johnsdev.wustl.edu:8080");
    
    const int minimumLineEditWidth = 250;
    
    QLabel* usernameLabel = new QLabel("Username: ");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_usernameLineEdit->setText(defaultUserName);
    
    QLabel* passwordLabel = new QLabel("Password: ");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setText(defaultPassword);
    
    QLabel* forgotUsernameLabel = new QLabel("<html>"
                                             "<bold><a href=\"register/forgotUsername\">Forgot Username</a></bold>"
                                             "</html>");
    QObject::connect(forgotUsernameLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QLabel* forgotPasswordLabel = new QLabel("<html>"
                                             "<bold><a href=\"register/forgotPassword\">Forgot Password</a></bold>"
                                             "</html>");
    QObject::connect(forgotPasswordLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QLabel* registerLabel = new QLabel("<html>"
                                       "<bold><a href=\"register/register\">Register</a></bold>"
                                       "</html>");
    QObject::connect(registerLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));

    const QString zipFileName = FileInformation::assembleFileComponents(SystemUtilities::getTempDirectory(),
                                                                        sceneFile->getFileNameNoPathNoExtension() ,
                                                                        "zip");

    QLabel* zipFileNameLabel = new QLabel("Zip File Name: ");
    m_zipFileNameLineEdit = new QLineEdit;
    m_zipFileNameLineEdit->setText(zipFileName);
    
    QLabel* extractDirectoryLabel = new QLabel("Extract to Directory: ");
    m_extractDirectoryNameLineEdit = new QLineEdit();
    m_extractDirectoryNameLineEdit->setText("ExtDir");
    
    const AString defaultDirName(SystemUtilities::getUserName()
                                 + "_"
                                 + m_sceneFile->getBalsaStudyID());
    m_extractDirectoryNameLineEdit->setText(defaultDirName);
    
    
    /*
     * Scene BALSA Study ID
     */
    QLabel* studyIDLabel = new QLabel("BALSA Study ID: ");
    m_fileBalsaStudyIDLineEdit = new QLineEdit();
    m_fileBalsaStudyIDLineEdit->setToolTip("Press Edit button to change Study ID for use with BALSA Database");
    m_fileBalsaStudyIDLineEdit->setReadOnly(true);
    
    /*
     * Edit BALSA Study ID button
     */
    m_editBalsaStudyIDPushButton = new QPushButton("Edit...");
    m_editBalsaStudyIDPushButton->setToolTip("Edit the Scene File's BALSA Study ID");
    QObject::connect(m_editBalsaStudyIDPushButton, SIGNAL(clicked()),
                     this, SLOT(editFileBalsaStudyIDButtonClicked()));
    
    m_getBalsaStudyIDPushButton = new QPushButton("Get...");
    m_getBalsaStudyIDPushButton->setToolTip("Get the BALSA Study ID by sending Study Title to BALSA");
    QObject::connect(m_getBalsaStudyIDPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::getBalsaStudyIDPushButtonClicked);
    
    /*
     * Scene BALSA Study Title
     */
    QLabel* studyTitleLabel = new QLabel("BALSA Study Title: ");
    m_balsaStudyTitleLineEdit = new QLineEdit();
    m_balsaStudyTitleLineEdit->setToolTip("Press Edit button to change Study Title for use with BALSA Database");
    m_balsaStudyTitleLineEdit->setReadOnly(true);
    

    /*
     * Edit BALSA Title button
     */
    m_editBalsaStudyTitlePushButton = new QPushButton("Edit...");
    m_editBalsaStudyTitlePushButton->setToolTip("Edit the Scene File's BALSA Study Title");
    QObject::connect(m_editBalsaStudyTitlePushButton, SIGNAL(clicked()),
                     this, SLOT(editFileBalsaStudyTitleButtonClicked()));
    
    /*
     * Base Directory
     */
    QLabel* baseDirectoryLabel = new QLabel("Base Directory: ");
    m_baseDirectoryLineEdit = new QLineEdit();
    m_baseDirectoryLineEdit->setToolTip("Press Browse or Edit button to change base directory");
    m_baseDirectoryLineEdit->setReadOnly(true);
    m_editBaseDirectoryPushButton = new QPushButton("Edit...");
    m_editBaseDirectoryPushButton->setToolTip("Edit the base directory in a text edit");
    QObject::connect(m_editBaseDirectoryPushButton, SIGNAL(clicked()),
                     this, SLOT(editBaseDirectoryPushButtonClicked()));
    m_browseBaseDirectoryPushButton = new QPushButton("Browse...");
    m_browseBaseDirectoryPushButton->setToolTip("Use a file system dialog to choose the base directory");
    QObject::connect(m_browseBaseDirectoryPushButton, SIGNAL(clicked()),
                     this, SLOT(browseBaseDirectoryPushButtonClicked()));
    
    
    
    
    
    QWidget* dialogWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(dialogWidget);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    int row = 0;
    gridLayout->addWidget(databaseNameLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_databaseComboBox, row, 1);
    gridLayout->addWidget(registerLabel, row, 2);
    row++;
    gridLayout->addWidget(usernameLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_usernameLineEdit, row, 1);
    gridLayout->addWidget(forgotUsernameLabel, row, 2);
    row++;
    gridLayout->addWidget(passwordLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_passwordLineEdit, row, 1);
    gridLayout->addWidget(forgotPasswordLabel, row, 2);
    row++;
    gridLayout->addWidget(zipFileNameLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_zipFileNameLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(extractDirectoryLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_extractDirectoryNameLineEdit, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(baseDirectoryLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_baseDirectoryLineEdit, row, 1);
    gridLayout->addWidget(m_editBaseDirectoryPushButton, row, 2);
    gridLayout->addWidget(m_browseBaseDirectoryPushButton, row, 3);
    row++;
    gridLayout->addWidget(studyTitleLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_balsaStudyTitleLineEdit, row, 1);
    gridLayout->addWidget(m_editBalsaStudyTitlePushButton, row, 2);
    row++;
    gridLayout->addWidget(studyIDLabel, row, 0, Qt::AlignRight);
    gridLayout->addWidget(m_fileBalsaStudyIDLineEdit, row, 1);
    gridLayout->addWidget(m_editBalsaStudyIDPushButton, row, 2);
    gridLayout->addWidget(m_getBalsaStudyIDPushButton, row, 3);
    
    
    setCentralWidget(dialogWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
    
    loadSceneFileMetaDataWidgets();
}

/**
 * Destructor.
 */
BalsaDatabaseUploadSceneFileDialog::~BalsaDatabaseUploadSceneFileDialog()
{
}

/**
 * @return Name of selected database.
 */
AString
BalsaDatabaseUploadSceneFileDialog::getDataBaseURL() const
{
    return m_databaseComboBox->currentText().trimmed();
}


/**
 * Gets called when the user clicks a link in the forgot username
 * or password label.
 *
 * @param linkPath
 *     Path relative to database.
 */
void
BalsaDatabaseUploadSceneFileDialog::labelHtmlLinkClicked(const QString& linkPath)
{
    if (linkPath.isEmpty() == false) {
        const QString dbURL = getDataBaseURL();
        const AString linkURL = (getDataBaseURL() +
                                 (linkPath.startsWith("/") ? "" : "/")
                                 + linkPath);
        QDesktopServices::openUrl(QUrl(linkURL));
    }
}


/**
 * Gets called when the OK button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::okButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    if (m_sceneFile->getBalsaStudyID().trimmed().isEmpty()) {
        const QString msg("The BALSA Study ID is missing.  You must either: (1) Go to "
                          "<a href=\"https://balsa.wustl.edu\">BALSA Database</a> and get a BALSA Study ID or "
                          "(2) Enter a Study Title and press the <B>Get</B> button to request a BALSA Study ID "
                          "using the Study Title");
        WuQMessageBox::errorOk(this, msg);
        return;
    }
    
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

    ProgressReportingDialog progressDialog("Upload Scene File to BALSA",
                                           "",
                                           this);
    
    progressDialog.setCancelButton((QPushButton*)0); // no cancel button
    
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    const AString zipFileName = m_zipFileNameLineEdit->text().trimmed();
    const AString extractToDirectoryName = m_extractDirectoryNameLineEdit->text().trimmed();

    AString errorMessage;
    CaretPointer<BalsaDatabaseManager> balsaDatabaseManager(new BalsaDatabaseManager());
    const bool successFlag = balsaDatabaseManager->uploadZippedSceneFile(getDataBaseURL(),
                                                                         username,
                                                                         password,
                                                                         m_sceneFile,
                                                                         zipFileName,
                                                                         extractToDirectoryName,
                                                                         errorMessage);
    
    cursor.restoreCursor();
    
    progressDialog.setValue(progressDialog.maximum());
    
    if (successFlag) {
        WuQMessageBox::informationOk(this, "Upload was successful");
    }
    else {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    WuQDialogModal::okButtonClicked();    
}

/**
 * Load the Scene File BALSA Study ID Line Edit
 */
void
BalsaDatabaseUploadSceneFileDialog::loadSceneFileMetaDataWidgets()
{
    CaretAssert(m_sceneFile);
    
    m_fileBalsaStudyIDLineEdit->setText(m_sceneFile->getBalsaStudyID());
    m_baseDirectoryLineEdit->setText(m_sceneFile->getBaseDirectory());
    m_balsaStudyTitleLineEdit->setText(m_sceneFile->getBalsaStudyTitle());
}

/**
 * Called when Edit Study ID button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::editFileBalsaStudyIDButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    WuQDataEntryDialog ded("Edit BALSA Database Info",
                           m_editBalsaStudyIDPushButton,
                           WuQDialog::SCROLL_AREA_AS_NEEDED);
    
    QLineEdit* lineEdit = ded.addLineEditWidget("BALSA Study ID");
    lineEdit->setText(m_sceneFile->getBalsaStudyID());
    lineEdit->setMinimumWidth(200);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const AString idText = lineEdit->text().trimmed();
        m_sceneFile->setBalsaStudyID(idText);
        loadSceneFileMetaDataWidgets();
    }
}

/**
 * Called when get BALSA study ID push button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::getBalsaStudyIDPushButtonClicked()
{
    const AString title = m_balsaStudyTitleLineEdit->text().trimmed();
    if (title.isEmpty()) {
        WuQMessageBox::errorOk(m_getBalsaStudyIDPushButton,
                               "The BALSA Study Title is required to get a BALSA Study ID");
        return;
    }
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    const AString zipFileName = m_zipFileNameLineEdit->text().trimmed();
    const AString extractToDirectoryName = m_extractDirectoryNameLineEdit->text().trimmed();
    
    AString studyID;
    AString errorMessage;
    CaretPointer<BalsaDatabaseManager> balsaDatabaseManager(new BalsaDatabaseManager());
    const bool successFlag = balsaDatabaseManager->getStudyIDFromStudyTitle(getDataBaseURL(),
                                                                         username,
                                                                         password,
                                                                         title,
                                                                         studyID,
                                                                         errorMessage);
    
    cursor.restoreCursor();
    
    if (successFlag) {
        m_sceneFile->setBalsaStudyID(studyID);
        loadSceneFileMetaDataWidgets();
    }
    else {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
}

/**
 * Called when Edit Study Title button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::editFileBalsaStudyTitleButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    WuQDataEntryDialog ded("Edit BALSA Database Info",
                           m_editBalsaStudyTitlePushButton,
                           WuQDialog::SCROLL_AREA_AS_NEEDED);
    
    QLineEdit* lineEdit = ded.addLineEditWidget("BALSA Study Title");
    lineEdit->setText(m_sceneFile->getBalsaStudyTitle());
    lineEdit->setMinimumWidth(200);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const AString titleText = lineEdit->text().trimmed();
        m_sceneFile->setBalsaStudyTitle(titleText);
        loadSceneFileMetaDataWidgets();
    }
}
/**
 * Called when upload scene file is selected.
 */
void
BalsaDatabaseUploadSceneFileDialog::editBaseDirectoryPushButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    WuQDataEntryDialog ded("Edit Base Directory",
                           m_editBaseDirectoryPushButton,
                           WuQDialog::SCROLL_AREA_AS_NEEDED);
    
    QLineEdit* lineEdit = ded.addLineEditWidget("Base Directory");
    lineEdit->setText(m_sceneFile->getBaseDirectory());
    lineEdit->setMinimumWidth(500);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const AString idText = lineEdit->text().trimmed();
        m_sceneFile->setBaseDirectory(idText);
        loadSceneFileMetaDataWidgets();
    }
}

/**
 * Called when upload scene file is selected.
 */
void
BalsaDatabaseUploadSceneFileDialog::browseBaseDirectoryPushButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    /*
     * Let user choose directory path
     */
    QString directoryName;
    FileInformation fileInfo(m_sceneFile->getBaseDirectory());
    if (fileInfo.exists()) {
        if (fileInfo.isDirectory()) {
            directoryName = fileInfo.getAbsoluteFilePath();
        }
    }
    AString newDirectoryName = CaretFileDialog::getExistingDirectoryDialog(m_browseBaseDirectoryPushButton,
                                                                           "Choose Base Directory",
                                                                           directoryName);
    /*
     * If user cancels,  return
     */
    if (newDirectoryName.isEmpty()) {
        return;
    }
    
    /*
     * Set name of new scene file and add to brain
     */
    m_sceneFile->setBaseDirectory(newDirectoryName);
    loadSceneFileMetaDataWidgets();
}

