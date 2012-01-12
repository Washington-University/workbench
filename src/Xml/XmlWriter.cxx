/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <ostream>
#include <sstream>

#include "CaretLogger.h"
#include "XmlWriter.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param writer - Writer to which XML is written.
 */
XmlWriter::XmlWriter(std::ostream& writerIn)
   : writer(writerIn) {
   this->indentationSpaces = 0;
   this->numberOfDecimalPlaces = 6;
   //AString::setFloatDigitsRightOfDecimal(this->numberOfDecimalPlaces);
   
}

/**
 * Write the XML Start Document.
 *
 * @param xmlVersion - Version number, eg: "1.0".
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeStartDocument(const AString& xmlVersion) throw (XmlException) {
   writer << ("<?xml version=\"" + xmlVersion.toStdString() + "\" encoding=\"UTF-8\"?>\n");
}

/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeStartDocument(const char* localName) throw(XmlException) {
//   AString ln = localName;
//   this->writeStartDocument(ln);
//}

/**
 * Write the XML Start document.
 *
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeStartDocument() throw (XmlException) {
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
XmlWriter::writeDTD(const AString& rootTag, const AString& dtdURL) throw(XmlException) {
   writer << ("<!DOCTYPE "
                + rootTag.toStdString() + " SYSTEM \""
                + dtdURL.toStdString() + "\">\n");
}

/**
 * Closes any start tags and writes corresponding end tags.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeEndDocument() throw(XmlException) {
   while (this->elementStack.empty() == false) {
      writeEndElement();
   }

   writer.flush();
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
        str.append(AString::number(values[i],'f',this->numberOfDecimalPlaces));
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
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCharacters(const char* localName, const char* text) {
//   this->writeElementCharacters(localName, text);
//}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCharacters(const AString& localName, const AString& text)
                                                 throw(XmlException) {
   this->writeIndentation();
   writer << ("<" + localName.toStdString() + ">");
   this->writeCharacters(text);
   writer << ("</" + localName.toStdString() + ">\n");
}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCharacters(const char* localName, AString text)
//                                                 throw(XmlException) {
//   AString ln = localName;
//   this->writeElementCharacters(ln, text);
//}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCData(const char* localName, const char* text) {
//   this->writeElementCData(localName, text);
//}

/**
 * Write an element on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeElementCData(const char* localName, AString text) {
//   this->writeElementCData(localName, text);
//}

/**
 * Write a CData section on one line.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementCData(const AString& localName, const AString& text)
                                                 throw(XmlException) {
   this->writeIndentation();
   writer << ("<" + localName.toStdString() + ">");
   this->writeCData(text);
   writer << ("</" + localName.toStdString() + ">\n");
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
                              const AString& text)
                                                 throw(XmlException) {
   this->writeIndentation();
   writer << ("<" + localName.toStdString());
   int32_t numAtts = attributes.getNumberOfAttributes();
   for (int32_t i = 0; i < numAtts; i++) {
      writer << (" "
                   + attributes.getName(i).toStdString()
                   + "=\""
                   + attributes.getValue(i).toStdString()
                   + "\"");
   }
   writer << (">");
   this->writeCData(text);
   writer << ("</" + localName.toStdString() + ">\n");
}

/**
 * Write an element with no spacing between start and end tags.
 *
 * @param localName - local name of tag to write.
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeElementNoSpace(const AString& localName, const AString& text)
                                                 throw(XmlException) {
   this->writeIndentation();
   writer << ("<" + localName.toStdString() + ">");
   writer << (text.toStdString());
   writer << ("</" + localName.toStdString() + ">\n");
}

/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @throws IOException if an I/O error occurs.
 */
//void writeStartElement(const char* localName) throw(XmlException) {
//   AString ln = localName;
//   this->writeStartElement(ln);
//}
/**
 * Writes a start tag to the output.
 *
 * @param localName - local name of tag to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeStartElement(const AString& localName) throw(XmlException) {
   this->writeIndentation();
   writer << ("<" + localName.toStdString() + ">\n");
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
                  const XmlAttributes& attributes) throw(XmlException) {
   this->writeIndentation();
   writer << ("<" + localName.toStdString() + " ");

   int32_t attIndentSpaces = localName.length() + 2;
   AString attIndentString(attIndentSpaces, ' ');

   int32_t  numAtts = attributes.getNumberOfAttributes();
   for (int32_t i = 0; i < numAtts; i++) {
      if (i > 0) {
         this->writeIndentation();
         writer << (attIndentString.toStdString());
      }
      writer << (attributes.getName(i).toStdString()
                   + "=\""
                   + attributes.getValue(i).toStdString()
                   + "\"");
      if (i < (numAtts - 1)) {
         writer << ("\n");
      }
   }

   writer << (">\n");
   this->indentationSpaces++;
   this->elementStack.push(localName);
}

/**
 * Writes an end tag to the output.
 *
 * @throws XmlAttributes if an I/O error occurs
 */
void
XmlWriter::writeEndElement() throw(XmlException) {
   if (this->elementStack.empty()) {
      throw XmlException("Unbalanced start/end element calls.");
   }

    AString localName = this->elementStack.top();
    this->elementStack.pop();
    this->indentationSpaces--;
    this->writeIndentation();
    writer << ("</" + localName.toStdString() + ">\n");
}

/**
 * Writes a CData section.
 *
 * @param data - data to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeCData(const AString& data) throw(XmlException) {
   writer << "<![CDATA[";
   this->writeCharacters(data);
   writer << "]]>";
}

/**
 * Writes text to the output.  Avoids unprintable characters which cause
 * problems with some XML parsers.
 *
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeCharacters(const AString& text) throw(XmlException) {
   const wchar_t CARRIAGE_RETURN = 13;
   const wchar_t LINE_FEED = 10;
   const wchar_t TAB = 9;

   std::string tempstring = text.toStdString();
   int32_t num = tempstring.length();
   for (int32_t i = 0; i < num; i++) {
      char c = tempstring.at(i);

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
         writer << c;
      }
      else {
          CaretLogWarning("Unicode value of character not written: " + (int)c);
      }
   }
   //writer << (text);
}

/**
 * Writes text with indentation to the output.
 *
 * @param text - text to write.
 * @throws XmlAttributes if an I/O error occurs.
 */
void
XmlWriter::writeCharactersWithIndent(const AString& text) throw(XmlException) {
   this->writeIndentation();
   writer << (text.toStdString());
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
void XmlWriter::writeIndentation() throw(XmlException) {
   if (this->indentationSpaces > 0) {
      AString sb(indentationSpaces * 3, ' ');
      writer << (sb.toStdString());
   }
}


