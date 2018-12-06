
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
 * @param objectType
 *    Type of object
 * @param objectName
 *    Name of object
 * @param objectToolTip
 *    Tooltip of object
 * @param value
 *    Value for the command.
 */
WuQMacroCommand::WuQMacroCommand(const WuQMacroObjectTypeEnum::Enum objectType,
                                 const QString& objectName,
                                 const QString& objectToolTip,
                                 const QVariant value)
: CaretObjectTracksModification(),
m_objectType(objectType),
m_objectName(objectName),
m_objectToolTip(objectToolTip),
m_value(value),
m_macroMouseEvent(NULL)
{
    switch (m_objectType) {
        case WuQMacroObjectTypeEnum::ACTION:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroObjectTypeEnum::CHECK_BOX:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroObjectTypeEnum::COMBO_BOX:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroObjectTypeEnum::DOUBLE_SPIN_BOX:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::FLOAT;
            break;
        case WuQMacroObjectTypeEnum::INVALID:
            break;
        case WuQMacroObjectTypeEnum::LINE_EDIT:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroObjectTypeEnum::LIST_WIDGET:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroObjectTypeEnum::MENU:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::STRING;
            break;
        case WuQMacroObjectTypeEnum::MOUSE_USER_EVENT:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::MOUSE;
            CaretAssertMessage(0, "Must use constructor for mouse event");
            break;
        case WuQMacroObjectTypeEnum::PUSH_BUTTON:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroObjectTypeEnum::RADIO_BUTTON:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::BOOLEAN;
            break;
        case WuQMacroObjectTypeEnum::SLIDER:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroObjectTypeEnum::SPIN_BOX:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroObjectTypeEnum::TAB_BAR:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroObjectTypeEnum::TAB_WIDGET:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::INTEGER;
            break;
        case WuQMacroObjectTypeEnum::TOOL_BUTTON:
            m_objectDataValueType = WuQMacroDataValueTypeEnum::BOOLEAN;
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
 * @param objectToolTip
 *    Tooltip of object
 * @param value
 *    Value for the command.
 */
WuQMacroCommand::WuQMacroCommand(const QString& objectName,
                                 const QString& objectToolTip,
                                 WuQMacroMouseEventInfo* mouseEventInfo)
: CaretObjectTracksModification(),
m_objectType(WuQMacroObjectTypeEnum::MOUSE_USER_EVENT),
m_objectName(objectName),
m_objectToolTip(objectToolTip),
m_value((int)0),
m_macroMouseEvent(mouseEventInfo)
{
    m_objectDataValueType = WuQMacroDataValueTypeEnum::MOUSE;
    
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
    m_objectType = obj.m_objectType;
    m_objectName = obj.m_objectName;
    m_objectToolTip = obj.m_objectToolTip;
    m_value = obj.m_value;
    m_macroMouseEvent = NULL;
    m_objectDataValueType = obj.m_objectDataValueType;
    
    switch (m_objectDataValueType) {
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
 * @return The object' type
 */
WuQMacroObjectTypeEnum::Enum
WuQMacroCommand::getObjectType() const
{
    return m_objectType;
}

/**
 * @return Type of data value for object
 */
WuQMacroDataValueTypeEnum::Enum
WuQMacroCommand::getObjectDataValueType() const
{
    return m_objectDataValueType;
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
 * @return The object's tooltip
 */
QString
WuQMacroCommand::getObjectToolTip() const
{
    return m_objectToolTip;
}

/**
 * @return The object's value.
 */
QVariant
WuQMacroCommand::getObjectValue() const
{
    return m_value;
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
    QString s("WuQMacroCommand name=%1, type=%2, tooltip=%3, value=%4");
    s = s.arg(m_objectName
              ).arg(WuQMacroObjectTypeEnum::toGuiName(m_objectType)
                    ).arg(m_objectToolTip
                    ).arg(m_value.toString());
    return s;
}

