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
#include <limits>

#include "CaretAssert.h"
#include "CaretLogger.h"

#define __BRAIN_STRUCTURE_DEFINE__
#include "BrainStructure.h"
#undef __BRAIN_STRUCTURE_DEFINE__
#include "Brain.h"
#include "BrainStructureNodeAttributes.h"
#include "CaretPointLocator.h"
#include "CaretPreferences.h"
#include "DisplayPropertiesInformation.h"
#include "ElapsedTimer.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventIdentificationSymbolRemoval.h"
#include "EventManager.h"
#include "EventNodeDataFilesGet.h"
#include "EventModelDisplayControllerAdd.h"
#include "EventModelDisplayControllerDelete.h"
#include "EventSurfacesGet.h"
#include "IdentificationManager.h"
#include "LabelFile.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "ModelDisplayControllerSurface.h"
#include "OverlaySet.h"
#include "RgbaFile.h"
#include "SessionManager.h"
#include "Surface.h"

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
    this->nodeAttributes = new BrainStructureNodeAttributes();
    this->volumeInteractionSurface = NULL;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i] = new OverlaySet(this);
    }
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_GET_NODE_DATA_FILES);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_SURFACES_GET);
}

/**
 * Destructor.
 */
BrainStructure::~BrainStructure()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete this->overlaySet[i];
    }
    
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

    delete this->nodeAttributes;
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
BrainStructure::addSurface(Surface* surface,
                           const bool initilizeOverlaysFlag) throw (DataFileException)
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
        this->nodeAttributes->update(numSurfaceNodes);
    }
    
    surface->setBrainStructure(this);
    
    this->surfaces.push_back(surface);

    /*
     * Create a model controller for the surface.
     */
    ModelDisplayControllerSurface* mdcs = new ModelDisplayControllerSurface(this->brain,
                                                                            surface);
    this->surfaceControllerMap.insert(std::make_pair(surface, mdcs));
    
    if (initilizeOverlaysFlag) {
        this->initializeOverlays();
    }
    
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
 * Get all surfaces of the given type in this brain structure.
 * @param surfaceType
 *   Type of surface
 * @param surfacesOut
 *   Output that will contain the surfaces.
 */
void 
BrainStructure::getSurfacesOfType(const SurfaceTypeEnum::Enum surfaceType,
                                  std::vector<Surface*>& surfacesOut) const
{
    surfacesOut.clear();
    
    const int32_t numSurfaces = this->getNumberOfSurfaces();
    for (int32_t i = 0; i < numSurfaces; i++) {
        if (this->surfaces[i]->getSurfaceType() == surfaceType) {
            surfacesOut.push_back(this->surfaces[i]);
        }
    }
}

/**
 * @return The surface used for volume interaction.
 * Returns NULL if no anatomical surfaces.
 */
const Surface* 
BrainStructure::getVolumeInteractionSurfacePrivate() const
{
    bool valid = false;
    if (this->volumeInteractionSurface != NULL) {
        const int32_t numSurfaces = this->getNumberOfSurfaces();
        for (int32_t i = 0; i < numSurfaces; i++) {
            if (this->surfaces[i] == this->volumeInteractionSurface) {
                valid = true;
                break;
            }
        }
    }
    if (valid) {
        return this->volumeInteractionSurface;
    }
    this->volumeInteractionSurface = NULL;
    
    /*
     * Give preference to anatomical surfaces but if there are none
     * (perhaps the surface types are missing), use all surfaces.
     */
    std::vector<Surface*> interactionSurfaces;
    this->getSurfacesOfType(SurfaceTypeEnum::ANATOMICAL, 
                            interactionSurfaces);
    if (interactionSurfaces.empty()) {
        interactionSurfaces = this->surfaces;
    }
    
    if (interactionSurfaces.empty() == false) {
        /*
         * Default to first surface
         */
        this->volumeInteractionSurface = interactionSurfaces[0];
        
        /*
         * Now look for a surface with certain strings in their name
         */
        Surface* midThicknessSurface = NULL;
        Surface* whiteMatterSurface  = NULL;
        Surface* pialSurface         = NULL;
        Surface* anatomicalSurface   = NULL;
        Surface* fiducialSurface     = NULL;
        const int32_t numSurfaces = static_cast<int32_t>(interactionSurfaces.size());
        for (int32_t i = 0; i < numSurfaces; i++) {
            const AString name = interactionSurfaces[i]->getFileNameNoPath().toLower();
            if (name.indexOf("midthick") >= 0) {
                midThicknessSurface = interactionSurfaces[i];
            }
            if (name.indexOf("white") >= 0) {
                whiteMatterSurface = interactionSurfaces[i];
            }
            if (name.indexOf("pial") >= 0) {
                pialSurface = interactionSurfaces[i];
            }
            if (name.indexOf("anatomical") >= 0) {
                anatomicalSurface = interactionSurfaces[i];
            }
            if (name.indexOf("fiducial") >= 0) {
                fiducialSurface = interactionSurfaces[i];
            }
        }
        
        if (midThicknessSurface != NULL) {
            this->volumeInteractionSurface = midThicknessSurface;
        }
        else if (whiteMatterSurface != NULL) {
            this->volumeInteractionSurface = whiteMatterSurface;
        }
        else if (pialSurface != NULL) {
            this->volumeInteractionSurface = pialSurface;
        }
        else if (anatomicalSurface != NULL) {
            this->volumeInteractionSurface = anatomicalSurface;
        }
        else if (fiducialSurface != NULL) {
            this->volumeInteractionSurface = fiducialSurface;
        }
    }
    
    if (this->volumeInteractionSurface != NULL) {
        CaretLogFiner("Volume Interaction Surface for "
                      + StructureEnum::toGuiName(this->structure)
                      + ": " 
                      + this->volumeInteractionSurface->getFileNameNoPath());
    }
    else {
        CaretLogFiner("Volume Interaction Surface for "
                      + StructureEnum::toGuiName(this->structure)
                      + " is invalid.");
    }
    
    return this->volumeInteractionSurface;
}

