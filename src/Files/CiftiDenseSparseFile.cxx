
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

#include "BoundingBox.h"
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
#include "GraphicsPrimitiveV3fT2f.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "TabDrawingInfo.h"
#include "VolumeGraphicsPrimitiveManager.h"

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
    
    m_graphicsPrimitiveManager.reset(new VolumeGraphicsPrimitiveManager(this, this));

    clearPrivate();
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
    
    m_volumeMappingDimensions.resize(5);
    std::fill(m_volumeMappingDimensions.begin(),
              m_volumeMappingDimensions.end(),
              0);
    
    m_boundingBox.resetZeros();
    m_boundingBoxValidFlag = false;
    m_graphicsPrimitiveManager->clear();
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
    m_graphicsPrimitiveManager->clear();
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
            CaretAssert(dataIndex < static_cast<int64_t>(m_loadedRowData.size()));
            
            const int64_t data4 = dataIndex * 4;
            CaretAssertVectorIndex(m_rgba, data4 + 3);
            
            surfaceRGBAOut[iNode4]   = static_cast<float>(m_rgba[data4])   / 255.0;
            surfaceRGBAOut[iNode4+1] = static_cast<float>(m_rgba[data4+1]) / 255.0;
            surfaceRGBAOut[iNode4+2] = static_cast<float>(m_rgba[data4+2]) / 255.0;
            surfaceRGBAOut[iNode4+3] = static_cast<float>(m_rgba[data4+3]) / 255.0;
            
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
CiftiDenseSparseFile::getThresholdData(const CaretMappableDataFile* threshMapFile,
                                       const int32_t threshMapIndex,
                                       std::vector<float>& thresholdDataOut) const
{
    CaretAssert(threshMapFile);
    CaretAssert(threshMapIndex >= 0);
    const CiftiDenseSparseFile* thresholdDenseSparseFile = dynamic_cast<const CiftiDenseSparseFile*>(threshMapFile);
    CaretAssert(thresholdDenseSparseFile);
    if (thresholdDenseSparseFile == NULL) {
        return false;
    }
    
    thresholdDataOut.resize(m_loadedRowData.size());
    
    switch (getBrainordinateMappingMatch(threshMapFile)) {
        case BrainordinateMappingMatch::EQUAL:
            thresholdDataOut = thresholdDenseSparseFile->getLoadedRowData();
            break;
        case BrainordinateMappingMatch::NO:
            CaretAssert(0); /* should never happen */
            break;
        case BrainordinateMappingMatch::SUBSET:
        {
            /*
             * Since this file is a "subset" of the other file, will need to
             * data using the structures in each file.
             */
            std::vector<float> thresholdingFileMapData(thresholdDenseSparseFile->getLoadedRowData());
            
            const CiftiBrainModelsMap* threshBrainMap = thresholdDenseSparseFile->getBrainordinateMapping();
            CaretAssert(threshBrainMap);
            const CiftiBrainModelsMap* dataBrainMap = getBrainordinateMapping();
            CaretAssert(dataBrainMap);
            
            const std::vector<CiftiBrainModelsMap::ModelInfo>& dataModelsMap   = dataBrainMap->getModelInfo();
            const std::vector<CiftiBrainModelsMap::ModelInfo>& threshModelsMap = threshBrainMap->getModelInfo();
            for (const auto& dataModelsInfo : dataModelsMap) {
                const StructureEnum::Enum structure = dataModelsInfo.m_structure;
                for (const auto& threshModelsInfo : threshModelsMap) {
                    if (structure == threshModelsInfo.m_structure) {
                        CaretAssert(dataModelsInfo.m_indexCount == threshModelsInfo.m_indexCount);
                        CaretAssertVectorIndex(thresholdingFileMapData, (threshModelsInfo.m_indexStart + threshModelsInfo.m_indexCount) - 1);
                        CaretAssertVectorIndex(thresholdDataOut, (dataModelsInfo.m_indexStart + dataModelsInfo.m_indexCount) - 1);
                        std::copy_n(&thresholdingFileMapData[threshModelsInfo.m_indexStart],
                                    threshModelsInfo.m_indexCount,
                                    &thresholdDataOut[dataModelsInfo.m_indexStart]);
                        break;
                    }
                }
            }
        }
            break;
    }
    
    return true;
}

/**
 * @return Pointer to mapping of data to brainordinates.
 *         Will be NULL if data does not map to brainordinates.
 */
