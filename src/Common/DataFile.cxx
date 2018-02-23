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

#include <QFileInfo>

#include "CaretLogger.h"
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
    m_filename = df.m_filename;
    m_fileReadWarnings = df.m_fileReadWarnings;
    m_modifiedFlag = false;
    m_timeOfLastReadOrWrite = QDateTime();
}

/**
 * Initialize the members of the data file.
 */
void 
DataFile::initializeMembersDataFile()
{
    m_filename = "";
    m_fileReadWarnings.clear();
    m_modifiedFlag = false;
    m_timeOfLastReadOrWrite = QDateTime();
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
    return m_filename;
}

/**
 * @return Name of the data file excluding any path.
 */
AString 
DataFile::getFileNameNoPath() const
{
    FileInformation fileInfo(m_filename);
    return fileInfo.getFileName();
}

/**
 * Set the name of the data file.
 * This method is virtual so NEVER call it from
 * a constructor.  Instead, use setFileNameProtected().
 *
 * @param filename
 *     New name of data file.
 */
void 
DataFile::setFileName(const AString& filename)
{
    setFileNameProtected(filename);
}

/**
 * Set the name of the data file.
 * This method is NOT virtual so it may be called
 * from a constructor.
 *
 * @param filename
 *     New name of data file.
 */
void
DataFile::setFileNameProtected(const AString& filename)
{
    if (m_filename != filename) {
        m_filename = filename;
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
    m_modifiedFlag = true;
}

/**
 * Set the status to unmodified.
 */
void 
DataFile::clearModified()
{
    m_modifiedFlag = false;
    
    /*
     * This method, clearModified(), is called by all file
     * types at the conclusion of readFile() and writeFile().
     */
    setTimeOfLastReadOrWrite();
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool 
DataFile::isModified() const
{
    return m_modifiedFlag;
}

/**
 * @return True if a file with the current file name exists, else false.
 */
bool
DataFile::exists() const
{
    FileInformation fileInfo(getFileName());
    return fileInfo.exists();
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

/**
 * Add a warning (non-fatal) message about some issue while reading file file.
 *
 * @param warning
 *     Warning message about some issue.
 */
void
DataFile::addFileReadWarning(const AString& warning)
{
    m_fileReadWarnings.appendWithNewLine(warning);
}

/**
 * @return Any warning message from reading the data file.
 *
 * NOTE: When this method is called, the warning message
 * is cleared for this file.
 */
AString
DataFile::getFileReadWarnings() const
{
    const AString warningMessage = m_fileReadWarnings;
    m_fileReadWarnings.clear();
    return warningMessage;
}

/**
 * Set the time this file was last read or written to the current time.
 */
void
DataFile::setTimeOfLastReadOrWrite()
{
    m_timeOfLastReadOrWrite = getLastModifiedTime();
}

/**
 * @return True if this file been modified since it was last read or written.
 * (modified by an external program)?
 *
 * If any of these conditions are met, false is returned:
 * (1) The name is empty; (2) The file is on the network;
 * (3) The file does not exist; (4) The modified time
 * from the file system is invalid; (5) The last time this
 * file was read or written is invalid.
 */
bool
DataFile::isModifiedSinceTimeOfLastReadOrWrite() const
{
    const QDateTime lastModTime = getLastModifiedTime();
    
    if (lastModTime.isNull()) {
        return false;
    }
    
    if (m_timeOfLastReadOrWrite.isNull()) {
        return false;
    }
    
    CaretLogFine("Testing reload of "
                 + getFileNameNoPath()
                 + " last read/write: "
                 + m_timeOfLastReadOrWrite.toString(Qt::ISODate)
                 + " last modified: "
                 + lastModTime.toString(Qt::ISODate)
                 + ((m_timeOfLastReadOrWrite == lastModTime) ? " EQUAL" : " NOT EQUAL"));
    
    /*
     * If last modified time is newer than
     * time of file read or written
     */
    if (lastModTime != m_timeOfLastReadOrWrite) {
        return true;
    }
    
    return false;
}

/**
 * @return True if this file been modified since it was last read or written.
 * (modified by an external program)?
 *
 * If any of these conditions are met, a null (.isNull()) is returned:
 * (1) The name is empty; (2) The file is on the network;
 * (3) The file does not exist; (4) The modified time
 * from the file system is invalid.
 */
QDateTime
DataFile::getLastModifiedTime() const
{
    QDateTime lastModTime;
    
    const AString name = getFileName();
    if (name.isEmpty()) {
        return lastModTime;
    }
    
    if (isFileOnNetwork(name)) {
        return lastModTime;
    }
    
    QFileInfo fileInfo(name);
    if ( ! fileInfo.exists()) {
        return lastModTime;
    }
    
    lastModTime = fileInfo.lastModified();
    
    return lastModTime;
}


