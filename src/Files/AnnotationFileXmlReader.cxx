
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

#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "CaretAssert.h"
#include "DataFileException.h"
#include "GiftiXmlElements.h"
#include "XmlStreamReaderHelper.h"

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
    m_streamHelper.grabNew(NULL);
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
AnnotationFileXmlReader::readFile(const QString& filename,
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
    
    readFileContentFromXmlStreamReader(filename,
                                       annotationFile);
    
    file.close();
    
    if (m_stream->hasError()) {
        m_streamHelper->throwDataFileException("There was an error reading the annotation file in XML format (reported by QXmlStreamReader): "
                                + m_stream->errorString());
    }
}

/**
 * Read the given annotation file contained in a string in XML format.
 *
 * @param fileInString
 *    String containing the file.
 * @param annotationFile
 *    Read into this annotation file.
 * @throws
 *    DataFileException if there is an error reading the file.
 */
void
AnnotationFileXmlReader::readFileFromString(const QString& fileInString,
                                            AnnotationFile* annotationFile)
{
    /*
     * Create an XML stream writer
     */
    m_stream.grabNew(new QXmlStreamReader(fileInString));
    
    readFileContentFromXmlStreamReader("SceneFileName",
                                       annotationFile);
    
    if (m_stream->hasError()) {
        m_streamHelper->throwDataFileException("There was an error reading the annotation file in XML format (reported by QXmlStreamReader): "
                                               + m_stream->errorString());
    }
}

/**
 * Read content of the file from the XML stream reader.
 *
 * @param filename
 *    Name of the file.
 * @param annotationFile
 *    Read into this annotation file.
 * @throws
 *    DataFileException if there is an error reading the file.
 */
void
AnnotationFileXmlReader::readFileContentFromXmlStreamReader(const QString& filename,
                                        AnnotationFile* annotationFile)
{
    /*
     * Create the helper for reading XML
     */
    if (m_streamHelper != NULL) {
        m_streamHelper.grabNew(NULL);
    }
    m_streamHelper.grabNew(new XmlStreamReaderHelper(filename,
                                                     m_stream));
    
    if (m_stream->atEnd()) {
        m_streamHelper->throwDataFileException("Error reading.  File appears to have no XML content.");
    }
    
    const bool fileElementValid = m_stream->readNextStartElement();
    if ( ! fileElementValid) {
        m_streamHelper->throwDataFileException("Appears to have no XML elements.");
    }
    
    const QStringRef fileElementName = m_stream->name();
    if (fileElementName != ELEMENT_ANNOTATION_FILE) {
        m_streamHelper->throwDataFileException("First element is "
                                               + fileElementName.toString()
                                               + " but should be "
                                               + ELEMENT_ANNOTATION_FILE);
    }
    
    QXmlStreamAttributes fileAttributes = m_stream->attributes();
    const QStringRef versionText = fileAttributes.value(ATTRIBUTE_VERSION);
    if (versionText.isEmpty()) {
        m_streamHelper->throwDataFileException("Version attribute ("
                                               + ATTRIBUTE_VERSION
                                               + ") is missing from the file element "
                                               + ELEMENT_ANNOTATION_FILE);
    }
    
    const int32_t versionNumber  = versionText.toString().toInt();
    if (versionNumber == XML_VERSION_ONE) {
        readVersionOne(annotationFile);
    }
    else {
        m_streamHelper->throwDataFileException("File version number "
                                               + versionText.toString()
                                               + " is not supported by this version of the software.");
    }
}

/**
 * Read a version one Annotation XML file.
 *
 * @param annotationFile
 *     Add annotations to this file.
 */
