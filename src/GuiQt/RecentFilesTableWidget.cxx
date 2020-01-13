
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __RECENT_FILES_TABLE_WIDGET_DECLARE__
#include "RecentFilesTableWidget.h"
#undef __RECENT_FILES_TABLE_WIDGET_DECLARE__

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QCursor>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "RecentFileItem.h"
#include "RecentFileItemsContainer.h"
#include "RecentFileItemsFilter.h"
#include "WuQImageLabel.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::RecentFilesTableWidget
 * \brief Widget containing recent file and directory items
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
RecentFilesTableWidget::RecentFilesTableWidget()
: QTableWidget()
{
    setAlternatingRowColors(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QAbstractItemView::SingleSelection);
    
    QHeaderView* horizHeader = horizontalHeader();
    horizHeader->setSortIndicatorShown(true);
    QObject::connect(horizHeader, &QHeaderView::sortIndicatorChanged,
                     this, &RecentFilesTableWidget::sortIndicatorClicked);
    
    QHeaderView* vertHeader = verticalHeader();
    vertHeader->setVisible(false);
    
    QObject::connect(this, &QTableWidget::cellClicked,
                     this, &RecentFilesTableWidget::tableCellClicked);
    QObject::connect(this, &QTableWidget::cellDoubleClicked,
                     this, &RecentFilesTableWidget::tableCellDoubleClicked);

    m_favoriteFilledIcon  = loadIcon(":/RecentFilesDialog/favorite_filled.png");
    m_favoriteOutlineIcon = loadIcon(":/RecentFilesDialog/favorite_outline.png");
    m_forgetIcon          = loadIcon(":/RecentFilesDialog/forget_black.png");
    m_forgetOnIcon        = loadIcon(":/RecentFilesDialog/forget_red.png");
    m_shareIcon           = loadIcon(":/RecentFilesDialog/share.png");
}

/**
 * Destructor.
 */
RecentFilesTableWidget::~RecentFilesTableWidget()
{
}

/**
 * @return The selected item or NULL if no item is selected
 */
RecentFileItem*
RecentFilesTableWidget::getSelectedItem()
{
    const int32_t row = currentRow();
    if ((row >= 0)
        && (row < rowCount())) {
        CaretAssertVectorIndex(m_recentItems, row);
        return m_recentItems[row];
    }
    
    return NULL;
}

/**
 * @return The size hint adjusted for width of recent files table
 */
QSize
RecentFilesTableWidget::sizeHint() const
{
    QSize sz = QTableWidget::sizeHint();
    
    int32_t x(0);
    for (int32_t i = 0; i < columnCount(); i++) {
        x += columnWidth(i);
    }
    sz.setWidth(x);
    
    return sz;
}

/**
 * Update number of rows in table
 * @param numberOfItems
 *  Number of files for table
 */
