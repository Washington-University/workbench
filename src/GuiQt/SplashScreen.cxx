
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
    
    ApplicationInformation appInfo;
    QLabel* workbenchLabel = new QLabel(appInfo.getNameForGuiLabel());
//    QLabel* workbenchLabel  = new QLabel("<html>"
//                                         "Connectome<br>"
//                                         "Workbench"
//                                         "</html>");
    workbenchLabel->setStyleSheet(labelStyle);
    workbenchLabel->setAlignment(Qt::AlignCenter);

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
    m_dataFileTreeWidget = new QTreeWidget();
    m_dataFileTreeWidget->setHeaderLabels(headerText);
    QObject::connect(m_dataFileTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(dataFileTreeWidgetItemClicked(QTreeWidgetItem*)));
    QObject::connect(m_dataFileTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                     this, SLOT(dataFileTreeWidgetItemDoubleClicked(QTreeWidgetItem*)));

    QScrollArea* treeScrollArea = new QScrollArea();
    treeScrollArea->setWidget(m_dataFileTreeWidget);
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
    
    const int32_t treeDesiredWidth = loadDataFileTreeWidget();
    
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
 * @return The selected data file name.
 */
AString 
SplashScreen::getSelectedDataFileName() const
{
    return m_selectedDataFileName;
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
SplashScreen::dataFileTreeWidgetItemClicked(QTreeWidgetItem* item)
{
    m_selectedDataFileName = "";
    
    if (item != NULL) {
        if ( ! item->isDisabled()) {
            m_selectedDataFileName = item->data(0, Qt::UserRole).toString();
        }
    }
}

/**
 * Called when spec file tree widget item is double-clicked.
 * @param item
 *    Item clicked.
 */
void 
SplashScreen::dataFileTreeWidgetItemDoubleClicked(QTreeWidgetItem* item)
{
    m_selectedDataFileName = "";
    
    if (item != NULL) {
        if ( ! item->isDisabled()) {
            m_selectedDataFileName = item->data(0, Qt::UserRole).toString();
        
            /*
             * Accept is like hitting OK button
             */
            this->accept();
        }
    }
}

/**
 * Called when a push button was added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 */
WuQDialogModal::DialogUserButtonResult 
SplashScreen::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_openOtherSpecFilePushButton) {
        chooseDataFileViaOpenFileDialog();
    }
    else {
        CaretAssertMessage(0, "Unrecognized user pushbutton clicked \""
                           + userPushButton->text()
                           + "\"");
    }
    return WuQDialogModal::RESULT_NONE;
}

/**
 * Choose a data file with the Open File Dialog.
 */
void 
SplashScreen::chooseDataFileViaOpenFileDialog()
{
    QStringList filenameFilterList;
    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SCENE));
    filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
    CaretFileDialog fd(this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::ExistingFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.selectNameFilter(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION));
    
    AString errorMessages;
    
    if (fd.exec()) {
        QStringList selectedFiles = fd.selectedFiles();
        if (selectedFiles.empty() == false) {   
            m_selectedDataFileName = selectedFiles.at(0);
            
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
SplashScreen::loadDataFileTreeWidget()
{
    m_dataFileTreeWidget->clear();
    
    QTreeWidgetItem* selectedItem = addDirectorySpecFiles();
    
    if (selectedItem != NULL) {
        addDirectorySceneFiles();
    }
    else {
        selectedItem = addDirectorySceneFiles();
    }
    
    QTreeWidgetItem* specItem = addRecentSpecFiles();
    if (specItem != NULL) {
        selectedItem = specItem;
    }
    
    QTreeWidgetItem* sceneItem = addRecentSceneFiles();
    if (selectedItem == NULL) {
        selectedItem = sceneItem;
    }
    
    int nameColWidth = 0;
    int pathColWidth = 0;
    if (selectedItem != NULL) {
        m_dataFileTreeWidget->setCurrentItem(selectedItem);
        
        nameColWidth = m_dataFileTreeWidget->QAbstractItemView::sizeHintForColumn(0) + 25;
        pathColWidth = m_dataFileTreeWidget->QAbstractItemView::sizeHintForColumn(1);
        m_dataFileTreeWidget->setColumnWidth(0,
                                             nameColWidth);
        
        this->dataFileTreeWidgetItemClicked(selectedItem);
    }
    
    int treeWidgetWidth = (nameColWidth
                           + pathColWidth);
    if (treeWidgetWidth < 250) {
        treeWidgetWidth = 250;
    }
    
    m_dataFileTreeWidget->setMinimumWidth(treeWidgetWidth);
    
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
            m_dataFileTreeWidget->addTopLevelItem(titleItem);
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
                fullPath = fileInfo.getAbsoluteFilePath();
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
            
            m_dataFileTreeWidget->addTopLevelItem(lwi);
            
            if (firstItem == NULL) {
                firstItem = lwi;
            }
        }
    } 
    
    return firstItem;
}

