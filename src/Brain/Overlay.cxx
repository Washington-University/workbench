
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

#define __OVERLAY_DECLARE__
#include "Overlay.h"
#undef __OVERLAY_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventOverlayValidate.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "PlainTextStringBuilder.h"
#include "RgbaFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


/**
 * \class Overlay
 * \brief  An overlay for selection of mappable data.
 */

/**
 * Constructor for files in the given structurs and perhaps volume files.
 *
 * @param includeSurfaceStructures
 *    Surface structures for files available in this overlay.
 * @param includeVolumeFiles
 *    Include (or not) volume files.
 */
Overlay::Overlay(const std::vector<StructureEnum::Enum>& includeSurfaceStructures,
                 const Overlay::IncludeVolumeFiles includeVolumeFiles)
: m_includeSurfaceStructures(includeSurfaceStructures),
m_includeVolumeFiles(includeVolumeFiles)
{
    m_opacity = 1.0;
    
    m_name = "Overlay ";
    m_enabled = true;
    m_paletteDisplayedFlag = false;
    m_yokingGroup = OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF;
    
    m_wholeBrainVoxelDrawingMode = WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_opacity", &m_opacity);
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add("m_paletteDisplayedFlag", &m_paletteDisplayedFlag);
    m_sceneAssistant->add<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>("m_wholeBrainVoxelDrawingMode",
                                                                                        &m_wholeBrainVoxelDrawingMode);
    m_sceneAssistant->add<OverlayYokingGroupEnum, OverlayYokingGroupEnum::Enum>("m_yokingGroup",
                                                                                &m_yokingGroup);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_OVERLAY_VALIDATE);
}

/**
 * Destructor.
 */
Overlay::~Overlay()
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
Overlay::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_OVERLAY_VALIDATE) {
        EventOverlayValidate* eov = dynamic_cast<EventOverlayValidate*>(event);
        CaretAssert(eov);
        eov->testValidOverlay(this);
    }
}

/**
 * Set the number of this overlay.
 * 
 * @param overlayIndex
 *    Index for this overlay.
 */
void 
Overlay::setOverlayNumber(const int32_t overlayIndex)
{    
    m_name = "Overlay " + AString::number(overlayIndex + 1);
}

/**
 * Get the opacity.
 * 
 * @return  The opacity.
 */
float 
Overlay::getOpacity() const
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
Overlay::setOpacity(const float opacity)
{
    m_opacity = opacity;
}

/**
 * @return The voxel drawing mode for whole brain.
 */
WholeBrainVoxelDrawingMode::Enum
Overlay::getWholeBrainVoxelDrawingMode() const
{
    return m_wholeBrainVoxelDrawingMode;
}

/**
 * Set the voxel drawing mode for whole brain.
 *
 * @param wholeBrainVoxelDrawingMode
 *    New mode.
 */
void
Overlay::setWholeBrainVoxelDrawingMode(const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode)
{
    m_wholeBrainVoxelDrawingMode = wholeBrainVoxelDrawingMode;
}

AString
Overlay::getName() const
{
    return m_name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Overlay::toString() const
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
Overlay::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    Overlay* me = const_cast<Overlay*>(this);
    if (me != NULL) {
        if (me->isEnabled()) {
            CaretMappableDataFile* mapFile = NULL;
            int32_t mapIndex = 0;
            me->getSelectionData(mapFile,
                                 mapIndex);
            if (mapFile != NULL) {
                descriptionOut.addLine("File: "+
                                       mapFile->getFileNameNoPath());
                if (mapFile->hasMapAttributes()) {
                    if ((mapIndex >= 0)
                        && (mapIndex < mapFile->getNumberOfMaps())) {
                        descriptionOut.addLine("Map Index: "
                                               + AString::number(mapIndex + 1));
                        descriptionOut.addLine("Map Name: "
                                               + mapFile->getMapName(mapIndex));
                    }
                }
                descriptionOut.addLine("Structure: "
                                       + StructureEnum::toGuiName(mapFile->getStructure()));
            }
        }
    }
}

/**
 * @return Enabled status for this surface overlay.
 */
bool 
Overlay::isEnabled() const
{
    return m_enabled;
}

/**
 * Set the enabled status for this surface overlay.
 * @param enabled
 *    New status.
 */
void 
Overlay::setEnabled(const bool enabled)
{
    m_enabled = enabled;
}

