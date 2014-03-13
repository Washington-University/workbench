#ifndef __CIFTI_FACADE_H__
#define __CIFTI_FACADE_H__

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


#include "CaretObject.h"
#include "CiftiXML.h"
#include "DataFileTypeEnum.h"
#include "NiftiEnums.h"

namespace caret {

    class CiftiInterface;
    class GiftiMetaData;
    
    class CiftiFacade : public CaretObject {
        
    public:
        enum CiftiFileType {
            CIFTI_INVALID,
            CIFTI_DENSE,
            CIFTI_LABEL,
            CIFTI_SCALAR,
            CIFTI_DENSE_PARCEL,
            CIFTI_DATA_SERIES,
            CIFTI_PARCEL,
            CIFTI_PARCEL_DENSE,
            CIFTI_PARCEL_SCALAR,
            CIFTI_PARCEL_SERIES
        };
        
        CiftiFacade(const DataFileTypeEnum::Enum dataFileType,
                    CiftiInterface* ciftiInterface);
        
        virtual ~CiftiFacade();
        
        bool isValidCiftiFile() const;
        
        int32_t getNumberOfRows() const;
        
        int32_t getNumberOfColumns() const;
        
        int32_t getNumberOfMaps() const;
        
        int32_t getMapDataCount() const;
        
        void getFileMetadata(GiftiMetaData* metadataOut);
        
        void setFileMetadata(GiftiMetaData* metadataIn);
        
        bool isMappingDataToBrainordinateParcels() const;
        
        const std::vector<int64_t>* getSurfaceDataIndicesForMappingToBrainordinates(const StructureEnum::Enum structure,
                                                                                    const int64_t surfaceNumberOfNodes) const;

        bool getParcelElementForSelectedParcel(CiftiParcelsMap::Parcel &parcelOut, const StructureEnum::Enum &structure, const AString &parcelName) const;

        bool getParcelElementForSelectedParcel(CiftiParcelsMap::Parcel &parcelOut, const StructureEnum::Enum &structure, const int64_t &selectionIndex) const;

        bool getParcelNodesElementForSelectedParcel(std::set<int64_t> &parcelNodesOut, const StructureEnum::Enum &structure, const int64_t &selectionIndex) const;

        const std::vector<CiftiBrainModelsMap::VolumeMap>* getVolumeMapForMappingDataToBrainordinates() const;
        
        
//        bool getParcelMapForMappingToBrainordinates(std::vector<CiftiParcelElement>& parcelsOut) const;
        
//        bool getParcelSurfaceMapForMappingToBrainordinates(CiftiParcelNodesElement& nodeParcelsOut,
//                                                           const StructureEnum::Enum structure) const;
        
        bool getMetadataForMapOrSeriesIndex(const int32_t mapIndex,
                                            GiftiMetaData* metadataOut);
        
        void setMetadataForMapOrSeriesIndex(const int32_t mapIndex,
                                            GiftiMetaData* metadataIn);
        
        GiftiLabelTable* getLabelTableForMapOrSeriesIndex(const int32_t mapIndex);
        
        PaletteColorMapping* getPaletteColorMappingForMapOrSeriesIndex(const int32_t mapIndex);
        
        AString getNameForMapOrSeriesIndex(const int32_t mapIndex) const;
        
        void setNameForMapOrSeriesIndex(const int32_t mapIndex,
                                        const AString name);
                                        
        bool getDataForMapOrSeriesIndex(const int32_t mapIndex,
                                        std::vector<float>& dataOut) const;
        
        bool getSeriesDataForSurfaceNode(const StructureEnum::Enum structure,
                                         const int32_t nodeIndex,
                                         std::vector<float>& seriesDataOut) const;
        
        bool getSeriesDataForVoxelAtCoordinate(const float xyz[3],
                                               std::vector<float>& seriesDataOut) const;
        
        bool containsSurfaceDataForMappingToBrainordinates() const;
        
        bool containsVolumeDataForMappingToBrainordinates() const;
        
//        bool isConnectivityMatrixFile() const;
        
        bool containsMapAttributes() const;
        
        bool isBrainordinateDataColoredWithPalette() const;
        
        bool isBrainordinateDataColoredWithLabelTable() const;
        
        void getMapIntervalStartStepAndUnits(float& startValueOut,
                                             float& stepValueOut,
                                             NiftiTimeUnitsEnum::Enum& unitsOut) const;
        
        CiftiFileType getCiftiFileType() const;
        
        // ADD_NEW_METHODS_HERE
        
    private:
        CiftiFacade(const CiftiFacade&);

        CiftiFacade& operator=(const CiftiFacade&);
        
        bool getSurfaceMapForMappingDataToBrainordinates(std::vector<CiftiBrainModelsMap::SurfaceMap>& mappingOut,
                                                         const StructureEnum::Enum structure) const;
        
        const DataFileTypeEnum::Enum m_dataFileType;
        
        CiftiInterface* m_ciftiInterface;
        
        CiftiFileType m_ciftiFileType;
        
        bool m_validCiftiFile;
        
        int32_t m_numberOfRows;
        
        int32_t m_numberOfColumns;
        
        int32_t m_numberOfMaps;
        
        NiftiTimeUnitsEnum::Enum m_mapIntervalUnits;
        
        float m_mapIntervalStartValue;
        
        float m_mapIntervalStepValue;
        
        /**
         * For each structure, the vector contains indices into the data for
         * each node in the surface.  If the value is negative, there is no data
         * the node.  Since CIFTI mappings do not change, caching this information
         * saves time.
         */
        mutable std::map<StructureEnum::Enum, std::vector<int64_t> > m_mapsOfDataIndicesForSurfaceNodes;

        mutable std::map<StructureEnum::Enum, std::vector<int64_t> > m_mapsOfParcelIndicesForDataIndices;
        
        /**
         * Cache volume mapping since CIFTI mappings do not change
         * to save time.
         */
        std::vector<CiftiBrainModelsMap::VolumeMap> m_volumeMapping;
        bool m_volumeMappingValid;
        
//        bool m_connectivityMatrixFileFlag;
        
        bool m_useColumnMapsForBrainordinateMapping;

        bool m_useRowMapsForBrainordinateMapping;
        
        bool m_useAlongRowMethodsForMapAttributes;
        
        bool m_loadBrainordinateDataFromColumns;
        
        bool m_loadBrainordinateDataFromRows;
        
        bool m_brainordinateDataColoredWithPalette;
        
        bool m_brainordinateDataColoredWithLabelTable;
        
        bool m_useParcelsForBrainordinateMapping;
        
        bool m_containsMapAttributes;
        
        bool m_containsSurfaceDataForMappingToBrainordinates;
        
    };
    
#ifdef __CIFTI_FACADE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_FACADE_DECLARE__

} // namespace
#endif  //__CIFTI_FACADE_H__
