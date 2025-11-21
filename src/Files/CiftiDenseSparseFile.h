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
#include "DisplayGroupEnum.h"
#include "SceneClassAssistant.h"
#include "VoxelIJK.h"

namespace caret {

    class ConnectivityDataLoaded;
    class CiftiBrainordinateScalarFile;
    
    class CiftiDenseSparseFile : public CaretMappableDataFile {
        
    public:
        CiftiDenseSparseFile();
        
        virtual ~CiftiDenseSparseFile();
        
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
        
        bool isDataLoadingEnabled() const;
        
        void setDataLoadingEnabled(const bool loadingEnabled);
                
        void finishRestorationOfScene();
        
        bool supportsWriting() const;
        
        CiftiBrainordinateScalarFile* newCiftiScalarFileFromLoadedRowData(const AString& destinationDirectory,
                                                                          AString& errorMessageOut) const;
        
        void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        bool hasCiftiXML() const;
        
        const CiftiXML getCiftiXML() const;

        virtual void getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                        std::vector<float>& dataOut) const override;

        virtual BrainordinateMappingMatch getBrainordinateMappingMatchImplementation(const CaretMappableDataFile* mapFile) const override;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        CiftiDenseSparseFile(const CiftiDenseSparseFile&);

        CiftiDenseSparseFile& operator=(const CiftiDenseSparseFile&);
        
    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        

    private:
        bool loadRowsForAveraging(const std::vector<int64_t>& rowIndices);
        
        void clearPrivate();
        
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
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_DENSE_SPARSE_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_DENSE_SPARSE_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_DENSE_SPARSE_FILE__H_
