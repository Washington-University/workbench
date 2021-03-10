#ifndef __HELP_VIEWER_DIALOG_H__
#define __HELP_VIEWER_DIALOG_H__

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

#include <QListWidgetItem>
#include <QTextBrowser>
#include <QTreeWidgetItem>

#include "WuQDialogNonModal.h"

class QFileInfo;
class QLineEdit;
class QListWidget;
class QSplitter;
class QToolButton;
class QTreeWidget;
class QUrl;


namespace caret {

    class CommandOperation;
    class HelpSearchListItem;
    class HelpTreeWidgetItem;
    
    class HelpViewerDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        HelpViewerDialog(QWidget* parent,
                         Qt::WindowFlags f = Qt::WindowFlags());
        
        virtual ~HelpViewerDialog();
        
        virtual void updateDialog();
        
        void showHelpPageWithName(const AString& helpPageName);
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void topicIndexTreeItemChanged(QTreeWidgetItem* currentItem,
                                       QTreeWidgetItem* previousItem);
        
        void helpPagePrintButtonClicked();
        
        void topicSearchLineEditStartSearch();
        
        void topicSearchLineEditCursorPositionChanged(int,int);
        
        void topicExpandAllTriggered();
        
        void topicCollapseAllTriggered();
        
        void topicSearchListWidgetItemClicked(QListWidgetItem*);
        
    private:
        enum TreeItemType {
            TREE_ITEM_NONE,
            TREE_ITEM_HELP_PAGE,
            TREE_ITEM_WB_COMMAND
        };
        
        HelpViewerDialog(const HelpViewerDialog&);

        HelpViewerDialog& operator=(const HelpViewerDialog&);
        
        QWidget* createTableOfContentsWidget();
        
        QWidget* createIndexSearchWidget();
        
        QWidget* createHelpViewerWidget();
        
        void loadHelpTopicsIntoIndexTree();
        
        HelpTreeWidgetItem* createHelpTreeWidgetItemForHelpPage(QTreeWidgetItem* parent,
                                                                const AString& itemText,
                                                                const AString& helpPageURL);
        
        void displayHelpTextForHelpTreeWidgetItem(HelpTreeWidgetItem* helpItem);
        
        void addItemToParentMenu(QTreeWidgetItem* parentMenu,
                                 QTreeWidgetItem* item,
                                 const AString& itemName);
                               
        HelpTreeWidgetItem* loadWorkbenchHelpInfoFromDirectory(QTreeWidgetItem* parent,
                                                const QFileInfo& dirInfo);
        
        void addToAllItems(HelpTreeWidgetItem* helpItem);
        
        void loadSearchListWidget();
        
        /// the help browser
        QTextBrowser* m_helpBrowser;
        
        /// the splitter
        QSplitter* m_splitter;
        
        /// the topic index tree widget
        QTreeWidget* m_topicIndexTreeWidget;
        
        /// line edit for searching topics
        QLineEdit* m_topicSearchLineEdit;
        
        QListWidget* m_topicSearchListWidget;
        
        /// tracks first mouse click in search topic line edit
        bool m_topicSearchLineEditFirstMouseClick;
        
        /// All help pages in tree widget
        std::vector<HelpTreeWidgetItem*> m_allHelpTreeWidgetItems;
        
        /// All items in search list widget
        std::vector<HelpSearchListItem*> m_allHelpSearchListWidgetItems;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class HelpTextBrowser;

    };
    
    /**
     * The help text browser.
     */
    class HelpTextBrowser : public QTextBrowser {
        Q_OBJECT
        
    public:
        HelpTextBrowser(HelpViewerDialog* parentHelpViewerDialog);
        
        virtual ~HelpTextBrowser();
        
        virtual QVariant loadResource(int type, const QUrl& url);
        
        virtual void setSource(const QUrl& url);
        
    private:
        HelpViewerDialog* m_parentHelpViewerDialog;
        
    };
    
    /**
     * Base class for items in the tree widget.
     */
    class HelpTreeWidgetItem : public QTreeWidgetItem {
        
    public:
        enum TreeItemType {
            TREE_ITEM_NONE,
            TREE_ITEM_HELP_PAGE_URL,
            TREE_ITEM_HELP_TEXT
        };
        
        static HelpTreeWidgetItem* newInstanceForCommandOperation(QTreeWidgetItem* parent,
                                                           CommandOperation* commandOperation);
        
        static HelpTreeWidgetItem* newInstanceForHtmlHelpPage(QTreeWidgetItem* parent,
                                                   const AString& itemText,
                                                   const AString& helpPageURL);
        
        static HelpTreeWidgetItem* newInstanceEmptyItem(QTreeWidgetItem* parent,
                                                        const AString& itemText);
        
    private:
        HelpTreeWidgetItem(QTreeWidgetItem* parent,
                           const TreeItemType treeItemType,
                           const AString& itemText,
                           const AString& helpPageURL,
                           const AString& helpText);
        
        HelpTreeWidgetItem(const TreeItemType treeItemType,
                           const AString& itemText,
                           const AString& helpPageURL,
                           const AString& helpText);
        
    public:
        
        virtual ~HelpTreeWidgetItem();
        
        const TreeItemType m_treeItemType;
        
        const AString m_helpPageURL;
        
        const AString m_helpText;
    };
    
    class HelpSearchListItem : public QListWidgetItem {
    public:
        HelpSearchListItem(HelpTreeWidgetItem* matchingTreeWidgetItem);
        
        ~HelpSearchListItem();
        
        static bool sortAlphabetically(const HelpSearchListItem* h1,
                                       const HelpSearchListItem* h2);
        
        HelpTreeWidgetItem* m_matchingTreeWidgetItem;
    };
    
#ifdef __HELP_VIEWER_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HELP_VIEWER_DIALOG_DECLARE__

} // namespace
#endif  //__HELP_VIEWER_DIALOG_H__
