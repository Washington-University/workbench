
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __CHART_OVERLAY_DECLARE__
#include "ChartOverlay.h"
#undef __CHART_OVERLAY_DECLARE__

#include "AnnotationColorBar.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "ChartableTwoInterface.h"
#include "ChartOverlaySet.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartOverlay 
 * \brief Overlay for charts.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *     Type of charts allowed in this overlay
 * @param overlayIndex
 *     Index of this overlay.
 */
ChartOverlay::ChartOverlay(ChartOverlaySet* parentChartOverlaySet,
                           const ChartTwoDataTypeEnum::Enum chartDataType,
                           const int32_t overlayIndex)
: CaretObject(),
m_parentChartOverlaySet(parentChartOverlaySet),
m_chartDataType(chartDataType),
m_overlayIndex(overlayIndex)
{
    if (overlayIndex > 0) {
        CaretAssert(m_parentChartOverlaySet == NULL);
    }
    else {
        CaretAssert(m_parentChartOverlaySet);
    }
    
   // m_chartCompoundDataType
    m_name = "Overlay " + AString::number(overlayIndex + 1);
    m_enabled = (m_overlayIndex == 0);
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    
    m_colorBar = std::unique_ptr<AnnotationColorBar>(new AnnotationColorBar(AnnotationAttributesDefaultTypeEnum::NORMAL));
    m_colorBar->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
    
    m_selectedMapFile  = NULL;
    m_selectedMapIndex = -1;
    m_allMapsSelectedFlag = false;

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup",
                                                                        &m_mapYokingGroup);
    m_sceneAssistant->add("m_colorBar", "AnnotationColorBar", m_colorBar.get());
    m_sceneAssistant->add("m_allMapsSelectedFlag", &m_allMapsSelectedFlag);
    
//    EventManager::get()->addEventListener(this,
//                                          EventTypeEnum::EVENT_OVERLAY_VALIDATE);
}

/**
 * Destructor.
 */
ChartOverlay::~ChartOverlay()
{
    EventManager::get()->removeAllEventsFromListener(this);
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartOverlay::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
ChartOverlay::toString() const
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
ChartOverlay::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    ChartOverlay* me = const_cast<ChartOverlay*>(this);
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
            }
        }
    }
}

/**
 * @return The chart data type for this chart overlay.
 */
ChartTwoDataTypeEnum::Enum
ChartOverlay::getChartDataType() const
{
    return m_chartDataType;
}

/**
 * Get the chart compound data type
 */
ChartTwoCompoundDataType
ChartOverlay::getChartTwoCompoundDataType() const
{
    return m_chartCompoundDataType;
}

/**
 * Set the compound chart type for charts displayed in this overlay.
 * MUST match simplae data type for this chart unless invalid.
 * Note that overlay index zero, allows any chart type.
 *
 * @param chartCompoundDataType
 *     Type of charts for display in this overlay.
 */
void
ChartOverlay::setChartTwoCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType)
{
    if (m_overlayIndex == 0) {
        CaretAssertMessage(0, "ChartOverlay::setChartTwoCompoundDataType() should not be called "
                           " for first overlay");
        return;
    }
//    do for overlay zero ??
        
    if (chartCompoundDataType.getChartDataType() != ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID) {
        CaretAssert(m_chartDataType == chartCompoundDataType.getChartDataType());
    }
    m_chartCompoundDataType = chartCompoundDataType;
}

/**
 * @return Enabled status for this surface overlay.
 */
bool
ChartOverlay::isEnabled() const
{
    /*
     * First overlay is ALWAYS enabled
     */
    if (m_overlayIndex == 0) {
        return true;
    }
    
    return m_enabled;
}

/**
 * Set the enabled status for this surface overlay.
 * @param enabled
 *    New status.
 */
void
ChartOverlay::setEnabled(const bool enabled)
{
    if (m_overlayIndex == 0) {
        if ( ! enabled) {
            CaretAssertMessage(0, "Request to disable first overlay should NEVER happen");
            CaretLogSevere("Request to disable first overlay should NEVER happen");
            return;
        }
    }
    m_enabled = enabled;
}

/**
 * Copy the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void
ChartOverlay::copyData(const ChartOverlay* overlay)
{
    CaretAssert(overlay);
    
    /*
     * These members are not copied since they
     * identify the overlay:
     *    m_parentChartOverlaySet
     *    m_name
     *    m_overlayIndex
     *
     */
    
    /*
     * NEVER disable the first overlay
     */
    if (m_overlayIndex > 0) {
        m_enabled = overlay->m_enabled;
    }
    
    m_mapYokingGroup = overlay->m_mapYokingGroup;
    
    *m_colorBar = *overlay->m_colorBar;
    
    m_selectedMapFile = overlay->m_selectedMapFile;
    m_selectedMapIndex = overlay->m_selectedMapIndex;
}

