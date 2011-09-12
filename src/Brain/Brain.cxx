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

#include <iostream>

#include "CaretAssert.h"

#include "Brain.h"
#include "BrainStructure.h"
#include "EventDataFileRead.h"
#include "EventManager.h"
#include "PaletteFile.h"
#include "Surface.h"
#include <algorithm>

using namespace caret;

/**
 *  Constructor.
 */
Brain::Brain()
{
    this->paletteFile = new PaletteFile();
    
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
 * Get a brain structure that uses the specified structure.
 *
 * @param structure
 *    The desired structure.
 * @param createIfNotFound
 *    If there is not a matching brain structure, create one.
 * @return
 *    Pointer to brain structure or NULL if no match..
 */
BrainStructure* 
Brain::getBrainStructure(/*Structure structure,*/
                                  bool createIfNotFound)
{
    if (this->brainStructures.empty()) {
        BrainStructure* bs = new BrainStructure(this);
        this->addBrainStructure(bs);
    }
    return this->getBrainStructure(0);
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
    Surface* s = new Surface();
    s->readFile(filename);
    
    BrainStructure* bs = this->getBrainStructure(true);
    if (bs != NULL) {
        bs->addSurface(s);
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
        switch (dataFileType) {
            case DataFileTypeEnum::BORDER_PROJECTION:
                readDataFileEvent->setErrorMessage("Reading not implemented for: border projection");
                break;
            case DataFileTypeEnum::CIFTI:
                readDataFileEvent->setErrorMessage("Reading not implemented for: cifti");
                break;
            case DataFileTypeEnum::FOCI_PROJECTION:
                readDataFileEvent->setErrorMessage("Reading not implemented for: foci projection");
                break;
            case DataFileTypeEnum::LABEL:
                readDataFileEvent->setErrorMessage("Reading not implemented for: label");
                break;
            case DataFileTypeEnum::METRIC:
                readDataFileEvent->setErrorMessage("Reading not implemented for: metric");
                break;
            case DataFileTypeEnum::PALETTE:
                readDataFileEvent->setErrorMessage("Reading not implemented for: palette");
                break;
            case DataFileTypeEnum::RGBA:
                readDataFileEvent->setErrorMessage("Reading not implemented for: rgba");
                break;
            case DataFileTypeEnum::SCENE:
                readDataFileEvent->setErrorMessage("Reading not implemented for: scene");
                break;
            case DataFileTypeEnum::SPECIFICATION:
                readDataFileEvent->setErrorMessage("Reading not implemented for: specification");
                break;
            case DataFileTypeEnum::SURFACE_ANATOMICAL:
            case DataFileTypeEnum::SURFACE_INFLATED:
            case DataFileTypeEnum::SURFACE_VERY_INFLATED:
            case DataFileTypeEnum::SURFACE_FLAT:
                this->readSurfaceFile(filename);
                break;
            case DataFileTypeEnum::UNKNOWN:
                readDataFileEvent->setErrorMessage("Unable to read files of type");
                break;
            case DataFileTypeEnum::VOLUME_ANATOMY:
                readDataFileEvent->setErrorMessage("Reading not implemented for: volume anatomy");
                break;
            case DataFileTypeEnum::VOLUME_FUNCTIONAL:
                readDataFileEvent->setErrorMessage("Reading not implemented for: functional");
                break;
            case DataFileTypeEnum::VOLUME_LABEL:
                readDataFileEvent->setErrorMessage("Reading not implemented for: label");
                break;
        }
    }
    catch (DataFileException e) {
        readDataFileEvent->setErrorMessage(e.whatString());
    }    
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
            std::cout << "Received read data file event in " << __FILE__ << std::endl;            
            this->processReadDataFileEvent(readDataFileEvent);
        }
    }
}


