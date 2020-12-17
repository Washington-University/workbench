
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __XML_STREAM_READER_HELPER_DECLARE__
#include "XmlStreamReaderHelper.h"
#undef __XML_STREAM_READER_HELPER_DECLARE__

#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

#include "CaretAssert.h"
#include "DataFileException.h"
#include "GiftiMetaData.h"

using namespace caret;


    
/**
 * \class caret::XmlStreamReaderHelper 
 * \brief Helper class for using QXmlStreamReader
 * \ingroup Files
 *
 * Assists with reading a file using QXmlStreamReader.
 * If there is an error while using any of these methods,
 * a DataFileException will be thrown containing the 
 * name of the file, a message, and also the line and 
 * column of the error.
 */

/**
 * Constructor.
 *
 * @param filename
 *    Name of file that is being read using the QXmlStreamReader
 * @param stream
 *    The QXmlStreamReader
 */
XmlStreamReaderHelper::XmlStreamReaderHelper(const QString& filename,
                                             QXmlStreamReader* stream)
: CaretObject(),
m_filename(filename),
m_stream(stream)
{
    CaretAssert(stream);
}

/**
 * Destructor.
 */
XmlStreamReaderHelper::~XmlStreamReaderHelper()
{
}

/**
 * Get the string value for the given attribute name from the
 * given attributes.  If the attribute is missing or if the
 * attributes value is empty text, the defaultValue is 
 * returned.  This method WILL NOT throw an exception.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @param defaultValue
 *     Value that is returned if the attribute is missing or if the value
 *     is empty text.
 * @return
 *     String value for the attribute.
 */
QString
XmlStreamReaderHelper::getOptionalAttributeStringValue(const QXmlStreamAttributes& attributes,
                                        const QString& /*elementName*/,
                                        const QString& attributeName,
                                        const QString& defaultValue)
{
    QString valueString;
    
    if (attributes.hasAttribute(attributeName)) {
        valueString = attributes.value(attributeName).toString();
        if (valueString.isEmpty()) {
            valueString = defaultValue;
        }
    }
    else {
        valueString = defaultValue;
    }
    
    return valueString;
}


