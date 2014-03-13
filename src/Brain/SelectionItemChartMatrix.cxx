
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

#define __SELECTION_ITEM_CHART_MATRIX_DECLARE__
#include "SelectionItemChartMatrix.h"
#undef __SELECTION_ITEM_CHART_MATRIX_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartMatrix 
 * \brief Contains selection of a data-series chart.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartMatrix::SelectionItemChartMatrix()
: SelectionItem(SelectionItemDataTypeEnum::CHART_MATRIX)
{
    m_chartableMatrixInterface = NULL;
    m_matrixRowIndex           = -1;
    m_matrixColumnIndex        = -1;
}

/**
 * Destructor.
 */
SelectionItemChartMatrix::~SelectionItemChartMatrix()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartMatrix::SelectionItemChartMatrix(const SelectionItemChartMatrix& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartMatrix(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartMatrix&
SelectionItemChartMatrix::operator=(const SelectionItemChartMatrix& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartMatrix(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartMatrix::copyHelperSelectionItemChartMatrix(const SelectionItemChartMatrix& obj)
{
    m_chartableMatrixInterface = obj.m_chartableMatrixInterface;
    m_matrixRowIndex           = obj.m_matrixRowIndex;
    m_matrixColumnIndex        = obj.m_matrixColumnIndex;
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartMatrix::isValid() const
{
    if ((m_chartableMatrixInterface != NULL)
        && (m_matrixRowIndex >= 0)
        && (m_matrixColumnIndex >= 0)) {
        return true;
    }
    
    return false;
}

/**
 * Reset the selections.
 */
void
SelectionItemChartMatrix::reset()
{
    m_chartableMatrixInterface = NULL;
    m_matrixRowIndex           = -1;
    m_matrixColumnIndex        = -1;
}

/**
 * @return The Chartable Matrix Interface (file)
 */
ChartableMatrixInterface*
SelectionItemChartMatrix::getChartableMatrixInterface() const
{
    return m_chartableMatrixInterface;
}

/**
 * @return Matrix row index.
 */
int32_t
SelectionItemChartMatrix::getMatrixRowIndex() const
{
    return m_matrixRowIndex;
}

/**
 * @return Matrix column index.
 */
int32_t
SelectionItemChartMatrix::getMatrixColumnIndex() const
{
    return m_matrixColumnIndex;
}

/**
 * Set the selection information.
 *
 * @param chartableMatrixInterface
 *    The chartable matrix interface (file)
 * @param matrixRowIndex
 *    Row index
 * @param matrixColumnIndex
 *    Column index
 */
void
SelectionItemChartMatrix::setChartMatrix(ChartableMatrixInterface* chartableMatrixInterface,
                    const int32_t matrixRowIndex,
                    const int32_t matrixColumnIndex)
{
    CaretAssert(chartableMatrixInterface);
    CaretAssert(matrixRowIndex >= 0);
    CaretAssert(matrixColumnIndex >= 0);
    
    m_chartableMatrixInterface = chartableMatrixInterface;
    m_matrixRowIndex           = matrixRowIndex;
    m_matrixColumnIndex        = matrixColumnIndex;
}

