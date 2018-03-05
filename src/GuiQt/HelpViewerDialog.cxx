
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
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#if QT_VERSION >= 0x050000
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#else // QT_VERSION
#include <qprinter.h>
#include <qprintdialog.h>
#endif // QT_VERSION
#include <QTextBrowser>
#include <QSplitter>
#include <QTabWidget>
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
#include "WuQtUtilities.h"

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
    m_helpBrowser = NULL;
    m_topicSearchLineEditFirstMouseClick = true;
    setApplyButtonText("");
    
    QTabWidget* indexSearchTabWidget = new QTabWidget();
    indexSearchTabWidget->addTab(createTableOfContentsWidget(),
                                 "Table of Contents");
    indexSearchTabWidget->addTab(createIndexSearchWidget(),
                                 "Search");
    
    /*
     * Need some space above the tab widget
     */
    QWidget* indexSearchWidget = new QWidget();
    QVBoxLayout* indexSearchLayout = new QVBoxLayout(indexSearchWidget);
    indexSearchLayout->addWidget(indexSearchTabWidget);
    
    /*
     * Create the splitter and add the widgets to the splitter
     */
    m_splitter = new QSplitter;
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(indexSearchWidget);
    m_splitter->addWidget(createHelpViewerWidget());
    QList<int> sizeList;
    sizeList << 225 << 375;
    m_splitter->setSizes(sizeList);
    
    setCentralWidget(m_splitter,
                     WuQDialog::SCROLL_AREA_NEVER);

    loadHelpTopicsIntoIndexTree();
    
    loadSearchListWidget();
    
    resize(800,
           600);
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
 * @return Create and return the table of contents widget.
 */
QWidget*
HelpViewerDialog::createTableOfContentsWidget()
{
    /*
     * Create the tree widget for the help topics
     */
    m_topicIndexTreeWidget = new QTreeWidget;
    m_topicIndexTreeWidget->setColumnCount(1);
    m_topicIndexTreeWidget->setColumnHidden(0, false);
    m_topicIndexTreeWidget->headerItem()->setHidden(true);
    QObject::connect(m_topicIndexTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,
                                                                       QTreeWidgetItem*)),
                     this, SLOT(topicIndexTreeItemChanged(QTreeWidgetItem*,
                                                          QTreeWidgetItem*)));
    
    /*
     * Collapse All button
     */
    QAction* collapseAllAction = WuQtUtilities::createAction("Collapse All",
                                                             "",
                                                             this,
                                                             this,
                                                             SLOT(topicCollapseAllTriggered()));
    QToolButton* collapseAllToolButton = new QToolButton;
    collapseAllToolButton->setDefaultAction(collapseAllAction);
    
    /*
     * Expand All button
     */
    QAction* expandAllAction = WuQtUtilities::createAction("Expand All",
                                                           "",
                                                           this,
                                                           this,
                                                           SLOT(topicExpandAllTriggered()));
    QToolButton* expandAllToolButton = new QToolButton;
    expandAllToolButton->setDefaultAction(expandAllAction);
    
    /*
     * Layout for collapse/expand all buttons
     */
    QHBoxLayout* collapseExpandLayout = new QHBoxLayout;
    collapseExpandLayout->addStretch();
    collapseExpandLayout->addWidget(collapseAllToolButton);
    collapseExpandLayout->addWidget(expandAllToolButton);
    collapseExpandLayout->addStretch();
    
    /*
     * Layout for search line edit and topics
     */
    QWidget* topicWidgets = new QWidget();
    QVBoxLayout* topicLayout = new QVBoxLayout(topicWidgets);
    WuQtUtilities::setLayoutMargins(topicLayout, 0);
    topicLayout->addLayout(collapseExpandLayout);
    topicLayout->addWidget(m_topicIndexTreeWidget);
    
    return topicWidgets;
}

/**
 * @return Create and return the search widget.
 */
