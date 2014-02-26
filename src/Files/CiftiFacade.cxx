
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

#define __CIFTI_FACADE_DECLARE__
#include "CiftiFacade.h"
#undef __CIFTI_FACADE_DECLARE__

#include "CaretAssert.h"
#include "CiftiInterface.h"
#include "CaretLogger.h"
#include "GiftiMetaData.h"
using namespace caret;


    
/**
 * \class caret::CiftiFacade 
 * \brief A simpler interface (facade pattern) for CIFTI Interface access.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param ciftiInterface
 *    The CIFTI Interface.
 */
CiftiFacade::CiftiFacade(const DataFileTypeEnum::Enum dataFileType,
                         CiftiInterface* ciftiInterface)
: CaretObject(),
m_dataFileType(dataFileType),
m_ciftiInterface(ciftiInterface),
m_ciftiFileType(CIFTI_INVALID),
m_validCiftiFile(false),
m_volumeMappingValid(false),
m_containsSurfaceDataForMappingToBrainordinates(false)
{
    CaretAssert(ciftiInterface);
    
    switch (dataFileType) {
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            m_ciftiFileType = CIFTI_DENSE;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            m_ciftiFileType = CIFTI_LABEL;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            m_ciftiFileType = CIFTI_DENSE_PARCEL;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            m_ciftiFileType = CIFTI_SCALAR;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            m_ciftiFileType = CIFTI_DATA_SERIES;
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            m_ciftiFileType = CIFTI_PARCEL;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            m_ciftiFileType = CIFTI_PARCEL_DENSE;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            m_ciftiFileType = CIFTI_PARCEL_SCALAR;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            m_ciftiFileType = CIFTI_PARCEL_SERIES;
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::UNKNOWN:
            break;
    }
    
    m_mapIntervalUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
    m_mapIntervalStartValue = 1.0;
    m_mapIntervalStepValue  = 1.0;
    
    m_numberOfColumns = m_ciftiInterface->getNumberOfColumns();
    m_numberOfRows    = m_ciftiInterface->getNumberOfRows();
    m_numberOfMaps    = 0;
    
//    m_connectivityMatrixFileFlag = false;
    m_useColumnMapsForBrainordinateMapping = false;
    m_useRowMapsForBrainordinateMapping = false;
    m_useAlongRowMethodsForMapAttributes = false;
    m_loadBrainordinateDataFromColumns = false;
    m_loadBrainordinateDataFromRows = false;
    m_brainordinateDataColoredWithPalette = false;
    m_brainordinateDataColoredWithLabelTable = false;
    m_useParcelsForBrainordinateMapping = false;
    m_containsMapAttributes = false;
    
    switch (m_ciftiFileType) {
        case CIFTI_INVALID:
            break;
        case CIFTI_DENSE:
//            m_connectivityMatrixFileFlag = true;
            m_useRowMapsForBrainordinateMapping = true;
            m_numberOfMaps = 1;
            m_loadBrainordinateDataFromRows = true;
            m_brainordinateDataColoredWithPalette = true;
            break;
        case CIFTI_LABEL:
            m_useColumnMapsForBrainordinateMapping = true;
            m_useAlongRowMethodsForMapAttributes = true;
            m_loadBrainordinateDataFromColumns = true;
            m_numberOfMaps = m_numberOfColumns;
            m_brainordinateDataColoredWithLabelTable = true;
            m_containsMapAttributes = true;
            break;
        case CIFTI_SCALAR:
            m_useColumnMapsForBrainordinateMapping = true;
            m_useAlongRowMethodsForMapAttributes = true;
            m_loadBrainordinateDataFromColumns = true;
            m_numberOfMaps = m_numberOfColumns;
            m_brainordinateDataColoredWithPalette = true;
            m_containsMapAttributes = true;
            break;
        case CIFTI_DENSE_PARCEL:
//            m_connectivityMatrixFileFlag = true;
            m_useRowMapsForBrainordinateMapping = true;
            m_numberOfMaps = 1;
            m_loadBrainordinateDataFromRows = true;
            m_brainordinateDataColoredWithPalette = true;
            m_useParcelsForBrainordinateMapping = true;
            break;
        case CIFTI_DATA_SERIES:
            m_useColumnMapsForBrainordinateMapping = true;
            m_useAlongRowMethodsForMapAttributes = true;
            m_loadBrainordinateDataFromColumns = true;
            m_numberOfMaps = m_numberOfColumns;
            m_brainordinateDataColoredWithPalette = true;
            break;
        case CIFTI_PARCEL:
//            m_connectivityMatrixFileFlag = true;
            m_useRowMapsForBrainordinateMapping = true;
            m_numberOfMaps = 1;
            m_loadBrainordinateDataFromRows = true;
            m_brainordinateDataColoredWithPalette = true;
            m_useParcelsForBrainordinateMapping = true;
            break;
        case CIFTI_PARCEL_DENSE:
//            m_connectivityMatrixFileFlag = true;
            m_useRowMapsForBrainordinateMapping = true;
            m_numberOfMaps = 1;
            m_loadBrainordinateDataFromRows = true;
            m_brainordinateDataColoredWithPalette = true;
            break;
        case CIFTI_PARCEL_SCALAR:
            m_useColumnMapsForBrainordinateMapping = true;
            m_useAlongRowMethodsForMapAttributes = true;
            m_loadBrainordinateDataFromColumns = true;
            m_numberOfMaps = m_numberOfColumns;
            m_brainordinateDataColoredWithPalette = true;
            m_useParcelsForBrainordinateMapping = true;
            m_containsMapAttributes = true;
            break;
        case CIFTI_PARCEL_SERIES:
            m_useColumnMapsForBrainordinateMapping = true;
            m_useAlongRowMethodsForMapAttributes = true;
            m_loadBrainordinateDataFromColumns = true;
            m_numberOfMaps = m_numberOfColumns;
            m_brainordinateDataColoredWithPalette = true;
            m_useParcelsForBrainordinateMapping = true;
            //m_containsMapAttributes = true;
            break;
    }
    
    CaretAssertMessage((m_ciftiFileType != CIFTI_INVALID),
                       ("Support for "
                        + DataFileTypeEnum::toName(m_dataFileType)
                        + " needs to be implemented or is invalid type."));
    
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    if (m_ciftiFileType != CIFTI_INVALID) {
        m_validCiftiFile = true;
        
        if (ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == caret::CiftiMappingType::SERIES)
        {
            const CiftiSeriesMap& mySeriesMap = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW);
            m_mapIntervalStartValue = mySeriesMap.getStart();//NOTE: CiftiXML no longer tracks if the XML contained a start value, and has default 0
            m_mapIntervalStepValue = mySeriesMap.getStep();
            m_mapIntervalUnits = NiftiTimeUnitsEnum::NIFTI_UNITS_SEC;//TODO: get the units (which isn't a nifti enum), rather than assuming time
        }
        
    }

    /*
     * Check for surface node mappings
     */
    m_containsSurfaceDataForMappingToBrainordinates = false;
    std::vector<StructureEnum::Enum> allStructures;
    StructureEnum::getAllEnums(allStructures);
    for (std::vector<StructureEnum::Enum>::iterator iter = allStructures.begin();
         iter != allStructures.end();
         iter++) {
        if (m_useColumnMapsForBrainordinateMapping) {
            if (m_ciftiInterface->hasColumnSurfaceData(*iter)) {
                m_containsSurfaceDataForMappingToBrainordinates = true;
                break;
            }
        }
        else if (m_useRowMapsForBrainordinateMapping) {
            if (m_ciftiInterface->hasRowSurfaceData(*iter)) {
                m_containsSurfaceDataForMappingToBrainordinates = true;
                break;
            }
        }
    }
  
    /*
     * Cache voxel mapping
     */
    m_volumeMapping.clear();
    
    m_volumeMappingValid = false;
    if (m_useColumnMapsForBrainordinateMapping) {
        m_volumeMappingValid = m_ciftiInterface->getVolumeMapForColumns(m_volumeMapping);
    }
    else if (m_useRowMapsForBrainordinateMapping) {
        m_volumeMappingValid = m_ciftiInterface->getVolumeMapForRows(m_volumeMapping);
    }
    else {
        CaretAssert(0);
    }
    
    CaretAssertMessage(m_containsSurfaceDataForMappingToBrainordinates
                       || m_volumeMappingValid,
                       "Contains NO data for mapping to brainordinates");
}

