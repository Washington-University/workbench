
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
 * @param tabIndex
 * Index of the tab
 * @param overlayIndex
 * Index of overlasy
 */
MediaOverlay::MediaOverlay(const int32_t tabIndex,
                           const int32_t overlayIndex)
: m_tabIndex(tabIndex),
m_overlayIndex(overlayIndex)
{
    m_opacity = 1.0;
    m_enabled = false;
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    m_selectedFile  = NULL;
    m_selectedFrameIndex = -1;
    m_allFramesSelectedFlag = true;
    m_selectedChannelIndex = s_ALL_CHANNELS_INDEX;
    m_cziResolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO2;
    m_cziManualPyramidLayerIndex = 1;
    m_name = "Overlay ";
    
    /* Note file and frame index saved in 'saveScene' */
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_opacity", &m_opacity);
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup", &m_mapYokingGroup);
    m_sceneAssistant->add("m_allFramesSelectedFlag", &m_allFramesSelectedFlag);
    m_sceneAssistant->add("m_selectedChannelIndex", &m_selectedChannelIndex);
    m_sceneAssistant->add<CziImageResolutionChangeModeEnum,CziImageResolutionChangeModeEnum::Enum>("m_cziResolutionChangeMode", &m_cziResolutionChangeMode);
    m_sceneAssistant->add("m_cziManualPyramidLayerIndex", &m_cziManualPyramidLayerIndex);
    
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
 * Set the CZI pyramid layer index
 * @param pyramidLayerIndex
 *   New pyramid layer index
 */
void
MediaOverlay::setCziPyramidLayerIndex(const int32_t pyramidLayerIndex)
{
    m_cziManualPyramidLayerIndex = pyramidLayerIndex;
}

/**
 * Set all CZI scenes selected
 * @param selectAll
 *    New status
 */
void
MediaOverlay::setCziAllScenesSelected(const bool selectAll)
{
    m_allFramesSelectedFlag = selectAll;
}

/**
 * @return Value returned by 'getSelectedChannelIndex()' when ALL channels are selected
 */
int32_t
MediaOverlay::getAllChannelsSelectedIndexValue()
{
    return s_ALL_CHANNELS_INDEX;
}

/**
 * @return The selected channel index
 * @see getAllChannelsSelectedIndexValue()
 */
int32_t
MediaOverlay::getSelectedChannelIndex() const
{
    return m_selectedChannelIndex;
}

/**
 * Set the selected channel index
 * @param channelIndex
 *    New  channel index
 */
void
MediaOverlay::setSelectedChannelIndex(const int32_t channelIndex)
{
    m_selectedChannelIndex = channelIndex;
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
            const SelectionData selectionData(getSelectionData());
            if (selectionData.m_selectedMediaFile != NULL) {
                descriptionOut.addLine("File: "+
                                       selectionData.m_selectedMediaFile->getFileNameNoPath());
                descriptionOut.addLine("Frame Index: "
                                       + AString::number(selectionData.m_selectedFrameIndex + 1));
                descriptionOut.addLine("All Frames: "
                                       + AString::fromBool(selectionData.m_allFramesSelectedFlag));
                descriptionOut.addLine("Channel Index: "
                                       + AString::number(selectionData.m_selectedChannelIndex));
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
    m_opacity                    = overlay->m_opacity;
    m_enabled                    = overlay->m_enabled;
    m_mapYokingGroup             = overlay->m_mapYokingGroup;
    m_selectedFile               = overlay->m_selectedFile;
    m_selectedFrameIndex         = overlay->m_selectedFrameIndex;
    m_allFramesSelectedFlag      = overlay->m_allFramesSelectedFlag;
    m_selectedChannelIndex       = overlay->m_selectedChannelIndex;
    m_cziResolutionChangeMode    = overlay->m_cziResolutionChangeMode;
    m_cziManualPyramidLayerIndex = overlay->m_cziManualPyramidLayerIndex;
}

/**
 * Swap the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
MediaOverlay::swapData(MediaOverlay* overlay)
{
    MediaOverlay* swapOverlay = new MediaOverlay(m_tabIndex, m_overlayIndex);
    
    swapOverlay->copyData(overlay);
    
    overlay->copyData(this);
    copyData(swapOverlay);
    
    delete swapOverlay;
}

/**
 * @return Selection data for this overlay
 */
MediaOverlay::SelectionData
MediaOverlay::getSelectionData() const
{
    MediaOverlay* nonConstThis(const_cast<MediaOverlay*>(this));
    CaretAssert(nonConstThis);
    return nonConstThis->getSelectionData();
}

/**
 * @return Selection data for this overlay
 */
MediaOverlay::SelectionData
MediaOverlay::getSelectionData()
{
    std::vector<MediaFile*> allFiles;
    
    /**
     * Get the data files.
     */
    EventMediaFilesGet mediaFilesEvent;
    EventManager::get()->sendEvent(mediaFilesEvent.getPointer());
    allFiles = mediaFilesEvent.getMediaFiles();
    
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
            if ( ! allFiles.empty()) {
                for (auto& file : allFiles) {
                    m_selectedFile = file;
                    m_selectedFrameIndex = 0;
                    break;
                }
            }
        }
    }
    
    CziImageFile* cziImageFile((m_selectedFile != NULL)
                               ? m_selectedFile->castToCziImageFile()
                               : NULL);
    const int32_t numberOfFrames((m_selectedFile != NULL)
                                 ? m_selectedFile->getNumberOfFrames()
                                 : 0);
    const bool fileSupportsAllFramesFlag((cziImageFile != NULL)
                                         && (numberOfFrames > 1));
    
    if (m_selectedFile != NULL) {
        if (m_selectedFile != m_previousSelectedFile) {
            /*
             * Selected file has changed.
             * Set to first frame.
             * Enable all frames if supported by file
             */
            m_selectedFrameIndex = 0;
            m_allFramesSelectedFlag = fileSupportsAllFramesFlag;
        }
    }

    bool supportsYokingFlag(false);
    AString selectedFrameName;
    if (m_selectedFile != NULL) {
        if ((m_selectedFrameIndex >= 0)
            && (m_selectedFrameIndex < numberOfFrames)) {
            selectedFrameName = m_selectedFile->getFrameName(m_selectedFrameIndex);
        }
        supportsYokingFlag = (numberOfFrames > 1);
    }
    
    int32_t cziManualPyramidLayerMinimumValue(0);
    int32_t cziManualPyramidLayerMaximumValue(0);
    if (cziImageFile != NULL) {
        cziImageFile->getPyramidLayerRangeForFrame(m_selectedFrameIndex,
                                                   m_allFramesSelectedFlag,
                                                   cziManualPyramidLayerMinimumValue,
                                                   cziManualPyramidLayerMaximumValue);
    }
    
    int32_t selectedChannelIndex(s_ALL_CHANNELS_INDEX);
    if (m_selectedFile != NULL) {
        const MediaFileChannelInfo* channelInfo(const_cast<const MediaFile*>(m_selectedFile)->getMediaFileChannelInfo());
        CaretAssert(channelInfo);
        if (channelInfo->isChannelsSupported()) {
            
            selectedChannelIndex = m_selectedChannelIndex;
            
            /*
             * If all channels selected but not supported,
             * switch to first channel
             */
            if (selectedChannelIndex == s_ALL_CHANNELS_INDEX) {
                if ( ! channelInfo->isAllChannelsSelectionSupported()) {
                    selectedChannelIndex = 0;
                }
            }
            
            /*
             * If an individual channel is selected but individual channels
             * are not supported, switch to ALL channels
             */
            if (selectedChannelIndex >= 0) {
                if ( ! channelInfo->isSingleChannelSelectionSupported()) {
                    selectedChannelIndex = s_ALL_CHANNELS_INDEX;
                }
            }
            
            /*
             * If single channel selected, ensure it is valid
             */
            if (channelInfo->isSingleChannelSelectionSupported()) {
                if (selectedChannelIndex >= channelInfo->getNumberOfChannels()) {
                    selectedChannelIndex = channelInfo->getNumberOfChannels() - 1;
                }
            }
        }
    }
    SelectionData selectionDataOut(m_tabIndex,
                                   m_overlayIndex,
                                   allFiles,
                                   m_selectedFile,
                                   cziImageFile,
                                   m_selectedFrameIndex,
                                   selectedFrameName,
                                   fileSupportsAllFramesFlag,
                                   m_allFramesSelectedFlag,
                                   supportsYokingFlag,
                                   selectedChannelIndex,
                                   m_cziResolutionChangeMode,
                                   m_cziManualPyramidLayerIndex,
                                   cziManualPyramidLayerMinimumValue,
                                   cziManualPyramidLayerMaximumValue);
   
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
    
    const SelectionData selectionData(getSelectionData());
    
    if ((selectionData.m_selectedMediaFile != NULL)
        && (selectionData.m_selectedFrameIndex >= 0)) {
        sceneClass->addPathName("selectedFileNameWithPath",
                                selectionData.m_selectedMediaFile->getFileName());
        sceneClass->addString("selectedFile",
                              selectionData.m_selectedMediaFile->getFileNameNoPath());
        sceneClass->addString("selectedName",
                              selectionData.m_selectedFrameName);
        sceneClass->addInteger("selectedFrameIndex",
                               selectionData.m_selectedFrameIndex);
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
    
    /**
     * Get the data files.
     */
    EventMediaFilesGet mediaFilesEvent;
    EventManager::get()->sendEvent(mediaFilesEvent.getPointer());
    std::vector<MediaFile*> mediaFiles(mediaFilesEvent.getMediaFiles());

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
    for (auto& dataFile : mediaFiles) {
        if (selectedFileNameWithPath == dataFile->getFileName()) {
            matchedFile = dataFile;
            break;
        }
    }
    
    /*
     * Match by name only
     */
    if (matchedFile == NULL) {
        for (auto& dataFile : mediaFiles) {
            if (selectedFileName == dataFile->getFileName()) {
                matchedFile = dataFile;
                break;
            }
        }
    }
    
    if (matchedFile == NULL) {
        CaretLogWarning("Unable to restore image overlay file: "
                        + selectedFileName);
        if ( ! mediaFiles.empty()) {
            CaretAssertVectorIndex(mediaFiles, 0);
            matchedFile = mediaFiles[0];
        }
    }
    
    setSelectionData(matchedFile,
                     selectedFrameIndex);
    m_previousSelectedFile = matchedFile;
}


