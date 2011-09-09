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
#include "EventGetModelDisplayControllers.h"
#include "EventLoadSurfaceFile.h"
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
    
    EventManager::get()->addEventListener(this, Event::EVENT_LOAD_SURFACE_FILE);
    EventManager::get()->addEventListener(this, Event::EVENT_GET_MODEL_DISPLAY_CONTROLLERS);
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
 * Is the model controller valid?
 *
 * @param modelController
 *    Model controller tested for validity.
 *
 * @return
 *    true if controller is valid, else false.
 */
bool 
Brain::isModelControllerValid(const ModelDisplayController* modelController)
{
    std::vector<ModelDisplayController*> controllers;
    this->getAllModelControllers(controllers);

    if (std::find(controllers.begin(), 
                  controllers.end(), 
                  modelController) 
        != controllers.end()) {
        return true;
    }
    return false;
}

/**
 * Get all of the model controllers.
 *
 * @return
 *    Vector containing all model controllers.
 */
void
Brain::getAllModelControllers(std::vector<ModelDisplayController*>& controllers)
{
    controllers.clear();
    
    int numBrainStructures = this->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numBrainStructures; i++) {
        BrainStructure* bs = this->getBrainStructure(i);
        int numSurfaces = bs->getNumberOfSurfaces();
        for (int32_t j = 0; j < numSurfaces; j++) {
            Surface* s = bs->getSurface(i);
            controllers.push_back(s->getModelController());
        }
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
    if (event->getEventType() == Event::EVENT_LOAD_SURFACE_FILE) {
        EventLoadSurfaceFile* loadSurfaceFileEvent =
             dynamic_cast<EventLoadSurfaceFile*>(event);
        CaretAssert(loadSurfaceFileEvent);
        
        std::cout << "Received load surface event in " << __func__ << std::endl;
        
        try {
            this->readSurfaceFile(loadSurfaceFileEvent->getSurfaceFileName());
        }
        catch (DataFileException e) {
            loadSurfaceFileEvent->setErrorMessage(e.whatString());
        }
    }
    else if (event->getEventType() == Event::EVENT_GET_MODEL_DISPLAY_CONTROLLERS) {
        EventGetModelDisplayControllers* getModelsEvent =
            dynamic_cast<EventGetModelDisplayControllers*>(event);
        CaretAssert(getModelsEvent);
        
        std::cout << "Received get models event in " << __func__ << std::endl;
        
        std::vector<ModelDisplayController*> controllers;
        this->getAllModelControllers(controllers);
        getModelsEvent->addModelDisplayControllers(controllers);
    }
}


