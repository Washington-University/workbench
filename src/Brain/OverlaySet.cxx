
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
#include <deque>

#define __OVERLAY_SET_DECLARE__
#include "OverlaySet.h"
#undef __OVERLAY_SET_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "Overlay.h"
#include "OverlaySetInitializer.h"
#include "PlainTextStringBuilder.h"
#include "Scene.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class OverlaySet
 * \brief Contains a set of overlay assignments
 *
 * The maximum number of overlays is fixed.  The number
 * of overlays presented to the user varies and is
 * controlled using the ToolBox in a Browser Window.
 * 
 * The primary overlay is always the overlay at index zero.
 * The underlay is the overlay at (numberOfDisplayedOverlays - 1).
 * When models are colored, the overlays are assigned 
 * starting with the underlay and concluding with the primary
 * overlay.
 */

/**
 * \class OverlaySet
 * \brief Contains a set of overlay assignments
 *
 * The maximum number of overlays is fixed.  The number
 * of overlays presented to the user varies and is
 * controlled using the ToolBox in a Browser Window.
 * 
 * The primary overlay is always the overlay at index zero.
 * The underlay is the overlay at (numberOfDisplayedOverlays - 1).
 * When models are colored, the overlays are assigned 
 * starting with the underlay and concluding with the primary
 * overlay.
 */

/**
 * Constructor for the given surface structures, surface types, and volumes.
 *
 * @param name
 *     Name for this overlay set
 * @param tabIndex
 *     Index of tab for this overlay set.
 * @param includeSurfaceStructures
 *     Surface structures for data files displayed in this overlay set.
 * @param includeVolumeFiles
 *     Surface structures for data files displayed in this overlay set.
 */
OverlaySet::OverlaySet(const AString& name,
                       const int32_t tabIndex,
                       const std::vector<StructureEnum::Enum>& includeSurfaceStructures,
                       const Overlay::IncludeVolumeFiles includeVolumeFiles)
: CaretObject(),
m_name(name),
m_tabIndex(tabIndex),
m_includeSurfaceStructures(includeSurfaceStructures),
m_includeVolumeFiles(includeVolumeFiles)
{
    m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_numberOfDisplayedOverlays",
                          &m_numberOfDisplayedOverlays);
    
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i] = new Overlay(includeSurfaceStructures,
                                    includeVolumeFiles);
    }
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
}

/**
 * Destructor.
 */
OverlaySet::~OverlaySet()
{
    EventManager::get()->removeAllEventsFromListener(this);
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        delete m_overlays[i];
    }
    delete m_sceneAssistant;
}

/**
 * Copy the given overlay set to this overlay set.
 * @param overlaySet
 *    Overlay set that is copied.
 */
void 
OverlaySet::copyOverlaySet(const OverlaySet* overlaySet)
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i]->copyData(overlaySet->getOverlay(i));
    }
    m_numberOfDisplayedOverlays = overlaySet->m_numberOfDisplayedOverlays;
}

/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
Overlay* 
OverlaySet::getPrimaryOverlay()
{
    return m_overlays[0];
}

/**
 * @return Returns the underlay which is the lowest
 * displayed overlay.
 */
Overlay* 
OverlaySet::getUnderlay()
{
    return m_overlays[getNumberOfDisplayedOverlays() - 1];
}

/*
 * Get the bottom-most overlay that is a volume file for the given
 * browser tab.
 * @param browserTabContent
 *    Content of browser tab.
 * @return Returns the bottom-most overlay that is set a a volume file.
 * Will return NULL if no, enabled overlays are set to a volume file.
 */
Overlay*
OverlaySet::getUnderlayContainingVolume()
{
    Overlay* underlayOut(NULL);
    
    const int32_t numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = (numOverlays - 1); i >= 0; i--) {
        if (m_overlays[i]->isEnabled()) {
            CaretMappableDataFile* mapFile = NULL;
            int32_t mapIndex;
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
            m_overlays[i]->getSelectionData(mapFile,
                                            mapIndex);
            
            if (mapFile != NULL) {
                if (mapFile->isVolumeMappable()) {
                    const VolumeMappableInterface* vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
                    if (vf != NULL) {
                        underlayOut = m_overlays[i];
                        break;
                    }
                }
            }
        }
    }
    
    if (underlayOut == NULL) {
        /*
         * If we are here, either there are no volume files or
         * no overlays are enabled containing a volume file.
         * So, find the lowest layer than contains a volume file,
         * even if the layer is disabled.
         */
        for (int32_t i = 0; i < numOverlays; i++) {
            CaretMappableDataFile* mapFile = NULL;
            int32_t mapIndex;
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
            m_overlays[i]->getSelectionData(mapFile,
                                            mapIndex);
            
            if (mapFile != NULL) {
                if (mapFile->isVolumeMappable()) {
                    const VolumeMappableInterface* vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
                    if (vf != NULL) {
                        underlayOut = m_overlays[i];
                        break;
                    }
                }
            }
        }
    }
    
    return underlayOut;
}

