
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
 * SUBSTITUTE GOODS OR  SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <map>
#include <set>

#define __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__
#include "CiftiFiberTrajectoryFile.h"
#undef __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretSparseFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiInterface.h"
#include "ConnectivityDataLoaded.h"
#include "FiberOrientationTrajectory.h"
#include "FiberTrajectoryMapProperties.h"
#include "GiftiMetaData.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiFiberTrajectoryFile 
 * \brief File that contains trajectories
 */

/**
 * Constructor.
 */
CiftiFiberTrajectoryFile::CiftiFiberTrajectoryFile()
: CaretMappableDataFile(DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY)
{
    m_connectivityDataLoaded = new ConnectivityDataLoaded();
    m_fiberTrajectoryMapProperties = new FiberTrajectoryMapProperties();
    m_metadata = new GiftiMetaData();
    m_sparseFile = NULL;
    m_matchingFiberOrientationFile = NULL;
    m_matchingFiberOrientationFileName = "";
    m_dataLoadingEnabled = true;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_dataLoadingEnabled",
                          &m_dataLoadingEnabled);
    m_sceneAssistant->add("m_matchingFiberOrientationFileName",
                          &m_matchingFiberOrientationFileName);
    m_sceneAssistant->add("m_fiberTrajectoryMapProperties",
                          "FiberTrajectoryMapProperties",
                          m_fiberTrajectoryMapProperties);
    m_sceneAssistant->add("m_connectivityDataLoaded",
                          "ConnectivityDataLoaded",
                          m_connectivityDataLoaded);
}

/**
 * Destructor.
 */
CiftiFiberTrajectoryFile::~CiftiFiberTrajectoryFile()
{
    clearPrivate();
    delete m_fiberTrajectoryMapProperties;
    delete m_metadata;
    // DO NOT DELETE (owned by Brain):  m_matchingFiberOrientationFile.

    delete m_sceneAssistant;
    delete m_connectivityDataLoaded;
    
}

/**
 * Cleare data in this file.
 */
void
CiftiFiberTrajectoryFile::clear()
{
    CaretMappableDataFile::clear();
    
    clearPrivate();
}


/**
 * Cleare data in this file but not the parent class.
 */
void
CiftiFiberTrajectoryFile::clearPrivate()
{
    m_metadata->clear();
        
    clearLoadedFiberOrientations();
    
    if (m_sparseFile != NULL) {
        delete m_sparseFile;
        m_sparseFile = NULL;
    }
    
    m_matchingFiberOrientationFile = NULL;
    m_matchingFiberOrientationFileName = "";
}


/**
 * @return True if the file is empty.
 */
bool
CiftiFiberTrajectoryFile::isEmpty() const
{
    if (m_sparseFile != NULL) {
        return false;
    }
    return true;
}

/**
 * @return Is data loading enabled?
 */
bool
CiftiFiberTrajectoryFile::isDataLoadingEnabled() const
{
    return m_dataLoadingEnabled;
}

/**
 * Set data loading enabled.
 *
 * @param loadingEnabled
 *    New status of data loading.
 */
void
CiftiFiberTrajectoryFile::setDataLoadingEnabled(const bool loadingEnabled)
{
    m_dataLoadingEnabled = loadingEnabled;
}

/**
 * @return The selected matching fiber orientation file. May be NULL.
 */
const CiftiFiberOrientationFile*
CiftiFiberTrajectoryFile::getMatchingFiberOrientationFile() const
{
    return m_matchingFiberOrientationFile;
}

/**
 * @return The selected matching fiber orientation file. May be NULL.
 */
CiftiFiberOrientationFile*
CiftiFiberTrajectoryFile::getMatchingFiberOrientationFile()
{
    return m_matchingFiberOrientationFile;
}

/**
 * Is the given fiber orientation file compatible with this fiber trajectory file
 *
 * @param fiberOrientationFile
 *    File tested for compatibilty
 * @return
 *    True if file is compatible, else false.
 */