const CiftiBrainModelsMap*
CiftiDenseSparseFile::getBrainordinateMapping() const
{
    if (m_sparseFile == NULL) {
        return NULL;
    }
    const CiftiXML& ciftiXML(m_sparseFile->getCiftiXML());
    return &ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
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
    
    m_graphicsPrimitiveManager->clear();
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
            if (m_fileNumberOfRows <= 0 || m_fileNumberOfColumns <= 0) {
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
        
        const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
        if (alongRowMap.hasVolumeData()) {
            CaretAssert(m_volumeMappingDimensions.size() == 5);
            CaretAssertVectorIndex(m_volumeMappingDimensions, 4);
            
            const VolumeSpace& volSpace = alongRowMap.getVolumeSpace();
            const int64_t* dims(volSpace.getDims());
            m_volumeMappingDimensions[0] = dims[0];
            m_volumeMappingDimensions[1] = dims[1];
            m_volumeMappingDimensions[2] = dims[2];
            m_volumeMappingDimensions[3] = 1;  /* time */
            m_volumeMappingDimensions[4] = 1;  /* components */

        }

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
int64_t
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
 * @return The row index index of the voxel at the given XYZ
 */
int64_t
CiftiDenseSparseFile::getRowIndexFromVolumeXYZ(const Vector3D& xyz) const
{
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& colMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if ( ! colMap.hasVolumeData()) {
        return -1;
    }
    const VolumeSpace& colSpace = colMap.getVolumeSpace();
    int64_t ijk[3];
    colSpace.enclosingVoxel(xyz, ijk);
    const int64_t rowIndex = colMap.getIndexForVoxel(ijk);

    return rowIndex;
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
 * @return
 *    True if identification data is valid
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
 * Get the identification information for a surface node in the given maps.
 *
 * @param mapIndices
 *    Indices of maps for which identification information is requested.
 * @param xyz
 *    XYZ location in volume
 * @param dataValueSeparator
 *    Separator between multiple data values
 * @param digitsRightOfDecimal
 *    Digits right of decimal for real data
 * @param ijkOut
 *    Output with IJK corresponding to input XYZ
 * @param textOut
 *    Output containing identification information.
 * @return
 *    True if identification data is valid
 */
bool
CiftiDenseSparseFile::getVolumeVoxelIdentificationForMaps(const std::vector<int32_t>& /*mapIndices*/,
                                                          const float xyz[3],
                                                          const AString& /*dataValueSeparator*/,
                                                          const int32_t digitsRightOfDecimal,
                                                          int64_t ijkOut[3],
                                                          AString& textOut) const
{
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return false;
    }
    const VolumeSpace& volSpace(alongRowMap.getVolumeSpace());
    Vector3D ijkFloat;
    volSpace.spaceToIndex(xyz, ijkFloat);
    ijkOut[0] = static_cast<int64_t>(ijkFloat[0]);
    ijkOut[1] = static_cast<int64_t>(ijkFloat[1]);
    ijkOut[2] = static_cast<int64_t>(ijkFloat[2]);
    
    bool validFlag(false);

    if (volSpace.indexValid(ijkOut)) {
        const int64_t dataOffset(alongRowMap.getIndexForVoxel(ijkOut));
        if (dataOffset >= 0) {
            CaretAssertVectorIndex(m_loadedRowData, dataOffset);
            const float value = m_loadedRowData[dataOffset];
            textOut = AString::number(value, 'f', digitsRightOfDecimal);
            validFlag = true;
        }
    }
    
    return validFlag;
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
    
    for (int64_t rowIndex : rowIndices) {
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
CiftiDenseSparseFile::loadDataForRowIndexPrivate(const int64_t rowIndex,
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
        CiftiMappableDataFile::addCiftiXmlToDataFileContentInformation(dataFileInformation,
                                                                       m_sparseFile->getCiftiXML());
    }
}

/**
 * @return True if this file has CIFTI XML
 */
bool CiftiDenseSparseFile::hasCiftiXML() const
{
    return true;
}

/**
 * @return the CIFTI XML
 */
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




/**
 * @return Instance cast to a Volume Mappable CaretMappableDataFile
 */
CaretMappableDataFile*
CiftiDenseSparseFile::castToVolumeMappableDataFile()
{
    return this;
}

/**
 * @return Instance cast to a Volume Mappable CaretMappableDataFile (const method)
 */
const CaretMappableDataFile*
CiftiDenseSparseFile::castToVolumeMappableDataFile() const
{
    return this;
}

/**
 * Get the dimensions of the volume.
 *
 * @param dimOut1
 *     First dimension (i) out.
 * @param dimOut2
 *     Second dimension (j) out.
 * @param dimOut3
 *     Third dimension (k) out.
 * @param dimTimeOut
 *     Time dimensions out (number of maps)
 * @param numComponents
 *     Number of components per voxel.
 */
void
CiftiDenseSparseFile::getDimensions(int64_t& dimOut1,
                           int64_t& dimOut2,
                           int64_t& dimOut3,
                           int64_t& dimTimeOut,
                           int64_t& numComponents) const
{
    CaretAssert(m_volumeMappingDimensions.size() == 5);
    CaretAssertVectorIndex(m_volumeMappingDimensions, 4);
    dimOut1       = m_volumeMappingDimensions[0];
    dimOut2       = m_volumeMappingDimensions[1];
    dimOut3       = m_volumeMappingDimensions[2];
    dimTimeOut    = m_volumeMappingDimensions[3];
    numComponents = m_volumeMappingDimensions[4];
}

/**
 * Get the dimensions of the volume.
 *
 * @param dimsOut
 *     Will contain 5 elements: (0) X-dimension, (1) Y-dimension
 * (2) Z-dimension, (3) time, (4) components.
 */
void
CiftiDenseSparseFile::getDimensions(std::vector<int64_t>& dimsOut) const
{
    CaretAssert(m_volumeMappingDimensions.size() == 5);
    dimsOut = m_volumeMappingDimensions;
}

/**
 * @return The number of componenents per voxel.
 */
const int64_t&
CiftiDenseSparseFile::getNumberOfComponents() const
{
    CaretAssert(m_volumeMappingDimensions.size() == 5);
    CaretAssertVectorIndex(m_volumeMappingDimensions, 4);
    return m_volumeMappingDimensions[4];
}

/**
 * Get the value of the voxel containing the given coordinate.
 *
 * @param coordinateIn
 *    The 3D coordinate
 * @param validOut
 *    If not NULL, will indicate if the coordinate (and hence the
 *    returned value) is valid.
 * @param mapIndex
 *    Index of map.
 * @param component
 *    Voxel component.
 * @return
 *    Value of voxel containing the given coordinate.
 */
float
CiftiDenseSparseFile::getVoxelValue(const float* coordinateIn,
                                    bool* validOut,
                                    const int64_t /*mapIndex*/,
                                    const int64_t /*component*/) const
{
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return 0;
    }
    const VolumeSpace& volSpace(alongRowMap.getVolumeSpace());
    Vector3D ijkFloat;
    volSpace.spaceToIndex(coordinateIn, ijkFloat);
    const int64_t ijk[3] {
        static_cast<int64_t>(ijkFloat[0]),
        static_cast<int64_t>(ijkFloat[1]),
        static_cast<int64_t>(ijkFloat[2])
    };

    float valueOut(0);
    if (validOut != NULL) {
        *validOut = false;
    }
    if (volSpace.indexValid(ijk)) {
        const int64_t dataOffset(alongRowMap.getIndexForVoxel(ijk));
        if (dataOffset >= 0) {
            CaretAssertVectorIndex(m_loadedRowData, dataOffset);
            valueOut = m_loadedRowData[dataOffset];
            if (validOut != NULL) {
                *validOut = true;
            }
        }
    }
    return valueOut;
}

/**
 * Get the value of the voxel containing the given coordinate.
 *
 * @param coordinateX
 *    The X coordinate
 * @param coordinateY
 *    The Y coordinate
 * @param coordinateZ
 *    The Z coordinate
 * @param validOut
 *    If not NULL, will indicate if the coordinate (and hence the
 *    returned value) is valid.
 * @param mapIndex
 *    Index of map.
 * @param component
 *    Voxel component.
 * @return
 *    Value of voxel containing the given coordinate.
 */
float
CiftiDenseSparseFile::getVoxelValue(const float coordinateX,
                            const float coordinateY,
                            const float coordinateZ,
                            bool* validOut,
                            const int64_t mapIndex,
                            const int64_t component) const
{
    const Vector3D xyz(coordinateX,
                       coordinateY,
                       coordinateZ);
    return getVoxelValue(xyz,
                         validOut,
                         mapIndex,
                         component);
}

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param coordOut1
 *     Output first (x) coordinate.
 * @param coordOut2
 *     Output first (y) coordinate.
 * @param coordOut3
 *     Output first (z) coordinate.
 */
void
CiftiDenseSparseFile::indexToSpace(const float& indexIn1,
                          const float& indexIn2,
                          const float& indexIn3,
                          float& coordOut1,
                          float& coordOut2,
                          float& coordOut3) const
{
    coordOut1 = 0.0;
    coordOut2 = 0.0;
    coordOut3 = 0.0;
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return;
    }
    const VolumeSpace& volSpace = alongRowMap.getVolumeSpace();
    volSpace.indexToSpace(indexIn1, indexIn2, indexIn3, coordOut1, coordOut2, coordOut3);
}

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param coordOut
 *     Output XYZ coordinates.
 */
void
CiftiDenseSparseFile::indexToSpace(const float& indexIn1,
                          const float& indexIn2,
                          const float& indexIn3,
                          float* coordOut) const
{
    indexToSpace(indexIn1,
                 indexIn2,
                 indexIn3,
                 coordOut[0],
                 coordOut[1],
                 coordOut[2]);
}

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn
 *     IJK indices
 * @param coordOut
 *     Output XYZ coordinates.
 */
void
CiftiDenseSparseFile::indexToSpace(const int64_t* indexIn,
                          float* coordOut) const
{
    indexToSpace(indexIn[0],
                 indexIn[1],
                 indexIn[2],
                 coordOut[0],
                 coordOut[1],
                 coordOut[2]);
}

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @return
 *     Output XYZ coordinates.
 */
Vector3D
CiftiDenseSparseFile::indexToSpace(const int64_t ijk[3]) const
{
    
    Vector3D xyz;
    indexToSpace(ijk,
                 xyz);
    
    return xyz;
}

/**
 * Convert a coordinate to indices.  Note that output indices
 * MAY NOT BE WITHIN THE VALID VOXEL DIMENSIONS.
 *
 * @param coordIn1
 *     First (x) input coordinate.
 * @param coordIn2
 *     Second (y) input coordinate.
 * @param coordIn3
 *     Third (z) input coordinate.
 * @param indexOut1
 *     First output index (i).
 * @param indexOut2
 *     First output index (j).
 * @param indexOut3
 *     First output index (k).
 */
void
CiftiDenseSparseFile::enclosingVoxel(const float& coordIn1,
                            const float& coordIn2,
                            const float& coordIn3,
                            int64_t& indexOut1,
                            int64_t& indexOut2,
                            int64_t& indexOut3) const
{
    indexOut1 = 0;
    indexOut2 = 0;
    indexOut3 = 0;
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return;
    }
    const VolumeSpace& volSpace = alongRowMap.getVolumeSpace();
    volSpace.enclosingVoxel(coordIn1, coordIn2, coordIn3,
                            indexOut1, indexOut2, indexOut3);
}

