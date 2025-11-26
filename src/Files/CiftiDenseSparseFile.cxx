
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

#include <map>
#include <set>

#include <QDir>

#define __CIFTI_DENSE_SPARSE_FILE_DECLARE__
#include "CiftiDenseSparseFile.h"
#undef __CIFTI_DENSE_SPARSE_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretSparseFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiBrainModelsMap.h"
#include "CiftiBrainordinateScalarFile.h"
#include "ConnectivityDataLoaded.h"
#include "DataFileContentInformation.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "FileInformation.h"
#include "FastStatistics.h"
#include "GiftiMetaData.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiDenseSparseFile 
 * \brief File that contains sparse representation of dense file
 */

/**
 * Constructor.
 */
CiftiDenseSparseFile::CiftiDenseSparseFile()
: CaretMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_SPARSE)
{
    m_connectivityDataLoaded.reset(new ConnectivityDataLoaded());
    m_fileMetadata.reset(new GiftiMetaData());
    m_sparseFile.reset();
    m_dataLoadingEnabled = true;
    
    m_mapMetadata.reset(new GiftiMetaData());
    
    m_mapPaletteColorMapping.reset(new PaletteColorMapping());
    
    m_mapHistogram.reset(new Histogram());
    
    m_mapHistogramLimitedRange.reset(new Histogram());
    
    m_mapFastStatistics.reset(new FastStatistics());

    m_sceneAssistant.reset(new SceneClassAssistant());
    m_sceneAssistant->add("m_dataLoadingEnabled",
                          &m_dataLoadingEnabled);
    m_sceneAssistant->add("m_connectivityDataLoaded",
                          "ConnectivityDataLoaded",
                          m_connectivityDataLoaded.get());
}

/**
 * Destructor.
 */
CiftiDenseSparseFile::~CiftiDenseSparseFile()
{
    clearPrivate();
}

/**
 * Cleare data in this file.
 */
void
CiftiDenseSparseFile::clear()
{
    CaretMappableDataFile::clear();
    
    clearPrivate();
}


/**
 * Cleare data in this file but not the parent class.
 */
void
CiftiDenseSparseFile::clearPrivate()
{
    m_fileMetadata->clear();
    m_sparseFile.reset();
    m_fileNumberOfRows = 0;
    m_fileNumberOfColumns = 0;
    m_loadedDataDescriptionForFileCopy.clear();
    m_loadedDataDescriptionForMapName.clear();
    m_rgbaValidFlag = false;
}


/**
 * @return True if the file is empty.
 */
bool
CiftiDenseSparseFile::isEmpty() const
{
    return ( ! m_sparseFile);
}

/**
 * @return Is data loading enabled?
 * @param mapIndex
 *    Index of map
 */
bool
CiftiDenseSparseFile::isMapDataLoadingEnabled(const int32_t /*mapIndex*/) const
{
    return m_dataLoadingEnabled;
}

/**
 * Set data loading enabled.
 *
 * @param mapIndex
 *    Index of map
 * @param loadingEnabled
 *    New status of data loading.
 */
