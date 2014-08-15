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

#include <algorithm>
#include <cmath>
#include <limits>

#include "CaretAssert.h"

#include "Border.h"
#include "BorderFile.h"
#include "BorderPointFromSearch.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartingDataManager.h"
#include "ChartableBrainordinateInterface.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixDenseFile.h"
#include "CiftiConnectivityMatrixDenseParcelFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiConnectivityMatrixParcelDenseFile.h"
#include "CiftiParcelSeriesFile.h"
#include "CiftiParcelScalarFile.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFiberOrientation.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesSurface.h"
#include "DisplayPropertiesVolume.h"
#include "ElapsedTimer.h"
#include "EventBrowserTabGetAll.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventDataFileAdd.h"
#include "EventDataFileDelete.h"
#include "EventDataFileRead.h"
#include "EventDataFileReload.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventModelGetAll.h"
#include "EventPaletteGetByName.h"
#include "EventProgressUpdate.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventManager.h"
#include "FiberOrientationSamplesLoader.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentificationManager.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "ModelChart.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "LabelFile.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "PaletteFile.h"
#include "RgbaFile.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneFile.h"
#include "SelectionManager.h"
#include "SessionManager.h"
#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "Surface.h"
#include "SurfaceProjectedItem.h"
#include "SystemUtilities.h"
#include "VolumeFile.h"
#include "VolumeSurfaceOutlineSetModel.h"



using namespace caret;

/**
 *  Constructor.
 */
Brain::Brain()
{
    m_chartingDataManager = new ChartingDataManager(this);
    m_fiberOrientationSamplesLoader = new FiberOrientationSamplesLoader();
    
    m_paletteFile = new PaletteFile();
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileName()));
    m_paletteFile->clearModified();
    
    m_specFile = new SpecFile();
    m_specFile->setFileName("");
    m_specFile->clearModified();
    
    m_modelChart = NULL;
    m_surfaceMontageModel = NULL;
    m_volumeSliceModel = NULL;
    m_wholeBrainModel = NULL;
    
    m_displayPropertiesBorders = new DisplayPropertiesBorders();
    m_displayProperties.push_back(m_displayPropertiesBorders);
    
    m_displayPropertiesFiberOrientation = new DisplayPropertiesFiberOrientation();
    m_displayProperties.push_back(m_displayPropertiesFiberOrientation);
    
    m_displayPropertiesFoci = new DisplayPropertiesFoci();
    m_displayProperties.push_back(m_displayPropertiesFoci);
    
    m_displayPropertiesLabels = new DisplayPropertiesLabels();
    m_displayProperties.push_back(m_displayPropertiesLabels);
    
    m_displayPropertiesSurface = new DisplayPropertiesSurface();
    m_displayProperties.push_back(m_displayPropertiesSurface);
    
    m_displayPropertiesVolume = new DisplayPropertiesVolume();
    m_displayProperties.push_back(m_displayPropertiesVolume);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DATA_FILE_ADD);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DATA_FILE_DELETE);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DATA_FILE_RELOAD);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_GET_DISPLAYED_DATA_FILES);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_PALETTE_GET_BY_NAME);
    
    m_isSpecFileBeingRead = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("displayPropertiesBorders", 
                          "DisplayPropertiesBorders", 
                          m_displayPropertiesBorders);
    
    m_sceneAssistant->add("displayPropertiesFiberOrientation",
                          "DisplayPropertiesFiberOrientation",
                          m_displayPropertiesFiberOrientation);
    
    m_sceneAssistant->add("displayPropertiesFoci",
                          "DisplayPropertiesFoci", 
                          m_displayPropertiesFoci);
    
    m_sceneAssistant->add("m_displayPropertiesLabels",
                          "DisplayPropertiesLabels",
                          m_displayPropertiesLabels);
    
    m_sceneAssistant->add("m_displayPropertiesSurface",
                          "DisplayPropertiesSurface",
                          m_displayPropertiesSurface);
    
    m_sceneAssistant->add("displayPropertiesVolume",
                          "DisplayPropertiesVolume", 
                          m_displayPropertiesVolume);
    
    m_selectionManager = new SelectionManager();

    m_identificationManager = new IdentificationManager();
    
    updateChartModel();
}

/**
 * Destructor.
 */
Brain::~Brain()
{
    EventManager::get()->removeAllEventsFromListener(this);

    delete m_sceneAssistant;
    
    for (std::vector<DisplayProperties*>::iterator iter = m_displayProperties.begin();
         iter != m_displayProperties.end();
         iter++) {
        delete *iter;
    }
    m_displayProperties.clear();
    
    resetBrain();

    delete m_specFile;
    delete m_chartingDataManager;
    delete m_fiberOrientationSamplesLoader;
    delete m_paletteFile;
    if (m_modelChart != NULL) {
        delete m_modelChart;
    }
    if (m_surfaceMontageModel != NULL) {
        delete m_surfaceMontageModel;
    }
    if (m_volumeSliceModel != NULL) {
        delete m_volumeSliceModel;
    }
    if (m_wholeBrainModel != NULL) {
        delete m_wholeBrainModel;
    }

    delete m_selectionManager;
    delete m_identificationManager;
}

/**
 * Get number of brain structures.
 *
 * @return
 *    Number of brain structure.
 */
int 
Brain::getNumberOfBrainStructures() const
{
    return static_cast<int>(m_brainStructures.size());
}

/**
 * Add a brain structure.
 * 
 * @param brainStructure
 *    Brain structure to add.
 */
void 
Brain::addBrainStructure(BrainStructure* brainStructure)
{
    m_brainStructures.push_back(brainStructure);
}

/**
 * Get a brain structure at specified index.
 *
 * @param indx
 *    Index of brain structure.
 * @return
 *    Pointer to brain structure at index.
 */
BrainStructure* 
Brain::getBrainStructure(const int32_t indx)
{
    CaretAssertVectorIndex(m_brainStructures, indx);
    return m_brainStructures[indx];
    
}

/**
 * Get a brain structure at specified index.
 *
 * @param indx
 *    Index of brain structure.
 * @return
 *    Pointer to brain structure at index.
 */
const BrainStructure*
Brain::getBrainStructure(const int32_t indx) const
{
    CaretAssertVectorIndex(m_brainStructures, indx);
    return m_brainStructures[indx];
    
}

/**
 * Find, and possibly create, a brain structure that 
 * models the specified structure.
 *
 * @param structure
 *    The desired structure.
 * @param createIfNotFound
 *    If there is not a matching brain structure, create one.
 * @return
 *    Pointer to brain structure or NULL if no match.
 */
BrainStructure* 
Brain::getBrainStructure(StructureEnum::Enum structure,
                         bool createIfNotFound)
{
    for (std::vector<BrainStructure*>::iterator iter = m_brainStructures.begin();
         iter != m_brainStructures.end();
         iter++) {
        BrainStructure* bs = *iter;
        if (bs->getStructure() == structure) {
            return bs;
        }
    }
    
    if (createIfNotFound) {
        BrainStructure* bs = new BrainStructure(this, structure);
        m_brainStructures.push_back(bs);
        return bs;
    }
    
    return NULL;
}

/**
 * Reset the brain structure.
 * @param keepSceneFiles
 *    Status of keeping scene files.
 * @param keepSpecFile
 *    Status of keeping spec file.
 */
void 
Brain::resetBrain(const ResetBrainKeepSceneFiles keepSceneFiles,
                  const ResetBrainKeepSpecFile keepSpecFile)
{    m_isSpecFileBeingRead = false;
    
    int num = getNumberOfBrainStructures();
    for (int32_t i = 0; i < num; i++) {
        delete m_brainStructures[i];
    }
    
    for (std::vector<VolumeFile*>::iterator vfi = m_volumeFiles.begin();
         vfi != m_volumeFiles.end();
         vfi++) {
        VolumeFile* vf = *vfi;
        delete vf;
    }
    m_volumeFiles.clear();
    
    m_brainStructures.clear();
    
    for (std::vector<BorderFile*>::iterator bfi = m_borderFiles.begin();
         bfi != m_borderFiles.end();
         bfi++) {
        BorderFile* bf = *bfi;
        delete bf;
    }
    m_borderFiles.clear();
    
    for (std::vector<FociFile*>::iterator ffi = m_fociFiles.begin();
         ffi != m_fociFiles.end();
         ffi++) {
        FociFile* ff = *ffi;
        delete ff;
    }
    m_fociFiles.clear();
    
    for (std::vector<CiftiBrainordinateDataSeriesFile*>::iterator cdsfi = m_connectivityDataSeriesFiles.begin();
         cdsfi != m_connectivityDataSeriesFiles.end();
         cdsfi++) {
        CiftiBrainordinateDataSeriesFile* cbdsf = *cdsfi;
        delete cbdsf;
    }
    m_connectivityDataSeriesFiles.clear();
    
    for (std::vector<CiftiBrainordinateLabelFile*>::iterator clfi = m_connectivityDenseLabelFiles.begin();
         clfi != m_connectivityDenseLabelFiles.end();
         clfi++) {
        CiftiBrainordinateLabelFile* clf = *clfi;
        delete clf;
    }
    m_connectivityDenseLabelFiles.clear();
    
    for (std::vector<CiftiConnectivityMatrixDenseFile*>::iterator clfi = m_connectivityMatrixDenseFiles.begin();
         clfi != m_connectivityMatrixDenseFiles.end();
         clfi++) {
        CiftiConnectivityMatrixDenseFile* clf = *clfi;
        delete clf;
    }
    m_connectivityMatrixDenseFiles.clear();
    
    for (std::vector<CiftiConnectivityMatrixDenseParcelFile*>::iterator clfi = m_connectivityMatrixDenseParcelFiles.begin();
         clfi != m_connectivityMatrixDenseParcelFiles.end();
         clfi++) {
        CiftiConnectivityMatrixDenseParcelFile* clf = *clfi;
        delete clf;
    }
    m_connectivityMatrixDenseParcelFiles.clear();
    
    for (std::vector<CiftiBrainordinateScalarFile*>::iterator clfi = m_connectivityDenseScalarFiles.begin();
         clfi != m_connectivityDenseScalarFiles.end();
         clfi++) {
        CiftiBrainordinateScalarFile* clf = *clfi;
        delete clf;
    }
    m_connectivityDenseScalarFiles.clear();
    
    for (std::vector<CiftiParcelSeriesFile*>::iterator clfi = m_connectivityParcelSeriesFiles.begin();
         clfi != m_connectivityParcelSeriesFiles.end();
         clfi++) {
        CiftiParcelSeriesFile* pdsf = *clfi;
        delete pdsf;
    }
    m_connectivityParcelSeriesFiles.clear();
    
    for (std::vector<CiftiParcelScalarFile*>::iterator clfi = m_connectivityParcelScalarFiles.begin();
         clfi != m_connectivityParcelScalarFiles.end();
         clfi++) {
        CiftiParcelScalarFile* psf = *clfi;
        delete psf;
    }
    m_connectivityParcelScalarFiles.clear();
    
    for (std::vector<CiftiFiberOrientationFile*>::iterator clfi = m_connectivityFiberOrientationFiles.begin();
         clfi != m_connectivityFiberOrientationFiles.end();
         clfi++) {
        CiftiFiberOrientationFile* clf = *clfi;
        delete clf;
    }
    m_connectivityFiberOrientationFiles.clear();
    
    for (std::vector<CiftiFiberTrajectoryFile*>::iterator clfi = m_connectivityFiberTrajectoryFiles.begin();
         clfi != m_connectivityFiberTrajectoryFiles.end();
         clfi++) {
        CiftiFiberTrajectoryFile* clf = *clfi;
        delete clf;
    }
    m_connectivityFiberTrajectoryFiles.clear();
    

    for (std::vector<CiftiConnectivityMatrixParcelFile*>::iterator clfi = m_connectivityMatrixParcelFiles.begin();
         clfi != m_connectivityMatrixParcelFiles.end();
         clfi++) {
        CiftiConnectivityMatrixParcelFile* clf = *clfi;
        delete clf;
    }
    m_connectivityMatrixParcelFiles.clear();
    
    for (std::vector<CiftiConnectivityMatrixParcelDenseFile*>::iterator clfi = m_connectivityMatrixParcelDenseFiles.begin();
         clfi != m_connectivityMatrixParcelDenseFiles.end();
         clfi++) {
        CiftiConnectivityMatrixParcelDenseFile* clf = *clfi;
        delete clf;
    }
    m_connectivityMatrixParcelDenseFiles.clear();
    
    
    if (m_paletteFile != NULL) {
        delete m_paletteFile;
    }
    m_paletteFile = new PaletteFile();
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileName()));
    m_paletteFile->clearModified();
    
    m_fiberOrientationSamplesLoader->reset();
    
    switch (keepSceneFiles) {
        case RESET_BRAIN_KEEP_SCENE_FILES_NO:
            for (std::vector<SceneFile*>::iterator sfi = m_sceneFiles.begin();
                 sfi != m_sceneFiles.end();
                 sfi++) {
                SceneFile* sf = *sfi;
                delete sf;
            }
            m_sceneFiles.clear();
            break;
        case RESET_BRAIN_KEEP_SCENE_FILES_YES:
            break;
    }
    
    switch (keepSpecFile) {
        case RESET_BRAIN_KEEP_SPEC_FILE_NO:
            m_specFile->clear();
            m_specFile->setFileName("");
            m_specFile->clearModified();
            break;
        case RESET_BRAIN_KEEP_SPEC_FILE_YES:
            break;
    }
    
    for (std::vector<DisplayProperties*>::iterator iter = m_displayProperties.begin();
         iter != m_displayProperties.end();
         iter++) {
        (*iter)->reset();
    }
    
    m_identificationManager->removeAllIdentifiedItems();
    m_selectionManager->reset();
    m_selectionManager->setLastSelectedItem(NULL);
    
    if (m_modelChart != NULL) {
        m_modelChart->reset();
    }
    
    updateAfterFilesAddedOrRemoved();
}


/**
 * Reset the brain structure.
 */
void 
Brain::resetBrain()
{
    resetBrain(RESET_BRAIN_KEEP_SCENE_FILES_NO,
                     RESET_BRAIN_KEEP_SPEC_FILE_NO);
}

/**
 * Reset the brain structure but keep spec and scene files.
 */
void 
Brain::resetBrainKeepSceneFiles()
{
    /*
     * Save all of the non-modified files so that loading
     * of them can be avoided later
     */
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    m_nonModifiedFilesForRestoringScene.clear();
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        CaretDataFile* caretDataFile = *iter;
        if (caretDataFile->isModified()) {
            continue;
        }
        
        const DataFileTypeEnum::Enum dataFileType = caretDataFile->getDataFileType();
        
        bool keepFileFlag = true;
        switch (dataFileType) {
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::PALETTE:
                keepFileFlag = false;
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SCENE:
                keepFileFlag = false;
                break;
            case DataFileTypeEnum::SPECIFICATION:
                keepFileFlag = false;
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                keepFileFlag = false;
                break;
            case DataFileTypeEnum::VOLUME:
                break;
        }
        
        if (keepFileFlag) {
            if (removeWithoutDeleteDataFile(caretDataFile)) {
                m_nonModifiedFilesForRestoringScene.push_back(caretDataFile);
            }
        }
    }
    
    
    resetBrain(RESET_BRAIN_KEEP_SCENE_FILES_YES,
                     RESET_BRAIN_KEEP_SPEC_FILE_NO);
}

/**
 * Copy all display properties from the source tab to the target tab.
 * @param sourceTabIndex
 *    Index of source tab.
 * @param targetTabIndex
 *    Index of target tab.
 */
void 
Brain::copyDisplayProperties(const int32_t sourceTabIndex,
                             const int32_t targetTabIndex)
{
    for (std::vector<DisplayProperties*>::iterator iter = m_displayProperties.begin();
         iter != m_displayProperties.end();
         iter++) {
        (*iter)->copyDisplayProperties(sourceTabIndex,
                                       targetTabIndex);
    }
}

/**
 * Read a surface file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
Surface* 
Brain::addReadOrReloadSurfaceFile(const FileModeAddReadReload fileMode,
                       CaretDataFile* caretDataFile,
                       const AString& filename,
                       const StructureEnum::Enum structureIn,
                       const bool markDataFileAsModified) throw (DataFileException)
{
    Surface* surface = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (caretDataFile != NULL) {
        surface = dynamic_cast<Surface*>(caretDataFile);
        CaretAssert(surface);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = surface->getStructure();
    }
    else {
        surface = new Surface();
    }
    
    bool addFlag    = false;
    bool readFlag   = false;
    bool reloadFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag   = true;
            reloadFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            surface->readFile(filename);
        }
        catch (const DataFileException& dfe) {
            if (reloadFlag) {
                BrainStructure* bs = getBrainStructure(structure, true);
                if (bs != NULL) {
                    bs->removeWithoutDeleteDataFile(surface);
                }
            }
            
            delete surface;
            throw dfe;
        }
    }
    
    if (addFlag) {
        
    }
    
    
    bool fileIsModified = false;
    if (structureIn != StructureEnum::INVALID) {
        if (surface->getStructure() != structureIn) {
            surface->setStructure(structureIn);
            fileIsModified = markDataFileAsModified;
        }
    }
    
    structure = surface->getStructure();
    if (structure == StructureEnum::INVALID) {
        if (caretDataFile == NULL) {
            delete surface;
        }
        AString message = "Structure in "
        + filename
        + " is not valid.";
        DataFileException e(message);
        e.setErrorInvalidStructure(true);
        CaretLogThrowing(e);
        throw e;
    }

    const bool brainStructureExists = (getBrainStructure(structure, false) != NULL);
    BrainStructure* bs = getBrainStructure(structure, true);
    if (bs != NULL) {
        /*
         * Initialize the overlays if the brain structure did NOT exist 
         * AND a spec file is NOT being read
         */
        bool initializeOverlaysFlag = false;
        if (brainStructureExists == false) {
            if (m_isSpecFileBeingRead == false) {
                initializeOverlaysFlag = true;
            }
        }
        bs->addSurface(surface,
                       addFlag,
                       initializeOverlaysFlag);
    }
    else {
        if (caretDataFile == NULL) {
            delete surface;
        }
        AString message = "Failed to create a BrainStructure for surface with structure "
        + StructureEnum::toGuiName(structure)
        + ".";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    surface->clearModified();
    if (fileIsModified) {
        surface->setModified();
    }
    
    return surface;
}

