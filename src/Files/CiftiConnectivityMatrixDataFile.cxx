
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

#include <cmath>

#define __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__
#include "CiftiConnectivityMatrixDataFile.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_DECLARE__

#include "CiftiFile.h"
#include "CaretLogger.h"
#include "CiftiInterface.h"
#include "CiftiXnat.h"
#include "DescriptiveStatistics.h"
#include "FastStatistics.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "SceneClass.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::CiftiConnectivityMatrixDataFile 
 * \brief Abstract class for connectivity matrix type files.
 * \ingroup Files
 *
 * This connectivity matrix base class CiftiConnectivityMatrixDataFile::is designed for Cifti files
 * that contain a connectivity matrix that typically contains correlation
 * values that indicate the strength of connectivity from one (or a group)
 * of brainordinates to one (or a group) of brainordinates.
 *
 * For these file, each matrix CiftiConnectivityMatrixDataFile::is a "map".
 */

/**
 * Constructor.
 */
CiftiConnectivityMatrixDataFile::CiftiConnectivityMatrixDataFile(const DataFileTypeEnum::Enum dataFileType)
: CaretMappableDataFile(dataFileType)
{
    clearPrivate();
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixDataFile::~CiftiConnectivityMatrixDataFile()
{
    clearPrivate();
}

/**
 * Clear the contents of the file.
 */
void
CiftiConnectivityMatrixDataFile::clear()
{
    CaretMappableDataFile::clear();
}

/**
 * Clear the contents of the file.
 * Note that "clear()" is virtual and cannot be called from destructor.
 */
void
CiftiConnectivityMatrixDataFile::clearPrivate()
{
    if (m_ciftiInterface != NULL) {
        m_ciftiInterface.grabNew(NULL);
    }
    m_ciftiXML = NULL; // do not delete since it points to data that will be deleted
    m_mapContent.clear();
    CaretPointer<MapContent> mc(new MapContent());
    m_mapContent.push_back(mc);
}

/**
 * @return Is this file empty?
 */
bool
CiftiConnectivityMatrixDataFile::isEmpty() const
{
    if (getNumberOfMaps() > 0) {
        return false;
    }
    return true;
}

/**
 * @return structure file maps to.
 */
StructureEnum::Enum
CiftiConnectivityMatrixDataFile::getStructure() const
{
    /*
     * CIFTI files apply to all structures.
     */
    return StructureEnum::ALL;
}

/**
 * Set the structure to which file maps.
 * @param structure
 *    New structure to which file maps.
 */
void
CiftiConnectivityMatrixDataFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* CIFTI files may apply to all structures */
}

/**
 * @return Metadata for the file.
 */
GiftiMetaData*
CiftiConnectivityMatrixDataFile::getFileMetaData()
{
    return m_fileMetaData;
}

/**
 * @return Metadata for the file.
 */
const GiftiMetaData*
CiftiConnectivityMatrixDataFile:: getFileMetaData() const
{
    return m_fileMetaData;
}

/**
 * Read the file.
 * 
 * @param filename
 *    Name of the file to read.
 * @throw
 *    DataFileException if there is an error reading the file.
 */
void
CiftiConnectivityMatrixDataFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();
    
    try {
        if (DataFile::isFileOnNetwork(filename)) {
            CiftiXnat* ciftiXnat = new CiftiXnat();
            const AString username = CaretDataFile::getFileReadingUsername();
            if (username.isEmpty() == false) {
                ciftiXnat->setAuthentication(filename,
                                             username,
                                             CaretDataFile::getFileReadingPassword());
            }
            ciftiXnat->openURL(filename);
            m_ciftiInterface.grabNew(ciftiXnat);
        }
        else {
            /*
             * Dense files is VERY large, so read data from the file
             * only as needed.
             */
            CiftiFile* ciftiFile = new CiftiFile();
            if (getDataFileType() == DataFileTypeEnum::CONNECTIVITY_DENSE) {
                ciftiFile->openFile(filename,
                                    ON_DISK);
            }
            else {
                ciftiFile->openFile(filename,
                                    IN_MEMORY);
            }
            m_ciftiInterface.grabNew(ciftiFile);
        }
        
        setFileName(filename);
    }
    catch (CiftiFileException& e) {
        clear();
        throw DataFileException(e.whatString());
    }
    
    if (m_ciftiInterface != NULL) {
        m_ciftiXML = const_cast<CiftiXML*>(&m_ciftiInterface->getCiftiXML());
        
        const IndicesMapToDataType rowType = m_ciftiXML->getMappingType(CIFTI_INDEX_LOADING);
        const IndicesMapToDataType colType = m_ciftiXML->getMappingType(CIFTI_INDEX_VIEWING);
        const AString msg = ("CIFTI Connectivity File: " + getFileNameNoPath() + "\n"
                             "Rows=" + AString::number(m_ciftiInterface->getNumberOfRows())
                             + "Columns=" + AString::number(m_ciftiInterface->getNumberOfColumns())
                             + "RowType=" + AString::number(rowType)
                             + "ColType=" + AString::number(colType));
        CaretLogFine(msg);
    }
}

/**
 * Write the file.
 *
 * @param filename
 *    Name of the file to write.
 * @throw
 *    DataFileException if there is an error writing the file.
 */
void
CiftiConnectivityMatrixDataFile::writeFile(const AString& filename) throw (DataFileException)
{
    throw DataFileException("Writing of file type "
                            + DataFileTypeEnum::toName(getDataFileType())
                            + " named "
                            + filename
                            + " is not implemented");
}

/**
 * @return Is the data mappable to a surface?
 */
bool
CiftiConnectivityMatrixDataFile::isSurfaceMappable() const
{
    return true;
}

/**
 * @return Is the data mappable to a volume?
 */
bool
CiftiConnectivityMatrixDataFile::isVolumeMappable() const
{
    return true;
}

/**
 * @return The number of maps in the file.
 * Note: Caret5 used the term 'columns'.
 */
int32_t
CiftiConnectivityMatrixDataFile::getNumberOfMaps() const
{
    return 1;
//    return m_mapContent.size();
}

