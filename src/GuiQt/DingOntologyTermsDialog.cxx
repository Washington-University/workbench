
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

#define __DING_ONTOLOGY_TERMS_DIALOG_DECLARE__
#include "DingOntologyTermsDialog.h"
#undef __DING_ONTOLOGY_TERMS_DIALOG_DECLARE__

#include <deque>

#include <QCompleter>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTabWidget>
#include <QTableView>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "DingOntologyTermsFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::DingOntologyTermsDialog
 * \brief Dialog for selecting name from ding ontology
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param dingOntologyTermsFile
 *   The ding ontology terms file MUST be valid
 * @param parent
 *   Parent widget
 */
DingOntologyTermsDialog::DingOntologyTermsDialog(const DingOntologyTermsFile* dingOntologyTermsFile,
                                                 QWidget* parent)
: WuQDialogModal("Ding Ontology",
                 parent),
m_dingOntologyTermsFile(dingOntologyTermsFile)
{
    CaretAssert(m_dingOntologyTermsFile);
    
    /*
     * When dialog is closed, its position and size are saved
     * and use next time dialog is opened (in same session)
     */
    setSaveWindowPositionForNextTime("DingOntologyTermsDialog");
    
    QWidget* treeWidget(createTreeWidget());
    
    QWidget* tableWidget(createTableWidget());
    
    /*
     * Note: Table columns are:
     * 1 - Abbreviated Name
     * 2 - Descriptive Name
     * 3 - Abbreviated Name and Descriptive Name used for "QCompleter"
     */
    m_abbreviatedNameCompleterColumnIndex = 2;
    m_abbreviatedNameCompleter = new QCompleter(this);
    m_abbreviatedNameCompleter->setModel(m_dingOntologyTermsFile->getTableModel());
    m_abbreviatedNameCompleter->setCompletionRole(Qt::EditRole);
    m_abbreviatedNameCompleter->setCompletionRole(m_dingOntologyTermsFile->getAbbreviatedNameItemRole());
    m_abbreviatedNameCompleter->setCompletionColumn(m_abbreviatedNameCompleterColumnIndex);
    m_abbreviatedNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(m_abbreviatedNameCompleter, QOverload<const QString &>::of(&QCompleter::activated),
                     this, &DingOntologyTermsDialog::abbreviatedNameCompleterActivated);
    
    const bool useDescriptiveNameCompleterFlag(true);
    m_descriptiveNameCompleter = NULL;
    if (useDescriptiveNameCompleterFlag) {
        m_descriptiveNameCompleterColumnIndex = 2;
        m_descriptiveNameCompleter = new QCompleter(this);
        m_descriptiveNameCompleter->setFilterMode(Qt::MatchContains);
        m_descriptiveNameCompleter->setModel(m_dingOntologyTermsFile->getTableModel());
        m_descriptiveNameCompleter->setCompletionRole(Qt::EditRole);
        m_descriptiveNameCompleter->setCompletionRole(m_dingOntologyTermsFile->getDescriptiveNameItemRole());
        m_descriptiveNameCompleter->setCompletionColumn(m_descriptiveNameCompleterColumnIndex);
        m_descriptiveNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        QObject::connect(m_descriptiveNameCompleter, QOverload<const QString &>::of(&QCompleter::activated),
                         this, &DingOntologyTermsDialog::descriptiveNameCompleterActivated);
    }
    

    m_tabWidget = new QTabWidget();
    m_treeViewTabIndex  = m_tabWidget->addTab(treeWidget, "Hierarchy");
    m_tableViewTabIndex = m_tabWidget->addTab(tableWidget, "List");
    QObject::connect(m_tabWidget, &QTabWidget::tabBarClicked,
                     this, &DingOntologyTermsDialog::tabBarClicked);
    
    QLabel* abbreviatedNameLabel(new QLabel(DingOntologyTermsFile::getAbbreviatedNameTitle() + ":"));
    m_abbreviatedNameLineEdit = new QLineEdit();
    if (m_abbreviatedNameCompleter != NULL) {
        m_abbreviatedNameLineEdit->setCompleter(m_abbreviatedNameCompleter);
    }
    QObject::connect(m_abbreviatedNameLineEdit, &QLineEdit::textChanged,
                     this, &DingOntologyTermsDialog::abbeviatedTextLineEditChanged);
    m_abbreviatedNameLineEdit->setToolTip("Enter text to find matches");
    
    QLabel* descriptiveNameLabel(new QLabel(DingOntologyTermsFile::getDescriptiveNameTitle() + ":"));
    m_descriptiveNameLineEdit  = new QLineEdit();
    if (m_descriptiveNameCompleter != NULL) {
        m_descriptiveNameLineEdit->setCompleter(m_descriptiveNameCompleter);
    }
    m_descriptiveNameLineEdit->setToolTip("Enter text to find matches");

    QGridLayout* namesLayout(new QGridLayout());
    namesLayout->setColumnStretch(0, 0);
    namesLayout->setColumnStretch(1, 100);
    namesLayout->addWidget(abbreviatedNameLabel, 0, 0);
    namesLayout->addWidget(m_abbreviatedNameLineEdit, 0, 1);
    namesLayout->addWidget(descriptiveNameLabel, 1, 0);
    namesLayout->addWidget(m_descriptiveNameLineEdit, 1, 1);
    
    QWidget* dialogWidget(new QWidget());
    QVBoxLayout* dialogLayout(new QVBoxLayout(dialogWidget));
    dialogLayout->addWidget(m_tabWidget);
    dialogLayout->addLayout(namesLayout);
    
    setCentralWidget(dialogWidget, SCROLL_AREA_NEVER);
    
    m_tabWidget->setCurrentIndex(m_treeViewTabIndex);
    tabBarClicked(m_tabWidget->currentIndex());
    
    CaretAssert(m_treeViewExpandToLevelSpinBox);
    m_treeViewExpandToLevelSpinBox->setValue(s_previousTreeViewExpansionDepth);
    treeViewExpandToLevelSpinBoxValueChanged(m_treeViewExpandToLevelSpinBox->value());
    if (s_previousSelectedModelIndex.isValid()) {
        m_treeView->setCurrentIndex(s_previousSelectedModelIndex);
    }
}

