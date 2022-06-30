
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

#include "AnnotationColorBar.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventOverlayValidate.h"
#include "LabelFile.h"
#include "MetricDynamicConnectivityFile.h"
#include "MetricFile.h"
#include "PlainTextStringBuilder.h"
#include "RgbaFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "VolumeDynamicConnectivityFile.h"
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
    m_selectedMapFile  = NULL;
    m_selectedMapIndex = -1;
    m_name = "Overlay ";
    m_enabled = true;
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    
    m_wholeBrainVoxelDrawingMode = WholeBrainVoxelDrawingMode::DRAW_VOXELS_ON_TWO_D_SLICES;
    
    m_colorBar = new AnnotationColorBar(AnnotationAttributesDefaultTypeEnum::NORMAL);
    m_colorBar->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_opacity", &m_opacity);
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>("m_wholeBrainVoxelDrawingMode",
                                                                                        &m_wholeBrainVoxelDrawingMode);
    m_sceneAssistant->add<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup",
                                                                        &m_mapYokingGroup);
    m_sceneAssistant->add("m_colorBar", "AnnotationColorBar", m_colorBar);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_OVERLAY_VALIDATE);
}

/**
 * Destructor.
 */
Overlay::~Overlay()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_colorBar;
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
    
    m_selectedMapFile = overlay->m_selectedMapFile;
    m_selectedMapIndex = overlay->m_selectedMapIndex;
    m_mapYokingGroup = overlay->m_mapYokingGroup;
    
    m_wholeBrainVoxelDrawingMode = overlay->m_wholeBrainVoxelDrawingMode;
    
    *m_colorBar = *overlay->m_colorBar;
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
        
        if (mapFile->isSurfaceMappable()) {
            if (showSurfaceMapFiles) {
                
                for (std::vector<StructureEnum::Enum>::const_iterator iter = m_includeSurfaceStructures.begin();
                     iter != m_includeSurfaceStructures.end();
                     iter++) {
                    if (mapFile->isMappableToSurfaceStructure(*iter)) {
                        useIt = true;
                        break;
                    }
                }
            }
        }
        if (mapFile->isVolumeMappable()) {
            if (showVolumeMapFiles) {
                useIt = true;
            }
        }
        
        if (useIt) {
            switch (mapFile->getDataFileType()) {
                case DataFileTypeEnum::ANNOTATION:
                    break;
                case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                    break;
                case DataFileTypeEnum::BORDER:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                {
                    CiftiConnectivityMatrixDenseDynamicFile* dynConnFile = dynamic_cast<CiftiConnectivityMatrixDenseDynamicFile*>(mapFile);
                    CaretAssert(dynConnFile);
                    useIt = dynConnFile->isEnabledAsLayer();
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                    break;
                case DataFileTypeEnum::CZI_IMAGE_FILE:
                    break;
                case DataFileTypeEnum::CZI_META_FILE:
                    break;
                case DataFileTypeEnum::FOCI:
                    break;
                case DataFileTypeEnum::IMAGE:
                    break;
                case DataFileTypeEnum::LABEL:
                    break;
                case DataFileTypeEnum::METRIC:
                    break;
                case DataFileTypeEnum::METRIC_DYNAMIC:
                {
                    MetricDynamicConnectivityFile* metricDynFile = dynamic_cast<MetricDynamicConnectivityFile*>(mapFile);
                    CaretAssert(metricDynFile);
                    useIt = metricDynFile->isEnabledAsLayer();
                }
                    break;
                case DataFileTypeEnum::PALETTE:
                    break;
                case DataFileTypeEnum::RGBA:
                    break;
                case DataFileTypeEnum::SCENE:
                    break;
                case DataFileTypeEnum::SPECIFICATION:
                    break;
                case DataFileTypeEnum::SURFACE:
                    break;
                case DataFileTypeEnum::UNKNOWN:
                    break;
                case DataFileTypeEnum::VOLUME:
                    break;
                case DataFileTypeEnum::VOLUME_DYNAMIC:
                    {
                        VolumeDynamicConnectivityFile* volDynFile = dynamic_cast<VolumeDynamicConnectivityFile*>(mapFile);
                        CaretAssert(volDynFile);
                        useIt = volDynFile->isEnabledAsLayer();
                    }
                    break;
            }
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
        /*
         * Invalidate seleted file and disable yoking since 
         * the selected file will change.
         */
        m_selectedMapFile = NULL;
        m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
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
    }
    else {
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
                    }
                }
            }
        }
    }
    
    selectedMapFileOut = m_selectedMapFile;
    if (selectedMapFileOut != NULL) {
//        /*
//         * Update for overlay yoking
//         */
//        if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//            const int32_t yokeMapIndex = MapYokingGroupEnum::getSelectedMapIndex(m_mapYokingGroup);
//            if ((yokeMapIndex >= 0)
//                && (yokeMapIndex < selectedMapFileOut->getNumberOfMaps())) {
//                m_selectedMapIndex = yokeMapIndex;
//            }
//            else if (yokeMapIndex >= selectedMapFileOut->getNumberOfMaps()) {
//                m_selectedMapIndex = selectedMapFileOut->getNumberOfMaps() - 1;
//            }
//        }
//        
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
    
    if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        if (m_selectedMapFile == NULL) {
            m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
        }
//        if (selectedMapFile != NULL) {
//            MapYokingGroupEnum::setSelectedMapIndex(m_mapYokingGroup,
//                                                        selectedMapIndex);
//        }
//        else {
//            m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
//        }
    }
}

/**
 * @return Selected map yoking group.
 */
MapYokingGroupEnum::Enum
Overlay::getMapYokingGroup() const
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
Overlay::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
{
    m_mapYokingGroup = mapYokingGroup;
}

/**
 * @return The color bar displayed in graphics window.
 */
AnnotationColorBar*
Overlay::getColorBar()
{
    return m_colorBar;
}

/**
 * @return The color bar displayed in graphics window (const method).
 */
const AnnotationColorBar*
Overlay::getColorBar() const
{
    return m_colorBar;
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

    
    /*
     * "m_paletteDisplayedFlag" controlled display of palette colorbar 
     * prior to the addition of AnnotationColorBar
     */
    const AString paletteDisplayedFlagString = sceneClass->getStringValue("m_paletteDisplayedFlag");
    if ( ! paletteDisplayedFlagString.isEmpty()) {
        m_colorBar->reset();
        m_colorBar->setDisplayed(sceneClass->getBooleanValue("m_paletteDisplayedFlag"));
    }
    
    /*
     * OverlayYokingGroup was replaced by MapYokingGroup.
     * If an overlay yoking group is found, convert it to 
     * a map yoking group.
     */
    const AString overlayYokingGroupName = sceneClass->getEnumeratedTypeValueAsString("m_yokingGroup",
                                                                      "");
    if ( ! overlayYokingGroupName.isEmpty()) {
        bool valid = false;
        const MapYokingGroupEnum::Enum mapGroup = MapYokingGroupEnum::fromOverlayYokingGroupEnumName(overlayYokingGroupName,
                                                                              &valid);
        if (valid) {
            m_mapYokingGroup = mapGroup;
        }
    }
    
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
                }
                    break;
                    
                case 1: {
                    const AString fileName = mapFile->getFileNameNoPath();
                    if (fileName == selectedMapFileName) {
                        testIt = true;
                    }
                }
                    break;
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



