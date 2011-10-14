
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

#define __CARET_DATA_FILE_DECLARE__
#include "CaretDataFile.h"
#undef __CARET_DATA_FILE_DECLARE__

using namespace caret;


    
/**
 * \class CaretDataFile 
 * \brief A data file with abstract methods for caret data
 *
 * This class is essentially an interface that defines methods
 * that are supported by most Caret Data Files.
 */
/**
 * Constructor.
 */
CaretDataFile::CaretDataFile(const DataFileTypeEnum::Enum dataFileType)
: DataFile()
{
    this->dataFileType = dataFileType;
}

/**
 * Destructor.
 */
CaretDataFile::~CaretDataFile()
{
    
}

/**
 * @return The type of this data file.
 */
DataFileTypeEnum::Enum 
CaretDataFile::getDataFileType() const
{
    return this->dataFileType; 
}

/**
 * Copy constructor.
 * @param cdf
 *    Instance that is copied to this.
 */
CaretDataFile::CaretDataFile(const CaretDataFile& cdf)
{
    this->copyDataCaretDataFile(cdf);
}

/**
 * Assignment operator.
 * @param cdf
 *    Instance that is assigned to this.
 */
CaretDataFile& 
CaretDataFile::operator=(const CaretDataFile& cdf)
{
    if (this != &cdf) {
        this->copyDataCaretDataFile(cdf);
    }
    return *this;
}

/**
 * Assists with copying instances of this class.
 * @param cdf
 *    Instance that is copied to this.
 */
void 
CaretDataFile::copyDataCaretDataFile(const CaretDataFile& cdf)
{
    this->dataFileType = cdf.dataFileType;
}

