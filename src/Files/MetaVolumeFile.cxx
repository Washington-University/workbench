
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

#define __META_VOLUME_FILE_DECLARE__
#include "MetaVolumeFile.h"
#undef __META_VOLUME_FILE_DECLARE__

#include <QXmlStreamReader>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "DataSliceArray.h"
#include "FastStatistics.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "Histogram.h"
#include "MetaVolumeFileXmlStreamReader.h"
#include "MetaVolumeFileXmlStreamWriter.h"
#include "PaletteColorMapping.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class caret::MetaVolumeFile 
 * \brief Meta Volume file encapsulates VolumeFile's
 * \ingroup Files
 */

/**
 * Constructor.
 */
MetaVolumeFile::MetaVolumeFile()
: CaretMappableDataFile(DataFileTypeEnum::META_VOLUME)
{
    m_fileMapInfo.reset(new MapInfo(this,
                                    -1));
}

/**
 * Destructor.
 */
MetaVolumeFile::~MetaVolumeFile()
{
}

void
MetaVolumeFile::clearModified()
{
    CaretMappableDataFile::clearModified();
    m_fileMapInfo->clearModified();
    for (auto& vf : m_volumeFiles) {
        vf->clearModified();
    }
    for (auto& mm : m_mapInfo) {
        mm->clearModified();
    }
}

/**
 * @return True if this file is modified, else false.
 */
bool
MetaVolumeFile::isModifiedExcludingPaletteColorMapping() const
{
    if (CaretMappableDataFile::isModifiedExcludingPaletteColorMapping()) {
        return true;
    }
    if (getFileMetaData()->isModified()) {
        return true;
    }
    for (auto& vf : m_volumeFiles) {
        if (vf->isModifiedExcludingPaletteColorMapping()) {
            return true;
        }
    }
    for (auto& mm : m_mapInfo) {
        if (mm->isModifiedExcludingPaletteColorMapping()) {
            return true;
        }
    }
    return false;
}

/**
 * Clear the file.
 */
void
MetaVolumeFile::clear()
{
    m_fileMapInfo.reset(new MapInfo(this,
                                    -1));
    m_volumeFiles.clear();
    for (auto& mm : m_mapInfo) {
        mm->clearModified();
    }
}

/**
 * @return True if the file is empty, else false.
 */
bool
MetaVolumeFile::isEmpty() const
{
    return (getNumberOfMaps() == 0);
}

/**
 * Add a volume file.  All volume files in this meta-volume MUST have the same number of maps.
 * @param volumeFile
 *    File to add
 * @return
 *    Function result with success or failure
 */
FunctionResult
MetaVolumeFile::addVolumeFile(VolumeFile* volumeFile)
{
    CaretAssert(volumeFile);
    
    const int32_t numMaps(volumeFile->getNumberOfMaps());
    if (numMaps <= 0) {
        return FunctionResult::error("Number of maps is zero");
    }
    if (m_volumeFiles.empty()) {
        for (int32_t i = 0; i < numMaps; i++) {
            m_mapInfo.emplace_back(new MapInfo(this,
                                               i));
        }
    }
    else {
        CaretAssertVectorIndex(m_volumeFiles, 0);
        if (m_volumeFiles[0]->getNumberOfMaps() != numMaps) {
            const AString msg("Number of maps should be "
                              + AString::number(m_volumeFiles[0]->getNumberOfMaps())
                              + " but this file contains "
                              + AString::number(numMaps)
                              + " maps.");
            return FunctionResult::error(msg);
        }
    }
    
    volumeFile->setParentMetaVolumeFile(this);
    m_volumeFiles.emplace_back(volumeFile);
    
    return FunctionResult::ok();
}

/**
 * @return Number of volume files
 */
int32_t
MetaVolumeFile::getNumberOfVolumeFiles() const
{
    return m_volumeFiles.size();
}

/**
 * @return VolumeFile at the given index
 * @param index
 *    Index of the volume file
 */
