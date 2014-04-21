
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <QAction>
#include <QDesktopServices>
#include <QDir>
#include <QIcon>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>
#include <QTreeWidget>
#include <QUrl>
#include <QVBoxLayout>

#define __SPLASH_SCREEN_DECLARE__
#include "SplashScreen.h"
#undef __SPLASH_SCREEN_DECLARE__

#include "ApplicationInformation.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretPreferences.h"
#include "DataFile.h"
#include "DataFileTypeEnum.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "SessionManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::SplashScreen 
 * \brief Splash Screen display when Workbench is started.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
SplashScreen::SplashScreen(QWidget* parent)
: WuQDialogModal("",
                 parent)
{
    /*
     * Removes title bar
     */
//    this->setWindowFlags(Qt::SplashScreen);

    QLabel* imageLabel = NULL;
    QPixmap pixmap;
    if (WuQtUtilities::loadPixmap(":/Splash/startup_image.png", pixmap)) {
        imageLabel = new QLabel();
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
    }

    const QString labelStyle = ("QLabel { "
                                " font: 20px bold "
                                "}");
    
    QLabel* workbenchLabel  = new QLabel("<html>"
                                         "Connectome<br>"
                                         "Workbench"
                                         "</html>");
    workbenchLabel->setStyleSheet(labelStyle);
    workbenchLabel->setAlignment(Qt::AlignCenter);

    ApplicationInformation appInfo;
    QLabel* versionLabel = new QLabel("Version: "
                                      + appInfo.getVersion());
    versionLabel->setAlignment(Qt::AlignCenter);
    
    QLabel* hcpWebsiteLabel = new QLabel("<html>"
                                         "Visit<br>"
                                         "<bold><a href=\"http://www.humanconnectome.org\">Human Connectome Project</a></bold><br>"
                                         "Website"
                                         "</html>");
    hcpWebsiteLabel->setStyleSheet(labelStyle);
    hcpWebsiteLabel->setAlignment(Qt::AlignCenter);
    QObject::connect(hcpWebsiteLabel, SIGNAL(linkActivated(const QString&)),
                     this, SLOT(websiteLinkActivated(const QString&)));

    QToolButton* twitterToolButton = NULL;
    QIcon twitterIcon;
    if (WuQtUtilities::loadIcon(":/Splash/twitter.png", twitterIcon)) {
        QAction* twitterAction = WuQtUtilities::createAction("Twitter",
                                                             "Follow HCP on Twitter",
                                                             this,
                                                             this,
                                                             SLOT(twitterActionTriggered()));
        twitterAction->setIcon(twitterIcon);
        twitterToolButton = new QToolButton();
        twitterToolButton->setDefaultAction(twitterAction);
    }
    
    QStringList headerText;
    headerText.append("Spec File");
    headerText.append("Path");
    m_specFileTreeWidget = new QTreeWidget();
    m_specFileTreeWidget->setHeaderLabels(headerText);
    QObject::connect(m_specFileTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(specFileTreeWidgetItemClicked(QTreeWidgetItem*)));
    QObject::connect(m_specFileTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                     this, SLOT(specFileTreeWidgetItemDoubleClicked(QTreeWidgetItem*)));

    QScrollArea* treeScrollArea = new QScrollArea();
    treeScrollArea->setWidget(m_specFileTreeWidget);
    treeScrollArea->setWidgetResizable(true);
    
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftColumnLayout = new QVBoxLayout(leftWidget);
    if (imageLabel != NULL) {
        leftColumnLayout->addWidget(imageLabel);
        leftColumnLayout->addSpacing(15);
    }
    leftColumnLayout->addWidget(workbenchLabel);
    leftColumnLayout->addWidget(versionLabel);
    leftColumnLayout->addSpacing(10);
    leftColumnLayout->addWidget(hcpWebsiteLabel);
    if (twitterToolButton != NULL) {
        leftColumnLayout->addSpacing(3);
        leftColumnLayout->addWidget(twitterToolButton, 0, Qt::AlignHCenter);
    }
    leftColumnLayout->addStretch();
    
    QWidget* widget = new QWidget();
    QHBoxLayout* horizLayout = new QHBoxLayout(widget);
    horizLayout->addWidget(leftWidget);
    horizLayout->addWidget(treeScrollArea);
    
    const int32_t treeDesiredWidth = loadSpecFileTreeWidget();
    
    m_openOtherSpecFilePushButton = addUserPushButton("Open Other...",
                                                      QDialogButtonBox::AcceptRole);
    
    setCancelButtonText("Skip");
    setOkButtonText("Open");

    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);

    /*
     * Set a minimum width so spec files are visible
     */
    int totalWidth = (leftWidget->sizeHint().width()
                      + treeDesiredWidth);
    const int maxWidth = std::max(WuQtUtilities::getMinimumScreenSize().width() - 300,
                                  600);
    if (totalWidth > maxWidth) {
        totalWidth = maxWidth;
    }
    widget->setMinimumWidth(totalWidth);
}

