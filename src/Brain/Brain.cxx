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

#include "CaretAssert.h"

#include "Brain.h"
#include "BrainStructure.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventDataFileRead.h"
#include "EventModelDisplayControllerAdd.h"
#include "EventModelDisplayControllerDelete.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "MetricFile.h"
#include "ModelDisplayControllerVolume.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "LabelFile.h"
#include "PaletteFile.h"
#include "RgbaFile.h"
#include "SessionManager.h"
#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "Surface.h"
#include "SystemUtilities.h"
#include "VolumeFile.h"
#include <algorithm>

using namespace caret;

/**
 *  Constructor.
 */
Brain::Brain()
{
    this->connectivityLoaderManager = new ConnectivityLoaderManager(this);
    this->paletteFile = new PaletteFile();
    this->specFile = new SpecFile();
    this->volumeSliceController = NULL;
    this->wholeBrainController = NULL;
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES);
}

/**
 * Destructor.
 */
Brain::~Brain()
{
    EventManager::get()->removeAllEventsFromListener(this);

    this->resetBrain();
    delete this->connectivityLoaderManager;
    delete this->paletteFile;
    delete this->specFile;
    if (this->volumeSliceController != NULL) {
        delete this->volumeSliceController;
    }
    if (this->wholeBrainController != NULL) {
        delete this->wholeBrainController;
    }
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
    return static_cast<int>(this->brainStructures.size());
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
    this->brainStructures.push_back(brainStructure);
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
    CaretAssertVectorIndex(this->brainStructures, indx);
    return this->brainStructures[indx];
    
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
    for (std::vector<BrainStructure*>::iterator iter = this->brainStructures.begin();
         iter != this->brainStructures.end();
         iter++) {
        BrainStructure* bs = *iter;
        if (bs->getStructure() == structure) {
            return bs;
        }
    }
    
    if (createIfNotFound) {
        BrainStructure* bs = new BrainStructure(this, structure);
        this->brainStructures.push_back(bs);
        return bs;
    }
    
    return NULL;
}

/**
 * Reset the brain structure.
 */
