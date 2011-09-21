#ifndef __EVENT_GET_NODE_DATA_FILES_H__
#define __EVENT_GET_NODE_DATA_FILES_H__

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

    class GiftiTypeFile;
    class LabelFile;
    class MetricFile;
    class ModelDisplayController;
    class RgbaFile;
    class Surface;
    
    /// Event that gets all node data files.
    class EventGetNodeDataFiles : public Event {
        
    public:
        EventGetNodeDataFiles();
        
        EventGetNodeDataFiles(const Surface* surface);
        
        virtual ~EventGetNodeDataFiles();
        
        void addFile(GiftiTypeFile* nodeDataFile);        
        
        /**
         * @return Returns the surface for which associated data
         * files are requested.  If NULL, then all node data files
         * are requested. 
         */
        const Surface* getSurface() const { return this->surface; }
        
        /** @return Returns the label files. */
        std::vector<LabelFile*> getLabelFiles() const { return this->labelFiles; }
        
        /** @return Returns the metric files. */
        std::vector<MetricFile*> getMetricFiles() const { return this->metricFiles; }
        
        /** @return Returns the rgba files. */
        std::vector<RgbaFile*> getRgbaFiles() const { return this->rgbaFiles; }
        
        void getAllFiles(std::vector<GiftiTypeFile*>& allFilesOut) const;
        
    private:
        EventGetNodeDataFiles(const EventGetNodeDataFiles&);
        
        EventGetNodeDataFiles& operator=(const EventGetNodeDataFiles&);
        
        const Surface* surface;
        
        std::vector<LabelFile*> labelFiles;
        
        std::vector<MetricFile*> metricFiles;
        
        std::vector<RgbaFile*> rgbaFiles;
        
    };

} // namespace

#endif // __EVENT_GET_NODE_DATA_FILES_H__
