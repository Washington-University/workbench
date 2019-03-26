
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WB_MACRO_CUSTOM_DATA_INFO_DECLARE__
#include "WbMacroCustomDataInfo.h"
#undef __WB_MACRO_CUSTOM_DATA_INFO_DECLARE__

#include <limits>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WbMacroCustomDataInfo 
 * \brief  Information about custom data such as valid data range
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomDataInfo::WbMacroCustomDataInfo(const WuQMacroDataValueTypeEnum::Enum dataType)
: CaretObject(),
m_dataType(dataType)
{
    m_integerRange[0] = std::numeric_limits<int32_t>::min();
    m_integerRange[1] = std::numeric_limits<int32_t>::max();
    
    m_floatRange[0] = -std::numeric_limits<float>::max();
    m_floatRange[1] =  std::numeric_limits<float>::max();
}

/**
 * Destructor.
 */
WbMacroCustomDataInfo::~WbMacroCustomDataInfo()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WbMacroCustomDataInfo::WbMacroCustomDataInfo(const WbMacroCustomDataInfo& obj)
: CaretObject(obj)
{
    this->copyHelperWbMacroCustomDataInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
WbMacroCustomDataInfo&
WbMacroCustomDataInfo::operator=(const WbMacroCustomDataInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperWbMacroCustomDataInfo(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
WbMacroCustomDataInfo::copyHelperWbMacroCustomDataInfo(const WbMacroCustomDataInfo& obj)
{
    m_dataType         = obj.m_dataType;
    m_floatRange       = obj.m_floatRange;
    m_integerRange     = obj.m_integerRange;
    m_stringListValues = obj.m_stringListValues;
}

/**
 * @return The data type
 */
WuQMacroDataValueTypeEnum::Enum
WbMacroCustomDataInfo::getDataType() const
{
    return m_dataType;
}

/**
 * @return Range for float data
 */
std::array<float, 2>
WbMacroCustomDataInfo::getFloatRange() const
{
    return m_floatRange;
}

/**
 * Set the range of float data
 *
 * @param range
 *      Range of the float data
 */
void
WbMacroCustomDataInfo::setFloatRange(const std::array<float, 2>& range)
{
    m_floatRange = range;
}

/**
 * @return Range for integer data
 */
std::array<int32_t, 2>
WbMacroCustomDataInfo::getIntegerRange() const
{
    return m_integerRange;
}

/**
 * Set the range of integer data
 *
 * @param range
 *      Range of the integer data
 */
void
WbMacroCustomDataInfo::setIntegerRange(const std::array<int32_t, 2>& range)
{
    m_integerRange = range;
}

/**
 * @return List of string values
 */
std::vector<QString>
WbMacroCustomDataInfo::getStringListValues() const
{
    return m_stringListValues;
}

/**
 * Set the list of string values
 *
 * @param range
 *      List of string values
 */
void
WbMacroCustomDataInfo::setStringListValues(const std::vector<QString>& values)
{
    m_stringListValues = values;
}



