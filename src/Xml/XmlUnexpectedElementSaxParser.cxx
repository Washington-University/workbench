
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

#define __XML_UNEXPECTED_ELEMENT_SAX_PARSER_DECLARE__
#include "XmlUnexpectedElementSaxParser.h"
#undef __XML_UNEXPECTED_ELEMENT_SAX_PARSER_DECLARE__

#include <QTextStream>
#include <XmlWriter.h>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::XmlUnexpectedElementSaxParser 
 * \brief Assists with parsing unexpected elements in XML.
 * \ingroup Xml
 *
 * Assists with processing unexpected XML elements that may actually be new elements
 * or elements that may have been removed.  It allows the XML parser to skip these 
 * new elements so that reading of XML does not prematurely terminate.  It is able
 * to handle nested child elements with the same name.  It concludes processing when
 * an element name that matches that passed to the first startElement() method and
 * all children have been processed.
 *
 * When an unexpected element is encountered:
 * (1) Create an instance of this class and call the startElement() method;
 * (2) Call startElement(), characters() and endElement() methods of this class
 * until endElement() returns a 'Done' status.
 * (3) Call getUnexpectedContentXML() method to get/print the unexpected XML.
 * (4) Destroy the instance of this class.
 */

/**
 * Constructor.
 */
XmlUnexpectedElementSaxParser::XmlUnexpectedElementSaxParser()
: CaretObject()
{
    m_textStreamString.reserve(1000);
    m_textStream = std::unique_ptr<QTextStream>(new QTextStream(&m_textStreamString,
                                                                QIODevice::WriteOnly));
    m_xmlWriter = std::unique_ptr<XmlWriter>(new XmlWriter(*m_textStream));
}

/**
 * Destructor.
 */
XmlUnexpectedElementSaxParser::~XmlUnexpectedElementSaxParser()
{
}

/*
 * Process characters.
 *
 * @param ch The characters from the XML document.
 */
void
XmlUnexpectedElementSaxParser::characters(const char* ch)
{
    if (AString(ch).trimmed().isEmpty()) {
        /* all white space */
        return;
    }
    
    if (m_hadElementCharacters) {
        m_xmlWriter->writeCharacters(AString(ch));
    }
    else {
        m_xmlWriter->writeCharactersWithIndent(AString(ch));
    }
    
    m_hadElementCharacters = true;
}

/**
 * Process an end element.
 *
 * @param uri
 *    the Namespace URI, or the empty string if the element
 *    has no Namespace URI or if Namespace processing is not
 *    being performed
 * @param localName
 *    the local name (without prefix), or the empty string if
 *    Namespace processing is not being performed
 * @param qName
 *    the qualified name (with prefix), or the empty string
 *    if qualified names are not available
 * @return
 *    The return code indicating status of element processing.
 */
XmlUnexpectedElementSaxParser::ReturnCodeEnum
XmlUnexpectedElementSaxParser::endElement(const AString& /*uri*/, const AString& localName, const AString& /*qName*/)
{
    if (m_elementNameStack.empty()) {
        return ReturnCodeEnum::ERROR;
    }
    
    if (localName == m_elementNameStack.top()) {
        m_elementNameStack.pop();
        
        if (m_hadElementCharacters) {
            m_xmlWriter->writeCharacters("\n");
            m_hadElementCharacters = false;
        }
        
        m_xmlWriter->writeEndElement();

        if (m_elementNameStack.empty()) {
            return ReturnCodeEnum::DONE;
        }
        
        return ReturnCodeEnum::NOT_DONE;
    }
    
    return ReturnCodeEnum::ERROR;
}

/**
 * Process a start element.
 *
 * @param uri
 *    the Namespace URI, or the empty string if the element
 *    has no Namespace URI or if Namespace processing is not
 *    being performed
 * @param localName
 *    the local name (without prefix), or the empty string if
 *    Namespace processing is not being performed
 * @param qName
 *    the qualified name (with prefix), or the empty string
 *    if qualified names are not available
 * @param atts
 *    the attributes attached to the element. If there are
 *    no attributes, it shall be an empty Attributes object.
 */
void
XmlUnexpectedElementSaxParser::startElement(const AString& /*uri*/, const AString& localName, const AString& /*qName*/, const XmlAttributes& atts)
{
    m_elementNameStack.push(localName);
    m_xmlWriter->writeStartElement(localName, atts);
}

/**
 * @return The unexpected content XML.
 */
AString
XmlUnexpectedElementSaxParser::getUnexpectedContentXML() const
{
    return m_textStreamString;
}