/**
 * Copy the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
Overlay::copyData(const Overlay* overlay)
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
    
//    m_mapFiles = overlay->m_mapFiles;
    m_selectedMapFile = overlay->m_selectedMapFile;
    m_selectedMapIndex = overlay->m_selectedMapIndex;
//    m_selectedMapUniqueID = overlay->m_selectedMapUniqueID;
    m_paletteDisplayedFlag = overlay->m_paletteDisplayedFlag;
    m_yokingGroup = overlay->m_yokingGroup;
}

/**
 * Swap the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
Overlay::swapData(Overlay* overlay)
{
    Overlay* swapOverlay = new Overlay(m_includeSurfaceStructures,
                                       m_includeVolumeFiles);
    
    swapOverlay->copyData(overlay);
    
    overlay->copyData(this);
    copyData(swapOverlay);
    
    delete swapOverlay;
}

///**
// * Get the current selection.  If the current selection is
// * invalid, new map data will be selected.
// *
// * @param mapDataFileTypeOut
// *    Type of map file out.
// * @param selectedMapUniqueIDOut
// *    UniqueID of map that is selected.
// */
//void 
//Overlay::getSelectionData(DataFileTypeEnum::Enum& mapDataFileTypeOut,
//                          AString& selectedMapUniqueIDOut)
//{
//    std::vector<CaretMappableDataFile*> allFiles;
//    CaretMappableDataFile* selectedFile;
//    int32_t selectedIndex;
//    getSelectionData(allFiles,
//                           selectedFile,
//                           selectedMapUniqueIDOut,
//                           selectedIndex);
//    
//    mapDataFileTypeOut = DataFileTypeEnum::UNKNOWN;
//    if (selectedFile != NULL) {
//        mapDataFileTypeOut = selectedFile->getDataFileType();
//    }
//    else {
//        selectedMapUniqueIDOut = "";
//    }    
//}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void 
Overlay::getSelectionData(CaretMappableDataFile* &selectedMapFileOut,
                          int32_t& selectedMapIndexOut)
{
    std::vector<CaretMappableDataFile*> mapFiles;
    //AString mapUniqueID;
    
    getSelectionData(mapFiles, 
                           selectedMapFileOut, 
                           //mapUniqueID,
                           selectedMapIndexOut);
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param mapFilesOut
 *    Contains all map files that can be selected.
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedMapUniqueIDOut
 *    UniqueID of selected map.
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void 
Overlay::getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                          CaretMappableDataFile* &selectedMapFileOut,
                          //AString& selectedMapUniqueIDOut,
                          int32_t& selectedMapIndexOut)
{
    mapFilesOut.clear();
    selectedMapFileOut = NULL;
    //selectedMapUniqueIDOut = "";
    selectedMapIndexOut = -1;
    
    /**
     * Get the data files.
     */
    std::vector<CaretMappableDataFile*> allDataFiles;
    EventCaretMappableDataFilesGet eventGetMapDataFiles;
    EventManager::get()->sendEvent(eventGetMapDataFiles.getPointer());
    eventGetMapDataFiles.getAllFiles(allDataFiles);
    
    bool showVolumeMapFiles  = false;
    switch (m_includeVolumeFiles) {
        case INCLUDE_VOLUME_FILES_NO:
            break;
        case INCLUDE_VOLUME_FILES_YES:
            showVolumeMapFiles = true;
            break;
    }

    bool showSurfaceMapFiles = false;
    if ( ! m_includeSurfaceStructures.empty()) {
        showSurfaceMapFiles = true;
    }
    
    /*
     * Use only those data files that meet criteria.
     */
    for (std::vector<CaretMappableDataFile*>::iterator iter = allDataFiles.begin();
         iter != allDataFiles.end();
         iter++) {
        CaretMappableDataFile* mapFile = *iter;
        bool useIt = false;
        bool mappable = false;
        
        if (mapFile->isSurfaceMappable()) {
            mappable = true;
            if (showSurfaceMapFiles) {
                
                for (std::vector<StructureEnum::Enum>::const_iterator iter = m_includeSurfaceStructures.begin();
                     iter != m_includeSurfaceStructures.end();
                     iter++) {
                    if (mapFile->isMappableToSurfaceStructure(*iter)) {
                        useIt = true;
                        break;
                    }
                }
                
//                const StructureEnum::Enum mapFileStructure = mapFile->getStructure();
//                
//                if (mapFileStructure == StructureEnum::ALL) {
//                    useIt = true;
//                }
//                else if (std::find(m_includeSurfaceStructures.begin(),
//                              m_includeSurfaceStructures.end(),
//                              mapFile->getStructure())
//                         != m_includeSurfaceStructures.end()) {
//                    useIt = true;
//                }
            }
        }
        if (mapFile->isVolumeMappable()) {
            mappable = true;
            if (showVolumeMapFiles) {
                useIt = true;
            }
        }
        
        if (mappable == false) {
            CaretLogSevere("Map file is neither surface nor volume mappable: " + mapFile->getFileName());
        }
        
        if (useIt) {
            mapFilesOut.push_back(mapFile);
        }
    }
    
    /*
     * Does selected data file still no longer exist?
     */
    if (std::find(mapFilesOut.begin(), 
                  mapFilesOut.end(),
                  m_selectedMapFile) == mapFilesOut.end()) {
        m_selectedMapFile = NULL;
    }
    
    /*
     * If selected data file is valid, see if selected
     * map is still valid.  If not, use first map.
     */
    if (m_selectedMapFile != NULL) {
        if (m_selectedMapIndex >= m_selectedMapFile->getNumberOfMaps()) {
            m_selectedMapIndex = m_selectedMapFile->getNumberOfMaps() - 1;
        }
        if (m_selectedMapIndex < 0) {
            m_selectedMapIndex = 0;
        }
//        const int32_t mapIndex = m_selectedMapFile->getMapIndexFromUniqueID(m_selectedMapUniqueID);
//        if (mapIndex < 0) {
//            m_selectedMapUniqueID = m_selectedMapFile->getMapUniqueID(0);
//        }
    }
    else {
//        /*
//         * Look for a file that contains the selected map unique ID.
//         */
//        if (m_selectedMapUniqueID.isEmpty() == false) {
//            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
//                 iter != mapFilesOut.end();
//                 iter++) {
//                CaretMappableDataFile* mapTypeFile = *iter;
//                const int32_t mapIndex = mapTypeFile->getMapIndexFromUniqueID(m_selectedMapUniqueID);
//                if (mapIndex >= 0) {
//                    m_selectedMapFile = mapTypeFile;
//                    break;
//                }
//            }
//        }
        
        /*
         * Use first map in first file that has one or more maps.
         */
        if (m_selectedMapFile == NULL) {
            if (mapFilesOut.empty() == false) {
                for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
                     iter != mapFilesOut.end();
                     iter++) {
                    CaretMappableDataFile* mapTypeFile = *iter;
                    if (mapTypeFile->getNumberOfMaps() > 0) {
                        m_selectedMapFile = mapTypeFile;
                        m_selectedMapIndex = 0;
//                        m_selectedMapUniqueID = mapTypeFile->getMapUniqueID(0);
                    }
                }
            }
        }
    }
    
    selectedMapFileOut = m_selectedMapFile;
    if (selectedMapFileOut != NULL) {
        /*
         * Update for overlay yoking
         */
        if (m_yokingGroup != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
            const int32_t yokeMapIndex = OverlayYokingGroupEnum::getSelectedMapIndex(m_yokingGroup);
            if ((yokeMapIndex >= 0)
                && (yokeMapIndex < selectedMapFileOut->getNumberOfMaps())) {
                m_selectedMapIndex = yokeMapIndex;
//                m_selectedMapUniqueID = selectedMapFileOut->getMapUniqueID(yokeMapIndex);
            }
            else if (yokeMapIndex >= selectedMapFileOut->getNumberOfMaps()) {
                m_selectedMapIndex = selectedMapFileOut->getNumberOfMaps() - 1;
//                selectedMapUniqueIDOut = selectedMapFileOut->getNumberOfMaps() - 1;
            }
        }
        
//        selectedMapUniqueIDOut = m_selectedMapUniqueID;
        selectedMapIndexOut = m_selectedMapIndex;  //m_selectedMapFile->getMapIndexFromUniqueID(selectedMapUniqueIDOut);
    }
}

