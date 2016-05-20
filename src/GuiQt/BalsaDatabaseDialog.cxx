
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

#define __BALSA_DATABASE_DIALOG_DECLARE__
#include "BalsaDatabaseDialog.h"
#undef __BALSA_DATABASE_DIALOG_DECLARE__

#include <QDesktopServices>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CursorDisplayScoped.h"
#include "CommandOperationManager.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "OperationException.h"
#include "OperationZipSceneFile.h"
#include "ProgramParameters.h"
#include "SceneFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BalsaDatabaseDialog 
 * \brief Wizard dialog for uploading to BALSA Database.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param sceneFileName
 *     Name of Scene File that will be uploaded to BALSA.
 * @param parent
 *     Parent widget of dialog.
 */
BalsaDatabaseDialog::BalsaDatabaseDialog(const SceneFile* sceneFile,
                                         QWidget* parent)
: QWizard(parent),
m_sceneFile(sceneFile)
{
    CaretAssert(sceneFile);
    
    setWindowTitle("BALSA Database");
    
    m_pageLogin = new BalsaDatabaseLoginPage(this);
    
    m_pageCreateZipFile = new BalsaDatabaseTestingPage(this);
 
    addPage(m_pageLogin);
    addPage(m_pageCreateZipFile);

    setOption(QWizard::NoCancelButton, false);
    setOption(QWizard::NoDefaultButton, false);
    setWizardStyle(QWizard::ModernStyle);
}

/**
 * Destructor.
 */
BalsaDatabaseDialog::~BalsaDatabaseDialog()
{
}

/* =============================================================================
 *
 * Login Page
 */

/**
 * Contruct Login page.
 *
 * @param parentDialog
 *     Parent BalsaDatabaseDialog.
 */
