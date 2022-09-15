
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

#define __HISTOLOGY_OVERLAY_DECLARE__
#include "HistologyOverlay.h"
#undef __HISTOLOGY_OVERLAY_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventHistologySlicesFilesGet.h"
#include "HistologySlice.h"
#include "HistologySlicesFile.h"
#include "HistologySliceImage.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


/**
 * \class Overlay
 * \brief  An overlay for selection of media data.
 */

/**
 * Constructor
 * @param tabIndex
 * Index of the tab
 * @param overlayIndex
 * Index of overlasy
 */
HistologyOverlay::HistologyOverlay(const int32_t tabIndex,
                           const int32_t overlayIndex)
: m_tabIndex(tabIndex),
m_overlayIndex(overlayIndex)
{
    m_opacity = 1.0;
    m_enabled = false;
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    m_selectedFile  = NULL;
    m_selectedSliceIndex = -1;
    m_name = "Overlay ";
    
    /* Note file and slice index saved in 'saveScene' */
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_opacity", &m_opacity);
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup", &m_mapYokingGroup);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_OVERLAY_VALIDATE);
}

/**
 * Destructor.
 */
HistologyOverlay::~HistologyOverlay()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_sceneAssistant;
}

/**
 * Receives events that this object is listening for.
 *
 * @param event
 *    An event.
 */
void
HistologyOverlay::receiveEvent(Event* /*event*/)
{
}

/**
 * Set the number of this overlay.
 * 
 * @param overlayIndex
 *    Index for this overlay.
 */
void 
HistologyOverlay::setOverlayNumber(const int32_t overlayIndex)
{    
    m_name = "Overlay " + AString::number(overlayIndex + 1);
}

/**
 * Get the opacity.
 * 
 * @return  The opacity.
 */
float 
HistologyOverlay::getOpacity() const
{
    return m_opacity;
}

/**
 * Set the opacity.
 *
 * @param opacity
 *    New value for opacity.
 */
void 
HistologyOverlay::setOpacity(const float opacity)
{
    m_opacity = opacity;
}

AString
HistologyOverlay::getName() const
{
    return m_name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologyOverlay::toString() const
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
HistologyOverlay::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    HistologyOverlay* me = const_cast<HistologyOverlay*>(this);
    if (me != NULL) {
        if (me->isEnabled()) {
            const SelectionData selectionData(getSelectionData());
            if (selectionData.m_selectedFile != NULL) {
                descriptionOut.addLine("File: "+
                                       selectionData.m_selectedFile->getFileNameNoPath());
                descriptionOut.addLine("Slice Index: "
                                       + AString::number(selectionData.m_selectedSliceIndex + 1));
            }
        }
    }
}

/**
 * @return Enabled status for this surface overlay.
 */
bool 
HistologyOverlay::isEnabled() const
{
    return m_enabled;
}

/**
 * Set the enabled status for this surface overlay.
 * @param enabled
 *    New status.
 */
void 
HistologyOverlay::setEnabled(const bool enabled)
{
    m_enabled = enabled;
}

/**
 * Copy the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
HistologyOverlay::copyData(const HistologyOverlay* overlay)
{
    CaretAssert(overlay);
    
    /*
     * These members are not copied since they
     * identify the overlay:
     *    name
     *    overlayIndex
     *
     */
    m_previousSelectedFile       = m_selectedFile;
    m_opacity                    = overlay->m_opacity;
    m_enabled                    = overlay->m_enabled;
    m_mapYokingGroup             = overlay->m_mapYokingGroup;
    m_selectedFile               = overlay->m_selectedFile;
    m_selectedSliceIndex         = overlay->m_selectedSliceIndex;
}

/**
 * Swap the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
HistologyOverlay::swapData(HistologyOverlay* overlay)
{
    HistologyOverlay* swapOverlay = new HistologyOverlay(m_tabIndex, m_overlayIndex);
    
    swapOverlay->copyData(overlay);
    
    overlay->copyData(this);
    copyData(swapOverlay);
    
    delete swapOverlay;
}

/**
 * @return Drawing information for this overlay
 * @param selectedSliceIndex
 *    Slice selected for drawingt
 */