/*
 * Get the bottom-most overlay that is a volume file for the given
 * browser tab and return its volume file.
 * @param browserTabContent
 *    Content of browser tab.
 * @return Returns the bottom-most overlay that is set a a volume file.
 * Will return NULL if no, enabled overlays are set to a volume file.
 */
VolumeMappableInterface*
OverlaySet::getUnderlayVolume()
{
    VolumeMappableInterface* vf = NULL;
 
    Overlay* underlay = getUnderlayContainingVolume();
    if (underlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex;
        underlay->getSelectionData(mapFile, mapIndex);
        if (mapFile != NULL) {
            vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
        }
    }
    
    return vf;
}

/*
 * Get the bottom-most overlay that is a volume file for the given
 * browser tab and return its volume file.
 * @param browserTabContent
 *    Content of browser tab.
 * @return Returns the bottom-most overlay that is set a a volume file.
 * Will return NULL if no, enabled overlays are set to a volume file.
 */
const VolumeMappableInterface*
OverlaySet::getUnderlayVolume() const
{
    OverlaySet* nonConstOverlaySet(const_cast<OverlaySet*>(this));
    return nonConstOverlaySet->getUnderlayVolume();
}

/**
 * If NO overlay (any overlay) is set to a volume, set the underlay to the first
 * volume that it finds.
 * @return Returns the volume file that was selected or NULL if no
 *    volume file was found.
 */
