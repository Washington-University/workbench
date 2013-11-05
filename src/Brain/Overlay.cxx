
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __OVERLAY_DECLARE__
#include "Overlay.h"
#undef __OVERLAY_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventOverlayValidate.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
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
 * Constructor for surface controllers.
 * @param modelDisplayControllerSurface
 *    Controller that is for surfaces.
 */
Overlay::Overlay(BrainStructure* brainStructure)
: CaretObject()
{
    CaretAssert(brainStructure);
    
    m_volumeController  = NULL;
    m_wholeBrainController  = NULL;
    m_surfaceMontageController = NULL;
    
    initializeOverlay(NULL,
                      brainStructure);
}

/**
 * Constructor for volume controllers.
 * @param modelDisplayControllerVolume
 *    Controller that is for volumes.
 */
Overlay::Overlay(ModelVolume* modelDisplayControllerVolume)
: CaretObject()
{
    CaretAssert(modelDisplayControllerVolume);
    
    m_volumeController  = modelDisplayControllerVolume;
    m_wholeBrainController  = NULL;
    m_surfaceMontageController = NULL;
    
    initializeOverlay(m_volumeController,
                            NULL);
}

/**
 * Constructor for whole brain controllers.
 * @param modelDisplayControllerWholeBrain
 *    Controller that is for whole brains.
 */
Overlay::Overlay(ModelWholeBrain* modelDisplayControllerWholeBrain)
: CaretObject()
{
    CaretAssert(modelDisplayControllerWholeBrain);
    
    m_volumeController  = NULL;
    m_wholeBrainController  = modelDisplayControllerWholeBrain;
    m_surfaceMontageController = NULL;
    
    initializeOverlay(m_wholeBrainController,
                            NULL);
}

/**
 * Constructor for surface montage controllers.
 * @param modelDisplayControllerSurfaceMontage
 *    Controller that is for surface montage.
 */
Overlay::Overlay(ModelSurfaceMontage* modelDisplayControllerSurfaceMontage)
: CaretObject()
{
    CaretAssert(modelDisplayControllerSurfaceMontage);
    
    m_volumeController  = NULL;
    m_wholeBrainController  = NULL;
    m_surfaceMontageController = modelDisplayControllerSurfaceMontage;
    
    initializeOverlay(m_surfaceMontageController,
                            NULL);
}

/**
 * Initialize the overlay's members.
 * @param modelDisplayController
 *    Controller that uses this overlay.
 */
void
Overlay::initializeOverlay(Model* modelDisplayController,
                           BrainStructure* brainStructure)
{
    m_brainStructure = brainStructure;
    
    if (modelDisplayController == NULL) {
        CaretAssert(m_brainStructure != NULL);
    }
    else if (m_brainStructure == NULL) {
        CaretAssert(modelDisplayController != NULL);
    }
    else {
        CaretAssertMessage(0, "Both mode and brain structure are NULL");
    }
    
    m_brainStructure = brainStructure;
    
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
    Overlay* me = const_cast<Overlay*>(this);
    if (me != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = 0;
        me->getSelectionData(mapFile,
                             mapIndex);
        AString msg = ("MapFile=");
        if (mapFile != NULL) {
            msg += (mapFile->getFileNameNoPath()
                    + ", mapIndex="
                    + AString::number(mapIndex));
        }
        else {
            msg += "NONE";
        }
        
        return msg;
    }
    return "INVALID";
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
    m_brainStructure = overlay->m_brainStructure;
    m_volumeController  = overlay->m_volumeController;
    m_wholeBrainController = overlay->m_wholeBrainController;
    m_surfaceMontageController = overlay->m_surfaceMontageController;
    
    m_opacity = overlay->m_opacity;
    m_enabled = overlay->m_enabled;
    
//    m_mapFiles = overlay->m_mapFiles;
    m_selectedMapFile = overlay->m_selectedMapFile;
    m_selectedMapUniqueID = overlay->m_selectedMapUniqueID;
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
    Overlay* swapOverlay = NULL;
    
    if (m_brainStructure != NULL) {
        swapOverlay = new Overlay(m_brainStructure);
    }
    else if (m_volumeController != NULL) {
        swapOverlay = new Overlay(m_volumeController);
    }
    else if (m_wholeBrainController != NULL) {
        swapOverlay = new Overlay(m_wholeBrainController);
    }
    else if (m_surfaceMontageController != NULL) {
        swapOverlay = new Overlay(m_surfaceMontageController);
    }
    else {
        CaretAssertMessage(0, "Unknown overlay type");
    }
    
    swapOverlay->copyData(overlay);
    
    overlay->copyData(this);
    copyData(swapOverlay);
    
    delete swapOverlay;
}

