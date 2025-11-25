#ifndef __CIFTI_DENSE_SPARSE_FILE__H_
#define __CIFTI_DENSE_SPARSE_FILE__H_

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

#include <memory>

#include "BrainConstants.h"
#include "CaretMappableDataFile.h"
#include "CaretSparseFile.h"
#include "CiftiFileDynamicLoadingInterface.h"
#include "DisplayGroupEnum.h"
#include "FunctionResult.h"
#include "SceneClassAssistant.h"
#include "VoxelIJK.h"

namespace caret {
    
    class ConnectivityDataLoaded;
    class CiftiBrainordinateScalarFile;
    
    class CiftiDenseSparseFile : public CaretMappableDataFile, public CiftiFileDynamicLoadingInterface {
        
    public:
        CiftiDenseSparseFile();
        
        virtual ~CiftiDenseSparseFile();
        
        virtual void clear() override;
        
        bool isEmpty() const override;
        
        virtual StructureEnum::Enum getStructure() const override;
        
        virtual void setStructure(const StructureEnum::Enum structure) override;
        
        virtual GiftiMetaData* getFileMetaData() override;
        
        virtual const GiftiMetaData* getFileMetaData() const override;
        
        virtual bool isSurfaceMappable() const override;
        
        virtual bool isVolumeMappable() const override;
        
        virtual int32_t getNumberOfMaps() const override;
        
        virtual bool hasMapAttributes() const override;
        
        virtual AString getMapName(const int32_t mapIndex) const override;
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName) override;
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const override;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex) override;
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const override;
        
        virtual bool isMappedWithPalette() const override;
        
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex) override;
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex) override;
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                 const float mostPositiveValueInclusive,
                                                 const float leastPositiveValueInclusive,
                                                 const float leastNegativeValueInclusive,
                                                 const float mostNegativeValueInclusive,
                                                 const bool includeZeroValues) override;
        
        virtual int64_t getDataSizeUncompressedInBytes() const override;
        
        virtual const FastStatistics* getFileFastStatistics() override;
        
        virtual const Histogram* getFileHistogram() override;
        
        virtual const Histogram* getFileHistogram(const float mostPositiveValueInclusive,
                                                  const float leastPositiveValueInclusive,
                                                  const float leastNegativeValueInclusive,
                                                  const float mostNegativeValueInclusive,
                                                  const bool includeZeroValues) override;
        
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) override;
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const override;
        
        virtual bool getMapSurfaceNodeColoring(const int32_t mapIndex,
                                               const StructureEnum::Enum structure,
                                               float* surfaceRGBAOut,
                                               float* dataValuesOut,
                                               const int32_t surfaceNumberOfNodes);
        
        void invalidateColoringInAllMaps();
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) override;
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const override;
        
        virtual void getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const override;
        
        virtual bool isMapColoringValid(const int32_t mapIndex) const;
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex) override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;
        
        int32_t getMappingSurfaceNumberOfNodes(const StructureEnum::Enum structure) const;
        
        void getBrainordinateFromRowIndex(const int64_t rowIndex,
                                          StructureEnum::Enum& surfaceStructureOut,
                                          int32_t& surfaceNodeIndexOut,
                                          int32_t& surfaceNumberOfNodesOut,
                                          bool& surfaceNodeValidOut,
                                          int64_t voxelIJKOut[3],
                                          float voxelXYZOut[3],
                                          bool& voxelValidOut) const;
        
        virtual void loadMapDataForSurfaceNode(const int32_t /*mapIndex*/,
                                               const int32_t surfaceNumberOfNodes,
                                               const StructureEnum::Enum structure,
                                               const int32_t nodeIndex,
                                               int64_t& rowIndexOut,
                                               int64_t& columnIndexOut) override;
        
        virtual void loadMapAverageDataForSurfaceNodes(const int32_t /*mapIndex*/,
                                                      const int32_t surfaceNumberOfNodes,
                                                      const StructureEnum::Enum structure,
                                                      const std::vector<int32_t>& nodeIndices) override;
        
        virtual void loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                     const float xyz[3],
                                                     int64_t& rowIndexOut,
                                                     int64_t& columnIndexOut) override;
        
        virtual bool loadMapAverageDataForVoxelIndices(const int32_t mapIndex,
                                                       const int64_t volumeDimensionIJK[3],
                                                       const std::vector<VoxelIJK>& voxelIndices) override;
        
        virtual void loadDataForRowIndex(const int64_t rowIndex) override;
        
        virtual void loadDataForColumnIndex(const int64_t columnIndex) override;
        
        virtual bool isMapDataLoadingEnabled(const int32_t mapIndex) const override;
        
        virtual void setMapDataLoadingEnabled(const int32_t mapIndex,
                                              const bool enabled) override;
        
        void finishRestorationOfScene();
        
        int32_t getRowIndexFromSurfaceVertex(const StructureEnum::Enum surfaceStrucure,
                                             const int32_t surfaceNumberOfNodes,
                                             const int32_t nodeIndex) const;

        virtual bool getSurfaceNodeIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                         const StructureEnum::Enum structure,
                                                         const int nodeIndex,
                                                         const int32_t surfaceNumberOfNodes,
                                                         const AString& dataValueSeparator,
                                                         const int32_t digitsRightOfDecimal,
                                                         AString& textOut) const override;
        
        bool supportsWriting() const;
        
