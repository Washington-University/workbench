#ifndef __CIFTI_MAPPABLE_DATA_FILE_H__
#define __CIFTI_MAPPABLE_DATA_FILE_H__

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

#include "CaretMappableDataFile.h"
#include "CaretPointer.h"
#include "CaretObjectTracksModification.h"
#include "CiftiMappingType.h"
#include "CiftiXMLElements.h"
#include "DisplayGroupEnum.h"
#include "VolumeMappableInterface.h"

#include <set>

namespace caret {
    
    class ChartData;
    class ChartDataCartesian;
    class CiftiFile;
    class CiftiParcelsMap;
    class CiftiXML;
    class FastStatistics;
    class GroupAndNameHierarchyModel;
    class Histogram;
    class SparseVolumeIndexer;

    
    class CiftiMappableDataFile :
    public CaretMappableDataFile,
    public VolumeMappableInterface {
        
    protected:
        /**
         * Method for accessing data with CIFTI
         */
        enum DataAccessMethod {
            /**
             * Invalid data access method.
             */
            DATA_ACCESS_METHOD_INVALID,
            /**
             * No access to data
             */
            DATA_ACCESS_NONE,
            /**
             * The data is accessed from CiftiFile using ROW Methods
             * and from the XML using ALONG_COLUMN
             */
            DATA_ACCESS_FILE_ROWS_OR_XML_ALONG_COLUMN,
            /**
             * Use ALONG_ROW to access CIFTI data
             * which means one is accessing COLUMNS of data
             */
            DATA_ACCESS_FILE_COLUMNS_OR_XML_ALONG_ROW
        };
        
        /**
         * Method for color mapping a map
         */
        enum ColorMappingMethod {
            /**
             * Invalid color mapping method. 
             */
            COLOR_MAPPING_METHOD_INVALID,
            /**
             * Color data using a label table
             */
            COLOR_MAPPING_METHOD_LABEL_TABLE,
            /**
             * Color data using a color palette
             */
            COLOR_MAPPING_METHOD_PALETTE
        };
        
        /**
         * Source of palette color mapping.
         * Some CIFTI files provide one palette color mapping per file.
         * Others have one palette color mapping for each map in the file.
         */
        enum PaletteColorMappingSourceType {
            PALETTE_COLOR_MAPPING_SOURCE_INVALID,
            /**
             * Use file's palette color mapping.
             */
            PALETTE_COLOR_MAPPING_SOURCE_FROM_FILE,
            /**
             * Use map's palette color mapping.
             */
            PALETTE_COLOR_MAPPING_SOURCE_FROM_MAP
        };
        
        /**
         * Type of map data in the file.
         */
        enum FileMapDataType {
            /**
             * Invalid file map data type
             */
            FILE_MAP_DATA_TYPE_INVALID,
            /**
             * The file contains a connectivity matrix.  There is only
             * 'one map' and the data for the map is selectively read
             * and replaced based upon user actions.
             */
            FILE_MAP_DATA_TYPE_MATRIX,
            /**
             * The file contains one or more maps and all maps are loaded
             * when the file is read.
             */
            FILE_MAP_DATA_TYPE_MULTI_MAP
        };
        
        
        /** How to read the file */
        enum FileDataReadingType {
            /** Read all data in the file */
            FILE_READ_DATA_ALL,
            /** Open the file but only read data as needed */
            FILE_READ_DATA_AS_NEEDED
        };

        CiftiMappableDataFile(const DataFileTypeEnum::Enum dataFileType);
        
    public:
        virtual ~CiftiMappableDataFile();
        
        static CiftiMappableDataFile* newInstanceForCiftiFileTypeAndSurface(const DataFileTypeEnum::Enum ciftiFileType,
                                                                     const StructureEnum::Enum structure,
                                                                     const int32_t numberOfNodes,
                                                                     AString& errorMessageOut);
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        static void addCiftiXmlToDataFileContentInformation(DataFileContentInformation& dataFileInformation,
                                                            const CiftiXML& ciftiXML);
        
        static void getDataFileContentInformationForGenericCiftiFile(const AString& filename,
                                                                     DataFileContentInformation& dataFileInformation);
        
        virtual void clear();
        
        virtual bool isEmpty() const;
        
        virtual bool isMappableToSurfaceStructure(const StructureEnum::Enum structure) const;
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual void setPreferOnDiskReading(const bool& prefer);
        
        virtual void readFile(const AString& ciftiMapFileName);
        
        virtual void writeFile(const AString& filename);
        
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
        
        virtual void getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut);
        
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
        
