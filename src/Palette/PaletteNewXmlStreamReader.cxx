
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __PALETTE_NEW_XML_STREAM_READER_DECLARE__
#include "PaletteNewXmlStreamReader.h"
#undef __PALETTE_NEW_XML_STREAM_READER_DECLARE__

#include <QFile>
#include <QXmlStreamReader>

#include "CaretAssert.h"
#include "PaletteNew.h"

using namespace caret;
    
/**
 * \class caret::PaletteNewXmlStreamReader 
 * \brief Read a palette from XML
 * \ingroup Palette
 */

/**
 * Constructor.
 */
PaletteNewXmlStreamReader::PaletteNewXmlStreamReader()
: PaletteNewXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
PaletteNewXmlStreamReader::~PaletteNewXmlStreamReader()
{
}

/**
 * Read a palette from the given file name
 * @param filename
 *    Name of file
 * @return
 *    Function result with palette or error
 */
FunctionResultValue<PaletteNew*>
PaletteNewXmlStreamReader::readFromFile(const AString& filename)
{
    if (filename.isEmpty()) {
        return FunctionResultValue<PaletteNew*>(NULL,
                                                "Filename is empty.",
                                                false);
    }
    
    PaletteNew* paletteOut(NULL);
    AString errorMessage;
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
        QXmlStreamReader xmlReader(&file);
        if ( ! xmlReader.hasError()) {
            paletteOut = readPaletteContent(xmlReader);
        }
        
        if (xmlReader.hasError()) {
            errorMessage = xmlReader.errorString();
        }
    }
    else {
        errorMessage = ("Unable to open file " + filename
                        + "\nError: " + file.errorString());
    }
    
    return FunctionResultValue<PaletteNew*>(paletteOut,
                                            errorMessage,
                                            (paletteOut != NULL));
}

/**
 * Read a palette from the given string
 * @param string
 *    String containing a palette in XML
 * @return
 *    Function result with palette or error
 */
FunctionResultValue<PaletteNew*>
PaletteNewXmlStreamReader::readFromString(const AString& string)
{
    if (string.isEmpty()) {
        return FunctionResultValue<PaletteNew*>(NULL,
                                                "String containing palette is empty.",
                                                false);
    }
    
    PaletteNew* paletteOut(NULL);
    AString errorMessage;
    QXmlStreamReader xmlReader(string);
    if ( ! xmlReader.hasError()) {
        paletteOut = readPaletteContent(xmlReader);
    }
    
    if (xmlReader.hasError()) {
        errorMessage = xmlReader.errorString();
    }
    
    return FunctionResultValue<PaletteNew*>(paletteOut,
                                            errorMessage,
                                            (paletteOut != NULL));
}


/**
 * Read the palette from the xml reader
 * @param xmlReader
 *   The XML reader
 * @return palette
 *   Palette if success
 */