//        CiftiBrainordinateScalarFile* newCiftiScalarFileFromLoadedRowData(const AString& destinationDirectory,
//                                                                          AString& errorMessageOut) const;
        
        void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        bool hasCiftiXML() const;
        
        const CiftiXML getCiftiXML() const;
        
        bool isEnabledAsLayer() const;
        
        void setEnabledAsLayer(const bool enabled);
        
        virtual void getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                        std::vector<float>& dataOut) const override;
        
        virtual BrainordinateMappingMatch getBrainordinateMappingMatchImplementation(const CaretMappableDataFile* mapFile) const override;
        
        // ADD_NEW_METHODS_HERE
        
        const std::vector<float>& getLoadedRowData() const;
        
        AString getLoadedRowFileDescription() const;
        
        AString getLoadedRowMapName() const;
        
        bool getMapDataForSurface(const int32_t mapIndex,
                                  const StructureEnum::Enum structure,
                                  std::vector<float>& surfaceMapData,
                                  std::vector<float>* roiData) const;
        
        bool getSurfaceDataIndicesForMappingToBrainordinates(const StructureEnum::Enum structure,
                                                             const int64_t surfaceNumberOfNodes,
                                                             std::vector<int64_t>& dataIndicesForNodes) const;
    private:
        CiftiDenseSparseFile(const CiftiDenseSparseFile&);
        
        CiftiDenseSparseFile& operator=(const CiftiDenseSparseFile&);
        
    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass) override;
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass) override;
        
        
    private:
        bool loadRowsForAveraging(const std::vector<int64_t>& rowIndices);
        
        FunctionResult loadDataForRowIndexPrivate(const int32_t rowIndex,
                                                  std::vector<float>& dataOut) const;
        
        bool getThresholdData(const CaretMappableDataFile* threshMapFile,
                              const int32_t threshMapIndex,
                              std::vector<float>& thresholdDataOut) const;
        
        void clearPrivate();
        
        void clearLoadedData();
        
        void writeLoadedDataToFile(const AString& filename) const;
        
        std::unique_ptr<CaretSparseFile> m_sparseFile;
        
        bool m_dataLoadingEnabled;
        
        AString m_loadedDataDescriptionForFileCopy;
        AString m_loadedDataDescriptionForMapName;
        
        std::unique_ptr<ConnectivityDataLoaded> m_connectivityDataLoaded;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        std::unique_ptr<GiftiMetaData> m_fileMetadata;
        
        /* Note: File has one and only one map */
        
        std::unique_ptr<GiftiMetaData> m_mapMetadata;
        
        std::unique_ptr<PaletteColorMapping> m_mapPaletteColorMapping;
        
        std::unique_ptr<Histogram> m_mapHistogram;
        
        std::unique_ptr<Histogram> m_mapHistogramLimitedRange;
        
        std::unique_ptr<FastStatistics> m_mapFastStatistics;
        
        std::vector<float> m_loadedRowData;
        
        std::vector<float> m_rgba;
        
        bool m_rgbaValidFlag = false;
        
        int32_t m_fileNumberOfRows = 0;
        
        int32_t m_fileNumberOfColumns = 0;
        
        bool m_enabledAsLayerFlag = true;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __CIFTI_DENSE_SPARSE_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_DENSE_SPARSE_FILE_DECLARE__
    
} // namespace
#endif  //__CIFTI_DENSE_SPARSE_FILE__H_

