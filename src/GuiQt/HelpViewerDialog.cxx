
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __HELP_VIEWER_DIALOG_DECLARE__
#include "HelpViewerDialog.h"
#undef __HELP_VIEWER_DIALOG_DECLARE__

#include <QAction>
#include <QDesktopServices>
#include <QFile>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextBrowser>
#include <QSplitter>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CommandOperation.h"
#include "CommandOperationManager.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::HelpViewerDialog 
 * \brief Dialog that displays the applications help information.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent of this dialog.
 * @param f
 *    Qt's window flags.
 */
HelpViewerDialog::HelpViewerDialog(QWidget* parent,
                                   Qt::WindowFlags f)
: WuQDialogNonModal("Help",
                    parent,
                    f)
{
    setApplyButtonText("");

    /*
     * create the help browser
     */
    m_helpBrowser = new QTextBrowser;
    m_helpBrowser->setMinimumWidth(400);
    m_helpBrowser->setMinimumHeight(200);
    m_helpBrowser->setOpenExternalLinks(false);
    m_helpBrowser->setOpenLinks(false);
    QObject::connect(m_helpBrowser, SIGNAL(anchorClicked(const QUrl&)),
                     this, SLOT(helpPageAnchorClicked(const QUrl&)));
    
    /*
     * Create the tree widget for the help topics
     */
    QStringList indexTreeHeaderLabels;
    indexTreeHeaderLabels << "Help Page Index" << "Location";
    m_indexTreeWidget = new QTreeWidget;
    m_indexTreeWidget->setColumnCount(2);
    m_indexTreeWidget->setHeaderLabels(indexTreeHeaderLabels);
    m_indexTreeWidget->setColumnHidden(0, false);
    m_indexTreeWidget->setColumnHidden(1, true);
    QObject::connect(m_indexTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(indexTreeItemSelected(QTreeWidgetItem*,int)));
    /*
     * Search line edit and list widget
     */
    m_searchLineEdit = new QLineEdit;
    m_searchLineEdit->setText("Enter search here");
    QObject::connect(m_searchLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(slotSearchLineEdit()));
    QStringList searchTreeHeaderLabels;
    searchTreeHeaderLabels << "Matching Help Pages" << "Location";
    m_searchTreeWidget = new QTreeWidget;
    m_searchTreeWidget->setColumnCount(2);
    m_searchTreeWidget->setHeaderLabels(searchTreeHeaderLabels);
    m_searchTreeWidget->setColumnHidden(0, false);
    m_searchTreeWidget->setColumnHidden(1, true);
    QObject::connect(m_searchTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(searchTreeItemSelected(QTreeWidgetItem*,int)));
    QWidget* searchWidget = new QWidget;
    QVBoxLayout* searchLayout = new QVBoxLayout(searchWidget);
    searchLayout->addWidget(m_searchLineEdit);
    searchLayout->addWidget(m_searchTreeWidget);
    
    /*
     * create the back toolbar button
     */
    QAction* backwardAction = new QAction("Back", this);
    connect(m_helpBrowser, SIGNAL(backwardAvailable(bool)),
            backwardAction, SLOT(setEnabled(bool)));
    connect(backwardAction, SIGNAL(triggered()),
            m_helpBrowser, SLOT(backward()));
    QToolButton* backwardButton = new QToolButton;
    backwardButton->setDefaultAction(backwardAction);
    
    /*
     * Create the forward toolbar button
     */
    QAction* forewardAction = new QAction("Fwd", this);
    connect(m_helpBrowser, SIGNAL(forwardAvailable(bool)),
            forewardAction, SLOT(setEnabled(bool)));
    connect(forewardAction, SIGNAL(triggered()),
            m_helpBrowser, SLOT(forward()));
    QToolButton* forwardButton = new QToolButton;
    connect(m_helpBrowser, SIGNAL(forwardAvailable(bool)),
            forwardButton, SLOT(setEnabled(bool)));
    forwardButton->setDefaultAction(forewardAction);
    
    /*
     * Create the home toolbar button
     */
    QAction* homeAction = new QAction("Home", this);
    connect(homeAction, SIGNAL(triggered()),
            m_helpBrowser, SLOT(home()));
    QToolButton* homeButton = new QToolButton;
    homeButton->setDefaultAction(homeAction);
    
    /*
     * Create the print toolbar button
     */
    QAction* printAction = new QAction("Print", this);
    connect(printAction, SIGNAL(triggered()),
            this, SLOT(slotPrint()));
    QToolButton* printButton = new QToolButton;
    printButton->setDefaultAction(printAction);
    
    /*
     * Find button
     */
    QToolButton* findPushButton = new QToolButton;
    findPushButton->setText("Find");
    QObject::connect(findPushButton, SIGNAL(clicked()),
                     this, SLOT(slotFindInBrowser()));
    
    /*
     * Next button
     */
    m_findNextPushButton = new QToolButton;
    m_findNextPushButton->setText("Next");
    m_findNextPushButton->setEnabled(false);
    QObject::connect(m_findNextPushButton, SIGNAL(clicked()),
                     this, SLOT(slotFindNextInBrowser()));
    
    /*
     * Layout for toolbuttons
     */
    QHBoxLayout* toolButtonLayout = new QHBoxLayout;
    toolButtonLayout->addWidget(homeButton);
    toolButtonLayout->addWidget(backwardButton);
    toolButtonLayout->addWidget(forwardButton);
    toolButtonLayout->addWidget(printButton);
    toolButtonLayout->addWidget(findPushButton);
    toolButtonLayout->addWidget(m_findNextPushButton);
    toolButtonLayout->addStretch();
    
    /*
     * Layout for help browser and buttons
     */
    QWidget* helpBrowserWidgets = new QWidget;
    QVBoxLayout* helpBrowserLayout = new QVBoxLayout(helpBrowserWidgets);
    helpBrowserLayout->addLayout(toolButtonLayout);
    helpBrowserLayout->addWidget(m_helpBrowser);
    
    /*
     * Tab widget for index and search
     */
    QTabWidget* indexSearchTabWidget = new QTabWidget;
    indexSearchTabWidget->addTab(m_indexTreeWidget, "Index");
    indexSearchTabWidget->addTab(searchWidget, "Search");
    
    /*
     * Create the splitter and add the widgets to the splitter
     */
    m_splitter = new QSplitter;
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(indexSearchTabWidget);
    m_splitter->addWidget(helpBrowserWidgets);
    QList<int> sizeList;
    sizeList << 175 << 375;
    m_splitter->setSizes(sizeList);
    
    setCentralWidget(m_splitter,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    loadIndexTree();
}

/**
 * Destructor.
 */
HelpViewerDialog::~HelpViewerDialog()
{
}

/**
 * Update the content of the dialog.
 */
void
HelpViewerDialog::updateDialog()
{
}

/**
 * Show the help page with the given name.
 *
 * @param helpPageName
 *    Name of help page.
 */
void
HelpViewerDialog::showHelpPageWithName(const AString& helpPageName)
{
    CaretAssertMessage(0, "Not implmented yet.");
    const AString pageName = QString(helpPageName).replace('_', ' ');
    if (pageName.isEmpty()) {
        return;
    }
    
    //    for (int i = 0; i < m_topicTreeWidget->count(); i++) {
    //        QListWidgetItem* lwi = m_workbenchIndexListWidget->item(i);
    //        if (lwi->text() == pageName) {
    //            m_workbenchIndexListWidget->setCurrentItem(lwi);
    //            workbenchIndexListWidgetItemClicked(lwi);
    //            return;
    //        }
    //    }
    
    CaretLogSevere("Could not find help page \""
                   + helpPageName
                   + "\" for loading.");
}

/**
 * called to find in browser window.
 */
void
HelpViewerDialog::slotFindInBrowser()
{
    bool ok = false;
    const QString txt = QInputDialog::getText(this,
                                              "Find",
                                              "Find Text",
                                              QLineEdit::Normal,
                                              m_findInBrowserText,
                                              &ok);
    if (ok) {
        m_findNextPushButton->setEnabled(false);
        m_findInBrowserText = txt.trimmed();
        if (m_findInBrowserText.isEmpty() == false) {
            m_helpBrowser->moveCursor(QTextCursor::Start);
            if (m_helpBrowser->find(m_findInBrowserText)) {
                m_findNextPushButton->setEnabled(true);
            }
        }
    }
}

/**
 * called to find next in browser window.
 */
void
HelpViewerDialog::slotFindNextInBrowser()
{
    if (m_helpBrowser->find(m_findInBrowserText) == false) {
        m_helpBrowser->moveCursor(QTextCursor::Start);
        m_helpBrowser->find(m_findInBrowserText);
    }
}

/**
 * called to print currently displayed page.
 */
void
HelpViewerDialog::slotPrint()
{
    QPrinter printer;
    QPrintDialog* printDialog = new QPrintDialog(&printer, this);
    if (printDialog->exec() == QPrintDialog::Accepted) {
        m_helpBrowser->document()->print(&printer);
    }
}

/**
 * load the index tree.
 */
void
HelpViewerDialog::loadIndexTree()
{
    m_indexTreeWidget->blockSignals(true);
    
    
    QTreeWidgetItem* workbenchItem = new QTreeWidgetItem(m_indexTreeWidget,
                                                         TREE_ITEM_NONE);
    workbenchItem->setText(0, "Workbench");
    
    QTreeWidgetItem* menuItem = new QTreeWidgetItem(workbenchItem,
                                                    TREE_ITEM_NONE);
    menuItem->setText(0, "Menus");
    
    new HelpTreeWidgetItem(menuItem,
                           "wb_view Menu (Mac Only)",
                           ":/HelpFiles/wb_view_Menu.htm");
    new HelpTreeWidgetItem(menuItem,
                           "File Menu",
                           ":/HelpFiles/File_Menu_filt.htm");
    new HelpTreeWidgetItem(menuItem,
                           "Data Menu",
                           ":/HelpFiles/Data_Menu_filt.htm");
    new HelpTreeWidgetItem(menuItem,
                           "Preferences",
                           ":/HelpFiles/Preferences_filt.htm");
    new HelpTreeWidgetItem(menuItem,
                           "Splash Screen",
                           ":/HelpFiles/Splash_Screen_filt.htm");
    
//    createHelpPageFileItem("wb_view Menu (Mac Only)", ":/HelpFiles/wb_view_Menu.htm", menuItem);
//    createHelpPageFileItem("File Menu", ":/HelpFiles/File_Menu_filt.htm", menuItem);
//    createHelpPageFileItem("Data Menu", ":/HelpFiles/Data_Menu_filt.htm", menuItem);
//    
//    createHelpPageFileItem("Preferences", ":/HelpFiles/Preferences_filt.htm", workbenchItem);
//    createHelpPageFileItem("Splash Screen", ":/HelpFiles/Splash_Screen_filt.htm", workbenchItem);
    
//    QDir resourceDir(":/");
//    if (resourceDir.exists()) {
//        QDir helpFilesDir = resourceDir;
//        helpFilesDir.cd("HelpFiles");
//        if (helpFilesDir.exists()) {
//            QStringList htmlFileFilter;
//            htmlFileFilter << "*.htm" << "*.html";
//            QFileInfoList fileList = helpFilesDir.entryInfoList(htmlFileFilter,
//                                                               QDir::Files,
//                                                               QDir::Name);
//            if (fileList.size() > 0) {
//                for (int i = 0; i < fileList.size(); i++) {
//                    const QString pathName = fileList.at(i).absoluteFilePath();
//                    const QString indexName = fileList.at(i).baseName().replace('_', ' ');
//
//                    const QString resourcePathName = "qrc" + pathName;
//                    QTreeWidgetItem* twi = new QTreeWidgetItem(workbenchItem,
//                                                               TREE_ITEM_HELP_PAGE);
//                    twi->setText(0, indexName);
//                    twi->setData(0, Qt::UserRole, resourcePathName);
//                }
//            }
//            else {
//                CaretLogSevere("No HTML (*.html, *.htm) Help Files Found in Resource directory "
//                               + helpFilesDir.absolutePath()
//                               + " not found.");
//            }
//        }
//        else {
//            CaretLogSevere("Help Files Resource directory "
//                           + helpFilesDir.absolutePath()
//                           + " not found.");
//        }
//    }
//    else {
//        CaretLogSevere("Resource directory "
//                       + resourceDir.absolutePath()
//                       + " not found.");
//    }
    
    m_indexTreeWidget->setItemExpanded(menuItem,
                                       true);
    m_indexTreeWidget->setItemExpanded(workbenchItem,
                                       true);
    
    /*
     * Load commands
     */
    CommandOperationManager* commandOperationManager = CommandOperationManager::getCommandOperationManager();
    std::vector<CommandOperation*> commandOperations = commandOperationManager->getCommandOperations();
    
    if ( ! commandOperations.empty()) {
        QTreeWidgetItem* wbCommandItem = new QTreeWidgetItem(m_indexTreeWidget,
                                                             TREE_ITEM_NONE);
        wbCommandItem->setText(0, "wb_command");
        
        const uint64_t numberOfCommands = commandOperations.size();
        for (uint64_t i = 0; i < numberOfCommands; i++) {
            CommandOperation* op = commandOperations[i];
            
            /*
             * Gets added to its parent in constructor
             */
            new HelpTreeWidgetItem(wbCommandItem,
                                   op);
        }
        
        
        
//        std::map<AString, CommandOperation*> cmdMap;
//        const uint64_t numberOfCommands = commandOperations.size();
//        for (uint64_t i = 0; i < numberOfCommands; i++) {
//            CommandOperation* op = commandOperations[i];
//            cmdMap[op->getOperationShortDescription()] = op;
//        }
//        
//        QTreeWidgetItem* wbCommandItem = new QTreeWidgetItem(m_indexTreeWidget,
//                                                             TREE_ITEM_NONE);
//        wbCommandItem->setText(0, "wb_command");
//        
//        for (std::map<AString, CommandOperation*>::iterator iter = cmdMap.begin();
//             iter != cmdMap.end();
//             iter++) {
//            QTreeWidgetItem* twi = new QTreeWidgetItem(wbCommandItem,
//                                                       TREE_ITEM_WB_COMMAND);
//            twi->setText(0, iter->second->getOperationShortDescription());
//            QVariant commandPointer = qVariantFromValue((void*)iter->second);
//            twi->setData(0, Qt::UserRole, commandPointer);
//        }
    }
    
    m_indexTreeWidget->blockSignals(false);
}

/**
 * Create a help page item for HTML loaded from a file.
 *
 * @param topicName
 *    Name displayed in topics list.
 * @param filePath
 *    Path to the file.
 * @param parent
 *    Parent of item that is created.
 * @return
 *    Item that was created.
 */
QTreeWidgetItem*
HelpViewerDialog::createHelpPageFileItem(const AString& topicName,
                                         const AString& filePath,
                                         QTreeWidgetItem* parent) const
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent,
                                                TREE_ITEM_HELP_PAGE);
    item->setText(0, topicName);
    item->setData(0, Qt::UserRole, filePath);
    
    return item;
}

