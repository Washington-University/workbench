
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
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "Vector3D.h"

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