QWidget*
HelpViewerDialog::createIndexSearchWidget()
{
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
    QObject::connect(m_topicSearchLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(topicSearchLineEditStartSearch()));
    QObject::connect(m_topicSearchLineEdit, SIGNAL(textEdited(const QString&)),
                     this, SLOT(topicSearchLineEditStartSearch()));
    QObject::connect(m_topicSearchLineEdit, SIGNAL(cursorPositionChanged(int,int)),
                     this, SLOT(topicSearchLineEditCursorPositionChanged(int,int)));
    
    /*
     * List widget containing matched topics
     */
    m_topicSearchListWidget = new QListWidget();
    m_topicSearchListWidget->setSortingEnabled(false);
    QObject::connect(m_topicSearchListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                     this, SLOT(topicSearchListWidgetItemClicked(QListWidgetItem*)));
    
    /*
     * Layout for search line edit and topics
     */
    QWidget* searchWidgets = new QWidget();
    QVBoxLayout* searchLayout = new QVBoxLayout(searchWidgets);
    WuQtUtilities::setLayoutSpacingAndMargins(searchLayout, 4, 2);
    searchLayout->addWidget(m_topicSearchLineEdit);
    searchLayout->addWidget(m_topicSearchListWidget);
    
    return searchWidgets;
}

/**
 * @return Create and return the help viewer widget.
 */
QWidget*
HelpViewerDialog::createHelpViewerWidget()
{
    /*   DISPLAY HELP SECTION */
    
    /*
     * create the back toolbar button
     */
    QToolButton* backwardButton = new QToolButton;
    backwardButton->setArrowType(Qt::LeftArrow);
    backwardButton->setToolTip("Show the previous page");
    
    /*
     * Create the forward toolbar button
     */
    QToolButton* forwardButton = new QToolButton;
    forwardButton->setArrowType(Qt::RightArrow);
    forwardButton->setToolTip("Show the next page");
    
    /*
     * Create the print toolbar button
     */
    QToolButton* printButton = new QToolButton;
    connect(printButton, SIGNAL(clicked()),
            this, SLOT(helpPagePrintButtonClicked()));
    printButton->setText("Print");
    //printButton->hide();
    
    /**
     *  Copy button
     */
    QToolButton* copyButton = new QToolButton;
    copyButton->setText("Copy");
    copyButton->setToolTip("Copies selected help text to clipboard.");
    copyButton->setEnabled(false);
    
    /*
     * create the help browser
     */
    m_helpBrowser = new HelpTextBrowser(this);
    m_helpBrowser->setMinimumWidth(400);
    m_helpBrowser->setMinimumHeight(200);
    m_helpBrowser->setOpenExternalLinks(false);
    m_helpBrowser->setOpenLinks(true);
    QObject::connect(forwardButton, SIGNAL(clicked()),
                     m_helpBrowser, SLOT(forward()));
    QObject::connect(backwardButton, SIGNAL(clicked()),
                     m_helpBrowser, SLOT(backward()));
    
    /*
     * Hook up copy button to help browser
     */
    QObject::connect(m_helpBrowser, SIGNAL(copyAvailable(bool)),
                     copyButton, SLOT(setEnabled(bool)));
    QObject::connect(copyButton, SIGNAL(clicked()),
                     m_helpBrowser, SLOT(copy()));
    
    /*
     * Layout for toolbuttons
     */
    QHBoxLayout* toolButtonLayout = new QHBoxLayout;
    toolButtonLayout->addWidget(new QLabel("Navigate:"));
    toolButtonLayout->addWidget(backwardButton);
    toolButtonLayout->addWidget(forwardButton);
    toolButtonLayout->addStretch();
    toolButtonLayout->addWidget(copyButton);
    toolButtonLayout->addWidget(printButton);
    
    /*
     * Layout for help browser and buttons
     */
    QWidget* helpBrowserWidgets = new QWidget;
    QVBoxLayout* helpBrowserLayout = new QVBoxLayout(helpBrowserWidgets);
    helpBrowserLayout->addLayout(toolButtonLayout);
    helpBrowserLayout->addWidget(m_helpBrowser);
    
    return helpBrowserWidgets;
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
    
    CaretLogSevere("Could not find help page \""
                   + helpPageName
                   + "\" for loading.");
}

