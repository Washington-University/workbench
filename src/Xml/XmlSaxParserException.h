#ifndef __XMLSAXPARSEREXCEPTION_H__
#define __XMLSAXPARSEREXCEPTION_H__

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
        
        XmlSaxParserException(const AString& s);

        XmlSaxParserException(const AString& s, const int32_t lineNumber, const int32_t columnNumber);
        
        XmlSaxParserException(const XmlSaxParserException& e);
        
        XmlSaxParserException& operator=(const XmlSaxParserException& e);
        
        virtual AString whatString() const throw();
        
        virtual ~XmlSaxParserException() throw();
        
        /**
         * Get the line number of the parsing exception.
         * @return Line Number of parsing exception.
         */
        int32_t getLineNumber() const { return m_lineNumber; }
        
        /**
         * Get the column number of the parsing exception.
         * @return Column Number of parsing exception.
         */
        int32_t getColumnNumber() const { return m_columnNumber; }
        
    private:
        
        void initializeMembersXmlSaxParserException();
        
        int32_t m_lineNumber;
        
        int32_t m_columnNumber;
    };
    
} // namespace

#endif // __XMLSAXPARSEREXCEPTION_H__