VolumeFile*
MetaVolumeFile::getVolumeFile(const int32_t index)
{
    CaretAssertVectorIndex(m_volumeFiles, index);
    return m_volumeFiles[index].get();
}

/**
 * @return VolumeFile at the given index
 * @param index
 *    Index of the volume file
 */
const VolumeFile*
MetaVolumeFile::getVolumeFile(const int32_t index) const
{
    CaretAssertVectorIndex(m_volumeFiles, index);
    return m_volumeFiles[index].get();
}

/**
 * @return First volume file containing the given coordinate.
 * Assumes that the volume files do not overlap.
 * NULL if none found.
 * @param mapIndex
 *    Index of map
 * @param xyz
 *    The coordinate.
 * @param voxelValueOut
 *    If volume returned is valid, this contains value at voxel for map.
 */
const VolumeFile*
MetaVolumeFile::getVolumeFileContainingXYZ(const int32_t mapIndex,
                                           const Vector3D& xyz,
                                           float& voxelValueOut) const
{
    voxelValueOut = 0.0;
    
    for (const auto& vf : m_volumeFiles) {
        bool validFlag(false);
        voxelValueOut = vf->getVoxelValue(xyz[0],
                                          xyz[1],
                                          xyz[2],
                                          &validFlag,
                                          mapIndex);
        if (validFlag) {
            return vf.get();
        }
    }
    
    return NULL;
}

/**
 * Read the data file.
 *
 * @param filenameIn
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void 
MetaVolumeFile::readFile(const AString& filenameIn)
{
    clear();
    
    AString filename = filenameIn;
    if (DataFile::isFileOnNetwork(filename)) {
        throw DataFileException(filename,
                                "Reading from network is not supported for this file type.");
    }
    else {
        FileInformation specInfo(filename);
        filename = specInfo.getAbsoluteFilePath();
    }
    checkFileReadability(filename);
    
    this->setFileName(filename);
    
    try {
        MetaVolumeFileXmlStreamReader streamReader;
        streamReader.readFile(filename,
                              this);
    }
    catch (const DataFileException& e) {
        DataFileException dfe(filename,
                              e.whatString());
        CaretLogThrowing(dfe);
        throw dfe;
    }
    
    this->setFileName(filename);
    
    this->clearModified();
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
MetaVolumeFile::writeFile(const AString& filename)
{
    try {
        setFileName(filename);
        MetaVolumeFileXmlStreamWriter writer;
        writer.writeFile(this);
    }
    catch (const DataFileException& e) {
        DataFileException dfe(filename,
                              e.whatString());
        CaretLogThrowing(dfe);
        throw dfe;
    }
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
MetaVolumeFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
MetaVolumeFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
MetaVolumeFile::getFileMetaData()
{
    return m_fileMapInfo->getMetaData();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
MetaVolumeFile::getFileMetaData() const
{
    return m_fileMapInfo->getMetaData();
}

/**
 * @return Is the data mappable to a surface?
 */
bool 
MetaVolumeFile::isSurfaceMappable() const
{
    return false;
}

/**
 * @return Is the data mappable to a volume?
 */
bool 
MetaVolumeFile::isVolumeMappable() const
{
    return true;
}

/**
 * @return The number of maps in the file.
 * Note: Caret5 used the term 'columns'.
 */
