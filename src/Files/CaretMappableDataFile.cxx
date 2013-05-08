
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

#define __CARET_MAPPABLE_DATA_FILE_DECLARE__
#include "CaretMappableDataFile.h"
#undef __CARET_MAPPABLE_DATA_FILE_DECLARE__

using namespace caret;


    
/**
 * Constructor.
 */
CaretMappableDataFile::CaretMappableDataFile(const DataFileTypeEnum::Enum dataFileType)
: CaretDataFile(dataFileType)
{
    
}

/**
 * Destructor.
 */
CaretMappableDataFile::~CaretMappableDataFile()
{
    
}

/**
 * Constructor.
 * @param cmdf
 *   Instance that is copied.
 */
CaretMappableDataFile::CaretMappableDataFile(const CaretMappableDataFile& cmdf)
: CaretDataFile(cmdf)
{
    this->copyCaretMappableDataFile(cmdf);
}

/**
 * Constructor.
 * @param cmdf
 *   Instance that is assigned to this.
 * @return
 *   Reference to this instance.
 */
CaretMappableDataFile& 
CaretMappableDataFile::operator=(const CaretMappableDataFile& cmdf)
{
    if (this != &cmdf) {
        CaretDataFile::operator=(cmdf);
        this->copyCaretMappableDataFile(cmdf);
    }
    return *this;
}

/**
 * Assists with copying instances of this class.
 */
void 
CaretMappableDataFile::copyCaretMappableDataFile(const CaretMappableDataFile&)
{
    
}

// note: method is documented in header file
bool
CaretMappableDataFile::hasMapAttributes() const
{
    return true;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromNameOrNumber(const AString& mapName)
{
    bool ok = false;
    int32_t ret = mapName.toInt(&ok) - 1;//compensate for 1-indexing that command line parsing uses
    if (ok)
    {
        if (ret < 0 || ret >= getNumberOfMaps())
        {
            ret = -1;
        }
    } else {//DO NOT search by name if the string was parsed as an integer correctly, or some idiot who names their maps as integers will get confused
            //when getting map "12" out of a file after the file expands to more than 12 elements suddenly does something different
        ret = getMapIndexFromName(mapName);
    }
    return ret;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromName(const AString& mapName)
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (mapName == getMapName(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromUniqueID(const AString& uniqueID) const
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (uniqueID == getMapUniqueID(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
void
CaretMappableDataFile::updateScalarColoringForAllMaps(const PaletteFile* paletteFile)
{
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
        updateScalarColoringForMap(iMap,
                             paletteFile);
    }
}

