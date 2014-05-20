
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
#include "XmlUtilities.h"

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
    m_topicSearchLineEditFirstMouseClick = true;
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
    m_topicIndexTreeWidget = new QTreeWidget;
    m_topicIndexTreeWidget->setColumnCount(2);
    m_topicIndexTreeWidget->setHeaderLabels(indexTreeHeaderLabels);
    m_topicIndexTreeWidget->setColumnHidden(0, false);
    m_topicIndexTreeWidget->setColumnHidden(1, true);
    QObject::connect(m_topicIndexTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(topicIndexTreeItemSelected(QTreeWidgetItem*,int)));
    /*
     * Search line edit and list widget
     */
    m_topicSearchLineEdit = new QLineEdit;
    m_topicSearchLineEdit->setText("Enter search text here");
    QObject::connect(m_topicSearchLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(topicSearchLineEditStartSearch()));
    QObject::connect(m_topicSearchLineEdit, SIGNAL(textEdited(const QString&)),
                     this, SLOT(topicSearchLineEditStartSearch()));
    QObject::connect(m_topicSearchLineEdit, SIGNAL(cursorPositionChanged(int,int)),
                     this, SLOT(topicSearchLineEditCursorPositionChanged(int,int)));
    QStringList searchTreeHeaderLabels;
    searchTreeHeaderLabels << "Matching Help Pages" << "Location";
    m_topicSearchTreeWidget = new QTreeWidget;
    m_topicSearchTreeWidget->setColumnCount(2);
    m_topicSearchTreeWidget->setHeaderLabels(searchTreeHeaderLabels);
    m_topicSearchTreeWidget->setColumnHidden(0, false);
    m_topicSearchTreeWidget->setColumnHidden(1, true);
    QObject::connect(m_topicSearchTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(topicSearchTreeItemSelected(QTreeWidgetItem*,int)));
    QWidget* searchWidget = new QWidget;
    QVBoxLayout* searchLayout = new QVBoxLayout(searchWidget);
    searchLayout->addWidget(m_topicSearchLineEdit);
    searchLayout->addWidget(m_topicSearchTreeWidget);
    
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
            this, SLOT(helpTextPrintButtonClicked()));
    QToolButton* printButton = new QToolButton;
    printButton->setDefaultAction(printAction);
    
    /*
     * Find button
     */
    QToolButton* findPushButton = new QToolButton;
    findPushButton->setText("Find");
    QObject::connect(findPushButton, SIGNAL(clicked()),
                     this, SLOT(helpTextFindButtonClicked()));
    
    /*
     * Next button
     */
    m_findNextPushButton = new QToolButton;
    m_findNextPushButton->setText("Next");
    m_findNextPushButton->setEnabled(false);
    QObject::connect(m_findNextPushButton, SIGNAL(clicked()),
                     this, SLOT(helpTextFindNextButtonClicked()));
    
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
    indexSearchTabWidget->addTab(m_topicIndexTreeWidget, "Index");
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
    
    loadHelpTopicsIntoIndexTree();
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
HelpViewerDialog::helpTextFindButtonClicked()
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
HelpViewerDialog::helpTextFindNextButtonClicked()
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
HelpViewerDialog::helpTextPrintButtonClicked()
{
    QPrinter printer;
    QPrintDialog* printDialog = new QPrintDialog(&printer, this);
    if (printDialog->exec() == QPrintDialog::Accepted) {
        m_helpBrowser->document()->print(&printer);
    }
}

/**
 * load the index tree with the help topics.
 */