/**
 * Get the string value for the given attribute name from the
 * given attributes.  If the attribute name is not found or
 * its value is an empty string, a DataFileException is thrown.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @param alternateAttributeNameOne
 *     Use if attributeName is not found.
 * @param alternateAttributeNameTwo
 *     Use if neither attributeName nor alternateAttributeNameOne is not found.
 * @return
 *     String value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
QString
XmlStreamReaderHelper::getRequiredAttributeStringValue(const QXmlStreamAttributes& attributes,
                                                       const QString& elementName,
                                                       const QString& attributeName,
                                                       const QString& alternateAttributeNameOne,
                                                       const QString& alternateAttributeNameTwo)
{
    QString valueString;
    
    QString foundAttributeName;
    
    if (attributes.hasAttribute(attributeName)) {
        foundAttributeName = attributeName;
    }
    else if (( ! alternateAttributeNameOne.isEmpty())
             && attributes.hasAttribute(alternateAttributeNameOne)) {
        foundAttributeName = alternateAttributeNameOne;
    }
    else if (( ! alternateAttributeNameTwo.isEmpty())
             && attributes.hasAttribute(alternateAttributeNameTwo)) {
        foundAttributeName = alternateAttributeNameTwo;
    }
    
    if ( ! foundAttributeName.isEmpty()) {
        valueString = attributes.value(foundAttributeName).toString();
        if (valueString.isEmpty()) {
            throwDataFileException("Value for attribute "
                                   + foundAttributeName
                                   + " in element "
                                   + elementName
                                   + " is empty");
        }
    }
    else {
        std::vector<AString> attNames;
        for (int32_t i = 0; i < attributes.count(); i++) {
            attNames.push_back(attributes.at(i).name().toString());
        }
        
        AString msg(attributeName
                    + " is missing from element "
                    + elementName);
        if ( ! attributes.empty()) {
            msg.appendWithNewLine("Available attributes: "
                                  + AString::join(attNames, "; "));
        }
        throwDataFileException(msg);
        
    }
    
    return valueString;
}

/**
 * Get the bool value for the given attribute name from the
 * given attributes.  If the attribute is missing or if the
 * attributes value is empty text, the defaultValue is
 * returned.  This method WILL NOT throw an exception.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @param defaultValue
 *     Value that is returned if the attribute is missing.
 * @return
 *     Boolean value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
bool
XmlStreamReaderHelper::getOptionalAttributeBoolValue(const QXmlStreamAttributes& attributes,
                                   const QString& elementName,
                                   const QString& attributeName,
                                   const bool defaultValue)
{
    const AString stringValue = getOptionalAttributeStringValue(attributes,
                                                                elementName,
                                                                attributeName,
                                                                AString::fromBool(defaultValue));
    
    const bool value = stringValue.toBool();
    
    return value;
}


/**
 * Get the bool value for the given attribute name from the
 * given attributes.  If the attribute name is not found or
 * its value is an empty string, a DataFileException is thrown.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @return
 *     Boolean value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
bool
XmlStreamReaderHelper::getRequiredAttributeBoolValue(const QXmlStreamAttributes& attributes,
                                                       const QString& elementName,
                                                       const QString& attributeName)
{
    const AString stringValue = getRequiredAttributeStringValue(attributes,
                                                                elementName,
                                                                attributeName);
    
    const bool value = stringValue.toBool();
    
    return value;
}

/**
 * Get the int value for the given attribute name from the
 * given attributes.  If the attribute is missing or if the
 * attributes value is empty text, the defaultValue is
 * returned.  This method WILL NOT throw an exception.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @param defaultValue
 *     Value that is returned if the attribute is missing.
 * @return
 *     int value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
int
XmlStreamReaderHelper::getOptionalAttributeIntValue(const QXmlStreamAttributes& attributes,
                                                    const QString& elementName,
                                                    const QString& attributeName,
                                                    const int defaultValue)
{
    const AString stringValue = getOptionalAttributeStringValue(attributes,
                                                                elementName,
                                                                attributeName,
                                                                AString::number(defaultValue));
    
    const int value = stringValue.toInt();
    
    return value;
}

/**
 * Get the int value for the given attribute name from the
 * given attributes.  If the attribute name is not found or
 * its value is an empty string, a DataFileException is thrown.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @return
 *     Integer value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
int
XmlStreamReaderHelper::getRequiredAttributeIntValue(const QXmlStreamAttributes& attributes,
                                                      const QString& elementName,
                                                      const QString& attributeName)
{
    const QString stringValue = getRequiredAttributeStringValue(attributes,
                                                                elementName,
                                                                attributeName);
    
    bool valid;
    const int value = stringValue.toInt(&valid);
    
    if ( ! valid) {
        throwDataFileException("Value for attribute "
                               + attributeName
                               + " is not a valid integer value "
                               + stringValue);
    }
    
    return value;
}

/**
 * Get the float value for the given attribute name from the
 * given attributes.  If the attribute is missing or if the
 * attributes value is empty text, the defaultValue is
 * returned.  This method WILL NOT throw an exception.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @param defaultValue
 *     Value that is returned if the attribute is missing.
 * @return
 *     float value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
float
XmlStreamReaderHelper::getOptionalAttributeFloatValue(const QXmlStreamAttributes& attributes,
                                                     const QString& elementName,
                                                     const QString& attributeName,
                                                     const float defaultValue)
{
    const AString stringValue = getOptionalAttributeStringValue(attributes,
                                                                elementName,
                                                                attributeName,
                                                                AString::number(defaultValue));
    
    const float value = stringValue.toFloat();
    
    return value;
}

/**
 * Get the int value for the given attribute name from the
 * given attributes.  If the attribute name is not found or
 * its value is an empty string, a DataFileException is thrown.
 *
 * @param attributes
 *     The XML attributes.
 * @param elementName
 *     Name of element containing the attributes.
 * @param attributeName
 *     Name of the attribute.
 * @return
 *     Integer value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
float
XmlStreamReaderHelper::getRequiredAttributeFloatValue(const QXmlStreamAttributes& attributes,
                                                        const QString& elementName,
                                                        const QString& attributeName)
{
    const QString stringValue = getRequiredAttributeStringValue(attributes,
                                                                elementName,
                                                                attributeName);
    
    bool valid;
    const float value = stringValue.toFloat(&valid);
    
    if ( ! valid) {
        throwDataFileException("Value for attribute "
                               + attributeName
                               + " is not a valid integer value "
                               + stringValue);
    }
    
    return value;
}

/**
 * Read the GIFTI metadata (Copied from CiftiXMLReader::parseMetaData).
 *
 * @param metadata
 *     GIFTI metadata to which metadata is added.
 */
