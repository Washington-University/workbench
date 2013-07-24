
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
#include "FiberOrientationTrajectory.h"
#include "FiberTrajectoryMapProperties.h"
#include "GiftiMetaData.h"
#include "SceneClass.h"

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
    m_fiberTrajectoryMapProperties = new FiberTrajectoryMapProperties();
    m_metadata = new GiftiMetaData();
    m_sparseFile = NULL;
    m_dataLoadingEnabled = true;
}

/**
 * Destructor.
 */
CiftiFiberTrajectoryFile::~CiftiFiberTrajectoryFile()
{
    clearPrivate();
    delete m_fiberTrajectoryMapProperties;
    delete m_metadata;
    
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
}


/**
 * @return True if the file is empty.
 */
bool
CiftiFiberTrajectoryFile::isEmpty() const
{
    if (m_sparseFile != NULL) {
        return true;
    }
    return false;
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
}

/**
 * Load data for the given surface node.
 * @param fiberOrientFile
 *    Fiber orientations.
 * @param structure
 *    Structure in which surface node is located.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the surface node.
 */
void
CiftiFiberTrajectoryFile::loadDataForSurfaceNode(CiftiFiberOrientationFile* fiberOrientFile,
                                                 const StructureEnum::Enum structure,
                                                 const int32_t surfaceNumberOfNodes,
                                                 const int32_t nodeIndex) throw (DataFileException)
{
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    clearLoadedFiberOrientations();
    
    if (m_sparseFile == NULL) {
        throw DataFileException("No data has been loaded.");
    }
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiXML* orientXML = fiberOrientFile->getCiftiXML();
    if (trajXML.mappingMatches(CiftiXML::ALONG_ROW,
                               *orientXML,
                               CiftiXML::ALONG_COLUMN) == false) {
        QString msg = (getFileNameNoPath()
                       + " rows="
                       + QString::number(trajXML.getNumberOfRows())
                       + " cols="
                       + QString::number(trajXML.getNumberOfColumns())
                       + "   "
                       + fiberOrientFile->getFileNameNoPath()
                       + " rows="
                       + QString::number(orientXML->getNumberOfRows())
                       + " cols="
                       + QString::number(orientXML->getNumberOfColumns()));
        throw DataFileException("Row to Columns do not match: "
                                + msg);
    }
    
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
            const int64_t numFiberOrientations = fiberOrientFile->getNumberOfFiberOrientations();
            const int64_t fiberIndex = fiberIndices[iFiber];
            if (fiberIndex < numFiberOrientations) {
                const FiberOrientation* fiberOrientation = fiberOrientFile->getFiberOrientations(fiberIndex);
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
    }
}

/**
 * Load average data for the given surface nodes.
 * @param fiberOrientFile
 *    Fiber orientations.
 * @param structure
 *    Structure in which surface node is located.
 * @param nodeIndices
 *    Indices of the surface nodes.
 */
void
CiftiFiberTrajectoryFile::loadDataAverageForSurfaceNodes(CiftiFiberOrientationFile* fiberOrientFile,
                                                         const StructureEnum::Enum structure,
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
    
    if (m_sparseFile == NULL) {
        throw DataFileException("No data has been loaded.");
    }
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiXML* orientXML = fiberOrientFile->getCiftiXML();
    if (trajXML.mappingMatches(CiftiXML::ALONG_ROW,
                               *orientXML,
                               CiftiXML::ALONG_COLUMN) == false) {
        QString msg = (getFileNameNoPath()
                       + " rows="
                       + QString::number(trajXML.getNumberOfRows())
                       + " cols="
                       + QString::number(trajXML.getNumberOfColumns())
                       + "   "
                       + fiberOrientFile->getFileNameNoPath()
                       + " rows="
                       + QString::number(orientXML->getNumberOfRows())
                       + " cols="
                       + QString::number(orientXML->getNumberOfColumns()));
        throw DataFileException("Row to Columns do not match: "
                                + msg);
    }
    
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
                        const FiberOrientation* fiberOrientation = fiberOrientFile->getFiberOrientations(fiberOrientationIndex);
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
    
    m_loadedDataDescriptionForMapName = ("Structure: "
                                         + StructureEnum::toName(structure)
                                         + ", Averaged Node Count: "
                                         + AString::number(numberOfNodes));
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

    sceneClass->addBoolean("m_dataLoadingEnabled",
                           m_dataLoadingEnabled);
    sceneClass->addClass(m_fiberTrajectoryMapProperties->saveToScene(sceneAttributes,
                                                                     "m_fiberTrajectoryMapProperties"));
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
    CaretMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    m_dataLoadingEnabled = sceneClass->getBooleanValue("m_dataLoadingEnabled",
                                                       true);
    
    m_fiberTrajectoryMapProperties->restoreFromScene(sceneAttributes,
                                                     sceneClass->getClass("m_fiberTrajectoryMapProperties"));
}