/**
 * @return hint for size of this dialog
 */
QSize
DingOntologyTermsDialog::sizeHint() const
{
    return QSize(600, 500);
}

/**
 * Destructor.
 */
DingOntologyTermsDialog::~DingOntologyTermsDialog()
{
}

/**
 * @return Newly created instance of the tree widget for ontology terms
 */
QWidget*
DingOntologyTermsDialog::createTreeWidget()
{
    QStandardItemModel* treeModel(m_dingOntologyTermsFile->getTreeModel());
    m_treeView = new QTreeView();
    m_treeView->header()->hide();
    m_treeView->setModel(treeModel);
    QObject::connect(m_treeView, &QTreeView::clicked,
                     this, &DingOntologyTermsDialog::treeViewItemClicked);
    QObject::connect(m_treeView, &QTreeView::doubleClicked,
                     this, &DingOntologyTermsDialog::treeViewItemDoubleClicked);
    QObject::connect(m_treeView, &QTreeView::expanded,
                     this, &DingOntologyTermsDialog::treeViewItemExpanded);
    
    m_treeView->expandAll();
    
    QLabel* expandToLevelLabel(new QLabel("Expand to Level"));
    m_treeViewExpandToLevelSpinBox = new QSpinBox();
    m_treeViewExpandToLevelSpinBox->setMinimum(1);
    m_treeViewExpandToLevelSpinBox->setMaximum(1000);
    m_treeViewExpandToLevelSpinBox->setSingleStep(1);
    QObject::connect(m_treeViewExpandToLevelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &DingOntologyTermsDialog::treeViewExpandToLevelSpinBoxValueChanged);
    
    QToolButton* collapseAllToolButton(new QToolButton());
    collapseAllToolButton->setText("Collapse All");
    QObject::connect(collapseAllToolButton, &QToolButton::clicked,
                     this, &DingOntologyTermsDialog::treeViewCollapseAllButtonClicked);
    
    QToolButton* expandAllToolButton(new QToolButton());
    expandAllToolButton->setText("Expand All");
    QObject::connect(expandAllToolButton, &QToolButton::clicked,
                     this, &DingOntologyTermsDialog::treeViewExpandAllButtonClicked);
    
    QHBoxLayout* expandCollapseLayout(new QHBoxLayout());
    expandCollapseLayout->addWidget(expandToLevelLabel);
    expandCollapseLayout->addWidget(m_treeViewExpandToLevelSpinBox);
    expandCollapseLayout->addStretch();
    expandCollapseLayout->addWidget(collapseAllToolButton);
    expandCollapseLayout->addWidget(expandAllToolButton);
    
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addLayout(expandCollapseLayout, 0);
    layout->addWidget(m_treeView, 100);

    return widget;
}

