
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

#define __WU_Q_MACRO_GROUP_XML_READER_DECLARE__
#include "WuQMacroGroupXmlReader.h"
#undef __WU_Q_MACRO_GROUP_XML_READER_DECLARE__

#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

#include "CaretAssert.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroGroup.h"
#include "WuQMacroMouseEventInfo.h"

using namespace caret;



/**
 * \class caret::WuQMacroGroupXmlReader
 * \brief Reads macro group from XML format
 * \ingroup Common
 */

/**
 * Constructor.
 */
WuQMacroGroupXmlReader::WuQMacroGroupXmlReader()
: WuQMacroGroupXmlFormatBase()
{
}

/**
 * Destructor.
 */
WuQMacroGroupXmlReader::~WuQMacroGroupXmlReader()
{
}

/**
 * Read XML from the given string into the given macro group
 *
 * @param xmlString
 *    The string containing XML
 * @param macroGroup
 *    The macro group
 */
void
WuQMacroGroupXmlReader::readFromString(const QString& xmlString,
                                       WuQMacroGroup* macroGroup)
{
    CaretAssert(macroGroup);
    macroGroup->clear();
    
    if (xmlString.isEmpty()) {
        m_xmlStreamReader->raiseError("String that should contain XML is empty.");
        return;
    }
    
    m_xmlStreamReader.reset(new QXmlStreamReader(xmlString));
    
    if (m_xmlStreamReader->atEnd()) {
        m_xmlStreamReader->raiseError("At end when trying to start reading.  Appears to have no XML content.");
    }
    else {
        if (m_xmlStreamReader->readNextStartElement()) {
            const QStringRef groupElement = m_xmlStreamReader->name();
            if (groupElement == ELEMENT_MACRO_GROUP) {
                const QXmlStreamAttributes attributes = m_xmlStreamReader->attributes();
                const QStringRef name = attributes.value(ATTRIBUTE_NAME);
                const QStringRef versionText = attributes.value(ATTRIBUTE_VERSION);
                if (versionText.isEmpty()) {
                    m_xmlStreamReader->raiseError(ATTRIBUTE_VERSION
                                                  + " is missing from element "
                                                  + ELEMENT_MACRO_GROUP);
                }
                else if (versionText == VALUE_VERSION_ONE) {
                    macroGroup->setName(name.toString());
                    readVersionOne(macroGroup);
                }
                else {
                    m_xmlStreamReader->raiseError(ATTRIBUTE_VERSION
                                                  + "="
                                                  + versionText.toString()
                                                  + " is not supported by "
                                                  + ELEMENT_MACRO_GROUP
                                                  + ".  Check for software update.");
                }
            }
            else {
                m_xmlStreamReader->raiseError("First XML element is \""
                                              + m_xmlStreamReader->name().toString()
                                              + "\" but should be \""
                                              + ELEMENT_MACRO_GROUP
                                              + "\"");
            }
        }
        else {
            m_xmlStreamReader->raiseError("No XML elements found");
        }
    }
    
    if (m_xmlStreamReader->hasError()) {
        macroGroup->clear();
    }
}

/**
 * Read version one of macro group
 *
 * @param macroGroup
 *    The macro group
 */
void
WuQMacroGroupXmlReader::readVersionOne(WuQMacroGroup* macroGroup)
{
    CaretAssert(macroGroup);
    
    WuQMacro* macro(NULL);
    
    while ( ! m_xmlStreamReader->atEnd()) {
        m_xmlStreamReader->readNext();
        if (m_xmlStreamReader->isStartElement()) {
            const QString elementName = m_xmlStreamReader->name().toString();
        
            if (elementName == ELEMENT_MACRO) {
                macro = readMacroVersionOne();
                if (macro != NULL) {
                    macroGroup->addMacro(macro);
                }
            }
            else if (elementName == ELEMENT_DESCRIPTION) {
                const QString text = m_xmlStreamReader->readElementText();
                if (macro != NULL) {
                    macro->setDescription(text);
                }
            }
            else if (elementName == ELEMENT_MACRO_COMMAND) {
                WuQMacroCommand* macroCommand = readMacroCommandVersionOne();
                if (macroCommand != NULL) {
                    if (macro != NULL) {
                        macro->addMacroCommand(macroCommand);
                    }
                    else {
                        delete macroCommand;
                        m_xmlStreamReader->skipCurrentElement();
                    }
                }
            }
            else if (elementName == ELEMENT_MOUSE_EVENT_INFO) {
                CaretAssert(0);
            }
            else {
                addToWarnings("Unexpected element="
                              + elementName
                              + "\"");
                m_xmlStreamReader->skipCurrentElement();
            }
        }
        
        if (m_xmlStreamReader->hasError()) {
            break;
        }
    }
}