/**
 * Add recent scene files.
 */
QTreeWidgetItem*
SplashScreen::addRecentSceneFiles()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    std::vector<AString> recentSceneFiles;
    prefs->getPreviousSceneFiles(recentSceneFiles);
    
    QTreeWidgetItem* firstItem = NULL;
    
    const int32_t numRecentSceneFiles = static_cast<int>(recentSceneFiles.size());
    for (int32_t i = 0; i < numRecentSceneFiles; i++) {
        QString path;
        QString name;
        QString fullPath;
        
        if (firstItem == NULL) {
            QStringList itemText;
            itemText.append("Recent Scene Files");
            itemText.append("-------------------------------");
            QTreeWidgetItem* titleItem = new QTreeWidgetItem(itemText);
            titleItem->setDisabled(true);
            m_dataFileTreeWidget->addTopLevelItem(titleItem);
        }
        
        const QString sceneFileName = recentSceneFiles[i];
        if (DataFile::isFileOnNetwork(sceneFileName)) {
            const int lastSlash = sceneFileName.lastIndexOf('/');
            name = sceneFileName.mid(lastSlash + 1);
            path = sceneFileName.left(lastSlash);
            fullPath = sceneFileName;
        }
        else {
            FileInformation fileInfo(sceneFileName);
            path = fileInfo.getPathName().trimmed();
            name = fileInfo.getFileName().trimmed();
            fullPath = fileInfo.getAbsoluteFilePath();
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
            
            m_dataFileTreeWidget->addTopLevelItem(lwi);
            
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
            itemText.append("Current Directory Spec Files");
            itemText.append(dirName);
            QTreeWidgetItem* titleItem = new QTreeWidgetItem(itemText);
            titleItem->setDisabled(true);
            m_dataFileTreeWidget->addTopLevelItem(titleItem);
        }
        
        FileInformation fileInfo(specFileList.at(i));
        const QString name = fileInfo.getFileName().trimmed();
        const QString fullPath = fileInfo.getAbsoluteFilePath();
        
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
        m_dataFileTreeWidget->addTopLevelItem(lwi);

        if (firstItem == NULL) {
            firstItem = lwi;
        }
    }
    
    return firstItem;
}

/**
 * Add scene files in current directory
 */
QTreeWidgetItem*
SplashScreen::addDirectorySceneFiles()
{
    Brain* brain = GuiManager::get()->getBrain();
    const QString dirName = brain->getCurrentDirectory();
    
    QTreeWidgetItem* firstItem = NULL;
    
    QStringList nameFilters;
    nameFilters.append("*." + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SCENE));
    QDir dir(dirName);
    QStringList sceneFileList = dir.entryList(nameFilters,
                                             QDir::Files,
                                             QDir::Name);
    const int32_t numFiles = sceneFileList.count();
    for (int32_t i = 0; i < numFiles; i++) {
        if (firstItem == NULL) {
            QStringList itemText;
            itemText.append("Current Directory Scene");
            itemText.append(dirName);
            QTreeWidgetItem* titleItem = new QTreeWidgetItem(itemText);
            titleItem->setDisabled(true);
            m_dataFileTreeWidget->addTopLevelItem(titleItem);
        }
        
        FileInformation fileInfo(sceneFileList.at(i));
        const QString name = fileInfo.getFileName().trimmed();
        const QString fullPath = fileInfo.getAbsoluteFilePath();
        
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
        m_dataFileTreeWidget->addTopLevelItem(lwi);
        
        if (firstItem == NULL) {
            firstItem = lwi;
        }
    }
    
    return firstItem;
}


