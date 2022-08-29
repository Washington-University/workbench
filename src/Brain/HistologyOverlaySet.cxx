
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

#define __HISTOLOGY_OVERLAY_SET_DECLARE__
#include "HistologyOverlaySet.h"
#undef __HISTOLOGY_OVERLAY_SET_DECLARE__

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ImageFile.h"
#include "HistologySlicesFile.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "HistologyOverlay.h"
#include "ModelMedia.h"
#include "PlainTextStringBuilder.h"
#include "Scene.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class HistologyOverlaySet
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
 */
HistologyOverlaySet::HistologyOverlaySet(const AString& name,
                                 const int32_t tabIndex)
: CaretObject(),
m_name(name),
m_tabIndex(tabIndex)
{
    m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_numberOfDisplayedOverlays",
                          &m_numberOfDisplayedOverlays);
    
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i] = new HistologyOverlay(m_tabIndex,
                                         i);
    }
    
    initializeOverlays();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
}

/**
 * Destructor.
 */
HistologyOverlaySet::~HistologyOverlaySet()
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
HistologyOverlaySet::copyHistologyOverlaySet(const HistologyOverlaySet* overlaySet)
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i]->copyData(overlaySet->getOverlay(i));
    }
    m_numberOfDisplayedOverlays = overlaySet->m_numberOfDisplayedOverlays;
}

/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
HistologyOverlay*
HistologyOverlaySet::getPrimaryOverlay()
{
    return m_overlays[0];
}

/**
 * @return Returns the underlay which is the lowest
 * displayed overlay.
 */
HistologyOverlay*
HistologyOverlaySet::getUnderlay()
{
    return m_overlays[getNumberOfDisplayedOverlays() - 1];
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
const HistologyOverlay*
HistologyOverlaySet::getOverlay(const int32_t overlayNumber) const
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
HistologyOverlay*
HistologyOverlaySet::getOverlay(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(m_overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, 
                          overlayNumber);
    return m_overlays[overlayNumber];    
}

/**
 * @return The bottom-most overlay that is enabled
 */
HistologyOverlay*
HistologyOverlaySet::getBottomMostEnabledOverlay()
{
    HistologyOverlay* overlay(NULL);
    
    const int32_t numOverlays(getNumberOfDisplayedOverlays());
    for (int32_t i = 0; i < numOverlays; i++) {
        HistologyOverlay* ov(getOverlay(i));
        CaretAssert(ov);
        if (ov->isEnabled()) {
            overlay = ov;
        }
    }
    return overlay;
}

/**
 * @return Media file in bottom most enabled overlay or NULL if none selected.
 */
HistologySlicesFile*
HistologyOverlaySet::getBottomMostHistologySlicesFile()
{
    HistologySlicesFile* histologySlicesFile(NULL);
    HistologyOverlay* underlay = getBottomMostEnabledOverlay();
    if (underlay != NULL) {
        HistologyOverlay::SelectionData selectionData(underlay->getSelectionData());
        histologySlicesFile = selectionData.m_selectedFile;
    }
    return histologySlicesFile;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologyOverlaySet::toString() const
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
HistologyOverlaySet::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Media Overlay Set");
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        if (getOverlay(i)->isEnabled()) {
            descriptionOut.pushIndentation();
            
            descriptionOut.addLine("Media Overlay "
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
 * @return All displayed media files
 */
std::vector<HistologySlicesFile*>
HistologyOverlaySet::getDisplayedHistologySlicesFiles() const
{
    std::vector<HistologySlicesFile*> histologySlicesFilesOut;

    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        if (getOverlay(i)->isEnabled()) {
            HistologyOverlay::SelectionData selectionData(getOverlay(i)->getSelectionData());
            HistologySlicesFile* mf(selectionData.m_selectedFile);
            if (mf != NULL) {
                histologySlicesFilesOut.push_back(mf);
            }
        }
    }
    
    return histologySlicesFilesOut;
}

/**
 * Get displayed media files and indices of overlays containing the media files
 * @param histologySlicesFileOut
 *    Output containing displayed media files
 * @param overlayIndicesOut
 *    Output containing overlay indices of displayed media files
 */
void
HistologyOverlaySet::getDisplayedHistologySlicesFileAndOverlayIndices(std::vector<HistologySlicesFile*>& histologySlicesFilesOut,
                                                        std::vector<int32_t>& overlayIndicesOut) const
{
    histologySlicesFilesOut.clear();
    overlayIndicesOut.clear();
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        if (getOverlay(i)->isEnabled()) {
            HistologyOverlay::SelectionData selectionData(getOverlay(i)->getSelectionData());

            HistologySlicesFile* mf(selectionData.m_selectedFile);
            if (mf != NULL) {
                histologySlicesFilesOut.push_back(mf);
                overlayIndicesOut.push_back(i);
            }
        }
    }
    
    CaretAssert(histologySlicesFilesOut.size() == overlayIndicesOut.size());
}


/**
 * Add a displayed overlay.  If the maximum
 * number of surface overlays is reached,
 * this method has no effect.
 */
void 
HistologyOverlaySet::addDisplayedOverlay()
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
HistologyOverlaySet::getNumberOfDisplayedOverlays() const
{
    return m_numberOfDisplayedOverlays;
}

/**
 * Sets the number of displayed overlays.
 * @param numberOfDisplayedOverlays
 *   Number of overlays for display.
 */
void 
HistologyOverlaySet::setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays)
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
HistologyOverlaySet::insertOverlayAbove(const int32_t overlayIndex)
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
HistologyOverlaySet::insertOverlayBelow(const int32_t overlayIndex)
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
HistologyOverlaySet::removeDisplayedOverlay(const int32_t overlayIndex)
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
HistologyOverlaySet::moveDisplayedOverlayUp(const int32_t overlayIndex)
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
HistologyOverlaySet::moveDisplayedOverlayDown(const int32_t overlayIndex)
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
HistologyOverlaySet::initializeOverlays()
{
    /*
     * Enable top overlay
     */
    const int32_t numberOfDisplayedOverlays(getNumberOfDisplayedOverlays());
    for (int32_t i = 0; i < numberOfDisplayedOverlays; i++) {
        getOverlay(i)->setEnabled(i == 0);
    }
}


/**
 * For the given caret  data file, find overlays in which the
 * file is selected and return the indices of the selected maps.
 *
 * @param histologySlicesFile
 *    The caret  data file.
 * @param isLimitToEnabledOverlays
 *    If true, only include  indices for overlay that are enabled.
 *    Otherwise, include  indices for all overlays.
 * @param selectedIndicesOut
 *    Output containing map indices for the given caret  data files
 *    that are selected as overlays in this overlay set.
 */
void
HistologyOverlaySet::getSelectedIndicesForFile(const HistologySlicesFile* histologySlicesFile,
                                           const bool isLimitToEnabledOverlays,
                                           std::vector<int32_t>& selectedIndicesOut) const
{
    selectedIndicesOut.clear();
    
    /*
     * Put indices in a set to avoid duplicates and keep them sorted.
     */
    std::set<int32_t> indicesSet;
    
    const int32_t numberOfOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        HistologyOverlay* overlay = const_cast<HistologyOverlay*>(getOverlay(i));
        bool checkIt = true;
        if (isLimitToEnabledOverlays) {
            if (overlay->isEnabled() == false) {
                checkIt = false;
            }
        }
        
        if (checkIt) {
            const HistologyOverlay::SelectionData selectionData(overlay->getSelectionData());
            if (selectionData.m_selectedFile == histologySlicesFile) {
                indicesSet.insert(selectionData.m_selectedSliceIndex);
            }
        }
    }
    
    selectedIndicesOut.insert(selectedIndicesOut.end(),
                                 indicesSet.begin(),
                                 indicesSet.end());
}