/**
 * called when an index tree item is clicked.
 *
 * @param item
 *     Tree widget item that was clicked.
 * @param column
 *     Column of item.
 */
void
HelpViewerDialog::indexTreeItemSelected(QTreeWidgetItem* item, int column)
{
    if (item != NULL) {
        m_helpBrowser->clear();
        
        HelpTreeWidgetItem* helpItem = dynamic_cast<HelpTreeWidgetItem*>(item);
        if (helpItem != NULL) {
            switch (helpItem->m_treeItemType) {
                case HelpTreeWidgetItem::TREE_ITEM_NONE:
                    break;
                case HelpTreeWidgetItem::TREE_ITEM_HELP_PAGE_URL:
                {
                    const AString resourceURL = helpItem->m_helpPageURL;
                    if (resourceURL.startsWith("qrc:")) {
                            QUrl url(resourceURL);
                            m_helpBrowser->setSource(url);
                    }
                    else {
                        QFile file(resourceURL);
                        if (file.exists()) {
                            QUrl url = QUrl::fromLocalFile(resourceURL);
                            m_helpBrowser->setSource(url);
                        }
                        else {
                            const QString msg("Help page \""
                                              + resourceURL
                                              + "\": not found");
                            CaretLogSevere(msg);
                        }
                    }
                }
                    break;
                case HelpTreeWidgetItem::TREE_ITEM_HELP_TEXT:
                    m_helpBrowser->setText(helpItem->m_helpText);
                    break;
            }
        }
        else {
            switch (item->type()) {
                case TREE_ITEM_NONE:
                    break;
                case TREE_ITEM_HELP_PAGE:
                {
                    QVariant itemData = item->data(column, Qt::UserRole);
                    
                    const QString resourceFileName = itemData.toString();
                    loadPageAtURL(resourceFileName);
                    
//                    if (resourceFileName.startsWith("qrc:")) {
//                        QString urlText = itemData.toString();
//                        if (! urlText.isEmpty()) {
//                            QUrl url(urlText);
//                            m_helpBrowser->setSource(url);
//                        }
//                    }
//                    else {
//                        QFile file(resourceFileName);
//                        if (file.exists()) {
//                            QUrl url = QUrl::fromLocalFile(resourceFileName);
//                            m_helpBrowser->setSource(url);
//                        }
//                        else {
//                            const QString msg("Help page \""
//                                              + resourceFileName
//                                              + "\": not found");
//                            CaretLogSevere(msg);
//                        }
//                        
//                        
//                        //                    if (file.open(QFile::ReadOnly)) {
//                        //                        QTextStream stream(&file);
//                        //                        m_helpBrowserWidget->setHtml(stream.readAll());
//                        //                    }
//                        //                    else {
//                        //                        const QString msg("Unable to open help page from \""
//                        //                                          + resourceFileName
//                        //                                          + "\": "
//                        //                                          + file.errorString());
//                        //                        CaretLogSevere(msg);
//                        //                    }
//                    }
                    
                }
                    break;
                case TREE_ITEM_WB_COMMAND:
                {
                    QVariant itemData = item->data(column, Qt::UserRole);
                    void* ptr = itemData.value<void*>();
                    CommandOperation* command = (CommandOperation*)ptr;
                    m_helpBrowser->setText(command->getHelpInformation("wb_command"));
                }
                    break;
            }
        }
    }
}