/**
 * Get the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString CiftiConnectivityMatrixDataFile::getMapName(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    return m_mapContent[mapIndex]->m_name;
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
CiftiConnectivityMatrixDataFile::setMapName(const int32_t mapIndex,
                        const AString& mapName)
{
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    m_mapContent[mapIndex]->m_name = mapName;
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
CiftiConnectivityMatrixDataFile::getMapMetaData(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    return m_mapContent[mapIndex]->m_metaData;
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
CiftiConnectivityMatrixDataFile::getMapMetaData(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent, mapIndex);
    return m_mapContent[mapIndex]->m_metaData;
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
CiftiConnectivityMatrixDataFile::getMapUniqueID(const int32_t mapIndex) const
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
CiftiConnectivityMatrixDataFile::isMappedWithPalette() const
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
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const DescriptiveStatistics*
CiftiConnectivityMatrixDataFile::getMapStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    DescriptiveStatistics* ds = m_mapContent[mapIndex]->m_descriptiveStatistics;
    if (m_mapContent[mapIndex]->m_data.empty()) {
        ds->update(NULL,
                   0);
    }
    else {
        ds->update(&m_mapContent[mapIndex]->m_data[0],
                   m_mapContent[mapIndex]->m_data.size());
    }
    return ds;
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
CiftiConnectivityMatrixDataFile::getMapFastStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    const FastStatistics* fs = m_mapContent[mapIndex]->getFastStatistics();
    return fs;
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
CiftiConnectivityMatrixDataFile::getMapHistogram(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    Histogram* h = m_mapContent[mapIndex]->m_histogram;
    if (m_mapContent[mapIndex]->m_data.empty()) {
        h->update(NULL,
                   0);
    }
    else {
        h->update(&m_mapContent[mapIndex]->m_data[0],
                   m_mapContent[mapIndex]->m_data.size());
    }
    return h;
}

/**
 * Get statistics describing the distribution of data
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
const DescriptiveStatistics*
CiftiConnectivityMatrixDataFile::getMapStatistics(const int32_t mapIndex,
                                                      const float mostPositiveValueInclusive,
                                                      const float leastPositiveValueInclusive,
                                                      const float leastNegativeValueInclusive,
                                                      const float mostNegativeValueInclusive,
                                                      const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    DescriptiveStatistics* ds = m_mapContent[mapIndex]->m_descriptiveStatistics;
    if (m_mapContent[mapIndex]->m_data.empty()) {
        ds->update(NULL,
                   0);
    }
    else {
        ds->update(&m_mapContent[mapIndex]->m_data[0],
                   m_mapContent[mapIndex]->m_data.size(),
                   mostPositiveValueInclusive,
                   leastPositiveValueInclusive,
                   leastNegativeValueInclusive,
                   mostNegativeValueInclusive,
                   includeZeroValues);
    }
    return ds;
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
CiftiConnectivityMatrixDataFile::getMapHistogram(const int32_t mapIndex,
                                         const float mostPositiveValueInclusive,
                                         const float leastPositiveValueInclusive,
                                         const float leastNegativeValueInclusive,
                                         const float mostNegativeValueInclusive,
                                         const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    Histogram* h = m_mapContent[mapIndex]->m_histogram;
    if (m_mapContent[mapIndex]->m_data.empty()) {
        h->update(NULL,
                  0);
    }
    else {
        h->update(&m_mapContent[mapIndex]->m_data[0],
                  m_mapContent[mapIndex]->m_data.size(),
                  mostPositiveValueInclusive,
                  leastPositiveValueInclusive,
                  leastNegativeValueInclusive,
                  mostNegativeValueInclusive,
                  includeZeroValues);
    }
    return h;    
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
CiftiConnectivityMatrixDataFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_paletteColorMapping;
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (constant) (will be NULL for data
 *    not mapped using a palette).
 */
const PaletteColorMapping*
CiftiConnectivityMatrixDataFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_paletteColorMapping;
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool
CiftiConnectivityMatrixDataFile::isMappedWithLabelTable() const
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
CiftiConnectivityMatrixDataFile::getMapLabelTable(const int32_t /*mapIndex*/)
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
const
GiftiLabelTable* CiftiConnectivityMatrixDataFile::getMapLabelTable(const int32_t /*mapIndex*/) const
{
    return NULL;
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
CiftiConnectivityMatrixDataFile::updateScalarColoringForMap(const int32_t mapIndex,
                                        const PaletteFile* paletteFile)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    m_mapContent[mapIndex]->updateColoring(mapIndex,
                                           paletteFile);
}

/**
 * Get the volume data for a map.
 *
 * @param mapIndex
 *    Index of map.
 * @return Volume data for the map.  May be NULL if there is no
 * volume data for map.
 */
VolumeFile*
CiftiConnectivityMatrixDataFile::getMapVolume(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_volumeFile;    
}

/**
 * Get the volume data for a map.
 *
 * @param mapIndex
 *    Index of map.
 * @return Volume data for the map.  May be NULL if there is no
 * volume data for map.
 */
const VolumeFile*
CiftiConnectivityMatrixDataFile::getMapVolume(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_volumeFile;
}

/**
 * @return Is loading of data enabled.  Note that if
 * disabled, any previously loaded data is NOT removed
 * so that it can still be displayed but not updated.
 */
bool
CiftiConnectivityMatrixDataFile::isMapDataLoadingEnabled(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_dataLoadingEnabled;
}

/**
 * Set loading of data enabled.  Note that if
 * disabled, any previously loaded data is NOT removed
 * so that it can still be displayed but not updated.
 *
 * @param dataLoadingEnabled
 *   New data loading enabled status.
 */
void
CiftiConnectivityMatrixDataFile::setMapDataLoadingEnabled(const int32_t mapIndex,
                                                       const bool dataLoadingEnabled)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    m_mapContent[mapIndex]->m_dataLoadingEnabled = dataLoadingEnabled;
}

