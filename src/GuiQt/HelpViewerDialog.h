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

#include <QTreeWidgetItem>

#include "WuQDialogNonModal.h"

class QLineEdit;
class QSplitter;
class QTextBrowser;
class QToolButton;
class QTreeWidget;
class QUrl;


namespace caret {

    class CommandOperation;
    class HelpTreeWidgetItem;
    
    class HelpViewerDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        HelpViewerDialog(QWidget* parent,
                         Qt::WindowFlags f = 0);
        
        virtual ~HelpViewerDialog();
        
        virtual void updateDialog();
        
        void showHelpPageWithName(const AString& helpPageName);
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void helpPageAnchorClicked(const QUrl&);
        
        void topicIndexTreeItemSelected(QTreeWidgetItem* item, int column);
        
        void topicSearchTreeItemSelected(QTreeWidgetItem* item, int column);
        
        void helpTextPrintButtonClicked();
        
        void helpTextFindButtonClicked();
        
        void helpTextFindNextButtonClicked();
        
        void topicSearchLineEditStartSearch();
        
        void topicSearchLineEditCursorPositionChanged(int,int);
        
    private:
        enum TreeItemType {
            TREE_ITEM_NONE,
            TREE_ITEM_HELP_PAGE,
            TREE_ITEM_WB_COMMAND
        };
        
        HelpViewerDialog(const HelpViewerDialog&);

        HelpViewerDialog& operator=(const HelpViewerDialog&);
        
        void loadHelpTopicsIntoIndexTree();
        
        HelpTreeWidgetItem* createHelpTreeWidgetItemForHelpPage(QTreeWidgetItem* parent,
                                                                const AString& itemText,
                                                                const AString& helpPageURL);
        
        void displayHelpTextForHelpTreeWidgetItem(HelpTreeWidgetItem* helpItem);
        
        void displayHttpInUsersWebBrowser(const AString& urlText);
        
        /// the help browser
        QTextBrowser* m_helpBrowser;
        
        /// the splitter
        QSplitter* m_splitter;
        
        /// the topic index tree widget
        QTreeWidget* m_topicIndexTreeWidget;
        
        /// the search tree widget
        QTreeWidget* m_topicSearchTreeWidget;
        
        /// line edit for searching topics
        QLineEdit* m_topicSearchLineEdit;
        
        /// tracks first mouse click in search topic line edit
        bool m_topicSearchLineEditFirstMouseClick;
        
        /// text when searching browser
        QString m_findInBrowserText;
        
        /// find next toolbutton
        QToolButton* m_findNextPushButton;
        
        /// All help pages
        std::vector<HelpTreeWidgetItem*> m_allHelpWidgetItems;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
    
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
        
    private:
        HelpTreeWidgetItem(QTreeWidgetItem* parent,
                           const TreeItemType treeItemType,
                           const AString& itemText,
                           const AString& helpPageURL,
                           const AString& helpText);
        
    public:
        
        virtual ~HelpTreeWidgetItem();
        
        const TreeItemType m_treeItemType;
        
        const AString m_helpPageURL;
        
        const AString m_helpText;
        
        
    };
    
#ifdef __HELP_VIEWER_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HELP_VIEWER_DIALOG_DECLARE__

} // namespace
#endif  //__HELP_VIEWER_DIALOG_H__
