
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
 * \class SpecFileDataFile
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
    this->filename  = filename;
    this->structure = structure;
    this->selected  = false;
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
    this->selected = true;
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
    
