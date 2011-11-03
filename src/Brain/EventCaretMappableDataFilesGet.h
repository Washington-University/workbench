#ifndef __EVENT_CARET_MAPPABLE_DATA_FILES_GET_H__
#define __EVENT_CARET_MAPPABLE_DATA_FILES_GET_H__

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

#include "Event.h"

namespace caret {

    class CaretMappableDataFile;
    class ConnectivityLoaderFile;
    class LabelFile;
    class MetricFile;
    class ModelDisplayController;
    class RgbaFile;
    class Surface;
    class VolumeFile;
    
    /// Event that gets all caret mappable data files.
    class EventCaretMappableDataFilesGet : public Event {
        
    public:
        EventCaretMappableDataFilesGet();
        
        EventCaretMappableDataFilesGet(const Surface* surface);
        
        virtual ~EventCaretMappableDataFilesGet();
        
        void addFile(CaretMappableDataFile* mapDataFile);        
        
        /**
         * @return Returns the surface for which associated map data
         * files are requested.  If NULL, then map data files
         * from all brain structures are requested. 
         */
        const Surface* getSurface() const { return this->surface; }
        
        /** @return Returns the connectivity loader files. */
        std::vector<ConnectivityLoaderFile*> getConnectivityLoaderFiles() const { return this->connectivityLoaderFiles; }
        
        /** @return Returns the label files. */
        std::vector<LabelFile*> getLabelFiles() const { return this->labelFiles; }
        
        /** @return Returns the metric files. */
        std::vector<MetricFile*> getMetricFiles() const { return this->metricFiles; }
        
        /** @return Returns the rgba files. */
        std::vector<RgbaFile*> getRgbaFiles() const { return this->rgbaFiles; }
        
        /** @return Returns the volume files. */
        std::vector<VolumeFile*> getVolumeFiles() const { return this->volumeFiles; }
        
        void getAllFiles(std::vector<CaretMappableDataFile*>& allFilesOut) const;
        
    private:
        EventCaretMappableDataFilesGet(const EventCaretMappableDataFilesGet&);
        
        EventCaretMappableDataFilesGet& operator=(const EventCaretMappableDataFilesGet&);
        
        const Surface* surface;
        
        std::vector<ConnectivityLoaderFile*> connectivityLoaderFiles;
        
        std::vector<LabelFile*> labelFiles;
        
        std::vector<MetricFile*> metricFiles;
        
        std::vector<RgbaFile*> rgbaFiles;
        
        std::vector<VolumeFile*> volumeFiles;
        
    };

} // namespace

#endif // __EVENT_CARET_MAPPABLE_DATA_FILES_GET_H__
