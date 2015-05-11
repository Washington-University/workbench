
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
#include <QXmlStreamWriter>


#define __ANNOTATION_FILE_XML_WRITER_DECLARE__
#include "AnnotationFileXmlWriter.h"
#undef __ANNOTATION_FILE_XML_WRITER_DECLARE__

#include "AnnotationArrow.h"
#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "CaretAssert.h"
#include "DataFileException.h"

using namespace caret;



/**
 * \class caret::AnnotationFileXmlWriter
 * \brief Writes an Annotation File in XML format.
 * \ingroup Files
 */

/**
 * Constructor.
 */
AnnotationFileXmlWriter::AnnotationFileXmlWriter()
: AnnotationFileXmlFormatBase()
{
    m_stream.grabNew(NULL);
}

/**
 * Destructor.
 */
AnnotationFileXmlWriter::~AnnotationFileXmlWriter()
{
}


/**
 * Write the given Annotation File in XML format.  Name of the file
 * is obtained from the file.
 *
 * @param annotationFile
 *     Annotation File written in XML format.
 * @throws
 *     DataFileException if there is an error writing the file.
 */
void
AnnotationFileXmlWriter::writeFile(const AnnotationFile* annotationFile)
{
    CaretAssert(annotationFile);
    
    /*
     * Open the file
     */
    QFile file(annotationFile->getFileName());
    if ( ! file.open(QFile::WriteOnly)) {
        throw DataFileException(annotationFile->getFileName(),
                                "Error opening for writing: "
                                + file.errorString());
    }
    
    /*
     * Create an XML stream writer
     */
    m_stream.grabNew(new QXmlStreamWriter(&file));
    
    m_stream->setAutoFormatting(true);
    m_stream->writeStartDocument();
    
    m_stream->writeStartElement(ELEMENT_ANNOTATION_FILE);
    m_stream->writeAttribute(ATTRIBUTE_VERSION,
                             AString::number(XML_VERSION_ONE));
    
    const int32_t numberOfAnnotations = annotationFile->getNumberOfAnnotations();
    for (int32_t i = 0; i < numberOfAnnotations; i++) {
        const Annotation* annotation = annotationFile->getAnnotation(i);
        CaretAssert(annotation);
        
        switch (annotation->getType()) {
            case AnnotationTypeEnum::ARROW:
                writeArrow(dynamic_cast<const AnnotationArrow*>(annotation));
                break;
            case AnnotationTypeEnum::BOX:
                writeBox(dynamic_cast<const AnnotationBox*>(annotation));
                break;
            case AnnotationTypeEnum::IMAGE:
                writeImage(dynamic_cast<const AnnotationImage*>(annotation));
                break;
            case AnnotationTypeEnum::LINE:
                writeLine(dynamic_cast<const AnnotationLine*>(annotation));
                break;
            case AnnotationTypeEnum::OVAL:
                writeOval(dynamic_cast<const AnnotationOval*>(annotation));
                break;
            case AnnotationTypeEnum::TEXT:
                writeText(dynamic_cast<const AnnotationText*>(annotation));
                break;
        }
    }
    
    m_stream->writeEndElement(); // ELEMENT_ANNOTATION_FILE
    
    m_stream->writeEndDocument();
    
    file.close();
    
    if (m_stream->hasError()) {
        throw DataFileException(annotationFile->getFileName(),
                                "There was an error writing the annotation file in XML format (reported by QXmlStreamWriter).");
    }
}

/**
 * Write the given annotation arrow in XML.
 *
 * @param arrow
 *     The annotation arrow.
 */
void
AnnotationFileXmlWriter::writeArrow(const AnnotationArrow* arrow)
{
    CaretAssert(arrow);
    
    writeOneDimensionalAnnotation(arrow,
                                  ELEMENT_ARROW);
}

/**
 * Write the given annotation box in XML.
 *
 * @param box
 *     The annotation box.
 */
void
AnnotationFileXmlWriter::writeBox(const AnnotationBox* box)
{
    CaretAssert(box);
    
    writeTwoDimensionalAnnotation(box,
                                  ELEMENT_BOX);
}

/**
 * Write the given annotation image in XML.
 *
 * @param image
 *     The annotation image.
 */
void
AnnotationFileXmlWriter::writeImage(const AnnotationImage* image)
{
    CaretAssert(image);
}