bool
CiftiFiberTrajectoryFile::isFiberOrientationFileCombatible(const CiftiFiberOrientationFile* fiberOrientationFile) const
{
    CaretAssert(fiberOrientationFile);
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiXML* orientXML = fiberOrientationFile->getCiftiXML();
    if (trajXML.mappingMatches(CiftiXML::ALONG_ROW,
                               *orientXML,
                               CiftiXML::ALONG_COLUMN)) {
        return true;
    }
    
    return false;
}


/**
 * Set the selected matching fiber orientation file.  No test of compatibility
 * is made.
 *
 * @param matchingFiberOrientationFile
 *    New selection for matching fiber orientation file.
 */
void
CiftiFiberTrajectoryFile::setMatchingFiberOrientationFile(CiftiFiberOrientationFile* matchingFiberOrientationFile)
{
    m_matchingFiberOrientationFile = matchingFiberOrientationFile;
    if (m_matchingFiberOrientationFile != NULL) {
        m_matchingFiberOrientationFileName = m_matchingFiberOrientationFile->getFileNameNoPath();
    }
    else {
        m_matchingFiberOrientationFileName = "";
    }
}

/**
 * Update the matching fiber orientation file from the first compatible file in the list.
 * If none are found, the matching file will become NULL.  If the current matching file
 * is valid, no action is taken.
 *
 * @param matchingFiberOrientationFiles
 *    The fiber orientation files.
 */