/**
 * Convert a coordinate to indices.  Note that output indices
 * MAY NOT BE WITHIN THE VALID VOXEL DIMENSIONS.
 *
 * @param coordIn1
 *     First (x) input coordinate.
 * @param coordIn2
 *     Second (y) input coordinate.
 */
void
CiftiDenseSparseFile::enclosingVoxel(const float* coordIn,
                            int64_t* indexOut) const
{
    enclosingVoxel(coordIn[0], coordIn[1], coordIn[2],
                   indexOut[0], indexOut[1], indexOut[2]);
}

/**
 * Determine in the given voxel indices are valid (within the volume).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param coordOut1
 *     Output first (x) coordinate.
 * @param brickIndex
 *     Time/map index (default 0).
 * @param component
 *     Voxel component (default 0).
 */
bool
CiftiDenseSparseFile::indexValid(const int64_t& indexIn1,
                        const int64_t& indexIn2,
                        const int64_t& indexIn3,
                        const int64_t brickIndex,
                        const int64_t component) const
{
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return false;
    }
    const VolumeSpace& volSpace = alongRowMap.getVolumeSpace();
    if (brickIndex != 0) {
        return false;
    }
    if (component != 0) {
        return false;
    }
    return volSpace.indexValid(indexIn1, indexIn2, indexIn3);
}