VolumeMappableInterface* 
OverlaySet::setUnderlayToVolume()
{
    VolumeMappableInterface * vf = getUnderlayVolume();
    
    if (vf == NULL) {
        const int32_t overlayIndex = getNumberOfDisplayedOverlays() - 1;
        if (overlayIndex >= 0) {
            std::vector<CaretMappableDataFile*> mapFiles;
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            m_overlays[overlayIndex]->getSelectionData(mapFiles, 
                                                          mapFile, 
                                                          mapIndex);
            
            const int32_t numMapFiles = static_cast<int32_t>(mapFiles.size());
            for (int32_t i = 0; i < numMapFiles; i++) {
                if (mapFiles[i]->isVolumeMappable()) {
                    vf = dynamic_cast<VolumeMappableInterface*>(mapFiles[i]);
                    if (vf != NULL) {
                        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(vf);
                        CaretAssert(cmdf);
                        m_overlays[overlayIndex]->setSelectionData(cmdf, 0);
                        break;
                    }
                }
            }
        }
    }
    
    return vf;
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
const Overlay* 
OverlaySet::getOverlay(const int32_t overlayNumber) const
{
    CaretAssertArrayIndex(m_overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, 
                          overlayNumber);
    return m_overlays[overlayNumber];    
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
Overlay* 
OverlaySet::getOverlay(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(m_overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, 
                          overlayNumber);
    return m_overlays[overlayNumber];    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OverlaySet::toString() const
{
    PlainTextStringBuilder tb;
    getDescriptionOfContent(tb);
    return tb.getText();
}

/**
 * Get a text description of the window's content.
 *
 * @param descriptionOut
 *    Description of the window's content.
 */
void
OverlaySet::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Overlay Set");
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        if (getOverlay(i)->isEnabled()) {
            descriptionOut.pushIndentation();
            
            descriptionOut.addLine("Overlay "
                    + AString::number(i + 1)
                    + ": ");
            
            descriptionOut.pushIndentation();
            getOverlay(i)->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
            
            descriptionOut.popIndentation();
        }
    }
}


/**
 * Add a displayed overlay.  If the maximum
 * number of surface overlays is reached,
 * this method has no effect.
 */
void 
OverlaySet::addDisplayedOverlay()
{
    m_numberOfDisplayedOverlays++;
    if (m_numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
}

/**
 * @return Returns the number of displayed overlays. 
 */
int32_t 
OverlaySet::getNumberOfDisplayedOverlays() const
{
    return m_numberOfDisplayedOverlays;
}

/**
 * Sets the number of displayed overlays.
 * @param numberOfDisplayedOverlays
 *   Number of overlays for display.
 */
void 
OverlaySet::setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays)
{
    const int32_t oldNumberOfDisplayedOverlays = m_numberOfDisplayedOverlays;
    m_numberOfDisplayedOverlays = numberOfDisplayedOverlays;
    if (m_numberOfDisplayedOverlays < BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    }
    if (m_numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
    
    /*
     * If one overlay added (probably through GUI),
     * shift all overlays down one position so that 
     * new overlay appears at the top
     */ 
    const int32_t numberOfOverlaysAdded = m_numberOfDisplayedOverlays - oldNumberOfDisplayedOverlays;
    if (numberOfOverlaysAdded == 1) {
        for (int32_t i = (m_numberOfDisplayedOverlays - 1); i >= 0; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}

/**
 * Insert an overlay below this overlay
 * @param overlayIndex
 *     Index of overlay for which an overlay is added below
 */
void 
OverlaySet::insertOverlayAbove(const int32_t overlayIndex)
{
    if (m_numberOfDisplayedOverlays < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays++;
        
        for (int32_t i = (m_numberOfDisplayedOverlays - 2); i >= overlayIndex; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}

/**
 * Insert an overlay above this overlay
 * @param overlayIndex
 *     Index of overlay for which an overlay is added above
 */
void 
OverlaySet::insertOverlayBelow(const int32_t overlayIndex)
{
    if (m_numberOfDisplayedOverlays < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays++;
        
        for (int32_t i = (m_numberOfDisplayedOverlays - 2); i > overlayIndex; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}


/**
 * Remove a displayed overlay.  This method will have
 * no effect if the minimum number of overlays are
 * displayed
 *
 * @param overlayIndex 
 *    Index of overlay for removal from display.
 */
void 
OverlaySet::removeDisplayedOverlay(const int32_t overlayIndex)
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayIndex);
    m_overlays[overlayIndex]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    
    if (m_numberOfDisplayedOverlays > BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays--;
        for (int32_t i = overlayIndex; i < m_numberOfDisplayedOverlays; i++) {
            m_overlays[i]->copyData(m_overlays[i+1]);
        }
    }
}

/**
 * Move the overlay at the given index up one level
 * (swap it with overlayIndex - 1).  This method will
 * have no effect if the overlay is the top-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved up.
 */
void 
OverlaySet::moveDisplayedOverlayUp(const int32_t overlayIndex)
{
    if (overlayIndex > 0) {
        m_overlays[overlayIndex]->swapData(m_overlays[overlayIndex - 1]);
    }
}

/**
 * Move the overlay at the given index down one level
 * (swap it with overlayIndex + 1).  This method will
 * have no effect if the overlay is the bottom-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved down.
 */
void 
OverlaySet::moveDisplayedOverlayDown(const int32_t overlayIndex)
{
    const int32_t nextOverlayIndex = overlayIndex + 1;
    if (nextOverlayIndex < m_numberOfDisplayedOverlays) {
        m_overlays[overlayIndex]->swapData(m_overlays[nextOverlayIndex]);
    }
}

/**
 * Initialize the overlays.
 */
void
OverlaySet::initializeOverlays()
{
    bool isMatchToVolumeUnderlay = false;
    bool isVolumesForHistology   = false;
    bool atLeastOneOverlayEnabledFlag(false);
    
    switch (m_includeVolumeFiles) {
        case Overlay::INCLUDE_VOLUME_FILES_NO:
            break;
        case Overlay::INCLUDE_VOLUME_FILES_YES:
            isMatchToVolumeUnderlay = true;
            break;
        case Overlay::INCLUDE_VOLUME_FILES_FOR_HISTOLOGY_MODEL:
            isVolumesForHistology = true;
            break;
    }
    
    {
        const bool logFlag(m_tabIndex == 0);
        OverlaySetInitializer initializer;
        std::vector<OverlaySetInitializer::FileAndMapIndex> layers(initializer.initializeOverlaySet(m_includeSurfaceStructures,
                                                                                                    isMatchToVolumeUnderlay,
                                                                                                    logFlag));
        if (layers.size() > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
            layers.resize(BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS);
        }
        
        /*
         * Note: There are minimum and maximum number of overlays
         * that are applied when setNumberOfDisplayedOverlays() is
         * called.  Thus the displayed number of overlays may
         * be different than the number of layers to load.
         *
         * Layers from overlay initializer are ordered underlay to
         * overlay.  Put underlay at bottom of these overlays.
         */
        int32_t layerIndex(0);
        const int32_t numLayers(layers.size());
        setNumberOfDisplayedOverlays(numLayers);
        const int32_t numDisplayedLayers(getNumberOfDisplayedOverlays());
        for (int32_t i = (numDisplayedLayers - 1); i >= 0; i--) {
            Overlay* overlay(getOverlay(i));
            CaretAssert(overlay);
            if (layerIndex < numLayers) {
                CaretAssertVectorIndex(layers, layerIndex);
                overlay->setSelectionData(layers[layerIndex].m_file,
                                          layers[layerIndex].m_mapIndex);
                overlay->setEnabled(true);
                atLeastOneOverlayEnabledFlag = true;
                ++layerIndex;
            }
            else {
                overlay->setEnabled(false);
            }
            overlay->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
        }
    }
    
    /*
     * If no overlays were enabled, enable the underlay
     * if it contains a file.  This problem was observed
     * if one loaded an RGB volume (no overlay was enabled).
     */
    if ( ! atLeastOneOverlayEnabledFlag) {
        const int32_t numberOfDisplayedOverlays(getNumberOfDisplayedOverlays());
        if (numberOfDisplayedOverlays > 0) {
            const int32_t overlayIndex(numberOfDisplayedOverlays - 1);
            Overlay* overlay(getOverlay(overlayIndex));
            CaretMappableDataFile* mapFile(NULL);
            int32_t mapIndex(0);
            overlay->getSelectionData(mapFile, mapIndex);
            if ((mapFile != NULL)
                && (mapIndex >= 0)) {
                overlay->setEnabled(true);
            }
        }
    }
    
    if (isVolumesForHistology) {
        for (Overlay* overlay : m_overlays) {
            CaretAssert(overlay);
            overlay->setEnabled(false);
        }
    }
}


/**
 * Get any label files that are selected and applicable for the given surface.
 * @param surface
 *    Surface for which label files are desired.
 * @param labelFilesOut
 *    Label files that are applicable to the given surface.
 * @param labelMapIndicesOut
 *    Selected map indices in the output label files.
 */
void 
OverlaySet::getLabelFilesForSurface(const Surface* surface,
                                    std::vector<LabelFile*>& labelFilesOut,
                                    std::vector<int32_t>& labelMapIndicesOut)
{
    CaretAssert(surface);
    
    labelFilesOut.clear();
    labelMapIndicesOut.clear();
    
    const int32_t numberOfOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        Overlay* overlay = getOverlay(i);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            overlay->getSelectionData(mapFile, mapIndex);
            if (mapFile != NULL) {
                if (mapFile->getDataFileType() == DataFileTypeEnum::LABEL) {
                    if (mapFile->getStructure() == surface->getStructure()) {
                        LabelFile* labelFile = dynamic_cast<LabelFile*>(mapFile);
                        labelFilesOut.push_back(labelFile);
                        labelMapIndicesOut.push_back(mapIndex);
                    }
                }
            }
        }
    }
}

/**
 * For the given caret mappable data file, find overlays in which the
 * file is selected and return the indices of the selected maps.
 *
 * @param caretMappableDataFile
 *    The caret mappable data file.
 * @param isLimitToEnabledOverlays
 *    If true, only include map indices for overlay that are enabled.  
 *    Otherwise, include map indices for all overlays.
 * @param selectedMapIndicesOut
 *    Output containing map indices for the given caret mappable data files
 *    that are selected as overlays in this overlay set.
 */
void
OverlaySet::getSelectedMapIndicesForFile(const CaretMappableDataFile* caretMappableDataFile,
                                         const bool isLimitToEnabledOverlays,
                                         std::vector<int32_t>& selectedMapIndicesOut) const
{
    selectedMapIndicesOut.clear();
    
    /*
     * Put indices in a set to avoid duplicates and keep them sorted.
     */
    std::set<int32_t> mapIndicesSet;
    
    const int32_t numberOfOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        Overlay* overlay = const_cast<Overlay*>(getOverlay(i));
        bool checkIt = true;
        if (isLimitToEnabledOverlays) {
            if ( ! overlay->isEnabled()) {
                checkIt = false;
            }
        }
        
        if (checkIt) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            overlay->getSelectionData(mapFile, mapIndex);
            if (mapFile == caretMappableDataFile) {
                mapIndicesSet.insert(mapIndex);
            }
        }
    }
    
    selectedMapIndicesOut.insert(selectedMapIndicesOut.end(),
                                 mapIndicesSet.begin(),
                                 mapIndicesSet.end());
}

/**
 * Reset the yoking status of all overlays to off.
 */
void
OverlaySet::resetOverlayYokingToOff()
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    }
}