PaletteNew*
PaletteNewXmlStreamReader::readPaletteContent(QXmlStreamReader& xmlReader)
{
    if (xmlReader.atEnd()) {
        xmlReader.raiseError("At end of file when starting to read.  Is file empty?");
        return NULL;
    }
    
    xmlReader.readNextStartElement();
    if (xmlReader.name() != ELEMENT_PALETTE) {
        xmlReader.raiseError("First element is \""
                             + xmlReader.name().toString()
                             + "\" but should be "
                             + ELEMENT_PALETTE);
        return NULL;
    }
    
    const QXmlStreamAttributes atts = xmlReader.attributes();
    const AString version           = atts.value(ATTRIBUTE_VERSION).toString();
    if (version.isEmpty()) {
        xmlReader.raiseError(ATTRIBUTE_VERSION + " attribute is missing from "
                             + ELEMENT_PALETTE);
        return NULL;
    }
    else {
        if (version != "2") {
            xmlReader.raiseError("Palette version must be 2 but is "
                                 + version);
            return NULL;
        }
    }
    
    const AString paletteName(atts.value(ATTRIBUTE_NAME).toString());
    if (paletteName.isEmpty()) {
        xmlReader.raiseError(ATTRIBUTE_NAME + " attribute is missing from "
                             + ELEMENT_PALETTE);
        return NULL;
    }
    
    std::vector<PaletteNew::ScalarColor> negativeRange;
    std::vector<PaletteNew::ScalarColor> positiveRange;
    float zeroRgb[3] { 0.0, 0.0, 0.0 };
    bool haveZeroRgbFlag(false);
    
    bool endElementFound(false);
    
    while ( ( ! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == ELEMENT_NEGATIVE_RANGE) {
                    negativeRange = readRange(xmlReader,
                                              ELEMENT_NEGATIVE_RANGE);
                }
                else if (xmlReader.name() == ELEMENT_POSITIVE_RANGE) {
                    positiveRange = readRange(xmlReader,
                                              ELEMENT_POSITIVE_RANGE);
                }
                else if (xmlReader.name() == ELEMENT_ZERO_COLOR) {
                    const QXmlStreamAttributes atts = xmlReader.attributes();
                    if (atts.hasAttribute(ATTRIBUTE_RED)
                        && atts.hasAttribute(ATTRIBUTE_GREEN)
                        && atts.hasAttribute(ATTRIBUTE_BLUE)) {
                        const float red    = atts.value(ATTRIBUTE_RED).toFloat();
                        const float green  = atts.value(ATTRIBUTE_GREEN).toFloat();
                        const float blue   = atts.value(ATTRIBUTE_BLUE).toFloat();
                        zeroRgb[0] = red;
                        zeroRgb[1] = green;
                        zeroRgb[2] = blue;
                        haveZeroRgbFlag = true;
                    }
                    else {
                        xmlReader.raiseError(ELEMENT_ZERO_COLOR
                                             + " is missing at least one of these attributes: "
                                             + ATTRIBUTE_RED
                                             + ", "
                                             + ATTRIBUTE_GREEN
                                             + ", "
                                             + ATTRIBUTE_BLUE);
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(xmlReader.name().toString());
                    xmlReader.skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_PALETTE) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
        
        if (xmlReader.hasError()) {
            return NULL;
        }
    }
    
    AString errorMessage;
    if (positiveRange.size() < 2) {
        errorMessage.appendWithNewLine("Positive range contains less than two ScalarColor elements.");
    }
    if (negativeRange.size() < 2) {
        errorMessage.appendWithNewLine("Negative range contains less than two ScalarColor elements.");
    }
    if ( ! haveZeroRgbFlag) {
        errorMessage.appendWithNewLine("Zero color was not found.");
    }
    if ( ! errorMessage.isEmpty()) {
        xmlReader.raiseError(errorMessage);
        return NULL;
    }
    
    CaretAssert( ! xmlReader.hasError());
    
    PaletteNew* palette(new PaletteNew(positiveRange,
                                       zeroRgb,
                                       negativeRange));
    palette->setName(paletteName);
    
    return palette;
}

/**
 * Read the negative or positive range from the XML
 * @param xmlReader
 *   The XML reader
 * @rangeXmlElement
 *   The XML element for either negative or positive range
 */
std::vector<PaletteNew::ScalarColor>
PaletteNewXmlStreamReader::readRange(QXmlStreamReader& xmlReader,
                                     const QString& rangeXmlElement)
{
    std::vector<PaletteNew::ScalarColor> scalarColors;
    
    bool endElementFound(false);
    
    while ( ( ! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == ELEMENT_SCALAR_COLOR) {
                    const QXmlStreamAttributes atts = xmlReader.attributes();
                    if (atts.hasAttribute(ATTRIBUTE_SCALAR)
                        && atts.hasAttribute(ATTRIBUTE_RED)
                        && atts.hasAttribute(ATTRIBUTE_GREEN)
                        && atts.hasAttribute(ATTRIBUTE_BLUE)) {
                        const float scalar = atts.value(ATTRIBUTE_SCALAR).toFloat();
                        const float red    = atts.value(ATTRIBUTE_RED).toFloat();
                        const float green  = atts.value(ATTRIBUTE_GREEN).toFloat();
                        const float blue   = atts.value(ATTRIBUTE_BLUE).toFloat();
                        float rgb[3] { red, green, blue };
                        scalarColors.push_back(PaletteNew::ScalarColor(scalar, rgb));
                    }
                    else {
                        xmlReader.raiseError("A "
                                             + rangeXmlElement
                                             + " "
                                             + ELEMENT_SCALAR_COLOR
                                             + " is missing at least one of these attributes: "
                                             + ATTRIBUTE_SCALAR
                                             + ", "
                                             + ATTRIBUTE_RED
                                             + ", "
                                             + ATTRIBUTE_GREEN
                                             + ", "
                                             + ATTRIBUTE_BLUE);
                        return scalarColors;
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(xmlReader.name().toString());
                    xmlReader.skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == rangeXmlElement) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }

    return scalarColors;
}
