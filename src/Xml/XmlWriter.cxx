/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <ostream>
#include <sstream>

#include <QTextStream>

#include "AString.h"
#include "CaretLogger.h"
#include "XmlWriter.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param writer - Writer to which XML is written.
 */
XmlWriter::XmlWriter(std::ostream& writerIn)
   : stdOutputStreamWriter(&writerIn), 
     qTextStreamWriter(NULL),
     outputStreamType(OUTPUT_STREAM_STD_OUTPUT_STREAM)
{
   this->indentationSpaces = 0;
   this->numberOfDecimalPlaces = 6;
}

XmlWriter::XmlWriter(QTextStream& writerIn)
   : stdOutputStreamWriter(NULL), 
     qTextStreamWriter(&writerIn),
     outputStreamType(OUTPUT_STREAM_Q_TEXT_STREAM)
{
    this->indentationSpaces = 0;
    this->numberOfDecimalPlaces = 6;
}


/**
 * Write the XML Start Document.
 *
 * @param xmlVersion - Version number, eg: "1.0".
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeStartDocument(const AString& xmlVersion) {
   this->writeTextToOutputStream("<?xml version=\"" 
                                 + xmlVersion
                                 + "\" encoding=\"UTF-8\"?>\n");
}

/**
 * Write the XML Start document.
 *
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeStartDocument() {
   this->writeStartDocument("1.0");
}

/**
 * Write a DTD section.
 *
 * @param rootTag - the root tag of the XML document.
 * @param dtdURL - URL of DTD.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeDTD(const AString& rootTag, const AString& dtdURL) {
   this->writeTextToOutputStream("<!DOCTYPE "
                                 + rootTag 
                                 + " SYSTEM \""
                                 + dtdURL + "\">\n");
}

/**
 * Closes any start tags and writes corresponding end tags.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeEndDocument() {
   while (this->elementStack.empty() == false) {
      writeEndElement();
   }
    
    this->flushOutputStream();
}

/**
 * Flush the output stream.
 */
void 
XmlWriter::flushOutputStream()
{
    switch (this->outputStreamType) {
        case OUTPUT_STREAM_Q_TEXT_STREAM:
            qTextStreamWriter->flush();
            break;
        case OUTPUT_STREAM_STD_OUTPUT_STREAM:
            stdOutputStreamWriter->flush();
            break;
    }
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, const float f) {
   AString text = AString::number(f); //AString::number(f, 'f', this->numberOfDecimalPlaces);
   this->writeElementCharacters(localName, text);
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, const float* values, const int32_t num) {
   AString str;
   for (int32_t i = 0; i < num; i++) {
       if (i > 0) {
           str.append(" ");
       }
       str.append(AString::number(values[i],'f',this->numberOfDecimalPlaces)); //AString::number(values[i], 'f', this->numberOfDecimalPlaces);
   }
   this->writeElementCharacters(localName,str);
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, const int32_t value) {
    const AString text = AString::number(value);
    this->writeElementCharacters(localName, text);
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, const int* values, const int32_t num) {
    AString str;
    for (int32_t i = 0; i < num; i++) {
        if (i > 0) {
            str.append(" ");
        }
        str.append(AString::number(values[i]));
    }
    this->writeElementCharacters(localName, str);
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param value - bool to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, const bool value) {
    const AString text = AString::fromBool(value);
    this->writeElementCharacters(localName, text);
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, const AString& text) {
   this->writeIndentation();
   this->writeTextToOutputStream("<" + localName + ">");
   this->writeCharacters(text);
   this->writeTextToOutputStream("</" + localName + ">\n");
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param attributes - attribute for element.
 * @param text - text to write.
 * @throws XmlException if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, 
                                  const XmlAttributes& attributes,
                                  const AString& text) {
    this->writeIndentation();
    this->writeTextToOutputStream("<" + localName);
    int32_t numAtts = attributes.getNumberOfAttributes();
    for (int32_t i = 0; i < numAtts; i++) {
        this->writeTextToOutputStream(" "
                                      + attributes.getName(i)
                                      + "=\""
                                      + attributes.getValue(i)
                                      + "\"");
    }
    this->writeTextToOutputStream(">");
    this->writeCharacters(text);
    this->writeTextToOutputStream("</" + localName + ">\n");
}

