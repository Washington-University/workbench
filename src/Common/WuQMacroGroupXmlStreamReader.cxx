
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

#define __WU_Q_MACRO_GROUP_XML_STREAM_READER_DECLARE__
#include "WuQMacroGroupXmlStreamReader.h"
#undef __WU_Q_MACRO_GROUP_XML_STREAM_READER_DECLARE__

#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QTextStream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacro.h"
#include "WuQMacroCommand.h"
#include "WuQMacroGroup.h"
#include "WuQMacroMouseEventInfo.h"

using namespace caret;



/**
 * \class caret::WuQMacroGroupXmlStreamReader
 * \brief Reads macro group from XML format
 * \ingroup Common
 */

/**
 * Constructor.
 */
WuQMacroGroupXmlStreamReader::WuQMacroGroupXmlStreamReader()
: WuQMacroGroupXmlStreamBase()
{
}

/**
 * Destructor.
 */
WuQMacroGroupXmlStreamReader::~WuQMacroGroupXmlStreamReader()
{
}

/**
 * Read XML from the given string into the given macro group
 *
 * @param xmlString
 *    The string containing XML
 * @param macroGroup
 *    The macro group
 * @param errorMessageOut
 *    Output error message
 * @return
 *    True if successful, else false is returned and description in errorMessageOut
 */
bool
WuQMacroGroupXmlStreamReader::readFromString(const QString& xmlString,
                                             WuQMacroGroup* macroGroup,
                                             QString& errorMessageOut)
{
    errorMessageOut.clear();
    CaretAssert(macroGroup);
    macroGroup->clear();
    
    
    if (xmlString.isEmpty()) {
        errorMessageOut = "String that should contain XML is empty.";
        return false;
    }
    
    QXmlStreamReader xmlReader(xmlString);
    
    if (xmlReader.atEnd()) {
        xmlReader.raiseError("At end when trying to start reading.  Appears to have no XML content.");
    }
    else {
        xmlReader.readNextStartElement();
        readMacroGroup(xmlReader,
                       macroGroup);
    }
    
    if (xmlReader.hasError()) {
        errorMessageOut = xmlReader.errorString();
        macroGroup->clear();
        return false;
    }
    
    macroGroup->clearModified();

    return true;
}

/**
 * Read macro group from the given XML stream reader.  It assumes that
 * the start element for the macro group has already been read and is
 * the current element.  If xmlReader.hasError() is set after this
 * method is called, there was an error reading the macro group.
 *
 * @param xmlReader
 *    The XML stream reader
 * @param macroGroup
 *    The macro group
 */
void
WuQMacroGroupXmlStreamReader::readMacroGroup(QXmlStreamReader& xmlReader,
                                             WuQMacroGroup* macroGroup)
{
    if (xmlReader.name() == ELEMENT_MACRO_GROUP) {
        const QXmlStreamAttributes attributes = xmlReader.attributes();
        const QStringRef name = attributes.value(ATTRIBUTE_NAME);
        const QStringRef versionText = attributes.value(ATTRIBUTE_VERSION);
        QString uniqueIdentifier = attributes.value(ATTRIBUTE_UNIQUE_IDENTIFIER).toString();
        if (uniqueIdentifier.isEmpty()) {
            addToWarnings(xmlReader,
                          ELEMENT_MACRO_GROUP
                          + " is missing attribute or value is empty: "
                          + ATTRIBUTE_UNIQUE_IDENTIFIER);
        }
        if (versionText.isEmpty()) {
            xmlReader.raiseError(ATTRIBUTE_VERSION
                                          + " is missing from element "
                                          + ELEMENT_MACRO_GROUP);
        }
        else if (versionText == VALUE_VERSION_ONE) {
            macroGroup->setName(name.toString());
            macroGroup->setUniqueIdentifier(uniqueIdentifier);
            
            readVersionOne(xmlReader,
                           macroGroup);
        }
        else {
            xmlReader.raiseError(ATTRIBUTE_VERSION
                                          + "="
                                          + versionText.toString()
                                          + " is not supported by "
                                          + ELEMENT_MACRO_GROUP
                                          + ".  Check for software update.");
        }
    }
    else {
        xmlReader.raiseError("Element should be \""
                             + ELEMENT_MACRO_GROUP
                             + "\" but is \""
                             + xmlReader.text().toString()
                             + "\" while reading MacroGroup");
    }
    
    if ( ! m_warningMessage.isEmpty()) {
        CaretLogWarning("Reading Macro's Warnings:  "
                        + m_warningMessage);
        m_warningMessage.clear();
    }
}

