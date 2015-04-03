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

#include "DataFile.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
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
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
DataFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    dataFileInformation.addNameAndValue("Name", getFileName());
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

/**
 * Is the filename a path on the network (http:// etc)
 * @param filename
 *     Name of file.
 * @return true if filename appears to be a network path.
 */
bool 
DataFile::isFileOnNetwork(const AString& filename)
{
    if (filename.startsWith("http://")
        || filename.startsWith("https://")) {
        return true;
    }
    
    return false;
}

/**
 * If the filename is local, make sure the file exists and
 * its permissions allow the file to be read.
 *
 * @param filename
 *    Name of file.
 * @throws DataFileException
 *    If there is a problem that will prevent the file from being read.
 */
void
DataFile::checkFileReadability(const AString& filename)
{
    if (filename.isEmpty()) {
        throw DataFileException("Name of file for reading is empty.");
    }
    
    if (isFileOnNetwork(filename)) {
        return;
    }
    
    FileInformation fileInfo(filename);
    if (fileInfo.exists() == false) {
        throw DataFileException(filename,
                                "File does not exist.");
    }
    
    if (fileInfo.isDirectory()) {
        throw DataFileException(filename,
                                "Filename is a directory, not a file.");
    }
    
    if (fileInfo.isReadable() == false) {
        throw DataFileException(filename,
                                "File is not readable due its permissions.");
    }
}

/**
 * If the filename is local, see if the file exists and
 * its permissions allow the file to be written.
 *
 * @param filename
 *    Name of file.
 * @throws DataFileException
 *    If there is a problem that will prevent the file from being read.
 */
void
DataFile::checkFileWritability(const AString& filename)
{
    if (filename.isEmpty()) {
        throw DataFileException("Name of file for writing is empty.");
    }
    
    if (isFileOnNetwork(filename)) {
        return;
    }
    
    FileInformation fileInfo(filename);
    if (fileInfo.exists()) {
        if (fileInfo.isDirectory()) {
            throw DataFileException(filename,
                                    "Filename is a directory, not a file.");
        }
        
        
        if (fileInfo.isWritable() == false) {
            throw DataFileException(filename,
                                    "File is not writable due its permissions.");
        }
    }
}

