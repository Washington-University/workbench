
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

#include <QDesktopServices>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
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
    m_helpTopicHistoryIndex = -1;
    m_topicSearchLineEditFirstMouseClick = true;
    setApplyButtonText("");

    /*
     * create the help browser
     */
    m_helpBrowser = new HelpTextBrowser(); //QTextBrowser;
    m_helpBrowser->setMinimumWidth(400);
    m_helpBrowser->setMinimumHeight(200);
    m_helpBrowser->setOpenExternalLinks(false);
    m_helpBrowser->setOpenLinks(false);
    QObject::connect(m_helpBrowser, SIGNAL(anchorClicked(const QUrl&)),
                     this, SLOT(helpPageAnchorClicked(const QUrl&)));
    
    /*
     * Create the tree widget for the help topics
     */
    m_topicIndexTreeWidget = new QTreeWidget;
    m_topicIndexTreeWidget->setColumnCount(1);
    m_topicIndexTreeWidget->setColumnHidden(0, false);
    m_topicIndexTreeWidget->headerItem()->setHidden(true);
    QObject::connect(m_topicIndexTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                     this, SLOT(topicIndexTreeItemSelected(QTreeWidgetItem*,int)));
    
    /*
     * Search line edit and list widget
     */
    const AString searchText = ("All searches are case insensitive.\n"
                                "\n"
                                "You may use wildcard characters:\n"
                                "    * - Matches any characters.\n"
                                "    ? - Matches a single character.\n");
    const AString topicSearchToolTipText = ("Enter text to search content of ALL help pages.\n"
                                            + searchText);
    m_topicSearchLineEdit = new QLineEdit;
    m_topicSearchLineEdit->setToolTip(topicSearchToolTipText.convertToHtmlPage());
    //m_topicSearchLineEdit->setText("Enter topic search text here");
    QObject::connect(m_topicSearchLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(topicSearchLineEditStartSearch()));
    QObject::connect(m_topicSearchLineEdit, SIGNAL(textEdited(const QString&)),
                     this, SLOT(topicSearchLineEditStartSearch()));
    QObject::connect(m_topicSearchLineEdit, SIGNAL(cursorPositionChanged(int,int)),
                     this, SLOT(topicSearchLineEditCursorPositionChanged(int,int)));
    
    /*
     * create the back toolbar button
     */
    QToolButton* backwardButton = new QToolButton;
    backwardButton->setArrowType(Qt::LeftArrow);
    backwardButton->setToolTip("Show the previous page");
    connect(backwardButton, SIGNAL(clicked()),
            this, SLOT(helpPageBackButtonClicked()));
    
    /*
     * Create the forward toolbar button
     */
    QToolButton* forwardButton = new QToolButton;
    forwardButton->setArrowType(Qt::RightArrow);
    forwardButton->setToolTip("Show the next page");
    connect(forwardButton, SIGNAL(clicked()),
            this, SLOT(helpPageForwardButtonClicked()));
    
    /*
     * Create the print toolbar button
     */
    QToolButton* printButton = new QToolButton;
    connect(printButton, SIGNAL(clicked()),
            this, SLOT(helpPagePrintButtonClicked()));
    printButton->setText("Print");
    printButton->hide();
    
    /*
     * Line edit for searching help text
     */
    const AString helpTextSearchToolTipText = ("Enter text to search displayed help page.\n"
                                               "Press RETURN key to start the search.");
    m_helpTextFindLineEdit = new QLineEdit;
    m_helpTextFindLineEdit->setToolTip(helpTextSearchToolTipText.convertToHtmlPage());
    QObject::connect(m_helpTextFindLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(helpTextSearchLineEditStartSearch()));
    //QObject::connect(m_helpTextFindLineEdit, SIGNAL(textEdited(const QString&)),
    //                 this, SLOT(helpTextSearchLineEditStartSearch()));

    /*
     * Find previous button
     */
    
    m_helpTextFindPreviousToolButton = new QToolButton;
    m_helpTextFindPreviousToolButton->setToolTip("Find previous location of text");
    m_helpTextFindPreviousToolButton->setArrowType(Qt::UpArrow);
    QObject::connect(m_helpTextFindPreviousToolButton, SIGNAL(clicked()),
                     this, SLOT(helpTextFindPreviousButtonClicked()));
    
    /*
     * Find next button
     */
    m_helpTextFindNextToolButton = new QToolButton;
    m_helpTextFindNextToolButton->setToolTip("Find next location of text");
    m_helpTextFindNextToolButton->setArrowType(Qt::DownArrow);
    QObject::connect(m_helpTextFindNextToolButton, SIGNAL(clicked()),
                     this, SLOT(helpTextFindNextButtonClicked()));
    
    /*
     * Layout for toolbuttons
     */
    QHBoxLayout* toolButtonLayout = new QHBoxLayout;
    toolButtonLayout->addWidget(new QLabel("Navigate:"));
    toolButtonLayout->addWidget(backwardButton);
    toolButtonLayout->addWidget(forwardButton);
    toolButtonLayout->addWidget(printButton);
    toolButtonLayout->addStretch();
    toolButtonLayout->addWidget(new QLabel("Find:"));
    toolButtonLayout->addWidget(m_helpTextFindLineEdit);
    toolButtonLayout->addWidget(m_helpTextFindPreviousToolButton);
    toolButtonLayout->addWidget(m_helpTextFindNextToolButton);
    
    /*
     * Layout for help browser and buttons
     */
    QWidget* helpBrowserWidgets = new QWidget;
    QVBoxLayout* helpBrowserLayout = new QVBoxLayout(helpBrowserWidgets);
    helpBrowserLayout->addLayout(toolButtonLayout);
    helpBrowserLayout->addWidget(m_helpBrowser);
    
    /*
     * Layout for search line edit and topics
     */
    QWidget* topicWidgets = new QWidget();
    QVBoxLayout* topicLayout = new QVBoxLayout(topicWidgets);
    topicLayout->addWidget(m_topicSearchLineEdit);
    topicLayout->addWidget(m_topicIndexTreeWidget);

    /*
     * Create the splitter and add the widgets to the splitter
     */
    m_splitter = new QSplitter;
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(topicWidgets);
    m_splitter->addWidget(helpBrowserWidgets);
    QList<int> sizeList;
    sizeList << 225 << 375;
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
    
    QTreeWidgetItem* glossaryItem = new QTreeWidgetItem(workbenchItem,
                                                        TREE_ITEM_NONE);
    glossaryItem->setText(0, "Glossary");
    
    QTreeWidgetItem* otherItem = new QTreeWidgetItem(workbenchItem,
                                                        TREE_ITEM_NONE);
    otherItem->setText(0, "Other");
    
    
    QTreeWidgetItem* menuWbViewItem = NULL;
    QTreeWidgetItem* menuFileItem = NULL;
    QTreeWidgetItem* menuViewItem = NULL;
    QTreeWidgetItem* menuDataItem = NULL;
    QTreeWidgetItem* menuSurfaceItem = NULL;
    QTreeWidgetItem* menuConnectItem = NULL;
    QTreeWidgetItem* menuDevelopItem = NULL;
    QTreeWidgetItem* menuWindowItem = NULL;
    QTreeWidgetItem* menuHelpItem = NULL;
    std::vector<QTreeWidgetItem*> unknownMenuItems;
    
    
    QDir resourceHelpDirectory(":/HelpFiles");
    
    // CAN BE SET TO FIND FILES WITHOUT FULL PATH
    //m_helpBrowser->setSearchPaths(QStringList(":/HelpFiles/Menus/File_Menu"));
    
    if (resourceHelpDirectory.exists()) {
        QStringList htmlFileFilter;
        htmlFileFilter << "*";
        //htmlFileFilter << "*.htm" << "*.html";
        QDirIterator dirIter(":/HelpFiles",
                             htmlFileFilter,
                             QDir::NoFilter,
                             QDirIterator::Subdirectories);
        
        while (dirIter.hasNext()) {
            dirIter.next();
            const QFileInfo fileInfo = dirIter.fileInfo();
            const QString name       = fileInfo.baseName();
            const QString filePath   = fileInfo.filePath();
 
            std::cout << qPrintable("name / filePath: "
                                    + name
                                    + " / "
                                    + filePath) << std::endl;
            
            
            if (filePath.endsWith(".htm")
                || filePath.endsWith(".html")) {
                if (name.contains("Menu",
                                  Qt::CaseInsensitive)) {
                    QTreeWidgetItem* item = createHelpTreeWidgetItemForHelpPage(NULL,
                                                                                name,
                                                                                filePath);
                    if (name.contains("wb_view")) {
                        menuWbViewItem = item;
                    }
                    else if (name.startsWith("File",
                                             Qt::CaseInsensitive)) {
                        menuFileItem = item;
                    }
                    else if (name.startsWith("View",
                                             Qt::CaseInsensitive)) {
                        menuViewItem = item;
                    }
                    else if (name.startsWith("Data",
                                             Qt::CaseInsensitive)) {
                        menuDataItem = item;
                    }
                    else if (name.startsWith("Surface",
                                             Qt::CaseInsensitive)) {
                        menuSurfaceItem = item;
                    }
                    else if (name.startsWith("Connect",
                                             Qt::CaseInsensitive)) {
                        menuConnectItem = item;
                    }
                    else if (name.startsWith("Develop",
                                             Qt::CaseInsensitive)) {
                        menuDevelopItem = item;
                    }
                    else if (name.startsWith("Window",
                                             Qt::CaseInsensitive)) {
                        menuWindowItem = item;
                    }
                    else if (name.startsWith("Help",
                                             Qt::CaseInsensitive)) {
                        menuHelpItem = item;
                    }
                    else {
                        CaretLogSevere("Unrecognized menu name, has a new menu been added? \""
                                       + name);
                        unknownMenuItems.push_back(item);
                    }
                }
                else if (filePath.contains("Glossary")) {
                    createHelpTreeWidgetItemForHelpPage(glossaryItem,
                                                        name,
                                                        filePath);
                }
                else {
                    createHelpTreeWidgetItemForHelpPage(otherItem,
                                                        name,
                                                        filePath);
                }
            }
        }
        
        glossaryItem->sortChildren(0,
                                   Qt::AscendingOrder);
        otherItem->sortChildren(0,
                                Qt::AscendingOrder);
        
    }
    else {
        CaretLogSevere("Resource directory "
                       + resourceHelpDirectory.absolutePath()
                       + " not found.");
    }
    
    addItemToParentMenu(menuItem,
                        menuWbViewItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuFileItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuViewItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuDataItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuSurfaceItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuConnectItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuDevelopItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuWindowItem,
                        "");
    addItemToParentMenu(menuItem,
                        menuHelpItem,
                        "");
    for (std::vector<QTreeWidgetItem*>::iterator unknownIter = unknownMenuItems.begin();
         unknownIter != unknownMenuItems.end();
         unknownIter++) {
        addItemToParentMenu(menuItem,
                            *unknownIter,
                            "");
    }
    
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
            sortCommandsMap.insert(std::make_pair(op->getCommandLineSwitch(),
                                                  op));
        }
        
        QTreeWidgetItem* wbCommandItem = new QTreeWidgetItem(m_topicIndexTreeWidget,
                                                             TREE_ITEM_NONE);
        wbCommandItem->setText(0, "wb_command");
        
        QFont commandFont = wbCommandItem->font(0);
        commandFont.setPointSize(10);
        
        for (std::map<QString, CommandOperation*>::iterator mapIter = sortCommandsMap.begin();
             mapIter != sortCommandsMap.end();
             mapIter++) {
            CommandOperation* op = mapIter->second;
            
            HelpTreeWidgetItem* item = HelpTreeWidgetItem::newInstanceForCommandOperation(wbCommandItem,
                                                                 op);
            item->setFont(0, commandFont);
            m_allHelpWidgetItems.push_back(item);
        }
        
        m_topicIndexTreeWidget->setItemExpanded(wbCommandItem,
                                                true);
    }
    
    m_topicIndexTreeWidget->blockSignals(false);
}

