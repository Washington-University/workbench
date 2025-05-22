
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

#define __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__
#include "CiftiFiberTrajectoryFile.h"
#undef __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretSparseFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiMappableDataFile.h"
#include "ConnectivityDataLoaded.h"
#include "DataFileContentInformation.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "FiberOrientationTrajectory.h"
#include "FiberTrajectoryMapProperties.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "PaletteColorMapping.h"
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
    m_fiberTrajectoryFileType = FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE;
    
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
    
    m_fiberTrajectoryFileType = FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE;
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
    if (*(trajXML.getMap(CiftiXML::ALONG_ROW)) == *(orientXML->getMap(CiftiXML::ALONG_COLUMN))) {
        return true;
    }
    
    return false;
}


/**
 * Set the selected matching fiber orientation file.  No test of compatibility
 * is made.  If this is a "single row" trajectory file, the data for the single
 * row is loaded.
 *
 * @param matchingFiberOrientationFile
 *    New selection for matching fiber orientation file.
 */
void
CiftiFiberTrajectoryFile::setMatchingFiberOrientationFile(CiftiFiberOrientationFile* matchingFiberOrientationFile)
{
    m_matchingFiberOrientationFile = matchingFiberOrientationFile;
    if (m_matchingFiberOrientationFile != NULL) {
//        m_matchingFiberOrientationFileName = m_matchingFiberOrientationFile->getFileNameNoPath();
        m_matchingFiberOrientationFileName = m_matchingFiberOrientationFile->getFileName();
        
        switch (m_fiberTrajectoryFileType) {
            case FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE:
                break;
            case FIBER_TRAJECTORY_LOAD_SINGLE_ROW:
                loadDataForRowIndex(0);
                const CiftiXML& sparseXML = m_sparseFile->getCiftiXML();
                if (sparseXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::SCALARS)
                {
                    m_loadedDataDescriptionForMapName = sparseXML.getScalarsMap(CiftiXML::ALONG_COLUMN).getMapName(0);
                } else {
                    m_loadedDataDescriptionForMapName = "";
                }
                break;
        }
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
     * If a scene has been restored, we want to match to the fiber orientation
     * file name that was restored from the scene
     */
    if (m_matchingFiberOrientationFileNameFromRestoredScene.isEmpty() == false) {
        bool matched = false;
    
        CiftiFiberOrientationFile* matchedOrientFile = NULL;
        int64_t matchedOrientFileCount = 0;
        
        const FileInformation fileInfo(m_matchingFiberOrientationFileNameFromRestoredScene);
        const AString matchingFileNameNoPath = fileInfo.getFileName();
        
        for (std::vector<CiftiFiberOrientationFile*>::iterator iter = matchingFiberOrientationFiles.begin();
             iter != matchingFiberOrientationFiles.end();
             iter++) {
            /*
             * Try and see if it matches for this file
             * (1) Verify compatibility
             * (2) Match name of file without path
             * (3) Match path starting at end of path
             */
            CiftiFiberOrientationFile* orientationFile = *iter;
            CaretAssert(orientationFile);
            if (isFiberOrientationFileCombatible(orientationFile)) {
                if (matchingFileNameNoPath == orientationFile->getFileNameNoPath()) {
                    const AString orientationFileName = orientationFile->getFileName();
                    const int64_t endMatchCount = orientationFileName.countMatchingCharactersFromEnd(m_matchingFiberOrientationFileNameFromRestoredScene);
                    
                    if (endMatchCount > matchedOrientFileCount) {
                        matchedOrientFile = orientationFile;
                        matchedOrientFileCount = endMatchCount;
                    }
                }
            }
//            if (orientationFile->getFileNameNoPath() == m_matchingFiberOrientationFileNameFromRestoredScene) {
//                if (isFiberOrientationFileCombatible(orientationFile)) {
//                    setMatchingFiberOrientationFile(orientationFile);
//                    matched = true;
//                }
//            }
        }
        
        if (matchedOrientFileCount > 0) {
            setMatchingFiberOrientationFile(matchedOrientFile);
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
    return true;
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
 * @return The estimated size of data after it is uncompressed
 * and loaded into RAM.  A negative value indicates that the
 * file size cannot be computed.
 */
int64_t
CiftiFiberTrajectoryFile::getDataSizeUncompressedInBytes() const
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
CiftiFiberTrajectoryFile::getFileFastStatistics()
{
    return NULL;
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
CiftiFiberTrajectoryFile::getFileHistogram()
{
    return NULL;
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
CiftiFiberTrajectoryFile::getFileHistogram(const float /*mostPositiveValueInclusive*/,
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
 * Get the palette normalization modes that are supported by the file.
 *
 * @param modesSupportedOut
 *     Palette normalization modes supported by a file.  Will be
 *     empty for files that are not mapped with a palette.  If there
 *     is more than one suppported mode, the first mode in the
 *     vector is assumed to be the default mode.
 */
void
CiftiFiberTrajectoryFile::getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const
{
    modesSupportedOut.clear();
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
CiftiFiberTrajectoryFile::updateScalarColoringForMap(const int32_t /*mapIndex*/)
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
CiftiFiberTrajectoryFile::readFile(const AString& filename)
{
    clear();

    checkFileReadability(filename);
    
    try {
        m_sparseFile = new CaretSparseFile();
        m_sparseFile->readFile(filename);
        setFileName(filename);
        
        m_fiberTrajectoryFileType = FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE;
        
        const CiftiXML& xml = m_sparseFile->getCiftiXML();
        if (xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::SCALARS) {
            m_fiberTrajectoryFileType = FIBER_TRAJECTORY_LOAD_SINGLE_ROW;
        }
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
CiftiFiberTrajectoryFile::writeFile(const AString& filename)
{
    switch (m_fiberTrajectoryFileType) {
        case FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE:
            throw DataFileException(filename,
                                    "Writing of Cifti Trajectory Files that load by brainordinate is not supported.");
            break;
        case FIBER_TRAJECTORY_LOAD_SINGLE_ROW:
            writeLoadedDataToFile(filename);
            break;
    }
}

class FiberFractionAndIndex {
public:
    FiberFractionAndIndex(const FiberFractions& fiberFraction,
                          const int64_t fiberIndex) {
        m_fiberFraction = fiberFraction;
        m_fiberIndex = fiberIndex;
    }
    
    bool operator<(const FiberFractionAndIndex& other) const {
        return (m_fiberIndex < other.m_fiberIndex);
    }
    
    FiberFractions m_fiberFraction;
    int64_t m_fiberIndex;
};

class FiberTrajectoryComparison {
public:
    bool operator() (const FiberOrientationTrajectory* left,
                     const FiberOrientationTrajectory* right) const {
        return (left->getFiberOrientationIndex() < right->getFiberOrientationIndex());
    }
};

/**
 * Create a new fiber trajectory file from the loaded data of this file.
 *
 * @param errorMessageOut
 *    Error message if creation of new fiber trajectory file failed.
 * @param 
 *    Pointer to new file that was created or NULL if creation failed.
 */
CiftiFiberTrajectoryFile*
CiftiFiberTrajectoryFile::newFiberTrajectoryFileFromLoadedRowData(const AString& destinationDirectory,
                                                                  AString& errorMessageOut) const
{
    errorMessageOut = "";
    
    const int64_t numTraj = static_cast<int64_t>(m_fiberOrientationTrajectories.size());
    if (numTraj <= 0) {
        errorMessageOut = "No data is loaded so cannot create file.";
        return NULL;
    }
    
    CiftiFiberTrajectoryFile* newFile = NULL;
    try {
        newFile = new CiftiFiberTrajectoryFile();
        AString rowInfo = "";
        if (m_loadedDataDescriptionForFileCopy.isEmpty() == false) {
            rowInfo = ("_"
                       + m_loadedDataDescriptionForFileCopy);
        }
        
        
        /*
         * May need to convert a remote path to a local path
         */
        FileInformation initialFileNameInfo(getFileName());
        const AString scalarFileName = initialFileNameInfo.getAsLocalAbsoluteFilePath(destinationDirectory,
                                                                                      getDataFileType());
        
        /*
         * Create name of scalar file with row/column information
         */
        FileInformation scalarFileInfo(scalarFileName);
        AString thePath, theName, theExtension;
        scalarFileInfo.getFileComponents(thePath,
                                         theName,
                                         theExtension);
        theName.append(rowInfo);
        const AString newFileName = FileInformation::assembleFileComponents(thePath,
                                                                            theName,
                                                                            theExtension);
        
        
        
        
        
        const AString tempFileName = (QDir::tempPath()
                                      + "/"
                                      + newFile->getFileNameNoPath());
        std::cout << "Filename: " << qPrintable(tempFileName) << std::endl;
        
        writeLoadedDataToFile(tempFileName);
        
        newFile->readFile(tempFileName);
        newFile->setFileName(newFileName);
        newFile->setMatchingFiberOrientationFile(const_cast<CiftiFiberOrientationFile*>(getMatchingFiberOrientationFile()));
        newFile->m_fiberTrajectoryMapProperties->copy(*getFiberTrajectoryMapProperties());
        newFile->setModified();
        return newFile;
    }
    catch (const DataFileException& dfe) {
        if (newFile != NULL) {
            delete newFile;
        }
        errorMessageOut = dfe.whatString();
        return NULL;
    }

    return NULL;
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
CiftiFiberTrajectoryFile::writeLoadedDataToFile(const AString& filename) const
{
    CiftiXML xml = m_sparseFile->getCiftiXML();
    
    /*
     * Copy the pointers to the fiber orientation trajectories and sort
     * by fiber orientation index.
     */
    std::vector<const FiberOrientationTrajectory*> trajectories(m_fiberOrientationTrajectories.begin(),
                                                                m_fiberOrientationTrajectories.end());
    
    bool isWriteFullRow = false;
    if (static_cast<int64_t>(trajectories.size()) == xml.getDimensionLength(CiftiXML::ALONG_ROW)) {
        isWriteFullRow = true;
    }
    else {
        /*
         * Sort by fiber orientation index.
         */
        std::sort(trajectories.begin(),
                  trajectories.end(),
                  FiberTrajectoryComparison());
    }
    
    std::vector<int64_t> fiberIndices;
    std::vector<FiberFractions> fiberFractions;
    
    for (std::vector<const FiberOrientationTrajectory*>::const_iterator iter = trajectories.begin();
         iter != trajectories.end();
         iter++) {
        const FiberOrientationTrajectory* fot = *iter;
        
        std::vector<float> proportions = fot->getFiberFractions();
        if (proportions.size() < 3) {
            proportions.resize(3, 0.0);
        }
        
        const float totalCount = fot->getFiberFractionTotalCount();
        FiberFractions ff;
        ff.totalCount = totalCount; //(totalCount + 0.5);
        ff.distance = fot->getFiberFractionDistance();
        ff.fiberFractions = proportions;
        fiberIndices.push_back(fot->getFiberOrientationIndex());
        fiberFractions.push_back(ff);
//        
//        for (int64_t i = 0; i < 3; i++) {
//            if (vec[i] < -0.002f) {
//                std::cout << "Fiber " << ctr << vec[i] << std::endl;
//            }
//        }
    }
    
    /*
     * Write to temp file!!!!!
     */
    CiftiScalarsMap tempMap;
    tempMap.setLength(1);
    tempMap.setMapName(0, m_loadedDataDescriptionForMapName);
    xml.setMap(CiftiXML::ALONG_COLUMN, tempMap);
    
    CaretSparseFileWriter sparseWriter(filename,
                                       xml);
    const int64_t rowIndex = 0;
    if (isWriteFullRow) {
        sparseWriter.writeFibersRow(rowIndex,
                                    &fiberFractions[0]);
    }
    else {
        sparseWriter.writeFibersRowSparse(rowIndex,
                                          fiberIndices,
                                          fiberFractions);
    }
    
    sparseWriter.finish();
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
    m_loadedDataDescriptionForFileCopy = "";
    
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
CiftiFiberTrajectoryFile::validateAssignedMatchingFiberOrientationFile()
{
    if (m_sparseFile == NULL) {
        throw DataFileException(getFileName(),
                                "No data has been loaded.");
    }
    if (m_matchingFiberOrientationFile == NULL) {
        throw DataFileException(getFileName(),
                                "No fiber orientation file is assigned.");
    }
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiXML* orientXML = m_matchingFiberOrientationFile->getCiftiXML();
    if (*(trajXML.getMap(CiftiXML::ALONG_ROW)) != *(orientXML->getMap(CiftiXML::ALONG_COLUMN))) {
        QString msg = ("Row to Columns do not match: rows="
                       + QString::number(trajXML.getDimensionLength(CiftiXML::ALONG_COLUMN))
                       + " cols="
                       + QString::number(trajXML.getDimensionLength(CiftiXML::ALONG_ROW))
                       + "   "
                       + m_matchingFiberOrientationFile->getFileNameNoPath()
                       + " rows="
                       + QString::number(orientXML->getDimensionLength(CiftiXML::ALONG_COLUMN))
                       + " cols="
                       + QString::number(orientXML->getDimensionLength(CiftiXML::ALONG_ROW)));
        throw DataFileException(getFileName(),
                                msg);
    }
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
CiftiFiberTrajectoryFile::getBrainordinateFromRowIndex(const int64_t rowIndex,
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
 * Load data for the given surface node.
 * @param structure
 *    Structure in which surface node is located.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the surface node.
 * @return 
 *    Index of row that was loaded or -1 if no data was found for node.
 */
int64_t
CiftiFiberTrajectoryFile::loadDataForSurfaceNode(const StructureEnum::Enum structure,
                                                 const int32_t surfaceNumberOfNodes,
                                                 const int32_t nodeIndex)
{
    switch (m_fiberTrajectoryFileType) {
        case FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE:
            break;
        case FIBER_TRAJECTORY_LOAD_SINGLE_ROW:
            return -1;
            break;
    }
    
    if (m_dataLoadingEnabled == false) {
        return -1;
    }
    
    clearLoadedFiberOrientations();
    
    validateAssignedMatchingFiberOrientationFile();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& colMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (colMap.hasSurfaceData(structure) == false) {
        return -1;
    }
    if (colMap.getSurfaceNumberOfNodes(structure) != surfaceNumberOfNodes) {
        return -1;
    }
    
    const int64_t rowIndex = colMap.getIndexForNode(nodeIndex,
                                                    structure);
    if (rowIndex < 0) {
        return -1;
    }
    
    std::vector<int64_t> fiberIndices;
    std::vector<FiberFractions> fiberFractions;
    
    
    bool rowTest = false;
    if (rowTest) {
        /*
         * Test loading a full row instead of sparse.
         */
        const int numCols = trajXML.getDimensionLength(CiftiXML::ALONG_ROW);
        fiberFractions.resize(numCols);
        m_sparseFile->getFibersRow(rowIndex, &fiberFractions[0]);
        
        for (int64_t i = 0; i < numCols; i++) {
            fiberIndices.push_back(i);
        }
    }
    else {
        m_sparseFile->getFibersRowSparse(rowIndex,
                                         fiberIndices,
                                         fiberFractions);
    }
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
                FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberIndex,
                                                                                 fiberOrientation);
                fot->setFiberFractions(fiberFractions[iFiber]);
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
        m_loadedDataDescriptionForFileCopy = ("Row_"
                                              + AString::number(rowIndex));
        
        m_connectivityDataLoaded->setSurfaceNodeLoading(structure,
                                                        surfaceNumberOfNodes,
                                                        nodeIndex,
                                                        rowIndex,
                                                        -1);
    }
    else {
        m_connectivityDataLoaded->reset();
        return -1;
    }
    
    return rowIndex;
}

void
CiftiFiberTrajectoryFile::finishFiberOrientationTrajectoriesAveraging()
{
    for (std::vector<FiberOrientationTrajectory*>::iterator iter = m_fiberOrientationTrajectories.begin();
         iter != m_fiberOrientationTrajectories.end();
         iter++) {
        FiberOrientationTrajectory* fot = *iter;
        fot->finishAveraging();
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
                                                         const std::vector<int32_t>& nodeIndices)
{
    switch (m_fiberTrajectoryFileType) {
        case FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE:
            break;
        case FIBER_TRAJECTORY_LOAD_SINGLE_ROW:
            return;
            break;
    }
    
    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    clearLoadedFiberOrientations();
    
    if (surfaceNumberOfNodes <= 0) {
        return;
    }
    
    validateAssignedMatchingFiberOrientationFile();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& colMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    
    if (colMap.hasSurfaceData(structure) == false) {
        return;
    }
    if (colMap.getSurfaceNumberOfNodes(structure) != surfaceNumberOfNodes) {
        return;
    }
    
    /*
     * This map uses the index of a fiber orientation (from the Fiber Orientation File)
     * to a FiberOrientationTrajectory instance.  For averaging, items that have
     * a matching fiber orientation index are averaged.
     */
    std::map<int64_t, FiberOrientationTrajectory*> fiberOrientationIndexMapToFiberTrajectory;
    
    std::vector<int64_t> rowIndicesToLoad;
    
    const int32_t numberOfNodes = static_cast<int32_t>(nodeIndices.size());
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t nodeIndex = nodeIndices[i];
        
        /*
         * Get and load row for node
         */
        const int64_t rowIndex = colMap.getIndexForNode(nodeIndex,
                                                        structure);
        if (rowIndex >= 0) {
            rowIndicesToLoad.push_back(rowIndex);
        }
    }
    
    if (loadRowsForAveraging(rowIndicesToLoad)) {
        m_connectivityDataLoaded->setSurfaceAverageNodeLoading(structure,
                                                               surfaceNumberOfNodes,
                                                               nodeIndices);
        
        m_loadedDataDescriptionForMapName = ("Structure: "
                                             + StructureEnum::toName(structure)
                                             + ", Averaged Node Count: "
                                             + AString::number(numberOfNodes));
        m_loadedDataDescriptionForFileCopy = ("Averaged_Node_Count_"
                                              + AString::number(numberOfNodes));
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
CiftiFiberTrajectoryFile::loadRowsForAveraging(const std::vector<int64_t>& rowIndices)
{
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const int64_t numberOfColumns = trajXML.getDimensionLength(CiftiXML::ALONG_ROW);
    
    std::vector<FiberFractions> fiberFractionsForRowVector(numberOfColumns);
    FiberFractions* fiberFractionsForRow = &fiberFractionsForRowVector[0];
    
    const int64_t numberOfRowsToLoad = static_cast<int64_t>(rowIndices.size());
    if (numberOfRowsToLoad <= 0) {
        return false;
    }
    
    const int32_t progressUpdateInterval = 1;
    EventProgressUpdate progressEvent(0,
                                      numberOfRowsToLoad,
                                      0,
                                      ("Loading data for "
                                       + QString::number(numberOfRowsToLoad)
                                       + " brainordinates in file ")
                                      + getFileNameNoPath());
    
    EventManager::get()->sendEvent(progressEvent.getPointer());
    for (int64_t iCol = 0; iCol < numberOfColumns; iCol++) {
        const FiberOrientation* fiberOrientation = m_matchingFiberOrientationFile->getFiberOrientations(iCol);
        CaretAssert(fiberOrientation);
        m_fiberOrientationTrajectories.push_back(new FiberOrientationTrajectory(iCol,
                                                                                fiberOrientation));
    }
    
    bool userCancelled = false;
    
    for (int64_t iRow = 0; iRow < numberOfRowsToLoad; iRow++) {
        const int64_t rowIndex = rowIndices[iRow];
        
        if ((iRow % progressUpdateInterval) == 0) {
            progressEvent.setProgress(iRow,
                                      "");
            EventManager::get()->sendEvent(progressEvent.getPointer());
            if (progressEvent.isCancelled()) {
                userCancelled = true;
                break;
            }
        }
        
        m_sparseFile->getFibersRow(rowIndex,
                                   fiberFractionsForRow);
        
        for (int64_t iCol = 0; iCol < numberOfColumns; iCol++) {
            FiberOrientationTrajectory* fot = m_fiberOrientationTrajectories[iCol];
            fot->addFiberFractionsForAveraging(fiberFractionsForRow[iCol]);
        }
    }
    
    if (userCancelled) {
        clearLoadedFiberOrientations();
        return false;
    }
    
    finishFiberOrientationTrajectoriesAveraging();
    
    return true;
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * @param xyz
 *    Coordinate of voxel.
 * @return
 *    Index of row that was loaded or -1 if no data was found for coordinate.
 * @throw
 *    DataFileException if there is an error.
 */
int64_t
CiftiFiberTrajectoryFile::loadMapDataForVoxelAtCoordinate(const float xyz[3])
{
    m_connectivityDataLoaded->reset();
    
    switch (m_fiberTrajectoryFileType) {
        case FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE:
            break;
        case FIBER_TRAJECTORY_LOAD_SINGLE_ROW:
            return -1;
            break;
    }
    
    if (m_dataLoadingEnabled == false) {
        return -1;
    }
    
    clearLoadedFiberOrientations();
    
    validateAssignedMatchingFiberOrientationFile();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& colMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (!colMap.hasVolumeData()) return -1;
    const VolumeSpace& colSpace = colMap.getVolumeSpace();
    int64_t ijk[3];
    colSpace.enclosingVoxel(xyz, ijk);
    const int64_t rowIndex = colMap.getIndexForVoxel(ijk);
    if (rowIndex < 0) {
        return -1;
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
                FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberIndex,
                                                                                 fiberOrientation);
                fot->setFiberFractions(fiberFractions[iFiber]);
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
        m_loadedDataDescriptionForFileCopy = ("Row_"
                                              + AString::number(rowIndex));
        m_connectivityDataLoaded->setVolumeXYZLoading(xyz,
                                                      rowIndex,
                                                      -1);
    }
    else {
        return -1;
    }
    
    return rowIndex;
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
                                                            const std::vector<VoxelIJK>& voxelIndices)
{
    switch (m_fiberTrajectoryFileType) {
        case FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE:
            break;
        case FIBER_TRAJECTORY_LOAD_SINGLE_ROW:
            return;
            break;
    }

    if (m_dataLoadingEnabled == false) {
        return;
    }
    
    clearLoadedFiberOrientations();
    
    validateAssignedMatchingFiberOrientationFile();
    
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiBrainModelsMap& colMap = trajXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    
    if (colMap.hasVolumeData() == false) {
        return;
    }
    
    std::vector<int64_t> rowIndicesToLoad;
    const int32_t numberOfVoxels = static_cast<int32_t>(voxelIndices.size());
    for (int32_t i = 0; i < numberOfVoxels; i++) {
        /*
         * Get and load row for voxel
         */
        const int64_t rowIndex = colMap.getIndexForVoxel(voxelIndices[i].m_ijk);
        if (rowIndex >= 0) {
            rowIndicesToLoad.push_back(rowIndex);
        }
    }
    
    if (loadRowsForAveraging(rowIndicesToLoad)) {
        m_connectivityDataLoaded->setVolumeAverageVoxelLoading(volumeDimensionIJK,
                                                               voxelIndices);
        
        m_loadedDataDescriptionForMapName = ("Averaged Voxel Count: "
                                             + AString::number(numberOfVoxels));
        m_loadedDataDescriptionForFileCopy = ("Average_Voxel_Count_"
                                              + AString::number(numberOfVoxels));
    }
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
CiftiFiberTrajectoryFile::loadDataForRowIndex(const int64_t rowIndex)
{
    clearLoadedFiberOrientations();
    
    validateAssignedMatchingFiberOrientationFile();
    
    std::vector<int64_t> fiberIndices;
    std::vector<FiberFractions> fiberFractions;
    m_sparseFile->getFibersRowSparse(rowIndex,
                                     fiberIndices,
                                     fiberFractions);
    CaretAssert(fiberIndices.size() == fiberFractions.size());
    
    const int64_t numFibers = static_cast<int64_t>(fiberIndices.size());
    
    if (numFibers > 0) {
        m_fiberOrientationTrajectories.reserve(numFibers);
        
        for (int64_t iFiber = 0; iFiber < numFibers; iFiber++) {
            const int64_t numFiberOrientations = m_matchingFiberOrientationFile->getNumberOfFiberOrientations();
            const int64_t fiberIndex = fiberIndices[iFiber];
            if (fiberIndex < numFiberOrientations) {
                const FiberOrientation* fiberOrientation = m_matchingFiberOrientationFile->getFiberOrientations(fiberIndex);
                FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberIndex,
                                                                                 fiberOrientation);
                fot->setFiberFractions(fiberFractions[iFiber]);
                m_fiberOrientationTrajectories.push_back(fot);
            }
            else{
                CaretLogSevere("Invalid index="
                               + QString::number(fiberIndex)
                               + " into fiber orientations");
            }
        }
        
        m_loadedDataDescriptionForMapName = ("Row: "
                                             + AString::number(rowIndex));
        m_loadedDataDescriptionForFileCopy = ("Row_"
                                              + AString::number(rowIndex));
        
        m_connectivityDataLoaded->setRowColumnLoading(rowIndex,
                                                      -1);
    }
    else {
        throw DataFileException(getFileName(),
                                "Row "
                                + AString::number(rowIndex)
                                + " is invalid or contains no data.");
    }
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
CiftiFiberTrajectoryFile::finishRestorationOfScene()
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
                               "Fiber Trajectory never loads by column.");
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
            int64_t rowIndex;
            int64_t columnIndex;
            m_connectivityDataLoaded->getVolumeXYZLoading(volumeXYZ,
                                                          rowIndex,
                                                          columnIndex);
            loadMapDataForVoxelAtCoordinate(volumeXYZ);
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
        {
            int64_t volumeDimensionsIJK[3];
            std::vector<VoxelIJK> voxelIndicesIJK;
            m_connectivityDataLoaded->getVolumeAverageVoxelLoading(volumeDimensionsIJK,
                                                                   voxelIndicesIJK);
            loadMapAverageDataForVoxelIndices(volumeDimensionsIJK,
                                              voxelIndicesIJK);
        }
            break;
    }
    
    setDataLoadingEnabled(loadingEnabledStatus);
}

/**
 * @return a REFERENCE to the fiber fractions that were loaded.
 */
const std::vector<FiberOrientationTrajectory*>*
CiftiFiberTrajectoryFile::getLoadedFiberOrientationTrajectories() const
{
    return &m_fiberOrientationTrajectories;
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

    m_matchingFiberOrientationFile = NULL;
    m_matchingFiberOrientationFileName = "";
    
    CaretMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    m_matchingFiberOrientationFileNameFromRestoredScene = m_matchingFiberOrientationFileName;
}

/**
 * @return True if this file type supports writing, else false.
 *
 * Fiber trajectory files do NOT support writing.
 */
bool
CiftiFiberTrajectoryFile::supportsWriting() const
{
    switch (m_fiberTrajectoryFileType) {
        case FIBER_TRAJECTORY_LOAD_BY_BRAINORDINATE:
            break;
        case FIBER_TRAJECTORY_LOAD_SINGLE_ROW:
            return true;
            break;
    }
    
    return false;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CiftiFiberTrajectoryFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
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

bool CiftiFiberTrajectoryFile::hasCiftiXML() const
{
    return true;
}

const CiftiXML CiftiFiberTrajectoryFile::getCiftiXML() const
{
    if (m_sparseFile != NULL)
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
CiftiFiberTrajectoryFile::getDataForSelector(const MapFileDataSelector& /*mapFileDataSelector*/,
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
CiftiFiberTrajectoryFile::getBrainordinateMappingMatchImplementation(const CaretMappableDataFile* /*mapFile*/) const
{
    return BrainordinateMappingMatch::NO;
}