/**
 * Load Workbench help from the given directory and add it to the
 * the given parent.  Also process any subdirectories
 *
 * @param parent
 *    Parent tree widget item
 * @param dirInfo
 *    The directory examined for HTML pages and subdirectories
 * @return
 *    Tree widget item that was created.
 */
HelpTreeWidgetItem*
HelpViewerDialog::loadWorkbenchHelpInfoFromDirectory(QTreeWidgetItem* parent,
                                                     const QFileInfo& dirInfo)
{
    QDir directory(dirInfo.absoluteFilePath());
    
    /*
     * Get all HTML pages in the directory
     * and file an HTML page that is the same
     * name as the directory.
     */
    QStringList htmlNameFilter;
    htmlNameFilter << "*.htm" << "*.html";
    QFileInfoList htmlFileList = directory.entryInfoList(htmlNameFilter,
                                                QDir::Files,
                                                QDir::Name);
    
    QString dirHtmlPageName;
    QFileInfoList otherHtmlPagesList;
    QListIterator<QFileInfo> htmlFileIter(htmlFileList);
    while (htmlFileIter.hasNext()) {
        const QFileInfo htmlFileInfo = htmlFileIter.next();
        if (htmlFileInfo.baseName() == dirInfo.baseName()) {
            dirHtmlPageName = htmlFileInfo.absoluteFilePath();
        }
        else {
            otherHtmlPagesList.append(htmlFileInfo.absoluteFilePath());
        }
    }
    
    /*
     * Create a tree widget item for this directory
     * that may have a help page.
     */
    HelpTreeWidgetItem* treeItem = NULL;
    if ( ! dirHtmlPageName.isEmpty()) {
        treeItem = createHelpTreeWidgetItemForHelpPage(parent,
                                                       dirInfo.baseName(),
                                                       dirHtmlPageName);
    }
    else {
        QString text = dirInfo.fileName();
        text = text.replace('_', ' ');
        treeItem = HelpTreeWidgetItem::newInstanceEmptyItem(parent,
                                                            text);
    }
    
    /*
     * Add items for any other HTML pages found in the directory
     */
    QListIterator<QFileInfo> otherHtmlPageIter(otherHtmlPagesList);
    while (otherHtmlPageIter.hasNext()) {
        const QFileInfo pageInfo = otherHtmlPageIter.next();
        createHelpTreeWidgetItemForHelpPage(treeItem,
                                            pageInfo.baseName(),
                                            pageInfo.absoluteFilePath());
    }
    
    /*
     * Add any subdirectories as children
     */
    QFileInfoList subDirList = directory.entryInfoList((QDir::AllDirs | QDir::NoDotAndDotDot),
                                                       QDir::Name);
    QListIterator<QFileInfo> subDirIter(subDirList);
    while (subDirIter.hasNext()) {
        const QFileInfo subDirInfo = subDirIter.next();
        const QString name = subDirInfo.fileName();
        if (name.endsWith(".fld")) {
            /* 
             * Ignore directories ending in ".fld" which is created when
             * a Microsoft Word file is saved as HTML.  The ".fld" 
             * directory contains image files from the Word file.
             */
        }
        else {
            loadWorkbenchHelpInfoFromDirectory(treeItem,
                                               subDirInfo);
        }
    }
    
    return treeItem;
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
    workbenchItem->setText(0, "wb_view");
    
    QDir resourceHelpDirectory(":/HelpFiles");
    
    QTreeWidgetItem* glossaryItem = NULL;
    
    // CAN BE SET TO FIND FILES WITHOUT FULL PATH
    //m_helpBrowser->setSearchPaths(QStringList(":/HelpFiles/Menus/File_Menu"));
    
    QFileInfoList subDirList = resourceHelpDirectory.entryInfoList((QDir::AllDirs | QDir::NoDotAndDotDot),
                                                                   QDir::Name);
    QListIterator<QFileInfo> subDirIter(subDirList);
    while (subDirIter.hasNext()) {
        const QFileInfo subDirInfo = subDirIter.next();
        
        HelpTreeWidgetItem* item = loadWorkbenchHelpInfoFromDirectory(workbenchItem,
                                                                   subDirInfo);
        
        /*
         * Is this the GLOSSARY?
         * If so, move it so that it is a top level item.
         */
        if (subDirInfo.baseName().toLower() == "glossary") {
            if (glossaryItem != NULL) {
                CaretAssertMessage(0, "There should be only one glossary subdirectory !!!!");
            }
            glossaryItem = item;
            workbenchItem->removeChild(glossaryItem);
            m_topicIndexTreeWidget->addTopLevelItem(glossaryItem);
        }
    }
    
    
    /*
     * Load commands
     */
    CommandOperationManager* commandOperationManager = CommandOperationManager::getCommandOperationManager();
    std::vector<CommandOperation*> commandOperations = commandOperationManager->getCommandOperations();
    
    QTreeWidgetItem* wbCommandItem = NULL;
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
        
        wbCommandItem = new QTreeWidgetItem(m_topicIndexTreeWidget,
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
            
            addToAllItems(item);
        }
    }

    /*
     * Using setExpanded on a QTreeWidgetItem only expands its immediate children.
     * So, expand everything and then collapse Glossary and wb_command items so
     * that only wb_view items are expanded.
     */
    m_topicIndexTreeWidget->expandAll();
    if (glossaryItem != NULL) {
        glossaryItem->setExpanded(false);
    }
    if (wbCommandItem != NULL) {
        wbCommandItem->setExpanded(false);
    }
    
    m_topicIndexTreeWidget->sortItems(0, Qt::AscendingOrder);
    
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
    
    addToAllItems(helpItem);
    
    return helpItem;
}