/**
 * Read version one of macro
 *
 * @return The macro
 */
WuQMacro*
WuQMacroGroupXmlReader::readMacroVersionOne()
{
    WuQMacro* macro(NULL);
    
    const QXmlStreamAttributes attributes = m_xmlStreamReader->attributes();
    QString macroName = attributes.value(ATTRIBUTE_NAME).toString();
    QString shortCutKeyString = attributes.value(ATTRIBUTE_SHORT_CUT_KEY).toString();
    if (shortCutKeyString.isEmpty()) {
        shortCutKeyString = WuQMacroShortCutKeyEnum::toName(WuQMacroShortCutKeyEnum::Key_None);
        addToWarnings(ELEMENT_MACRO
                      + " is missing attribute or value is empty: "
                      + ATTRIBUTE_SHORT_CUT_KEY);
    }
    
    bool validShortCutKey(false);
    WuQMacroShortCutKeyEnum::Enum shortCutKey = WuQMacroShortCutKeyEnum::fromName(shortCutKeyString,
                                                                                  &validShortCutKey);
    if ( ! validShortCutKey) {
        shortCutKey = WuQMacroShortCutKeyEnum::Key_None;
        addToWarnings(ELEMENT_MACRO
                      + " attribute "
                      + ATTRIBUTE_SHORT_CUT_KEY
                      + " has invalid value "
                      + shortCutKeyString);
    }

    
    if (macroName.isEmpty()) {
        addToWarnings(ELEMENT_MACRO
                      + " is missing attribute or value is empty: "
                      + ATTRIBUTE_NAME);
        static uint32_t missingCounter = 1;
        macroName = ("Missing Name_"
                     + QString::number(missingCounter));
    }
    
    macro = new WuQMacro();
    macro->setName(macroName);
    macro->setShortCutKey(shortCutKey);
    
    return macro;
}

/**
 * Read version one of macro command
 *
 * @Return The macro group
 */
