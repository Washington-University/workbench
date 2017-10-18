
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

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTabWidget>

#include "BalsaDatabaseManager.h"
#include "BalsaStudySelectionDialog.h"
#include "BalsaUserRoles.h"
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
#include "SceneBasePathWidget.h"
#include "SceneFile.h"
#include "SystemUtilities.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQWidgetDisabler.h"

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
    
    m_userRoles.reset(new BalsaUserRoles);
    
    QWidget* loginWidget = createLoginWidget();
    m_uploadWidget = createUploadWidget();
    m_uploadPushButton->setEnabled(false);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(loginWidget);
    dialogLayout->addWidget(m_uploadWidget);
    
    setCentralWidget(dialogWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
    
    loginInformationChanged();
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
    
    setOkButtonText("");
    setCancelButtonText("Close");
    
    m_basePathWidget->updateSceneFile(m_sceneFile);
}

/**
 * Destructor.
 */
BalsaDatabaseUploadSceneFileDialog::~BalsaDatabaseUploadSceneFileDialog()
{
}

/**
 * Called when the Cancel button (we have changed text
 * to Close) is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::cancelButtonClicked()
{
    /*
     * Will logout of database and disable part of dialog
     */
    loginInformationChanged();
    
    WuQDialogModal::cancelButtonClicked();
}


