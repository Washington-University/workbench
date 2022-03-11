
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

#define __SPEC_FILE_GROUP_FILE_DECLARE__
#include "SpecFileDataFile.h"
#undef __SPEC_FILE_GROUP_FILE_DECLARE__

#include "FileInformation.h"

using namespace caret;

/**
 * \class caret::SpecFileDataFile
 * \brief  Name of file and its attributes including structure.
 * \ingroup Files
 *
 * Contains the name of a file and is attributes including
 * the file's structure.  Note that not all files use a 
 * structure.
 */

/**
 * Constructor.
 *
 * @param filename
 *    Name of file.
 * @param dataFileType
 *    Type of data file.
 * @param structure
 *    Structure file represents (not all files have structure).
 * @param specFileMember
 *    True if this file is a member of the spec file and is thus
 *    written to the spec file.
 */
SpecFileDataFile::SpecFileDataFile(const AString& filename,
                                   const DataFileTypeEnum::Enum dataFileType,
                                   const StructureEnum::Enum structure,
                                   const bool specFileMember)
: CaretObjectTracksModification()
{
    m_caretDataFile = NULL;
    m_filename  = filename;
    m_dataFileType = dataFileType;
    m_structure = structure;
    m_loadingSelected  = true;
    m_savingSelected = false;
    m_specFileMember = specFileMember;
    m_removeWhenSavingToSceneFlag = false;
}

/**
 * Copy constructor.
 * @param sfdf
 *    Object of this type that is copied.
 */
SpecFileDataFile::SpecFileDataFile(const SpecFileDataFile& sfdf)
: CaretObjectTracksModification(sfdf)
{
    copyHelper(sfdf);
}

/**
 * Assignment operator.
 * @parm sfdf
 *    Object that is assigned to this object.
 * @return
 *    Reference to this object.
 */
SpecFileDataFile& 
SpecFileDataFile::operator=(const SpecFileDataFile& sfdf)
{
    if (this != &sfdf) {
        CaretObjectTracksModification::operator=(sfdf);
        copyHelper(sfdf);
    }
    
    return *this;
}

/**
 * Copy from the given object to this object.
 * @param sfdf
 *    Object from which data is copied.
 */
void 
SpecFileDataFile::copyHelper(const SpecFileDataFile& sfdf)
{
    m_caretDataFile = sfdf.m_caretDataFile;
    m_filename  = sfdf.m_filename;
    m_dataFileType = sfdf.m_dataFileType;
    m_structure = sfdf.m_structure;
    m_loadingSelected  = sfdf.m_loadingSelected;
    m_savingSelected  = sfdf.m_savingSelected;
    m_specFileMember = sfdf.m_specFileMember;
    m_removeWhenSavingToSceneFlag = sfdf.m_removeWhenSavingToSceneFlag;
}


/**
 * Destructor.
 */
SpecFileDataFile::~SpecFileDataFile()
{
    
}

/**
 * @return The file's name;
 */
AString 
SpecFileDataFile::getFileName() const
{
    if (m_caretDataFile != NULL) {
        m_filename = m_caretDataFile->getFileName();
    }
    return m_filename;
}

void
SpecFileDataFile::setFileName(const AString& fileName)
{
    m_filename = fileName;
    if (m_caretDataFile != NULL) {
        m_caretDataFile->setFileName(fileName);
    }
    setModified();
}

/**
 * @return The caret data file that is loaded for this spec file entry.
 * Will return NULL if the file has not been loaded.
 */
CaretDataFile*
SpecFileDataFile::getCaretDataFile()
{
    return m_caretDataFile;
}

/**
 * Set the caret data file for this spec file entry.
 * @param caretDataFile
 *    The caret data file.
 */
void
SpecFileDataFile::setCaretDataFile(CaretDataFile* caretDataFile)
{
    m_caretDataFile = caretDataFile;
}

/**
 * @return The file's structure.
 */
StructureEnum::Enum 
SpecFileDataFile::getStructure() const
{
    if (m_caretDataFile != NULL) {
        m_structure = m_caretDataFile->getStructure();
    }
    return m_structure;
}

/**
 * Set the structure.
 * @param structure
 *   New value for structure.
 */
void 
SpecFileDataFile::setStructure(const StructureEnum::Enum structure)
{
    if (m_structure != structure) {
        m_structure = structure;
        setModified();
    }
}

/**
 * @return The data file type for this group.
 */
DataFileTypeEnum::Enum
SpecFileDataFile::getDataFileType() const
{
    return m_dataFileType;
}

/**
 * @return The file's saving selection status.
 */
bool 
SpecFileDataFile::isSavingSelected() const
{
    return m_savingSelected;
}

/**
 * Set the file's saving selection status.
 * DOES NOT alter modification status.
 * @param selected
 *    New selection status.
 */   
void 
SpecFileDataFile::setSavingSelected(const bool selected)
{
    if (m_savingSelected != selected) {
        m_savingSelected = selected;
    }
}

/**
 * @return The file's loading selection status.
 */
bool
SpecFileDataFile::isLoadingSelected() const
{
    return m_loadingSelected;
}

/**
 * Set the file's loading selection status.
 * @param selected
 *    New selection status.
 */
void
SpecFileDataFile::setLoadingSelected(const bool selected)
{
    // note: does not cause modification status to change
    m_loadingSelected = selected;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SpecFileDataFile::toString() const
{
    const AString info =
    "name=" + m_filename
    + ", structure=" + StructureEnum::toGuiName(m_structure)
    + ", dataFileType=" + DataFileTypeEnum::toGuiName(m_dataFileType);
    //+ ", selected=" + AString::fromBool(m_selected);
    return info;
}
   
/**
 * @return True if this file is a selected as a member of the spec file.
 * If true, then the spec file is written, this file will be listed in 
 * the spec file.
 */
bool
SpecFileDataFile::isSpecFileMember() const
{
    return m_specFileMember;
}

/**
 * Set this file is a selected as a member of the spec file using the given
 * status.
 * 
 * @param status  If true, then the spec file is written, this file will
 * be listed in the spec file.
 */
void
SpecFileDataFile::setSpecFileMember(const bool status)
{
    if (m_specFileMember != status) {
        m_specFileMember = status;
        setModified();
    }
}

/**
 * @return If true, do not write this file to the scene when saving the scene
 */
bool
SpecFileDataFile::isRemoveWhenSavingToScene() const
{
    return m_removeWhenSavingToSceneFlag;
}

/**
 * Set do not write this file to scene when saving to scene
 * @param status
 *    New status
 */
void
SpecFileDataFile::setRemoveWhenSavingToScene(const bool status)
{
    m_removeWhenSavingToSceneFlag = status;
}

/**
 * @return True if the file "exists". 
 *
 * If the file is on the file system, true is returned if the file exists,
 * else false.
 *
 * If the file is remote (http, ftp, etc), true is ALWAYS returned.
 */
bool
SpecFileDataFile::exists() const
{
    if (DataFile::isFileOnNetwork(getFileName())) {
        return true;
    }
    
    FileInformation fileInfo(getFileName());
    if (fileInfo.exists()) {
        return true;
    }
    
    return false;
}