/**
 * Called when selected index tree item changes.
 *
 * @param currentItem
 *    The selected item
 * @param previousItem
 *    The previously selected item
 */
void
HelpViewerDialog::topicIndexTreeItemChanged(QTreeWidgetItem* currentItem,
                                            QTreeWidgetItem* /*previousItem*/)
{
    if (currentItem != NULL) {
        /*
         * Note not all items are castable to HelpTreeWidgetItem.
         * Items not castable are category items that have an arrow to
         * expand/collapse its children.
         */
        HelpTreeWidgetItem* helpItem = dynamic_cast<HelpTreeWidgetItem*>(currentItem);
        if (helpItem != NULL) {
            displayHelpTextForHelpTreeWidgetItem(helpItem);
            m_topicIndexTreeWidget->scrollToItem(helpItem,
                                                 QTreeWidget::EnsureVisible);
        }
        else {
            const AString html = AString(currentItem->text(0)).convertToHtmlPage();
            m_helpBrowser->setHtml(html);
        }
    }
}

/**
 * Add the help item to all items and to the search list widget.
 *
 * @helpItem
 *     Help item to add.
 */
void
HelpViewerDialog::addToAllItems(HelpTreeWidgetItem* helpItem)
{
    m_allHelpTreeWidgetItems.push_back(helpItem);
}

/**
 * Load the search list widget.  List all Workbench items in alphabetical
 * order followed by all commands in alphabetical order.
 */