/**
 * Destructor.
 */
CiftiFacade::~CiftiFacade()
{
    m_mapsOfDataIndicesForSurfaceNodes.clear();
}

/**
 * @return The CIFTI file type.
 */
CiftiFacade::CiftiFileType
CiftiFacade::getCiftiFileType() const
{
    return m_ciftiFileType;
}

/**
 * @return True if the file is a valid CIFTI file, else false.
 */
bool
CiftiFacade::isValidCiftiFile() const
{
    return m_validCiftiFile;
}


/**
 * @return The number of rows.
 */
int32_t
CiftiFacade::getNumberOfRows() const
{
    return m_numberOfRows;
}

/**
 * @return The number of columns.
 */
int32_t
CiftiFacade::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

/**
 * @return The number of maps.
 */
int32_t
CiftiFacade::getNumberOfMaps() const
{
    return m_numberOfMaps;
}

/**
 * Return the number of elements contained in ONE map.
 */
int32_t
CiftiFacade::getMapDataCount() const
{
    int32_t count = 0;
    
    if (m_loadBrainordinateDataFromColumns) {
        count = m_numberOfRows;
    }
    else if (m_loadBrainordinateDataFromRows) {
        count = m_numberOfColumns;
    }
    else {
        CaretAssert(0);
    }
    
    return count;
}


