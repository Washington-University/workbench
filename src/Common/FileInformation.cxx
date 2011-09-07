
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __FILE_INFORMATION_DECLARE__
#include "FileInformation.h"
#undef __FILE_INFORMATION_DECLARE__

#include <sys/stat.h>

using namespace caret;


/**
 * Constructor.
 * @param pathname
 *    Name of path for which information is obtained.
 */
FileInformation::FileInformation(const AString& pathname)
: CaretObject()
{
    this->pathname = pathname;
    
    this->fileSize = 0;
    this->pathExists = false;
    this->pathIsDirectory = false;
    this->pathIsFile = false;
    this->pathIsSymbolicLink = false;
    
    struct stat stats;
    int result = stat(qPrintable(this->pathname), &stats);    
    if (result >= 0) {
        this->pathExists = true;
        this->fileSize = stats.st_size;
        this->pathIsDirectory = S_ISDIR(stats.st_mode);
        this->pathIsFile      = S_ISREG(stats.st_mode);
        this->pathIsSymbolicLink = S_ISLNK(stats.st_mode);
    }
}

/**
 * Destructor.
 */
FileInformation::~FileInformation()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
FileInformation::toString() const
{
    return ("FileInformation for " + this->pathname);
}

