
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
    
    /*
     * Icons
     */
    m_lockClosedIconValid = WuQtUtilities::loadIcon(":/BalsaUploadDialog/lock-closed.png",
                                                    m_lockClosedIcon);
    m_lockOpenIconValid = WuQtUtilities::loadIcon(":/BalsaUploadDialog/lock-open.png",
                                                  m_lockOpenIcon);
    
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(createUploadTab(), "Upload");
    tabWidget->addTab(createAdvancedTab(), "Advanced");
    
    setCentralWidget(tabWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
    
    updateBalsaStudyIDLockButtonIcon();
    validateData();
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
BalsaDatabaseUploadSceneFileDialog::~BalsaDatabaseUploadSceneFileDialog()
{
}

/**
 * @return New instance of the upload tab.
 */
QWidget*
BalsaDatabaseUploadSceneFileDialog::createUploadTab()
{
    QString defaultUserName = "balsaTest";
    QString defaultPassword = "@2password";
    
#ifdef NDEBUG
    defaultUserName = "";
    defaultPassword = "";
#endif
    
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
                     this, [=] { this->validateData(); });
    
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
                     this, [=] { this->validateData(); });
    
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
                     this, [=] { this->validateData(); });
    
    
    /*
     * Scene BALSA Study ID
     */
    m_balsaStudyIDLabel = new QLabel("");
    m_balsaStudyIDLineEdit = new QLineEdit();
    m_balsaStudyIDLineEdit->setReadOnly(true);
    m_balsaStudyIDLineEdit->setToolTip("The Study ID is available from the BALSA Database; click the Get button to get a Study ID");
    m_balsaStudyIDLineEdit->setValidator(createValidator(LabelName::LABEL_STUDY_ID));
    QObject::connect(m_balsaStudyIDLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateData(); });
    
    /*
     * Get BALSA Study ID button
     */
    const bool showGetStudyIdButtonFlag = false;
    m_getBalsaStudyIDPushButton = NULL;
    if (showGetStudyIdButtonFlag) {
        m_getBalsaStudyIDPushButton = new QPushButton("Get...");
        m_getBalsaStudyIDPushButton->setToolTip("Get the BALSA Study ID by sending Study Title to BALSA");
        QObject::connect(m_getBalsaStudyIDPushButton, &QPushButton::clicked,
                         this, &BalsaDatabaseUploadSceneFileDialog::getBalsaStudyIDPushButtonClicked);
    }
    
    /*
     * BALSA Study ID Lock button
     */
    m_balsaStudyIDLockPushButton = new QPushButton("Lock");
    m_balsaStudyIDLockPushButton->setToolTip("Unlock to allow manual editing of the Study ID.");
    QObject::connect(m_balsaStudyIDLockPushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::balsaStudyIDLockPushButtonClicked);
    if (m_lockClosedIconValid
        && m_lockOpenIconValid) {
        m_balsaStudyIDLockPushButton->setText("");
    }
    
    /*
     * Scene BALSA Study Title
     */
    m_balsaStudyTitleLabel = new QLabel("");
    m_balsaStudyTitleLineEdit = new QLineEdit();
    m_balsaStudyTitleLineEdit->setToolTip("Title for the study");
    m_balsaStudyTitleLineEdit->setValidator(createValidator(LabelName::LABEL_STUDY_TITLE));
    QObject::connect(m_balsaStudyTitleLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateData(); });
    
    /**
     * Select Study Push Button
     */
    m_selectStudyTitlePushButton = new QPushButton("Add/Choose...");
    m_selectStudyTitlePushButton->setToolTip("Add new study or choose from user's studies in BALSA Database");
    QObject::connect(m_selectStudyTitlePushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseUploadSceneFileDialog::selectStudyTitleButtonClicked);
    
    /*
     * Base Directory
     */
    m_baseDirectoryLabel = new QLabel("");
    m_baseDirectoryLineEdit = new QLineEdit();
    m_baseDirectoryLineEdit->setToolTip("Directory that contains all data files");
    m_baseDirectoryLineEdit->setValidator(createValidator(LabelName::LABEL_BASE_DIRECTORY));
    QObject::connect(m_baseDirectoryLineEdit, &QLineEdit::textEdited,
                     this, [=] { this->validateData(); });
    
    /*
     * Browse for base directory
     */
    m_browseBaseDirectoryPushButton = new QPushButton("Browse...");
    m_browseBaseDirectoryPushButton->setToolTip("Use a file system dialog to choose the base directory");
    QObject::connect(m_browseBaseDirectoryPushButton, SIGNAL(clicked()),
                     this, SLOT(browseBaseDirectoryPushButtonClicked()));
    
    int columnCounter = 0;
    const int COLUMN_LABEL = columnCounter++;
    const int COLUMN_DATA_WIDGET = columnCounter++;
    const int COLUMN_BUTTON_ONE = columnCounter++;
    const int COLUMN_BUTTON_TWO = columnCounter++;
    
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
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
    gridLayout->addWidget(m_extractDirectoryNameLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_extractDirectoryNameLineEdit, row, COLUMN_DATA_WIDGET);
    row++;
    gridLayout->addWidget(m_baseDirectoryLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_baseDirectoryLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(m_browseBaseDirectoryPushButton, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_balsaStudyTitleLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_balsaStudyTitleLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(m_selectStudyTitlePushButton, row, COLUMN_BUTTON_ONE, 1, 2);
    row++;
    gridLayout->addWidget(m_balsaStudyIDLabel, row, COLUMN_LABEL, Qt::AlignRight);
    gridLayout->addWidget(m_balsaStudyIDLineEdit, row, COLUMN_DATA_WIDGET);
    gridLayout->addWidget(m_balsaStudyIDLockPushButton, row, COLUMN_BUTTON_ONE);
    if (m_getBalsaStudyIDPushButton != NULL) {
        gridLayout->addWidget(m_getBalsaStudyIDPushButton, row, COLUMN_BUTTON_TWO);
    }
    
    
    m_balsaStudyIDLineEdit->setText(m_sceneFile->getBalsaStudyID());
    AString baseDirectory = m_sceneFile->getBalsaBaseDirectory();
    if (baseDirectory.isEmpty()) {
        FileInformation fileInfo(m_sceneFile->getFileName());
        baseDirectory = fileInfo.getPathName();
    }
    m_baseDirectoryLineEdit->setText(baseDirectory);
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
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(zipDirectoryGroupBox);
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
    validateData();
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
 * Gets called when the OK button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::okButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    m_sceneFile->setBalsaStudyID(m_balsaStudyIDLineEdit->text().trimmed());
    m_sceneFile->setBalsaStudyTitle(m_balsaStudyTitleLineEdit->text().trimmed());
    m_sceneFile->setBalsaBaseDirectory(m_baseDirectoryLineEdit->text().trimmed());
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
    if ( ! m_baseDirectoryLineEdit->hasAcceptableInput()) {
        msg.appendWithNewLine("Base Directory is invalid.<p>");
    }
    if ( ! m_balsaStudyTitleLineEdit->hasAcceptableInput()) {
        msg.appendWithNewLine("Study Title is invalid.<p>");
    }
    if ( ! m_balsaStudyIDLineEdit->hasAcceptableInput()) {
        msg.appendWithNewLine("Study ID is invalid.  To get a Study ID, press the \"Get\" button which "
                              "sends the Study Title to the Database and returns a Study ID.  You may "
                              "also go to https://balsa.wustl.edu to get a Study ID<p>");
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
    
    AString errorMessage;
    const bool successFlag = m_balsaDatabaseManager->uploadZippedSceneFile(getDataBaseURL(),
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
 * Select a Study Title from those user has in BALSA Database
 */
void
BalsaDatabaseUploadSceneFileDialog::selectStudyTitleButtonClicked()
{
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    
    std::vector<BalsaStudyInformation> studyInfo;
    studyInfo.emplace(studyInfo.end(), "AV4H", "Study One");
    studyInfo.emplace(studyInfo.end(), "BX32", "Study Two");
    studyInfo.emplace(studyInfo.end(), "CR33", "Study Three");
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    AString errorMessage;
    if ( ! m_balsaDatabaseManager->getAllStudyInformationForUser(getDataBaseURL(),
                                                                 username,
                                                                 password,
                                                                 studyInfo,
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
        validateData();
    }
}

/**
 * Update the Icon in the BALSA Study ID Lock Button
 */
void
BalsaDatabaseUploadSceneFileDialog::updateBalsaStudyIDLockButtonIcon()
{
    
    if (m_lockClosedIconValid
        && m_lockOpenIconValid) {
        if (m_balsaStudyIDLineEdit->isReadOnly()) {
            m_balsaStudyIDLockPushButton->setIcon(m_lockClosedIcon);
        }
        else {
            m_balsaStudyIDLockPushButton->setIcon(m_lockOpenIcon);
        }
    }
}

/**
 * Called when BALSA Study ID Lock Button is clicked
 */
void
BalsaDatabaseUploadSceneFileDialog::balsaStudyIDLockPushButtonClicked()
{
    /*
     * Toggle the 'read only' status of the Study ID's Line Edit
     */
    m_balsaStudyIDLineEdit->setReadOnly( ! m_balsaStudyIDLineEdit->isReadOnly());
    
    updateBalsaStudyIDLockButtonIcon();
}


/**
 * Called when get BALSA study ID push button is clicked.
 */
void
BalsaDatabaseUploadSceneFileDialog::getBalsaStudyIDPushButtonClicked()
{
    CaretAssert(m_getBalsaStudyIDPushButton);
    
    const AString title = m_balsaStudyTitleLineEdit->text().trimmed();
    if (title.isEmpty()) {
        WuQMessageBox::errorOk(m_getBalsaStudyIDPushButton,
                               "The Study Title is required to get a Study ID");
        return;
    }
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    const AString username = m_usernameLineEdit->text().trimmed();
    const AString password = m_passwordLineEdit->text().trimmed();
    
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
        m_balsaStudyIDLineEdit->setText(studyID);
        validateData();
    }
    else {
        WuQMessageBox::errorOk(this,
                               errorMessage);
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
    AString newDirectoryName = CaretFileDialog::getExistingDirectoryDialog(m_browseBaseDirectoryPushButton,
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
    
    validateData();
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
    FileInformation fileInfo(m_baseDirectoryLineEdit->text().trimmed());
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
    m_baseDirectoryLineEdit->setText(newDirectoryName);
    validateData();
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
        case LabelName::LABEL_BASE_DIRECTORY:
            regEx.setPattern(".+");
            break;
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
BalsaDatabaseUploadSceneFileDialog::validateData()
{
    updateAllLabels();
    if (m_getBalsaStudyIDPushButton != NULL) {
        m_getBalsaStudyIDPushButton->setEnabled(m_balsaStudyTitleLineEdit->hasAcceptableInput());
    }
}

/**
 * Update all of the labels.
 */
void
BalsaDatabaseUploadSceneFileDialog::updateAllLabels()
{
    setLabelText(LabelName::LABEL_BASE_DIRECTORY);
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
        case LabelName::LABEL_BASE_DIRECTORY:
            label = m_baseDirectoryLabel;
            labelText = "Base Directory";
            validFlag = m_baseDirectoryLineEdit->hasAcceptableInput();
            break;
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