void
HelpViewerDialog::loadPageAtURL(const AString& pageUrlText)
{
    m_helpBrowser->clear();
    
    if (pageUrlText.startsWith("qrc:")) {
        QUrl url(pageUrlText);
        m_helpBrowser->setSource(url);
    }
    else {
        QFile file(pageUrlText);
        if (file.exists()) {
            QUrl url = QUrl::fromLocalFile(pageUrlText);
            m_helpBrowser->setSource(url);
        }
        else {
            const QString msg("Help page \""
                              + pageUrlText
                              + "\": not found");
            m_helpBrowser->setText(msg);
        }
    }
}


/**
 * Gets called when a help page link is clicked.
 *
 * @param url
 *    URL of link that was clicked.
 */
void
HelpViewerDialog::helpPageAnchorClicked(const QUrl& url)
{
    if (url.toString().startsWith("http")) {
        if (! QDesktopServices::openUrl(url)) {
            WuQMessageBox::errorOk(this, "Failed to load " + url.toString());
        }
    }
    else {
        loadPageAtURL(url.toString());
    }
    std::cout << "\n\nLink clicked: " << qPrintable(url.toString()) << std::endl << std::endl;
}


/**
 * called when a search tree item is clicked.
 *
 * @param item
 *     Tree widget item that was clicked.
 * @param column
 *     Column of item.
 */