/**
 * Set the selected map file and map.
 * @param selectedMapFile 
 *    File that is selected.
 * @param selectedMapName
 *    Map name that is selected.
 */
void 
Overlay::setSelectionData(CaretMappableDataFile* selectedMapFile,
                          const int32_t selectedMapIndex)
{
    m_selectedMapFile = selectedMapFile;
    m_selectedMapIndex = selectedMapIndex;
//    m_selectedMapUniqueID = selectedMapFile->getMapUniqueID(selectedMapIndex);
    
    if (m_yokingGroup != OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
        if (selectedMapFile != NULL) {
            OverlayYokingGroupEnum::setSelectedMapIndex(m_yokingGroup,
                                                        selectedMapIndex);
        }
    }
}

/**
 * @return Is display of palette in graphics window enabled?
 */
bool 
Overlay::isPaletteDisplayEnabled() const
{
    return m_paletteDisplayedFlag;
}

/**
 * Set display of palette in graphics window.
 * @param enabled
 *   New status for palette display in graphics window.
 */
void 
Overlay::setPaletteDisplayEnabled(const bool enabled)
{
    m_paletteDisplayedFlag = enabled;
}

/**
 * @return Selected yoking group.
 */
OverlayYokingGroupEnum::Enum
Overlay::getYokingGroup() const
{
    return m_yokingGroup;
}

