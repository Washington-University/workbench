
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

#define __OME_DIMENSION_SIZES_DECLARE__
#include "OmeDimensionSizes.h"
#undef __OME_DIMENSION_SIZES_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::OmeDimensionSizes 
 * \brief Dimension sizes for each axis in OME
 * \ingroup OmeZarr
 */

/*
 * Constructor with all sizes invalid
 */
OmeDimensionSizes::OmeDimensionSizes()
: CaretObject()
{
    
}

/**
 * Constructor.
 * @param x
 *    Number of elements in X-dimension
 * @param y
 *    Number of elements in Y-dimension
 * @param z
 *    Number of elements in Z-dimension
 * @param time
 *    Number of elements in Time-dimension
 * @param channels
 *    Number of elements in Channels-dimension
 *
 */
OmeDimensionSizes::OmeDimensionSizes(const int64_t x,
                             const int64_t y,
                             const int64_t z,
                             const int64_t time,
                             const int64_t channels)
: CaretObject(),
m_x(x),
m_y(y),
m_z(z),
m_time(time),
m_channels(channels)
{
    
}

/**
 * Destructor.
 */
OmeDimensionSizes::~OmeDimensionSizes()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
OmeDimensionSizes::OmeDimensionSizes(const OmeDimensionSizes& obj)
: CaretObject(obj)
{
    this->copyHelperOmeDimensionSizes(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
OmeDimensionSizes&
OmeDimensionSizes::operator=(const OmeDimensionSizes& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperOmeDimensionSizes(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
OmeDimensionSizes::copyHelperOmeDimensionSizes(const OmeDimensionSizes& obj)
{
    m_x        = obj.m_x;
    m_y        = obj.m_y;
    m_z        = obj.m_z;
    m_time     = obj.m_time;
    m_channels = obj.m_channels;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeDimensionSizes::toString() const
{
    return ("Size X=" + AString::number(m_x)
            + "Y=" + AString::number(m_y)
            + "Z=" + AString::number(m_z)
            + "Time=" + AString::number(m_time)
            + "Channels=" + AString::number(m_channels));
}

