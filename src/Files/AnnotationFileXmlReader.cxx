
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

#include <QFile>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

#define __ANNOTATION_FILE_XML_READER_DECLARE__
#include "AnnotationFileXmlReader.h"
#undef __ANNOTATION_FILE_XML_READER_DECLARE__

#include "AnnotationArrow.h"
#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "CaretAssert.h"
#include "DataFileException.h"

using namespace caret;


    
/**
 * \class caret::AnnotationFileXmlReader 
 * \brief Read Annotation File from XML format.
 * \ingroup Files
 */

/**
 * Constructor.
 */
AnnotationFileXmlReader::AnnotationFileXmlReader()
: AnnotationFileXmlFormatBase()
{
    m_stream.grabNew(new QXmlStreamReader());
}

/**
 * Destructor.
 */
AnnotationFileXmlReader::~AnnotationFileXmlReader()
{
}

/**
 * Read the given annotation file in XML format.
 *
 * @param filename
 *    Name of file.
 * @param annotationFile
 *    Read into this annotation file.
 * @throws
 *    DataFileException if there is an error reading the file.
 */
void
AnnotationFileXmlReader::readFile(const AString& filename,
                                  AnnotationFile* annotationFile)
{
    CaretAssert(annotationFile);
    m_filename = filename;
    
    /*
     * Open the file
     */
    QFile file(m_filename);
    if ( ! file.open(QFile::ReadOnly)) {
        const QString msg("Error opening for reading."
                          + file.errorString());
        throw DataFileException(m_filename,
                                msg);
    }
    
    /*
     * Create an XML stream writer
     */
    m_stream.grabNew(new QXmlStreamReader(&file));
    
    if (m_stream->atEnd()) {
        throwDataFileException("Error reading.  File appears to have no XML content.");
    }
    
    const bool fileElementValid = m_stream->readNextStartElement();
    if ( ! fileElementValid) {
        throwDataFileException("Appears to have no XML elements.");
    }
    
    const QStringRef fileElementName = m_stream->name();
    if (fileElementName != ELEMENT_ANNOTATION_FILE) {
        throwDataFileException("First element is "
                                + fileElementName.toString()
                                + " but should be "
                                + ELEMENT_ANNOTATION_FILE);
    }
    
    QXmlStreamAttributes fileAttributes = m_stream->attributes();
    const QStringRef versionText = fileAttributes.value(ATTRIBUTE_VERSION);
    if (versionText.isEmpty()) {
        throwDataFileException("Version attribute ("
                                + ATTRIBUTE_VERSION
                                + ") is missing from the file element "
                                + ELEMENT_ANNOTATION_FILE);
    }
    
    const int32_t versionNumber  = versionText.toString().toInt();
    if (versionNumber == XML_VERSION_ONE) {
        readVersionOne();
    }
    else {
        throwDataFileException("File version number "
                                 + versionText.toString()
                                 + " is not supported by this version of the software.");
    }
    
    file.close();
    
    if (m_stream->hasError()) {
        throwDataFileException("There was an error reading the annotation file in XML format (reported by QXmlStreamReader): "
                                + m_stream->errorString());
    }
}

/**
 * Read a version one Annotation XML file.
 */