/**
 * Write a CData section on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCData(const AString& localName, const AString& text) {
   this->writeIndentation();
   this->writeTextToOutputStream("<" + localName + ">");
   this->writeCData(text);
   this->writeTextToOutputStream("</" + localName + ">\n");
}

/**
 * Write a CData section on one line with attributes.
 *
 * @param localName - local name of tag to write.
 * @param attributes - attribute for element.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCData(const AString& localName,
                              const XmlAttributes& attributes,
                              const AString& text) {
   this->writeIndentation();
   this->writeTextToOutputStream("<" + localName);
   int32_t numAtts = attributes.getNumberOfAttributes();
   for (int32_t i = 0; i < numAtts; i++) {
      this->writeTextToOutputStream(" "
                                    + attributes.getName(i)
                                    + "=\""
                                    + attributes.getValue(i)
                                    + "\"");
   }
   this->writeTextToOutputStream(">");
   this->writeCData(text);
   this->writeTextToOutputStream("</" + localName + ">\n");
}

/**
 * Write an element with no spacing between start and end tags.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementNoSpace(const AString& localName, const AString& text) {
   this->writeIndentation();
   this->writeTextToOutputStream("<" + localName + ">");
   this->writeTextToOutputStream(text);
   this->writeTextToOutputStream("</" + localName + ">\n");
}

/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeStartElement(const AString& localName) {
   this->writeIndentation();
   this->writeTextToOutputStream("<" + localName + ">\n");
   this->indentationSpaces++;
   this->elementStack.push(localName);
}

/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @param attributes - attributes for start tag
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeStartElement(const AString& localName,
                  const XmlAttributes& attributes) {
    int32_t  numAtts = attributes.getNumberOfAttributes();
    if (numAtts <= 0) {
        writeStartElement(localName);
        return;
    }
    
   this->writeIndentation();
   this->writeTextToOutputStream("<" + localName + " ");

   int32_t attIndentSpaces = localName.length() + 2;
   AString attIndentString(attIndentSpaces, ' ');

   for (int32_t i = 0; i < numAtts; i++) {
      if (i > 0) {
         this->writeIndentation();
         this->writeTextToOutputStream(attIndentString);
      }
      this->writeTextToOutputStream(attributes.getName(i)
                                    + "=\""
                                    + attributes.getValue(i)
                                    + "\"");
      if (i < (numAtts - 1)) {
         this->writeTextToOutputStream("\n");
      }
   }

   this->writeTextToOutputStream(">\n");
   this->indentationSpaces++;
   this->elementStack.push(localName);
}

/**
 * Writes an end tag to the output.
 *
 * @throws XmlAttributes if an I/O error occurs
 */
void
XmlWriter::writeEndElement() {
   if (this->elementStack.empty()) {
      throw XmlException("Unbalanced start/end element calls.");
   }

    AString localName = this->elementStack.top();
    this->elementStack.pop();
    this->indentationSpaces--;
    this->writeIndentation();
    this->writeTextToOutputStream("</" + localName + ">\n");
}

/**
 * Writes a CData section.
 *
 * @param data - data to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeCData(const AString& data) {
   this->writeTextToOutputStream("<![CDATA[");
   this->writeCharacters(data);
   this->writeTextToOutputStream("]]>");
}

/**
 * Write text to the output stream.  Expects all characters
 * to be valid.
 */
void 
XmlWriter::writeTextToOutputStream(const AString& text)
{
    switch (this->outputStreamType) {
        case OUTPUT_STREAM_Q_TEXT_STREAM:
            *qTextStreamWriter << text;
            break;
        case OUTPUT_STREAM_STD_OUTPUT_STREAM:
            *stdOutputStreamWriter << (text.toStdString());
            break;
    }
}

/**
 * Writes text to the output.  Avoids unprintable characters which cause
 * problems with some XML parsers.
 *
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeCharacters(const AString& text)  {
    switch (this->outputStreamType) {
        case OUTPUT_STREAM_Q_TEXT_STREAM:
        {
            const ushort CARRIAGE_RETURN = 13;
            const ushort LINE_FEED = 10;
            const ushort TAB = 9;
            
            int32_t num = text.length();
            for (int32_t i = 0; i < num; i++) {
                QChar c = text[i];
                ushort u = c.unicode();
                
                bool printIt = true; //c.isPrint();
                
                if (u < 32) {
                    printIt = false;
                    
                    if ((u == CARRIAGE_RETURN) ||
                        (u == LINE_FEED) ||
                        (u == TAB)) {
                        printIt = true;
                    }
                }
                if (printIt) {
                    *qTextStreamWriter << c;
                }
                else {
                    CaretLogWarning("Unicode value of character not written: " 
                                    + AString::number((int)u));
                }
            }
            
        }
            break;
        case OUTPUT_STREAM_STD_OUTPUT_STREAM:
        {
            const wchar_t CARRIAGE_RETURN = 13;
            const wchar_t LINE_FEED = 10;
            const wchar_t TAB = 9;
            
            std::string tempstring = text.toStdString();
            int32_t num = tempstring.length();
            for (int32_t i = 0; i < num; i++) {
                char c = tempstring[i];
                
                bool printIt = true; //c.isPrint();
                
                if (c < 32) {
                    printIt = false;
                    
                    if ((c == CARRIAGE_RETURN) ||
                        (c == LINE_FEED) ||
                        (c == TAB)) {
                        printIt = true;
                    }
                }
                if (printIt) {
                    *stdOutputStreamWriter << c;
                }
                else {
                    CaretLogWarning("Unicode value of character not written: " + AString::number((int)c));
                }
            }
            
        }
            break;
    }
}

/**
 * Writes text with indentation to the output.
 *
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeCharactersWithIndent(const AString& text) {
   this->writeIndentation();
   this->writeTextToOutputStream(text);
}

void
XmlWriter::setNumberOfDecimalPlaces(int32_t decimals) {
   this->numberOfDecimalPlaces = decimals;
}

/**
 * Write indentation spaces.
 *
 * @throws XmlAttributes if an I/O error occurs.
 */
void XmlWriter::writeIndentation() {
   if (this->indentationSpaces > 0) {
      AString sb(indentationSpaces * 3, ' ');
      this->writeTextToOutputStream(sb);
   }
}


