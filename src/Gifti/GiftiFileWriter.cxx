
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

#include <fstream>
#include <memory>

#define __GIFTI_FILE_WRITER_DECLARE__
#include "GiftiFileWriter.h"
#undef __GIFTI_FILE_WRITER_DECLARE__

#include "CaretHierarchy.h"
#include "FileInformation.h"
#include "GiftiDataArray.h"
#include "GiftiXmlElements.h"

#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class GiftiFileWriter 
 * \brief Writes GIFTI files.
 *
 * Writes a GIFTI data array file.  In most cases, using the
 * GiftiDataArrayFile's writeFile() method should be used for writing a file.
 * This class can be used to incrementally write a file.  After constructing
 * an object of this class, call start(), call writeDataArray() for each
 * data array, and lastly call finish().
 */
/**
 * Constructor.
 * 
 * @param filename - name of the file.
 * @param encoding - encoding of the file.
 */
GiftiFileWriter::GiftiFileWriter(const AString& filename,
                                 const GiftiEncodingEnum::Enum encoding)
: CaretObject()
{
    this->xmlFileOutputStream = NULL;
    this->externalFileOutputStream = NULL;
    this->maximumExternalFileSize = 1024 * 1024 * 1024;
    this->filename = filename;
    this->encoding = encoding;
    this->xmlWriter = NULL;
    this->dataArraysWrittenCounter = 0;
}

/**
 * Destructor.
 */
GiftiFileWriter::~GiftiFileWriter()
{
    this->closeFiles();
    
    if (this->xmlWriter != NULL) {
        delete this->xmlWriter;
        this->xmlWriter = NULL;
    }
}

/**
 * Start writing the file.
 *
 * @param numberOfDataArrays - The number of data arrays that for file
 *    that is being written.
 * @param metadata - The file's metadata or null if no metadata.
 * @param labelTable - The file's label table or null if no labels.
 * @throws GiftiException - If an error occurs.
 */
void 
GiftiFileWriter::start(const int numberOfDataArrays,
           GiftiMetaData* metadata,
           GiftiLabelTable* labelTable)
{
    this->numberOfDataArrays = numberOfDataArrays;
    
    //
    // Does the file need to be opened?
    //
    if (this->xmlFileOutputStream == NULL) {
        //
        // Open the file
        //
        char* name = this->filename.toCharArray();
        this->xmlFileOutputStream = new std::ofstream(name);
        delete[] name;
        if (! this->xmlFileOutputStream->good()) {
            delete this->xmlFileOutputStream;
            this->xmlFileOutputStream = NULL;
            AString msg = "Unable to open " + this->filename + " for writing.";
            throw GiftiException(msg);
        }
                
        //
        // Remove any existing external files.
        //
        this->removeExternalFiles();
    }
    
    try {
        //
        // Format the version string so that it ends with at most one zero
        //
        const AString versionString = AString::number(GiftiFile::getCurrentFileVersion());
        
        //
        // Create the xml writer
        //
        this->xmlWriter = new XmlWriter(*this->xmlFileOutputStream);
        
        //
        // Write header info
        //
        this->xmlWriter->writeStartDocument("1.0");
        //this->xmlWriter.writeDTD(GiftiXmlElements.TAG_GIFTI,
        //                "http://www.nitrc.org/frs/download.php/115/gifti.dtd");
        
        //
        // Write GIFTI root element
        //
        XmlAttributes attributes;
        
        attributes.addAttribute("xmlns:xsi",
                                "http://www.w3.org/2001/XMLSchema-instance");
        attributes.addAttribute("xsi:noNamespaceSchemaLocation",
                                "http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd");
        attributes.addAttribute(GiftiXmlElements::ATTRIBUTE_GIFTI_VERSION,
                                versionString);
        attributes.addAttribute(GiftiXmlElements::ATTRIBUTE_GIFTI_NUMBER_OF_DATA_ARRAYS,
                                this->numberOfDataArrays);
        this->xmlWriter->writeStartElement(GiftiXmlElements::TAG_GIFTI,
                                         attributes);
        
        
        //
        // Write Metadata
        //
        GiftiMetaData tempMD;
        if (metadata != NULL) {
            tempMD = *metadata;
        }
        // update hierarchy metadata, write label table before the metadata if we have a hierarchy, since the table is shorter and easier to read
        bool labelTableWritten = false;
        if (labelTable != NULL)
        {
            auto myHier = labelTable->getHierarchy();
            if (myHier.isEmpty())
            {
                tempMD.remove("CaretHierarchy");
            } else {
                tempMD.set("CaretHierarchy", myHier.writeXMLToString());
                labelTable->writeAsXML(*this->xmlWriter);
                labelTableWritten = true;
            }
        }

        if (!tempMD.isEmpty())
        {
            tempMD.writeAsXML(*this->xmlWriter);
        }
        
        // if we didn't write the label table before the metadata, do so now
        if (labelTable != NULL && !labelTableWritten)
        {
            labelTable->writeAsXML(*this->xmlWriter);
        }
    }
    catch (const GiftiException& e) {
        this->closeFiles();
        throw e;
    }
}

/**
 * Write a GIFTI Data Array.
 *
 * @param gda - The data array.
 * @throws GiftiException - If an error occurs.
 */
