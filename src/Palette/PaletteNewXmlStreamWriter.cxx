
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

#define __PALETTE_NEW_XML_STREAM_WRITER_DECLARE__
#include "PaletteNewXmlStreamWriter.h"
#undef __PALETTE_NEW_XML_STREAM_WRITER_DECLARE__

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "PaletteNew.h"

using namespace caret;

/**
 * \class caret::PaletteNewXmlStreamWriter 
 * \brief XML writer for PaletteNew instances
 * \ingroup Palette
 */

/**
 * Constructor.
 */
PaletteNewXmlStreamWriter::PaletteNewXmlStreamWriter()
: PaletteNewXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
PaletteNewXmlStreamWriter::~PaletteNewXmlStreamWriter()
{
}

/**
 * Write the palette to the file with the given name
 * @param palette
 *    The palette
 * @param filename
 *    Name of output file
 * @return
 *    Function result with success/failure
 */
FunctionResult
PaletteNewXmlStreamWriter::writeToFile(const PaletteNew& palette,
                                       const AString& filename)
{
    if (filename.isEmpty()) {
        return FunctionResult::error("Filename is empty for writing palette.");
    }
    
    QFile file(filename);
    if ( ! file.open(QFile::WriteOnly)) {
        return FunctionResult::error("Unable to open for writing\n"
                                     "Filename=" + filename + "\n"
                                     "Error=" + file.errorString());
    }
    
    QXmlStreamWriter xmlWriter(&file);
    if (xmlWriter.hasError()) {
        return FunctionResult::error("Unable to create XML stream writer\n"
                                     "Filename=" + filename + "\n"
                                     "Error=" + getXmlWriterErrorString(xmlWriter));
    }
    
    const bool errorFlag( ! writePaletteContent(xmlWriter,
                                                palette));
    
    if (xmlWriter.hasError()
        || errorFlag) {
        return FunctionResult::error("Error writing XML\n"
                                     "Filename=" + filename + "\n"
                                     "Error=" + getXmlWriterErrorString(xmlWriter));
    }

    return FunctionResult::ok();
}

/**
 * Write the given palette to a string
 * @param palette
 *    Palette that will be written
 * @return
 *    Function result with string or error
 */
FunctionResultValue<AString>
PaletteNewXmlStreamWriter::writeToString(const PaletteNew& palette)
{
    AString string;
    AString errorMessage;
    QXmlStreamWriter xmlWriter(&string);
    if (xmlWriter.hasError()) {
        errorMessage = ("Unable to create XML stream writer for palette="
                        + palette.getName() + "\n"
                        "Error=" + getXmlWriterErrorString(xmlWriter));
    }
    
    if (errorMessage.isEmpty()) {
        const bool errorFlag( ! writePaletteContent(xmlWriter,
                                                    palette));
        

        if (xmlWriter.hasError()
            || errorFlag) {
            errorMessage = ("Error writing XML for palette="
                            + palette.getName() + "\n"
                            "Error=" + getXmlWriterErrorString(xmlWriter));
        }
    }
    
    return FunctionResultValue<AString>(string,
                                        errorMessage,
                                        errorMessage.isEmpty());
}

/**
 * Write the palette to the XML stream writer
 * @param xmlWriter
 *    The XML writer
 * @param palette
 *    The palette
 * @return
 *    True if successful, else false (returning result is needed since QXmlStreamWriter::raiseError() not supported before Qt 6.10).
 */
bool
PaletteNewXmlStreamWriter::writePaletteContent(QXmlStreamWriter& xmlWriter,
                                               const PaletteNew& palette)
{
    AString errorMessage;
    const std::vector<PaletteNew::ScalarColor> positiveRange(palette.getPosRange());
    const std::vector<PaletteNew::ScalarColor> negativeRange(palette.getNegRange());
    if (positiveRange.size() < 2) {
        errorMessage.appendWithNewLine("Positive range must contain at least two elements");
    }
    if (negativeRange.size() < 2) {
        errorMessage.appendWithNewLine("Negative range must contain at least two elements");
    }
    if ( ! errorMessage.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        xmlWriter.raiseError(errorMessage);
#endif
        return false;
    }

    const AString versionString("2");
    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartDocument();
    
    xmlWriter.writeStartElement(ELEMENT_PALETTE);
    xmlWriter.writeAttribute(ATTRIBUTE_VERSION,
                             versionString);
    xmlWriter.writeAttribute(ATTRIBUTE_NAME,
                             palette.getName());

    writeRange(xmlWriter,
               ELEMENT_POSITIVE_RANGE,
               positiveRange);

    writeRange(xmlWriter,
               ELEMENT_NEGATIVE_RANGE,
               negativeRange);

    float zeroRGB[3];
    palette.getZeroColor(zeroRGB);
    xmlWriter.writeStartElement(ELEMENT_ZERO_COLOR);
    xmlWriter.writeAttribute(ATTRIBUTE_RED,
                             AString::number(zeroRGB[0], 'f', RGB_PRECISION));
    xmlWriter.writeAttribute(ATTRIBUTE_GREEN,
                             AString::number(zeroRGB[1], 'f', RGB_PRECISION));
    xmlWriter.writeAttribute(ATTRIBUTE_BLUE,
                             AString::number(zeroRGB[2], 'f', RGB_PRECISION));
    xmlWriter.writeEndElement();
    
    xmlWriter.writeEndElement();
    
    xmlWriter.writeEndDocument();
    
    return true;
}

/**
 * Write the given range to the stream writer
 * @param xmlWriter
 *    The XML writer
 * @param rangeXmlElement
 *    The range element (positive or negative)
 * @param rangeScalarColor
 *   ScalarColor's for the range
 */
void
PaletteNewXmlStreamWriter::writeRange(QXmlStreamWriter& xmlWriter,
                                      const AString& rangeXmlElement,
                                      const std::vector<PaletteNew::ScalarColor>& rangeScalarColors)
{
    xmlWriter.writeStartElement(rangeXmlElement);
    for (const PaletteNew::ScalarColor& sc : rangeScalarColors) {
        xmlWriter.writeStartElement(ELEMENT_SCALAR_COLOR);
        xmlWriter.writeAttribute(ATTRIBUTE_SCALAR,
                                 AString::number(sc.scalar, 'f', SCALAR_PRECISION));
        xmlWriter.writeAttribute(ATTRIBUTE_RED,
                                 AString::number(sc.color[0], 'f', RGB_PRECISION));
        xmlWriter.writeAttribute(ATTRIBUTE_GREEN,
                                 AString::number(sc.color[1], 'f', RGB_PRECISION));
        xmlWriter.writeAttribute(ATTRIBUTE_BLUE,
                                 AString::number(sc.color[2], 'f', RGB_PRECISION));
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
}

/**
 * QXmlWriter::errorString() was added in Qt 6.10.
 * @return The XML error string, if supported.
 * @param xmlWriter
 *    The XML writer with error.
 */
AString
PaletteNewXmlStreamWriter::getXmlWriterErrorString(QXmlStreamWriter& xmlWriter)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    return xmlWriter.errorString();
#else
    return AString("Unknown error.  Qt is too old to provide error message.");
#endif
}


