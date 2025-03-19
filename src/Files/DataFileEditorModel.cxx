
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __DATA_FILE_EDITOR_MODEL_DECLARE__
#include "DataFileEditorModel.h"
#undef __DATA_FILE_EDITOR_MODEL_DECLARE__

#include <iostream>

#include "CaretAssert.h"
#include "DataFileEditorItem.h"
#include "DataFileEditorColumnContent.h"

using namespace caret;
    
/**
 * \class caret::DataFileEditorModel 
 * \brief Model for DataFileEditorItem's
 * \ingroup Files
 */

/**
 * Constructor.
 */
DataFileEditorModel::DataFileEditorModel()
: QStandardItemModel()
{
}

/**
 * Destructor.
 */
DataFileEditorModel::~DataFileEditorModel()
{
}

/**
 * @return DataFileEditorItems associated wtih the given model indices
 */
std::vector<DataFileEditorItem*>
DataFileEditorModel::getItemsFromIndices(const QModelIndexList& indexesList) const
{
    std::vector<DataFileEditorItem*> itemsSelected;

    const int32_t numIndices(indexesList.length());
    for (int32_t i = 0 ; i < numIndices; i++) {
        const QModelIndex modelIndex(indexesList.at(i));
        QStandardItem* standardItem(itemFromIndex(modelIndex));
        if (standardItem != NULL) {
            DataFileEditorItem* item(dynamic_cast<DataFileEditorItem*>(standardItem));
            if (item != NULL) {
                itemsSelected.push_back(item);
            }
        }
    }

    return itemsSelected;
}

/**
 * @return DataFileEditorItem at the given row and column index (NULL if not valid)
 * @param rowIndex
 *    Index of row
 * @param columnIndex
 *    Index of column
 */
DataFileEditorItem* 
DataFileEditorModel::getDataFileItemAtRowColumn(const int32_t rowIndex,
                                                const int32_t columnIndex) const
{
    DataFileEditorItem* itemOut(NULL);
    
    if ((rowIndex >= 0)
        && (rowIndex < rowCount())
        && (columnIndex >= 0)
        && (columnIndex < columnCount())) {
        QStandardItem* standardItem(item(rowIndex, columnIndex));
        if (standardItem != NULL) {
            itemOut = dynamic_cast<DataFileEditorItem*>(standardItem);
        }
    }
    
    return itemOut;
}

/**
 * @return Column index for sorting when loaded into GUI
 */
int32_t
DataFileEditorModel::getDefaultSortingColumnIndex() const
{
    return m_defaultSortingColumnIndex;
}

/**
 * Set column index for sorting when loaded into GUI
 * @param columnIndex
 *    The column index
 */
void
DataFileEditorModel::setDefaultSortingColumnIndex(const int32_t columnIndex)
{
    m_defaultSortingColumnIndex = columnIndex;
}

/**
 * Set the number of columns and the column titles from the given model content
 */
void
DataFileEditorModel::setNumberOfColumnsAndColumnTitles(const DataFileEditorColumnContent& modelContent)
{
    const int32_t numColumns(modelContent.getNumberOfColumns());
    if (numColumns < 0) {
        return;
    }
    QList<QString> columnTitles;
    int32_t sortingColumnIndex(-1);
    setColumnCount(numColumns);
    for (int32_t iCol = 0; iCol < numColumns; iCol++) {
        columnTitles << modelContent.getColumnName(iCol);
        if (modelContent.getColumnDataType(iCol) == modelContent.getDefaultSortingColumnDataType()) {
            sortingColumnIndex = iCol;
        }
    }
    
    setHorizontalHeaderLabels(columnTitles);

    if (sortingColumnIndex >= 0) {
        setDefaultSortingColumnIndex(sortingColumnIndex);
    }
}

/**
 * @return The model index of an item matching the given texts
 * @param firstText
 *    First text to look for
 * @param secondText
 *    Look for this if first text not found
 */
QModelIndex
DataFileEditorModel::findItemsWithText(const AString& firstText,
                                       const AString& secondText) const
{
    QModelIndex mi;
    
    QList<QStandardItem*> items;
    if ( ! firstText.isEmpty()) {
        items = findItems(firstText);
    }
    if (items.empty()) {
        if ( ! secondText.isEmpty()) {
            items = findItems(secondText);
        }
    }
    if ( ! items.empty()) {
        mi = items.at(0)->index();
    }
    
    return mi;
}