/**
 * @return The login widget
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createLoginWidget()
{
    AString defaultUserName;
    AString defaultPassword;
    
    /*
     * Create the BALSA database manager
     */
    m_balsaDatabaseManager = std::unique_ptr<BalsaDatabaseManager>(new BalsaDatabaseManager());
    
    /*
     * Database selection
     */
    m_databaseLabel = new QLabel("");
    m_databaseComboBox = new QComboBox();
    m_databaseComboBox->setEditable(true);
    m_databaseComboBox->addItem("https://balsa.wustl.edu");
    m_databaseComboBox->addItem("http://johnsdev.wustl.edu:8080");
    m_databaseComboBox->addItem("https://johnsdev.wustl.edu:8080");
    QObject::connect(m_databaseComboBox, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::activated),
                     this, [=] { this->loginInformationChanged(); });
    QObject::connect(m_databaseComboBox, &QComboBox::editTextChanged,
                     this, [=] { this->loginInformationChanged(); });
    
    const int minimumLineEditWidth = 250;
    
    /*
     * Username
     */
    m_usernameLabel = new QLabel("");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_usernameLineEdit->setText(defaultUserName);
    m_usernameLineEdit->setValidator(createValidator(LabelName::LABEL_USERNAME));
    QObject::connect(m_usernameLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->loginInformationChanged(); });
    
    /*
     * Password
     */
    m_passwordLabel = new QLabel("");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setText(defaultPassword);
    m_passwordLineEdit->setValidator(createValidator(LabelName::LABEL_PASSWORD));
    QObject::connect(m_passwordLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->loginInformationChanged(); });
    
    /*
     * Forgot username label/link
     */
    QLabel* forgotUsernameLabel = new QLabel("<html>"
                                             "<bold><a href=\"register/forgotUsername\">Forgot Username</a></bold>"
                                             "</html>");
    QObject::connect(forgotUsernameLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    /*
     * Forgot password label/link
     */
    QLabel* forgotPasswordLabel = new QLabel("<html>"
                                             "<bold><a href=\"register/forgotPassword\">Forgot Password</a></bold>"
                                             "</html>");
    QObject::connect(forgotPasswordLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    /*
     * Register label/link
     */
    QLabel* registerLabel = new QLabel("<html>"
                                       "<bold><a href=\"register/register\">Register</a></bold>"
                                       "</html>");
    QObject::connect(registerLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    /*
     * Login push button
     */
    m_loginPushButton = new QPushButton("Login");
    QObject::connect(m_loginPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::loginButtonClicked);
    
    int columnCounter = 0;
    const int COLUMN_LABEL = columnCounter++;
    const int COLUMN_DATA_WIDGET = columnCounter++;
    const int COLUMN_BUTTON_ONE = columnCounter++;
    const int COLUMN_BUTTON_TWO = columnCounter++;
    
    QGroupBox* groupBox = new QGroupBox("Database Login");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(2);
    gridLayout->setColumnStretch(COLUMN_LABEL, 0);
    gridLayout->setColumnStretch(COLUMN_DATA_WIDGET, 100);
    gridLayout->setColumnStretch(COLUMN_BUTTON_ONE, 0);
    gridLayout->setColumnStretch(COLUMN_BUTTON_TWO, 0);
    int row = 0;
    gridLayout->addWidget(m_databaseLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_databaseComboBox, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(registerLabel, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_usernameLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_usernameLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(forgotUsernameLabel, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_passwordLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_passwordLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(forgotPasswordLabel, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->setRowMinimumHeight(row, 10); // empty row
    row++;
    gridLayout->addWidget(m_loginPushButton, row, COLUMN_DATA_WIDGET, 1, 1, Qt::AlignLeft);
    
    return groupBox;
}

/**
 * Called when login information (database, username, password) is changed
 */
void
BalsaDatabaseUploadSceneFileDialog::loginInformationChanged()
{
    m_balsaDatabaseManager->logout();
    m_uploadPushButton->setEnabled(false);
    m_userRoles->resetToAllInvalid();
    updateAllLabels();
}

/**
 * @return The upload widget
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createUploadWidget()
{
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createUploadTab(), "Upload");
    tabWidget->addTab(createAdvancedTab(), "Advanced");
    
    return tabWidget;
}


/**
 * @return New instance of the upload tab.
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createUploadTab()
{
    /*
     * Extract to directory
     */
    const AString defaultExtractDirectoryName = m_sceneFile->getDefaultExtractToDirectoryName();
    m_extractDirectoryNameLabel = new QLabel("");
    m_extractDirectoryNameLineEdit = new QLineEdit();
    m_extractDirectoryNameLineEdit->setText("ExtDir");
    m_extractDirectoryNameLineEdit->setValidator(createValidator(LabelName::LABEL_EXTRACT_DIRECTORY));
    m_extractDirectoryNameLineEdit->setToolTip("Directory that is created when user unzips the ZIP file");
    m_extractDirectoryNameLineEdit->setText(defaultExtractDirectoryName);
    QObject::connect(m_extractDirectoryNameLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateUploadData(); });
    
    
    /*
     * Scene BALSA Study ID
     */
    m_balsaStudyIDLabel = new QLabel("");
    m_balsaStudyIDLineEdit = new QLineEdit();
    m_balsaStudyIDLineEdit->setReadOnly(true);
    m_balsaStudyIDLineEdit->setToolTip("The Study ID is available from the BALSA Database; click the Get button to get a Study ID");
    m_balsaStudyIDLineEdit->setValidator(createValidator(LabelName::LABEL_STUDY_ID));
    m_balsaStudyIDLineEdit->setReadOnly(true);
    QObject::connect(m_balsaStudyIDLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateUploadData(); });
    
    /*
     * Scene BALSA Study Title
     */
    m_balsaStudyTitleLabel = new QLabel("");
    m_balsaStudyTitleLineEdit = new QLineEdit();
    m_balsaStudyTitleLineEdit->setToolTip("Title for the study");
    m_balsaStudyTitleLineEdit->setValidator(createValidator(LabelName::LABEL_STUDY_TITLE));
    m_balsaStudyTitleLineEdit->setReadOnly(true);
    QObject::connect(m_balsaStudyTitleLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateUploadData(); });
    
    /**
     * Select Study Push Button
     */
    m_selectStudyTitlePushButton = new QPushButton("Add/Choose/Rename...");
    m_selectStudyTitlePushButton->setToolTip("Add new study, choose a study, or edit a study's title in BALSA Database");
    QObject::connect(m_selectStudyTitlePushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::selectStudyTitleButtonClicked);
    
    /*
     * Roles button
     */
    QPushButton* rolesPushButton = new QPushButton("Test Roles...");
    rolesPushButton->setToolTip("Test getting the user's roles");
    QObject::connect(rolesPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::rolesButtonClicked);
    rolesPushButton->setVisible(false);

    /*
     * Auto-save checkbox
     */
    m_autoSaveSceneFileCheckBox = new QCheckBox("Auto-Save Scene File");
    m_autoSaveSceneFileCheckBox->setChecked(true);
    QObject::connect(m_autoSaveSceneFileCheckBox, &QCheckBox::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::autoSaveCheckBoxClicked);
    
    /*
     * Upload push button
     */
    m_uploadPushButton = new QPushButton("Upload");
    QObject::connect(m_uploadPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::uploadButtonClicked);
    
    
    int columnCounter = 0;
    const int COLUMN_LABEL = columnCounter++;
    const int COLUMN_DATA_WIDGET = columnCounter++;
    const int COLUMN_BUTTON_ONE = columnCounter++;
    const int COLUMN_BUTTON_TWO = columnCounter++;
    
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    gridLayout->setSpacing(2);
    gridLayout->setColumnMinimumWidth(COLUMN_DATA_WIDGET, 300);
    gridLayout->setColumnStretch(COLUMN_LABEL, 0);
    gridLayout->setColumnStretch(COLUMN_DATA_WIDGET, 100);
    gridLayout->setColumnStretch(COLUMN_BUTTON_ONE, 0);
    gridLayout->setColumnStretch(COLUMN_BUTTON_TWO, 0);
    int row = 0;
    gridLayout->addWidget(m_extractDirectoryNameLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_extractDirectoryNameLineEdit, row, COLUMN_DATA_WIDGET);
    row++;
    gridLayout->addWidget(m_balsaStudyTitleLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_balsaStudyTitleLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(m_selectStudyTitlePushButton, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_balsaStudyIDLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_balsaStudyIDLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(rolesPushButton, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_autoSaveSceneFileCheckBox, row, COLUMN_DATA_WIDGET, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->setRowMinimumHeight(row, 10); // empty row
    row++;
    gridLayout->addWidget(m_uploadPushButton, row, COLUMN_DATA_WIDGET, 1, 1, Qt::AlignLeft);
    row++;
    
    m_balsaStudyIDLineEdit->setText(m_sceneFile->getBalsaStudyID());
//    AString baseDirectory = m_sceneFile->getBalsaBaseDirectory();
//    if (baseDirectory.isEmpty()) {
//        baseDirectory = m_sceneFile->findBaseDirectoryForDataFiles();
//    }
    m_balsaStudyTitleLineEdit->setText(m_sceneFile->getBalsaStudyTitle());
    m_extractDirectoryNameLineEdit->setText(m_sceneFile->getBalsaExtractToDirectoryName());
    
    if (m_extractDirectoryNameLineEdit->text().trimmed().isEmpty()) {
        m_extractDirectoryNameLineEdit->setText(m_sceneFile->getDefaultExtractToDirectoryName());
    }
    
    return widget;
}

/**
 * @return New instance of the advanced tab
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createAdvancedTab()
{
    m_zipFileTemporaryDirectoryRadioButton = new QRadioButton("Use System Temporary Directory");
    m_zipFileCustomDirectoryRadioButton = new QRadioButton("Custom: ");
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(m_zipFileTemporaryDirectoryRadioButton);
    buttGroup->addButton(m_zipFileCustomDirectoryRadioButton);
    m_zipFileTemporaryDirectoryRadioButton->setChecked(true); // do before signal
    QObject::connect(buttGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
                     [=](int id){ this->zipFileDirectoryRadioButtonClicked(id); });
    
    m_zipFileCustomDirectoryLineEdit = new QLineEdit();

    QPushButton* browseCustomDirectoryPushButton = new QPushButton("Browse...");
    QObject::connect(browseCustomDirectoryPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::browseZipFileCustomDirectoryPushButtonClicked);
    
    QGroupBox* zipDirectoryGroupBox = new QGroupBox("Zip File Directory");
    QGridLayout* gridLayout = new QGridLayout(zipDirectoryGroupBox);
    gridLayout->setSpacing(2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    int row = 0;
    gridLayout->addWidget(m_zipFileTemporaryDirectoryRadioButton, 0, 0, 1, 3);
    gridLayout->addWidget(m_zipFileCustomDirectoryRadioButton, 1, 0);
    gridLayout->addWidget(m_zipFileCustomDirectoryLineEdit, 1, 1);
    gridLayout->addWidget(browseCustomDirectoryPushButton, 1, 2);
    row++;
    
    m_basePathWidget = new SceneBasePathWidget();

    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(zipDirectoryGroupBox);
    //layout->addWidget(m_basePathWidget, 0, Qt::AlignLeft);
    layout->addWidget(m_basePathWidget);
    layout->addStretch();
    
    return widget;
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
 * Called when a Zip File Directory radio button is clicked
 */
void
BalsaDatabaseUploadSceneFileDialog::zipFileDirectoryRadioButtonClicked(int)
{
    validateUploadData();
}

/**
 * Get the ZIP file name.
 *
 * @param errorMessageOut
 *    Output if failure to create zip file name.
 *
 * @return
 *    Name of ZIP file.  If empty, check errorMessageOut for explanation.
 */
AString
BalsaDatabaseUploadSceneFileDialog::getZipFileNameWithPath(AString& errorMessageOut) const
{
    errorMessageOut = "";
    
    AString directoryName;
    if (m_zipFileTemporaryDirectoryRadioButton->isChecked()) {
        directoryName = SystemUtilities::getTempDirectory();
        if ( ! FileInformation(directoryName).exists()) {
            errorMessageOut = "Default temporary directory is invalid.  Please choose a Custom Zip File Directory on Advanced tab";
            return "";
        }
    }
    else if (m_zipFileCustomDirectoryRadioButton->isChecked()) {
        directoryName = m_zipFileCustomDirectoryLineEdit->text().trimmed();
        if (directoryName.isEmpty()) {
            errorMessageOut = "Zip File Custom directory is selected but is empty (see Advanced tab).";
            return "";
        }
        if ( ! FileInformation(directoryName).exists()) {
            errorMessageOut = "Zip File Custom directory is invalid.  Please choose a Custom Zip File Directory on Advanced tab";
            return "";
        }
    }
    else {
        CaretAssert(0);
    }
    
    FileInformation zipFileName(directoryName,
                                "BalsaUpload_" + QString::number(QDateTime::currentMSecsSinceEpoch())+ ".zip");

    return zipFileName.getAbsoluteFilePath();
}

/**
 * Gets called when the login button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::loginButtonClicked()
{
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    
    m_uploadPushButton->setEnabled(false);
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    /*
     * If operation takes a while
     */
    WuQWidgetDisabler blocker(m_loginPushButton);
    
    AString errorMessage;
    if ( ! m_balsaDatabaseManager->login(getDataBaseURL(),
                                         username,
                                         password,
                                         errorMessage)) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    if (m_balsaDatabaseManager->getUserRoles(*m_userRoles,
                                             errorMessage)) {
        if ( ! m_userRoles->isSubmitter()) {
            cursor.restoreCursor();
            const AString msg("Login to BALSA was successful.  However, your have not agreed "
                              "to the BALSA Submission Terms and Conditions that are required "
                              "to upload data to BALSA.  You will need to login to BALSA "
                              "using your web browser and complete this agreement.");
            const bool okPressedFlag = WuQMessageBox::warningAcceptReject(this,
                                                                          msg,
                                                                          "OK",
                                                                          "OK, Go to BALSA in Web Browser");
            if ( ! okPressedFlag) {
                labelHtmlLinkClicked("/");
            }
            return;
        }
    }
    else {
        cursor.restoreCursor();
        const AString msg("Unable to verify that you have agreed to the BALSA "
                          "Submission Terms and Conditions.  If you have not "
                          "agreed to terms and conditions, uploading will fail.\n\n"
                          + errorMessage);
        WuQMessageBox::warningOk(this, msg);
    }
    
    m_uploadPushButton->setEnabled(true);
}

/**
 * Gets called when the upload button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::uploadButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    m_sceneFile->setBalsaStudyID(m_balsaStudyIDLineEdit->text().trimmed());
    m_sceneFile->setBalsaStudyTitle(m_balsaStudyTitleLineEdit->text().trimmed());
    AString zipFileErrorMessage;
    const AString zipFileName = getZipFileNameWithPath(zipFileErrorMessage);
    
    m_sceneFile->setBalsaExtractToDirectoryName(m_extractDirectoryNameLineEdit->text().trimmed());
    
    AString msg;
    if ( ! m_usernameLineEdit->hasAcceptableInput()) {
        msg.appendWithNewLine("Username is missing.  If you do not have an account, press the \"Register\" link.  "
                              "If you have forgotten your username, press the \"Forgot Username\" link.<p>");
    }
    if ( ! m_passwordLineEdit->hasAcceptableInput()) {
        msg.appendWithNewLine("Password is missing.  If you do not remember your password, press the \"Forgot Password\" link.<p>");
    }
    if (zipFileName.isEmpty()) {
        msg.appendWithNewLine(zipFileErrorMessage + "<p>");
    }
    if ( ! m_extractDirectoryNameLineEdit->hasAcceptableInput()) {
        msg.appendWithNewLine("Extract to Directory is invalid.<p>");
    }
    
    switch (m_sceneFile->getBasePathType()) {
        case SceneFileBasePathTypeEnum::AUTOMATIC:
            break;
        case SceneFileBasePathTypeEnum::CUSTOM:
        {
            if (m_sceneFile->getBalsaBaseDirectory().isEmpty()) {
                msg.appendWithNewLine("CUSTOM Base Path (on Advanced Tab) is invalid.  "
                                      "Select Base Path or use AUTOMATIC.<p>");
            }
            else {
                if ( ! FileInformation(m_sceneFile->getBalsaBaseDirectory()).exists()) {
                    msg.appendWithNewLine("CUSTOM Base Directory (on Advanced Tab) is not a valid directory on this computer.  "
                                          "Select a valid Base Path or use AUTOMATIC.<p>");
                }
            }
        }
            break;
    }
    
    bool invalidStudyFlag = false;
    if ( ! m_balsaStudyTitleLineEdit->hasAcceptableInput()) {
        invalidStudyFlag = true;
    }
    if ( ! m_balsaStudyIDLineEdit->hasAcceptableInput()) {
        invalidStudyFlag = true;
    }
    if (invalidStudyFlag) {
        msg.appendWithNewLine("Study ID and/or Title is invalid.  Press the \""
                              + m_selectStudyTitlePushButton->text()
                              + "\" button to choose or create a BALSA Study ID and Title.");
    }
    
    if ( ! msg.isEmpty()) {
        msg.insert(0, "<html>");
        msg.append("</html>");
        WuQMessageBox::errorOk(this, msg);
        return;
    }
    
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    
    const AString extractToDirectoryName = m_extractDirectoryNameLineEdit->text().trimmed();
    
    if ( ! saveSceneFile("")) {
        return;
    }
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    ProgressReportingDialog progressDialog("Upload Scene File to BALSA",
                                           "",
                                           this);
    
    progressDialog.setCancelButton((QPushButton*)0); // no cancel button
    
    AString errorMessage;
    const bool successFlag = m_balsaDatabaseManager->uploadZippedSceneFile(m_sceneFile,
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
 * Save the scene file if it is modified.  If auto-save is OFF, the user is
 * prompted to confirm saving.
 *
 * @param message
 *     Message containing reason for saving scene file.  If empty, the
 *     default "file has been modified and must be saved" message is displayed.
 * @return
 *     True if the scene file was saved successfully, else false.
 */
bool
BalsaDatabaseUploadSceneFileDialog::saveSceneFile(const AString& saveMessage)
{
    if (m_sceneFile->isModified()) {
        if ( ! m_autoSaveSceneFileCheckBox->isChecked()) {
            QString msg("The scene file is modified and must be saved before continuing.  Would you like "
                              "to save the scene file using its current name and continue?");
            if ( ! saveMessage.isEmpty()) {
                msg = saveMessage;
            }
            
            if ( ! WuQMessageBox::warningOkCancel(this, msg)) {
                return false;
            }
            
        }
        try {
            Brain* brain = GuiManager::get()->getBrain();
            brain->writeDataFile(m_sceneFile);
        }
        catch (const DataFileException& e) {
            WuQMessageBox::errorOk(this, e.whatString());
            return false;
        }
    }
    
    return true;
}

/**
 * Gets called when auto save checkbox is checked.
 *
 * @param checked
 *     Check status
 */
void
BalsaDatabaseUploadSceneFileDialog::autoSaveCheckBoxClicked(bool /*checked*/)
{
    
}

void
BalsaDatabaseUploadSceneFileDialog::rolesButtonClicked()
{
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();

    AString roleNames;
    AString errorMessage;
    
    BalsaUserRoles balsaUserRoles;
    if ( ! m_balsaDatabaseManager->getUserRoles(balsaUserRoles,
                                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    std::cout << "*** Role Names: " << balsaUserRoles.toString() << std::endl;
}

/**
 * Select a Study Title from those user has in BALSA Database
 */
void
BalsaDatabaseUploadSceneFileDialog::selectStudyTitleButtonClicked()
{
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    
    std::vector<BalsaStudyInformation> studyInfo;
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    AString errorMessage;
    if ( ! m_balsaDatabaseManager->getAllStudyInformation(studyInfo,
                                                          errorMessage)) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    cursor.restoreCursor();
    
    const AString selectedStudyName = m_balsaStudyTitleLineEdit->text().trimmed();
    
    BalsaStudySelectionDialog dialog(studyInfo,
                                     selectedStudyName,
                                     m_balsaDatabaseManager.get(),
                                     getDataBaseURL(),
                                     username,
                                     password,
                                     this);
    if (dialog.exec() == BalsaStudySelectionDialog::Accepted) {
        BalsaStudyInformation bsi = dialog.getSelectedStudyInformation();
        if ( ! bsi.isEmpty()) {
            m_balsaStudyIDLineEdit->setText(bsi.getStudyID());
            m_balsaStudyTitleLineEdit->setText(bsi.getStudyTitle());
        }
        validateUploadData();
    }
}

/**
 * Choose the custom directory
 */
void
BalsaDatabaseUploadSceneFileDialog::browseZipFileCustomDirectoryPushButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    /*
     * Let user choose directory path
     */
    QString directoryName;
    FileInformation fileInfo(m_zipFileCustomDirectoryLineEdit->text().trimmed());
    if (fileInfo.exists()) {
        if (fileInfo.isDirectory()) {
            directoryName = fileInfo.getAbsoluteFilePath();
        }
    }
    AString newDirectoryName = CaretFileDialog::getExistingDirectoryDialog(m_zipFileCustomDirectoryRadioButton,
                                                                           "Choose Custom Zip File Directory",
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
    m_zipFileCustomDirectoryLineEdit->setText(newDirectoryName);
    FileInformation newFileInfo(newDirectoryName);
    if (newFileInfo.exists()
        && newFileInfo.isDirectory()) {
        m_zipFileCustomDirectoryRadioButton->setChecked(true);
    }
    
    validateUploadData();
}

/**
 * Create a regular expression validatory for the give label/data.
 *
 * @param labelName
 *     'Name' of label.
 */
QRegularExpressionValidator*
BalsaDatabaseUploadSceneFileDialog::createValidator(const LabelName labelName)
{
    QRegularExpression regEx;
    
    switch (labelName) {
        case LabelName::LABEL_DATABASE:
            regEx.setPattern(".+");
            break;
        case LabelName::LABEL_EXTRACT_DIRECTORY:
            regEx.setPattern(".+");
            break;
        case LabelName::LABEL_PASSWORD:
            regEx.setPattern(".+");
            break;
        case LabelName::LABEL_STUDY_ID:
        {
            const AString lowerCaseNoVowels("bcdfghjklmnpqrstvwxyz");
            const AString upperCaseNoVowels(lowerCaseNoVowels.toUpper());
            regEx.setPattern("[0-9" + lowerCaseNoVowels + upperCaseNoVowels + "]+");
        }
            break;
        case LabelName::LABEL_STUDY_TITLE:
            regEx.setPattern(".+");
            break;
        case LabelName::LABEL_USERNAME:
            regEx.setPattern(".+");
            break;
    }
    CaretAssert(regEx.isValid());
    
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(regEx,
                                                                             this);
    return validator;
}

/**
 * Called by validators.
 */
void
BalsaDatabaseUploadSceneFileDialog::validateUploadData()
{
    updateAllLabels();
}

/**
 * Update all of the labels.
 */
void
BalsaDatabaseUploadSceneFileDialog::updateAllLabels()
{
    setLabelText(LabelName::LABEL_DATABASE);
    setLabelText(LabelName::LABEL_EXTRACT_DIRECTORY);
    setLabelText(LabelName::LABEL_PASSWORD);
    setLabelText(LabelName::LABEL_STUDY_ID);
    setLabelText(LabelName::LABEL_STUDY_TITLE);
    setLabelText(LabelName::LABEL_USERNAME);
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
BalsaDatabaseUploadSceneFileDialog::setLabelText(const LabelName labelName)
{
    QLabel* label = NULL;
    AString labelText;
    bool validFlag = false;
    switch (labelName) {
        case LabelName::LABEL_DATABASE:
            label = m_databaseLabel;
            labelText = "Database";
            validFlag = true;
            break;
        case LabelName::LABEL_EXTRACT_DIRECTORY:
            label = m_extractDirectoryNameLabel;
            labelText = "Extract to Directory";
            validFlag = m_extractDirectoryNameLineEdit->hasAcceptableInput();
            break;
        case LabelName::LABEL_PASSWORD:
            label = m_passwordLabel;
            labelText = "Password";
            validFlag = m_passwordLineEdit->hasAcceptableInput();
            break;
        case LabelName::LABEL_STUDY_ID:
            label = m_balsaStudyIDLabel;
            labelText = "Study ID";
            validFlag = m_balsaStudyIDLineEdit->hasAcceptableInput();
            break;
        case LabelName::LABEL_STUDY_TITLE:
            label = m_balsaStudyTitleLabel;
            labelText = "Study Title";
            validFlag = m_balsaStudyTitleLineEdit->hasAcceptableInput();
            break;
        case LabelName::LABEL_USERNAME:
            label = m_usernameLabel;
            labelText = "Username";
            validFlag = m_usernameLineEdit->hasAcceptableInput();
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
