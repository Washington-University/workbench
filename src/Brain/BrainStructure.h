
#ifndef __BRAIN_STRUCTURE_H__
#define __BRAIN_STRUCTURE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <map>
#include <vector>

#include <stdint.h>

#include "BrainConstants.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"
#include "SurfaceTypeEnum.h"

namespace caret {
    
    class Brain;
    class BrainStructureNodeAttributes;
    class CaretDataFile;
    class LabelFile;
    class MetricFile;
    class ModelSurface;
    class OverlaySet;
    class OverlaySetArray;
    class RgbaFile;
    class Surface;
    
    /**
     * Maintains view of some type of object.
     */
    class BrainStructure : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        BrainStructure(Brain* brain,
                       StructureEnum::Enum structure);
        
        ~BrainStructure();
        
        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        BrainStructure(const BrainStructure& s);
        
        BrainStructure& operator=(const BrainStructure& s);

    public:
        void addLabelFile(LabelFile* labelFile,
                          const bool addFileToBrainStructure);
        
        void addMetricFile(MetricFile* metricFile,
                           const bool addFileToBrainStructure);
        
        void addRgbaFile(RgbaFile* rgbaFile,
                         const bool addFileToBrainStructure);
        
        void addSurface(Surface* surface,
                        const bool addFileToBrainStructure,
                        const bool initilizeOverlaysFlag);
        
        int getNumberOfSurfaces() const;
        
        Surface* getSurface(const int32_t indx);
        
        const Surface* getSurface(const int32_t indx) const;
        
        void getSurfacesOfType(const SurfaceTypeEnum::Enum surfaceType,
                               std::vector<Surface*>& surfacesOut) const;
        
        Surface* getSurfaceOfTypeAndSecondaryType(const SurfaceTypeEnum::Enum surfaceType,
                                                  const SecondarySurfaceTypeEnum::Enum secondarySurfaceType) const;

        bool containsSurface(const Surface* surface);
        
        const Surface* getPrimaryAnatomicalSurface() const;
        
        Surface* getPrimaryAnatomicalSurface();
        
        const Surface* getSurfaceContainingTextInName(const AString& text) const;
        
        Surface* getSurfaceContainingTextInName(const AString& text);
        
        Surface* getSurfaceWithName(const AString& surfaceFileName,
                                    const bool useAbsolutePath);
        
        void getSurfaces(std::vector<Surface*>& surfacesOut) const;
        
        void setPrimaryAnatomicalSurface(Surface* surface);
        
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
        
        BrainStructureNodeAttributes* getNodeAttributes();
        
        const BrainStructureNodeAttributes* getNodeAttributes() const;
        
        bool hasDataFiles() const;
        
        void getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut) const;
        
        bool removeWithoutDeleteDataFile(const CaretDataFile* caretDataFile);
        
        //bool removeAndDeleteDataFile(CaretDataFile* caretDataFile);
                
        bool getMetricShapeMap(MetricFile* &metricFileOut,
                               int32_t& shapeMapIndexOut) const;
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
        
        void matchSurfacesToPrimaryAnatomical(const bool matchStatus);
        
    private:
        const Surface* getPrimaryAnatomicalSurfacePrivate() const;
        
        const Surface* getSurfaceContainingTextInNamePrivate(const AString& text) const;

        bool removeAndMaybeDeleteSurface(Surface* surface,
                           const bool deleteSurfaceFile);
        
        Brain* m_brain;
        
        StructureEnum::Enum m_structure;
        
        /** Overlays sets for this model and for each tab */
        //OverlaySet* m_overlaySet[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        OverlaySetArray* m_overlaySetArray;
        
        std::vector<Surface*> m_surfaces;
        
        std::vector<LabelFile*> m_labelFiles;
        
        std::vector<MetricFile*> m_metricFiles;
        
        std::vector<RgbaFile*> m_rgbaFiles;
        
        /** Maps a surface to its model */
        std::map<Surface*, ModelSurface*> m_surfaceModelMap; 
        
        /** Unique number assigned to each brain structure. */
        int64_t m_brainStructureIdentifier;
        
        /** Generates unique number assigned to each brain structure */
        static int64_t s_brainStructureIdentifierCounter;
        
        BrainStructureNodeAttributes* m_nodeAttributes;
        
        mutable Surface* m_primaryAnatomicalSurface;
    };
    
#ifdef __BRAIN_STRUCTURE_DEFINE__
    int64_t BrainStructure::s_brainStructureIdentifierCounter = 1;
#endif // __BRAIN_STRUCTURE_DEFINE__

} // namespace


#endif // __BRAIN_STRUCTURE_H__