void 
GiftiFileWriter::writeDataArray(GiftiDataArray* gda)
{
    this->verifyOpened();
    
    if (this->dataArraysWrittenCounter >= this->numberOfDataArrays) {
        this->closeFiles();
        throw GiftiException("PROGRAMMER ERROR: the number of data arrays "
                                 "written exceeds the number of data arrays in the file "
                                 "passed to start.");
    }
    try {
        //         //
        //         // Get the external file name.
        //         //
        //         String externalFileName = "";
        //         if (this.encoding == GiftiEncoding.EXTERNAL_FILE_BINARY) {
        //            externalFileName = this.getExternalFileNameForWriting();
        //         }
        //
        // Writing to an external binary data file?
        //
        if (this->encoding == GiftiEncodingEnum::EXTERNAL_FILE_BINARY) {
            if (this->externalFileOutputStream == NULL) {
                char* name = this->getExternalFileNameForWriting().toCharArray();
                this->externalFileOutputStream = new std::ofstream(name,std::fstream::binary);
                delete[] name;
                if (! *this->externalFileOutputStream) {
                    this->closeFiles();
                    const AString msg = ("Unable to open " + this->getExternalFileNameForWriting() + " for writing.");
                    throw GiftiException(msg);
                }
            }
            const int64_t fileOffset = this->externalFileOutputStream->tellp();
            FileInformation myInfo(this->getExternalFileNameForWriting());//TODO: get filename only without doing a stat?
            gda->setExternalFileInformation(myInfo.getFileName(),
                                            fileOffset);
        }
        
        //
        // Write data array
        //
        gda->writeAsXML(*this->xmlFileOutputStream, 
                        this->externalFileOutputStream,
                        this->encoding);
        
        //
        // Increment counter of data arrays written
        //
        this->dataArraysWrittenCounter++;
    }
    catch (const GiftiException& e) {
        this->closeFiles();
        throw e;
    }    
    catch (const XmlException& e) {
        this->closeFiles();
        throw GiftiException(e);
    }    
}

/**
 * Finish writing the file. Closes any open files.
 * @throws GiftiException If file error or number of data arrays written
 *   does not match number of data arrays passed to start() method.
 */
void 
GiftiFileWriter::finish()
{
    this->verifyOpened();
    
    try {
        //
        // Write GIFTI root end element
        //
        this->xmlWriter->writeEndElement();
        this->xmlWriter->writeEndDocument();
    }
    catch (const XmlException& e) {
        this->closeFiles();
        throw GiftiException(e);
    }
    
    //
    // Close the file
    //
    this->closeFiles();
}

/**
 * Get the maximum external file size.
 *
 * @return The size.
 */
long 
GiftiFileWriter::getMaximumExternalFileSize() const
{    
    return this->maximumExternalFileSize;
}

/**
 * Set the maximum external file size.  Starts a new external file when
 * this size is reached or exceeded.
 *
 * @param size Desired maximum size.
 */
void 
GiftiFileWriter::setMaximumExternalFileSize(const long size)
{
    this->maximumExternalFileSize = size;
}

/**
 * Close any open files.
 */
void 
GiftiFileWriter::closeFiles()
{
    if (this->xmlFileOutputStream != NULL) {
        if (this->xmlFileOutputStream->is_open())
        {
            this->xmlFileOutputStream->close();
        }
        delete this->xmlFileOutputStream;
        this->xmlFileOutputStream = NULL;
    }
    
    if (this->externalFileOutputStream != NULL) {
        if (this->externalFileOutputStream->is_open())
        {
            this->externalFileOutputStream->close();
        }
        delete this->externalFileOutputStream;
        this->externalFileOutputStream = NULL;
    }
}

/**
 * Verify the file is properly opened.
 * 
 * @throws GiftiException If file was not opened properly.
 */
void 
GiftiFileWriter::verifyOpened()
{
    if (this->xmlWriter == NULL) {
        throw GiftiException("Trying to write to file named \""
                                 + this->filename
                                 + "\" but the file was not properly opened "
                                 "by calling start() or opening the file "
                                 "failed.");
    }
}

/**
 * Remove any existing external data files.
 *
 * @throws GiftiException - If unable to delete a file.
 */
void 
GiftiFileWriter::removeExternalFiles()
{
    int maxFiles = 50;  // more than enough
    for (int counter = -1; counter <= maxFiles; counter++) {
        AString name = this->getExternalFileNamePrefix() + AString::number(counter);
        if (counter < 0) {
            name = this->getExternalFileNamePrefix();  // old version of ext file
        }
        
        FileInformation fileInfo(name);
        if (fileInfo.exists()) {
            if (fileInfo.remove() == false) {
                throw GiftiException("Unable to delete an existing external "
                                         " file named \""
                                         + name
                                         + "\".");
            }
        }
    }
    
}

/**
 * Get the name of the external file prefix.
 *
 * @return  The XML file name + ".data"
 */
AString 
GiftiFileWriter::getExternalFileNamePrefix() const
{
      return this->filename + ".data";
}

/**
 * Get the name of the external file that should be used for writing.
 * 
 * @return Name of external file.
 */
AString 
GiftiFileWriter::getExternalFileNameForWriting() const
{
    return this->getExternalFileNamePrefix();
    
    // COULD use multiple files for external data when large.
    //      boolean done = false;
    //      int counter = 1;
    //      String name = this.getExternalFileNamePrefix() + counter;
    //      while (done == false) {
    //         File file = new File(name);
    //         if (file.exists()) {
    //            if (file.length() > this.maximumExternalFileSize) {
    //               counter++;
    //               name = this.getExternalFileNamePrefix() + counter;
    //            }
    //            else {
    //               done = true;
    //            }
    //         }
    //         else {
    //            done = true;
    //         }
    //      }
    //
    //      return name;

}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GiftiFileWriter::toString() const
{
    return "GiftiFileWriter";
}
