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
#include "CiftiScalarFile.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityLoaderManager.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFiberOrientation.h"
#include "DisplayPropertiesFoci.h"
#include "DisplayPropertiesInformation.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesSurface.h"
#include "DisplayPropertiesVolume.h"
#include "ElapsedTimer.h"
#include "EventBrowserTabGetAll.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventDataFileRead.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventModelGetAll.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "IdentificationManager.h"
#include "MetricFile.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "LabelFile.h"
#include "PaletteFile.h"
#include "RgbaFile.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneFile.h"
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
    m_connectivityLoaderManager = new ConnectivityLoaderManager(this);
    m_paletteFile = new PaletteFile();
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileName()));
    m_paletteFile->clearModified();
    m_specFileName = "";
    m_surfaceMontageController = NULL;
    m_volumeSliceController = NULL;
    m_wholeBrainController = NULL;
    
    m_displayPropertiesBorders = new DisplayPropertiesBorders(this);
    m_displayProperties.push_back(m_displayPropertiesBorders);
    
    m_displayPropertiesFiberOrientation = new DisplayPropertiesFiberOrientation(this);
    m_displayProperties.push_back(m_displayPropertiesFiberOrientation);
    
    m_displayPropertiesFoci = new DisplayPropertiesFoci(this);
    m_displayProperties.push_back(m_displayPropertiesFoci);
    
    m_displayPropertiesInformation = new DisplayPropertiesInformation(this);
    m_displayProperties.push_back(m_displayPropertiesInformation);
    
    m_displayPropertiesLabels = new DisplayPropertiesLabels(this);
    m_displayProperties.push_back(m_displayPropertiesLabels);
    
    m_displayPropertiesSurface = new DisplayPropertiesSurface(this);
    m_displayProperties.push_back(m_displayPropertiesSurface);
    
    m_displayPropertiesVolume = new DisplayPropertiesVolume(this);
    m_displayProperties.push_back(m_displayPropertiesVolume);
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES);
    
    m_isSpecFileBeingRead = false;
    m_fileReadingUsername = "";
    m_fileReadingPassword = "";
    
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
    
    m_sceneAssistant->add("displayPropertiesInformation", 
                          "DisplayPropertiesInformation", 
                          m_displayPropertiesInformation);
    
    m_sceneAssistant->add("m_displayPropertiesLabels",
                          "m_displayPropertiesLabels",
                          m_displayPropertiesLabels);
    
    m_sceneAssistant->add("m_displayPropertiesSurface",
                          "DisplayPropertiesSurface",
                          m_displayPropertiesSurface);
    
    m_sceneAssistant->add("displayPropertiesVolume",
                          "DisplayPropertiesVolume", 
                          m_displayPropertiesVolume);
    
    this->m_identificationManager = new IdentificationManager();
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

    delete m_connectivityLoaderManager;
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

    delete this->m_identificationManager;
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
    
    for (std::vector<ConnectivityLoaderFile*>::iterator clfi = m_connectivityDenseFiles.begin();
         clfi != m_connectivityDenseFiles.end();
         clfi++) {
        ConnectivityLoaderFile* clf = *clfi;
        delete clf;
    }
    m_connectivityDenseFiles.clear();
    
    for (std::vector<ConnectivityLoaderFile*>::iterator clfi = m_connectivityDenseLabelFiles.begin();
         clfi != m_connectivityDenseLabelFiles.end();
         clfi++) {
        ConnectivityLoaderFile* clf = *clfi;
        delete clf;
    }
    m_connectivityDenseLabelFiles.clear();
    
    for (std::vector<CiftiScalarFile*>::iterator clfi = m_connectivityDenseScalarFiles.begin();
         clfi != m_connectivityDenseScalarFiles.end();
         clfi++) {
        CiftiScalarFile* clf = *clfi;
        delete clf;
    }
    m_connectivityDenseScalarFiles.clear();
    
    for (std::vector<ConnectivityLoaderFile*>::iterator clfi = m_connectivityFiberOrientationFiles.begin();
         clfi != m_connectivityFiberOrientationFiles.end();
         clfi++) {
        ConnectivityLoaderFile* clf = *clfi;
        delete clf;
    }
    m_connectivityFiberOrientationFiles.clear();
    
    for (std::vector<ConnectivityLoaderFile*>::iterator cltsfi = m_connectivityTimeSeriesFiles.begin();
         cltsfi != m_connectivityTimeSeriesFiles.end();
         cltsfi++) {
        ConnectivityLoaderFile* clf = *cltsfi;
        delete clf;
    }
    m_connectivityTimeSeriesFiles.clear();
    
    if (m_paletteFile != NULL) {
        delete m_paletteFile;
    }
    m_paletteFile = new PaletteFile();
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileName()));
    m_paletteFile->clearModified();
    
    m_connectivityLoaderManager->reset();
    
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
            m_specFileName = "";
            break;
        case RESET_BRAIN_KEEP_SPEC_FILE_YES:
            break;
    }
    
    for (std::vector<DisplayProperties*>::iterator iter = m_displayProperties.begin();
         iter != m_displayProperties.end();
         iter++) {
        (*iter)->reset();
    }
    
    m_identificationManager->reset();
    m_identificationManager->setLastIdentifiedItem(NULL);
    
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
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
Surface* 
Brain::readSurfaceFile(const AString& filename,
                       const StructureEnum::Enum structureIn) throw (DataFileException)
{
    Surface* surface = new Surface();
    try {
        surface->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        delete surface;
        throw dfe;
    }
    
    if (structureIn != StructureEnum::INVALID) {
        surface->setStructure(structureIn);
    }
    const StructureEnum::Enum structure = surface->getStructure();
    if (structure == StructureEnum::INVALID) {
        delete surface;
        AString message = "Structure in "
        + filename
        + " is not valid.";
        DataFileException e(message);
        e.setErrorInvalidStructure(true);
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = getBrainStructure(structure, true);
    if (bs != NULL) {
        const bool initializeOverlaysFlag = (m_isSpecFileBeingRead == false);
        bs->addSurface(surface,
                       initializeOverlaysFlag);
    }
    else {
        delete surface;
        AString message = "Failed to create a BrainStructure for surface with structure "
        + StructureEnum::toGuiName(structure)
        + ".";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    surface->clearModified();

    return surface;
}

/**
 * Read a label file.
 *
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
LabelFile* 
Brain::readLabelFile(const AString& filename,
                     const StructureEnum::Enum structureIn) throw (DataFileException)
{
    LabelFile* labelFile = new LabelFile();
    try {
        labelFile->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        delete labelFile;
        throw dfe;
    }
    
    if (structureIn != StructureEnum::INVALID) {
        labelFile->setStructure(structureIn);
    }
    
    const StructureEnum::Enum structure = labelFile->getStructure();
    if (structure == StructureEnum::INVALID) {
        delete labelFile;
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
            bs->addLabelFile(labelFile);
        }
        catch (const DataFileException& e) {
            delete labelFile;
            throw e;
        }
    }
    else {
        delete labelFile;
        AString message = "Must read a surface with structure "
        + StructureEnum::toGuiName(structure)
        + " before reading its label files.";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    labelFile->clearModified();
    
    return labelFile;
}

/**
 * Read a metric file.
 *
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
MetricFile* 
Brain::readMetricFile(const AString& filename,
                      const StructureEnum::Enum structureIn) throw (DataFileException)
{
    MetricFile* metricFile = new MetricFile();
    try {
        metricFile->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        delete metricFile;
        throw dfe;
    }
    
    if (structureIn != StructureEnum::INVALID) {
        metricFile->setStructure(structureIn);
    }
    
    const StructureEnum::Enum structure = metricFile->getStructure();
    if (structure == StructureEnum::INVALID) {
        delete metricFile;
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
            bs->addMetricFile(metricFile);
        }
        catch (const DataFileException& e) {
            delete metricFile;
            throw e;
        }
    }
    else {
        delete metricFile;
        AString message = "Must read a surface with structure "
        + StructureEnum::toGuiName(structure)
        + " before reading its metric files.";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    metricFile->clearModified();
    
    return metricFile;
}

/**
 * Read an RGBA file.
 *
 * @param filename
 *    Name of the file.
 * @param structureIn
 *    Structure of label file.
 * @throws DataFileException
 *    If reading failed.
 * @return Pointer to file that was read.
 */
RgbaFile* 
Brain::readRgbaFile(const AString& filename,
                    const StructureEnum::Enum structureIn) throw (DataFileException)
{
    RgbaFile* rgbaFile = new RgbaFile();
    try {
        rgbaFile->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        delete rgbaFile;
        throw dfe;
    }
    
    if (structureIn != StructureEnum::INVALID) {
        rgbaFile->setStructure(structureIn);
    }
    
    const StructureEnum::Enum structure = rgbaFile->getStructure();
    if (structure == StructureEnum::INVALID) {
        delete rgbaFile;
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
            bs->addRgbaFile(rgbaFile);
        }
        catch (const DataFileException& e) {
            delete rgbaFile;
            throw e;
        }
    }
    else {
        delete rgbaFile;
        AString message = "Must read a surface with structure "
        + StructureEnum::toGuiName(structure)
        + " before reading its RGBA files.";
        DataFileException e(message);
        CaretLogThrowing(e);
        throw e;
    }
    
    rgbaFile->clearModified();
    
    return rgbaFile;
}

/**
 * Read a volume file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readVolumeFile(const AString& filename) throw (DataFileException)
{
/*
    std::vector<int64_t> dim;
    dim.push_back(176);
    dim.push_back(208);
    dim.push_back(176);
    std::vector<float> row1;
    row1.push_back(1.0);
    row1.push_back(0.0);
    row1.push_back(0.0);
    row1.push_back(1.0);
    std::vector<float> row2;
    row2.push_back(0.0);
    row2.push_back(1.0);
    row2.push_back(0.0);
    row2.push_back(1.0);
    std::vector<float> row3;
    row3.push_back(0.0);
    row3.push_back(0.0);
    row3.push_back(1.0);
    row3.push_back(1.0);
    
    std::vector<std::vector<float> > indexToSpace;
    indexToSpace.push_back(row1);
    indexToSpace.push_back(row2);
    indexToSpace.push_back(row3);
    
    VolumeFile* vTest = new VolumeFile(dim, indexToSpace, 1);
    m_volumeFiles.push_back(vTest);
*/
    VolumeFile* vf = new VolumeFile();
    try {
        vf->readFile(filename);
    }
    catch (const DataFileException& e) {
        delete vf;
        throw e;
    }
    vf->clearModified();
    
    m_volumeFiles.push_back(vf);
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
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readBorderFile(const AString& filename) throw (DataFileException)
{
    BorderFile* bf = new BorderFile;
    try {
        bf->readFile(filename);
        m_borderFiles.push_back(bf);
    }
    catch (DataFileException& dfe) {
        delete bf;
        throw dfe;
    }
}

/**
 * Read a foci file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readFociFile(const AString& filename) throw (DataFileException)
{
    FociFile* ff = new FociFile;
    try {
        ff->readFile(filename);
        m_fociFiles.push_back(ff);
    }
    catch (DataFileException& dfe) {
        delete ff;
        throw dfe;
    }
}

/**
 * Read a connectivity dense file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readConnectivityDenseFile(const AString& filename) throw (DataFileException)
{
    ConnectivityLoaderFile* clf = new ConnectivityLoaderFile();

    try {
        if (DataFile::isFileOnNetwork(filename)) {
            clf->setupNetworkFile(filename,
                                  DataFileTypeEnum::CONNECTIVITY_DENSE,
                                  m_fileReadingUsername,
                                  m_fileReadingPassword);
        }
        else {
            clf->setupLocalFile(filename, 
                                DataFileTypeEnum::CONNECTIVITY_DENSE);
        }
        
        validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        delete clf;
        throw dfe;
    }
    
    m_connectivityDenseFiles.push_back(clf);
}

/**
 * Read a connectivity dense label file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void
Brain::readConnectivityDenseLabelFile(const AString& filename) throw (DataFileException)
{
    ConnectivityLoaderFile* clf = new ConnectivityLoaderFile();
    
    try {
        if (DataFile::isFileOnNetwork(filename)) {
            clf->setupNetworkFile(filename,
                                  DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL,
                                  m_fileReadingUsername,
                                  m_fileReadingPassword);
        }
        else {
            clf->setupLocalFile(filename,
                                DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL);
        }
        
        validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        delete clf;
        throw dfe;
    }
    
    m_connectivityDenseLabelFiles.push_back(clf);
}

/**
 * Read a connectivity dense scalar file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void
Brain::readConnectivityDenseScalarFile(const AString& filename) throw (DataFileException)
{
    CiftiScalarFile* clf = new CiftiScalarFile();
    
    try {
        clf->readFile(filename);
//        if (DataFile::isFileOnNetwork(filename)) {
//            clf->setupNetworkFile(filename,
//                                  DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
//                                  m_fileReadingUsername,
//                                  m_fileReadingPassword);
//        }
//        else {
//            clf->setupLocalFile(filename,
//                                DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR);
//        }
        
        //validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        delete clf;
        throw dfe;
    }
    
    m_connectivityDenseScalarFiles.push_back(clf);
}

/**
 * Read a connectivity fiber orientation file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void
Brain::readConnectivityFiberOrientationFile(const AString& filename) throw (DataFileException)
{
    ConnectivityLoaderFile* clf = new ConnectivityLoaderFile();
    
    try {
        if (DataFile::isFileOnNetwork(filename)) {
            clf->setupNetworkFile(filename,
                                  DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY,
                                  m_fileReadingUsername,
                                  m_fileReadingPassword);
        }
        else {
            clf->setupLocalFile(filename,
                                DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY);
        }
        
        //validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        delete clf;
        throw dfe;
    }
    
    m_connectivityFiberOrientationFiles.push_back(clf);
}

/**
 * Read a connectivity time series file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readConnectivityTimeSeriesFile(const AString& filename) throw (DataFileException)
{
    ConnectivityLoaderFile* clf = new ConnectivityLoaderFile();
    
    try {
        if (DataFile::isFileOnNetwork(filename)) {
            clf->setupNetworkFile(filename,
                                  DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES,
                                  m_fileReadingUsername,
                                  m_fileReadingPassword);
        }
        else {
            clf->setupLocalFile(filename, 
                                DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
            clf->loadFrame(0);
        }
        
        validateConnectivityFile(clf);
    }
    catch (const DataFileException& dfe) {
        delete clf;
        throw dfe;
    }
    
    m_connectivityTimeSeriesFiles.push_back(clf);
}

/**
 * Verify the number of nodes in the CIFTI file matches any 
 * surface structures that are currently loaded.
 * @param clf
 *    The CIFTI file.
 * @throw If the number of nodes for a structure in the CIFTI file
 *    does not match the number of nodes in a currently loaded
 *    structure.
 */
void 
Brain::validateConnectivityFile(const ConnectivityLoaderFile* clf) throw (DataFileException)
{
    const int32_t numBrainStructures = getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        const StructureEnum::Enum structure = getBrainStructure(i)->getStructure();
        const int numNodes = getBrainStructure(i)->getNumberOfNodes();
        
        const int numConnNodes = clf->getSurfaceNumberOfNodes(structure);
        if (numConnNodes > 0) {
            if (numNodes != numConnNodes) {
                AString msg = ("The CIFTI file "
                               + clf->getFileNameNoPath()
                               + " contains "
                               + AString::number(numConnNodes)
                               + " for structure "
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
 * Read a palette file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readPaletteFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Reading not implemented for: palette");
}

/**
 * Read a scene file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readSceneFile(const AString& filename) throw (DataFileException)
{
    SceneFile* sf = new SceneFile;
    try {
        sf->readFile(filename);
        m_sceneFiles.push_back(sf);
    }
    catch (DataFileException& dfe) {
        delete sf;
        throw dfe;
    }
}

/**
 * Get mappable connectivity files of ALL Types
 * @param connectivityFilesOut
 *   Contains all mappable connectivity files on exit.
 */
void 
Brain::getMappableConnectivityFilesOfAllTypes(std::vector<ConnectivityLoaderFile*>& connectivityFilesOfAllTypes) const
{
    connectivityFilesOfAllTypes.clear();
    connectivityFilesOfAllTypes.insert(connectivityFilesOfAllTypes.end(),
                                       m_connectivityDenseFiles.begin(),
                                       m_connectivityDenseFiles.end());
    connectivityFilesOfAllTypes.insert(connectivityFilesOfAllTypes.end(),
                                       m_connectivityDenseLabelFiles.begin(),
                                       m_connectivityDenseLabelFiles.end());
//    connectivityFilesOfAllTypes.insert(connectivityFilesOfAllTypes.end(),
//                                       m_connectivityDenseScalarFiles.begin(),
//                                       m_connectivityDenseScalarFiles.end());
    connectivityFilesOfAllTypes.insert(connectivityFilesOfAllTypes.end(),
                                       m_connectivityTimeSeriesFiles.begin(),
                                       m_connectivityTimeSeriesFiles.end());
}

/**
 * @return Number of connectivity dense files.
 */
int32_t 
Brain::getNumberOfConnectivityDenseFiles() const
{
    return m_connectivityDenseFiles.size(); 
}

/**
 * Get the connectivity dense file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense file at index.
 */
ConnectivityLoaderFile* 
Brain::getConnectivityDenseFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityDenseFiles, indx);
    return m_connectivityDenseFiles[indx];
}

/**
 * Get the connectivity dense file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity dense file at index.
 */
const ConnectivityLoaderFile* 
Brain::getConnectivityDenseFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityDenseFiles, indx);
    return m_connectivityDenseFiles[indx];
}

