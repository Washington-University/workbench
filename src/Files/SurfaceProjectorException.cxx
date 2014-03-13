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


#include "SurfaceProjectorException.h"

#include <cstring>

using namespace caret;

/**
 * Constructor.
 *
 */
SurfaceProjectorException::SurfaceProjectorException()
: CaretException()
{
    this->initializeMembersSurfaceProjectorException();
}

/**
 * Constructor.
 *
 * @param  s  Description of the exception.
 *
 */
SurfaceProjectorException::SurfaceProjectorException(
                   const AString& s)
: CaretException(s)
{
    this->initializeMembersSurfaceProjectorException();
}

/**
 * Copy Constructor.
 * @param e
 *     Exception that is copied.
 */
SurfaceProjectorException::SurfaceProjectorException(const SurfaceProjectorException& e)
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
SurfaceProjectorException& 
SurfaceProjectorException::operator=(const SurfaceProjectorException& e)
{
    if (this != &e) {
        CaretException::operator=(e);
    }
    
    return *this;
}

/**
 * Destructor
 */
SurfaceProjectorException::~SurfaceProjectorException() throw()
{
}

void
SurfaceProjectorException::initializeMembersSurfaceProjectorException()
{
}

