
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __NEUROGLANCER_ANNOTATION_FILE_DECLARE__
#include "NeuroglancerAnnotationFile.h"
#undef __NEUROGLANCER_ANNOTATION_FILE_DECLARE__

#include "CaretAssert.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "GiftiMetaData.h"

using namespace caret;
    
/**
 * \class caret::NeuroglancerAnnotationFile 
 * \brief Neuroglancer Annotations File
 * \ingroup Files
 */

/**
 * Constructor.
 */
NeuroglancerAnnotationFile::NeuroglancerAnnotationFile()
: CaretDataFile(DataFileTypeEnum::NEUROGLANCER_ANNOTATION)
{
    m_fileMetaData.reset(new GiftiMetaData());
}

/**
 * Destructor.
 */
NeuroglancerAnnotationFile::~NeuroglancerAnnotationFile()
{
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
NeuroglancerAnnotationFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
}

/**
 * @return Is this foci file empty (contains zero focuss)?
 */
bool
NeuroglancerAnnotationFile::isEmpty() const
{
    return true;
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
NeuroglancerAnnotationFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
NeuroglancerAnnotationFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* does nothing since focuss apply to all structures */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
NeuroglancerAnnotationFile::getFileMetaData()
{
    return m_fileMetaData.get();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
NeuroglancerAnnotationFile::getFileMetaData() const
{
    return m_fileMetaData.get();
}

/**
 * Clear the focus file.
 */
void
NeuroglancerAnnotationFile::clear()
{
    CaretDataFile::clear();
    m_fileMetaData->clear();
}

/**
 * @return Is this foci file modified?
 */
bool
NeuroglancerAnnotationFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    if (m_fileMetaData->isModified()) {
        return true;
    }
    
//    /*
//     * Note, these members do not affect modification status:
//     * classNameHierarchy
//     */
//    
//    const int32_t numFoci = getNumberOfFoci();
//    for (int32_t i = 0; i < numFoci; i++) {
//        if (m_foci[i]->isModified()) {
//            return true;
//        }
//    }
    
    return false;
}

/**
 * Clear the modification status of this foci file.
 */
void
NeuroglancerAnnotationFile::clearModified()
{
    CaretDataFile::clearModified();
    
    m_fileMetaData->clearModified();
    
//    const int32_t numFoci = getNumberOfFoci();
//    for (int32_t i = 0; i < numFoci; i++) {
//        m_foci[i]->clearModified();
//    }
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
NeuroglancerAnnotationFile::readFile(const AString& filename)
{
    clear();
    
    checkFileReadability(filename);
    
    setFileName(filename);
    
    clearModified();
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
NeuroglancerAnnotationFile::writeFile(const AString& filename)
{
    throw DataFileException("Writing of NeuroglancerAnnotationFile is not supported for "
                            + filename);
}