void
RecentFilesTableWidget::updateTableDimensions(const int32_t numberOfItems)
{
    const int32_t numExistingRows = rowCount();
    
    if (numberOfItems != numExistingRows) {
        /*
         * Resize table to match number of files
         */
        setRowCount(numberOfItems);
        setColumnCount(COLUMN_COUNT);
        
        if (numExistingRows > numberOfItems) {
            /*
             * Remove extra items
             */
            m_recentItems.resize(numberOfItems);
        }
    }
    
    /*
     * If needed, add additional rows
     */
    for (int32_t iRow = numExistingRows; iRow < numberOfItems; iRow++) {
        m_recentItems.push_back(NULL);
        
        for (int32_t iCol = 0; iCol < COLUMN_COUNT; iCol++) {
            QWidget* widget(NULL);
            QTableWidgetItem* tableItem(NULL);
            const COLUMNS column = static_cast<COLUMNS>(iCol);
            bool selectableFlag(false);
            AString toolTipText;
            switch (column) {
                case COLUMN_COUNT:
                    CaretAssert(0);
                    break;
                case COLUMN_NAME:
                {
                    WuQImageLabel* label = new WuQImageLabel();
                    label->setAlignment(Qt::AlignLeft);
                    label->setTextFormat(Qt::RichText);
                    widget = label;
                    
                    QObject::connect(label, &WuQImageLabel::clicked,
                                     [=] { tableCellClicked(iRow, COLUMN_NAME); });
                    QObject::connect(label, &WuQImageLabel::doubleClicked,
                                     [=] { tableCellDoubleClicked(iRow, COLUMN_NAME); });
                }
                    break;
                case COLUMN_DATE_TIME:
                    tableItem = new QTableWidgetItem();
                    toolTipText = "Data last modified";
                    selectableFlag = true;
                    break;
                case COLUMN_FAVORITE:
                {
                    WuQImageLabel* label = new WuQImageLabel();
                    label->setAlignment(Qt::AlignCenter);
                    QObject::connect(label, &WuQImageLabel::clicked,
                                     [=] { tableCellClicked(iRow, COLUMN_FAVORITE); });
                    QObject::connect(label, &WuQImageLabel::doubleClicked,
                                     [=] { tableCellDoubleClicked(iRow, COLUMN_FAVORITE); });
                    toolTipText = "Add/Remove this row as a favorite";
                    widget = label;
                }
                    break;
                case COLUMN_SHARE:
                {
                    WuQImageLabel* label = new WuQImageLabel();
                    label->setAlignment(Qt::AlignCenter);
                    if (m_shareIcon) {
                        label->setPixmap(m_shareIcon->pixmap(s_pixmapSizeXY, s_pixmapSizeXY));
                    }
                    else {
                        label->setText("fav");
                    }
                    QObject::connect(label, &WuQImageLabel::clicked,
                                     [=] { tableCellClicked(iRow, COLUMN_SHARE); });
                    QObject::connect(label, &WuQImageLabel::doubleClicked,
                                     [=] { tableCellDoubleClicked(iRow, COLUMN_SHARE); });
                    toolTipText = "Share path to this directory/file";
                    widget = label;
                }
                    break;
                case COLUMN_FORGET:
                {
                    WuQImageLabel* label = new WuQImageLabel();
                    label->setAlignment(Qt::AlignCenter);
                    QObject::connect(label, &WuQImageLabel::clicked,
                                     [=] { tableCellClicked(iRow, COLUMN_FORGET); });
                    QObject::connect(label, &WuQImageLabel::doubleClicked,
                                     [=] { tableCellDoubleClicked(iRow, COLUMN_FORGET); });
                    toolTipText = ("Forget this row (removed from recent directory/file history)\n"
                                   "Actual directory/file is NOT deleted");
                    widget = label;
                }
                    break;
            }
            
            if (tableItem != NULL) {
                Qt::ItemFlags flags(Qt::ItemIsEnabled);
                if (selectableFlag) {
                    flags.setFlag(Qt::ItemIsSelectable, true);
                }
                tableItem->setFlags(flags);
                tableItem->setData(Qt::UserRole, iRow);
                if ( ! toolTipText.isEmpty()) {
                    tableItem->setToolTip(toolTipText);
                }
                setItem(iRow, iCol, tableItem);
            }
            else if (widget != NULL) {
                if ( ! toolTipText.isEmpty()) {
                    widget->setToolTip(toolTipText);
                }
                setCellWidget(iRow, iCol, widget);
            }
            else {
                CaretAssert(0);
            }
        }
    }
}

/**
 * @return Name for column index
 * @param columnIndex
 * Index of column
 */
AString
RecentFilesTableWidget::getColumnName(const int32_t columnIndex) const
{
    AString name;
    
    const COLUMNS column = static_cast<COLUMNS>(columnIndex);
    switch (column) {
        case COLUMN_COUNT:
            name = "PROGRAM ERROR";
            CaretAssert(0);
            break;
        case COLUMN_NAME:
            name = "Name";
            break;
        case COLUMN_DATE_TIME:
            name = "Last Accessed";
            break;
        case COLUMN_FAVORITE:
            name = "Favorite";
            break;
        case COLUMN_SHARE:
            name = "Share";
            break;
        case COLUMN_FORGET:
            name = "Forget";
            break;
    }
    
    return name;
}


/**
 * Update the content with recent file items container
 * @param recentFileItemsContainer
 *    The container
 * @param itemsFilter
 *    Filter for requesting items from the container
 */
