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
#include "EventNodeDataFilesGet.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "RgbaFile.h"

using namespace caret;

/**
 * Constructor for node data files that are 
 * associated with ANY surface.
 */
EventNodeDataFilesGet::EventNodeDataFilesGet()
: Event(EventTypeEnum::EVENT_GET_NODE_DATA_FILES),
  surface(NULL)
{
}

/**
 * Constructor for node data files that are
 * associated with a specific surface.
 */
EventNodeDataFilesGet::EventNodeDataFilesGet(const Surface* surfaceIn)
: Event(EventTypeEnum::EVENT_GET_NODE_DATA_FILES),
  surface(surfaceIn)
{
}

/**
 * Destructor.
 */
EventNodeDataFilesGet::~EventNodeDataFilesGet()
{
    
}

/**
 * Add a node data file.
 * @param nodeDataFile
 *    Data file that is added.
 */
void 
EventNodeDataFilesGet::addFile(GiftiTypeFile* nodeDataFile)
{
    CaretAssert(nodeDataFile);
    if (nodeDataFile->getNumberOfColumns() <= 0) {
        return;
    }
    
    LabelFile* lf = dynamic_cast<LabelFile*>(nodeDataFile);
    if (lf != NULL) {
        this->labelFiles.push_back(lf);
        return;
    }
    
    MetricFile* mf = dynamic_cast<MetricFile*>(nodeDataFile);
    if (mf != NULL) {
        this->metricFiles.push_back(mf);
        return;
    }

    RgbaFile* rf = dynamic_cast<RgbaFile*>(nodeDataFile);
    if (rf != NULL) {
        this->rgbaFiles.push_back(rf);
        return;
    }
    
    CaretAssertMessage(0, 
                       "Unsupported node data file: " 
                       + AString(typeid(nodeDataFile).name()) 
                       + "  New node data file added?");
}

/** 
 * @return Returns all data files.
 */
void 
EventNodeDataFilesGet::getAllFiles(std::vector<GiftiTypeFile*>& allFilesOut) const
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
}