///**
// * @return True if the interface is for a connectivity matrix file,
// *         else false.
// */
//bool
//CiftiFacade::isConnectivityMatrixFile() const
//{
//    return m_connectivityMatrixFileFlag;
//}

/**
 * Get the metadata for the file.
 *
 * @param metadataOut
 *    Is cleared and metadata for the map in placed into this.
 */
void
CiftiFacade::getFileMetadata(GiftiMetaData* metadataOut)
{
    CaretAssert(metadataOut);
    metadataOut->clear();

    *metadataOut = *(m_ciftiInterface->getCiftiXML().getFileMetaData());//NOTE: could pass through instead, if modification is desired outside class
}

void
CiftiFacade::setFileMetadata(GiftiMetaData* metadataIn)
{
    CaretAssert(metadataIn);
    
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    *(ciftiXML.getFileMetaData()) = *metadataIn;
}

/**
 * Get the mapping for the surface with the given structure for mapping
 * to brainordinates such as surface coloring or obtaining data values.
 *
 * @param mappingOut
 *    Output surface mapping.
 * @param structure
 *    Structure for the surface.
 * @return
 *    True if mapping is valid, else false.
 */
bool
CiftiFacade::getSurfaceMapForMappingDataToBrainordinates(std::vector<CiftiBrainModelsMap::SurfaceMap>& mappingOut,
                                                         const StructureEnum::Enum structure) const
{
    mappingOut.clear();
    
    bool validFlag = false;
    if (m_useColumnMapsForBrainordinateMapping) {
        validFlag = m_ciftiInterface->getSurfaceMapForColumns(mappingOut,
                                                              structure);
    }
    else if (m_useRowMapsForBrainordinateMapping) {
        validFlag = m_ciftiInterface->getSurfaceMapForRows(mappingOut,
                                                           structure);
    }
    else {
        CaretAssert(0);
    }
    
    return validFlag;
}

/** 
 * Get the mapping for voxels  for mapping to brainordinates such as 
 * voxel coloring or obtaining data values.
 *
 * @param mappingOut
 *    Output surface mapping.
 * @param structure
 *    Structure for the surface.
 * @return
 *    True if mapping is valid, else false.
 */
const std::vector<CiftiBrainModelsMap::VolumeMap>*
CiftiFacade::getVolumeMapForMappingDataToBrainordinates() const
{
    if (m_volumeMappingValid) {
        return &m_volumeMapping;
    }
    
    return NULL;
}

///**
// * Get the parcels for mapping to brainordinates.
// *
// * @param parcelsOut
// *    Output containing parcels.
// * @return True if parcels are valid, else false.
// */
//bool
//CiftiFacade::getParcelMapForMappingToBrainordinates(std::vector<CiftiParcelElement>& parcelsOut) const
//{
//    bool validFlag = false;
//    
//    if (m_useColumnMapsForBrainordinateMapping) {
//        validFlag = m_ciftiInterface->getCiftiXML().getParcelsForColumns(parcelsOut);
//    }
//    else if (m_useRowMapsForBrainordinateMapping) {
//        validFlag = m_ciftiInterface->getCiftiXML().getParcelsForRows(parcelsOut);
//    }
//    
//    return validFlag;
//}