/**
 * Swap my data with data from the given overlay.
 * @param overlay
 *    Overlay from which data is swapped.
 */
void
ChartOverlay::swapData(ChartOverlay* overlay)
{
    std::unique_ptr<ChartOverlay> swapOverlay = std::unique_ptr<ChartOverlay>(new ChartOverlay(m_parentChartOverlaySet,
                                                                                               overlay->m_chartDataType,
                                                                                               overlay->m_overlayIndex));
    swapOverlay->copyData(overlay);
    
    overlay->copyData(this);
    copyData(swapOverlay.get());
}

/**
 * @return Selected map yoking group.
 */
MapYokingGroupEnum::Enum
ChartOverlay::getMapYokingGroup() const
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
ChartOverlay::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
{
    m_mapYokingGroup = mapYokingGroup;
}

/**
 * @return The color bar displayed in graphics window.
 */
AnnotationColorBar*
ChartOverlay::getColorBar()
{
    return m_colorBar.get();
}

/**
 * @return The color bar displayed in graphics window (const method).
 */
const AnnotationColorBar*
ChartOverlay::getColorBar() const
{
    return m_colorBar.get();
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
ChartOverlay::getSelectionData(CaretMappableDataFile* &selectedMapFileOut,
                          int32_t& selectedMapIndexOut)
{
    std::vector<CaretMappableDataFile*> mapFiles;
    
    getSelectionData(mapFiles,
                     selectedMapFileOut,
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
ChartOverlay::getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                          CaretMappableDataFile* &selectedMapFileOut,
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
    
    /*
     * Use only those data files that meet criteria.
     */
    for (auto mapFile : allDataFiles) {
        CaretAssert(mapFile);
        ChartableTwoInterface* chartingFile = dynamic_cast<ChartableTwoInterface*>(mapFile);
        if (chartingFile != NULL) {
            bool useIt = false;
            
            std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
            chartingFile->getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
            
            for (auto& compoundType : chartCompoundDataTypes) {
                if (m_chartDataType == compoundType.getChartDataType()) {
                    if (m_overlayIndex == 0) {
                        /*
                         * The first overlay displays ALL files that match the
                         * enumerated chart type
                         */
                        useIt = true;
                    }
                    else {
                        if (m_chartCompoundDataType == compoundType) {
                            /*
                             * If not the first overlay, the enumerated type
                             * and dimensions must also match
                             */
                            useIt = true;
                        }
                    }
                }
            }
            
            if (useIt) {
                mapFilesOut.push_back(mapFile);
            }
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
    
    /*
     * Update the compound data type if this is the FIRST OVERLAY
     */
    if (m_overlayIndex == 0) {
        if (m_selectedMapFile != NULL) {
            ChartableTwoInterface* chartFile = dynamic_cast<ChartableTwoInterface*>(m_selectedMapFile);
            CaretAssert(chartFile);
            chartFile->getChartCompoundDataTypeForChartDataType(m_chartDataType,
                                                                        m_chartCompoundDataType);
        }
        CaretAssert(m_parentChartOverlaySet);
        m_parentChartOverlaySet->firstOverlaySelectionChanged();
    }
    //If selected type changes, need to update other overlays in the overlay set with
    //the selected data type}
}

/**
 * Set the selected map file and map.
 * @param selectedMapFile
 *    File that is selected.
 * @param selectedMapName
 *    Map name that is selected.
 */
void
ChartOverlay::setSelectionData(CaretMappableDataFile* selectedMapFile,
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
    
    /*
     * By calling getSelectionData(), it will validate the
     * selected file
     */
    CaretMappableDataFile* filePointer = NULL;
    int32_t mapIndex = -1;
    getSelectionData(filePointer, mapIndex);
}

/**
 * @return All maps selected.
 */
bool
ChartOverlay::isAllMapsSelected() const
{
    return m_allMapsSelectedFlag;
}

/**
 * Set all maps selected.
 *
 * @param status
 *     New status.
 */
void
ChartOverlay::setAllMapsSelected(const bool status)
{
    m_allMapsSelectedFlag = status;
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartOverlay::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartOverlay",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ChartOverlay::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

