
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

#define __WU_Q_TABLE_WIDGET_MODEL_DECLARE__
#include "WuQTableWidgetModel.h"
#undef __WU_Q_TABLE_WIDGET_MODEL_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQTableWidgetModel 
 * \brief Model for displaying data in a WuQTableWidget
 * \ingroup Common
 */

/**
 * Constructor.
 *
 * @param columnContentInfo
 *    Describes contents of each column.  This model will take ownership of
 *    the items and delete them when this table model is deleted.
 * @param numberOfRows
 *    Number of rows for the table.
 */
WuQTableWidgetModel::WuQTableWidgetModel(const std::vector<WuQTableWidgetModelColumnContent*>& columnContentInfo,
                                         const int32_t numberOfRows)
: QObject()
{
    m_columnContentInfo = columnContentInfo;
    
    m_numberOfRows = numberOfRows;
    m_numberOfColumns = static_cast<int32_t>(m_columnContentInfo.size());
    
    for (int32_t iCol = 0; iCol < m_numberOfColumns; iCol++) {
        columnContentInfo[iCol]->setNumberOfRows(numberOfRows);
    }
}

/**
 * Destructor.
 */
WuQTableWidgetModel::~WuQTableWidgetModel()
{
    for (std::vector<WuQTableWidgetModelColumnContent*>::iterator iter = m_columnContentInfo.begin();
         iter != m_columnContentInfo.end();
         iter++) {
        delete *iter;
    }
    m_columnContentInfo.clear();
}

/**
 * @return Number of rows in table.
 */
int32_t
WuQTableWidgetModel::getNumberOfRows() const
{
    return m_numberOfRows;
}

/**
 * @return Number of columns in table.
 */
int32_t
WuQTableWidgetModel::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

/**
 * Get the content information for a column in the table.
 *
 * @param columnIndex
 *    Index of column for which column information is requested.
 */
const WuQTableWidgetModelColumnContent*
WuQTableWidgetModel::getColumnContentInfo(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    return m_columnContentInfo[columnIndex];
}

bool
WuQTableWidgetModel::isChecked(const int32_t rowIndex,
               const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnCheckBox*>(m_columnContentInfo[columnIndex]) != NULL);
    const WuQTableWidgetModelColumnCheckBox* checkBoxColumn = static_cast<const WuQTableWidgetModelColumnCheckBox*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(checkBoxColumn->m_selectionStatus, rowIndex);
    
    return checkBoxColumn->m_selectionStatus[rowIndex];
}

double
WuQTableWidgetModel::getDouble(const int32_t rowIndex,
                 const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnDoubleSpinBox*>(m_columnContentInfo[columnIndex]) != NULL);
    const WuQTableWidgetModelColumnDoubleSpinBox* doubleColumn = static_cast<const WuQTableWidgetModelColumnDoubleSpinBox*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(doubleColumn->m_values, rowIndex);
    
    return doubleColumn->m_values[rowIndex];
}

int32_t
WuQTableWidgetModel::getInteger(const int32_t rowIndex,
                   const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnIntegerSpinBox*>(m_columnContentInfo[columnIndex]) != NULL);
    const WuQTableWidgetModelColumnIntegerSpinBox* integerColumn = static_cast<const WuQTableWidgetModelColumnIntegerSpinBox*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(integerColumn->m_values, rowIndex);
    
    return integerColumn->m_values[rowIndex];
}

AString
WuQTableWidgetModel::getText(const int32_t rowIndex,
                  const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnText*>(m_columnContentInfo[columnIndex]) != NULL);
    const WuQTableWidgetModelColumnText* textColumn = static_cast<const WuQTableWidgetModelColumnText*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(textColumn->m_values, rowIndex);
    
    return textColumn->m_values[rowIndex];
}

/**
 * Get the icon for the column.  Note that the icon is the same in all rows.
 *
 * @param rowIndex
 *     Row for the icon.
 * @param columnIndex
 *     Column for the icon.
 * @return
 *     The icon for the column.
 */