/**
 * @return Newly created instance of the table widget for ontology terms
 */
QWidget*
DingOntologyTermsDialog::createTableWidget()
{
    /*
     * At this time, sort filter does not work
     * for allowing case insensitive sorting
     */
    QSortFilterProxyModel* tableSortFilterProxyModel(NULL);
    const bool useSortFilterFlag(false);
    if (useSortFilterFlag) {
        tableSortFilterProxyModel = new QSortFilterProxyModel(this);
        tableSortFilterProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        tableSortFilterProxyModel->setSourceModel(m_dingOntologyTermsFile->getTableModel());
    }
    
    /*
     * Note: Table columns are:
     * 1 - Abbreviated Name
     * 2 - Descriptive Name
     * 3 - Abbreviated Name and Descriptive Name used for "QCompleter"
     * Note column 3 is hidden from view.
     */

    m_tableView = new QTableView();
    if (useSortFilterFlag) {
        m_tableView->setModel(tableSortFilterProxyModel);
        CaretAssert(0);  /* COMPLETER NEEDS UPDATE??? */
    }
    else {
        m_tableView->setModel(m_dingOntologyTermsFile->getTableModel());
    }
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setSortingEnabled(true);
    m_tableView->horizontalHeader()->setSortIndicatorShown(true);
    QObject::connect(m_tableView, &QTableView::clicked,
                     this, &DingOntologyTermsDialog::tableViewItemClicked);
    QObject::connect(m_tableView, &QTableView::doubleClicked,
                     this, &DingOntologyTermsDialog::tableViewItemDoubleClicked);
    
    m_tableView->setColumnHidden(2, true);
    
    /*
     * Sort by abbreviated name
     */
    m_tableView->sortByColumn(0, Qt::AscendingOrder);
    
    QWidget* widget(new QWidget);
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->addWidget(m_tableView);
    
    return widget;
}

/**
 * Called when the user abbreviated text line edit is changed
 * @param text
 *    New text
 */
void
DingOntologyTermsDialog::abbeviatedTextLineEditChanged(const QString& /*text*/)
{
}

/**
 * Called when the user chooses an item in the abbreviated name completer
 * @param Text
 *    Text that was selected
 */
