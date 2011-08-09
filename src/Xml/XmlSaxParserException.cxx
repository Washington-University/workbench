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
 * @param  s  
 *      Description of the exception.
 * @param  lineNumber
 *      Line number of the parsing exception.
 * @param  columnNumber
 *      Column number of the parsing exception.
 *
 */
XmlSaxParserException::XmlSaxParserException(const QString& s, 
                                         const int32_t lineNumber, 
                                         const int32_t columnNumber)
: CaretException(s)
{
    this->initializeMembersXmlSaxParserException();
    this->lineNumber = lineNumber;
    this->columnNumber = columnNumber;
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
XmlSaxParserException::XmlSaxParserException(const QString& s)
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
    this->lineNumber = -1;
    this->columnNumber = -1;
}