const QIcon*
WuQTableWidgetModel::getIcon(const int32_t /*rowIndex*/,
                             const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnText*>(m_columnContentInfo[columnIndex]) != NULL);
    const WuQTableWidgetModelColumnIcon* iconColumn = static_cast<const WuQTableWidgetModelColumnIcon*>(m_columnContentInfo[columnIndex]);
    
    return iconColumn->getIcon();
}

void
WuQTableWidgetModel::getColorSwatch(const int32_t rowIndex,
                    const int32_t columnIndex,
                    float rgbaOut[4]) const
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnColorSwatch*>(m_columnContentInfo[columnIndex]) != NULL);
    const WuQTableWidgetModelColumnColorSwatch* colorColumn = static_cast<const WuQTableWidgetModelColumnColorSwatch*>(m_columnContentInfo[columnIndex]);
    const int32_t rgbaIndex = rowIndex * 4;
    CaretAssertVectorIndex(colorColumn->m_rgba, rowIndex+3);
    
    rgbaOut[0] = colorColumn->m_rgba[rgbaIndex];
    rgbaOut[1] = colorColumn->m_rgba[rgbaIndex+1];
    rgbaOut[2] = colorColumn->m_rgba[rgbaIndex+2];
    rgbaOut[3] = colorColumn->m_rgba[rgbaIndex+3];
}

void
WuQTableWidgetModel::setColorSwatch(const int32_t rowIndex,
                    const int32_t columnIndex,
                    const float rgba[4])
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<WuQTableWidgetModelColumnColorSwatch*>(m_columnContentInfo[columnIndex]) != NULL);
    WuQTableWidgetModelColumnColorSwatch* colorColumn = static_cast<WuQTableWidgetModelColumnColorSwatch*>(m_columnContentInfo[columnIndex]);
    const int32_t rgbaIndex = rowIndex * 4;
    CaretAssertVectorIndex(colorColumn->m_rgba, rowIndex+3);
    
    colorColumn->m_rgba[rgbaIndex]   = rgba[0];
    colorColumn->m_rgba[rgbaIndex+1] = rgba[1];
    colorColumn->m_rgba[rgbaIndex+2] = rgba[2];
    colorColumn->m_rgba[rgbaIndex+3] = rgba[3];
}


void
WuQTableWidgetModel::setChecked(const int32_t rowIndex,
               const int32_t columnIndex,
                const bool checkBoxSelected)
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnCheckBox*>(m_columnContentInfo[columnIndex]) != NULL);
    WuQTableWidgetModelColumnCheckBox* checkBoxColumn = static_cast<WuQTableWidgetModelColumnCheckBox*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(checkBoxColumn->m_selectionStatus, rowIndex);
    
    checkBoxColumn->m_selectionStatus[rowIndex] = checkBoxSelected;
}

void
WuQTableWidgetModel::setDouble(const int32_t rowIndex,
                 const int32_t columnIndex,
                 const double doubleValue)
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnDoubleSpinBox*>(m_columnContentInfo[columnIndex]) != NULL);
    WuQTableWidgetModelColumnDoubleSpinBox* doubleColumn = static_cast<WuQTableWidgetModelColumnDoubleSpinBox*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(doubleColumn->m_values, rowIndex);
    
    doubleColumn->m_values[rowIndex] = doubleValue;
}


void
WuQTableWidgetModel::setInteger(const int32_t rowIndex,
                const int32_t columnIndex,
                const int32_t integerValue)
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnIntegerSpinBox*>(m_columnContentInfo[columnIndex]) != NULL);
    WuQTableWidgetModelColumnIntegerSpinBox* integerColumn = static_cast<WuQTableWidgetModelColumnIntegerSpinBox*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(integerColumn->m_values, rowIndex);
    
    integerColumn->m_values[rowIndex] = integerValue;
}

void
WuQTableWidgetModel::setText(const int32_t rowIndex,
               const int32_t columnIndex,
               const AString& textValue)
{
    CaretAssertVectorIndex(m_columnContentInfo, columnIndex);
    
    CaretAssert(dynamic_cast<const WuQTableWidgetModelColumnText*>(m_columnContentInfo[columnIndex]) != NULL);
    WuQTableWidgetModelColumnText* textColumn = static_cast<WuQTableWidgetModelColumnText*>(m_columnContentInfo[columnIndex]);
    CaretAssertVectorIndex(textColumn->m_values, rowIndex);
    
    textColumn->m_values[rowIndex] = textValue;
    
}