        const CiftiParcelsMap* getCiftiParcelsMapForBrainordinateMapping() const;
        
        const CiftiParcelsMap* getCiftiParcelsMapForLoading() const;
        
        const CiftiParcelsMap* getCiftiParcelsMapForDirection(const int direction) const;
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;
        
        virtual void updateScalarColoringForAllMaps(const PaletteFile* paletteFile);
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex,
                                                const PaletteFile* paletteFile);
        
        virtual bool isMapColoringValid(const int32_t mapIndex) const;
        
        virtual void getDimensions(int64_t& dimOut1,
                                   int64_t& dimOut2,
                                   int64_t& dimOut3,
                                   int64_t& dimTimeOut,
                                   int64_t& numComponents) const;
        
        virtual void getDimensions(std::vector<int64_t>& dimsOut) const;
        
        virtual void getMapDimensions(std::vector<int64_t> &dim) const;

        virtual const int64_t& getNumberOfComponents() const;
        
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float& coordOut1,
                                  float& coordOut2,
                                  float& coordOut3) const;
        
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float* coordOut) const;
        
        virtual void indexToSpace(const int64_t* indexIn,
                                  float* coordOut) const;
        
        virtual void enclosingVoxel(const float& coordIn1,
                                    const float& coordIn2,
                                    const float& coordIn3,
                                    int64_t& indexOut1,
                                    int64_t& indexOut2,
                                    int64_t& indexOut3) const;
        
        virtual bool indexValid(const int64_t& indexIn1,
                                const int64_t& indexIn2,
                                const int64_t& indexIn3,
                                const int64_t brickIndex = 0,
                                const int64_t component = 0) const;
        
        virtual const VolumeSpace& getVolumeSpace() const;
        
        virtual void getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const;
        
        virtual int64_t getVoxelColorsForSliceInMap(const PaletteFile* paletteFile,
                                                 const int32_t mapIndex,
                                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                 const int64_t sliceIndex,
                                                 const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 uint8_t* rgbaOut) const;
        
        virtual int64_t getVoxelColorsForSubSliceInMap(const PaletteFile* paletteFile,
                                                    const int32_t mapIndex,
                                                    const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                    const int64_t sliceIndex,
                                                    const int64_t firstCornerVoxelIndex[3],
                                                    const int64_t lastCornerVoxelIndex[3],
                                                    const int64_t voxelCountIJK[3],
                                                    const DisplayGroupEnum::Enum displayGroup,
                                                    const int32_t tabIndex,
                                                    uint8_t* rgbaOut) const;
        
        virtual void getVoxelColorInMap(const PaletteFile* paletteFile,
                                        const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t mapIndex,
                                        const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        uint8_t rgbaOut[4]) const;
        
        virtual void getVoxelColorInMapForLabelData(const PaletteFile* paletteFile,
                                                    const std::vector<float>& dataForMap,
                                        const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t mapIndex,
                                        const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        uint8_t rgbaOut[4]) const;
        
        virtual bool getMapVolumeVoxelValue(const int32_t mapIndex,
                                            const float xyz[3],
                                            int64_t ijkOut[3],
                                            float& numericalValueOut,
                                            bool& numericalValueOutValid,
                                            AString& textValueOut) const;
        
        virtual bool getMapVolumeVoxelValues(const std::vector<int32_t> mapIndices,
                                             const float xyz[3],
                                             int64_t ijkOut[3],
                                             std::vector<float>& numericalValuesOut,
                                             std::vector<bool>& numericalValuesOutValid,
                                             AString& textValueOut) const;
        
        int64_t getMapDataOffsetForVoxelAtCoordinate(const float coordinate[3],
                                                     const int32_t mapIndex) const;
        
        virtual float getVoxelValue(const float* coordinateIn,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const;
        
        virtual float getVoxelValue(const float coordinateX,
                                    const float coordinateY,
                                    const float coordinateZ,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const;
        
        virtual bool getVolumeVoxelIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                         const float xyz[3],
                                                         int64_t ijkOut[3],
                                                         AString& textOut) const;
        
        std::vector<int32_t> getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const;
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        const GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel() const;
        
        virtual bool getMapSurfaceNodeValue(const int32_t mapIndex,
                                            const StructureEnum::Enum structure,
                                            const int nodeIndex,
                                            const int32_t numberOfNodes,
                                            float& numericalValueOut,
                                            bool& numericalValueOutValid,
                                            AString& textValueOut) const;
        
        virtual bool getMapSurfaceNodeValues(const std::vector<int32_t>& mapIndices,
                                             const StructureEnum::Enum structure,
                                             const int nodeIndex,
                                             const int32_t numberOfNodes,
                                             std::vector<float>& numericalValuesOut,
                                             std::vector<bool>& numericalValuesOutValid,
                                             AString& textValueOut) const;
        
        virtual bool getSurfaceNodeIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                            const StructureEnum::Enum structure,
                                                            const int nodeIndex,
                                                            const int32_t numberOfNodes,
                                                            AString& textOut) const;
        
        int32_t getMappingSurfaceNumberOfNodes(const StructureEnum::Enum structure) const;
        
        bool getSeriesDataForSurfaceNode(const StructureEnum::Enum structure,
                                         const int32_t nodeIndex,
                                         std::vector<float>& seriesDataOut) const;
        
        bool getSeriesDataForVoxelAtCoordinate(const float xyz[3],
                                               std::vector<float>& seriesDataOut) const;
        
        virtual bool getMapSurfaceNodeColoring(const PaletteFile* paletteFile,
                                               const int32_t mapIndex,
                                               const StructureEnum::Enum structure,
                                               float* surfaceRGBAOut,
                                               float* dataValuesOut,
                                               const int32_t surfaceNumberOfNodes);
        
        virtual void clearModified();
        
        virtual bool isModifiedExcludingPaletteColorMapping() const;
        
        virtual NiftiTimeUnitsEnum::Enum getMapIntervalUnits() const;
        
        virtual void getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                                float& mapIntervalStepValueOut) const;
        
        virtual bool getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                             float& dataRangeMaximumOut) const;
        
        bool getMapDataForSurface(const int32_t mapIndex,
                                  const StructureEnum::Enum structure,
                                  std::vector<float>& surfaceMapData,
                                  std::vector<float>* roiData = NULL) const;
        
        void setMapDataForSurface(const int32_t mapIndex,
                                  const StructureEnum::Enum structure,
                                  const std::vector<float> surfaceMapData);
        
        bool getParcelNodesElementForSelectedParcel(std::set<int64_t> &parcelNodesOut,
                                                    const StructureEnum::Enum &structure,
                                                    const int64_t &selectionIndex) const;
        
        void invalidateColoringInAllMaps();
        
        void getBrainordinateFromRowIndex(const int64_t rowIndex,
                                          StructureEnum::Enum& surfaceStructureOut,
                                          int32_t& surfaceNodeIndexOut,
                                          int32_t& surfaceNumberOfNodesOut,
                                          bool& surfaceNodeValidOut,
                                          int64_t voxelIJKOut[3],
                                          float voxelXYZOut[3],
                                          bool& voxelValidOut) const;
        
    private:
        
        CiftiMappableDataFile(const CiftiMappableDataFile&);
        
        CiftiMappableDataFile& operator=(const CiftiMappableDataFile&);
        
    public:
        
        virtual void getMapData(const int32_t mapIndex,
                                std::vector<float>& dataOut) const;
        
        virtual void setMapData(const int32_t mapIndex,
                                const std::vector<float>& data);
        
        virtual void getMatrixRGBA(std::vector<float>& rgba, PaletteFile *paletteFile);
    
        virtual void getFileData(std::vector<float>& data) const;
        
        const CiftiFile* getCiftiFile() const { return m_ciftiFile; }
        
    protected:
        virtual bool getParcelLabelMapSurfaceNodeValue(const int32_t mapIndex,
                                            const StructureEnum::Enum structure,
                                            const int nodeIndex,
                                            const int32_t numberOfNodes,
                                            float& numericalValueOut,
                                            bool& numericalValueOutValid,
                                            AString& textValueOut) const;
        
        void updateForChangeInMapDataWithMapIndex(const int32_t mapIndex);
        
        ChartDataCartesian* helpLoadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                       const int32_t nodeIndex);
        
        ChartDataCartesian* helpLoadChartDataForSurfaceNodeAverage(const StructureEnum::Enum structure,
                                                          const std::vector<int32_t>& nodeIndices);
        
        ChartDataCartesian* helpLoadChartDataForVoxelAtCoordinate(const float xyz[3]);
        
        void helpMapFileGetMatrixDimensions(int32_t& numberOfRowsOut,
                                            int32_t& numberOfColumnsOut) const;
        
        bool helpMapFileLoadChartDataMatrixRGBA(int32_t& numberOfRowsOut,
                                                int32_t& numberOfColumnsOut,
                                                const std::vector<int32_t>& rowIndicesIn,
                                                std::vector<float>& rgbaOut) const;
        
        bool helpMatrixFileLoadChartDataMatrixRGBA(int32_t& numberOfRowsOut,
                                                   int32_t& numberOfColumnsOut,
                                                   const std::vector<int32_t>& rowIndicesIn,
                                                   std::vector<float>& rgbaOut) const;
        
