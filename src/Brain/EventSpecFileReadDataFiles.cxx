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
#include "EventSpecFileReadDataFiles.h"

using namespace caret;

/**
 * Constructor.
 * 
 * @param loadIntoBrain
 *    Brain into which file is loaded.
 * @param specFile
 *    Spec file that has its files read.
 */
EventSpecFileReadDataFiles::EventSpecFileReadDataFiles(Brain* loadIntoBrain,
                                     SpecFile* specFile)
: Event(EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)
{
    this->loadIntoBrain = loadIntoBrain;
    this->specFile  = specFile;
    
    CaretAssert(this->loadIntoBrain);
    CaretAssert(this->specFile);
}

/**
 * Destructor.
 */
EventSpecFileReadDataFiles::~EventSpecFileReadDataFiles()
{
    
}

/**
 * @return The spec file that is to have its data files loaded.
 */
SpecFile* 
EventSpecFileReadDataFiles::getSpecFile()
{
    return this->specFile;
}

/**
 * @return The brain into which files is loaded.
 */
Brain* 
EventSpecFileReadDataFiles::getLoadIntoBrain()
{
    return this->loadIntoBrain;
}
                                     

