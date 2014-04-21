#ifndef __HELP_DIALOG_H__
#define __HELP_DIALOG_H__

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


#include "WuQDialogNonModal.h"

class QListWidget;
class QListWidgetItem;
class QSplitter;
class QTreeWidget;
class QTreeWidgetItem;
class QTextBrowser;

namespace caret {

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
        void topicTreeItemSelected(QTreeWidgetItem*,int);
        
    private:
        enum TreeItemType {
            TREE_ITEM_NONE,
            TREE_ITEM_HELP_PAGE,
            TREE_ITEM_WB_COMMAND
        };
        
        HelpViewerDialog(const HelpViewerDialog&);

        HelpViewerDialog& operator=(const HelpViewerDialog&);

        void loadHelpTopics();
        
        QTreeWidgetItem* createHelpPageFileItem(const AString& topicName,
                                                const AString& filePath,
                                                QTreeWidgetItem* parent) const;
        
        QTreeWidget* m_topicTreeWidget;

        QSplitter* m_splitter;
        
        QTextBrowser* m_helpBrowserWidget;
        
        bool m_helpDataHasBeenLoadedFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HELP_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HELP_DIALOG_DECLARE__

} // namespace
#endif  //__HELP_DIALOG_H__