///**
// * Get the Parcel Nodes Element for the surface with the given structure.
// */
//bool
//CiftiFacade::getParcelSurfaceMapForMappingToBrainordinates(CiftiParcelNodesElement& nodeParcelsOut,
//                                                   const StructureEnum::Enum structure) const
//{
//    nodeParcelsOut.m_structure = StructureEnum::StructureEnum::INVALID;
//    nodeParcelsOut.m_nodes.clear();
//    
//    std::vector<CiftiParcelElement> parcels;
//    getParcelMapForMappingToBrainordinates(parcels);
//    
//    /*
//     * Match to structure.
//     */
//    for (std::vector<CiftiParcelElement>::iterator iter = parcels.begin();
//         iter != parcels.end();
//         iter++) {
//        CiftiParcelElement& cpe = *iter;
//        std::vector<CiftiParcelNodesElement> parcelNodes = cpe.m_nodeElements;
//        for (std::vector<CiftiParcelNodesElement>::iterator nodesIter = parcelNodes.begin();
//             nodesIter != parcelNodes.end();
//             nodesIter++) {
//            CiftiParcelNodesElement& cpne = *nodesIter;
//            if (cpne.m_structure == structure) {
//                nodeParcelsOut = cpne;
//            }
//            if (nodeParcelsOut.m_nodes.empty() == false) {
//                return true;
//            }
//        }
//    }
//    return false;
//}

/**
 * @return True if brainordinate mapping uses parcels.
 */
bool
CiftiFacade::isMappingDataToBrainordinateParcels() const
{
    return m_useParcelsForBrainordinateMapping;
}

bool
CiftiFacade::getParcelElementForSelectedParcel(CiftiParcelsMap::Parcel &parcelOut, const StructureEnum::Enum &structure, const AString &parcelName) const
{    
    if (m_ciftiInterface->getCiftiXML().getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::PARCELS) return false;
    const std::vector<CiftiParcelsMap::Parcel>& parcels = m_ciftiInterface->getCiftiXML().getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();
    
    for(std::vector<CiftiParcelsMap::Parcel>::const_iterator iter = parcels.begin();iter != parcels.end();iter++)
    {
        const CiftiParcelsMap::Parcel &cpe = *iter;
        if(cpe.m_name == parcelName)
        {
            std::map<StructureEnum::Enum, std::set<int64_t> >::const_iterator findStruct = cpe.m_surfaceNodes.find(structure);
            if (findStruct != cpe.m_surfaceNodes.end())
            {
                parcelOut = cpe;
                return true;
            }
        }
    }
    return false;
}

bool CiftiFacade::getParcelElementForSelectedParcel(CiftiParcelsMap::Parcel &parcelOut, const StructureEnum::Enum &structure, const int64_t &selectionIndex) const
{    
    if (m_ciftiInterface->getCiftiXML().getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::PARCELS) return false;
    const std::vector<CiftiParcelsMap::Parcel>& parcels = m_ciftiInterface->getCiftiXML().getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();

    if(m_ciftiInterface->checkColumnIndex(selectionIndex))
    {
        parcelOut = parcels[selectionIndex];

        std::map<StructureEnum::Enum, std::set<int64_t> >::const_iterator findStruct = parcelOut.m_surfaceNodes.find(structure);
        if (findStruct != parcelOut.m_surfaceNodes.end())
        {
            return true;
        }
    }
    return false;
}


bool CiftiFacade::getParcelNodesElementForSelectedParcel(std::set<int64_t> &parcelNodesOut, const StructureEnum::Enum &structure, const int64_t &selectionIndex) const
{
    if (m_ciftiInterface->getCiftiXML().getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::PARCELS) return false;
    const std::vector<CiftiParcelsMap::Parcel>& parcels = m_ciftiInterface->getCiftiXML().getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();

    if(m_ciftiInterface->checkColumnIndex(selectionIndex))
    {
        const CiftiParcelsMap::Parcel& parcelOut = parcels[selectionIndex];
        std::map<StructureEnum::Enum, std::set<int64_t> >::const_iterator findStruct = parcelOut.m_surfaceNodes.find(structure);
        if (findStruct != parcelOut.m_surfaceNodes.end())
        {
            parcelNodesOut = findStruct->second;
            return true;
        }            
    }
    return false;
}
                                              