void
AnnotationFileXmlReader::readVersionOne()
{
    while (m_stream->readNextStartElement()) {
        const QString annotationName = m_stream->name().toString();
        if (annotationName == ELEMENT_ARROW) {
            CaretPointer<AnnotationArrow> annotation(new AnnotationArrow());
            readOneDimensionalAnnotation(ELEMENT_ARROW,
                                         annotation);
        }
        else if (annotationName == ELEMENT_BOX) {
            CaretPointer<AnnotationBox> annotation(new AnnotationBox());
            readTwoDimensionalAnnotation(ELEMENT_BOX,
                                         annotation);
        }
        else if (annotationName == ELEMENT_LINE) {
            CaretPointer<AnnotationLine> annotation(new AnnotationLine());
            readOneDimensionalAnnotation(ELEMENT_LINE,
                                         annotation);
        }
        else if (annotationName == ELEMENT_OVAL) {
            CaretPointer<AnnotationOval> annotation(new AnnotationOval());
            readTwoDimensionalAnnotation(ELEMENT_OVAL,
                                         annotation);
        }
        else if (annotationName == ELEMENT_TEXT) {
            CaretPointer<AnnotationText> annotation(new AnnotationText());
            readTwoDimensionalAnnotation(ELEMENT_TEXT,
                                         annotation);
        }
        else {
            throwDataFileException("Unexpected XML element "
                                   + annotationName);
        }

        /*
         * These elements have no other child elements so move on
         */
        m_stream->skipCurrentElement();
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
AnnotationFileXmlReader::throwDataFileException(const QString message)
{
    throw DataFileException(m_filename,
                            (message
                             + " at line "
                             + QString::number(m_stream->lineNumber())
                             + ", column "
                             + QString::number(m_stream->columnNumber())));
}

/**
 * Read the next start element which should be a coordinate
 * with the given element name.
 *
 * @param annotation
 *     One-dimensional annotation that has its data read.
 * @throw
 *     DataFileException
 */
void
AnnotationFileXmlReader::readCoordinate(const QString& coordinateElementName,
                                        AnnotationCoordinate* coordinate)
{
    const bool elementValid = m_stream->readNextStartElement();
    if ( ! elementValid) {
        throwDataFileException("Failed to read element "
                               + coordinateElementName);
    }
    
    if (m_stream->name() != coordinateElementName) {
        throwDataFileException("Expected elment "
                               + coordinateElementName
                               + " but read element "
                               + m_stream->name().toString());
    }
    
    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    /*
     * XYZ coordinate
     */
    const float xyz[3] = {
        getRequiredAttributeFloatValue(attributes, coordinateElementName, ATTRIBUTE_COORD_X),
        getRequiredAttributeFloatValue(attributes, coordinateElementName, ATTRIBUTE_COORD_Y),
        getRequiredAttributeFloatValue(attributes, coordinateElementName, ATTRIBUTE_COORD_Z)
    };
    coordinate->setXYZ(xyz);
    
    /*
     * Surface coordinate
     */
    const int32_t numberOfNodes = getRequiredAttributeFloatValue(attributes,
                                                                 coordinateElementName,
                                                                 ATTRIBUTE_COORD_SURFACE_NUMBER_OF_NODES);
    const int32_t nodeIndex = getRequiredAttributeFloatValue(attributes,
                                                                 coordinateElementName,
                                                                 ATTRIBUTE_COORD_SURFACE_NODE_INDEX);
    const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                coordinateElementName,
                                                                ATTRIBUTE_COORD_SURFACE_STRUCTURE);
    bool valid = false;
    StructureEnum::Enum structure = StructureEnum::fromName(valueString,
                                                          &valid);
    if (valid) {
        coordinate->setSurfaceSpace(structure, numberOfNodes, nodeIndex);
    }
    else {
        throwDataFileException("Invalid value "
                               + valueString
                               + " for attribute "
                               + ATTRIBUTE_COORD_SURFACE_STRUCTURE);
    }
    
    /*
     * No other child elements so move on
     */
    m_stream->skipCurrentElement();
}

/**
 * Read the attributes common to all annotation elements.
 *
 * @param annotation
 *     The annotation.
 * @param annotationElementName
 *     Name of element of annotation.
 * @param attributes
 *     The XML attributes.
 * @throw
 *     DataFileException if there is an error reading the attributes.
 */
void
AnnotationFileXmlReader::readAnnotationAttributes(Annotation* annotation,
                                                  const QString& annotationElementName,
                                                  const QXmlStreamAttributes& attributes)
{
    {
        /*
         * Coordinate space
         */
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_COORDINATE_SPACE);
        bool valid = false;
        AnnotationCoordinateSpaceEnum::Enum value = AnnotationCoordinateSpaceEnum::fromName(valueString,
                                                                                            &valid);
        if (valid) {
            annotation->setCoordinateSpace(value);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_COORDINATE_SPACE);
        }
    }
    
    {
        /*
         * Background color
         */
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_BACKGROUND_CARET_COLOR);
        bool valid = false;
        CaretColorEnum::Enum value = CaretColorEnum::fromName(valueString,
                                                              &valid);
        if (valid) {
            annotation->setBackgroundColor(value);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_BACKGROUND_CARET_COLOR);
        }
    }
    
    {
        /*
         * Background custom color
         */
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_BACKGROUND_CUSTOM_RGBA);
        std::vector<float> rgba;
        AString::toNumbers(valueString, rgba);
        if (rgba.size() == 4) {
            annotation->setCustomBackgroundColor(&rgba[0]);
        }
        else {
            throwDataFileException(ATTRIBUTE_BACKGROUND_CUSTOM_RGBA
                                   + " must contain 4 elements but "
                                   + valueString
                                   + " contains "
                                   + QString::number(rgba.size())
                                   + " elements");
        }
    }
    
    {
        /*
         * Foreground color
         */
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_FOREGROUND_CARET_COLOR);
        bool valid = false;
        CaretColorEnum::Enum value = CaretColorEnum::fromName(valueString,
                                                              &valid);
        if (valid) {
            annotation->setForegroundColor(value);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_FOREGROUND_CARET_COLOR);
        }
    }
    
    {
        /*
         * Foreground custom color
         */
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_FOREGROUND_CUSTOM_RGBA);
        std::vector<float> rgba;
        AString::toNumbers(valueString, rgba);
        if (rgba.size() == 4) {
            annotation->setCustomForegroundColor(&rgba[0]);
        }
        else {
            throwDataFileException(ATTRIBUTE_FOREGROUND_CUSTOM_RGBA
                                   + " must contain 4 elements but "
                                   + valueString
                                   + " contains "
                                   + QString::number(rgba.size())
                                   + " elements");
        }
    }
    
    /*
     * Foreground line width
     */
    annotation->setForegroundLineWidth(getRequiredAttributeFloatValue(attributes,
                                                                      annotationElementName,
                                                                      ATTRIBUTE_FOREGROUND_LINE_WIDTH));
    /*
     * Tab Index
     */
    annotation->setTabIndex(getRequiredAttributeIntValue(attributes,
                                                         annotationElementName,
                                                         ATTRIBUTE_TAB_INDEX));
    /*
     * Window Index
     */
    annotation->setWindowIndex(getRequiredAttributeIntValue(attributes,
                                                            annotationElementName,
                                                            ATTRIBUTE_WINDOW_INDEX));
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
 * @return
 *     String value for the attribute.
 * @throw
 *     DataFileException if attribute is missing or value is
 *     an empty string.
 */
