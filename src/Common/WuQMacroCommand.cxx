
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_COMMAND_DECLARE__
#include "WuQMacroCommand.h"
#undef __WU_Q_MACRO_COMMAND_DECLARE__

#include "CaretAssert.h"
#include "WuQMacroMouseEventInfo.h"

using namespace caret;

/**
 * \class caret::WuQMacroCommand 
 * \brief Issues a QObject's signal so that its slots execute
 * \ingroup WuQMacro
 */

/**
 * Constructor for a macro command
 *
 * @param classType
 *    Type of object class
 * @param objectName
 *    Name of object
 * @param dataValue
 *    Data value for the command.
 * @param dataValueTwo
 *    Second Data value for the command.
 */
WuQMacroCommand::WuQMacroCommand(const WuQMacroClassTypeEnum::Enum classType,
                                 const QString& objectName,
                                 const QVariant dataValue,
                                 const QVariant dataValueTwo)
: CaretObjectTracksModification(),
m_classType(classType),
m_objectName(objectName),
m_dataValue(dataValue),
m_dataValueTwo(dataValueTwo),
m_macroMouseEvent(NULL)
{
    /*
     * Second data value is used by only some commands
     */
    m_dataTypeTwo = WuQMacroDataValueTypeEnum::STRING;
    
    switch (m_classType) {
        case WuQMacroClassTypeEnum::ACTION:
            m_dataType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroClassTypeEnum::ACTION_GROUP:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            m_dataTypeTwo = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroClassTypeEnum::BUTTON_GROUP:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            m_dataTypeTwo = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroClassTypeEnum::CHECK_BOX:
            m_dataType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroClassTypeEnum::COMBO_BOX:
            m_dataType    = WuQMacroDataValueTypeEnum::INTEGER;
            m_dataTypeTwo = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroClassTypeEnum::DOUBLE_SPIN_BOX:
            m_dataType = WuQMacroDataValueTypeEnum::FLOAT;
            break;
        case WuQMacroClassTypeEnum::INVALID:
            break;
        case WuQMacroClassTypeEnum::LINE_EDIT:
            m_dataType = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroClassTypeEnum::LIST_WIDGET:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            m_dataTypeTwo = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroClassTypeEnum::MENU:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            m_dataTypeTwo = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroClassTypeEnum::MOUSE_USER_EVENT:
            m_dataType = WuQMacroDataValueTypeEnum::MOUSE;
            CaretAssertMessage(0, "Must use constructor for mouse event");
            break;
        case WuQMacroClassTypeEnum::PUSH_BUTTON:
            m_dataType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroClassTypeEnum::RADIO_BUTTON:
            m_dataType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroClassTypeEnum::SLIDER:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroClassTypeEnum::SPIN_BOX:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroClassTypeEnum::TAB_BAR:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroClassTypeEnum::TAB_WIDGET:
            m_dataType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroClassTypeEnum::TOOL_BUTTON:
            m_dataType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
    }
    
    setModified();
}

/**
 * Constructor for a macro command containing a mouse event
 *
 * @param objectType
 *    Type of object
 * @param objectName
 *    Name of object
 * @param value
 *    Value for the command.
 */
WuQMacroCommand::WuQMacroCommand(const QString& objectName,
                                 WuQMacroMouseEventInfo* mouseEventInfo)
: CaretObjectTracksModification(),
m_classType(WuQMacroClassTypeEnum::MOUSE_USER_EVENT),
m_objectName(objectName),
m_dataValue((int)0),
m_dataValueTwo(""),
m_macroMouseEvent(mouseEventInfo)
{
    m_dataType    = WuQMacroDataValueTypeEnum::MOUSE;
    m_dataTypeTwo = WuQMacroDataValueTypeEnum::STRING;
    
    setModified();
}

/**
 * Destructor.
 */
WuQMacroCommand::~WuQMacroCommand()
{
    if (m_macroMouseEvent != NULL) {
        delete m_macroMouseEvent;
    }
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacroCommand::WuQMacroCommand(const WuQMacroCommand& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperWuQMacroCommand(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
WuQMacroCommand&
WuQMacroCommand::operator=(const WuQMacroCommand& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperWuQMacroCommand(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
WuQMacroCommand::copyHelperWuQMacroCommand(const WuQMacroCommand& obj)
{
    m_classType = obj.m_classType;
    m_objectName   = obj.m_objectName;
    m_dataValue    = obj.m_dataValue;
    m_dataType     = obj.m_dataType;
    m_dataTypeTwo  = obj.m_dataTypeTwo;
    m_dataValueTwo = obj.m_dataValueTwo;
    m_macroMouseEvent = NULL;
    
    switch (m_dataType) {
        case WuQMacroDataValueTypeEnum::BOOLEAN:
            break;
        case WuQMacroDataValueTypeEnum::FLOAT:
            break;
        case WuQMacroDataValueTypeEnum::INTEGER:
            break;
        case WuQMacroDataValueTypeEnum::MOUSE:
            CaretAssert(obj.m_macroMouseEvent);
            m_macroMouseEvent = new WuQMacroMouseEventInfo(*obj.m_macroMouseEvent);
            break;
        case WuQMacroDataValueTypeEnum::STRING:
            break;
    }
}

/**
 * @return The object' class type
 */
WuQMacroClassTypeEnum::Enum
WuQMacroCommand::getClassType() const
{
    return m_classType;
}

/**
 * @return Type of data value for object
 */
WuQMacroDataValueTypeEnum::Enum
WuQMacroCommand::getDataType() const
{
    return m_dataType;
}

/**
 * @return Type of second data value for object
 */
WuQMacroDataValueTypeEnum::Enum
WuQMacroCommand::getDataTypeTwo() const
{
    return m_dataTypeTwo;
}

/**
 * @return The object's name
 */
QString
WuQMacroCommand::getObjectName() const
{
    return m_objectName;
}

/**
 * @return The object's data value.
 */
QVariant
WuQMacroCommand::getDataValue() const
{
    return m_dataValue;
}

/**
 * @return The object's second data value.
 */
QVariant
WuQMacroCommand::getDataValueTwo() const
{
    return m_dataValueTwo;
}

/**
 * @return Point to mouse event information
 */
const WuQMacroMouseEventInfo*
WuQMacroCommand::getMouseEventInfo() const
{
    return m_macroMouseEvent;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
WuQMacroCommand::toString() const
{
    QString s("WuQMacroCommand name=%1, type=%2, value=%3, valueTwo=%4");
    s = s.arg(m_objectName
              ).arg(WuQMacroClassTypeEnum::toGuiName(m_classType)
                    ).arg(m_dataValue.toString()
                    ).arg(m_dataValueTwo.toString());
    return s;
}