/**
 * Get the current selection.  If the current selection is
 * invalid, new map data will be selected.
 *
 * @param mapDataFileTypeOut
 *    Type of map file out.
 * @param selectedMapUniqueIDOut
 *    UniqueID of map that is selected.
 */
void 
Overlay::getSelectionData(DataFileTypeEnum::Enum& mapDataFileTypeOut,
                          AString& selectedMapUniqueIDOut)
{
    std::vector<CaretMappableDataFile*> allFiles;
    CaretMappableDataFile* selectedFile;
    int32_t selectedIndex;
    getSelectionData(allFiles,
                           selectedFile,
                           selectedMapUniqueIDOut,
                           selectedIndex);
    
    mapDataFileTypeOut = DataFileTypeEnum::UNKNOWN;
    if (selectedFile != NULL) {
        mapDataFileTypeOut = selectedFile->getDataFileType();
    }
    else {
        selectedMapUniqueIDOut = "";
    }    
}

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
    AString mapUniqueID;
    
    getSelectionData(mapFiles, 
                           selectedMapFileOut, 
                           mapUniqueID, 
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
                          AString& selectedMapUniqueIDOut,
                          int32_t& selectedMapIndexOut)
{
    mapFilesOut.clear();
    selectedMapFileOut = NULL;
    selectedMapUniqueIDOut = "";
    selectedMapIndexOut = -1;
    
    /**
     * Get the data files.
     */
    std::vector<CaretMappableDataFile*> allDataFiles;
    EventCaretMappableDataFilesGet eventGetMapDataFiles;
    EventManager::get()->sendEvent(eventGetMapDataFiles.getPointer());
    eventGetMapDataFiles.getAllFiles(allDataFiles);
    
    bool showSurfaceMapFiles = false;
    bool showVolumeMapFiles  = false;

    /*
     * If a surface is displayed, restrict selections to files that
     * match the structure of the displayed surface.
     */
    StructureEnum::Enum selectedSurfaceStructure = StructureEnum::ALL;
    if (m_brainStructure != NULL) {
        selectedSurfaceStructure = m_brainStructure->getStructure();
        showSurfaceMapFiles = true;
    }
    
    /*
     * If a volume is selected, restrict selections to volume files.
     */
    if (m_volumeController != NULL) {
        showVolumeMapFiles = true;
    }
    
    /*
     * If whole brain is selected, show surface and volume files.
     */
    if (m_wholeBrainController != NULL) {
        showSurfaceMapFiles = true;
        showVolumeMapFiles = true;
    }
    
    /*
     * If surface montage is selected, show surface files
     */
    if (m_surfaceMontageController != NULL) {
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
                if (selectedSurfaceStructure == StructureEnum::ALL) {
                    useIt = true;
                }
                else if (mapFile->getStructure() == StructureEnum::ALL) {
                    useIt = true;
                }
                else if (selectedSurfaceStructure == mapFile->getStructure()) {
                    useIt = true;
                }
            }
        }
        if (mapFile->isVolumeMappable()) {
            mappable = true;
            if (showVolumeMapFiles) {
                useIt = true;
            }
        }
        
        if (mappable == false) {
            CaretAssertMessage(0, "Map file is neither surface nor volume mappable: " + mapFile->getFileName());
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
        const int32_t mapIndex = m_selectedMapFile->getMapIndexFromUniqueID(m_selectedMapUniqueID);
        if (mapIndex < 0) {
            m_selectedMapUniqueID = m_selectedMapFile->getMapUniqueID(0);
        }
    }
    else {
        /*
         * Look for a file that contains the selected map unique ID.
         */
        if (m_selectedMapUniqueID.isEmpty() == false) {
            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
                 iter != mapFilesOut.end();
                 iter++) {
                CaretMappableDataFile* mapTypeFile = *iter;
                const int32_t mapIndex = mapTypeFile->getMapIndexFromUniqueID(m_selectedMapUniqueID);
                if (mapIndex >= 0) {
                    m_selectedMapFile = mapTypeFile;
                    break;
                }
            }
        }
        
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
                        m_selectedMapUniqueID = mapTypeFile->getMapUniqueID(0);
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
                m_selectedMapUniqueID = selectedMapFileOut->getMapUniqueID(yokeMapIndex);
            }
            else if (yokeMapIndex >= selectedMapFileOut->getNumberOfMaps()) {
                selectedMapUniqueIDOut = selectedMapFileOut->getNumberOfMaps() - 1;
            }
        }
        
        selectedMapUniqueIDOut = m_selectedMapUniqueID;
        selectedMapIndexOut = m_selectedMapFile->getMapIndexFromUniqueID(selectedMapUniqueIDOut);
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
    m_selectedMapUniqueID = selectedMapFile->getMapUniqueID(selectedMapIndex);
    
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
    AString selectedMapUniqueID;
    int32_t selectedMapIndex;
    getSelectionData(mapFiles, 
                     selectedMapFile, 
                     selectedMapUniqueID, 
                     selectedMapIndex);
    
    if ((selectedMapFile != NULL) 
        && (selectedMapIndex >= 0)) {
        sceneClass->addPathName("selectedMapFileNameWithPath",
                                selectedMapFile->getFileName());
        sceneClass->addString("selectedMapFile",
                              selectedMapFile->getFileNameNoPath());
        sceneClass->addString("selectedMapUniqueID",
                              selectedMapUniqueID);
        sceneClass->addString("selectedMapName",
                              selectedMapFile->getMapName(selectedMapIndex));
    }
    else {
        sceneClass->addString("selectedMapFile",
                              "");
        sceneClass->addString("selectedMapUniqueID",
                              "");
        sceneClass->addString("selectedMapName",
                              "");
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
                     unusedSelectedMapUniqueID, 
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
    
    bool found = false;

    /*
     * First try to find file by filename INCLUDING path and map by unique ID
     */
    if (selectedMapFileNameWithPath.isEmpty() == false) {
        if (selectedMapUniqueID.isEmpty() == false) {
            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFiles.begin();
                 iter != mapFiles.end();
                 iter++) {
                CaretMappableDataFile* mapFile = *iter;
                const AString fileName = mapFile->getFileName();
                if (fileName == selectedMapFileNameWithPath) {
                    CaretMappableDataFile* mapFile = *iter;
                    const int mapIndex = mapFile->getMapIndexFromUniqueID(selectedMapUniqueID);
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
     * Second try to find file by filename WITHOUT path and map by unique ID
     */
    if (found == false) {
        if (selectedMapUniqueID.isEmpty() == false) {
            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFiles.begin();
                 iter != mapFiles.end();
                 iter++) {
                CaretMappableDataFile* mapFile = *iter;
                const AString fileName = mapFile->getFileNameNoPath();
                if (fileName == selectedMapFileName) {
                    CaretMappableDataFile* mapFile = *iter;
                    const int mapIndex = mapFile->getMapIndexFromUniqueID(selectedMapUniqueID);
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
}