int32_t 
MetaVolumeFile::getNumberOfMaps() const
{
    if ( ! m_volumeFiles.empty()) {
        CaretAssertVectorIndex(m_volumeFiles, 0);
        return m_volumeFiles[0]->getNumberOfMaps();
    }
    return 0;
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
MetaVolumeFile::getMapName(const int32_t mapIndex) const
{
    AString name;
    
    /*
     * m_mapInfo[].m_mapName is only set if the default
     * map name is overwritten by the user
     */
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    if ( ! m_mapInfo[mapIndex]->getMapName().isEmpty()) {
        name = m_mapInfo[mapIndex]->getMapName();
    }
    
    if (name.isEmpty()) {
        /*
         * Use map name from first file
         */
        if ( ! m_volumeFiles.empty()) {
            const VolumeFile* vf(m_volumeFiles[0].get());
            if ((mapIndex >= 0)
                && (mapIndex < vf->getNumberOfMaps())) {
                name = vf->getMapName(mapIndex);
            }
        }
    }
    
    if (name.isEmpty()) {
        /*
         * Generic map name is index of map
         */
        name = ("Map " + AString::number(mapIndex + 1));
    }
    
    return name;
}

/**
 * @return The map name from in 'MapInfo'.  This is used only when the
 * MetaVolumeFile is written.  This map name in MapInfo is normally empty
 * and is only non-empty if the user has overwritten the map name.
 */
AString
MetaVolumeFile::getMapNameFromMapInfo(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getMapName();
}

/**
 * Set the name of the map at the given index.
 *
 * If the file does not have map attributes (hasMapAttributes())
 * calling this method will have not change the file.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mapName
 *    New name for the map.
 */
void 
MetaVolumeFile::setMapName(const int32_t mapIndex,
                           const AString& mapName)
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    if (mapName != getMapName(mapIndex)) {
        m_mapInfo[mapIndex]->setMapName(mapName);
        setModified();
    }
}

/**
 * Get the metadata for the map at the given index
 *
 * If the file does not have map attributes (hasMapAttributes())
 * a valid metadata object will be returned but changing its
 * content will have no effect on the file.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map (const value).
 */
const GiftiMetaData* 
MetaVolumeFile::getMapMetaData(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getMetaData();
}

/**
 * Get the metadata for the map at the given index
 *
 * If the file does not have map attributes (hasMapAttributes())
 * a valid metadata object will be returned but changing its
 * content will have no effect on the file.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map (const value).
 */
