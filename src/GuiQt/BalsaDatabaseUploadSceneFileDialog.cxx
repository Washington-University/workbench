
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

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTabWidget>
#include <QToolButton>

#include "BalsaDatabaseManager.h"
#include "BalsaStudySelectionDialog.h"
#include "BalsaUserRoles.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CaretPreferences.h"
#include "CursorDisplayScoped.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ProgressReportingDialog.h"
#include "SceneBasePathWidget.h"
#include "SceneFile.h"
#include "SessionManager.h"
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

    if (s_username.isEmpty()) {
        s_username = SessionManager::get()->getCaretPreferences()->getBalsaUserName();
    }
    
    m_userRoles.reset(new BalsaUserRoles);
    
    QWidget* loginWidget = createLoginWidget();
    m_uploadWidget = createTabWidget();
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(loginWidget);
    dialogLayout->addWidget(m_uploadWidget);
    
    setCentralWidget(dialogWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
    
    loginInformationChanged();
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
    
    setOkButtonText("Upload");
    setCancelButtonText("Close");
    
    m_basePathWidget->updateWithSceneFile(m_sceneFile);
    updateUserRolesLabel();
    
    disableAutoDefaultForAllPushButtons();
    m_loginPushButton->setAutoDefault(true);
    m_loginPushButton->setDefault(true);
    showPasswordActionTriggered(m_showPasswordAction->isChecked());
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
    s_username = m_usernameLineEdit->text();
    s_password = m_passwordLineEdit->text();
    
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
    /*
     * Create the BALSA database manager
     */
    m_balsaDatabaseManager = std::unique_ptr<BalsaDatabaseManager>(new BalsaDatabaseManager());
    
    const int minimumLineEditWidth = 250;
    
    /*
     * Username
     */
    m_usernameLabel = new QLabel("");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_usernameLineEdit->setText(s_username);
    m_usernameLineEdit->setValidator(createValidator(LabelName::LABEL_USERNAME));
    QObject::connect(m_usernameLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->loginInformationChanged(); });
    QObject::connect(m_usernameLineEdit, &QLineEdit::returnPressed,
                     this, &BalsaDatabaseUploadSceneFileDialog::returnPressedUsernameOrPassword);
    
    /*
     * Password
     */
    m_passwordLabel = new QLabel("");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    if (s_password.isEmpty()) {
        const bool allowEnvPasswordFlag(false);
        if (allowEnvPasswordFlag) {
            QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
            m_passwordLineEdit->setText(environment.value("WORKBENCH_BALSA_PD"));
        }
    }
    else {
        m_passwordLineEdit->setText(s_password);
    }
    m_passwordLineEdit->setValidator(createValidator(LabelName::LABEL_PASSWORD));
    QObject::connect(m_passwordLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->loginInformationChanged(); });
    QObject::connect(m_passwordLineEdit, &QLineEdit::returnPressed,
                     this, &BalsaDatabaseUploadSceneFileDialog::returnPressedUsernameOrPassword);
    
    /*
     * Forgot username label/link
     */
    QLabel* forgotUsernameLabel = new QLabel("<html>"
                                             "<bold><a href=\"register/forgotUsername\">Forgot Username</a></bold>"
                                             "</html>");
    QObject::connect(forgotUsernameLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    /*
     * Show password tool button
     */
    m_showPasswordAction = new QAction("Show", this);
    m_showPasswordAction->setCheckable(true);
    m_showPasswordAction->setChecked(false);
    QObject::connect(m_showPasswordAction, &QAction::triggered,
                     this, &BalsaDatabaseUploadSceneFileDialog::showPasswordActionTriggered);
    QToolButton* showPasswordToolButton = new QToolButton();
    showPasswordToolButton->setDefaultAction(m_showPasswordAction);
    
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
    
    /*
     * Label for User Roles
     */
    m_userRolesLabel = new QLabel("                 ");
    
    int columnCounter = 0;
    const int COLUMN_LABEL = columnCounter++;
    const int COLUMN_DATA_WIDGET_ONE = columnCounter++;
    const int COLUMN_DATA_WIDGET_TWO = columnCounter++;
    const int COLUMN_BUTTON_ONE = columnCounter++;
    const int COLUMN_BUTTON_TWO = columnCounter++;
    
    QGroupBox* groupBox = new QGroupBox("BALSA Login");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(5);
    gridLayout->setColumnStretch(COLUMN_LABEL, 0);
    gridLayout->setColumnStretch(COLUMN_DATA_WIDGET_ONE, 0);
    gridLayout->setColumnStretch(COLUMN_DATA_WIDGET_TWO, 100);
    gridLayout->setColumnStretch(COLUMN_BUTTON_ONE, 0);
    int row = 0;
    gridLayout->addWidget(m_usernameLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_usernameLineEdit, row, COLUMN_DATA_WIDGET_ONE, 1, 2);
    gridLayout->addWidget(registerLabel, row, COLUMN_BUTTON_ONE);
    gridLayout->addWidget(forgotUsernameLabel, row, COLUMN_BUTTON_TWO);
    row++;
    gridLayout->addWidget(m_passwordLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_passwordLineEdit, row, COLUMN_DATA_WIDGET_ONE, 1, 2);
    gridLayout->addWidget(showPasswordToolButton, row, COLUMN_BUTTON_ONE);
    gridLayout->addWidget(forgotPasswordLabel, row, COLUMN_BUTTON_TWO);
    row++;
    gridLayout->setRowMinimumHeight(row, 10); // empty row
    row++;
    gridLayout->addWidget(m_loginPushButton, row, COLUMN_DATA_WIDGET_ONE);
    gridLayout->addWidget(m_userRolesLabel, row, COLUMN_DATA_WIDGET_TWO, 1, Qt::AlignLeft);
    
    return groupBox;
}