/**
 * Reset the yoking status of all overlays to off.
 */
void
HistologyOverlaySet::resetOverlayYokingToOff()
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
HistologyOverlaySet::saveToScene(const SceneAttributes* sceneAttributes,
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
HistologyOverlaySet::restoreFromScene(const SceneAttributes* sceneAttributes,
                             const SceneClass* sceneClass)
{
    initializeOverlays();
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
HistologyOverlaySet::receiveEvent(Event* event)
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
                HistologyOverlay* overlay = getOverlay(j);
                
                HistologyOverlay::SelectionData selectionData(overlay->getSelectionData());
                if (selectionData.m_selectedFile != NULL) {
                    mapYokeEvent->addHistologySlicesYokedFile(selectionData.m_selectedFile,
                                                              overlay->getMapYokingGroup(),
                                                              m_tabIndex);
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
            const HistologySlicesFile* eventHistologySlicesFile = selectMapEvent->getHistologySlicesFile();
//            const MapYokingGroupEnum::MediaAllFramesStatus allFramesStatus(MapYokingGroupEnum::getMediaAllFramesStatus(eventYokingGroup));
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                HistologyOverlay* mediaOverlay = getOverlay(j);
                
                if (mediaOverlay->getMapYokingGroup() == selectMapEvent->getMapYokingGroup()) {
                    HistologyOverlay::SelectionData selectionData(mediaOverlay->getSelectionData());
                    
                    if (selectionData.m_selectedFile != NULL) {
                        if (yokingGroupMapIndex < selectionData.m_selectedFile->getNumberOfHistologySlices()) {
                            mediaOverlay->setSelectionData(selectionData.m_selectedFile,
                                                           yokingGroupMapIndex);
                        }
                        
//                        switch (allFramesStatus) {
//                            case MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_NO_CHANGE:
//                                break;
//                            case MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_ON:
//                                mediaOverlay->setCziAllScenesSelected(true);
//                                break;
//                            case MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_OFF:
//                                mediaOverlay->setCziAllScenesSelected(false);
//                                break;
//                        }
                        
                        if (selectionData.m_selectedFile == eventHistologySlicesFile) {
                            /* only alter status if event was sent by mappable file */
                            if (selectMapEvent->getHistologySlicesFile() != NULL) {
                                mediaOverlay->setEnabled(yokingGroupSelectedStatus);
                            }
                        }
                    }
                }
            }
            
            selectMapEvent->setEventProcessed();
        }
    }
}