void 
Brain::resetBrain()
{
    int num = this->getNumberOfBrainStructures();
    for (int32_t i = 0; i < num; i++) {
        delete this->brainStructures[i];
    }
    
    for (std::vector<VolumeFile*>::iterator vfi = this->volumeFiles.begin();
         vfi != this->volumeFiles.end();
         vfi++) {
        VolumeFile* vf = *vfi;
        delete vf;
    }
    this->volumeFiles.clear();
    
    this->brainStructures.clear();
    this->paletteFile->clear();
    
    this->connectivityLoaderManager->reset();
    
    this->specFile->clear();
    
    this->updateVolumeSliceController();
    this->updateWholeBrainController();
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
 */
void 
Brain::readSurfaceFile(const AString& filename,
                       const StructureEnum::Enum structureIn) throw (DataFileException)
{
    Surface* surface = new Surface();
    surface->readFile(filename);
    
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
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = this->getBrainStructure(structure, true);
    if (bs != NULL) {
        bs->addSurface(surface);
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
 */
void 
Brain::readLabelFile(const AString& filename,
                     const StructureEnum::Enum structureIn) throw (DataFileException)
{
    LabelFile* labelFile = new LabelFile();
    labelFile->readFile(filename);
    
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
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = this->getBrainStructure(structure, false);
    if (bs != NULL) {
        try {
            bs->addLabelFile(labelFile);
        }
        catch (DataFileException e) {
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
 */
void 
Brain::readMetricFile(const AString& filename,
                      const StructureEnum::Enum structureIn) throw (DataFileException)
{
    MetricFile* metricFile = new MetricFile();
    metricFile->readFile(filename);
    
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
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = this->getBrainStructure(structure, false);
    if (bs != NULL) {
        try {
            bs->addMetricFile(metricFile);
        }
        catch (DataFileException e) {
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
 */
void 
Brain::readRgbaFile(const AString& filename,
                    const StructureEnum::Enum structureIn) throw (DataFileException)
{
    RgbaFile* rgbaFile = new RgbaFile();
    rgbaFile->readFile(filename);
    
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
        CaretLogThrowing(e);
        throw e;
    }
    
    BrainStructure* bs = this->getBrainStructure(structure, false);
    if (bs != NULL) {
        try {
            bs->addRgbaFile(rgbaFile);
        }
        catch (DataFileException e) {
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
    this->volumeFiles.push_back(vTest);
*/
    VolumeFile* vf = new VolumeFile();
    try {
        vf->readFile(filename);
    }
    catch (DataFileException e) {
        delete vf;
        throw e;
    }
    vf->clearModified();
    this->volumeFiles.push_back(vf);
}

/**
 * @return  Number of volume files.
 */
int32_t 
Brain::getNumberOfVolumeFiles() const
{
    return this->volumeFiles.size();
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
    CaretAssertVectorIndex(this->volumeFiles, volumeFileIndex);
    return this->volumeFiles[volumeFileIndex];
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
    CaretAssertVectorIndex(this->volumeFiles, volumeFileIndex);
    return this->volumeFiles[volumeFileIndex];
}

/**
 * Read a border projection file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readBorderProjectionFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Reading not implemented for: border projection");
}

/**
 * Read a connectivity file.
 *
 * @param filename
 *    Name of the file.
 * @param connectivityFileType
 *    Type of connectivity file to read.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readConnectivityFile(const AString& filename,
                            const DataFileTypeEnum::Enum connectivityFileType) throw (DataFileException)
{
    this->connectivityLoaderManager->addConnectivityLoaderFile(filename,
                                                               connectivityFileType);
}

/**
 * Read a foci projection file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readFociProjectionFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Reading not implemented for: foci projection");
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
Brain::readSceneFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Reading not implemented for: scene");
}

/*
 * @return The palette file.
 */
PaletteFile* 
Brain::getPaletteFile()
{
    return this->paletteFile;
}

/*
 * @return The palette file.
 */
const PaletteFile* 
Brain::getPaletteFile() const
{
    return this->paletteFile;
}

/*
 * @return The spec file.
 */
SpecFile* 
Brain::getSpecFile()
{
    return this->specFile;
}

/**
 * Update the volume slice controller.
 */
void 
Brain::updateVolumeSliceController()
{
    bool isValid = false;
    if (this->getNumberOfVolumeFiles() > 0) {
        isValid = true;
    }
    
    if (isValid) {
        if (this->volumeSliceController == NULL) {
            this->volumeSliceController = new ModelDisplayControllerVolume(this);
            EventModelDisplayControllerAdd eventAddModel(this->volumeSliceController);
            EventManager::get()->sendEvent(eventAddModel.getPointer());
        }
    }
    else {
        if (this->volumeSliceController != NULL) {
            EventModelDisplayControllerDelete eventDeleteModel(this->volumeSliceController);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete this->volumeSliceController;
            this->volumeSliceController = NULL;
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
    if (this->getNumberOfBrainStructures() > 0) {
        isValid = true;
    }
     
    if (isValid) {
        if (this->wholeBrainController == NULL) {
            this->wholeBrainController = new ModelDisplayControllerWholeBrain(this);
            EventModelDisplayControllerAdd eventAddModel(this->wholeBrainController);
            EventManager::get()->sendEvent(eventAddModel.getPointer());
        }
    }
    else {
        if (this->wholeBrainController != NULL) {
            EventModelDisplayControllerDelete eventDeleteModel(this->wholeBrainController);
            EventManager::get()->sendEvent(eventDeleteModel.getPointer());
            delete this->wholeBrainController;
            this->wholeBrainController = NULL;
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
    const AString filename = readDataFileEvent->getDataFileName();
    const DataFileTypeEnum::Enum dataFileType = readDataFileEvent->getDataFileType();
    
    try {
        this->readDataFile(dataFileType,
                           StructureEnum::INVALID,
                           filename);
    }
    catch (DataFileException e) {
        readDataFileEvent->setErrorMessage(e.whatString());
    }    
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
                    const AString& dataFileNameIn) throw (DataFileException)
{
    const AString dataFileName = this->updateFileNameForReading(dataFileNameIn);
    
    if (dataFileName.contains("://") == false) {
        FileInformation fileInfo(dataFileName);
        if (fileInfo.exists() == false) {
            throw DataFileException(dataFileName
                                    + " does not exist!");
        }
    }
    
    switch (dataFileType) {
        case DataFileTypeEnum::BORDER_PROJECTION:
            this->readBorderProjectionFile(dataFileName);
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            this->readConnectivityFile(dataFileName, dataFileType);
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            this->readConnectivityFile(dataFileName, dataFileType);
            break;
        case DataFileTypeEnum::FOCI_PROJECTION:
            this->readFociProjectionFile(dataFileName);
            break;
        case DataFileTypeEnum::LABEL:
            this->readLabelFile(dataFileName, structure);
            break;
        case DataFileTypeEnum::METRIC:
            this->readMetricFile(dataFileName, structure);
            break;
        case DataFileTypeEnum::PALETTE:
            this->readPaletteFile(dataFileName);
            break;
        case DataFileTypeEnum::RGBA:
            this->readRgbaFile(dataFileName, structure);
            break;
        case DataFileTypeEnum::SCENE:
            this->readSceneFile(dataFileName);
            break;
        case DataFileTypeEnum::SPECIFICATION:
            CaretLogSevere("PROGRAM ERROR: Reading spec file should never call Brain::readDataFile()");
            throw DataFileException("PROGRAM ERROR: Reading spec file should never call Brain::readDataFile()");
            break;
        case DataFileTypeEnum::SURFACE:
            this->readSurfaceFile(dataFileName, structure);
            break;
        case DataFileTypeEnum::UNKNOWN:
            throw DataFileException("Unable to read files of type");
            break;
        case DataFileTypeEnum::VOLUME:
            this->readVolumeFile(dataFileName);
            break;
    }    
    
    this->updateVolumeSliceController();
    this->updateWholeBrainController();
}

/**
 * Load the data files selected in a spec file.
 * @param readSpecFileDataFilesEvent
 *    Event containing the spec file.
 */
void 
Brain::loadFilesSelectedInSpecFile(EventSpecFileReadDataFiles* readSpecFileDataFilesEvent)
{
    AString errorMessage;
    
    SpecFile* sf = readSpecFileDataFilesEvent->getSpecFile();
    CaretAssert(sf);
    
    this->resetBrain();
    
    if (this->specFile != NULL) {
        delete this->specFile;
    }
    this->specFile = new SpecFile(*sf);
    
    FileInformation fileInfo(sf->getFileName());
    this->setCurrentDirectory(fileInfo.getPathName());
    
    
    const int32_t numFileGroups = sf->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0; ig < numFileGroups; ig++) {
        SpecFileDataFileTypeGroup* group = sf->getDataFileTypeGroup(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        const int32_t numFiles = group->getNumberOfFiles();
        for (int32_t iFile = 0; iFile < numFiles; iFile++) {
            SpecFileDataFile* fileInfo = group->getFileInformation(iFile);
            if (fileInfo->isSelected()) {
                const AString filename = fileInfo->getFileName();
                const StructureEnum::Enum structure = fileInfo->getStructure();
                try {
                    this->readDataFile(dataFileType, 
                                       structure, 
                                       filename);
                }
                catch (DataFileException e) {
                    if (errorMessage.isEmpty() == false) {
                        errorMessage += "\n";
                    }
                    errorMessage += e.whatString();
                }
            }
        }
    }
    
    const AString specFileName = this->specFile->getFileName();
    FileInformation specFileInfo(specFileName);
    if (specFileInfo.isAbsolute()) {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->addToPreviousSpecFiles(specFileName);
    }
    
    if (errorMessage.isEmpty() == false) {
        readSpecFileDataFilesEvent->setErrorMessage(errorMessage);
    }
}

/**
 * Given a file name, if the path to the file is relative
 * prepend it with the current directory for this brain.
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
    FileInformation fileInfo(filename);
    if (fileInfo.isAbsolute()) {
        return filename;
    }
    
    if (currentDirectory.isEmpty()) {
        return filename;
    }
    
    FileInformation pathFileInfo(this->currentDirectory, filename);
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
            this->processReadDataFileEvent(readDataFileEvent);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET) {
        EventCaretMappableDataFilesGet* dataFilesEvent =
        dynamic_cast<EventCaretMappableDataFilesGet*>(event);
        CaretAssert(dataFilesEvent);
        
        for (std::vector<VolumeFile*>::iterator volumeIter = this->volumeFiles.begin();
             volumeIter != this->volumeFiles.end();
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
            this->loadFilesSelectedInSpecFile(readSpecFileDataFilesEvent);
        }
    }
}

/**
 * @return The connectivity loader manager.
 */
ConnectivityLoaderManager* 
Brain::getConnectivityLoaderManager()
{
    return this->connectivityLoaderManager;
}

/**
 * @return The connectivity loader manager.
 */
const ConnectivityLoaderManager* 
Brain::getConnectivityLoaderManager() const
{
    return this->connectivityLoaderManager;
}

/**
 * @return  The current directory.
 */
AString 
Brain::getCurrentDirectory() const
{
    if (this->currentDirectory.isEmpty()) {
        this->currentDirectory = SystemUtilities::systemCurrentDirectory();
    }
    return this->currentDirectory;
}

/**
 * Set the current directory.
 * @param currentDirectory
 *    New value for current directory.
 */
void 
Brain::setCurrentDirectory(const AString& currentDirectory)
{
    this->currentDirectory = currentDirectory;
}

/**
 * Get all loaded data files.
 * @param allDataFilesOut
 *    Data files are loaded into this parameter.
 */
void 
Brain::getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut)
{
    allDataFilesOut.clear();
    
    const int32_t numBrainStructures = this->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        this->getBrainStructure(i)->getAllDataFiles(allDataFilesOut);
    }
    
    allDataFilesOut.push_back(this->paletteFile);
    
    allDataFilesOut.insert(allDataFilesOut.end(),
                           this->volumeFiles.begin(),
                           this->volumeFiles.end());    
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
    caretDataFile->writeFile(caretDataFile->getFileName());
    caretDataFile->clearModified();
    
    this->specFile->addDataFile(caretDataFile->getDataFileType(), 
                                caretDataFile->getStructure(), 
                                caretDataFile->getFileName());
    this->specFile->writeFile(this->specFile->getFileName());
}

/**
 * Remove a data file from memory (does NOT delete file on disk.)
 * @param caretDataFile
 *    Data file to remove.
 * @return
 *    true if file was written, else false.
 */
bool 
Brain::removeDataFile(CaretDataFile* caretDataFile)
{
    const int32_t numBrainStructures = this->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        if (this->getBrainStructure(i)->removeDataFile(caretDataFile)) {
            return true;
        }
    }
    
    if (this->paletteFile == caretDataFile) {
        throw DataFileException("Cannot remove PaletteFile.");
    }
    
    std::vector<VolumeFile*>::iterator volumeIterator = 
    std::find(this->volumeFiles.begin(),
              this->volumeFiles.end(),
              caretDataFile);
    if (volumeIterator != this->volumeFiles.end()) {
        delete caretDataFile;
        this->volumeFiles.erase(volumeIterator);
        return true;
    }
    
    return false;
}