/**
 * Determine in the given voxel indices are valid (within the volume).
 *
 * @param indexIn
 *     IJK
 * @param brickIndex
 *     Time/map index (default 0).
 * @param component
 *     Voxel component (default 0).
 */
bool
CiftiDenseSparseFile::indexValid(const int64_t* indexIn,
                        const int64_t brickIndex,
                        const int64_t component) const
{
    return indexValid(indexIn[0], indexIn[1], indexIn[2],
                      brickIndex, component);
}

/**
 * Get a bounding box for the voxel coordinate ranges.
 *
 * @param boundingBoxOut
 *    The output bounding box.
 */
void
CiftiDenseSparseFile::getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const
{
    if ( ! m_boundingBoxValidFlag) {
        const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
        const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
        if (alongRowMap.hasVolumeData()) {
            const std::vector<CiftiBrainModelsMap::VolumeMap>& volMap(alongRowMap.getFullVolumeMap());
            
            m_boundingBox.resetForUpdate();
            for (auto& vm : volMap) {
                m_boundingBox.update(indexToSpace(vm.m_ijk));
            }
        }
        else {
            m_boundingBox.resetZeros();
        }
        
        m_boundingBoxValidFlag = true;
    }
    
    boundingBoxOut = m_boundingBox;
}

/**
 * Get a bounding box containing the non-zero voxel coordinate ranges
 * @param mapIndex
 *    Index of map
 * @param boundingBoxOut
 *    Output containing coordinate range of non-zero voxels
 */
