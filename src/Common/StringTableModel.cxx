
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
m_floatingPointPrecsion(floatingPointPrecision),
m_stringTable(NULL)
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
            width = std::max(m_stringTable[getOffset(i, j)].length(),
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

