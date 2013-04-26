#ifndef __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_H__
#define __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_H__

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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
#include "CiftiXML.h"
#include "SceneableInterface.h"

namespace caret {

    class CiftiInterface;
    class VolumeFile;
    
    class CiftiConnectivityMatrixDataFile
    : public CaretMappableDataFile,
    public SceneableInterface
    {
    
    public:
        CiftiConnectivityMatrixDataFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~CiftiConnectivityMatrixDataFile();
        
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
        
        virtual VolumeFile* getMapVolume(const int32_t mapIndex);
        
        virtual const VolumeFile* getMapVolume(const int32_t mapIndex) const;
        
        bool isMapDataLoadingEnabled(const int32_t mapIndex) const;
        
        void setMapDataLoadingEnabled(const int32_t mapIndex,
                                      const bool enabled);
        
        virtual int64_t loadMapDataForSurfaceNode(const int32_t mapIndex,
                                                  const int32_t surfaceNumberOfNodes,
                                                  const StructureEnum::Enum structure,
                                       const int32_t nodeIndex) throw (DataFileException);
        
        virtual void loadMapAverageDataForSurfaceNodes(const int32_t mapIndex,
                                                       const int32_t surfaceNumberOfNodes,
                                                       const StructureEnum::Enum structure,
                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException);
        
        virtual int64_t loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                        const float xyz[3]) throw (DataFileException);
        
        virtual bool getMapVolumeVoxelValue(const int32_t mapIndex,
                                            const float xyz[3],
                                 int64_t ijkOut[3],
                                            float &valueOut,
                                            AString& textOut) const;
        
        virtual bool getMapSurfaceNodeValue(const int32_t mapIndex,
                                            const StructureEnum::Enum structure,
                                 const int nodeIndex,
                                 const int32_t numberOfNodes,
                                 float& valueOut,
                                            AString& textOut) const;
        
        virtual bool getMapSurfaceNodeColoring(const int32_t mapIndex,
                                       const StructureEnum::Enum structure,
                                    float* surfaceRGBA,
                                    const int32_t surfaceNumberOfNodes);

        virtual int32_t getSurfaceNumberOfNodesForLoading(const StructureEnum::Enum structure) const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
   private:
        CiftiConnectivityMatrixDataFile(const CiftiConnectivityMatrixDataFile&);

        CiftiConnectivityMatrixDataFile& operator=(const CiftiConnectivityMatrixDataFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        void clearPrivate();
        
        int64_t getRowIndexForNodeWhenLoading(const StructureEnum::Enum structure,
                                              const int64_t surfaceNumberOfNodes,
                                               const int64_t nodeIndex) const;
        
        int64_t getRowIndexForVoxelWhenLoading(const int32_t mapIndex,
                                               const float xyz[3]) const;
        
        int64_t getColumnIndexForNodeWhenViewing(const StructureEnum::Enum structure,
                                                 const int64_t surfaceNumberOfNodes,
                                               const int64_t nodeIndex) const;
        
        int64_t getColumnIndexForVoxelWhenViewing(const int32_t mapIndex,
                                               const float xyz[3]) const;
        
        // ADD_NEW_MEMBERS_HERE

        class MapContent {
          
        public:
            MapContent();
            
            ~MapContent();
            
            void updateData(const CiftiInterface* ciftiInterface,
                            const int32_t mapIndex,
                            const float* data,
                            const int64_t dataCount,
                            const AString& mapName);

            void updateColoring(const int32_t mapIndex,
                                const PaletteFile* paletteFile);

            void createVolume(const CiftiInterface* ciftiInterface);
            
            bool voxelXYZToIJK(const float xyz[3],
                               int64_t ijkOut[3]) const;
            
            const FastStatistics* getFastStatistics();
            
            /** connectivity data */
            std::vector<float> m_data;
            
            /** count of elements in m_data */
            int64_t m_dataCount;
            
            /** coloring for connectivity data */
            std::vector<float> m_rgba;
            
            /** holds connectivity data and coloring for voxels */
            CaretPointer<VolumeFile> m_volumeFile;
            
            /** metadata for map */
            CaretPointer<GiftiMetaData> m_metaData;
            
            /** descriptive statistics for map */
            CaretPointer<DescriptiveStatistics> m_descriptiveStatistics;
            
            /** fast statistics for map */
            CaretPointer<FastStatistics> m_fastStatistics;
            
            /** histogram for map */
            CaretPointer<Histogram> m_histogram;
            
            /** name of map */
            AString m_name;
            
            /** palette color mapping for map from CIFTI file - DO NOT DELETE!!! */
            PaletteColorMapping* m_ciftiFilePaletteColorMapping;
            
            /** Maps CIFTI data into volume voxels */
            std::vector<CiftiVolumeMap> m_ciftiToVolumeMapping;
            
            /** Indicates if data loading is enabled */
            bool m_dataLoadingEnabled;
        };
        
        /** Data for each map */
        std::vector<CaretPointer<MapContent> > m_mapContent;

        /** The CIFTI interface (could be local file or on network) */
        CaretPointer<CiftiInterface> m_ciftiInterface;
        
        /** Metadata for the file */
        CaretPointer<GiftiMetaData> m_fileMetaData;
        
    public:
        /** 
         * Index for loading data from CIFTI.
         * Value is the same as CiftiXML::ALONG_COLUMNS.
         */
        static const int32_t CIFTI_INDEX_LOADING;
        
        /** 
         * Index for viewing data from CIFTI.
         * Value is the same as CiftiXML::ALONG_ROWS.
         */
        static const int32_t CIFTI_INDEX_VIEWING;
    };
    
#ifdef __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__
    const int32_t CiftiConnectivityMatrixDataFile::CIFTI_INDEX_LOADING = 1;
    const int32_t CiftiConnectivityMatrixDataFile::CIFTI_INDEX_VIEWING = 0;
#endif // __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_H__