/* ================================================================== */

/**
 * \class caret::WuQTableWidgetModelColumn
 * \brief Abstract class for column content in a WuQTableWidgetModel
 * \ingroup Common
 */

/**
 * Base class for column content information.
 *
 * @param columnTitle
 *    Title for the column.
 * @param columnAlignment
 *    Alignment for the column.
 * @param columnDataType
 *    Data type for the column.
 */
WuQTableWidgetModelColumnContent::WuQTableWidgetModelColumnContent(const AString& columnTitle,
                                                                   const ColumnAlignment columnAlignment,
                                                             const ColumnDataType columnDataType)
: m_columnTitle(columnTitle),
m_columnAlignment(columnAlignment),
m_columnDataType(columnDataType)
{
}


/**
 * Destructor.
 */
WuQTableWidgetModelColumnContent::~WuQTableWidgetModelColumnContent()
{
    
}

/**
 * @return Title for the column.
 */
AString
WuQTableWidgetModelColumnContent::getColumnTitle() const
{
    return m_columnTitle;
}

/**
 * @return Alignment for the column.
 */
WuQTableWidgetModelColumnContent::ColumnAlignment
WuQTableWidgetModelColumnContent::getColumnAlignment() const
{
    return m_columnAlignment;
}

/**
 * @return Data type for the column.
 */
WuQTableWidgetModelColumnContent::ColumnDataType
WuQTableWidgetModelColumnContent::getColumnDataType() const
{
    return m_columnDataType;
}


/* ================================================================== */

/**
 * \class caret::WuQTableWidgetModelColumnCheckBox
 * \brief Column contains a check box.
 * \ingroup Common
 */

/**
 * Column containing a check box.
 *
 * @param columnAlignment
 *    Alignment for the column.
 */
WuQTableWidgetModelColumnCheckBox::WuQTableWidgetModelColumnCheckBox(const AString& columnTitle,
                                                                     const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment)
: WuQTableWidgetModelColumnContent(columnTitle,
                                   columnAlignment,
                            COLUMN_DATA_CHECK_BOX){
    
}
    
/**
 * Destructor.
 */
WuQTableWidgetModelColumnCheckBox::~WuQTableWidgetModelColumnCheckBox()
{
    
}

/**
 * Set the number of rows in the column.  Any new elements added
 * receive the default value for the data type.
 * 
 * @param numberOfRows
 *    Number of rows for column.
 */
void
WuQTableWidgetModelColumnCheckBox::setNumberOfRows(const int32_t numberOfRows)
{
    m_selectionStatus.resize(numberOfRows,
                             false);
}

/* ================================================================== */

/**
 * \class caret::WuQTableWidgetModelColumnColorSwatch
 * \brief Column contains a color swatch.
 * \ingroup Common
 */

/**
 * Column containing a color swatch.
 *
 * @param columnAlignment
 *    Alignment for the column.
 */
WuQTableWidgetModelColumnColorSwatch::WuQTableWidgetModelColumnColorSwatch(const AString& columnTitle,
                                                                           const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment)
: WuQTableWidgetModelColumnContent(columnTitle,
                                   columnAlignment,
                            COLUMN_DATA_COLOR_SWATCH){
    
}

/**
 * Destructor.
 */
WuQTableWidgetModelColumnColorSwatch::~WuQTableWidgetModelColumnColorSwatch()
{
    
}

/**
 * Set the number of rows in the column.  Any new elements added
 * receive the default value for the data type.
 *
 * @param numberOfRows
 *    Number of rows for column.
 */
void
WuQTableWidgetModelColumnColorSwatch::setNumberOfRows(const int32_t numberOfRows)
{
    const int32_t oldCount = m_rgba.size() / 4;
    
    m_rgba.resize(numberOfRows * 4);
    
    const int32_t newCount = m_rgba.size() / 4;
    
    for (int32_t i = oldCount; i < newCount; i++) {
        const int32_t i4 = i * 4;
        m_rgba[i4]   = 0.0;
        m_rgba[i4+1] = 0.0;
        m_rgba[i4+2] = 0.0;
        m_rgba[i4+3] = 1.0;
    }
}

