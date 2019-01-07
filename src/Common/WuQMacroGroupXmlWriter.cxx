
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

#define __WU_Q_MACRO_GROUP_XML_WRITER_DECLARE__
#include "WuQMacroGroupXmlWriter.h"
#undef __WU_Q_MACRO_GROUP_XML_WRITER_DECLARE__

#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroGroup.h"
#include "WuQMacroMouseEventInfo.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroGroupXmlWriter 
 * \brief Writes contents of macro group to XML format
 * \ingroup Common
 */

/**
 * Constructor
 */
WuQMacroGroupXmlWriter::WuQMacroGroupXmlWriter()
: WuQMacroGroupXmlFormatBase()
{
}

/**
 * Destructor.
 */
WuQMacroGroupXmlWriter::~WuQMacroGroupXmlWriter()
{
}

/**
 * Constructor for writing the macro group to a text string
 *
 * @param macroGroup
 *     Macro group that is written to XML
 * @param contentTextString
 *     Pointer to string to which XML is written
 */
void
WuQMacroGroupXmlWriter::writeToString(const WuQMacroGroup* macroGroup,
                                      QString& contentTextString)
{
    CaretAssert(macroGroup);
    contentTextString.clear();
    
    m_xmlStreamWriter.reset(new QXmlStreamWriter(&contentTextString));
    m_xmlStreamWriter->setAutoFormatting(true);
    m_xmlStreamWriter->writeStartElement(ELEMENT_MACRO_GROUP);
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_NAME, macroGroup->getName());
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_VERSION, "1");
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_UNIQUE_IDENTIFIER,
                                      macroGroup->getUniqueIdentifier());
    
    const int32_t numMacros = macroGroup->getNumberOfMacros();
    for (int32_t i = 0; i < numMacros; i++) {
        const WuQMacro* macro = macroGroup->getMacroAtIndex(i);
        CaretAssert(macro);
        writeMacroToXML(macro);
    }
    m_xmlStreamWriter->writeEndElement();
    
    m_xmlStreamWriter.reset();
}


/**
 * Write a macro to XML format
 * 
 * @param macro
 *     The macro
 */
void
WuQMacroGroupXmlWriter::writeMacroToXML(const WuQMacro* macro)
{
    CaretAssert(macro);
    
    m_xmlStreamWriter->writeStartElement(ELEMENT_MACRO);
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_NAME,
                                      macro->getName());
    const QString shortCutText = WuQMacroShortCutKeyEnum::toName(macro->getShortCutKey());
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_SHORT_CUT_KEY,
                                      shortCutText);
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_UNIQUE_IDENTIFIER,
                                      macro->getUniqueIdentifier());
    m_xmlStreamWriter->writeTextElement(ELEMENT_DESCRIPTION,
                                        macro->getDescription());
    
    const int32_t numCommands = macro->getNumberOfMacroCommands();
    for (int32_t i = 0; i < numCommands; i++) {
        const WuQMacroCommand* macroCommand = macro->getMacroCommandAtIndex(i);
        CaretAssert(macroCommand);
        writeMacroCommandToXML(macroCommand);
    }
    
    m_xmlStreamWriter->writeEndElement();
}

/**
 * Write a macro command to XML format
 *
 * @param macroCommand
 *     The macro command
 */