void
RecentFilesTableWidget::updateContent(RecentFileItemsContainer* recentFileItemsContainer,
                                      const RecentFileItemsFilter& itemsFilter)
{
    
    RecentFileItem* previousSelectedItem = getSelectedItem();
    m_recentItems.clear();
    clearSelectedItem();
    
    if (recentFileItemsContainer != m_recentFileItemsContainer) {
        previousSelectedItem = NULL;
    }
    
    m_recentFileItemsFilter = itemsFilter;
    
    m_recentFileItemsContainer = recentFileItemsContainer;
    if (m_recentFileItemsContainer != NULL) {
        m_recentItems = m_recentFileItemsContainer->getItems(itemsFilter);
        sortRecentItems();
    }

    const int32_t numberOfRecentItems = static_cast<int32_t>(m_recentItems.size());
    
    updateTableDimensions(numberOfRecentItems);
    
    CaretAssert(rowCount() == numberOfRecentItems);
        
    int32_t selectedRowIndex(-1);
    for (int32_t iRow = 0; iRow < numberOfRecentItems; iRow++) {
        updateRow(iRow);
        CaretAssertVectorIndex(m_recentItems, iRow);
        if (m_recentItems[iRow] == previousSelectedItem) {
            selectedRowIndex = iRow;
        }
    }
    
    /*
     * First time files inserted?
     */
    if (m_lastNumberRecentItems == 0) {
        if (numberOfRecentItems > 0) {
            QStringList columnNames;
            for (int32_t i = 0; i < COLUMN_COUNT; i++) {
                columnNames << getColumnName(i);
            }
            setHorizontalHeaderLabels(columnNames);
            
            resizeColumnsToContents();
            resizeRowsToContents();
            
            horizontalHeader()->setSectionResizeMode(COLUMN_FAVORITE, QHeaderView::ResizeToContents);
            horizontalHeader()->setSectionResizeMode(COLUMN_SHARE, QHeaderView::ResizeToContents);
            horizontalHeader()->setSectionResizeMode(COLUMN_FORGET, QHeaderView::ResizeToContents);
        }
    }
    
    m_lastNumberRecentItems = numberOfRecentItems;
    
    if (selectedRowIndex < 0) {
        if (numberOfRecentItems > 0) {
            selectedRowIndex = 0;
        }
    }
    
    update();
    tableCellClicked(selectedRowIndex, COLUMN_NAME);
    resizeRowsToContents();
    update();
    updateHeaderSortingKey();
}

/**
 * Update the content of a row
 * @param rowIndex
 * Row index
 */
void
RecentFilesTableWidget::updateRow(const int32_t rowIndex)
{
    CaretAssertVectorIndex(m_recentItems, rowIndex);
    RecentFileItem* recentItem = m_recentItems[rowIndex];
    CaretAssert(recentItem);
    
    bool enabledFavoriteAndForget(false);
    switch (m_recentFileItemsContainer->getMode()) {
        case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
            break;
        case RecentFileItemsContainerModeEnum::FAVORITES:
            enabledFavoriteAndForget = true;
            break;
        case RecentFileItemsContainerModeEnum::OTHER:
            break;
        case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
            enabledFavoriteAndForget = true;
            break;
        case RecentFileItemsContainerModeEnum::RECENT_FILES:
            enabledFavoriteAndForget = true;
            break;
    }
    
    switch (recentItem->getFileItemType()) {
        case RecentFileItemTypeEnum::DIRECTORY:
            break;
        case RecentFileItemTypeEnum::SCENE_FILE:
            break;
        case RecentFileItemTypeEnum::SPEC_FILE:
            break;
    }
    
    for (int32_t iCol = 0; iCol < COLUMN_COUNT; iCol++) {
        const COLUMNS column = static_cast<COLUMNS>(iCol);
        
        QTableWidgetItem* tableItem(item(rowIndex, iCol));
        QWidget* widget(cellWidget(rowIndex, iCol));
        switch (column) {
            case COLUMN_COUNT:
                CaretAssert(0);
                break;
            case COLUMN_NAME:
            {
                CaretAssert(widget);
                QLabel* label = qobject_cast<QLabel*>(widget);
                CaretAssert(label);
                AString text("<html>&nbsp;<b><font size=\"+1\">%1</font></b><br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%2</html>");
                label->setText(text.arg(recentItem->getFileName()).arg(recentItem->getPathName()));
            }
                break;
            case COLUMN_DATE_TIME:
            {
                tableItem->setText(recentItem->getLastAccessDateTimeAsString());
            }
                break;
            case COLUMN_FAVORITE:
            {
                CaretAssert(widget);
                QLabel* label = qobject_cast<QLabel*>(widget);
                CaretAssert(label);
                
                label->setText("");
                if (enabledFavoriteAndForget) {
                    if (recentItem->isFavorite()) {
                        if (m_favoriteFilledIcon) {
                            label->setPixmap(m_favoriteFilledIcon->pixmap(s_pixmapSizeXY, s_pixmapSizeXY));
                        }
                        else {
                            label->setText("Yes");
                        }
                    }
                    else {
                        if (m_favoriteOutlineIcon) {
                            label->setPixmap(m_favoriteOutlineIcon->pixmap(s_pixmapSizeXY, s_pixmapSizeXY));
                        }
                        else {
                            label->setText("No");
                        }
                    }
                }
                else {
                    label->setPixmap(QPixmap());
                }
            }
                break;
            case COLUMN_SHARE:
                CaretAssert(widget);
                break;
            case COLUMN_FORGET:
            {
                CaretAssert(widget);
                QLabel* label = qobject_cast<QLabel*>(widget);
                CaretAssert(label);
                
                /*
                 * Note: Same icon is used for forget on/off but may change
                 */
                label->setText("");
                if (enabledFavoriteAndForget) {
                    if (recentItem->isForget()) {
                        if (m_forgetOnIcon) {
                            label->setPixmap(m_forgetOnIcon->pixmap(s_pixmapSizeXY, s_pixmapSizeXY));
                        }
                        else {
                            label->setText("X");
                        }
                    }
                    else {
                        if (m_forgetIcon) {
                            label->setPixmap(m_forgetIcon->pixmap(s_pixmapSizeXY, s_pixmapSizeXY));
                        }
                        else {
                            label->setText("X");
                        }
                    }
                }
                else {
                    label->setPixmap(QPixmap());
                }
            }
                break;
        }
    }
}


