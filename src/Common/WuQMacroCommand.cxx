
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
#include "CaretLogger.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroMouseEventInfo.h"
#include "WuQMacroStandardItemTypeEnum.h"

using namespace caret;

/**
 * \class caret::WuQMacroCommand 
 * \brief Issues a QObject's signal so that its slots execute
 * \ingroup WuQMacro
 */

/**
 * Create a new instance of a macro comand for a custom command
 *
 * @param commandType
 *    Type of command
 * @param customOperationTypeName
 *    Name of a custom operation
 * @param mouseEventInfo
 *    Information about mouse event.
 * @param widgetType
 *    Type of widget
 * @param version
 *    Version of command
 * @param objectName
 *    Name of object
 * @param objectDescriptiveName
 *    Descriptive name of macro command
 * @param objectToolTip
 *    ToolTip for object
 * @param delayInSeconds
 *    Delay in seconds
 * @param errorMessageOut
 *    Output with error messag if new instance fails
 * @return
 *    The widget command or NULL if failure
 */
WuQMacroCommand*
WuQMacroCommand::newInstanceCustomCommand(const QString& customOperationName,
                                                 const int32_t version,
                                                 const QString& objectName,
                                                 const QString& objectDescriptiveName,
                                          const QString& objectToolTip,
                                          const float delayInSeconds,
                                          QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (customOperationName.isEmpty()) {
        errorMessageOut = "Custom operation may not be empty for a custom operation macro command";
        return NULL;
    }
    
    WuQMacroMouseEventInfo* mouseEventInfo(NULL);
    WuQMacroCommand* mc = new WuQMacroCommand(WuQMacroCommandTypeEnum::CUSTOM_OPERATION,
                                              customOperationName,
                                              mouseEventInfo,
                                              WuQMacroWidgetTypeEnum::INVALID,
                                              version,
                                              objectName,
                                              objectDescriptiveName,
                                              objectToolTip,
                                              delayInSeconds);
    return mc;
}

/**
 * Create a new instance of a macro command for mouse operation
 *
 * @param commandType
 *    Type of command
 * @param customOperationTypeName
 *    Name of a custom operation
 * @param mouseEventInfo
 *    Information about mouse event (will take ownership)
 * @param widgetType
 *    Type of widget
 * @param version
 *    Version of command
 * @param objectName
 *    Name of object
 * @param objectDescriptiveName
 *    Descriptive name of macro command
 * @param objectToolTip
 *    ToolTip for object
 * @param delayInSeconds
 *    Delay in seconds
 * @param errorMessageOut
 *    Output with error messag if new instance fails
 * @return
 *    The widget command or NULL if failure
 */
WuQMacroCommand*
WuQMacroCommand::newInstanceMouseCommand(WuQMacroMouseEventInfo* mouseEventInfo,
                                                const int32_t version,
                                                const QString& objectName,
                                                const QString& objectDescriptiveName,
                                         const QString& objectToolTip,
                                         const float delayInSeconds,
                                         QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (mouseEventInfo == NULL) {
        errorMessageOut = "Mouse info is invalid (NULL) for a macro mouse command";
        return NULL;
    }
    
    const QString customOperationName("");
    WuQMacroCommand* mc = new WuQMacroCommand(WuQMacroCommandTypeEnum::MOUSE,
                                              customOperationName,
                                              mouseEventInfo,
                                              WuQMacroWidgetTypeEnum::INVALID,
                                              version,
                                              objectName,
                                              objectDescriptiveName,
                                              objectToolTip,
                                              delayInSeconds);
    return mc;
}

/**
 * Create a new instance of a macro command for a Qt Widget
 *
 * @param commandType
 *    Type of command
 * @param customOperationTypeName
 *    Name of a custom operation
 * @param mouseEventInfo
 *    Information about mouse event.
 * @param widgetType
 *    Type of widget
 * @param version
 *    Version of command
 * @param objectName
 *    Name of object
 * @param objectDescriptiveName
 *    Descriptive name of macro command
 * @param objectToolTip
 *    ToolTip for object
 * @param delayInSeconds
 *    Delay in seconds
 * @param errorMessageOut
 *    Output with error messag if new instance fails
 * @return
 *    The widget command or NULL if failure
 */