void
AnnotationFileXmlReader::readVersionOne(AnnotationFile* annotationFile)
{
    while (m_stream->readNextStartElement()) {
        bool skipCurrentElementFlag = true;
        
        const QString elementName = m_stream->name().toString();
        if (elementName == GiftiXmlElements::TAG_METADATA) {
            m_streamHelper->readMetaData(annotationFile->getFileMetaData());
            skipCurrentElementFlag = false;
        }
        else if (elementName == ELEMENT_BOX) {
            CaretPointer<AnnotationBox> annotation(new AnnotationBox());
            readTwoDimensionalAnnotation(ELEMENT_BOX,
                                         annotation);
            annotationFile->addAnnotation(annotation.releasePointer());
        }
        else if (elementName == ELEMENT_LINE) {
            CaretPointer<AnnotationLine> annotation(new AnnotationLine());
            readOneDimensionalAnnotation(ELEMENT_LINE,
                                         annotation);
            annotationFile->addAnnotation(annotation.releasePointer());
        }
        else if (elementName == ELEMENT_OVAL) {
            CaretPointer<AnnotationOval> annotation(new AnnotationOval());
            readTwoDimensionalAnnotation(ELEMENT_OVAL,
                                         annotation);
            annotationFile->addAnnotation(annotation.releasePointer());
        }
        else if (elementName == ELEMENT_TEXT) {
            CaretPointer<AnnotationText> annotation(new AnnotationText());
            readTwoDimensionalAnnotation(ELEMENT_TEXT,
                                         annotation);
            annotationFile->addAnnotation(annotation.releasePointer());
        }
        else {
            m_streamHelper->throwDataFileException("Unexpected XML element "
                                   + elementName);
        }

        /*
         * These elements have no other child elements so move on
         */
        if (skipCurrentElementFlag) {
            m_stream->skipCurrentElement();
        }
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
AnnotationFileXmlReader::readCoordinate(const QString& coordinateElementName,
                                        AnnotationCoordinate* coordinate)
{
    CaretAssert(coordinate);
    
    const bool elementValid = m_stream->readNextStartElement();
    if ( ! elementValid) {
        m_streamHelper->throwDataFileException("Failed to read element "
                               + coordinateElementName);
    }
    
    if (m_stream->name() != coordinateElementName) {
        m_streamHelper->throwDataFileException("Expected elment "
                               + coordinateElementName
                               + " but read element "
                               + m_stream->name().toString());
    }
    
    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    /*
     * XYZ coordinate
     */
    const float xyz[3] = {
        m_streamHelper->getRequiredAttributeFloatValue(attributes, coordinateElementName, ATTRIBUTE_COORD_X),
        m_streamHelper->getRequiredAttributeFloatValue(attributes, coordinateElementName, ATTRIBUTE_COORD_Y),
        m_streamHelper->getRequiredAttributeFloatValue(attributes, coordinateElementName, ATTRIBUTE_COORD_Z)
    };
    coordinate->setXYZ(xyz);
    
    /*
     * Surface coordinate
     */
    const int32_t numberOfNodes = m_streamHelper->getRequiredAttributeIntValue(attributes,
                                                                 coordinateElementName,
                                                                 ATTRIBUTE_COORD_SURFACE_NUMBER_OF_NODES);
    const int32_t nodeIndex = m_streamHelper->getRequiredAttributeIntValue(attributes,
                                                                 coordinateElementName,
                                                                 ATTRIBUTE_COORD_SURFACE_NODE_INDEX);
    const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                coordinateElementName,
                                                                ATTRIBUTE_COORD_SURFACE_STRUCTURE);
    const float offsetDistance = m_streamHelper->getRequiredAttributeFloatValue(attributes,
                                                                                coordinateElementName,
                                                                                ATTRIBUTE_COORD_SURFACE_NODE_OFFSET);
    bool valid = false;
    StructureEnum::Enum structure = StructureEnum::fromName(valueString,
                                                          &valid);
    if (valid) {
        coordinate->setSurfaceSpace(structure, numberOfNodes, nodeIndex, offsetDistance);
    }
    else {
        m_streamHelper->throwDataFileException("Invalid value "
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
    CaretAssert(annotation);
    
    {
        /*
         * Coordinate space
         */
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_COORDINATE_SPACE);
        bool valid = false;
        AnnotationCoordinateSpaceEnum::Enum value = AnnotationCoordinateSpaceEnum::fromName(valueString,
                                                                                            &valid);
        if (valid) {
            annotation->setCoordinateSpace(value);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_COORDINATE_SPACE);
        }
    }
    
    {
        /*
         * Background color
         */
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_BACKGROUND_CARET_COLOR);
        bool valid = false;
        CaretColorEnum::Enum value = CaretColorEnum::fromName(valueString,
                                                              &valid);
        if (valid) {
            annotation->setBackgroundColor(value);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_BACKGROUND_CARET_COLOR);
        }
    }
    
    {
        /*
         * Background custom color
         */
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_BACKGROUND_CUSTOM_RGBA);
        std::vector<float> rgba;
        AString::toNumbers(valueString, rgba);
        if (rgba.size() == 4) {
            annotation->setCustomBackgroundColor(&rgba[0]);
        }
        else {
            m_streamHelper->throwDataFileException(ATTRIBUTE_BACKGROUND_CUSTOM_RGBA
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
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_FOREGROUND_CARET_COLOR);
        bool valid = false;
        CaretColorEnum::Enum value = CaretColorEnum::fromName(valueString,
                                                              &valid);
        if (valid) {
            annotation->setForegroundColor(value);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_FOREGROUND_CARET_COLOR);
        }
    }
    
    {
        /*
         * Foreground custom color
         */
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    annotationElementName,
                                                                    ATTRIBUTE_FOREGROUND_CUSTOM_RGBA);
        std::vector<float> rgba;
        AString::toNumbers(valueString, rgba);
        if (rgba.size() == 4) {
            annotation->setCustomForegroundColor(&rgba[0]);
        }
        else {
            m_streamHelper->throwDataFileException(ATTRIBUTE_FOREGROUND_CUSTOM_RGBA
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
    annotation->setForegroundLineWidth(m_streamHelper->getRequiredAttributeFloatValue(attributes,
                                                                      annotationElementName,
                                                                      ATTRIBUTE_FOREGROUND_LINE_WIDTH));
    /*
     * Tab Index
     */
    annotation->setTabIndex(m_streamHelper->getRequiredAttributeIntValue(attributes,
                                                         annotationElementName,
                                                         ATTRIBUTE_TAB_INDEX));
    /*
     * Window Index
     */
    annotation->setWindowIndex(m_streamHelper->getRequiredAttributeIntValue(attributes,
                                                            annotationElementName,
                                                            ATTRIBUTE_WINDOW_INDEX));
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
    CaretAssert(annotation);

    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    readAnnotationAttributes(annotation,
                             annotationElementName,
                             attributes);
    
    AnnotationLine* line = dynamic_cast<AnnotationLine*>(annotation);
    if (line != NULL) {
        line->setDisplayEndArrow(m_streamHelper->getOptionalAttributeBoolValue(attributes,
                                                                               annotationElementName,
                                                                               ATTRIBUTE_LINE_END_ARROW,
                                                                               false));
        line->setDisplayStartArrow(m_streamHelper->getOptionalAttributeBoolValue(attributes,
                                                                                 annotationElementName,
                                                                                 ATTRIBUTE_LINE_START_ARROW,
                                                                                 false));
    }
    
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
    CaretAssert(annotation);
    
    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    readAnnotationAttributes(annotation,
                             annotationElementName,
                             attributes);
    
    /*
     * Shape width
     */
    annotation->setWidth(m_streamHelper->getRequiredAttributeFloatValue(attributes,
                                                        annotationElementName,
                                                        ATTRIBUTE_WIDTH));
    /*
     * Shape height
     */
    annotation->setHeight(m_streamHelper->getRequiredAttributeFloatValue(attributes,
                                                        annotationElementName,
                                                        ATTRIBUTE_HEIGHT));
    /*
     * Shape rotation angle
     */
    annotation->setRotationAngle(m_streamHelper->getRequiredAttributeFloatValue(attributes,
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
        readTextDataElement(textAnn);
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
    CaretAssert(textAnnotation);
    
    const bool elementValid = m_stream->readNextStartElement();
    if ( ! elementValid) {
        m_streamHelper->throwDataFileException("Failed to read element "
                               + ELEMENT_TEXT_DATA);
    }
    
    if (m_stream->name() != ELEMENT_TEXT_DATA) {
        m_streamHelper->throwDataFileException("Expected elment "
                               + ELEMENT_TEXT_DATA
                               + " but read element "
                               + m_stream->name().toString());
    }
    
    const QXmlStreamAttributes attributes = m_stream->attributes();
    
    textAnnotation->setBoldEnabled(m_streamHelper->getRequiredAttributeBoolValue(attributes,
                                                                 ELEMENT_TEXT_DATA,
                                                                 ATTRIBUTE_TEXT_FONT_BOLD));
    textAnnotation->setItalicEnabled(m_streamHelper->getRequiredAttributeBoolValue(attributes,
                                                                   ELEMENT_TEXT_DATA,
                                                                   ATTRIBUTE_TEXT_FONT_ITALIC));
    textAnnotation->setUnderlineEnabled(m_streamHelper->getRequiredAttributeBoolValue(attributes,
                                                                   ELEMENT_TEXT_DATA,
                                                                   ATTRIBUTE_TEXT_FONT_UNDERLINE));
    
    {
        const QString defaultValue = AnnotationTextConnectTypeEnum::toName(AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_NONE);
        const QString valueString = m_streamHelper->getOptionalAttributeStringValue(attributes,
                                                                                    ELEMENT_TEXT_DATA,
                                                                                    ATTRIBUTE_TEXT_CONNECT_BRAINORDINATE,
                                                                                    defaultValue);
        bool valid = false;
        AnnotationTextConnectTypeEnum::Enum connectValue = AnnotationTextConnectTypeEnum::fromName(valueString,
                                                                                               &valid);
        if (valid) {
            textAnnotation->setConnectToBrainordinate(connectValue);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_CONNECT_BRAINORDINATE);
        }
    }
    
    {
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                                    ELEMENT_TEXT_DATA,
                                                                                    ATTRIBUTE_TEXT_FONT_NAME);
        bool valid = false;
        AnnotationFontNameEnum::Enum fontName = AnnotationFontNameEnum::fromName(valueString,
                                                                                 &valid);
        if (valid) {
            textAnnotation->setFont(fontName);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                                   + valueString
                                                   + " for attribute "
                                                   + ATTRIBUTE_TEXT_FONT_NAME);
        }
    }
    
    {
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_FONT_SIZE);
        bool valid = false;
        AnnotationFontSizeEnum::Enum fontSize = AnnotationFontSizeEnum::fromName(valueString,
                                                                                 &valid);
        if (valid) {
            textAnnotation->setFontSize(fontSize);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_FONT_SIZE);
        }
    }

    {
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT);
        bool valid = false;
        AnnotationTextAlignHorizontalEnum::Enum alignment = AnnotationTextAlignHorizontalEnum::fromName(valueString,
                                                                                 &valid);
        if (valid) {
            textAnnotation->setHorizontalAlignment(alignment);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT);
        }
    }
    
    {
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT);
        bool valid = false;
        AnnotationTextAlignVerticalEnum::Enum alignment = AnnotationTextAlignVerticalEnum::fromName(valueString,
                                                                                                        &valid);
        if (valid) {
            textAnnotation->setVerticalAlignment(alignment);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT);
        }
    }
    
    {
        const QString valueString = m_streamHelper->getRequiredAttributeStringValue(attributes,
                                                                    ELEMENT_TEXT_DATA,
                                                                    ATTRIBUTE_TEXT_ORIENTATION);
        bool valid = false;
        AnnotationTextOrientationEnum::Enum orientation = AnnotationTextOrientationEnum::fromName(valueString,
                                                                                                    &valid);
        if (valid) {
            textAnnotation->setOrientation(orientation);
        }
        else {
            m_streamHelper->throwDataFileException("Invalid value "
                                   + valueString
                                   + " for attribute "
                                   + ATTRIBUTE_TEXT_ORIENTATION);
        }
    }
    
    textAnnotation->setViewportHeightWhenCreated(m_streamHelper->getOptionalAttributeFloatValue(attributes,
                                                                                               ELEMENT_TEXT_DATA,
                                                                                               ATTRIBUTE_TEXT_VIEWPORT_HEIGHT,
                                                                                                -1.0));
                                                 
    
    /*
     * Read the annotation's text which will also finish reading through
     * the closing element.
     */
    const QString textChars = m_stream->readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
    if (m_stream->hasError()) {
        m_streamHelper->throwDataFileException("There was an error reading the text annotation's characters: "
                               + m_stream->errorString());
    }
    textAnnotation->setText(textChars);
    
    if (m_stream->hasError()) {
        m_streamHelper->throwDataFileException("There was an error reading the annotation file in XML format (reported by QXmlStreamReader): "
                               + m_stream->errorString());
    }
}


