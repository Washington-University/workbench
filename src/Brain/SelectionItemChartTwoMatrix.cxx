
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __SELECTION_ITEM_CHART_TWO_MATRIX_DECLARE__
#include "SelectionItemChartTwoMatrix.h"
#undef __SELECTION_ITEM_CHART_TWO_MATRIX_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTwoMatrix 
 * \brief Selection of chart two matrix
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTwoMatrix::SelectionItemChartTwoMatrix()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TWO_MATRIX)
{
    m_fileMatrixChart = NULL;
    m_rowIndex = -1;
    m_columnIndex = -1;
}

/**
 * Destructor.
 */
SelectionItemChartTwoMatrix::~SelectionItemChartTwoMatrix()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTwoMatrix::SelectionItemChartTwoMatrix(const SelectionItemChartTwoMatrix& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTwoMatrix(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTwoMatrix&
SelectionItemChartTwoMatrix::operator=(const SelectionItemChartTwoMatrix& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTwoMatrix(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTwoMatrix::copyHelperSelectionItemChartTwoMatrix(const SelectionItemChartTwoMatrix& obj)
{
    m_fileMatrixChart = obj.m_fileMatrixChart;
    m_rowIndex        = obj.m_rowIndex;
    m_columnIndex     = obj.m_columnIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartTwoMatrix::isValid() const
{
    if ((m_fileMatrixChart != NULL)
        && (m_rowIndex >= 0)
        && (m_columnIndex >= 0)) {
        return true;
    }
    
    return false;
}

/**
 * Reset the selections.
 */
void
SelectionItemChartTwoMatrix::reset()
{
    m_fileMatrixChart = NULL;
    m_rowIndex = -1;
    m_columnIndex = -1;
}

/**
 * @return Matrix chart that was selected.
 */
ChartableTwoFileMatrixChart*
SelectionItemChartTwoMatrix::getFileMatrixChart() const
{
    return m_fileMatrixChart;
}

/**
 * @return Row index selected.
 */
int32_t
SelectionItemChartTwoMatrix::getRowIndex() const
{
    return m_rowIndex;
}

/**
 * @return column index selected.
 */
int32_t
SelectionItemChartTwoMatrix::getColumnIndex() const
{
    return m_columnIndex;
}

/**
 * Set the matrix chart.
 *
 * @param fileMatrixChart
 *     Matrix chart that was selected.
 * @param rowIndex
 *     Index of row selected.
 * @param columnIndex
 *     Index of column selected.
 */
void
SelectionItemChartTwoMatrix::setMatrixChart(ChartableTwoFileMatrixChart* fileMatrixChart,
                                            const int32_t rowIndex,
                                            const int32_t columnIndex)
{
    m_fileMatrixChart = fileMatrixChart;
    m_rowIndex        = rowIndex;
    m_columnIndex     = columnIndex;
}


