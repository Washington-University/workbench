
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

#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationGroup.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "CaretAssert.h"
#include "DataFileException.h"
#include "XmlStreamWriterHelper.h"
#include "XmlUtilities.h"

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
    m_streamHelper.grabNew(NULL);
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
    
    const QString filename = annotationFile->getFileName();
    if (filename.isEmpty()) {
        throw DataFileException("Name for writing annotation file is empty.");
    }
    
    /*
     * Open the file
     */
    QFile file(filename);
    if ( ! file.open(QFile::WriteOnly)) {
        throw DataFileException(filename,
                                "Error opening for writing: "
                                + file.errorString());
    }
    
    /*
     * Create an XML stream writer
     */
    m_stream.grabNew(new QXmlStreamWriter(&file));

    writeFileContentToXmlStreamWriter(annotationFile,
                                      filename);
    
    
    file.close();
    
    if (m_stream->hasError()) {
        throw DataFileException(filename,
                                "There was an error writing the annotation file in XML format (reported by QXmlStreamWriter).");
    }
}

/**
 * Write the given Annotation File in XML format into the given string.
 *
 * @param annotationFile
 *     The annotation file written to the stream writer.
 * @param fileContentString
 *     Will contain XML version of the file on exit.
 * @throws
 *     DataFileException if there is an error writing the file.
 */
void
AnnotationFileXmlWriter::writeFileToString(const AnnotationFile* annotationFile,
                                           QString& fileContentString)
{
    fileContentString.clear();
    
    m_stream.grabNew(new QXmlStreamWriter(&fileContentString));
    
    writeFileContentToXmlStreamWriter(annotationFile,
                                      "Scene Annotation File");
    
    if (m_stream->hasError()) {
        throw DataFileException("There was an error writing the scene annotation file in XML format (reported by QXmlStreamWriter).");
    }
}

/**
 * Write the content of the file to the XML Stream Writer.
 *
 * @param annotationFile
 *     The annotation file written to the stream writer.
 * @param filename
 *     Name of the file.
 */
void
AnnotationFileXmlWriter::writeFileContentToXmlStreamWriter(const AnnotationFile* annotationFile,
                                                           const QString& filename)
{
    CaretAssert(m_stream);
    m_stream->setAutoFormatting(true);
    
    /*
     * Create the helper for reading XML
     */
    if (m_streamHelper != NULL) {
        m_streamHelper.grabNew(NULL);
    }
    m_streamHelper.grabNew(new XmlStreamWriterHelper(filename,
                                                     m_stream));
    
    m_stream->writeStartDocument();
    
    m_stream->writeStartElement(ELEMENT_ANNOTATION_FILE);
    m_stream->writeAttribute(ATTRIBUTE_VERSION,
                             AString::number(XML_VERSION_TWO));
    
    m_streamHelper->writeMetaData(annotationFile->getFileMetaData());
    
//    std::vector<Annotation*> annotations;
//    annotationFile->getAllAnnotations(annotations);
//    const int32_t numberOfAnnotations = static_cast<int32_t>(annotations.size());
//    for (int32_t i = 0; i < numberOfAnnotations; i++) {
//        CaretAssertVectorIndex(annotations, i);
//        const Annotation* annotation = annotations[i];
//        CaretAssert(annotation);
//        
//        switch (annotation->getType()) {
//            case AnnotationTypeEnum::BOX:
//                writeBox(dynamic_cast<const AnnotationBox*>(annotation));
//                break;
//            case AnnotationTypeEnum::COLOR_BAR:
//                CaretAssertMessage(0, "Color bar is NEVER written to an annotation file");
//                break;
//            case AnnotationTypeEnum::IMAGE:
//                writeImage(dynamic_cast<const AnnotationImage*>(annotation));
//                break;
//            case AnnotationTypeEnum::LINE:
//                writeLine(dynamic_cast<const AnnotationLine*>(annotation));
//                break;
//            case AnnotationTypeEnum::OVAL:
//                writeOval(dynamic_cast<const AnnotationOval*>(annotation));
//                break;
//            case AnnotationTypeEnum::TEXT:
//                writeText(dynamic_cast<const AnnotationText*>(annotation));
//                break;
//        }
//    }

    std::vector<AnnotationGroup*> annotationGroups;
    annotationFile->getAllAnnotationGroups(annotationGroups);
    
    for (std::vector<AnnotationGroup*>::iterator groupIter = annotationGroups.begin();
         groupIter != annotationGroups.end();
         groupIter++) {
        writeGroup(*groupIter);
    }

    m_stream->writeEndElement(); // ELEMENT_ANNOTATION_FILE
    
    m_stream->writeEndDocument();
}

