
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

#define __SELECTION_ITEM_CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN_DECLARE__
#include "SelectionItemCiftiConnectivityMatrixRowColumn.h"
#undef __SELECTION_ITEM_CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN_DECLARE__

#include "CaretAssert.h"
using namespace caret;



/**
 * \class caret::SelectionItemCiftiConnectivityMatrixRowColumn
 * \brief Contains selection of a row or column in a CIFTI connectivity matrix file.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemCiftiConnectivityMatrixRowColumn::SelectionItemCiftiConnectivityMatrixRowColumn()
: SelectionItem(SelectionItemDataTypeEnum::CIFTI_CONNECTIVITY_MATRIX_ROW_COLUMN)
{
    m_ciftiConnectivityMatrixFile = NULL;
    m_matrixRowIndex           = -1;
    m_matrixColumnIndex        = -1;
}

/**
 * Destructor.
 */
SelectionItemCiftiConnectivityMatrixRowColumn::~SelectionItemCiftiConnectivityMatrixRowColumn()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemCiftiConnectivityMatrixRowColumn::SelectionItemCiftiConnectivityMatrixRowColumn(const SelectionItemCiftiConnectivityMatrixRowColumn& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemCiftiConnectivityMatrixRowColumn(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
SelectionItemCiftiConnectivityMatrixRowColumn&
SelectionItemCiftiConnectivityMatrixRowColumn::operator=(const SelectionItemCiftiConnectivityMatrixRowColumn& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemCiftiConnectivityMatrixRowColumn(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
SelectionItemCiftiConnectivityMatrixRowColumn::copyHelperSelectionItemCiftiConnectivityMatrixRowColumn(const SelectionItemCiftiConnectivityMatrixRowColumn& obj)
{
    m_ciftiConnectivityMatrixFile = obj.m_ciftiConnectivityMatrixFile;
    m_matrixRowIndex           = obj.m_matrixRowIndex;
    m_matrixColumnIndex        = obj.m_matrixColumnIndex;
}

/**
 * @return True if the CIFTI connectivity matrix row or column is valid, else false.
 */
bool
SelectionItemCiftiConnectivityMatrixRowColumn::isValid() const
{
    if (m_ciftiConnectivityMatrixFile != NULL) {
        if ((m_matrixRowIndex >= 0)
            || (m_matrixColumnIndex >= 0)) {
            return true;
        }
    }
    
    return false;
}

/**
 * Reset the selections.
 */
void
SelectionItemCiftiConnectivityMatrixRowColumn::reset()
{
    m_ciftiConnectivityMatrixFile = NULL;
    m_matrixRowIndex           = -1;
    m_matrixColumnIndex        = -1;
}

/**
 * @return CIFTI connectivity matrix file.
 */
CiftiMappableConnectivityMatrixDataFile*
SelectionItemCiftiConnectivityMatrixRowColumn::getCiftiConnectivityMatrixFile() const
{
    return m_ciftiConnectivityMatrixFile;
}

/**
 * @return CIFTI connectivity matrix row index.  A negative value
 * indicates that the row index is invalid.
 */
int32_t
SelectionItemCiftiConnectivityMatrixRowColumn::getMatrixRowIndex() const
{
    return m_matrixRowIndex;
}

/**
 * @return CIFTI connectivity matrix column index.  A negative value
 * indicates that the column index is invalid.
 */
int32_t
SelectionItemCiftiConnectivityMatrixRowColumn::getMatrixColumnIndex() const
{
    return m_matrixColumnIndex;
}

/**
 * Set the selection to a CIFTI connectivity matrix file row.
 *
 * @param ciftiConnectivityMatrixFile
 *    The CIFTI connectivity matrix file
 * @param matrixRowIndex
 *    Row index
 */
void
SelectionItemCiftiConnectivityMatrixRowColumn::setFileRow(CiftiMappableConnectivityMatrixDataFile* ciftiConnectivityMatrixFile,
                                                          const int32_t matrixRowIndex)
{
    CaretAssert(ciftiConnectivityMatrixFile);
    CaretAssert(matrixRowIndex >= 0);
    
    m_ciftiConnectivityMatrixFile = ciftiConnectivityMatrixFile;
    m_matrixRowIndex           = matrixRowIndex;
    m_matrixColumnIndex        = -1;
}

/**
 * Set the selection to a CIFTI connectivity matrix file column.
 *
 * @param ciftiConnectivityMatrixFile
 *    The CIFTI connectivity matrix file
 * @param matrixColumnIndex
 *    Column index
 */
void
SelectionItemCiftiConnectivityMatrixRowColumn::setFileColumn(CiftiMappableConnectivityMatrixDataFile* ciftiConnectivityMatrixFile,
                                                             const int32_t matrixColumnIndex)
{
    CaretAssert(ciftiConnectivityMatrixFile);
    CaretAssert(matrixColumnIndex >= 0);
    
    m_ciftiConnectivityMatrixFile = ciftiConnectivityMatrixFile;
    m_matrixRowIndex           = -1;
    m_matrixColumnIndex        = matrixColumnIndex;
}