void
CiftiDenseSparseFile::getNonZeroVoxelCoordinateBoundingBox(const int32_t /*mapIndex*/,
                                                  BoundingBox& boundingBoxOut) const
{
    /*
     * File only contains voxels with data so use the bounding box
     */
    getVoxelSpaceBoundingBox(boundingBoxOut);
}

/**
 * Get the voxel colors for a slice in the map.
 *
 * @param mapIndex
 *    Index of the map.
 * @param slicePlane
 *    The slice plane.
 * @param sliceIndex
 *    Index of the slice.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param rgbaOut
 *    Output containing the rgba values (must have been allocated
 *    by caller to sufficient count of elements in the slice).
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
CiftiDenseSparseFile::getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                            const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                            const int64_t sliceIndex,
                                            const TabDrawingInfo& tabDrawingInfo,
                                            uint8_t* rgbaOut) const
{
    int64_t firstVoxelIJK[3] { 0, 0, 0 };
    int64_t rowStepIJK[3] { 0, 0, 0 };
    int64_t columnStepIJK[3] { 0, 0, 0 };
    int64_t numberOfRows(0);
    int64_t numberOfColumns(0);
    
    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            firstVoxelIJK[0] = 0;
            firstVoxelIJK[1] = 0;
            firstVoxelIJK[2] = sliceIndex;
            rowStepIJK[0]    = 0;
            rowStepIJK[1]    = 1;
            rowStepIJK[2]    = 0;
            columnStepIJK[0] = 1;
            columnStepIJK[1] = 0;
            columnStepIJK[2] = 0;
            numberOfRows     = dimJ;
            numberOfColumns  = dimI;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            firstVoxelIJK[0] = 0;
            firstVoxelIJK[1] = sliceIndex;
            firstVoxelIJK[2] = 0;
            rowStepIJK[0]    = 0;
            rowStepIJK[1]    = 0;
            rowStepIJK[2]    = 1;
            columnStepIJK[0] = 1;
            columnStepIJK[1] = 0;
            columnStepIJK[2] = 0;
            numberOfRows     = dimK;
            numberOfColumns  = dimI;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            firstVoxelIJK[0] = sliceIndex;
            firstVoxelIJK[1] = dimJ - 1;
            firstVoxelIJK[2] = 0;
            rowStepIJK[0]    = 0;
            rowStepIJK[1]    = 0;
            rowStepIJK[2]    = 1;
            columnStepIJK[0] = 0;
            columnStepIJK[1] = -1;
            columnStepIJK[2] = 0;
            numberOfRows     = dimK;
            numberOfColumns  = dimJ;
            break;
    }

    return getVoxelColorsForSliceInMap(mapIndex,
                                       firstVoxelIJK,
                                       rowStepIJK,
                                       columnStepIJK,
                                       numberOfRows,
                                       numberOfColumns,
                                       tabDrawingInfo,
                                       rgbaOut);
}

/**
 * Get voxel coloring for a set of voxels.
 *
 * @param mapIndex
 *     Index of map.
 * @param firstVoxelIJK
 *    IJK Indices of first voxel
 * @param rowStepIJK
 *    IJK Step for moving to next row.
 * @param columnStepIJK
 *    IJK Step for moving to next column.
 * @param numberOfRows
 *    Number of rows.
 * @param numberOfColumns
 *    Number of columns.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param rgbaOut
 *    RGBA color components out.
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
CiftiDenseSparseFile::getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                            const int64_t firstVoxelIJK[3],
                                            const int64_t rowStepIJK[3],
                                            const int64_t columnStepIJK[3],
                                            const int64_t numberOfRows,
                                            const int64_t numberOfColumns,
                                            const TabDrawingInfo& /*tabDrawingInfo*/,
                                            uint8_t* rgbaOut) const
{
    if ( ! isMapColoringValid(mapIndex)) {
        CiftiDenseSparseFile* nonConstThis(const_cast<CiftiDenseSparseFile*>(this));
        CaretAssert(nonConstThis);
        nonConstThis->updateScalarColoringForMap(mapIndex);
    }
    const int64_t mapRgbaCount(m_rgba.size());
    
    
    /*
     * RGBA size will be zero if no data has been loaded for a CIFTI
     * matrix type file (user clicking brainordinate).
     */
    if (mapRgbaCount <= 0) {
        return 0;
    }
    
    /*
     * Zero out the RGBA output
     */
    const int64_t numSliceVoxels(numberOfRows * numberOfColumns);
    const int64_t numSliceRGBA(numSliceVoxels * 4);
    std::fill(&rgbaOut[0], &rgbaOut[numSliceRGBA], 0);
    
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return 0;
    }
    const VolumeSpace& volSpace(alongRowMap.getVolumeSpace());

    int64_t rowIJK[3] = { firstVoxelIJK[0], firstVoxelIJK[1], firstVoxelIJK[2] };
    int64_t rgbaOutIndex4 = 0;
    
    int64_t validVoxelCount = 0;
    
    for (int32_t iRow = 0; iRow < numberOfRows; iRow++) {
        int64_t ijk[3] = { rowIJK[0], rowIJK[1], rowIJK[2] };
        
        for (int32_t iCol = 0; iCol < numberOfColumns; iCol++) {
            if (volSpace.indexValid(ijk)) {
                const int64_t dataOffset(alongRowMap.getIndexForVoxel(ijk));
                if (dataOffset >= 0) {
                    const int64_t rgbaOffset(dataOffset * 4);
                    CaretAssertVectorIndex(m_rgba, rgbaOffset + 3);
                    const uint8_t alpha(m_rgba[rgbaOffset + 3]);
                    if (alpha > 0) {
                        rgbaOut[rgbaOutIndex4]   = m_rgba[rgbaOffset];
                        rgbaOut[rgbaOutIndex4+1] = m_rgba[rgbaOffset+1];
                        rgbaOut[rgbaOutIndex4+2] = m_rgba[rgbaOffset+2];
                        rgbaOut[rgbaOutIndex4+3] = alpha;
                        validVoxelCount++;
                    }
                }
            }
            rgbaOutIndex4 += 4;
            
            ijk[0] += columnStepIJK[0];
            ijk[1] += columnStepIJK[1];
            ijk[2] += columnStepIJK[2];
        }
        
        rowIJK[0] += rowStepIJK[0];
        rowIJK[1] += rowStepIJK[1];
        rowIJK[2] += rowStepIJK[2];
    }
    
    return validVoxelCount;
}