/**
 * Add an item to the menu's item.
 * If the given item is NULL, it was not found an an error message will
 * be logged.
 *
 * @param parentMenu
 *    The parent menu item.
 * @param item
 *    The item that is added to the parent menu.
 * @param itemName
 *    Name for item.
 */
void
HelpViewerDialog::addItemToParentMenu(QTreeWidgetItem* parentMenu,
                                      QTreeWidgetItem* item,
                                      const AString& itemName)
{
    CaretAssert(parentMenu);
    
    if (item != NULL) {
        if ( ! itemName.isEmpty()) {
            item->setText(0,
                          itemName);
        }
        parentMenu->addChild(item);
    }
    else {
        CaretLogSevere("Did not find help for menu: "
                       + itemName);
    }
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
            displayHelpTextForHelpTreeWidgetItem(helpItem,
                                                 true);
        }
    }
}

/**
 * Called when help text back button is clicked.
 */
void
HelpViewerDialog::helpPageBackButtonClicked()
{
    if (m_helpTopicHistoryIndex > 0) {
        m_helpTopicHistoryIndex--;
        if (m_helpTopicHistoryIndex < static_cast<int32_t>(m_helpTopicHistoryItems.size())) {
            displayHelpTextForHelpTreeWidgetItem(m_helpTopicHistoryItems[m_helpTopicHistoryIndex],
                                                 false);
            printHistory();
        }
    }
}

