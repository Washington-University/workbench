
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __CZI_IMAGE_FILE_META_DATA_XML_READER_DECLARE__
#include "CziImageFileMetaDataXmlReader.h"
#undef __CZI_IMAGE_FILE_META_DATA_XML_READER_DECLARE__

#include <QXmlStreamReader>

#include "AnnotationArrow.h"
#include "AnnotationCoordinate.h"
#include "AnnotationBox.h"
#include "AnnotationFile.h"
#include "AnnotationLine.h"
#include "AnnotationMarker.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPolyLine.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziImageFile.h"
#include "EventAnnotationAddToRemoveFromFile.h"
#include "EventManager.h"
#include "XmlStreamReaderHelper.h"

using namespace caret;


    
/**
 * \class caret::CziImageFileMetaDataXmlReader 
 * \brief Read select metadata from a CZI File
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziImageFileMetaDataXmlReader::CziImageFileMetaDataXmlReader()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
CziImageFileMetaDataXmlReader::~CziImageFileMetaDataXmlReader()
{
}

/**
 * Parse the XML from the CZI file in the given xml text
 * @param cziImageFile
 *    The CZI image file
 * @param xmlText
 *    The XML text
 */
FunctionResultValue<AnnotationFile*>
CziImageFileMetaDataXmlReader::readXmlFromString(CziImageFile* cziImageFile,
                                                 const AString& xmlText)
{
    CaretAssert(cziImageFile);
    m_cziImageFileNameNoPath = cziImageFile->getFileNameNoPath();
    
    m_imageLogicalX = cziImageFile->getLogicalBoundsRect().x();
    m_imageLogicalY = cziImageFile->getLogicalBoundsRect().y();
    m_imageWidth  = cziImageFile->getLogicalBoundsRect().width();
    m_imageHeight = cziImageFile->getLogicalBoundsRect().height();
    if ((m_imageWidth < 1)
        || (m_imageHeight < 1)) {
        const QString errorMessage("Image size invalid.  Width="
                                   + AString::number(m_imageWidth)
                                   + ", Height="
                                   + AString::number(m_imageHeight));
        return FunctionResultValue<AnnotationFile*>(NULL,
                                                    errorMessage,
                                                    false);
    }
    
    m_annotationFile.reset(new AnnotationFile());
    
    m_reader.reset(new QXmlStreamReader(xmlText));
    m_helper.reset(new XmlStreamReaderHelper(m_cziImageFileNameNoPath,
                                             m_reader.get()));
    
    parseXML();
    
    if (m_reader->hasError()) {
        const QString errorMessage("Error reading XML from "
                                   + m_cziImageFileNameNoPath
                                   + ": "
                                   + m_reader->errorString()
                                   + " Line: "
                                   + AString::number(m_reader->lineNumber())
                                   + " Column: "
                                   + AString::number(m_reader->columnNumber()));
        return FunctionResultValue<AnnotationFile*>(NULL,
                                                    errorMessage,
                                                    false);
    }
    
    if ( ! m_unrecognizedShapeNames.empty()) {
        AString txt("Unrecgonized shapes in "
                    + m_cziImageFileNameNoPath
                    + ":");
        for (const AString& s : m_unrecognizedShapeNames) {
            txt.appendWithNewLine("   " + s);
        }
        CaretLogSevere(txt);
    }

    /*
     * Note that an empty annotation file is not an error
     */
    return FunctionResultValue<AnnotationFile*>(m_annotationFile.release());
}

/**
 * Parse the XML
 */