/**
 * Called when a table item is clicked
 * @param row
 * Row clicked
 * @param column
 * Column clicked
 */
void
RecentFilesTableWidget::tableCellClicked(int row, int column)
{
    int32_t rowToSelect(-1);
    if ((row >= 0)
        && (row < rowCount())) {
        const COLUMNS tableColumn = static_cast<COLUMNS>(column);
        CaretAssertVectorIndex(m_recentItems, row);
        RecentFileItem* recentItem = m_recentItems[row];
        
        rowToSelect = row;
        
        switch (tableColumn) {
            case COLUMN_COUNT:
                break;
            case COLUMN_FAVORITE:
                recentItem->setFavorite( ! recentItem->isFavorite());
                updateRow(row);
                break;
            case COLUMN_FORGET:
                recentItem->setForget( ! recentItem->isForget());
                updateRow(row);
                break;
            case COLUMN_NAME:
                break;
            case COLUMN_DATE_TIME:
                break;
            case COLUMN_SHARE:
                showShareMenuForRow(row);
                break;
        }
    }
    /*
     * Select both the NAME and DATE/TIME columns in the row
     */
    QSignalBlocker blocker(this);
    clearSelectedItem();

    if (rowToSelect >= 0) {
        /*
         * Must set focus or else the selected item will not be highlighted
         * if the table does not have focus
         */
        setFocus();
        QItemSelectionModel::SelectionFlags flags;
        flags.setFlag(QItemSelectionModel::Select, true);
        flags.setFlag(QItemSelectionModel::Rows, true);
        setCurrentCell(row, COLUMN_NAME, flags);
    }
    
    emit selectedItemChanged(getSelectedItem());
}

/**
 * Clear the selected item
 */
void
RecentFilesTableWidget::clearSelectedItem()
{
    clearSelection();
    setCurrentItem(NULL);
}


/**
 * Called when a table item is double-clicked
 * @param row
 * Row clicked
 * @param column
 * Column clicked
 */
void
RecentFilesTableWidget::tableCellDoubleClicked(int row, int column)
{
    tableCellClicked(row, column);
    RecentFileItem* selectedItem(getSelectedItem());
    if (selectedItem != NULL) {
        const COLUMNS tableColumn = static_cast<COLUMNS>(column);

        bool emitFlag(false);
        switch (tableColumn) {
            case COLUMN_DATE_TIME:
                emitFlag = true;
                break;
            case COLUMN_FAVORITE:
                break;
            case COLUMN_FORGET:
                break;
            case COLUMN_NAME:
                emitFlag = true;
                break;
            case COLUMN_SHARE:
                break;
            case COLUMN_COUNT:
                break;
        }
        
        if (emitFlag) {
            emit selectedItemDoubleClicked(selectedItem);
        }
    }
}

/**
 * @return Pointer to icon read from the given file name (NULL) if failure to load icon file
 * @param iconFileName
 *  Name of file containing icon
 */
std::unique_ptr<QIcon>
RecentFilesTableWidget::loadIcon(const AString& iconFileName) const
{
    std::unique_ptr<QIcon> iconOut;

    QIcon icon;
    if (WuQtUtilities::loadIcon(iconFileName,
                                icon)) {
        iconOut.reset(new QIcon(icon));
    }
    
    return iconOut;
}