QString
AnnotationFileXmlReader::getRequiredAttributeStringValue(const QXmlStreamAttributes& attributes,
                                                         const QString& elementName,
                                                         const QString& attributeName)
{
    QString valueString;
    
    if (attributes.hasAttribute(attributeName)) {
        valueString = attributes.value(attributeName).toString();
        if (valueString.isEmpty()) {
            throwDataFileException("Value for attribute "
                                   + attributeName
                                   + " in element "
                                   + elementName
                                   + " is empty");
        }
    }
    else {
        throwDataFileException(attributeName
                               + " is missing from element "
                               + elementName);
    }

    return valueString;
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
AnnotationFileXmlReader::getRequiredAttributeBoolValue(const QXmlStreamAttributes& attributes,
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
AnnotationFileXmlReader::getRequiredAttributeIntValue(const QXmlStreamAttributes& attributes,
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
AnnotationFileXmlReader::getRequiredAttributeFloatValue(const QXmlStreamAttributes& attributes,
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
 * Read a one dimensional annotation.
 *
 * @param annotationElementName
 *     Name of one-dimensional attribute.
 * @param annotation
 *     One-dimensional annotation that has its data read.
 */
void
AnnotationFileXmlReader::readOneDimensionalAnnotation(const QString& annotationElementName,
                                                      AnnotationOneDimensionalShape* annotation)
{
    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    readAnnotationAttributes(annotation,
                             annotationElementName,
                             attributes);
    
    readCoordinate(ELEMENT_COORDINATE_ONE,
                   annotation->getStartCoordinate());
    readCoordinate(ELEMENT_COORDINATE_TWO,
                   annotation->getEndCoordinate());
}

/**
 * Read a two dimensional annotation.
 *
 * @param annotationElementName
 *     Name of two-dimensional attribute.
 * @param annotation
 *     Two-dimensional annotation that has its data read.
 */
void
AnnotationFileXmlReader::readTwoDimensionalAnnotation(const QString& annotationElementName,
                                                      AnnotationTwoDimensionalShape* annotation)
{
    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    readAnnotationAttributes(annotation,
                             annotationElementName,
                             attributes);
    
    /*
     * Shape width
     */
    annotation->setWidth(getRequiredAttributeFloatValue(attributes,
                                                        annotationElementName,
                                                        ATTRIBUTE_WIDTH));
    /*
     * Shape height
     */
    annotation->setHeight(getRequiredAttributeFloatValue(attributes,
                                                        annotationElementName,
                                                        ATTRIBUTE_HEIGHT));
    /*
     * Shape rotation angle
     */
    annotation->setRotationAngle(getRequiredAttributeFloatValue(attributes,
                                                        annotationElementName,
                                                        ATTRIBUTE_ROTATION_ANGLE));
    
    /*
     * Read the coordinate
     */
    readCoordinate(ELEMENT_COORDINATE_ONE,
                   annotation->getCoordinate());
    
    /*
     * Is this a text annotation?
     */
    if (annotationElementName == ELEMENT_TEXT) {
        AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
        CaretAssert(textAnn);
        
    }
}

/**
 * Read the next start element which should be a coordinate
 * with the given element name.
 *
 * @param annotation
 *     One-dimensional annotation that has its data read.
 * @throw
 *     DataFileException
 */
void
AnnotationFileXmlReader::readTextDataElement(AnnotationText *textAnnotation)
{
    const bool elementValid = m_stream->readNextStartElement();
    if ( ! elementValid) {
        throwDataFileException("Failed to read element "
                               + ELEMENT_TEXT_DATA);
    }
    
    if (m_stream->name() != ELEMENT_TEXT_DATA) {
        throwDataFileException("Expected elment "
                               + ELEMENT_TEXT_DATA
                               + " but read element "
                               + m_stream->name().toString());
    }
    
    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    textAnnotation->setBoldEnabled(getRequiredAttributeBoolValue(attributes,
                                                                 ELEMENT_TEXT_DATA,
                                                                 ATTRIBUTE_TEXT_FONT_BOLD));
    textAnnotation->setItalicEnabled(getRequiredAttributeBoolValue(attributes,
                                                                   ELEMENT_TEXT_DATA,
                                                                   ATTRIBUTE_TEXT_FONT_ITALIC));
    textAnnotation->setUnderlineEnabled(getRequiredAttributeBoolValue(attributes,
                                                                   ELEMENT_TEXT_DATA,
                                                                   ATTRIBUTE_TEXT_FONT_UNDERLINE));
    
    {
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_FONT_NAME);
        bool valid = false;
        AnnotationFontNameEnum::Enum fontName = AnnotationFontNameEnum::fromName(valueString,
                                                                                 &valid);
        if (valid) {
            textAnnotation->setFont(fontName);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_FONT_NAME);
        }
    }
    
    {
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_FONT_SIZE);
        bool valid = false;
        AnnotationFontSizeEnum::Enum fontSize = AnnotationFontSizeEnum::fromName(valueString,
                                                                                 &valid);
        if (valid) {
            textAnnotation->setFontSize(fontSize);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_FONT_SIZE);
        }
    }

    {
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT);
        bool valid = false;
        AnnotationTextAlignHorizontalEnum::Enum alignment = AnnotationTextAlignHorizontalEnum::fromName(valueString,
                                                                                 &valid);
        if (valid) {
            textAnnotation->setHorizontalAlignment(alignment);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT);
        }
    }
    
    {
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT);
        bool valid = false;
        AnnotationTextAlignVerticalEnum::Enum alignment = AnnotationTextAlignVerticalEnum::fromName(valueString,
                                                                                                        &valid);
        if (valid) {
            textAnnotation->setVerticalAlignment(alignment);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT);
        }
    }
    
    {
        const QString valueString = getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_ORIENTATION);
        bool valid = false;
        AnnotationTextOrientationEnum::Enum orientation = AnnotationTextOrientationEnum::fromName(valueString,
                                                                                                    &valid);
        if (valid) {
            textAnnotation->setOrientation(orientation);
        }
        else {
            throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_ORIENTATION);
        }
    }
    
    /*
     * Read the annotation's text.
     */
    textAnnotation->setText(m_stream->text().toString());
    
    /*
     * No other child elements so move on
     */
    m_stream->skipCurrentElement();
}


