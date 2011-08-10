#ifndef __XMLSAXPARSEREXCEPTION_H__
#define __XMLSAXPARSEREXCEPTION_H__

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


#include <exception>
#include <QString>

#include "CaretException.h"
#include <stdint.h>

namespace caret {
    
    /**
     * An exception thrown during SAX XML parsing.
     */
    class XmlSaxParserException : public CaretException {
        
    public:
        XmlSaxParserException();
        
        XmlSaxParserException(const CaretException& e);
        
        XmlSaxParserException(const QString& s);
        
        XmlSaxParserException(const QString& s, const int32_t lineNumber, const int32_t columnNumber);
        
        XmlSaxParserException(const XmlSaxParserException& e);
        
        XmlSaxParserException& operator=(const XmlSaxParserException& e);
        
        virtual ~XmlSaxParserException() throw();
        
        /**
         * Get the line number of the parsing exception.
         * @return Line Number of parsing exception.
         */
        int32_t getLineNumber() const { return lineNumber; }
        
        /**
         * Get the column number of the parsing exception.
         * @return Column Number of parsing exception.
         */
        int32_t getColumnNumber() const { return columnNumber; }
        
    private:
        
        void initializeMembersXmlSaxParserException();
        
        int32_t lineNumber;
        
        int32_t columnNumber;
    };
    
} // namespace

#endif // __XMLSAXPARSEREXCEPTION_H__