std::vector<HistologyOverlay::DrawingData>
HistologyOverlay::getDrawingData(const int32_t selectedSliceIndex) const
{
    std::vector<HistologyOverlay::DrawingData> drawingDataOut;
    
    if (isEnabled()) {
        const SelectionData selectionData(getSelectionData());
        if (selectionData.m_selectedFile != NULL) {
            HistologySlice* slice(selectionData.m_selectedFile->getHistologySliceByIndex(selectedSliceIndex));
            if (slice != NULL) {
                const int32_t numImages(slice->getNumberOfHistologySliceImages());
                for (int32_t iImage = 0; iImage < numImages; iImage++) {
                    HistologySliceImage* sliceImage(slice->getHistologySliceImage(iImage));
                    CaretAssert(sliceImage);
                    MediaFile* mediaFile(sliceImage->getMediaFile());

                    DrawingData dd(m_tabIndex,
                                   m_overlayIndex,
                                   selectionData.m_selectedFile,
                                   mediaFile,
                                   selectedSliceIndex,
                                   m_selectedFile->getSliceNumberBySliceIndex(selectedSliceIndex),
                                   iImage,
                                   selectionData.m_supportsYokingFlag);
                    drawingDataOut.push_back(dd);
                }
            }
        }
    }
    
    return drawingDataOut;
}

/**
 * @return Selection data for this overlay
 */
HistologyOverlay::SelectionData
HistologyOverlay::getSelectionData() const
{
    HistologyOverlay* nonConstThis(const_cast<HistologyOverlay*>(this));
    CaretAssert(nonConstThis);
    return nonConstThis->getSelectionData();
}

/**
 * @return Selection data for this overlay
 */
HistologyOverlay::SelectionData
HistologyOverlay::getSelectionData()
{
    EventHistologySlicesFilesGet histologySlicesFilesEvent;
    EventManager::get()->sendEvent(histologySlicesFilesEvent.getPointer());
    std::vector<HistologySlicesFile*> allFiles(histologySlicesFilesEvent.getHistologySlicesFiles());
    
    /*
     * Does selected data file no longer exist?
     */
    if (std::find(allFiles.begin(),
                  allFiles.end(),
                  m_selectedFile) == allFiles.end()) {
        /*
         * Invalidate seleted file and disable yoking since
         * the selected file will change.
         */
        m_selectedFile = NULL;
        m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    }
    
    /*
     * If selected data file is valid, see if selected
     * indewx is still valid.  If not, use first map.
     */
    if (m_selectedFile != NULL) {
        if (m_selectedSliceIndex >= m_selectedFile->getNumberOfHistologySlices()) {
            m_selectedSliceIndex = m_selectedFile->getNumberOfHistologySlices() - 1;
        }
        if (m_selectedSliceIndex < 0) {
            m_selectedSliceIndex = 0;
        }
    }
    else {
        /*
         * Use in first file
         */
        if (m_selectedFile == NULL) {
            if ( ! allFiles.empty()) {
                for (auto& file : allFiles) {
                    m_selectedFile = file;
                    m_selectedSliceIndex = 0;
                    break;
                }
            }
        }
    }
    const int32_t numberOfSlices((m_selectedFile != NULL)
                                 ? m_selectedFile->getNumberOfHistologySlices()
                                 : 0);
    
    if (m_selectedFile != NULL) {
        if (m_selectedFile != m_previousSelectedFile) {
            /*
             * Selected file has changed.
             * Set to first slice index.
             */
            m_selectedSliceIndex = 0;
        }
    }

    bool supportsYokingFlag(false);
    int32_t selectedSliceNumber(-1);
    if (m_selectedFile != NULL) {
        if ((m_selectedSliceIndex >= 0)
            && (m_selectedSliceIndex < numberOfSlices)) {
            const HistologySlice* hs(m_selectedFile->getHistologySliceByIndex(m_selectedSliceIndex));
            CaretAssert(hs);
            selectedSliceNumber = hs->getSliceNumber();
        }
        supportsYokingFlag = (numberOfSlices > 1);
    }
    
    
    SelectionData selectionDataOut(m_tabIndex,
                                   m_overlayIndex,
                                   allFiles,
                                   m_selectedFile,
                                   m_selectedSliceIndex,
                                   selectedSliceNumber,
                                   supportsYokingFlag);
   
    /*
     * Needed to catch change of selected file in this function
     */
    m_previousSelectedFile = m_selectedFile;
    
    return selectionDataOut;
}

