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
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiBrainordinateScalarFile.h"
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
  m_surface(NULL)
{
}

/**
 * Constructor for map data files that are
 * associated with a specific surface by
 * the structure type.
 */
EventCaretMappableDataFilesGet::EventCaretMappableDataFilesGet(const Surface* surfaceIn)
: Event(EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET),
  m_surface(surfaceIn)
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
    
    CiftiMappableConnectivityMatrixDataFile* cmdf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(mapDataFile);
    if (cmdf != NULL) {
        m_ciftiConnectivityMatrixDataFiles.push_back(cmdf);
        return;
    }
    
    CiftiBrainordinateScalarFile* csf = dynamic_cast<CiftiBrainordinateScalarFile*>(mapDataFile);
    if (csf != NULL) {
        m_ciftiScalarFiles.push_back(csf);
        return;
    }
    
    CiftiBrainordinateLabelFile* clf = dynamic_cast<CiftiBrainordinateLabelFile*>(mapDataFile);
    if (clf != NULL) {
        m_ciftiLabelFiles.push_back(clf);
        return;
    }
    
    CiftiBrainordinateDataSeriesFile* cdsf = dynamic_cast<CiftiBrainordinateDataSeriesFile*>(mapDataFile);
    if (cdsf != NULL) {
        m_ciftiDataSeriesFiles.push_back(cdsf);
        return;
    }
    
    LabelFile* lf = dynamic_cast<LabelFile*>(mapDataFile);
    if (lf != NULL) {
        m_labelFiles.push_back(lf);
        return;
    }
    
    MetricFile* mf = dynamic_cast<MetricFile*>(mapDataFile);
    if (mf != NULL) {
        m_metricFiles.push_back(mf);
        return;
    }

    RgbaFile* rf = dynamic_cast<RgbaFile*>(mapDataFile);
    if (rf != NULL) {
        m_rgbaFiles.push_back(rf);
        return;
    }
    
    VolumeFile* vf = dynamic_cast<VolumeFile*>(mapDataFile);
    if (vf != NULL) {
        m_volumeFiles.push_back(vf);
        return;
    }
    
    CaretAssertMessage(0, 
                       "Unsupported map data file: " 
                       + AString(typeid(mapDataFile).name()) 
                       + "  New map data file added?");
}

/** 
 * Get all map data files.
 *
 * @param allFilesOut
 *    All map data files output.
 */
void 
EventCaretMappableDataFilesGet::getAllFiles(std::vector<CaretMappableDataFile*>& allFilesOut) const
{
    allFilesOut.clear();
    
    allFilesOut.insert(allFilesOut.end(),
                       m_ciftiConnectivityMatrixDataFiles.begin(),
                       m_ciftiConnectivityMatrixDataFiles.end());
    allFilesOut.insert(allFilesOut.end(),
                       m_ciftiLabelFiles.begin(),
                       m_ciftiLabelFiles.end());
    allFilesOut.insert(allFilesOut.end(),
                       m_ciftiScalarFiles.begin(),
                       m_ciftiScalarFiles.end());
    allFilesOut.insert(allFilesOut.end(),
                       m_ciftiDataSeriesFiles.begin(),
                       m_ciftiDataSeriesFiles.end());
    allFilesOut.insert(allFilesOut.end(),
                       m_labelFiles.begin(), 
                       m_labelFiles.end());
    allFilesOut.insert(allFilesOut.end(), 
                       m_metricFiles.begin(), 
                       m_metricFiles.end());
    allFilesOut.insert(allFilesOut.end(), 
                       m_rgbaFiles.begin(), 
                       m_rgbaFiles.end());
    allFilesOut.insert(allFilesOut.end(), 
                       m_volumeFiles.begin(), 
                       m_volumeFiles.end());
}