/**
 * Get the voxel colors for a sub slice in the map.
 *
 * @param mapIndex
 *    Index of the map.
 * @param slicePlane
 *    The slice plane.
 * @param sliceIndex
 *    Index of the slice.
 * @param firstCornerVoxelIndex
 *    Indices of voxel for first corner of sub-slice (inclusive).
 * @param lastCornerVoxelIndex
 *    Indices of voxel for last corner of sub-slice (inclusive).
 * @param voxelCountIJK
 *    Voxel counts for each axis.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param rgbaOut
 *    Output containing the rgba values (must have been allocated
 *    by caller to sufficient count of elements in the slice).
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
CiftiDenseSparseFile::getVoxelColorsForSubSliceInMap(const int32_t mapIndex,
                                               const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                               const int64_t /*sliceIndex*/,
                                               const int64_t firstCornerVoxelIndex[3],
                                               const int64_t lastCornerVoxelIndex[3],
                                               const int64_t voxelCountIJK[3],
                                               const TabDrawingInfo& tabDrawingInfo,
                                               uint8_t* rgbaOut) const
{
    int64_t firstVoxelIJK[3] { 0, 0, 0 };
    int64_t rowStepIJK[3] { 0, 0, 0 };
    int64_t columnStepIJK[3] { 0, 0, 0 };
    int64_t numberOfRows(0);
    int64_t numberOfColumns(0);
    
    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            CaretAssert(0);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            firstVoxelIJK[0] = firstCornerVoxelIndex[0];
            firstVoxelIJK[1] = firstCornerVoxelIndex[1];
            firstVoxelIJK[2] = firstCornerVoxelIndex[2];
            rowStepIJK[0]    = 0;
            rowStepIJK[1]    = ((lastCornerVoxelIndex[1] > firstCornerVoxelIndex[1])
                                ? 1 : -1);
            rowStepIJK[2]    = 0;
            columnStepIJK[0] = ((lastCornerVoxelIndex[0] > firstCornerVoxelIndex[0])
                                ? 1 : -1);
            columnStepIJK[1] = 0;
            columnStepIJK[2] = 0;
            numberOfRows     = voxelCountIJK[1];
            numberOfColumns  = voxelCountIJK[0];
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            firstVoxelIJK[0] = firstCornerVoxelIndex[0];
            firstVoxelIJK[1] = firstCornerVoxelIndex[1];
            firstVoxelIJK[2] = firstCornerVoxelIndex[2];
            rowStepIJK[0]    = 0;
            rowStepIJK[1]    = 0;
            rowStepIJK[2]    = ((lastCornerVoxelIndex[2] > firstCornerVoxelIndex[2])
                                ? 1 : -1);
            columnStepIJK[0] = ((lastCornerVoxelIndex[0] > firstCornerVoxelIndex[0])
                                ? 1 : -1);
            columnStepIJK[1] = 0;
            columnStepIJK[2] = 0;
            numberOfRows     = voxelCountIJK[2];
            numberOfColumns  = voxelCountIJK[0];
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            firstVoxelIJK[0] = firstCornerVoxelIndex[0];
            firstVoxelIJK[1] = firstCornerVoxelIndex[1];
            firstVoxelIJK[2] = firstCornerVoxelIndex[2];
            rowStepIJK[0]    = 0;
            rowStepIJK[1]    = 0;
            rowStepIJK[2]    = ((lastCornerVoxelIndex[2] > firstCornerVoxelIndex[2])
                                ? 1 : -1);
            columnStepIJK[0] = 0;
            columnStepIJK[1] = ((lastCornerVoxelIndex[1] > firstCornerVoxelIndex[1])
                                ? 1 : -1); 
            columnStepIJK[2] = 0;
            numberOfRows     = voxelCountIJK[2];
            numberOfColumns  = voxelCountIJK[1];
            break;
    }
    
    return getVoxelColorsForSliceInMap(mapIndex,
                                       firstVoxelIJK,
                                       rowStepIJK,
                                       columnStepIJK,
                                       numberOfRows,
                                       numberOfColumns,
                                       tabDrawingInfo,
                                       rgbaOut);
}