void
WuQMacroGroupXmlWriter::writeMacroCommandToXML(const WuQMacroCommand* macroCommand)
{
    CaretAssert(macroCommand);
    
    m_xmlStreamWriter->writeStartElement(ELEMENT_MACRO_COMMAND);
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_NAME,
                                      macroCommand->getObjectName());
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_OBJECT_CLASS,
                                      WuQMacroClassTypeEnum::toName(macroCommand->getClassType()));
    {
        m_xmlStreamWriter->writeAttribute(ATTRIBUTE_OBJECT_DATA_TYPE,
                                          WuQMacroDataValueTypeEnum::toName(macroCommand->getDataType()));
        
        const QVariant dataValue(macroCommand->getDataValue());
        QString stringValue;
        switch (macroCommand->getDataType()) {
            case WuQMacroDataValueTypeEnum::BOOLEAN:
                stringValue = (dataValue.toBool() ? VALUE_BOOL_TRUE : VALUE_BOOL_FALSE);
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
                stringValue = QString::number(dataValue.toFloat());
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
                stringValue = QString::number(dataValue.toInt());
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                stringValue = "MouseEvent";
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                stringValue = dataValue.toString();
                break;
        }
        m_xmlStreamWriter->writeAttribute(ATTRIBUTE_OBJECT_VALUE,
                                          stringValue);
    }
    
    {
        m_xmlStreamWriter->writeAttribute(ATTRIBUTE_OBJECT_DATA_TYPE_TWO,
                                          WuQMacroDataValueTypeEnum::toName(macroCommand->getDataTypeTwo()));
        
        const QVariant dataValueTwo(macroCommand->getDataValueTwo());
        QString stringValue;
        switch (macroCommand->getDataTypeTwo()) {
            case WuQMacroDataValueTypeEnum::BOOLEAN:
                stringValue = (dataValueTwo.toBool() ? VALUE_BOOL_TRUE : VALUE_BOOL_FALSE);
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
                stringValue = QString::number(dataValueTwo.toFloat());
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
                stringValue = QString::number(dataValueTwo.toInt());
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                stringValue = "MouseEvent";
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                stringValue = dataValueTwo.toString();
                break;
        }
        m_xmlStreamWriter->writeAttribute(ATTRIBUTE_OBJECT_VALUE_TWO,
                                          stringValue);
    }
    
    
    switch (macroCommand->getClassType()) {
        case WuQMacroClassTypeEnum::ACTION:
            break;
        case WuQMacroClassTypeEnum::ACTION_GROUP:
            break;
        case WuQMacroClassTypeEnum::BUTTON_GROUP:
            break;
        case WuQMacroClassTypeEnum::CHECK_BOX:
            break;
        case WuQMacroClassTypeEnum::COMBO_BOX:
            break;
        case WuQMacroClassTypeEnum::DOUBLE_SPIN_BOX:
            break;
        case WuQMacroClassTypeEnum::INVALID:
            break;
        case WuQMacroClassTypeEnum::LINE_EDIT:
            break;
        case WuQMacroClassTypeEnum::LIST_WIDGET:
            break;
        case WuQMacroClassTypeEnum::MENU:
            break;
        case WuQMacroClassTypeEnum::MOUSE_USER_EVENT:
            writeMacroMouseEventInfo(macroCommand->getMouseEventInfo());
            break;
        case WuQMacroClassTypeEnum::PUSH_BUTTON:
            break;
        case WuQMacroClassTypeEnum::RADIO_BUTTON:
            break;
        case WuQMacroClassTypeEnum::SLIDER:
            break;
        case WuQMacroClassTypeEnum::SPIN_BOX:
            break;
        case WuQMacroClassTypeEnum::TAB_BAR:
            break;
        case WuQMacroClassTypeEnum::TAB_WIDGET:
            break;
        case WuQMacroClassTypeEnum::TOOL_BUTTON:
            break;
    }
    
    const QString toolTip = macroCommand->getObjectToolTip();
    if (toolTip != NULL) {
        m_xmlStreamWriter->writeTextElement(ELEMENT_TOOL_TIP,
                                            toolTip);
    }
    m_xmlStreamWriter->writeEndElement();
}

/**
 * Write a macro mouse event info to XML format
 *
 * @param mouseEventInfo
 *     The macro mouse event info
 */
void
WuQMacroGroupXmlWriter::writeMacroMouseEventInfo(const WuQMacroMouseEventInfo* mouseEventInfo)
{
    CaretAssert(mouseEventInfo);
    
    m_xmlStreamWriter->writeStartElement(ELEMENT_MOUSE_EVENT_INFO);
    
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_BUTTON, QString::number(mouseEventInfo->getMouseButton()));
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_BUTTONS_MASK, QString::number(mouseEventInfo->getMouseButtonsMask()));
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_EVENT_TYPE, WuQMacroMouseEventTypeEnum::toName(mouseEventInfo->getMouseEventType()));
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_KEYBOARD_MODIFIERS_MASK, QString::number(mouseEventInfo->getKeyboardModifiersMask()));
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_LOCAL_X, QString::number(mouseEventInfo->getLocalX()));
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_LOCAL_Y, QString::number(mouseEventInfo->getLocalY()));
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_WIDGET_WIDTH, QString::number(mouseEventInfo->getWidgetWidth()));
    m_xmlStreamWriter->writeAttribute(ATTRIBUTE_MOUSE_WIDGET_HEIGHT, QString::number(mouseEventInfo->getWidgetHeight()));
    
    m_xmlStreamWriter->writeEndElement();
}

