
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

#define __WU_Q_MACRO_COMMAND_PARAMETER_DECLARE__
#include "WuQMacroCommandParameter.h"
#undef __WU_Q_MACRO_COMMAND_PARAMETER_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQMacroCommandParameter 
 * \brief A paramater contained in a WuQMacroCommand
 * \ingroup Common
 */

/**
 * Constructor.
 */
WuQMacroCommandParameter::WuQMacroCommandParameter()
: CaretObjectTracksModification()
{
    
}

/**
 * Constructor.
 *
 * @param dataType
 *     Data type of the parameter
 * @param name
 *     Name of the parameter
 * @param value
 *     Value of the parameter
 */
WuQMacroCommandParameter::WuQMacroCommandParameter(const WuQMacroDataValueTypeEnum::Enum dataType,
                                                   const QString& name,
                                                   const QVariant& value)
: CaretObjectTracksModification(),
m_dataType(dataType),
m_name(name),
m_value(value)
{

}


/**
 * Destructor.
 */
WuQMacroCommandParameter::~WuQMacroCommandParameter()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacroCommandParameter::WuQMacroCommandParameter(const WuQMacroCommandParameter& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperWuQMacroCommandParameter(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
WuQMacroCommandParameter&
WuQMacroCommandParameter::operator=(const WuQMacroCommandParameter& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperWuQMacroCommandParameter(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
WuQMacroCommandParameter::copyHelperWuQMacroCommandParameter(const WuQMacroCommandParameter& obj)
{
    m_dataType = obj.m_dataType;
    m_name     = obj.m_name;
    m_value    = obj.m_value;
    setModified();
}

/**
 * @return Data type of this parameter
 */
WuQMacroDataValueTypeEnum::Enum
WuQMacroCommandParameter::getDataType() const
{
    return m_dataType;
}

/**
 * @return Name of this parameter
 */
QString
WuQMacroCommandParameter::getName() const
{
    return m_name;
}

/**
 * @return Value of this parameter
 */
QVariant
WuQMacroCommandParameter::getValue() const
{
    return m_value;
}

/**
 * Set the value of this parameter
 *
 * @param value
 *     New value of parameter
 */
void
WuQMacroCommandParameter::setValue(const QVariant& value)
{
    if (value != m_value) {
        m_value = value;
        setModified();
    }
}