/**
 * Read a label file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
LabelFile* 
Brain::addReadOrReloadLabelFile(const FileModeAddReadReload fileMode,
                     CaretDataFile* caretDataFile,
                     const AString& filename,
                     const StructureEnum::Enum structureIn,
                     const bool markDataFileAsModified) throw (DataFileException)
{
    LabelFile* labelFile = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (caretDataFile != NULL) {
        labelFile = dynamic_cast<LabelFile*>(caretDataFile);
        CaretAssert(labelFile);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = labelFile->getStructure();
    }
    else {
        labelFile = new LabelFile();
    }

    bool addFlag    = false;
    bool readFlag   = false;
    bool reloadFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag   = true;
            reloadFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            labelFile->readFile(filename);
        }
        catch (const DataFileException& dfe) {
            if (reloadFlag) {
                BrainStructure* bs = getBrainStructure(structure, true);
                if (bs != NULL) {
                    bs->removeWithoutDeleteDataFile(labelFile);
                }
            }

            delete labelFile;
            throw dfe;
        }
    }
    
    if (addFlag) {
        
    }
    
    
    bool fileIsModified = false;
    if (structureIn != StructureEnum::INVALID) {
        if (labelFile->getStructure() != structureIn) {
            labelFile->setStructure(structureIn);
            fileIsModified = markDataFileAsModified;
        }
    }
    
    structure = labelFile->getStructure();
    if (structure == StructureEnum::INVALID) {
        if (caretDataFile == NULL) {
            delete labelFile;
        }
        AString message = "Structure in "
        + filename
        + " is not valid.";
        DataFileException e(message);
        e.setErrorInvalidStructure(true);
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = getBrainStructure(structure, false);
    if (bs != NULL) {
        try {
            bs->addLabelFile(labelFile,
                             addFlag);
        }
        catch (const DataFileException& e) {
            if (caretDataFile == NULL) {
                delete labelFile;
            }
            throw e;
        }
    }
    else {
        if (caretDataFile == NULL) {
            delete labelFile;
        }
        AString message = "Must read a surface with structure "
        + StructureEnum::toGuiName(structure)
        + " before reading its label files.";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    labelFile->clearModified();
    if (fileIsModified) {
        labelFile->setModified();
    }
    
    return labelFile;
}

/**
 * Read a metric file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
MetricFile* 
Brain::addReadOrReloadMetricFile(const FileModeAddReadReload fileMode,
                      CaretDataFile* caretDataFile,
                      const AString& filename,
                      const StructureEnum::Enum structureIn,
                      const bool markDataFileAsModified) throw (DataFileException)
{
    MetricFile* metricFile = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (caretDataFile != NULL) {
        metricFile = dynamic_cast<MetricFile*>(caretDataFile);
        CaretAssert(metricFile);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = metricFile->getStructure();
    }
    else {
        metricFile = new MetricFile();
    }

    bool addFlag    = false;
    bool readFlag   = false;
    bool reloadFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag   = true;
            reloadFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            metricFile->readFile(filename);
        }
        catch (const DataFileException& dfe) {
            if (reloadFlag) {
                BrainStructure* bs = getBrainStructure(structure, true);
                if (bs != NULL) {
                    bs->removeWithoutDeleteDataFile(metricFile);
                }
            }

            delete metricFile;
            throw dfe;
        }
    }
    
    if (addFlag) {
        
    }
    
    
    bool fileIsModified = false;
    if (structureIn != StructureEnum::INVALID) {
        if (metricFile->getStructure() != structureIn) {
            metricFile->setStructure(structureIn);
            fileIsModified = markDataFileAsModified;
        }
    }
    
    structure = metricFile->getStructure();
    if (structure == StructureEnum::INVALID) {
        if (caretDataFile == NULL) {
            delete metricFile;
        }
        AString message = "Structure in "
        + filename
        + " is not valid.";
        DataFileException e(message);
        e.setErrorInvalidStructure(true);
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = getBrainStructure(structure, false);
    if (bs != NULL) {
        try {
            bs->addMetricFile(metricFile,
                              addFlag);
        }
        catch (const DataFileException& e) {
            if (caretDataFile == NULL) {
                delete metricFile;
            }
            throw e;
        }
    }
    else {
        if (caretDataFile == NULL) {
            delete metricFile;
        }
        AString message = "Must read a surface with structure "
        + StructureEnum::toGuiName(structure)
        + " before reading its metric files.";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    metricFile->clearModified();
    if (fileIsModified) {
        metricFile->setModified();
    }
    
    return metricFile;
}

/**
 * Read an RGBA file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
RgbaFile* 
Brain::addReadOrReloadRgbaFile(const FileModeAddReadReload fileMode,
                    CaretDataFile* caretDataFile,
                    const AString& filename,
                    const StructureEnum::Enum structureIn,
                    const bool markDataFileAsModified) throw (DataFileException)
{
    RgbaFile* rgbaFile = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (caretDataFile != NULL) {
        rgbaFile = dynamic_cast<RgbaFile*>(caretDataFile);
        CaretAssert(rgbaFile);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = rgbaFile->getStructure();
    }
    else {
        rgbaFile = new RgbaFile();
    }

    bool addFlag    = false;
    bool readFlag   = false;
    bool reloadFlag = false;
    
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag  = true;
            reloadFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            rgbaFile->readFile(filename);
        }
        catch (const DataFileException& dfe) {
            if (reloadFlag) {
                BrainStructure* bs = getBrainStructure(structure, true);
                if (bs != NULL) {
                    bs->removeWithoutDeleteDataFile(rgbaFile);
                }
            }
            
            delete rgbaFile;
            throw dfe;
        }
    }
    
    if (addFlag) {
        
    }
    
    
    bool fileIsModified = false;
    if (structureIn != StructureEnum::INVALID) {
        if (rgbaFile->getStructure() != structureIn) {
            rgbaFile->setStructure(structureIn);
            fileIsModified = markDataFileAsModified;
        }
    }
    
    structure = rgbaFile->getStructure();
    if (structure == StructureEnum::INVALID) {
        if (caretDataFile == NULL) {
            delete rgbaFile;
        }
        AString message = "Structure in "
                        + filename
                        + " is not valid.";
        DataFileException e(message);
        e.setErrorInvalidStructure(true);
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = getBrainStructure(structure, false);
    if (bs != NULL) {
        try {
            bs->addRgbaFile(rgbaFile,
                            addFlag);
        }
        catch (const DataFileException& e) {
            if (caretDataFile == NULL) {
                delete rgbaFile;
            }
            throw e;
        }
    }
    else {
        if (caretDataFile == NULL) {
            delete rgbaFile;
        }
        AString message = "Must read a surface with structure "
        + StructureEnum::toGuiName(structure)
        + " before reading its RGBA files.";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    rgbaFile->clearModified();
    if (fileIsModified) {
        rgbaFile->setModified();
    }
    
    return rgbaFile;
}

/**
 * Read a volume file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
VolumeFile*
Brain::addReadOrReloadVolumeFile(const FileModeAddReadReload fileMode,
                      CaretDataFile* caretDataFile,
                      const AString& filename) throw (DataFileException)
{
    VolumeFile* vf = NULL;
    if (caretDataFile != NULL) {
        vf = dynamic_cast<VolumeFile*>(caretDataFile);
        CaretAssert(vf);
    }
    else {
        vf = new VolumeFile();
    }

    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            vf->readFile(filename);
        }
        catch (const DataFileException& e) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete vf;
            }
            throw e;
        }
    }
    
    vf->clearModified();
    
    ElapsedTimer timer;
    timer.start();
    vf->updateScalarColoringForAllMaps(m_paletteFile);
    CaretLogInfo("Time to color volume data is "
                 + AString::number(timer.getElapsedTimeSeconds(), 'f', 3)
                 + " seconds.");
    
    if (addFlag) {
        m_volumeFiles.push_back(vf);
    }
    
    return vf;
}

/**
 * @return  Number of volume files.
 */
int32_t 
Brain::getNumberOfVolumeFiles() const
{
    return m_volumeFiles.size();
}

/**
 * Get the volume file at the given index.
 * @param volumeFileIndex
 *    Index of the volume file.
 * @return
 *    Volume file at the given index.
 */
VolumeFile* 
Brain::getVolumeFile(const int32_t volumeFileIndex)
{
    CaretAssertVectorIndex(m_volumeFiles, volumeFileIndex);
    return m_volumeFiles[volumeFileIndex];
}

/**
 * Get the volume file at the given index.
 * @param volumeFileIndex
 *    Index of the volume file.
 * @return
 *    Volume file at the given index.
 */
const VolumeFile* 
Brain::getVolumeFile(const int32_t volumeFileIndex) const
{
    CaretAssertVectorIndex(m_volumeFiles, volumeFileIndex);
    return m_volumeFiles[volumeFileIndex];
}

/**
 * Read a border file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
BorderFile*
Brain::addReadOrReloadBorderFile(const FileModeAddReadReload fileMode,
                      CaretDataFile* caretDataFile,
                      const AString& filename) throw (DataFileException)
{
    BorderFile* bf = NULL;
    if (caretDataFile != NULL) {
        bf = dynamic_cast<BorderFile*>(caretDataFile);
        CaretAssert(bf);
    }
    else {
        bf = new BorderFile();
    }

    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            bf->readFile(filename);
        }
        catch (DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete bf;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_borderFiles.push_back(bf);
    }
    
    
    return bf;
}

/**
 * Read a foci file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
FociFile*
Brain::addReadOrReloadFociFile(const FileModeAddReadReload fileMode,
                    CaretDataFile* caretDataFile,
                    const AString& filename) throw (DataFileException)
{
    FociFile* ff = NULL;
    if (caretDataFile != NULL) {
        ff = dynamic_cast<FociFile*>(caretDataFile);
        CaretAssert(ff);
    }
    else {
        ff = new FociFile();
    }

    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            ff->readFile(filename);
        }
        catch (DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete ff;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_fociFiles.push_back(ff);
    }
    
    
    return ff;
}

/**
 * Validate a CIFTI Mappable Data File.
 * A file is valid if its surface mappings match the loaded surfaces.
 *
 * @param ciftiMapFile
 *    File examined for validity.
 * @throws DataFileException
 *    If the file is found to be incompatible with the loaded surfaces.
 */
void
Brain::validateCiftiMappableDataFile(const CiftiMappableDataFile* ciftiMapFile) const throw (DataFileException)
{
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        const StructureEnum::Enum structure = getBrainStructure(i)->getStructure();
        const int numNodes = getBrainStructure(i)->getNumberOfNodes();
        
        const int numConnNodes = ciftiMapFile->getMappingSurfaceNumberOfNodes(structure);
        if (numConnNodes > 0) {
            if (numNodes != numConnNodes) {
                AString msg = ("The CIFTI file "
                               + ciftiMapFile->getFileNameNoPath()
                               + " contains "
                               + AString::number(numConnNodes)
                               + " nodes for structure "
                               + StructureEnum::toGuiName(structure)
                               + " but the corresponding surface brain structure contains "
                               + AString::number(numNodes)
                               + " nodes.");
                throw DataFileException(msg);
            }
        }
    }
}


/**
 * Read a connectivity matrix dense file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixDenseFile*
Brain::addReadOrReloadConnectivityDenseFile(const FileModeAddReadReload fileMode,
                                 CaretDataFile* caretDataFile,
                                 const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixDenseFile* cmdf = NULL;
    if (caretDataFile != NULL) {
        cmdf = dynamic_cast<CiftiConnectivityMatrixDenseFile*>(caretDataFile);
        CaretAssert(cmdf);
    }
    else {
        cmdf = new CiftiConnectivityMatrixDenseFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            cmdf->readFile(filename);
            cmdf->clearModified();
            validateCiftiMappableDataFile(cmdf);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete cmdf;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityMatrixDenseFiles.push_back(cmdf);
    }
    
    return cmdf;
}

/**
 * Read a connectivity dense label file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiBrainordinateLabelFile*
Brain::addReadOrReloadConnectivityDenseLabelFile(const FileModeAddReadReload fileMode,
                                      CaretDataFile* caretDataFile,
                                      const AString& filename) throw (DataFileException)
{
    CiftiBrainordinateLabelFile* file = NULL;
    if (caretDataFile != NULL) {
        file = dynamic_cast<CiftiBrainordinateLabelFile*>(caretDataFile);
        CaretAssert(file);
    }
    else {
        file = new CiftiBrainordinateLabelFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            file->readFile(filename);
            validateCiftiMappableDataFile(file);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete file;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityDenseLabelFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a connectivity dense parcel file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixDenseParcelFile*
Brain::addReadOrReloadConnectivityMatrixDenseParcelFile(const FileModeAddReadReload fileMode,
                                             CaretDataFile* caretDataFile,
                                             const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixDenseParcelFile* file = NULL;
    if (caretDataFile != NULL) {
        file = dynamic_cast<CiftiConnectivityMatrixDenseParcelFile*>(caretDataFile);
        CaretAssert(file);
    }
    else {
        file = new CiftiConnectivityMatrixDenseParcelFile();
    }

    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            file->readFile(filename);
            validateCiftiMappableDataFile(file);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete file;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityMatrixDenseParcelFiles.push_back(file);
    }
    
    return file;
}

/**
 * Update the fiber orientation files assigned to matching
 * fiber trajectory files.  This is typically called after
 * files are added or removed.
 */
void
Brain::updateFiberTrajectoryMatchingFiberOrientationFiles()
{
    for (std::vector<CiftiFiberTrajectoryFile*>::iterator iter = m_connectivityFiberTrajectoryFiles.begin();
         iter != m_connectivityFiberTrajectoryFiles.end();
         iter++) {
        CiftiFiberTrajectoryFile* trajFile = *iter;
        trajFile->updateMatchingFiberOrientationFileFromList(m_connectivityFiberOrientationFiles);
    }
}
/**
 * Read a connectivity dense scalar file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiBrainordinateScalarFile*
Brain::addReadOrReloadConnectivityDenseScalarFile(const FileModeAddReadReload fileMode,
                                       CaretDataFile* caretDataFile,
                                       const AString& filename) throw (DataFileException)
{
    CiftiBrainordinateScalarFile* clf = NULL;
    if (caretDataFile != NULL) {
        clf = dynamic_cast<CiftiBrainordinateScalarFile*>(caretDataFile);
        CaretAssert(clf);
    }
    else {
        clf = new CiftiBrainordinateScalarFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            clf->readFile(filename);
            validateCiftiMappableDataFile(clf);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete clf;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityDenseScalarFiles.push_back(clf);
    }
    
    return clf;
}

/**
 * Read a connectivity parcel data series file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiParcelSeriesFile*
Brain::addReadOrReloadConnectivityParcelSeriesFile(const FileModeAddReadReload fileMode,
                                        CaretDataFile* caretDataFile,
                                        const AString& filename) throw (DataFileException)
{
    CiftiParcelSeriesFile* clf = NULL;
    if (caretDataFile != NULL) {
        clf = dynamic_cast<CiftiParcelSeriesFile*>(caretDataFile);
        CaretAssert(clf);
    }
    else {
        clf = new CiftiParcelSeriesFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            clf->readFile(filename);
            validateCiftiMappableDataFile(clf);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete clf;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityParcelSeriesFiles.push_back(clf);
    }
    
    return clf;
}

/**
 * Read a connectivity parcel scalar file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiParcelScalarFile*
Brain::addReadOrReloadConnectivityParcelScalarFile(const FileModeAddReadReload fileMode,
                                        CaretDataFile* caretDataFile,
                                        const AString& filename) throw (DataFileException)
{
    CiftiParcelScalarFile* clf = NULL;
    if (caretDataFile != NULL) {
        clf = dynamic_cast<CiftiParcelScalarFile*>(caretDataFile);
        CaretAssert(clf);
    }
    else {
        clf = new CiftiParcelScalarFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            clf->readFile(filename);
            validateCiftiMappableDataFile(clf);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete clf;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityParcelScalarFiles.push_back(clf);
    }
    
    return clf;
}


/**
 * Find a cifti scalar file containing shape information.
 *
 * @param ciftiScalarShapeFileOut
 *    Output CIFTI Scalar file that meets requirements (NULL if no matches.
 * @param shapeMapIndexOut
 *    Output Index of map meeting requirements.
 */