/**
 * Called when login information (database, username, password) is changed
 */
void
BalsaDatabaseUploadSceneFileDialog::loginInformationChanged()
{
    m_balsaDatabaseManager->logout();
    setOkButtonEnabled(false);
    m_selectStudyTitlePushButton->setEnabled(false);
    m_userRoles->resetToAllInvalid();
    updateAllLabels();
}

/**
 * Called when show password action is triggered.
 *
 * @param checked
 *     New status.
 */
void
BalsaDatabaseUploadSceneFileDialog::showPasswordActionTriggered(bool checked)
{
    if (checked) {
        m_passwordLineEdit->setEchoMode(QLineEdit::Normal);
    }
    else {
        m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}

/**
 * @return The upload widget
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createTabWidget()
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
    m_extractDirectoryNameLineEdit->setToolTip("Directory created when data files are extracted from ZIP archive");
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
    m_selectStudyTitlePushButton = new QPushButton("Create/Select/Rename...");
    m_selectStudyTitlePushButton->setToolTip("Add new study, choose a study, or edit a study's title in BALSA Database");
    QObject::connect(m_selectStudyTitlePushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::selectStudyTitleButtonClicked);
    
    /*
     * Test button
     */
    QPushButton* testPushButton = new QPushButton("Test...");
    testPushButton->setToolTip("Test some functionality");
    QObject::connect(testPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::testButtonClicked);
    testPushButton->setVisible(false);

    /*
     * Auto-save checkbox
     */
    m_autoSaveSceneFileCheckBox = new QCheckBox("Auto-Save Scene File");
    m_autoSaveSceneFileCheckBox->setChecked(true);
    QObject::connect(m_autoSaveSceneFileCheckBox, &QCheckBox::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::autoSaveCheckBoxClicked);
        
    int columnCounter = 0;
    const int COLUMN_LABEL = columnCounter++;
    const int COLUMN_DATA_WIDGET = columnCounter++;
    const int COLUMN_BUTTON_ONE = columnCounter++;
    const int COLUMN_BUTTON_TWO = columnCounter++;
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(5);
    gridLayout->setColumnMinimumWidth(COLUMN_DATA_WIDGET, 300);
    gridLayout->setColumnStretch(COLUMN_LABEL, 0);
    gridLayout->setColumnStretch(COLUMN_DATA_WIDGET, 100);
    gridLayout->setColumnStretch(COLUMN_BUTTON_ONE, 0);
    gridLayout->setColumnStretch(COLUMN_BUTTON_TWO, 0);
    int row = 0;
    gridLayout->addWidget(m_balsaStudyTitleLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_balsaStudyTitleLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(m_selectStudyTitlePushButton, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_balsaStudyIDLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_balsaStudyIDLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(testPushButton, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_extractDirectoryNameLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_extractDirectoryNameLineEdit, row, COLUMN_DATA_WIDGET);
    row++;
    gridLayout->addWidget(m_autoSaveSceneFileCheckBox, row, COLUMN_DATA_WIDGET, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->setRowMinimumHeight(row, 15); // empty row
    row++;
    
    m_balsaStudyIDLineEdit->setText(m_sceneFile->getBalsaStudyID());
    m_balsaStudyTitleLineEdit->setText(m_sceneFile->getBalsaStudyTitle());
    m_extractDirectoryNameLineEdit->setText(m_sceneFile->getBalsaExtractToDirectoryName());
    
    if (m_extractDirectoryNameLineEdit->text().trimmed().isEmpty()) {
        m_extractDirectoryNameLineEdit->setText(m_sceneFile->getDefaultExtractToDirectoryName());
    }
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(gridLayout);
    layout->addStretch();
    return widget;
}

/**
 * @return A new BALSA Database selection widget
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createBalsaDatabaseSelectionWidget()
{
    bool isWustlDomainFlag = false;
    /* DOES NOT WORK ON MACOS 10.14, See note in: SystemUtilities::getLocalHostName()
       QString hostName = SystemUtilities::getLocalHostName();
       bool isWustlDomainFlag = hostName.endsWith(".wustl.edu");
     */
#ifndef NDEBUG
    isWustlDomainFlag = true;
#endif
    /*
     * Database selection
     */
    m_databaseLabel = new QLabel("");
    m_databaseComboBox = new QComboBox();
    m_databaseComboBox->setEditable(true);
    m_databaseComboBox->addItem("https://balsa.wustl.edu");
    if (isWustlDomainFlag) {
        m_databaseComboBox->addItem("http://johnsdev.wustl.edu:8080");
        m_databaseComboBox->addItem("https://johnsdev.wustl.edu:8080");
    }
    QObject::connect(m_databaseComboBox, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::textActivated),
                     this, [=] { this->loginInformationChanged(); });
    QObject::connect(m_databaseComboBox, &QComboBox::editTextChanged,
                     this, [=] { this->loginInformationChanged(); });
    
    QPushButton* whatsThisPushButton = new QPushButton("What's this?");
    whatsThisPushButton->setSizePolicy(QSizePolicy::Fixed, whatsThisPushButton->sizePolicy().verticalPolicy());
    QObject::connect(whatsThisPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::whatsThisDatabase);
    
    int columnCounter = 0;
    const int COLUMN_LABEL = columnCounter++;
    const int COLUMN_WIDGET = columnCounter++;
    
    QGroupBox* groupBox = new QGroupBox("BALSA Database");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(5);
    gridLayout->setColumnStretch(COLUMN_LABEL, 0);
    gridLayout->setColumnStretch(COLUMN_WIDGET, 100);
    int row = 0;
    gridLayout->addWidget(whatsThisPushButton, row, COLUMN_LABEL, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_databaseLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_databaseComboBox, row, COLUMN_WIDGET);
    row++;
    
    return groupBox;
}

/**
 * @return A new ZIP file directory widget
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::creatZipFileDirectoryWidget()
{
    m_zipFileTemporaryDirectoryRadioButton = new QRadioButton("System Temporary");
    m_zipFileCustomDirectoryRadioButton = new QRadioButton("Custom: ");
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(m_zipFileTemporaryDirectoryRadioButton);
    buttGroup->addButton(m_zipFileCustomDirectoryRadioButton);
    m_zipFileTemporaryDirectoryRadioButton->setChecked(true); // do before signal
    QObject::connect(buttGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::idClicked),
                     [=](int id){ this->zipFileDirectoryRadioButtonClicked(id); });
    
    m_zipFileAutomaticDirectoryLineEdit = new QLineEdit();
    m_zipFileAutomaticDirectoryLineEdit->setReadOnly(true);
    m_zipFileAutomaticDirectoryLineEdit->setText(SystemUtilities::getTempDirectory());
    
    m_zipFileCustomDirectoryLineEdit = new QLineEdit();
    
    QPushButton* copyAutomaticDirectoryPushButton = new QPushButton("Copy");
    QObject::connect(copyAutomaticDirectoryPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::copyAutomaticDirectoryPushButtonClicked);
    copyAutomaticDirectoryPushButton->setToolTip("Copy system temporary directory to clipboard");
    
    QPushButton* browseCustomDirectoryPushButton = new QPushButton("Browse...");
    QObject::connect(browseCustomDirectoryPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::browseZipFileCustomDirectoryPushButtonClicked);
    
    QPushButton* whatsThisPushButton = new QPushButton("What's this?");
    whatsThisPushButton->setSizePolicy(QSizePolicy::Fixed, whatsThisPushButton->sizePolicy().verticalPolicy());
    QObject::connect(whatsThisPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::whatsThisZipFileDirectory);
    
    QGroupBox* zipDirectoryGroupBox = new QGroupBox("Zip File Directory");
    QGridLayout* gridLayout = new QGridLayout(zipDirectoryGroupBox);
    gridLayout->setSpacing(2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    int row = 0;
    gridLayout->addWidget(whatsThisPushButton, row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_zipFileTemporaryDirectoryRadioButton, row, 0);
    gridLayout->addWidget(m_zipFileAutomaticDirectoryLineEdit, row, 1);
    gridLayout->addWidget(copyAutomaticDirectoryPushButton, row, 2);
    row++;
    gridLayout->addWidget(m_zipFileCustomDirectoryRadioButton, row, 0);
    gridLayout->addWidget(m_zipFileCustomDirectoryLineEdit, row, 1);
    gridLayout->addWidget(browseCustomDirectoryPushButton, row, 2);
    row++;
    
    return zipDirectoryGroupBox;
}

/**
 * @return New instance of the advanced tab
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createAdvancedTab()
{
    QWidget* databaseSelectionWidget = createBalsaDatabaseSelectionWidget();
    QWidget* zipDirectoryWidget = creatZipFileDirectoryWidget();
    m_basePathWidget = new SceneBasePathWidget();

    QLabel* infoLabel = new QLabel("NOTE: In most instances, the default selections should be used.");
    infoLabel->setWordWrap(true);
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(infoLabel);
    layout->addWidget(m_basePathWidget);
    layout->addWidget(zipDirectoryWidget);
    layout->addWidget(databaseSelectionWidget);
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
    if (username.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               "Username is empty");
        return;
    }
    const AString password = m_passwordLineEdit->text().trimmed();
    if (password.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               "Password is empty");
        return;
    }
    
    setOkButtonEnabled(false);
    m_selectStudyTitlePushButton->setEnabled(false);
    m_userRoles->resetToAllInvalid();
    updateUserRolesLabel();
    
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
            const AString msg("Login to BALSA was successful.  However, you have not agreed "
                              "to the BALSA Submission Terms and Conditions that are required "
                              "to upload data to BALSA.  You will need to login to BALSA "
                              "using your web browser and complete this agreement.");
            const bool okPressedFlag = WuQMessageBox::warningAcceptReject(this,
                                                                          msg,
                                                                          "OK",
                                                                          "OK, Go to BALSA in Web Browser");
            if ( ! okPressedFlag) {
                labelHtmlLinkClicked("about/submission");
            }
            return;
        }
    }
    else {
        m_userRoles->resetToAllInvalid();
        
        cursor.restoreCursor();
        const AString msg("Unable to verify that you have agreed to the BALSA "
                          "Submission Terms and Conditions.  If you have not "
                          "agreed to terms and conditions, uploading will fail.\n\n"
                          + errorMessage);
        WuQMessageBox::warningOk(this, msg);
    }
    
    updateUserRolesLabel();
    
    setOkButtonEnabled(true);
    m_selectStudyTitlePushButton->setEnabled(true);
    
    SessionManager::get()->getCaretPreferences()->setBalsaUserName(username);
}

/**
 * Check the BALSA database to see if the "Extraction Directory Prefix" in the database
 * is different than the value in the dialog.  If so, warn user.
 *
 * @return 
 *     True if uploading should continue, otherwise false.
 */
bool
BalsaDatabaseUploadSceneFileDialog::checkBalsaExtractionDirectoryPrefix()
{
    bool validFlag = true;
    
    const AString studyID = m_balsaStudyIDLineEdit->text().trimmed();
    if ( ! studyID.isEmpty()) {
        if (m_balsaDatabaseManager->isStudyIDValid(studyID)) {
            AString balsaDirectoryName;
            AString errorMessage;
            if (m_balsaDatabaseManager->getStudyExtractDirectoryPrefix(studyID,
                                                                       balsaDirectoryName,
                                                                       errorMessage)) {
                if ( ! balsaDirectoryName.isEmpty()) {
                    const AString currentDirName = m_extractDirectoryNameLineEdit->text().trimmed();
                    if (balsaDirectoryName != currentDirName) {
                        AString msg("<html>"
                                    "The <b>Extraction Directory Prefix</b> in this dialog differs from the "
                                    "<b>Extraction Directory Prefix</b> for this study in the BALSA Database."
                                    "<p>"
                                    "This may be caused by:"
                                    "<ul>"
                                    "<li>A scene file containing a different base directory in the "
                                    "same study was uploaded to BALSA"
                                    "<li>The Extraction Directory Prefix has been edited in this scene file"
                                    "<li>The Extraction Directory Prefix has been edited through the "
                                    "BALSA Database web interface (Edit Study: Study Details)"
                                    "</ul>"
                                    "<p>"
                                    "For the Extraction Directory Prefix:"
                                    "</html>");
                        WuQDataEntryDialog ded("Warning, Extraction Directory Prefix",
                                               this);
                        ded.setTextAtTop(msg, true);
                        QRadioButton* balsaDirRadioButton = ded.addRadioButton("Change to \""
                                                                               + balsaDirectoryName
                                                                               + "\"");
                        
                        QRadioButton* dialogDirRadioButton = ded.addRadioButton("No change, use \""
                                                                                + currentDirName
                                                                                + "\" from this dialog");
                        balsaDirRadioButton->setChecked(true);
                        ded.setOkButtonText("Continue");
                        if (ded.exec() == WuQDataEntryDialog::Accepted) {
                            AString dirName;
                            if (balsaDirRadioButton->isChecked()){
                                dirName = balsaDirectoryName;
                            }
                            else if (dialogDirRadioButton->isChecked()) {
                                dirName = currentDirName;
                            }
                            else {
                                CaretAssert(0);
                            }
                            m_extractDirectoryNameLineEdit->setText(dirName);
                            m_sceneFile->setBalsaExtractToDirectoryName(dirName);
                        }
                        else {
                            validFlag = false;
                        }
                    }
                }
            }
            else {
                AString msg("BALSA was unable to provide an updated \"Extraction Directory Prefix\" for Study ID \""
                            + studyID
                            + "\".  You may continue uploading.");
                WuQMessageBox::warningOk(this, msg);
            }
        }
    }
    
    return validFlag;
}

/**
 * Update roles label with user's valid roles
 */
void
BalsaDatabaseUploadSceneFileDialog::updateUserRolesLabel()
{
    m_userRolesLabel->setText("");
    
    const bool showRolesFlag = false;
    if (showRolesFlag) {
        const AString rolesText = m_userRoles->getRolesForDisplayInGUI();
        if ( ! rolesText.isEmpty()) {
            m_userRolesLabel->setText("   BALSA Roles: "
                                      + rolesText);
        }
    }
}


/**
 * Gets called when return is pressed in either username
 * or password.  If both contain text, try to login.
 */
void
BalsaDatabaseUploadSceneFileDialog::returnPressedUsernameOrPassword()
{
    if (m_usernameLineEdit->text().trimmed().isEmpty()) {
        return;
    }
    if (m_passwordLineEdit->text().trimmed().isEmpty()) {
        return;
    }
    
    loginButtonClicked();
}


/**
 * Gets called when the Upload button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::okButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    m_sceneFile->setBalsaStudyID(m_balsaStudyIDLineEdit->text().trimmed());
    m_sceneFile->setBalsaStudyTitle(m_balsaStudyTitleLineEdit->text().trimmed());
    AString zipFileErrorMessage;
    const AString zipFileName = getZipFileNameWithPath(zipFileErrorMessage);
    
    m_sceneFile->setBalsaExtractToDirectoryName(m_extractDirectoryNameLineEdit->text().trimmed());
    
    s_username = m_usernameLineEdit->text();
    s_password = m_passwordLineEdit->text();
    
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
        msg.appendWithNewLine("Extraction Directory Prefix is invalid.<p>");
    }
    
    AString basePathErrorMessage;
    if ( ! m_basePathWidget->isValid(basePathErrorMessage)) {
        msg.appendWithNewLine(basePathErrorMessage);
    }
    else {
        switch (m_sceneFile->getBasePathType()) {
            case SceneFileBasePathTypeEnum::AUTOMATIC:
                break;
            case SceneFileBasePathTypeEnum::CUSTOM:
            {
                if (m_sceneFile->getBalsaCustomBaseDirectory().isEmpty()) {
                    msg.appendWithNewLine("CUSTOM Base Path (on Advanced Tab) is invalid.  "
                                          "Select Base Path or use AUTOMATIC.<p>");
                }
                else {
                    if ( ! FileInformation(m_sceneFile->getBalsaCustomBaseDirectory()).exists()) {
                        msg.appendWithNewLine("CUSTOM Base Directory (on Advanced Tab) is not a valid directory on this computer.  "
                                              "Select a valid Base Path or use AUTOMATIC.<p>");
                    }
                }
            }
                break;
        }
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
    else {
        /*
         * Note: Test of study ID for editable status will move
         * into BALSA Database so disabled until that time.
         */
        const bool testStudyIdEditableFlag(false);
        if (testStudyIdEditableFlag) {
            if (m_extractDirectoryNameLineEdit->hasAcceptableInput()) {
                const AString studyID = m_balsaStudyIDLineEdit->text().trimmed();
                AString errorMessage;
                if ( ! m_balsaDatabaseManager->isStudyEditableByUser(studyID,
                                                                     errorMessage)) {
                    msg.appendWithNewLine(errorMessage);
                }
            }
        }
    }
    
    if ( ! msg.isEmpty()) {
        msg.insert(0, "<html>");
        msg.append("</html>");
        WuQMessageBox::errorOk(this, msg);
        return;
    }
    
    if ( ! checkBalsaExtractionDirectoryPrefix()) {
        return;
    }
    
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    
    const AString extractToDirectoryName = m_extractDirectoryNameLineEdit->text().trimmed();
    
    AString saveMessage("The scene file is modified and must be saved before continuing.");
    AString errorMessage;
    bool sceneFileModStatus = m_sceneFile->isModified();
    if (m_balsaDatabaseManager->updateSceneIDs(m_sceneFile,
                                               errorMessage)) {
        if ( ! sceneFileModStatus) {
            if (m_sceneFile->isModified()) {
                saveMessage = "The scene file was modified to add BALSA Scene Identifiers and must be saved before continuing.";
            }
        }
    }
    else {
        return;
    }
    
    if ( ! saveSceneFile(saveMessage)) {
        return;
    }
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    ProgressReportingDialog progressDialog("Upload Scene File to BALSA",
                                           "",
                                           this);
    
    progressDialog.setCancelButton((QPushButton*)0); // no cancel button
    
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
            AString msg(saveMessage
                        +  "  Would you like to save the scene file using its current name and continue?");
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
BalsaDatabaseUploadSceneFileDialog::testButtonClicked()
{
    const int32_t buttonIndex = WuQMessageBox::informationTwoButtons(this,
                                                                     "You have not agreed to Submitter Terms",
                                                                     "OK",
                                                                     "OK, View Terms in Web Browser");
    switch (buttonIndex) {
        case 1:
            break;
        case 2:
            labelHtmlLinkClicked("about/submission");
            break;
        default:
            CaretAssert(0);
    }
    return;
    
    bool testRolesFlag = false;
    bool testNewIDsFlag = true;
    if (testRolesFlag) {
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
    
    if (testNewIDsFlag) {
        int32_t numberOfSceneIDs = 2;
        std::vector<AString> sceneIDs;
        AString errorMessage;
        if ( ! m_balsaDatabaseManager->getSceneIDs(numberOfSceneIDs,
                                                   sceneIDs,
                                                   errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
            return;
        }
        for (auto id : sceneIDs) {
            std::cout << "Scene ID " << id << std::endl;
        }
    }
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
            m_extractDirectoryNameLineEdit->setText(bsi.getStudyExtractionDirectoryName());
        }
        validateUploadData();
    }
}