void
HelpViewerDialog::loadHelpTopicsIntoIndexTree()
{
    m_topicIndexTreeWidget->blockSignals(true);
    
    
    QTreeWidgetItem* workbenchItem = new QTreeWidgetItem(m_topicIndexTreeWidget,
                                                         TREE_ITEM_NONE);
    workbenchItem->setText(0, "Workbench");
    
    QTreeWidgetItem* menuItem = new QTreeWidgetItem(workbenchItem,
                                                    TREE_ITEM_NONE);
    menuItem->setText(0, "Menus");
    
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "wb_view Menu (Mac Only)",
                           ":/HelpFiles/wb_view_Menu.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "File Menu",
                           ":/HelpFiles/File_Menu_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "View Menu",
                           ":/HelpFiles/View_Menu_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "Data Menu",
                           ":/HelpFiles/Data_Menu_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "Surface Menu",
                           ":/HelpFiles/Surface_Menu_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "Connect Menu",
                           ":/HelpFiles/Connect_Menu_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "Window Menu",
                           ":/HelpFiles/Window_Menu_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "Help Menu",
                           ":/HelpFiles/Help_Menu_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "Preferences",
                           ":/HelpFiles/Preferences_filt.htm");
    createHelpTreeWidgetItemForHelpPage(menuItem,
                           "Splash Screen",
                           ":/HelpFiles/Splash_Screen_filt.htm");
    
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
    
    m_topicIndexTreeWidget->setItemExpanded(menuItem,
                                       true);
    m_topicIndexTreeWidget->setItemExpanded(workbenchItem,
                                       true);
    
    /*
     * Load commands
     */
    CommandOperationManager* commandOperationManager = CommandOperationManager::getCommandOperationManager();
    std::vector<CommandOperation*> commandOperations = commandOperationManager->getCommandOperations();
    
    if ( ! commandOperations.empty()) {
        /*
         * Use map to sort commands by short description
         */
        std::map<QString, CommandOperation*> sortCommandsMap;
        for (std::vector<CommandOperation*>::iterator vecIter = commandOperations.begin();
             vecIter != commandOperations.end();
             vecIter++) {
            CommandOperation* op = *vecIter;
            sortCommandsMap.insert(std::make_pair(op->getOperationShortDescription(),
                                                  op));
        }
        
        QTreeWidgetItem* wbCommandItem = new QTreeWidgetItem(m_topicIndexTreeWidget,
                                                             TREE_ITEM_NONE);
        wbCommandItem->setText(0, "wb_command");
        
        for (std::map<QString, CommandOperation*>::iterator mapIter = sortCommandsMap.begin();
             mapIter != sortCommandsMap.end();
             mapIter++) {
            CommandOperation* op = mapIter->second;
            
            HelpTreeWidgetItem* item
            = HelpTreeWidgetItem::newInstanceForCommandOperation(wbCommandItem,
                                                                                           op);
            m_allHelpWidgetItems.push_back(item);
        }
//        const uint64_t numberOfCommands = commandOperations.size();
//        for (uint64_t i = 0; i < numberOfCommands; i++) {
//            CommandOperation* op = commandOperations[i];
//            
//            /*
//             * Gets added to its parent in constructor
//             */
//            HelpTreeWidgetItem* item = new HelpTreeWidgetItem(wbCommandItem,
//                                                              op);
//            m_allHelpWidgetItems.push_back(item);
//        }
    }
    
    m_topicIndexTreeWidget->blockSignals(false);
}

/**
 * Create a help tree widget item for a help page URL.
 *
 * @param parent
 *    Parent for item in index.
 * @param itemText
 *    Text for the item shown in the topic index.
 * @param helpPageURL
 *    URL for the help page.
 */
