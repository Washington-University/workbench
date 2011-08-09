/* 
 * File:   XmlWriter.h
 * Author: john
 *
 * Created on November 6, 2009, 10:22 AM
 */

#ifndef __XML_WRITER_H__
#define	__XML_WRITER_H__

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


#include <stdint.h>
#include <ostream>
#include <stack>
#include <QString>

#include "CaretObject.h"
#include "XmlException.h"
#include "XmlAttributes.h"

namespace caret {

/**
 * Writes XML to a writer with indentation (pretty printing).
 * Similar to StAX.
 */
class XmlWriter : public CaretObject {

public:
    XmlWriter(std::ostream& writerIn);

   void writeStartDocument(const QString& xmlVersion) throw (XmlException);

   //void writeStartDocument(const char* localName) throw(XmlException) {
   //   QString ln = localName;
   //   this->writeStartDocument(ln);
   //}

   void writeStartDocument() throw (XmlException);

   void writeDTD(const QString& rootTag, const QString& dtdURL) throw(XmlException);

   void writeEndDocument() throw(XmlException);

   void writeElementCharacters(const QString& localName, const float f);

   void writeElementCharacters(const QString& localName, const float* values, const int32_t num);

    void writeElementCharacters(const QString& localName, const int32_t value);


    void writeElementCharacters(const QString& localName, const int* values, const int32_t num);


    //void writeElementCharacters(const char* localName, const char* text) {
   //   this->writeElementCharacters(localName, text);
   //}

   void writeElementCharacters(const QString& localName, const QString& text)
                                                    throw(XmlException);

   //void writeElementCharacters(const char* localName, QString text)
   //                                                 throw(XmlException) {
   //   QString ln = localName;
   //   this->writeElementCharacters(ln, text);
   //}
   
   //void writeElementCData(const char* localName, const char* text) {
   //   this->writeElementCData(localName, text);
   //}

   //void writeElementCData(const char* localName, QString text) {
   //   this->writeElementCData(localName, text);
   //}

   void writeElementCData(const QString& localName, const QString& text)
                                                    throw(XmlException);

   void writeElementCData(const QString& localName,
                                 const XmlAttributes& attributes,
                                 const QString& text)
                                                    throw(XmlException);

   void writeElementNoSpace(const QString& localName, const QString& text)
                                                    throw(XmlException);
   //void writeStartElement(const char* localName) throw(XmlException) {
   //   QString ln = localName;
   //   this->writeStartElement(ln);
   //}
   void writeStartElement(const QString& localName) throw(XmlException);

   void writeStartElement(const QString& localName,
                     const XmlAttributes& attributes) throw(XmlException);

   void writeEndElement() throw(XmlException);

   void writeCData(const QString& data) throw(XmlException);

   void writeCharacters(const QString& text) throw(XmlException);

   void writeCharactersWithIndent(const QString& text) throw(XmlException);

   void setNumberOfDecimalPlaces(const int32_t decimals);

private:
   void writeIndentation() throw(XmlException);

   /** The writer to which XML is written */
   std::ostream& writer;

   /** The indentation amount for new element tags. */
   int32_t indentationSpaces;

   /** The element stack used for closing elements. */
    std::stack<QString> elementStack;

    /** Number of decimal places for float data */
    int32_t numberOfDecimalPlaces;
};

}  // namespace

#endif	/* __XML_WRITER_H__ */