WuQMacroCommand*
WuQMacroCommand::newInstanceWidgetCommand(const WuQMacroWidgetTypeEnum::Enum widgetType,
                                                 const int32_t version,
                                                 const QString& objectName,
                                                 const QString& objectDescriptiveName,
                                          const QString& objectToolTip,
                                          const float delayInSeconds,
                                          QString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (widgetType == WuQMacroWidgetTypeEnum::INVALID) {
        errorMessageOut = "Widget type is invalid for Widget Macro Command";
        return NULL;
    }
    
    const QString customOperationName("");
    WuQMacroMouseEventInfo* mouseEventInfo(NULL);
    WuQMacroCommand* mc = new WuQMacroCommand(WuQMacroCommandTypeEnum::WIDGET,
                                              customOperationName,
                                              mouseEventInfo,
                                              widgetType,
                                              version,
                                              objectName,
                                              objectDescriptiveName,
                                              objectToolTip,
                                              delayInSeconds);
    return mc;
}

/**
 * Constructor for a macro command
 *
 * @param commandType
 *    Type of command
 * @param customOperationTypeName
 *    Name of a custom operation
 * @param mouseEventInfo
 *    Information about mouse event.
 * @param widgetType
 *    Type of widget
 * @param version
 *    Version of command
 * @param objectName
 *    Name of object
 * @param objectDescriptiveName
 *    Descriptive name of macro command
 * @param objectToolTip
 *    ToolTip for object
 * @param delayInSeconds
 *    Delay in seconds
 */
WuQMacroCommand::WuQMacroCommand(const WuQMacroCommandTypeEnum::Enum commandType,
                                 const QString& customOperationTypeName,
                                 WuQMacroMouseEventInfo* mouseEventInfo,
                                 const WuQMacroWidgetTypeEnum::Enum widgetType,
                                 const int32_t version,
                                 const QString& objectName,
                                 const QString& objectDescriptiveName,
                                 const QString& objectToolTip,
                                 const float delayInSeconds)
: QStandardItem(),
TracksModificationInterface(),
m_commandType(commandType),
m_customOperationTypeName(customOperationTypeName),
m_macroMouseEvent(mouseEventInfo),
m_widgetType(widgetType),
m_version(version),
m_objectName(objectName),
m_descriptiveName(objectDescriptiveName),
m_delayInSeconds(delayInSeconds)
{
    if (objectDescriptiveName.isEmpty()) {
        CaretLogWarning("Empty descriptive name for "
                        + objectName);
    }
    
    switch (m_commandType) {
        case WuQMacroCommandTypeEnum::CUSTOM_OPERATION:
            break;
        case WuQMacroCommandTypeEnum::MOUSE:
            CaretAssert(m_macroMouseEvent);
            setText("Mouse");
            break;
        case WuQMacroCommandTypeEnum::WIDGET:
            break;
    }
    
    setFlags(Qt::ItemIsEnabled
             | Qt::ItemIsSelectable);
    setToolTip(objectToolTip);
    
    updateTitle();
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
    removeAllParameters();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacroCommand::WuQMacroCommand(const WuQMacroCommand& obj)
: QStandardItem(obj),
TracksModificationInterface()
{
    setFlags(Qt::ItemIsEnabled
             | Qt::ItemIsSelectable);
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
        QStandardItem::operator=(obj);
        this->copyHelperWuQMacroCommand(obj);
    }
    return *this;
}

/**
 * @return The type of this item for Qt
 */
