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
#include "ElapsedTimer.h"
#include "GiftiFile.h"
#include "GiftiTypeFile.h"
#include "GiftiMetaDataNames.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * Constructor.
 */
GiftiTypeFile::GiftiTypeFile()
: DataFile()
{
    this->initializeMembersGiftiTypeFile();   
}

/**
 * Destructor.
 */
GiftiTypeFile::~GiftiTypeFile()
{
    if (this->giftiFile != NULL) {
        delete this->giftiFile;
        this->giftiFile = NULL;
    }
}

/**
 * Copy Constructor.
 *
 * @param gtf
 *    File that is copied.
 */
GiftiTypeFile::GiftiTypeFile(const GiftiTypeFile& gtf)
: DataFile(gtf)
{
    this->copyHelperGiftiTypeFile(gtf);
}

/**
 * Assignment operator.
 *
 * @param gtf
 *     File whose contents are copied to this file.
 */
GiftiTypeFile& 
GiftiTypeFile::operator=(const GiftiTypeFile& gtf)
{
    if (this != &gtf) {
        DataFile::operator=(gtf);
        this->copyHelperGiftiTypeFile(gtf);
    }
    return *this;
}

/**
 * Clear the contents of this file.
 */
void 
GiftiTypeFile::clear()
{
    DataFile::clear();
    this->giftiFile->clear(); 
}

/**
 * Clear modified status.
 */
void 
GiftiTypeFile::clearModified()
{
    DataFile::clearModified();
    this->giftiFile->clearModified();
}

/**
 * Is this file modified?
 *
 * @return true if modified, else false.
 */
bool 
GiftiTypeFile::isModified() const
{
    if (DataFile::isModified()) {
        return true;
    }
    if (this->giftiFile->isModified()) {
        return true;
    }
    return false;
}

/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
GiftiTypeFile::isEmpty() const
{
    return this->giftiFile->isEmpty();
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
GiftiTypeFile::readFile(const AString& filename) throw (DataFileException)
{
    ElapsedTimer et;
    et.start();
    this->giftiFile->readFile(filename);
    this->validateDataArraysAfterReading();
    this->clearModified();
    
    this->setFileName(filename);
    
    AString msg = ("Time to read " 
                   + filename 
                   + " was " 
                   + AString::number(et.getElapsedTimeSeconds())
                   + " seconds.");
    CaretLogInfo(msg);
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
GiftiTypeFile::writeFile(const AString& filename) throw (DataFileException)
{
    this->giftiFile->writeFile(filename);
    this->clearModified();
}
/**
 * Helps with file copying.
 * 
 * @param gtf
 *    File that is copied.
 */
void 
GiftiTypeFile::copyHelperGiftiTypeFile(const GiftiTypeFile& gtf)
{
    if (this->giftiFile != NULL) {
        delete this->giftiFile;
    }
    this->giftiFile = new GiftiFile(*gtf.giftiFile);
    this->validateDataArraysAfterReading();
}

/**
 * Initialize members of this class.
 */
void 
GiftiTypeFile::initializeMembersGiftiTypeFile()
{
    this->giftiFile = new GiftiFile();
}

/**
 * Get information about this file's contents.
 * @return
 *    Information about the file's contents.
 */
AString 
GiftiTypeFile::toString() const
{
    return this->giftiFile->toString();
}

StructureEnum::Enum 
GiftiTypeFile::getStructure() const
{
    AString structurePrimaryName;
    
    /*
     * Surface contains anatomical structure in pointset array.
     */
    const SurfaceFile* surfaceFile = dynamic_cast<const SurfaceFile*>(this);
    if (surfaceFile != NULL) {
        const GiftiDataArray* gda = this->giftiFile->getDataArrayWithIntent(NiftiIntentEnum::NIFTI_INTENT_POINTSET);
        const GiftiMetaData* metadata = gda->getMetaData();
        structurePrimaryName = metadata->get(GiftiMetaDataNames::ANATOMICAL_STRUCTURE_PRIMARY);
    }
    else {
        const GiftiMetaData* metadata = this->giftiFile->getMetaData();
        structurePrimaryName = metadata->get(GiftiMetaDataNames::ANATOMICAL_STRUCTURE_PRIMARY);
    }
    
    bool isValid = false;
    StructureEnum::Enum structure = StructureEnum::fromGuiName(structurePrimaryName, &isValid);
    return structure;
}
                                                                                
                                                                                


