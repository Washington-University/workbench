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


#include "XmlSaxParserException.h"

#include <cstring>

using namespace caret;

/**
 * Constructor.
 *
 */
XmlSaxParserException::XmlSaxParserException()
: CaretException()
{
    this->initializeMembersXmlSaxParserException();
}

/**
 * Constructor.
 *
 */
XmlSaxParserException::XmlSaxParserException(const CaretException& e)
: CaretException(e)
{
    this->initializeMembersXmlSaxParserException();
}

/**
 * Constructor.
 *
 * @param  s  
 *      Description of the exception.
 * @param  lineNumber
 *      Line number of the parsing exception.
 * @param  columnNumber
 *      Column number of the parsing exception.
 *
 */
XmlSaxParserException::XmlSaxParserException(const AString& s, 
                                         const int32_t lineNumber, 
                                         const int32_t columnNumber)
: CaretException(s)
{
    this->initializeMembersXmlSaxParserException();
    m_lineNumber = lineNumber;
    m_columnNumber = columnNumber;
}

/**
 * Constructor.
 *
 * @param  s  
 *      Description of the exception.
 * @param  lineNumber
 *      Line number of the parsing exception.
 * @param  columnNumber
 *      Column number of the parsing exception.
 *
 */
XmlSaxParserException::XmlSaxParserException(const AString& s)
: CaretException(s)
{
    this->initializeMembersXmlSaxParserException();
}

/**
 * Copy Constructor.
 * @param e
 *     Exception that is copied.
 */
XmlSaxParserException::XmlSaxParserException(const XmlSaxParserException& e)
: CaretException(e)
{
    m_lineNumber = e.m_lineNumber;
    m_columnNumber = e.m_columnNumber;
}

/**
 * Assignment operator.
 * @param e
 *     Exception that is copied.
 * @return 
 *     Copy of the exception.
 */
XmlSaxParserException& 
XmlSaxParserException::operator=(const XmlSaxParserException& e)
{
    if (this != &e) {
        CaretException::operator=(e);
    }
    m_lineNumber = e.m_lineNumber;
    m_columnNumber = e.m_columnNumber;
    
    return *this;
}

/**
 * Destructor
 */
XmlSaxParserException::~XmlSaxParserException() throw()
{
}

void
XmlSaxParserException::initializeMembersXmlSaxParserException()
{
    m_lineNumber = -1;
    m_columnNumber = -1;
}

/**
 * Get a message describing the exception.
 * @return A message describing the exception.
 */
AString
XmlSaxParserException::whatString() const throw()
{
    AString s = CaretException::whatString();
    if (m_lineNumber >= 0) {
        s += ("\nline number: "
              + AString::number(m_lineNumber));
    }
    if (m_columnNumber >= 0) {
        s += (" column number: "
              + AString::number(m_columnNumber));
    }
    return s;
}