/**
 * @return The surface used for volume interaction.
 * Returns NULL if no anatomical surfaces.
 */
const Surface* 
BrainStructure::getVolumeInteractionSurface() const
{
    return this->getVolumeInteractionSurfacePrivate();
}

/**
 * @return The surface used for volume interaction.
 * Returns NULL if no anatomical surfaces.
 */
Surface* 
BrainStructure::getVolumeInteractionSurface()
{
    /*
     * Kludge to avoid duplicated code and ease maintenance
     */
    const Surface* constSurface = this->getVolumeInteractionSurfacePrivate();
    Surface* s = (Surface*)constSurface;
    return s;
}

/**
 * Set the volume interaction surface.
 * @param volumeInteractionSurface
 *    New volume interaction surface.
 */
void 
BrainStructure::setVolumeInteractionSurface(Surface* volumeInteractionSurface)
{
    this->volumeInteractionSurface = volumeInteractionSurface;
}

/**
 * Find and return the first surface encountered that contains the
 * given text in the name of the surface's filename.  Text is searched
 * in an case-insensitive mode.
 *
 * @param text
 *   Text that is to bound in the surface's filenname.
 * @return
 *   Surface that contains the given text in its filename or 
 *   NULL if no surface matches.
 */
Surface* 
BrainStructure::getSurfaceContainingTextInName(const AString& text)
{
    /*
     * Kludge to avoid duplicated code and ease maintenance
     */
    const Surface* constSurface = this->getSurfaceContainingTextInNamePrivate(text);
    Surface* s = (Surface*)constSurface;
    return s;
}


/**
 * Find and return the first surface encountered that contains the
 * given text in the name of the surface's filename.  Text is searched
 * in an case-insensitive mode.
 *
 * @param text
 *   Text that is to bound in the surface's filenname.
 * @return
 *   Surface that contains the given text in its filename or 
 *   NULL if no surface matches.
 */
const Surface* 
BrainStructure::getSurfaceContainingTextInName(const AString& text) const
{
    return this->getSurfaceContainingTextInNamePrivate(text);
}

/**
 * Find and return the first surface encountered that contains the
 * given text in the name of the surface's filename.  Text is searched
 * in an case-insensitive mode.
 *
 * @param text
 *   Text that is to bound in the surface's filenname.
 * @return
 *   Surface that contains the given text in its filename or 
 *   NULL if no surface matches.
 */