void
XmlStreamReaderHelper::readMetaData(GiftiMetaData* metadata)
{
    CaretAssert(metadata);
    
    metadata->clear();
    
    while (!(m_stream->isEndElement() && (m_stream->name().toString() == "MetaData"))
           && !m_stream->hasError()) {
        
        m_stream->readNext();
        if(m_stream->isStartElement()) {
            QString elementName = m_stream->name().toString();
            if(elementName == "MD") {
                readMetaDataElement(metadata);
            }
            else throwDataFileException("unknown element in MetaData: " + elementName);
        }
    }
    //check for end element
    if(!m_stream->isEndElement() || (m_stream->name().toString() != "MetaData"))
        throwDataFileException("MetaData end tag not found.");
}

/**
 * Read the GIFTI metadata (Copied from CiftiXMLReader::parseMetaDataElement).
 *
 * @param metadata
 *     GIFTI metadata to which metadata is added.
 */
void
XmlStreamReaderHelper::readMetaDataElement(GiftiMetaData* metadata)
{
    QString name;
    QString value;
    bool haveName = false;
    bool haveValue = false;
    
    while (!(m_stream->isEndElement() && (m_stream->name().toString() == "MD"))
           && !m_stream->hasError()) {
        //test = m_stream->name().toString();
        m_stream->readNext();
        
        if(m_stream->isStartElement()) {
            QString elementName = m_stream->name().toString();
            if(elementName == "Name") {
                m_stream->readNext();
                if(m_stream->tokenType() != QXmlStreamReader::Characters) {
                    return;
                }
                name = m_stream->text().toString();
                haveName = true;
                m_stream->readNext();
                if(!m_stream->isEndElement()) {
                    throwDataFileException("End element for meta data name tag not found.");
                }
            }
            else if(elementName == "Value") {
                m_stream->readNext();
                if(m_stream->tokenType() != QXmlStreamReader::Characters) {
                    return;
                }
                value = m_stream->text().toString();
                haveValue = true;
                m_stream->readNext();
                if(!m_stream->isEndElement()) {
                    throwDataFileException("End element for meta data value tag not found.");
                }
            }
            else {
                throwDataFileException("unknown element in MD: " + elementName);
            }
        }
        
    }
    if (!haveName
        || !haveValue) {
        throwDataFileException("MD element is missing name or value");
    }
    metadata->set(name,
                  value);
    
    if(!m_stream->isEndElement()
       || (m_stream->name().toString() != "MD")) {
        throwDataFileException("End element for MD tag not found");
    }
}


/**
 * Throw a data file exception with the given message and add
 * the line and column numbers to the message.
 *
 * @param message
 *     Message included in the exception.
 * @throw
 *     Always throws a DataFileException.
 */
void
XmlStreamReaderHelper::throwDataFileException(const QString message)
{
    throw DataFileException(m_filename,
                            (message
                             + " at line "
                             + QString::number(m_stream->lineNumber())
                             + ", column "
                             + QString::number(m_stream->columnNumber())));
}


