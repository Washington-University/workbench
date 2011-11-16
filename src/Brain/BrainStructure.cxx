/*LICENSE_START*/ 
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

#include <algorithm>

#include "CaretAssert.h"
#include "CaretLogger.h"

#define __BRAIN_STRUCTURE_DEFINE__
#include "BrainStructure.h"
#undef __BRAIN_STRUCTURE_DEFINE__
#include "BrainStructureNodeAttributes.h"
#include "EventBrainStructureGet.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventIdentificationSymbolRemoval.h"
#include "EventManager.h"
#include "EventNodeDataFilesGet.h"
#include "EventModelDisplayControllerAdd.h"
#include "EventModelDisplayControllerDelete.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelDisplayControllerSurface.h"
#include "RgbaFile.h"
#include "Surface.h"

#include <algorithm>

using namespace caret;

/**
 * Constructor.
 * 
 */
BrainStructure::BrainStructure(Brain* brain,
                               StructureEnum::Enum structure)
{
    this->brainStructureIdentifier = BrainStructure::brainStructureIdentifierCounter++;
    
    this->brain = brain;
    this->structure = structure;
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_BRAIN_STRUCTURE_GET);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_GET_NODE_DATA_FILES);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL);
}

/**
 * Destructor.
 */
BrainStructure::~BrainStructure()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    /*
     * Make a copy of all surface pointers since
     * deleting surfaces will alter the actual
     * vector that stores the surfaces.
     */
    std::vector<Surface*> allSurfaces(this->surfaces);
    
    for (uint64_t i = 0; i < allSurfaces.size(); i++) {
        this->deleteSurface(allSurfaces[i]);
    }
    this->surfaces.clear();
    
    for (uint64_t i = 0; i < labelFiles.size(); i++) {
        delete this->labelFiles[i];
        this->labelFiles[i] = NULL;
    }
    this->labelFiles.clear();
    
    for (uint64_t i = 0; i < metricFiles.size(); i++) {
        delete this->metricFiles[i];
        this->metricFiles[i] = NULL;
    }
    this->metricFiles.clear();
    
    for (uint64_t i = 0; i < rgbaFiles.size(); i++) {
        delete this->rgbaFiles[i];
        this->rgbaFiles[i] = NULL;
    }
    this->rgbaFiles.clear();

    for (uint64_t i = 0; i < this->nodeAttributes.size(); i++) {
        delete this->nodeAttributes[i];
        this->nodeAttributes[i] = NULL;
    }
    this->nodeAttributes.clear();
}

/**
 * Get the structure for this BrainStructure.
 *
 * @return The structure.
 */
StructureEnum::Enum 
BrainStructure::getStructure() const
{
    return this->structure;
}

/**
 * Add a label file.
 *
 * @param labelFile
 *    Label file that is added.
 * @throw DataFileException
 *    If the number of nodes in the label file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addLabelFile(LabelFile* labelFile) throw (DataFileException)
{
    CaretAssert(labelFile);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (labelFile->getNumberOfNodes() != numNodes) {
            AString message = " File contains "
            + AString::number(labelFile->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    this->labelFiles.push_back(labelFile);
}

/**
 * Add a metric file.
 *
 * @param metricFile
 *    Metric file that is added.
 * @throw DataFileException
 *    If the number of nodes in the metric file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addMetricFile(MetricFile* metricFile) throw (DataFileException)
{
    CaretAssert(metricFile);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (metricFile->getNumberOfNodes() != numNodes) {
            AString message = " File contains "
            + AString::number(metricFile->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    this->metricFiles.push_back(metricFile);
}

/**
 * Add an RGBA file.
 *
 * @param rgbaFile
 *    RGBA file that is added.
 * @throw DataFileException
 *    If the number of nodes in the RGBA file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addRgbaFile(RgbaFile* rgbaFile) throw (DataFileException)
{
    CaretAssert(rgbaFile);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (rgbaFile->getNumberOfNodes() != numNodes) {
            AString message = " File contains "
            + AString::number(rgbaFile->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    this->rgbaFiles.push_back(rgbaFile);
}

/**
 * Add a surface.
 *
 * @param surface
 *    Surface that is added.
 * @throw DataFileException
 *    If the number of nodes in the surface does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addSurface(Surface* surface) throw (DataFileException)
{
    CaretAssert(surface);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (surface->getNumberOfNodes() != numNodes) {
            AString message = " Surface contains "
            + AString::number(surface->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    if (numNodes == 0) {
        const int32_t numSurfaceNodes = surface->getNumberOfNodes();
        if (numSurfaceNodes > 0) {
            this->nodeAttributes.resize(numSurfaceNodes);
            for (int32_t i = 0; i < numSurfaceNodes; i++) {
                this->nodeAttributes[i] = new BrainStructureNodeAttributes();
            }
        }
    }
    
    surface->setBrainStructureIdentifier(this->brainStructureIdentifier);
    
    this->surfaces.push_back(surface);

    /*
     * Create a model controller for the surface.
     */
    ModelDisplayControllerSurface* mdcs = new ModelDisplayControllerSurface(surface);
    this->surfaceControllerMap.insert(std::make_pair(surface, mdcs));
    
    /*
     * Send the controller added event.
     */
    EventModelDisplayControllerAdd addEvent(mdcs);
    EventManager::get()->sendEvent(addEvent.getPointer());
}

