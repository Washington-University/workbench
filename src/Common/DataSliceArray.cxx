
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

#define __DATA_SLICE_ARRAY_DECLARE__
#include "DataSliceArray.h"
#undef __DATA_SLICE_ARRAY_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DataSliceArray 
 * \brief Container for multiple DataSlice elements
 * \ingroup Common
 */

/**
 * Constructor.
 */
DataSliceArray::DataSliceArray()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
DataSliceArray::~DataSliceArray()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DataSliceArray::DataSliceArray(const DataSliceArray& obj)
: CaretObject(obj)
{
    this->copyHelperDataSliceArray(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DataSliceArray&
DataSliceArray::operator=(const DataSliceArray& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDataSliceArray(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DataSliceArray::copyHelperDataSliceArray(const DataSliceArray& obj)
{
    m_dataSlices = obj.m_dataSlices;
}

/**
 * Add a data slice
 * @param dataPointer
 *    Pointer to first data element
 * @param dataLength
 *    Number of data elements
 * @param dataStride
 *    Offset to next element (use 1 for contiguous data)
 */
void
DataSliceArray::addDataSlice(const float* dataPointer,
                             const int64_t dataLength,
                             const int64_t dataStride)
{
    m_dataSlices.emplace_back(dataPointer,
                              dataLength,
                              dataStride);
    m_allDataLength += dataLength;
}

/**
 * @return All data in a vector
 * @param dataOut
 *   Contains all data upon exit.
 */
void
DataSliceArray::getAllData(std::vector<float>& dataOut) const
{
    dataOut.clear();
    
    if (m_allDataLength <= 0) {
        return;
    }
    
    dataOut.resize(m_allDataLength);
    
    int64_t dataIndex(0);
    for (const auto& ds :  m_dataSlices) {
        const float* slicePointer(ds.getDataPointer());
        CaretAssert(slicePointer);
        const int64_t sliceLength(ds.getDataLength());
        const int64_t sliceStride(ds.getDataStride());
        int64_t sliceIndex(0);
        for (int64_t i = 0; i < sliceLength; i++) {
            CaretAssert(dataIndex < m_allDataLength);
            dataOut[dataIndex] = slicePointer[sliceIndex];
            dataIndex  += 1;
            sliceIndex += sliceStride;
        }
    }
    
    CaretAssert(dataIndex == m_allDataLength);
}


/**
 * @return Length (total number of data elements)
 */
int64_t
DataSliceArray::getLength() const
{
    return m_allDataLength;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DataSliceArray::toString() const
{
    return "DataSliceArray";
}

