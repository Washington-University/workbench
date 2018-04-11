#ifndef __CIFTI_FIBER_TRAJECTORY_FILE__H_
#define __CIFTI_FIBER_TRAJECTORY_FILE__H_

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

#include "BrainConstants.h"
#include "CaretMappableDataFile.h"
#include "CaretSparseFile.h"
#include "DisplayGroupEnum.h"
#include "SceneClassAssistant.h"
#include "VoxelIJK.h"

namespace caret {

    class CiftiFiberOrientationFile;
    class ConnectivityDataLoaded;
    class FiberOrientationTrajectory;
    class FiberTrajectoryMapProperties;
    class GiftiMetaData;
    
    class CiftiFiberTrajectoryFile : public CaretMappableDataFile {
        
    public:
        CiftiFiberTrajectoryFile();
        
        virtual ~CiftiFiberTrajectoryFile();
        
        virtual void clear();
        
        bool isEmpty() const;
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual bool isSurfaceMappable() const;
        
        virtual bool isVolumeMappable() const;
        
        virtual int32_t getNumberOfMaps() const;
        
        virtual bool hasMapAttributes() const;
        
        virtual AString getMapName(const int32_t mapIndex) const;
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const;
        
        virtual bool isMappedWithPalette() const;
        
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                 const float mostPositiveValueInclusive,
                                                 const float leastPositiveValueInclusive,
                                                 const float leastNegativeValueInclusive,
                                                 const float mostNegativeValueInclusive,
                                                 const bool includeZeroValues);
        
        virtual int64_t getDataSizeUncompressedInBytes() const;
        
        virtual const FastStatistics* getFileFastStatistics();
        
        virtual const Histogram* getFileHistogram();
        
        virtual const Histogram* getFileHistogram(const float mostPositiveValueInclusive,
                                                  const float leastPositiveValueInclusive,
                                                  const float leastNegativeValueInclusive,
                                                  const float mostNegativeValueInclusive,
                                                  const bool includeZeroValues);
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;
        
        virtual void getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const;
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex) override;
        
        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);
        
        void getBrainordinateFromRowIndex(const int64_t rowIndex,
                                          StructureEnum::Enum& surfaceStructureOut,
                                          int32_t& surfaceNodeIndexOut,
                                          int32_t& surfaceNumberOfNodesOut,
                                          bool& surfaceNodeValidOut,
                                          int64_t voxelIJKOut[3],
                                          float voxelXYZOut[3],
                                          bool& voxelValidOut) const;
        
        int64_t loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                    const int32_t surfaceNumberOfNodes,
                                    const int32_t nodeIndex);
        
        void loadDataAverageForSurfaceNodes(const StructureEnum::Enum structure,
                                            const int32_t surfaceNumberOfNodes,
                                            const std::vector<int32_t>& nodeIndices);
        
        virtual int64_t loadMapDataForVoxelAtCoordinate(const float xyz[3]);
        
        virtual void loadMapAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices);
        
        void loadDataForRowIndex(const int64_t rowIndex);
        
        const std::vector<FiberOrientationTrajectory*>& getLoadedFiberOrientationTrajectories() const;
        
        void clearLoadedFiberOrientations();
        
        FiberTrajectoryMapProperties* getFiberTrajectoryMapProperties();
        
        const FiberTrajectoryMapProperties* getFiberTrajectoryMapProperties() const;
        
        bool isDataLoadingEnabled() const;
        
        void setDataLoadingEnabled(const bool loadingEnabled);
        
        CiftiFiberOrientationFile* getMatchingFiberOrientationFile();
        
        const CiftiFiberOrientationFile* getMatchingFiberOrientationFile() const;
        
        bool isFiberOrientationFileCombatible(const CiftiFiberOrientationFile* fiberOrientationFile) const;
        
        void setMatchingFiberOrientationFile(CiftiFiberOrientationFile* matchingFiberOrientationFile);
        
        void updateMatchingFiberOrientationFileFromList(std::vector<CiftiFiberOrientationFile*> matchingFiberOrientationFiles);
        
        void finishRestorationOfScene();
        
        bool supportsWriting() const;
        
        CiftiFiberTrajectoryFile* newFiberTrajectoryFileFromLoadedRowData(const AString& destinationDirectory,
                                                                          AString& errorMessageOut) const;
        
        void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        bool hasCiftiXML() const;
        
        const CiftiXML getCiftiXML() const;

        virtual void getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                        std::vector<float>& dataOut) const override;

        virtual BrainordinateMappingMatch getBrainordinateMappingMatch(const CaretMappableDataFile* mapFile) const override;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        CiftiFiberTrajectoryFile(const CiftiFiberTrajectoryFile&);

        CiftiFiberTrajectoryFile& operator=(const CiftiFiberTrajectoryFile&);
        
    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        

    private:
        /**
         * Type of fiber trajectory file
         */
        enum FiberTrajectoryFileType {
            /** Load data by brainordinate */
            FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE,
            /** Load single row (does not map to a brainordinate) */
            FIBER_TRAJECTORY_LOAD_SINGLE_ROW
        };
        
        bool loadRowsForAveraging(const std::vector<int64_t>& rowIndices);
        
        void clearPrivate();
        
        void validateAssignedMatchingFiberOrientationFile();
        
        void finishFiberOrientationTrajectoriesAveraging();
       
        void writeLoadedDataToFile(const AString& filename) const;
        
        /** True if file supports loading of data by row */
        FiberTrajectoryFileType m_fiberTrajectoryFileType;
        
        CaretSparseFile* m_sparseFile;
        
        GiftiMetaData* m_metadata;
        
        CiftiFiberOrientationFile* m_matchingFiberOrientationFile;
        AString m_matchingFiberOrientationFileName;
        AString m_matchingFiberOrientationFileNameFromRestoredScene;
        
        std::vector<FiberOrientationTrajectory*> m_fiberOrientationTrajectories;

        FiberTrajectoryMapProperties* m_fiberTrajectoryMapProperties;
        
        bool m_dataLoadingEnabled;
        
        AString m_loadedDataDescriptionForFileCopy;
        AString m_loadedDataDescriptionForMapName;
        
        ConnectivityDataLoaded* m_connectivityDataLoaded;
        
        SceneClassAssistant* m_sceneAssistant;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_FIBER_TRAJECTORY_FILE__H_