/**
 * Load connectivity data for the surface's node.
 *
 * For a dense connectivity file, the data loaded is
 * the connectivity from the node to other brainordinates.
 * For a dense time series file, the data loaded is the
 * time-series for this node.
 *
 * @param surfaceFile
 *    Surface file used for structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndex
 *    Index of node number.
 * @return
 *    Index of the row that was loaded from the CIFTI file (negative if invalid).
 */
int64_t
CiftiConnectivityMatrixDataFile::loadMapDataForSurfaceNode(const int32_t mapIndex,
                                                           const int32_t surfaceNumberOfNodes,
                                                        const StructureEnum::Enum structure,
                                               const int32_t nodeIndex) throw (DataFileException)
{
    if (m_ciftiInterface == NULL) {
        throw DataFileException("Connectivity matrix file named \""
                                + getFileNameNoPath()
                                + "\" of type\""
                                + DataFileTypeEnum::toName(getDataFileType())
                                + "\" does not have a file loaded.");
    }
    

    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    MapContent* mapContent = m_mapContent[mapIndex];
    
    /*
     * Loading of data disabled?
     */
    if (mapContent->m_dataLoadingEnabled == false) {
        return -1;
    }
    
    int64_t rowIndex = -1;
    
    try {
        bool dataWasLoaded = false;
        
        rowIndex = getRowIndexForNodeWhenLoading(structure, surfaceNumberOfNodes, nodeIndex);
        
        if (rowIndex >= 0) {
            const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
            if (dataCount > 0) {
                std::vector<float> dataVector(dataCount);
                float* data = &dataVector[0];
                
                CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiInterface->getNumberOfRows()));
                m_ciftiInterface->getRow(data, rowIndex);
                mapContent->updateData(m_ciftiInterface,
                                       mapIndex,
                                       data,
                                       dataCount);
                CaretLogFine("Read row for node " + AString::number(nodeIndex));
                
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read row for node " + AString::number(nodeIndex));
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
    
    return rowIndex;
}

/**
 * Get the index of a row when loading data for a voxel at a coordinate.
 * @param mapIndex
 *    Index of the map.
 * @param xyz
 *    Coordinate of the voxel.
 * @return
 *    Index of row corresponding to voxel or negative if no row in the
 *    matrix corresponds to the voxel.
 */
int64_t
CiftiConnectivityMatrixDataFile::getRowIndexForVoxelWhenLoading(const int32_t mapIndex,
                                                                const float xyz[3]) const
{
    int64_t rowIndex = -1;
    
    /*
     * Get the mapping type
     */
    const IndicesMapToDataType rowMappingType = m_ciftiXML->getMappingType(CIFTI_INDEX_LOADING);
    
        bool isBrainModels = false;
        bool isParcels     = false;
        switch (rowMappingType) {
            case CIFTI_INDEX_TYPE_BRAIN_MODELS:
                isBrainModels = true;
                break;
            case CIFTI_INDEX_TYPE_FIBERS:
                break;
            case CIFTI_INDEX_TYPE_INVALID:
                break;
            case CIFTI_INDEX_TYPE_LABELS:
                break;
            case CIFTI_INDEX_TYPE_PARCELS:
                isParcels = true;
                break;
            case CIFTI_INDEX_TYPE_SCALARS:
                break;
            case CIFTI_INDEX_TYPE_TIME_POINTS:
                break;
        }
        
        if (isBrainModels) {
            rowIndex = m_ciftiXML->getRowIndexForVoxelCoordinate(xyz);
        }
        else if (isParcels) {
            CaretAssertVectorIndex(m_mapContent,
                                   mapIndex);
            int64_t ijk[3];
            if (m_mapContent[mapIndex]->voxelXYZToIJK(xyz, ijk)) {
                rowIndex = m_ciftiXML->getColumnParcelForVoxel(ijk);
            }
        }
        else {
            CaretAssert(0);
            CaretLogSevere("Invalid row mapping type for connectivity file "
                           + DataFileTypeEnum::toName(getDataFileType()));
        }
    
    return rowIndex;
    
}

/**
 * Get the index of a row when loading data for a voxel at a coordinate.
 * @param mapIndex
 *    Index of the map.
 * @param xyz
 *    Coordinate of the voxel.
 * @return
 *    Index of row corresponding to voxel or negative if no row in the
 *    matrix corresponds to the voxel.
 */
int64_t
CiftiConnectivityMatrixDataFile::getColumnIndexForVoxelWhenViewing(const int32_t mapIndex,
                                                                   const float xyz[3]) const
{
    int64_t columnIndex = -1;
    
    /*
     * Get the mapping type
     */
    const IndicesMapToDataType columnMappingType = m_ciftiXML->getMappingType(CIFTI_INDEX_VIEWING);
    
    bool isBrainModels = false;
    bool isParcels     = false;
    switch (columnMappingType) {
        case CIFTI_INDEX_TYPE_BRAIN_MODELS:
            isBrainModels = true;
            break;
        case CIFTI_INDEX_TYPE_FIBERS:
            break;
        case CIFTI_INDEX_TYPE_INVALID:
            break;
        case CIFTI_INDEX_TYPE_LABELS:
            break;
        case CIFTI_INDEX_TYPE_PARCELS:
            isParcels = true;
            break;
        case CIFTI_INDEX_TYPE_SCALARS:
            break;
        case CIFTI_INDEX_TYPE_TIME_POINTS:
            break;
    }
    
    if (isBrainModels) {
        columnIndex = m_ciftiXML->getColumnIndexForVoxelCoordinate(xyz);
    }
    else if (isParcels) {
        CaretAssertVectorIndex(m_mapContent,
                               mapIndex);
        int64_t ijk[3];
        if (m_mapContent[mapIndex]->voxelXYZToIJK(xyz, ijk)) {
            columnIndex = m_ciftiXML->getRowParcelForVoxel(ijk);
        }
    }
    else {
        CaretAssert(0);
        CaretLogSevere("Invalid row mapping type for connectivity file "
                       + DataFileTypeEnum::toName(getDataFileType()));
    }
    
    return columnIndex;
    
}

/**
 * Get the index of a row when loading data for a surface node.
 * @param structure
 *    Structure of the surface.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the node.
 * @return
 *    Index of row corresponding to node or negative if no row in the 
 *    matrix corresponds to the node.
 */
int64_t
CiftiConnectivityMatrixDataFile::getRowIndexForNodeWhenLoading(const StructureEnum::Enum structure,
                                                               const int64_t surfaceNumberOfNodes,
                                                               const int64_t nodeIndex) const
{
    int64_t rowIndex = -1;
    
    /*
     * Get the mapping type
     */
    const IndicesMapToDataType rowMappingType = m_ciftiXML->getMappingType(CIFTI_INDEX_LOADING);
    
    if (m_ciftiXML->getSurfaceNumberOfNodes(CIFTI_INDEX_LOADING, structure) == surfaceNumberOfNodes) {
        bool isBrainModels = false;
        bool isParcels     = false;
        switch (rowMappingType) {
            case CIFTI_INDEX_TYPE_BRAIN_MODELS:
                isBrainModels = true;
                break;
            case CIFTI_INDEX_TYPE_FIBERS:
                break;
            case CIFTI_INDEX_TYPE_INVALID:
                break;
            case CIFTI_INDEX_TYPE_LABELS:
                break;
            case CIFTI_INDEX_TYPE_PARCELS:
                isParcels = true;
                break;
            case CIFTI_INDEX_TYPE_SCALARS:
                break;
            case CIFTI_INDEX_TYPE_TIME_POINTS:
                break;
        }
        
        if (isBrainModels) {
            rowIndex = m_ciftiXML->getRowIndexForNode(nodeIndex, structure);
        }
        else if (isParcels) {
            rowIndex = m_ciftiXML->getColumnParcelForNode(nodeIndex,
                                                          structure);
        }
        else {
            CaretAssert(0);
            CaretLogSevere("Invalid row mapping type for connectivity file "
                           + DataFileTypeEnum::toName(getDataFileType()));
        }
    }
    
    return rowIndex;
}

/**
 * Get the index of a column when loading data for a surface node.
 * @param structure
 *    Structure of the surface.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the node.
 * @return
 *    Index of column corresponding to node or negative if no column in the
 *    matrix corresponds to the node.
 */
int64_t
CiftiConnectivityMatrixDataFile::getColumnIndexForNodeWhenViewing(const StructureEnum::Enum structure,
                                                                  const int64_t surfaceNumberOfNodes,
                                                                  const int64_t nodeIndex) const
{
    int64_t columnIndex = -1;
    
    /*
     * Get the mapping type
     */
    const IndicesMapToDataType columnMappingType = m_ciftiXML->getMappingType(CIFTI_INDEX_VIEWING);
    
    if (m_ciftiXML->getSurfaceNumberOfNodes(CIFTI_INDEX_VIEWING, structure) == surfaceNumberOfNodes) {
        bool isBrainModels = false;
        bool isParcels     = false;
        switch (columnMappingType) {
            case CIFTI_INDEX_TYPE_BRAIN_MODELS:
                isBrainModels = true;
                break;
            case CIFTI_INDEX_TYPE_FIBERS:
                break;
            case CIFTI_INDEX_TYPE_INVALID:
                break;
            case CIFTI_INDEX_TYPE_LABELS:
                break;
            case CIFTI_INDEX_TYPE_PARCELS:
                isParcels = true;
                break;
            case CIFTI_INDEX_TYPE_SCALARS:
                break;
            case CIFTI_INDEX_TYPE_TIME_POINTS:
                break;
        }
        
        if (isBrainModels) {
            columnIndex = m_ciftiXML->getColumnIndexForNode(nodeIndex, structure);
        }
        else if (isParcels) {
            columnIndex = m_ciftiXML->getRowParcelForNode(nodeIndex,
                                                          structure);
        }
        else {
            CaretAssert(0);
            CaretLogSevere("Invalid column mapping type for connectivity file "
                           + DataFileTypeEnum::toName(getDataFileType()));
        }
    }
    
    return columnIndex;    
}

/**
 * Load connectivity data for the surface's nodes and then average the data.
 *
 * For a dense connectivity file, the data loaded is
 * the connectivity from the node to other brainordinates.
 * For a dense time series file, the data loaded is the
 * time-series for this node.
 *
 * @param surfaceFile
 *    Surface file used for structure.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param structure
 *    Surface's structure.
 * @param nodeIndices
 *    Indices of nodes.
 */
void
CiftiConnectivityMatrixDataFile::loadMapAverageDataForSurfaceNodes(const int32_t mapIndex,
                                                                   const int32_t surfaceNumberOfNodes,
                                                                const StructureEnum::Enum structure,
                                                       const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    if (m_ciftiInterface == NULL) {
        throw DataFileException("Connectivity matrix file named \""
                                + getFileNameNoPath()
                                + "\" of type\""
                                + DataFileTypeEnum::toName(getDataFileType())
                                + "\" does not have a file loaded.");
    }
    
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    MapContent* mapContent = m_mapContent[mapIndex];
    
    /*
     * Loading of data disabled?
     */
    if (mapContent->m_dataLoadingEnabled == false) {
        return;
    }
    
    const int32_t numberOfNodeIndices = static_cast<int32_t>(nodeIndices.size());
    if (numberOfNodeIndices <= 0) {
        return;
    }
    
    try {
        bool dataWasLoaded = false;
        
        const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
        if (dataCount > 0) {
            /*
             * Contains the average row
             */
            std::vector<float> dataAverageVector(dataCount, 0.0);
            float* dataAverage = &dataAverageVector[0];
            
            /*
             * Contains row for a node
             */
            std::vector<float> dataRowVector(dataCount, 0.0);
            float* dataRow = &dataRowVector[0];
            
            int64_t rowSuccessCount = 0;
            
            /*
             * Read rows for each node
             */
            for (int32_t i = 0; i < numberOfNodeIndices; i++) {
                const int32_t nodeIndex = nodeIndices[i];
                const int64_t rowIndex = getRowIndexForNodeWhenLoading(structure,
                                                                       surfaceNumberOfNodes,
                                                                       nodeIndex);
                
                if (rowIndex >= 0) {
                    CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiInterface->getNumberOfRows()));
                    m_ciftiInterface->getRow(dataRow, rowIndex);
                    
                    for (int64_t j = 0; j < dataCount; j++) {
                        dataAverage[j] += dataRow[j];
                    }
                    rowSuccessCount++;
                    
                    CaretLogFine("Read row for node " + AString::fromNumbers(nodeIndices, ","));
                }
                else {
                    CaretLogFine("Failed reading row for node " + AString::number(nodeIndex));
                }
            }
            
            if (rowSuccessCount > 0) {
                /*
                 * Average the data
                 */
                for (int64_t i = 0; i < dataCount; i++) {
                    dataAverage[i] /= rowSuccessCount;
                }
                
                /*
                 * Update the viewed data
                 */
                mapContent->updateData(m_ciftiInterface,
                                       mapIndex,
                                       dataAverage,
                                       dataCount);
                
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read rows for node average" + AString::fromNumbers(nodeIndices, ","));
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * For a dense connectivity file, the data loaded is
 * the connectivity from the voxel to other brainordinates.
 * For a dense time series file, the data loaded is the
 * time-series for this voxel.
 *
 * @param xyz
 *    Coordinate of voxel.
 */
int64_t
CiftiConnectivityMatrixDataFile::loadMapDataForVoxelAtCoordinate(const int32_t mapIndex,
                                                              const float xyz[3]) throw (DataFileException)
{
    if (m_ciftiInterface == NULL) {
        throw DataFileException("Connectivity matrix file named \""
                                + getFileNameNoPath()
                                + "\" of type\""
                                + DataFileTypeEnum::toName(getDataFileType())
                                + "\" does not have a file loaded.");
    }
    
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    MapContent* mapContent = m_mapContent[mapIndex];
    
    /*
     * Loading of data disabled?
     */
    if (mapContent->m_dataLoadingEnabled == false) {
        return -1;
    }
    
    int64_t rowIndex = -1;
    
    try {
        bool dataWasLoaded = false;
        
        int64_t rowIndex = getRowIndexForVoxelWhenLoading(mapIndex,
                                                          xyz);
        
        if (rowIndex >= 0) {
            const int64_t dataCount = m_ciftiInterface->getNumberOfColumns();
            if (dataCount > 0) {
                std::vector<float> dataVector(dataCount);
                float* data = &dataVector[0];
                
                CaretAssert((rowIndex >= 0) && (rowIndex < m_ciftiInterface->getNumberOfRows()));
                m_ciftiInterface->getRow(data, rowIndex);
                mapContent->updateData(m_ciftiInterface,
                                       mapIndex,
                                       data,
                                       dataCount);
                CaretLogFine("Read row for voxel " + AString::fromNumbers(xyz, 3, ","));
                
                dataWasLoaded = true;
            }
        }
        
        if (dataWasLoaded == false) {
            CaretLogFine("FAILED to read row for voxel " + AString::fromNumbers(xyz, 3, ","));
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
    
    return rowIndex;    
}

/**
 * Get connectivity value for a voxel at the given coordinate.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param valueOut
 *     Output containing the node's value, value only if true returned.0
 * @param textOut
 *     Text containing node value and for some types, the parcel.
 * @return
 *    true if a value was available for the voxel, else false.
 */
bool
CiftiConnectivityMatrixDataFile::getMapVolumeVoxelValue(const int32_t mapIndex,
                                                     const float xyz[3],
                                            int64_t ijkOut[3],
                                                        float &valueOut,
                                                        AString& textOut) const
{
    valueOut = 0.0;
    textOut = "";
    
    if (m_ciftiInterface == NULL) {
        throw DataFileException("Connectivity matrix file named \""
                                + getFileNameNoPath()
                                + "\" of type\""
                                + DataFileTypeEnum::toName(getDataFileType())
                                + "\" does not have a file loaded.");
    }
    
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    const MapContent* mapContent = m_mapContent[mapIndex];
    
    try {
        int64_t columnIndex = getColumnIndexForVoxelWhenViewing(mapIndex,
                                                                xyz);
        
        if ((columnIndex >= 0)
            && (columnIndex < mapContent->m_dataCount)) {
            valueOut = mapContent->m_data[columnIndex];
            
            textOut = AString::number(valueOut);
            
            std::vector<CiftiParcelElement> parcels;
            m_ciftiXML->getParcelsForColumns(parcels);
            textOut = AString::number(valueOut);
            if (columnIndex < static_cast<int32_t>(parcels.size())) {
                textOut += (" "
                            + parcels[columnIndex].m_parcelName);
            }
            
            mapContent->voxelXYZToIJK(xyz, ijkOut);
            
            return true;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
    
    return false;
}

/**
 * Get connectivity value for a surface's node.
 * @param structure
 *     Surface's structure.
 * @param nodeIndex
 *     Index of the node
 * @param numberOfNodes
 *     Number of nodes in the surface.
 * @param valueOut
 *     Output containing the node's value, value only if true returned.
 * @param textOut
 *     Text containing node value and for some types, the parcel.
 * @return
 *    true if a value was available for the node, else false.
 */
bool
CiftiConnectivityMatrixDataFile::getMapSurfaceNodeValue(const int32_t mapIndex,
                                                        const StructureEnum::Enum structure,
                                                        const int nodeIndex,
                                                        const int32_t numberOfNodes,
                                                        float& valueOut,
                                                        AString& textOut) const
{
    valueOut = 0.0;
    textOut = "";
    if (m_ciftiInterface == NULL) {
        throw DataFileException("Connectivity matrix file named \""
                                + getFileNameNoPath()
                                + "\" of type\""
                                + DataFileTypeEnum::toName(getDataFileType())
                                + "\" does not have a file loaded.");
    }
    
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    const MapContent* mapContent = m_mapContent[mapIndex];
    
    try {
        int64_t columnIndex = getColumnIndexForNodeWhenViewing(structure,
                                                               numberOfNodes,
                                                               nodeIndex);
        
        if ((columnIndex >= 0)
            && (columnIndex < mapContent->m_dataCount)) {
            valueOut = mapContent->m_data[columnIndex];
            
            std::vector<CiftiParcelElement> parcels;
            m_ciftiXML->getParcelsForColumns(parcels);
            textOut = AString::number(valueOut);
            if (columnIndex < static_cast<int32_t>(parcels.size())) {
                textOut += (" "
                            + parcels[columnIndex].m_parcelName);
            }
            return true;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }

    return false;
}

/**
 * Get the node coloring for the surface.
 * @param surface
 *    Surface whose nodes are colored.
 * @param surfaceRGBA
 *    Filled with RGBA coloring for the surface's nodes.
 *    Contains numberOfNodes * 4 elements.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @return
 *    True if coloring is valid, else false.
 */
bool
CiftiConnectivityMatrixDataFile::getMapSurfaceNodeColoring(const int32_t mapIndex,
                                                        const StructureEnum::Enum structure,
                                               float* surfaceRGBA,
                                               const int32_t surfaceNumberOfNodes)
{
    if (m_ciftiInterface == NULL) {
        throw DataFileException("Connectivity matrix file named \""
                                + getFileNameNoPath()
                                + "\" of type\""
                                + DataFileTypeEnum::toName(getDataFileType())
                                + "\" does not have a file loaded.");
    }
    
    
    /*
     * Get content for map.
     */
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    MapContent* mapContent = m_mapContent[mapIndex];
    
    try {
        if (m_ciftiXML->getSurfaceNumberOfNodes(CIFTI_INDEX_VIEWING, structure) == surfaceNumberOfNodes) {
            for (int32_t i = 0; i < surfaceNumberOfNodes; i++) {
                const int64_t columnIndex = getColumnIndexForNodeWhenViewing(structure,
                                                                       surfaceNumberOfNodes,
                                                                       i);
                const int64_t surfaceRgbaOffset = i * 4;
                if ((columnIndex >= 0)
                    && (columnIndex < mapContent->m_dataCount)) {
                    const int64_t rgbaOffset = columnIndex * 4;
                    surfaceRGBA[surfaceRgbaOffset]   = mapContent->m_rgba[rgbaOffset];
                    surfaceRGBA[surfaceRgbaOffset+1] = mapContent->m_rgba[rgbaOffset+1];
                    surfaceRGBA[surfaceRgbaOffset+2] = mapContent->m_rgba[rgbaOffset+2];
                    surfaceRGBA[surfaceRgbaOffset+3] = mapContent->m_rgba[rgbaOffset+3];
                }
                else {
                    surfaceRGBA[surfaceRgbaOffset]   = 0.0;
                    surfaceRGBA[surfaceRgbaOffset+1] = 0.0;
                    surfaceRGBA[surfaceRgbaOffset+2] = 0.0;
                    surfaceRGBA[surfaceRgbaOffset+3] = -1.0;
                }
            }
            
            return true;
        }
    }
    catch (CiftiFileException& e) {
        throw DataFileException(e.whatString());
    }
    
    return false;
}

/**
 * Get the number of nodes for a surface of the given structure in this file
 * for loading data.
 *
 * 
 * @param structure
 *    The surface structure.
 * @return  Number of nodes in this file for the given structure or negative if
 *    this file is invalid (not yet loaded) or the given structure is not in
 *    this file.
 */
int32_t
CiftiConnectivityMatrixDataFile::getSurfaceNumberOfNodesForLoading(const StructureEnum::Enum structure) const
{
    if (m_ciftiInterface == NULL) {
        return 0;
    }
    const int32_t numberOfNodes = m_ciftiXML->getSurfaceNumberOfNodes(CIFTI_INDEX_LOADING,
                                                                   structure);
    return numberOfNodes;
}


/* ========================================================================== */

/**
 * Constructor.
 */
CiftiConnectivityMatrixDataFile::MapContent::MapContent()
{
    m_descriptiveStatistics.grabNew(new DescriptiveStatistics());
    m_fastStatistics.grabNew(new FastStatistics());
    m_histogram.grabNew(new Histogram());
    m_metaData.grabNew(new GiftiMetaData());
    m_paletteColorMapping.grabNew(new PaletteColorMapping());
    
    m_dataLoadingEnabled = true;
    m_dataCount = 0;
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixDataFile::MapContent::~MapContent()
{
}

/**
 * Update the map's content.
 *
 * @param data
 *    Data for the map.
 * @param dataCount
 *    Count of data elements.
 * @param mapName
 *    Name of map.
 * @param paletteColorMapping
 *    Palette color mapping for map.
 * @param metadataNamesValues
 *    Metadata names and values.
 */
void
CiftiConnectivityMatrixDataFile::MapContent::updateData(const CiftiInterface* ciftiInterface,
                                                        const int32_t mapIndex,
                                                        const float* data,
                                                        const int64_t dataCount)
{
    m_dataCount = dataCount;
    /*
     * Resize and set data.
     */
    m_data.resize(m_dataCount);
    for (int64_t i = 0; i < m_dataCount; i++) {
        m_data[i] = data[i];
    }
    
    /*
     * Resize RGBA.  Values are filled in updateColoring()
     */
    m_rgba.resize(m_dataCount * 4);
    
    /*
     * Set name of map.
     */
    m_name = ciftiInterface->getMapNameForColumnIndex(mapIndex);
    
    /*
     * Need access the CIFTI_XML.
     */
    const CiftiXML& ciftiXML = ciftiInterface->getCiftiXML();
    *m_paletteColorMapping = *ciftiXML.getFilePalette();
    
    const std::map<AString, AString>* metaDataMap = ciftiXML.getFileMetaData();
    m_metaData->clear();
    for (std::map<AString, AString>::const_iterator iter = metaDataMap->begin();
         iter != metaDataMap->end();
         iter++) {
        m_metaData->set(iter->first,
                        iter->second);
    }

    m_descriptiveStatistics->invalidateData();
    
    
    const IndicesMapToDataType mappingType = ciftiXML.getMappingType(CIFTI_INDEX_VIEWING);
    
    bool isBrainModels = false;
    bool isParcels     = false;
    switch (mappingType) {
        case CIFTI_INDEX_TYPE_BRAIN_MODELS:
            isBrainModels = true;
            break;
        case CIFTI_INDEX_TYPE_FIBERS:
            break;
        case CIFTI_INDEX_TYPE_INVALID:
            break;
        case CIFTI_INDEX_TYPE_LABELS:
            break;
        case CIFTI_INDEX_TYPE_PARCELS:
            isParcels = true;
            break;
        case CIFTI_INDEX_TYPE_SCALARS:
            break;
        case CIFTI_INDEX_TYPE_TIME_POINTS:
            break;
    }
    
    /*
     * Is there volume data?
     */
    bool needVolumeFlag = false;
    m_ciftiToVolumeMapping.clear();
    const bool haveRowVolumeData = true;  //ciftiInterface->hasRowVolumeData()
    if (haveRowVolumeData) {
        if (isBrainModels) {
            /*
             * Get mapping from CIFTI to voxels
             */
            ciftiInterface->getVolumeMapForRows(m_ciftiToVolumeMapping);
            
            if (m_ciftiToVolumeMapping.empty() == false) {
                needVolumeFlag = true;
                
                /*
                 * Create the volume
                 */
                createVolume(ciftiInterface);
                
                /*
                 * Load data into the volume
                 */
                m_volumeFile->setValueAllVoxels(0.0);
                for (std::vector<CiftiVolumeMap>::const_iterator iter = m_ciftiToVolumeMapping.begin();
                     iter != m_ciftiToVolumeMapping.end();
                     iter++) {
                    const CiftiVolumeMap& vm = *iter;
                    
                    CaretAssertArrayIndex(m_data,
                                          m_dataCount,
                                          vm.m_ciftiIndex);
                    m_volumeFile->setValue(m_data[vm.m_ciftiIndex], vm.m_ijk);
                }
            }
            
        }
        else if (isParcels) {
            std::vector<CiftiParcelElement> parcels;
            ciftiXML.getParcelsForRows(parcels);

            if (parcels.empty() == false) {
                needVolumeFlag = true;
                
                /*
                 * Create the volume
                 */
                createVolume(ciftiInterface);

                /*
                 * Load data into the volume
                 */
                const int64_t numParcels = static_cast<int64_t>(parcels.size());
                for (int64_t iParcel = 0; iParcel < numParcels; iParcel++) {
                    const CiftiParcelElement& cfp = parcels[iParcel];
                    const int64_t numVoxelIndices = static_cast<int64_t>(cfp.m_voxelIndicesIJK.size());
                    const int64_t numVoxels = (numVoxelIndices / 3);
                    for (int64_t ivoxel = 0; ivoxel < numVoxels; ivoxel++) {
                        const int64_t i3 = ivoxel * 3;
                        const voxelIndexType i = cfp.m_voxelIndicesIJK[i3];
                        const voxelIndexType j = cfp.m_voxelIndicesIJK[i3+1];
                        const voxelIndexType k = cfp.m_voxelIndicesIJK[i3+2];
                        
                        CiftiVolumeMap cvm;
                        cvm.m_ciftiIndex = iParcel;
                        cvm.m_ijk[0] = i;
                        cvm.m_ijk[1] = j;
                        cvm.m_ijk[2] = k;
                        m_ciftiToVolumeMapping.push_back(cvm);
                    }
                }
            }
        }
    }
    
    /*
     * If volume is not needed, remove it
     */
    if (needVolumeFlag == false) {
        if (m_volumeFile != NULL) {
            m_volumeFile.grabNew(NULL);
        }
    }
}

/**
 * Create/Delete volume
 *
 * @param ciftiInterface
 *    The CIFTI interface
 * @param needVolumeFlag
 *    True if volume needed, else false.
 */
void
CiftiConnectivityMatrixDataFile::MapContent::createVolume(const CiftiInterface* ciftiInterface)
{
    /*
     * Get volume attributes and make sure orthogonal
     */
    VolumeFile::OrientTypes orientation[3];
    int64_t dimensions[3];
    float origin[3];
    float spacing[3];
    if (ciftiInterface->getVolumeAttributesForPlumb(orientation,
                                                    dimensions,
                                                    origin,
                                                    spacing)) {
        bool createVolumeFlag = false;
        
        if (m_volumeFile == NULL) {
            createVolumeFlag = true;
        }
        
        if (m_volumeFile != NULL) {
            /*
             * Compare attributes of existing volume and
             * requested attributes to see if volume needs
             * to be recreated.
             */
            int64_t dimI, dimJ, dimK, dimTime, numComp;
            m_volumeFile->getDimensions(dimI, dimJ, dimK, dimTime, numComp);
            if ((dimI != dimensions[0])
                || (dimJ != dimensions[1])
                || (dimK != dimensions[2])) {
                createVolumeFlag = true;
            }
            else {
                float x0 = 0, y0 = 0, z0 = 0, x1 = 0, y1 = 0, z1 = 0;
                m_volumeFile->indexToSpace(0, 0, 0, x0, y0, z0);
                m_volumeFile->indexToSpace(1, 1, 1, x1, y1, z1);
                const float dx = x1 - x0;
                const float dy = y1 - y0;
                const float dz = z1 - z0;
                
                const float diffOrigin[3] = {
                    std::fabs(x0 - origin[0]),
                    std::fabs(y0 - origin[1]),
                    std::fabs(z0 - origin[2])
                };
                
                const float diffSpacing[3] = {
                    std::fabs(dx - spacing[0]),
                    std::fabs(dy - spacing[1]),
                    std::fabs(dz - spacing[2]),
                };
                
                const float maxDiff = 0.0001;
                if ((diffOrigin[0] > maxDiff)
                    || (diffOrigin[1] > maxDiff)
                    || (diffOrigin[2] > maxDiff)) {
                    createVolumeFlag = true;
                }
                else if ((diffSpacing[0] > maxDiff)
                         || (diffSpacing[1] > maxDiff)
                         || (diffSpacing[2] > maxDiff)) {
                    createVolumeFlag = true;
                }
            }
        }
        
        /*
         * If needed, recreate the volume
         */
        if (createVolumeFlag) {
            std::vector<int64_t> dimensionsNew;
            dimensionsNew.push_back(dimensions[0]);
            dimensionsNew.push_back(dimensions[1]);
            dimensionsNew.push_back(dimensions[2]);
            
            std::vector<float> row1;
            row1.push_back(spacing[0]);
            row1.push_back(0.0);
            row1.push_back(0.0);
            row1.push_back(origin[0]);
            
            std::vector<float> row2;
            row2.push_back(0.0);
            row2.push_back(spacing[1]);
            row2.push_back(0.0);
            row2.push_back(origin[1]);
            
            std::vector<float> row3;
            row3.push_back(0.0);
            row3.push_back(0.0);
            row3.push_back(spacing[2]);
            row3.push_back(origin[2]);
            
            std::vector<std::vector<float> > indexToSpace;
            indexToSpace.push_back(row1);
            indexToSpace.push_back(row2);
            indexToSpace.push_back(row3);
            
            int64_t numComponents = 1;
            
            m_volumeFile.grabNew(new VolumeFile(dimensionsNew,
                                                indexToSpace,
                                                numComponents));
        }
        
    }

}

/**
 * Update coloring for this map.
 *
 * @param mapIndex
 *    Index of map.
 * @param paletteFile
 *    File containing the palettes.
 */
void
CiftiConnectivityMatrixDataFile::MapContent::updateColoring(const int32_t mapIndex,
                                                            const PaletteFile* paletteFile)
{
    const AString paletteName = m_paletteColorMapping->getSelectedPaletteName();
    const Palette* palette = paletteFile->getPaletteByName(paletteName);
    if (palette == NULL) {
        CaretLogWarning("Missing palette named \""
                        + paletteName
                        + "\" for coloring connectivity data");
    }
    if (m_data.empty()) {
        return;
    }
    
    NodeAndVoxelColoring::colorScalarsWithPalette(getFastStatistics(),
                                                  m_paletteColorMapping,
                                                  palette,
                                                  &m_data[0],
                                                  &m_data[0],
                                                  m_data.size(),
                                                  &m_rgba[0]);
    
    CaretLogFine("Connectivity Data Average/Min/Max: "
                 + QString::number(m_fastStatistics->getMean())
                 + " "
                 + QString::number(m_fastStatistics->getMostNegativeValue())
                 + " "
                 + QString::number(m_fastStatistics->getMostPositiveValue()));
    
    
    if (m_ciftiToVolumeMapping.empty() == false) {
        /*
         * Update colors in map.
         */
        CaretAssert(m_volumeFile);
        m_volumeFile->clearVoxelColoringForMap(mapIndex);
        
        for (std::vector<CiftiVolumeMap>::const_iterator iter = m_ciftiToVolumeMapping.begin();
             iter != m_ciftiToVolumeMapping.end();
             iter++) {
            const CiftiVolumeMap& vm = *iter;
            const int64_t dataRGBAIndex = vm.m_ciftiIndex * 4;
            const float* rgba = &m_rgba[dataRGBAIndex];
            m_volumeFile->setVoxelColorInMap(vm.m_ijk[0],
                                             vm.m_ijk[1],
                                             vm.m_ijk[2],
                                             mapIndex,
                                             rgba);
        }
    }
}

/**
 * Convert the coordinate to a voxel index.
 *
 * @param xyz
 *    The coordinate.
 * @param ijkOut
 *    The output voxel indices.
 * @return true if voxel indices valid, else false.
 */
bool
CiftiConnectivityMatrixDataFile::MapContent::voxelXYZToIJK(const float xyz[3],
                                                           int64_t ijkOut[3]) const
{
    if (m_volumeFile != NULL) {
        int64_t ijk[10];
        m_volumeFile->enclosingVoxel(xyz,
                                     ijk);
        if (m_volumeFile->indexValid(ijk)) {
            ijkOut[0] = ijk[0];
            ijkOut[1] = ijk[1];
            ijkOut[2] = ijk[2];
            return  true;
        }
    }
    
    return false;
}


/**
 * @return Fast statistics for this map.
 */
const FastStatistics*
CiftiConnectivityMatrixDataFile::MapContent::getFastStatistics()
{
    if (m_data.empty()) {
        m_fastStatistics->update(NULL,
                   0);
    }
    else {
        m_fastStatistics->update(&m_data[0],
                                 m_data.size());
    }
    
    return m_fastStatistics;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
CiftiConnectivityMatrixDataFile::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                                    const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CiftiConnectivityMatrixDataFile",
                                            1);
    
    const int32_t numMaps = getNumberOfMaps();
    if (numMaps > 0) {
        bool* mapEnabledArray = new bool[numMaps];
        for (int32_t i = 0; i < numMaps; i++) {
            mapEnabledArray[i] = m_mapContent[i]->m_dataLoadingEnabled;
        }
        
        sceneClass->addBooleanArray("mapEnabled",
                                    mapEnabledArray,
                                    numMaps);
        delete[] mapEnabledArray;
    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
CiftiConnectivityMatrixDataFile::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                         const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const int32_t numMaps = getNumberOfMaps();
    if (numMaps > 0) {
        bool* mapEnabledArray = new bool[numMaps];
        
        sceneClass->getBooleanArrayValue("mapEnabled",
                                         mapEnabledArray,
                                         numMaps);
        
        for (int32_t i = 0; i < numMaps; i++) {
            m_mapContent[i]->m_dataLoadingEnabled = mapEnabledArray[i];
        }
        
        delete[] mapEnabledArray;
    }
}