/**
 * Get the voxel coloring for the voxel at the given indices.
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param brickIndex
 *     Time/map index.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param rgbaOut
 *     Output containing RGBA values for voxel at the given indices.
 */
void
CiftiDenseSparseFile::getVoxelColorInMap(const int64_t indexIn1,
                                const int64_t indexIn2,
                                const int64_t indexIn3,
                                const int64_t /*brickIndex*/,
                                const TabDrawingInfo& tabDrawingInfo,
                                uint8_t rgbaOut[4]) const
{
    return getVoxelColorInMap(indexIn1,
                              indexIn2,
                              indexIn3,
                              tabDrawingInfo.getTabIndex(),
                              rgbaOut);
}

/**
 * Get the voxel coloring for the voxel at the given indices.
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @param brickIndex
 *     Time/map index.
 * @param rgbaOut
 *     Output containing RGBA values for voxel at the given indices.
 */
void
CiftiDenseSparseFile::getVoxelColorInMap(const int64_t indexIn1,
                                const int64_t indexIn2,
                                const int64_t indexIn3,
                                const int64_t /*tabIndex*/,
                                uint8_t rgbaOut[4]) const
{
    const int32_t mapIndex(0);
    if (isMapColoringValid(mapIndex)) {
        CiftiDenseSparseFile* nonConstThis(const_cast<CiftiDenseSparseFile*>(this));
        CaretAssert(nonConstThis);
        nonConstThis->updateScalarColoringForMap(mapIndex);
    }
    const int64_t mapRgbaCount(m_rgba.size());
    
    
    /*
     * RGBA size will be zero if no data has been loaded for a CIFTI
     * matrix type file (user clicking brainordinate).
     */
    if (mapRgbaCount <= 0) {
        return;
    }
    
    /*
     * Zero out the RGBA output
     */
    std::fill(&rgbaOut[0], &rgbaOut[4], 0);
    
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return;
    }
    const VolumeSpace& volSpace(alongRowMap.getVolumeSpace());
    
    int64_t ijk[3] = { indexIn1, indexIn2, indexIn3 };
    if (volSpace.indexValid(ijk)) {
        const int64_t dataOffset(alongRowMap.getIndexForVoxel(ijk));
        if (dataOffset >= 0) {
            const int64_t rgbaOffset(dataOffset * 4);
            CaretAssertVectorIndex(m_rgba, rgbaOffset + 3);
            const uint8_t alpha(m_rgba[rgbaOffset + 3]);
            if (alpha > 0) {
                rgbaOut[0]   = m_rgba[rgbaOffset];
                rgbaOut[1] = m_rgba[rgbaOffset+1];
                rgbaOut[2] = m_rgba[rgbaOffset+2];
                rgbaOut[3] = alpha;
            }
        }
    }
}