/**
 * Set the selected yoking group.
 *
 * @param yokingGroup
 *    New value for yoking group.
 */
void
Overlay::setYokingGroup(const OverlayYokingGroupEnum::Enum yokingGroup)
{
    m_yokingGroup = yokingGroup;
    
    if (m_yokingGroup == OverlayYokingGroupEnum::OVERLAY_YOKING_GROUP_OFF) {
        return;
    }
    
//    /*
//     * Find another browser tab using the same yoking as 'me' and copy
//     * yoked data from the other browser tab.
//     */
//    for (std::set<BrowserTabContent*>::iterator iter = s_allBrowserTabContent.begin();
//         iter != s_allBrowserTabContent.end();
//         iter++) {
//        BrowserTabContent* btc = *iter;
//        if (btc != this) {
//            if (btc->getYokingGroup() == m_yokingGroup) {
//                *m_viewingTransformation = *btc->m_viewingTransformation;
//                *m_volumeSliceViewingTransformation = *btc->m_volumeSliceViewingTransformation;
//                *m_volumeSliceSettings = *btc->m_volumeSliceSettings;
//                *m_wholeBrainSliceSettings = *btc->m_wholeBrainSliceSettings;
//                break;
//            }
//        }
//    }
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
Overlay::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Overlay",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* selectedMapFile = NULL;
    //AString selectedMapUniqueID;
    int32_t selectedMapIndex;
    getSelectionData(mapFiles, 
                     selectedMapFile, 
                     //selectedMapUniqueID,
                     selectedMapIndex);
    
    if ((selectedMapFile != NULL) 
        && (selectedMapIndex >= 0)) {
        sceneClass->addPathName("selectedMapFileNameWithPath",
                                selectedMapFile->getFileName());
        sceneClass->addString("selectedMapFile",
                              selectedMapFile->getFileNameNoPath());
//        sceneClass->addString("selectedMapUniqueID",
//                              selectedMapUniqueID);
        sceneClass->addString("selectedMapName",
                              selectedMapFile->getMapName(selectedMapIndex));
        sceneClass->addInteger("selectedMapIndex",
                               selectedMapIndex);
    }
    else {
        sceneClass->addPathName("selectedMapFileNameWithPath",
                                "");
        sceneClass->addString("selectedMapFile",
                              "");
//        sceneClass->addString("selectedMapUniqueID",
//                              "");
        sceneClass->addString("selectedMapName",
                              "");
        sceneClass->addInteger("selectedMapIndex",
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
Overlay::restoreFromScene(const SceneAttributes* sceneAttributes,
                                       const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    /*
     * Making a call to getSelectionData() to get the availble
     * map files
     */
    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* unusedSelectedMapFile = NULL;
    AString unusedSelectedMapUniqueID;
    int32_t unusedSelectedMapIndex;
    getSelectionData(mapFiles, 
                     unusedSelectedMapFile, 
                     //unusedSelectedMapUniqueID,
                     unusedSelectedMapIndex);
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    const AString selectedMapFileNameWithPath = sceneClass->getPathNameValue("selectedMapFileNameWithPath");
    
    const AString selectedMapFileName = sceneClass->getStringValue("selectedMapFile",
                                                                   "");
    const AString selectedMapUniqueID = sceneClass->getStringValue("selectedMapUniqueID",
                                                                   "");
    const AString selectedMapName = sceneClass->getStringValue("selectedMapName",
                                                                   "");
    const int32_t selectedMapIndex = sceneClass->getIntegerValue("selectedMapIndex",
                                                                 -1);
    
    bool found = false;

    /*
     * Is used when the file is found but a map is not matched
     */
    CaretMappableDataFile* matchedMapFile = NULL;
    
    /*
     * First try to find file by filename INCLUDING path and map by unique ID
     */

    /*
     * Find map by unique ID, map index, and map file
     */
    CaretMappableDataFile* foundUniqueIdMapFile = NULL;
    int32_t foundUniqueIdMapIndex= -1;
    CaretMappableDataFile* foundMapNameFile = NULL;
    int32_t foundMapNameIndex  = -1;
    CaretMappableDataFile* foundMapIndexFile = NULL;
    int32_t foundMapIndex = -1;
    
    /*
     * Try to match files twice.  First time by name with path, then 
     * by name without path.
     */
    for (int iTries = 0; iTries < 2; iTries++) {
        for (std::vector<CaretMappableDataFile*>::iterator iter = mapFiles.begin();
             iter != mapFiles.end();
             iter++) {
            CaretMappableDataFile* mapFile = *iter;
            
            bool testIt = false;
            switch (iTries) {
                case 0: {
                    const AString fileName = mapFile->getFileName();
                    if (fileName == selectedMapFileNameWithPath) {
                        testIt = true;
                    }
                };
                    
                case 1: {
                    const AString fileName = mapFile->getFileNameNoPath();
                    if (fileName == selectedMapFileName) {
                        testIt = true;
                    }
                };
            }
            
            
            if (testIt) {
                CaretMappableDataFile* mapFile = *iter;
                matchedMapFile = mapFile;
                
                if (foundUniqueIdMapIndex < 0) {
                    const int uniqueIndex = mapFile->getMapIndexFromUniqueID(selectedMapUniqueID);
                    if (uniqueIndex >= 0) {
                        foundUniqueIdMapFile  = mapFile;
                        foundUniqueIdMapIndex = uniqueIndex;
                    }
                }
                
                if (foundMapNameIndex < 0) {
                    if ( ! selectedMapName.isEmpty()) {
                        const int mapNameIndex = mapFile->getMapIndexFromName(selectedMapName);
                        if (mapNameIndex >= 0) {
                            foundMapNameFile  = mapFile;
                            foundMapNameIndex = mapNameIndex;
                        }
                    }
                    
                }
                
                if (foundMapIndex < 0) {
                    if (selectedMapIndex >= 0) {
                        if (selectedMapIndex < mapFile->getNumberOfMaps()) {
                            foundMapIndexFile = mapFile;
                            foundMapIndex     = selectedMapIndex;
                        }
                    }
                }
            }
        }
    }
    
    if (! found) {
        if (foundMapIndex >= 0) {
            if (foundMapIndexFile != NULL) {
                setSelectionData(foundMapIndexFile,
                                 foundMapIndex);
                found = true;
            }
        }
    }
    
    if (! found) {
        if (foundUniqueIdMapIndex >= 0) {
            if (foundUniqueIdMapFile != NULL) {
                setSelectionData(foundUniqueIdMapFile,
                                 foundUniqueIdMapIndex);
                found = true;
            }
        }
    }
    
    if (! found) {
        if (foundMapNameIndex >= 0) {
            if (foundMapNameFile != NULL) {
                setSelectionData(foundMapNameFile,
                                 foundMapNameIndex);
                found = true;
            }
        }
    }
    
    
//    /*
//     * Second try to find file by filename WITHOUT path and map by unique ID
//     */
//    if (found == false) {
//        if (selectedMapUniqueID.isEmpty() == false) {
//            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFiles.begin();
//                 iter != mapFiles.end();
//                 iter++) {
//                CaretMappableDataFile* mapFile = *iter;
//                const AString fileName = mapFile->getFileNameNoPath();
//                if (fileName == selectedMapFileName) {
//                    CaretMappableDataFile* mapFile = *iter;
//                    matchedMapFile = mapFile;
//                    
//                    const int mapIndex = mapFile->getMapIndexFromUniqueID(selectedMapUniqueID);
//                    if (mapIndex >= 0) {
//                        setSelectionData(mapFile,
//                                         mapIndex);
//                        found = true;
//                        break;
//                    }
//                }
//            }
//        }
//    }
    
    if (found == false) {
        /*
         * If not found by unique ID, try to find map by name
         */
        if (selectedMapName.isEmpty() == false) {
            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFiles.begin();
                 iter != mapFiles.end();
                 iter++) {
                CaretMappableDataFile* mapFile = *iter;
                const AString fileName = mapFile->getFileNameNoPath();
                if (fileName == selectedMapFileName) {
                    CaretMappableDataFile* mapFile = *iter;
                    matchedMapFile = mapFile;
                    
                    const int32_t mapIndex = mapFile->getMapIndexFromName(selectedMapName);
                    if (mapIndex >= 0) {
                        setSelectionData(mapFile,
                                         mapIndex);
                        found = true;
                        break;
                    }
                }
            }
        }
    }

    /*
     * If file found but not matching map, use first map from the file.
     * This may occur when the map does not have a name.
     */
    if (found == false) {
        if (matchedMapFile != NULL) {
            if (matchedMapFile->getNumberOfMaps() > 0) {
                setSelectionData(matchedMapFile, 0);
            }
        }
    }
}



