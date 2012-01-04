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

#include <fstream>
#include <memory>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#define __SPEC_FILE_DEFINE__
#include "SpecFile.h"
#undef __SPEC_FILE_DEFINE__
#include "SpecFileDataFileTypeGroup.h"
#include "SpecFileSaxReader.h"
#include "SystemUtilities.h"
#include "XmlSaxParser.h"
#include "XmlWriter.h"

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
    this->initializeSpecFile();
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
 * Copy constructor.
 * @param sf
 *    Spec file whose data is copied.
 */
SpecFile::SpecFile(const SpecFile& sf)
: DataFile(sf)
{
    this->initializeSpecFile();
    this->copyHelperSpecFile(sf);
}

/**
 * Assignment operator.
 * @param sf
 *    Spec file whose data is copied.
 * @return
 *    Reference to this file.
 */
SpecFile& 
SpecFile::operator=(const SpecFile& sf)
{
    if (this != &sf) {
        this->copyHelperSpecFile(sf);
    }
    
    return *this;
}

void 
SpecFile::initializeSpecFile()
{
    this->metadata = new GiftiMetaData();
    
    std::vector<DataFileTypeEnum::Enum> allEnums;
    DataFileTypeEnum::getAllEnums(allEnums, false, false);
    
    /*
     * Do surface files first since they need to be loaded before other files
     */
    for (std::vector<DataFileTypeEnum::Enum>::iterator iter = allEnums.begin();
         iter != allEnums.end();
         iter++) {
        DataFileTypeEnum::Enum dataFileType = *iter;
        const AString typeName = DataFileTypeEnum::toName(dataFileType);
        if (typeName.startsWith("SURFACE")) {
            SpecFileDataFileTypeGroup* dftg = new SpecFileDataFileTypeGroup(dataFileType);
            this->dataFileTypeGroups.push_back(dftg);
        }
    }
    
    /*
     * Do remaining file types excluding surfaces
     */
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
 * Copy helper.
 * @param sf
 *    Spec file whose data is copied.
 */
void 
SpecFile::copyHelperSpecFile(const SpecFile& sf)
{
    this->clearData();
    
    if (this->metadata != NULL) {
        delete this->metadata;
    }
    this->metadata = new GiftiMetaData(*sf.metadata);
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = sf.dataFileTypeGroups.begin();
         iter != sf.dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* group = *iter;
        const int numFiles = group->getNumberOfFiles();
        for (int32_t i = 0; i < numFiles; i++) {
            SpecFileDataFile* file = group->getFileInformation(i);
            this->addDataFile(group->getDataFileType(), 
                              file->getStructure(), 
                              file->getFileName());
        }
    }
    
    this->setFileName(sf.getFileName());
    this->clearModified();
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
    AString name = filename;
    
    if (this->getFileName().isEmpty() == false) {
        name = SystemUtilities::relativePath(name, FileInformation(this->getFileName()).getPathName());
    }
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->getDataFileType() == dataFileType) {
            /*
             * If already in file, no need to add it a second time.
             */
            const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
            for (int32_t i = 0; i < numFiles; i++) {
                if (dataFileTypeGroup->getFileInformation(i)->getFileName() == name) {
                    return;
                }
            }
            
            SpecFileDataFile* sfdf = new SpecFileDataFile(name,
                                                          structure);
            dataFileTypeGroup->addFileInformation(sfdf);
            return;
        }
    }
                        
    DataFileException e("Data File Type: " 
                        + DataFileTypeEnum::toName(dataFileType)
                        + " not allowed "
                        + " for file "
                        + filename);
    CaretLogThrowing(e);
    throw e;
}

/**
 * @return ALL of the connectivity file types (NEVER delete contents of returned vector.
 */
void 
SpecFile::getAllConnectivityFileTypes(std::vector<SpecFileDataFile*>& connectivityDataFilesOut)
{
    connectivityDataFilesOut.clear();
    
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (DataFileTypeEnum::isConnectivityDataType(dataFileTypeGroup->getDataFileType())) {
            const int32_t numFiles = dataFileTypeGroup->getNumberOfFiles();
            for (int32_t i = 0; i < numFiles; i++) {
                connectivityDataFilesOut.push_back(dataFileTypeGroup->getFileInformation(i));
            }
        }
    }
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
    
    this->clearData();
}

