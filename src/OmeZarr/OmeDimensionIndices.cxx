
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __OME_DIMENSION_INDICES_DECLARE__
#include "OmeDimensionIndices.h"
#undef __OME_DIMENSION_INDICES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::OmeDimensionIndices 
 * \brief Contains indices for accessing elements of an OME-ZARR image
 * \ingroup OmeZarr
 *
 * https://ngff.openmicroscopy.org/0.4/index.html#multiscale-md
 */

/**
 * Constructor for invalid indices
 */
OmeDimensionIndices::OmeDimensionIndices()
: CaretObject()
{
    
}

/**
 * Constructor.
 *
 * @param xDimensionIndex
 *    Index for the X-dimension
 * @param yDimensionIndex
 *    Index for the Y-dimension
 * @param zDimensionIndex
 *    Index for the Z-dimension
 * @param timeDimensionIndex
 *    Index for the time dimension
 * @param channelDimensionIndex
 *    Index for the channel dimension
 */
OmeDimensionIndices::OmeDimensionIndices(const int64_t xDimensionIndex,
                                         const int64_t yDimensionIndex,
                                         const int64_t zDimensionIndex,
                                         const int64_t timeDimensionIndex,
                                         const int64_t channelDimensionIndex)
: CaretObject(),
m_xDimensionIndex(xDimensionIndex),
m_yDimensionIndex(yDimensionIndex),
m_zDimensionIndex(zDimensionIndex),
m_timeDimensionIndex(timeDimensionIndex),
m_channelDimensionIndex(channelDimensionIndex)
{
    
}

/**
 * Destructor.
 */
OmeDimensionIndices::~OmeDimensionIndices()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
OmeDimensionIndices::OmeDimensionIndices(const OmeDimensionIndices& obj)
: CaretObject(obj)
{
    this->copyHelperOmeDimensionIndices(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
OmeDimensionIndices&
OmeDimensionIndices::operator=(const OmeDimensionIndices& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperOmeDimensionIndices(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
OmeDimensionIndices::copyHelperOmeDimensionIndices(const OmeDimensionIndices& obj)
{
    m_xDimensionIndex       = obj.m_xDimensionIndex;
    m_yDimensionIndex       = obj.m_yDimensionIndex;
    m_zDimensionIndex       = obj.m_zDimensionIndex;
    m_timeDimensionIndex    = obj.m_timeDimensionIndex;
    m_channelDimensionIndex = obj.m_channelDimensionIndex;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeDimensionIndices::toString() const
{
    return ("Indices: X=" + AString::number(m_xDimensionIndex)
            + ", Y=" + AString::number(m_yDimensionIndex)
            + ", Z=" + AString::number(m_zDimensionIndex)
            + ", Time=" + AString::number(m_timeDimensionIndex)
            + ", Channel=" + AString::number(m_channelDimensionIndex));
}