void 
BrainStructure::deleteSurface(Surface* surface)
{
    CaretAssert(surface);
    
    std::vector<Surface*>::iterator iter =
    std::find(this->surfaces.begin(),
              this->surfaces.end(),
              surface);
    
    CaretAssertMessage((iter != this->surfaces.end()),
                       "Trying to delete surface not in brain structure.");
    
    std::map<Surface*, ModelDisplayControllerSurface*>::iterator controllerIter = 
        this->surfaceControllerMap.find(surface);

    CaretAssertMessage((controllerIter != this->surfaceControllerMap.end()),
                       "Surface does not map to a model controller");

    ModelDisplayControllerSurface* mdcs = controllerIter->second;
    
    /*
     * Remove from surface to controller map.
     */
    this->surfaceControllerMap.erase(controllerIter);
    
    /*
     * Remove the surface.
     */
    this->surfaces.erase(iter);
    
    /*
     * Send the controller deleted event.
     */
    EventModelDisplayControllerDelete deleteEvent(mdcs);
    EventManager::get()->sendEvent(deleteEvent.getPointer());
    
    /*
     * Delete the controller and the surface.
     */
    delete mdcs;
    delete surface;
}

/**
 * Get the number of surfaces.
 *
 * @return
 *    Number of surfaces.
 */
int 
BrainStructure::getNumberOfSurfaces() const
{
    return static_cast<int>(this->surfaces.size());
}

/**
 * Get a surface at the specified index.
 * 
 * @param indx
 *    Index of surface.
 * @return 
 *    Surface at the specified index.
 */
Surface* 
BrainStructure::getSurface(int indx)
{
    CaretAssertVectorIndex(this->surfaces, indx);
    
    return this->surfaces[indx];
}

/**
 * Is the surface in this brain structure?
 * @param surface
 *   Surface that is tested for being in this brain structure.
 * @return Returns true if surface in brain structure, else false.
 */
bool 
BrainStructure::containsSurface(const Surface* surface)
{
    CaretAssert(surface);
    if (std::find(this->surfaces.begin(),
                  this->surfaces.end(),
                  surface) != this->surfaces.end()) {
        return true;
    }
    return false;
}

/**
 * Get the brain that this brain structure is in.
 */
Brain* 
BrainStructure::getBrain()
{
    return this->brain;    
}

/**
 * Get the number of nodes used by this brain structure.
 *
 * @return Number of nodes.
 */
int32_t 
BrainStructure::getNumberOfNodes() const
{
    if (this->surfaces.empty() == false) {
        return surfaces[0]->getNumberOfNodes();
    }
    return 0;
}

/**
 * Get all of the label files.
 * @param labelFilesOut
 *    Will contain all label files after this method exits.
 */
void 
BrainStructure::getLabelFiles(std::vector<LabelFile*>& labelFilesOut) const
{
    labelFilesOut.clear();
    labelFilesOut.insert(labelFilesOut.end(),
                          this->labelFiles.begin(),
                          this->labelFiles.end());
}

/**
 * Get the number of label files.
 * @return Number of label files.
 */
int32_t 
BrainStructure::getNumberOfLabelFiles() const
{
    return this->labelFiles.size();
}

/**
 * Get a label file at the specified index.
 * @param fileIndex
 *    Index of the label file.
 * @return
 *    Metric file at the index.
 */
LabelFile* 
BrainStructure::getLabelFile(const int32_t fileIndex)
{
    CaretAssertVectorIndex(this->labelFiles, fileIndex);
    return this->labelFiles[fileIndex];
}

/**
 * Get a label file at the specified index.
 * @param fileIndex
 *    Index of the label file.
 * @return
 *    Metric file at the index.
 */
const LabelFile* 
BrainStructure::getLabelFile(const int32_t fileIndex) const
{
    CaretAssertVectorIndex(this->labelFiles, fileIndex);
    return this->labelFiles[fileIndex];
}

