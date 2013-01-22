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

#include "CaretObject.h"
#include "XmlException.h"
#include "XmlAttributes.h"

class QTextStream;

namespace caret {
    
    class AString;
    /**
     * Writes XML to a writer with indentation (pretty printing).
     * Similar to StAX.
     */
    class XmlWriter : public CaretObject {
        
    public:
        XmlWriter(std::ostream& writerIn);
        
        XmlWriter(QTextStream& writerIn);
        
        void writeStartDocument(const AString& xmlVersion) throw (XmlException);
        
        void writeStartDocument() throw (XmlException);
        
        void writeDTD(const AString& rootTag, const AString& dtdURL) throw(XmlException);
        
        void writeEndDocument() throw(XmlException);
        
        void writeElementCharacters(const AString& localName, const float f);
        
        void writeElementCharacters(const AString& localName, const float* values, const int32_t num);
        
        void writeElementCharacters(const AString& localName, const int32_t value);
        
        void writeElementCharacters(const AString& localName, const bool value);
        
        
        void writeElementCharacters(const AString& localName, const int* values, const int32_t num);
        
        
        void writeElementCharacters(const AString& localName, const AString& text) throw(XmlException);
        
        void writeElementCharacters(const AString& localName, 
                                    const XmlAttributes& attributes,
                                    const AString& text) throw(XmlException);
        
        void writeElementCData(const AString& localName, const AString& text) throw(XmlException);
        
        void writeElementCData(const AString& localName,
                               const XmlAttributes& attributes,
                               const AString& text) throw(XmlException);
        
        void writeElementNoSpace(const AString& localName, const AString& text) throw(XmlException);
        void writeStartElement(const AString& localName) throw(XmlException);
        
        void writeStartElement(const AString& localName,
                               const XmlAttributes& attributes) throw(XmlException);
        
        void writeEndElement() throw(XmlException);
        
        void writeCData(const AString& data) throw(XmlException);
        
        void writeCharacters(const AString& text) throw(XmlException);
        
        void writeCharactersWithIndent(const AString& text) throw(XmlException);
        
        void setNumberOfDecimalPlaces(const int32_t decimals);
        
    protected:
        void flushOutputStream();
        
        void writeTextToOutputStream(const AString& text);
        
    private:
        enum OutputStreamType {
            OUTPUT_STREAM_Q_TEXT_STREAM,
            OUTPUT_STREAM_STD_OUTPUT_STREAM
        };
        
        void writeIndentation() throw(XmlException);
        
        /** The std::ostream writer to which XML is written */
        std::ostream* stdOutputStreamWriter;
        
        /** The QTextStream writer to which XML is written */
        QTextStream* qTextStreamWriter;
        
        /** The indentation amount for new element tags. */
        int32_t indentationSpaces;
        
        /** The element stack used for closing elements. */
        std::stack<AString> elementStack;
        
        /** Number of decimal places for float data */
        int32_t numberOfDecimalPlaces;
        
        OutputStreamType outputStreamType; 
    };
    
}  // namespace

#endif	/* __XML_WRITER_H__ */

