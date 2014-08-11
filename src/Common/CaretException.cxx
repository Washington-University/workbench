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


#include "CaretException.h"

#include "SystemUtilities.h"
#include <cstring>

using namespace caret;

/**
 * Constructor.
 *
 */
CaretException::CaretException()
: std::runtime_error("")
{
    this->initializeMembersCaretException();
}

/**
 * Constructor.
 *
 * @param  s  Description of the exception.
 *
 */
CaretException::CaretException(
                   const AString& s)
: std::runtime_error(s.toStdString())
{
    this->initializeMembersCaretException();
    this->exceptionDescription = s;
}

/**
 * Copy Constructor.
 * @param e
 *     Exception that is copied.
 */
CaretException::CaretException(const CaretException& e)
: std::runtime_error(e)
{
    this->exceptionDescription = e.exceptionDescription;
    this->callStack = e.callStack;
}

/**
 * Assignment operator.
 * @param e
 *     Exception that is copied.
 * @return 
 *     Copy of the exception.
 */
CaretException& 
CaretException::operator=(const CaretException& e)
{
    if (this != &e) {
        std::runtime_error::operator=(e);
        this->exceptionDescription = e.exceptionDescription;
        this->callStack = e.callStack;
    }
    
    return *this;
}

/**
 * Destructor
 */
CaretException::~CaretException() throw()
{
}

void
CaretException::initializeMembersCaretException()
{
    this->exceptionDescription = "";
    this->callStack = SystemUtilities::getBackTrace();
}

/**
 * Get the current call stack.
 * @return String containing the call stack.
 */;
AString 
CaretException::getCallStack() const
{
    return callStack;
}

/**
 * Get a message describing the exception.
 * @return A message describing the exception.
 */
AString
CaretException::whatString() const throw() 
{
    return this->exceptionDescription;  
}

/**
 * Allow subclasses to override the exception description.
 *
 * @param  s  Description of the exception.
 */
void
CaretException::setExceptionDescription(const AString& s)
{
    this->exceptionDescription = s;
}
