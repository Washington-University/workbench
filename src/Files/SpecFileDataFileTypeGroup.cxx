
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

#define __SPEC_FILE_GROUP_DECLARE__
#include "SpecFileDataFileTypeGroup.h"
#undef __SPEC_FILE_GROUP_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "SpecFileDataFile.h"

using namespace caret;

/**
 * \class caret::SpecFileDataFileTypeGroup
 * \brief Groups files of the same DataFileType.
 *
 * Groups files of the same data type in a SpecFile.
 */

/**
 * Constructor.
 * @param dataFileType
 *     Type of data file.
 */
SpecFileDataFileTypeGroup::SpecFileDataFileTypeGroup(const DataFileTypeEnum::Enum dataFileType)
: CaretObjectTracksModification()
{
    this->dataFileType = dataFileType;
}

/**
 * Destructor.
 */
SpecFileDataFileTypeGroup::~SpecFileDataFileTypeGroup()
{
    this->removeAllFileInformation();
}

/**
 * @return The data file type for this group.
 */
DataFileTypeEnum::Enum 
SpecFileDataFileTypeGroup::getDataFileType() const
{
    return this->dataFileType;
}

/**
 * @return The number of files.
 */
int32_t 
SpecFileDataFileTypeGroup::getNumberOfFiles() const
{
    return this->files.size();
}

/**
 * @return The number of files selected for loading.
 */
int32_t 
SpecFileDataFileTypeGroup::getNumberOfFilesSelectedForLoading() const
{
    int count = 0;
    
    for (std::vector<SpecFileDataFile*>::const_iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        SpecFileDataFile* file = *iter;
        if (file->isLoadingSelected()) {
            count++;
        }
    }
    return count;
}

/**
 * @return The number of files selected for saving.
 */
int32_t
SpecFileDataFileTypeGroup::getNumberOfFilesSelectedForSaving() const
{
    int count = 0;
    
    for (std::vector<SpecFileDataFile*>::const_iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        SpecFileDataFile* file = *iter;
        if (file->isSavingSelected()) {
            count++;
        }
    }
    return count;
}

/**
 * Add a file.
 * @param fileInformation
 *    New file information.
 */
void 
SpecFileDataFileTypeGroup::addFileInformation(SpecFileDataFile* fileInformation)
{
    CaretAssert(fileInformation);
    this->files.push_back(fileInformation);
    
    /*
     * Do not set the modfication status when a data file is added
     */
}

/**
 * Remove file at given index.
 * @param fileIndex
 *    Index of file for removal.
 */
void 
SpecFileDataFileTypeGroup::removeFileInformation(const int32_t fileIndex)
{
    CaretAssertVectorIndex(this->files, 
                           fileIndex);
    delete this->files[fileIndex];
    this->files.erase(this->files.begin() + fileIndex);
    setModified();
}

/**
 * Remove any files that are not "in spec" and do not have an
 * associated caret data file.
 */
void
SpecFileDataFileTypeGroup::removeFileInformationIfNotInSpecAndNoCaretDataFile()
{
    const int32_t numFiles = getNumberOfFiles();
    for (int32_t i = (numFiles - 1); i >= 0; i--) {
        SpecFileDataFile* sfdf = getFileInformation(i);
        if (sfdf->isSpecFileMember() == false) {
            if (sfdf->getCaretDataFile() == NULL) {
                removeFileInformation(i);
            }
        }
    }
}

/**
 * Remove all files.
 */
void 
SpecFileDataFileTypeGroup::removeAllFileInformation()
{
    for (std::vector<SpecFileDataFile*>::iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        delete *iter;
    }
    this->files.clear();
    setModified();
}

/**
 * Get information for a file.
 * @param fileIndex
 *    Index of file for which information is requested.
 * @return
 *    Information for file.
 */
SpecFileDataFile* 
SpecFileDataFileTypeGroup::getFileInformation(const int32_t fileIndex)
{
    CaretAssertVectorIndex(this->files, 
                           fileIndex);
    return this->files[fileIndex];
}

/**
 * Get information for a file.
 * @param fileIndex
 *    Index of file for which information is requested.
 * @return
 *    Information for file.
 */
const SpecFileDataFile* 
SpecFileDataFileTypeGroup::getFileInformation(const int32_t fileIndex) const
{
    CaretAssertVectorIndex(this->files, 
                           fileIndex);
    return this->files[fileIndex];
}

/**
 * Set the loading selection status of all files.
 * @param selectionStatus
 *   New loading selection status for all files.
 */
void 
SpecFileDataFileTypeGroup::setAllFilesSelectedForLoading(bool selectionStatus)
{
    for (std::vector<SpecFileDataFile*>::iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        SpecFileDataFile* file = *iter;
        file->setLoadingSelected(selectionStatus);
    }
}

/**
 * Set the saving selection status of all files.
 * @param selectionStatus
 *   New saving selection status for all files.
 */
void
SpecFileDataFileTypeGroup::setAllFilesSelectedForSaving(bool selectionStatus)
{
    for (std::vector<SpecFileDataFile*>::iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        SpecFileDataFile* file = *iter;
        file->setSavingSelected(selectionStatus);
    }
}

/**
 * Set the save status to on for any files that are modified.
 */
void
SpecFileDataFileTypeGroup::setModifiedFilesSelectedForSaving()
{
    for (std::vector<SpecFileDataFile*>::iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        SpecFileDataFile* file = *iter;
        CaretDataFile* cdf = file->getCaretDataFile();
        if (cdf != NULL) {
            if (cdf->isModified()) {
                CaretMappableDataFile* mapFile = dynamic_cast<CaretMappableDataFile*>(cdf);
                if (mapFile != NULL) {
                    /*
                     * For mappable data file, we only default saving to on when the modification
                     * does not involve the color palette.  Color palettes can be saved to a
                     * scene so it not necessary to save the file when the palette is modified.
                     * This prevents unintentional saving of the mappable data files,
                     * especially when creating scenes.
                     */
                    if (mapFile->isModifiedExcludingPaletteColorMapping()) {
                        file->setSavingSelected(true);
                    }
                }
                else {
                    file->setSavingSelected(true);
                }
            }
        }
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SpecFileDataFileTypeGroup::toString() const
{
    AString info = "DataFileType=" + DataFileTypeEnum::toName(this->dataFileType) + "\n";
    for (std::vector<SpecFileDataFile*>::const_iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        SpecFileDataFile* file = *iter;
        info += ("    " + file->toString() + "\n");

    }
    
    return info;
}    

/**
 * Set the status to unmodified.
 */
void
SpecFileDataFileTypeGroup::clearModified()
{
    CaretObjectTracksModification::clearModified();
    
    for (std::vector<SpecFileDataFile*>::iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        SpecFileDataFile* sfdf = *iter;
        sfdf->clearModified();
    }
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool
SpecFileDataFileTypeGroup::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    
    for (std::vector<SpecFileDataFile*>::const_iterator iter = this->files.begin();
         iter != this->files.end();
         iter++) {
        const SpecFileDataFile* sfdf = *iter;
        if (sfdf->isModified()) {
            return true;
        }
    }
    
    return false;
}