/**
 * Destructor.
 */
SplashScreen::~SplashScreen()
{
    
}

/**
 * @return The selected spec file name.
 */
AString 
SplashScreen::getSelectedSpecFileName() const
{
    return m_selectedSpecFileName;
}

/**
 * Called when a label's hyperlink is selected.
 * @param link
 *   The URL.
 */
void
SplashScreen::websiteLinkActivated(const QString& link)
{
    if (link.isEmpty() == false) {
        QDesktopServices::openUrl(QUrl(link));
    }
}

/**
 * Display twitter page in web browser.
 */
void 
SplashScreen::twitterActionTriggered()
{
    websiteLinkActivated("http://twitter.com/#!/HumanConnectome");
}

/**
 * Called when spec file tree widget item is clicked.
 * @param item
 *    Item clicked.
 */
void 
SplashScreen::specFileTreeWidgetItemClicked(QTreeWidgetItem* item)
{
    m_selectedSpecFileName = "";
    
    if (item != NULL) {
        m_selectedSpecFileName = item->data(0, Qt::UserRole).toString();
    }
}

/**
 * Called when spec file tree widget item is double-clicked.
 * @param item
 *    Item clicked.
 */
void 
SplashScreen::specFileTreeWidgetItemDoubleClicked(QTreeWidgetItem* item)
{
    m_selectedSpecFileName = "";
    
    if (item != NULL) {
        m_selectedSpecFileName = item->data(0, Qt::UserRole).toString();
        
        /*
         * Accept is like hitting OK button
         */
        this->accept();
    }
}

/**
 * Called when a push button was added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 */
WuQDialogModal::ModalDialogUserButtonResult 
SplashScreen::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_openOtherSpecFilePushButton) {
        chooseSpecFileViaOpenFileDialog();
    }
    else {
        CaretAssertMessage(0, "Unrecognized user pushbutton clicked \""
                           + userPushButton->text()
                           + "\"");
    }
    return WuQDialogModal::RESULT_NONE;
}

/**
 * Choose a spec file with the Open File Dialog.
 */
void 
SplashScreen::chooseSpecFileViaOpenFileDialog()
{
    QStringList filenameFilterList;
    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
    CaretFileDialog fd(this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::ExistingFile);
    fd.setViewMode(CaretFileDialog::List);
    
    AString errorMessages;
    
    if (fd.exec()) {
        QStringList selectedFiles = fd.selectedFiles();
        if (selectedFiles.empty() == false) {   
            m_selectedSpecFileName = selectedFiles.at(0);
            
            /*
             * Accept indicates user has 'accepted' the
             * dialog so dialog is closed (like OK clicked)
             */
            accept();
        }
    }
}

/**
 * Load Spec Files into the tree widget.
 */
int32_t 
SplashScreen::loadSpecFileTreeWidget()
{
    m_specFileTreeWidget->clear();
    
    QTreeWidgetItem* selectedItem = addDirectorySpecFiles();
    
    QTreeWidgetItem* specItem = addRecentSpecFiles();
    if (specItem != NULL) {
        selectedItem = specItem;
    }
    
    int nameColWidth = 0;
    int pathColWidth = 0;
    if (selectedItem != NULL) {
        m_specFileTreeWidget->setCurrentItem(selectedItem);
        
        nameColWidth = m_specFileTreeWidget->QAbstractItemView::sizeHintForColumn(0) + 25;
        pathColWidth = m_specFileTreeWidget->QAbstractItemView::sizeHintForColumn(1);
        m_specFileTreeWidget->setColumnWidth(0,
                                             nameColWidth);
        
        this->specFileTreeWidgetItemClicked(selectedItem);
    }
    
    int treeWidgetWidth = (nameColWidth
                           + pathColWidth);
    if (treeWidgetWidth < 250) {
        treeWidgetWidth = 250;
    }
    
    m_specFileTreeWidget->setMinimumWidth(treeWidgetWidth);
    
    return treeWidgetWidth;
}

