
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

#define __DATA_FILE_EDITOR_COLUMN_CONTENT_DECLARE__
#include "DataFileEditorColumnContent.h"
#undef __DATA_FILE_EDITOR_COLUMN_CONTENT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DataFileEditorColumnContent 
 * \brief Describes content of columns a DataFileEditorModel
 * \ingroup Files
 */

/**
 * Constructor.
 */
DataFileEditorColumnContent::DataFileEditorColumnContent()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
DataFileEditorColumnContent::~DataFileEditorColumnContent()
{
}

/**
 * Add a column to the model
 * @param columnDataType
 *    Type of data in the column
 * @param columnName
 *    Name for column
 */
void
DataFileEditorColumnContent::addColumn(const DataFileEditorItemTypeEnum::Enum columnDataType,
                                      const QString& columnName) {
    m_columnInfo.push_back(ColumnInfo(columnDataType,
                                      columnName));
}

/**
 * Set the data type of the column for default sorting
 * @param columnDataType
 *    Type of data in the column
 */
void
DataFileEditorColumnContent::setDefaultSortingColumnDataType(const DataFileEditorItemTypeEnum::Enum columnDataType)
{
    m_defaultSortingColumnDataType = columnDataType;
}

/**
 * @return Column ata type for default sorting
 */
DataFileEditorItemTypeEnum::Enum
DataFileEditorColumnContent::getDefaultSortingColumnDataType() const
{
    return m_defaultSortingColumnDataType;
}

/**
 * @return Number of columns in the model
 */
int32_t 
DataFileEditorColumnContent::getNumberOfColumns() const
{
    return m_columnInfo.size();
}

/**
 * @return Data type in the given column
 * @param columnIndex
 *    Index of the column
 */
DataFileEditorItemTypeEnum::Enum
DataFileEditorColumnContent::getColumnDataType(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnInfo, columnIndex);
    return m_columnInfo[columnIndex].m_dataType;
}

/**
 * @return Name of the given column
 * @param columnIndex
 *    Index of the column
 */
QString 
DataFileEditorColumnContent::getColumnName(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnInfo, columnIndex);
    return m_columnInfo[columnIndex].m_name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DataFileEditorColumnContent::toString() const
{
    return "DataFileEditorColumnContent";
}