/**
 * Called when help text forward button is clicked.
 */
void
HelpViewerDialog::helpPageForwardButtonClicked()
{
    if (m_helpTopicHistoryIndex >= 0) {
        if (m_helpTopicHistoryIndex < (static_cast<int32_t>(m_helpTopicHistoryItems.size() - 1))) {
            m_helpTopicHistoryIndex++;
            displayHelpTextForHelpTreeWidgetItem(m_helpTopicHistoryItems[m_helpTopicHistoryIndex],
                                                 false);
            printHistory();
        }
    }
}


/**
 * Add to help topic history
 *
 * @param item
 *     new item for history
 */
void
HelpViewerDialog::addToHelpTopicHistory(HelpTreeWidgetItem* item)
{
    if (m_helpTopicHistoryIndex >= 0) {
        if (m_helpTopicHistoryIndex < static_cast<int32_t>(m_helpTopicHistoryItems.size() - 1)) {
            m_helpTopicHistoryItems.erase(m_helpTopicHistoryItems.begin() + m_helpTopicHistoryIndex + 1,
                                         m_helpTopicHistoryItems.end());
        }
    }
    
    m_helpTopicHistoryItems.push_back(item);
    m_helpTopicHistoryIndex = m_helpTopicHistoryItems.size() - 1;
    
    printHistory();
}