const Surface* 
BrainStructure::getSurfaceContainingTextInNamePrivate(const AString& text) const
{
    for (std::vector<Surface*>::const_iterator iter = this->surfaces.begin();
         iter != this->surfaces.end();
         iter++) {
        const Surface* surface = *iter;
        const AString name = surface->getFileNameNoPath();
        if (name.indexOf(text,
                         0,
                         Qt::CaseInsensitive) >= 0) {
            return surface;
        }
    }
    
    return NULL;
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
 * Get the brain that this brain structure is in.
 */
const Brain* 
BrainStructure::getBrain() const
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
    else if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idLocationEvent =
        dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idLocationEvent);

        //const bool contralateralIdEnabled = SessionManager::get()->getCaretPreferences()->isContralateralIdentificationEnabled();
        DisplayPropertiesInformation* dpi = this->brain->getDisplayPropertiesInformation();
        const bool contralateralIdEnabled = dpi->isContralateralIdentificationEnabled();
        
        NodeIdentificationTypeEnum::Enum identificationType = NodeIdentificationTypeEnum::NONE;
        int32_t highlighNodeIndex = -1;
        
        BrainStructure* contralateralBrainStructure = NULL;
        int32_t contralateralHighlightNodeIndex = -1;
        Surface* contralateralSurface = NULL;
        NodeIdentificationTypeEnum::Enum contralateralIdentificationType = NodeIdentificationTypeEnum::NONE;
        
        switch (idLocationEvent->getIdentificationType()) {
            case EventIdentificationHighlightLocation::IDENTIFICATION_SURFACE:
                if ((idLocationEvent->getSurfaceStructure() == this->getStructure()) 
                    && (idLocationEvent->getSurfaceNumberOfNodes() == this->getNumberOfNodes())) { 
                    highlighNodeIndex = idLocationEvent->getSurfaceNodeNumber();
                    identificationType = NodeIdentificationTypeEnum::NORMAL;
                }
                else if (contralateralIdEnabled) {
                    if (this->getNumberOfNodes() == idLocationEvent->getSurfaceNumberOfNodes()) {
                        if (StructureEnum::isCortexContralateral(this->getStructure(), 
                                                                 idLocationEvent->getSurfaceStructure())) {
                            highlighNodeIndex = idLocationEvent->getSurfaceNodeNumber();
                            identificationType = NodeIdentificationTypeEnum::CONTRALATERAL;
                        }
                    }
                }
                break;
            case EventIdentificationHighlightLocation::IDENTIFICATION_VOLUME:
            {
                const Surface* s = this->getVolumeInteractionSurface();
                if (s != NULL) {
                    const float* xyz = idLocationEvent->getXYZ();
                    const float toleranceDistance = 10.0;
                    int32_t nearestNodeIndex = s->closestNode(xyz, toleranceDistance);
                    if (nearestNodeIndex >= 0) {
                        highlighNodeIndex = nearestNodeIndex;
                        identificationType = NodeIdentificationTypeEnum::NORMAL;
                    }
                    if (contralateralIdEnabled 
                        && (highlighNodeIndex >= 0)) {
                        const StructureEnum::Enum contralateralStructure = StructureEnum::getContralateralStructure(this->getStructure());
                        if (contralateralStructure != StructureEnum::INVALID) {
                            contralateralBrainStructure = brain->getBrainStructure(contralateralStructure,
                                                                                        false);
                            if (contralateralBrainStructure != NULL) {
                                contralateralSurface = contralateralBrainStructure->getVolumeInteractionSurface();
                                if (contralateralSurface != NULL) {
                                    if (this->getNumberOfNodes() == contralateralSurface->getNumberOfNodes()) {
                                        contralateralHighlightNodeIndex = highlighNodeIndex;
                                        contralateralIdentificationType = NodeIdentificationTypeEnum::CONTRALATERAL;
                                    }
                                }
                            }
                        }
                    }
                }
            }
                break;
        }
        
        IdentificationManager* idManager = idLocationEvent->getIdentificationManager();
        if (highlighNodeIndex >= 0) {
            BrainStructureNodeAttributes* nodeAtts = this->getNodeAttributes();
            nodeAtts->setIdentificationType(highlighNodeIndex,
                                            identificationType);
            idManager->addAdditionalSurfaceNodeIdentification(this->getVolumeInteractionSurface(), 
                                                              highlighNodeIndex,
                                                              (identificationType == NodeIdentificationTypeEnum::CONTRALATERAL));
            idLocationEvent->setEventProcessed();
        }
        if (contralateralHighlightNodeIndex >= 0) {
            BrainStructureNodeAttributes* nodeAtts = contralateralBrainStructure->getNodeAttributes();
            nodeAtts->setIdentificationType(contralateralHighlightNodeIndex,
                                            contralateralIdentificationType);
            idManager->addAdditionalSurfaceNodeIdentification(contralateralSurface, 
                                                              contralateralHighlightNodeIndex,
                                                              true);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL) {
        EventIdentificationSymbolRemoval* idRemovalEvent =
            dynamic_cast<EventIdentificationSymbolRemoval*>(event);
        CaretAssert(idRemovalEvent);
        
        if (idRemovalEvent->isRemoveAllSurfaceSymbols()) {
            this->nodeAttributes->setAllIdentificationNone();
        }
        else if (idRemovalEvent->isRemoveSurfaceNodeSymbol()) {
            const StructureEnum::Enum idStructure = idRemovalEvent->getSurfaceStructure();
            const int32_t idNodeNumber = idRemovalEvent->getSurfaceNodeNumber();
            if (idStructure == this->structure) {
                if (this->getNumberOfNodes() >= idNodeNumber) {
                    this->nodeAttributes->setIdentificationType(idNodeNumber,
                                                                NodeIdentificationTypeEnum::NONE);
                }
            }
            else if (this->structure == StructureEnum::getContralateralStructure(idStructure)) {
                if (this->getNumberOfNodes() >= idNodeNumber) {
                    this->nodeAttributes->setIdentificationType(idNodeNumber,
                                                                NodeIdentificationTypeEnum::NONE);
                }
            }
        }
        idRemovalEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_SURFACES_GET) {
        EventSurfacesGet* getSurfacesEvent =
            dynamic_cast<EventSurfacesGet*>(event);
        CaretAssert(getSurfacesEvent);
        
        const int32_t numSurfaces = this->getNumberOfSurfaces();
        for (int32_t i = 0; i < numSurfaces; i++) {
            getSurfacesEvent->addSurface(this->getSurface(i));
        }
        getSurfacesEvent->setEventProcessed();
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
 * Get the attributes for this brain structure.
 * @return
 *    Attributes for this brain structure.
 */
BrainStructureNodeAttributes* 
BrainStructure::getNodeAttributes()
{
    return this->nodeAttributes;
}

/**
 * Get the attributes for this brain structure.
 * @return
 *    Attributes for this brain structure.
 */
const BrainStructureNodeAttributes* 
BrainStructure::getNodeAttributes() const
{
    return this->nodeAttributes;
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

/**
 * Find a map in a metric file that contains shape data.
 * It first looks for a shape NIFTI intent code.  If that
 * is not found it looks for curvature, shape, depth, etc.
 * @param metricFileOut
 *    Output metric file that contains the shape map.
 * @param shapeMapIndexOut
 *    Output containing index of shape map.
 * @return
 *    True if a shape map was found, else false.
 */
bool 
BrainStructure::getMetricShapeMap(MetricFile* &shapeMetricFileOut,
                                  int32_t& shapeMapIndexOut) const
{
    shapeMetricFileOut = NULL;
    shapeMapIndexOut   = -1;
    
    MetricFile* depthMetricFile = NULL;
    int32_t     depthMapIndex = -1;
    MetricFile* curvatureMetricFile = NULL;
    int32_t     curvatureMapIndex = -1;
    MetricFile* shapeNamedMetricFile = NULL;
    MetricFile* curvatureNamedMetricFile = NULL;
    MetricFile* depthNamedMetricFile = NULL;
    
    const int numFiles = this->metricFiles.size();
    for (int32_t i = 0; i < numFiles; i++) {
        MetricFile* mf = this->metricFiles[i];
        const AString filename = mf->getFileNameNoPath().toLower();
        const int32_t numMaps = mf->getNumberOfMaps();
        for (int32_t j = 0; j < numMaps; j++) {
            const AString mapName = mf->getMapName(j).toLower();
            if (mapName.contains("depth")) {
                if (depthMetricFile == NULL) {
                    depthMetricFile = mf;
                    depthMapIndex   = j;
                }
            }
            else if (mapName.contains("curv")) {
                if (curvatureMetricFile == NULL) {
                    curvatureMetricFile = mf;
                    curvatureMapIndex   = j;
                }
            }
        }
        
        if (filename.contains("shape")) {
            if (numMaps > 0) {
                shapeNamedMetricFile = mf;
            }
        }
        if (filename.contains("curv")) {
            if (numMaps > 0) {
                curvatureNamedMetricFile = mf;
            }
        }
        if (filename.contains("depth")) {
            if (numMaps > 0) {
                depthNamedMetricFile = mf;
            }
        }
    }
    
    if (depthMetricFile != NULL) {
        shapeMetricFileOut = depthMetricFile;
        shapeMapIndexOut   = depthMapIndex;
    }
    else if (curvatureMetricFile != NULL) {
        shapeMetricFileOut = curvatureMetricFile;
        shapeMapIndexOut   = curvatureMapIndex;
    }
    else if (depthNamedMetricFile != NULL) {
        shapeMetricFileOut = depthNamedMetricFile;
        shapeMapIndexOut   = 0;
    }
    else if (curvatureNamedMetricFile != NULL) {
        shapeMetricFileOut = curvatureNamedMetricFile;
        shapeMapIndexOut   = 0;
    }
    else if (shapeNamedMetricFile != NULL) {
        shapeMetricFileOut = shapeNamedMetricFile;
        shapeMapIndexOut   = 0;
    }
    
    if ((shapeMetricFileOut != NULL) 
        && (shapeMapIndexOut >= 0)) {
        return true;
    }
    
    return false;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
BrainStructure::getOverlaySet(const int tabIndex)
{
    CaretAssertArrayIndex(this->overlaySet, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return this->overlaySet[tabIndex];
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
BrainStructure::getOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(this->overlaySet, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return this->overlaySet[tabIndex];
}

/**
 * Initilize the overlays for this controller.
 */
void 
BrainStructure::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i]->initializeOverlays();
    }
}

