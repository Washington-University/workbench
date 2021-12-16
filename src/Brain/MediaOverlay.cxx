
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

#define __MEDIA_OVERLAY_DECLARE__
#include "MediaOverlay.h"
#undef __MEDIA_OVERLAY_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziImageFile.h"
#include "EventManager.h"
#include "EventMediaFilesGet.h"
#include "EventOverlayValidate.h"
#include "MediaFile.h"
#include "ImageFile.h"
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
 */
MediaOverlay::MediaOverlay()
{
    m_opacity = 1.0;
    m_selectedFile  = NULL;
    m_selectedFrameIndex = -1;
    m_name = "Overlay ";
    m_enabled = false;
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    m_cziResolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO_PYRAMID;
    m_cziAllScenesSelectedFlag = true;
    m_cziPyramidLayerIndex = 1;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_opacity", &m_opacity);
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup", &m_mapYokingGroup);
    m_sceneAssistant->add<CziImageResolutionChangeModeEnum,CziImageResolutionChangeModeEnum::Enum>("m_cziResolutionChangeMode", &m_cziResolutionChangeMode);
    m_sceneAssistant->add("m_cziAllScenesSelectedFlag", &m_cziAllScenesSelectedFlag);
    m_sceneAssistant->add("m_cziPyramidLayerIndex", &m_cziPyramidLayerIndex);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_OVERLAY_VALIDATE);
}

/**
 * Destructor.
 */
MediaOverlay::~MediaOverlay()
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
MediaOverlay::receiveEvent(Event* /*event*/)
{
}

/**
 * Set the number of this overlay.
 * 
 * @param overlayIndex
 *    Index for this overlay.
 */
void 
MediaOverlay::setOverlayNumber(const int32_t overlayIndex)
{    
    m_name = "Overlay " + AString::number(overlayIndex + 1);
}

/**
 * Get the opacity.
 * 
 * @return  The opacity.
 */
float 
MediaOverlay::getOpacity() const
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
MediaOverlay::setOpacity(const float opacity)
{
    m_opacity = opacity;
}

AString
MediaOverlay::getName() const
{
    return m_name;
}

/**
 * @return The CZI pyramid layer index
 */
int32_t
MediaOverlay::getCziPyramidLayerIndex() const
{
    return m_cziPyramidLayerIndex;
}

/**
 * Set the CZI pyramid layer index
 * @param pyramidLayerIndex
 *   New pyramid layer index
 */
void
MediaOverlay::setCziPyramidLayerIndex(const int32_t pyramidLayerIndex)
{
    m_cziPyramidLayerIndex = pyramidLayerIndex;
}

/**
 * @return The valid range of the pyramid layer indices
 */
std::array<int32_t, 2>
MediaOverlay::getCziPyramidLayerRange() const
{
    std::array<int32_t, 2> indexRange { 1, 1 };
    
    MediaFile* mediaFile(NULL);
    int32_t frameIndex(-1);
    const_cast<MediaOverlay*>(this)->getSelectionData(mediaFile, frameIndex);
    
    if (mediaFile != NULL) {
        const CziImageFile* cziFile(mediaFile->castToCziImageFile());
        cziFile->getPyramidLayerRange(indexRange[0], indexRange[1]);
    }
    
    return indexRange;
}


/**
 * @return Is all CZI scenes selected
 */
bool
MediaOverlay::isAllCziScenesSelected() const
{
    return m_cziAllScenesSelectedFlag;
}

/**
 * Set all CZI scenes selected
 * @param selectAll
 *    New status
 */
void
MediaOverlay::setCziAllScenesSelected(const bool selectAll)
{
    m_cziAllScenesSelectedFlag = selectAll;
}

/**
 * @return The CZI resolution change mode
 */
CziImageResolutionChangeModeEnum::Enum
MediaOverlay::getCziResolutionChangeMode() const
{
    return m_cziResolutionChangeMode;
}

/**
 * Set the CZI resolution change mode
 * @param resolutionChangeMode
 *    New change mode
 */
void
MediaOverlay::setCziResolutionChangeMode(const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode)
{
    m_cziResolutionChangeMode = resolutionChangeMode;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
MediaOverlay::toString() const
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
MediaOverlay::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    MediaOverlay* me = const_cast<MediaOverlay*>(this);
    if (me != NULL) {
        if (me->isEnabled()) {
            MediaFile* file = NULL;
            int32_t index = 0;
            me->getSelectionData(file,
                                 index);
            if (file != NULL) {
                descriptionOut.addLine("File: "+
                                       file->getFileNameNoPath());
                descriptionOut.addLine("Frame Index: "
                                       + AString::number(index + 1));
            }
        }
    }
}

/**
 * @return Enabled status for this surface overlay.
 */
bool 
MediaOverlay::isEnabled() const
{
    return m_enabled;
}

/**
 * Set the enabled status for this surface overlay.
 * @param enabled
 *    New status.
 */
void 
MediaOverlay::setEnabled(const bool enabled)
{
    m_enabled = enabled;
}

