#ifndef __CIFTI_BRAINORDINATE_FILE_H__
#define __CIFTI_BRAINORDINATE_FILE_H__

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
    class GroupAndNameHierarchyModel;
    class VolumeFile;
    
    class CiftiBrainordinateFile: public CaretMappableDataFile, public SceneableInterface
    {
    
    public:
        CiftiBrainordinateFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~CiftiBrainordinateFile();
        
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
        
        virtual bool getMapVolumeVoxelValue(const int32_t mapIndex,
                                            const float xyz[3],
                                 int64_t ijkOut[3],
                                            AString& textOut) const;
        
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
        
        AString getMapDataTypeName() const;
        
        std::vector<int32_t> getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const;
        
        GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel();
        
        const GroupAndNameHierarchyModel* getGroupAndNameHierarchyModel() const;
        
   private:
        CiftiBrainordinateFile(const CiftiBrainordinateFile&);

        CiftiBrainordinateFile& operator=(const CiftiBrainordinateFile&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        void clearPrivate();
        
        void validateKeysAndLabels() const;
        
        class MapContent {
          
        public:
            enum MapContentDataType {
                MAP_CONTENT_DATA_TYPE_LABELS,
                MAP_CONTENT_DATA_TYPE_SCALARS
            };
            
            MapContent(CiftiInterface* ciftiInterface,
                       CiftiXML* m_ciftiXML,
                       const MapContentDataType mapContentDataType,
                       const int32_t mapIndex,
                       const float* mapData,
                       const int64_t mapDataCount);
            
            ~MapContent();
            
//            void updateColoring(const int32_t mapIndex,
//                                const PaletteFile* paletteFile);

            void updateColoring(const PaletteFile* paletteFile);
            
            void createVolume(const CiftiInterface* ciftiInterface);
            
            const FastStatistics* getFastStatistics();
            
            /** 
             * Data for the map.  Since the data is stored in row-major
             * order, the data for a map will not be in consecutive 
             * memory locations when there is more than one map.  Since
             * a number of operations (statistics, coloring) expect the
             * data to be in consecutive memory locations, the data must
             * be copied.
             */
            std::vector<float> m_data;
            
            /** count of elements in m_data */
            int64_t m_dataCount;
            
            /** coloring for data */
            std::vector<float> m_rgba;
            
            /** holds data and coloring for voxels */
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
            
            /** palette color mapping for map.  DO NOT delete since it points to value in the CIFTI XML */
            PaletteColorMapping* m_paletteColorMapping;
            
            /** label table for map.  DO NOT delete since it points to value in the CIFTI XML */
            GiftiLabelTable* m_labelTable;
            
            /** Maps CIFTI data into volume voxels */
            std::vector<CiftiVolumeMap> m_ciftiToVolumeMapping;
            
            /** Type of data in map */
            MapContentDataType m_mapContentDataType;
        };
        
        // ADD_NEW_MEMBERS_HERE
        
        /** Data for each map */
        std::vector<CaretPointer<MapContent> > m_mapContent;

        /** The CIFTI interface (could be local file or on network) */
        CaretPointer<CiftiInterface> m_ciftiInterface;
        
        /** The CIFTI XML (Do not delete since points to data in m_ciftiInterface */
        CiftiXML* m_ciftiXML;
        
        /** Metadata for the file */
        CaretPointer<GiftiMetaData> m_fileMetaData;
        
        /** Type of data in columns */
        MapContent::MapContentDataType m_columnMapDataType;
        
        
        /** Holds class and name hierarchy used for display selection */
        mutable GroupAndNameHierarchyModel* m_classNameHierarchy;
        
        /** force an update of the class and name hierarchy */
        mutable bool m_forceUpdateOfGroupAndNameHierarchy;
        
    };
    
#ifdef __CIFTI_BRAINORDINATE_FILE_DECLARE__
#endif // __CIFTI_BRAINORDINATE_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_BRAINORDINATE_FILE_H__