/**
 * Get all of the metric files.
 * @param metricFilesOut
 *    Will contain all metric files after this method exits.
 */
void 
BrainStructure::getMetricFiles(std::vector<MetricFile*>& metricFilesOut) const
{
    metricFilesOut.clear();
    metricFilesOut.insert(metricFilesOut.end(),
                          this->metricFiles.begin(),
                          this->metricFiles.end());
}

/**
 * Get the number of metric files.
 * @return Number of metric files.
 */
int32_t 
BrainStructure::getNumberOfMetricFiles() const
{
    return this->metricFiles.size();
}

/**
 * Get a metric file at the specified index.
 * @param fileIndex
 *    Index of the metric file.
 * @return
 *    Metric file at the index.
 */
MetricFile* 
BrainStructure::getMetricFile(const int32_t fileIndex)
{
    CaretAssertVectorIndex(this->metricFiles, fileIndex);
    return this->metricFiles[fileIndex];
}

/**
 * Get a metric file at the specified index.
 * @param fileIndex
 *    Index of the metric file.
 * @return
 *    Metric file at the index.
 */
const MetricFile* 
BrainStructure::getMetricFile(const int32_t fileIndex) const
{
    CaretAssertVectorIndex(this->metricFiles, fileIndex);
    return this->metricFiles[fileIndex];
}

/**
 * Get the number of rgba files.
 * @return Number of rgba files.
 */
int32_t 
BrainStructure::getNumberOfRgbaFiles() const
{
    return this->rgbaFiles.size();
}

/**
 * Get a rgba file at the specified index.
 * @param fileIndex
 *    Index of the rgba file.
 * @return
 *    Metric file at the index.
 */
RgbaFile* 
BrainStructure::getRgbaFile(const int32_t fileIndex)
{
    CaretAssertVectorIndex(this->rgbaFiles, fileIndex);
    return this->rgbaFiles[fileIndex];
}

/**
 * Get a rgba file at the specified index.
 * @param fileIndex
 *    Index of the rgba file.
 * @return
 *    Metric file at the index.
 */
const RgbaFile* 
BrainStructure::getRgbaFile(const int32_t fileIndex) const
{
    CaretAssertVectorIndex(this->rgbaFiles, fileIndex);
    return this->rgbaFiles[fileIndex];
}

/**
 * Get all of the rgba files.
 * @param rgbaFilesOut
 *    Will contain all rgba files after this method exits.
 */
void 
BrainStructure::getRgbaFiles(std::vector<RgbaFile*>& rgbaFilesOut) const
{
    rgbaFilesOut.clear();
    rgbaFilesOut.insert(rgbaFilesOut.end(),
                          this->rgbaFiles.begin(),
                          this->rgbaFiles.end());
}


/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
BrainStructure::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_GET_NODE_DATA_FILES) {
        EventNodeDataFilesGet* dataFilesEvent =
            dynamic_cast<EventNodeDataFilesGet*>(event);
        CaretAssert(dataFilesEvent);
        
        const Surface* associatedSurface = dataFilesEvent->getSurface();
        if (associatedSurface != NULL) {
            if (this->containsSurface(associatedSurface) == false) {
                return;
            }
        }
        
        for (std::vector<LabelFile*>::iterator labelIter = this->labelFiles.begin();
             labelIter != this->labelFiles.end();
             labelIter++) {
            dataFilesEvent->addFile(*labelIter);
        }
        
        for (std::vector<MetricFile*>::iterator metricIter = this->metricFiles.begin();
             metricIter != this->metricFiles.end();
             metricIter++) {
            dataFilesEvent->addFile(*metricIter);
        }
        
        for (std::vector<RgbaFile*>::iterator rgbaIter = this->rgbaFiles.begin();
             rgbaIter != this->rgbaFiles.end();
             rgbaIter++) {
            dataFilesEvent->addFile(*rgbaIter);
        }
        
        dataFilesEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET) {
        EventCaretMappableDataFilesGet* dataFilesEvent =
            dynamic_cast<EventCaretMappableDataFilesGet*>(event);
        CaretAssert(dataFilesEvent);
        
        const Surface* associatedSurface = dataFilesEvent->getSurface();
        if (associatedSurface != NULL) {
            if (this->containsSurface(associatedSurface) == false) {
                return;
            }
        }
        
        for (std::vector<LabelFile*>::iterator labelIter = this->labelFiles.begin();
             labelIter != this->labelFiles.end();
             labelIter++) {
            dataFilesEvent->addFile(*labelIter);
        }
        
        for (std::vector<MetricFile*>::iterator metricIter = this->metricFiles.begin();
             metricIter != this->metricFiles.end();
             metricIter++) {
            dataFilesEvent->addFile(*metricIter);
        }
        
        for (std::vector<RgbaFile*>::iterator rgbaIter = this->rgbaFiles.begin();
             rgbaIter != this->rgbaFiles.end();
             rgbaIter++) {
            dataFilesEvent->addFile(*rgbaIter);
        }
        
        dataFilesEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_STRUCTURE_GET) {
        EventBrainStructureGet* brainStructureEvent =
        dynamic_cast<EventBrainStructureGet*>(event);
        CaretAssert(brainStructureEvent);
        
        if (this->brainStructureIdentifier == brainStructureEvent->getbrainStructureIdentifier()) {
            brainStructureEvent->setBrainStructure(this);
            brainStructureEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL) {
        EventIdentificationSymbolRemoval* idRemovalEvent =
            dynamic_cast<EventIdentificationSymbolRemoval*>(event);
        CaretAssert(idRemovalEvent);
        
        for (uint32_t i = 0; i < this->nodeAttributes.size(); i++) {
            this->nodeAttributes[i]->setIdentified(false);
        }
        idRemovalEvent->setEventProcessed();
    }
}

