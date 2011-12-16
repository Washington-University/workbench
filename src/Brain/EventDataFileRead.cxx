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

#include "CaretAssert.h"
#include "EventDataFileRead.h"

using namespace caret;

/**
 * Constructor.
 * 
 * @param loadIntoBrain
 *    Brain into which file is loaded.
 * @param dataFileType
 *    Type of file this for loading.
 * @param dataFileName
 *    Name of data file to load.
 */
EventDataFileRead::EventDataFileRead(Brain* loadIntoBrain,
                                     const DataFileTypeEnum::Enum dataFileType,
                                     const AString& dataFileName)
: Event(EventTypeEnum::EVENT_DATA_FILE_READ)
{
    this->loadIntoBrain = loadIntoBrain;
    this->structure     = StructureEnum::INVALID;
    this->dataFileType  = dataFileType;
    this->dataFileName  = dataFileName;
    
    CaretAssert(this->loadIntoBrain);
    CaretAssert(this->dataFileType != DataFileTypeEnum::UNKNOWN);
    CaretAssert(this->dataFileName.isEmpty() == false);
}

/**
 * Constructor for loading a file with specified structure.
 * 
 * @param loadIntoBrain
 *    Brain into which file is loaded.
 * @param structure
 *    Structure for file.
 * @param dataFileType
 *    Type of file this for loading.
 * @param dataFileName
 *    Name of data file to load.
 */
EventDataFileRead::EventDataFileRead(Brain* loadIntoBrain,
                                     const StructureEnum::Enum structure,
                                     const DataFileTypeEnum::Enum dataFileType,
                                     const AString& dataFileName)
: Event(EventTypeEnum::EVENT_DATA_FILE_READ)
{
    this->loadIntoBrain = loadIntoBrain;
    this->structure     = structure;
    this->dataFileType  = dataFileType;
    this->dataFileName  = dataFileName;
    
    CaretAssert(this->loadIntoBrain);
    CaretAssert(this->dataFileType != DataFileTypeEnum::UNKNOWN);
    CaretAssert(this->dataFileName.isEmpty() == false);
}

/**
 * Destructor.
 */
EventDataFileRead::~EventDataFileRead()
{
    
}

/**
 * Get the name of the data file that is to be loaded.
 *
 * @return Name of data file to load.
 */
AString 
EventDataFileRead::getDataFileName() const
{
    return this->dataFileName;
}

/**
 * Get the type of data file for loading.
 
 * @return Type of file for loading.
 */
DataFileTypeEnum::Enum 
EventDataFileRead::getDataFileType() const
{
    return this->dataFileType;
}

/**
 * Get brain into which file is loaded.
 
 * @return Brain into which file is loaded.
 */
Brain* 
EventDataFileRead::getLoadIntoBrain()
{
    return this->loadIntoBrain;
}
                           
/**
 * @return The structure associated with the data file.
 */
StructureEnum::Enum 
EventDataFileRead::getStructure() const
{
    return this->structure;
}