/* ================================================================== */

/**
 * \class caret::WuQTableWidgetModelColumnDoubleSpinBox
 * \brief Column contains a double spin box.
 * \ingroup Common
 */

/**
 * Column containing a double spin box.
 *
 * @param columnAlignment
 *    Alignment for the column.
 * @param minimumValue
 *    Minimum value allowed.
 * @param maximumValue
 *    Maximum value allowed.
 * @param stepSize
 *    Step size of spin box.
 * @param digitsRightOfDecimalPoint
 *    Digits displayed right of the decimal point.
 */
WuQTableWidgetModelColumnDoubleSpinBox::WuQTableWidgetModelColumnDoubleSpinBox(const AString& columnTitle,
                                                                               const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                                                               const double minimumValue,
                                                                               const double maximumValue,
                                                                               const double stepSize,
                                                                               const int32_t digitsRightOfDecimalPoint)
: WuQTableWidgetModelColumnContent(columnTitle,
                                   columnAlignment,
                            COLUMN_DATA_DOUBLE_SPIN_BOX),
m_minimumValue(minimumValue),
m_maximumValue(maximumValue),
m_stepSize(stepSize),
m_digitsRightOfDecimalPoint(digitsRightOfDecimalPoint)
{
    
}

/**
 * Destructor.
 */
WuQTableWidgetModelColumnDoubleSpinBox::~WuQTableWidgetModelColumnDoubleSpinBox()
{
    
}

/**
 * @return minimum value allowed.
 */
double
WuQTableWidgetModelColumnDoubleSpinBox::getMinimumValue() const
{
    return m_minimumValue;
}

/**
 * @return maximum value allowed.
 */
double
WuQTableWidgetModelColumnDoubleSpinBox::getMaximumValue() const
{
    return m_maximumValue;
}

/**
 * @return Step size value.
 */
double
WuQTableWidgetModelColumnDoubleSpinBox::getStepSize() const
{
    return m_stepSize;
}

/**
 * @return Number of digits displayed right of the decimal point.
 */
double
WuQTableWidgetModelColumnDoubleSpinBox::getDigitsRightOfDecimalPoint() const
{
    return m_digitsRightOfDecimalPoint;
}

/**
 * Set the number of rows in the column.  Any new elements added
 * receive the default value for the data type.
 *
 * @param numberOfRows
 *    Number of rows for column.
 */
void
WuQTableWidgetModelColumnDoubleSpinBox::setNumberOfRows(const int32_t numberOfRows)
{
    m_values.resize(numberOfRows,
                    0.0);
}

/* ================================================================== */

/**
 * \class caret::WuQTableWidgetModelColumnIcon
 * \brief Column contains an icon.
 * \ingroup Common
 */

/**
 * Column containing a color swatch.
 *
 * @param columnAlignment
 *    Alignment for the column.
 * @param icon
 *    Pointer to the icon that is displayed in ALL ROWS for this column.
 *    This icon WILL NOT be deleted by this instance and the caller is 
 *    responsible for deleting the icon when it is no longer needed.
 */
WuQTableWidgetModelColumnIcon::WuQTableWidgetModelColumnIcon(const AString& columnTitle,
                                                             const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                                             QIcon* icon)
: WuQTableWidgetModelColumnContent(columnTitle,
                                   columnAlignment,
                            COLUMN_DATA_ICON),
m_icon(icon)
{
    
}

/**
 * Destructor.
 */
WuQTableWidgetModelColumnIcon::~WuQTableWidgetModelColumnIcon()
{
    
}

/**
 * Set the number of rows in the column.  Any new elements added
 * receive the default value for the data type.
 *
 * @param numberOfRows
 *    Number of rows for column.
 */
void
WuQTableWidgetModelColumnIcon::setNumberOfRows(const int32_t /*numberOfRows */)
{
}