/**
 * Add recent spec files.
 */
QTreeWidgetItem* 
SplashScreen::addRecentSpecFiles()
{    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    std::vector<AString> recentSpecFiles;
    prefs->getPreviousSpecFiles(recentSpecFiles);
    
    QTreeWidgetItem* firstItem = NULL;
    
    const int32_t numRecentSpecFiles = static_cast<int>(recentSpecFiles.size());
    for (int32_t i = 0; i < numRecentSpecFiles; i++) {
        QString path;
        QString name;
        QString fullPath;
        
        if (firstItem == NULL) {
            QStringList itemText;
            itemText.append("Recent Spec Files");
            itemText.append("-------------------------------");
            QTreeWidgetItem* titleItem = new QTreeWidgetItem(itemText);
            titleItem->setDisabled(true);
            m_specFileTreeWidget->addTopLevelItem(titleItem);
        }
        
        const QString specFileName = recentSpecFiles[i];
        if (DataFile::isFileOnNetwork(specFileName)) {
            const int lastSlash = specFileName.lastIndexOf('/');
            name = specFileName.mid(lastSlash + 1);
            path = specFileName.left(lastSlash);
            fullPath = specFileName;
        }
        else {
            FileInformation fileInfo(specFileName);
                path = fileInfo.getPathName().trimmed();
                name = fileInfo.getFileName().trimmed();
                fullPath = fileInfo.getFilePath();
        }
            
        if (name.isEmpty() == false) {
            QStringList treeText;
            treeText.append("    " + name);
            treeText.append(path);
            
            QTreeWidgetItem* lwi = new QTreeWidgetItem(treeText);
            lwi->setData(0,
                         Qt::UserRole, 
                         fullPath);            
            lwi->setData(1,
                         Qt::UserRole, 
                         fullPath);   
            
            m_specFileTreeWidget->addTopLevelItem(lwi);
            
            if (firstItem == NULL) {
                firstItem = lwi;
            }
        }
    } 
    
    return firstItem;
}

/**
 * Add spec files in current directory
 */
QTreeWidgetItem* 
SplashScreen::addDirectorySpecFiles()
{
    Brain* brain = GuiManager::get()->getBrain();
    const QString dirName = brain->getCurrentDirectory();
    
    QTreeWidgetItem* firstItem = NULL;
    
    QStringList nameFilters;
    nameFilters.append("*." + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION));
    QDir dir(dirName);
    QStringList specFileList = dir.entryList(nameFilters,
                                             QDir::Files,
                                             QDir::Name);
    const int32_t numFiles = specFileList.count();
    for (int32_t i = 0; i < numFiles; i++) {
        if (firstItem == NULL) {
            QStringList itemText;
            itemText.append("Current Directory");
            itemText.append(dirName);
            QTreeWidgetItem* titleItem = new QTreeWidgetItem(itemText);
            titleItem->setDisabled(true);
            m_specFileTreeWidget->addTopLevelItem(titleItem);
        }
        
        FileInformation fileInfo(specFileList.at(i));
        const QString name = fileInfo.getFileName().trimmed();
        const QString fullPath = fileInfo.getFilePath();
        
        QStringList treeText;
        treeText.append("    " + name);
        treeText.append(" . "); // Use . for current directory
        
        QTreeWidgetItem* lwi = new QTreeWidgetItem(treeText);
        lwi->setData(0,
                     Qt::UserRole, 
                     fullPath);            
        lwi->setData(1,
                     Qt::UserRole, 
                     fullPath);            
        m_specFileTreeWidget->addTopLevelItem(lwi);

        if (firstItem == NULL) {
            firstItem = lwi;
        }
    }
    
    return firstItem;
}