/**
 * Copy the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
MediaOverlay::copyData(const MediaOverlay* overlay)
{
    CaretAssert(overlay);
    
    /*
     * These members are not copied since they
     * identify the overlay:
     *    name
     *    overlayIndex
     *
     */
    m_opacity = overlay->m_opacity;
    m_enabled = overlay->m_enabled;
    
    m_selectedFile = overlay->m_selectedFile;
    m_selectedFrameIndex = overlay->m_selectedFrameIndex;
    m_mapYokingGroup = overlay->m_mapYokingGroup;
    m_cziResolutionChangeMode = overlay->m_cziResolutionChangeMode;
    m_cziAllScenesSelectedFlag = overlay->m_cziAllScenesSelectedFlag;
    m_cziPyramidLayerIndex = overlay->m_cziPyramidLayerIndex;
}

/**
 * Swap the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
MediaOverlay::swapData(MediaOverlay* overlay)
{
    MediaOverlay* swapOverlay = new MediaOverlay();
    
    swapOverlay->copyData(overlay);
    
    overlay->copyData(this);
    copyData(swapOverlay);
    
    delete swapOverlay;
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param selectedFileOut
 *    The selected  file.  May be NULL.
 * @param selectedFrameIndexOut
 *    Index  in the selected file.
 */
void 
MediaOverlay::getSelectionData(MediaFile* &selectedFileOut,
                               int32_t& selectedFrameIndexOut)
{
    std::vector<MediaFile*> files;
    
    getSelectionData(files,
                     selectedFileOut,
                     selectedFrameIndexOut);
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param filesOut
 *    Contains all files that can be selected.
 * @param selectedFileOut
 *    The selected  file.  May be NULL.
 * @param selectedFrameIndexOut
 *    Index  in the selected file.
 */
void 
MediaOverlay::getSelectionData(std::vector<MediaFile*>& filesOut,
                          MediaFile* &selectedFileOut,
                          int32_t& selectedFrameIndexOut)
{
    filesOut.clear();
    selectedFileOut = NULL;
    selectedFrameIndexOut = -1;
    
    /**
     * Get the data files.
     */
    EventMediaFilesGet mediaFilesEvent;
    EventManager::get()->sendEvent(mediaFilesEvent.getPointer());
    filesOut = mediaFilesEvent.getMediaFiles();
    
    
    /*
     * Does selected data file still no longer exist?
     */
    if (std::find(filesOut.begin(),
                  filesOut.end(),
                  m_selectedFile) == filesOut.end()) {
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
        if (m_selectedFrameIndex >= m_selectedFile->getNumberOfFrames()) {
            m_selectedFrameIndex = m_selectedFile->getNumberOfFrames() - 1;
        }
        if (m_selectedFrameIndex < 0) {
            m_selectedFrameIndex = 0;
        }
    }
    else {
        /*
         * Use in first file
         */
        if (m_selectedFile == NULL) {
            if ( ! filesOut.empty()) {
                for (auto& file : filesOut) {
                    m_selectedFile = file;
                    m_selectedFrameIndex = 0;
                    break;
                }
            }
        }
    }
    
    selectedFileOut = m_selectedFile;
    if (selectedFileOut != NULL) {
        selectedFrameIndexOut = m_selectedFrameIndex;
    }
}

/**
 * Set the selected  file and index name.
 * @param selectedFile
 *    File that is selected.
 * @param selectedName
 *    Index name that is selected.
 */
void 
MediaOverlay::setSelectionData(MediaFile* selectedFile,
                               const int32_t selectedFrameIndex)
{
    m_selectedFile  = selectedFile;
    m_selectedFrameIndex = selectedFrameIndex;
    
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
MediaOverlay::getMapYokingGroup() const
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
MediaOverlay::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
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
MediaOverlay::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "MediaOverlay",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    std::vector<MediaFile*> files;
    MediaFile* selectedFile = NULL;
    int32_t selectedFrameIndex;
    getSelectionData(files,
                     selectedFile,
                     selectedFrameIndex);
    
    if ((selectedFile != NULL)
        && (selectedFrameIndex >= 0)) {
        sceneClass->addPathName("selectedFileNameWithPath",
                                selectedFile->getFileName());
        sceneClass->addString("selectedFile",
                              selectedFile->getFileNameNoPath());
        sceneClass->addString("selectedName",
                              AString::number(selectedFrameIndex));
        sceneClass->addInteger("selectedFrameIndex",
                               selectedFrameIndex);
    }
    else {
        sceneClass->addPathName("selectedFileNameWithPath",
                                "");
        sceneClass->addString("selectedFile",
                              "");
        sceneClass->addString("selectedName",
                              "");
        sceneClass->addInteger("selectedFrameIndex",
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
MediaOverlay::restoreFromScene(const SceneAttributes* sceneAttributes,
                                       const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    /*
     * Making a call to getSelectionData() to get the availble
     * files
     */
    std::vector<MediaFile*> allFiles;
    MediaFile* unusedSelectedFile = NULL;
    int32_t unusedselectedFrameIndex;
    getSelectionData(allFiles,
                     unusedSelectedFile,
                     unusedselectedFrameIndex);
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);

    
    const AString selectedFileNameWithPath = sceneClass->getPathNameValue("selectedFileNameWithPath");
    
    const AString selectedFileName = sceneClass->getStringValue("selectedFile",
                                                                   "");
    const AString selectedName = sceneClass->getStringValue("selectedName",
                                                                   "");
    const int32_t selectedFrameIndex = sceneClass->getIntegerValue("selectedFrameIndex",
                                                                 -1);
    

    MediaFile* matchedFile = NULL;
    
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
                     selectedFrameIndex);
}



