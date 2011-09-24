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

#include "CaretLogger.h"
#include "SpecFile.h"

using namespace caret;

/**
 * \class SpecFile
 * \brief A spec file groups caret data files.
 */


/**
 * Constructor.
 */
SpecFile::SpecFile()
: DataFile()
{
}

/**
 * Destructor.
 */
SpecFile::~SpecFile()
{
}

/**
 * Clear the contents of this file.
 */
void 
SpecFile::clear()
{
    DataFile::clear();
}

/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
SpecFile::isEmpty() const
{
    return true;
}

/**
 * Read the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
SpecFile::readFile(const AString& filename) throw (DataFileException)
{
    this->setFileName(filename);
    
    this->clearModified();
}

/**
 * Write the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
SpecFile::writeFile(const AString& filename) throw (DataFileException)
{
    
    this->setFileName(filename);
    
    this->clearModified();
}

/**
 * Get information about this file's contents.
 * @return
 *    Information about the file's contents.
 */
AString 
SpecFile::toString() const
{
    return "SpecFile";
}