HelpTreeWidgetItem*
HelpViewerDialog::createHelpTreeWidgetItemForHelpPage(QTreeWidgetItem* parent,
                                                      const AString& itemText,
                                                      const AString& helpPageURL)
{
    HelpTreeWidgetItem* helpItem = HelpTreeWidgetItem::newInstanceForHtmlHelpPage(parent,
                                                                                  itemText,
                                                                                  helpPageURL);
    
    m_allHelpWidgetItems.push_back(helpItem);
    
    return helpItem;
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
HelpViewerDialog::topicIndexTreeItemSelected(QTreeWidgetItem* item, int /*column*/)
{
    if (item != NULL) {
        /*
         * Note not all items are castable to HelpTreeWidgetItem.
         * Items not castable are category items that have an arrow to
         * expand/collapse its children.
         */
        HelpTreeWidgetItem* helpItem = dynamic_cast<HelpTreeWidgetItem*>(item);
        if (helpItem != NULL) {
            displayHelpTextForHelpTreeWidgetItem(helpItem);
        }
    }
}

/**
 * Display the help information for the given help item.
 *
 * @param helpItem
 *    Item for which help text is loaded.
 */
void
HelpViewerDialog::displayHelpTextForHelpTreeWidgetItem(HelpTreeWidgetItem* helpItem)
{
    CaretAssert(helpItem);
    switch (helpItem->m_treeItemType) {
        case HelpTreeWidgetItem::TREE_ITEM_NONE:
            break;
        case HelpTreeWidgetItem::TREE_ITEM_HELP_PAGE_URL:
            displayHttpInUsersWebBrowser(helpItem->m_helpPageURL);
            break;
        case HelpTreeWidgetItem::TREE_ITEM_HELP_TEXT:
            m_helpBrowser->clear();
            m_helpBrowser->setText(helpItem->m_helpText);
            break;
    }
}

/**
 * For an external (http) link, load it into the user's web browser.
 *
 * @param urlText
 *     Text of the URL.
 */
void
HelpViewerDialog::displayHttpInUsersWebBrowser(const AString& urlText)
{
    if (WuQMessageBox::warningOkCancel(this, "The link clicked will be displayed in your web browser.")) {
        if (! QDesktopServices::openUrl(urlText)) {
            WuQMessageBox::errorOk(this,
                                   ("Failed to load "
                                    + urlText));
        }
    }
}


/**
 * Gets called when an HTML link is clicked in the text of a help page.
 *
 * @param url
 *    URL of link that was clicked.
 */
void
HelpViewerDialog::helpPageAnchorClicked(const QUrl& url)
{
    const AString urlText = url.toString();
    if (urlText.startsWith("http")) {
        displayHttpInUsersWebBrowser(urlText);
    }
    else {
        const AString path = url.path();
        if (path.isEmpty()) {
            CaretLogSevere("No path from URL: "
                           + url.toString());
            WuQMessageBox::errorOk(this, "Error: clicked link \""
                                   + url.toString()
                                   + "\" appears to be invalid.");
        }
        else {
            AString htmlPageName = path;
            const int32_t slashPos = htmlPageName.lastIndexOf("/");
            if (slashPos >= 0) {
                htmlPageName = htmlPageName.mid(slashPos + 1);
            }
            bool foundMatchingPage = false;
            
            for (std::vector<HelpTreeWidgetItem*>::iterator iter = m_allHelpWidgetItems.begin();
                 iter != m_allHelpWidgetItems.end();
                 iter++) {
                HelpTreeWidgetItem* item = *iter;
                CaretAssert(item);
                if (item->m_helpPageURL.endsWith(htmlPageName)) {
                    displayHelpTextForHelpTreeWidgetItem(item);
                    foundMatchingPage = true;
                    break;
                }
            }
            
            if ( ! foundMatchingPage) {
                WuQMessageBox::errorOk(this,
                                       ("Unable to find matching help page for \""
                                        + url.toString()
                                        + "\""));
            }
        }
    }
}

/**
 * called when a topic search tree item is clicked.
 *
 * @param item
 *     Tree widget item that was clicked.
 * @param column
 *     Column of item.
 */
void
HelpViewerDialog::topicSearchTreeItemSelected(QTreeWidgetItem* item, int /*column*/)
{
    CaretAssert(item);
    
    void* helpItemPointer = item->data(0, Qt::UserRole).value<void*>();
    HelpTreeWidgetItem* helpItem = (HelpTreeWidgetItem*)helpItemPointer;
    displayHelpTextForHelpTreeWidgetItem(helpItem);
    helpTextFindNextButtonClicked();
}

/**
 * Called when search text is changed or return pressed to start
 * searching the help topics
 */
void
HelpViewerDialog::topicSearchLineEditStartSearch()
{
    m_topicSearchTreeWidget->clear();
    
    const QString searchText = m_topicSearchLineEdit->text();
    if ( ! searchText.isEmpty()) {
        for (std::vector<HelpTreeWidgetItem*>::iterator iter = m_allHelpWidgetItems.begin();
             iter != m_allHelpWidgetItems.end();
             iter++) {
            HelpTreeWidgetItem* helpItem = *iter;
            if (helpItem->m_helpText.contains(searchText)) {
                QTreeWidgetItem* searchItem = new QTreeWidgetItem(m_topicSearchTreeWidget);
                searchItem->setText(0, helpItem->text(0));
                
                void* helpItemPointer = (void*)helpItem;
                searchItem->setData(0, Qt::UserRole,
                                    qVariantFromValue<void*>(helpItemPointer));
            }
        }

        m_findInBrowserText = searchText;
        m_findNextPushButton->setEnabled(true);
    }   
}

/**
 * Called when the cursor position is changed
 */
void
HelpViewerDialog::topicSearchLineEditCursorPositionChanged(int,int)
{
    if (m_topicSearchLineEditFirstMouseClick) {
        m_topicSearchLineEditFirstMouseClick = false;
        m_topicSearchLineEdit->clear();
    }
}



// ========================================================================= //


/**
 * Create a new help tree widget item for a wb_command item.
 *
 * @param parent
 *    Parent for item in index.
 * @param commandOperation
 *    The command.
 */
HelpTreeWidgetItem*
HelpTreeWidgetItem::newInstanceForCommandOperation(QTreeWidgetItem* parent,
                                                   CommandOperation* commandOperation)
{
    const AString itemText = commandOperation->getOperationShortDescription();
    const AString helpInfoCopy = commandOperation->getHelpInformation("wb_command");
    const AString helpText = helpInfoCopy.convertToHtmlPage();
    const AString helpPageURL("");
    
    HelpTreeWidgetItem* instance = new HelpTreeWidgetItem(parent,
                                                          TREE_ITEM_HELP_TEXT,
                                                          itemText,
                                                          helpPageURL,
                                                          helpText);
    return instance;
}

/**
 * Create a new help tree widget item for a help page URL.
 *
 * @param parent
 *    Parent for item in index.
 * @param itemText
 *    Text for the item shown in the topic index.
 * @param helpPageURL
 *    URL for the help page.
 */
HelpTreeWidgetItem*
HelpTreeWidgetItem::newInstanceForHtmlHelpPage(QTreeWidgetItem* parent,
                                           const AString& itemText,
                                           const AString& helpPageURL)
{
    CaretAssertMessage( ( ! itemText.startsWith(":/")),
                       "All help pages must be resources (page name starts with \":/\")");
    
    QString helpText;
    
    QFile file(helpPageURL);
    if (file.exists()) {
        if (file.open(QFile::ReadOnly)) {
            QTextStream stream(&file);
            helpText = stream.readAll();
            file.close();
        }
        else {
            AString msg = ("Help file exists but unable to open for reading: "
                           + helpPageURL);
            CaretLogSevere(msg);
            msg = msg.convertToHtmlPage();
        }
    }
    else {
        AString msg = ("HTML Help file missing: "
                       + helpPageURL);
        CaretLogSevere(msg);
        msg = msg.convertToHtmlPage();
    }
    
    HelpTreeWidgetItem* instance = new HelpTreeWidgetItem(parent,
                                                          TREE_ITEM_HELP_TEXT,
                                                          itemText,
                                                          helpPageURL,
                                                          helpText);
    return instance;
}

/**
 * Constructor.
 *
 * @param parent
 *    Parent for item in index.
 * @param treeItemType
 *    Type of tree item.
 * @param itemText
 *    Text for the item shown in the topic index.
 * @param helpPageURL
 *    URL for external help pages
 * @param helpText
 *    Text displayed in help browser.
 */
HelpTreeWidgetItem::HelpTreeWidgetItem(QTreeWidgetItem* parent,
                                       const TreeItemType treeItemType,
                                       const AString& itemText,
                                       const AString& helpPageURL,
                                       const AString& helpText)
: QTreeWidgetItem(parent),
m_treeItemType(treeItemType),
m_helpPageURL(helpPageURL),
m_helpText(helpText)
{
    setText(0, itemText);
}

/**
 * Destructor.
 */
HelpTreeWidgetItem::~HelpTreeWidgetItem()
{
    
}

