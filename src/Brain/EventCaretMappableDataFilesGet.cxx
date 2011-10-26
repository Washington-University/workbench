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

#include <typeinfo>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "RgbaFile.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * Constructor for ALL map data files.
 */
EventCaretMappableDataFilesGet::EventCaretMappableDataFilesGet()
: Event(EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET),
  surface(NULL)
{
}

/**
 * Constructor for map data files that are
 * associated with a specific surface by
 * the structure type.
 */
EventCaretMappableDataFilesGet::EventCaretMappableDataFilesGet(const Surface* surfaceIn)
: Event(EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET),
  surface(surfaceIn)
{
}

/**
 * Destructor.
 */
EventCaretMappableDataFilesGet::~EventCaretMappableDataFilesGet()
{
    
}

/**
 * Add a map data file.
 * @param mapDataFile
 *    Map data file that is added.
 */
void 
EventCaretMappableDataFilesGet::addFile(CaretMappableDataFile* mapDataFile)
{
    CaretAssert(mapDataFile);
    if (mapDataFile->getNumberOfMaps() <= 0) {
        return;
    }
    
    LabelFile* lf = dynamic_cast<LabelFile*>(mapDataFile);
    if (lf != NULL) {
        this->labelFiles.push_back(lf);
        return;
    }
    
    MetricFile* mf = dynamic_cast<MetricFile*>(mapDataFile);
    if (mf != NULL) {
        this->metricFiles.push_back(mf);
        return;
    }

    RgbaFile* rf = dynamic_cast<RgbaFile*>(mapDataFile);
    if (rf != NULL) {
        this->rgbaFiles.push_back(rf);
        return;
    }
    
    VolumeFile* vf = dynamic_cast<VolumeFile*>(mapDataFile);
    if (vf != NULL) {
        this->volumeFiles.push_back(vf);
        return;
    }
    
    CaretAssertMessage(0, 
                       "Unsupported map data file: " 
                       + AString(typeid(mapDataFile).name()) 
                       + "  New map data file added?");
}

/** 
 * @return Returns all map data files.
 */
void 
EventCaretMappableDataFilesGet::getAllFiles(std::vector<CaretMappableDataFile*>& allFilesOut) const
{
    allFilesOut.clear();
    
    allFilesOut.insert(allFilesOut.end(), 
                       this->labelFiles.begin(), 
                       this->labelFiles.end());
    allFilesOut.insert(allFilesOut.end(), 
                       this->metricFiles.begin(), 
                       this->metricFiles.end());
    allFilesOut.insert(allFilesOut.end(), 
                       this->rgbaFiles.begin(), 
                       this->rgbaFiles.end());
//    allFilesOut.insert(allFilesOut.end(), 
//                       this->volumeFiles.begin(), 
//                       this->volumeFiles.end());
}


