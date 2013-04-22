#ifndef __CIFTI_MAPPABLE_DATA_FILE_H__
#define __CIFTI_MAPPABLE_DATA_FILE_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretMappableDataFile.h"
#include "CaretPointer.h"
#include "CiftiXMLElements.h"
#include "SceneableInterface.h"
#include "VolumeMappableInterface.h"

namespace caret {

    class CiftiInterface;
    class CiftiXML;
    class DescriptiveStatistics;
    class FastStatistics;
    class GroupAndNameHierarchyModel;
    class Histogram;
    class SparseVolumeIndexer;
    
    class CiftiMappableDataFile :
        public CaretMappableDataFile,
        public SceneableInterface,
        public VolumeMappableInterface {
        
    public:
        /** Access (row/column) of a type of data */
        enum DataAccess {
            /** Data access invalid */
            DATA_ACCESS_INVALID,
            /** Data is accessed using the column methods */
            DATA_ACCESS_WITH_COLUMN_METHODS,
            /** Data is accessed using the row methods */
            DATA_ACCESS_WITH_ROW_METHODS
        };
        
        /** How to read the file */
        enum FileReading {
            /** Read all data in the file */
            FILE_READ_DATA_ALL,
            /** Open the file but only read data as needed */
            FILE_READ_DATA_AS_NEEDED
        };
        
        CiftiMappableDataFile(const DataFileTypeEnum::Enum dataFileType,
                              const FileReading fileReading,
                              const IndicesMapToDataType rowIndexType,
                              const IndicesMapToDataType columnIndexType,
                              const DataAccess brainordinateMappedDataAccess,
                              const DataAccess seriesDataAccess);
        
        virtual ~CiftiMappableDataFile();
        
        virtual void clear();
        
        virtual bool isEmpty() const;
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        virtual bool isSurfaceMappable() const;
        
        virtual bool isVolumeMappable() const;
        
        virtual int32_t getNumberOfMaps() const;
        
        virtual AString getMapName(const int32_t mapIndex) const;
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const;
        
        virtual bool isMappedWithPalette() const;
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex);
        
        virtual const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex,
                                                              const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues);
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                 const float mostPositiveValueInclusive,
                                                 const float leastPositiveValueInclusive,
                                                 const float leastNegativeValueInclusive,
                                                 const float mostNegativeValueInclusive,
                                                 const bool includeZeroValues);
        
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        virtual bool isMappedWithLabelTable() const;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex,
                                                const PaletteFile* paletteFile);
        
        virtual void getDimensions(int64_t& dimOut1,
                                   int64_t& dimOut2,
                                   int64_t& dimOut3,
                                   int64_t& dimTimeOut,
                                   int64_t& numComponents) const;
        
        virtual void getDimensions(std::vector<int64_t>& dimsOut) const;
        
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
        
        virtual void getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const;
        
        virtual void getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                 const int64_t sliceIndex,
                                                 uint8_t* rgbaOut) const;
        
        virtual void getVoxelColorInMap(const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t mapIndex,
                                        uint8_t rgbaOut[4]) const;
        
        virtual const float& getValue(const int64_t& indexIn1,
                                      const int64_t& indexIn2,
                                      const int64_t& indexIn3,
                                      const int64_t mapIndex = 0,
                                      const int64_t component = 0) const;

        std::vector<int32_t> getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const;
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        const GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel() const;
        
        virtual bool getMapSurfaceNodeValue(const int32_t mapIndex,
                                            const StructureEnum::Enum structure,
                                            const int nodeIndex,
                                            const int32_t numberOfNodes,
                                            AString& textOut) const;
        
        virtual bool getMapSurfaceNodeColoring(const int32_t mapIndex,
                                               const StructureEnum::Enum structure,
                                               float* surfaceRGBAOut,
                                               float* dataValuesOut,
                                               const int32_t surfaceNumberOfNodes);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        CiftiMappableDataFile(const CiftiMappableDataFile&);

        CiftiMappableDataFile& operator=(const CiftiMappableDataFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        /** The CIFTI XML (Do not delete since points to data in m_ciftiInterface */
        //CiftiXML* m_ciftiXML;
        class MapContent {
            
        public:
            
            MapContent(const IndicesMapToDataType mapContentDataType,
                       const int64_t dataCount,
                       const std::map<AString, AString> metadataMap,
                       PaletteColorMapping* paletteColorMapping,
                       GiftiLabelTable* labelTable);
            
            ~MapContent();
            
            void updateColoring(const std::vector<float>& data,
                                const PaletteFile* paletteFile);
            
            /** Type of data in map */
            const IndicesMapToDataType m_mapContentDataType;
            
            /** Count of data elements in map. */
            const int64_t m_dataCount;
            
            /** Metadata for the map. */
            CaretPointer<GiftiMetaData> m_metadata;
            
            /** Palette color mapping for map */
            PaletteColorMapping* m_paletteColorMapping;
            
            /** Label table for map */
            GiftiLabelTable* m_labelTable;
            
            /** RGBA coloring for map */
            std::vector<float> m_rgba;
            
            /** descriptive statistics for map */
            CaretPointer<DescriptiveStatistics> m_descriptiveStatistics;
            
            /** fast statistics for map */
            CaretPointer<FastStatistics> m_fastStatistics;
            
            /** histogram for map */
            CaretPointer<Histogram> m_histogram;
        };
        
        void clearPrivate();
        
        static AString ciftiIndexTypeToName(const IndicesMapToDataType ciftiIndexType);
        
        void getMapData(const int32_t mapIndex,
                        std::vector<float>& dataOut) const;
        
        void validateKeysAndLabels() const;
        
        /** How to read data from file */
        const FileReading m_fileReading;
        
        /** Type of data along the row (dimension 0) */
        const IndicesMapToDataType m_rowIndexType;
        
        /** Type of data along the column (dimension 1) */
        const IndicesMapToDataType m_columnIndexType;
        
        /** Location of brainordinate mapped data */
        const DataAccess m_brainordinateMappedDataAccess;
        
        /** Location of series data */
        const DataAccess m_seriesDataAccess;
        
        /** Contains data related to each map */
        std::vector<MapContent*> m_mapContent;
        
        /** True if data is mapped with a palette */
        bool m_dataIsMappedWithPalette;
        
        /** True if data is mapped with a label table */
        bool m_dataIsMappedWithLabelTable;
        
        /** True if the file contains surface data */
        bool m_containsSurfaceData;
        
        /** True if the file contains volume data */
        bool m_containsVolumeData;
        
        /** True if one map per file */
        bool m_oneMapPerFile;
        
        /** The CIFTI interface (could be local file or on network) */
        CaretPointer<CiftiInterface> m_ciftiInterface;
        
        /** The files metadata. */
        CaretPointer<GiftiMetaData> m_metadata;
        
        /** Number of rows in the file */
        int32_t m_numberOfRows;
        
        /** Number of columns in the file */
        int32_t m_numberOfColumns;
        
        /** Dimensions of volume data */
        int64_t m_volumeDimensions[5];
        
        /** Fast conversion of IJK to data offset */
        CaretPointer<SparseVolumeIndexer> m_voxelIndicesToOffset;
        
        /** Holds class and name hierarchy used for display selection */
        mutable CaretPointer<GroupAndNameHierarchyModel> m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        mutable bool m_forceUpdateOfGroupAndNameHierarchy;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_MAPPABLE_DATA_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_MAPPABLE_DATA_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_MAPPABLE_DATA_FILE_H__