void
HelpViewerDialog::loadSearchListWidget()
{
    std::vector<HelpSearchListItem*> commandItems;
    std::vector<HelpSearchListItem*> workbenchItems;
    
    for (std::vector<HelpTreeWidgetItem*>::iterator allIter = m_allHelpTreeWidgetItems.begin();
         allIter != m_allHelpTreeWidgetItems.end();
         allIter++) {
        HelpTreeWidgetItem* treeItem = *allIter;
        
        switch (treeItem->m_treeItemType) {
            case HelpTreeWidgetItem::TREE_ITEM_NONE:
                /*
                 * Do not allow in search
                 */
                continue;
                break;
            case HelpTreeWidgetItem::TREE_ITEM_HELP_PAGE_URL:
                break;
            case HelpTreeWidgetItem::TREE_ITEM_HELP_TEXT:
                break;
        }
        
        HelpSearchListItem* searchItem = new HelpSearchListItem(treeItem);
        m_allHelpSearchListWidgetItems.push_back(searchItem);
        
        if (treeItem->text(0).startsWith("-")) {
            commandItems.push_back(searchItem);
        }
        else {
            workbenchItems.push_back(searchItem);
        }
    }
    
    std::sort(commandItems.begin(),
              commandItems.end(),
              HelpSearchListItem::sortAlphabetically);
    
    std::sort(workbenchItems.begin(),
              workbenchItems.end(),
              HelpSearchListItem::sortAlphabetically);
    
    for (std::vector<HelpSearchListItem*>::iterator wbIter = workbenchItems.begin();
         wbIter != workbenchItems.end();
         wbIter++) {
        m_topicSearchListWidget->addItem(*wbIter);
    }
    for (std::vector<HelpSearchListItem*>::iterator cmdIter = commandItems.begin();
         cmdIter != commandItems.end();
         cmdIter++) {
        m_topicSearchListWidget->addItem(*cmdIter);
    }
}

/**
 * Called when an item in the search list widget is clicked.
 *
 * @param item
 *    The selected item
 */
