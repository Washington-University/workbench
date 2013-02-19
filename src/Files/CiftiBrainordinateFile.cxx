
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

#define __CIFTI_BRAINORDINATE_FILE_DECLARE__
#include "CiftiBrainordinateFile.h"
#undef __CIFTI_BRAINORDINATE_FILE_DECLARE__

#include "CiftiFile.h"
#include "CaretLogger.h"
#include "CiftiInterface.h"
#include "CiftiXnat.h"
#include "DescriptiveStatistics.h"
#include "FastStatistics.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "SceneClass.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::CiftiBrainordinateFile 
 * \brief Abstract class for brainordinate type files.
 * \ingroup Files
 *
 * This base class is designed for CIFTI files that map to brainordinates.
 * In these types of files, each row contains data for a brainordinate.  Each
 * column contains one set of data that maps to the brainordinates.
 */

/**
 * Constructor.
 */
CiftiBrainordinateFile::CiftiBrainordinateFile(const DataFileTypeEnum::Enum dataFileType)
: CaretMappableDataFile(dataFileType)
{
    clearPrivate();
}

/**
 * Destructor.
 */
CiftiBrainordinateFile::~CiftiBrainordinateFile()
{
    clearPrivate();
}

/**
 * Clear the contents of the file.
 */
void
CiftiBrainordinateFile::clear()
{
    CaretMappableDataFile::clear();
    clearPrivate();
}

/**
 * Clear the contents of the file.
 * Note that "clear()" is virtual and cannot be called from destructor.
 */
void
CiftiBrainordinateFile::clearPrivate()
{
    if (m_ciftiInterface != NULL) {
        m_ciftiInterface.grabNew(NULL);
    }
    m_ciftiXML = NULL;
    m_mapContent.clear();
}

/**
 * @return Is this file empty?
 */
bool
CiftiBrainordinateFile::isEmpty() const
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
CiftiBrainordinateFile::getStructure() const
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
CiftiBrainordinateFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* CIFTI files may apply to all structures */
}

/**
 * @return Metadata for the file.
 */
GiftiMetaData*
CiftiBrainordinateFile::getFileMetaData()
{
    return m_fileMetaData;
}

/**
 * @return Metadata for the file.
 */