BalsaDatabaseLoginPage::BalsaDatabaseLoginPage(BalsaDatabaseDialog* parentDialog)
: QWizardPage(0),
m_parentDialog(parentDialog)
{
    setTitle("Login to the BALSA Database");
    setSubTitle("Enter username and password to login to the BALSA Database");
    
    QLabel* uploadLabel = new QLabel("<html>"
                                     "Login for "
                                     "<bold><a href=\"https://balsa.wustl.edu\">BALSA Database</a></bold>"
                                     "</html>");
    QObject::connect(uploadLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    
    const int minimumLineEditWidth = 250;
    
    QLabel* usernameLabel = new QLabel("Username: ");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setMinimumWidth(minimumLineEditWidth);
    
    QLabel* passwordLabel = new QLabel("Password: ");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setMinimumWidth(minimumLineEditWidth);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    
    QLabel* forgotUsernameLabel = new QLabel("<html>"
                                             "<bold><a href=\"https://balsa.wustl.edu/register/forgotUsername\">Forgot Username</a></bold>"
                                             "</html>");
    QObject::connect(forgotUsernameLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QLabel* forgotPasswordLabel = new QLabel("<html>"
                                             "<bold><a href=\"https://balsa.wustl.edu/register/forgotPassword\">Forgot Password</a></bold>"
                                             "</html>");
    QObject::connect(forgotPasswordLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QLabel* registerLabel = new QLabel("<html>"
                                       "<bold><a href=\"https://balsa.wustl.edu/register/register\">Register</a></bold>"
                                       "</html>");
    QObject::connect(registerLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(labelHtmlLinkClicked(const QString&)));
    
    QHBoxLayout* linkLabelsLayout = new QHBoxLayout();
    linkLabelsLayout->addSpacing(5);
    linkLabelsLayout->addWidget(forgotUsernameLabel);
    linkLabelsLayout->addStretch();
    linkLabelsLayout->addWidget(forgotPasswordLabel);
    linkLabelsLayout->addStretch();
    linkLabelsLayout->addWidget(registerLabel);
    linkLabelsLayout->addSpacing(5);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    int row = 0;
    gridLayout->addWidget(uploadLabel, row, 0, 1, 2);
    row++;
    gridLayout->addWidget(usernameLabel, row, 0);
    gridLayout->addWidget(m_usernameLineEdit, row, 1);
    row++;
    gridLayout->addWidget(passwordLabel, row, 0);
    gridLayout->addWidget(m_passwordLineEdit, row, 1);
    row++;
    gridLayout->addLayout(linkLabelsLayout, row, 0, 1, 2);
}


/**
 * Destructor.
 */
BalsaDatabaseLoginPage::~BalsaDatabaseLoginPage()
{
    
}

/**
 * Gets called when the user clicks a link in the forgot username
 * or password label.
 */
void
BalsaDatabaseLoginPage::labelHtmlLinkClicked(const QString& linkURL)
{
    if (linkURL.isEmpty() == false) {
        QDesktopServices::openUrl(QUrl(linkURL));
    }
}


/**
 * Returns true if Next/Finish page should be enabled
 * implying that the page's content is valid.
 */
bool
BalsaDatabaseLoginPage::isComplete() const
{
    return true;
}

/* =============================================================================
 *
 * Test Upload Page
 */

/**
 * Contruct Login page.
 *
 * @param parentDialog
 *     Parent BalsaDatabaseDialog.
 */
BalsaDatabaseTestingPage::BalsaDatabaseTestingPage(BalsaDatabaseDialog* parentDialog)
: QWizardPage(0),
m_parentDialog(parentDialog)
{
    setTitle("Create Zip File");
    setSubTitle("Creates the ZIP file that will be uploaded to the BALSA Database.");
    
    QLabel* zipFileNameLabel = new QLabel("Zip File Name");
    m_testingZipFileNameLineEdit = new QLineEdit;
    m_testingZipFileNameLineEdit->setText("Scene.zip");
    
    QLabel* extractDirectoryLabel = new QLabel("Extract to Directory");
    m_testingExtractDirectoryNameLineEdit = new QLineEdit();
    m_testingExtractDirectoryNameLineEdit->setText("ExtDir");
    
    QPushButton* zipScenePushButton = new QPushButton("Zip Scene File");
    QObject::connect(zipScenePushButton, SIGNAL(clicked()),
                     this, SLOT(runZipSceneFile()));
    
    QGridLayout* layout = new QGridLayout(this);
    int32_t row = 0;
    layout->addWidget(zipFileNameLabel, row, 0);
    layout->addWidget(m_testingZipFileNameLineEdit, row, 1);
    row++;
    layout->addWidget(extractDirectoryLabel, row, 0);
    layout->addWidget(m_testingExtractDirectoryNameLineEdit, row, 1);
    row++;
    layout->addWidget(zipScenePushButton, row, 0, 1, 2, Qt::AlignHCenter);
    row++;
    
    
}

BalsaDatabaseTestingPage::~BalsaDatabaseTestingPage()
{
    
}

bool
BalsaDatabaseTestingPage::isComplete() const
{
    return true;
}

void
BalsaDatabaseTestingPage::runZipSceneFile()
{
    const SceneFile* sceneFile = m_parentDialog->m_sceneFile;
    
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(this,
                               "Scene file is invalid.");
        return;
    }
    const QString sceneFileName = sceneFile->getFileName();
    if (sceneFileName.isEmpty()) {
        WuQMessageBox::errorOk(this, "Scene File does not have a name.");
        return;
    }
    
    const AString extractToDirectoryName = m_testingExtractDirectoryNameLineEdit->text().trimmed();
    if (extractToDirectoryName.isEmpty()) {
        WuQMessageBox::errorOk(this, "Extract to directory is empty.");
        return;
    }
    
    const AString zipFileName = m_testingZipFileNameLineEdit->text().trimmed();
    if (zipFileName.isEmpty()) {
        WuQMessageBox::errorOk(this, "Zip File name is empty");
        return;
    }
    
    AString baseDirectoryName;
    if ( ! sceneFile->getBaseDirectory().isEmpty()) {
        /* validate ? */
        baseDirectoryName = sceneFile->getBaseDirectory();
    }
    /*
     * Create parameters for running zip scene file command.
     * Need to use strdup() since QString::toAscii() returns
     * QByteArray instance that will go out of scope.  Use
     * strdup() for all parameters since "free" is later
     * used to free the memory allocated by strdup().
     */
    std::vector<char*> argvVector;
    argvVector.push_back(strdup("wb_command_in_wb_view"));
    argvVector.push_back(strdup(OperationZipSceneFile::getCommandSwitch().toAscii().constData()));
    argvVector.push_back(strdup(sceneFileName.toAscii().constData()));
    argvVector.push_back(strdup(extractToDirectoryName.toAscii().constData()));
    argvVector.push_back(strdup(zipFileName.toAscii().constData()));
    if ( ! baseDirectoryName.isEmpty()) {
        argvVector.push_back(strdup("-base-dir"));
        argvVector.push_back(strdup(baseDirectoryName.toAscii().constData()));
    }
    
    //    for (uint32_t i = 0; i < argvVector.size(); i++) {
    //        std::cout << "Zip Scene File Param " << i << ": " << argvVector[i] << std::endl;
    //    }
    
    
    try {
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        CommandOperationManager* cmdMgr = CommandOperationManager::getCommandOperationManager();
        ProgramParameters progParams(argvVector.size(),
                                     &argvVector[0]);
        cmdMgr->runCommand(progParams);
        
        cursor.restoreCursor();
        
        WuQMessageBox::informationOk(this, "Zip file successfully created.");
    }
    catch (const CaretException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    /*
     * Free memory from use of strdup().
     */
    for (std::vector<char*>::iterator charIter = argvVector.begin();
         charIter != argvVector.end();
         charIter++) {
        std::free(*charIter);
    }
    
}




