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
#include "EventDataFileRead.h"
#include "EventManager.h"
#include "MetricFile.h"
#include "LabelFile.h"
#include "PaletteFile.h"
#include "RgbaFile.h"
#include "SpecFile.h"
#include "Surface.h"
#include <algorithm>

using namespace caret;

/**
 *  Constructor.
 */
Brain::Brain()
{
    this->paletteFile = new PaletteFile();
    this->specFile = new SpecFile();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DATA_FILE_READ);
}

/**
 * Destructor.
 */
Brain::~Brain()
{
    EventManager::get()->removeAllEventsFromListener(this);

    this->resetBrain();
    delete this->paletteFile;
    delete this->specFile;
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
    this->brainStructures.clear();
    this->paletteFile->clear();
}

/**
 * Read a surface file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readSurfaceFile(const AString& filename) throw (DataFileException)
{
    Surface* surface = new Surface();
    surface->readFile(filename);
    
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
}

/**
 * Read a label file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readLabelFile(const AString& filename) throw (DataFileException)
{
    LabelFile* labelFile = new LabelFile();
    labelFile->readFile(filename);
    
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
}

/**
 * Read a metric file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readMetricFile(const AString& filename) throw (DataFileException)
{
    MetricFile* metricFile = new MetricFile();
    metricFile->readFile(filename);
    
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
}

/**
 * Read an RGBA file.
 *
 * @param filename
 *    Name of the file.
 * @throws DataFileException
 *    If reading failed.
 */
void 
Brain::readRgbaFile(const AString& filename) throw (DataFileException)
{
    RgbaFile* rgbaFile = new RgbaFile();
    rgbaFile->readFile(filename);
    
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
 * @return The spec file.
 */
SpecFile* 
Brain::getSpecFile()
{
    return this->specFile;
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
 * @param dataFileName
 *    Name of data file to read.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
Brain::readDataFile(const DataFileTypeEnum::Enum dataFileType,
                    const AString& dataFileName) throw (DataFileException)
{
    switch (dataFileType) {
        case DataFileTypeEnum::BORDER_PROJECTION:
            throw DataFileException("Reading not implemented for: border projection");
            break;
        case DataFileTypeEnum::CIFTI:
            throw DataFileException("Reading not implemented for: cifti");
            break;
        case DataFileTypeEnum::FOCI_PROJECTION:
            throw DataFileException("Reading not implemented for: foci projection");
            break;
        case DataFileTypeEnum::LABEL:
            this->readLabelFile(dataFileName);
            break;
        case DataFileTypeEnum::METRIC:
            this->readMetricFile(dataFileName);
            break;
        case DataFileTypeEnum::PALETTE:
            throw DataFileException("Reading not implemented for: palette");
            break;
        case DataFileTypeEnum::RGBA:
            this->readRgbaFile(dataFileName);
            break;
        case DataFileTypeEnum::SCENE:
            throw DataFileException("Reading not implemented for: scene");
            break;
        case DataFileTypeEnum::SPECIFICATION:
            this->specFile->readFile(dataFileName);
            CaretLogSevere(this->specFile->toString());
            break;
        case DataFileTypeEnum::SURFACE_ANATOMICAL:
        case DataFileTypeEnum::SURFACE_INFLATED:
        case DataFileTypeEnum::SURFACE_VERY_INFLATED:
        case DataFileTypeEnum::SURFACE_FLAT:
            this->readSurfaceFile(dataFileName);
            break;
        case DataFileTypeEnum::UNKNOWN:
            throw DataFileException("Unable to read files of type");
            break;
        case DataFileTypeEnum::VOLUME_ANATOMY:
            throw DataFileException("Reading not implemented for: volume anatomy");
            break;
        case DataFileTypeEnum::VOLUME_FUNCTIONAL:
            throw DataFileException("Reading not implemented for: functional");
            break;
        case DataFileTypeEnum::VOLUME_LABEL:
            throw DataFileException("Reading not implemented for: label");
            break;
    }    
}

void 
Brain::loadFilesSelectedInSpecFile(SpecFile* specFile)
{
    
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
        
        readDataFileEvent->setEventProcessed();
        
        /*
         * Make sure event is for this brain
         */
        if (readDataFileEvent->getLoadIntoBrain() == this) {
            this->processReadDataFileEvent(readDataFileEvent);
        }
    }
}


