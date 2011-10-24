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
/*LICENSE_END*/



#include "CiftiFileException.h"

using namespace caret;

CiftiFileException::CiftiFileException(const AString& msg)
{
    exceptionDescription = msg;
}

/**
 * Constructor.
 */
CiftiFileException::CiftiFileException(const AString& filename, const AString& msg)
{
    AString s("Error  ");
    s.append(filename);
    s.append(": ");
    s.append(msg);
    exceptionDescription = s;
}


/**
 * Destructor.
 */
CiftiFileException::~CiftiFileException() throw()
{
}

/**
 * Text message describing exception.
 */

const char*
CiftiFileException::what() const throw() 
{
    return exceptionDescription.toAscii().constData();
}

/**
 * Text message describing exception.
 */
AString
CiftiFileException::whatAString() const throw()
{
    return exceptionDescription;
}
