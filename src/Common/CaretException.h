#ifndef __CARETEXCEPTION_H__
#define __CARETEXCEPTION_H__

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


#include <stdexcept>
#include <AString.h>

namespace caret {

/**
 * An exception thrown during Caret processing.
 */
class CaretException : public std::runtime_error {

public:
    CaretException();

    CaretException(const AString& s);

    CaretException(const CaretException& e);
        
    CaretException& operator=(const CaretException& e);
        
    virtual ~CaretException() throw();
    
    virtual AString whatString() const throw();

    AString getCallStack() const;
        
protected:
    void setExceptionDescription(const AString& s);
        
private:
    /// Description of the exception
    AString exceptionDescription;
    
    /// the call stack
    AString callStack;
        
    void initializeMembersCaretException();
};

} // namespace

#endif // __CARETEXCEPTION_H__