int
WuQMacroCommand::type() const
{
    /*
     * This must be different than the type returned by of macro
     * subclasses of QStandardItem
     */
    return WuQMacroStandardItemTypeEnum::toIntegerCode(WuQMacroStandardItemTypeEnum::MACRO_COMMAND);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
WuQMacroCommand::copyHelperWuQMacroCommand(const WuQMacroCommand& obj)
{
    m_commandType = obj.m_commandType;
    m_customOperationTypeName = obj.m_customOperationTypeName;
    if (m_macroMouseEvent != NULL) {
        m_macroMouseEvent = NULL;
    }
    if (obj.m_macroMouseEvent != NULL) {
        m_macroMouseEvent = new WuQMacroMouseEventInfo(*obj.m_macroMouseEvent);
    }
    m_widgetType    = obj.m_widgetType;
    m_version       = obj.m_version;
    m_objectName   = obj.m_objectName;
    m_descriptiveName = obj.m_descriptiveName;
    m_delayInSeconds = obj.m_delayInSeconds;
    removeAllParameters();
    for (const auto p : obj.m_parameters) {
        m_parameters.push_back(new WuQMacroCommandParameter(*p));
    }
    setText(obj.text());
    updateTitle();
    setModified();
}

/**
 * Update the title for this command
 */
void
WuQMacroCommand::updateTitle()
{
    QString title = "Unknown";
    
    QVariant dataValue;
    if (getNumberOfParameters() > 0) {
        CaretAssertVectorIndex(m_parameters, 0);
        dataValue = m_parameters[0]->getValue();
    }
    
    QVariant dataValueTwo;
    if (getNumberOfParameters() > 1) {
        CaretAssertVectorIndex(m_parameters, 1);
        dataValueTwo = m_parameters[1]->getValue();
    }
    
    switch (m_widgetType) {
        case WuQMacroWidgetTypeEnum::ACTION:
            title = ("Turn "
                     + QString((dataValue.toBool() ? "On" : "Off")));
            break;
        case WuQMacroWidgetTypeEnum::ACTION_CHECKABLE:
            title = ("Turn "
                     + QString((dataValue.toBool() ? "On" : "Off")));
            break;
        case WuQMacroWidgetTypeEnum::ACTION_GROUP:
            title = ("Select Name "
                     + dataValue.toString()
                     + " else "
                     + " index "
                     + QString::number(dataValueTwo.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::BUTTON_GROUP:
            title = ("Select Name "
                     + dataValue.toString()
                     + " else "
                     + " index "
                     + QString::number(dataValueTwo.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::CHECK_BOX:
            title = ("Turn "
                     + QString((dataValue.toBool() ? "On" : "Off")));
            break;
        case WuQMacroWidgetTypeEnum::COMBO_BOX:
            title = ("Select Name "
                     + dataValue.toString()
                     + " else "
                     + " index "
                     + QString::number(dataValueTwo.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::DOUBLE_SPIN_BOX:
            title = ("Set to value "
                     + QString::number(dataValue.toFloat()));
            break;
        case WuQMacroWidgetTypeEnum::INVALID:
            break;
        case WuQMacroWidgetTypeEnum::LINE_EDIT:
            title = ("Set to text "
                     + dataValue.toString());
            break;
        case WuQMacroWidgetTypeEnum::LIST_WIDGET:
            title = ("Select Name "
                     + dataValue.toString()
                     + " else "
                     + " index "
                     + QString::number(dataValueTwo.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::MENU:
            title = ("Select Name "
                     + dataValue.toString()
                     + " else "
                     + " index "
                     + QString::number(dataValueTwo.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::PUSH_BUTTON:
            title = ("Click Button");
            break;
        case WuQMacroWidgetTypeEnum::PUSH_BUTTON_CHECKABLE:
            title = ("Turn "
                     + QString((dataValue.toBool() ? "On" : "Off")));
            break;
        case WuQMacroWidgetTypeEnum::RADIO_BUTTON:
            title = "Click Button";
            break;
        case WuQMacroWidgetTypeEnum::SLIDER:
            title = ("Move to "
                     + AString::number(dataValue.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::SPIN_BOX:
            title = ("Set to "
                     + AString::number(dataValue.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::TAB_BAR:
            title = ("Select Name "
                     + dataValue.toString()
                     + " else "
                     + " index "
                     + QString::number(dataValueTwo.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::TAB_WIDGET:
            title = ("Select Name "
                     + dataValue.toString()
                     + " else "
                     + " index "
                     + QString::number(dataValueTwo.toInt()));
            break;
        case WuQMacroWidgetTypeEnum::TOOL_BUTTON:
            title = ("Click Button");
            break;
        case WuQMacroWidgetTypeEnum::TOOL_BUTTON_CHECKABLE:
            title = ("Turn "
                     + QString((dataValue.toBool() ? "On" : "Off")));
            break;
    }
    
    if ( ! m_descriptiveName.isEmpty()) {
        setText(m_descriptiveName);
    }
    else {
        setText(title
                + " "
                + m_objectName);
    }
}

/**
 * @return The command type
 */
WuQMacroCommandTypeEnum::Enum
WuQMacroCommand::getCommandType() const
{
    return m_commandType;
}

/**
 * @return The command's widget type
 */
WuQMacroWidgetTypeEnum::Enum
WuQMacroCommand::getWidgetType() const
{
    return m_widgetType;
}

/**
 * @return Version of the command
 */
int32_t
WuQMacroCommand::getVersion() const
{
    return m_version;
}

/**
 * Add a parameter to the macro command
 *
 * @param dataType
 *     Data type of the parameter
 * @param name
 *     Name of the parameter
 * @param value
 *     Value of the parameter
 */
void
WuQMacroCommand::addParameter(const WuQMacroDataValueTypeEnum::Enum dataType,
                              const QString& name,
                              const QVariant& value)
{
    WuQMacroCommandParameter* parameter = new WuQMacroCommandParameter(dataType,
                                                                       name,
                                                                       value);
    addParameter(parameter);
}

/**
 * Add a parameter to the macro command
 *
 * @param parameter
 *     Parameter to add
 */
void
WuQMacroCommand::addParameter(WuQMacroCommandParameter* parameter)
{
    CaretAssert(parameter);
    if (parameter == NULL) {
        return;
    }
    
    m_parameters.push_back(parameter);
    setModified();
}


/**
 * @return The number of parameters
 */
int32_t
WuQMacroCommand::getNumberOfParameters() const
{
    return m_parameters.size();
}

/**
 * @return Parameter at the given index
 *
 * @param index
 *     Index of the parameter.
 */
WuQMacroCommandParameter*
WuQMacroCommand::getParameterAtIndex(const int32_t index)
{
    CaretAssertVectorIndex(m_parameters, index);
    return m_parameters[index];
}

/**
 * @return Parameter at the given index (const method)
 *
 * @param index
 *     Index of the parameter.
 */
const WuQMacroCommandParameter*
WuQMacroCommand::getParameterAtIndex(const int32_t index) const
{
    CaretAssertVectorIndex(m_parameters, index);
    return m_parameters[index];
}

/**
 * Get the index of the given parameter in this macro
 *
 * @param parameter
 *     Paramater for which index is requested
 * @return
 *     Index of parameter or -1 If not found
 */
int32_t
WuQMacroCommand::getIndexOfParameter(const WuQMacroCommandParameter* parameter) const
{
    CaretAssert(parameter);
    
    const int32_t numParams = getNumberOfParameters();
    for (int32_t i = 0; i < numParams; i++) {
        CaretAssertVectorIndex(m_parameters, i);
        if (m_parameters[i] == parameter) {
            return i;
        }
    }
    
    return -1;
}

/**
 * Remove all parameters in this command
 */
void
WuQMacroCommand::removeAllParameters()
{
    for (auto p : m_parameters) {
        delete p;
    }
    m_parameters.clear();
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
 * @return The object's descriptive name
 */
QString
WuQMacroCommand::getDescriptiveName() const
{
    return m_descriptiveName;
}

/**
 * @return ToolTip for the object
 */
QString
WuQMacroCommand::getObjectToolTip() const
{
    return toolTip();
}

/**
 * Set the object's tooltip
 *
 * @param objectToolTip
 *     New value for object tooltip
 */
void
WuQMacroCommand::setObjectToolTip(const QString& objectToolTip)
{
    if (toolTip() != objectToolTip) {
        setToolTip(objectToolTip);
        setModified();
    }
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
 * @return Point to mouse event information
 */
WuQMacroMouseEventInfo*
WuQMacroCommand::getMouseEventInfo()
{
    return m_macroMouseEvent;
}

/**
 * Set the mouse event info
 *
 * @param mouseEventInfo
 *     The new mouse event info
 */
void
WuQMacroCommand::setMouseEventInfo(WuQMacroMouseEventInfo* mouseEventInfo)
{
    if (m_macroMouseEvent != NULL) {
        delete m_macroMouseEvent;
        m_macroMouseEvent = NULL;
    }
    
    m_macroMouseEvent = mouseEventInfo;
    
    if (m_macroMouseEvent != NULL) {
        QString title;
        
        switch (m_macroMouseEvent->getMouseEventType()) {
            case WuQMacroMouseEventTypeEnum::BUTTON_PRESS:
                title = "Mouse Press ";
                break;
            case WuQMacroMouseEventTypeEnum::BUTTON_RELEASE:
                title = "Mouse Release ";
                break;
            case WuQMacroMouseEventTypeEnum::DOUBLE_CLICKED:
                title = "Mouse Double Click ";
                break;
            case WuQMacroMouseEventTypeEnum::MOVE:
                title = "Mouse Move ";
                break;
        }
        
        title.append(m_objectName);
        setText(title);
    }
}

/**
 * @return True if this command the same mouse event type as the given command.
 *         Must be mouse move events only.
 *
 * @param command
 *     The other command
 */
bool
WuQMacroCommand::isMouseEventMatch(const WuQMacroCommand* command) const
{
    if (command->getCommandType() == WuQMacroCommandTypeEnum::MOUSE) {
        if (getCommandType() == WuQMacroCommandTypeEnum::MOUSE) {
            const WuQMacroMouseEventInfo* myMouse = getMouseEventInfo();
            CaretAssert(myMouse);
            const WuQMacroMouseEventInfo* otherMouse = command->getMouseEventInfo();
            CaretAssert(otherMouse);
            if ((myMouse->getMouseEventType() == WuQMacroMouseEventTypeEnum::MOVE)
                && (myMouse->getMouseEventType() == WuQMacroMouseEventTypeEnum::MOVE)) {
                if ((myMouse->getMouseButton() == otherMouse->getMouseButton())
                    && (myMouse->getMouseButtonsMask() == otherMouse->getMouseButtonsMask())
                    && (myMouse->getKeyboardModifiersMask() == otherMouse->getKeyboardModifiersMask())
                    && (myMouse->getWidgetWidth() == otherMouse->getWidgetWidth())
                    && (myMouse->getWidgetHeight() == otherMouse->getWidgetHeight())) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

/**
 * @return Delay in seconds
 */
float
WuQMacroCommand::getDelayInSeconds() const
{
    return m_delayInSeconds;
}

/**
 * Set delay in seconds
 *
 * @param seconds
 *     New delay value
 */
void
WuQMacroCommand::setDelayInSeconds(const float seconds)
{
    if (seconds != m_delayInSeconds) {
        m_delayInSeconds = seconds;
        setModified();
    }
}

/**
 * @return The custom operation command type name
 * Used when class type is WuQMacroWidgetTypeEnum::CUSTOM_OPERATION
 */
QString
WuQMacroCommand::getCustomOperationTypeName() const
{
    return m_customOperationTypeName;
}

/**
 * Set the custom operation command type name
 * Used when class type is WuQMacroWidgetTypeEnum::CUSTOM_OPERATION
 *
 * @param customOperationCommandTypeName
 *    New value
 */
void
WuQMacroCommand::setCustomOperationTypeName(const QString& customOperationTypeName)
{
    if (m_customOperationTypeName != customOperationTypeName) {
        m_customOperationTypeName = customOperationTypeName;
        setModified();
    }
}

/**
 * @return True if this instance is modified
 */
bool
WuQMacroCommand::isModified() const
{
    if (m_modifiedStatusFlag) {
        return true;
    }
    
    for (const auto p : m_parameters) {
        if (p->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modified status
 */
void
WuQMacroCommand::clearModified()
{
    m_modifiedStatusFlag = false;
    
    for (auto p : m_parameters) {
        p->clearModified();
    }
}

/**
 * Set the modification status to modified
 */
void
WuQMacroCommand::setModified()
{
    m_modifiedStatusFlag = true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
WuQMacroCommand::toString() const
{
    QString s("WuQMacroCommand text=%1 name=%2, commandType=%3, widgetType=%4");
    s = s.arg(text()
              ).arg(m_objectName
                    ).arg(WuQMacroCommandTypeEnum::toName(m_commandType)
                          ).arg(WuQMacroWidgetTypeEnum::toName(m_widgetType));
    
    for (const auto p : m_parameters) {
        s.append(", value=" + p->getValue().toString());
    }
    return s;
}

