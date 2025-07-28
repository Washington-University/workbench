#ifndef __META_VOLUME_FILE_H__
#define __META_VOLUME_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include <array>
#include <memory>
#include <vector>

#include "CaretMappableDataFile.h"
#include "CaretObjectTracksModification.h"
#include "FunctionResult.h"

namespace caret {
    class DataSliceArray;
    class Vector3D;
    class VolumeFile;

    class MetaVolumeFile : public CaretMappableDataFile {
        
    private:
        
        class MapInfo : public CaretObjectTracksModification {
            
        public:
            
            MapInfo(MetaVolumeFile* parentMetaVolumeFile,
                    const int32_t mapIndex);
            
            ~MapInfo();
            
            virtual void clearModified();
            
            bool isModifiedExcludingPaletteColorMapping() const;
            
            bool isModifiedPaletteColorMapping() const;
            
            void updateForChangeInMapData();
            
            bool isFastStatisticsValid() const;
            
            const FastStatistics* getFastStatistics() const;
            
            bool isHistogramValid(const int32_t numberOfBuckets) const;
            
            const Histogram* getHistogram(const int32_t numberOfBuckets) const;
            
            bool isHistogramLimitedValuesValid(const int32_t numberOfBuckets,
                                               const float mostPositiveValueInclusive,
                                               const float leastPositiveValueInclusive,
                                               const float leastNegativeValueInclusive,
                                               const float mostNegativeValueInclusive,
                                               const bool includeZeroValues) const;
            
            const Histogram* getHistogramLimitedValues(const int32_t numberOfBuckets,
                                                       const float mostPositiveValueInclusive,
                                                       const float leastPositiveValueInclusive,
                                                       const float leastNegativeValueInclusive,
                                                       const float mostNegativeValueInclusive,
                                                       const bool includeZeroValues) const;
                        
            GiftiMetaData* getMetaData();
            
            const GiftiMetaData* getMetaData() const;
            
            PaletteColorMapping* getPaletteColorMapping();
            
            const PaletteColorMapping* getPaletteColorMapping() const;
            
        private:
            /** Parent meta-volume file */
            MetaVolumeFile* m_parentMetaVolumeFile;
            
            /** Index of this map */
            const int32_t m_mapIndex;
            
            /** Metadata for the map. Points to data in CiftiFile so DO NOT delete */
            std::unique_ptr<GiftiMetaData> m_metadata;
            
            /** Palette color mapping for map. Points to data in CiftiFile so DO NOT delete  */
            std::unique_ptr<PaletteColorMapping> m_paletteColorMapping;
            
            /** fast statistics for map */
            mutable std::unique_ptr<FastStatistics> m_fastStatistics;
            
            /** histogram for all of map map */
            mutable std::unique_ptr<Histogram> m_histogram;
            
            mutable int32_t m_histogramNumberOfBuckets = 100;
            
            /** histogram for limited values from map */
            mutable std::unique_ptr<Histogram> m_histogramLimitedValues;
            
            mutable int32_t m_histogramLimitedValuesNumberOfBuckets = 100;
            mutable float m_histogramLimitedValuesMostPositiveValueInclusive;
            mutable float m_histogramLimitedValuesLeastPositiveValueInclusive;
            mutable float m_histogramLimitedValuesLeastNegativeValueInclusive;
            mutable float m_histogramLimitedValuesMostNegativeValueInclusive;
            mutable bool m_histogramLimitedValuesIncludeZeroValues;
            
        };

        
    public:
        MetaVolumeFile();
        
        virtual ~MetaVolumeFile();
        
        MetaVolumeFile(const MetaVolumeFile&) = delete;

        MetaVolumeFile& operator=(const MetaVolumeFile&) = delete;
        
        FunctionResult addVolumeFile(VolumeFile* volumeFile);
        
        int32_t getNumberOfVolumeFiles() const;
        
        VolumeFile* getVolumeFile(const int32_t index);
        
