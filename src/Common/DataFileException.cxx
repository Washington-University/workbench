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

#include "DataFileException.h"
#include "FileInformation.h"

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
 * Constructor that accepts the exception messge.
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
 * Constructor that accepts name of file and the exception message.
 *
 * The exception message will become name of the file, a newline,
 * path of the file, a newline, and the description of the 
 * exception.
 *
 * @param  dataFileName  Name of the data file that caused exception.
 * @param  s  Description of the exception.
 *
 */
DataFileException::DataFileException(const AString& dataFileName,
                                     const AString& s)
: CaretException(s)
{
    this->initializeMembersDataFileException();
    
    AString msg;
    if ( ! dataFileName.isEmpty()) {
        FileInformation fileInfo(dataFileName);
        const AString pathName = fileInfo.getPathName();
        
        msg.appendWithNewLine("NAME OF FILE: " + fileInfo.getFileName());
        if ( ! pathName.isEmpty()) {
            if (pathName != ".") {
                msg.appendWithNewLine("PATH TO FILE: " + pathName);
            }
        }
        msg.append("\n");
    }
    msg.appendWithNewLine(s);

    this->setExceptionDescription(msg);
}

/**
 * Copy Constructor.
 * @param e
 *     Exception that is copied.
 */
DataFileException::DataFileException(const DataFileException& e)
: CaretException(e)
{
    this->errorInvalidStructure = e.errorInvalidStructure;
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
        this->errorInvalidStructure = e.errorInvalidStructure;
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
    this->errorInvalidStructure = false;
}

/**
 * @return  True if the file could not be read due 
 * to an invalid structure.
 */
bool 
DataFileException::isErrorInvalidStructure() const
{
    return this->errorInvalidStructure;
}

/**
 * Set the invalid structure status when the file 
 * cannot be read due to the structure being invalid.
 *
 * @param status
 *    New invalid structure status (true if invalid).
 */
void 
DataFileException::setErrorInvalidStructure(const bool status)
{
    this->errorInvalidStructure = status;
}
