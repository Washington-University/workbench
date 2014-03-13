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

#include "CaretAssert.h"
#include "EventDataFileRead.h"

using namespace caret;

/**
 * Constructor.
 * 
 * @param loadIntoBrain
 *    Brain into which file is loaded.
 */
EventDataFileRead::EventDataFileRead(Brain* loadIntoBrain)
: Event(EventTypeEnum::EVENT_DATA_FILE_READ)
{
    this->loadIntoBrain = loadIntoBrain;
    
    this->username = "";
    this->password = "";
    
    CaretAssert(this->loadIntoBrain);
}

/**
 * Destructor.
 */
EventDataFileRead::~EventDataFileRead()
{
    
}

/**
 * Add a data file for reading.
 *
 * @param dataFileType
 *    Type of data file.
 * @param dataFileName
 *    Name of the data file.
 */
void
EventDataFileRead::addDataFile(const DataFileTypeEnum::Enum dataFileType,
                 const AString& dataFileName)
{
    CaretAssert(dataFileType != DataFileTypeEnum::UNKNOWN);
    CaretAssert(dataFileName.isEmpty() == false);
    
    m_dataFiles.push_back(FileData(StructureEnum::INVALID,
                                   dataFileType,
                                   dataFileName,
                                   false));
}

/**
 * Add a data file for reading.
 *
 * @param structure
 *    Structure for file if not present in file.
 * @param dataFileType
 *    Type of data file.
 * @param dataFileName
 *    Name of the data file.
 */
void
EventDataFileRead::addDataFile(const StructureEnum::Enum structure,
                 const DataFileTypeEnum::Enum dataFileType,
                 const AString& dataFileName)
{
    //CaretAssert(structure != StructureEnum::INVALID);
    CaretAssert(dataFileType != DataFileTypeEnum::UNKNOWN);
    CaretAssert(dataFileName.isEmpty() == false);

    m_dataFiles.push_back(FileData(structure,
                                   dataFileType,
                                   dataFileName,
                                   false));    
}

/**
 * @return Number of data files to read.
 */
int32_t
EventDataFileRead::getNumberOfDataFilesToRead() const
{
    return m_dataFiles.size();
}

/**
 * Get the name of the data file that is to be loaded.
 *
 * @param dataFileIndex
 *    Index of the data file.
 * @return Name of data file to load.
 */
AString 
EventDataFileRead::getDataFileName(const int32_t dataFileIndex) const
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return m_dataFiles[dataFileIndex].m_dataFileName;
}

/**
 * Get the type of data file for loading.
 *
 * @param dataFileIndex
 *    Index of the data file.
 * @return Type of file for loading.
 */
DataFileTypeEnum::Enum 
EventDataFileRead::getDataFileType(const int32_t dataFileIndex) const
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return m_dataFiles[dataFileIndex].m_dataFileType;
}

/**
 * Get brain into which file is loaded.
 
 * @return Brain into which file is loaded.
 */
Brain* 
EventDataFileRead::getLoadIntoBrain()
{
    return this->loadIntoBrain;
}
                           
/**
 * @param dataFileIndex
 *    Index of the data file.
 * @return The structure associated with the data file.
 */
StructureEnum::Enum 
EventDataFileRead::getStructure(const int32_t dataFileIndex) const
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return m_dataFiles[dataFileIndex].m_structure;
}

/**
 * Get the error message for a file.
 * @param dataFileIndex
 *    Index of the file.
 * @return
 *    Error message.
 */
AString
EventDataFileRead::getFileErrorMessage(const int32_t dataFileIndex) const
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return m_dataFiles[dataFileIndex].m_errorMessage;
}

/**
 * Set the error message for a file.
 * @param dataFileIndex
 *    Index of the file.
 * @param errorMessage
 *    Error message.
 */
void
EventDataFileRead::setFileErrorMessage(const int32_t dataFileIndex,
                                       const AString& errorMessage)
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    m_dataFiles[dataFileIndex].m_errorMessage = errorMessage;
}

/**
 * Was there an error reading the file at the given index.
 * @param dataFileIndex
 *    Index of the file.
 * @return
 *    true if there was an error, otherwise false.
 */
bool
EventDataFileRead::isFileError(const int32_t dataFileIndex) const
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return (m_dataFiles[dataFileIndex].m_errorMessage.isEmpty() == false);
}

/**
 * @param dataFileIndex
 *    Index of the data file.
 * @return  True if the file could not be read due
 * to an invalid structure.
 */
bool 
EventDataFileRead::isFileErrorInvalidStructure(const int32_t dataFileIndex) const
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return m_dataFiles[dataFileIndex].m_invalidStructureError;
}

/**
 * Set the invalid structure status.
 * @param dataFileIndex
 *    Index of the data file.
 * @param status
 *    New invalid structure status (true if invalid).
 */
void 
EventDataFileRead::setFileErrorInvalidStructure(const int32_t dataFileIndex,
                                            const bool status)
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    m_dataFiles[dataFileIndex].m_invalidStructureError = status;
}

/**
 * @return File that was read for the given index.
 */
CaretDataFile*
EventDataFileRead::getDataFileRead(const int32_t dataFileIndex)
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return m_dataFiles[dataFileIndex].m_caretDataFileThatWasRead;
}

/**
 * Set the file that was read.
 *
 * @param dataFileIndex
 *    Index of the file.
 * @param caretDataFile
 *    Pointer to file that was read for given index.
 */
void
EventDataFileRead::setDataFileRead(const int32_t dataFileIndex,
                                   CaretDataFile* caretDataFile)
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    m_dataFiles[dataFileIndex].m_caretDataFileThatWasRead = caretDataFile;
}

/**
 * @return The username.
 */
AString 
EventDataFileRead::getUsername() const
{
    return this->username;
}

/**
 * @return The password.
 */
AString EventDataFileRead::getPassword() const
{
    return this->password;
}

/**
 * Set the username and password.
 *
 * @param username
 *     Name of user account.
 * @param password
 *     Password of user account.
 */
void 
EventDataFileRead::setUsernameAndPassword(const AString& username,
                                          const AString& password)
{
    this->username = username;
    this->password = password;
}

/**
 * @param dataFileIndex
 *    Index of the data file.
 * @return After file is read, mark it as modified.
 */
bool
EventDataFileRead::isFileToBeMarkedModified(const int32_t dataFileIndex) const
{
    CaretAssertVectorIndex(m_dataFiles, dataFileIndex);
    return m_dataFiles[dataFileIndex].m_markFileAsModified;
}