void
CziImageFileMetaDataXmlReader::parseXML()
{
    bool endElementFoundFlag(false);
    
    const QString layersTagName("Layers");
    const QString layerTagName("Layer");
    const QString elementTagName("Elements");
    
    enum class State {
        LAYERS,
        LAYER,
        ELEMENTS,
        OTHER
    };
    
    State state = State::OTHER;
    
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == layersTagName) {
                    if (state == State::OTHER) {
                        state = State::LAYERS;
                    }
                    else {
                        m_reader->raiseError(layersTagName
                                             + " start tag encountered in unexpected location");
                    }
                }
                else if (m_reader->name() == layerTagName) {
                    if (state == State::LAYERS) {
                        state = State::LAYER;
                    }
                    else {
                        m_reader->raiseError(layerTagName
                                             + " start tag encountered in unexpected location");
                    }
                }
                else if (m_reader->name() == elementTagName) {
                    if (state == State::LAYER) {
                        state = State::ELEMENTS;
                    }
                    else {
                        m_reader->raiseError(elementTagName
                                             + " start tag encountered in unexpected location");
                    }
                }
                else {
                    if (state == State::ELEMENTS) {
                        const AString idName = m_reader->attributes().value("Id").toString();
                        readShape(m_reader->name().toString(),
                                  idName);
                    }
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == layersTagName) {
                    if (state == State::LAYERS) {
                        state = State::OTHER;
                    }
                    else {
                        m_reader->raiseError(layersTagName
                                             + " end tag encountered in unexpected location");
                    }
                }
                else if (m_reader->name() == layerTagName) {
                    if (state == State::LAYER) {
                        state = State::LAYERS;
                    }
                    else {
                        m_reader->raiseError(layerTagName
                                             + " end tag encountered in unexpected location");
                    }
                }
                else if (m_reader->name() == elementTagName) {
                    if (state == State::ELEMENTS) {
                        state = State::LAYER;
                    }
                    else {
                        m_reader->raiseError(elementTagName
                                             + " end tag encountered in unexpected location");
                    }
                }
                break;
            default:
                break;
        }
    }
}