void
DingOntologyTermsDialog::abbreviatedNameCompleterActivated(const QString& text)
{
    const bool debugFlag(false);
    if (debugFlag) {
        std::cout << "Abbreviated Completer Text: " << text.toStdString() << std::endl;
    }
    
    const QStandardItemModel* tableModel(m_dingOntologyTermsFile->getTableModel());
    if (tableModel != NULL) {
        /*
         * We need to use Qt::MatchStartsWith because we match to
         * getAbbreviatedNameItemRole() which only contains the abbreviated name
         * BUT the completions are in the third column and contain the
         * abbreviated name followed by the descriptive name.
         */
        QList<QStandardItem*> matchingItems(tableModel->findItems(text,
                                                                  Qt::MatchStartsWith,
                                                                  m_abbreviatedNameCompleterColumnIndex));
        
        if (debugFlag) {
            const int32_t numItems(matchingItems.size());
            for (int32_t i = 0; i < numItems; i++) {
                const QStandardItem* item(matchingItems.at(i));
                const QString abbrevName(m_dingOntologyTermsFile->getAbbreviatedName(item));
                const QString descripName(m_dingOntologyTermsFile->getDescriptiveName(item));
                std::cout << "   Matching: " << abbrevName.toStdString()
                << " ---- " << descripName.toStdString() << std::endl;
            }
        }
        /*
         * There may be multiple items that match but use the first item.
         * If the user types CaH, these items will appear and there will
         * be four matches:
         * CaH - head of caudate
         * CaHdl - dorsolateral part of CaH
         * CaHr - rostral pole of CaH
         * CaHvm - ventromedial part of CaH
         */
        if (matchingItems.size() >= 1) {
            /*
             * Default to first item
             */
            const QStandardItem* matchedItem(matchingItems.first());

            /*
             * Look for an exact match
             */
            const int32_t numItems(matchingItems.size());
            for (int32_t i = 0; i < numItems; i++) {
                const QStandardItem* item(matchingItems.at(i));
                const QString abbrevName(m_dingOntologyTermsFile->getAbbreviatedName(item));
                
                if (text.trimmed() == abbrevName.trimmed()) {
                    matchedItem = item;
                    break;
                }
            }
            
            CaretAssert(matchedItem);
            
            /*
             * Update the line edits with selected names
             */
            const QString abbrevName(m_dingOntologyTermsFile->getAbbreviatedName(matchedItem));
            const QString descripName(m_dingOntologyTermsFile->getDescriptiveName(matchedItem));
            m_abbreviatedNameLineEdit->setText(abbrevName);
            m_descriptiveNameLineEdit->setText(descripName);
            
            /*
             * Select row in the table model
             */
            m_tableView->selectRow(matchedItem->row());
        }
    }
    
    /*
     * Highlighting item in tree does not work at this time
     */
    const bool highlightMatchingTreeModelItemFlag(false);
    if (highlightMatchingTreeModelItemFlag) {
        const QStandardItemModel* treeModel(m_dingOntologyTermsFile->getTreeModel());
        if (treeModel != NULL) {
            const int32_t numCols(treeModel->columnCount());
            for (int32_t iCol = 0; iCol < numCols; iCol++) {
                QList<QStandardItem*> matchingItems(treeModel->findItems(text,
                                                                         Qt::MatchStartsWith | Qt::MatchRecursive,
                                                                         iCol));
                if (matchingItems.size() == 1) {
                    const QStandardItem* item(matchingItems.first());
                    m_treeView->scrollTo(item->index());
                    break;
                }
            }
        }
    }
}

/**
 * Called when the user chooses an item in the descriptive name completer
 * @param Text
 *    Text that was selected
 */