void
HelpViewerDialog::searchTreeItemSelected(QTreeWidgetItem* item, int /*column*/)
{
//    const QString webPage(item->text(1));
//    
//    //std::cout << "Item selected is: " << webPage.toAscii().constData() << std::endl;
//    
//    if (webPage.isEmpty() == false) {
//        loadPage(webPage);
//        slotFindNextInBrowser();
//    }
}

/**
 * get all web page names and titles.
 *
 * @param pagesOut
 *    All help pages output.
 */
void
HelpViewerDialog::getAllWebPages(QVector<QPair<QString,QString> >& pagesOut) const
{
//    pagesOut.clear();
//    
//    //
//    // Search through the tree widget to find all items with URLs
//    //
//    const int numItems = m_indexTreeWidget->topLevelItemCount();
//    for (int i = 0; i < numItems; i++) {
//        const QTreeWidgetItem* topItem = m_indexTreeWidget->topLevelItem(i);
//        if (topItem->text(1).isEmpty() == false) {
//            const QString pageName = topItem->text(0);
//            const QString pageURL  = topItem->text(1);
//            pagesOut.push_back(qMakePair(pageName, pageURL));
//        }
//        
//        //
//        // Search children of this item
//        //
//        const int numSubItems = topItem->childCount();
//        for (int j = 0; j < numSubItems; j++) {
//            const QTreeWidgetItem* subItem = topItem->child(j);
//            if (subItem->text(1).isEmpty() == false) {
//                const QString pageName = subItem->text(0);
//                const QString pageURL  = subItem->text(1);
//                pagesOut.push_back(qMakePair(pageName, pageURL));
//            }
//        }
//    }
}

