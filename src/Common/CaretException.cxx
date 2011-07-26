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


#include "CaretException.h"
#include "SystemUtilities.h"
#include <cstring>

using namespace caret;

/**
 * Constructor.
 *
 */
CaretException::CaretException()
: std::exception()
{
    this->initializeMembersCaretException();
}

/**
 * Constructor that uses stack trace from the exception
 * passed in as a parameter.
 * 
 * @param e Any exception whose stack trace becomes
 * this exception's stack trace.
 *
 */
CaretException::CaretException(
                    const std::exception& e)
: std::exception(e)
{
    this->exceptionDescription = e.what();
}

/**
 * Constructor.
 *
 * @param  s  Description of the exception.
 *
 */
CaretException::CaretException(
                   const std::string& s)
: std::exception()
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
: std::exception(e)
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
        std::exception::operator=(e);
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
std::string 
CaretException::getCallStack() const
{
    return callStack;
}

/**
 * Get a message describing the exception.
 * @return A message describing the exception.
 */
std::string
CaretException::whatString() const throw() 
{
    return this->exceptionDescription;  
}

