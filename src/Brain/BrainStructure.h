
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
#include "SurfaceTypeEnum.h"

namespace caret {
    
    class Brain;
    class BrainStructureNodeAttributes;
    class CaretDataFile;
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
        
        void getSurfacesOfType(const SurfaceTypeEnum::Enum surfaceType,
                               std::vector<Surface*>& surfacesOut) const;
        
        bool containsSurface(const Surface* surface);
        
        Brain* getBrain();
        
        const Brain* getBrain() const;
        
        int32_t getNumberOfNodes() const;

        StructureEnum::Enum getStructure() const;
        
        int32_t getNumberOfLabelFiles() const;
        
        LabelFile* getLabelFile(const int32_t fileIndex);
        
        const LabelFile* getLabelFile(const int32_t fileIndex) const;
        
        void getLabelFiles(std::vector<LabelFile*>& labelFilesOut) const;
        
        int32_t getNumberOfMetricFiles() const;
        
        MetricFile* getMetricFile(const int32_t fileIndex);
        
        const MetricFile* getMetricFile(const int32_t fileIndex) const;
        
        void getMetricFiles(std::vector<MetricFile*>& metricFilesOut) const;
        
        int32_t getNumberOfRgbaFiles() const;
        
        RgbaFile* getRgbaFile(const int32_t fileIndex);
        
        const RgbaFile* getRgbaFile(const int32_t fileIndex) const;
        
        void getRgbaFiles(std::vector<RgbaFile*>& labelFilesOut) const;
        
        int64_t getBrainStructureIdentifier() const;
        
        BrainStructureNodeAttributes* getNodeAttributes(const int32_t nodeIndex);
        
        const BrainStructureNodeAttributes* getNodeAttributes(const int32_t nodeIndex) const;
        
        void getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut);
        
        bool removeDataFile(CaretDataFile* caretDataFile);
        
    private:
        Brain* brain;
        
        StructureEnum::Enum structure;
        
        std::vector<Surface*> surfaces;
        
        std::vector<LabelFile*> labelFiles;
        
        std::vector<MetricFile*> metricFiles;
        
        std::vector<RgbaFile*> rgbaFiles;
        
        /** Maps a surface to its model controller */
        std::map<Surface*, ModelDisplayControllerSurface*> surfaceControllerMap; 
        
        /** Unique number assigned to each brain structure. */
        int64_t brainStructureIdentifier;
        
        /** Generates unique number assigned to each brain structure */
        static int64_t brainStructureIdentifierCounter;
        
        std::vector<BrainStructureNodeAttributes*> nodeAttributes;
    };
    
#ifdef __BRAIN_STRUCTURE_DEFINE__
    int64_t BrainStructure::brainStructureIdentifierCounter = 1;
#endif // __BRAIN_STRUCTURE_DEFINE__

} // namespace


#endif // __BRAIN_STRUCTURE_H__