/**
 * @return The icon for the column.  Note: the icon may be NULL as it must
 * bet set by the GUI since QIcon is part of Qt's GUI module.
 */
const QIcon*
WuQTableWidgetModelColumnIcon::getIcon() const
{
    return m_icon;
}

/**
 * Set the Icon for the column.  Note: QIcon is part of Qt's GUI module
 * and so the icon is referenced by a pointer.  The caller is responsible
 * for managing the destruction of the icon.
 *
 * @param icon
 *     Icon for display in the column.
 */
void
WuQTableWidgetModelColumnIcon::setIcon(const QIcon* icon)
{
    
}

/* ================================================================== */

/**
 * \class caret::WuQTableWidgetModelColumnDoubleSpinBox
 * \brief Column contains a double spin box.
 * \ingroup Common
 */

/**
 * Column containing a double spin box.
 *
 * @param columnAlignment
 *    Alignment for the column.
 * @param minimumValue
 *    Minimum value allowed.
 * @param maximumValue
 *    Maximum value allowed.
 * @param stepSize
 *    Step size of spin box.
 * @param digitsRightOfDecimalPoint
 *    Digits displayed right of the decimal point.
 */
WuQTableWidgetModelColumnIntegerSpinBox::WuQTableWidgetModelColumnIntegerSpinBox(const AString& columnTitle,
                                                                                 const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                                                               const int32_t minimumValue,
                                                                               const int32_t maximumValue,
                                                                               const int32_t stepSize)
: WuQTableWidgetModelColumnContent(columnTitle,
                                   columnAlignment,
                            COLUMN_DATA_DOUBLE_SPIN_BOX),
m_minimumValue(minimumValue),
m_maximumValue(maximumValue),
m_stepSize(stepSize)
{
    
}

/**
 * Destructor.
 */
WuQTableWidgetModelColumnIntegerSpinBox::~WuQTableWidgetModelColumnIntegerSpinBox()
{
    
}

/**
 * @return minimum value allowed.
 */
int32_t
WuQTableWidgetModelColumnIntegerSpinBox::getMinimumValue() const
{
    return m_minimumValue;
}

/**
 * @return maximum value allowed.
 */
int32_t
WuQTableWidgetModelColumnIntegerSpinBox::getMaximumValue() const
{
    return m_maximumValue;
}

/**
 * @return Step size value.
 */
int32_t
WuQTableWidgetModelColumnIntegerSpinBox::getStepSize() const
{
    return m_stepSize;
}

/**
 * Set the number of rows in the column.  Any new elements added
 * receive the default value for the data type.
 *
 * @param numberOfRows
 *    Number of rows for column.
 */
void
WuQTableWidgetModelColumnIntegerSpinBox::setNumberOfRows(const int32_t numberOfRows)
{
    m_values.resize(numberOfRows,
                    0);
}


/* ================================================================== */

/**
 * \class caret::WuQTableWidgetModelColumnText
 * \brief Column contains text
 * \ingroup Common
 */

/**
 * Column containing text.
 *
 * @param columnTitle
 *    Title for column
 * @param columnAlignment
 *    Alignment for the column.
 * @param textEditable
 *    Editable status of text.
 */
WuQTableWidgetModelColumnText::WuQTableWidgetModelColumnText(const AString& columnTitle,
                                                             const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                                             const TextEditable textEditable)
: WuQTableWidgetModelColumnContent(columnTitle,
                                   columnAlignment,
                            COLUMN_DATA_TEXT),
m_textEditable(textEditable)
{
    
}

/**
 * Destructor.
 */
WuQTableWidgetModelColumnText::~WuQTableWidgetModelColumnText()
{
    
}

WuQTableWidgetModelColumnText::TextEditable
WuQTableWidgetModelColumnText::getTextEditable() const
{
    return m_textEditable;
}


/**
 * Set the number of rows in the column.  Any new elements added
 * receive the default value for the data type.
 *
 * @param numberOfRows
 *    Number of rows for column.
 */
void
WuQTableWidgetModelColumnText::setNumberOfRows(const int32_t numberOfRows)
{
    m_values.resize(numberOfRows,
                    "");
}



