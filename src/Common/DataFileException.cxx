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


#include "DataFileException.h"

#include <cstring>

using namespace caret;

/**
 * Constructor.
 *
 */
DataFileException::DataFileException()
: CaretException()
{
    this->initializeMembersDataFileException();
}

/**
 * Constructor that uses stack trace from the exception
 * passed in as a parameter.
 * 
 * @param e Any exception whose stack trace becomes
 * this exception's stack trace.
 *
 */
DataFileException::DataFileException(
                    const CaretException& e)
: CaretException(e)
{
    this->initializeMembersDataFileException();
}

/**
 * Constructor.
 *
 * @param  s  Description of the exception.
 *
 */
DataFileException::DataFileException(const AString& s)
: CaretException(s)
{
    this->initializeMembersDataFileException();
}

/**
 * Copy Constructor.
 * @param e
 *     Exception that is copied.
 */
DataFileException::DataFileException(const DataFileException& e)
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
DataFileException& 
DataFileException::operator=(const DataFileException& e)
{
    if (this != &e) {
        CaretException::operator=(e);
    }
    
    return *this;
}

/**
 * Destructor
 */
DataFileException::~DataFileException() throw()
{
}

void
DataFileException::initializeMembersDataFileException()
{
}

