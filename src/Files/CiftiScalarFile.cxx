
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

#define __CIFTI_SCALAR_FILE_DECLARE__
#include "CiftiScalarFile.h"
#undef __CIFTI_SCALAR_FILE_DECLARE__

#include "CiftiInterface.h"
#include "CiftiXML.h"
#include "ConnectivityLoaderFile.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
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
    m_ciftiFile = NULL;
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
    
    delete m_ciftiFile;
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
    if (m_ciftiFile != NULL) {
        delete m_ciftiFile;
    }
    
    m_ciftiFile = new ConnectivityLoaderFile();
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
    return m_ciftiFile->isEmpty();
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
    return m_ciftiFile->getFileMetaData();
}


/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
CiftiScalarFile::getFileMetaData() const
{
    return m_ciftiFile->getFileMetaData();
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
    return 1;
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
CiftiScalarFile::getMapName(const int32_t /* mapIndex */) const
{
    return "scalars";
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
CiftiScalarFile::setMapName(const int32_t /*mapIndex*/,
                        const AString& /*mapName*/)
{
    /* nothing */
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
CiftiScalarFile::getMapMetaData(const int32_t /* mapIndex */) const
{
    return getFileMetaData();
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
CiftiScalarFile::getMapMetaData(const int32_t /*mapIndex */)
{
    return getFileMetaData();
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
CiftiScalarFile::getMapUniqueID(const int32_t /* mapIndex */) const
{
    return m_uniqueID;
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
    return m_ciftiFile->getMapStatistics(mapIndex);
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
    return m_ciftiFile->getMapFastStatistics(mapIndex);
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
    return m_ciftiFile->getMapHistogram(mapIndex);
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
CiftiScalarFile::getMapStatistics(const int32_t mapIndex,
                                                      const float mostPositiveValueInclusive,
                                                      const float leastPositiveValueInclusive,
                                                      const float leastNegativeValueInclusive,
                                                      const float mostNegativeValueInclusive,
                                                      const bool includeZeroValues)
{
    return m_ciftiFile->getMapStatistics(mapIndex,
                                         mostPositiveValueInclusive,
                                         leastPositiveValueInclusive,
                                         leastNegativeValueInclusive,
                                         mostNegativeValueInclusive,
                                         includeZeroValues);
}


const Histogram*
CiftiScalarFile::getMapHistogram(const int32_t mapIndex,
                                         const float mostPositiveValueInclusive,
                                         const float leastPositiveValueInclusive,
                                         const float leastNegativeValueInclusive,
                                         const float mostNegativeValueInclusive,
                                         const bool includeZeroValues)
{
    return m_ciftiFile->getMapHistogram(mapIndex,
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
CiftiScalarFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    return m_ciftiFile->getMapPaletteColorMapping(mapIndex);
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
    return m_ciftiFile->getMapPaletteColorMapping(mapIndex);
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
CiftiScalarFile::getMapLabelTable(const int32_t /* mapIndex */)
{
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
CiftiScalarFile::getMapLabelTable(const int32_t /* mapIndex */) const
{
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
    if (m_isColoringValid == false) {
        m_ciftiFile->updateRGBAColoring(palette, mapIndex);
        m_isColoringValid = true;
    }
    
    return m_ciftiFile->getSurfaceNodeColoring(structure,
                                               nodeRGBA,
                                               numberOfNodes);
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
    setFileName("");
    
    m_isSurfaceMappable = false;
    m_isVolumeMappable  = false;
    if (DataFile::isFileOnNetwork(filename)) {
        m_ciftiFile->setupNetworkFile(filename,
                              DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                                      "",
                                      "");
//                              m_fileReadingUsername,
//                              m_fileReadingPassword);
    }
    else {
        m_ciftiFile->setupLocalFile(filename,
                            DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR);
    }
    
    const CiftiXML& xml = m_ciftiFile->ciftiInterface->getCiftiXML();
    
    if (xml.getNumberOfColumns() != 1) {
        m_ciftiFile->clear();
        throw DataFileException(filename
                                + " contains "
                                + AString::number(xml.getNumberOfColumns())
                                + " but only one column supported at this time.");
    }
    if (xml.getNumberOfRows() <= 0) {
        m_ciftiFile->clear();
        throw DataFileException(filename
                                + " contains no columns of data.");
    }
    
    std::vector<StructureEnum::Enum> surfaceStructures;
    std::vector<StructureEnum::Enum> volumeStructures;
    xml.getStructureListsForColumns(surfaceStructures,
                                    volumeStructures);
    
    m_isSurfaceMappable = (surfaceStructures.empty() == false);
    m_isVolumeMappable  = (volumeStructures.empty() == false);

    m_isColoringValid = false;
    
    setFileName(filename);
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
