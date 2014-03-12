
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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