void
Brain::getCiftiShapeMap(CiftiBrainordinateScalarFile* &ciftiScalarShapeFileOut,
                        int32_t& ciftiScalarhapeFileMapIndexOut,
                        std::vector<CiftiBrainordinateScalarFile*>& ciftiScalarNotShapeFilesOut) const
{
    ciftiScalarShapeFileOut = NULL;
    ciftiScalarhapeFileMapIndexOut   = -1;
    ciftiScalarNotShapeFilesOut.clear();
    
    CiftiBrainordinateScalarFile* depthMetricFile = NULL;
    int32_t     depthMapIndex = -1;
    CiftiBrainordinateScalarFile* depthNamedMetricFile = NULL;
    CiftiBrainordinateScalarFile* curvatureMetricFile = NULL;
    int32_t     curvatureMapIndex = -1;
    CiftiBrainordinateScalarFile* curvatureNamedMetricFile = NULL;
    CiftiBrainordinateScalarFile* shapeNamedMetricFile = NULL;
    CiftiBrainordinateScalarFile* sulcMetricFile = NULL;
    int32_t     sulcMapIndex = -1;
    CiftiBrainordinateScalarFile* sulcNamedMetricFile = NULL;
    
    const int numFiles = static_cast<int32_t>(m_connectivityDenseScalarFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        CiftiBrainordinateScalarFile* cf = m_connectivityDenseScalarFiles[i];
        const AString filename = cf->getFileNameNoPath().toLower();
        const int32_t numMaps = cf->getNumberOfMaps();
        for (int32_t j = 0; j < numMaps; j++) {
            const AString mapName = cf->getMapName(j).toLower();
            if (mapName.contains("sulc")) {
                if (sulcMetricFile == NULL) {
                    sulcMetricFile = cf;
                    sulcMapIndex   = j;
                }
            }
            else if (mapName.contains("depth")) {
                if (depthMetricFile == NULL) {
                    depthMetricFile = cf;
                    depthMapIndex   = j;
                }
            }
            else if (mapName.contains("curv")) {
                if (curvatureMetricFile == NULL) {
                    curvatureMetricFile = cf;
                    curvatureMapIndex   = j;
                }
            }
        }
        
        if (filename.contains("sulc")) {
            if (numMaps > 0) {
                sulcNamedMetricFile = cf;
            }
        }
        if (filename.contains("shape")) {
            if (numMaps > 0) {
                shapeNamedMetricFile = cf;
            }
        }
        if (filename.contains("curv")) {
            if (numMaps > 0) {
                curvatureNamedMetricFile = cf;
            }
        }
        if (filename.contains("depth")) {
            if (numMaps > 0) {
                depthNamedMetricFile = cf;
            }
        }
    }
    
    if (sulcMetricFile != NULL) {
        ciftiScalarShapeFileOut = sulcMetricFile;
        ciftiScalarhapeFileMapIndexOut   = sulcMapIndex;
    }
    else if (depthMetricFile != NULL) {
        ciftiScalarShapeFileOut = depthMetricFile;
        ciftiScalarhapeFileMapIndexOut   = depthMapIndex;
    }
    else if (curvatureMetricFile != NULL) {
        ciftiScalarShapeFileOut = curvatureMetricFile;
        ciftiScalarhapeFileMapIndexOut   = curvatureMapIndex;
    }
    else if (sulcNamedMetricFile != NULL) {
        ciftiScalarShapeFileOut = sulcNamedMetricFile;
        ciftiScalarhapeFileMapIndexOut   = 0;
    }
    else if (depthNamedMetricFile != NULL) {
        ciftiScalarShapeFileOut = depthNamedMetricFile;
        ciftiScalarhapeFileMapIndexOut   = 0;
    }
    else if (curvatureNamedMetricFile != NULL) {
        ciftiScalarShapeFileOut = curvatureNamedMetricFile;
        ciftiScalarhapeFileMapIndexOut   = 0;
    }
    else if (shapeNamedMetricFile != NULL) {
        ciftiScalarShapeFileOut = shapeNamedMetricFile;
        ciftiScalarhapeFileMapIndexOut   = 0;
    }

    /*
     * Get all shape type files (NULLs okay)
     */
    std::vector<CiftiBrainordinateScalarFile*> ciftiShapeFiles;
    ciftiShapeFiles.push_back(sulcMetricFile);
    ciftiShapeFiles.push_back(depthMetricFile);
    ciftiShapeFiles.push_back(curvatureMetricFile);
    ciftiShapeFiles.push_back(sulcNamedMetricFile);
    ciftiShapeFiles.push_back(depthNamedMetricFile);
    ciftiShapeFiles.push_back(curvatureNamedMetricFile);
    ciftiShapeFiles.push_back(shapeNamedMetricFile);

    /*
     * Find files that are NOT shape files
     */
    for (int32_t i = 0; i < numFiles; i++) {
        CiftiBrainordinateScalarFile* cf = m_connectivityDenseScalarFiles[i];
        if (std::find(ciftiShapeFiles.begin(),
                      ciftiShapeFiles.end(),
                      cf) == ciftiShapeFiles.end()) {
            ciftiScalarNotShapeFilesOut.push_back(cf);
        }
    }
}


/**
 * Read a connectivity fiber orientation file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiFiberOrientationFile*
Brain::addReadOrReloadConnectivityFiberOrientationFile(const FileModeAddReadReload fileMode,
                                            CaretDataFile* caretDataFile,
                                            const AString& filename) throw (DataFileException)
{
    CiftiFiberOrientationFile* cfof = NULL;
    if (caretDataFile != NULL) {
        cfof = dynamic_cast<CiftiFiberOrientationFile*>(caretDataFile);
        CaretAssert(cfof);
    }
    else {
        cfof = new CiftiFiberOrientationFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            cfof->readFile(filename);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete cfof;
            }
            throw dfe;
        }
    }
    
    /*
     * If first fiber orientation file, default the above and below limits
     * to +/- one-half voxel size.
     */
    if (m_connectivityFiberOrientationFiles.empty()) {
        float voxelSizes[3];
        cfof->getVolumeSpacing(voxelSizes);
        float maxVoxelSize = std::max(std::fabs(voxelSizes[0]),
                                            std::max(std::fabs(voxelSizes[1]),
                                                     std::fabs(voxelSizes[2])));
        if (maxVoxelSize <= 0.0) {
            maxVoxelSize = 1.0;
        }
        
        const float aboveLimit =  maxVoxelSize;
        const float belowLimit = -maxVoxelSize;
        m_displayPropertiesFiberOrientation->setAboveAndBelowLimitsForAll(aboveLimit,
                                                                          belowLimit);
    }
    
    if (addFlag) {
        m_connectivityFiberOrientationFiles.push_back(cfof);
    }
    
    return cfof;
}

/**
 * Read a connectivity fiber trajectory file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiFiberTrajectoryFile*
Brain::addReadOrReloadConnectivityFiberTrajectoryFile(const FileModeAddReadReload fileMode,
                                           CaretDataFile* caretDataFile,
                                           const AString& filename) throw (DataFileException)
{
    CiftiFiberTrajectoryFile* cftf = NULL;
    if (caretDataFile != NULL) {
        cftf = dynamic_cast<CiftiFiberTrajectoryFile*>(caretDataFile);
        CaretAssert(cftf);
    }
    else {
        cftf = new CiftiFiberTrajectoryFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            cftf->readFile(filename);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete cftf;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityFiberTrajectoryFiles.push_back(cftf);
    }
    
    return cftf;
}

/**
 * Read a connectivity parcel file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixParcelFile*
Brain::addReadOrReloadConnectivityMatrixParcelFile(const FileModeAddReadReload fileMode,
                                        CaretDataFile* caretDataFile,
                                        const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixParcelFile* file = NULL;
    if (caretDataFile != NULL) {
        file = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(caretDataFile);
        CaretAssert(file);
    }
    else {
        file = new CiftiConnectivityMatrixParcelFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            file->readFile(filename);
            validateCiftiMappableDataFile(file);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete file;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityMatrixParcelFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a connectivity parcel dense file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixParcelDenseFile*
Brain::addReadOrReloadConnectivityMatrixParcelDenseFile(const FileModeAddReadReload fileMode,
                                             CaretDataFile* caretDataFile,
                                             const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixParcelDenseFile* file = NULL;
    if (caretDataFile != NULL) {
        file = dynamic_cast<CiftiConnectivityMatrixParcelDenseFile*>(caretDataFile);
        CaretAssert(file);
    }
    else {
        file = new CiftiConnectivityMatrixParcelDenseFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            file->readFile(filename);
            validateCiftiMappableDataFile(file);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete file;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityMatrixParcelDenseFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a connectivity data series file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiBrainordinateDataSeriesFile*
Brain::addReadOrReloadConnectivityDataSeriesFile(const FileModeAddReadReload fileMode,
                                      CaretDataFile* caretDataFile,
                                      const AString& filename) throw (DataFileException)
{
    CiftiBrainordinateDataSeriesFile* file = NULL;
    if (caretDataFile != NULL) {
        file = dynamic_cast<CiftiBrainordinateDataSeriesFile*>(caretDataFile);
        CaretAssert(file);
    }
    else {
        file = new CiftiBrainordinateDataSeriesFile();
    }
    
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            file->readFile(filename);
            validateCiftiMappableDataFile(file);
        }
        catch (const DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete file;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_connectivityDataSeriesFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a palette file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
PaletteFile*
Brain::addReadOrReloadPaletteFile(const FileModeAddReadReload fileMode,
                       CaretDataFile* /*caretDataFile*/,
                       const AString& /*filename*/) throw (DataFileException)
{
    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        
    }
    
    if (addFlag) {
        
    }
    
    throw DataFileException("Reading not implemented for: palette");
    
    return NULL;
}

/**
 * Read a scene file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
SceneFile*
Brain::addReadOrReloadSceneFile(const FileModeAddReadReload fileMode,
                     CaretDataFile* caretDataFile,
                     const AString& filename) throw (DataFileException)
{
    SceneFile* sf = NULL;
    if (caretDataFile != NULL) {
        sf = dynamic_cast<SceneFile*>(caretDataFile);
        CaretAssert(sf);
    }
    else {
        sf = new SceneFile();
    }

    bool addFlag  = false;
    bool readFlag = false;
    switch (fileMode) {
        case FILE_MODE_ADD:
            addFlag = true;
            break;
        case FILE_MODE_READ:
            addFlag = true;
            readFlag = true;
            break;
        case FILE_MODE_RELOAD:
            readFlag = true;
            break;
    }
    
    if (readFlag) {
        try {
            sf->readFile(filename);
        }
        catch (DataFileException& dfe) {
            if (caretDataFile != NULL) {
                removeAndDeleteDataFile(caretDataFile);
            }
            else {
                delete sf;
            }
            throw dfe;
        }
    }
    
    if (addFlag) {
        m_sceneFiles.push_back(sf);
    }
    
    return sf;
}

/**
 * @return Number of connectivity matrix dense files.
 */
int32_t 
Brain::getNumberOfConnectivityMatrixDenseFiles() const
{
    return m_connectivityMatrixDenseFiles.size();
}

/**
 * Get the connectivity dense file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense file at index.
 */
CiftiConnectivityMatrixDenseFile*
Brain::getConnectivityMatrixDenseFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityMatrixDenseFiles, indx);
    return m_connectivityMatrixDenseFiles[indx];
}

/**
 * Get the connectivity dense file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense file at index.
 */
const CiftiConnectivityMatrixDenseFile* 
Brain::getConnectivityMatrixDenseFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityMatrixDenseFiles, indx);
    return m_connectivityMatrixDenseFiles[indx];
}

/**
 * Get ALL connectivity matrix dense files.
 * @param connectivityFilesOut
 *   Contains all connectivity dense files on exit.
 */
void 
Brain::getConnectivityMatrixDenseFiles(std::vector<CiftiConnectivityMatrixDenseFile*>& connectivityDenseFilesOut) const
{
    connectivityDenseFilesOut = m_connectivityMatrixDenseFiles;
}

/**
 * @return Number of connectivity dense label files.
 */
int32_t
Brain::getNumberOfConnectivityDenseLabelFiles() const
{
    return m_connectivityDenseLabelFiles.size();
}

/**
 * Get the connectivity dense label file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense label file at index.
 */
CiftiBrainordinateLabelFile*
Brain::getConnectivityDenseLabelFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityDenseLabelFiles, indx);
    return m_connectivityDenseLabelFiles[indx];
}

/**
 * Get the connectivity dense label file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense label file at index.
 */
const CiftiBrainordinateLabelFile*
Brain::getConnectivityDenseLabelFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityDenseLabelFiles, indx);
    return m_connectivityDenseLabelFiles[indx];
}

/**
 * Get ALL connectivity dense label files.
 * @param connectivityDenseLabelFilesOut
 *   Contains all connectivity dense labelfiles on exit.
 */
void
Brain::getConnectivityDenseLabelFiles(std::vector<CiftiBrainordinateLabelFile*>& connectivityDenseLabelFilesOut) const
{
    connectivityDenseLabelFilesOut = m_connectivityDenseLabelFiles;
}

/**
 * Get all of the CIFTI Mappable Data Files
 * @param allCiftiMappableDataFilesOut
 *    Contains all CIFTI Mappable Data files upon exit.
 */
void
Brain::getAllCiftiMappableDataFiles(std::vector<CiftiMappableDataFile*>& allCiftiMappableDataFilesOut) const
{
    allCiftiMappableDataFilesOut.clear();
    
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        CiftiMappableDataFile* cmdf = dynamic_cast<CiftiMappableDataFile*>(*iter);
        if (cmdf != NULL) {
            allCiftiMappableDataFilesOut.push_back(cmdf);
        }
    }
}

/**
 * Get all of the Brainordinate Chartable Data Files.  Only files that implement the 
 * ChartableBrainordinateInterface AND return true for ChartableBrainordinateInterface::isChartingSupported()
 * are included in the returned files.
 *
 * @param chartableDataFilesOut
 *    Contains all chartable data files upon exit.
 */
void
Brain::getAllChartableBrainordinateDataFiles(std::vector<ChartableBrainordinateInterface*>& chartableDataFilesOut) const
{
    chartableDataFilesOut.clear();
    
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        ChartableBrainordinateInterface* chartFile = dynamic_cast<ChartableBrainordinateInterface*>(*iter);
        if (chartFile != NULL) {
            if (chartFile->isBrainordinateChartingSupported()) {
                chartableDataFilesOut.push_back(chartFile);
            }
        }
    }
}

/**
 * Get all of the Brainordinate Chartable Data Files.  Only files that implement the
 * ChartableBrainordinateInterface AND return true for ChartableBrainordinateInterface::isChartingSupported()
 * and support a chart of the given data type are included in the returned files.
 *
 * @param chartDataType
 *    Desired chart data type.
 * @param chartableDataFilesOut
 *    Contains all chartable data files upon exit.
 */
void
Brain::getAllChartableBrainordinateDataFilesForChartDataType(const ChartDataTypeEnum::Enum chartDataType,
                                                std::vector<ChartableBrainordinateInterface*>& chartableDataFilesOut) const
{
    chartableDataFilesOut.clear();
    
    std::vector<ChartableBrainordinateInterface*> chartFiles;
    getAllChartableBrainordinateDataFiles(chartFiles);
    
    for (std::vector<ChartableBrainordinateInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableBrainordinateInterface* chartFile = *iter;
        if (chartFile->isBrainordinateChartDataTypeSupported(chartDataType)) {
            chartableDataFilesOut.push_back(chartFile);
        }
    }
}


/**
 * Get all of the Brainordinate Chartable Data Files.  Only files that implement the
 * ChartableBrainordinateInterface, return true for ChartableBrainordinateInterface::isChartingSupported(),
 * AND return true for ChartableBrainordinateInterface::isChartingEnabled() for any tab index
 * are included in the returned files.
 *
 * @param chartableDataFilesOut
 *    Contains all chartable data files upon exit.
 */
void
Brain::getAllChartableBrainordinateDataFilesWithChartingEnabled(std::vector<ChartableBrainordinateInterface*>& chartableDataFilesOut) const
{
    chartableDataFilesOut.clear();
    
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    const std::vector<int32_t> tabIndices = allTabsEvent.getBrowserTabIndices();
    const int32_t numTabs = static_cast<int32_t>(tabIndices.size());
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        ChartableBrainordinateInterface* chartFile = dynamic_cast<ChartableBrainordinateInterface*>(*iter);
        if (chartFile != NULL) {
            if (chartFile->isBrainordinateChartingSupported()) {
                for (int32_t iTab = 0; iTab < numTabs; iTab++) {
                    const int32_t tabIndex = tabIndices[iTab];
                    if (chartFile->isBrainordinateChartingEnabled(tabIndex)) {
                        chartableDataFilesOut.push_back(chartFile);
                        break;
                    }
                }
            }
        }
    }
}

/**
 * Get all of the Chartable Matrix Data Files.  Only files that implement the
 * ChartableMatrixInterface AND return true for ChartableMatrixInterface::isChartingSupported()
 * are included in the returned files.
 *
 * @param chartableDataFilesOut
 *    Contains all chartable data files upon exit.
 */
void
Brain::getAllChartableMatrixDataFiles(std::vector<ChartableMatrixInterface*>& chartableDataFilesOut) const
{
    chartableDataFilesOut.clear();
    
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        ChartableMatrixInterface* chartFile = dynamic_cast<ChartableMatrixInterface*>(*iter);
        if (chartFile != NULL) {
            if (chartFile->isMatrixChartingSupported()) {
                chartableDataFilesOut.push_back(chartFile);
            }
        }
    }
}

/**
 * Get all of the Chartable Matrix Data Files.  Only files that implement the
 * ChartableMatrixInterface AND return true for ChartableMatrixInterface::isChartingSupported()
 * and support a chart of the given data type are included in the returned files.
 *
 * @param chartDataType
 *    Desired chart data type.
 * @param chartableDataFilesOut
 *    Contains all chartable data files upon exit.
 */
void
Brain::getAllChartableMatrixDataFilesForChartDataType(const ChartDataTypeEnum::Enum chartDataType,
                                                      std::vector<ChartableMatrixInterface*>& chartableDataFilesOut) const
{
    chartableDataFilesOut.clear();
    
    std::vector<ChartableMatrixInterface*> chartFiles;
    getAllChartableMatrixDataFiles(chartFiles);
    
    for (std::vector<ChartableMatrixInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableMatrixInterface* chartFile = *iter;
        if (chartFile->isMatrixChartDataTypeSupported(chartDataType)) {
            chartableDataFilesOut.push_back(chartFile);
        }
    }
}

/**
 * @return Number of cifti dense parcel files.
 */