GiftiMetaData* 
MetaVolumeFile::getMapMetaData(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getMetaData();
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
MetaVolumeFile::getMapUniqueID(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getMetaData()->getUniqueID();
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool 
MetaVolumeFile::isMappedWithPalette() const
{
    return true;
}

/**
 * @return The estimated size of data after it is uncompressed
 * and loaded into RAM.  A negative value indicates that the
 * file size cannot be computed.
 */
int64_t 
MetaVolumeFile::getDataSizeUncompressedInBytes() const
{
    int64_t dataSize(0);
    for (const auto& vf : m_volumeFiles) {
        dataSize += vf->getDataSizeUncompressedInBytes();
    }
    return dataSize;
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
MetaVolumeFile::getMapFastStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getFastStatistics();
}

/**
 * Get a histogram for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Histogram for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram* 
MetaVolumeFile::getMapHistogram(const int32_t mapIndex)
{
    int32_t numberOfBuckets = 0;
    switch (getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            numberOfBuckets = getFileHistogramNumberOfBuckets();
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
            break;
    }
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getHistogram(numberOfBuckets);
}

/**
 * Get a histogram for the map at the given index of data
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
 *    Histogram for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram* 
MetaVolumeFile::getMapHistogram(const int32_t mapIndex,
                                const float mostPositiveValueInclusive,
                                const float leastPositiveValueInclusive,
                                const float leastNegativeValueInclusive,
                                const float mostNegativeValueInclusive,
                                const bool includeZeroValues)
{
    int32_t numberOfBuckets = 0;
    switch (getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            numberOfBuckets = getFileHistogramNumberOfBuckets();
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
            break;
    }

    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getHistogramLimitedValues(numberOfBuckets,
                                                          mostPositiveValueInclusive,
                                                          leastPositiveValueInclusive,
                                                          leastNegativeValueInclusive,
                                                          mostNegativeValueInclusive,
                                                          includeZeroValues);
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
MetaVolumeFile::getFileFastStatistics()
{
    return m_fileMapInfo->getFastStatistics();
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
MetaVolumeFile::getFileHistogram()
{
    const int32_t numBuckets = getFileHistogramNumberOfBuckets();
    return m_fileMapInfo->getHistogram(numBuckets);
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
MetaVolumeFile::getFileHistogram(const float mostPositiveValueInclusive,
                                          const float leastPositiveValueInclusive,
                                          const float leastNegativeValueInclusive,
                                          const float mostNegativeValueInclusive,
                                          const bool includeZeroValues)
{
    const int32_t numBuckets = getFileHistogramNumberOfBuckets();
    return m_fileMapInfo->getHistogramLimitedValues(numBuckets,
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
MetaVolumeFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getPaletteColorMapping();
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
MetaVolumeFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapInfo, mapIndex);
    return m_mapInfo[mapIndex]->getPaletteColorMapping();
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool 
MetaVolumeFile::isMappedWithLabelTable() const
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
MetaVolumeFile::getMapLabelTable(const int32_t /*mapIndex*/)
{
    return NULL;
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
const GiftiLabelTable* 
MetaVolumeFile::getMapLabelTable(const int32_t /*mapIndex*/) const
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
MetaVolumeFile::getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const
{
    modesSupportedOut.clear();
    
    modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
    modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
}

/**
 * Update coloring for a map.
 *
 * @param mapIndex
 *    Index of map.
 * @param paletteFile
 *    Palette file containing palettes.
 */
void 
MetaVolumeFile::updateScalarColoringForMap(const int32_t mapIndex)
{
    for (auto& vf : m_volumeFiles) {
        vf->updateScalarColoringForMap(mapIndex);
    }
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
MetaVolumeFile::getDataForSelector(const MapFileDataSelector& /*mapFileDataSelector*/,
                                   std::vector<float>& dataOut) const
{
    dataOut.clear();
    CaretLogSevere("MetaVolumeFile::getDataForSelector() not immplemented");
}

/*
 * Are all brainordinates in this file also in the given file?
 * That is, the brainordinates are equal to or a subset of the brainordinates
 * in the given file.
 *
 * @param mapFile
 *     The given map file.
 * @return
 *     Match status.
 */
CaretMappableDataFile::BrainordinateMappingMatch
MetaVolumeFile::getBrainordinateMappingMatchImplementation(const CaretMappableDataFile* mapFile) const
{
    CaretAssert(mapFile);
    
    const std::array<int64_t, 3> myDims(getVolumeFilesMatchingDimension());
    if (myDims[0] < 0) {
        return BrainordinateMappingMatch::NO;
    }
    
    const VolumeFile* otherVolumeFile(dynamic_cast<const VolumeFile*>(mapFile));
    if (otherVolumeFile != NULL) {
        int64_t dimI, dimJ, dimK, dimTime, dimComp;
        otherVolumeFile->getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
        std::array<int64_t, 3> otherDims { dimI, dimJ, dimK };
        if (myDims == otherDims) {
            return BrainordinateMappingMatch::EQUAL;
        }
    }
    else {
        const MetaVolumeFile* otherMetaVolumeFile(dynamic_cast<const MetaVolumeFile*>(mapFile));
        if (otherMetaVolumeFile != NULL) {
            const std::array<int64_t, 3> otherDims(otherMetaVolumeFile->getVolumeFilesMatchingDimension());
            if (myDims == otherDims) {
                return BrainordinateMappingMatch::EQUAL;
            }
        }
    }
    
    return BrainordinateMappingMatch::NO;
}

/**
 * @return If all volume files have the same IJK dimensions, return the IJK dimensions in an array.
 * If not, return an array of -1.
 */
std::array<int64_t, 3>
MetaVolumeFile::getVolumeFilesMatchingDimension() const
{
    std::array<int64_t, 3> dimsAll;
    dimsAll.fill(-1);
    
    bool firstFlag(true);
    for (const auto& vf : m_volumeFiles) {
        int64_t dimI, dimJ, dimK, dimTime, dimComp;
        vf->getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
        std::array<int64_t, 3> dims { dimI, dimJ, dimK };
        if (firstFlag) {
            firstFlag = false;
            dimsAll   = dims;
        }
        else {
            if (dimsAll != dims) {
                dimsAll.fill(-1);
                return dimsAll;
            }
        }
    }
    
    return dimsAll;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
MetaVolumeFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretMappableDataFile::addToDataFileContentInformation(dataFileInformation);
    
    for (auto& vf : m_volumeFiles) {
        vf->addToDataFileContentInformation(dataFileInformation);
    }
}

/**
 * @return Names (absolute path) of all child data files of this file.
 * This includes the CZI Image Files, Distance File, and the Non-Linear
 * Transform Files.
 */
std::vector<AString>
MetaVolumeFile::getChildDataFilePathNames() const
{
    std::vector<AString> childDataFilePathNames;
    
    for (auto& vf : m_volumeFiles) {
        childDataFilePathNames.push_back(vf->getFileName());
    }

    return childDataFilePathNames;
}

/**
 * Get the data for a map.  Note a map's data is the summation of the data
 * for the given map index in ALL volume files.
 * 
 * @param mapIndex
 *    Index of map
 * @param data
 *    Output with data for the map
 */
void
MetaVolumeFile::getMapData(const int32_t index,
                           std::vector<float>& dataOut) const
{
    dataOut.clear();
    
    std::unique_ptr<DataSliceArray> dataSliceArray(getDataSliceArrayForMap(index));
    dataSliceArray->getAllData(dataOut);
}

/**
 * @return The DataSliceArray for the given map index.
 * @param mapIndex
 *    Index of the map.
 */
std::unique_ptr<DataSliceArray>
MetaVolumeFile::getDataSliceArrayForMap(const int32_t mapIndex) const
{
    std::unique_ptr<DataSliceArray> dataSliceArray(new DataSliceArray());
    
    if (mapIndex == s_ALL_FILE_DATA_MAP_INDEX) {
        /**
         * Data from all maps in all files
         */
        for (const auto& vf : m_volumeFiles) {
            const int32_t numMaps(vf->getNumberOfMaps());
            for (int32_t imap = 0; imap < numMaps; imap++) {
                const float* frame(vf->getFrame(imap));
                int64_t dimI, dimJ, dimK, dimTime, dimComp;
                vf->getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
                const int64_t numVoxels(dimI * dimJ * dimK);
                dataSliceArray->addDataSlice(frame,
                                             numVoxels);
            }
        }

        return dataSliceArray;
    }
    
    /**
     * Data from one map in each files
     */
    for (const auto& vf : m_volumeFiles) {
        const float* frame(vf->getFrame(mapIndex));
        int64_t dimI, dimJ, dimK, dimTime, dimComp;
        vf->getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
        const int64_t numVoxels(dimI * dimJ * dimK);
        dataSliceArray->addDataSlice(frame,
                                     numVoxels);
    }
    
    return dataSliceArray;
}

/**
 * Pre-color all maps to save time when user is sequencing through the maps in the GUI
 */
void
MetaVolumeFile::preColorAllMaps()
{
    const int32_t numFiles(getNumberOfVolumeFiles());
    for (int32_t iVolume = 0; iVolume < numFiles; iVolume++) {
        VolumeFile* vf(getVolumeFile(iVolume));
        if (vf != NULL) {
            vf->preColorAllMaps();
        }
    }
}


/* ==============================================================================================*/

/**
 * Constructor.
 * @param parentMetaVolumeFile
 *    The parent meta-volume file
 * @param mapIndex
 *    Index of map
 */
MetaVolumeFile::MapInfo::MapInfo(MetaVolumeFile* parentMetaVolumeFile,
                                 const int32_t mapIndex)
: 
m_parentMetaVolumeFile(parentMetaVolumeFile),
m_mapIndex(mapIndex)
{
    m_fastStatistics.reset();
    m_histogram.reset();
    m_histogramLimitedValues.reset();
    m_paletteColorMapping.reset(new PaletteColorMapping());
    m_metadata.reset(new GiftiMetaData());
    m_mapName.clear();
}

/**
 * Destructor.
 */
MetaVolumeFile::MapInfo::~MapInfo()
{
    
}

/**
 * Clear modifed status of this instance
 */
void
MetaVolumeFile::MapInfo::clearModified()
{
    CaretObjectTracksModification::clearModified();
    m_metadata->clearModified();
    m_paletteColorMapping->clearModified();
}

/**
 * @return True if this instance is modifed with the exception of palette color mapping
 */
bool
MetaVolumeFile::MapInfo::isModifiedExcludingPaletteColorMapping() const
{
    if (m_metadata->isModified()) {
        return true;
    }
    return false;
}

/**
 * @return True if this instance is modifed with the exception of palette color mapping
 */
bool
MetaVolumeFile::MapInfo::isModifiedPaletteColorMapping() const
{
    return m_paletteColorMapping->isModified();
}

/**
 * Invalidate the coloring (usually due to palette or data changes).
 */
void
MetaVolumeFile::MapInfo::updateForChangeInMapData()
{
    m_fastStatistics.reset();
    m_histogram.reset();
    m_histogramLimitedValues.reset();
}

/**
 * @return True if fast statistics is valid, else false.
 */
bool
MetaVolumeFile::MapInfo::isFastStatisticsValid() const
{
    return (m_fastStatistics.get() != NULL);
}

/**
 * @return Fast statistics for the map
 */
const FastStatistics*
MetaVolumeFile::MapInfo::getFastStatistics() const
{
    if (m_fastStatistics == NULL) {
        m_fastStatistics.reset(new FastStatistics());
        std::unique_ptr<DataSliceArray> dataSliceArray(m_parentMetaVolumeFile->getDataSliceArrayForMap(m_mapIndex));
        if (dataSliceArray) {
            m_fastStatistics->update(*dataSliceArray.get());
        }
    }
    
    return m_fastStatistics.get();
}

/**
 * @param numberOfBuckets,
 *    Number of buckets in the histogram.
 * @return True if histogram is valid, else false.
 */
bool
MetaVolumeFile::MapInfo::isHistogramValid(const int32_t numberOfBuckets) const
{
    return ((m_histogram != NULL)
            && (m_histogramNumberOfBuckets == numberOfBuckets));
}

/**
 * @return Histogram for the given map
 * @param numberOfBuckets
 *    Number of buckets for histogram
 */
const Histogram*
MetaVolumeFile::MapInfo::getHistogram(const int32_t numberOfBuckets) const
{
    if (m_histogramNumberOfBuckets != numberOfBuckets) {
        m_histogram.reset();
    }
    if (m_histogram == NULL) {
        m_histogram.reset(new Histogram(numberOfBuckets));
        std::unique_ptr<DataSliceArray> dataSliceArray(m_parentMetaVolumeFile->getDataSliceArrayForMap(m_mapIndex));
        if (dataSliceArray) {
            m_histogram->update(numberOfBuckets,
                                *dataSliceArray.get());
        }
        m_histogramNumberOfBuckets = numberOfBuckets;
    }
    
    return m_histogram.get();
}

/**
 * Is limited values histogram valid?  Is is valid when it exists and
 * the limited value parameters have not changed.
 *
 * @param numberOfBuckets
 *    Number of buckets in the histogram.
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
 *
 * @return true if valid, else false.
 */
bool
MetaVolumeFile::MapInfo::isHistogramLimitedValuesValid(const int32_t numberOfBuckets,
                                                                 const float mostPositiveValueInclusive,
                                                                 const float leastPositiveValueInclusive,
                                                                 const float leastNegativeValueInclusive,
                                                                 const float mostNegativeValueInclusive,
                                                                 const bool includeZeroValues) const
{
    if (m_histogramLimitedValues == NULL) {
        return false;
    }
    else if ((numberOfBuckets != m_histogramLimitedValuesNumberOfBuckets)
             || (mostPositiveValueInclusive != m_histogramLimitedValuesMostPositiveValueInclusive)
             || (leastPositiveValueInclusive != m_histogramLimitedValuesLeastPositiveValueInclusive)
             || (leastNegativeValueInclusive != m_histogramLimitedValuesLeastNegativeValueInclusive)
             || (mostNegativeValueInclusive != m_histogramLimitedValuesMostNegativeValueInclusive)
             || (includeZeroValues != m_histogramLimitedValuesIncludeZeroValues)) {
        return false;
    }
    
    return true;
}

/**
 * @return Histogram with limited values
 *
 * @param numberOfBuckets
 *    Number of buckets in the histogram.
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
 *
 * @return true if valid, else false.
 */
const Histogram*
MetaVolumeFile::MapInfo::getHistogramLimitedValues(const int32_t numberOfBuckets,
                                                   const float mostPositiveValueInclusive,
                                                   const float leastPositiveValueInclusive,
                                                   const float leastNegativeValueInclusive,
                                                   const float mostNegativeValueInclusive,
                                                   const bool includeZeroValues) const
{
    if ((numberOfBuckets != m_histogramLimitedValuesNumberOfBuckets)
        || (mostPositiveValueInclusive != m_histogramLimitedValuesMostPositiveValueInclusive)
        || (leastPositiveValueInclusive != m_histogramLimitedValuesLeastPositiveValueInclusive)
        || (leastNegativeValueInclusive != m_histogramLimitedValuesLeastNegativeValueInclusive)
        || (mostNegativeValueInclusive != m_histogramLimitedValuesMostNegativeValueInclusive)
        || (includeZeroValues != m_histogramLimitedValuesIncludeZeroValues)) {
        m_histogramLimitedValues.reset();
    }
    if (m_histogramLimitedValues == NULL) {
        m_histogramLimitedValues.reset(new Histogram(numberOfBuckets));
        std::unique_ptr<DataSliceArray> dataSliceArray(m_parentMetaVolumeFile->getDataSliceArrayForMap(m_mapIndex));
        if (dataSliceArray) {
            m_histogramLimitedValues->update(numberOfBuckets,
                                             *dataSliceArray.get());
        }
        m_histogramNumberOfBuckets = numberOfBuckets;
        m_histogramLimitedValuesMostPositiveValueInclusive = mostPositiveValueInclusive;
        m_histogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
        m_histogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
        m_histogramLimitedValuesMostNegativeValueInclusive = mostNegativeValueInclusive;
        m_histogramLimitedValuesIncludeZeroValues = includeZeroValues;
    }
    
    return m_histogramLimitedValues.get();
}

/**
 * @return The GIFTI metadata
 */
GiftiMetaData*
MetaVolumeFile::MapInfo::getMetaData()
{
    return m_metadata.get();
}

/**
 * @return The GIFTI metadata (const)
 */
const GiftiMetaData*
MetaVolumeFile::MapInfo::getMetaData() const
{
    return m_metadata.get();
}

/**
 * @return The Palette Color Mapping
 */
PaletteColorMapping*
MetaVolumeFile::MapInfo::getPaletteColorMapping()
{
    return m_paletteColorMapping.get();
}

/**
 * @return The Palette Color Mapping (const)
 */
const PaletteColorMapping*
MetaVolumeFile::MapInfo::getPaletteColorMapping() const
{
    return m_paletteColorMapping.get();
}

/**
 * @return The map name
 */
AString
MetaVolumeFile::MapInfo::getMapName() const
{
    return m_mapName;
}

/*
 * Set the map name
 * @param mapName
 *
 * @param mapName
 * New name for map
 */
void
MetaVolumeFile::MapInfo::setMapName(const AString& mapName)
{
    if (mapName != m_mapName) {
        m_mapName = mapName;
        setModified();
    }
}

