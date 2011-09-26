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

#include <memory>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiMetaData.h"
#define __SPEC_FILE_DEFINE__
#include "SpecFile.h"
#undef __SPEC_FILE_DEFINE__
#include "SpecFileDataFileTypeGroup.h"
#include "SpecFileSaxReader.h"
#include "XmlSaxParser.h"

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
    this->metadata = new GiftiMetaData();
    
    std::vector<DataFileTypeEnum::Enum> allEnums;
    DataFileTypeEnum::getAllEnums(allEnums);
    
    for (std::vector<DataFileTypeEnum::Enum>::iterator iter = allEnums.begin();
         iter != allEnums.end();
         iter++) {
        DataFileTypeEnum::Enum dataFileType = *iter;
        if (dataFileType == DataFileTypeEnum::UNKNOWN) {
            // ignore
        }
        else {
            const AString typeName = DataFileTypeEnum::toName(dataFileType);
            if (typeName.startsWith("SURFACE")) {
                SpecFileDataFileTypeGroup* dftg = new SpecFileDataFileTypeGroup(dataFileType);
                this->dataFileTypeGroups.push_back(dftg);
            }
        }
    }
    
    for (std::vector<DataFileTypeEnum::Enum>::iterator iter = allEnums.begin();
         iter != allEnums.end();
         iter++) {
        DataFileTypeEnum::Enum dataFileType = *iter;
        if (dataFileType == DataFileTypeEnum::UNKNOWN) {
            // ignore
        }
        else {
            const AString typeName = DataFileTypeEnum::toName(dataFileType);
            if (typeName.startsWith("SURFACE") == false) {
                SpecFileDataFileTypeGroup* dftg = new SpecFileDataFileTypeGroup(dataFileType);
                this->dataFileTypeGroups.push_back(dftg);
            }
        }
    }
}

/**
 * Destructor.
 */
SpecFile::~SpecFile()
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        delete *iter;
    }
    this->dataFileTypeGroups.clear();
    delete this->metadata;
    this->metadata = NULL;
}

/**
 * Add a data file to this spec file.
 * @param dataFileType
 *   Type of data file.
 * @param structure
 *   Structure of data file (not all files use structure).
 * @param filename
 *   Name of the file.
 *
 * @throws DataFileException
 *   If data file type is UNKNOWN.
 */
void 
SpecFile::addDataFile(const DataFileTypeEnum::Enum dataFileType,
                      const StructureEnum::Enum structure,
                      const AString& filename) throw (DataFileException)
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->getDataFileType() == dataFileType) {
            SpecFileDataFile* sfdf = new SpecFileDataFile(filename,
                                                          structure);
            dataFileTypeGroup->addFileInformation(sfdf);
            return;
        }
    }
    
    DataFileException e("Data File Type: " 
                        + DataFileTypeEnum::toName(dataFileType)
                        + " not allowed.");
    CaretLogThrowing(e);
    throw e;
}

/**
 * Add a data file to this spec file.
 * @param dataFileTypeName
 *   Name of type of data file.
 * @param structure
 *   Name of Structure of data file (not all files use structure).
 * @param filename
 *   Name of the file.
 *
 * @throws DataFileException
 *   If data file type is UNKNOWN.
 */
void 
SpecFile::addDataFile(const AString& dataFileTypeName,
                 const AString& structureName,
                 const AString& filename) throw (DataFileException)
{
    bool validType = false;
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromName(dataFileTypeName, &validType);
    bool validStructure = false;
    StructureEnum::Enum structure = StructureEnum::fromGuiName(structureName, &validStructure);
    this->addDataFile(dataFileType,
                      structure,
                      filename);
}


/**
 * Clear the file..
 */
void 
SpecFile::clear()
{
    DataFile::clear();
    this->metadata->clear();
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->removeAllFileInformation();
    }
}

/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
SpecFile::isEmpty() const
{
    return (this->getNumberOfFiles() <= 0);
}

/**
 * @return The number of files.
 */
int32_t 
SpecFile::getNumberOfFiles() const
{
    int count = 0;
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        count += dataFileTypeGroup->getNumberOfFiles();
    }
    
    return count;
}

/**
 * @return The number of files selected.
 */
int32_t 
SpecFile::getNumberOfFilesSelected() const
{
    int count = 0;
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        count += dataFileTypeGroup->getNumberOfFilesSelected();
    }
    
    return count;
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
    SpecFileSaxReader saxReader(this);
    std::auto_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (XmlSaxParserException& e) {
        this->setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        AString msg =
        "Parse Error while reading "
        + filename;
        
        if ((lineNum >= 0) && (colNum >= 0)) {
            msg += (" line/col ("
                    + AString::number(e.getLineNumber())
                    + "/"
                    + AString::number(e.getColumnNumber())
                    + ")");
        }
        
        msg += (": " + e.whatString());
        
        DataFileException dfe(msg);
        CaretLogThrowing(dfe);
        throw dfe;
    }

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
    CaretLogSevere("Not implemented");
    
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
    AString info = "name=" + this->getFileName() + "\n";
    info += this->metadata->toString() + "\n";
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        info += (dataFileTypeGroup->toString() + "\n");
    }
    
    return info;
}

/**
 * @return The number of data file type groups.
 */
int32_t 
SpecFile::getNumberOfDataFileTypeGroups() const
{
    return this->dataFileTypeGroups.size();
}

/**
 * Get the data file type group for the given index.
 * @param dataFileTypeGroupIndex
 *    Index of data file type group.
 * @return Data file type group at given index.
 */
SpecFileDataFileTypeGroup* 
SpecFile::getDataFileTypeGroup(const int32_t dataFileTypeGroupIndex)
{
    CaretAssertVectorIndex(this->dataFileTypeGroups, dataFileTypeGroupIndex);
    return this->dataFileTypeGroups[dataFileTypeGroupIndex];
}

/**
 * Get the data file type group for the given data file type.
 * @param dataFileType
 *    Data file type requested.
 * @return Data file type group for requested data file type or 
 *    NULL if no matching item found.
 */
SpecFileDataFileTypeGroup* 
SpecFile::getDataFileTypeGroup(const DataFileTypeEnum::Enum dataFileType)
{
    for (std::vector<SpecFileDataFileTypeGroup*>::iterator iter = dataFileTypeGroups.begin();
     iter != dataFileTypeGroups.end();
     iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->getDataFileType() == dataFileType) {
            return dataFileTypeGroup;
        }
    }
    
    return NULL;
}

/**
 * Set the file's selection status.
 * @param selected
 *    New selection status.
 */   
void 
SpecFile::setAllFilesSelected(bool selectionStatus)
{
    for (std::vector<SpecFileDataFileTypeGroup*>::iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        dataFileTypeGroup->setAllFilesSelected(selectionStatus);
    }    
}

/**
 * @return The version of the file as a number.
 */
float 
SpecFile::getFileVersion()
{
    return SpecFile::specFileVersion;
}

/**
 * @return The version of the file as a string.
 */
AString 
SpecFile::getFileVersionAsString()
{
    return AString::number(SpecFile::specFileVersion);
}



