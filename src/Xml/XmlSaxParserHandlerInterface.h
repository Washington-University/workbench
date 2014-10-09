#ifndef __XMLSAXPARSERHANDLERINTERFACE_H__
#define __XMLSAXPARSERHANDLERINTERFACE_H__

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


#include <exception>
#include <AString.h>

#include "CaretObject.h"
#include "XmlAttributes.h"
#include "XmlSaxParserException.h"

namespace caret {
    
    //lass XmlSaxParserException;
    
    /**
     * Abstract class for XML SAX Parsing.  The design of this
     * class is based of the handler classes in Java.
     */
    class XmlSaxParserHandlerInterface {
        
    protected:
        XmlSaxParserHandlerInterface() { }
        
        virtual ~XmlSaxParserHandlerInterface() { }

    private:
        XmlSaxParserHandlerInterface(const XmlSaxParserHandlerInterface& s);
        XmlSaxParserHandlerInterface& operator=(const XmlSaxParserHandlerInterface&);
        
    public:
        /**
         * The Parser will invoke this method at the beginning of 
         * every element in the XML document; there will be a corresponding
         * endElement event for every startElement event (even when the 
         * element is empty). All of the element's content will be 
         * reported, in order, before the corresponding endElement event.
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
         * @throws
         *    XmlParsingException
         *        If an error is encountered and parsing should cease.
         */
        virtual void startElement(const AString& uri,
                                  const AString& localName,
                                  const AString& qName,
                                  const XmlAttributes& atts) 
                      = 0;
        
        /**
         * Receive notification of an end element.
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
         * @param attributes
         *    the attributes attached to the element. If there are 
         *    no attributes, it shall be an empty Attributes object.
         * @throws
         *    XmlParsingException
         *        If an error is encountered and parsing should cease.
         */
        virtual void endElement(const AString& namespaceURI,
                                       const AString& localName,
                                       const AString& qualifiedName) 
                      = 0;
        
        /**
         * Receive notification of characters.  
         * The Parser will call this method to report each chunk of 
         * character data. SAX parsers may return all contiguous character 
         * data in a single chunk, or they may split it into several 
         * chunks; however, all of the characters in any single event 
         * must come from the same external entity so that the Locator
         * provides useful information.
         *
         * @param ch The characters from the XML document.
         */
        virtual void characters(const char* ch) = 0;

        /**
         * Receive notification of a warning.
         *
         * SAX parsers will use this method to report conditions that are not 
         * errors or fatal errors as defined by the XML recommendation. The 
         * default behaviour is to take no action.
         *
         * The SAX parser must continue to provide normal parsing events 
         * after invoking this method: it should still be possible for 
         * the application to process the document through to the end.
         * 
         * Filters may use this method to report other, non-XML warnings as well.   
         *
         * @param exception 
         *     Warning information in an exception.
         * @param XmlSaxParserExcetion
         *     Thrown to stop parsing.
         */
        virtual void warning(const XmlSaxParserException& exception) = 0;
        
        /**
         * Receive notification of a recoverable error.
         *
         * This corresponds to the definition of "error" in section 
         * 1.2 of the W3C XML 1.0 Recommendation. For example, a validating 
         * parser would use this callback to report the violation of a 
         * validity constraint. The default behaviour is to take no action.
         *
         * The SAX parser must continue to provide normal parsing events 
         * after invoking this method: it should still be possible for the 
         * application to process the document through to the end. If the 
         * application cannot do so, then the parser should report a fatal 
         * error even if the XML recommendation does not require it to do so.
         *
         * 
         * Filters may use this method to report other, non-XML warnings as well.   
         *
         * @param exception 
         *     Warning information in an exception.
         * @param XmlSaxParserExcetion
         *     Thrown to stop parsing.
         */
        virtual void error(const XmlSaxParserException& exception) = 0;
        
        /**
         * Receive notification of a non-recoverable error.
         *
         * There is an apparent contradiction between the documentation for 
         * this method and the documentation for ContentHandler.endDocument(). 
         * Until this ambiguity is resolved in a future major release, clients 
         * should make no assumptions about whether endDocument() will or will 
         * not be invoked when the parser has reported a fatalError() or 
         * thrown an exception.
         * 
         * This corresponds to the definition of "fatal error" in section 1.2 of 
         * the W3C XML 1.0 Recommendation. For example, a parser would use this 
         * callback to report the violation of a well-formedness constraint.
         * 
         * The application must assume that the document is unusable after the 
         * parser has invoked this method, and should continue (if at all) only 
         * for the sake of collecting additional error messages: in fact, SAX 
         * parsers are free to stop reporting any other events once this method 
         * has been invoked.
         *
         * @param exception 
         *     Warning information in an exception.
         * @param XmlSaxParserExcetion
         *     Thrown to stop parsing.
         */
        virtual void fatalError(const XmlSaxParserException& exception) = 0;
        
        /**
         * Receive notification of the beginning of a document.
         *
         * @throws XmlParsingException  
         *        If an error is encountered and parsing should cease.
         */
        virtual void startDocument() = 0;

        /** 
         * Receive notification of the end of a document.
         *
         * @throws XmlParsingException  
         *        If an error is encountered and parsing should cease.
         */
        virtual void endDocument() = 0;
    };
    
} // namespace

#endif // __XMLSAXPARSERHANDLERINTERFACE_H__