void
DingOntologyTermsDialog::descriptiveNameCompleterActivated(const QString& text)
{
    const QStandardItemModel* tableModel(m_dingOntologyTermsFile->getTableModel());
    if (tableModel != NULL) {
        /*
         * We need to use Qt::MatchStartsWith because we match to
         * getAbbreviatedNameItemRole() which only contains the abbreviated name
         * BUT the completions are in the third column and contain the
         * abbreviated name followed by the descriptive name.
         */
        QList<QStandardItem*> matchingItems(tableModel->findItems(text,
                                                                  Qt::MatchContains,
                                                                  m_descriptiveNameCompleterColumnIndex));
        if (matchingItems.size() >= 1) {
            /*
             * Update the line edits with selected names
             */
            const QStandardItem* item(matchingItems.first());
            const QString abbrevName(m_dingOntologyTermsFile->getAbbreviatedName(item));
            const QString descripName(m_dingOntologyTermsFile->getDescriptiveName(item));
            m_abbreviatedNameLineEdit->setText(abbrevName);
            m_descriptiveNameLineEdit->setText(descripName);
            
            /*
             * Select row in the table model
             */
            m_tableView->selectRow(item->row());
        }
    }
    
    /*
     * Highlighting item in tree does not work at this time
     */
    const bool highlightMatchingTreeModelItemFlag(false);
    if (highlightMatchingTreeModelItemFlag) {
        const QStandardItemModel* treeModel(m_dingOntologyTermsFile->getTreeModel());
        if (treeModel != NULL) {
            const int32_t numCols(treeModel->columnCount());
            for (int32_t iCol = 0; iCol < numCols; iCol++) {
                QList<QStandardItem*> matchingItems(treeModel->findItems(text,
                                                                         Qt::MatchStartsWith,
                                                                         iCol));
                if (matchingItems.size() == 1) {
                    const QStandardItem* item(matchingItems.first());
                    m_descriptiveNameLineEdit->setText(m_dingOntologyTermsFile->getDescriptiveName(item));
                    
                    std::cout << "Tree view item in row " << item->row() << " column " << iCol << std::endl;
                    break;
                }
                if ( ! matchingItems.isEmpty()) {
                    std::cout << "Table matched " << matchingItems.size() << " items" << std::endl;
                }
            }
        }
    }
}

/**
 * Called when tab bar is clicked by the user
 * @param index
 *    Index of the tab
 */
void
DingOntologyTermsDialog::tabBarClicked(int /*index*/)
{
}

/**
 * Called when expand to level spin box values is changed
 */
void
DingOntologyTermsDialog::treeViewExpandToLevelSpinBoxValueChanged(int value)
{
    m_treeView->expandToDepth(value - 1);
}

/**
 * Called when collapse all button is clicked
 */
void
DingOntologyTermsDialog::treeViewCollapseAllButtonClicked(bool)
{
    CaretAssert(m_treeView);

    /*
     * QTreeView::collapseAll() collapse one level too deep
     * so set the tree depth
     */
    QSignalBlocker blocker(m_treeViewExpandToLevelSpinBox);
    m_treeViewExpandToLevelSpinBox->setValue(1);
    
    /*
     * Needed to apply the new depth value
     */
    treeViewExpandToLevelSpinBoxValueChanged(m_treeViewExpandToLevelSpinBox->value());
}

/**
 * Called when expand all button is clicked
 */
void
DingOntologyTermsDialog::treeViewExpandAllButtonClicked(bool)
{
    CaretAssert(m_treeView);
    m_treeView->expandAll();
}

/**
 * @return Depth in tree of an item in the tree
 * @param index
 *    Model index of item
 */
int32_t
DingOntologyTermsDialog::getTreeViewItemDepth(const QModelIndex& index) const
{
    int32_t depth(0);
    
    QModelIndex modelIndex(index);
    while (modelIndex.isValid()) {
        modelIndex = modelIndex.parent();
        if (modelIndex.isValid()) {
            ++depth;
        }
    }
    
    return depth;
}


/**
 * Called when an item is expanded in the tree view
 * @param index
 *    Index of item expanded
 */
void
DingOntologyTermsDialog::treeViewItemExpanded(const QModelIndex& /*index*/)
{
}


/**
 * Called when an item is clicked in the tree view
 * @param index
 *    Index of item clicked
 */
void
DingOntologyTermsDialog::treeViewItemClicked(const QModelIndex& index)
{
    const QStandardItem* item(getTreeViewItemAtModelIndex(index));
    setAbbreviatedAndDescriptiveNameLineEdits(item);
}