/**
 * called to search all help pages.
 */
void
HelpViewerDialog::slotSearchLineEdit()
{
//    searchTreeWidget->clear();
//    
//    const QString searchText = searchLineEdit->text();
//    if (searchText.isEmpty() == false) {
//        QVector<QPair<QString,QString> > pages;
//        getAllWebPages(pages);
//        for (int i = 0; i < pages.count(); i++) {
//            const QString pageTitle = pages[i].first.toAscii().constData();
//            const QString pageURL   = pages[i].second.toAscii().constData();
//            
//            //std::cout << "Searching: "
//            //          << pageTitle.toAscii().constData()
//            //          << std::endl;
//            if (FileUtilities::findTextInFile(pageURL, searchText, false)) {
//                //std::cout << "   Page matches "
//                //          << std::endl;
//                searchTreeWidget->addTopLevelItem(createTreeItem(pageTitle, pageURL));
//            }      
//        }
//        
//        findInBrowserText = searchText;
//        findNextPushButton->setEnabled(true);
//    }   
}


// ========================================================================= //

HelpTreeWidgetItem::HelpTreeWidgetItem(QTreeWidgetItem* parent,
                                       CommandOperation* commandOperation)
: QTreeWidgetItem(parent),
m_treeItemType(TREE_ITEM_HELP_TEXT)
{
    setText(0, commandOperation->getOperationShortDescription());
    m_helpText = commandOperation->getHelpInformation("wb_command");
}

HelpTreeWidgetItem::HelpTreeWidgetItem(QTreeWidgetItem* parent,
                                       const AString& itemText,
                                       const AString& helpPageURL)
: QTreeWidgetItem(parent),
m_treeItemType(TREE_ITEM_HELP_PAGE_URL)
{
    setText(0, itemText);
    m_helpPageURL = helpPageURL;
    
    QFile file(helpPageURL);
    if (file.exists()) {
        if (file.open(QFile::ReadOnly)) {
            QTextStream stream(&file);
            m_helpText = stream.readAll();
            file.close();
        }
        else {
            const QString msg("Error reading resource page \""
                              + helpPageURL
                              + "\": not found");
            m_helpText = msg;
            CaretLogSevere(msg);
        }
    }
    else {
        const QString msg("Help resource page \""
                          + helpPageURL
                          + "\": not found");
        m_helpText = msg;
        CaretLogSevere(msg);
    }
}

HelpTreeWidgetItem::~HelpTreeWidgetItem()
{
    
}