int32_t
Brain::getNumberOfConnectivityMatrixDenseParcelFiles() const
{
    return m_connectivityMatrixDenseParcelFiles.size();
}

/**
 * Get the cifti dense parcel file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti dense parcel file at index.
 */
CiftiConnectivityMatrixDenseParcelFile*
Brain::getConnectivityMatrixDenseParcelFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityMatrixDenseParcelFiles, indx);
    return m_connectivityMatrixDenseParcelFiles[indx];
}

/**
 * Get the connectivity dense parcel file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti dense parcel file at index.
 */
const CiftiConnectivityMatrixDenseParcelFile*
Brain::getConnectivityMatrixDenseParcelFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityMatrixDenseParcelFiles, indx);
    return m_connectivityMatrixDenseParcelFiles[indx];
}

/**
 * Get ALL cifti dense parcel files.
 * @param connectivityFilesOut
 *   Contains all cifti dense parcel files on exit.
 */
void
Brain::getConnectivityMatrixDenseParcelFiles(std::vector<CiftiConnectivityMatrixDenseParcelFile*>& connectivityDenseParcelFilesOut) const
{
    connectivityDenseParcelFilesOut = m_connectivityMatrixDenseParcelFiles;
}

/**
 * @return Number of connectivity dense scalar files.
 */
int32_t
Brain::getNumberOfConnectivityDenseScalarFiles() const
{
    return m_connectivityDenseScalarFiles.size();
}

/**
 * Get the connectivity dense scalar file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense scalar file at index.
 */
CiftiBrainordinateScalarFile*
Brain::getConnectivityDenseScalarFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityDenseScalarFiles, indx);
    return m_connectivityDenseScalarFiles[indx];
}

/**
 * Get the connectivity dense scalar file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense scalar file at index.
 */
const CiftiBrainordinateScalarFile*
Brain::getConnectivityDenseScalarFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityDenseScalarFiles, indx);
    return m_connectivityDenseScalarFiles[indx];
}

/**
 * Get ALL connectivity dense scalr files.
 * @param connectivityDenseScalarFilesOut
 *   Contains all connectivity dense files on exit.
 */
void
Brain::getConnectivityDenseScalarFiles(std::vector<CiftiBrainordinateScalarFile*>& connectivityDenseScalarFilesOut) const
{
    connectivityDenseScalarFilesOut = m_connectivityDenseScalarFiles;
}

/**
 * @return Number of connectivity parcel scalar files.
 */
int32_t
Brain::getNumberOfConnectivityParcelScalarFiles() const
{
    return m_connectivityParcelScalarFiles.size();
}

/**
 * Get the connectivity parcel scalar file at the given index.
 * @param indx
 *    Index of file.
 * @return Connectivity parcel scalar file at index.
 */
CiftiParcelScalarFile*
Brain::getConnectivityParcelScalarFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityParcelScalarFiles, indx);
    return m_connectivityParcelScalarFiles[indx];
}

/**
 * Get the connectivity parcel scalar file at the given index.
 * @param indx
 *    Index of file.
 * @return Connectivity parcel scalar file at index.
 */
const CiftiParcelScalarFile*
Brain::getConnectivityParcelScalarFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityParcelScalarFiles, indx);
    return m_connectivityParcelScalarFiles[indx];
}

/**
 * Get ALL connectivity parcel scalar files.
 * @param connectivityParcelScalarFilesOut
 *   Contains all connectivity parcel files on exit.
 */
void
Brain::getConnectivityParcelScalarFiles(std::vector<CiftiParcelScalarFile*>& connectivityParcelScalarFilesOut) const
{
    connectivityParcelScalarFilesOut = m_connectivityParcelScalarFiles;
}

/**
 * @return Number of connectivity parcel data series files.
 */
int32_t
Brain::getNumberOfConnectivityParcelSeriesFiles() const
{
    return m_connectivityParcelSeriesFiles.size();
}

/**
 * Get the connectivity parcel data series file at the given index.
 * @param indx
 *    Index of file.
 * @return Connectivity parcel data series file at index.
 */
CiftiParcelSeriesFile*
Brain::getConnectivityParcelSeriesFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityParcelSeriesFiles, indx);
    return m_connectivityParcelSeriesFiles[indx];
}

/**
 * Get the connectivity parcel data series file at the given index.
 * @param indx
 *    Index of file.
 * @return Connectivity parcel data series file at index.
 */
const CiftiParcelSeriesFile*
Brain::getConnectivityParcelSeriesFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityParcelSeriesFiles, indx);
    return m_connectivityParcelSeriesFiles[indx];
}

/**
 * Get ALL connectivity parcel data series files.
 * @param connectivityParcelSeriesFilesOut
 *   Contains all connectivity parcel files on exit.
 */
void
Brain::getConnectivityParcelSeriesFiles(std::vector<CiftiParcelSeriesFile*>& connectivityParcelSeriesFilesOut) const
{
    connectivityParcelSeriesFilesOut = m_connectivityParcelSeriesFiles;
}

/**
 * @return Number of connectivity fiber orientation files.
 */
int32_t
Brain::getNumberOfConnectivityFiberOrientationFiles() const
{
    return m_connectivityFiberOrientationFiles.size();
}

/**
 * Get the connectivity fiber orientation file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity fiber orientation file at index.
 */
CiftiFiberOrientationFile*
Brain::getConnectivityFiberOrientationFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityFiberOrientationFiles, indx);
    return m_connectivityFiberOrientationFiles[indx];
}

/**
 * Get the connectivity fiber orientation file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity fiber orientation file at index.
 */
const CiftiFiberOrientationFile*
Brain::getConnectivityFiberOrientationFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityFiberOrientationFiles, indx);
    return m_connectivityFiberOrientationFiles[indx];
}

/**
 * Get ALL connectivity fiber orientation files.
 * @param connectivityFiberOrientationFilesOut
 *   Contains all connectivity fiber orientation files on exit.
 */
void
Brain::getConnectivityFiberOrientationFiles(std::vector<CiftiFiberOrientationFile*>& connectivityFiberOrientationFilesOut) const
{
    connectivityFiberOrientationFilesOut = m_connectivityFiberOrientationFiles;
}

/**
 * @return Number of connectivity fiber trajectory files.
 */
int32_t
Brain::getNumberOfConnectivityFiberTrajectoryFiles() const
{
    return m_connectivityFiberTrajectoryFiles.size();
}

/**
 * Get the connectivity fiber trajectory file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity fiber trajectory file at index.
 */
CiftiFiberTrajectoryFile*
Brain::getConnectivityFiberTrajectoryFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityFiberTrajectoryFiles, indx);
    return m_connectivityFiberTrajectoryFiles[indx];
}

/**
 * Get the connectivity fiber trajectory file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity fiber trajectory file at index.
 */
const CiftiFiberTrajectoryFile*
Brain::getConnectivityFiberTrajectoryFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityFiberTrajectoryFiles, indx);
    return m_connectivityFiberTrajectoryFiles[indx];
}

/**
 * Get ALL connectivity fiber trajectory files.
 * @param connectivityFiberTrajectoryFilesOut
 *   Contains all connectivity fiber trajectory files on exit.
 */
void
Brain::getConnectivityFiberTrajectoryFiles(std::vector<CiftiFiberTrajectoryFile*>& connectivityFiberTrajectoryFilesOut) const
{
    connectivityFiberTrajectoryFilesOut = m_connectivityFiberTrajectoryFiles;
}

/**
 * @return Number of cifti parcel files.
 */
int32_t
Brain::getNumberOfConnectivityMatrixParcelFiles() const
{
    return m_connectivityMatrixParcelFiles.size();
}

/**
 * Get the cifti parcel file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti parcel file at index.
 */
CiftiConnectivityMatrixParcelFile*
Brain::getConnectivityMatrixParcelFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityMatrixParcelFiles, indx);
    return m_connectivityMatrixParcelFiles[indx];
}

/**
 * Get the connectivity parcel file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti parcel file at index.
 */
const CiftiConnectivityMatrixParcelFile*
Brain::getConnectivityMatrixParcelFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityMatrixParcelFiles, indx);
    return m_connectivityMatrixParcelFiles[indx];
}

/**
 * Get ALL cifti parcel files.
 * @param connectivityFilesOut
 *   Contains all cifti parcel files on exit.
 */
void
Brain::getConnectivityMatrixParcelFiles(std::vector<CiftiConnectivityMatrixParcelFile*>& connectivityParcelFilesOut) const
{
    connectivityParcelFilesOut = m_connectivityMatrixParcelFiles;
}

/**
 * @return Number of cifti parcel dense files.
 */
int32_t
Brain::getNumberOfConnectivityMatrixParcelDenseFiles() const
{
    return m_connectivityMatrixParcelDenseFiles.size();
}

/**
 * Get the cifti parcel dense file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti parcel dense file at index.
 */
CiftiConnectivityMatrixParcelDenseFile*
Brain::getConnectivityMatrixParcelDenseFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityMatrixParcelDenseFiles, indx);
    return m_connectivityMatrixParcelDenseFiles[indx];
}

/**
 * Get the connectivity parcel dense file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti parcel dense file at index.
 */
const CiftiConnectivityMatrixParcelDenseFile*
Brain::getConnectivityMatrixParcelDenseFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityMatrixParcelDenseFiles, indx);
    return m_connectivityMatrixParcelDenseFiles[indx];
}

/**
 * Get ALL cifti parcel dense files.
 * @param connectivityFilesOut
 *   Contains all cifti parcel dense files on exit.
 */
void
Brain::getConnectivityMatrixParcelDenseFiles(std::vector<CiftiConnectivityMatrixParcelDenseFile*>& connectivityParcelDenseFilesOut) const
{
    connectivityParcelDenseFilesOut = m_connectivityMatrixParcelDenseFiles;
}

/**
 * @return Number of cifti data series files.
 */
int32_t
Brain::getNumberOfConnectivityDataSeriesFiles() const
{
    return m_connectivityDataSeriesFiles.size();
}

/**
 * Get the cifti data series file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti data series file at index.
 */
CiftiBrainordinateDataSeriesFile*
Brain::getConnectivityDataSeriesFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityDataSeriesFiles, indx);
    return m_connectivityDataSeriesFiles[indx];
}

/**
 * Get the connectivity data series file at the given index.
 * @param indx
 *    Index of file.
 * @return cifti data series file at index.
 */
const CiftiBrainordinateDataSeriesFile*
Brain::getConnectivityDataSeriesFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityDataSeriesFiles, indx);
    return m_connectivityDataSeriesFiles[indx];
}

/**
 * Get ALL cifti data series files.
 * @param connectivityDataSeriesFilesOut
 *   Contains all cifti data series files on exit.
 */
void
Brain::getConnectivityDataSeriesFiles(std::vector<CiftiBrainordinateDataSeriesFile*>& connectivityDataSeriesFilesOut) const
{
    connectivityDataSeriesFilesOut = m_connectivityDataSeriesFiles;
}

/**
 * Get all of the cifti connectivity type data files.
 * 
 * param allCiftiConnectivityMatrixFiles
 *   Will contain the files upon exit.
 */
void
Brain::getAllCiftiConnectivityMatrixFiles(std::vector<CiftiMappableConnectivityMatrixDataFile*>& allCiftiConnectivityMatrixFiles) const
{
    allCiftiConnectivityMatrixFiles.clear();
    
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        CiftiMappableConnectivityMatrixDataFile* cmdf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(*iter);
        if (cmdf != NULL) {
            allCiftiConnectivityMatrixFiles.push_back(cmdf);
        }
    }
}

/**
 * Add a data file to the brain.
 *
 * This will add the file to its corresponding data file type and add the
 * file into the spec file.
 *
 * @param caretDataFile
 *    The caret data file.
 * @throw DataFileException
 *    If there is an error.
 */
