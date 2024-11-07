
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

#define __OME_COORDINATE_TRANSFORMATIONS_DECLARE__
#include "OmeCoordinateTransformations.h"
#undef __OME_COORDINATE_TRANSFORMATIONS_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::OmeCoordinateTransformations 
 * \brief NGFF CoordinateTransformations
 * \ingroup OmeZarr
 *
 * A coordinate transformation is found in the .zattrs file
 *  https://ngff.openmicroscopy.org/0.4/index.html#trafo-md
 */

/**
 * Constructor for invalid instance.
 */
OmeCoordinateTransformations::OmeCoordinateTransformations()
: CaretObject(),
m_type(OmeCoordinateTransformationTypeEnum::INVALID)
{
    
}
/**
 * Constructor.
 * @param type
 *    The type of transform
 * @param values
 *    Values defining the transform
 */
OmeCoordinateTransformations::OmeCoordinateTransformations(const OmeCoordinateTransformationTypeEnum::Enum type,
                                                           const std::vector<float>& transformValues)
: CaretObject(),
m_type(type),
m_transformValues(transformValues)
{
    
}

/**
 * Destructor.
 */
OmeCoordinateTransformations::~OmeCoordinateTransformations()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
OmeCoordinateTransformations::OmeCoordinateTransformations(const OmeCoordinateTransformations& obj)
: CaretObject(obj)
{
    this->copyHelperOmeCoordinateTransformations(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
OmeCoordinateTransformations&
OmeCoordinateTransformations::operator=(const OmeCoordinateTransformations& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperOmeCoordinateTransformations(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
OmeCoordinateTransformations::copyHelperOmeCoordinateTransformations(const OmeCoordinateTransformations& obj)
{
    m_type            = obj.m_type;
    m_transformValues = obj.m_transformValues;
}

/**
 * @return The type of the transform
 */
OmeCoordinateTransformationTypeEnum::Enum 
OmeCoordinateTransformations::getType() const
{
    return m_type;
}

/**
 * @return The transform values
 */
std::vector<float>
OmeCoordinateTransformations::getTransformValues() const
{
    return m_transformValues;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeCoordinateTransformations::toString() const
{
    AString txt;
    txt.appendWithNewLine("Type: " + OmeCoordinateTransformationTypeEnum::toGuiName(m_type)
                          + " Transform: " + AString::fromNumbers(m_transformValues));
    return txt;
}