        const VolumeFile* getVolumeFile(const int32_t index) const;
        
        const VolumeFile* getVolumeFileContainingXYZ(const int32_t mapIndex,
                                                     const Vector3D& xyz,
                                                     float& voxelValueOut) const;
        
        virtual void clearModified() override;
        
        virtual bool isModifiedExcludingPaletteColorMapping() const override;
        
        virtual void clear() override;
        
        virtual bool isEmpty() const override;
        
        virtual void readFile(const AString& filenameIn) override;
        
        virtual void writeFile(const AString& filename) override;
        
        virtual StructureEnum::Enum getStructure() const override;
        
        virtual void setStructure(const StructureEnum::Enum structure) override;
        
        virtual GiftiMetaData* getFileMetaData() override;
        
        virtual const GiftiMetaData* getFileMetaData() const override;

        virtual bool isSurfaceMappable() const override;
        
        virtual bool isVolumeMappable() const override;
        
        virtual int32_t getNumberOfMaps() const override;
        
        virtual AString getMapName(const int32_t mapIndex) const override;
        
        virtual void setMapName(const int32_t mapIndex,
                                const AString& mapName) override;
        
        virtual const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const override;
        
        virtual GiftiMetaData* getMapMetaData(const int32_t mapIndex) override;
        
        virtual AString getMapUniqueID(const int32_t mapIndex) const override;
        
        virtual bool isMappedWithPalette() const override;
        
        virtual int64_t getDataSizeUncompressedInBytes() const override;
        
        virtual const FastStatistics* getMapFastStatistics(const int32_t mapIndex) override;
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex) override;
        
        virtual const Histogram* getMapHistogram(const int32_t mapIndex,
                                                 const float mostPositiveValueInclusive,
                                                 const float leastPositiveValueInclusive,
                                                 const float leastNegativeValueInclusive,
                                                 const float mostNegativeValueInclusive,
                                                 const bool includeZeroValues) override;
        
        virtual const FastStatistics* getFileFastStatistics() override;
        
        virtual const Histogram* getFileHistogram() override;
        
        virtual const Histogram* getFileHistogram(const float mostPositiveValueInclusive,
                                                  const float leastPositiveValueInclusive,
                                                  const float leastNegativeValueInclusive,
                                                  const float mostNegativeValueInclusive,
                                                  const bool includeZeroValues) override;
        
        virtual PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) override;
        
        virtual const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const override;
        
        virtual bool isMappedWithLabelTable() const override;
        
        virtual GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) override;
        
        virtual const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const override;
        
        virtual void getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const override;
        
        virtual void updateScalarColoringForMap(const int32_t mapIndex) override;
        
        virtual void getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                                        std::vector<float>& dataOut) const override;
        
        virtual BrainordinateMappingMatch getBrainordinateMappingMatchImplementation(const CaretMappableDataFile* mapFile) const override;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) override;
        
        virtual std::vector<AString> getChildDataFilePathNames() const override;

        // ADD_NEW_METHODS_HERE

    private:
        void getMapData(const int32_t index,
                        std::vector<float>& dataOut) const;
        
        std::unique_ptr<DataSliceArray> getDataSliceArrayForMap(const int32_t mapIndex) const;
        
        std::array<int64_t, 3> getVolumeFilesMatchingDimension() const;
        
        std::unique_ptr<MapInfo> m_fileMapInfo;
        
        std::vector<std::unique_ptr<VolumeFile>> m_volumeFiles;
        
        std::vector<std::unique_ptr<MapInfo>> m_mapInfo;
        
        /** Map index for ALL file data (dfata from ALL maps) */
        static const int32_t s_ALL_FILE_DATA_MAP_INDEX;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_VOLUME_FILE_DECLARE__
    const int32_t MetaVolumeFile::s_ALL_FILE_DATA_MAP_INDEX = -1;
#endif // __META_VOLUME_FILE_DECLARE__

} // namespace
#endif  //__META_VOLUME_FILE_H__