/**
 * Read a shape with the given tag name
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShape(const AString& shapeTagName,
                                         const AString& idName)
{
    if (shapeTagName == "Arrow") {
        readShapeArrow(shapeTagName,
                       idName);
    }
    else if (shapeTagName == "Circle") {
        readShapeCircle(shapeTagName,
                        idName);
    }
    else if (shapeTagName == "Ellipse") {
        readShapeEllipse(shapeTagName,
                         idName);
    }
    else if (shapeTagName == "Events") {
        readShapeEvents(shapeTagName,
                        idName);
    }
    else if (shapeTagName == "Line") {
        readShapeLine(shapeTagName,
                      idName);
    }
    else if (shapeTagName == "Marker") {
        readShapeMarker(shapeTagName,
                        idName);
    }
    else if (shapeTagName == "Rectangle") {
        readShapeRectangle(shapeTagName,
                           idName);
    }
    else if (shapeTagName == "ScaleBar") {
        readShapeScaleBar(shapeTagName,
                          idName);
    }
    else if (shapeTagName == "TextBox") {
        readShapeTextBox(shapeTagName,
                         idName);
    }
    else {
        m_unrecognizedShapeNames.insert(shapeTagName);
        m_reader->skipCurrentElement();
    }
}


/**
 * Read an arrow
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeArrow(const AString& shapeTagName,
                                              const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString strokeText;
    AString strokeThicknessText;
    AString x1Text;
    AString y1Text;
    AString x2Text;
    AString y2Text;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("Stroke")) {
                    strokeText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("StrokeThickness")) {
                    strokeThicknessText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("X1")) {
                    x1Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("X2")) {
                    x2Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Y1")) {
                    y1Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Y2")) {
                    y2Text = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (strokeText.isNotEmpty()
        && strokeThicknessText.isNotEmpty()
        && x1Text.isNotEmpty()
        && x2Text.isNotEmpty()
        && y1Text.isNotEmpty()
        && y2Text.isNotEmpty()) {
        const std::array<uint8_t, 4> rgba(colorToRgba(strokeText));
        float lineWidth(strokeThicknessText.toFloat()
                        / static_cast<float>(m_imageHeight));
        if (lineWidth < 0.5) {
            lineWidth = 0.5;
        }
        
        const Vector3D p1(x1Text.toFloat(),
                          y1Text.toFloat(),
                          0.0);
        const Vector3D p2(x2Text.toFloat(),
                          y2Text.toFloat(),
                          0.0);
        
        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::ARROW,
                                                        AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationArrow* arrow(dynamic_cast<AnnotationArrow*>(ann));
        CaretAssert(arrow);
        arrow->setCziName("Arrow_" + idName);
        arrow->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        arrow->setCustomLineColor(rgba.data());
        arrow->setLineColor(CaretColorEnum::CUSTOM);
        arrow->setLineWidthPercentage(lineWidth);
        arrow->getStartCoordinate()->setXYZ(p2);
        arrow->getEndCoordinate()->setXYZ(p1);

        addAnnotation(ann);
    }
}

/**
 * Read a circle
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeCircle(const AString& shapeTagName,
                                               const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString centerXText;
    AString centerYText;
    AString radiusText;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("CenterX")) {
                    centerXText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("CenterY")) {
                    centerYText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Radius")) {
                    radiusText = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (centerXText.isNotEmpty()
        && centerYText.isNotEmpty()
        && radiusText.isNotEmpty()) {
        
        const Vector3D xyz(centerXText.toFloat(),
                           centerYText.toFloat(),
                           0.0);
        
        CaretAssert(m_imageWidth > 0.0);
        CaretAssert(m_imageHeight > 0.0);
        const float radius(radiusText.toFloat());
        const float width(radius
                          / static_cast<float>(m_imageWidth)
                          * 100.0);
        const float ratio(static_cast<float>(m_imageWidth)
                          / static_cast<float>(m_imageHeight));
        const float height(radius
                           / static_cast<float>(m_imageHeight)
                           * 100.0
                           * ratio);
        
        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::OVAL,
                                                        AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationOval* oval(dynamic_cast<AnnotationOval*>(ann));
        CaretAssert(oval);
        oval->setCziName("Circle_" + idName);
        oval->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        oval->getCoordinate(0)->setXYZ(xyz);
        oval->setWidth(width);
        oval->setHeight(height);
        oval->setLineColor(CaretColorEnum::RED);
        oval->setBackgroundColor(CaretColorEnum::NONE);
        
        addAnnotation(ann);
    }
}

/**
 * Read an ellipse
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeEllipse(const AString& shapeTagName,
                                                const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString centerXText;
    AString centerYText;
    AString radiusXText;
    AString radiusYText;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("CenterX")) {
                    centerXText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("CenterY")) {
                    centerYText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("RadiusX")) {
                    radiusXText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("RadiusY")) {
                    radiusYText = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (centerXText.isNotEmpty()
        && centerYText.isNotEmpty()
        && radiusXText.isNotEmpty()
        && radiusYText.isNotEmpty()) {
        
        const Vector3D xyz(centerXText.toFloat(),
                           centerYText.toFloat(),
                           0.0);
        
        CaretAssert(m_imageWidth > 0.0);
        CaretAssert(m_imageHeight > 0.0);
        const float radiusX(radiusXText.toFloat());
        const float radiusY(radiusYText.toFloat());
        const float width(radiusX
                          / static_cast<float>(m_imageWidth)
                          * 100.0);
        const float ratio(static_cast<float>(m_imageWidth)
                          / static_cast<float>(m_imageHeight));
        const float height(radiusY
                           / static_cast<float>(m_imageHeight)
                           * 100.0
                           * ratio);
        
        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::OVAL,
                                                        AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationOval* ellipse(dynamic_cast<AnnotationOval*>(ann));
        CaretAssert(ellipse);
        ellipse->setCziName("Ellipse" + idName);
        ellipse->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        ellipse->getCoordinate(0)->setXYZ(xyz);
        ellipse->setWidth(width);
        ellipse->setHeight(height);
        ellipse->setLineColor(CaretColorEnum::RED);
        ellipse->setBackgroundColor(CaretColorEnum::NONE);
        
        addAnnotation(ann);
    }
}

/**
 * Read an events shape
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeEvents(const AString& shapeTagName,
                                               const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString foregroundText;
    AString strokeText;
    AString pointsText;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("Foreground")) {
                    foregroundText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Stroke")) {
                    strokeText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Points")) {
                    pointsText = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (foregroundText.isEmpty()
        && strokeText.isEmpty()) {
        foregroundText = "#FF0000FF";
    }
    else {
        if (strokeText.isNotEmpty()) {
            foregroundText = strokeText;
        }
    }
    if (foregroundText.isNotEmpty()
        && pointsText.isNotEmpty()) {
        const std::array<uint8_t, 4> rgba(colorToRgba(strokeText));
        const float lineWidth(0.5);
        const float pctHeight(2.5);
        const std::vector<Vector3D> xyz(pointsToXYZ(pointsText));
        
        for (const Vector3D& v : xyz) {
            
            Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::MARKER,
                                                            AnnotationAttributesDefaultTypeEnum::USER));
            AnnotationMarker* marker(dynamic_cast<AnnotationMarker*>(ann));
            CaretAssert(marker);
            marker->setCziName("Event_" + idName);
            marker->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
            marker->setCustomLineColor(rgba.data());
            marker->setLineColor(CaretColorEnum::CUSTOM);
            marker->setLineWidthPercentage(lineWidth);
            marker->setWidth(pctHeight);
            marker->setHeight(pctHeight);
            marker->getCoordinate()->setXYZ(v);
            
            addAnnotation(ann);
        }
    }
}

/**
 * Read a line
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeLine(const AString& shapeTagName,
                                             const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString strokeText;
    AString strokeThicknessText;
    AString x1Text;
    AString y1Text;
    AString x2Text;
    AString y2Text;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("Stroke") ){
                    strokeText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("StrokeThickness")) {
                    strokeThicknessText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("X1")) {
                    x1Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("X2")) {
                    x2Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Y1")) {
                    y1Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Y2")) {
                    y2Text = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (strokeText.isNotEmpty()
        && strokeThicknessText.isNotEmpty()
        && x1Text.isNotEmpty()
        && x2Text.isNotEmpty()
        && y1Text.isNotEmpty()
        && y2Text.isNotEmpty()) {
        const std::array<uint8_t, 4> rgba(colorToRgba(strokeText));
        float lineWidth(strokeThicknessText.toFloat()
                        / static_cast<float>(m_imageHeight));
        if (lineWidth < 0.5) {
            lineWidth = 0.5;
        }
        
        const Vector3D p1(x1Text.toFloat(),
                          y1Text.toFloat(),
                          0.0);
        const Vector3D p2(x2Text.toFloat(),
                          y2Text.toFloat(),
                          0.0);
                
        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::POLYLINE,
                                                        AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationPolyLine* polyLine(dynamic_cast<AnnotationPolyLine*>(ann));
        CaretAssert(polyLine);
        polyLine->setCziName("Line_" + idName);
        polyLine->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        polyLine->setCustomLineColor(rgba.data());
        polyLine->setLineColor(CaretColorEnum::CUSTOM);
        polyLine->setLineWidthPercentage(lineWidth);
        addPolyLineCoordinate(polyLine, p1);
        addPolyLineCoordinate(polyLine, p2);
        
        addAnnotation(ann);
    }
}

/**
 * Read a marker
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeMarker(const AString& shapeTagName,
                                               const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString strokeText;
    AString strokeThicknessText;
    AString positionText;
    AString renderedSizeText;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("Stroke")) {
                    strokeText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("StrokeThickness")) {
                    strokeThicknessText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Position")) {
                    positionText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("RenderedSize")) {
                    renderedSizeText = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (strokeText.isNotEmpty()
        && strokeThicknessText.isNotEmpty()
        && positionText.isNotEmpty()
        && renderedSizeText.isNotEmpty()) {
        const std::array<uint8_t, 4> rgba(colorToRgba(strokeText));
        float lineWidth(strokeThicknessText.toFloat()
                              / static_cast<float>(m_imageHeight));
        if (lineWidth < 0.5) {
            lineWidth = 0.5;
        }
        const Vector3D renderedSize(xyPositionToXYZ(renderedSizeText));
        
        float pctHeight(renderedSize[1]);
        if ((m_imageWidth > 0)
            && (m_imageHeight > 0)) {
            pctHeight = (renderedSize[1] * 100.0);
        }
        const float minPct(2.5);
        if (pctHeight < minPct) {
            pctHeight = minPct;
        }
        const Vector3D xyz(xyPositionToXYZ(positionText));

        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::MARKER,
                                                        AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationMarker* marker(dynamic_cast<AnnotationMarker*>(ann));
        CaretAssert(marker);
        marker->setCziName("Marker_" + idName);
        marker->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        marker->setCustomLineColor(rgba.data());
        marker->setLineColor(CaretColorEnum::CUSTOM);
        marker->setLineWidthPercentage(lineWidth);
        marker->setWidth(pctHeight);
        marker->setHeight(pctHeight);
        marker->getCoordinate()->setXYZ(xyz);
        
        addAnnotation(ann);
    }
}

/**
 * Read a rectangle
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeRectangle(const AString& shapeTagName,
                                               const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString leftText;
    AString topText;
    AString widthText;
    AString heightText;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("Left")) {
                    leftText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Top")) {
                    topText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Width")) {
                    widthText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Height")) {
                    heightText = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (leftText.isNotEmpty()
        && topText.isNotEmpty()
        && widthText.isNotEmpty()
        && heightText.isNotEmpty()) {
        
        const float left(leftText.toFloat());
        const float top(topText.toFloat());
        const float width(widthText.toFloat());
        const float height(heightText.toFloat());
        const float centerX(left + (width * 0.5));
        const float centerY(top + (height * 0.5));
        
        const Vector3D xyz(centerX,
                           centerY,
                           0.0);
        
        CaretAssert(m_imageWidth > 0.0);
        CaretAssert(m_imageHeight > 0.0);
        const float widthPct(width
                             / static_cast<float>(m_imageWidth)
                             * 100.0);
        const float ratio(static_cast<float>(m_imageWidth)
                          / static_cast<float>(m_imageHeight));
        const float heightPct(height
                              / static_cast<float>(m_imageHeight)
                              * 100.0
                              * ratio);
        
        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::BOX,
                                                        AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationBox* box(dynamic_cast<AnnotationBox*>(ann));
        CaretAssert(box);
        box->setCziName("Rectangle_" + idName);
        box->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        box->getCoordinate(0)->setXYZ(xyz);
        box->setWidth(widthPct);
        box->setHeight(heightPct);
        box->setLineColor(CaretColorEnum::RED);
        box->setBackgroundColor(CaretColorEnum::NONE);
        
        addAnnotation(ann);
    }
}

/**
 * Add a coordinate to a polyline
 * @param polyLine
 *   The poly line
 * @param xyz
 *   Coordinate to add
 */
