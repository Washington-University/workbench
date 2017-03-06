#ifndef __XML_UNEXPECTED_ELEMENT_SAX_PARSER_H__
#define __XML_UNEXPECTED_ELEMENT_SAX_PARSER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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


#include <memory>
#include <stack>

#include "CaretObject.h"

class QTextStream;

namespace caret {
    class XmlAttributes;
    class XmlWriter;
    
    class XmlUnexpectedElementSaxParser : public CaretObject {
        
    public:
        /**
         * Return code from end element method.
         */
        enum class ReturnCodeEnum {
            /** Done processing unrecognized XML */
            DONE,
            /** Error encountered (unbalanced elements, unlikely since the SAX parser should catch this condition */
            ERROR,
            /** Still processing unrecognized XML */
            NOT_DONE
        };
        
        XmlUnexpectedElementSaxParser();
        
        virtual ~XmlUnexpectedElementSaxParser();
        
        void startElement(const AString& uri,
                          const AString& localName,
                          const AString& qName,
                          const XmlAttributes& atts);
        
        ReturnCodeEnum endElement(const AString& namespaceURI,
                                  const AString& localName,
                                  const AString& qName);
        
        void characters(const char* ch);

        AString getUnexpectedContentXML() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        XmlUnexpectedElementSaxParser(const XmlUnexpectedElementSaxParser&);

        XmlUnexpectedElementSaxParser& operator=(const XmlUnexpectedElementSaxParser&);
        
        std::stack<AString> m_elementNameStack;
        
        AString m_textStreamString;
        
        std::unique_ptr<QTextStream> m_textStream;
        
        std::unique_ptr<XmlWriter> m_xmlWriter;
        
        bool m_hadElementCharacters = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __XML_UNEXPECTED_ELEMENT_SAX_PARSER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __XML_UNEXPECTED_ELEMENT_SAX_PARSER_DECLARE__

} // namespace
#endif  //__XML_UNEXPECTED_ELEMENT_SAX_PARSER_H__
