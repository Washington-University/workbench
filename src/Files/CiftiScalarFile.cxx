
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

#define __CIFTI_SCALAR_FILE_DECLARE__
#include "CiftiScalarFile.h"
#undef __CIFTI_SCALAR_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "CiftiInterface.h"
#include "CiftiXML.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "FastStatistics.h"
#include "GiftiMetaDataXmlElements.h"
#include "GiftiXmlElements.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteFile.h"
#include "SystemUtilities.h"

using namespace caret;


    
/**
 * \class caret::CiftiScalarFile 
 * \brief CIFTI file that stores scalars.
 */

/**
 * Constructor.
 */
CiftiScalarFile::CiftiScalarFile()
: CaretMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR)
{
    m_ciftiDiskFile  = NULL;
    m_ciftiInterface = NULL;
    m_metadata       = new GiftiMetaData();
    
    clearPrivate();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
}

/**
 * Destructor.
 */
CiftiScalarFile::~CiftiScalarFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    clearPrivate();
    
    delete m_metadata;
}

/**
 * Clear the file.
 */
void
CiftiScalarFile::clear()
{
    CaretMappableDataFile::clear();
    clearPrivate();
    
}

/**
 * Clear the data (note that clear() is virtual so cannot
 * be called from constructor/destructor).
 */
void
CiftiScalarFile::clearPrivate()
{
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; i++) {
        delete m_mapData[i];
    }
    m_mapData.clear();
    
    if (m_ciftiInterface != NULL) {
        delete m_ciftiInterface;
        m_ciftiInterface = NULL;
    }
    /* Note: m_ciftiInterface and m_ciftiDiskFile are the same pointer so do not double delete */
    m_ciftiDiskFile = NULL;
    
    m_metadata->clear();
    
    m_isSurfaceMappable = false;
    m_isVolumeMappable  = false;
    m_uniqueID = SystemUtilities::createUniqueID();
    m_isColoringValid = false;
}

/**
 * Is the file empty (contains no data)?
 *
 * @return
 *    true if the file is empty, else false.
 */