//        bool helpLoadChartDataMatrixRGBA(int32_t& numberOfRowsOut,
//                                         int32_t& numberOfColumnsOut,
//                                         std::vector<float>& rgbaOut) const;
//
//        bool helpLoadChartDataMatrixRGBAWithRowIndicese(int32_t& numberOfRowsOut,
//                                                        int32_t& numberOfColumnsOut,
//                                                        const std::vector<int32_t>& rowIndices,
//                                                        std::vector<float>& rgbaOut) const;
        
    private:
        class MapContent : public CaretObjectTracksModification {
            
        public:
            
            MapContent(CiftiFile* ciftiFile,
                       const FileMapDataType fileMapDataType,
                       const int32_t readingDirectionForCiftiXML,
                       const int32_t mappingDirectionForCiftiXML,
                       const int32_t mapIndex);
            
            ~MapContent();
            
            virtual void clearModified();
            
            virtual bool isModified() const;
            
            void updateForChangeInMapData();
            
            void updateColoring(const std::vector<float>& data,
                                const PaletteFile* paletteFile,
                                const FastStatistics* fastStatistics);
            
            bool isFastStatisticsValid() const;
            
            void updateFastStatistics(const std::vector<float>& data);
            
            bool isHistogramValid() const;
            
            void updateHistogram(const std::vector<float>& data);
            
            bool isHistogramLimitedValuesValid(const float mostPositiveValueInclusive,
                                               const float leastPositiveValueInclusive,
                                               const float leastNegativeValueInclusive,
                                               const float mostNegativeValueInclusive,
                                               const bool includeZeroValues) const;
            
            void updateHistogramLimitedValues(const std::vector<float>& data,
                                              const float mostPositiveValueInclusive,
                                              const float leastPositiveValueInclusive,
                                              const float leastNegativeValueInclusive,
                                              const float mostNegativeValueInclusive,
                                              const bool includeZeroValues);
            
            AString getName() const;
            
            void setName(const AString& name);
            
            /** The CIFTI file pointer */
            CiftiFile *m_ciftiFile;
            
            /** Maps or Matrix file */
            const FileMapDataType m_fileMapDataType;
            
            /** Direction for obtaining reading information (CiftiXML::ALONG_ROW, etc) */
            const int32_t m_readingDirectionForCiftiXML;
            
            /** Direction for obtaining mapping information (CiftiXML::ALONG_ROW, etc) */
            const int32_t m_mappingDirectionForCiftiXML;
            
            /** Index of this map */
            const int32_t m_mapIndex;
            
            /** Name of the map */
            AString m_mapName;
            
            /** Count of data elements in map. */
            int64_t m_dataCount;
            
            /** Metadata for the map. Points to data in CiftiFile so DO NOT delete */
            GiftiMetaData* m_metadata;
            
            /** Palette color mapping for map. Points to data in CiftiFile so DO NOT delete  */
            PaletteColorMapping* m_paletteColorMapping;
            
            /** Label table for map. Points to data in CiftiFile so DO NOT delete  */
            GiftiLabelTable* m_labelTable;

            /** Indicates data is mapped with a lable table */
            bool m_dataIsMappedWithLabelTable;
            
            /** RGBA coloring for map */
            std::vector<uint8_t> m_rgba;
            
            /** RGBA coloring is valid */
            bool m_rgbaValid;
            
            /** fast statistics for map */
            CaretPointer<FastStatistics> m_fastStatistics;
            
            /** histogram for all of map map */
            CaretPointer<Histogram> m_histogram;
        
            /** histogram for limited values from map */
            CaretPointer<Histogram> m_histogramLimitedValues;
            
            float m_histogramLimitedValuesMostPositiveValueInclusive;
            float m_histogramLimitedValuesLeastPositiveValueInclusive;
            float m_histogramLimitedValuesLeastNegativeValueInclusive;
            float m_histogramLimitedValuesMostNegativeValueInclusive;
            bool m_histogramLimitedValuesIncludeZeroValues;
            
        private:
            /** Name of map */
            AString m_name;
            
            /**
             * For maps that do not have metadata, a metadata instance
             * is still needed even though it essentially does nothing.
             */
            CaretPointer<GiftiMetaData> m_metadataForMapsWithNoMetaData;
        };
        
        void clearPrivate();
        
    protected:
        void initializeAfterReading(const AString& filename);
        
        void validateMappingTypes(const AString& filename);
        
        void resetDataLoadingMembers();
        
        void validateKeysAndLabels() const;
        
        virtual void validateAfterFileReading();
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
        bool getSurfaceDataIndicesForMappingToBrainordinates(const StructureEnum::Enum structure,
                                                             const int64_t surfaceNumberOfNodes,
                                                             std::vector<int64_t>& dataIndicesForNodes) const;
        
        void setupCiftiReadingMappingDirection();
        
        static AString mappingTypeToName(const CiftiMappingType::MappingType mappingType);

        /**
         * Point to the CIFTI file object.
         */
        CaretPointer<CiftiFile> m_ciftiFile;
        
        /**
         * How to read data from the file
         */
        FileDataReadingType m_fileDataReadingType;
        
        /**
         * Method used when reading data from the file.
         */
        DataAccessMethod m_dataReadingAccessMethod;
        
        /** Direction for obtaining mapping information (CiftiXML::ALONG_ROW, etc) */
        int32_t m_dataReadingDirectionForCiftiXML;
        
        /**
         * Method used when mapping loaded data to brainordinates.
         */
        DataAccessMethod m_dataMappingAccessMethod;
        
        /** Direction for obtaining mapping information (CiftiXML::ALONG_ROW, etc) */
        int32_t m_dataMappingDirectionForCiftiXML;
        
        /**
         * Method used when mapping data to colors (LabelTable or Palette).
         */
        ColorMappingMethod m_colorMappingMethod;
        
        /**
         * Source of color palette (file or per map)
         */
        PaletteColorMappingSourceType m_paletteColorMappingSource;
        
        /*
         * Supported palette normalization modes
         */
        std::vector<PaletteNormalizationModeEnum::Enum> m_paletteNormalizationModesSupported;
        
        /**
         * Type of data in the file's map(s).
         */
        FileMapDataType m_fileMapDataType;
        
        /** Contains data related to each map */
        std::vector<MapContent*> m_mapContent;
        
        /** True if the file contains surface data */
        bool m_containsSurfaceData;
        
        /** True if the file contains surface data */
        bool m_containsVolumeData;
        
        float m_mappingTimeStart;
        
        float m_mappingTimeStep;
        
        NiftiTimeUnitsEnum::Enum m_mappingTimeUnits;
        
        /** Fast statistics used when statistics computed on all data in file */
        CaretPointer<FastStatistics> m_fileFastStatistics;
        
        /** Histogram used when statistics computed on all data in file */
        CaretPointer<Histogram> m_fileHistogram;
        
        /** Histogram with limited values used when statistics computed on all data in file */
        CaretPointer<Histogram> m_fileHistorgramLimitedValues;
        
        float m_fileHistogramLimitedValuesMostPositiveValueInclusive;
        float m_fileHistogramLimitedValuesLeastPositiveValueInclusive;
        float m_fileHistogramLimitedValuesLeastNegativeValueInclusive;
        float m_fileHistogramLimitedValuesMostNegativeValueInclusive;
        bool m_fileHistogramLimitedValuesIncludeZeroValues;
        
        /** Fast conversion of IJK to data offset */
        CaretPointer<SparseVolumeIndexer> m_voxelIndicesToOffset;
        
        /** Holds class and name hierarchy used for display selection */
        mutable CaretPointer<GroupAndNameHierarchyModel> m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        mutable bool m_forceUpdateOfGroupAndNameHierarchy;

        
        static const int32_t S_CIFTI_XML_ALONG_INVALID;
        
//        std::vector<int64_t> m_ciftiDimensions;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __CIFTI_MAPPABLE_DATA_FILE_DECLARE__
    const int32_t CiftiMappableDataFile::S_CIFTI_XML_ALONG_INVALID = -1;
#endif // __CIFTI_MAPPABLE_DATA_FILE_DECLARE__
    
} // namespace

#endif  //__CIFTI_MAPPABLE_DATA_FILE_H__