/**
 * Called when an item is double-clicked in the tree view
 * @param index
 *    Index of item clicked
 */
void
DingOntologyTermsDialog::treeViewItemDoubleClicked(const QModelIndex& index)
{
    const QStandardItem* item(getTreeViewItemAtModelIndex(index));
    setAbbreviatedAndDescriptiveNameLineEdits(item);
}

/**
 * Called when an item is clicked in the table view
 * @param index
 *    Index of item clicked
 */
void
DingOntologyTermsDialog::tableViewItemClicked(const QModelIndex& index)
{
    const QStandardItem* item(getTableViewItemAtModelIndex(index));
    setAbbreviatedAndDescriptiveNameLineEdits(item);
    if (item != NULL) {
        m_tableView->selectRow(item->row());
    }
}

/**
 * Called when an item is double-clicked in the table view
 * @param index
 *    Index of item clicked
 */
void
DingOntologyTermsDialog::tableViewItemDoubleClicked(const QModelIndex& index)
{
    const QStandardItem* item(getTableViewItemAtModelIndex(index));
    setAbbreviatedAndDescriptiveNameLineEdits(item);
    if (item != NULL) {
        m_tableView->selectRow(item->row());
    }
}

/**
 * Set the abbreviated and descriptive name line edits from names in the standard item
 * @param item
 *    Standard item containing names
 */
void
DingOntologyTermsDialog::setAbbreviatedAndDescriptiveNameLineEdits(const QStandardItem* item)
{
    if (item != NULL) {
        m_abbreviatedNameLineEdit->setText(m_dingOntologyTermsFile->getAbbreviatedName(item));
        m_descriptiveNameLineEdit->setText(m_dingOntologyTermsFile->getDescriptiveName(item));
    }
}


/**
 * @return The tree view item at the model index or NULL if not valid.
 * @param index
 *    The model index.
 */
const QStandardItem*
DingOntologyTermsDialog::getTreeViewItemAtModelIndex(const QModelIndex& index)
{
    return getItemAtModelIndex(m_dingOntologyTermsFile->getTreeModel(),
                               index);
}

/**
 * @return The list view item at the model index or NULL if not valid.
 * @param index
 *    The model index.
 */
const QStandardItem*
DingOntologyTermsDialog::getTableViewItemAtModelIndex(const QModelIndex& index)
{
    return getItemAtModelIndex(m_dingOntologyTermsFile->getTableModel(),
                               index);
}

/**
 * @return The  item at the model's index or NULL if not valid.
 * @param model
 *    The model
 * @param index
 *    The model index.
 */
const QStandardItem*
DingOntologyTermsDialog::getItemAtModelIndex(const QStandardItemModel* model,
                                             const QModelIndex& index)
{
    CaretAssert(model);
    if (index.isValid()) {
        const QStandardItem* item(model->itemFromIndex(index));
        return item;
    }
    return NULL;
}

/**
 * Called when OK button clicked
 */
void
DingOntologyTermsDialog::okButtonClicked()
{
    s_previousTreeViewExpansionDepth = m_treeViewExpandToLevelSpinBox->value();
    s_previousSelectedModelIndex     = m_treeView->currentIndex();
    
    AString errorMessage;
    if (getAbbreviatedName().isEmpty()) {
        errorMessage.appendWithNewLine("Abbreviated Name is invalid.");
    }
    if (getDescriptiveName().isEmpty()) {
        errorMessage.appendWithNewLine("Descriptive Name is invalid.");
    }
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this,
                                errorMessage);
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}

/**
 * @return The abbeviated name selected by theuser
 */
QString
DingOntologyTermsDialog::getAbbreviatedName() const
{
    return m_abbreviatedNameLineEdit->text().trimmed();
}

/**
 * @return The descriptive name selected by theuser
 */
QString
DingOntologyTermsDialog::getDescriptiveName() const
{
    return m_descriptiveNameLineEdit->text().trimmed();
}