bool
CiftiScalarFile::isEmpty() const
{
    return m_mapData.empty();
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
CiftiScalarFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
CiftiScalarFile::setStructure(const StructureEnum::Enum /*structure */)
{
    /* nothing */
}


/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
CiftiScalarFile::getFileMetaData()
{
    return m_metadata;
}


/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
CiftiScalarFile::getFileMetaData() const
{
    return m_metadata;
}


/**
 * @return Is the data mappable to a surface?
 */
bool
CiftiScalarFile::isSurfaceMappable() const
{
    return m_isSurfaceMappable;
}


/**
 * @return Is the data mappable to a volume?
 */
bool
CiftiScalarFile::isVolumeMappable() const
{
    return m_isVolumeMappable;
}


/**
 * @return The number of maps in the file.
 * Note: Caret5 used the term 'columns'.
 */
int32_t
CiftiScalarFile::getNumberOfMaps() const
{
    return m_mapData.size();
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
CiftiScalarFile::getMapName(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    
    AString name = m_mapData[mapIndex]->m_metadata->get(GiftiMetaDataXmlElements::METADATA_NAME_NAME);
    if (name.isEmpty()) {
        name = "scalars";
    }
    
    return name;
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
CiftiScalarFile::setMapName(const int32_t mapIndex,
                        const AString& mapName)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    
    m_mapData[mapIndex]->m_metadata->set(GiftiMetaDataXmlElements::METADATA_NAME_NAME,
                                         mapName);
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
CiftiScalarFile::getMapMetaData(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    return m_mapData[mapIndex]->m_metadata;
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
CiftiScalarFile::getMapMetaData(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    return m_mapData[mapIndex]->m_metadata;
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
CiftiScalarFile::getMapUniqueID(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    const AString uuid = m_mapData[mapIndex]->m_metadata->getUniqueID();
    return uuid;
}


/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool
CiftiScalarFile::isMappedWithPalette() const
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
CiftiScalarFile::getMapStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    m_mapData[mapIndex]->m_descriptiveStatistics->update(m_mapData[mapIndex]->m_data,
                                                         m_numberOfDataElements);
    return m_mapData[mapIndex]->m_descriptiveStatistics;
}


/**
 * Get fast tatistics describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Fast statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const FastStatistics*
CiftiScalarFile::getMapFastStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    m_mapData[mapIndex]->m_fastStatistics->update(m_mapData[mapIndex]->m_data,
                                                  m_numberOfDataElements);
    return m_mapData[mapIndex]->m_fastStatistics;
}


/**
 * Get hisogram describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Histogram for data
 */
const Histogram*
CiftiScalarFile::getMapHistogram(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    m_mapData[mapIndex]->m_histogram->update(m_mapData[mapIndex]->m_data,
                                             m_numberOfDataElements);
    return m_mapData[mapIndex]->m_histogram;
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
 *    Descriptive statistics for data.
 */
const DescriptiveStatistics*
CiftiScalarFile::getMapStatistics(const int32_t mapIndex,
                                                      const float mostPositiveValueInclusive,
                                                      const float leastPositiveValueInclusive,
                                                      const float leastNegativeValueInclusive,
                                                      const float mostNegativeValueInclusive,
                                                      const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    m_mapData[mapIndex]->m_descriptiveStatistics->update(m_mapData[mapIndex]->m_data,
                                                         m_numberOfDataElements,
                                                         mostPositiveValueInclusive,
                                                         leastPositiveValueInclusive,
                                                         leastNegativeValueInclusive,
                                                         mostNegativeValueInclusive,
                                                         includeZeroValues);
    return m_mapData[mapIndex]->m_descriptiveStatistics;
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
 *    Histogram for data.
 */
const Histogram*
CiftiScalarFile::getMapHistogram(const int32_t mapIndex,
                                         const float mostPositiveValueInclusive,
                                         const float leastPositiveValueInclusive,
                                         const float leastNegativeValueInclusive,
                                         const float mostNegativeValueInclusive,
                                         const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    m_mapData[mapIndex]->m_histogram->update(m_mapData[mapIndex]->m_data,
                                             m_numberOfDataElements,
                                             mostPositiveValueInclusive,
                                             leastPositiveValueInclusive,
                                             leastNegativeValueInclusive,
                                             mostNegativeValueInclusive,
                                             includeZeroValues);
    return m_mapData[mapIndex]->m_histogram;
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
CiftiScalarFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    return m_ciftiInterface->getCiftiXML().getMapPalette(CiftiXML::ALONG_ROW, mapIndex);
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
CiftiScalarFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
    return m_ciftiInterface->getCiftiXML().getMapPalette(CiftiXML::ALONG_ROW, mapIndex);
}


/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool
CiftiScalarFile::isMappedWithLabelTable() const
{
    return false;
}


/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Returns NULL since this file does not support label tables.
 */
GiftiLabelTable*
CiftiScalarFile::getMapLabelTable(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    return NULL;
}


/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Returns NULL since this file does not support label tables.
 */
const GiftiLabelTable*
CiftiScalarFile::getMapLabelTable(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    return NULL;
}

/**
 * Get the node coloring for the surface.
 * @param structure
 *    structure of surface whose nodes are colored.
 * @param mapIndex
 *    Index of map.
 * @param palette
 *    Palette used to color data.
 * @param nodeRGBA
 *    Filled with RGBA coloring for the surface's nodes.
 *    Contains numberOfNodes * 4 elements.
 * @param numberOfNodes
 *    Number of nodes in the surface.
 * @return
 *    True if coloring is valid, else false.
 */
bool
CiftiScalarFile::getSurfaceNodeColoring(const StructureEnum::Enum structure,
                                        const int32_t mapIndex,
                                        const Palette* palette,
                                               float* nodeRGBA,
                                               const int32_t numberOfNodes)
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    MapData* md = m_mapData[mapIndex];
    std::vector<CiftiSurfaceMap> nodeMap;
    
    m_ciftiInterface->getSurfaceMapForColumns(nodeMap,
                                                  structure);
    
    if (nodeMap.empty() == false) {
        if (m_isColoringValid == false) {
            const FastStatistics* statistics = getMapFastStatistics(mapIndex);
            const PaletteColorMapping* paletteColorMapping = getMapPaletteColorMapping(mapIndex);
            
            const AString paletteName = paletteColorMapping->getSelectedPaletteName();
            NodeAndVoxelColoring::colorScalarsWithPalette(md->m_fastStatistics,
                                                          getMapPaletteColorMapping(mapIndex),
                                                          palette,
                                                          md->m_data,
                                                          md->m_data,
                                                          m_numberOfDataElements,
                                                          md->m_dataRGBA);
            
            CaretLogFine("Connectivity Data Average/Min/Max: "
                         + QString::number(statistics->getMean())
                         + " "
                         + QString::number(statistics->getMostNegativeValue())
                         + " "
                         + QString::number(statistics->getMostPositiveValue()));
            m_isColoringValid = true;
        }
        
        std::fill(nodeRGBA, (nodeRGBA + (numberOfNodes * 4)), 0.0);
        const int64_t numNodeMaps = static_cast<int32_t>(nodeMap.size());
        for (int i = 0; i < numNodeMaps; i++) {
            const int64_t node4 = nodeMap[i].m_surfaceNode * 4;
            const int64_t cifti4 = nodeMap[i].m_ciftiIndex * 4;
            CaretAssertArrayIndex(nodeRGBA, (numberOfNodes * 4), node4);
            CaretAssertArrayIndex(this->dataRGBA, (m_numberOfDataElements * 4), cifti4);
            nodeRGBA[node4]   = md->m_dataRGBA[cifti4];
            nodeRGBA[node4+1] = md->m_dataRGBA[cifti4+1];
            nodeRGBA[node4+2] = md->m_dataRGBA[cifti4+2];
            nodeRGBA[node4+3] = md->m_dataRGBA[cifti4+3];
        }
        return true;
    }

    return false;
}

/**
 * Get connectivity value for a voxel at the given coordinate.
 * @param mapIndex
 *     Index of map for which data is requested.
 * @param xyz
 *     Coordinate of voxel.
 * @param ijkOut
 *     Voxel indices of value.
 * @param valueOut
 *     Output containing the node's value, value only if true returned.
 * @return
 *    true if a value was available for the voxel, else false.
 */
bool
CiftiScalarFile::getVolumeVoxelValue(const int32_t mapIndex,
                                     const float xyz[3],
                                            int64_t ijkOut[3],
                                            float &valueOut) const
{
    CaretAssertVectorIndex(m_mapData,
                           mapIndex);
    
    MapData* md = m_mapData[mapIndex];
    VolumeFile* volumeFile = md->getVolumeFile();
    if (volumeFile != NULL) {
        int64_t vfIJK[3];
        volumeFile->enclosingVoxel(xyz,
                                   vfIJK);
        if (volumeFile->indexValid(vfIJK)) {
            std::vector<CiftiVolumeMap> volumeMap;
            
            m_ciftiInterface->getVolumeMapForColumns(volumeMap);
            
            const int64_t numMaps = static_cast<int64_t>(volumeMap.size());
            for (int64_t i = 0; i < numMaps; i++) {
                if (volumeMap[i].m_ijk[0] == vfIJK[0]
                    && volumeMap[i].m_ijk[1] == vfIJK[1]
                    && volumeMap[i].m_ijk[2] == vfIJK[2]) {
                    CaretAssertArrayIndex(md->m_data,
                                          m_numberOfDataElements,
                                          volumeMap[i].m_ciftiIndex);
                    valueOut = md->m_data[volumeMap[i].m_ciftiIndex];
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
 * @param valueOut
 *     Output containing the node's value, value only if true returned.
 * @return
 *    true if a value was available for the node, else false.
 */
bool
CiftiScalarFile::getSurfaceNodeValue(const StructureEnum::Enum structure,
                                     const int32_t mapIndex,
                                            const int32_t nodeIndex,
                                            const int32_t numberOfNodes,
                                            float& valueOut) const
{
    CaretAssertVectorIndex(m_mapData,
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
                MapData* md = m_mapData[mapIndex];
                CaretAssertArrayIndex(md->m_data,
                                      m_numberOfDataElements,
                                      nodeMap[i].m_ciftiIndex);
                valueOut = md->m_data[nodeMap[i].m_ciftiIndex];
                return true;
            }
        }
    }
    
    return false;
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
CiftiScalarFile::updateScalarColoringForMap(const int32_t /*mapIndex*/,
                                       const PaletteFile* /*paletteFile*/)
{
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
CiftiScalarFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();
    
    try {
        setFileName("");
        
        m_isSurfaceMappable = false;
        m_isVolumeMappable  = false;
        
        m_ciftiDiskFile = new CiftiFile();
        m_ciftiDiskFile->openFile(filename,
                                  IN_MEMORY);
        m_ciftiInterface = m_ciftiDiskFile;
        
        m_numberOfDataElements = m_ciftiInterface->getNumberOfRows();
        if (m_numberOfDataElements < 1) {
            throw CiftiFileException("Invalid number of rows: "
                                     + AString::number(m_numberOfDataElements));
        }
        const int32_t numMaps = m_ciftiInterface->getNumberOfColumns();
        if (numMaps < 1) {
            throw CiftiFileException("Invalid number of columns: "
                                     + AString::number(numMaps));
        }
        
        for (int32_t i = 0; i < numMaps; i++) {
            MapData* md = new MapData(m_numberOfDataElements);
            m_ciftiInterface->getColumn(md->m_data,
                                        i);
            md->initializeMetaData(filename,
                                   m_numberOfDataElements);
            
            md->createVolumeFile(m_ciftiInterface,
                                 m_numberOfDataElements);
            
            m_mapData.push_back(md);
        }
        
        const CiftiXML& xml = m_ciftiInterface->getCiftiXML();
        
        std::vector<StructureEnum::Enum> surfaceStructures;
        std::vector<StructureEnum::Enum> volumeStructures;
        xml.getStructureListsForColumns(surfaceStructures,
                                        volumeStructures);
        
        m_isSurfaceMappable = (surfaceStructures.empty() == false);
        m_isVolumeMappable  = (volumeStructures.empty() == false);
        
        m_isColoringValid = false;
        
        setFileName(filename);
    }
    catch (const CiftiFileException& e) {
        clear();
        
        throw DataFileException(e);
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
CiftiScalarFile::writeFile(const AString& filename) throw (DataFileException)
{
    throw DataFileException("Writing "
                            + filename
                            + " not supported for file type "
                            + DataFileTypeEnum::toGuiName(getDataFileType()));
    this->setFileName(filename);
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   The event.
 */
void
CiftiScalarFile::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE) {
        EventSurfaceColoringInvalidate* colorEvent =
             dynamic_cast<EventSurfaceColoringInvalidate*>(event);
        CaretAssert(colorEvent);
        
        m_isColoringValid = false;
        
        colorEvent->setEventProcessed();
    }
}

//=========================================================================
/*
 * Map Data.
 */

/**
 * Constructor.
 */
CiftiScalarFile::MapData::MapData(const int32_t numberOfElements)
: CaretObject()
{
    CaretAssert(numberOfElements > 0);
    
    m_data                = new float[numberOfElements];
    m_dataRGBA            = new float[numberOfElements* 4];
    m_descriptiveStatistics = new DescriptiveStatistics();
    m_fastStatistics      = new FastStatistics();
    m_histogram           = new Histogram();
    m_metadata            = new GiftiMetaData();
    //m_paletteColorMapping = new PaletteColorMapping();
    m_volumeFile          = NULL;
    
    m_isColoringValid = false;
}

/**
 * Destructor.
 */
CiftiScalarFile::MapData::~MapData()
{
    delete[] m_data;
    delete[] m_dataRGBA;
    delete   m_descriptiveStatistics;
    delete   m_fastStatistics;
    delete   m_histogram;
    delete   m_metadata;
    //delete   m_paletteColorMapping;
    if (m_volumeFile != NULL) {
        delete m_volumeFile;
    }
}

/**
 * @return Volume file containing connectivity values.
 */
VolumeFile*
CiftiScalarFile::MapData::getVolumeFile()
{
    return m_volumeFile;
}

/**
 * Create a volume containig the connectivity values.
 * @param ciftiInterface
 *    The CIFTI Interface.
 */
void
CiftiScalarFile::MapData::createVolumeFile(CiftiInterface* ciftiInterface,
                                           const int32_t numberOfDataElements)
{
    m_volumeFile = NULL;
    
    VolumeFile::OrientTypes orientation[3];
    int64_t dimensions[3];
    float origin[3];
    float spacing[3];
    if (ciftiInterface->getVolumeAttributesForPlumb(orientation,
                                                          dimensions,
                                                          origin,
                                                          spacing) == false) {
        return;
    }
    
    if (dimensions[0] <= 0) {
        return;
    }
    vector<int64_t> dimensionsNew;
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
    
    vector<vector<float> > indexToSpace;
    indexToSpace.push_back(row1);
    indexToSpace.push_back(row2);
    indexToSpace.push_back(row3);
    
    int64_t numComponents = 1;
    
    m_volumeFile = new VolumeFile(dimensionsNew,
                                  indexToSpace,
                                  numComponents);
    
    std::vector<CiftiVolumeMap> volumeMaps;
    ciftiInterface->getVolumeMapForColumns(volumeMaps);
    
    
    if (volumeMaps.empty() == false) {
        m_volumeFile->setValueAllVoxels(0.0);
        
        for (std::vector<CiftiVolumeMap>::const_iterator iter = volumeMaps.begin();
             iter != volumeMaps.end();
             iter++) {
            const CiftiVolumeMap& vm = *iter;
            
            CaretAssertArrayIndex(m_data,
                                  numberOfDataElements,
                                  vm.m_ciftiIndex);
            m_volumeFile->setValue(m_data[vm.m_ciftiIndex],
                                   vm.m_ijk);
        }
    }    
}

/**
 * Initialize map metadata.
 * @param fileName
 *    Name of file containing map.
 * @param numberOfDataElements
 *    Number of elements in the data.
 */
void
CiftiScalarFile::MapData::initializeMetaData(const AString& /*fileName*/,
                                            const int32_t /*numberOfDataElements*/)
{
    //bool havePalette = false;//TODO: metadata is now stored inside the CiftiXML, change this?
    
    AString mapName = m_metadata->get(GiftiMetaDataXmlElements::METADATA_NAME_NAME);
    if (mapName.isEmpty()) {
        mapName = "scalars";
        m_metadata->set(GiftiMetaDataXmlElements::METADATA_NAME_NAME,
                        mapName);
    }
    
    /*const AString paletteString = m_metadata->get(GiftiMetaDataXmlElements::METADATA_NAME_PALETTE_COLOR_MAPPING);
    if (paletteString.isEmpty() == false) {
        try {
            m_paletteColorMapping->decodeFromStringXML(paletteString);
            m_paletteColorMapping->clearModified();
            havePalette = true;
        }
        catch (const XmlException& e) {
            m_paletteColorMapping = new PaletteColorMapping();
            CaretLogSevere("Failed to parse Palette XML: " + e.whatString());
        }
    }
    
    if (havePalette == false) {
        PaletteFile::setDefaultPaletteColorMapping(m_paletteColorMapping,
                                                   DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                                                   fileName,
                                                   mapName,
                                                   m_data,
                                                   numberOfDataElements);
    }//*/
}