/*
 * Called when a column header sort indicator is clicked by user
 * @param logicalIndex
 *   Index of clicked header
 * @param sortOrder
 *   Order for sorting
 */
void
RecentFilesTableWidget::sortIndicatorClicked(int logicalIndex,
                                             Qt::SortOrder sortOrder)
{
    if (m_recentFileItemsContainer != NULL) {
        bool updateFlag(false);
        const COLUMNS column = static_cast<COLUMNS>(logicalIndex);
        switch (column) {
            case COLUMN_NAME:
                switch (sortOrder) {
                    case Qt::AscendingOrder:
                        m_sortingKey = RecentFileItemSortingKeyEnum::NAME_ASCENDING;
                        break;
                    case Qt::DescendingOrder:
                        m_sortingKey = RecentFileItemSortingKeyEnum::NAME_DESCENDING;
                        break;
                }
                updateFlag = true;
                break;
            case COLUMN_DATE_TIME:
                switch (sortOrder) {
                    case Qt::AscendingOrder:
                        m_sortingKey = RecentFileItemSortingKeyEnum::DATE_ASCENDING;
                        break;
                    case Qt::DescendingOrder:
                        m_sortingKey = RecentFileItemSortingKeyEnum::DATE_DESCENDING;
                        break;
                }
                updateFlag = true;
                break;
            case COLUMN_COUNT:
            case COLUMN_FAVORITE:
            case COLUMN_FORGET:
            case COLUMN_SHARE:
                break;
        }
        
        if (updateFlag) {
            clearSelectedItem();
            updateContent(m_recentFileItemsContainer,
                          m_recentFileItemsFilter);
        }
    }
}

/**
 * Sort the items
 */
void
RecentFilesTableWidget::sortRecentItems()
{
    QHeaderView* horizHeader = horizontalHeader();
    CaretAssert(horizHeader);
    QSignalBlocker headerBlocker(horizHeader);
    
    RecentFileItemsContainer::sort(m_sortingKey,
                                   m_recentItems);
}

/**
 * Updates the table header's sorting key (up or down arrow indicating column sorted
 */
void
RecentFilesTableWidget::updateHeaderSortingKey()
{
    QHeaderView* horizHeader = horizontalHeader();
    horizHeader->setSortIndicatorShown(true);
    switch (m_sortingKey) {
        case RecentFileItemSortingKeyEnum::DATE_ASCENDING:
            horizHeader->setSortIndicator(COLUMN_DATE_TIME, Qt::AscendingOrder);
            break;
        case RecentFileItemSortingKeyEnum::DATE_DESCENDING:
            horizHeader->setSortIndicator(COLUMN_DATE_TIME, Qt::DescendingOrder);
            break;
        case RecentFileItemSortingKeyEnum::NAME_ASCENDING:
            horizHeader->setSortIndicator(COLUMN_NAME, Qt::AscendingOrder);
            break;
        case RecentFileItemSortingKeyEnum::NAME_DESCENDING:
            horizHeader->setSortIndicator(COLUMN_NAME, Qt::DescendingOrder);
            break;
    }
}

/**
 * Show the share menu for the give row index
 * @param rowIndex
 * Index of row
 */
void
RecentFilesTableWidget::showShareMenuForRow(const int32_t rowIndex)
{
    CaretAssertVectorIndex(m_recentItems, rowIndex);
    
    QMenu menu(this);
    QAction* copyToClipboardAction =  menu.addAction("Copy Pathname to Clipboard");
    QAction* shareViaEmailAction   = menu.addAction("Email Pathname...");
    
    const AString pathName(m_recentItems[rowIndex]->getPathAndFileName());
    
    QPoint globalPos = QCursor::pos();
    QAction* selectedAction = menu.exec(globalPos);
    if (selectedAction == copyToClipboardAction) {
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(pathName);
    }
    else if (selectedAction == shareViaEmailAction) {
        AString subject;
        switch (m_recentItems[rowIndex]->getFileItemType()) {
            case RecentFileItemTypeEnum::DIRECTORY:
                subject = "Directory";
                break;
            case RecentFileItemTypeEnum::SCENE_FILE:
                subject = "Scene File";
                break;
            case RecentFileItemTypeEnum::SPEC_FILE:
                subject = "Spec File";
                break;
        }
        const QString mailArg("mailto:?&subject=" + subject
                              + "&body=" + pathName);
        QDesktopServices::openUrl(QUrl(mailArg,
                                       QUrl::TolerantMode));
    }
}