/**
 * Get the indices into the data for the surface nodes of the given structure.
 * Works for brainordinates mapped a surfaces or parcels.
 *
 * @param structure
 *    Surface's structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @return 
 *    Upon exit, a vector that contains 'surfaceNumberOfNodes' elements
 *    where the vector is indexed by the node index and the value at the
 *    index is an offset into the data for the node or a negative value 
 *    if there is no corresponding data for the node index.
 *    NULL is returned if there is no mapping for the structure.
 */
const std::vector<int64_t>* 
CiftiFacade::getSurfaceDataIndicesForMappingToBrainordinates(const StructureEnum::Enum structure,
                                                             const int64_t surfaceNumberOfNodes) const
{
    if (surfaceNumberOfNodes <= 0) {
        return NULL;
    }
    
    /*
     * Data is cached internally since the CIFTI mappings do not change
     * and obtaining the mappings is costly.  See if data has been
     * cached for the given structure.
     */
    std::map<StructureEnum::Enum, std::vector<int64_t> >::iterator structureIter =
        m_mapsOfDataIndicesForSurfaceNodes.find(structure);
    if (structureIter != m_mapsOfDataIndicesForSurfaceNodes.end()) {
        /*
         * Data indices are available, so simply return them
         */
        const std::vector<int64_t>* dataIndices = &structureIter->second;
        const int64_t numNodes = static_cast<int64_t>(dataIndices->size());
        
        if (numNodes == surfaceNumberOfNodes) {
            return dataIndices;
        }
        else {
            /*
             * Number of node has changed?  Should not happen
             * but remove structure from cached data and let
             * the mappings be recreated.
             */
            m_mapsOfDataIndicesForSurfaceNodes.erase(structureIter);
        }
    }
    
    std::vector<int64_t> dataIndicesForNodes(surfaceNumberOfNodes,
                                             -1);
    
    const CiftiXML& myXML = m_ciftiInterface->getCiftiXML();
    if (m_useParcelsForBrainordinateMapping) {
        int myDir = -1;
        if (m_useColumnMapsForBrainordinateMapping) {
            myDir = CiftiXML::ALONG_COLUMN;
        } else if (m_useRowMapsForBrainordinateMapping) {
            myDir = CiftiXML::ALONG_ROW;
        } else {
            CaretAssert(0);
        }
        const CiftiParcelsMap& myParcelMap = myXML.getParcelsMap(myDir);
        for (int64_t i = 0; i < surfaceNumberOfNodes; i++) {
            dataIndicesForNodes[i] = myParcelMap.getIndexForNode(i, structure);
        }
    } else {
        std::vector<CiftiBrainModelsMap::SurfaceMap> surfaceMaps;
        if (getSurfaceMapForMappingDataToBrainordinates(surfaceMaps,
                                                        structure)) {
            const int64_t numSurfaceMaps = static_cast<int64_t>(surfaceMaps.size());
            for (int64_t i = 0; i < numSurfaceMaps; i++) {
                const CiftiBrainModelsMap::SurfaceMap& csm = surfaceMaps[i];
                CaretAssert((csm.m_surfaceNode >= 0)
                            && (csm.m_surfaceNode < surfaceNumberOfNodes));
                dataIndicesForNodes[csm.m_surfaceNode] = csm.m_ciftiIndex;
            }
        }
    }
    
    /*
     * Insert returns a pair containing
     *  1) iterator pointing to inserted element
     *  2) bool with true indicating new element created or false if the key
     *     alread existed.
     */
    std::pair<std::map<StructureEnum::Enum, std::vector<int64_t> >::iterator, bool> insertInfo =
    m_mapsOfDataIndicesForSurfaceNodes.insert(std::make_pair(structure,
                                                             dataIndicesForNodes));
    CaretAssert(insertInfo.second);
    
    /*
     * Return pointer to the vector containing the node indices
     */
    return &insertInfo.first->second;
}

/**
 * @return True if there is data for mapping to any surface.
 */
bool
CiftiFacade::containsSurfaceDataForMappingToBrainordinates() const
{
    return m_containsSurfaceDataForMappingToBrainordinates;
}

/**
 * @return True if there is data for mapping to any volume.
 */