WuQMacroCommand*
WuQMacroGroupXmlReader::readMacroCommandVersionOne()
{
    WuQMacroCommand* macroCommand(NULL);
    
    const QXmlStreamAttributes attributes = m_xmlStreamReader->attributes();
    const QStringRef objectName = attributes.value(ATTRIBUTE_NAME);
    const QStringRef dataTypeString = attributes.value(ATTRIBUTE_OBJECT_DATA_TYPE);
    const QStringRef classString = attributes.value(ATTRIBUTE_OBJECT_CLASS);
    const QStringRef valueString = attributes.value(ATTRIBUTE_OBJECT_VALUE);
    const QStringRef dataTypeTwoString = attributes.value(ATTRIBUTE_OBJECT_DATA_TYPE_TWO);
    const QStringRef valueTwoString = attributes.value(ATTRIBUTE_OBJECT_VALUE_TWO);
    
    QString es;
    if (objectName.isEmpty()) es.append(ATTRIBUTE_NAME + " ");
    if (dataTypeString.isEmpty()) es.append(ATTRIBUTE_OBJECT_DATA_TYPE + " ");
    if (dataTypeTwoString.isEmpty()) es.append(ATTRIBUTE_OBJECT_DATA_TYPE + " ");
    if (classString.isEmpty()) es.append(ATTRIBUTE_OBJECT_CLASS + " ");
// empty could be valid for line edit    if (valueString.isEmpty()) es.append(ATTRIBUTE_OBJECT_VALUE + " ");
    if ( ! es.isEmpty()) {
        addToWarnings(ELEMENT_MACRO_COMMAND
                      + " is missing attribute(s): "
                      + es);
        return NULL;
    }
    
    
    bool objectTypeValid(false);
    WuQMacroClassTypeEnum::Enum objectClass = WuQMacroClassTypeEnum::fromName(classString.toString(),
                                                                               &objectTypeValid);
    if (! objectTypeValid) {
        es.append(classString.toString()
                  + " is not valid for attribute "
                  + ATTRIBUTE_OBJECT_CLASS
                  + " ");
        return NULL;
    }
    
    bool dataTypeValid(false);
    WuQMacroDataValueTypeEnum::Enum dataValueType = WuQMacroDataValueTypeEnum::fromName(dataTypeString.toString(),
                                                                                        &dataTypeValid);
    if (! dataTypeValid) {
        es.append(dataTypeString.toString()
                  + " is not valid for attribute "
                  + ATTRIBUTE_OBJECT_DATA_TYPE
                  + " ");
        return NULL;
    }
    
    bool dataTypeTwoValid(false);
    WuQMacroDataValueTypeEnum::Enum dataValueTypeTwo = WuQMacroDataValueTypeEnum::fromName(dataTypeTwoString.toString(),
                                                                                           &dataTypeTwoValid);
    if (! dataTypeTwoValid) {
        es.append(dataTypeTwoString.toString()
                  + " is not valid for attribute "
                  + ATTRIBUTE_OBJECT_DATA_TYPE_TWO
                  + " ");
        return NULL;
    }
    
    
    if ( ! es.isEmpty()) {
        addToWarnings(ELEMENT_MACRO_COMMAND
                      + " has invalid attributes: "
                      + es);
        return NULL;
    }
    
    if (objectClass == WuQMacroClassTypeEnum::MOUSE_USER_EVENT) {
        WuQMacroMouseEventInfo* mouseEventInfo = readMacroMouseEventInfo();
        if (mouseEventInfo != NULL) {
            macroCommand = new WuQMacroCommand(objectName.toString(),
                                               mouseEventInfo);
        }
    }
    else {
        QVariant value;
        switch (dataValueType) {
            case WuQMacroDataValueTypeEnum::BOOLEAN:
            {
                const bool boolValue = ((valueString == VALUE_BOOL_TRUE) ? true : false);
                value.setValue(boolValue);
            }
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
            {
                const double floatValue = valueString.toFloat();
                value.setValue(floatValue);
            }
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
            {
                const int32_t intValue = valueString.toInt();
                value.setValue(intValue);
            }
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                CaretAssertMessage(0, "Mouse is special case handled above");
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                value.setValue(valueString.toString());
                break;
        }
        
        QVariant valueTwo;
        switch (dataValueTypeTwo) {
            case WuQMacroDataValueTypeEnum::BOOLEAN:
            {
                const bool boolValue = ((valueTwoString == VALUE_BOOL_TRUE) ? true : false);
                valueTwo.setValue(boolValue);
            }
                break;
            case WuQMacroDataValueTypeEnum::FLOAT:
            {
                const double floatValue = valueTwoString.toFloat();
                valueTwo.setValue(floatValue);
            }
                break;
            case WuQMacroDataValueTypeEnum::INTEGER:
            {
                const int32_t intValue = valueTwoString.toInt();
                valueTwo.setValue(intValue);
            }
                break;
            case WuQMacroDataValueTypeEnum::MOUSE:
                CaretAssertMessage(0, "Mouse is special case handled above");
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                valueTwo.setValue(valueTwoString.toString());
                break;
        }
        
        macroCommand = new WuQMacroCommand(objectClass,
                                           objectName.toString(),
                                           value,
                                           valueTwo);
    }
    
    return macroCommand;
}

/**
 * @return Read and return the mouse event information
 */