void
Brain::addDataFile(CaretDataFile* caretDataFile) throw (DataFileException)
{
    CaretAssert(caretDataFile);
    
    caretDataFile->setFileName(updateFileNameForWriting(caretDataFile->getFileName()));
    
    const StructureEnum::Enum structure = caretDataFile->getStructure();
    
    BrainStructure* brainStructure = getBrainStructure(structure,
                                                       false);
    
    const DataFileTypeEnum::Enum dataFileType = caretDataFile->getDataFileType();
            switch (dataFileType) {
                case DataFileTypeEnum::BORDER:
                {
                    BorderFile* file = dynamic_cast<BorderFile*>(caretDataFile);
                    CaretAssert(file);
                    m_borderFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE:
                {
                    CiftiConnectivityMatrixDenseFile* file = dynamic_cast<CiftiConnectivityMatrixDenseFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityMatrixDenseFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                {
                    CiftiBrainordinateLabelFile* file = dynamic_cast<CiftiBrainordinateLabelFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityDenseLabelFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                {
                    CiftiConnectivityMatrixDenseParcelFile* file = dynamic_cast<CiftiConnectivityMatrixDenseParcelFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityMatrixDenseParcelFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                {
                    CiftiBrainordinateScalarFile* file = dynamic_cast<CiftiBrainordinateScalarFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityDenseScalarFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                {
                    CiftiBrainordinateDataSeriesFile* file = dynamic_cast<CiftiBrainordinateDataSeriesFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityDataSeriesFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                {
                    CiftiFiberOrientationFile* file = dynamic_cast<CiftiFiberOrientationFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityFiberOrientationFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                {
                    CiftiFiberTrajectoryFile* file = dynamic_cast<CiftiFiberTrajectoryFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityFiberTrajectoryFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                {
                    CiftiConnectivityMatrixParcelFile* file = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityMatrixParcelFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                {
                    CiftiConnectivityMatrixParcelDenseFile* file = dynamic_cast<CiftiConnectivityMatrixParcelDenseFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityMatrixParcelDenseFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                {
                    CiftiParcelScalarFile* file = dynamic_cast<CiftiParcelScalarFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityParcelScalarFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                {
                    CiftiParcelSeriesFile* file = dynamic_cast<CiftiParcelSeriesFile*>(caretDataFile);
                    CaretAssert(file);
                    m_connectivityParcelSeriesFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::FOCI:
                {
                    FociFile* file = dynamic_cast<FociFile*>(caretDataFile);
                    CaretAssert(file);
                    m_fociFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::LABEL:
                {
                    LabelFile* file = dynamic_cast<LabelFile*>(caretDataFile);
                    CaretAssert(file);
                    if (structure == StructureEnum::INVALID) {
                        throw DataFileException("Structure in label file is INVALID.");
                    }
                    if (brainStructure == NULL) {
                        throw DataFileException("Must load surface(s) with matching structure prior to label files");
                    }
                    brainStructure->addLabelFile(file,
                                                 true);
                }
                    break;
                case DataFileTypeEnum::METRIC:
                {
                    MetricFile* file = dynamic_cast<MetricFile*>(caretDataFile);
                    CaretAssert(file);
                    if (structure == StructureEnum::INVALID) {
                        throw DataFileException("Structure in metric file is INVALID.");
                    }
                    if (brainStructure == NULL) {
                        throw DataFileException("Must load surface(s) with matching structure prior to metric files");
                    }
                    brainStructure->addMetricFile(file,
                                                 true);
                }
                    break;
                case DataFileTypeEnum::PALETTE:
                {
                    throw DataFileException("Adding palette files not supported at this time.");
                }
                    break;
                case DataFileTypeEnum::RGBA:
                {
                    RgbaFile* file = dynamic_cast<RgbaFile*>(caretDataFile);
                    CaretAssert(file);
                    if (structure == StructureEnum::INVALID) {
                        throw DataFileException("Structure in rgba file is INVALID.");
                    }
                    if (brainStructure == NULL) {
                        throw DataFileException("Must load surface(s) with matching structure prior to label files");
                    }
                    brainStructure->addRgbaFile(file,
                                                 true);
                }
                    break;
                case DataFileTypeEnum::SCENE:
                {
                    SceneFile* file = dynamic_cast<SceneFile*>(caretDataFile);
                    CaretAssert(file);
                    m_sceneFiles.push_back(file);
                }
                    break;
                case DataFileTypeEnum::SPECIFICATION:
                    CaretLogSevere("PROGRAM ERROR: Reading spec file should never call Brain::addReadOrReloadDataFile()");
                    throw DataFileException("PROGRAM ERROR: Reading spec file should never call Brain::addReadOrReloadDataFile()");
                    break;
                case DataFileTypeEnum::SURFACE:
                {
                    Surface* file = dynamic_cast<Surface*>(caretDataFile);
                    if (structure == StructureEnum::INVALID) {
                        throw DataFileException("Structure in surface file is INVALID.");
                    }
                    if (file == NULL) {
                        throw DataFileException("Cannot add SurfaceFile but can add a Surface.");
                    }
                    if (brainStructure == NULL) {
                        brainStructure = getBrainStructure(structure,
                                                           true);
                    }
                    brainStructure->addSurface(file,
                                               true,
                                               true);
                }
                    break;
                case DataFileTypeEnum::UNKNOWN:
                    throw DataFileException("Unable to read files of type");
                    break;
                case DataFileTypeEnum::VOLUME:
                {
                    VolumeFile* file = dynamic_cast<VolumeFile*>(caretDataFile);
                    CaretAssert(file);
                    m_volumeFiles.push_back(file);
                }
                    break;
            }
            
            m_specFile->addCaretDataFile(caretDataFile);
}


/**
 * @return Number of border files.
 */
int32_t 
Brain::getNumberOfBorderFiles() const
{
    return m_borderFiles.size();
}

/**
 * @return The border file.
 * @param indx Index of the border file.
 */
BorderFile* 
Brain::getBorderFile(const int32_t indx)
{
    CaretAssertVectorIndex(m_borderFiles, indx);
    return m_borderFiles[indx];
}

/**
 * @return The border file.
 * @param indx Index of the border file.
 */
const BorderFile* 
Brain::getBorderFile(const int32_t indx) const
{
    CaretAssertVectorIndex(m_borderFiles, indx);
    return m_borderFiles[indx];
}

/**
 * @return Number of foci files.
 */
int32_t 
Brain::getNumberOfFociFiles() const
{
    return m_fociFiles.size();
}

/**
 * @return The foci file.
 * @param indx Index of the foci file.
 */
FociFile* 
Brain::getFociFile(const int32_t indx)
{
    CaretAssertVectorIndex(m_fociFiles, indx);
    return m_fociFiles[indx];
}

/**
 * @return The foci file.
 * @param indx Index of the foci file.
 */
const FociFile* 
Brain::getFociFile(const int32_t indx) const
{
    CaretAssertVectorIndex(m_fociFiles, indx);
    return m_fociFiles[indx];
}

/**
 * @return Number of scene files.
 */
int32_t 
Brain::getNumberOfSceneFiles() const
{
    return m_sceneFiles.size();
}

/**
 * @return The scene file.
 * @param indx Index of the scene file.
 */
SceneFile* 
Brain::getSceneFile(const int32_t indx)
{
    CaretAssertVectorIndex(m_sceneFiles, indx);
    return m_sceneFiles[indx];
}

/**
 * @return The scene file.
 * @param indx Index of the scene file.
 */
const SceneFile* 
Brain::getSceneFile(const int32_t indx) const
{
    CaretAssertVectorIndex(m_sceneFiles, indx);
    return m_sceneFiles[indx];
}

/**
 * @return The Spec File.
 */
const SpecFile*
Brain::getSpecFile() const
{
    return m_specFile;
}

/**
 * @return The Spec File.
 */
SpecFile*
Brain::getSpecFile()
{
    return m_specFile;
}

/*
 * @return The palette file.
 */
PaletteFile* 
Brain::getPaletteFile()
{
    return m_paletteFile;
}

/*
 * @return The palette file.
 */
const PaletteFile* 
Brain::getPaletteFile() const
{
    return m_paletteFile;
}

/**
 * Find the surface with the given name.
 * @param surfaceFileName
 *    Name of surface.
 * @param useAbsolutePath
 *    If true the given surfaceFileName is an absolute path.
 *    If false, the given surfaceFileName is just the file
 *    name without any path.
 */
Surface*
Brain::getSurfaceWithName(const AString& surfaceFileName,
                            const bool useAbsolutePath)
{
    for (std::vector<BrainStructure*>::iterator iter = m_brainStructures.begin();
         iter != m_brainStructures.end();
         iter++) {
        BrainStructure* bs = *iter;
        Surface* surface = bs->getSurfaceWithName(surfaceFileName,
                                                  useAbsolutePath);
        if (surface != NULL) {
            return surface;
        }
    }
    
    return NULL;
}

/**
 * @return The volume interaction surfaces from all brain structures.
 */
std::vector<const Surface*>
Brain::getVolumeInteractionSurfaces() const
{
    std::vector<const Surface*> surfaces;
    
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        surfaces.push_back(m_brainStructures[i]->getVolumeInteractionSurface());
    }
    
    return surfaces;
}

/**
 * @return The volume interaction surfaces from all brain structures.
 */
std::vector<const SurfaceFile*>
Brain::getVolumeInteractionSurfaceFiles() const
{
    std::vector<const Surface*> surfaces = getVolumeInteractionSurfaces();
    std::vector<const SurfaceFile*> surfaceFiles;
    surfaceFiles.insert(surfaceFiles.end(),
                        surfaces.begin(),
                        surfaces.end());
    
    return surfaceFiles;
}

/**
 * Get the volume interaction surface for the given structure.
 *
 * @param structure
 *    Structure for which a volume interaction surface is requested.
 * @return
 *    The volume interaction surface corresonding to the given structure.
 *    NULL may be returned if a surface is not available.
 */
const Surface*
Brain::getVolumeInteractionSurfaceForStructure(const StructureEnum::Enum structure) const
{
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        if (m_brainStructures[i]->getStructure() == structure) {
            return m_brainStructures[i]->getVolumeInteractionSurface();
        }
    }
    
    return NULL;
}


/**
 * Get the volume interaction surface nearest the given coordinate and
 * within the given tolerance.
 *
 * @param xyz
 *     The coordinate.
 * @param tolerance
 *     The tolerance (if negative tolerance is ignored).
 * @return
 *     Nearest surface or NULL if nearest surface not within tolerance.
 */
Surface*
Brain::getVolumeInteractionSurfaceNearestCoordinate(const float xyz[3],
                                                      const float tolerance)
{
    Surface* nearestSurface = NULL;
    float nearestDistance = ((tolerance > 0.0)
                             ? tolerance
                             : std::numeric_limits<float>::max());
    
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        Surface* surface = m_brainStructures[i]->getVolumeInteractionSurface();
        const int32_t nodeIndex = surface->closestNode(xyz,
                                                tolerance);
        if (nodeIndex >= 0) {
            const float dist = MathFunctions::distanceSquared3D(xyz,
                                                                surface->getCoordinate(nodeIndex));
            if (dist < nearestDistance) {
                nearestDistance = dist;
                nearestSurface = surface;
            }
        }
    }
    
    return nearestSurface;
}

/**
 * Update the chart model.
 */
void
Brain::updateChartModel()
{
    std::vector<ChartableBrainordinateInterface*> chartableBrainordinateFiles;
    getAllChartableBrainordinateDataFiles(chartableBrainordinateFiles);
    
    std::vector<ChartableMatrixInterface*> chartableMatrixFiles;
    getAllChartableMatrixDataFiles(chartableMatrixFiles);
    
    const int32_t numberOfChartableFiles = (chartableBrainordinateFiles.size()
                                            + chartableMatrixFiles.size());
    
    if (numberOfChartableFiles > 0) {
        if (m_modelChart == NULL) {
            m_modelChart = new ModelChart(this);
            EventModelAdd eventAddModel(m_modelChart);
            EventManager::get()->sendEvent(eventAddModel.getPointer());
            
            if (m_isSpecFileBeingRead == false) {
                m_modelChart->initializeOverlays();
            }
        }
    }
    else {
        if (m_modelChart != NULL) {
            EventModelDelete eventDeleteModel(m_modelChart);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete m_modelChart;
            m_modelChart = NULL;
        }
    }
}

/**
 * Update the volume slice model.
 */
void 
Brain::updateVolumeSliceModel()
{
    bool isValid = false;
    std::vector<CaretMappableDataFile*> allCaretMappableDataFiles;
    getAllMappableDataFiles(allCaretMappableDataFiles);
    for (std::vector<CaretMappableDataFile*>::iterator iter = allCaretMappableDataFiles.begin();
         iter != allCaretMappableDataFiles.end();
         iter++) {
        CaretMappableDataFile* cmdf = *iter;
        if (cmdf->isVolumeMappable()) {
            if (cmdf->isEmpty() == false) {
                isValid = true;
                break;                
            }
        }
    }
    
    if (isValid) {
        if (m_volumeSliceModel == NULL) {
            m_volumeSliceModel = new ModelVolume(this);
            EventModelAdd eventAddModel(m_volumeSliceModel);
            EventManager::get()->sendEvent(eventAddModel.getPointer());

            if (m_isSpecFileBeingRead == false) {
                m_volumeSliceModel->initializeOverlays();
            }
        }
    }
    else {
        if (m_volumeSliceModel != NULL) {
            EventModelDelete eventDeleteModel(m_volumeSliceModel);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete m_volumeSliceModel;
            m_volumeSliceModel = NULL;
        }
    }
    
}

/**
 * Update the whole brain model.
 */
void 
Brain::updateWholeBrainModel()
{
    bool isValid = false;
    if ((getNumberOfBrainStructures() > 0)
         || (getNumberOfVolumeFiles() > 0)) {
        isValid = true;
    }
     
    if (isValid) {
        if (m_wholeBrainModel == NULL) {
            m_wholeBrainModel = new ModelWholeBrain(this);
            EventModelAdd eventAddModel(m_wholeBrainModel);
            EventManager::get()->sendEvent(eventAddModel.getPointer());
            
            if (m_isSpecFileBeingRead == false) {
                m_wholeBrainModel->initializeOverlays();
            }
        }
        
        m_wholeBrainModel->updateModel();
    }
    else {
        if (m_wholeBrainModel != NULL) {
            EventModelDelete eventDeleteModel(m_wholeBrainModel);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete m_wholeBrainModel;
            m_wholeBrainModel = NULL;
        }
    }    
}

/**
 * Update the surface montage model
 */
void 
Brain::updateSurfaceMontageModel()
{
    bool isValid = false;
    if (getNumberOfBrainStructures() > 0) {
        isValid = true;
    }
    
    if (isValid) {
        if (m_surfaceMontageModel == NULL) {
            m_surfaceMontageModel = new ModelSurfaceMontage(this);
            EventModelAdd eventAddModel(m_surfaceMontageModel);
            EventManager::get()->sendEvent(eventAddModel.getPointer());
            
            if (m_isSpecFileBeingRead == false) {
                m_surfaceMontageModel->initializeOverlays();
            }
        }
    }
    else {
        if (m_surfaceMontageModel != NULL) {
            EventModelDelete eventDeleteModel(m_surfaceMontageModel);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete m_surfaceMontageModel;
            m_surfaceMontageModel = NULL;
        }
    }
}

/**
 * Process a reload data file event.
 * @param reloadDataFileEvent
 *    Event containing file for reloading and my be updated with error messages.
 */
void
Brain::processReloadDataFileEvent(EventDataFileReload* reloadDataFileEvent)
{
    /*
     * Verify that file is already in memory.
     */
    std::vector<CaretDataFile*> allDataFiles;
    getAllDataFiles(allDataFiles);
    
    CaretDataFile* caretDataFile = reloadDataFileEvent->getCaretDataFile();
    CaretAssert(caretDataFile);
    
    if (std::find(allDataFiles.begin(),
                  allDataFiles.end(),
                  caretDataFile) == allDataFiles.end()) {
        reloadDataFileEvent->setErrorMessage("ERROR: "
                                             + caretDataFile->getFileNameNoPath()
                                             + " was not found as a loaded file.");
        return;
    }
    
    CaretDataFile::setFileReadingUsernameAndPassword(reloadDataFileEvent->getUsername(),
                                                     reloadDataFileEvent->getPassword());
    
    try {
        addReadOrReloadDataFile(FILE_MODE_RELOAD,
                             caretDataFile,
                             caretDataFile->getDataFileType(),
                             caretDataFile->getStructure(),
                             caretDataFile->getFileName(),
                             false);
    }
    catch (const DataFileException& dfe) {
        reloadDataFileEvent->setErrorMessage(dfe.whatString());
    }
    updateAfterFilesAddedOrRemoved();
}


/**
 * Process a read data file event.
 * @param readDataFileEvent
 *   Event describing file for reading and may be updated with error messages.
 */
void 
Brain::processReadDataFileEvent(EventDataFileRead* readDataFileEvent)
{
    CaretDataFile::setFileReadingUsernameAndPassword(readDataFileEvent->getUsername(),
                                                     readDataFileEvent->getPassword());
    
    const int32_t numberOfFilesToRead = readDataFileEvent->getNumberOfDataFilesToRead();
    EventProgressUpdate progressEvent(0,
                                      numberOfFilesToRead,
                                      0,
                                      "Starting to read data file(s)");
    EventManager::get()->sendEvent(progressEvent.getPointer());
    
    AString eventErrorMessage;
    for (int32_t i = 0; i < numberOfFilesToRead; i++) {
        const AString filename = readDataFileEvent->getDataFileName(i);
        const DataFileTypeEnum::Enum dataFileType = readDataFileEvent->getDataFileType(i);
        const StructureEnum::Enum structure = readDataFileEvent->getStructure(i);
        const bool setFileModifiedStatus = readDataFileEvent->isFileToBeMarkedModified(i);
        
        const AString shortName = FileInformation(filename).getFileName();
        progressEvent.setProgress(i,
                                  ("Reading " + shortName));
        EventManager::get()->sendEvent(progressEvent.getPointer());
        if (progressEvent.isCancelled()) {
            eventErrorMessage.appendWithNewLine("File reading cancelled.");
            break;
        }
        
        try {
            CaretDataFile* fileRead = readDataFile(dataFileType,
                         structure,
                         filename,
                         setFileModifiedStatus);
            readDataFileEvent->setDataFileRead(i,
                                               fileRead);
        }
        catch (const DataFileException& e) {
            if (e.isErrorInvalidStructure()) {
                readDataFileEvent->setFileErrorInvalidStructure(i,
                                                                e.isErrorInvalidStructure());
            }
            else {
                eventErrorMessage.appendWithNewLine(e.whatString());
                readDataFileEvent->setFileErrorMessage(i,
                                                       e.whatString());
            }
        }
    }
    
    readDataFileEvent->setErrorMessage(eventErrorMessage);
    
    CaretDataFile::setFileReadingUsernameAndPassword("",
                                                     "");
}

/**
 * Read, or reload a data file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param dataFileType
 *    Type of data file to read.
 * @param structure
 *    Struture of file (used if not invalid)
 * @param dataFileName
 *    Name of data file to read.
 * @param markDataFileAsModified
 *    If file has invalid structure and settings structure, mark file modified
 * @return 
 *    In some cases this will return a pointer to the file that was read so
 *    beware that this value may be NULL.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
CaretDataFile*
Brain::addReadOrReloadDataFile(const FileModeAddReadReload fileMode,
                            CaretDataFile* caretDataFile,
                            const DataFileTypeEnum::Enum dataFileType,
                            const StructureEnum::Enum structure,
                            const AString& dataFileName,
                            const bool markDataFileAsModified) throw (DataFileException)
{
    CaretDataFile* caretDataFileRead = NULL;

    switch (fileMode) {
        case FILE_MODE_ADD:
            CaretAssert(caretDataFile != NULL);
            break;
        case FILE_MODE_READ:
            CaretAssert(caretDataFile == NULL);
            break;
        case FILE_MODE_RELOAD:
            CaretAssert(caretDataFile != NULL);
            break;
    }
    
    try {
        
        ElapsedTimer et;
        et.start();
        
        switch (dataFileType) {
            case DataFileTypeEnum::BORDER:
                caretDataFileRead  = addReadOrReloadBorderFile(fileMode,
                                                   caretDataFile,
                                                   dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                caretDataFileRead  = addReadOrReloadConnectivityDenseFile(fileMode,
                                                              caretDataFile,
                                                              dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                caretDataFileRead  = addReadOrReloadConnectivityDenseLabelFile(fileMode,
                                                                   caretDataFile,
                                                                   dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                caretDataFileRead  = addReadOrReloadConnectivityMatrixDenseParcelFile(fileMode,
                                                                          caretDataFile,
                                                                          dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                caretDataFileRead  = addReadOrReloadConnectivityDenseScalarFile(fileMode,
                                                                    caretDataFile,
                                                                        dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                caretDataFileRead  = addReadOrReloadConnectivityDataSeriesFile(fileMode,
                                                                   caretDataFile,
                                                                   dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                caretDataFileRead  = addReadOrReloadConnectivityFiberOrientationFile(fileMode,
                                                                         caretDataFile,
                                                                         dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                caretDataFileRead  = addReadOrReloadConnectivityFiberTrajectoryFile(fileMode,
                                                                        caretDataFile,
                                                                        dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                caretDataFileRead  = addReadOrReloadConnectivityMatrixParcelFile(fileMode,
                                                                     caretDataFile,
                                                                     dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                caretDataFileRead  = addReadOrReloadConnectivityMatrixParcelDenseFile(fileMode,
                                                                          caretDataFile,
                                                                          dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                caretDataFileRead  = addReadOrReloadConnectivityParcelScalarFile(fileMode,
                                                                     caretDataFile,
                                                                     dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                caretDataFileRead  = addReadOrReloadConnectivityParcelSeriesFile(fileMode,
                                                                     caretDataFile,
                                                                         dataFileName);
                break;
            case DataFileTypeEnum::FOCI:
                caretDataFileRead  = addReadOrReloadFociFile(fileMode,
                                                 caretDataFile,
                                                 dataFileName);
                break;
            case DataFileTypeEnum::LABEL:
                caretDataFileRead  = addReadOrReloadLabelFile(fileMode,
                                                  caretDataFile,
                                                  dataFileName,
                                                  structure,
                                                  markDataFileAsModified);
                break;
            case DataFileTypeEnum::METRIC:
                caretDataFileRead  = addReadOrReloadMetricFile(fileMode,
                                                   caretDataFile,
                                                   dataFileName,
                                                   structure,
                                                   markDataFileAsModified);
                break;
            case DataFileTypeEnum::PALETTE:
                caretDataFileRead  = addReadOrReloadPaletteFile(fileMode,
                                                    caretDataFile,
                                                    dataFileName);
                break;
            case DataFileTypeEnum::RGBA:
                caretDataFileRead  = addReadOrReloadRgbaFile(fileMode,
                                                 caretDataFile,
                                                 dataFileName,
                                                 structure,
                                                 markDataFileAsModified);
                break;
            case DataFileTypeEnum::SCENE:
                caretDataFileRead  = addReadOrReloadSceneFile(fileMode,
                                                  caretDataFile,
                                                  dataFileName);
                break;
            case DataFileTypeEnum::SPECIFICATION:
                CaretLogSevere("PROGRAM ERROR: Reading spec file should never call Brain::addReadOrReloadDataFile()");
                throw DataFileException("PROGRAM ERROR: Reading spec file should never call Brain::addReadOrReloadDataFile()");
                break;
            case DataFileTypeEnum::SURFACE:
                caretDataFileRead  = addReadOrReloadSurfaceFile(fileMode,
                                                    caretDataFile,
                                                    dataFileName,
                                                    structure,
                                                    markDataFileAsModified);
                break;
            case DataFileTypeEnum::UNKNOWN:
                throw DataFileException("Unable to read files of type");
                break;
            case DataFileTypeEnum::VOLUME:
                caretDataFileRead  = addReadOrReloadVolumeFile(fileMode,
                                                   caretDataFile,
                                                   dataFileName);
                break;
        }
        
        if (caretDataFileRead != NULL) {
            m_specFile->addCaretDataFile(caretDataFileRead);
            
        }
        m_specFile->addDataFile(dataFileType,
                                structure,
                                dataFileName,
                                true,
                                false,
                                false);
        
        AString msg = ("Time to read "
                       + dataFileName
                       + " was "
                       + AString::number(et.getElapsedTimeSeconds())
                       + " seconds.");
        CaretLogInfo(msg);
    }
    catch (const DataFileException& dfe) {
        if (caretDataFile != NULL) {
            m_specFile->removeCaretDataFile(caretDataFile);
        }
        throw dfe;
    }
   
    updateAfterFilesAddedOrRemoved();
    
    return caretDataFileRead;
}

/**
 * Read a data file.
 *
 * @param fileMode
 *    Mode for file adding, reading, or reloading.
 * @param caretDataFile
 *    File that is added or reloaded (MUST NOT BE NULL).  If NULL,
 *    the mode must be READING.
 * @param dataFileType
 *    Type of data file to read.
 * @param structure
 *    Struture of file (used if not invalid)
 * @param dataFileNameIn
 *    Name of data file to read.
 * @param markDataFileAsModified
 *    If file has invalid structure and settings structure, mark file modified
 * @throws DataFileException
 *    If there is an error reading the file.
 * @return
 *    Pointer to file that was read, if no errors.
 */
CaretDataFile*
Brain::readDataFile(const DataFileTypeEnum::Enum dataFileType,
                    const StructureEnum::Enum structure,
                    const AString& dataFileNameIn,
                    const bool markDataFileAsModified) throw (DataFileException)
{
    AString dataFileName = dataFileNameIn;
    
    /*
     * If possible, update path so that is absolute
     */
    dataFileName = updateFileNameForReading(dataFileName);
    
    /*
     * Since file is being read, it must exist
     */
    if (DataFile::isFileOnNetwork(dataFileName) == false) {
        FileInformation fileInfoFullPath(dataFileName);
        if (fileInfoFullPath.exists() == false) {
            throw DataFileException(dataFileName
                                    + " does not exist!");
        }
    }
    
    CaretDataFile* caretDataFileRead = addReadOrReloadDataFile(FILE_MODE_READ,
                                                            NULL,
                                                            dataFileType,
                                                            structure,
                                                            dataFileName,
                                                            markDataFileAsModified);
    
    return caretDataFileRead;
}

/**
 * Processing performed after adding or removing a data file.
 */
void
Brain::updateAfterFilesAddedOrRemoved()
{
    updateChartModel();
    updateVolumeSliceModel();
    updateWholeBrainModel();
    updateSurfaceMontageModel();
    
    updateFiberTrajectoryMatchingFiberOrientationFiles();
}

/**
 * Load the data files selected in a spec file.
 * @param readSpecFileDataFilesEvent
 *    Event containing the spec file.
 */
void
Brain::loadFilesSelectedInSpecFile(EventSpecFileReadDataFiles* readSpecFileDataFilesEvent)
{
    ElapsedTimer timer;
    timer.start();
    
    AString errorMessage;
    
    const SpecFile* sf = readSpecFileDataFilesEvent->getSpecFile();
    CaretAssert(sf);

    resetBrain();
    
    try  {
        m_specFile->clear();
        *m_specFile = *sf;
    }
    catch (const DataFileException& e) {
        CaretLogSevere("SPEC FILE TODO: "
                       + e.whatString());
    }
    
    m_isSpecFileBeingRead = true;
    
    CaretDataFile::setFileReadingUsernameAndPassword(readSpecFileDataFilesEvent->getUsername(),
                                                     readSpecFileDataFilesEvent->getPassword());

    FileInformation fileInfo(sf->getFileName());
    setCurrentDirectory(fileInfo.getPathName());
    
    const int32_t numberOfFilesToRead = sf->getNumberOfFilesSelectedForLoading();
    int32_t fileReadCounter = 0;
    
    EventProgressUpdate progressUpdate(0,
                                       numberOfFilesToRead,
                                       fileReadCounter,
                                       "Starting to read selected files");
    EventManager::get()->sendEvent(progressUpdate.getPointer());

    /*
     * Note: Need to read palette first since some of the individual file
     * reading routines update palette coloring when file is read
     */
    const int32_t numFileGroups = sf->getNumberOfDataFileTypeGroups();
    for (int32_t ig = -1; ig < numFileGroups; ig++) {
        const SpecFileDataFileTypeGroup* group = ((ig == -1)
                                               ? sf->getDataFileTypeGroupByType(DataFileTypeEnum::PALETTE)
                                               : sf->getDataFileTypeGroupByIndex(ig));
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        if (ig >= 0) {
            if (dataFileType == DataFileTypeEnum::PALETTE) {
                continue;
            }
        }
        const int32_t numFiles = group->getNumberOfFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            const SpecFileDataFile* dataFileInfo = group->getFileInformation(iFile);
            if (dataFileInfo->isLoadingSelected()) {
                const AString filename = dataFileInfo->getFileName();
                const StructureEnum::Enum structure = dataFileInfo->getStructure();

                /*
                 * Send event indicating progress of file reading
                 */
                FileInformation fileInfo(dataFileInfo->getFileName());
                progressUpdate.setProgress(fileReadCounter,
                                           ("Reading "
                                            + fileInfo.getFileName()));
                EventManager::get()->sendEvent(progressUpdate.getPointer());
                
                /*
                 * If user cancelled, reset brain and get out!
                 */
                if (progressUpdate.isCancelled()) {
                    resetBrain();
                    return;
                }
                
                try {
                    readDataFile(dataFileType,
                                 structure,
                                 filename,
                                 false);
                }
                catch (const DataFileException& e) {
                    if (errorMessage.isEmpty() == false) {
                        errorMessage += "\n";
                    }
                    errorMessage += e.whatString();
                }
                
                fileReadCounter++;
            }
        }
    }
    
    m_specFile->clearModified();
    
    const AString specFileName = sf->getFileName();
    if (DataFile::isFileOnNetwork(specFileName)) {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->addToPreviousSpecFiles(specFileName);
    }
    else {
        FileInformation specFileInfo(specFileName);
        if (specFileInfo.exists()
            && specFileInfo.isAbsolute()) {
            CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
            prefs->addToPreviousSpecFiles(specFileName);
        }
    }
    
    if (errorMessage.isEmpty() == false) {
        readSpecFileDataFilesEvent->setErrorMessage(errorMessage);
    }
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileNameNoPath()));
    m_paletteFile->clearModified();
    
    /*
     * Reset the volume interaction surfaces since they can get set
     * incorrectly when loading files
     */
    for (std::vector<BrainStructure*>::iterator bsi = m_brainStructures.begin();
         bsi != m_brainStructures.end();
         bsi++) {
        BrainStructure* bs = *bsi;
        bs->setVolumeInteractionSurface(NULL);
    }
    
    /*
     * Initialize the overlay for ALL models
     */
    EventModelGetAll getAllModels;
    EventManager::get()->sendEvent(getAllModels.getPointer());
    std::vector<Model*> allModels = getAllModels.getModels();
    for (std::vector<Model*>::iterator iter = allModels.begin();
         iter != allModels.end();
         iter++) {
        Model* mdc = *iter;
        mdc->initializeSelectedSurfaces();
        mdc->initializeOverlays();
    }
    
    /*
     * Initialize overlays for brain structures
     */
    for (std::vector<BrainStructure*>::iterator iter = m_brainStructures.begin();
         iter != m_brainStructures.end();
         iter++) {
        BrainStructure* bs = *iter;
        bs->initializeOverlays();
    }
    
    CaretLogInfo("Time to read files from spec file (in Brain) \""
                 + sf->getFileNameNoPath()
                 + "\" was "
                 + AString::number(timer.getElapsedTimeSeconds())
                 + " seconds.");
    
    m_isSpecFileBeingRead = false;
    
    CaretDataFile::setFileReadingUsernameAndPassword("",
                                                     "");
}

/**
 * Load files from the given spec file.
 * @param specFileToLoad
 *    Spec file from which selected files are read.
 * @param keepSceneFiles
 *    Controls clearing of scene files
 * @param keepSpecFile
 *    Controls clearing of spec file
 */
void
Brain::loadSpecFileFromScene(const SceneAttributes* sceneAttributes,
                             SpecFile* specFileToLoad,
                    const ResetBrainKeepSceneFiles keepSceneFiles,
                    const ResetBrainKeepSpecFile keepSpecFile)
{
    CaretAssert(specFileToLoad);
    
    EventProgressUpdate progressEvent(-1,
                                      -1,
                                      -1,
                                      "Resetting brain");
    EventManager::get()->sendEvent(progressEvent.getPointer());
    
    resetBrainKeepSceneFiles();

    /*
     * Try to set to current directory
     */
    const AString previousSpecFileName = m_specFile->getFileName();
    delete m_specFile;
    m_specFile = new SpecFile(*specFileToLoad);
    FileInformation newSpecFileInfo(m_specFile->getFileName());
    if (newSpecFileInfo.isAbsolute()) {
        setCurrentDirectory(newSpecFileInfo.getPathName());
    }
    else {
        if (previousSpecFileName.endsWith(m_specFile->getFileName()) == false) {
            FileInformation oldSpecFileInfo(previousSpecFileName);
            setCurrentDirectory(oldSpecFileInfo.getPathName());
        }
    }
    
    /*
     * Check to see if existing spec file exists
     */
    FileInformation specFileInfo(specFileToLoad->getFileName());
    const bool specFileValid = specFileInfo.exists();
    
    /*
     * Apply spec file pulled from scene
     */
    m_isSpecFileBeingRead = true;
    
    /*
     * Set current directory to directory containing scene file
     * but only if there is NOT a valid spec file
     */
    const AString sceneFileName = sceneAttributes->getSceneFileName();
    const bool sceneFileOnNetwork = DataFile::isFileOnNetwork(sceneFileName);
    if (specFileValid == false) {
        FileInformation sceneFileInfo(sceneFileName);
        if (sceneFileInfo.exists()) {
            setCurrentDirectory(sceneFileInfo.getPathName());
        }
    }
    
    progressEvent.setProgressMessage("Loading data files");
    EventManager::get()->sendEvent(progressEvent.getPointer());
    if (progressEvent.isCancelled()) {
        resetBrain(keepSceneFiles,
                   keepSpecFile);
        return;
    }
    
    /*
     * To speed file loading, non-modified files that were in memory
     * prior to restoring the scene are saved.  This map matches
     * an entry of a selected file to one of the non-modified in 
     * memory data files.
     */
    std::map<const SpecFileDataFile*, CaretDataFile*> specFilesEntryToNonModifiedFile;
    
    /*
     * Find non-modified files that match, by name, files that are to be
     * loaded from the spec file and associate them for later use.
     */
    if ( ! m_nonModifiedFilesForRestoringScene.empty()) {
        const int32_t numFileGroups = specFileToLoad->getNumberOfDataFileTypeGroups();
        for (int32_t ig = 0; ig < numFileGroups; ig++) {
            const SpecFileDataFileTypeGroup* group = specFileToLoad->getDataFileTypeGroupByIndex(ig);
            const int32_t numFiles = group->getNumberOfFiles();
            for (int32_t iFile = 0; iFile < numFiles; iFile++) {
                const SpecFileDataFile* fileInfo = group->getFileInformation(iFile);
                if (fileInfo->isLoadingSelected()) {
                    AString filename = fileInfo->getFileName();
                    
                    for (std::vector<CaretDataFile*>::iterator iter = m_nonModifiedFilesForRestoringScene.begin();
                         iter != m_nonModifiedFilesForRestoringScene.end();
                         iter++) {
                        CaretDataFile* caretDataFile = *iter;
                        if (caretDataFile != NULL) {
                            const AString nonModifiedFileName = caretDataFile->getFileName();
                            if (nonModifiedFileName == filename) {
                                specFilesEntryToNonModifiedFile.insert(std::make_pair(fileInfo,
                                                                               caretDataFile));
                                *iter = NULL;
                                CaretLogFine("Scene loading matched previous file: "
                                             + filename);
                            }
                        }
                    }
                }
            }
        }
    }

    /*
     * Delete any of the files that were in memory prior to loading the scene
     * that are not part of the scene being loaded.
     */
    for (std::vector<CaretDataFile*>::iterator iter = m_nonModifiedFilesForRestoringScene.begin();
         iter != m_nonModifiedFilesForRestoringScene.end();
         iter++) {
        CaretDataFile* caretDataFile = *iter;
        if (caretDataFile != NULL) {
            CaretLogFine("Scene loading removing previous file not needed: "
                         + caretDataFile->getFileName());
            delete caretDataFile;
        }
    }
    m_nonModifiedFilesForRestoringScene.clear();
    
    
    /*
     * Load new files and add existing files that were previously loaded.
     */
    const int32_t numFileGroups = specFileToLoad->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0; ig < numFileGroups; ig++) {
        const SpecFileDataFileTypeGroup* group = specFileToLoad->getDataFileTypeGroupByIndex(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        const int32_t numFiles = group->getNumberOfFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            const SpecFileDataFile* fileInfo = group->getFileInformation(iFile);
            if (fileInfo->isLoadingSelected()) {
                try {
                    
                    AString filename = fileInfo->getFileName();
                    
                    std::map<const SpecFileDataFile*, CaretDataFile*>::iterator specToFileIter = specFilesEntryToNonModifiedFile.find(fileInfo);
                    if (specToFileIter != specFilesEntryToNonModifiedFile.end()) {
                        const QString msg = ("Adding previous file "
                                             + FileInformation(filename).getFileName());
                        progressEvent.setProgressMessage(msg);
                        EventManager::get()->sendEvent(progressEvent.getPointer());
                        if (progressEvent.isCancelled()) {
                            resetBrain(keepSceneFiles,
                                       keepSpecFile);
                            return;
                        }
                        
                        CaretDataFile* caretDataFile = specToFileIter->second;
                        addReadOrReloadDataFile(FILE_MODE_ADD,
                                                caretDataFile,
                                                caretDataFile->getDataFileType(),
                                                caretDataFile->getStructure(),
                                                filename,
                                                false);
                    }
                    else {
                        const StructureEnum::Enum structure = fileInfo->getStructure();
                        
                        const QString msg = ("Loading "
                                             + FileInformation(filename).getFileName());
                        progressEvent.setProgressMessage(msg);
                        EventManager::get()->sendEvent(progressEvent.getPointer());
                        if (progressEvent.isCancelled()) {
                            resetBrain(keepSceneFiles,
                                       keepSpecFile);
                            return;
                        }
                        
                        if (sceneFileOnNetwork) {
                            if (DataFile::isFileOnNetwork(filename) == false) {
                                const int32_t lastSlashIndex = sceneFileName.lastIndexOf("/");
                                if (lastSlashIndex >= 0) {
                                    const AString newName = (sceneFileName.left(lastSlashIndex)
                                                             + "/"
                                                             + filename);
                                    filename = newName;
                                }
                            }
                        }
                        readDataFile(dataFileType,
                                     structure,
                                     filename,
                                     false);
                    }
                }
                catch (const DataFileException& e) {
                    sceneAttributes->addToErrorMessage(e.whatString());
                }
            }
        }
    }
    
    if (m_paletteFile != NULL) {
        delete m_paletteFile;
    }
    m_paletteFile = new PaletteFile();
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileNameNoPath()));
    m_paletteFile->clearModified();
    
    progressEvent.setProgressMessage("Initializing Overlays");
    EventManager::get()->sendEvent(progressEvent.getPointer());
    if (progressEvent.isCancelled()) {
        resetBrain(keepSceneFiles,
                   keepSpecFile);
        return;
    }
    
    /*
     * Initialize the overlay for ALL models
     */
    EventModelGetAll getAllModels;
    EventManager::get()->sendEvent(getAllModels.getPointer());
    std::vector<Model*> allModels = getAllModels.getModels();
    for (std::vector<Model*>::iterator iter = allModels.begin();
         iter != allModels.end();
         iter++) {
        Model* mdc = *iter;
        mdc->initializeOverlays();
    }
    
    /*
     * Initialize overlays for brain structures
     */
    for (std::vector<BrainStructure*>::iterator iter = m_brainStructures.begin();
         iter != m_brainStructures.end();
         iter++) {
        BrainStructure* bs = *iter;
        bs->initializeOverlays();
    }
}


/**
 * Update the filename for writing so that it is an absolute path.
 * 
 * @param filename
 *    Name of file.
 * @return
 *    Name prepended with path, if availble, and if 
 *    the input filename was a relative path.
 */
AString 
Brain::updateFileNameForReading(const AString& filename)
{
    /*
     * If file is on network, leave it unchanged
     */
    if (DataFile::isFileOnNetwork(filename)) {
        return filename;
    }
    
    FileInformation fileInfo(filename);
    if (fileInfo.isAbsolute()) {
        return filename;
    }

    if (m_currentDirectory.isEmpty()) {
        return filename;
    }
    
    FileInformation pathFileInfo(m_currentDirectory, filename);
    AString fullPathName = pathFileInfo.getAbsoluteFilePath();
    
    return fullPathName;
}

/**
 * Update the filename for writing so that it is an absolute path.
 * 
 * @param filename
 *    Name of file.
 * @return
 *    Name prepended with path, if availble, and if 
 *    the input filename was a relative path.
 */
AString 
Brain::updateFileNameForWriting(const AString& filename) throw (DataFileException)
{
    /*
     * If file is on network, leave it unchanged
     */
    if (DataFile::isFileOnNetwork(filename)) {
        throw DataFileException("Cannot write file on network: " 
                                + filename);
    }
    
    FileInformation fileInfo(filename);
    if (fileInfo.isAbsolute()) {
        return filename;
    }
    
    if (m_currentDirectory.isEmpty()) {
        return filename;
    }
    
    FileInformation pathFileInfo(m_currentDirectory, filename);
    AString fullPathName = pathFileInfo.getAbsoluteFilePath();
    
    return fullPathName;
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
Brain::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_ADD) {
        EventDataFileAdd* addDataFileEvent =
            dynamic_cast<EventDataFileAdd*>(event);
        CaretAssert(addDataFileEvent);
        
        try {
            addDataFile(addDataFileEvent->getCaretDataFile());
        }
        catch (const DataFileException& dfe) {
            addDataFileEvent->setErrorMessage(dfe.whatString());
        }
        addDataFileEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_DELETE) {
        EventDataFileDelete* deleteDataFileEvent =
        dynamic_cast<EventDataFileDelete*>(event);
        CaretAssert(deleteDataFileEvent);
        
        removeAndDeleteDataFile(deleteDataFileEvent->getCaretDataFile());

        deleteDataFileEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_READ) {
        EventDataFileRead* readDataFileEvent =
             dynamic_cast<EventDataFileRead*>(event);
        CaretAssert(readDataFileEvent);
        
        
        /*
         * Make sure event is for this brain
         */
        if (readDataFileEvent->getLoadIntoBrain() == this) {
            readDataFileEvent->setEventProcessed();
            processReadDataFileEvent(readDataFileEvent);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_RELOAD) {
        EventDataFileReload* reloadDataFileEvent =
        dynamic_cast<EventDataFileReload*>(event);
        CaretAssert(reloadDataFileEvent);
        
        if (reloadDataFileEvent->getBrain() == this) {
            reloadDataFileEvent->setEventProcessed();
            processReloadDataFileEvent(reloadDataFileEvent);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET) {
        EventCaretMappableDataFilesGet* dataFilesEvent =
        dynamic_cast<EventCaretMappableDataFilesGet*>(event);
        CaretAssert(dataFilesEvent);
        
        std::vector<CaretMappableDataFile*> allCaretMappableFiles;
        getAllMappableDataFiles(allCaretMappableFiles);
        
        for (std::vector<CaretMappableDataFile*>::iterator iter = allCaretMappableFiles.begin();
             iter != allCaretMappableFiles.end();
             iter++) {
            dataFilesEvent->addFile(*iter);
        }
        
        dataFilesEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES) {
        EventSpecFileReadDataFiles* readSpecFileDataFilesEvent =
        dynamic_cast<EventSpecFileReadDataFiles*>(event);
        CaretAssert(readSpecFileDataFilesEvent);
        
        
        /*
         * Make sure event is for this brain
         */
        if (readSpecFileDataFilesEvent->getLoadIntoBrain() == this) {
            readSpecFileDataFilesEvent->setEventProcessed();
            loadFilesSelectedInSpecFile(readSpecFileDataFilesEvent);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_DISPLAYED_DATA_FILES) {
        EventGetDisplayedDataFiles* displayedFilesEvent = dynamic_cast<EventGetDisplayedDataFiles*>(event);
        CaretAssert(displayedFilesEvent);
        
        /*
         * Get all visible browser tabs.
         */
        EventBrowserTabGetAll getAllTabsEvent;
        EventManager::get()->sendEvent(getAllTabsEvent.getPointer());
        
        
        std::set<const CaretDataFile*> dataFilesDisplayedInTabs;
        
        /*
         * Get files displayed in each tab.
         */
        const int32_t numberOfTabs = getAllTabsEvent.getNumberOfBrowserTabs();
        for (int32_t i = 0; i < numberOfTabs; i++) {
            BrowserTabContent* btc = getAllTabsEvent.getBrowserTab(i);
            const int32_t tabIndex = btc->getTabNumber();
            if (displayedFilesEvent->isTestForDisplayedDataFileInTabIndex(tabIndex)) {
                std::vector<CaretDataFile*> tabDataFiles;
                btc->getFilesDisplayedInTab(tabDataFiles);
                
                dataFilesDisplayedInTabs.insert(tabDataFiles.begin(),
                                                tabDataFiles.end());
            }
        }
        
        /*
         * See if any palette mappable files are displayed
         */
        bool havePaletteMappableFiles = false;
        for (std::set<const CaretDataFile*>::const_iterator iter = dataFilesDisplayedInTabs.begin();
             iter != dataFilesDisplayedInTabs.end();
             iter++) {
            const CaretMappableDataFile* mappableFile = dynamic_cast<const CaretMappableDataFile*>(*iter);
            if (mappableFile != NULL) {
                if (mappableFile->isMappedWithPalette()) {
                    havePaletteMappableFiles = true;
                    break;
                }
            }
        }
        
        /*
         * If there are ANY palette mappable data files, add the 
         * palette file.
         */
        if (havePaletteMappableFiles) {
            dataFilesDisplayedInTabs.insert(m_paletteFile);
        }
        
        for (std::set<const CaretDataFile*>::const_iterator iter = dataFilesDisplayedInTabs.begin();
             iter != dataFilesDisplayedInTabs.end();
             iter++) {
            displayedFilesEvent->addDisplayedDataFile(*iter);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_PALETTE_GET_BY_NAME) {
        EventPaletteGetByName* paletteGetByName = dynamic_cast<EventPaletteGetByName*>(event);
        CaretAssert(paletteGetByName);
        
        if (m_paletteFile != NULL) {
            Palette* palette = m_paletteFile->getPaletteByName(paletteGetByName->getPaletteName());
            if (palette != NULL) {
                paletteGetByName->setPalette(palette);
                paletteGetByName->setEventProcessed();
            }
        }
    }
}

/**
 * @return The chart model (warning may be NULL!)
 */
ModelChart*
Brain::getChartModel()
{
    return m_modelChart;
}

/**
 * @return The chart model (warning may be NULL!)
 */
const ModelChart*
Brain::getChartModel() const
{
    return m_modelChart;
}


/**
 * @return The charting data manager.
 */
ChartingDataManager*
Brain::getChartingDataManager()
{
    return m_chartingDataManager;
}

/**
 * @return The charting data manager.
 */
const ChartingDataManager*
Brain::getChartingDataManager() const
{
    return m_chartingDataManager;
}

/**
 * @return  The current directory.
 */
AString 
Brain::getCurrentDirectory() const
{
    if (m_currentDirectory.isEmpty()) {
        m_currentDirectory = SystemUtilities::systemCurrentDirectory();
    }
    return m_currentDirectory;
}

/**
 * Set the current directory.
 * @param currentDirectory
 *    New value for current directory.
 */
void 
Brain::setCurrentDirectory(const AString& currentDirectory)
{
    m_currentDirectory = currentDirectory;
}

/**
 * Get All CaretMappableDataFiles.
 *
 * @param allCaretMappableDataFilesOut
 *   Will contain instance of CaretMappableDataFiles upon exit. 
 */
void
Brain::getAllMappableDataFiles(std::vector<CaretMappableDataFile*>& allCaretMappableDataFilesOut) const
{
    allCaretMappableDataFilesOut.clear();
    
    std::vector<CaretDataFile*> allDataFiles;
    getAllDataFiles(allDataFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allDataFiles.begin();
         iter != allDataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(cdf);
        if (cmdf != NULL) {
            allCaretMappableDataFilesOut.push_back(cmdf);
        }
    }
}
    
/**
 * Get All CaretMappableDataFiles of the given data file type.
 *
 * @param caretMappableDataFilesOut
 *   Contains CaretMappableDataFiles matching data file type upon exit.
 */
void
Brain::getAllMappableDataFileWithDataFileType(const DataFileTypeEnum::Enum dataFileType,
                                              std::vector<CaretMappableDataFile*>& caretMappableDataFilesOut) const
{
    caretMappableDataFilesOut.clear();
    
    std::vector<CaretMappableDataFile*> allFiles;
    getAllMappableDataFiles(allFiles);
    
    for (std::vector<CaretMappableDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        CaretMappableDataFile* cmdf = *iter;
        if (cmdf->getDataFileType() == dataFileType) {
            caretMappableDataFilesOut.push_back(cmdf);
        }
    }
}

/**
 * Get all CaretDataFiles of the given data file type.
 *
 * @param dataFileType
 *     Type of data file.
 * @param caretDataFilesOut
 *     Data file of the given data file type that were found.
 */
void
Brain::getAllDataFilesWithDataFileType(const DataFileTypeEnum::Enum dataFileType,
                                     std::vector<CaretDataFile*>& caretDataFilesOut) const
{
    
    std::vector<CaretDataFile*> allDataFiles;
    getAllDataFiles(allDataFiles,
                    true);
    
    for (std::vector<CaretDataFile*>::iterator iter = allDataFiles.begin();
         iter != allDataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        if (cdf->getDataFileType() == dataFileType) {
            caretDataFilesOut.push_back(cdf);
        }
    }
    
    caretDataFilesOut.clear();
}


/**
 * Get all loaded data files.
 * @param allDataFilesOut
 *    Data files are loaded into this parameter.
 * @param includeSpecFile
 *    If true, the spec file is included as the first file.
 */
void 
Brain::getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut,
                       const bool includeSpecFile) const
{
    allDataFilesOut.clear();
    
    if (includeSpecFile) {
        if (m_specFile->isEmpty() == false) {
            allDataFilesOut.push_back(m_specFile);
        }
    }
    
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        getBrainStructure(i)->getAllDataFiles(allDataFilesOut);
    }
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_borderFiles.begin(),
                           m_borderFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_fociFiles.begin(),
                           m_fociFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityDenseScalarFiles.begin(),
                           m_connectivityDenseScalarFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityMatrixDenseFiles.begin(),
                           m_connectivityMatrixDenseFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityDataSeriesFiles.begin(),
                           m_connectivityDataSeriesFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityDenseLabelFiles.begin(),
                           m_connectivityDenseLabelFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityMatrixDenseParcelFiles.begin(),
                           m_connectivityMatrixDenseParcelFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityFiberOrientationFiles.begin(),
                           m_connectivityFiberOrientationFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityFiberTrajectoryFiles.begin(),
                           m_connectivityFiberTrajectoryFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityMatrixParcelFiles.begin(),
                           m_connectivityMatrixParcelFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityMatrixParcelDenseFiles.begin(),
                           m_connectivityMatrixParcelDenseFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityParcelScalarFiles.begin(),
                           m_connectivityParcelScalarFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityParcelSeriesFiles.begin(),
                           m_connectivityParcelSeriesFiles.end());
    
    allDataFilesOut.push_back(m_paletteFile);
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                              m_sceneFiles.begin(),
                              m_sceneFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_volumeFiles.begin(),
                           m_volumeFiles.end());    
}

/**
 * Determine if a file is still valid (pointer is for an existing data
 * of the same DataFileType.
 */
bool
Brain::isFileValid(const CaretDataFile* caretDataFile) const
{
    std::vector<CaretDataFile*> allDataFiles;
    getAllDataFiles(allDataFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allDataFiles.begin();
         iter != allDataFiles.end();
         iter++) {
        const CaretDataFile* cdf = *iter;
        if (caretDataFile == cdf) {
            if (caretDataFile->getDataFileType() == cdf->getDataFileType()) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * Get all of the modified files excluding the given data file types.
 *
 * @param excludeTheseDataTypes
 *    Data types of files that excluded.
 * @param modifiedDataFilesOut
 *    Output containing the modified files.
 *
 */
void
Brain::getAllModifiedFiles(const std::vector<DataFileTypeEnum::Enum>& excludeTheseDataTypes,
                           std::vector<CaretDataFile*>& modifiedDataFilesOut) const
{
    modifiedDataFilesOut.clear();
    
    if (std::find(excludeTheseDataTypes.begin(),
                  excludeTheseDataTypes.end(),
                  DataFileTypeEnum::SPECIFICATION) == excludeTheseDataTypes.end()) {
        if (m_specFile->isModified()) {
            modifiedDataFilesOut.push_back(m_specFile);
        }
    }
    
    std::vector<CaretDataFile*> dataFiles;
    getAllDataFiles(dataFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = dataFiles.begin();
         iter != dataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        
        /**
         * Ignore files whose data type is excluded.
         */
        if (std::find(excludeTheseDataTypes.begin(),
                      excludeTheseDataTypes.end(),
                      cdf->getDataFileType()) == excludeTheseDataTypes.end()) {
            if (cdf->isModified()) {
                modifiedDataFilesOut.push_back(cdf);
            }
        }
    }
}


///**
// * Are any data files modified (including spec file)?
// * @param excludeTheseDataTypes
// *    Do not check the modification status of any data files whose
// *    data type is contained in this parameter.
// */
//bool
//Brain::areFilesModified(const std::vector<DataFileTypeEnum::Enum>& excludeTheseDataTypes)
//{
//    if (std::find(excludeTheseDataTypes.begin(),
//                  excludeTheseDataTypes.end(),
//                  DataFileTypeEnum::SPECIFICATION) == excludeTheseDataTypes.end()) {
//        if (m_specFile->isModified()) {
//            return true;
//        }
//    }
//    
//    std::vector<CaretDataFile*> dataFiles;
//    getAllDataFiles(dataFiles);
//    
//    for (std::vector<CaretDataFile*>::iterator iter = dataFiles.begin();
//         iter != dataFiles.end();
//         iter++) {
//        CaretDataFile* cdf = *iter;
//        
//        /**
//         * Ignore files whose data type is excluded.
//         */
//        if (std::find(excludeTheseDataTypes.begin(),
//                      excludeTheseDataTypes.end(),
//                      cdf->getDataFileType()) == excludeTheseDataTypes.end()) {
//            if (cdf->isModified()) {
//                return true;
//            }
//        }
//    }
//    
//    return false;
//}


/**
 * Write a data file.
 * @param caretDataFile
 *    Data file to write.
 * @return
 *    true if file was written, else false.
 * @throw
 *    DataFileException if there was an error writing the file.
 */
void 
Brain::writeDataFile(CaretDataFile* caretDataFile) throw (DataFileException)
{
    /*
     * If file is relative path, update path using current directory
     */
    AString dataFileName = caretDataFile->getFileName();
    dataFileName = updateFileNameForWriting(dataFileName);
    caretDataFile->setFileName(dataFileName);

    caretDataFile->writeFile(caretDataFile->getFileName());
    caretDataFile->clearModified();
}

/**
 * Remove the data file from memory but DO NOT delete it.
 *
 * @param caretDataFile
 *    Caret file that is removed from the Brain.  After calling this method
 *    and the file was removed( true was returned), the caller is responsible
 *    for deleting the file when it is no longer needed.
 * @return
 *    True if the file was removed, else false.
 */
bool
Brain::removeWithoutDeleteDataFile(const CaretDataFile* caretDataFile)
{
    if (caretDataFile == NULL) {
        return false;
    }
    
    const bool wasRemoved = removeWithoutDeleteDataFilePrivate(caretDataFile);
    
    if (wasRemoved) {
        m_specFile->removeCaretDataFile(caretDataFile);
        
        updateAfterFilesAddedOrRemoved();
    }
    else {
        CaretLogSevere("Software bug: failed to remove file type="
                       + DataFileTypeEnum::toName(caretDataFile->getDataFileType())
                       + " name="
                       + caretDataFile->getFileName());
    }
    
    return wasRemoved;
}

/**
 * Remove the data file from memory but DO NOT delete it.
 *
 * @param caretDataFile
 *    Caret file that is removed from the Brain.  After calling this method
 *    and the file was removed( true was returned), the caller is responsible
 *    for deleting the file when it is no longer needed.
 * @return
 *    True if the file was removed, else false.
 */
bool
Brain::removeWithoutDeleteDataFilePrivate(const CaretDataFile* caretDataFile)
{
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        if (getBrainStructure(i)->removeWithoutDeleteDataFile(caretDataFile)) {
            return true;
        }
    }
    
    std::vector<BorderFile*>::iterator borderIterator = std::find(m_borderFiles.begin(),
                                                                  m_borderFiles.end(),
                                                                  caretDataFile);
    if (borderIterator != m_borderFiles.end()) {
        m_borderFiles.erase(borderIterator);
        return true;
    }
    
    std::vector<CiftiBrainordinateDataSeriesFile*>::iterator dataSeriesIterator = std::find(m_connectivityDataSeriesFiles.begin(),
                                                                                            m_connectivityDataSeriesFiles.end(),
                                                                                            caretDataFile);
    if (dataSeriesIterator != m_connectivityDataSeriesFiles.end()) {
        m_connectivityDataSeriesFiles.erase(dataSeriesIterator);
        return true;
    }
    
    std::vector<CiftiBrainordinateLabelFile*>::iterator connLabelIterator = std::find(m_connectivityDenseLabelFiles.begin(),
                                                                                      m_connectivityDenseLabelFiles.end(),
                                                                                      caretDataFile);
    if (connLabelIterator != m_connectivityDenseLabelFiles.end()) {
        m_connectivityDenseLabelFiles.erase(connLabelIterator);
        return true;
    }
    
    std::vector<CiftiConnectivityMatrixDenseFile*>::iterator connDenseIterator = std::find(m_connectivityMatrixDenseFiles.begin(),
                                                                                           m_connectivityMatrixDenseFiles.end(),
                                                                                           caretDataFile);
    if (connDenseIterator != m_connectivityMatrixDenseFiles.end()) {
        m_connectivityMatrixDenseFiles.erase(connDenseIterator);
        return true;
    }
    
    std::vector<CiftiConnectivityMatrixDenseParcelFile*>::iterator connDenseParcelIterator = std::find(m_connectivityMatrixDenseParcelFiles.begin(),
                                                                                                       m_connectivityMatrixDenseParcelFiles.end(),
                                                                                                       caretDataFile);
    if (connDenseParcelIterator != m_connectivityMatrixDenseParcelFiles.end()) {
        m_connectivityMatrixDenseParcelFiles.erase(connDenseParcelIterator);
        return true;
    }
    
    std::vector<CiftiBrainordinateScalarFile*>::iterator connScalarIterator = std::find(m_connectivityDenseScalarFiles.begin(),
                                                                                        m_connectivityDenseScalarFiles.end(),
                                                                                        caretDataFile);
    if (connScalarIterator != m_connectivityDenseScalarFiles.end()) {
        m_connectivityDenseScalarFiles.erase(connScalarIterator);
        return true;
    }
    
    std::vector<CiftiParcelSeriesFile*>::iterator connParcelSeriesIterator = std::find(m_connectivityParcelSeriesFiles.begin(),
                                                                                       m_connectivityParcelSeriesFiles.end(),
                                                                                       caretDataFile);
    if (connParcelSeriesIterator != m_connectivityParcelSeriesFiles.end()) {
        m_connectivityParcelSeriesFiles.erase(connParcelSeriesIterator);
        return true;
    }
    
    std::vector<CiftiParcelScalarFile*>::iterator connParcelScalarIterator = std::find(m_connectivityParcelScalarFiles.begin(),
                                                                                       m_connectivityParcelScalarFiles.end(),
                                                                                       caretDataFile);
    if (connParcelScalarIterator != m_connectivityParcelScalarFiles.end()) {
        m_connectivityParcelScalarFiles.erase(connParcelScalarIterator);
        return true;
    }
    
    
    std::vector<CiftiFiberOrientationFile*>::iterator connFiberOrientationIterator = std::find(m_connectivityFiberOrientationFiles.begin(),
                                                                                               m_connectivityFiberOrientationFiles.end(),
                                                                                               caretDataFile);
    if (connFiberOrientationIterator != m_connectivityFiberOrientationFiles.end()) {
        m_connectivityFiberOrientationFiles.erase(connFiberOrientationIterator);
        return true;
    }
    
    std::vector<CiftiFiberTrajectoryFile*>::iterator connFiberTrajectoryIterator = std::find(m_connectivityFiberTrajectoryFiles.begin(),
                                                                                             m_connectivityFiberTrajectoryFiles.end(),
                                                                                             caretDataFile);
    if (connFiberTrajectoryIterator != m_connectivityFiberTrajectoryFiles.end()) {
        m_connectivityFiberTrajectoryFiles.erase(connFiberTrajectoryIterator);
        return true;
    }
    
    std::vector<CiftiConnectivityMatrixParcelFile*>::iterator connParcelIterator = std::find(m_connectivityMatrixParcelFiles.begin(),
                                                                                             m_connectivityMatrixParcelFiles.end(),
                                                                                             caretDataFile);
    if (connParcelIterator != m_connectivityMatrixParcelFiles.end()) {
        m_connectivityMatrixParcelFiles.erase(connParcelIterator);
        return true;
    }
    
    std::vector<CiftiConnectivityMatrixParcelDenseFile*>::iterator connParcelDenseIterator = std::find(m_connectivityMatrixParcelDenseFiles.begin(),
                                                                                                       m_connectivityMatrixParcelDenseFiles.end(),
                                                                                                       caretDataFile);
    if (connParcelDenseIterator != m_connectivityMatrixParcelDenseFiles.end()) {
        m_connectivityMatrixParcelDenseFiles.erase(connParcelDenseIterator);
        return true;
    }
    
    std::vector<FociFile*>::iterator fociIterator = std::find(m_fociFiles.begin(),
                                                              m_fociFiles.end(),
                                                              caretDataFile);
    if (fociIterator != m_fociFiles.end()) {
        m_fociFiles.erase(fociIterator);
        return true;
    }
    
    if (m_paletteFile == caretDataFile) {
        if (m_paletteFile != NULL) {
            CaretLogSevere("Cannot remove PaletteFile at this time.");
        }
    }
    
    std::vector<SceneFile*>::iterator sceneIterator = std::find(m_sceneFiles.begin(),
                                                                m_sceneFiles.end(),
                                                                caretDataFile);
    if (sceneIterator != m_sceneFiles.end()) {
        m_sceneFiles.erase(sceneIterator);
        return true;
    }
    
    std::vector<VolumeFile*>::iterator volumeIterator =
    std::find(m_volumeFiles.begin(),
              m_volumeFiles.end(),
              caretDataFile);
    if (volumeIterator != m_volumeFiles.end()) {
        m_volumeFiles.erase(volumeIterator);
        return true;
    }
    
    return false;
}


/**
 * Remove AND DELETE a data file from memory (does NOT delete file on disk.)
 * Searches all of the loaded files for given file, and, when found
 * deletes the file.
 *
 * @param caretDataFile
 *    Data file to remove.  After calling this method and the file was 
 *    deleted (true was returned) this pointer is no longer valid.
 * @return
 *    true if file was removed, else false.
 */
bool 
Brain::removeAndDeleteDataFile(CaretDataFile* caretDataFile)
{
    if (removeWithoutDeleteDataFile(caretDataFile)) {
        delete caretDataFile;
        return true;
    }
    
    return false;
}

/**
 * @return The border display properties.
 */
DisplayPropertiesBorders* 
Brain::getDisplayPropertiesBorders()
{
    return m_displayPropertiesBorders;
}

/**
 * @return The border display properties.
 */
const DisplayPropertiesBorders* 
Brain::getDisplayPropertiesBorders() const
{
    return m_displayPropertiesBorders;
}

/**
 * @return The fiber orientation display properties.
 */
DisplayPropertiesFiberOrientation*
Brain::getDisplayPropertiesFiberOrientation()
{
    return m_displayPropertiesFiberOrientation;
}

/**
 * @return The fiber orientation display properties.
 */
const DisplayPropertiesFiberOrientation*
Brain::getDisplayPropertiesFiberOrientation() const
{
    return m_displayPropertiesFiberOrientation;
}

/**
 * @return The foci display properties.
 */
DisplayPropertiesFoci* 
Brain::getDisplayPropertiesFoci()
{
    return m_displayPropertiesFoci;
}

/**
 * @return The foci display properties.
 */
const DisplayPropertiesFoci*
Brain::getDisplayPropertiesFoci() const
{
    return m_displayPropertiesFoci;
}

/**
 * @return The label display properties.
 */
DisplayPropertiesLabels*
Brain::getDisplayPropertiesLabels()
{
    return m_displayPropertiesLabels;
}

/**
 * @return The label display properties.
 */
const DisplayPropertiesLabels* 
Brain::getDisplayPropertiesLabels() const
{
    return m_displayPropertiesLabels;
}

/**
 * @return The volume display properties.
 */
DisplayPropertiesVolume*
Brain::getDisplayPropertiesVolume()
{
    return m_displayPropertiesVolume;
}

/**
 * @return The volume display properties.
 */
const DisplayPropertiesVolume*
Brain::getDisplayPropertiesVolume() const
{
    return m_displayPropertiesVolume;
}

/**
 * @return The surface display properties.
 */
DisplayPropertiesSurface*
Brain::getDisplayPropertiesSurface()
{
    return m_displayPropertiesSurface;
}

/**
 * @return The volume display properties.
 */
const DisplayPropertiesSurface*
Brain::getDisplayPropertiesSurface() const
{
    return m_displayPropertiesSurface;
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
Brain::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    bool isSaveSpecFile = false;
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            isSaveSpecFile = true;
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Brain",
                                            1);
    
    /*
     * Get all data files
     */
    std::vector<CaretDataFile*> allCaretDataFiles;
    getAllDataFiles(allCaretDataFiles);
    
    /*
     * Save data files into an array.
     * Note that data file's saveToScene returns NULL if no data for saving.
     */
    std::vector<SceneClass*> allCaretDataFileScenes;
    for (std::vector<CaretDataFile*>::iterator iter = allCaretDataFiles.begin();
         iter != allCaretDataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        const AString caretDataFileName = cdf->getFileNameNoPath();
        SceneClass* caretDataFileSceneClass = cdf->saveToScene(sceneAttributes,
                                                      caretDataFileName);
        if (caretDataFileSceneClass != NULL) {
            allCaretDataFileScenes.push_back(caretDataFileSceneClass);
        }
    }
    if (allCaretDataFileScenes.empty() == false) {
        SceneClassArray* caretDataFileSceneArray = new SceneClassArray("allCaretDataFiles",
                                                                       allCaretDataFileScenes);
        sceneClass->addChild(caretDataFileSceneArray);
    }
    
    if (isSaveSpecFile) {
        SpecFile sf;
        sf.setFileName(m_specFile->getFileName());
        for (std::vector<CaretDataFile*>::iterator iter = allCaretDataFiles.begin();
             iter != allCaretDataFiles.end();
             iter++) {
            CaretDataFile* cdf = *iter;
            sf.addCaretDataFile(cdf);
        }
        
        sceneClass->addClass(sf.saveToScene(sceneAttributes, 
                                            "specFile"));
    }
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    

    /*
     * Save all models
     */
    std::vector<SceneClass*> modelClassVector;
    EventModelGetAll getAllModels;
    EventManager::get()->sendEvent(getAllModels.getPointer());
    std::vector<Model*> allModels = getAllModels.getModels();
    for (std::vector<Model*>::iterator iter = allModels.begin();
         iter != allModels.end();
         iter++) {
        Model* mdc = *iter;
        modelClassVector.push_back(mdc->saveToScene(sceneAttributes,
                                                    "models"));
    }
    SceneClassArray* modelsClassArray = new SceneClassArray("models",
                                                            modelClassVector);
    sceneClass->addChild(modelsClassArray);
    
    /*
     * Save all brain structures
     */
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    SceneClassArray* brainStructureClassArray = new SceneClassArray("m_brainStructures",
                                                                    numBrainStructures);
    for (int32_t i = 0; i < numBrainStructures; i++) {
        const AString name = ("m_brainStructures["
                              + AString::number(i)
                              + "]");
        brainStructureClassArray->setClassAtIndex(i, m_brainStructures[i]->saveToScene(sceneAttributes, 
                                                                                       name));
    }
    sceneClass->addChild(brainStructureClassArray);
    
    /*
     * Save Group/Name Selection Hierarchies
     */
    for (std::vector<BorderFile*>::iterator borderIter = m_borderFiles.begin();
         borderIter != m_borderFiles.end();
         borderIter++) {
        BorderFile* bf = *borderIter;
        sceneClass->addClass(bf->getGroupAndNameHierarchyModel()->saveToScene(sceneAttributes,
                                                         bf->getFileNameNoPath()));
    }
    for (std::vector<FociFile*>::iterator fociIter = m_fociFiles.begin();
         fociIter != m_fociFiles.end();
         fociIter++) {
        FociFile* ff = *fociIter;
        sceneClass->addClass(ff->getGroupAndNameHierarchyModel()->saveToScene(sceneAttributes,
                                                         ff->getFileNameNoPath()));
    }
    
    sceneClass->addClass(m_identificationManager->saveToScene(sceneAttributes,
                                                              "m_identificationManager"));
    
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
Brain::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    bool isLoadFiles = false;
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            isLoadFiles = true;
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    if (isLoadFiles) {
        SpecFile specFile;
        specFile.restoreFromScene(sceneAttributes, 
                                  sceneClass->getClass("specFile"));
        
        loadSpecFileFromScene(sceneAttributes,
                              &specFile, 
                           RESET_BRAIN_KEEP_SCENE_FILES_YES,
                           RESET_BRAIN_KEEP_SPEC_FILE_YES);
    }
    
    /*
     * Add all scene files to the spec file (but not a member of
     * the spec file) since a scene file may not be in the spec file.
     */
    const bool specFileModStatus = m_specFile->isModified();
    for (std::vector<SceneFile*>::iterator sceneIter = m_sceneFiles.begin();
         sceneIter != m_sceneFiles.end();
         sceneIter++) {
        m_specFile->addCaretDataFile(*sceneIter);
    }
    if (specFileModStatus == false) {
        m_specFile->clearModified();
    }
    
    /*
     * Get all data files
     */
    std::vector<CaretDataFile*> allCaretDataFiles;
    getAllDataFiles(allCaretDataFiles);
    
    /*
     * Restore data files
     */
    const SceneClassArray* caretDataFileSceneArray = sceneClass->getClassArray("allCaretDataFiles");
    if (caretDataFileSceneArray != NULL) {
        for (std::vector<CaretDataFile*>::iterator iter = allCaretDataFiles.begin();
             iter != allCaretDataFiles.end();
             iter++) {
            CaretDataFile* caretDataFile = *iter;
            const AString caretDataFileName = caretDataFile->getFileNameNoPath();
            
            const int32_t numCaretDataFileScenes = caretDataFileSceneArray->getNumberOfArrayElements();
            for (int32_t i = 0; i < numCaretDataFileScenes; i++) {
                const SceneClass* fileSceneClass = caretDataFileSceneArray->getClassAtIndex(i);
                if (caretDataFileName == fileSceneClass->getName()) {
                    caretDataFile->restoreFromScene(sceneAttributes,
                                                    fileSceneClass);
                }
            }
        }
    }
    
    /*
     * Fiber trajectory files need special handling after restoring a scene.
     */
    updateFiberTrajectoryMatchingFiberOrientationFiles();
    for (std::vector<CiftiFiberTrajectoryFile*>::iterator iter = m_connectivityFiberTrajectoryFiles.begin();
         iter != m_connectivityFiberTrajectoryFiles.end();
         iter++) {
        CiftiFiberTrajectoryFile* trajFile = *iter;
        trajFile->finishRestorationOfScene();
    }

    /*
     * Restore members
     */
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Need to color all connectivity matrix files
     */
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiMappableConnectivityMatrixDataFile* cmf = *iter;
        if (cmf->isEmpty() == false) {
            const int32_t mapIndex = 0;
            cmf->updateScalarColoringForMap(mapIndex,
                                            getPaletteFile());
        }
    }

    /*
     * Restore all models
     */
    EventModelGetAll getAllModels;
    EventManager::get()->sendEvent(getAllModels.getPointer());
    std::vector<Model*> allModels = getAllModels.getModels();
    const SceneClassArray* modelsClassArray = sceneClass->getClassArray("models");
    const int32_t numModelClasses = modelsClassArray->getNumberOfArrayElements();
    for (int32_t i = 0; i < numModelClasses; i++) {
        /*
         * Apply to all models.  Each model will only use the saved
         * scene information that is applicable.
         */
        for (std::vector<Model*>::iterator iter = allModels.begin();
             iter != allModels.end();
             iter++) {
            Model* mdc = *iter;
            mdc->restoreFromScene(sceneAttributes, 
                                  modelsClassArray->getClassAtIndex(i));
        }
    }
    
    /*
     * Apply each of the saved brain structures to each of the loaded brain structures.
     * The loaded brain structure will examine the structure and number of nodes in each
     * saved brain structure to determine the proper one to use for restoration
     */
    const SceneClassArray* brainStructureClassArray = sceneClass->getClassArray("m_brainStructures");
    const int32_t numSavedBrainStructures = brainStructureClassArray->getNumberOfArrayElements();
    const int32_t numValidBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numValidBrainStructures; i++) {
        BrainStructure* bs = m_brainStructures[i];
        for (int32_t j = 0; j < numSavedBrainStructures; j++) {
            bs->restoreFromScene(sceneAttributes, 
                                 brainStructureClassArray->getClassAtIndex(j));
        }
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    /*
     * Restore Group/Name Selection Hierarchies
     */
    for (std::vector<BorderFile*>::iterator borderIter = m_borderFiles.begin();
         borderIter != m_borderFiles.end();
         borderIter++) {
        BorderFile* bf = *borderIter;
        bf->getGroupAndNameHierarchyModel()->restoreFromScene(sceneAttributes,
                                                              sceneClass->getClass(bf->getFileNameNoPath()));
    }
    for (std::vector<FociFile*>::iterator fociIter = m_fociFiles.begin();
         fociIter != m_fociFiles.end();
         fociIter++) {
        FociFile* ff = *fociIter;
        ff->getGroupAndNameHierarchyModel()->restoreFromScene(sceneAttributes,
                                                              sceneClass->getClass(ff->getFileNameNoPath()));
    }

    m_identificationManager->restoreFromScene(sceneAttributes,
                                              sceneClass->getClass("m_identificationManager"));
}

/**
 * @return The selection manager.
 */
SelectionManager*
Brain::getSelectionManager()
{
    return m_selectionManager;
}

/**
 * @return The identification manager.
 */
IdentificationManager*
Brain::getIdentificationManager()
{
    return m_identificationManager;
}

/**
 * Get the fiber orientation sample vectors for display on a sphere.
 *
 * @param xVectors
 *    Vectors for X-orientation.
 * @param yVectors
 *    Vectors for Y-orientation.
 * @param zVectors
 *    Vectors for Z-orientation.
 * @param fiberOrientation
 *    The nearby fiber orientation
 * @param errorMessageOut
 *    Will contain any error messages.
 *    This error message will only be set in some cases when there is an
 *    error.
 * @return
 *    True if data is valid, else false.
 */
bool
Brain::getFiberOrientationSphericalSamplesVectors(std::vector<FiberOrientationSamplesVector>& xVectors,
                                                                          std::vector<FiberOrientationSamplesVector>& yVectors,
                                                                          std::vector<FiberOrientationSamplesVector>& zVectors,
                                                                          FiberOrientation* &fiberOrientationOut,
                                                                          AString& errorMessageOut)
{
    CaretAssert(m_fiberOrientationSamplesLoader);
    
    return m_fiberOrientationSamplesLoader->getFiberOrientationSphericalSamplesVectors(this,
                                                                                       xVectors,
                                                                                       yVectors,
                                                                                       zVectors,
                                                                                       fiberOrientationOut,
                                                                                       errorMessageOut);
}



