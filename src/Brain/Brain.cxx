/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <algorithm>
#include <limits>

#include "CaretAssert.h"

#include "Border.h"
#include "BorderFile.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "ChartingDataManager.h"
#include "ChartableInterface.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixDenseFile.h"
#include "CiftiConnectivityMatrixDenseParcelFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiConnectivityMatrixParcelDenseFile.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFiberOrientation.h"
#include "DisplayPropertiesFiberTrajectory.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesSurface.h"
#include "DisplayPropertiesVolume.h"
#include "ElapsedTimer.h"
#include "EventBrowserTabGetAll.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventDataFileRead.h"
#include "EventDataFileReload.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventModelGetAll.h"
#include "EventProgressUpdate.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentificationManager.h"
#include "MathFunctions.h"
#include "MetricFile.h"
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
    m_ciftiConnectivityMatrixDataFileManager = new CiftiConnectivityMatrixDataFileManager(this);
    m_chartingDataManager = new ChartingDataManager(this);
    m_paletteFile = new PaletteFile();
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileName()));
    m_paletteFile->clearModified();
    m_specFile = new SpecFile();
    m_specFile->setFileName("");
    m_specFile->clearModified();
    m_surfaceMontageController = NULL;
    m_volumeSliceController = NULL;
    m_wholeBrainController = NULL;
    
    m_displayPropertiesBorders = new DisplayPropertiesBorders(this);
    m_displayProperties.push_back(m_displayPropertiesBorders);
    
    m_displayPropertiesFiberOrientation = new DisplayPropertiesFiberOrientation(this);
    m_displayProperties.push_back(m_displayPropertiesFiberOrientation);
    
    m_displayPropertiesFiberTrajectory = new DisplayPropertiesFiberTrajectory(this);
    m_displayProperties.push_back(m_displayPropertiesFiberTrajectory);
    
    m_displayPropertiesFoci = new DisplayPropertiesFoci(this);
    m_displayProperties.push_back(m_displayPropertiesFoci);
    
    m_displayPropertiesLabels = new DisplayPropertiesLabels(this);
    m_displayProperties.push_back(m_displayPropertiesLabels);
    
    m_displayPropertiesSurface = new DisplayPropertiesSurface(this);
    m_displayProperties.push_back(m_displayPropertiesSurface);
    
    m_displayPropertiesVolume = new DisplayPropertiesVolume(this);
    m_displayProperties.push_back(m_displayPropertiesVolume);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DATA_FILE_RELOAD);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES);
    
    m_isSpecFileBeingRead = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("displayPropertiesBorders", 
                          "DisplayPropertiesBorders", 
                          m_displayPropertiesBorders);
    
    m_sceneAssistant->add("displayPropertiesFiberOrientation",
                          "DisplayPropertiesFiberOrientation",
                          m_displayPropertiesFiberOrientation);
    
    m_sceneAssistant->add("displayPropertiesFiberTrajectory",
                          "DisplayPropertiesFiberTrajectory",
                          m_displayPropertiesFiberTrajectory);
    
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

    m_identificationManager = new IdentificationManager(this);
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
    delete m_ciftiConnectivityMatrixDataFileManager;
    delete m_chartingDataManager;
    delete m_paletteFile;
    if (m_surfaceMontageController != NULL) {
        delete m_surfaceMontageController;
    }
    if (m_volumeSliceController != NULL) {
        delete m_volumeSliceController;
    }
    if (m_wholeBrainController != NULL) {
        delete m_wholeBrainController;
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
    
    m_ciftiConnectivityMatrixDataFileManager->reset();
    
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
    
    updateVolumeSliceController();
    updateWholeBrainController();
    updateSurfaceMontageController();

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
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
Surface* 
Brain::readSurfaceFile(CaretDataFile* reloadThisFileIfNotNull,
                       const AString& filename,
                       const StructureEnum::Enum structureIn,
                       const bool markDataFileAsModified) throw (DataFileException)
{
    Surface* surface = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (reloadThisFileIfNotNull != NULL) {
        surface = dynamic_cast<Surface*>(reloadThisFileIfNotNull);
        CaretAssert(surface);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = surface->getStructure();
    }
    else {
        surface = new Surface();
    }
    
    try {
        surface->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            BrainStructure* bs = getBrainStructure(structure, true);
            if (bs != NULL) {
                bs->removeDataFile(surface);
            }
        }
        else {
            delete surface;
        }
        throw dfe;
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
        if (reloadThisFileIfNotNull == NULL) {
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
        const bool isReloading = (reloadThisFileIfNotNull != NULL);
        bs->addSurface(surface,
                       isReloading,
                       initializeOverlaysFlag);
    }
    else {
        if (reloadThisFileIfNotNull == NULL) {
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
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
LabelFile* 
Brain::readLabelFile(CaretDataFile* reloadThisFileIfNotNull,
                     const AString& filename,
                     const StructureEnum::Enum structureIn,
                     const bool markDataFileAsModified) throw (DataFileException)
{
    LabelFile* labelFile = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (reloadThisFileIfNotNull != NULL) {
        labelFile = dynamic_cast<LabelFile*>(reloadThisFileIfNotNull);
        CaretAssert(labelFile);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = labelFile->getStructure();
    }
    else {
        labelFile = new LabelFile();
    }

    try {
        labelFile->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            BrainStructure* bs = getBrainStructure(structure, true);
            if (bs != NULL) {
                bs->removeDataFile(labelFile);
            }
        }
        else {
            delete labelFile;
        }
        throw dfe;
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
        if (reloadThisFileIfNotNull == NULL) {
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
            const bool isReloading = (reloadThisFileIfNotNull != NULL);
            bs->addLabelFile(labelFile,
                             isReloading);
        }
        catch (const DataFileException& e) {
            if (reloadThisFileIfNotNull == NULL) {
                delete labelFile;
            }
            throw e;
        }
    }
    else {
        if (reloadThisFileIfNotNull == NULL) {
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
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
MetricFile* 
Brain::readMetricFile(CaretDataFile* reloadThisFileIfNotNull,
                      const AString& filename,
                      const StructureEnum::Enum structureIn,
                      const bool markDataFileAsModified) throw (DataFileException)
{
    MetricFile* metricFile = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (reloadThisFileIfNotNull != NULL) {
        metricFile = dynamic_cast<MetricFile*>(reloadThisFileIfNotNull);
        CaretAssert(metricFile);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = metricFile->getStructure();
    }
    else {
        metricFile = new MetricFile();
    }

    try {
        metricFile->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            BrainStructure* bs = getBrainStructure(structure, true);
            if (bs != NULL) {
                bs->removeDataFile(metricFile);
            }
        }
        else {
            delete metricFile;
        }
        throw dfe;
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
        if (reloadThisFileIfNotNull == NULL) {
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
            const bool isReloading = (reloadThisFileIfNotNull != NULL);
            bs->addMetricFile(metricFile,
                              isReloading);
        }
        catch (const DataFileException& e) {
            if (reloadThisFileIfNotNull == NULL) {
                delete metricFile;
            }
            throw e;
        }
    }
    else {
        if (reloadThisFileIfNotNull == NULL) {
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
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
RgbaFile* 
Brain::readRgbaFile(CaretDataFile* reloadThisFileIfNotNull,
                    const AString& filename,
                    const StructureEnum::Enum structureIn,
                    const bool markDataFileAsModified) throw (DataFileException)
{
    RgbaFile* rgbaFile = NULL;
    StructureEnum::Enum structure = StructureEnum::INVALID;
    if (reloadThisFileIfNotNull != NULL) {
        rgbaFile = dynamic_cast<RgbaFile*>(reloadThisFileIfNotNull);
        CaretAssert(rgbaFile);
        
        /*
         * Need structure in case file reloading fails
         */
        structure = rgbaFile->getStructure();
    }
    else {
        rgbaFile = new RgbaFile();
    }

    try {
        rgbaFile->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            BrainStructure* bs = getBrainStructure(structure, true);
            if (bs != NULL) {
                bs->removeDataFile(rgbaFile);
            }
        }
        else {
            delete rgbaFile;
        }
        throw dfe;
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
        if (reloadThisFileIfNotNull == NULL) {
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
            const bool isReloading = (reloadThisFileIfNotNull != NULL);
            bs->addRgbaFile(rgbaFile,
                            isReloading);
        }
        catch (const DataFileException& e) {
            if (reloadThisFileIfNotNull == NULL) {
                delete rgbaFile;
            }
            throw e;
        }
    }
    else {
        if (reloadThisFileIfNotNull == NULL) {
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
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
VolumeFile*
Brain::readVolumeFile(CaretDataFile* reloadThisFileIfNotNull,
                      const AString& filename) throw (DataFileException)
{
    VolumeFile* vf = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        vf = dynamic_cast<VolumeFile*>(reloadThisFileIfNotNull);
        CaretAssert(vf);
    }
    else {
        vf = new VolumeFile();
    }

    try {
        vf->readFile(filename);
    }
    catch (const DataFileException& e) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete vf;
        }
        throw e;
    }
    vf->clearModified();
    
    vf->updateScalarColoringForAllMaps(m_paletteFile);
    
    if (reloadThisFileIfNotNull == NULL) {
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
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
BorderFile*
Brain::readBorderFile(CaretDataFile* reloadThisFileIfNotNull,
                      const AString& filename) throw (DataFileException)
{
    BorderFile* bf = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        bf = dynamic_cast<BorderFile*>(reloadThisFileIfNotNull);
        CaretAssert(bf);
    }
    else {
        bf = new BorderFile();
    }

    try {
        bf->readFile(filename);
        if (reloadThisFileIfNotNull == NULL) {
            m_borderFiles.push_back(bf);
        }
    }
    catch (DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete bf;
        }
        throw dfe;
    }
    
    return bf;
}

/**
 * Read a foci file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
FociFile*
Brain::readFociFile(CaretDataFile* reloadThisFileIfNotNull,
                    const AString& filename) throw (DataFileException)
{
    FociFile* ff = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        ff = dynamic_cast<FociFile*>(reloadThisFileIfNotNull);
        CaretAssert(ff);
    }
    else {
        ff = new FociFile();
    }

    try {
        ff->readFile(filename);
        if (reloadThisFileIfNotNull == NULL) {
            m_fociFiles.push_back(ff);
        }
    }
    catch (DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete ff;
        }
        throw dfe;
    }
    
    return ff;
}

/**
 * Read a connectivity matrix dense file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixDenseFile*
Brain::readConnectivityDenseFile(CaretDataFile* reloadThisFileIfNotNull,
                                 const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixDenseFile* cmdf = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        cmdf = dynamic_cast<CiftiConnectivityMatrixDenseFile*>(reloadThisFileIfNotNull);
        CaretAssert(cmdf);
    }
    else {
        cmdf = new CiftiConnectivityMatrixDenseFile();
    }
    
    try {
        cmdf->readFile(filename);
        cmdf->clearModified();
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete cmdf;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityMatrixDenseFiles.push_back(cmdf);
    }
    
    return cmdf;
}

/**
 * Read a connectivity dense label file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @return
 *    File that was read.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiBrainordinateLabelFile*
Brain::readConnectivityDenseLabelFile(CaretDataFile* reloadThisFileIfNotNull,
                                      const AString& filename) throw (DataFileException)
{
    CiftiBrainordinateLabelFile* file = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        file = dynamic_cast<CiftiBrainordinateLabelFile*>(reloadThisFileIfNotNull);
        CaretAssert(file);
    }
    else {
        file = new CiftiBrainordinateLabelFile();
    }
    
    try {
        file->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete file;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityDenseLabelFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a connectivity dense parcel file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixDenseParcelFile*
Brain::readConnectivityMatrixDenseParcelFile(CaretDataFile* reloadThisFileIfNotNull,
                                             const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixDenseParcelFile* file = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        file = dynamic_cast<CiftiConnectivityMatrixDenseParcelFile*>(reloadThisFileIfNotNull);
        CaretAssert(file);
    }
    else {
        file = new CiftiConnectivityMatrixDenseParcelFile();
    }

    try {
        file->readFile(filename);
        
        //validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete file;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityMatrixDenseParcelFiles.push_back(file);
    }
    
    return file;
}

/**
 * Convert the loaded row in a Cifti Connectivity Matrix File into a single
 * map in a Cifti Scalar File.  The file create is added to the "in-memory"
 * files.
 *
 * @param ciftiMatrixFile
 *     Cifti Matrix File whose loaded row is convert to a Cifti Scalar File.
 * @throw
 *     DataFileException if there is an error during conversion.
 */
void
Brain::convertCiftiMatrixFileToCiftiScalarFile(const CiftiMappableConnectivityMatrixDataFile* ciftiMatrixFile) throw (DataFileException)
{
    AString errorMessage;
    
    CiftiBrainordinateScalarFile* scalarFile =
    CiftiBrainordinateScalarFile::newInstanceFromRowInCiftiConnectivityMatrixFile(ciftiMatrixFile,
                                                                                  errorMessage);
    
    if (scalarFile == NULL) {
        throw DataFileException(errorMessage);
    }

    m_connectivityDenseScalarFiles.push_back(scalarFile);
    m_specFile->addCaretDataFile(scalarFile);
}

/**
 * Read a connectivity dense scalar file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiBrainordinateScalarFile*
Brain::readConnectivityDenseScalarFile(CaretDataFile* reloadThisFileIfNotNull,
                                       const AString& filename) throw (DataFileException)
{
    CiftiBrainordinateScalarFile* clf = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        clf = dynamic_cast<CiftiBrainordinateScalarFile*>(reloadThisFileIfNotNull);
        CaretAssert(clf);
    }
    else {
        clf = new CiftiBrainordinateScalarFile();
    }
    
    try {
        clf->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete clf;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityDenseScalarFiles.push_back(clf);
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
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiFiberOrientationFile*
Brain::readConnectivityFiberOrientationFile(CaretDataFile* reloadThisFileIfNotNull,
                                            const AString& filename) throw (DataFileException)
{
    CiftiFiberOrientationFile* cfof = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        cfof = dynamic_cast<CiftiFiberOrientationFile*>(reloadThisFileIfNotNull);
        CaretAssert(cfof);
    }
    else {
        cfof = new CiftiFiberOrientationFile();
    }
    
    try {
        cfof->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete cfof;
        }
        throw dfe;
    }
    
    /*
     * If first fiber orientation file, default the above and below limits
     * to +/- one-half voxel size.
     */
    if (m_connectivityFiberOrientationFiles.empty()) {
        float voxelSizes[3];
        cfof->getVolumeSpacing(voxelSizes);
        if (voxelSizes[2] > 0.0) {
            const float aboveLimit =  voxelSizes[2] * 0.5;
            const float belowLimit = -voxelSizes[2] * 0.5;
            m_displayPropertiesFiberOrientation->setAboveAndBelowLimitsForAll(aboveLimit,
                                                                              belowLimit);
        }
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityFiberOrientationFiles.push_back(cfof);
    }
    
    return cfof;
}

/**
 * Read a connectivity fiber trajectory file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiFiberTrajectoryFile*
Brain::readConnectivityFiberTrajectoryFile(CaretDataFile* reloadThisFileIfNotNull,
                                           const AString& filename) throw (DataFileException)
{
    CiftiFiberTrajectoryFile* cftf = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        cftf = dynamic_cast<CiftiFiberTrajectoryFile*>(reloadThisFileIfNotNull);
        CaretAssert(cftf);
    }
    else {
        cftf = new CiftiFiberTrajectoryFile();
    }
    
    try {
        cftf->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete cftf;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityFiberTrajectoryFiles.push_back(cftf);
    }
    
    return cftf;
}

/**
 * Read a connectivity parcel file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixParcelFile*
Brain::readConnectivityMatrixParcelFile(CaretDataFile* reloadThisFileIfNotNull,
                                        const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixParcelFile* file = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        file = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(reloadThisFileIfNotNull);
        CaretAssert(file);
    }
    else {
        file = new CiftiConnectivityMatrixParcelFile();
    }
    
    try {
        file->readFile(filename);
        
        //validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete file;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityMatrixParcelFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a connectivity parcel dense file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiConnectivityMatrixParcelDenseFile*
Brain::readConnectivityMatrixParcelDenseFile(CaretDataFile* reloadThisFileIfNotNull,
                                             const AString& filename) throw (DataFileException)
{
    CiftiConnectivityMatrixParcelDenseFile* file = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        file = dynamic_cast<CiftiConnectivityMatrixParcelDenseFile*>(reloadThisFileIfNotNull);
        CaretAssert(file);
    }
    else {
        file = new CiftiConnectivityMatrixParcelDenseFile();
    }
    
    try {
        file->readFile(filename);
        
        //validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete file;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityMatrixParcelDenseFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a connectivity data series file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
CiftiBrainordinateDataSeriesFile*
Brain::readConnectivityDataSeriesFile(CaretDataFile* reloadThisFileIfNotNull,
                                      const AString& filename) throw (DataFileException)
{
    CiftiBrainordinateDataSeriesFile* file = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        file = dynamic_cast<CiftiBrainordinateDataSeriesFile*>(reloadThisFileIfNotNull);
        CaretAssert(file);
    }
    else {
        file = new CiftiBrainordinateDataSeriesFile();
    }
    
    try {
        file->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete file;
        }
        throw dfe;
    }
    
    if (reloadThisFileIfNotNull == NULL) {
        m_connectivityDataSeriesFiles.push_back(file);
    }
    
    return file;
}

/**
 * Read a palette file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
PaletteFile*
Brain::readPaletteFile(CaretDataFile* /* reloadThisFileIfNotNull */,
                       const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Reading not implemented for: palette");
    return NULL;
}

/**
 * Read a scene file.
 *
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
SceneFile*
Brain::readSceneFile(CaretDataFile* reloadThisFileIfNotNull,
                     const AString& filename) throw (DataFileException)
{
    SceneFile* sf = NULL;
    if (reloadThisFileIfNotNull != NULL) {
        sf = dynamic_cast<SceneFile*>(reloadThisFileIfNotNull);
        CaretAssert(sf);
    }
    else {
        sf = new SceneFile();
    }

    try {
        sf->readFile(filename);
        if (reloadThisFileIfNotNull == NULL) {
            m_sceneFiles.push_back(sf);
        }
    }
    catch (DataFileException& dfe) {
        if (reloadThisFileIfNotNull != NULL) {
            removeDataFile(reloadThisFileIfNotNull);
        }
        else {
            delete sf;
        }
        throw dfe;
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
 * Get all of the Chartable Data Files.  Only files that implement the 
 * ChartableInterface AND return true for ChartableInterface::isChartingSupported()
 * are included in the returned files.
 *
 * @param chartableDataFilesOut
 *    Contains all chartable data files upon exit.
 */
void
Brain::getAllChartableDataFiles(std::vector<ChartableInterface*>& chartableDataFilesOut) const
{
    chartableDataFilesOut.clear();
    
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        ChartableInterface* chartFile = dynamic_cast<ChartableInterface*>(*iter);
        if (chartFile != NULL) {
            if (chartFile->isChartingSupported()) {
                chartableDataFilesOut.push_back(chartFile);
            }
        }
    }
}

/**
 * Get all of the Chartable Data Files.  Only files that implement the
 * ChartableInterface, return true for ChartableInterface::isChartingSupported(),
 * AND return true for ChartableInterface::isChartingEnabled()
 * are included in the returned files.
 *
 * @param chartableDataFilesOut
 *    Contains all chartable data files upon exit.
 */
void
Brain::getAllChartableDataFilesWithChartingEnabled(std::vector<ChartableInterface*>& chartableDataFilesOut) const
{
    chartableDataFilesOut.clear();
    
    std::vector<CaretDataFile*> allFiles;
    getAllDataFiles(allFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
         iter != allFiles.end();
         iter++) {
        ChartableInterface* chartFile = dynamic_cast<ChartableInterface*>(*iter);
        if (chartFile != NULL) {
            if (chartFile->isChartingSupported()) {
                if (chartFile->isChartingEnabled()) {
                    chartableDataFilesOut.push_back(chartFile);
                }
            }
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
//    allCiftiConnectivityMatrixFiles.insert(allCiftiConnectivityMatrixFiles.end(),
//                                           m_connectivityMatrixDenseFiles.begin(),
//                                           m_connectivityMatrixDenseFiles.end());
//    
//    allCiftiConnectivityMatrixFiles.insert(allCiftiConnectivityMatrixFiles.end(),
//                                           m_connectivityMatrixDenseParcelFiles.begin(),
//                                           m_connectivityMatrixDenseParcelFiles.end());
//    
//    
//    allCiftiConnectivityMatrixFiles.insert(allCiftiConnectivityMatrixFiles.end(),
//                                           m_connectivityMatrixParcelFiles.begin(),
//                                           m_connectivityMatrixParcelFiles.end());
//    
//    allCiftiConnectivityMatrixFiles.insert(allCiftiConnectivityMatrixFiles.end(),
//                                           m_connectivityMatrixParcelDenseFiles.begin(),
//                                           m_connectivityMatrixParcelDenseFiles.end());
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
 * @return Return a new BorderFile that has been added to the brain.
 */
BorderFile* 
Brain::addBorderFile()
{
    BorderFile* bf = new BorderFile();
    bf->setFileName(updateFileNameForWriting(bf->getFileName()));
    m_borderFiles.push_back(bf);
    m_specFile->addCaretDataFile(bf);
    return bf;
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
 * For the given border, find in the border files, the border
 * that is closest to points in the given border in the given border.
 *
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param surfaceFile
 *    Surface file used for unprojection of border points.
 * @param border
 *    Border whose endpoints are used to find a nearby border
 *    in the border files.
 * @param borderTestMode
 *    Mode for which given border points are used for finding
 *    the nearest border;
 * @param maximumDistance
 *    Maximum distance given border can be from a border point.
 * @param borderFileOut
 *    File containing the border that was nearest the given border.
 * @param borderFileIndexOut
 *    Index of border file containing the border that was nearest the given border.
 * @param borderOut
 *    Border containing the point nearest the given border.
 * @param borderIndexOut
 *    Index of border in the border file containing the point nearest the coordinate.
 * @param borderPointIndexOut
 *    Index of border point nearest the given border, in the border.
 * @param borderPointOut
 *    Point in border nearest the given border.
 * @return
 *    Returns true if a border was found that was within
 *    maximum distance of either endpoint in which case ALL of
 *    the output parameters will be valid.  Otherwise, false
 *    will be returned.
 */
bool 
Brain::findBorderNearestBorder(const DisplayGroupEnum::Enum displayGroup,
                               const int32_t browserTabIndex,
                               const SurfaceFile* surfaceFile,
                              const Border* border,
                              const NearestBorderTestMode borderTestMode,
                              const float maximumDistance,
                              BorderFile*& borderFileOut,
                              int32_t& borderFileIndexOut,
                              Border*& borderOut,
                              int32_t& borderIndexOut,
                              SurfaceProjectedItem*& borderPointOut,
                              int32_t& borderPointIndexOut,
                              float& distanceToBorderPointOut) const
{
    CaretAssert(surfaceFile);
    CaretAssert(border);
    
    const int32_t numPoints = border->getNumberOfPoints();
    if (numPoints <= 0) {
        return false;
    }
    
    borderFileOut = NULL;
    borderFileIndexOut = -1;
    borderOut = NULL; 
    borderIndexOut = -1;
    borderPointOut = NULL;
    borderPointIndexOut = -1;
    distanceToBorderPointOut = maximumDistance;
    
    bool testFirstBorderPoint = false;
    bool testLastBorderPoint  = false;
    bool testAllBorderPoints  = false;
    switch (borderTestMode) {
        case NEAREST_BORDER_TEST_MODE_ALL_POINTS:
            testAllBorderPoints = true;
            break;
        case NEAREST_BORDER_TEST_MODE_ENDPOINTS:
            testFirstBorderPoint = true;
            testLastBorderPoint = true;
            break;
    }
    for (int32_t i = 0; i < numPoints; i++) {
        bool testIt = testAllBorderPoints;
        if (testFirstBorderPoint) {
            if (i == 0) {
                testIt = true;
            }
        }
        if (testLastBorderPoint) {
            if (i == (numPoints - 1)) {
                testIt = true;
            }
        }
        
        if (testIt) {
            float xyz[3];
            const SurfaceProjectedItem* firstPoint = border->getPoint(i);
            if (firstPoint->getProjectedPosition(*surfaceFile, 
                                                 xyz, 
                                                 true)) {
                BorderFile* borderFile = NULL;
                int32_t borderFileIndex = -1;
                Border* border = NULL;
                int32_t borderIndex = -1;
                SurfaceProjectedItem* borderPoint = NULL;
                int32_t borderPointIndex = -1;
                float distanceToBorderPoint = 0.0;
                if (findBorderNearestXYZ(displayGroup,
                                         browserTabIndex,
                                         surfaceFile,
                                              xyz, 
                                              maximumDistance, 
                                              borderFile, 
                                              borderFileIndex, 
                                              border, 
                                              borderIndex, 
                                              borderPoint, 
                                              borderPointIndex,
                                              distanceToBorderPoint)) {
                    if (distanceToBorderPoint < distanceToBorderPointOut) {
                        borderFileOut = borderFile;
                        borderFileIndexOut = borderFileIndex;
                        borderOut = border;
                        borderIndexOut = borderIndex;
                        borderPointOut = borderPoint;
                        borderPointIndexOut = borderPointIndex;
                        distanceToBorderPointOut = distanceToBorderPoint;
                    }
                }
            }
        }
    }
    
    const bool valid = (borderOut != NULL);
    return valid;
}

/**
 * Find the border nearest the given coordinate within
 * the given maximum distance.
 *
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param surfaceFile
 *    Surface file used for unprojection of border points.
 * @param xyz
 *    Coordinate for nearest border.
 * @param maximumDistance
 *    Maximum distance coordinate can be from a border point.
 * @param borderFileOut
 *    File containing the border that was nearest the coordinate.
 * @param borderFileIndexOut
 *    Index of border file containing the border that was nearest the coordinate.
 * @param borderOut
 *    Border containing the point nearest the coordinate.
 * @param borderIndexOut
 *    Index of border in the border file containing the point nearest the coordinate.
 * @param borderPointIndexOut
 *    Index of border point nearest the coordinate, in the border.
 * @param borderPointOut
 *    Point in border nearest the coordinate.
 * @param distanceToBorderPointOut
 *    Distance to border point found nearest the given coordinate.
 * @return
 *    Returns true if a border point was found that was within
 *    maximum distance of the coordinate in which case ALL of
 *    the output parameters will be valid.  Otherwise, false
 *    will be returned.
 */
bool 
Brain::findBorderNearestXYZ(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t browserTabIndex,
                            const SurfaceFile* surfaceFile,
                           const float xyz[3],
                           const float maximumDistance,
                           BorderFile*& borderFileOut,
                           int32_t& borderFileIndexOut,
                           Border*& borderOut,
                           int32_t& borderIndexOut,
                           SurfaceProjectedItem*& borderPointOut,
                           int32_t& borderPointIndexOut,
                           float& distanceToBorderPointOut) const
{
    CaretAssert(surfaceFile);
    
    borderFileOut = NULL;
    borderFileIndexOut = -1;
    borderOut = NULL;
    borderIndexOut = -1;
    borderPointOut = NULL;
    borderPointIndexOut = -1;
    distanceToBorderPointOut = maximumDistance;
    
    const int32_t numBorderFiles = getNumberOfBorderFiles();
    for (int32_t iFile = 0; iFile < numBorderFiles; iFile++) {
        const BorderFile* borderFile = getBorderFile(iFile);
        Border* border = NULL;
        int32_t borderIndex = -1;
        SurfaceProjectedItem* borderPoint = NULL;
        int32_t borderPointIndex;
        float distanceToNearestBorderPoint = 0.0;
        bool valid = borderFile->findBorderNearestXYZ(displayGroup,
                                                      browserTabIndex,
                                                      surfaceFile,
                                                      xyz,
                                                      maximumDistance,
                                                      border,
                                                      borderIndex,
                                                      borderPoint,
                                                      borderPointIndex,
                                                      distanceToNearestBorderPoint);
        if (valid) {
//            std::cout << "Brain: Border: " << qPrintable(border->getName())
//            << " point index: " << borderPointIndex
//            << " distance: " << distanceToBorderPointOut << std::endl;
            
            if (distanceToNearestBorderPoint < distanceToBorderPointOut) {
                CaretAssert(border);
                CaretAssert(borderIndex >= 0);
                CaretAssert(borderPoint);
                CaretAssert(borderPointIndex >= 0);
                borderFileOut = (BorderFile*)borderFile;
                borderFileIndexOut = iFile;
                borderOut = border;
                borderIndexOut = borderIndex;
                borderPointOut = borderPoint;
                borderPointIndexOut = borderPointIndex;
                distanceToBorderPointOut = distanceToNearestBorderPoint;
            }
        }
    }
    
    const bool valid = (borderFileOut != NULL);
    return valid;
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
 * @return Return a new FociFile that has been added to the brain.
 */
FociFile* 
Brain::addFociFile()
{
    FociFile* ff = new FociFile();
    ff->setFileName(updateFileNameForWriting(ff->getFileName()));
    m_fociFiles.push_back(ff);
    m_specFile->addCaretDataFile(ff);
    return ff;
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
 * @return A new scene file that has been added to the brain.
 */
SceneFile* 
Brain::addSceneFile()
{
    SceneFile* sf = new SceneFile();
    sf->setFileName(updateFileNameForWriting(sf->getFileName()));
    m_sceneFiles.push_back(sf);
    m_specFile->addCaretDataFile(sf);
    return sf;
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
 * Update the volume slice controller.
 */
void 
Brain::updateVolumeSliceController()
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
    
//    bool isValid = false;
//    if (getNumberOfVolumeFiles() > 0) {
//        isValid = true;
//    }
    
    if (isValid) {
        if (m_volumeSliceController == NULL) {
            m_volumeSliceController = new ModelVolume(this);
            EventModelAdd eventAddModel(m_volumeSliceController);
            EventManager::get()->sendEvent(eventAddModel.getPointer());

            if (m_isSpecFileBeingRead == false) {
                m_volumeSliceController->initializeOverlays();
            }
        }
    }
    else {
        if (m_volumeSliceController != NULL) {
            EventModelDelete eventDeleteModel(m_volumeSliceController);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete m_volumeSliceController;
            m_volumeSliceController = NULL;
        }
    }
    
}

/**
 * Update the whole brain controller.
 */
void 
Brain::updateWholeBrainController()
{
    bool isValid = false;
    if ((getNumberOfBrainStructures() > 0)
         || (getNumberOfVolumeFiles() > 0)) {
        isValid = true;
    }
     
    if (isValid) {
        if (m_wholeBrainController == NULL) {
            m_wholeBrainController = new ModelWholeBrain(this);
            EventModelAdd eventAddModel(m_wholeBrainController);
            EventManager::get()->sendEvent(eventAddModel.getPointer());
            
            if (m_isSpecFileBeingRead == false) {
                m_wholeBrainController->initializeOverlays();
            }
        }
    }
    else {
        if (m_wholeBrainController != NULL) {
            EventModelDelete eventDeleteModel(m_wholeBrainController);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete m_wholeBrainController;
            m_wholeBrainController = NULL;
        }
    }    
}

/**
 * Update the surface montage controller
 */
void 
Brain::updateSurfaceMontageController()
{
    bool isValid = false;
    if (getNumberOfBrainStructures() > 0) {
        isValid = true;
    }
    
    if (isValid) {
        if (m_surfaceMontageController == NULL) {
            m_surfaceMontageController = new ModelSurfaceMontage(this);
            EventModelAdd eventAddModel(m_surfaceMontageController);
            EventManager::get()->sendEvent(eventAddModel.getPointer());
            
            if (m_isSpecFileBeingRead == false) {
                m_surfaceMontageController->initializeOverlays();
            }
        }
    }
    else {
        if (m_surfaceMontageController != NULL) {
            EventModelDelete eventDeleteModel(m_surfaceMontageController);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete m_surfaceMontageController;
            m_surfaceMontageController = NULL;
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

//    EventProgressUpdate progressEvent(0,
//                                      1,
//                                      0,
//                                      ("Reading: " + caretDataFile->getFileNameNoPath()));
//    EventManager::get()->sendEvent(progressEvent.getPointer());
    
    CaretDataFile::setFileReadingUsernameAndPassword("",
                                                     "");
    
    try {
        readOrReloadDataFile(caretDataFile,
                             caretDataFile->getDataFileType(),
                             caretDataFile->getStructure(),
                             caretDataFile->getFileName(),
                             false);
    }
    catch (const DataFileException& dfe) {
        reloadDataFileEvent->setErrorMessage(dfe.whatString());
    }
    postReadDataFileProcessing();
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
 * @param reloadThisDataFileIfNotNull
 * @param reloadThisFileIfNotNull
 *    If this value is not NULL, reload this file instead of creating and
 *    adding a new file.  NOTE: if this file fails to reload, IT WILL BE
 *    deleted.
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
Brain::readOrReloadDataFile(CaretDataFile* reloadThisDataFileIfNotNull,
                             const DataFileTypeEnum::Enum dataFileType,
                             const StructureEnum::Enum structure,
                             const AString& dataFileName,
                             const bool markDataFileAsModified) throw (DataFileException)
{
    CaretDataFile* caretDataFileRead = NULL;

    try {
        
        ElapsedTimer et;
        et.start();
        
        switch (dataFileType) {
            case DataFileTypeEnum::BORDER:
                caretDataFileRead = readBorderFile(reloadThisDataFileIfNotNull,
                                                   dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                caretDataFileRead = readConnectivityDenseFile(reloadThisDataFileIfNotNull,
                                                              dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                caretDataFileRead = readConnectivityDenseLabelFile(reloadThisDataFileIfNotNull,
                                                                   dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                caretDataFileRead = readConnectivityMatrixDenseParcelFile(reloadThisDataFileIfNotNull,
                                                                          dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                caretDataFileRead = readConnectivityDenseScalarFile(reloadThisDataFileIfNotNull,
                                                                        dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                caretDataFileRead = readConnectivityDataSeriesFile(reloadThisDataFileIfNotNull,
                                                                   dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                caretDataFileRead = readConnectivityFiberOrientationFile(reloadThisDataFileIfNotNull,
                                                                         dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                caretDataFileRead = readConnectivityFiberTrajectoryFile(reloadThisDataFileIfNotNull,
                                                                        dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                caretDataFileRead = readConnectivityMatrixParcelFile(reloadThisDataFileIfNotNull,
                                                                     dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                caretDataFileRead = readConnectivityMatrixParcelDenseFile(reloadThisDataFileIfNotNull,
                                                                          dataFileName);
                break;
            case DataFileTypeEnum::FOCI:
                caretDataFileRead = readFociFile(reloadThisDataFileIfNotNull,
                                                 dataFileName);
                break;
            case DataFileTypeEnum::LABEL:
                caretDataFileRead = readLabelFile(reloadThisDataFileIfNotNull,
                                                  dataFileName,
                                                  structure,
                                                  markDataFileAsModified);
                break;
            case DataFileTypeEnum::METRIC:
                caretDataFileRead = readMetricFile(reloadThisDataFileIfNotNull,
                                                   dataFileName,
                                                   structure,
                                                   markDataFileAsModified);
                break;
            case DataFileTypeEnum::PALETTE:
                caretDataFileRead = readPaletteFile(reloadThisDataFileIfNotNull,
                                                    dataFileName);
                break;
            case DataFileTypeEnum::RGBA:
                caretDataFileRead = readRgbaFile(reloadThisDataFileIfNotNull,
                                                 dataFileName,
                                                 structure,
                                                 markDataFileAsModified);
                break;
            case DataFileTypeEnum::SCENE:
                caretDataFileRead = readSceneFile(reloadThisDataFileIfNotNull,
                                                  dataFileName);
                break;
            case DataFileTypeEnum::SPECIFICATION:
                CaretLogSevere("PROGRAM ERROR: Reading spec file should never call Brain::readOrReloadDataFile()");
                throw DataFileException("PROGRAM ERROR: Reading spec file should never call Brain::readOrReloadDataFile()");
                break;
            case DataFileTypeEnum::SURFACE:
                caretDataFileRead = readSurfaceFile(reloadThisDataFileIfNotNull,
                                                    dataFileName,
                                                    structure,
                                                    markDataFileAsModified);
                break;
            case DataFileTypeEnum::UNKNOWN:
                throw DataFileException("Unable to read files of type");
                break;
            case DataFileTypeEnum::VOLUME:
                caretDataFileRead = readVolumeFile(reloadThisDataFileIfNotNull,
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
        if (reloadThisDataFileIfNotNull != NULL) {
            m_specFile->removeCaretDataFile(reloadThisDataFileIfNotNull);
        }
        throw dfe;
    }
    
    return caretDataFileRead;
}

/**
 * Read a data file.
 *
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
    
    CaretDataFile* caretDataFileRead = readOrReloadDataFile(NULL,
                                                            dataFileType,
                                                            structure,
                                                            dataFileName,
                                                            markDataFileAsModified);
    
    
//    if (addDataFileToSpecFile) {
//        if (m_specFileName.isEmpty() == false) {
//            FileInformation specFileInfo(m_specFileName);
//            QString relativePathDataFileName = SystemUtilities::relativePath(dataFileName,
//                                                                             specFileInfo.getPathName());
//            
//            StructureEnum::Enum dataFileStructure = structure;
//            if (dataFileStructure == StructureEnum::INVALID) {
//                if (caretDataFileRead != NULL) {
//                    dataFileStructure = caretDataFileRead->getStructure();
//                }
//                else {
//                    dataFileStructure = StructureEnum::ALL;
//                }
//            }
//            try {
//                SpecFile sf;
//                sf.readFile(m_specFileName);
//                sf.addDataFile(dataFileType,
//                               dataFileStructure,
//                               relativePathDataFileName,
//                               true,
//                               false,
//                               false);
//                sf.writeFile(m_specFileName);
//            }
//            catch (const DataFileException& e) {
//                addToSpecFileErrorMessageOut = ("Unable to add file \""
//                                             + dataFileName
//                                             + "\" to SpecFile \""
//                                             + m_specFileName
//                                             + "\", Error:"
//                                             + e.whatString());
//                CaretLogWarning(addToSpecFileErrorMessageOut);
//            }
//        }
//    }
    
    postReadDataFileProcessing();
    
    return caretDataFileRead;
}

/**
 * Processing performed after reading a data file.
 */
void
Brain::postReadDataFileProcessing()
{
    updateVolumeSliceController();
    updateWholeBrainController();
    updateSurfaceMontageController();
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
    
//    if (m_paletteFile != NULL) {
//        delete m_paletteFile;
//    }
//    m_paletteFile = new PaletteFile();
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
    
/*
    EventBrowserTabGetAll getAllTabs;
    EventManager::get()->sendEvent(getAllTabs.getPointer());
    const int32_t numTabs = getAllTabs.getNumberOfBrowserTabs();
    for (int32_t i = 0; i < numTabs; i++) {
        BrowserTabContent* btc = getAllTabs.getBrowserTab(i);
        btc->getVolumeSurfaceOutlineSet()->selectSurfacesAfterSpecFileLoaded(this, false);
    }
*/    
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
    
    resetBrain(keepSceneFiles,
                     keepSpecFile);
    
//    /*
//     * Set current directory to directory containing the scene file
//     * if the current spec file is not a valid file
//     */
//    
//    const AString currentSpecFileName = m_specFile->getFileName();
//    FileInformation currentSpecFileInfo(currentSpecFileName);
//    if (currentSpecFileInfo.exists() == false) {
//        
//    }
//    /*
//     * Try to set to current directory
//     */
//    const AString previousSpecFileName = m_specFile->getFileName();
//    delete m_specFile;
//    m_specFile = new SpecFile(*specFileToLoad);
//    FileInformation newSpecFileInfo(m_specFile->getFileName());
//    if (newSpecFileInfo.isAbsolute()) {
//        setCurrentDirectory(newSpecFileInfo.getPathName());
//    }
//    else {
//        if (previousSpecFileName.endsWith(m_specFile->getFileName()) == false) {
//            FileInformation oldSpecFileInfo(previousSpecFileName);
//            setCurrentDirectory(oldSpecFileInfo.getPathName());
//        }
//    }

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
    
    const int32_t numFileGroups = specFileToLoad->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0; ig < numFileGroups; ig++) {
        const SpecFileDataFileTypeGroup* group = specFileToLoad->getDataFileTypeGroupByIndex(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        const int32_t numFiles = group->getNumberOfFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            const SpecFileDataFile* fileInfo = group->getFileInformation(iFile);
            if (fileInfo->isLoadingSelected()) {
                AString filename = fileInfo->getFileName();
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
                try {
                    readDataFile(dataFileType,
                                       structure, 
                                       filename,
                                       false);
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
    AString fullPathName = pathFileInfo.getFilePath();
    
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
    AString fullPathName = pathFileInfo.getFilePath();
    
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
    if (event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_READ) {
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
        

        for (std::vector<CiftiConnectivityMatrixDenseFile*>::iterator icf = m_connectivityMatrixDenseFiles.begin();
             icf != m_connectivityMatrixDenseFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<CiftiBrainordinateLabelFile*>::iterator icf = m_connectivityDenseLabelFiles.begin();
             icf != m_connectivityDenseLabelFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<CiftiConnectivityMatrixDenseParcelFile*>::iterator icf = m_connectivityMatrixDenseParcelFiles.begin();
             icf != m_connectivityMatrixDenseParcelFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<CiftiBrainordinateScalarFile*>::iterator icf = m_connectivityDenseScalarFiles.begin();
             icf != m_connectivityDenseScalarFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<CiftiConnectivityMatrixParcelFile*>::iterator icf = m_connectivityMatrixParcelFiles.begin();
             icf != m_connectivityMatrixParcelFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<CiftiConnectivityMatrixParcelDenseFile*>::iterator icf = m_connectivityMatrixParcelDenseFiles.begin();
             icf != m_connectivityMatrixParcelDenseFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<CiftiBrainordinateDataSeriesFile*>::iterator ictsf = m_connectivityDataSeriesFiles.begin();
             ictsf != m_connectivityDataSeriesFiles.end();
             ictsf++) {
            dataFilesEvent->addFile(*ictsf);
        }
        
        for (std::vector<VolumeFile*>::iterator volumeIter = m_volumeFiles.begin();
             volumeIter != m_volumeFiles.end();
             volumeIter++) {
            dataFilesEvent->addFile(*volumeIter);
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
}

/**
 * @param The CIFTI connectivity matrix data file manager
 */
CiftiConnectivityMatrixDataFileManager*
Brain::getCiftiConnectivityMatrixDataFileManager()
{
    return m_ciftiConnectivityMatrixDataFileManager;
}

/**
 * @param The CIFTI connectivity matrix data file manager
 */
const CiftiConnectivityMatrixDataFileManager*
Brain::getCiftiConnectivityMatrixDataFileManager() const
{
    return m_ciftiConnectivityMatrixDataFileManager;
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
 * Determine all loaded data files that are displayed.
 * A file is 'displayed' if it is selected in 
 * any tab (whether or not the tab is visible).
 * The file may be displayed in a model (such as
 * surface or surface montage, selected in an 
 * overlay, or displayed as borders/foci.
 *
 * After calling this method, a method in 
 * CaretDataFile may be called that indicates if
 * the file is displayed in the graphical user-interface.
 */
void 
Brain::determineDisplayedDataFiles()
{
    /*
     * Get all browser tabs.
     */
    EventBrowserTabGetAll getAllTabsEvent;
    EventManager::get()->sendEvent(getAllTabsEvent.getPointer());
    
    std::set<const CaretDataFile*> displayedDataFiles;
    
    /*
     * Get files displayed in each tab.
     */
    const int32_t numberOfTabs = getAllTabsEvent.getNumberOfBrowserTabs();
    for (int32_t i = 0; i < numberOfTabs; i++) {
        BrowserTabContent* btc = getAllTabsEvent.getBrowserTab(i);
        std::vector<CaretDataFile*> tabDataFiles;
        btc->getFilesDisplayedInTab(tabDataFiles);
        displayedDataFiles.insert(tabDataFiles.begin(),
                                  tabDataFiles.end());
    }
    
    /*
     * See if any palette mappable files are displayed
     */
    bool havePaletteMappableFiles = false;
    for (std::set<const CaretDataFile*>::iterator iter = displayedDataFiles.begin();
         iter != displayedDataFiles.end();
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
     * Get all loaded files.
     */
    std::vector<CaretDataFile*> dataFiles;
    getAllDataFiles(dataFiles);
    
    /*
     * Output only those data files that were found to be displayed.
     */
    for (std::vector<CaretDataFile*>::iterator iter = dataFiles.begin();
         iter != dataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        bool displayFlag = false;
        
        if (cdf->getDataFileType() == DataFileTypeEnum::PALETTE) {
            if (havePaletteMappableFiles) {
                displayFlag = true;
            }
        }
        else if (displayedDataFiles.find(cdf) != displayedDataFiles.end()) {
            displayFlag = true;
        }
        
        cdf->setDisplayedInGUI(displayFlag);
    }
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
 * Are any data files modified (including spec file)?
 * @param excludeTheseDataTypes
 *    Do not check the modification status of any data files whose
 *    data type is contained in this parameter.
 */
bool
Brain::areFilesModified(const std::vector<DataFileTypeEnum::Enum>& excludeTheseDataTypes)
{
    if (std::find(excludeTheseDataTypes.begin(),
                  excludeTheseDataTypes.end(),
                  DataFileTypeEnum::SPECIFICATION) == excludeTheseDataTypes.end()) {
        if (m_specFile->isModified()) {
            return true;
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
                return true;
            }
        }
    }
    
    return false;
}


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
 * Remove a data file from memory (does NOT delete file on disk.)
 * Searches all of the loaded files for given file, and, when found
 * deletes the file.
 * @param caretDataFile
 *    Data file to remove.
 * @return
 *    true if file was removed, else false.
 */
bool 
Brain::removeDataFile(CaretDataFile* caretDataFile)
{
    if (caretDataFile == NULL) {
        return false;
    }

    bool wasRemoved = false;
    CaretDataFile* caretDataFileForRemovalFromSpecFile = caretDataFile;
    
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        if (getBrainStructure(i)->removeDataFile(caretDataFile)) {
            wasRemoved = true;
            caretDataFile = NULL;
        }
    }
    
    std::vector<BorderFile*>::iterator borderIterator = std::find(m_borderFiles.begin(),
                                                                  m_borderFiles.end(),
                                                                  caretDataFile);
    if (borderIterator != m_borderFiles.end()) {
        BorderFile* borderFile = *borderIterator;
        delete borderFile;
        m_borderFiles.erase(borderIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiBrainordinateDataSeriesFile*>::iterator dataSeriesIterator = std::find(m_connectivityDataSeriesFiles.begin(),
                                                                            m_connectivityDataSeriesFiles.end(),
                                                                            caretDataFile);
    if (dataSeriesIterator != m_connectivityDataSeriesFiles.end()) {
        CiftiBrainordinateDataSeriesFile* dataSeriesFile = *dataSeriesIterator;
        
        const AString name = dataSeriesFile->getFileName();
        delete dataSeriesFile;
        m_connectivityDataSeriesFiles.erase(dataSeriesIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiBrainordinateLabelFile*>::iterator connLabelIterator = std::find(m_connectivityDenseLabelFiles.begin(),
                                                                                      m_connectivityDenseLabelFiles.end(),
                                                                                      caretDataFile);
    if (connLabelIterator != m_connectivityDenseLabelFiles.end()) {
        CiftiBrainordinateLabelFile* connFile = *connLabelIterator;
        delete connFile;
        m_connectivityDenseLabelFiles.erase(connLabelIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiConnectivityMatrixDenseFile*>::iterator connDenseIterator = std::find(m_connectivityMatrixDenseFiles.begin(),
                                                                                                       m_connectivityMatrixDenseFiles.end(),
                                                                                                       caretDataFile);
    if (connDenseIterator != m_connectivityMatrixDenseFiles.end()) {
        CiftiConnectivityMatrixDenseFile* connFile = *connDenseIterator;
        delete connFile;
        m_connectivityMatrixDenseFiles.erase(connDenseIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiConnectivityMatrixDenseParcelFile*>::iterator connDenseParcelIterator = std::find(m_connectivityMatrixDenseParcelFiles.begin(),
                                                                                 m_connectivityMatrixDenseParcelFiles.end(),
                                                                                 caretDataFile);
    if (connDenseParcelIterator != m_connectivityMatrixDenseParcelFiles.end()) {
        CiftiConnectivityMatrixDenseParcelFile* connFile = *connDenseParcelIterator;
        delete connFile;
        m_connectivityMatrixDenseParcelFiles.erase(connDenseParcelIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiBrainordinateScalarFile*>::iterator connScalarIterator = std::find(m_connectivityDenseScalarFiles.begin(),
                                                                            m_connectivityDenseScalarFiles.end(),
                                                                            caretDataFile);
    if (connScalarIterator != m_connectivityDenseScalarFiles.end()) {
        CiftiBrainordinateScalarFile* connFile = *connScalarIterator;
        const AString name = connFile->getFileName();
        
        delete connFile;
        m_connectivityDenseScalarFiles.erase(connScalarIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiFiberOrientationFile*>::iterator connFiberOrientationIterator = std::find(m_connectivityFiberOrientationFiles.begin(),
                                                                            m_connectivityFiberOrientationFiles.end(),
                                                                            caretDataFile);
    bool remvovedFiberOrientationFile = false;
    if (connFiberOrientationIterator != m_connectivityFiberOrientationFiles.end()) {
        CiftiFiberOrientationFile* connFile = *connFiberOrientationIterator;
        delete connFile;
        m_connectivityFiberOrientationFiles.erase(connFiberOrientationIterator);
        wasRemoved = true;
        caretDataFile = NULL;
        
        remvovedFiberOrientationFile = true;
    }
    
    /*
     * If any fiber orientation files are removed, need to unload
     * any fiber trajectories that were loaded since the trajectories
     * point to fiber orientations in the fiber orientation file
     * that was deleted.
     */
    if (remvovedFiberOrientationFile) {
        for (std::vector<CiftiFiberTrajectoryFile*>::iterator clfi = m_connectivityFiberTrajectoryFiles.begin();
             clfi != m_connectivityFiberTrajectoryFiles.end();
             clfi++) {
            CiftiFiberTrajectoryFile* clf = *clfi;
            clf->clearLoadedFiberOrientations();
        }
    }
    
    std::vector<CiftiFiberTrajectoryFile*>::iterator connFiberTrajectoryIterator = std::find(m_connectivityFiberTrajectoryFiles.begin(),
                                                                                            m_connectivityFiberTrajectoryFiles.end(),
                                                                                            caretDataFile);
    if (connFiberTrajectoryIterator != m_connectivityFiberTrajectoryFiles.end()) {
        CiftiFiberTrajectoryFile* trajFile = *connFiberTrajectoryIterator;
        delete trajFile;
        m_connectivityFiberTrajectoryFiles.erase(connFiberTrajectoryIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiConnectivityMatrixParcelFile*>::iterator connParcelIterator = std::find(m_connectivityMatrixParcelFiles.begin(),
                                                                                     m_connectivityMatrixParcelFiles.end(),
                                                                                     caretDataFile);
    if (connParcelIterator != m_connectivityMatrixParcelFiles.end()) {
        CiftiConnectivityMatrixParcelFile* connFile = *connParcelIterator;
        delete connFile;
        m_connectivityMatrixParcelFiles.erase(connParcelIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }    

    std::vector<CiftiConnectivityMatrixParcelDenseFile*>::iterator connParcelDenseIterator = std::find(m_connectivityMatrixParcelDenseFiles.begin(),
                                                                                     m_connectivityMatrixParcelDenseFiles.end(),
                                                                                     caretDataFile);
    if (connParcelDenseIterator != m_connectivityMatrixParcelDenseFiles.end()) {
        CiftiConnectivityMatrixParcelDenseFile* connFile = *connParcelDenseIterator;
        delete connFile;
        m_connectivityMatrixParcelDenseFiles.erase(connParcelDenseIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }

    std::vector<FociFile*>::iterator fociIterator = std::find(m_fociFiles.begin(),
                                                                  m_fociFiles.end(),
                                                                  caretDataFile);
    if (fociIterator != m_fociFiles.end()) {
        FociFile* fociFile =  *fociIterator;
        delete fociFile;
        m_fociFiles.erase(fociIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    if (m_paletteFile == caretDataFile) {
        if (m_paletteFile != NULL) {
            throw DataFileException("Cannot remove PaletteFile at this time.");
        }
    }
    
    std::vector<SceneFile*>::iterator sceneIterator = std::find(m_sceneFiles.begin(),
                                                                m_sceneFiles.end(),
                                                                caretDataFile);
    if (sceneIterator != m_sceneFiles.end()) {
        SceneFile* sceneFile = *sceneIterator;
        delete sceneFile;
        m_sceneFiles.erase(sceneIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<VolumeFile*>::iterator volumeIterator = 
    std::find(m_volumeFiles.begin(),
              m_volumeFiles.end(),
              caretDataFile);
    if (volumeIterator != m_volumeFiles.end()) {
        VolumeFile* volumeFile = *volumeIterator;
        delete volumeFile;
        m_volumeFiles.erase(volumeIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    if (wasRemoved) {
        m_specFile->removeCaretDataFile(caretDataFileForRemovalFromSpecFile);
        
        updateVolumeSliceController();
        updateWholeBrainController();
        updateSurfaceMontageController();
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
 * @return The fiber trajectory display properties.
 */
DisplayPropertiesFiberTrajectory*
Brain::getDisplayPropertiesFiberTrajectory()
{
    return m_displayPropertiesFiberTrajectory;
}

/**
 * @return The fiber trajectory display properties.
 */
const DisplayPropertiesFiberTrajectory*
Brain::getDisplayPropertiesFiberTrajectory() const
{
    return m_displayPropertiesFiberTrajectory;
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
     * Save connectivity data
     */
    sceneClass->addClass(m_ciftiConnectivityMatrixDataFileManager->saveToScene(sceneAttributes,
                                                                               "m_ciftiConnectivityMatrixDataFileManager"));
    
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
     * Restore members
     */
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Restore connectivity data
     * Must be restored before models are restored for overlays to
     * be restored correctly.
     */
    m_ciftiConnectivityMatrixDataFileManager->restoreFromScene(sceneAttributes,
                                                               sceneClass->getClass("m_ciftiConnectivityMatrixDataFileManager"));

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