/**
 * Copy automatic directory to clipboard
 */
void
BalsaDatabaseUploadSceneFileDialog::copyAutomaticDirectoryPushButtonClicked()
{
    const QString txt = m_zipFileAutomaticDirectoryLineEdit->text().trimmed();
    if ( ! txt.isEmpty()) {
        QApplication::clipboard()->setText(txt,
                                           QClipboard::Clipboard);
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
            labelText = "Extraction Directory Prefix";
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

/**
 * Displays a dialog explaining the ZIP file directory options
 */
void
BalsaDatabaseUploadSceneFileDialog::whatsThisZipFileDirectory()
{
    const AString text("When a Scene File is uploaded to BALSA, the Scene File and all data files that it "
                       "references are placed into a ZIP file and the ZIP file is then uploaded to BALSA.  "
                       "While unlikely, creation of the ZIP file in the system temporary directory could fail "
                       "if there is insufficient space.  If this should happen, set a CUSTOM directory on a disk "
                       "with adequate space.");
    WuQMessageBox::informationOk(this,
                                 text);
}

/**
 * Displays a dialog explaining the database selection option
 */
void
BalsaDatabaseUploadSceneFileDialog::whatsThisDatabase()
{
    const AString text("This option is provided for developers of "
                       "Workbench (wb_view and wb_command) and BALSA Database software. "
                       "It allows the selection of develpment versions of the BALSA Database.  "
                       "To add a database, type the new name into the combo box.");
    WuQMessageBox::informationOk(this,
                                 text);
}