void
HelpViewerDialog::topicSearchListWidgetItemClicked(QListWidgetItem* item)
{
    HelpSearchListItem* searchItem = dynamic_cast<HelpSearchListItem*>(item);
    if (searchItem != NULL) {
        HelpTreeWidgetItem* helpItem = searchItem->m_matchingTreeWidgetItem;
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
    m_helpBrowser->setSource(helpItem->m_helpPageURL);
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
    
    for (std::vector<HelpSearchListItem*>::iterator iter = m_allHelpSearchListWidgetItems.begin();
         iter != m_allHelpSearchListWidgetItems.end();
         iter++) {
        HelpSearchListItem* helpListItem = *iter;
        CaretAssert(helpListItem);
        HelpTreeWidgetItem* helpTreeItem = helpListItem->m_matchingTreeWidgetItem;
        CaretAssert(helpTreeItem);
        
        bool showItemFlag = true;
        if (haveSearchTextFlag) {
            showItemFlag = false;
            
            if (haveWildcardSearchFlag) {
                if (regEx.exactMatch(helpTreeItem->m_helpText)) {
                    showItemFlag = true;
                }
            }
            else if (helpTreeItem->m_helpText.contains(searchText,
                                                   Qt::CaseInsensitive)) {
                showItemFlag = true;
            }
        }
        helpListItem->setHidden( ! showItemFlag);
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

/**
 * Expand all help topics
 */
void
HelpViewerDialog::topicExpandAllTriggered()
{
    m_topicIndexTreeWidget->expandAll();
}

/**
 * Collapse all help topics
 */
void
HelpViewerDialog::topicCollapseAllTriggered()
{
    m_topicIndexTreeWidget->collapseAll();
}


// ========================================================================= //

/**
 * Create a help viewer widget.
 *
 * @param parentHelpViewerDialog
 *    The parent help viewer dialog.
 */
HelpTextBrowser::HelpTextBrowser(HelpViewerDialog* parentHelpViewerDialog)
: QTextBrowser(parentHelpViewerDialog),
m_parentHelpViewerDialog(parentHelpViewerDialog)
{
    CaretAssert(parentHelpViewerDialog);
}

/**
 * Destructor.
 */
HelpTextBrowser::~HelpTextBrowser()
{
    
}

/**
 * Overrides superclass version so that images get loaded properly.
 * Setting search paths may eliminate need for this method.
 *
 * @param type
 *    Type of resource.
 * @param url
 *    URL of resource.
 * @return
 *    QVariant containing content for display in the help viewer.
 */
QVariant
HelpTextBrowser::loadResource(int type, const QUrl& url)
{
    const QString urlText = url.toString();
    
    QVariant result;
    
        for (std::vector<HelpTreeWidgetItem*>::iterator iter = m_parentHelpViewerDialog->m_allHelpTreeWidgetItems.begin();
             iter != m_parentHelpViewerDialog->m_allHelpTreeWidgetItems.end();
             iter++) {
            HelpTreeWidgetItem* treeItem = *iter;
            if (treeItem->m_helpPageURL == urlText) {
                result = treeItem->m_helpText;
                break;
            }
        }
        
        if ( ! result.isValid()) {
            result = QTextBrowser::loadResource(type, url);
            if (result.isValid()) {
                /* nothing */
            }
            else {
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

/**
 * Set the source of the help browser.
 *
 * @param url
 *     URL directing to content.
 */
void
HelpTextBrowser::setSource(const QUrl& url)
{
    const AString urlText = url.toString();
    if (urlText.startsWith("http:")) {
        if (WuQMessageBox::warningOkCancel(this,
                                           "The link clicked will be displayed in your web browser.")) {
            if ( ! QDesktopServices::openUrl(urlText)) {
                WuQMessageBox::errorOk(this,
                                       ("Failed to load "
                                        + urlText));
            }
        }
    }
    else if (urlText.startsWith("mailto")) {
        if (WuQMessageBox::warningOkCancel(this,
                                           "This link will open your mail client.")) {
            if ( ! QDesktopServices::openUrl(urlText)) {
                WuQMessageBox::errorOk(this,
                                       ("Failed to load "
                                        + urlText));
            }
        }
    }
    else {
        QTextBrowser::setSource(url);
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
    const AString itemText = commandOperation->getCommandLineSwitch();
    const AString helpInfoCopy = commandOperation->getHelpInformation("wb_command");
    const AString helpText = helpInfoCopy.convertToHtmlPageWithFontSize(2);
    const AString helpPageURL("command:/"
                              + commandOperation->getOperationShortDescription().replace(' ', '_'));
    
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
    
    AString text = itemText;
    text = text.replace('_', ' ');
    if (parent != NULL) {
        instance = new HelpTreeWidgetItem(parent,
                                          TREE_ITEM_HELP_TEXT,
                                          text,
                                          "qrc" + helpPageURL,
                                          helpText);
    }
    else {
        instance = new HelpTreeWidgetItem(TREE_ITEM_HELP_TEXT,
                                          text,
                                          "qrc" + helpPageURL,
                                          helpText);
    }
    
    return instance;
}

/**
 * Constructor for item with parent but only a name.
 *
 * @param parent
 *    Parent for item in index.
 * @param itemText
 *    Text for the item shown in the topic index.
 */
HelpTreeWidgetItem*
HelpTreeWidgetItem::newInstanceEmptyItem(QTreeWidgetItem* parent,
                                         const AString& itemText)
{
    HelpTreeWidgetItem* item = new HelpTreeWidgetItem(parent,
                                                      TREE_ITEM_NONE,
                                                     itemText,
                                                      "",
                                                      "");
    return item;
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

// ========================================================================= //

/**
 * Constructs a list widget item for use during search operation
 *
 * @param matchingTreeWidgetItem
 *    The matching tree widget item that contains the help information.
 */
HelpSearchListItem::HelpSearchListItem(HelpTreeWidgetItem* matchingTreeWidgetItem)
{
    CaretAssert(matchingTreeWidgetItem);
    m_matchingTreeWidgetItem = matchingTreeWidgetItem;
    setText(m_matchingTreeWidgetItem->text(0));
}

/**
 * Destructor.
 */
HelpSearchListItem::~HelpSearchListItem()
{
    
}

/**
 * Sort the two "pointed to" items alphabetically.
 *
 * @param h1
 *    First item.
 * @param h2
 *    Second item.
 * @return
 *    True if h1 is alphabetically before h2.
 */
bool
HelpSearchListItem::sortAlphabetically(const HelpSearchListItem* h1,
                               const HelpSearchListItem* h2)
{
    CaretAssert(h1);
    CaretAssert(h2);
    
    return (h1->m_matchingTreeWidgetItem->text(0) < h2->m_matchingTreeWidgetItem->text(0));
}