GraphicsPrimitiveV3fT2f*
CiftiDenseSparseFile::getSingleSliceVolumeDrawingPrimitive(const int32_t/* mapIndex*/,
                                                           const TabDrawingInfo& /*tabDrawingInfo*/) const
{
    CaretLogSevere("Single slice volume not supported for CIFTI Dense Sparse files");
    return NULL;
}

/**
 * Get the graphics primitive for drawing this volume using a TRIANGLES STRIP graphics primitive
 *
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
CiftiDenseSparseFile::getVolumeDrawingTriangleStripPrimitive(const int32_t mapIndex,
                                                                        const TabDrawingInfo& tabDrawingInfo) const
{
    return m_graphicsPrimitiveManager->getVolumeDrawingPrimitiveForMap(VolumeGraphicsPrimitiveManager::PrimitiveShape::TRIANGLE_STRIP,
                                                                       mapIndex,
                                                                       tabDrawingInfo);
}

/**
 * Get the graphics primitive for drawing this volume using a TRIANGLES FAN graphics primitive
 *
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
CiftiDenseSparseFile::getVolumeDrawingTriangleFanPrimitive(const int32_t mapIndex,
                                                                      const TabDrawingInfo& tabDrawingInfo) const
{
    return m_graphicsPrimitiveManager->getVolumeDrawingPrimitiveForMap(VolumeGraphicsPrimitiveManager::PrimitiveShape::TRIANGLE_FAN,
                                                                       mapIndex,
                                                                       tabDrawingInfo);
}

/**
 * Get the graphics primitive for drawing this volume using a TRIANGLES graphics primitive
 *
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
CiftiDenseSparseFile::getVolumeDrawingTrianglesPrimitive(const int32_t mapIndex,
                                                                    const TabDrawingInfo& tabDrawingInfo) const
{
    return m_graphicsPrimitiveManager->getVolumeDrawingPrimitiveForMap(VolumeGraphicsPrimitiveManager::PrimitiveShape::TRIANGLES,
                                                                       mapIndex,
                                                                       tabDrawingInfo);
}

/**
 * Create a graphics primitive for showing part of volume that intersects with an image from histology
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param mediaFile
 *    The medial file for drawing histology
 * @param volumeMappingMode
 *    The volume to image mapping mode
 * @param volumeSliceThickness
 *    The volume slice thickness for mapping volume to image
 * @param errorMessageOut
 *    Ouput with error message
 * @return
 *    Primitive for drawing intersection or NULL if failure
 */
GraphicsPrimitive*
CiftiDenseSparseFile::getHistologyImageIntersectionPrimitive(const int32_t mapIndex,
                                                                  const TabDrawingInfo& tabDrawingInfo,
                                                                  const MediaFile* mediaFile,
                                                                  const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                  const float volumeSliceThickness,
                                                                  AString& errorMessageOut) const
{
    return m_graphicsPrimitiveManager->getImageIntersectionDrawingPrimitiveForMap(mediaFile,
                                                                                  mapIndex,
                                                                                  tabDrawingInfo,
                                                                                  volumeMappingMode,
                                                                                  volumeSliceThickness,
                                                                                  errorMessageOut);
}

/**
 * Create a graphics primitive for showing part of volume that intersects with an image from histology
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param histologySlice
 *    The histology slice being drawn
 * @param volumeMappingMode
 *    The volume to image mapping mode
 * @param volumeSliceThickness
 *    The volume slice thickness for mapping volume to image
 * @param errorMessageOut
 *    Ouput with error message
 * @return
 *    Primitive for drawing intersection or NULL if failure
 */
std::vector<GraphicsPrimitive*>
CiftiDenseSparseFile::getHistologySliceIntersectionPrimitive(const int32_t mapIndex,
                                                                               const TabDrawingInfo& tabDrawingInfo,
                                                                               const HistologySlice* histologySlice,
                                                                               const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                               const float volumeSliceThickness,
                                                                               AString& errorMessageOut) const
{
    return m_graphicsPrimitiveManager->getImageIntersectionDrawingPrimitiveForMap(histologySlice,
                                                                                  mapIndex,
                                                                                  tabDrawingInfo,
                                                                                  volumeMappingMode,
                                                                                  volumeSliceThickness,
                                                                                  errorMessageOut);
}

/**
 * Get the volume space object, so we have access to all functions associated with volume spaces
 */
const VolumeSpace&
CiftiDenseSparseFile::getVolumeSpace() const
{
    const CiftiXML& ciftiXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& alongRowMap = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
    if ( ! alongRowMap.hasVolumeData()) {
        return m_dummyVolumeSpace;
    }
    return alongRowMap.getVolumeSpace();
}
