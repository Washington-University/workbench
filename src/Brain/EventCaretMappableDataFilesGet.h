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
    class CiftiMappableConnectivityMatrixDataFile;
    class CiftiBrainordinateDataSeriesFile;
    class CiftiBrainordinateLabelFile;
    class CiftiBrainordinateScalarFile;
    class LabelFile;
    class MetricFile;
    class Model;
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
        const Surface* getSurface() const { return m_surface; }
        
        /** @return Returns the cifti connectivity matrix data files */
        std::vector<CiftiMappableConnectivityMatrixDataFile*> getCiftiConnectivityMatrixDataFiles() const { return m_ciftiConnectivityMatrixDataFiles; }
        
        /** @return the cifti data series files */
        std::vector<CiftiBrainordinateDataSeriesFile*> getCiftiBrainordinateDataSeriesFiles() const { return m_ciftiDataSeriesFiles; }
        
        /** @return the cifti scalar files */
        std::vector<CiftiBrainordinateScalarFile*> getCiftiBrainordinateScalarFiles() const { return m_ciftiScalarFiles; }
        
        /** @return the cifti label files */
        std::vector<CiftiBrainordinateLabelFile*> getCiftiBrainordinateLabelFiles() const { return m_ciftiLabelFiles; }
        
        /** @return Returns the label files. */
        std::vector<LabelFile*> getLabelFiles() const { return m_labelFiles; }
        
        /** @return Returns the metric files. */
        std::vector<MetricFile*> getMetricFiles() const { return m_metricFiles; }
        
        /** @return Returns the rgba files. */
        std::vector<RgbaFile*> getRgbaFiles() const { return m_rgbaFiles; }
        
        /** @return Returns the volume files. */
        std::vector<VolumeFile*> getVolumeFiles() const { return m_volumeFiles; }
        
        void getAllFiles(std::vector<CaretMappableDataFile*>& allFilesOut) const;
        
    private:
        EventCaretMappableDataFilesGet(const EventCaretMappableDataFilesGet&);
        
        EventCaretMappableDataFilesGet& operator=(const EventCaretMappableDataFilesGet&);
        
        const Surface* m_surface;
        
        std::vector<CiftiMappableConnectivityMatrixDataFile*> m_ciftiConnectivityMatrixDataFiles;
        
        std::vector<CiftiBrainordinateScalarFile*> m_ciftiScalarFiles;
            
        std::vector<CiftiBrainordinateLabelFile*> m_ciftiLabelFiles;
        
        std::vector<CiftiBrainordinateDataSeriesFile*> m_ciftiDataSeriesFiles;
        
        std::vector<LabelFile*> m_labelFiles;
        
        std::vector<MetricFile*> m_metricFiles;
        
        std::vector<RgbaFile*> m_rgbaFiles;
        
        std::vector<VolumeFile*> m_volumeFiles;
        
    };

} // namespace

#endif // __EVENT_CARET_MAPPABLE_DATA_FILES_GET_H__
