
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

#define __OME_AXIS_DECLARE__
#include "OmeAxis.h"
#undef __OME_AXIS_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::OmeAxis 
 * \brief Model an NGFF Axis
 * \ingroup OmeZarr
 *
 * Describes an Axis in the .zattrs file 
 *  https://ngff.openmicroscopy.org/0.4/index.html#axes-md
 */

/**
 * Constructor.
 */
OmeAxis::OmeAxis()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
OmeAxis::~OmeAxis()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
OmeAxis::OmeAxis(const OmeAxis& obj)
: CaretObject(obj)
{
    this->copyHelperOmeAxis(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
OmeAxis&
OmeAxis::operator=(const OmeAxis& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperOmeAxis(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
OmeAxis::copyHelperOmeAxis(const OmeAxis& obj)
{
    m_axisType  = obj.m_axisType;
    m_spaceUnit = obj.m_spaceUnit;
    m_timeUnit  = obj.m_timeUnit;
    m_name      = obj.m_name;
}

/**
 * @return The axis type
 */
OmeAxisTypeEnum::Enum
OmeAxis::getAxisType() const
{
    return m_axisType;
}

/**
 * Set the axis type
 * @param axisType
 *    New axis type
 */
void
OmeAxis::setAxisType(const OmeAxisTypeEnum::Enum axisType)
{
    m_axisType = axisType;
}

/**
 * @return The space unit
 */
OmeSpaceUnitEnum::Enum
OmeAxis::getSpaceUnit() const
{
    return m_spaceUnit;
}

/**
 * Set the space unit
 * @param spaceUnit
 *    New space unit
 */
void
OmeAxis::setSpaceUnit(const OmeSpaceUnitEnum::Enum spaceUnit)
{
    m_spaceUnit = spaceUnit;
}

/**
 * @return The time unit
 */
OmeTimeUnitEnum::Enum
OmeAxis::getTimeUnit() const
{
    return m_timeUnit;
}

/**
 * Set the time unit
 * @param timeUnit
 *    The new time unit
 */
void
OmeAxis::setTimeUnit(const OmeTimeUnitEnum::Enum timeUnit)
{
    m_timeUnit = timeUnit;
}

/**
 * @return The name
 */
QString
OmeAxis::getName() const
{
    return m_name;
}

/**
 * Set the name
 * @param name
 *    The new name
 */
void
OmeAxis::setName(const QString& name)
{
    m_name = name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeAxis::toString() const
{
    AString txt("OmeAxis\n"
                "   Name: " + m_name +
                "   Type: " + OmeAxisTypeEnum::toName(m_axisType) +
                "   Space Unit: " + OmeSpaceUnitEnum::toName(m_spaceUnit) +
                "   Time Unit:  " + OmeTimeUnitEnum::toName(m_timeUnit));
    return txt;
}