/**
 * Write the given annotation group in XML.
 *
 * @param group
 *     The annotation group.
 */
void
AnnotationFileXmlWriter::writeGroup(const AnnotationGroup* group)
{
    CaretAssert(group);
    
    if (group->isEmpty()) {
        return;
    }
    
    m_stream->writeStartElement(ELEMENT_GROUP);
    
    m_stream->writeAttribute(ATTRIBUTE_COORDINATE_SPACE,
                             AnnotationCoordinateSpaceEnum::toName(group->getCoordinateSpace()));
    m_stream->writeAttribute(ATTRIBUTE_GROUP_TYPE,
                             AnnotationGroupTypeEnum::toName(group->getGroupType()));
    m_stream->writeAttribute(ATTRIBUTE_TAB_OR_WINDOW_INDEX,
                             QString::number(group->getTabOrWindowIndex()));
    m_stream->writeAttribute(ATTRIBUTE_UNIQUE_KEY,
                             QString::number(group->getUniqueKey()));
    
    std::vector<Annotation*> annotations;
    group->getAllAnnotations(annotations);
    
    for (std::vector<Annotation*>::iterator annIter = annotations.begin();
         annIter != annotations.end();
         annIter++) {
        const Annotation* annotation = *annIter;
        CaretAssert(annotation);
        
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
                writeBox(dynamic_cast<const AnnotationBox*>(annotation));
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                CaretAssertMessage(0, "Color bar is NEVER written to an annotation file");
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
    
    //    const AString indicesString = AString::fromNumbers(group->getAnnotationUniqueIdentifiers(), " ");
    //    m_stream->writeCharacters(indicesString);
    
    m_stream->writeEndElement();
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

    const int32_t imageWidth  = image->getImageWidth();
    const int32_t imageHeight = image->getImageHeight();
    const int32_t numberOfBytes = imageWidth * imageHeight * 4;
    if (numberOfBytes <= 0) {
        return;
    }
    
    const uint8_t* imageBytesRGBA = image->getImageBytesRGBA();
    
    QByteArray byteArray((const char*)imageBytesRGBA,
                         numberOfBytes);
    QByteArray byteArrayBase64(byteArray.toBase64());
    const QString stringBase64(QString::fromLatin1(byteArrayBase64.constData(),
                                                   byteArrayBase64.size()));

    QXmlStreamAttributes attributes;
    getTwoDimAnnotationPropertiesAsAttributes(image,
                                              attributes);
    
    
    QXmlStreamAttributes imageDataAttributes;
    imageDataAttributes.append(ATTRIBUTE_IMAGE_WIDTH,
                              AString::number(image->getImageWidth()));
    imageDataAttributes.append(ATTRIBUTE_IMAGE_HEIGHT,
                              AString::number(image->getImageHeight()));
   
    
    m_stream->writeStartElement(ELEMENT_IMAGE);
    m_stream->writeAttributes(attributes);

    writeCoordinate(image->getCoordinate(),
                    ELEMENT_COORDINATE_ONE);
    
    m_stream->writeStartElement(ELEMENT_IMAGE_RGBA_BYTES_IN_BASE64);
    m_stream->writeAttributes(imageDataAttributes);
    m_stream->writeCharacters(stringBase64);
    m_stream->writeEndElement();
    
    m_stream->writeEndElement();
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
    textDataAttributes.append(ATTRIBUTE_TEXT_CARET_COLOR,
                      CaretColorEnum::toName(text->getTextColor()));
    
    float rgba[4];
    text->getCustomTextColor(rgba);
    textDataAttributes.append(ATTRIBUTE_TEXT_CUSTOM_RGBA,
                      realArrayToString(rgba, 4));
    
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_BOLD,
                              AString::fromBool(text->isBoldStyleEnabled()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_ITALIC,
                              AString::fromBool(text->isItalicStyleEnabled()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_NAME,
                              AnnotationTextFontNameEnum::toName(text->getFont()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_POINT_SIZE,
                              AnnotationTextFontPointSizeEnum::toName(text->getFontPointSizeProtected()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_UNDERLINE,
                              AString::fromBool(text->isUnderlineStyleEnabled()));
    textDataAttributes.append(ATTRIBUTE_TEXT_HORIZONTAL_ALIGNMENT,
                              AnnotationTextAlignHorizontalEnum::toName(text->getHorizontalAlignment()));
    textDataAttributes.append(ATTRIBUTE_TEXT_ORIENTATION,
                              AnnotationTextOrientationEnum::toName(text->getOrientation()));
    textDataAttributes.append(ATTRIBUTE_TEXT_VERTICAL_ALIGNMENT,
                              AnnotationTextAlignVerticalEnum::toName(text->getVerticalAlignment()));
    textDataAttributes.append(ATTRIBUTE_TEXT_FONT_PERCENT_VIEWPORT_SIZE,
                              AString::number(text->getFontPercentViewportSizeProtected()));
    textDataAttributes.append(ATTRIBUTE_TEXT_CONNECT_BRAINORDINATE,
                              AnnotationTextConnectTypeEnum::toName(text->getConnectToBrainordinate()));
    
    QString annotationElementName;
    switch (text->getFontSizeType()) {
        case AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT:
            annotationElementName = ELEMENT_PERCENT_SIZE_TEXT;
            break;
        case AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_WIDTH:
            annotationElementName = ELEMENT_PERCENT_WIDTH_SIZE_TEXT;
            break;
        case AnnotationTextFontSizeTypeEnum::POINTS:
            annotationElementName = ELEMENT_POINT_SIZE_TEXT;
            break;
    }
    CaretAssert( ! annotationElementName.isEmpty());
    
    m_stream->writeStartElement(annotationElementName);

    m_stream->writeAttributes(attributes);
    
    writeCoordinate(text->getCoordinate(),
                    ELEMENT_COORDINATE_ONE);
    
    /*
     * Write the text data.
     * Note: QXmlStreamWriter::writeCharacters() will replace special 
     * characters (& " ' < >) with escape sequences
     */
    m_stream->writeStartElement(ELEMENT_TEXT_DATA);
    m_stream->writeAttributes(textDataAttributes);
    m_stream->writeCharacters(text->getText());
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
                      CaretColorEnum::toName(annotation->getLineColor()));
    
    annotation->getCustomLineColor(rgba);
    attributes.append(ATTRIBUTE_FOREGROUND_CUSTOM_RGBA,
                      realArrayToString(rgba, 4));
    
    attributes.append(ATTRIBUTE_FOREGROUND_LINE_WIDTH,
                      QString::number(annotation->getLineWidth()));
    
    attributes.append(ATTRIBUTE_TAB_INDEX,
                      QString::number(annotation->getTabIndex()));
    
    attributes.append(ATTRIBUTE_WINDOW_INDEX,
                      QString::number(annotation->getWindowIndex()));
    
    attributes.append(ATTRIBUTE_UNIQUE_KEY,
                      QString::number(annotation->getUniqueKey()));
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
    CaretAssert(shape);
    
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
    
    const AnnotationLine* line = dynamic_cast<const AnnotationLine*>(shape);
    if (line != NULL) {
        attributes.append(ATTRIBUTE_LINE_END_ARROW,
                          AString::fromBool(line->isDisplayEndArrow()));
        attributes.append(ATTRIBUTE_LINE_START_ARROW,
                          AString::fromBool(line->isDisplayStartArrow()));
    }
    
    m_stream->writeStartElement(annotationXmlElement);
    
    m_stream->writeAttributes(attributes);
    
    writeCoordinate(shape->getStartCoordinate(),
                    ELEMENT_COORDINATE_ONE);
    
    writeCoordinate(shape->getEndCoordinate(),
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
    float nodeOffset      = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
    AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorType = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
    coordinate->getSurfaceSpace(structure,
                                numberOfNodes,
                                nodeIndex,
                                nodeOffset,
                                surfaceOffsetVectorType);
    
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
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_SURFACE_NODE_OFFSET,
                             realToString(nodeOffset));
    
    m_stream->writeAttribute(ATTRIBUTE_COORD_SURFACE_NODE_OFFSET_VECTOR_TYPE,
                             AnnotationSurfaceOffsetVectorTypeEnum::toName(surfaceOffsetVectorType));
    
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



