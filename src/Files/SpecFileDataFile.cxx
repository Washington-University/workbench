
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

#define __SPEC_FILE_GROUP_FILE_DECLARE__
#include "SpecFileDataFile.h"
#undef __SPEC_FILE_GROUP_FILE_DECLARE__

using namespace caret;

/**
 * \class caret::SpecFileDataFile
 * \brief  Name of file and its attributes including structure.
 *
 * Contains the name of a file and is attributes including
 * the file's structure.  Note that not all files use a 
 * structure.
 */

/**
 * Constructor.
 * @param filename
 *    Name of file.
 * @param structure
 *    Structure file represents (not all files have structure).
 */
SpecFileDataFile::SpecFileDataFile(const AString& filename,
                                   const StructureEnum::Enum structure)
: CaretObject()
{
    this->removeFromSpecFileWhenWritten = false;
    this->filename  = filename;
    this->structure = structure;
    this->originalStructure = this->structure;
    this->selected  = true;
}

/**
 * Copy constructor.
 * @param sfdf
 *    Object of this type that is copied.
 */
SpecFileDataFile::SpecFileDataFile(const SpecFileDataFile& sfdf)
: CaretObject(sfdf)
{
    this->copyHelper(sfdf);
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
        this->copyHelper(sfdf);
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
    this->removeFromSpecFileWhenWritten = false; // do not copy!
    this->filename  = sfdf.filename;
    this->structure = sfdf.structure;
    this->originalStructure = this->structure;
    this->selected  = sfdf.selected;    
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
    return this->filename;
}

/**
 * @return The file's structure.
 */
StructureEnum::Enum 
SpecFileDataFile::getStructure() const
{
    return this->structure;
}

/**
 * Set the structure.
 * @param structure
 *   New value for structure.
 */
void 
SpecFileDataFile::setStructure(const StructureEnum::Enum structure)
{
    this->structure = structure;
}

/**
 * @return The file's selection status.
 */
bool 
SpecFileDataFile::isSelected() const
{
    return this->selected;
}

/**
 * Set the file's selection status.
 * @param selected
 *    New selection status.
 */   
void 
SpecFileDataFile::setSelected(const bool selected)
{
    this->selected = selected;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SpecFileDataFile::toString() const
{
    const AString info =
    "name=" + this->filename
    + ", structure=" + StructureEnum::toGuiName(this->structure);
    //+ ", selected=" + AString::fromBool(this->selected);
    return info;
}
   
/**
 * @return Is this file to be removed when the SpecFile is
 * written?
 */
bool 
SpecFileDataFile::isRemovedFromSpecFileWhenWritten() const
{
    return this->removeFromSpecFileWhenWritten;
}

/**
 * Set remove file from SpecFile when SpecFile is written.
 * @param removeIt
 *    New status.
 */
void 
SpecFileDataFile::setRemovedFromSpecFileWhenWritten(const bool removeIt)
{
    this->removeFromSpecFileWhenWritten = removeIt;
}

/**
 * @return Has this item been edited (typically thru spec file dialog)?
 * True is returned if the structure has been changed
 * or the file is tagged for removal from the SpecFile,
 * otherwise false.
 */
bool 
SpecFileDataFile::hasBeenEdited() const
{
    if (this->removeFromSpecFileWhenWritten) {
        return true;
    }
    if (this->structure != this->originalStructure) {
        return true;
    }
    
    return false;
}



