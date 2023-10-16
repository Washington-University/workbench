#ifndef __DING_ONTOLOGY_TERMS_DIALOG_H__
#define __DING_ONTOLOGY_TERMS_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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



#include <memory>

#include "WuQDialogModal.h"

class QCompleter;
class QLineEdit;
class QModelIndex;
class QStandardItem;
class QStandardItemModel;
class QTabWidget;
class QTableView;
class QTreeView;

namespace caret {

    class DingOntologyTermsFile;
    
    class DingOntologyTermsDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        DingOntologyTermsDialog(const DingOntologyTermsFile* dingOntologyTermsFile,
                                QWidget* parent = 0);
        
        virtual ~DingOntologyTermsDialog();
        
        DingOntologyTermsDialog(const DingOntologyTermsDialog&) = delete;

        DingOntologyTermsDialog& operator=(const DingOntologyTermsDialog&) = delete;
        
        virtual QSize sizeHint() const override;
        
        QString getAbbreviatedName() const;
        
        QString getDescriptiveName() const;
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked() override;
        
    private slots:
        void tabBarClicked(int index);
        
        void tableViewItemClicked(const QModelIndex& index);
        
        void tableViewItemDoubleClicked(const QModelIndex& index);
        
        void treeViewItemClicked(const QModelIndex& index);
        
        void treeViewItemDoubleClicked(const QModelIndex& index);
                
        void abbreviatedNameCompleterActivated(const QString& text);
        
        void descriptiveNameCompleterActivated(const QString& text);
        
        void abbeviatedTextLineEditChanged(const QString& text);
        
    private:
        QWidget* createTreeWidget();
        
        QWidget* createTableWidget();
        
        const QStandardItem* getTableViewItemAtModelIndex(const QModelIndex& index);
        
        const QStandardItem* getTreeViewItemAtModelIndex(const QModelIndex& index);
        
        const QStandardItem* getItemAtModelIndex(const QStandardItemModel* model,
                                                 const QModelIndex& index);
        
        void setAbbreviatedAndDescriptiveNameLineEdits(const QStandardItem* item);
        
        const DingOntologyTermsFile* m_dingOntologyTermsFile;
    
        int32_t m_tableViewTabIndex = -1;
        
        int32_t m_treeViewTabIndex = -1;
        
        QTabWidget* m_tabWidget = NULL;
        
        QTableView* m_tableView = NULL;
        
        QTreeView* m_treeView = NULL;
        
        QLineEdit* m_abbreviatedNameLineEdit = NULL;
        
        QLineEdit* m_descriptiveNameLineEdit = NULL;
        
        QCompleter* m_abbreviatedNameCompleter = NULL;
        
        QCompleter* m_descriptiveNameCompleter = NULL;
        
        int32_t m_abbreviatedNameCompleterColumnIndex = -1;
        
        int32_t m_descriptiveNameCompleterColumnIndex = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DING_ONTOLOGY_TERMS_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DING_ONTOLOGY_TERMS_DIALOG_DECLARE__

} // namespace
#endif  //__DING_ONTOLOGY_TERMS_DIALOG_H__