bool
CiftiFacade::containsVolumeDataForMappingToBrainordinates() const
{
    return m_volumeMappingValid;
    
//    std::vector<CiftiVolumeMap> mapping;
//    getVolumeMapForMappingDataToBrainordinates(mapping);
//    
//    if (mapping.empty()) {
//        return false;
//    }
//    
//    /*
//     * Check for parcel node mappings.
//     */
//    std::vector<CiftiParcelElement> parcels;
//    getParcelMapForMappingToBrainordinates(parcels);
//    for (std::vector<CiftiParcelElement>::iterator iter = parcels.begin();
//         iter != parcels.end();
//         iter++) {
//        const CiftiParcelElement& cpe = *iter;
//        
//        if (cpe.m_voxelIndicesIJK.empty() == false) {
//            return true;
//        }
//    }
//    
//    return true;
}

/**
 * Get the metadata for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param metadataOut
 *    Is cleared and metadata for the map in placed into this.
 * @return
 *    True if the map index is valid (even if there is no metadata).
 */
bool
CiftiFacade::getMetadataForMapOrSeriesIndex(const int32_t mapIndex,
                                    GiftiMetaData* metadataOut)
{
    CaretAssert((mapIndex >= 0) && (mapIndex < m_numberOfMaps));
    CaretAssert(metadataOut);
    metadataOut->clear();
    
    if (m_containsMapAttributes == false) {
        const AString msg("Requesting map metadata for file that does not have map attributes.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return true;
    } else if (m_useAlongRowMethodsForMapAttributes) {
        const CiftiXML& myXML = m_ciftiInterface->getCiftiXML();
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS)
        {
            *(metadataOut) = *(myXML.getScalarsMap(CiftiXML::ALONG_ROW).getMapMetadata(mapIndex));//NOTE: could pass the pointer through to modify outside class
            return true;
        } else if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS) {
            *(metadataOut) = *(myXML.getLabelsMap(CiftiXML::ALONG_ROW).getMapMetadata(mapIndex));//ditto
            return true;
        } else {
            return false;
        }
    } else {
        CaretAssert(0);
    }
    
    return false;
}

/**
 * Set the metadata for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param metadataOut
 *    Metadata that is placed using the given map or series index.
 */
void
CiftiFacade::setMetadataForMapOrSeriesIndex(const int32_t mapIndex,
                                    GiftiMetaData* metadataIn)
{
    CaretAssert((mapIndex >= 0) && (mapIndex < m_numberOfMaps));
    CaretAssert(metadataIn);
    
    if (m_containsMapAttributes == false) {
        const AString msg("Setting map metadata for file that does not have map attributes.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
    else if (m_useAlongRowMethodsForMapAttributes) {
        const CiftiXML& myXML = m_ciftiInterface->getCiftiXML();
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS)
        {
            *(myXML.getScalarsMap(CiftiXML::ALONG_ROW).getMapMetadata(mapIndex)) = *(metadataIn);
        } else if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS) {
            *(myXML.getLabelsMap(CiftiXML::ALONG_ROW).getMapMetadata(mapIndex)) = *(metadataIn);
        }
    }
    else {
        CaretAssert(0);
    }
}

/**
 * Get the label table for the given map or series index.
 * 
 * @param mapIndex
 *    Index of map or series.
 * @return
 *    Pointer to label table or NULL if label table not valid for 
 *    the given map or series index.
 */
GiftiLabelTable*
CiftiFacade::getLabelTableForMapOrSeriesIndex(const int32_t mapIndex)
{
    CaretAssert((mapIndex >= 0) && (mapIndex < m_numberOfMaps));
    GiftiLabelTable* labelTable = NULL;
    
    if (m_useAlongRowMethodsForMapAttributes) {
        const CiftiXML& myXML = m_ciftiInterface->getCiftiXML();
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS)
        {
            labelTable = myXML.getLabelsMap(CiftiXML::ALONG_ROW).getMapLabelTable(mapIndex);
        }
    }
    else {
        /* Not all files have label table. */
    }
    
    return labelTable;
}

/**
 * Get the palette color mapping for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @return
 *    Pointer to palette color mapping or NULL if palette color mapping
 *    not valid for the given map or series index.
 */