void
CziImageFileMetaDataXmlReader::addPolyLineCoordinate(AnnotationPolyLine* polyLine,
                                                     const Vector3D& xyz)
{
    AnnotationCoordinate* ac(new AnnotationCoordinate(AnnotationAttributesDefaultTypeEnum::USER));
    ac->setMediaFileNameAndPixelSpace(m_cziImageFileNameNoPath,
                                      xyz);
    polyLine->addCoordinate(ac);
}

/**
 * Read a scale bar
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeScaleBar(const AString& shapeTagName,
                                                 const AString& idName)
{
    std::cout << "Reading scale bar not implemented" << std::endl;
    
    bool endElementFoundFlag(false);
    
    AString orientationText;
    AString textText;
    AString x1Text;
    AString y1Text;
    AString x2Text;
    AString y2Text;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("Orientation")) {
                    orientationText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Text")) {
                    textText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("X1")) {
                    x1Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("X2")) {
                    x2Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Y1")) {
                    y1Text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Y2")) {
                    y2Text = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }

    /* note 'Text' is optional */
    if (orientationText.isNotEmpty()
        && x1Text.isNotEmpty()
        && x2Text.isNotEmpty()
        && y1Text.isNotEmpty()
        && y2Text.isNotEmpty()) {
        
        const Vector3D p1(x1Text.toFloat(),
                          y1Text.toFloat(),
                          0.0);
        const Vector3D p2(x2Text.toFloat(),
                          y2Text.toFloat(),
                          0.0);
        const float scaleBarLength((p1 - p2).length());
        
        /*
         * Create lines at end
         */
        const float tickPercent(0.1);
        Vector3D tickOffsetOne;
        Vector3D tickOffsetTwo;
        if (orientationText == "Horizontal") {
            tickOffsetOne.set(0.0,  scaleBarLength * tickPercent, 0.0);
            tickOffsetTwo.set(0.0, -scaleBarLength * tickPercent, 0.0);
        }
        else if (orientationText == "Vertical") {
            tickOffsetOne.set( scaleBarLength * tickPercent, 0.0, 0.0);
            tickOffsetTwo.set(-scaleBarLength * tickPercent, 0.0, 0.0);
        }
        
        
        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::POLYLINE,
                                                        AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationPolyLine* polyLine(dynamic_cast<AnnotationPolyLine*>(ann));
        CaretAssert(polyLine);
        polyLine->setCziName("ScaleBar" + idName);
        polyLine->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        polyLine->setLineWidthPercentage(0.5);
        addPolyLineCoordinate(polyLine, p1 + tickOffsetOne);
        addPolyLineCoordinate(polyLine, p1 + tickOffsetTwo);
        addPolyLineCoordinate(polyLine, p1);
        addPolyLineCoordinate(polyLine, p2);
        addPolyLineCoordinate(polyLine, p2 + tickOffsetOne);
        addPolyLineCoordinate(polyLine, p2 + tickOffsetTwo);

        addAnnotation(ann);
    }
}

