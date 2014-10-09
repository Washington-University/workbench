#ifndef __XML_WRITER_H__
#define	__XML_WRITER_H__

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
        
        void writeStartDocument(const AString& xmlVersion);
        
        void writeStartDocument();
        
        void writeDTD(const AString& rootTag, const AString& dtdURL);
        
        void writeEndDocument();
        
        void writeElementCharacters(const AString& localName, const float f);
        
        void writeElementCharacters(const AString& localName, const float* values, const int32_t num);
        
        void writeElementCharacters(const AString& localName, const int32_t value);
        
        void writeElementCharacters(const AString& localName, const bool value);
        
        
        void writeElementCharacters(const AString& localName, const int* values, const int32_t num);
        
        
        void writeElementCharacters(const AString& localName, const AString& text);
        
        void writeElementCharacters(const AString& localName, 
                                    const XmlAttributes& attributes,
                                    const AString& text);
        
        void writeElementCData(const AString& localName, const AString& text);
        
        void writeElementCData(const AString& localName,
                               const XmlAttributes& attributes,
                               const AString& text);
        
        void writeElementNoSpace(const AString& localName, const AString& text);
        void writeStartElement(const AString& localName);
        
        void writeStartElement(const AString& localName,
                               const XmlAttributes& attributes);
        
        void writeEndElement();
        
        void writeCData(const AString& data);
        
        void writeCharacters(const AString& text);
        
        void writeCharactersWithIndent(const AString& text);
        
        void setNumberOfDecimalPlaces(const int32_t decimals);
        
    protected:
        void flushOutputStream();
        
        void writeTextToOutputStream(const AString& text);
        
    private:
        enum OutputStreamType {
            OUTPUT_STREAM_Q_TEXT_STREAM,
            OUTPUT_STREAM_STD_OUTPUT_STREAM
        };
        
        void writeIndentation();
        
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