void
HelpViewerDialog::printHistory()
{
    const bool showHistory = false;
    if (showHistory) {
        const int32_t numHistory = static_cast<int32_t>(m_helpTopicHistoryItems.size());
        for (int32_t i = 0; i < numHistory; i++) {
            std::cout << i << ": " << qPrintable(m_helpTopicHistoryItems[i]->text(0)) << std::endl;
        }
        std::cout << "History Index: " << m_helpTopicHistoryIndex << std::endl;
        std::cout << std::endl;
    }
}


/**
 * Display the help information for the given help item.
 *
 * @param helpItem
 *    Item for which help text is loaded.
 * @param addToHistoryFlag
 *    Add item to the history.
 */
void
HelpViewerDialog::displayHelpTextForHelpTreeWidgetItem(HelpTreeWidgetItem* helpItem,
                                                       const bool addToHistoryFlag)
{
    CaretAssert(helpItem);
    switch (helpItem->m_treeItemType) {
        case HelpTreeWidgetItem::TREE_ITEM_NONE:
            break;
        case HelpTreeWidgetItem::TREE_ITEM_HELP_PAGE_URL:
            displayHttpInUsersWebBrowser(helpItem->m_helpPageURL);
            addToHelpTopicHistory(helpItem);
            break;
        case HelpTreeWidgetItem::TREE_ITEM_HELP_TEXT:
            m_helpBrowser->clear();
//            std::cout << "LOADING: " << qPrintable(helpItem->text(0))
//            << ":  " << qPrintable(helpItem->m_helpText)
//            << std::endl << std::endl;
            if (helpItem->m_helpText.contains("<html>",
                                              Qt::CaseInsensitive)) {
                QTextDocument td;
                td.setHtml(helpItem->m_helpText);
                const QString html = td.toHtml();
                std::cout << "Old/New length:" << helpItem->m_helpText.size() << " " << html.size() << std::endl;
                m_helpBrowser->setHtml(html);
                std::cout << "Cleaned HTML" << qPrintable(helpItem->text(0))
                            << ":  "  << std::endl << qPrintable(html)
                            << std::endl << std::endl;
            }
            else {
                m_helpBrowser->setText(helpItem->m_helpText);
            }
            if (addToHistoryFlag) {
                addToHelpTopicHistory(helpItem);
            }
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
                    displayHelpTextForHelpTreeWidgetItem(item,
                                                         true);
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
 * Called when search text is changed or return pressed to start
 * searching the help topics
 */
void
HelpViewerDialog::topicSearchLineEditStartSearch()
{
    const QString searchText = m_topicSearchLineEdit->text().trimmed();
    const bool haveSearchTextFlag = ( ! searchText.isEmpty());
    
    QRegExp regEx;
    bool haveWildcardSearchFlag = false;
    if (haveSearchTextFlag) {
        if (searchText.contains('*')
            || searchText.contains('?')) {
            haveWildcardSearchFlag = true;
            regEx.setPatternSyntax(QRegExp::Wildcard);
            regEx.setPattern(searchText);
            regEx.setCaseSensitivity(Qt::CaseInsensitive);
        }
    }
    
    for (std::vector<HelpTreeWidgetItem*>::iterator iter = m_allHelpWidgetItems.begin();
         iter != m_allHelpWidgetItems.end();
         iter++) {
        HelpTreeWidgetItem* helpItem = *iter;
        
        bool showItemFlag = true;
        if (haveSearchTextFlag) {
            showItemFlag = false;
            
            if (haveWildcardSearchFlag) {
                if (regEx.exactMatch(helpItem->m_helpText)) {
                    showItemFlag = true;
                }
            }
            else if (helpItem->m_helpText.contains(searchText,
                                                      Qt::CaseInsensitive)) {
                showItemFlag = true;
            }
        }
        helpItem->setHidden( ! showItemFlag);
    }
}

/**
 * Called to search the text of the displayed help page.
 */
void
HelpViewerDialog::helpTextSearchLineEditStartSearch()
{
    findInHelpText(FIND_FORWARDS);
}

/**
 * called to find in browser window.
 */
void
HelpViewerDialog::helpTextFindPreviousButtonClicked()
{
    findInHelpText(FIND_BACKWARDS);
}

/**
 * called to find next in browser window.
 */
void
HelpViewerDialog::helpTextFindNextButtonClicked()
{
    findInHelpText(FIND_FORWARDS);
}

/**
 * Execute a find operation in the help text.
 *
 * @param findDirection
 *     Direction for the find operation.
 */
void
HelpViewerDialog::findInHelpText(const FindDirection findDirection)
{
    const QString searchText = m_helpTextFindLineEdit->text().trimmed();
    const bool haveSearchTextFlag = ( ! searchText.isEmpty());
    
    if (haveSearchTextFlag) {
        switch (findDirection) {
            case FIND_BACKWARDS:
                if (! m_helpBrowser->find(searchText,
                                          QTextDocument::FindBackward)) {
                    m_helpBrowser->moveCursor(QTextCursor::End);
                    m_helpBrowser->find(searchText,
                                        QTextDocument::FindBackward);
                }
                break;
            case FIND_FORWARDS:
                if (! m_helpBrowser->find(searchText)) {
                    m_helpBrowser->moveCursor(QTextCursor::Start);
                    m_helpBrowser->find(searchText);
                }
                break;
        }
    }
}


/**
 * called to print currently displayed page.
 */
void
HelpViewerDialog::helpPagePrintButtonClicked()
{
    QPrinter printer;
    QPrintDialog* printDialog = new QPrintDialog(&printer, this);
    if (printDialog->exec() == QPrintDialog::Accepted) {
        m_helpBrowser->document()->print(&printer);
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
        topicSearchLineEditStartSearch();
    }
}

// ========================================================================= //

HelpTextBrowser::HelpTextBrowser(QWidget* parent)
: QTextBrowser(parent)
{
    
}

HelpTextBrowser::~HelpTextBrowser()
{
    
}

/**
 * Overrides superclass version so that images get loaded properly.
 * Setting search paths may eliminate need for this method.
 *
 * @param type
 *    Type of resource.
 * @param name
 *    URL of resource.
 */
QVariant
HelpTextBrowser::loadResource(int type, const QUrl& name)
{
    QUrl url = name;
    
    QVariant result = QTextBrowser::loadResource(type, url);
    if ( ! result.isValid()) {
        url = QUrl::fromLocalFile(":/" + name.toString());
        result = QTextBrowser::loadResource(type, url);

        if ( ! result.isValid()) {
            QString typeName("Unknown");
            if ( ! result.isValid()) {
                switch (type) {
                    case QTextDocument::HtmlResource:
                        typeName = "Html Resource";
                        break;
                    case QTextDocument::ImageResource:
                        typeName = "Image Resource";
                        break;
                    case QTextDocument::StyleSheetResource:
                        typeName = "Style Sheet Resource";
                        break;
                }
            }
            
            CaretLogSevere("Failed to load type: "
                           + typeName
                           + " file: "
                           + url.toString());
        }
    }
    
    return result;
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
    const AString itemText = commandOperation->getCommandLineSwitch();
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
            helpText = msg.convertToHtmlPage();
        }
    }
    else {
        AString msg = ("HTML Help file missing: "
                       + helpPageURL);
        CaretLogSevere(msg);
        helpText = msg.convertToHtmlPage();
    }
    
    HelpTreeWidgetItem* instance = NULL;
    if (parent != NULL) {
        instance = new HelpTreeWidgetItem(parent,
                                          TREE_ITEM_HELP_TEXT,
                                          itemText,
                                          helpPageURL,
                                          helpText);
    }
    else {
        instance = new HelpTreeWidgetItem(TREE_ITEM_HELP_TEXT,
                                          itemText,
                                          helpPageURL,
                                          helpText);
    }
    
    return instance;
}

/**
 * Constructor for item with parent
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
 * Constructor for item WITHOUT parent
 *
 * @param treeItemType
 *    Type of tree item.
 * @param itemText
 *    Text for the item shown in the topic index.
 * @param helpPageURL
 *    URL for external help pages
 * @param helpText
 *    Text displayed in help browser.
 */
HelpTreeWidgetItem::HelpTreeWidgetItem(const TreeItemType treeItemType,
                                       const AString& itemText,
                                       const AString& helpPageURL,
                                       const AString& helpText)
: QTreeWidgetItem(),
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