/**
 * Read a text box
 * @param shapeTagName
 *    Name of shape's tag
 * @param idName
 *    Value of ID attribute
 */
void
CziImageFileMetaDataXmlReader::readShapeTextBox(const AString& shapeTagName,
                                                const AString& idName)
{
    bool endElementFoundFlag(false);
    
    AString rotationText;
    AString foregroundText;
    AString fontSizeText;
    AString text;
    AString positionText;
    AString renderedSizeText;
    while ( ( ! m_reader->atEnd())
           && ( ! endElementFoundFlag)) {
        m_reader->readNext();
        
        switch (m_reader->tokenType()) {
            case QXmlStreamReader::StartElement:
                if (m_reader->name() == QLatin1String("Rotation")) {
                    rotationText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Foreground")) {
                    foregroundText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("FontSize")) {
                    fontSizeText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Text")) {
                    text = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("Position")) {
                    positionText = m_reader->readElementText().trimmed();
                }
                else if (m_reader->name() == QLatin1String("RenderedSize")) {
                    renderedSizeText = m_reader->readElementText().trimmed();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (m_reader->name() == shapeTagName) {
                    endElementFoundFlag = true;
                }
                break;
            default:
                break;
        }
    }
    
    if (foregroundText.isNotEmpty()
        && fontSizeText.isNotEmpty()
        && text.isNotEmpty()
        && positionText.isNotEmpty()
        && renderedSizeText.isNotEmpty()) {
        const std::array<uint8_t, 4> rgba(colorToRgba(foregroundText));
        const Vector3D xyz(xyPositionToXYZ(positionText));
        const Vector3D renderedSize(xyPositionToXYZ(renderedSizeText));
        float rotation(rotationText.isNotEmpty()
                       ? rotationText.toFloat()
                       : 0.0);
        
        float percentageHeight(10.0);
        if ((m_imageWidth > 0)
            && (m_imageHeight > 0)) {
            percentageHeight = (renderedSize[1]
                                / static_cast<float>(m_imageHeight));
            percentageHeight *= 100.0;
            CaretAssert((percentageHeight >= 0.0)
                        && (percentageHeight <= 100.0));
            
            const float minPct(2.0);
            if (percentageHeight < minPct) {
                percentageHeight = minPct;
            }
        }
        
        Annotation* ann(Annotation::newAnnotationOfType(AnnotationTypeEnum::TEXT,
                                                           AnnotationAttributesDefaultTypeEnum::USER));
        AnnotationPercentSizeText* textAnn(dynamic_cast<AnnotationPercentSizeText*>(ann));
        CaretAssert(textAnn);
        textAnn->setCziName("Text_" + idName);
        textAnn->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
        textAnn->getCoordinate(0)->setXYZ(xyz);
        textAnn->setCustomTextColor(rgba.data());
        textAnn->setFont(AnnotationTextFontNameEnum::VERA);
        textAnn->setFontPercentViewportSize(percentageHeight);
        textAnn->setText(text);
        textAnn->setTextColor(CaretColorEnum::CUSTOM);
        textAnn->setRotationAngle(rotation);
        textAnn->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
        textAnn->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        
        addAnnotation(ann);
    }
}

/**
 * Convert a CZI color to RGBA
 * @param colorText
 *    Text containing CZI color
 * @return
 *    The RGBA color
 */
std::array<uint8_t, 4>
CziImageFileMetaDataXmlReader::colorToRgba(const AString& colorText)
{
    std::array<uint8_t, 4> rgba;
    rgba.fill(0);
    
    if (colorText.length() == 9) {
        /* color text is like: #FF009933 */
        const int32_t alpha(colorText.mid(1, 2).toInt(nullptr, 16));
        const int32_t red(colorText.mid(3, 2).toInt(nullptr, 16));
        const int32_t green(colorText.mid(5, 2).toInt(nullptr, 16));
        const int32_t blue(colorText.mid(7, 2).toInt(nullptr, 16));
        
        rgba[0] = static_cast<uint8_t>(red);
        rgba[1] = static_cast<uint8_t>(green);
        rgba[2] = static_cast<uint8_t>(blue);
        rgba[3] = static_cast<uint8_t>(alpha);
    }
    else {
        CaretLogWarning("Color text is not length=9: "
                        + colorText);
    }
    return rgba;
}

/**
 * Convert a 2D CZI position to XYZ
 * @param xyPositionText
 *    The XY position
 * @return
 *    The XYZ value
 */
Vector3D
CziImageFileMetaDataXmlReader::xyPositionToXYZ(const AString& xyPositionText)
{
    Vector3D xyz(0.0, 0.0, 0.0);
    
    std::vector<float> numbers;
    AString::toNumbers(xyPositionText,
                       numbers);
    
    if (numbers.size() == 2) {
        CaretAssertVectorIndex(numbers, 1);
        xyz[0] = numbers[0];
        xyz[1] = numbers[1];
    }
    else if (numbers.size() == 3) {
        CaretAssertVectorIndex(numbers, 2);
        xyz[0] = numbers[0];
        xyz[1] = numbers[1];
        xyz[2] = numbers[2];
    }
    else {
        CaretLogWarning("Position text is not 2 numbers: "
                        + xyPositionText);
    }
    
    return xyz;
}

/**
 * Convert a 'points' string to a vector of Vector3D instances
 * @param pointsText
 *    The text containing one or more points
 * @return
 */
std::vector<Vector3D>
CziImageFileMetaDataXmlReader::pointsToXYZ(const AString& pointsText)
{
    std::vector<Vector3D> xyzOut;
    
    /*
     * Points text is in form:
     * <Points>11191.6671726004, 17144.6114758967 10060.0804104662,15893.9103177484</Points>
     */
#if QT_VERSION >= 0x060000
   QStringList indivPointText(pointsText.split(" ",
                                                Qt::SkipEmptyParts));
#else
    QStringList indivPointText(pointsText.split(" ",
                                                QString::SkipEmptyParts));
#endif
    const int32_t numIndivPoints(indivPointText.size());
    for (int32_t i = 0; i < numIndivPoints; i++) {
        xyzOut.push_back(xyPositionToXYZ(indivPointText[i]));
    }
    
    return xyzOut;
}


/**
 * Add an annotation to the annotation file
 * @param annotation
 *    Annotation to add to file
 */
void
CziImageFileMetaDataXmlReader::addAnnotation(Annotation* annotation)
{
    CaretAssert(annotation);
    CaretAssert(m_annotationFile);
    
    /*
     * Need to convert pixels to logical coordinates
     */
    annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL);
    for (int32_t i = 0; i < annotation->getNumberOfCoordinates(); i++) {
        AnnotationCoordinate* ac(annotation->getCoordinate(i));
        Vector3D xyz(ac->getXYZ());
        xyz[0] += m_imageLogicalX;
        xyz[1] += m_imageLogicalY;
        xyz[2]  = 0.0;
        ac->setMediaFileNameAndPixelSpace(m_cziImageFileNameNoPath,
                                          xyz);
    }
    
    /*
     * Annotation file uses a redo/undo system so annotation
     * must be added using an event
     */
    EventAnnotationAddToRemoveFromFile event(EventAnnotationAddToRemoveFromFile::Mode::MODE_CREATE,
                                             m_annotationFile.get(),
                                             annotation);
    EventManager::get()->sendEvent(event.getPointer());
}