/**
 * Read version one of macro group
 *
 * @param xmlReader
 *    The XML stream reader
 * @param macroGroup
 *    The macro group
 */
void
WuQMacroGroupXmlStreamReader::readVersionOne(QXmlStreamReader& xmlReader,
                                             WuQMacroGroup* macroGroup)
{
    CaretAssert(macroGroup);
    
    WuQMacro* macro(NULL);
    WuQMacroCommand* macroCommand(NULL);
    
    /*
     * Gets set when ending scene info directory element is read
     */
    bool endElementFound(false);
    
    while ( (! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        if (xmlReader.isStartElement()) {
            const QString elementName = xmlReader.name().toString();
        
            if (elementName == ELEMENT_MACRO) {
                macro = readMacroVersionOne(xmlReader);
            }
            else if (elementName == ELEMENT_DESCRIPTION) {
                const QString text = xmlReader.readElementText();
                if (macro != NULL) {
                    macro->setDescription(text);
                }
            }
            else if (elementName == ELEMENT_MACRO_COMMAND) {
                macroCommand = readMacroCommandAttributesVersionOne(xmlReader);
            }
            else if (elementName == ELEMENT_MOUSE_EVENT_INFO) {
                WuQMacroMouseEventInfo* mouseEventInfo = readMacroMouseEventInfo(xmlReader);
                if (mouseEventInfo != NULL) {
                    if (macroCommand != NULL) {
                        macroCommand->setMouseEventInfo(mouseEventInfo);
                    }
                    else {
                        addToWarnings(xmlReader,
                                      "Read WuQMacroMouseEventInfo but macroCommand is NULL");
                        delete mouseEventInfo;
                        mouseEventInfo = NULL;
                    }
                }
                else {
                    if (macroCommand != NULL) {
                        /*
                         * Command is invalid
                         */
                        delete macroCommand;
                        macroCommand = NULL;
                    }
                }
            }
            else if (elementName == ELEMENT_MACRO_COMMAND_TOOL_TIP) {
                const QString text = xmlReader.readElementText();
                if (macroCommand != NULL) {
                    macroCommand->setToolTip(text);
                }
            }
            else {
                addToWarnings(xmlReader,
                              "Unexpected element="
                              + elementName
                              + "\"");
                xmlReader.skipCurrentElement();
            }
        }
        else if (xmlReader.isEndElement()) {
            const QString elementName = xmlReader.name().toString();
            if (elementName == ELEMENT_MACRO) {
                if (macro != NULL) {
                    macroGroup->addMacro(macro);
                }
            }
            else if (elementName == ELEMENT_MACRO_COMMAND) {
                if (macro != NULL) {
                    if (macroCommand != NULL) {
                        macro->appendMacroCommand(macroCommand);
                    }
                }
                macroCommand = NULL;
            }
            else if (elementName == ELEMENT_MACRO_GROUP) {
                endElementFound = true;
            }
        }
    }
}

/**
 * Read version one of macro
 *
 * @param xmlReader
 *    The XML stream reader
 * @return The macro
 */
WuQMacro*
WuQMacroGroupXmlStreamReader::readMacroVersionOne(QXmlStreamReader& xmlReader)
{
    WuQMacro* macro(NULL);
    
    const QXmlStreamAttributes attributes = xmlReader.attributes();
    QString macroName = attributes.value(ATTRIBUTE_NAME).toString();
    QString shortCutKeyString = attributes.value(ATTRIBUTE_SHORT_CUT_KEY).toString();
    if (shortCutKeyString.isEmpty()) {
        shortCutKeyString = WuQMacroShortCutKeyEnum::toName(WuQMacroShortCutKeyEnum::Key_None);
        addToWarnings(xmlReader,
                      ELEMENT_MACRO
                      + " is missing attribute or value is empty: "
                      + ATTRIBUTE_SHORT_CUT_KEY);
    }
    QString uniqueIdentifier = attributes.value(ATTRIBUTE_UNIQUE_IDENTIFIER).toString();
    if (uniqueIdentifier.isEmpty()) {
        addToWarnings(xmlReader,
                      ELEMENT_MACRO
                      + " is missing attribute or value is empty: "
                      + ATTRIBUTE_UNIQUE_IDENTIFIER);
    }
    
    bool validShortCutKey(false);
    WuQMacroShortCutKeyEnum::Enum shortCutKey = WuQMacroShortCutKeyEnum::fromName(shortCutKeyString,
                                                                                  &validShortCutKey);
    if ( ! validShortCutKey) {
        shortCutKey = WuQMacroShortCutKeyEnum::Key_None;
        addToWarnings(xmlReader,
                      ELEMENT_MACRO
                      + " attribute "
                      + ATTRIBUTE_SHORT_CUT_KEY
                      + " has invalid value "
                      + shortCutKeyString);
    }

    
    if (macroName.isEmpty()) {
        addToWarnings(xmlReader,
                      ELEMENT_MACRO
                      + " is missing attribute or value is empty: "
                      + ATTRIBUTE_NAME);
        static uint32_t missingCounter = 1;
        macroName = ("Missing Name_"
                     + QString::number(missingCounter));
    }
    
    macro = new WuQMacro();
    macro->setName(macroName);
    macro->setShortCutKey(shortCutKey);
    macro->setUniqueIdentifier(uniqueIdentifier);
    
    return macro;
}

/**
 * Read version one of macro command attributes
 *
 * @param xmlReader
 *    The XML stream reader
 * @Return The macro group
 */
WuQMacroCommand*
WuQMacroGroupXmlStreamReader::readMacroCommandAttributesVersionOne(QXmlStreamReader& xmlReader)
{
    WuQMacroCommand* macroCommand(NULL);
    
    const QXmlStreamAttributes attributes = xmlReader.attributes();
    const QStringRef objectName = attributes.value(ATTRIBUTE_NAME);
    const QStringRef dataTypeString = attributes.value(ATTRIBUTE_OBJECT_DATA_TYPE);
    const QStringRef delayString = attributes.value(ATTRIBUTE_DELAY);
    const QStringRef descriptiveNameString = attributes.value(ATTRIBUTE_OBJECT_DESCRIPTIVE_NAME);
    const QStringRef classString = attributes.value(ATTRIBUTE_OBJECT_CLASS);
    const QStringRef valueString = attributes.value(ATTRIBUTE_OBJECT_VALUE);
    const QStringRef dataTypeTwoString = attributes.value(ATTRIBUTE_OBJECT_DATA_TYPE_TWO);
    const QStringRef valueTwoString = attributes.value(ATTRIBUTE_OBJECT_VALUE_TWO);
    
    QString es;
    if (objectName.isEmpty()) es.append(ATTRIBUTE_NAME + " ");
    if (dataTypeString.isEmpty()) es.append(ATTRIBUTE_OBJECT_DATA_TYPE + " ");
    if (dataTypeTwoString.isEmpty()) es.append(ATTRIBUTE_OBJECT_DATA_TYPE + " ");
    if (classString.isEmpty()) es.append(ATTRIBUTE_OBJECT_CLASS + " ");
    if ( ! es.isEmpty()) {
        addToWarnings(xmlReader,
                      ELEMENT_MACRO_COMMAND
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
        addToWarnings(xmlReader,
                      ELEMENT_MACRO_COMMAND
                      + " has invalid attributes: "
                      + es);
        return NULL;
    }
    
    if (objectClass == WuQMacroClassTypeEnum::MOUSE_USER_EVENT) {
        macroCommand = new WuQMacroCommand(WuQMacroClassTypeEnum::MOUSE_USER_EVENT,
                                           objectName.toString(),
                                           descriptiveNameString.toString());
    }
    else {
        QVariant value;
        switch (dataValueType) {
            case WuQMacroDataValueTypeEnum::INVALID:
                value.setValue(QString(""));
                break;
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
            case WuQMacroDataValueTypeEnum::NONE:
                value.setValue(QString());
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                value.setValue(valueString.toString());
                break;
        }
        
        QVariant valueTwo;
        switch (dataValueTypeTwo) {
            case WuQMacroDataValueTypeEnum::INVALID:
                valueTwo.setValue(QString(""));
                break;
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
                break;
            case WuQMacroDataValueTypeEnum::NONE:
                value.setValue(QString());
                break;
            case WuQMacroDataValueTypeEnum::STRING:
                valueTwo.setValue(valueTwoString.toString());
                break;
        }
        
        QString tooltip;
        macroCommand = new WuQMacroCommand(objectClass,
                                           objectName.toString(),
                                           descriptiveNameString.toString(),
                                           tooltip,
                                           value,
                                           valueTwo);
        
        if ( ! delayString.isEmpty()) {
            bool valid(false);
            float delayValue = delayString.toFloat(&valid);
            if ( ! valid) {
                delayValue = 1.0;
            }
            macroCommand->setDelayInSeconds(delayValue);
        }
    }
    
    return macroCommand;
}

/**
 * @param xmlReader
 *    The XML stream reader
 * @return Read and return the mouse event information
 */
WuQMacroMouseEventInfo*
WuQMacroGroupXmlStreamReader::readMacroMouseEventInfo(QXmlStreamReader& xmlReader)
{
    const QXmlStreamAttributes attributes = xmlReader.attributes();
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
    if (mouseButtonString.isEmpty()) es.append(ATTRIBUTE_MOUSE_BUTTON + " ");
    if (mouseButtonsMaskString.isEmpty()) es.append(ATTRIBUTE_MOUSE_BUTTONS_MASK + " ");
    if (keyboardModifiersMaskString.isEmpty()) es.append(ATTRIBUTE_MOUSE_KEYBOARD_MODIFIERS_MASK + " ");
    if (widgetWidthString.isEmpty()) es.append(ATTRIBUTE_MOUSE_WIDGET_WIDTH + " ");
    if (widgetHeightString.isEmpty()) es.append(ATTRIBUTE_MOUSE_WIDGET_HEIGHT + " ");
    if ( ! es.isEmpty()) {
        addToWarnings(xmlReader,
                      ELEMENT_MOUSE_EVENT_INFO
                      + " is missing required attribute(s): "
                      + es);
        return NULL;
    }
    
    bool validMouseEventTypeFlag(false);
    const WuQMacroMouseEventTypeEnum::Enum mouseEventType = WuQMacroMouseEventTypeEnum::fromName(mouseEventTypeString,
                                                                                                 &validMouseEventTypeFlag);
    if ( ! validMouseEventTypeFlag) {
        addToWarnings(xmlReader,
                      mouseEventTypeString
                      + " is not valid for attribute "
                      + ATTRIBUTE_MOUSE_EVENT_TYPE);
        return NULL;
    }
    
    WuQMacroMouseEventInfo* mouseInfo = new WuQMacroMouseEventInfo(mouseEventType,
                                                                   mouseButtonString.toUInt(),
                                                                   mouseButtonsMaskString.toUInt(),
                                                                   keyboardModifiersMaskString.toUInt(),
                                                                   widgetWidthString.toInt(),
                                                                   widgetHeightString.toInt());
    if (( ! xLocalString.isEmpty())
        && ( ! yLocalString.isEmpty())) {
        mouseInfo->addLocalXY(xLocalString.toInt(),
                              yLocalString.toInt());
    }
    
    QString xyString = xmlReader.readElementText();
    if ( ! xyString.isEmpty()) {
        QTextStream stream(&xyString);
        while ( ! stream.atEnd()) {
            int32_t x, y;
            stream >> x;
            if ( ! stream.atEnd()) {
                stream >> y;
                mouseInfo->addLocalXY(x, y);
            }
        }
    }
    
    return mouseInfo;
}

/**
 * Add to the warning message.  Warnings are used to skip over invalid
 * elements instead of declaring the entire XML invalid.
 *
 * @param xmlReader
 *    The XML stream reader
 * @param warning
 *    The warning message
 */
void
WuQMacroGroupXmlStreamReader::addToWarnings(QXmlStreamReader& xmlReader,
                                            const QString& warning)
{
    if ( ! m_warningMessage.isEmpty()) {
        m_warningMessage.append("\n");
    }
    m_warningMessage.append("Line="
                      + QString::number(xmlReader.lineNumber())
                      + ", Column="
                      + QString::number(xmlReader.columnNumber())
                      + ": "
                      + warning);
}