void
CiftiDenseSparseFile::setMapDataLoadingEnabled(const int32_t /*mapIndex*/,
                                               const bool enabled)
{
    m_dataLoadingEnabled = enabled;
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
CiftiDenseSparseFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
CiftiDenseSparseFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
CiftiDenseSparseFile::getFileMetaData()
{
    return m_fileMetadata.get();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
CiftiDenseSparseFile::getFileMetaData() const
{
    return m_fileMetadata.get();
}

/**
 * @return Is the data mappable to a surface?
 */
bool
CiftiDenseSparseFile::isSurfaceMappable() const
{
    return true;
}

/**
 * @return Is the data mappable to a volume?
 */
bool
CiftiDenseSparseFile::isVolumeMappable() const
{
    return true;
}

/**
 * @return The number of maps in the file.
 * Note: Caret5 used the term 'columns'.
 */
int32_t
CiftiDenseSparseFile::getNumberOfMaps() const
{
    /*
     * Always return 1.
     * If zero is returned, it will never appear in the overlays because
     * zero is interpreted as "nothing available".
     */
    return 1;
}

void
CiftiDenseSparseFile::invalidateColoringInAllMaps()
{
    m_rgbaValidFlag = false;
}

/**
 * Get the node coloring for the surface.
 * @param mapIndex
 *    Index of the map.
 * @param structure
 *    Surface structure nodes are colored.
 * @param surfaceRGBAOut
 *    Filled with RGBA coloring for the surface's nodes.
 *    Contains numberOfNodes * 4 elements.
 * @param dataValuesOut
 *    Data values for the nodes (elements are valid when the alpha value in
 *    the RGBA colors is valid (greater than zero).
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @return
 *    True if coloring is valid, else false.
 */
bool
CiftiDenseSparseFile::getMapSurfaceNodeColoring(const int32_t mapIndex,
                                                 const StructureEnum::Enum structure,
                                                 float* surfaceRGBAOut,
                                                 float* dataValuesOut,
                                                 const int32_t surfaceNumberOfNodes)
{
    const int32_t numCiftiNodes = getMappingSurfaceNumberOfNodes(structure);
    
    if (numCiftiNodes != surfaceNumberOfNodes) {
        return false;
    }
    
    /*
     * Map data may be empty for connectivity matrix files with no rows loaded.
     */
    if (m_loadedRowData.empty()) {
        return false;
    }
    
    const CiftiXML& xml(m_sparseFile->getCiftiXML());
    std::vector<CiftiBrainModelsMap::SurfaceMap> surfaceMap =
    xml.getBrainModelsMap(CiftiXML::ALONG_ROW).getSurfaceMap(structure);
    
    const CiftiBrainModelsMap& brainModelsMap = xml.getBrainModelsMap(CiftiXML::ALONG_ROW);
    
    if ( ! m_rgbaValidFlag) {
        updateScalarColoringForMap(mapIndex);
    }
    
    bool validColorsFlag(false);
    
    for (int32_t iNode = 0; iNode < surfaceNumberOfNodes; iNode++) {
        const int64_t dataIndex(brainModelsMap.getIndexForNode(iNode,
                                                               structure));
        const int64_t iNode4(iNode * 4);
        CaretAssertArrayIndex(surfaceRGBAOut, (surfaceNumberOfNodes * 4), iNode4);
        
        if (dataIndex >= 0) {
            CaretAssert(dataIndex < m_loadedRowData.size());
            
            const int64_t data4 = dataIndex * 4;
            CaretAssertArrayIndex(m_rgba, (m_loadedRowData.size() * 4), dataIndex*4);
            
            surfaceRGBAOut[iNode4]   = m_rgba[data4];
            surfaceRGBAOut[iNode4+1] = m_rgba[data4+1];
            surfaceRGBAOut[iNode4+2] = m_rgba[data4+2];
            surfaceRGBAOut[iNode4+3] = m_rgba[data4+3];
            
            dataValuesOut[iNode] = m_loadedRowData[dataIndex];
            
            validColorsFlag = true;
        }
        else {
            surfaceRGBAOut[iNode4]   =  0.0;
            surfaceRGBAOut[iNode4+1] =  0.0;
            surfaceRGBAOut[iNode4+2] =  0.0;
            surfaceRGBAOut[iNode4+3] = -1.0;
            
            dataValuesOut[iNode] = 0.0;
        }
    }

    return validColorsFlag;
}

/**
 * @return True if the file has map attributes (name and metadata).
 * For files that do not have map attributes, they should override
 * this method and return false.  If not overriden, this method
 * returns true.
 *
 * Some files (such as CIFTI Connectivity Matrix Files and CIFTI
 * Data-Series Files) do not have Map Attributes and thus there
 * is no map name nor map metadata and options to edit these
 * attributes should not be presented to the user.
 *
 * These CIFTI files do contain palette color mapping but it is
 * associated with the file.  To simplify palette color mapping editing
 * these file will return the file's palette color mapping for any
 * calls to getMapPaletteColorMapping().
 */
bool
CiftiDenseSparseFile::hasMapAttributes() const
{
    return false;
}

/**
 * Get the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString
CiftiDenseSparseFile::getMapName(const int32_t /*mapIndex*/) const
{
    return m_loadedDataDescriptionForMapName;
}

/**
 * Set the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mapName
 *    New name for the map.
 */
void
CiftiDenseSparseFile::setMapName(const int32_t /*mapIndex*/,
                                  const AString& /*mapName*/)
{
    CaretLogWarning("Setting map name not supported for CiftiDenseSparseFile");
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map (const value).
 */
const GiftiMetaData*
CiftiDenseSparseFile::getMapMetaData(const int32_t /*mapIndex*/) const
{
    return m_mapMetadata.get();
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map.
 */
GiftiMetaData*
CiftiDenseSparseFile::getMapMetaData(const int32_t /*mapIndex*/)
{
    return m_mapMetadata.get();
}

/**
 * Get the unique ID (UUID) for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    String containing UUID for the map.
 */
AString
CiftiDenseSparseFile::getMapUniqueID(const int32_t mapIndex) const
{
    const GiftiMetaData* md = getMapMetaData(mapIndex);
    const AString uniqueID = md->getUniqueID();
    return uniqueID;
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool
CiftiDenseSparseFile::isMappedWithPalette() const
{
    return true;
}

/**
 * Get statistics describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Fast statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const FastStatistics*
CiftiDenseSparseFile::getMapFastStatistics(const int32_t /*mapIndex*/)
{
    if (isMappedWithPalette()) {
        if ( ! m_mapFastStatistics) {
            m_mapFastStatistics.reset(new FastStatistics());
            m_mapFastStatistics->update(&m_loadedRowData[0],
                                        m_loadedRowData.size());
        }
    }
    
    return m_mapFastStatistics.get();
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Histogram for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiDenseSparseFile::getMapHistogram(const int32_t mapIndex)
{
    if (isMappedWithPalette()) {
        if ( ! m_mapHistogram) {
            int32_t numberOfBuckets = 0;
            switch (getPaletteNormalizationMode()) {
                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                    numberOfBuckets = getFileHistogramNumberOfBuckets();
                    break;
                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                    numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
                    break;
            }
            m_mapHistogram.reset(new Histogram(numberOfBuckets,
                                               &m_loadedRowData[0],
                                               m_loadedRowData.size()));
        }
    }
    
    return m_mapHistogram.get();
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette at the given index for
 * data within the given ranges.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiDenseSparseFile::getMapHistogram(const int32_t mapIndex,
                                       const float mostPositiveValueInclusive,
                                       const float leastPositiveValueInclusive,
                                       const float leastNegativeValueInclusive,
                                       const float mostNegativeValueInclusive,
                                       const bool includeZeroValues)
{
    if (isMappedWithPalette()) {
        if ( ! m_mapHistogram) {
            int32_t numberOfBuckets = 0;
            switch (getPaletteNormalizationMode()) {
                case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                    numberOfBuckets = getFileHistogramNumberOfBuckets();
                    break;
                case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                    numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
                    break;
            }
            m_mapHistogram.reset(new Histogram(numberOfBuckets));
            m_mapHistogram->update(numberOfBuckets,
                                   &m_loadedRowData[0],
                                   m_loadedRowData.size(),
                                   mostPositiveValueInclusive,
                                   leastPositiveValueInclusive,
                                   leastNegativeValueInclusive,
                                   mostNegativeValueInclusive,
                                   includeZeroValues);
        }
    }
    
    return m_mapHistogram.get();
}

/**
 * @return The estimated size of data after it is uncompressed
 * and loaded into RAM.  A negative value indicates that the
 * file size cannot be computed.
 */
int64_t
CiftiDenseSparseFile::getDataSizeUncompressedInBytes() const
{
    return -1;
}

/**
 * Get statistics describing the distribution of data
 * mapped with a color palette for all data within the file.
 *
 * @return
 *    Fast statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const FastStatistics*
CiftiDenseSparseFile::getFileFastStatistics()
{
    /* File has one and only one map */
    const int32_t mapIndex(0);
    return getMapFastStatistics(mapIndex);
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette for all data within
 * the file.
 *
 * @return
 *    Histogram for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiDenseSparseFile::getFileHistogram()
{
    /* File has one and only one map */
    const int32_t mapIndex(0);
    return getMapHistogram(mapIndex);
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette for all data in the file
 * within the given range of values.
 *
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
CiftiDenseSparseFile::getFileHistogram(const float mostPositiveValueInclusive,
                                          const float leastPositiveValueInclusive,
                                          const float leastNegativeValueInclusive,
                                          const float mostNegativeValueInclusive,
                                          const bool includeZeroValues)
{
    const int32_t mapIndex(0);
    return getMapHistogram(mapIndex,
                           mostPositiveValueInclusive,
                           leastPositiveValueInclusive,
                           leastNegativeValueInclusive,
                           mostNegativeValueInclusive,
                           includeZeroValues);
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (will be NULL for data
 *    not mapped using a palette).
 */
PaletteColorMapping*
CiftiDenseSparseFile::getMapPaletteColorMapping(const int32_t /*mapIndex*/)
{
    return m_mapPaletteColorMapping.get();
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (will be NULL for data
 *    not mapped using a palette).
 */
const PaletteColorMapping*
CiftiDenseSparseFile::getMapPaletteColorMapping(const int32_t /*mapIndex*/) const
{
    return m_mapPaletteColorMapping.get();
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool
CiftiDenseSparseFile::isMappedWithLabelTable() const
{
    return false;
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (will be NULL for data
 *    not mapped using a label table).
 */
GiftiLabelTable*
CiftiDenseSparseFile::getMapLabelTable(const int32_t /*mapIndex*/)
{
    return NULL;
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (constant) (will be NULL for data
 *    not mapped using a label table).
 */
const GiftiLabelTable*
CiftiDenseSparseFile::getMapLabelTable(const int32_t /*mapIndex*/) const
{
    return NULL;
}

/**
 * Get the palette normalization modes that are supported by the file.
 *
 * @param modesSupportedOut
 *     Palette normalization modes supported by a file.  Will be
 *     empty for files that are not mapped with a palette.  If there
 *     is more than one suppported mode, the first mode in the
 *     vector is assumed to be the default mode.
 */
void
CiftiDenseSparseFile::getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const
{
    modesSupportedOut.clear();
}

/**
 * @return True if the map coloring for the given map is valid
 */
bool
CiftiDenseSparseFile::isMapColoringValid(const int32_t /*mapIndex*/) const
{
    return m_rgbaValidFlag;
}

/**
 * Get the thresholding data from the given thresholding file in a vector that
 * has the same brainordinate mapping as this file.
 *
 * @param threshMapFile
 *     The thresholding file.
 * @param threshMapIndex
 *     The map index in the thresholding file.
 * @param thresholdDataOut
 *     Output containing data for thresholding a map in this file.
 */
bool
CiftiDenseSparseFile::getThresholdData(const CaretMappableDataFile* /*threshMapFile*/,
                                       const int32_t /*threshMapIndex*/,
                                       std::vector<float>& /*thresholdDataOut*/) const
{
//    CaretAssert(threshMapFile);
//    CaretAssert(threshMapIndex >= 0);
//    
//    thresholdDataOut.resize(m_loadedRowData.size());
//    
//    switch (getBrainordinateMappingMatch(threshMapFile)) {
//        case BrainordinateMappingMatch::EQUAL:
//            threshMapFile->getM
//            threshMapFile->getMapData(threshMapIndex,
//                                              thresholdDataOut);
//            break;
//        case BrainordinateMappingMatch::NO:
//            CaretAssert(0); /* should never happen */
//            break;
//        case BrainordinateMappingMatch::SUBSET:
//        {
//            /*
//             * Since this file is a "subset" of the other file, will need to
//             * data using the structures in each file.
//             */
//            std::vector<float> thresholdingFileMapData;
//            threshMapFile->getMapData(threshMapIndex, thresholdingFileMapData);
//            
//            const CiftiBrainModelsMap* threshBrainMap = thresholdCiftiMapFile->getBrainordinateMapping();
//            CaretAssert(threshBrainMap);
//            const CiftiBrainModelsMap* dataBrainMap = getBrainordinateMapping();
//            CaretAssert(dataBrainMap);
//            
//            const std::vector<CiftiBrainModelsMap::ModelInfo>& dataModelsMap   = dataBrainMap->getModelInfo();
//            const std::vector<CiftiBrainModelsMap::ModelInfo>& threshModelsMap = threshBrainMap->getModelInfo();
//            for (const auto& dataModelsInfo : dataModelsMap) {
//                const StructureEnum::Enum structure = dataModelsInfo.m_structure;
//                for (const auto& threshModelsInfo : threshModelsMap) {
//                    if (structure == threshModelsInfo.m_structure) {
//                        CaretAssert(dataModelsInfo.m_indexCount == threshModelsInfo.m_indexCount);
//                        CaretAssertVectorIndex(thresholdingFileMapData, (threshModelsInfo.m_indexStart + threshModelsInfo.m_indexCount) - 1);
//                        CaretAssertVectorIndex(thresholdDataOut, (dataModelsInfo.m_indexStart + dataModelsInfo.m_indexCount) - 1);
//                        std::copy_n(&thresholdingFileMapData[threshModelsInfo.m_indexStart],
//                                    threshModelsInfo.m_indexCount,
//                                    &thresholdDataOut[dataModelsInfo.m_indexStart]);
//                        break;
//                    }
//                }
//            }
//        }
//            break;
//    }
//    
//    return true;
    return false;
}

/**
 * Update scalar coloring for a map.
 *
 * Note that some CIFTI files can be slow to color due to the need to
 * retrieve data for the map.  Use isMapColoringValid() to avoid
 * unnecessary calls to isMapColoringValid.
 *
 * @param mapIndex
 *    Index of map.
 * @param paletteFile
 *    Palette file containing palettes.
 */
void
CiftiDenseSparseFile::updateScalarColoringForMap(const int32_t mapIndex)
{
    m_rgbaValidFlag = false;
    if (isMappedWithPalette()) {
        FastStatistics* statistics = NULL;
        switch (getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                statistics = const_cast<FastStatistics*>(getFileFastStatistics());
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                statistics = const_cast<FastStatistics*>(getMapFastStatistics(mapIndex));
                break;
        }
        
        CaretAssert( ! m_loadedRowData.empty());
        CaretAssert((m_loadedRowData.size() * 4) == m_rgba.size());
        
            CaretAssert(m_mapPaletteColorMapping);
            if (statistics != NULL) {
                bool useThreshMapFileFlag = false;
                switch (m_mapPaletteColorMapping->getThresholdType()) {
                    case PaletteThresholdTypeEnum::THRESHOLD_TYPE_FILE:
                        useThreshMapFileFlag = true;
                        break;
                    case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
                        break;
                    case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
                        break;
                    case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
                        break;
                    case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
                        break;
                }
                
                if (useThreshMapFileFlag) {
                    CaretLogSevere("Threholding is disabled for CiftiDenseSparseFile");
                    useThreshMapFileFlag = false;
                }
                
                if (useThreshMapFileFlag) {
                    const CaretMappableDataFileAndMapSelectionModel* threshFileModel = this->getMapThresholdFileSelectionModel(mapIndex);
                    CaretAssert(threshFileModel);
                    const CaretMappableDataFile* threshMapFile = threshFileModel->getSelectedFile();
                    if (threshMapFile != NULL) {
                        const int32_t threshMapIndex = threshFileModel->getSelectedMapIndex();
                        std::vector<float> thresholdData;
                        getThresholdData(threshMapFile,
                                         threshMapIndex,
                                         thresholdData);
                        CaretAssert(threshMapFile);
                        CaretAssert(threshMapIndex >= 0);
                        PaletteColorMapping* thresholdPaletteColorMapping = const_cast<PaletteColorMapping*>(threshMapFile->getMapPaletteColorMapping(threshMapIndex));
                        CaretAssert(thresholdPaletteColorMapping);
                        CaretAssert(m_loadedRowData.size() == thresholdData.size());
                        NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                                                                      m_mapPaletteColorMapping.get(),
                                                                      &m_loadedRowData[0],
                                                                      thresholdPaletteColorMapping,
                                                                      &thresholdData[0],
                                                                      m_loadedRowData.size(),
                                                                      &m_rgba[0]);
                    }
                    else {
                        NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                                                                      m_mapPaletteColorMapping.get(),
                                                                      &m_loadedRowData[0],
                                                                      m_mapPaletteColorMapping.get(),
                                                                      &m_loadedRowData[0],
                                                                      m_loadedRowData.size(),
                                                                      &m_rgba[0]);
                    }
                }
                else {
                    NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                                                                  m_mapPaletteColorMapping.get(),
                                                                  &m_loadedRowData[0],
                                                                  m_mapPaletteColorMapping.get(),
                                                                  &m_loadedRowData[0],
                                                                  m_loadedRowData.size(),
                                                                  &m_rgba[0]);
                }
            }
            else {
                std::fill(m_rgba.begin(),
                          m_rgba.end(),
                          0);
            }

        
        m_rgbaValidFlag = true;
    }
    else {
        CaretAssert(0);
    }
}

/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
CiftiDenseSparseFile::readFile(const AString& filename)
{
    clear();

    checkFileReadability(filename);
    
    try {
        m_sparseFile.reset(new CaretSparseFile());
        m_sparseFile->readFile(filename);
        
        const CiftiXML& ciftiXML(m_sparseFile->getCiftiXML());
        const int32_t numDims(ciftiXML.getNumberOfDimensions());
        if (numDims >= 2) {
            m_fileNumberOfRows = ciftiXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
            m_fileNumberOfColumns = ciftiXML.getDimensionLength(CiftiXML::ALONG_ROW);
            if ((m_fileNumberOfRows * m_fileNumberOfColumns) <= 0) {
                throw DataFileException("File dimensions invalid.  Rows="
                                        + AString::number(m_fileNumberOfRows)
                                        + ", Cols="
                                        + AString::number(m_fileNumberOfColumns));
            }
        }
        else {
            throw DataFileException("Number of dimensions="
                                    + AString::number(numDims)
                                    + " is less than 2.");
        }

        setFileName(filename);
        m_fileMetadata.reset(new GiftiMetaData(*ciftiXML.getFileMetaData()));
        m_loadedRowData.resize(m_fileNumberOfColumns);
        m_rgba.resize(m_fileNumberOfColumns * 4);
        clearLoadedData();

        clearModified();
    }
    catch (const DataFileException& e) {
        clear();
        throw e;
    }
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
CiftiDenseSparseFile::writeFile(const AString& filename)
{
    writeLoadedDataToFile(filename);
}

/**
 * Get the number of nodes for the structure for mapping data.
 *
 * @param structure
 *     Structure for which number of nodes is requested.
 * @return
 *     Number of nodes corresponding to structure.  If no matching structure
 *     is found, a negative value is returned.
 */
int32_t
CiftiDenseSparseFile::getMappingSurfaceNumberOfNodes(const StructureEnum::Enum structure) const
{
    int32_t numCiftiNodes = -1;
    
    const int32_t mappingDirection(CiftiXML::ALONG_ROW);
    CaretAssert(m_sparseFile);
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    switch (ciftiXML.getMappingType(mappingDirection)) {
        case CiftiMappingType::BRAIN_MODELS:
        {
            const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(mappingDirection);
            if (map.hasSurfaceData(structure)) {
                numCiftiNodes = map.getSurfaceNumberOfNodes(structure);
            }
        }
            break;
        case CiftiMappingType::LABELS:
            CaretAssertMessage(0, "Mapping type should never be LABELS");
            break;
        case CiftiMappingType::PARCELS:
            CaretAssertMessage(0, "Mapping type should never be PARCELS");
            break;
        case CiftiMappingType::SCALARS:
            CaretAssertMessage(0, "Mapping type should never be SCALARS");
            break;
        case CiftiMappingType::SERIES:
            CaretAssertMessage(0, "Mapping type should never be SERIES");
            break;
    }
    
    return numCiftiNodes;
}

///**
// * Create a new cifti scalar data file from the loaded data of this file.
// *
// * @param errorMessageOut
// *    Error message if creation of new fiber trajectory file failed.
// * @param 
// *    Pointer to new file that was created or NULL if creation failed.
// */
//CiftiBrainordinateScalarFile*
//CiftiDenseSparseFile::newCiftiScalarFileFromLoadedRowData(const AString& destinationDirectory,
//                                                                  AString& errorMessageOut) const
//{
//    errorMessageOut = "";
//    
////    const int64_t numTraj = static_cast<int64_t>(m_fiberOrientationTrajectories.size());
////    if (numTraj <= 0) {
////        errorMessageOut = "No data is loaded so cannot create file.";
////        return NULL;
////    }
//    
//    CiftiBrainordinateScalarFile* newFile = NULL;
//    try {
//        newFile = new CiftiBrainordinateScalarFile();
//        AString rowInfo = "";
//        if (m_loadedDataDescriptionForFileCopy.isEmpty() == false) {
//            rowInfo = ("_"
//                       + m_loadedDataDescriptionForFileCopy);
//        }
//        
//        
//        /*
//         * May need to convert a remote path to a local path
//         */
//        FileInformation initialFileNameInfo(getFileName());
//        const AString scalarFileName = initialFileNameInfo.getAsLocalAbsoluteFilePath(destinationDirectory,
//                                                                                      getDataFileType());
//        
//        /*
//         * Create name of scalar file with row/column information
//         */
//        FileInformation scalarFileInfo(scalarFileName);
//        AString thePath, theName, theExtension;
//        scalarFileInfo.getFileComponents(thePath,
//                                         theName,
//                                         theExtension);
//        theName.append(rowInfo);
//        const AString newFileName = FileInformation::assembleFileComponents(thePath,
//                                                                            theName,
//                                                                            theExtension);
//        
//        
//        
//        
//        
//        const AString tempFileName = (QDir::tempPath()
//                                      + "/"
//                                      + newFile->getFileNameNoPath());
//        std::cout << "Filename: " << qPrintable(tempFileName) << std::endl;
//        
//        writeLoadedDataToFile(tempFileName);
//        
//        newFile->readFile(tempFileName);
//        newFile->setFileName(newFileName);
//        newFile->setModified();
//        return newFile;
//    }
//    catch (const DataFileException& dfe) {
//        if (newFile != NULL) {
//            delete newFile;
//        }
//        errorMessageOut = dfe.whatString();
//        return NULL;
//    }
//
//    return NULL;
//}

/**
 * Write the loaded data to a file.
 *
 * @param filename
 *    Name of file to write.
 * @throw DataFileException
 *    If an error occurs.
 */
void
CiftiDenseSparseFile::writeLoadedDataToFile(const AString& /*filename*/) const
{
    throw DataFileException("Writing not supported.");
}


/**
 * Clear the loaded data
 */
void
CiftiDenseSparseFile::clearLoadedData()
{
    m_loadedDataDescriptionForMapName = "";
    m_loadedDataDescriptionForFileCopy = "";
    
    std::fill(m_loadedRowData.begin(),
              m_loadedRowData.end(),
              0.0);
    std::fill(m_rgba.begin(),
              m_rgba.end(),
              0);
    
    m_connectivityDataLoaded->reset();
    m_mapFastStatistics.reset();
    m_mapHistogram.reset();
    m_mapHistogramLimitedRange.reset();
}

/**
 * Get the brainordinate from the given row.
 *
 * @param rowIndex
 *     Index of the row.
 * @param surfaceStructureOut
 *     Will contain structure of surface if row is a surface node.
 * @param surfaceNodeIndexOut
 *     Will contain index of surface node if row is a surface node.
 * @param surfaceNumberOfNodesOut
 *     Will contain surfaces number of nodes if row is a surface node.
 * @param surfaceNodeValidOut
 *     Will be true upon exit if the row corresponded to a surface node.
 * @param voxelIJKOut
 *     Will contain the voxel's IJK indices if row is a surface node.
 * @param voxelXYZOut
 *     Will contain the voxel's XYZ coordinate if row is a surface node.
 * @param voxelValidOut
 *     Will be true upon exit if the row corresponded to a surface node.
 * @throw DataFileException
 *     If the rows are not for brainordinates or the row index is invalid.
 */
void
CiftiDenseSparseFile::getBrainordinateFromRowIndex(const int64_t rowIndex,
                                                    StructureEnum::Enum& surfaceStructureOut,
                                                    int32_t& surfaceNodeIndexOut,
                                                    int32_t& surfaceNumberOfNodesOut,
                                                    bool& surfaceNodeValidOut,
                                                    int64_t voxelIJKOut[3],
                                                    float voxelXYZOut[3],
                                                    bool& voxelValidOut) const
{
    surfaceNodeValidOut = false;
    voxelValidOut       = false;
    
    if (m_sparseFile == NULL) {
        return;
    }
    
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    
    if (ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) {
        throw DataFileException(getFileName(),
                                "File does not have brainordinate data for rows.");
        return;
    }
    
    const CiftiBrainModelsMap& brainMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);

    const int numRows = ciftiXML.getDimensionLength(CiftiXML::ALONG_COLUMN);

    if ((rowIndex < 0)
        || (rowIndex >= numRows)) {
        throw DataFileException(getFileName(),
                                "Row index "
                                + AString::number(rowIndex)
                                + " is invalid.  Number of rows is "
                                + AString::number(numRows));
    }
    
    const CiftiBrainModelsMap::IndexInfo indexInfo = brainMap.getInfoForIndex(rowIndex);
    
    switch (indexInfo.m_type) {
        case CiftiBrainModelsMap::SURFACE:
            surfaceStructureOut     = indexInfo.m_structure;
            surfaceNodeIndexOut     = indexInfo.m_surfaceNode;
            surfaceNumberOfNodesOut = brainMap.getSurfaceNumberOfNodes(surfaceStructureOut);
            surfaceNodeValidOut     = true;
            break;
        case CiftiBrainModelsMap::VOXELS:
        {
            const VolumeSpace& colSpace = brainMap.getVolumeSpace();
            voxelIJKOut[0] = indexInfo.m_ijk[0];
            voxelIJKOut[1] = indexInfo.m_ijk[1];
            voxelIJKOut[2] = indexInfo.m_ijk[2];
            colSpace.indexToSpace(voxelIJKOut, voxelXYZOut);
            voxelValidOut = true;
        }
            break;
    }
}

/**
 * @return the row number for the given surfaces node number
 * @param surfaceStructure
 *    The structrure
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface
 * @param nodeIndex
 *    The node index
 */
int32_t
CiftiDenseSparseFile::getRowIndexFromSurfaceVertex(const StructureEnum::Enum surfaceStrucure,
                                                   const int32_t surfaceNumberOfNodes,
                                                   const int32_t nodeIndex) const
{
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& brainMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if ( ! brainMap.hasSurfaceData(surfaceStrucure)) {
        CaretLogFine("No data for structure "
                     + StructureEnum::toGuiName(surfaceStrucure));
        return -1;
    }
    if (brainMap.getSurfaceNumberOfNodes(surfaceStrucure) != surfaceNumberOfNodes) {
        CaretLogFine("Structure has wrong number of nodes="
                     + AString::number(brainMap.getSurfaceNumberOfNodes(surfaceStrucure))
                     + ", number of nodes should be "
                     + AString::number(surfaceNumberOfNodes));
        return -1;
    }
    
    return brainMap.getIndexForNode(nodeIndex,
                                    surfaceStrucure);
}

/**
 * Get the identification information for a surface node in the given maps.
 *
 * @param mapIndices
 *    Indices of maps for which identification information is requested.
 * @param structure
 *    Structure of the surface.
 * @param nodeIndex
 *    Index of the node.
 * @param numberOfNodes
 *    Number of nodes in the surface.
 * @param dataValueSeparator
 *    Separator between multiple data values
 * @param digitsRightOfDecimal
 *    Digits right of decimal for real data
 * @param textOut
 *    Output containing identification information.
 */
bool
CiftiDenseSparseFile::getSurfaceNodeIdentificationForMaps(const std::vector<int32_t>& /*mapIndices*/,
                                                           const StructureEnum::Enum structure,
                                                           const int nodeIndex,
                                                           const int32_t surfaceNumberOfNodes,
                                                           const AString& /*dataValueSeparator*/,
                                                           const int32_t digitsRightOfDecimal,
                                                           AString& textOut) const
{
    CaretAssert(m_sparseFile);
    textOut = "";
    
    bool validID = false;
    
    std::vector<float> numericalValues;
    std::vector<bool>  numericalValuesValid;
    AString textValue;
    
    /* Use along row since we want value from loaded data */
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& brainMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! brainMap.hasSurfaceData(structure)) {
        CaretLogFine("No data for structure "
                     + StructureEnum::toGuiName(structure));
        return false;
    }
    if (brainMap.getSurfaceNumberOfNodes(structure) != surfaceNumberOfNodes) {
        CaretLogFine("Structure has wrong number of nodes="
                     + AString::number(brainMap.getSurfaceNumberOfNodes(structure))
                     + ", number of nodes should be "
                     + AString::number(surfaceNumberOfNodes));
        return false;
    }
    
    const int64_t columnIndex = brainMap.getIndexForNode(nodeIndex,
                                                         structure);
    if ((columnIndex >= 0)
        && (columnIndex < static_cast<int32_t>(m_loadedRowData.size()))) {
        CaretAssertVectorIndex(m_loadedRowData, columnIndex);
        const float value(m_loadedRowData[columnIndex]);
        textOut = AString::number(value, 'f', digitsRightOfDecimal);
        validID = true;
    }

    return validID;
}


/**
 * Load connectivity data for the surface's node.
 *
 * @param mapIndex
 *    Index of map.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndex
 *    Index of node number.
 * @param rowIndexOut
 *    Index of row corresponding to node or -1 if no row in the
 *    matrix corresponds to the node.
 * @param columnIndexOut
 *    Index of column corresponding to node or -1 if no column in the
 *    matrix corresponds to the node.
 * @throw
 *    DataFileException if there is an error.
 */
void
CiftiDenseSparseFile::loadMapDataForSurfaceNode(const int32_t /*mapIndex*/,
                                                const int32_t surfaceNumberOfNodes,
                                                const StructureEnum::Enum structure,
                                                const int32_t nodeIndex,
                                                int64_t& rowIndexOut,
                                                int64_t& columnIndexOut)
{
    rowIndexOut    = -1;
    columnIndexOut = -1;
    
    if ( ! isEnabledAsLayer()) {
        clearLoadedData();
        return;
    }
    if ( ! m_dataLoadingEnabled) {
        return;
    }
    
    clearLoadedData();

    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& brainMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if ( ! brainMap.hasSurfaceData(structure)) {
        CaretLogFine("No data for structure "
                      + StructureEnum::toGuiName(structure));
        return;
    }
    if (brainMap.getSurfaceNumberOfNodes(structure) != surfaceNumberOfNodes) {
        CaretLogFine("Structure has wrong number of nodes="
                                             + AString::number(brainMap.getSurfaceNumberOfNodes(structure))
                                             + ", number of nodes should be "
                                             + AString::number(surfaceNumberOfNodes));
        return;
    }
    
    const int64_t rowIndex = brainMap.getIndexForNode(nodeIndex,
                                                      structure);
    if (rowIndex > 0) {
        const FunctionResult loadRowResult(loadDataForRowIndexPrivate(rowIndex,
                                                                      m_loadedRowData));
        if (loadRowResult.isOk()) {
            m_loadedDataDescriptionForMapName = ("Row: "
                                                 + AString::number(rowIndex)
                                                 + ", Node Index: "
                                                 + AString::number(nodeIndex)
                                                 + ", Structure: "
                                                 + StructureEnum::toName(structure));
            m_loadedDataDescriptionForFileCopy = ("Row_"
                                                  + AString::number(rowIndex));
            
            m_connectivityDataLoaded->setSurfaceNodeLoading(structure,
                                                            surfaceNumberOfNodes,
                                                            nodeIndex,
                                                            rowIndex,
                                                            -1);
        }
        else {
            clearLoadedData();
            CaretLogFine(loadRowResult.getErrorMessage());
            return;
        }
    }
    else {
        clearLoadedData();
    }
    
    rowIndexOut = rowIndex;
}

/**
 * Load average data for the given surface nodes.
 *
 * @param mapIndex
 *    Index of the map
 * @param structure
 *    Structure in which surface node is located.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param nodeIndices
 *    Indices of the surface nodes.
 */
void
CiftiDenseSparseFile::loadMapAverageDataForSurfaceNodes(const int32_t /*mapIndex*/,
                                                        const int32_t surfaceNumberOfNodes,
                                                        const StructureEnum::Enum structure,
                                                        const std::vector<int32_t>& nodeIndices)
{
    if ( ! isEnabledAsLayer()) {
        clearLoadedData();
        return;
    }
    if ( ! m_dataLoadingEnabled) {
        return;
    }
    
    clearLoadedData();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& brainMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if ( ! brainMap.hasSurfaceData(structure)) {
        CaretLogFine("No data for structure "
                     + StructureEnum::toGuiName(structure));
        return;
    }
    if (brainMap.getSurfaceNumberOfNodes(structure) != surfaceNumberOfNodes) {
        CaretLogFine("Structure has wrong number of nodes="
                     + AString::number(brainMap.getSurfaceNumberOfNodes(structure))
                     + ", number of nodes should be "
                     + AString::number(surfaceNumberOfNodes));
        return;
    }

    std::vector<int64_t> rowIndices;
    for (int32_t nodeIndex : nodeIndices) {
        const int64_t rowIndex = brainMap.getIndexForNode(nodeIndex,
                                                          structure);
        if (rowIndex > 0) {
            rowIndices.push_back(rowIndex);
        }
    }

    if (loadRowsForAveraging(rowIndices)) {
        m_loadedDataDescriptionForMapName = ("Structure: "
                                             + StructureEnum::toName(structure)
                                             + ", Averaged Node Count: "
                                             + AString::number(nodeIndices.size()));
        m_loadedDataDescriptionForFileCopy = ("Averaged_Node_Count_"
                                              + AString::number(nodeIndices.size()));
        m_connectivityDataLoaded->setSurfaceAverageNodeLoading(structure,
                                                               surfaceNumberOfNodes,
                                                               nodeIndices);
    }
}

/**
 * Load the given rows for averaging.
 *
 * @param rowIndices
 *    Indices of rows for averaging.
 * @return
 *    True if data was loaded else false if no data or user cancelled.
 * @throw 
 *    DataFileException if there is an error.
 */
bool
CiftiDenseSparseFile::loadRowsForAveraging(const std::vector<int64_t>& rowIndices)
{
    if (rowIndices.empty()) {
        return false;
    }
    
    const int64_t rowLength(m_loadedRowData.size());
    std::vector<float> rowDataSum(rowLength, 0.0);
    std::vector<float> rowData(rowLength, 0.0);
    
    for (int32_t rowIndex : rowIndices) {
        if (rowIndex > 0) {
            const FunctionResult loadRowResult(loadDataForRowIndexPrivate(rowIndex,
                                                                          rowData));
            if (loadRowResult.isOk()) {
                for (int64_t j = 0; j < rowLength; j++) {
                    CaretAssertVectorIndex(rowDataSum, j);
                    CaretAssertVectorIndex(rowData, j);
                    rowDataSum[j] += rowData[j];
                }
            }
        }
    }
    
    const float numRowsFloat(rowIndices.size());
    for (int64_t j = 0; j < rowLength; j++) {
        CaretAssertVectorIndex(m_loadedRowData, j);
        m_loadedRowData[j] /= numRowsFloat;
    }
    
    return true;
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * @param mapIndex
 *    Index of map.
 * @param xyz
 *    Coordinate of voxel.
 * @param rowIndexOut
 *    Index of row corresponding to voxel or -1 if no row in the
 *    matrix corresponds to the voxel.
 * @param columnIndexOut
 *    Index of column corresponding to voxel or -1 if no column in the
 *    matrix corresponds to the voxel.
 * @throw
 *    DataFileException if there is an error.
 */
void
CiftiDenseSparseFile::loadMapDataForVoxelAtCoordinate(const int32_t /*mapIndex*/,
                                                      const float xyz[3],
                                                      int64_t& rowIndexOut,
                                                      int64_t& columnIndexOut)
{
    rowIndexOut = -1;
    columnIndexOut = -1;
    
    m_connectivityDataLoaded->reset();
    
    if ( ! isEnabledAsLayer()) {
        clearLoadedData();
        return;
        
    }
    if ( ! m_dataLoadingEnabled) {
        return;
    }
    
    clearLoadedData();
    
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& colMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if ( ! colMap.hasVolumeData()) {
        return;
    }
    const VolumeSpace& colSpace = colMap.getVolumeSpace();
    int64_t ijk[3];
    colSpace.enclosingVoxel(xyz, ijk);
    const int64_t rowIndex = colMap.getIndexForVoxel(ijk);
    if (rowIndex >= 0) {
        const FunctionResult loadRowResult(loadDataForRowIndexPrivate(rowIndex,
                                                                      m_loadedRowData));
        if (loadRowResult.isOk()) {
            m_loadedDataDescriptionForMapName = ("Row: "
                                                 + AString::number(rowIndex)
                                                 + ", Voxel XYZ: "
                                                 + AString::fromNumbers(xyz, 3, ","));
            m_loadedDataDescriptionForFileCopy = ("Row_"
                                                  + AString::number(rowIndex));
            m_connectivityDataLoaded->setVolumeXYZLoading(xyz,
                                                          rowIndex,
                                                          -1);
            rowIndexOut = rowIndex;
        }
    }
}

/**
 * Load connectivity data for the voxel indices and then average the data.
 *
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices of voxels.
 * @throw
 *    DataFileException if there is an error.
 */
bool
CiftiDenseSparseFile::loadMapAverageDataForVoxelIndices(const int32_t /*mapIndex*/,
                                                        const int64_t volumeDimensionIJK[3],
                                                        const std::vector<VoxelIJK>& voxelIndices)
{
    if ( ! isEnabledAsLayer()) {
        clearLoadedData();
        return false;
    }
    
    if ( ! m_dataLoadingEnabled) {
        return false;
    }
    
    clearLoadedData();

    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& colMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);

    if ( !colMap.hasVolumeData()) {
        return false;
    }

    std::vector<int64_t> rowIndices;
    
    for (const VoxelIJK& ijk : voxelIndices) {
        const int64_t rowIndex = colMap.getIndexForVoxel(ijk.m_ijk);
        if (rowIndex >= 0) {
            rowIndices.push_back(rowIndex);
        }
    }
    
    if ( ! rowIndices.empty()) {
        if (loadRowsForAveraging(rowIndices)) {
            m_connectivityDataLoaded->setVolumeAverageVoxelLoading(volumeDimensionIJK,
                                                                   voxelIndices);
            
            m_loadedDataDescriptionForMapName = ("Averaged Voxel Count: "
                                                 + AString::number(rowIndices.size()));
            m_loadedDataDescriptionForFileCopy = ("Average_Voxel_Count_"
                                                  + AString::number(rowIndices.size()));
            return true;
        }
    }
    return false;
}

/**
 * Load the given row index from the file even if the file is disabled for data loading
 *
 * @param rowIndex
 *    Index of row that is loaded.
 * @throw DataFileException
 *    If an error occurs.
 */
void
CiftiDenseSparseFile::loadDataForRowIndex(const int64_t rowIndex)
{
    const FunctionResult loadResult(loadDataForRowIndexPrivate(rowIndex,
                                                               m_loadedRowData));
    if (loadResult.isOk()) {
        m_loadedDataDescriptionForMapName = ("Row: "
                                             + AString::number(rowIndex));
        m_loadedDataDescriptionForFileCopy = ("Row_"
                                              + AString::number(rowIndex));
        
        m_connectivityDataLoaded->setRowColumnLoading(rowIndex,
                                                      -1);
    }
    else {
        throw DataFileException(loadResult.getErrorMessage());
    }
}

/**
 * Load the given row index from the file even if the file is disabled for data loading
 *
 * @param columnIndex
 *    Index of column that is loaded.
 * @throw DataFileException
 *    If an error occurs.
 */
void
CiftiDenseSparseFile::loadDataForColumnIndex(const int64_t /*columnIndex*/)
{
    clearLoadedData();
    CaretLogFine("Loading by column index not supported");
}

/**
 * Load data for the given row index.
 * @param rowIndex
 *    Index of the row.
 * @param dataOut
 *    Contains data upon exit
 * @return
 *    Function result with ok/error and error message
 */
FunctionResult
CiftiDenseSparseFile::loadDataForRowIndexPrivate(const int32_t rowIndex,
                                                 std::vector<float>& dataOut) const
{
    if ((rowIndex < 0)
        || (rowIndex >= m_fileNumberOfRows)) {
        return FunctionResult::error("Invalid row index="
                                     + AString::number(rowIndex)
                                     + "   numRows="
                                     + AString::number(m_fileNumberOfRows));
    }
    
    if (static_cast<int32_t>(dataOut.size()) != m_fileNumberOfColumns) {
        dataOut.resize(m_fileNumberOfColumns);
    }
    
    m_sparseFile->getRow(rowIndex, dataOut.data());
    
    return FunctionResult::ok();
}

/**
 * @return Reference to vector containing row data
 */
const std::vector<float>&
CiftiDenseSparseFile::getLoadedRowData() const
{
    return m_loadedRowData;
}

/**
 * @return String containg description of data for use in file name
 */
AString
CiftiDenseSparseFile::getLoadedRowFileDescription() const
{
    return m_loadedDataDescriptionForFileCopy;
}

/**
 * @return String containing map name for loaded data
 */
AString
CiftiDenseSparseFile::getLoadedRowMapName() const
{
    return m_loadedDataDescriptionForMapName;
}

bool
CiftiDenseSparseFile::getMapDataForSurface(const int32_t /*mapIndex*/,
                                           const StructureEnum::Enum structure,
                                           std::vector<float>& surfaceMapData,
                                           std::vector<float>* roiData) const
{
    surfaceMapData.clear();//empty data is secondary hint at failure
    CaretAssert(m_sparseFile);
    
    const int32_t surfaceNumNodes = getMappingSurfaceNumberOfNodes(structure);
    if (surfaceNumNodes < 1) return false;
    
    /*
     * Map data may be empty for connectivity matrix files with no rows loaded.
     */
    if (m_loadedRowData.empty())
    {
        return false;
    }
    std::vector<int64_t> dataIndicesForNodes;
    if (! getSurfaceDataIndicesForMappingToBrainordinates(structure,
                                                          surfaceNumNodes,
                                                          dataIndicesForNodes))
    {
        return false;//currently should never happen, this currently works for parcellated files
    }
    CaretAssert((int)dataIndicesForNodes.size() == surfaceNumNodes);
    
    surfaceMapData.resize(surfaceNumNodes, 0.0f);
    if (roiData != NULL)
    {
        roiData->clear();//make sure all values get initialized before setting the roi nodes
        roiData->resize(surfaceNumNodes, 0.0f);
    }
    for (int32_t iNode = 0; iNode < surfaceNumNodes; iNode++) {
        CaretAssertVectorIndex(dataIndicesForNodes,
                               iNode);
        
        const int64_t dataIndex = dataIndicesForNodes[iNode];
        if (dataIndex >= 0) {
            surfaceMapData[iNode] = m_loadedRowData[dataIndex];
            if (roiData != NULL)
            {
                (*roiData)[iNode] = 1.0f;
            }
        }
    }
    return true;
}

/**
 * Get the data indices corresponding to all nodes in the given surface.
 *
 * @param structure
 *     Surface's structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in the surface.
 * @param dataIndicesForNodes
 *     Will containg "surfaceNumberOfNodes" element where the values are
 *     indices into the CIFTI data.
 * @return True if valid, else false.
 */
bool
CiftiDenseSparseFile::getSurfaceDataIndicesForMappingToBrainordinates(const StructureEnum::Enum structure,
                                                                       const int64_t surfaceNumberOfNodes,
                                                                       std::vector<int64_t>& dataIndicesForNodes) const
{
    CaretAssert(m_sparseFile);
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    
    if (ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) {
        return false;
    }
    const CiftiBrainModelsMap& map = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (map.getSurfaceNumberOfNodes(structure) == surfaceNumberOfNodes) {
        const std::vector<CiftiBrainModelsMap::SurfaceMap> surfaceMap = map.getSurfaceMap(structure);
        
        dataIndicesForNodes.resize(surfaceNumberOfNodes);
        std::fill(dataIndicesForNodes.begin(),
                  dataIndicesForNodes.end(),
                  -1);
        for (std::vector<CiftiBrainModelsMap::SurfaceMap>::const_iterator iter = surfaceMap.begin();
             iter != surfaceMap.end();
             iter++) {
            const CiftiBrainModelsMap::SurfaceMap& nodeMap = *iter;
            CaretAssertVectorIndex(dataIndicesForNodes,
                                   nodeMap.m_surfaceNode);
            dataIndicesForNodes[nodeMap.m_surfaceNode] = nodeMap.m_ciftiIndex;
        }
        
        return true;
    }
    return false;
}

/**
 * Finish restoration of scene.
 * In this file's circumstances, the fiber orientation files were not 
 * available at the time the scene was restored. 
 *
 * @throws DataFileException 
 *    If there was an error restoring the data.
 */
void
CiftiDenseSparseFile::finishRestorationOfScene()
{
    /*
     * Loading of data may be disabled in the scene
     * so temporarily enabled loading and then 
     * restore the status.
     */
    const int32_t mapIndex(0);
    const bool loadingEnabledStatus = isMapDataLoadingEnabled(mapIndex);
    setMapDataLoadingEnabled(mapIndex, true);
    
    switch (m_connectivityDataLoaded->getMode()) {
        case ConnectivityDataLoaded::MODE_NONE:
            break;
        case ConnectivityDataLoaded::MODE_ROW:
        {
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getRowColumnLoading(rowIndex,
                                                          columnIndex);
            loadDataForRowIndex(rowIndex);
        }
            break;
        case ConnectivityDataLoaded::MODE_COLUMN:
        {
            /*
             * Never load by column !!!
             */
            CaretAssertMessage(0,
                               "Dense Sparse never loads by column.");
        }
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE:
        {
            StructureEnum::Enum structure;
            int32_t surfaceNumberOfNodes;
            int32_t surfaceNodeIndex;
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getSurfaceNodeLoading(structure,
                                                            surfaceNumberOfNodes,
                                                            surfaceNodeIndex,
                                                            rowIndex,
                                                            columnIndex);
            loadMapDataForSurfaceNode(mapIndex,
                                      surfaceNumberOfNodes,
                                      structure,
                                      surfaceNodeIndex,
                                      rowIndex,
                                      columnIndex);
        }
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE_AVERAGE:
        {
            StructureEnum::Enum structure;
            int32_t surfaceNumberOfNodes;
            std::vector<int32_t> surfaceNodeIndices;
            m_connectivityDataLoaded->getSurfaceAverageNodeLoading(structure,
                                                            surfaceNumberOfNodes,
                                                            surfaceNodeIndices);
            loadMapAverageDataForSurfaceNodes(mapIndex,
                                              surfaceNumberOfNodes,
                                              structure,
                                              surfaceNodeIndices);
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_XYZ:
        {
            float volumeXYZ[3];
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getVolumeXYZLoading(volumeXYZ,
                                                          rowIndex,
                                                          columnIndex);
            loadMapDataForVoxelAtCoordinate(mapIndex,
                                            volumeXYZ,
                                            rowIndex,
                                            columnIndex);
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
        {
            int64_t volumeDimensionsIJK[3];
            std::vector<VoxelIJK> voxelIndicesIJK;
            m_connectivityDataLoaded->getVolumeAverageVoxelLoading(volumeDimensionsIJK,
                                                                   voxelIndicesIJK);
            loadMapAverageDataForVoxelIndices(mapIndex,
                                              volumeDimensionsIJK,
                                              voxelIndicesIJK);
        }
            break;
    }
    
    setMapDataLoadingEnabled(mapIndex,
                             loadingEnabledStatus);
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CiftiDenseSparseFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                  SceneClass* sceneClass)
{
    CaretMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);

    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CiftiDenseSparseFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                       const SceneClass* sceneClass)
{
    m_connectivityDataLoaded->reset();

    CaretMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    finishRestorationOfScene();
}

/**
 * @return True if this file type supports writing, else false.
 *
 * Fiber trajectory files do NOT support writing.
 */
bool
CiftiDenseSparseFile::supportsWriting() const
{
    return false;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CiftiDenseSparseFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretMappableDataFile::addToDataFileContentInformation(dataFileInformation);
    
    if (m_sparseFile != NULL) {
        const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
        const CiftiBrainModelsMap& colMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
        
        //ciftiXML.getVoxelInfoInDataFileContentInformation(CiftiXML::ALONG_COLUMN,
        //                                                  dataFileInformation);
        
        if (colMap.hasVolumeData()) {
            VolumeSpace volumeSpace = colMap.getVolumeSpace();//TSC: copied/reimplemented from CiftiXML Old - I don't think it belongs in CiftiXML or CiftiBrainModelsMap
            const int64_t* dims = volumeSpace.getDims();
            dataFileInformation.addNameAndValue("Dimensions", AString::fromNumbers(dims, 3, ","));
            VolumeSpace::OrientTypes orientation[3];
            float spacing[3];
            float origin[3];
            volumeSpace.getOrientAndSpacingForPlumb(orientation, spacing, origin);
            dataFileInformation.addNameAndValue("Spacing", AString::fromNumbers(spacing, 3, ","));
            dataFileInformation.addNameAndValue("Origin", AString::fromNumbers(origin, 3, ","));
            
            const std::vector<std::vector<float> >& sform = volumeSpace.getSform();
            for (uint32_t i = 0; i < sform.size(); i++) {
                dataFileInformation.addNameAndValue(("sform row "
                                                     + AString::number(i)),
                                                    AString::fromNumbers(sform[i], ","));
            }
            std::vector<StructureEnum::Enum> volStructs = colMap.getVolumeStructureList();
            for (int i = 0; i < (int)volStructs.size(); ++i)
            {
                std::vector<CiftiBrainModelsMap::VolumeMap> voxels = colMap.getVolumeStructureMap(volStructs[i]);
                for (int j = 0; j < (int)voxels.size(); ++j)
                {
                    float xyz[3];
                    volumeSpace.indexToSpace(voxels[i].m_ijk, xyz);
                    const AString msg = ("ijk=("
                                         + AString::fromNumbers(voxels[j].m_ijk, 3, ", ")
                                         + "), xyz=("
                                         + AString::fromNumbers(xyz, 3, ", ")
                                         + "), row="
                                         + AString::number(voxels[j].m_ciftiIndex)
                                         + "  ");//TSC: huh?
                    dataFileInformation.addNameAndValue(StructureEnum::toGuiName(volStructs[i]), msg);//TSC: huh?
                }
            }
        }

        CiftiMappableDataFile::addCiftiXmlToDataFileContentInformation(dataFileInformation,
                                                                       ciftiXML);
    }
    

}

bool CiftiDenseSparseFile::hasCiftiXML() const
{
    return true;
}

const CiftiXML CiftiDenseSparseFile::getCiftiXML() const
{
    if (m_sparseFile)
    {
        return m_sparseFile->getCiftiXML();
    }
    return CiftiXML();
}

/**
 * Get data from the file as requested in the given map file data selector.
 *
 * @param mapFileDataSelector
 *     Specifies selection of data.
 * @param dataOut
 *     Output with data.  Will be empty if data does not support the map file data selector.
 */
void
CiftiDenseSparseFile::getDataForSelector(const MapFileDataSelector& /*mapFileDataSelector*/,
                                             std::vector<float>& dataOut) const
{
    dataOut.clear();
}

/**
 * Are all brainordinates in this file also in the given file?
 * That is, the brainordinates are equal to or a subset of the brainordinates
 * in the given file.
 *
 * @param mapFile
 *     The given map file.
 * @return
 *     True if brainordinates in this file are subset or equal to the given file, else false.
 */
CaretMappableDataFile::BrainordinateMappingMatch
CiftiDenseSparseFile::getBrainordinateMappingMatchImplementation(const CaretMappableDataFile* /*mapFile*/) const
{
    return BrainordinateMappingMatch::NO;
}

/**
 * @return True if this file is enabled as a layer
 */
bool
CiftiDenseSparseFile::isEnabledAsLayer() const
{
    return m_enabledAsLayerFlag;
}

/**
 * Set this file enabled as a layer
 * @param enabled
 *    New status
 */
void
CiftiDenseSparseFile::setEnabledAsLayer(const bool enabled)
{
    m_enabledAsLayerFlag = enabled;
}