/**
 * Set the selected  file and index name.
 * @param selectedFile
 *    File that is selected.
 * @param selectedSliceIndex
 *    Slice ndex  that is selected.
 */
void 
HistologyOverlay::setSelectionData(HistologySlicesFile* selectedFile,
                               const int32_t selectedSliceIndex)
{
    m_selectedFile  = selectedFile;
    m_selectedSliceIndex = selectedSliceIndex;
    
    if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        if (m_selectedFile == NULL) {
            m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
        }
    }
}

/**
 * @return Selected map yoking group.
 */
MapYokingGroupEnum::Enum
HistologyOverlay::getMapYokingGroup() const
{
    return m_mapYokingGroup;
}

/**
 * Set the map yoking group.
 *
 * @param mapYokingGroup
 *    New value for map yoking group.
 */
void
HistologyOverlay::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
{
    m_mapYokingGroup = mapYokingGroup;
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
HistologyOverlay::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologyOverlay",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    const SelectionData selectionData(getSelectionData());
    
    if ((selectionData.m_selectedFile != NULL)
        && (selectionData.m_selectedSliceIndex >= 0)) {
        sceneClass->addPathName("selectedFileNameWithPath",
                                selectionData.m_selectedFile->getFileName());
        sceneClass->addString("selectedFile",
                              selectionData.m_selectedFile->getFileNameNoPath());
        sceneClass->addInteger("selectedSliceIndex",
                               selectionData.m_selectedSliceIndex);
    }
    else {
        sceneClass->addPathName("selectedFileNameWithPath",
                                "");
        sceneClass->addString("selectedFile",
                              "");
        sceneClass->addInteger("selectedSliceIndex",
                               -1);
    }
    
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
HistologyOverlay::restoreFromScene(const SceneAttributes* sceneAttributes,
                                       const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    /**
     * Get the data files.
     */
    EventHistologySlicesFilesGet histologySlicesFilesEvent;
    EventManager::get()->sendEvent(histologySlicesFilesEvent.getPointer());
    std::vector<HistologySlicesFile*> allFiles(histologySlicesFilesEvent.getHistologySlicesFiles());

    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);

    
    const AString selectedFileNameWithPath = sceneClass->getPathNameValue("selectedFileNameWithPath");
    
    const AString selectedFileName = sceneClass->getStringValue("selectedFile",
                                                                   "");
    const int32_t selectedSliceIndex = sceneClass->getIntegerValue("selectedSliceIndex",
                                                                 -1);
    

    HistologySlicesFile* matchedFile = NULL;
    
    /*
     * Try to match with full path
     */
    for (auto& dataFile : allFiles) {
        if (selectedFileNameWithPath == dataFile->getFileName()) {
            matchedFile = dataFile;
            break;
        }
    }
    
    /*
     * Match by name only
     */
    if (matchedFile == NULL) {
        for (auto& dataFile : allFiles) {
            if (selectedFileName == dataFile->getFileName()) {
                matchedFile = dataFile;
                break;
            }
        }
    }
    
    if (matchedFile == NULL) {
        CaretLogWarning("Unable to restore image overlay file: "
                        + selectedFileName);
        if ( ! allFiles.empty()) {
            CaretAssertVectorIndex(allFiles, 0);
            matchedFile = allFiles[0];
        }
    }
    
    setSelectionData(matchedFile,
                     selectedSliceIndex);
    m_previousSelectedFile = matchedFile;
}


