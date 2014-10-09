#ifndef __XMLSAXPARSERWITHQT_H__
#define __XMLSAXPARSERWITHQT_H__

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


#include <AString.h>
#include <QXmlDefaultHandler>
#include "XmlAttributes.h"
#include "XmlSaxParser.h"

class QXmlSimpleReader;
class QXmlParseException;

namespace caret {
    
    //lass XmlSaxParserException;
    
    /**
     * Abstract class for XML SAX Parsing.  The design of this
     * class is based of the handler classes in Java.
     */
    class XmlSaxParserWithQt : public XmlSaxParser {
        
        
    public:
        XmlSaxParserWithQt();
        
        virtual ~XmlSaxParserWithQt();

        virtual void parseFile(const QString& filename,
                   XmlSaxParserHandlerInterface* handler);
        
        virtual void parseString(const QString& xmlString,
                           XmlSaxParserHandlerInterface* handler);
        
    private:
        XmlSaxParserWithQt(const XmlSaxParserWithQt& sp);
        XmlSaxParserWithQt& operator=(const XmlSaxParserWithQt&);
        
    protected:
    private:
        
        void initializeMembersXmlSaxParserWithQt();
        
        QXmlSimpleReader* xmlReader;
        
        class PrivateHandler : public QXmlDefaultHandler {
          
        public:
            PrivateHandler(XmlSaxParserHandlerInterface* handler);
            ~PrivateHandler();
            bool error(const QXmlParseException& exception);
            bool fatalError(const QXmlParseException& exception);
            bool warning(const QXmlParseException& exception);
            
            bool startDocument();
            bool startElement(const QString& namespaceURI,
                              const QString& localName,
                              const QString& qName,
                              const QXmlAttributes& atts);
            
            bool endDocument();
            bool endElement(const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName);
            
            bool characters(const QString& ch);
            
            virtual QString errorString() const;
            
        private:
            static XmlSaxParserException fromQXmlParseException(const QXmlParseException& e);
            static XmlAttributes fromQXmlAttributes(const QXmlAttributes& atts);
            XmlSaxParserHandlerInterface* handler;
            QString errorMessage;
        };
    };

    
} // namespace

#endif // __XMLSAXPARSERWITHQT_H__