const GiftiMetaData*
CiftiBrainordinateFile:: getFileMetaData() const
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
CiftiBrainordinateFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();
    
    try {
        if (DataFile::isFileOnNetwork(filename)) {
            CiftiXnat* ciftiXnat = new CiftiXnat();
            AString username = "";
            AString password = "";
            AString filenameToOpen = "";
            
            /*
             * Username and password may be embedded in URL, so extract them.
             */
            FileInformation fileInfo(filename);
            fileInfo.getRemoteUrlUsernameAndPassword(filenameToOpen,
                                                     username,
                                                     password);
            
            /*
             * Always override with a password entered by the user.
             */
            if (CaretDataFile::getFileReadingUsername().isEmpty() == false) {
                username = CaretDataFile::getFileReadingUsername();
                password = CaretDataFile::getFileReadingPassword();
            }
            
            ciftiXnat->setAuthentication(filenameToOpen,
                                         username,
                                         password);
            ciftiXnat->openURL(filenameToOpen);
            m_ciftiInterface.grabNew(ciftiXnat);
            
//            CiftiXnat* ciftiXnat = new CiftiXnat();
//            const AString username = CaretDataFile::getFileReadingUsername();
//            if (username.isEmpty() == false) {
//                ciftiXnat->setAuthentication(filename,
//                                             username,
//                                             CaretDataFile::getFileReadingPassword());
//            }
//            ciftiXnat->openURL(filename);
//            m_ciftiInterface.grabNew(ciftiXnat);
        }
        else {
            CiftiFile* ciftiFile = new CiftiFile();
            ciftiFile->openFile(filename,
                                    IN_MEMORY);
            m_ciftiInterface.grabNew(ciftiFile);
        }
        
        setFileName(filename);
        
        if (m_ciftiInterface != NULL) {
            m_ciftiXML = const_cast<CiftiXML*>(&m_ciftiInterface->getCiftiXML());
            
            const IndicesMapToDataType rowMappingType = m_ciftiXML->getMappingType(CiftiXML::ALONG_COLUMN);
            const IndicesMapToDataType columnMappingType = m_ciftiXML->getMappingType(CiftiXML::ALONG_ROW);

            /*
             * Validate type of data in rows and columns
             */
            AString errorMessage;
            if (rowMappingType != CIFTI_INDEX_TYPE_BRAIN_MODELS) {
                errorMessage.appendWithNewLine("Invalid row mapping data type="
                                               + AString::number(rowMappingType));
            }
            
            switch (columnMappingType) {
                case CIFTI_INDEX_TYPE_LABELS:
                    m_columnMapDataType = MapContent::MAP_CONTENT_DATA_TYPE_LABELS;
                    break;
                case CIFTI_INDEX_TYPE_SCALARS:
                    m_columnMapDataType = MapContent::MAP_CONTENT_DATA_TYPE_SCALARS;
                    break;
                default:
                    errorMessage.appendWithNewLine("Invalid column mapping data type="
                                                   + AString::number(columnMappingType));
                    break;
                    
            }
            if (errorMessage.isEmpty() == false) {
                clear();
                throw DataFileException(errorMessage);
            }
            
            const int32_t numCols = m_ciftiInterface->getNumberOfColumns();
            const int32_t numRows = m_ciftiInterface->getNumberOfRows();
            
            const IndicesMapToDataType rowType = m_ciftiXML->getMappingType(CiftiXML::ALONG_ROW);
            const IndicesMapToDataType colType = m_ciftiXML->getMappingType(CiftiXML::ALONG_COLUMN);
            const AString msg = ("CIFTI Brainordinate File: " + getFileNameNoPath() + "\n"
                                 "Rows=" + AString::number(numRows)
                                 + " Columns=" + AString::number(numCols)
                                 + " RowType=" + AString::number(rowType)
                                 + " ColType=" + AString::number(colType));
            CaretLogSevere(msg);
            
            std::vector<float> dataVector(numRows);
            float* dataPointer = &dataVector[0];
            
            for (int32_t columnIndex = 0; columnIndex < numCols; columnIndex++) {
                m_ciftiInterface->getColumn(dataPointer,
                                            columnIndex);
                CaretPointer<MapContent> mc(new MapContent(m_ciftiInterface,
                                                           m_ciftiXML,
                                                           m_columnMapDataType,
                                                           columnIndex,
                                                           dataPointer,
                                                           numRows));
                m_mapContent.push_back(mc);
            }
        }
        
        clearModified();
    }
    catch (CiftiFileException& e) {
        clear();
        throw DataFileException(e.whatString());
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
CiftiBrainordinateFile::writeFile(const AString& filename) throw (DataFileException)
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
CiftiBrainordinateFile::isSurfaceMappable() const
{
    return true;
}

/**
 * @return Is the data mappable to a volume?
 */
bool
CiftiBrainordinateFile::isVolumeMappable() const
{
    return true;
}

/**
 * @return The number of maps in the file.
 * Note: Caret5 used the term 'columns'.
 */
int32_t
CiftiBrainordinateFile::getNumberOfMaps() const
{
//    return 1;
    return m_mapContent.size();
}

/**
 * Get the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString CiftiBrainordinateFile::getMapName(const int32_t mapIndex) const
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
CiftiBrainordinateFile::setMapName(const int32_t mapIndex,
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
CiftiBrainordinateFile::getMapMetaData(const int32_t mapIndex) const
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
CiftiBrainordinateFile::getMapMetaData(const int32_t mapIndex)
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
CiftiBrainordinateFile::getMapUniqueID(const int32_t mapIndex) const
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
CiftiBrainordinateFile::isMappedWithPalette() const
{
    switch (m_columnMapDataType) {
        case MapContent::MAP_CONTENT_DATA_TYPE_LABELS:
            break;
        case MapContent::MAP_CONTENT_DATA_TYPE_SCALARS:
            return true;
            break;
    }
    return false;
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
CiftiBrainordinateFile::getMapStatistics(const int32_t mapIndex)
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
CiftiBrainordinateFile::getMapFastStatistics(const int32_t mapIndex)
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
CiftiBrainordinateFile::getMapHistogram(const int32_t mapIndex)
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
CiftiBrainordinateFile::getMapStatistics(const int32_t mapIndex,
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
CiftiBrainordinateFile::getMapHistogram(const int32_t mapIndex,
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
CiftiBrainordinateFile::getMapPaletteColorMapping(const int32_t mapIndex)
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
CiftiBrainordinateFile::getMapPaletteColorMapping(const int32_t mapIndex) const
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
CiftiBrainordinateFile::isMappedWithLabelTable() const
{
    switch (m_columnMapDataType) {
        case MapContent::MAP_CONTENT_DATA_TYPE_LABELS:
            return true;
            break;
        case MapContent::MAP_CONTENT_DATA_TYPE_SCALARS:
            break;
    }
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
CiftiBrainordinateFile::getMapLabelTable(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_labelTable;
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
GiftiLabelTable* CiftiBrainordinateFile::getMapLabelTable(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_labelTable;
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
CiftiBrainordinateFile::updateScalarColoringForMap(const int32_t mapIndex,
                                        const PaletteFile* paletteFile)
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
//    m_mapContent[mapIndex]->updateColoring(mapIndex,
//                                           paletteFile);
    m_mapContent[mapIndex]->updateColoring(paletteFile);
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
CiftiBrainordinateFile::getMapVolume(const int32_t mapIndex)
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
CiftiBrainordinateFile::getMapVolume(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapContent,
                           mapIndex);
    return m_mapContent[mapIndex]->m_volumeFile;
}

/**
 * Get connectivity value for a voxel at the given coordinate.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param textOut
 *     Text containing node value and for some types, the parcel.
 * @return
 *    true if a value was available for the voxel, else false.
 */
bool
CiftiBrainordinateFile::getMapVolumeVoxelValue(const int32_t mapIndex,
                                                     const float xyz[3],
                                            int64_t ijkOut[3],
                                                        AString& textOut) const
{
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
    
    const MapContent* mc = m_mapContent[mapIndex];
    if (mc->m_volumeFile != NULL) {
        int64_t vfIJK[3];
        mc->m_volumeFile->enclosingVoxel(xyz,
                                   vfIJK);
        if (mc->m_volumeFile->indexValid(vfIJK)) {
            std::vector<CiftiVolumeMap> volumeMap;
            
            m_ciftiInterface->getVolumeMapForColumns(volumeMap);
            
            const int64_t numMaps = static_cast<int64_t>(volumeMap.size());
            for (int64_t i = 0; i < numMaps; i++) {
                if (volumeMap[i].m_ijk[0] == vfIJK[0]
                    && volumeMap[i].m_ijk[1] == vfIJK[1]
                    && volumeMap[i].m_ijk[2] == vfIJK[2]) {
                    CaretAssertArrayIndex(mc->m_data,
                                          mc->m_dataCount,
                                          volumeMap[i].m_ciftiIndex);
                    const float value = mc->m_data[volumeMap[i].m_ciftiIndex];
                    
                    switch (m_columnMapDataType) {
                        case MapContent::MAP_CONTENT_DATA_TYPE_LABELS:
                        {
                            textOut = "Invalid Label Index";
                            
                            const GiftiLabelTable* glt = getMapLabelTable(mapIndex);
                            const int32_t labelKey = static_cast<int32_t>(value);
                            const GiftiLabel* gl = glt->getLabel(labelKey);
                            if (gl != NULL) {
                                textOut = gl->getName();
                            }
                        }
                            break;
                        case MapContent::MAP_CONTENT_DATA_TYPE_SCALARS:
                            textOut = AString::number(value);
                            break;
                    }
                    
                    ijkOut[0] = vfIJK[0];
                    ijkOut[1] = vfIJK[1];
                    ijkOut[2] = vfIJK[2];
                    
                    return true;
                }
            }
        }
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
 * @param textOut
 *     Text containing node value and for some types, the parcel.
 * @return
 *    true if a value was available for the node, else false.
 */
bool
CiftiBrainordinateFile::getMapSurfaceNodeValue(const int32_t mapIndex,
                                                        const StructureEnum::Enum structure,
                                                        const int nodeIndex,
                                                        const int32_t numberOfNodes,
                                                        AString& textOut) const
{
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
    std::vector<CiftiSurfaceMap> nodeMap;
    const int numCiftiNodes = m_ciftiInterface->getColumnSurfaceNumberOfNodes(structure);
    if (numberOfNodes != numCiftiNodes) {
        return false;
    }
    m_ciftiInterface->getSurfaceMapForColumns(nodeMap,
                                              structure);
    
    if (nodeMap.empty() == false) {
        const int64_t numNodeMaps = static_cast<int32_t>(nodeMap.size());
        for (int i = 0; i < numNodeMaps; i++) {
            if (nodeMap[i].m_surfaceNode == nodeIndex) {
                const MapContent* mc = m_mapContent[mapIndex];
                CaretAssertArrayIndex(mc->m_data,
                                      mc->m_dataCount,
                                      nodeMap[i].m_ciftiIndex);
                const float value = mc->m_data[nodeMap[i].m_ciftiIndex];
                
                switch (m_columnMapDataType) {
                    case MapContent::MAP_CONTENT_DATA_TYPE_LABELS:
                    {
                        textOut = "Invalid Label Index";
                        
                        const GiftiLabelTable* glt = getMapLabelTable(mapIndex);
                        const int32_t labelKey = static_cast<int32_t>(value);
                        const GiftiLabel* gl = glt->getLabel(labelKey);
                        if (gl != NULL) {
                            textOut = gl->getName();
                        }
                    }
                        break;
                    case MapContent::MAP_CONTENT_DATA_TYPE_SCALARS:
                        textOut = AString::number(value);
                        break;
                }

                return true;
            }
        }
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
CiftiBrainordinateFile::getMapSurfaceNodeColoring(const int32_t mapIndex,
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
    
    MapContent* mc = m_mapContent[mapIndex];
    std::vector<CiftiSurfaceMap> nodeMap;
    
    if (m_ciftiXML->getSurfaceNumberOfNodes(CiftiXML::ALONG_COLUMN,
                                            structure) == surfaceNumberOfNodes) {
        m_ciftiInterface->getSurfaceMapForColumns(nodeMap,
                                                  structure);
        
        if (nodeMap.empty() == false) {
            for (int64_t i = 0; i < surfaceNumberOfNodes; i++) {
                const int64_t i4 = i * 4;
                surfaceRGBA[i4]   =  0.0;
                surfaceRGBA[i4+1] =  0.0;
                surfaceRGBA[i4+2] =  0.0;
                surfaceRGBA[i4+3] = -1.0;
            }

            const int64_t numNodeMaps = static_cast<int32_t>(nodeMap.size());
            for (int i = 0; i < numNodeMaps; i++) {
                const int64_t node4 = nodeMap[i].m_surfaceNode * 4;
                const int64_t cifti4 = nodeMap[i].m_ciftiIndex * 4;
                CaretAssertArrayIndex(surfaceRGBA, (surfaceNumberOfNodes * 4), node4);
                CaretAssertArrayIndex(this->dataRGBA, (mc->m_dataCount * 4), cifti4);
                surfaceRGBA[node4]   = mc->m_rgba[cifti4];
                surfaceRGBA[node4+1] = mc->m_rgba[cifti4+1];
                surfaceRGBA[node4+2] = mc->m_rgba[cifti4+2];
                surfaceRGBA[node4+3] = mc->m_rgba[cifti4+3];
            }
            return true;
        }
    }

    return false;
}

/**
 * @return Name of map data type.
 */
AString
CiftiBrainordinateFile::getMapDataTypeName() const
{
    AString mapDataTypeName = "NONE";
    
    switch (m_columnMapDataType) {
        case MapContent::MAP_CONTENT_DATA_TYPE_LABELS:
            mapDataTypeName = "CIFTI LABELS";
            break;
        case MapContent::MAP_CONTENT_DATA_TYPE_SCALARS:
            mapDataTypeName = "CIFTI SCALARS";
            break;
    }
    
    return mapDataTypeName;
}


/* ========================================================================== */

/**
 * Constructor.
 *
 * @param ciftiInterface
 *    The CIFTI Interface.
 * @param ciftiXML
 *    The CIFTI XML.
 * @param mapContentDataType
 *    Type of data in the map.
 * @param mapIndex
 *    Index of the map.
 * @param mapData
 *    Data in the map.
 * @param mapDataCount
 *    Number of elements in the map.
 */
CiftiBrainordinateFile::MapContent::MapContent(CiftiInterface* ciftiInterface,
                                                   CiftiXML* ciftiXML,
                                                   const MapContentDataType mapContentDataType,
                                                   const int32_t mapIndex,
                                                   const float* mapData,
                                                   const int64_t mapDataCount)
{
    m_descriptiveStatistics.grabNew(new DescriptiveStatistics());
    m_fastStatistics.grabNew(new FastStatistics());
    m_histogram.grabNew(new Histogram());
    m_labelTable = NULL;
    m_metaData.grabNew(new GiftiMetaData());
    m_paletteColorMapping = NULL;

    m_mapContentDataType = mapContentDataType;
    
    /*
     * Allocate and load data.
     */
    m_dataCount = mapDataCount;
    m_data.resize(m_dataCount);
    for (int64_t i = 0; i < m_dataCount; i++) {
        m_data[i] = mapData[i];
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
     * Copy the palette color mapping
     */
    m_paletteColorMapping = ciftiXML->getMapPalette(CiftiXML::ALONG_ROW, mapIndex);
    
    /*
     * Copy the label table.
     */
    m_labelTable = const_cast<GiftiLabelTable*>(ciftiXML->getLabelTableForRowIndex(mapIndex));
    
    /*
     * Copy the metadata.
     */
    const std::map<AString, AString>* metaDataMap = ciftiXML->getMapMetadata(CiftiXML::ALONG_ROW,
                                                                                 mapIndex);
    m_metaData->clear();
    for (std::map<AString, AString>::const_iterator iter = metaDataMap->begin();
         iter != metaDataMap->end();
         iter++) {
        m_metaData->set(iter->first,
                        iter->second);
    }
    
    m_descriptiveStatistics->invalidateData();
    
    /*
     * Is there volume data?
     */
    bool needVolumeFlag = false;
    m_ciftiToVolumeMapping.clear();
    const bool haveColumnVolumeData = true;  //ciftiInterface->hasColumnVolumeData()
    if (haveColumnVolumeData) {
        /*
         * Get mapping from CIFTI to voxels
         */
        ciftiInterface->getVolumeMapForColumns(m_ciftiToVolumeMapping);
        
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
 * Destructor.
 */
CiftiBrainordinateFile::MapContent::~MapContent()
{
    /**
     * Do not delete these to data in CIFTI XML:
     *   m_giftiLabelTable
     *   m_paletteColorMapping
     */
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
CiftiBrainordinateFile::MapContent::createVolume(const CiftiInterface* ciftiInterface)
{
    /*
     * Get volume attributes and make sure orthogonal
     */
    VolumeFile::OrientTypes ciftiOrientation[3];
    int64_t ciftiDimensions[3];
    float ciftiOrigin[3];
    float ciftiSpacing[3];
    if (ciftiInterface->getVolumeAttributesForPlumb(ciftiOrientation,
                                                    ciftiDimensions,
                                                    ciftiOrigin,
                                                    ciftiSpacing)) {
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
            if ((dimI != ciftiDimensions[0])
                || (dimJ != ciftiDimensions[1])
                || (dimK != ciftiDimensions[2])) {
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
                    std::fabs(x0 - ciftiOrigin[0]),
                    std::fabs(y0 - ciftiOrigin[1]),
                    std::fabs(z0 - ciftiOrigin[2])
                };
                
                const float diffSpacing[3] = {
                    std::fabs(dx - ciftiSpacing[0]),
                    std::fabs(dy - ciftiSpacing[1]),
                    std::fabs(dz - ciftiSpacing[2]),
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
            dimensionsNew.push_back(ciftiDimensions[0]);
            dimensionsNew.push_back(ciftiDimensions[1]);
            dimensionsNew.push_back(ciftiDimensions[2]);
            
            std::vector<float> row1;
            row1.push_back(ciftiSpacing[0]);
            row1.push_back(0.0);
            row1.push_back(0.0);
            row1.push_back(ciftiOrigin[0]);
            
            std::vector<float> row2;
            row2.push_back(0.0);
            row2.push_back(ciftiSpacing[1]);
            row2.push_back(0.0);
            row2.push_back(ciftiOrigin[1]);
            
            std::vector<float> row3;
            row3.push_back(0.0);
            row3.push_back(0.0);
            row3.push_back(ciftiSpacing[2]);
            row3.push_back(ciftiOrigin[2]);
            
            std::vector<std::vector<float> > indexToSpace;
            indexToSpace.push_back(row1);
            indexToSpace.push_back(row2);
            indexToSpace.push_back(row3);
            
            int64_t numComponents = 1;
            
            VolumeFile* vf = new VolumeFile(dimensionsNew,
                                            indexToSpace,
                                            numComponents);
            
//            /*
//             * For comparison with CIFTI volume.
//             */
//            VolumeFile::OrientTypes volumeOrientation[3];
//            std::vector<int64_t> volumeDimensions;
//            float volumeOrigin[3];
//            float volumeSpacing[3];
//            float volumeCenter[3];
//            vf->getOrientAndSpacingForPlumb(volumeOrientation,
//                                            volumeSpacing,
//                                            volumeCenter);
//            vf->getDimensions(volumeDimensions);
//            vf->indexToSpace(0, 0, 0, volumeOrigin);
            
            m_volumeFile.grabNew(vf);
        }
    }
}

///**
// * Update coloring for this map.
// *
// * @param mapIndex
// *    Index of map.
// * @param paletteFile
// *    File containing the palettes.
// */
//void
//CiftiBrainordinateFile::MapContent::updateColoring(const int32_t mapIndex,
//                                                            const PaletteFile* paletteFile)
//{
//    if (m_data.empty()) {
//        return;
//    }
//    
//    switch (m_mapContentDataType) {
//        case MAP_CONTENT_DATA_TYPE_LABELS:
//            NodeAndVoxelColoring::colorIndicesWithLabelTable(m_labelTable,
//                                                             &m_data[0],
//                                                             m_dataCount,
//                                                             &m_rgba[0]);
//            break;
//        case MAP_CONTENT_DATA_TYPE_SCALARS:
//        {
//            CaretAssert(m_paletteColorMapping);
//            CaretAssert(paletteFile);
//            const AString paletteName = m_paletteColorMapping->getSelectedPaletteName();
//            const Palette* palette = paletteFile->getPaletteByName(paletteName);
//            if (palette != NULL) {
//                NodeAndVoxelColoring::colorScalarsWithPalette(getFastStatistics(),
//                                                              m_paletteColorMapping,
//                                                              palette,
//                                                              &m_data[0],
//                                                              &m_data[0],
//                                                              m_dataCount,
//                                                              &m_rgba[0]);
//            }
//            else {
//                CaretLogWarning("Missing palette named \""
//                                + paletteName
//                                + "\" for coloring connectivity data");
//            }
//        }
//            break;
//    }
//    
//    CaretLogFine("Connectivity Data Average/Min/Max: "
//                 + QString::number(m_fastStatistics->getMean())
//                 + " "
//                 + QString::number(m_fastStatistics->getMostNegativeValue())
//                 + " "
//                 + QString::number(m_fastStatistics->getMostPositiveValue()));
//    
//    
//    if (m_ciftiToVolumeMapping.empty() == false) {
//        /*
//         * Update colors in map.
//         */
//        CaretAssert(m_volumeFile);
//        m_volumeFile->clearVoxelColoringForMap(mapIndex);
//        
//        for (std::vector<CiftiVolumeMap>::const_iterator iter = m_ciftiToVolumeMapping.begin();
//             iter != m_ciftiToVolumeMapping.end();
//             iter++) {
//            const CiftiVolumeMap& vm = *iter;
//            const int64_t dataRGBAIndex = vm.m_ciftiIndex * 4;
//            const float* rgba = &m_rgba[dataRGBAIndex];
//            m_volumeFile->setVoxelColorInMap(vm.m_ijk[0],
//                                             vm.m_ijk[1],
//                                             vm.m_ijk[2],
//                                             mapIndex,
//                                             rgba);
//        }
//    }
//}

/**
 * Update coloring for this map.
 *
 * @param mapIndex
 *    Index of map.
 * @param paletteFile
 *    File containing the palettes.
 */
void
CiftiBrainordinateFile::MapContent::updateColoring(const PaletteFile* paletteFile)
{
    if (m_data.empty()) {
        return;
    }
    
    switch (m_mapContentDataType) {
        case MAP_CONTENT_DATA_TYPE_LABELS:
            NodeAndVoxelColoring::colorIndicesWithLabelTable(m_labelTable,
                                                             &m_data[0],
                                                             m_dataCount,
                                                             &m_rgba[0]);
            break;
        case MAP_CONTENT_DATA_TYPE_SCALARS:
        {
            CaretAssert(m_paletteColorMapping);
            CaretAssert(paletteFile);
            const AString paletteName = m_paletteColorMapping->getSelectedPaletteName();
            const Palette* palette = paletteFile->getPaletteByName(paletteName);
            if (palette != NULL) {
                NodeAndVoxelColoring::colorScalarsWithPalette(getFastStatistics(),
                                                              m_paletteColorMapping,
                                                              palette,
                                                              &m_data[0],
                                                              &m_data[0],
                                                              m_dataCount,
                                                              &m_rgba[0]);
            }
            else {
                CaretLogWarning("Missing palette named \""
                                + paletteName
                                + "\" for coloring connectivity data");
            }
        }
            break;
    }
    
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
        m_volumeFile->clearVoxelColoringForMap(0);
        
        for (std::vector<CiftiVolumeMap>::const_iterator iter = m_ciftiToVolumeMapping.begin();
             iter != m_ciftiToVolumeMapping.end();
             iter++) {
            const CiftiVolumeMap& vm = *iter;
            const int64_t dataRGBAIndex = vm.m_ciftiIndex * 4;
            const float* rgba = &m_rgba[dataRGBAIndex];
            m_volumeFile->setVoxelColorInMap(vm.m_ijk[0],
                                             vm.m_ijk[1],
                                             vm.m_ijk[2],
                                             0,
                                             rgba);
        }
    }
}

/**
 * @return Fast statistics for this map.
 */
const FastStatistics*
CiftiBrainordinateFile::MapContent::getFastStatistics()
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
CiftiBrainordinateFile::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                                    const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CiftiBrainordinateFile",
                                            1);
    
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
CiftiBrainordinateFile::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                         const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }    
}