/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
OverlaySet::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "OverlaySet",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    const int32_t numOverlaysToSave = getNumberOfDisplayedOverlays();
    
    std::vector<SceneClass*> overlayClassVector;
    for (int i = 0; i < numOverlaysToSave; i++) {
        overlayClassVector.push_back(m_overlays[i]->saveToScene(sceneAttributes, "m_overlays"));
    }
    
    SceneClassArray* overlayClassArray = new SceneClassArray("m_overlays",
                                                             overlayClassVector);
    sceneClass->addChild(overlayClassArray);
    
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
OverlaySet::restoreFromScene(const SceneAttributes* sceneAttributes,
                             const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    const SceneClassArray* overlayClassArray = sceneClass->getClassArray("m_overlays");
    if (overlayClassArray != NULL) {
        const int32_t numOverlays = std::min(overlayClassArray->getNumberOfArrayElements(),
                                             (int32_t)BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS);
        for (int32_t i = 0; i < numOverlays; i++) {
            m_overlays[i]->restoreFromScene(sceneAttributes, 
                                            overlayClassArray->getClassAtIndex(i));
        }
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
OverlaySet::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
        /*
         * The events intended for overlays are received here so that
         * only DISPLAYED overlays are updated.
         */
        EventMapYokingValidation* mapYokeEvent = dynamic_cast<EventMapYokingValidation*>(event);
        CaretAssert(mapYokeEvent);

        const MapYokingGroupEnum::Enum requestedYokingGroup = mapYokeEvent->getMapYokingGroup();
        if (requestedYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                Overlay* overlay = getOverlay(j);
                
                CaretMappableDataFile* mapFile = NULL;
                int32_t mapIndex = -1;
                overlay->getSelectionData(mapFile,
                                          mapIndex);
                if (mapFile != NULL) {
                    mapYokeEvent->addMapYokedFile(mapFile, overlay->getMapYokingGroup(), m_tabIndex);
                }
            }
        }
        
        mapYokeEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP) {
        /*
         * The events intended for overlays are received here so that
         * only DISPLAYED overlays are updated.
         */
        EventMapYokingSelectMap* selectMapEvent = dynamic_cast<EventMapYokingSelectMap*>(event);
        CaretAssert(selectMapEvent);
        const MapYokingGroupEnum::Enum eventYokingGroup = selectMapEvent->getMapYokingGroup();
        if (eventYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            const int32_t yokingGroupMapIndex = MapYokingGroupEnum::getSelectedMapIndex(eventYokingGroup);
            const bool yokingGroupSelectedStatus = MapYokingGroupEnum::isEnabled(eventYokingGroup);
            const CaretMappableDataFile* eventMapFile = selectMapEvent->getCaretMappableDataFile();
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                Overlay* overlay = getOverlay(j);
                
                if (overlay->getMapYokingGroup() == selectMapEvent->getMapYokingGroup()) {
                    CaretMappableDataFile* mapFile = NULL;
                    int32_t mapIndex = -1;
                    overlay->getSelectionData(mapFile,
                                              mapIndex);
                    
                    if (mapFile != NULL) {
                        if (yokingGroupMapIndex < mapFile->getNumberOfMaps()) {
                            overlay->setSelectionData(mapFile,
                                                      yokingGroupMapIndex);
                        }
                        
                        if (mapFile == eventMapFile) {
                            /* only alter status if event was sent by mappable file */
                            if (selectMapEvent->getCaretMappableDataFile() != NULL) {
                                overlay->setEnabled(yokingGroupSelectedStatus);
                            }
                        }
                    }
                }
            }
            
            selectMapEvent->setEventProcessed();
        }
    }
}

/**
 * @return True if an enabled overlay contains a volume file that is an oblique only volume
 */
bool
OverlaySet::hasObliqueOnlyVolumeSelected() const
{
    const int32_t numberOfOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        Overlay* overlay = const_cast<Overlay*>(getOverlay(i));
        if (overlay->isEnabled()) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            overlay->getSelectionData(mapFile, mapIndex);
            if (mapFile != NULL) {
                if (mapFile->getDataFileType() == DataFileTypeEnum::VOLUME) {
                    const VolumeFile* vf = dynamic_cast<const VolumeFile*>(mapFile);
                    CaretAssert(vf);
                    if ( ! vf->isPlumb()) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}