/**
 * Clear the spec file's data as if there were no files loaded.
 */
void 
SpecFile::clearData()
{
    this->metadata->clear();
    
    /*
     * Do not clear the vector, just remove file information from all types
     */
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
    this->setFileName(filename);

    try {
        //
        // Format the version string so that it ends with at most one zero
        //
        const AString versionString = AString::number(1.0);
        
        //
        // Open the file
        //
        char* name = this->getFileName().toCharArray();
        std::ofstream xmlFileOutputStream(name);
        delete[] name;
        if (! xmlFileOutputStream) {
            AString msg = "Unable to open " + this->getFileName() + " for writing.";
            throw DataFileException(msg);
        }
        //
        // Create the xml writer
        //
        XmlWriter xmlWriter(xmlFileOutputStream);
        
        //
        // Write header info
        //
        xmlWriter.writeStartDocument("1.0");
        
        //
        // Write GIFTI root element
        //
        XmlAttributes attributes;
        
        //attributes.addAttribute("xmlns:xsi",
        //                        "http://www.w3.org/2001/XMLSchema-instance");
        //attributes.addAttribute("xsi:noNamespaceSchemaLocation",
        //                        "http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd");
        attributes.addAttribute(SpecFile::XML_ATTRIBUTE_VERSION,
                                versionString);
        xmlWriter.writeStartElement(SpecFile::XML_TAG_SPEC_FILE,
                                           attributes);
        
        //
        // Write Metadata
        //
        if (metadata != NULL) {
            metadata->writeAsXML(xmlWriter);
        }
    
        /*
         * Remove any files that are tagged for removal.
         */
        this->removeFilesTaggedForRemoval();
        
        //
        // Write files
        //
        const int32_t numGroups = this->getNumberOfDataFileTypeGroups();
        for (int32_t i = 0; i < numGroups; i++) {
            SpecFileDataFileTypeGroup* group = this->getDataFileTypeGroup(i);
            const int32_t numFiles = group->getNumberOfFiles();
            for (int32_t j = 0; j < numFiles; j++) {
                SpecFileDataFile* file = group->getFileInformation(j);
                
                if (file->isRemovedFromSpecFileWhenWritten() == false) {
                    XmlAttributes atts;
                    atts.addAttribute(SpecFile::XML_ATTRIBUTE_STRUCTURE, 
                                      StructureEnum::toGuiName(file->getStructure()));
                    atts.addAttribute(SpecFile::XML_ATTRIBUTE_DATA_FILE_TYPE, 
                                      DataFileTypeEnum::toName(group->getDataFileType()));
                    xmlWriter.writeStartElement(SpecFile::XML_TAG_DATA_FILE, 
                                                atts);
                    xmlWriter.writeCharacters("      " + file->getFileName() + "\n");
                    xmlWriter.writeEndElement();
                }
            }
        }
        
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        
        xmlFileOutputStream.close();
        
        this->clearModified();
    }
    catch (GiftiException e) {
        throw DataFileException(e);
    }
    catch (XmlException e) {
        throw DataFileException(e);
    }
}

/**
 * If any files are marked for removal from SpecFile, remove them.
 */
void 
SpecFile::removeFilesTaggedForRemoval()
{
    const int32_t numGroups = static_cast<int32_t>(this->dataFileTypeGroups.size());
    for (int32_t ig = 0; ig < numGroups; ig++) {
        SpecFileDataFileTypeGroup* group = this->dataFileTypeGroups[ig];
        group->removeFilesTaggedForRemoval();
    }
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
 * @return Have any files in this spec file been edited (typically through spec file dialog?
 */
bool 
SpecFile::hasBeenEdited() const
{
    for (std::vector<SpecFileDataFileTypeGroup*>::const_iterator iter = dataFileTypeGroups.begin();
         iter != dataFileTypeGroups.end();
         iter++) {
        SpecFileDataFileTypeGroup* dataFileTypeGroup = *iter;
        if (dataFileTypeGroup->hasBeenEdited()) {
            return true;
        }
    }
    return false;
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