/**
 * @return Return the unique identifier for this brain structure.
 */
int64_t 
BrainStructure::getBrainStructureIdentifier() const
{
    return this->brainStructureIdentifier;
}

/**
 * Get the attributes for a node.
 * @param nodeIndex
 *    Index of the node.
 * @return
 *    Attributes for the given node.
 */
BrainStructureNodeAttributes* 
BrainStructure::getNodeAttributes(const int32_t nodeIndex)
{
    CaretAssertVectorIndex(this->nodeAttributes, nodeIndex);
    return this->nodeAttributes[nodeIndex];
}

/**
 * Get the attributes for a node.
 * @param nodeIndex
 *    Index of the node.
 * @return
 *    Attributes for the given node.
 */
const BrainStructureNodeAttributes* 
BrainStructure::getNodeAttributes(const int32_t nodeIndex) const
{
    CaretAssertVectorIndex(this->nodeAttributes, nodeIndex);
    return this->nodeAttributes[nodeIndex];
}

/**
 * Get all loaded data files.
 * @param allDataFilesOut
 *    Data files are loaded into this parameter.
 */
void 
BrainStructure::getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut)
{
    allDataFilesOut.insert(allDataFilesOut.end(),
                           this->surfaces.begin(),
                           this->surfaces.end());
    allDataFilesOut.insert(allDataFilesOut.end(),
                           this->labelFiles.begin(),
                           this->labelFiles.end());
    allDataFilesOut.insert(allDataFilesOut.end(),
                           this->metricFiles.begin(),
                           this->metricFiles.end());
    allDataFilesOut.insert(allDataFilesOut.end(),
                           this->rgbaFiles.begin(),
                           this->rgbaFiles.end());
}

/**
 * Remove a data file from memory (does NOT delete file on disk.)
 * @param caretDataFile
 *    Data file to remove.
 * @return
 *    true if file was written, else false.
 */
bool 
BrainStructure::removeDataFile(CaretDataFile* caretDataFile)
{
    std::vector<Surface*>::iterator surfaceIterator = 
        std::find(this->surfaces.begin(),
                  this->surfaces.end(),
                  caretDataFile);
    if (surfaceIterator != this->surfaces.end()) {
        Surface* s = *surfaceIterator;
        this->deleteSurface(s);
        return true;
    }
    
    std::vector<LabelFile*>::iterator labelIterator = 
    std::find(this->labelFiles.begin(),
              this->labelFiles.end(),
              caretDataFile);
    if (labelIterator != this->labelFiles.end()) {
        delete caretDataFile;
        this->labelFiles.erase(labelIterator);
        return true;
    }
    
    std::vector<MetricFile*>::iterator metricIterator = 
    std::find(this->metricFiles.begin(),
              this->metricFiles.end(),
              caretDataFile);
    if (metricIterator != this->metricFiles.end()) {
        delete caretDataFile;
        this->metricFiles.erase(metricIterator);
        return true;
    }
    
    std::vector<RgbaFile*>::iterator rgbaIterator = 
    std::find(this->rgbaFiles.begin(),
              this->rgbaFiles.end(),
              caretDataFile);
    if (rgbaIterator != this->rgbaFiles.end()) {
        delete caretDataFile;
        this->rgbaFiles.erase(rgbaIterator);
        return true;
    }
    
    return false;
}

