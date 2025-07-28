
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

#define __DATA_SLICE_DECLARE__
#include "DataSlice.h"
#undef __DATA_SLICE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DataSlice 
 * \brief Points to a 'slice' of data similar to a C++ std::slice.
 * \ingroup Common
 */

/**
 * Constructor.
 * @param dataPointer
 *    Pointer to first data element
 * @param dataLength
 *    Number of data elements
 * @param dataStride
 *    Offset to next element (use 1 for contiguous data)
 */
DataSlice::DataSlice(const float* dataPointer,
                     const int64_t dataLength,
                     const int64_t dataStride)
: CaretObject(),
m_dataPointer(const_cast<float*>(dataPointer)),
m_dataLength(dataLength),
m_dataStride(dataStride)
{
    
}

/**
 * Destructor.
 */
DataSlice::~DataSlice()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DataSlice::DataSlice(const DataSlice& obj)
: CaretObject(obj)
{
    this->copyHelperDataSlice(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DataSlice&
DataSlice::operator=(const DataSlice& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDataSlice(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DataSlice::copyHelperDataSlice(const DataSlice& obj)
{
    m_dataPointer = obj.m_dataPointer;
    m_dataLength  = obj.m_dataLength;
    m_dataStride  = obj.m_dataStride;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DataSlice::toString() const
{
    return "DataSlice";
}