PaletteColorMapping*
CiftiFacade::getPaletteColorMappingForMapOrSeriesIndex(const int32_t mapIndex)
{
    CaretAssert((mapIndex >= 0) && (mapIndex < m_numberOfMaps));
    PaletteColorMapping* pcm = NULL;
    const CiftiXML& myXML = m_ciftiInterface->getCiftiXML();
    if (m_containsMapAttributes == false) {
        /*
         * FILE PALETTE APPLIES TO ALL MAPS !!!!!
         */
        pcm = myXML.getFilePalette();
    }
    else if (m_useAlongRowMethodsForMapAttributes) {
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS)
        {
            pcm = myXML.getScalarsMap(CiftiXML::ALONG_ROW).getMapPalette(mapIndex);
        }
    }
    else {
        /* Not all files have palette mapping. */
    }
    
    return pcm;
}

/**
 * Get the name for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @return
 *    Name for given map or series index.  
 */
AString
CiftiFacade::getNameForMapOrSeriesIndex(const int32_t mapIndex) const
{
    CaretAssert((mapIndex >= 0) && (mapIndex < m_numberOfMaps));
    AString name;
    
    if (m_containsMapAttributes == false) {
        if ((m_ciftiFileType == CIFTI_DATA_SERIES)
            || (m_ciftiFileType == CIFTI_PARCEL_SERIES)) {
            /*
             * Data Series does not have map names
             * so construct map names from map interval
             * units when available.
             */
            AString unitsSuffix;
            switch (m_mapIntervalUnits) {
                case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                    unitsSuffix = " hertz";
                    break;
                case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                    unitsSuffix = " milliseconds";
                    break;
                case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                    unitsSuffix = " parts per million";
                    break;
                case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                    unitsSuffix = " seconds";
                    break;
                case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                    break;
                case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                    unitsSuffix = " microseconds";
                    break;
            }

            if (unitsSuffix.isEmpty()) {
                name = ("Map Index: "
                        + AString::number(mapIndex + 1));
            }
            else {
                const float value = (m_mapIntervalStartValue
                                     + (mapIndex * m_mapIntervalStepValue));
                name = (AString::number(value)
                        + unitsSuffix);
            }
            
            /*
             * For data series, name is index or 'unit' value if units valid
             */
            return name;
        }
        
        const AString msg("Getting map name for file that does not have map attributes.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
    else if (m_useAlongRowMethodsForMapAttributes) {
        const CiftiXML& myXML = m_ciftiInterface->getCiftiXML();
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS)
        {
            name = myXML.getScalarsMap(CiftiXML::ALONG_ROW).getMapName(mapIndex);
        } else if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS) {
            name = myXML.getLabelsMap(CiftiXML::ALONG_ROW).getMapName(mapIndex);
        }
    }
    else {
        CaretAssert(0);
    }
    
    return name;
}

/**
 * Set the name for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param name
 *    Name for the given map or series index.
 */
void
CiftiFacade::setNameForMapOrSeriesIndex(const int32_t mapIndex,
                                const AString name)
{
    CaretAssert((mapIndex >= 0) && (mapIndex < m_numberOfMaps));
    const CiftiXML& myXML = m_ciftiInterface->getCiftiXML();
    
    if (m_containsMapAttributes == false) {
        const AString msg("Setting map name for file that does not have map attributes.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
    else if (m_useAlongRowMethodsForMapAttributes) {
        if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS)
        {
            myXML.getScalarsMap(CiftiXML::ALONG_ROW).setMapName(mapIndex, name);
        } else if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS) {
            myXML.getLabelsMap(CiftiXML::ALONG_ROW).setMapName(mapIndex, name);
        }
    }
    else {
        CaretAssert(0);
    }
}

/**
 * Get the data for the given map or series index.
 *
 * @param mapIndex
 *    Index of map or series.
 * @param dataOut
 *    Data for given map or series is loaded into this.
 * @return
 *    True if output data is valid, else false.
 */
bool
CiftiFacade::getDataForMapOrSeriesIndex(const int32_t mapIndex,
                                        std::vector<float>& dataOut) const
{
    CaretAssert((mapIndex >= 0) && (mapIndex < m_numberOfMaps));
    if (m_loadBrainordinateDataFromColumns) {
        dataOut.resize(m_numberOfRows);
        m_ciftiInterface->getColumn(&dataOut[0],
                                    mapIndex);
    }
    else if (m_loadBrainordinateDataFromRows) {
        dataOut.resize(m_numberOfColumns);
        m_ciftiInterface->getRow(&dataOut[0],
                                 mapIndex);
    }
    else {
        CaretAssert(0);
    }
    
    return false;
}

