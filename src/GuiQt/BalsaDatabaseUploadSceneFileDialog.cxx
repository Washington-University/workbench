
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

#include "BalsaDatabaseManager.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CursorDisplayScoped.h"
#include "FileInformation.h"
#include "ProgressReportingDialog.h"
#include "SceneFile.h"
#include "SystemUtilities.h"
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
BalsaDatabaseUploadSceneFileDialog::BalsaDatabaseUploadSceneFileDialog(const SceneFile* sceneFile,
                                                                       QWidget* parent)
: WuQDialogModal("Upload Scene File to BALSA",
                 parent),
m_sceneFile(sceneFile)
{
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

    QLabel* zipFileNameLabel = new QLabel("Zip File Name");
    m_zipFileNameLineEdit = new QLineEdit;
    m_zipFileNameLineEdit->setText(zipFileName);
    
    QLabel* extractDirectoryLabel = new QLabel("Extract to Directory");
    m_extractDirectoryNameLineEdit = new QLineEdit();
    m_extractDirectoryNameLineEdit->setText("ExtDir");
    
    const AString defaultDirName(SystemUtilities::getUserName()
                                 + "_"
                                 + m_sceneFile->getBalsaStudyID());
    m_extractDirectoryNameLineEdit->setText(defaultDirName);
    
    QWidget* dialogWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(dialogWidget);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    int row = 0;
    gridLayout->addWidget(databaseNameLabel, row, 0);
    gridLayout->addWidget(m_databaseComboBox, row, 1);
    gridLayout->addWidget(registerLabel, row, 2);
    row++;
    gridLayout->addWidget(usernameLabel, row, 0);
    gridLayout->addWidget(m_usernameLineEdit, row, 1);
    gridLayout->addWidget(forgotUsernameLabel, row, 2);
    row++;
    gridLayout->addWidget(passwordLabel, row, 0);
    gridLayout->addWidget(m_passwordLineEdit, row, 1);
    gridLayout->addWidget(forgotPasswordLabel, row, 2);
    row++;
    gridLayout->addWidget(zipFileNameLabel, row, 0);
    gridLayout->addWidget(m_zipFileNameLineEdit, row, 1, 1, 2);
    row++;
    gridLayout->addWidget(extractDirectoryLabel, row, 0);
    gridLayout->addWidget(m_extractDirectoryNameLineEdit, row, 1, 1, 2);
    row++;
    
    
    setCentralWidget(dialogWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
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

