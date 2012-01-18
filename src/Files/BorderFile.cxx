
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

#include <algorithm>
#include <fstream>
#include <memory>
#define __BORDER_FILE_DECLARE__
#include "BorderFile.h"
#undef __BORDER_FILE_DECLARE__

#include "Border.h"
#include "BorderFileSaxReader.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiMetaData.h"
#include "XmlAttributes.h"
#include "XmlSaxParser.h"
#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class BorderFile 
 * \brief File containing borders.
 */

/**
 * Constructor.
 */
BorderFile::BorderFile()
: CaretDataFile(DataFileTypeEnum::BORDER)
{
    this->initializeBorderFile();
}

/**
 * Destructor.
 */
BorderFile::~BorderFile()
{
    delete this->metadata;
    
    for (std::vector<Border*>::iterator iter = this->borders.begin();
         iter != this->borders.end();
         iter++) {
        delete *iter;
    }
    this->borders.clear();
}

/**
 * Initialize members of a border file.
 */
void 
BorderFile::initializeBorderFile()
{
    this->metadata = new GiftiMetaData();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BorderFile::BorderFile(const BorderFile& obj)
: CaretDataFile(obj)
{
    this->initializeBorderFile();

    this->copyHelperBorderFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
BorderFile&
BorderFile::operator=(const BorderFile& obj)
{
    if (this != &obj) {
        this->clear();
        CaretDataFile::operator=(obj);
        this->copyHelperBorderFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
BorderFile::copyHelperBorderFile(const BorderFile& obj)
{
    *this->metadata = *obj.metadata;
    
    const int32_t numBorders = obj.getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        this->borders.push_back(new Border(*obj.getBorder(i)));
    }
    
    this->setModified();
}

/**
 * @return Is this border file empty (contains zero borders)?
 */
bool 
BorderFile::isEmpty() const
{
    return this->borders.empty();
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
BorderFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
BorderFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    // does nothing since borders apply to all structures
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
BorderFile::getFileMetaData()
{
    return this->metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
BorderFile::getFileMetaData() const
{
    return this->metadata;
}

/**
 * Clear the border file.
 */
void 
BorderFile::clear()
{
    CaretDataFile::clear();
    this->metadata->clear();
    const int32_t numBorders = this->getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        delete this->borders[i];
    }
    this->borders.clear();
}

/**
 * @return the number of borders.
 */
int32_t 
BorderFile::getNumberOfBorders() const
{
    return this->borders.size();
}

/**
 * Get the border at the given index.
 * @param indx
 *   Index of the border.
 * @return
 *   Border at the given index.
 */
Border* 
BorderFile::getBorder(const int32_t indx)
{
    CaretAssertVectorIndex(this->borders, indx);
    return this->borders[indx];
}

/**
 * Get the border at the given index.
 * @param indx
 *   Index of the border.
 * @return
 *   Border at the given index.
 */
const Border* 
BorderFile::getBorder(const int32_t indx) const
{
    CaretAssertVectorIndex(this->borders, indx);
    return this->borders[indx];
}

/**
 * Add a border.  NOTE: This border file
 * takes ownership of the 'border' and 
 * will handle deleting it.  After calling 
 * this method, the caller must never
 * do anything with the border that was passed
 * to this method.
 *
 * @param border
 *    Border added to this border file.
 */
void 
BorderFile::addBorder(Border* border)
{
    this->borders.push_back(border);
    this->setModified();
}

/**
 * Remove the border at the given index.
 * @param indx
 *   Index of border for removal.
 */
void 
BorderFile::removeBorder(const int32_t indx)
{
    CaretAssertVectorIndex(this->borders, indx);
    this->borders.erase(this->borders.begin() + indx);
    this->setModified();
}

/**
 * Remove the border.
 * @param border
 *   Border that will be removed and DELETED.
 */
void 
BorderFile::removeBorder(Border* border)
{
    std::vector<Border*>::iterator iter = std::find(this->borders.begin(),
                                                    this->borders.end(),
                                                    border);
    if (iter != this->borders.end()) {
        this->borders.erase(iter);
        this->setModified();
    }
    else {
        CaretLogWarning("Attempting to delete border not in border file with name: "
                        + border->getName());
    }
}

/**
 * @return The version of the file as a number.
 */
float 
BorderFile::getFileVersion()
{
    return BorderFile::borderFileVersion;
}

/**
 * @return The version of the file as a string.
 */
AString 
BorderFile::getFileVersionAsString()
{
    return AString::number(BorderFile::borderFileVersion);
}

/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void 
BorderFile::readFile(const AString& filename) throw (DataFileException)
{
    BorderFileSaxReader saxReader(this);
    std::auto_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
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
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void 
BorderFile::writeFile(const AString& filename) throw (DataFileException)
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
        attributes.addAttribute(BorderFile::XML_ATTRIBUTE_VERSION,
                                versionString);
        xmlWriter.writeStartElement(BorderFile::XML_TAG_BORDER_FILE,
                                    attributes);
        
        //
        // Write Metadata
        //
        if (metadata != NULL) {
            metadata->writeAsXML(xmlWriter);
        }
                
        //
        // Write borders
        //
        const int32_t numBorders = this->getNumberOfBorders();
        for (int32_t i = 0; i < numBorders; i++) {
            this->borders[i]->writeAsXML(xmlWriter);
        }
        
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        
        xmlFileOutputStream.close();
        
        this->clearModified();
    }
    catch (const GiftiException& e) {
        throw DataFileException(e);
    }
    catch (const XmlException& e) {
        throw DataFileException(e);
    }
}