/**
 * Get the series data (one data value from each map) for a surface node.
 * 
 * @param structure
 *     Surface's structure.
 * @param nodeIndex
 *     Index of the node.
 * @param seriesDataOut
 *     Series data for given node.
 * @return
 *     True if output data is valid, else false.
 */
bool
CiftiFacade::getSeriesDataForSurfaceNode(const StructureEnum::Enum structure,
                                         const int32_t nodeIndex,
                                         std::vector<float>& seriesDataOut) const
{
    const CiftiXML& ciftiXML = m_ciftiInterface->getCiftiXML();
    if (m_loadBrainordinateDataFromColumns) {
        if (m_useParcelsForBrainordinateMapping) {
            const int64_t parcelRowIndex = ciftiXML.getParcelsMap(CiftiXML::ALONG_COLUMN).getIndexForNode(nodeIndex,
                                                                                                          structure);
            if (parcelRowIndex >= 0) {
                seriesDataOut.resize(m_numberOfColumns);
                m_ciftiInterface->getRow(&seriesDataOut[0],
                                         parcelRowIndex);
                return true;
            }
        }
        else {
            const int64_t nodeRowIndex = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getIndexForNode(nodeIndex,
                                                                                                            structure);
            if (nodeRowIndex >= 0) {
                seriesDataOut.resize(m_numberOfColumns);
                m_ciftiInterface->getRow(&seriesDataOut[0],
                                         nodeRowIndex);
                return true;
            }
        }
    }
    else if (m_loadBrainordinateDataFromRows) {
        const int64_t nodeColumnIndex = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW).getIndexForNode(nodeIndex,
                                                                                                        structure);
        if (nodeColumnIndex >= 0) {
            seriesDataOut.resize(m_numberOfRows);
            m_ciftiInterface->getColumn(&seriesDataOut[0],
                                        nodeColumnIndex);
        }
    }
    else {
        CaretAssert(0);
    }
    
    return false;
}

/**
 * Get the series data (one data value from each map) for a voxel at
 * the given coordinate.
 *
 * @param xyz
 *     Coordinate of the voxel.
 * @param seriesDataOut
 *     Series data for given node.
 * @return
 *     True if output data is valid, else false.
 */
bool
CiftiFacade::getSeriesDataForVoxelAtCoordinate(const float xyz[3],
                                               std::vector<float>& seriesDataOut) const
{
    if (m_loadBrainordinateDataFromColumns) {
        seriesDataOut.resize(m_numberOfColumns);
        return m_ciftiInterface->getRowFromVoxelCoordinate(&seriesDataOut[0],
                                                           xyz);
    }
    else if (m_loadBrainordinateDataFromRows) {
        seriesDataOut.resize(m_numberOfRows);
        return m_ciftiInterface->getColumnFromVoxelCoordinate(&seriesDataOut[0],
                                                              xyz);
    }
    else {
        CaretAssert(0);
    }
    
    return false;
}

/**
 * Get the map units information.
 *
 * @param startValueOut
 *    The start value.
 * @param stepValueOut
 *    The step value.
 * @param unitsOut
 *    The unit of measurement.
 */
void
CiftiFacade::getMapIntervalStartStepAndUnits(float& startValueOut,
                                             float& stepValueOut,
                                             NiftiTimeUnitsEnum::Enum& unitsOut) const
{
    startValueOut = m_mapIntervalStartValue;
    stepValueOut  = m_mapIntervalStepValue;
    unitsOut      = m_mapIntervalUnits;
}

/**
 * @return Is the brainordinate data mapped with a palette?
 */
bool
CiftiFacade::isBrainordinateDataColoredWithPalette() const
{
    return m_brainordinateDataColoredWithPalette;
}

/**
 * @return Is the brainordinate data mapped with a label table?
 */
bool
CiftiFacade::isBrainordinateDataColoredWithLabelTable() const
{
    return m_brainordinateDataColoredWithLabelTable;
}

/**
 * @return True if the file contains map attributes (name and metadata).
 */
bool
CiftiFacade::containsMapAttributes() const
{
    return m_containsMapAttributes;
}



