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

#include "DataFile.h"
#include "FileInformation.h"

using namespace caret;

/**
 * Constructor.
 */
DataFile::DataFile()
: CaretObject(), DataFileInterface()
{
    this->initializeMembersDataFile();
}

/**
 * Destructor.
 */
DataFile::~DataFile()
{
    this->initializeMembersDataFile();
}

/**
 * Copy constructor.
 
 * @param df
 *     Data file that is copied.
 */
DataFile::DataFile(const DataFile& df)
: CaretObject(df), DataFileInterface()
{
    this->copyHelperDataFile(df);
}

/**
 * Assignment operator.
 
 * @param df
 *     Contents that is assigned to this file.
 * @return
 *     This file assigned the contents of "df".
 */
DataFile& 
DataFile::operator=(const DataFile& df)
{
    if (this != &df) {
        CaretObject::operator=(df);
        this->copyHelperDataFile(df);
    }
    return *this;
}

/**
 * Assists with copying file's contents.
 */
void 
DataFile::copyHelperDataFile(const DataFile& df)
{
    this->filename = df.filename;
    modifiedFlag = false;
}

/**
 * Initialize the members of the data file.
 */
void 
DataFile::initializeMembersDataFile()
{
    this->filename = "";
    modifiedFlag = false;
}

/**
 * Clear the contents of the file.
 */
void 
DataFile::clear()
{
    this->initializeMembersDataFile();
}

/**
 * @return Name of the data file including any path.
 */
AString 
DataFile::getFileName() const
{
    return this->filename;
}

/**
 * @return Name of the data file excluding any path.
 */
AString 
DataFile::getFileNameNoPath() const
{
    FileInformation fileInfo(this->filename);
    return fileInfo.getFileName();
}

/**
 * Set the name of the data file.
 *
 * @param filename
 *     New name of data file.
 */
void 
DataFile::setFileName(const AString& filename)
{
    if (this->filename != filename) {
        this->filename = filename;
        this->setModified();
    }
}

/**
 * Set the status to modified.
 */
void 
DataFile::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set the status to unmodified.
 */
void 
DataFile::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool 
DataFile::isModified() const
{
    return this->modifiedFlag;
}
