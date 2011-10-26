/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef LAYOUT_H
#define LAYOUT_H
#include "NiftiEnums.h"
#include "NiftiException.h"
#include "QFile"
#include "AString.h"
#include "VolumeFile.h"
#include "stdint.h"
#include "zlib.h"
#include "QMap"

namespace caret {
// NOT USED, yet...
enum MatrixDimensionsEnum {//ways of accessing matrix
    MATRIX_POINT,
    MATRIX_ROW,
    MATRIX_COLUMN,
    MATRIX_SLICE,
    MATRIX_FRAME,
    MATRIX_SERIES
};
// END NOT USED
enum MatrixIndexingOrderEnum {
    ROW_MAJOR_ORDER,
    COLUMN_MAJOR_ORDER
};

//constants for the size of each dimension
const int32_t MATRIX_POINT_DIMENSIONS=0;
const int32_t MATRIX_ROW_DIMENSIONS=1;
const int32_t MATRIX_COLUMN_DIMENSIONS=1;
const int32_t MATRIX_SLICE_DIMENSIONS=2;
const int32_t MATRIX_FRAME_DIMENSIONS=3;
const int32_t MATRIX_SERIES_DIMENSIONS=4;



class  LayoutType {
public:
    LayoutType();
    int32_t valueByteSize();
    void init();
    void initStorageOrder();
    void getStorageOrderAsVector(std::vector<int32_t> &vec);
    int32_t getLargestCommonDimension(std::vector<int32_t> &vec);//given two layouts, gets the largest dimension in common
                                                                 //for layouts that are the same, it's the same as the
                                                                 //smallest containing dimension for both layouts
    int32_t getContainingDimension(MatrixDimensionsEnum &e);//gets the smallest dimension that contains the (sub)layout for
                                                           //the given dimension
    int32_t getSmallestContainingDimension();//gets the highest order dimension that encapsulates the layout for this
                                             //particular matrix
    //data
    bool needsSwapping;
    bool layoutSet;
    int32_t componentDimensions;
    std::vector <int64_t> dimensions;
    //std::vector <int32_t> indexingOrder;//for later, this will determine the order of indexing (i.e. row vs column major order, etc.
    //std::vector <int32_t> storageOrder;//for later, this will determine how it is laid out on disk
    NiftiDataTypeEnum::Enum niftiDataType;//we borrow this enum from nifti, but could be used in generic matrix implementation
    double sclSlope;//for data scaling, may not belong here..
    double sclIntercept;
    QMap <int32_t, MatrixDimensionsEnum> storageOrder;
};
}
#endif //LAYOUT_H