/**
 * Get ALL connectivity dense files.
 * @param connectivityFilesOut
 *   Contains all connectivity dense files on exit.
 */
void 
Brain::getConnectivityDenseFiles(std::vector<ConnectivityLoaderFile*>& connectivityDenseFilesOut) const
{
    connectivityDenseFilesOut = m_connectivityDenseFiles;
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
ConnectivityLoaderFile*
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
const ConnectivityLoaderFile*
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
Brain::getConnectivityDenseLabelFiles(std::vector<ConnectivityLoaderFile*>& connectivityDenseLabelFilesOut) const
{
    connectivityDenseLabelFilesOut = m_connectivityDenseLabelFiles;
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
CiftiScalarFile*
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
const CiftiScalarFile*
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
Brain::getConnectivityDenseScalarFiles(std::vector<CiftiScalarFile*>& connectivityDenseScalarFilesOut) const
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
ConnectivityLoaderFile*
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
const ConnectivityLoaderFile*
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
Brain::getConnectivityFiberOrientationFiles(std::vector<ConnectivityLoaderFile*>& connectivityFiberOrientationFilesOut) const
{
    connectivityFiberOrientationFilesOut = m_connectivityFiberOrientationFiles;
}

/**
 * @return Number of connectivity time series files.
 */
int32_t 
Brain::getNumberOfConnectivityTimeSeriesFiles() const
{
    return m_connectivityTimeSeriesFiles.size(); 
}

/**
 * Get the connectivity time series file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity file at index.
 */
ConnectivityLoaderFile* 
Brain::getConnectivityTimeSeriesFile(int32_t indx)
{
    CaretAssertVectorIndex(m_connectivityTimeSeriesFiles, indx);
    return m_connectivityTimeSeriesFiles[indx];
}

/**
 * Get the connectivity time series file at the given index.
 * @param indx
 *    Index of file.
 * @return Conectivity file at index.
 */
const ConnectivityLoaderFile* 
Brain::getConnectivityTimeSeriesFile(int32_t indx) const
{
    CaretAssertVectorIndex(m_connectivityTimeSeriesFiles, indx);
    return m_connectivityTimeSeriesFiles[indx];
}

/**
 * Get ALL connectivity time series files.
 * @param connectivityTimeSeriesFilesOut
 *   Contains all connectivity time series files on exit.
 */
void 
Brain::getConnectivityTimeSeriesFiles(std::vector<ConnectivityLoaderFile*>& connectivityTimeSeriesFilesOut) const
{
    connectivityTimeSeriesFilesOut = m_connectivityTimeSeriesFiles;
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
 * @return Name of the spec file.
 */
AString 
Brain::getSpecFileName() const
{
    return m_specFileName;
}

/**
 * Is the spec file valid?
 *
 * The spec file is valid when:
 *    (1) The spec file's name is a path on the network (http..., etc)
 *    (2) The path is not on the network and the file exists.
 * 
 * @return Validity of spec file.
 */
bool
Brain::isSpecFileValid() const
{
    bool valid = false;
    
    if (DataFile::isFileOnNetwork(m_specFileName)) {
        valid = true;
    }
    else {
        FileInformation fileInfo(m_specFileName);
        valid = fileInfo.exists();
    }
    
    return valid;
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
 * Update the volume slice controller.
 */
void 
Brain::updateVolumeSliceController()
{
    bool isValid = false;
    if (getNumberOfVolumeFiles() > 0) {
        isValid = true;
    }
    
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
 * Process a read data file event.
 * @param readDataFileEvent
 *   Event describing file for reading and may be updated with error messages.
 */
void 
Brain::processReadDataFileEvent(EventDataFileRead* readDataFileEvent)
{
    m_fileReadingUsername = readDataFileEvent->getUsername();
    m_fileReadingPassword = readDataFileEvent->getPassword();
    
    const AString filename = readDataFileEvent->getDataFileName();
    const DataFileTypeEnum::Enum dataFileType = readDataFileEvent->getDataFileType();
    const StructureEnum::Enum structure = readDataFileEvent->getStructure();
    
    try {
        readDataFile(dataFileType,
                           structure,
                           filename,
                           readDataFileEvent->isAddDataFileToSpecFile());
    }
    catch (const DataFileException& e) {
        readDataFileEvent->setErrorMessage(e.whatString());
        readDataFileEvent->setErrorInvalidStructure(e.isErrorInvalidStructure());
    }    
    
    m_fileReadingUsername = "";
    m_fileReadingPassword = "";
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
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
Brain::readDataFile(const DataFileTypeEnum::Enum dataFileType,
                    const StructureEnum::Enum structure,
                    const AString& dataFileNameIn,
                    const bool addDataFileToSpecFile) throw (DataFileException)
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
        
        CaretDataFile* caretDataFileRead = NULL;
        
        ElapsedTimer et;
        et.start();
        
        switch (dataFileType) {
            case DataFileTypeEnum::BORDER:
                readBorderFile(dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                readConnectivityDenseFile(dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                readConnectivityDenseLabelFile(dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                readConnectivityDenseScalarFile(dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                readConnectivityTimeSeriesFile(dataFileName);
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                readConnectivityFiberOrientationFile(dataFileName);
                break;
            case DataFileTypeEnum::FOCI:
                readFociFile(dataFileName);
                break;
            case DataFileTypeEnum::LABEL:
                caretDataFileRead = readLabelFile(dataFileName, structure);
                break;
            case DataFileTypeEnum::METRIC:
                caretDataFileRead = readMetricFile(dataFileName, structure);
                break;
            case DataFileTypeEnum::PALETTE:
                readPaletteFile(dataFileName);
                break;
            case DataFileTypeEnum::RGBA:
                caretDataFileRead = readRgbaFile(dataFileName, structure);
                break;
            case DataFileTypeEnum::SCENE:
                readSceneFile(dataFileName);
                break;
            case DataFileTypeEnum::SPECIFICATION:
                CaretLogSevere("PROGRAM ERROR: Reading spec file should never call Brain::readDataFile()");
                throw DataFileException("PROGRAM ERROR: Reading spec file should never call Brain::readDataFile()");
                break;
            case DataFileTypeEnum::SURFACE:
                caretDataFileRead = readSurfaceFile(dataFileName, structure);
                break;
            case DataFileTypeEnum::UNKNOWN:
                throw DataFileException("Unable to read files of type");
                break;
            case DataFileTypeEnum::VOLUME:
                readVolumeFile(dataFileName);
                break;
        }
        
        AString msg = ("Time to read "
                       + dataFileName
                       + " was "
                       + AString::number(et.getElapsedTimeSeconds())
                       + " seconds.");
        CaretLogInfo(msg);
        
        AString addToSpecFileErrorMessage;
        if (addDataFileToSpecFile) {
            if (m_specFileName.isEmpty() == false) {
                FileInformation specFileInfo(m_specFileName);
                QString relativePathDataFileName = SystemUtilities::relativePath(dataFileName,
                                                                                 specFileInfo.getPathName());
                
                StructureEnum::Enum dataFileStructure = structure;
                if (dataFileStructure == StructureEnum::INVALID) {
                    if (caretDataFileRead != NULL) {
                        dataFileStructure = caretDataFileRead->getStructure();
                    }
                    else {
                        dataFileStructure = StructureEnum::ALL;
                    }
                }
                try {
                    SpecFile sf;
                    sf.readFile(m_specFileName);
                    sf.addDataFile(dataFileType,
                                   dataFileStructure,
                                   relativePathDataFileName,
                                   true);
                    sf.writeFile(m_specFileName);
                }
                catch (const DataFileException& e) {
                    addToSpecFileErrorMessage = ("Unable to add file \""
                                                 + dataFileName
                                                 + "\" to SpecFile \""
                                                 + m_specFileName
                                                 + "\", Error:"
                                                 + e.whatString());
                    CaretLogWarning(addToSpecFileErrorMessage);
                }
            }
        }
        
        updateVolumeSliceController();
        updateWholeBrainController();
        updateSurfaceMontageController();
        
        if (addToSpecFileErrorMessage.isEmpty() == false) {
            throw DataFileException(addToSpecFileErrorMessage);
        }
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
    
    m_isSpecFileBeingRead = true;
    
    m_fileReadingUsername = readSpecFileDataFilesEvent->getUsername();
    m_fileReadingPassword = readSpecFileDataFilesEvent->getPassword();
    
    FileInformation fileInfo(sf->getFileName());
    setCurrentDirectory(fileInfo.getPathName());
    
    const int32_t numFileGroups = sf->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0; ig < numFileGroups; ig++) {
        const SpecFileDataFileTypeGroup* group = sf->getDataFileTypeGroup(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        const int32_t numFiles = group->getNumberOfFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            const SpecFileDataFile* fileInfo = group->getFileInformation(iFile);
            if (fileInfo->isSelected()) {
                const AString filename = fileInfo->getFileName();
                const StructureEnum::Enum structure = fileInfo->getStructure();
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
            }
        }
    }
    
    const AString specFileName = sf->getFileName();
    if (DataFile::isFileOnNetwork(specFileName)) {
        m_specFileName = specFileName;
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->addToPreviousSpecFiles(specFileName);
    }
    else {
        FileInformation specFileInfo(specFileName);
        if (specFileInfo.exists()) {
            m_specFileName = specFileName;
        }
        else {
            m_specFileName = "";
        }
        if (specFileInfo.isAbsolute()) {
            CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
            prefs->addToPreviousSpecFiles(specFileName);
        }
    }
    
    if (errorMessage.isEmpty() == false) {
        readSpecFileDataFilesEvent->setErrorMessage(errorMessage);
    }
    
    if (m_paletteFile != NULL) {
        delete m_paletteFile;
    }
    m_paletteFile = new PaletteFile();
    m_paletteFile->setFileName(updateFileNameForWriting(m_paletteFile->getFileNameNoPath()));
    m_paletteFile->clearModified();
    
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
    m_fileReadingUsername = "";
    m_fileReadingPassword = "";
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
     * Check to see if existing spec file exists
     */
    FileInformation specFileInfo(m_specFileName);
    const bool specFileValid = specFileInfo.exists();
    
    /*
     * Apply spec file pulled from scene
     */
    m_isSpecFileBeingRead = true;
    m_specFileName = specFileToLoad->getFileName();   
    
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
    
    const int32_t numFileGroups = specFileToLoad->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0; ig < numFileGroups; ig++) {
        SpecFileDataFileTypeGroup* group = specFileToLoad->getDataFileTypeGroup(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        const int32_t numFiles = group->getNumberOfFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            SpecFileDataFile* fileInfo = group->getFileInformation(iFile);
            if (fileInfo->isSelected()) {
                AString filename = fileInfo->getFileName();
                const StructureEnum::Enum structure = fileInfo->getStructure();
                
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
    else if (event->getEventType() == EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET) {
        EventCaretMappableDataFilesGet* dataFilesEvent =
        dynamic_cast<EventCaretMappableDataFilesGet*>(event);
        CaretAssert(dataFilesEvent);
        

        for (std::vector<ConnectivityLoaderFile*>::iterator icf = m_connectivityDenseFiles.begin();
             icf != m_connectivityDenseFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<ConnectivityLoaderFile*>::iterator icf = m_connectivityDenseLabelFiles.begin();
             icf != m_connectivityDenseLabelFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<CiftiScalarFile*>::iterator icf = m_connectivityDenseScalarFiles.begin();
             icf != m_connectivityDenseScalarFiles.end();
             icf++) {
            dataFilesEvent->addFile(*icf);
        }
        
        for (std::vector<ConnectivityLoaderFile*>::iterator ictsf = m_connectivityTimeSeriesFiles.begin();
             ictsf != m_connectivityTimeSeriesFiles.end();
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
 * @return The connectivity loader manager.
 */
ConnectivityLoaderManager* 
Brain::getConnectivityLoaderManager()
{
    return m_connectivityLoaderManager;
}

/**
 * @return The connectivity loader manager.
 */
const ConnectivityLoaderManager* 
Brain::getConnectivityLoaderManager() const
{
    return m_connectivityLoaderManager;
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
 * Get all loaded data files.
 * @param allDataFilesOut
 *    Data files are loaded into this parameter.
 */
void 
Brain::getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut) const
{
    allDataFilesOut.clear();
    
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
                           m_connectivityDenseFiles.begin(),
                           m_connectivityDenseFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityDenseLabelFiles.begin(),
                           m_connectivityDenseLabelFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityDenseScalarFiles.begin(),
                           m_connectivityDenseScalarFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityFiberOrientationFiles.begin(),
                           m_connectivityFiberOrientationFiles.end());
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_connectivityTimeSeriesFiles.begin(),
                           m_connectivityTimeSeriesFiles.end());
    
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
 * Are any data files modified?
 * @param excludeConnectivityFiles
 *    If true, do not check connectivity files for modification status
 * @param excludeSceneFiles
 *    If true, do not check scene files for modification status
 */
bool
Brain::areFilesModified(const bool excludeConnectivityFiles,
                        const bool excludeSceneFiles)
{
    std::vector<CaretDataFile*> dataFiles;
    getAllDataFiles(dataFiles);
    
    for (std::vector<CaretDataFile*>::iterator iter = dataFiles.begin();
         iter != dataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        
        /**
         * Do not check connectivity files for modified status
         */
        bool checkIfModified = true;
    
        if (DataFileTypeEnum::isConnectivityDataType(cdf->getDataFileType())) {
            if (excludeConnectivityFiles) {
                checkIfModified = false;
            }
        }
    
        switch (cdf->getDataFileType()) {
            case DataFileTypeEnum::SCENE:
                if (excludeSceneFiles) {
                    checkIfModified = false;
                }
                break;
            default:
                break;
        }
        
        if (checkIfModified) {
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
 * @param isAddToSpecFile
 *    If true, add the data file to the spec file.
 * @return
 *    true if file was written, else false.
 * @throw
 *    DataFileException if there was an error writing the file.
 */
void 
Brain::writeDataFile(CaretDataFile* caretDataFile,
                     const bool isAddToSpecFile) throw (DataFileException)
{
    /*
     * If file is relative path, update path using current directory
     */
    AString dataFileName = caretDataFile->getFileName();
    dataFileName = updateFileNameForWriting(dataFileName);
    caretDataFile->setFileName(dataFileName);

    caretDataFile->writeFile(caretDataFile->getFileName());
    caretDataFile->clearModified();
    
    if (isAddToSpecFile) {
        if (m_specFileName.isEmpty() == false) {
            try {
                SpecFile sf;
                sf.readFile(m_specFileName);
                sf.addDataFile(caretDataFile->getDataFileType(), 
                               caretDataFile->getStructure(), 
                               caretDataFile->getFileName(),
                               true);
                sf.writeFile(m_specFileName);
            }
            catch (const DataFileException& e) {
                const AString msg = ("Unable to add file \""
                                             + dataFileName
                                             + "\" to SpecFile \""
                                             + m_specFileName
                                             + "\", Error:"
                                             + e.whatString());
                CaretLogWarning(msg);
                throw DataFileException(e);
            }
        }
    }
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
    
    std::vector<ConnectivityLoaderFile*>::iterator connIterator = std::find(m_connectivityDenseFiles.begin(),
                                                                            m_connectivityDenseFiles.end(),
                                                                            caretDataFile);
    if (connIterator != m_connectivityDenseFiles.end()) {
        ConnectivityLoaderFile* connFile = *connIterator;
        delete connFile;
        m_connectivityDenseFiles.erase(connIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }

    std::vector<ConnectivityLoaderFile*>::iterator connLabelIterator = std::find(m_connectivityDenseLabelFiles.begin(),
                                                                            m_connectivityDenseLabelFiles.end(),
                                                                            caretDataFile);
    if (connLabelIterator != m_connectivityDenseLabelFiles.end()) {
        ConnectivityLoaderFile* connFile = *connLabelIterator;
        delete connFile;
        m_connectivityDenseLabelFiles.erase(connLabelIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<CiftiScalarFile*>::iterator connScalarIterator = std::find(m_connectivityDenseScalarFiles.begin(),
                                                                            m_connectivityDenseScalarFiles.end(),
                                                                            caretDataFile);
    if (connScalarIterator != m_connectivityDenseScalarFiles.end()) {
        CiftiScalarFile* connFile = *connScalarIterator;
        delete connFile;
        m_connectivityDenseScalarFiles.erase(connScalarIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<ConnectivityLoaderFile*>::iterator connFiberOrientationIterator = std::find(m_connectivityFiberOrientationFiles.begin(),
                                                                            m_connectivityFiberOrientationFiles.end(),
                                                                            caretDataFile);
    if (connFiberOrientationIterator != m_connectivityFiberOrientationFiles.end()) {
        ConnectivityLoaderFile* connFile = *connFiberOrientationIterator;
        delete connFile;
        m_connectivityFiberOrientationFiles.erase(connFiberOrientationIterator);
        wasRemoved = true;
        caretDataFile = NULL;
    }
    
    std::vector<ConnectivityLoaderFile*>::iterator timeIterator = std::find(m_connectivityTimeSeriesFiles.begin(),
                                                                            m_connectivityTimeSeriesFiles.end(),
                                                                            caretDataFile);
    if (timeIterator != m_connectivityTimeSeriesFiles.end()) {
        ConnectivityLoaderFile* timeFile = *timeIterator;
        delete timeFile;
        m_connectivityTimeSeriesFiles.erase(timeIterator);
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
        updateVolumeSliceController();
        updateWholeBrainController();
        updateSurfaceMontageController();
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
 * @return The information display properties.
 */
DisplayPropertiesInformation*
Brain::getDisplayPropertiesInformation()
{
    return m_displayPropertiesInformation;
}

/**
 * @return The information display properties.
 */
const DisplayPropertiesInformation*
Brain::getDisplayPropertiesInformation() const
{
    return m_displayPropertiesInformation;
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
    
    if (isSaveSpecFile) {
        std::vector<CaretDataFile*> allFiles;
        getAllDataFiles(allFiles);
        SpecFile sf;
        sf.setFileName(m_specFileName);
        for (std::vector<CaretDataFile*>::iterator iter = allFiles.begin();
             iter != allFiles.end();
             iter++) {
            CaretDataFile* cdf = *iter;
            sf.addDataFile(cdf->getDataFileType(), 
                           cdf->getStructure(), 
                           cdf->getFileName(), 
                           true);
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
    sceneClass->addClass(m_connectivityLoaderManager->saveToScene(sceneAttributes,
                                                                  "m_connectivityLoaderManager"));
    
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
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
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
    
    /*
     * Restore connectivity data
     */
    m_connectivityLoaderManager->restoreFromScene(sceneAttributes,
                                                  sceneClass->getClass("m_connectivityLoaderManager"));

    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
}

/**
 * @return The identification manager.
 */
IdentificationManager*
Brain::getIdentificationManager()
{
    return this->m_identificationManager;
}