void
CiftiFiberTrajectoryFile::updateMatchingFiberOrientationFileFromList(std::vector<CiftiFiberOrientationFile*> matchingFiberOrientationFiles)
{
    /*
     * See if selected orientation file is still valid
     */
    for (std::vector<CiftiFiberOrientationFile*>::iterator iter = matchingFiberOrientationFiles.begin();
         iter != matchingFiberOrientationFiles.end();
         iter++) {
        if (*iter == m_matchingFiberOrientationFile) {
            return;
        }
    }
    
    /*
     * Invalidate matching file
     */
    m_matchingFiberOrientationFile = NULL;
    m_matchingFiberOrientationFileName = "";
    clearLoadedFiberOrientations();

    /*
     * If a scene has been restored, we want to match to the fiber orientation
     * file name that was restored from the scene
     */
    if (m_matchingFiberOrientationFileNameFromRestoredScene.isEmpty() == false) {
        bool matched = false;
        
        for (std::vector<CiftiFiberOrientationFile*>::iterator iter = matchingFiberOrientationFiles.begin();
             iter != matchingFiberOrientationFiles.end();
             iter++) {
            /*
             * Try and see if it matches for this file
             */
            CiftiFiberOrientationFile* orientationFile = *iter;
            if (orientationFile->getFileNameNoPath() == m_matchingFiberOrientationFileNameFromRestoredScene) {
                if (isFiberOrientationFileCombatible(orientationFile)) {
                    setMatchingFiberOrientationFile(orientationFile);
                    matched = true;
                }
            }
        }
        
        /*
         * Clear name so no attempt to use again
         */
        m_matchingFiberOrientationFileNameFromRestoredScene = "";
        
        if (matched) {
            return;
        }
    }
    
    /*
     * Try to find a matching file
     */
    for (std::vector<CiftiFiberOrientationFile*>::iterator iter = matchingFiberOrientationFiles.begin();
         iter != matchingFiberOrientationFiles.end();
         iter++) {
        /*
         * Try and see if it matches for this file
         */
        if (isFiberOrientationFileCombatible(*iter)) {
            setMatchingFiberOrientationFile(*iter);
            return;
        }
    }
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
CiftiFiberTrajectoryFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
CiftiFiberTrajectoryFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
CiftiFiberTrajectoryFile::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
CiftiFiberTrajectoryFile::getFileMetaData() const
{
    return m_metadata;
}

/**
 * @return Is the data mappable to a surface?
 */
bool
CiftiFiberTrajectoryFile::isSurfaceMappable() const
{
    return false;
}

/**
 * @return Is the data mappable to a volume?
 */
bool
CiftiFiberTrajectoryFile::isVolumeMappable() const
{
    return true;
}

/**
 * @return The number of maps in the file.
 * Note: Caret5 used the term 'columns'.
 */
int32_t
CiftiFiberTrajectoryFile::getNumberOfMaps() const
{
    /*
     * Always return 1.
     * If zero is returned, it will never appear in the overlays because
     * zero is interpreted as "nothing available".
     */
    return 1;
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
CiftiFiberTrajectoryFile::hasMapAttributes() const
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
CiftiFiberTrajectoryFile::getMapName(const int32_t /*mapIndex*/) const
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
CiftiFiberTrajectoryFile::setMapName(const int32_t /*mapIndex*/,
                                  const AString& /*mapName*/)
{
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
CiftiFiberTrajectoryFile::getMapMetaData(const int32_t /*mapIndex*/) const
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
CiftiFiberTrajectoryFile::getMapMetaData(const int32_t /*mapIndex*/)
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
CiftiFiberTrajectoryFile::getMapUniqueID(const int32_t mapIndex) const
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
CiftiFiberTrajectoryFile::isMappedWithPalette() const
{
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
CiftiFiberTrajectoryFile::getMapStatistics(const int32_t /*mapIndex*/)
{
    return NULL;
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
CiftiFiberTrajectoryFile::getMapFastStatistics(const int32_t /*mapIndex*/)
{
    return NULL;
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
CiftiFiberTrajectoryFile::getMapHistogram(const int32_t /*mapIndex*/)
{
    return NULL;
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
CiftiFiberTrajectoryFile::getMapStatistics(const int32_t /*mapIndex*/,
                                        const float /*mostPositiveValueInclusive*/,
                                        const float /*leastPositiveValueInclusive*/,
                                        const float /*leastNegativeValueInclusive*/,
                                        const float /*mostNegativeValueInclusive*/,
                                        const bool /*includeZeroValues*/)
{
    return NULL;
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
CiftiFiberTrajectoryFile::getMapHistogram(const int32_t /*mapIndex*/,
                                       const float /*mostPositiveValueInclusive*/,
                                       const float /*leastPositiveValueInclusive*/,
                                       const float /*leastNegativeValueInclusive*/,
                                       const float /*mostNegativeValueInclusive*/,
                                       const bool /*includeZeroValues*/)
{
    return NULL;
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
CiftiFiberTrajectoryFile::getMapPaletteColorMapping(const int32_t /*mapIndex*/)
{
    return NULL;
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
CiftiFiberTrajectoryFile::getMapPaletteColorMapping(const int32_t /*mapIndex*/) const
{
    return NULL;
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool
CiftiFiberTrajectoryFile::isMappedWithLabelTable() const
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
CiftiFiberTrajectoryFile::getMapLabelTable(const int32_t /*mapIndex*/)
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
CiftiFiberTrajectoryFile::getMapLabelTable(const int32_t /*mapIndex*/) const
{
    return NULL;
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
CiftiFiberTrajectoryFile::updateScalarColoringForMap(const int32_t /*mapIndex*/,
                                                  const PaletteFile* /*paletteFile*/)
{
}

/**
 * @return The fiber trajectory map properties (const method).
 */
FiberTrajectoryMapProperties*
CiftiFiberTrajectoryFile::getFiberTrajectoryMapProperties()
{
    return m_fiberTrajectoryMapProperties;
}

/**
 * @return The fiber trajectory map properties.
 */
const FiberTrajectoryMapProperties*
CiftiFiberTrajectoryFile::getFiberTrajectoryMapProperties() const
{
    return m_fiberTrajectoryMapProperties;
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
CiftiFiberTrajectoryFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();

    checkFileReadability(filename);
    
    try {
        m_sparseFile = new CaretSparseFile();
        m_sparseFile->readFile(filename);
        setFileName(filename);
        
        clearModified();
    }
    catch (const DataFileException& dfe) {
        clear();
        throw dfe;
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
CiftiFiberTrajectoryFile::writeFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Writing of Cifti Trajectory Files not supported.");
}

/**
 * Clear the loaded fiber orientations.
 */
void
CiftiFiberTrajectoryFile::clearLoadedFiberOrientations()
{
    const int64_t numFibers = static_cast<int64_t>(m_fiberOrientationTrajectories.size());
    for (int64_t i = 0; i < numFibers; i++) {
        delete m_fiberOrientationTrajectories[i];
    }
    m_fiberOrientationTrajectories.clear();
    
    m_loadedDataDescriptionForMapName = "";
    
    m_connectivityDataLoaded->reset();
}

/**
 * Validate that the assigned matching fiber orientation file is valid
 * (not NULL and row/column is compatible).
 *
 * @throws DataFileException 
 *    If fiber orientation file is NULL or incompatible.
 */
void
CiftiFiberTrajectoryFile::validateAssignedMatchingFiberOrientationFile() throw (DataFileException)
{
    if (m_sparseFile == NULL) {
        throw DataFileException("No data has been loaded.");
    }
    if (m_matchingFiberOrientationFile == NULL) {
        throw DataFileException("No fiber orientation file is assigned.");
    }
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiXML* orientXML = m_matchingFiberOrientationFile->getCiftiXML();
    if (trajXML.mappingMatches(CiftiXML::ALONG_ROW,
                               *orientXML,
                               CiftiXML::ALONG_COLUMN) == false) {
        QString msg = (getFileNameNoPath()
                       + " rows="
                       + QString::number(trajXML.getNumberOfRows())
                       + " cols="
                       + QString::number(trajXML.getNumberOfColumns())
                       + "   "
                       + m_matchingFiberOrientationFile->getFileNameNoPath()
                       + " rows="
                       + QString::number(orientXML->getNumberOfRows())
                       + " cols="
                       + QString::number(orientXML->getNumberOfColumns()));
        throw DataFileException("Row to Columns do not match: "
                                + msg);
    }
}

/**
 * Load data for the given surface node.
 * @param structure
 *    Structure in which surface node is located.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the surface node.
 */
void
CiftiFiberTrajectoryFile::loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                                 const int32_t surfaceNumberOfNodes,
                                                 const int32_t nodeIndex) throw (DataFileException)
{
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    clearLoadedFiberOrientations();
    
    validateAssignedMatchingFiberOrientationFile();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    if (trajXML.hasColumnSurfaceData(structure) == false) {
        return;
    }
    if (trajXML.getSurfaceNumberOfNodes(CiftiXML::ALONG_COLUMN, structure) != surfaceNumberOfNodes) {
        return;
    }
    
    const int64_t rowIndex = trajXML.getRowIndexForNode(nodeIndex,
                                                        structure);
    if (rowIndex < 0) {
        return;
    }
    
    std::vector<int64_t> fiberIndices;
    std::vector<FiberFractions> fiberFractions;
    m_sparseFile->getFibersRowSparse(rowIndex,
                                     fiberIndices,
                                     fiberFractions);
    CaretAssert(fiberIndices.size() == fiberFractions.size());
    
    const int64_t numFibers = static_cast<int64_t>(fiberIndices.size());
    
    CaretLogFine("For node "
                   + AString::number(nodeIndex)
                   + " number of rows loaded: "
                   + AString::number(numFibers));
    
    if (numFibers > 0) {
        m_fiberOrientationTrajectories.reserve(numFibers);
        
        for (int64_t iFiber = 0; iFiber < numFibers; iFiber++) {
            const int64_t numFiberOrientations = m_matchingFiberOrientationFile->getNumberOfFiberOrientations();
            const int64_t fiberIndex = fiberIndices[iFiber];
            if (fiberIndex < numFiberOrientations) {
                const FiberOrientation* fiberOrientation = m_matchingFiberOrientationFile->getFiberOrientations(fiberIndex);
                FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberOrientation,
                                                                                 rowIndex);
                fot->addFiberFractions(fiberFractions[iFiber]);
                m_fiberOrientationTrajectories.push_back(fot);
            }
            else{
                CaretLogSevere("Invalid index="
                               + QString::number(fiberIndex)
                               + " into fiber orientations");
            }
        }
        
        m_loadedDataDescriptionForMapName = ("Row: "
                                             + AString::number(rowIndex)
                                             + ", Node Index: "
                                             + AString::number(nodeIndex)
                                             + ", Structure: "
                                             + StructureEnum::toName(structure));
        
        m_connectivityDataLoaded->setSurfaceNodeLoading(structure,
                                                        surfaceNumberOfNodes,
                                                        nodeIndex);
    }
}

/**
 * Load average data for the given surface nodes.
 *
 * @param structure
 *    Structure in which surface node is located.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param nodeIndices
 *    Indices of the surface nodes.
 */
void
CiftiFiberTrajectoryFile::loadDataAverageForSurfaceNodes(const StructureEnum::Enum structure,
                                                         const int32_t surfaceNumberOfNodes,
                                                         const std::vector<int32_t>& nodeIndices) throw (DataFileException)
{
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    clearLoadedFiberOrientations();
    
    if (surfaceNumberOfNodes <= 0) {
        return;
    }
    
    validateAssignedMatchingFiberOrientationFile();

    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    
    if (trajXML.hasColumnSurfaceData(structure) == false) {
        return;
    }
    if (trajXML.getSurfaceNumberOfNodes(CiftiXML::ALONG_COLUMN, structure) != surfaceNumberOfNodes) {
        return;
    }
    
    /*
     * This map uses the index of a fiber orientation (from the Fiber Orientation File)
     * to a FiberOrientationTrajectory instance.  For averaging, items that have
     * a matching fiber orientation index are averaged.
     */
    std::map<int64_t, FiberOrientationTrajectory*> fiberOrientationIndexMapToFiberTrajectory;
    
    const int32_t numberOfNodes = static_cast<int32_t>(nodeIndices.size());
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t nodeIndex = nodeIndices[i];
        
        /*
         * Get and load row for node
         */
        const int64_t rowIndex = trajXML.getRowIndexForNode(nodeIndex,
                                                            structure);
        if (rowIndex >= 0) {
            std::vector<int64_t> fiberOrientationIndicesForRow;
            std::vector<FiberFractions> fiberFractions;
            m_sparseFile->getFibersRowSparse(rowIndex,
                                             fiberOrientationIndicesForRow,
                                             fiberFractions);
            
            CaretAssert(fiberOrientationIndicesForRow.size() == fiberFractions.size());
            
            /*
             * Process trajectory for node
             */
            const int64_t numItems = static_cast<int64_t>(fiberOrientationIndicesForRow.size());
            if (numItems > 0) {
                for (int64_t indx = 0; indx < numItems; indx++) {
                    const int64_t fiberOrientationIndex = fiberOrientationIndicesForRow[indx];
                    
                    /*
                     * See if the trajectory for the orientation has already been created
                     */
                    std::map<int64_t, FiberOrientationTrajectory*>::iterator trajIter = fiberOrientationIndexMapToFiberTrajectory.find(fiberOrientationIndex);
                    if (trajIter != fiberOrientationIndexMapToFiberTrajectory.end()) {
                        /*
                         * Add additional fiber fractions
                         */
                        FiberOrientationTrajectory* fot = trajIter->second;
                        CaretAssert(fot);
                        fot->addFiberFractions(fiberFractions[indx]);
                    }
                    else {
                        /*
                         * Create a new trajectory
                         */
                        const FiberOrientation* fiberOrientation = m_matchingFiberOrientationFile->getFiberOrientations(fiberOrientationIndex);
                        FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberOrientation,
                                                                                         rowIndex);
                        m_fiberOrientationTrajectories.push_back(fot);
                        
                        /*
                         * Add fiber fractions to trajectory
                         */
                        fot->addFiberFractions(fiberFractions[indx]);
                        
                        /*
                         * Add to map keyed by fiber orientation index for averaging
                         */
                        fiberOrientationIndexMapToFiberTrajectory.insert(std::make_pair(fiberOrientationIndex,
                                                                                        fot));
                    }
                }
            }
        }
    }
    
    m_connectivityDataLoaded->setSurfaceAverageNodeLoading(structure,
                                                           surfaceNumberOfNodes,
                                                           nodeIndices);
    
    m_loadedDataDescriptionForMapName = ("Structure: "
                                         + StructureEnum::toName(structure)
                                         + ", Averaged Node Count: "
                                         + AString::number(numberOfNodes));
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * @param xyz
 *    Coordinate of voxel.
 * @throw
 *    DataFileException if there is an error.
 */
void
CiftiFiberTrajectoryFile::loadMapDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    clearLoadedFiberOrientations();
    
    validateAssignedMatchingFiberOrientationFile();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
//    if (trajXML.hasColumnVolumeData() == false) {
//        return;
//    }

    const int64_t rowIndex = trajXML.getRowIndexForVoxelCoordinate(xyz);
    if (rowIndex < 0) {
        return;
    }
    
    std::vector<int64_t> fiberIndices;
    std::vector<FiberFractions> fiberFractions;
    m_sparseFile->getFibersRowSparse(rowIndex,
                                     fiberIndices,
                                     fiberFractions);
    CaretAssert(fiberIndices.size() == fiberFractions.size());
    
    const int64_t numFibers = static_cast<int64_t>(fiberIndices.size());
    
    CaretLogFine("For voxel at coordinate "
                 + AString::fromNumbers(xyz, 3, ",")
                 + " number of rows loaded: "
                 + AString::number(numFibers));
    
    if (numFibers > 0) {
        m_fiberOrientationTrajectories.reserve(numFibers);
        
        for (int64_t iFiber = 0; iFiber < numFibers; iFiber++) {
            const int64_t numFiberOrientations = m_matchingFiberOrientationFile->getNumberOfFiberOrientations();
            const int64_t fiberIndex = fiberIndices[iFiber];
            if (fiberIndex < numFiberOrientations) {
                const FiberOrientation* fiberOrientation = m_matchingFiberOrientationFile->getFiberOrientations(fiberIndex);
                FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberOrientation,
                                                                                 rowIndex);
                fot->addFiberFractions(fiberFractions[iFiber]);
                m_fiberOrientationTrajectories.push_back(fot);
            }
            else{
                CaretLogSevere("Invalid index="
                               + QString::number(fiberIndex)
                               + " into fiber orientations");
            }
        }
        
        m_loadedDataDescriptionForMapName = ("Row: "
                                             + AString::number(rowIndex)
                                             + ", Voxel XYZ: "
                                             + AString::fromNumbers(xyz, 3, ",")
                                             + ", Structure: ");
        
        m_connectivityDataLoaded->setVolumeXYZLoading(xyz);
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
void
CiftiFiberTrajectoryFile::loadMapAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                                                           const std::vector<VoxelIJK>& voxelIndices) throw (DataFileException)
{
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    clearLoadedFiberOrientations();
    
    validateAssignedMatchingFiberOrientationFile();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    
    if (trajXML.hasColumnVolumeData() == false) {
        return;
    }
    
    /*
     * This map uses the index of a fiber orientation (from the Fiber Orientation File)
     * to a FiberOrientationTrajectory instance.  For averaging, items that have
     * a matching fiber orientation index are averaged.
     */
    std::map<int64_t, FiberOrientationTrajectory*> fiberOrientationIndexMapToFiberTrajectory;
    
    const int32_t numberOfVoxels = static_cast<int32_t>(voxelIndices.size());
    for (int32_t i = 0; i < numberOfVoxels; i++) {
        /*
         * Get and load row for voxel
         */
        const int64_t rowIndex = trajXML.getRowIndexForVoxel(voxelIndices[i].m_ijk);
        if (rowIndex >= 0) {
            std::vector<int64_t> fiberOrientationIndicesForRow;
            std::vector<FiberFractions> fiberFractions;
            m_sparseFile->getFibersRowSparse(rowIndex,
                                             fiberOrientationIndicesForRow,
                                             fiberFractions);
            
            CaretAssert(fiberOrientationIndicesForRow.size() == fiberFractions.size());
            
            /*
             * Process trajectory for node
             */
            const int64_t numItems = static_cast<int64_t>(fiberOrientationIndicesForRow.size());
            if (numItems > 0) {
                for (int64_t indx = 0; indx < numItems; indx++) {
                    const int64_t fiberOrientationIndex = fiberOrientationIndicesForRow[indx];
                    
                    /*
                     * See if the trajectory for the orientation has already been created
                     */
                    std::map<int64_t, FiberOrientationTrajectory*>::iterator trajIter = fiberOrientationIndexMapToFiberTrajectory.find(fiberOrientationIndex);
                    if (trajIter != fiberOrientationIndexMapToFiberTrajectory.end()) {
                        /*
                         * Add additional fiber fractions
                         */
                        FiberOrientationTrajectory* fot = trajIter->second;
                        CaretAssert(fot);
                        fot->addFiberFractions(fiberFractions[indx]);
                    }
                    else {
                        /*
                         * Create a new trajectory
                         */
                        const FiberOrientation* fiberOrientation = m_matchingFiberOrientationFile->getFiberOrientations(fiberOrientationIndex);
                        FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberOrientation,
                                                                                         rowIndex);
                        m_fiberOrientationTrajectories.push_back(fot);
                        
                        /*
                         * Add fiber fractions to trajectory
                         */
                        fot->addFiberFractions(fiberFractions[indx]);
                        
                        /*
                         * Add to map keyed by fiber orientation index for averaging
                         */
                        fiberOrientationIndexMapToFiberTrajectory.insert(std::make_pair(fiberOrientationIndex,
                                                                                        fot));
                    }
                }
            }
        }
    }
    
    m_connectivityDataLoaded->setVolumeAverageVoxelLoading(volumeDimensionIJK,
                                                           voxelIndices);
    
    m_loadedDataDescriptionForMapName = ("Averaged Voxel Count: "
                                 + AString::number(numberOfVoxels));
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
CiftiFiberTrajectoryFile::finishRestorationOfScene() throw (DataFileException)
{
    /*
     * Loading of data may be disabled in the scene
     * so temporarily enabled loading and then 
     * restore the status.
     */
    const bool loadingEnabledStatus = isDataLoadingEnabled();
    setDataLoadingEnabled(true);
    
    switch (m_connectivityDataLoaded->getMode()) {
        case ConnectivityDataLoaded::MODE_NONE:
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE:
        {
            StructureEnum::Enum structure;
            int32_t surfaceNumberOfNodes;
            int32_t surfaceNodeIndex;
            m_connectivityDataLoaded->getSurfaceNodeLoading(structure,
                                                            surfaceNumberOfNodes,
                                                            surfaceNodeIndex);
            loadDataForSurfaceNode(structure,
                                   surfaceNumberOfNodes,
                                   surfaceNodeIndex);
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
            loadDataAverageForSurfaceNodes(structure,
                                   surfaceNumberOfNodes,
                                   surfaceNodeIndices);
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_XYZ:
        {
            float volumeXYZ[3];
            m_connectivityDataLoaded->getVolumeXYZLoading(volumeXYZ);
            CaretAssert(0); // NEED TO IMPLEMENT
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
        {
            int64_t volumeDimensionsIJK[3];
            std::vector<VoxelIJK> voxelIndicesIJK;
            m_connectivityDataLoaded->getVolumeAverageVoxelLoading(volumeDimensionsIJK,
                                                                   voxelIndicesIJK);
            CaretAssert(0); // NEED TO IMPLEMENT
        }
            break;
    }
    
    setDataLoadingEnabled(loadingEnabledStatus);
}

/**
 * @return a REFERENCE to the fiber fractions that were loaded.
 */
const std::vector<FiberOrientationTrajectory*>&
CiftiFiberTrajectoryFile::getLoadedFiberOrientationTrajectories() const
{
    return m_fiberOrientationTrajectories;
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
CiftiFiberTrajectoryFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
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
CiftiFiberTrajectoryFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                       const SceneClass* sceneClass)
{
    m_connectivityDataLoaded->reset();
    
    CaretMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    m_matchingFiberOrientationFileNameFromRestoredScene = m_matchingFiberOrientationFileName;
}



