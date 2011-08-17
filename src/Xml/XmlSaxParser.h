#ifndef __XMLSAXPARSER_H__
#define __XMLSAXPARSER_H__

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


#include <AString.h>

#include "CaretObject.h"
#include "XmlSaxParserException.h"

namespace caret {
    
    class XmlSaxParserHandlerInterface;
    
    /**
     * Abstract class for XML SAX Parsing.  The design of this
     * class is based of the handler classes in Java.
     */
    class XmlSaxParser : public CaretObject {
        
    public:
        static XmlSaxParser* createXmlParser() throw (XmlSaxParserException);
        
        virtual ~XmlSaxParser();
        
    protected:
        XmlSaxParser();
        
    public:
        /**
         * Parse the contents of the specified file using
         * the specified handler.
         * 
         * @param filename
         *    Name of file that is to be parsed.
         * @param handler
         *    Handler that will be called to process XML
         *    as it is read.
         * @throws XmlSaxParserException
         *    If an error occurs.
         */
        virtual void parseFile(const QString& filename,
                   XmlSaxParserHandlerInterface* handler) throw (XmlSaxParserException) = 0;
        
        /**
         * Parse the contents of the strring using
         * the specified handler.
         * 
         * @param xmlString
         *    String whose contents is parsed.
         * @param handler
         *    Handler that will be called to process XML
         *    as it is read.
         * @throws XmlSaxParserException
         *    If an error occurs.
         */
        virtual void parseString(const QString& xmlString,
                               XmlSaxParserHandlerInterface* handler) throw (XmlSaxParserException) = 0;
        
    protected:
        XmlSaxParser(const XmlSaxParser& sp);
        XmlSaxParser& operator=(const XmlSaxParser&);
        
    private:
        
        void initializeMembersXmlSaxParser();
    };
    
} // namespace

#endif // __XMLSAXPARSER_H__