/**
 * Write the given annotation line in XML.
 *
 * @param line
 *     The annotation line.
 */
void
AnnotationFileXmlWriter::writeLine(const AnnotationLine* line)
{
    CaretAssert(line);
    
    writeOneDimensionalAnnotation(line,
                                  ELEMENT_LINE);
}

/**
 * Write the given annotation oval in XML.
 *
 * @param oval
 *     The annotation oval.
 */
void
AnnotationFileXmlWriter::writeOval(const AnnotationOval* oval)
{
    CaretAssert(oval);
    
    writeTwoDimensionalAnnotation(oval,
                                  ELEMENT_OVAL);
}

/**
 * Write the given annotation text in XML.
 *
 * @param text
 *     The annotation text.
 */
void
AnnotationFileXmlWriter::writeText(const AnnotationText* text)
{
    CaretAssert(text);

    QXmlStreamAttributes attributes;
    getTwoDimAnnotationPropertiesAsAttributes(text,
                                              attributes);
    
    
    QXmlStreamAttributes textDataAttributes;
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_BOLD,
                              AString::fromBool(text->isBoldEnabled()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_ITALIC,
                              AString::fromBool(text->isItalicEnabled()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_NAME,
                              AnnotationFontNameEnum::toName(text->getFont()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_SIZE,
                              AnnotationFontSizeEnum::toName(text->getFontSize()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_UNDERLINE,
                              AString::fromBool(text->isUnderlineEnabled()));
    textDataAttributes.append(ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT,
                              AnnotationTextAlignHorizontalEnum::toName(text->getHorizontalAlignment()));
    textDataAttributes.append(ATTRIBUTE_TEXT_ORIENTATION,
                              AnnotationTextOrientationEnum::toName(text->getOrientation()));
    textDataAttributes.append(ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT,
                              AnnotationTextAlignVerticalEnum::toName(text->getVerticalAlignment()));
    
    
    m_stream->writeStartElement(ELEMENT_TEXT);

    m_stream->writeAttributes(attributes);
    
    writeCoordinate(text->getCoordinate(),
                    ELEMENT_COORDINATE_ONE);
    
    m_stream->writeStartElement(ELEMENT_TEXT_DATA);
    m_stream->writeAttributes(textDataAttributes);
    m_stream->writeCDATA(text->getText());
    m_stream->writeEndElement();
    
    m_stream->writeEndElement();
}

/*
 * Get the annotation's properties in XML attributes.
 *
 * @param annotation
 *    The Annotation.
 * @param attributes
 *    XML attributes to which properties are appended.
 */
void
AnnotationFileXmlWriter::getAnnotationPropertiesAsAttributes(const Annotation* annotation,
                                                             QXmlStreamAttributes& attributes)
{
    CaretAssert(annotation);
    
    attributes.append(ATTRIBUTE_COORDINATE_SPACE,
                      AnnotationCoordinateSpaceEnum::toName(annotation->getCoordinateSpace()));
    
    attributes.append(ATTRIBUTE_BACKGROUND_CARET_COLOR,
                      CaretColorEnum::toName(annotation->getBackgroundColor()));
    
    float rgba[4];
    annotation->getCustomBackgroundColor(rgba);
    attributes.append(ATTRIBUTE_BACKGROUND_CUSTOM_RGBA,
                      realArrayToString(rgba, 4));
    
    attributes.append(ATTRIBUTE_FOREGROUND_CARET_COLOR,
                      CaretColorEnum::toName(annotation->getForegroundColor()));
    
    annotation->getCustomForegroundColor(rgba);
    attributes.append(ATTRIBUTE_FOREGROUND_CUSTOM_RGBA,
                      realArrayToString(rgba, 4));
    
    attributes.append(ATTRIBUTE_FOREGROUND_LINE_WIDTH,
                      QString::number(annotation->getForegroundLineWidth()));
    
    attributes.append(ATTRIBUTE_TAB_INDEX,
                      QString::number(annotation->getTabIndex()));
    
    attributes.append(ATTRIBUTE_WINDOW_INDEX,
                      QString::number(annotation->getWindowIndex()));
    
    
}

/*
 * Get the two-dim annotation's properties in XML attributes.
 *
 * @param shape
 *    The two-dim annotation.
 * @param attributes
 *    XML attributes to which properties are appended.
 */
void
AnnotationFileXmlWriter::getTwoDimAnnotationPropertiesAsAttributes(const AnnotationTwoDimensionalShape* shape,
                                               QXmlStreamAttributes& attributes)
{
    getAnnotationPropertiesAsAttributes(shape,
                                        attributes);
    
    attributes.append(ATTRIBUTE_WIDTH,
                      QString::number(shape->getWidth()));
    
    attributes.append(ATTRIBUTE_HEIGHT,
                      QString::number(shape->getHeight()));
    
    attributes.append(ATTRIBUTE_ROTATION_ANGLE,
                      QString::number(shape->getRotationAngle()));
}

/**
 * Write the given two dimensional annotation in XML.
 *
 * @param shape
 *     The two-dimensional annotation.
 * @param annotationXmlElement
 *     The XML element for the annotation.
 */
void
AnnotationFileXmlWriter::writeTwoDimensionalAnnotation(const AnnotationTwoDimensionalShape* shape,
                                                       const QString& annotationXmlElement)
{
    CaretAssert(shape);
    
    QXmlStreamAttributes attributes;
    getTwoDimAnnotationPropertiesAsAttributes(shape,
                                              attributes);
    
    m_stream->writeStartElement(annotationXmlElement);
    
    m_stream->writeAttributes(attributes);
    
    writeCoordinate(shape->getCoordinate(),
                    ELEMENT_COORDINATE_ONE);
    
    m_stream->writeEndElement();
}


/**
 * Write the given one dimensional annotation in XML.
 *
 * @param shape
 *     The one-dimensional annotation.
 * @param annotationXmlElement
 *     The XML element for the annotation.
 */
void
AnnotationFileXmlWriter::writeOneDimensionalAnnotation(const AnnotationOneDimensionalShape* shape,
                                                       const QString& annotationXmlElement)
{
    CaretAssert(shape);
    
    QXmlStreamAttributes attributes;
    getAnnotationPropertiesAsAttributes(shape,
                                        attributes);
    
    m_stream->writeStartElement(annotationXmlElement);
    
    m_stream->writeAttributes(attributes);
    
    writeCoordinate(shape->getStartCoordinate(),
                    ELEMENT_COORDINATE_ONE);
    
    writeCoordinate(shape->getStartCoordinate(),
                    ELEMENT_COORDINATE_TWO);
    
    m_stream->writeEndElement();
}

/**
 * Write the given annotation coordinate in XML.
 *
 * @param coordinate
 *     The annotation coordinate.
 * @param coordinateXmlElement
 *     The XML element for the annotation coordinate.
 */
void
AnnotationFileXmlWriter::writeCoordinate(const AnnotationCoordinate* coordinate,
                                         const QString& coordinateXmlElement)
{
    CaretAssert(coordinate);
    
    float xyz[3];
    coordinate->getXYZ(xyz);
    
    StructureEnum::Enum structure = StructureEnum::INVALID;
    int32_t numberOfNodes = -1;
    int32_t nodeIndex     = -1;
    coordinate->getSurfaceSpace(structure,
                                numberOfNodes,
                                nodeIndex);
    
    m_stream->writeStartElement(coordinateXmlElement);
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_X,
                             realToString(xyz[0]));
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_Y,
                             realToString(xyz[1]));
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_Z,
                             realToString(xyz[2]));
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_SURFACE_STRUCTURE,
                             StructureEnum::toName(structure));
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_SURFACE_NUMBER_OF_NODES,
                             QString::number(numberOfNodes));
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_SURFACE_NODE_INDEX,
                             QString::number(nodeIndex));
    
    m_stream->writeEndElement();
}

/**
 * Convert a real value to a string format.
 *
 * @param value
 *     Value that is converted
 * @return
 *     String format for value.
 */
QString
AnnotationFileXmlWriter::realToString(const float value) const
{
    return QString::number(value, 'f', 6);
}

/**
 * Convert an array of real values to string format.
 *
 * @param values
 *     Array of values that are converted
 * @return
 *     String format for values
 */
QString
AnnotationFileXmlWriter::realArrayToString(const float values[],
                                           const int32_t numberOfValues) const
{
    QString text;
    
    for (int32_t i = 0; i < numberOfValues; i++) {
        if (i > 0) {
            text.append(";");
        }
        text.append(realToString(values[i]));
    }
                    
    return text;
}



