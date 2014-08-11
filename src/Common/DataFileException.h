#ifndef __DATAFILEEXCEPTION_H__
#define __DATAFILEEXCEPTION_H__

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

namespace caret {

/**
 * An exception thrown during data file operations.
 */
class DataFileException : public CaretException {

public:
    DataFileException();

    DataFileException(const CaretException& e);

    DataFileException(const AString& s);

    DataFileException(const AString& dataFileName,
                      const AString& s);
    
    DataFileException(const DataFileException& e);
    
    DataFileException& operator=(const DataFileException& e);
    
    virtual ~DataFileException() throw();
    
    bool isErrorInvalidStructure() const;
    
    void setErrorInvalidStructure(const bool status);
    
private:
        
    void initializeMembersDataFileException();
    
    bool errorInvalidStructure;    
};

} // namespace

#endif // __DATAFILEEXCEPTION_H__