WuQMacroMouseEventInfo*
WuQMacroGroupXmlReader::readMacroMouseEventInfo()
{
    if ( ! m_xmlStreamReader->readNextStartElement()) {
        addToWarnings("Failed to read start element for element "
                      + ELEMENT_MOUSE_EVENT_INFO);
        return NULL;
    }
    
    const QXmlStreamAttributes attributes = m_xmlStreamReader->attributes();
    const QString mouseEventTypeString = attributes.value(ATTRIBUTE_MOUSE_EVENT_TYPE).toString();
    const QString xLocalString = attributes.value(ATTRIBUTE_MOUSE_LOCAL_X).toString();
    const QString yLocalString = attributes.value(ATTRIBUTE_MOUSE_LOCAL_Y).toString();
    const QString mouseButtonString = attributes.value(ATTRIBUTE_MOUSE_BUTTON).toString();
    const QString mouseButtonsMaskString = attributes.value(ATTRIBUTE_MOUSE_BUTTONS_MASK).toString();
    const QString keyboardModifiersMaskString = attributes.value(ATTRIBUTE_MOUSE_KEYBOARD_MODIFIERS_MASK).toString();
    const QString widgetWidthString = attributes.value(ATTRIBUTE_MOUSE_WIDGET_WIDTH).toString();
    const QString widgetHeightString = attributes.value(ATTRIBUTE_MOUSE_WIDGET_HEIGHT).toString();
    
    QString es;
    if (mouseEventTypeString.isEmpty()) es.append(ATTRIBUTE_MOUSE_EVENT_TYPE + " ");
    if (xLocalString.isEmpty()) es.append(ATTRIBUTE_MOUSE_LOCAL_X + " ");
    if (yLocalString.isEmpty()) es.append(ATTRIBUTE_MOUSE_LOCAL_Y + " ");
    if (mouseButtonString.isEmpty()) es.append(ATTRIBUTE_MOUSE_BUTTON + " ");
    if (mouseButtonsMaskString.isEmpty()) es.append(ATTRIBUTE_MOUSE_BUTTONS_MASK + " ");
    if (keyboardModifiersMaskString.isEmpty()) es.append(ATTRIBUTE_MOUSE_KEYBOARD_MODIFIERS_MASK + " ");
    if (widgetWidthString.isEmpty()) es.append(ATTRIBUTE_MOUSE_WIDGET_WIDTH + " ");
    if (widgetHeightString.isEmpty()) es.append(ATTRIBUTE_MOUSE_WIDGET_HEIGHT + " ");
    if ( ! es.isEmpty()) {
        addToWarnings(ELEMENT_MOUSE_EVENT_INFO
                      + " is missing attribute(s): "
                      + es);
        return NULL;
    }
    
    bool validMouseEventTypeFlag(false);
    const WuQMacroMouseEventTypeEnum::Enum mouseEventType = WuQMacroMouseEventTypeEnum::fromName(mouseEventTypeString,
                                                                                                 &validMouseEventTypeFlag);
    if ( ! validMouseEventTypeFlag) {
        addToWarnings(mouseEventTypeString
                      + " is not valid for attribute "
                      + ATTRIBUTE_MOUSE_EVENT_TYPE);
        return NULL;
    }
    
    WuQMacroMouseEventInfo* mouseInfo = new WuQMacroMouseEventInfo(mouseEventType,
                                                                   xLocalString.toInt(),
                                                                   yLocalString.toInt(),
                                                                   mouseButtonString.toUInt(),
                                                                   mouseButtonsMaskString.toUInt(),
                                                                   keyboardModifiersMaskString.toUInt(),
                                                                   widgetWidthString.toInt(),
                                                                   widgetHeightString.toInt());
    return mouseInfo;
}

void
WuQMacroGroupXmlReader::addToWarnings(const QString& warning)
{
    if ( ! m_warningMessage.isEmpty()) {
        m_warningMessage.append("\n");
    }
    m_warningMessage.append("Line="
                      + QString::number(m_xmlStreamReader->lineNumber())
                      + ", Column="
                      + QString::number(m_xmlStreamReader->columnNumber())
                      + ": "
                      + warning);
}


/**
 * @return True if there are non-fatal warnings
 */
bool
WuQMacroGroupXmlReader::hasWarnings() const
{
    return ( ! m_warningMessage.isEmpty());
}

/**
 * @return The warning message
 */
QString
WuQMacroGroupXmlReader::getWarningMessage() const
{
    return m_warningMessage;
}

/**
 * @return True if there is an error
 */
bool
WuQMacroGroupXmlReader::hasError() const
{
    if (m_xmlStreamReader != NULL) {
        return m_xmlStreamReader->hasError();
    }
    return false;
}

/**
 * @return The error message
 */
QString
WuQMacroGroupXmlReader::getErrorMessage() const
{
    QString errorMessage;
    
    if (m_xmlStreamReader != NULL) {
        errorMessage.append("Line Number="
                            + QString::number(m_xmlStreamReader->lineNumber())
                            + ", Column Number="
                            + QString::number(m_xmlStreamReader->columnNumber())
                            + ".  ");
        errorMessage.append(m_xmlStreamReader->errorString());
    }
    
    return errorMessage;
}


