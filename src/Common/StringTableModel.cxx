
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __STRING_TABLE_MODEL_DECLARE__
#include "StringTableModel.h"
#undef __STRING_TABLE_MODEL_DECLARE__

#include "CaretAssert.h"
using namespace caret;



/**
 * \class caret::StringTableModel
 * \brief Creates a two-dimension table of strings.
 * \ingroup Common
 */

/**
 * Constructor that creates a table of the specified dimensions
 * with all column alignment set to right.
 *
 * @param numberOfRows
 *    Number of rows.
 * @param numColumns
 *    Number of columns.
 * @param floatingPointPrecision
 *    Precision for floating point data.
 */
StringTableModel::StringTableModel(const int32_t numberOfRows,
                                   const int32_t numberOfColumns,
                                   const int32_t floatingPointPrecision)
: CaretObject(),
m_numberOfRows(numberOfRows),
m_numberOfColumns(numberOfColumns),
m_floatingPointPrecsion(floatingPointPrecision)
{
    CaretAssert(m_numberOfRows > 0);
    CaretAssert(m_numberOfColumns > 0);
    
    m_stringTable.resize(m_numberOfRows
                         * m_numberOfColumns);
    m_columnAlignment.resize(m_numberOfColumns,
                             ALIGN_RIGHT);
}

/**
 * Destructor.
 */
StringTableModel::~StringTableModel()
{
}

/**
 * @return Number of rows in table
 */
int32_t
StringTableModel::getNumberOfRows() const
{
    return m_numberOfRows;
}

/**
 * @return Number of columns in table
 */
int32_t
StringTableModel::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

/**
 * Set column alignment.
 *
 * @param column
 *    Column for which alignment is set.
 * @param alignment
 *    New alignment value.
 */
void
StringTableModel::setColumnAlignment(const int32_t column,
                                     const Alignment alignment)
{
    CaretAssertVectorIndex(m_columnAlignment, column);
    m_columnAlignment[column] = alignment;
}

/**
 * Set an element in the table with a string.
 *
 * @param row
 *    Row index.
 * @param column
 *    Column.
 * @param value
 *    String value for given row/column.
 */
void
StringTableModel::setElement(const int32_t row,
                             const int32_t column,
                             const AString& value)
{
    const int32_t offset = getOffset(row,
                                     column);
    
    CaretAssertVectorIndex(m_stringTable, offset);
    m_stringTable[offset] = value;
}

/**
 * Set an element in the table with a C-style string.
 *
 * @param row
 *    Row index.
 * @param column
 *    Column.
 * @param value
 *    C-Style string value for given row/column.
 */
void
StringTableModel::setElement(const int32_t row,
                             const int32_t column,
                             const char* value)
{
    setElement(row,
               column,
               AString(value));
}

/**
 * Set an element in the table with a boolean value.
 *
 * @param row
 *    Row index.
 * @param column
 *    Column.
 * @param value
 *    Boolean value for given row/column.
 */
void
StringTableModel::setElement(const int32_t row,
                             const int32_t column,
                             const bool value)
{
    setElement(row,
               column,
               AString::fromBool(value));
}

/**
 * Set an element in the table with a 32-bit integer.
 *
 * @param row
 *    Row index.
 * @param column
 *    Column.
 * @param value
 *    Integer value for given row/column.
 */
void
StringTableModel::setElement(const int32_t row,
                             const int32_t column,
                             const int32_t value)
{
    setElement(row,
               column,
               AString::number(value));
}

/**
 * Set an element in the table with a 64-bit integer.
 *
 * @param row
 *    Row index.
 * @param column
 *    Column.
 * @param value
 *    64-Bit integer value for given row/column.
 */
void
StringTableModel::setElement(const int32_t row,
                             const int32_t column,
                             const int64_t value)
{
    setElement(row,
               column,
               AString::number(value));
}

/**
 * Set an element in the table with floating point value.
 *
 * @param row
 *    Row index.
 * @param column
 *    Column.
 * @param value
 *    Floating point value for given row/column.
 */
void
StringTableModel::setElement(const int32_t row,
                             const int32_t column,
                             const double value)
{
    setElement(row,
               column,
               AString::number(value,
                               'f',
                               m_floatingPointPrecsion));
}

/**
 * Get the offset to an element in the table.
 *
 * @param row
 *    Row index.
 * @param column
 *    Column.
 * @return
 *    Offset of element and given row and column.
 */
int32_t
StringTableModel::getOffset(const int32_t row,
                            const int32_t column) const
{
    CaretAssertArrayIndex(m_stringTable, m_numberOfRows, row);
    CaretAssertArrayIndex(m_stringTable, m_numberOfColumns, column);
    
    const int32_t offset = ((row * m_numberOfColumns)
                            + column);
    return offset;
}

/**
 * @return The table formatted into a text string.
 */
AString StringTableModel::getInString() const
{
    std::vector<int32_t> columnWidths(m_numberOfColumns,
                                      0);
    
    for (int32_t j = 0; j < m_numberOfColumns; j++) {
        int32_t width = 0;
        
        for (int32_t i = 0; i < m_numberOfRows; i++) {
            width = std::max(static_cast<int32_t>(m_stringTable[getOffset(i, j)].length()),
                             width);
        }
//        if (width > 0) {
//            width += 3;
//        }
        
        columnWidths[j] = width;
    }
    
    AString textOut;
    
    for (int32_t i = 0; i < m_numberOfRows; i++) {
        for (int32_t j = 0; j < m_numberOfColumns; j++) {
            const int32_t offset = getOffset(i, j);
            CaretAssertVectorIndex(m_stringTable, offset);
            AString txt = m_stringTable[offset];
            
            CaretAssertVectorIndex(m_columnAlignment, j);
            switch (m_columnAlignment[j]) {
                case ALIGN_LEFT:
                    textOut.append(txt.leftJustified(columnWidths[j]));
                    break;
                case ALIGN_RIGHT:
                    textOut.append(txt.rightJustified(columnWidths[j]));
                    break;
            }
            textOut.append("   ");
        }
        textOut.append("\n");
    }
    
    return textOut;
}

