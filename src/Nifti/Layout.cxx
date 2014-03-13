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

#include "QFile"
#include "Layout.h"

using namespace caret;

LayoutType::LayoutType() {
    init();
}

void LayoutType::init ()
{
    needsSwapping = false;
    layoutSet=false;
    componentDimensions=1;
    niftiDataType = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
    sclSlope=0.0;
    sclIntercept=0.0;
}

int32_t LayoutType::valueByteSize() { //convenience method for getting value byte size from data type
    switch(niftiDataType)
    {
    case NIFTI_TYPE_UINT8:
    case NIFTI_TYPE_INT8:
        return 1;
    case NIFTI_TYPE_UINT16:
    case NIFTI_TYPE_INT16:
        return 2;
    case NIFTI_TYPE_RGB24:
        return 3;
    case NIFTI_TYPE_UINT32:
    case NIFTI_TYPE_INT32:
    case NIFTI_TYPE_FLOAT32:
        return 4;
    case NIFTI_TYPE_UINT64:
    case NIFTI_TYPE_INT64:
    case NIFTI_TYPE_FLOAT64:
        return 8;
    default:
        return 0;
    }
}

/*
    MATRIX_POINT,
    MATRIX_ROW,
    MATRIX_COLUMN,
    MATRIX_SLICE,
    MATRIX_FRAME,
    MATRIX_SERIES*/

void LayoutType::initStorageOrder() {
    storageOrder.insert(0, MATRIX_ROW );
    storageOrder.insert(1, MATRIX_COLUMN);
    storageOrder.insert(2, MATRIX_SLICE);
    storageOrder.insert(3, MATRIX_FRAME);
    storageOrder.insert(4, MATRIX_SERIES);
}

void LayoutType::getStorageOrderAsVector(std::vector <int32_t> &vec) { //gets a vector mapping of sorder dimensions
    for(int i=0;i<storageOrder.count();i++)
    {
        vec[i] = storageOrder.value(i);
    }
}

int32_t LayoutType::getLargestCommonDimension(std::vector<int32_t> &)//vec)
{
    return 0;//unimplemented
}

int32_t LayoutType::getContainingDimension(MatrixDimensionsEnum &)//e)
{
    return 0;//unimplemented
}

int32_t LayoutType::getSmallestContainingDimension()
{
    return 0;//unimplemented
}
