
#ifndef __BRAIN_STRUCTURE_H__
#define __BRAIN_STRUCTURE_H__

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

#include <map>
#include <vector>

#include <stdint.h>

#include "CaretObject.h"
#include "DataFileException.h"
#include "EventListenerInterface.h"
#include "StructureEnum.h"

namespace caret {
    
    class Brain;
    class LabelFile;
    class MetricFile;
    class ModelDisplayControllerSurface;
    class RgbaFile;
    class Surface;
    
    /**
     * Maintains view of some type of object.
     */
    class BrainStructure : public CaretObject, public EventListenerInterface {
        
    public:
        BrainStructure(Brain* brain,
                       StructureEnum::Enum structure);
        
        ~BrainStructure();
        
        void receiveEvent(Event* event);
        
    private:
        BrainStructure(const BrainStructure& s);
        
        BrainStructure& operator=(const BrainStructure& s);

    public:
        void addLabelFile(LabelFile* labelFile) throw (DataFileException);
        
        void addMetricFile(MetricFile* metricFile) throw (DataFileException);
        
        void addRgbaFile(RgbaFile* rgbaFile) throw (DataFileException);
        
        void addSurface(Surface* surface) throw (DataFileException);
        
        void deleteSurface(Surface* surface);
        
        int getNumberOfSurfaces() const;
        
        Surface* getSurface(const int32_t indx);
        
        bool containsSurface(const Surface* surface);
        
        Brain* getBrain();
        
        int32_t getNumberOfNodes() const;

        StructureEnum::Enum getStructure() const;
        
        int32_t getNumberOfLabelFiles() const;
        
        LabelFile* getLabelFile(const int32_t fileIndex);
        
        const LabelFile* getLabelFile(const int32_t fileIndex) const;
        
        int32_t getNumberOfMetricFiles() const;
        
        MetricFile* getMetricFile(const int32_t fileIndex);
        
        const MetricFile* getMetricFile(const int32_t fileIndex) const;
        
        int32_t getNumberOfRgbaFiles() const;
        
        RgbaFile* getRgbaFile(const int32_t fileIndex);
        
        const RgbaFile* getRgbaFile(const int32_t fileIndex) const;
        
    private:
        Brain* brain;
        
        StructureEnum::Enum structure;
        
        std::vector<Surface*> surfaces;
        
        std::vector<LabelFile*> labelFiles;
        
        std::vector<MetricFile*> metricFiles;
        
        std::vector<RgbaFile*> rgbaFiles;
        
        /** Maps a surface to its model controller */
        std::map<Surface*, ModelDisplayControllerSurface*> surfaceControllerMap; 
    };
    
} // namespace

#endif // __BRAIN_STRUCTURE_H__
