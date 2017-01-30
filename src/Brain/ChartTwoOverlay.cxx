
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

#define __CHART_TWO_OVERLAY_DECLARE__
#include "ChartTwoOverlay.h"
#undef __CHART_TWO_OVERLAY_DECLARE__

#include "AnnotationColorBar.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartTwoOverlaySet.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoOverlay 
 * \brief Overlay for charts.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param parentChartTwoOverlaySet
 *     Parent of this chart overlay set.
 * @param chartDataType
 *     Type of charts allowed in this overlay
 * @param overlayIndex
 *     Index of this overlay.
 */
ChartTwoOverlay::ChartTwoOverlay(ChartTwoOverlaySet* parentChartTwoOverlaySet,
                           const ChartTwoDataTypeEnum::Enum chartDataType,
                           const int32_t overlayIndex)
: CaretObject(),
m_parentChartTwoOverlaySet(parentChartTwoOverlaySet),
m_chartDataType(chartDataType),
m_overlayIndex(overlayIndex)
{
    CaretAssert(m_parentChartTwoOverlaySet);
    
   // m_chartCompoundDataType
    m_name = "Overlay " + AString::number(overlayIndex + 1);
    m_enabled = (m_overlayIndex == 0);
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    
    m_colorBar = std::unique_ptr<AnnotationColorBar>(new AnnotationColorBar(AnnotationAttributesDefaultTypeEnum::NORMAL));
    m_colorBar->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
    
    m_matrixTriangularViewingMode = ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL;
    
    m_selectedMapFile  = NULL;
    m_selectedHistogramMapIndex = -1;
    m_allHistogramMapsSelectedFlag = false;

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup",
                                                                        &m_mapYokingGroup);
    m_sceneAssistant->add("m_colorBar", "AnnotationColorBar", m_colorBar.get());
    m_sceneAssistant->add<ChartTwoMatrixTriangularViewingModeEnum, ChartTwoMatrixTriangularViewingModeEnum::Enum>("m_matrixTriangularViewingMode",
                                                                        &m_matrixTriangularViewingMode);
    m_sceneAssistant->add("m_selectedHistogramMapIndex", &m_selectedHistogramMapIndex);
    m_sceneAssistant->add("m_allHistogramMapsSelectedFlag", &m_allHistogramMapsSelectedFlag);
    
//    EventManager::get()->addEventListener(this,
//                                          EventTypeEnum::EVENT_OVERLAY_VALIDATE);
}

/**
 * Destructor.
 */
ChartTwoOverlay::~ChartTwoOverlay()
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
ChartTwoOverlay::receiveEvent(Event* event)
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
ChartTwoOverlay::toString() const
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
ChartTwoOverlay::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    ChartTwoOverlay* me = const_cast<ChartTwoOverlay*>(this);
    if (me != NULL) {
        if (me->isEnabled()) {
            CaretMappableDataFile* mapFile = NULL;
            int32_t mapIndex = 0;
            me->getSelectionData(mapFile,
                                 mapIndex);
            if (mapFile != NULL) {
                descriptionOut.addLine("Overlay Index: " + AString::number(m_overlayIndex));
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
                
                const ChartableTwoFileDelegate* chartDelegate = mapFile->getChartingDelegate();
                ChartTwoCompoundDataType cdt;
                chartDelegate->getChartCompoundDataTypeForChartDataType(getChartDataType(),
                                                                        cdt);
                descriptionOut.addLine(cdt.toString());
            }
        }
    }
}

/**
 * @return The chart data type for this chart overlay.
 */
ChartTwoDataTypeEnum::Enum
ChartTwoOverlay::getChartDataType() const
{
    return m_chartDataType;
}

/**
 * Get the chart compound data type
 */
ChartTwoCompoundDataType
ChartTwoOverlay::getChartTwoCompoundDataType() const
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
ChartTwoOverlay::setChartTwoCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType)
{
    if (m_overlayIndex == 0) {
        CaretAssertMessage(0, "ChartTwoOverlay::setChartTwoCompoundDataType() should not be called "
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
ChartTwoOverlay::isEnabled() const
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
ChartTwoOverlay::setEnabled(const bool enabled)
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
ChartTwoOverlay::copyData(const ChartTwoOverlay* overlay)
{
    CaretAssert(overlay);
    
    /*
     * These members are not copied since they
     * identify the overlay:
     *    m_parentChartTwoOverlaySet
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
    m_matrixTriangularViewingMode = overlay->m_matrixTriangularViewingMode;
    
    m_selectedMapFile = overlay->m_selectedMapFile;
    m_selectedHistogramMapIndex = overlay->m_selectedHistogramMapIndex;
    m_allHistogramMapsSelectedFlag = overlay->m_allHistogramMapsSelectedFlag;
}

/**
 * Swap my data with data from the given overlay.
 * @param overlay
 *    Overlay from which data is swapped.
 */
void
ChartTwoOverlay::swapData(ChartTwoOverlay* overlay)
{
    std::unique_ptr<ChartTwoOverlay> swapOverlay = std::unique_ptr<ChartTwoOverlay>(new ChartTwoOverlay(m_parentChartTwoOverlaySet,
                                                                                               overlay->m_chartDataType,
                                                                                               overlay->m_overlayIndex));
    swapOverlay->copyData(overlay);
    
    overlay->copyData(this);
    copyData(swapOverlay.get());
}

/**
 * Is history supported ?
 */
bool
ChartTwoOverlay::isHistorySupported() const
{
    bool supportedFlag = false;
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            supportedFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    return supportedFlag;
}

/**
 * Is map yoking supported ?
 */
bool
ChartTwoOverlay::isMapYokingSupported() const
{
    bool supportedFlag = false;
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            supportedFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            supportedFlag = true;
            break;
    }
    return true;
}

/**
 * @return Selected map yoking group.
 */
MapYokingGroupEnum::Enum
ChartTwoOverlay::getMapYokingGroup() const
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
ChartTwoOverlay::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
{
    m_mapYokingGroup = mapYokingGroup;
}

/**
 * @return The color bar displayed in graphics window.
 */
AnnotationColorBar*
ChartTwoOverlay::getColorBar()
{
    return m_colorBar.get();
}

/**
 * @return The color bar displayed in graphics window (const method).
 */
const AnnotationColorBar*
ChartTwoOverlay::getColorBar() const
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
ChartTwoOverlay::getSelectionData(CaretMappableDataFile* &selectedMapFileOut,
                          int32_t& selectedMapIndexOut) const
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
 * @param selectedFileMapNamesOut
 *    Map names from selected file.
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void
ChartTwoOverlay::getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                               CaretMappableDataFile* &selectedMapFileOut,
                               std::vector<AString>& selectedFileMapNamesOut,
                               int32_t& selectedMapIndexOut) const
{
    getSelectionDataPrivate(mapFilesOut,
                            selectedMapFileOut,
                            &selectedFileMapNamesOut,
                            selectedMapIndexOut);
    
//    CaretAssertToDoFatal();
//    selectedFileMapNamesOut.clear();
//
//    getSelectionData(mapFilesOut,
//                     selectedMapFileOut,
//                     selectedMapIndexOut);
//    
//    if (selectedMapFileOut == NULL) {
//        return;
//    }
//    
//    switch (m_chartDataType) {
//        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
//            break;
//        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
//        {
//            const int32_t numMaps = selectedMapFileOut->getNumberOfMaps();
//            for (int32_t i = 0; i < numMaps; i++) {
//                selectedFileMapNamesOut.push_back(selectedMapFileOut->getMapName(i));
//            }
//        }
//            break;
//        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
//            break;
//        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
//        {
//            ChartableTwoFileDelegate* chartDelegate        = selectedMapFileOut->getChartingDelegate();
//            const ChartableTwoFileMatrixChart* matrixChart = chartDelegate->getMatrixCharting();
//            CaretAssert(matrixChart);
//            int32_t numRows = 0;
//            int32_t numCols = 0;
//            matrixChart->getMatrixDimensions(numRows, numCols);
//            for (int32_t i = 0; i < numRows; i++) {
//                selectedFileMapNamesOut.push_back("Row " + QString::number(i + 1));
//            }
//        }
//            break;
//    }
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param mapFilesOut
 *    Contains all map files that can be selected.
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void
ChartTwoOverlay::getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                               CaretMappableDataFile* &selectedMapFileOut,
                               int32_t& selectedMapIndexOut) const
{
    getSelectionDataPrivate(mapFilesOut,
                            selectedMapFileOut,
                            NULL,
                            selectedMapIndexOut);
}

///**
// * Return the selection information.  This method is typically
// * called to update the user-interface.
// *
// * @param mapFilesOut
// *    Contains all map files that can be selected.
// * @param selectedMapFileOut
// *    The selected map file.  May be NULL.
// * @param selectedMapIndexOut
// *    Index of selected map in the selected file.
// */
//void
//ChartTwoOverlay::getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
//                               CaretMappableDataFile* &selectedMapFileOut,
//                               int32_t& selectedMapIndexOut)
//{
//    mapFilesOut.clear();
//    selectedMapFileOut = NULL;
//    selectedMapIndexOut = -1;
//    
//    /**
//     * Get the data files.
//     */
//    std::vector<CaretMappableDataFile*> allDataFiles;
//    EventCaretMappableDataFilesGet eventGetMapDataFiles;
//    EventManager::get()->sendEvent(eventGetMapDataFiles.getPointer());
//    eventGetMapDataFiles.getAllFiles(allDataFiles);
//    
//    /*
//     * Use only those data files that meet criteria.
//     */
//    for (auto mapFile : allDataFiles) {
//        CaretAssert(mapFile);
//        ChartableTwoFileDelegate* chartingFile = mapFile->getChartingDelegate();
//        if (chartingFile->isChartingSupported()) {
//            bool useIt = false;
//            
//            std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
//            chartingFile->getSupportedChartCompoundDataTypes(chartCompoundDataTypes);
//            
//            for (auto& compoundType : chartCompoundDataTypes) {
//                if (m_chartDataType == compoundType.getChartDataType()) {
//                    if (m_overlayIndex == 0) {
//                        /*
//                         * The first overlay displays ALL files that match the
//                         * enumerated chart type
//                         */
//                        useIt = true;
//                    }
//                    else {
//                        if (m_chartCompoundDataType == compoundType) {
//                            /*
//                             * If not the first overlay, the enumerated type
//                             * and dimensions must also match
//                             */
//                            useIt = true;
//                        }
//                    }
//                }
//            }
//            
//            if (useIt) {
//                mapFilesOut.push_back(mapFile);
//            }
//        }
//    }
//    
//    /*
//     * Does selected data file still no longer exist?
//     */
//    if (std::find(mapFilesOut.begin(),
//                  mapFilesOut.end(),
//                  m_selectedMapFile) == mapFilesOut.end()) {
//        /*
//         * Invalidate seleted file and disable yoking since
//         * the selected file will change.
//         */
//        m_selectedMapFile = NULL;
//        m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
//    }
//    
//    /*
//     * If selected data file is valid, see if selected
//     * map is still valid.  If not, use first map.
//     */
//    if (m_selectedMapFile != NULL) {
//        if (m_selectedMapIndex >= m_selectedMapFile->getNumberOfMaps()) {
//            m_selectedMapIndex = m_selectedMapFile->getNumberOfMaps() - 1;
//        }
//        if (m_selectedMapIndex < 0) {
//            m_selectedMapIndex = 0;
//        }
//    }
//    else {
//        /*
//         * Use first map in first file that has one or more maps.
//         */
//        if (m_selectedMapFile == NULL) {
//            if (mapFilesOut.empty() == false) {
//                for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
//                     iter != mapFilesOut.end();
//                     iter++) {
//                    CaretMappableDataFile* mapTypeFile = *iter;
//                    if (mapTypeFile->getNumberOfMaps() > 0) {
//                        m_selectedMapFile = mapTypeFile;
//                        m_selectedMapIndex = 0;
//                        break;
//                    }
//                }
//            }
//        }
//    }
//    
//    selectedMapFileOut = m_selectedMapFile;
//    if (selectedMapFileOut != NULL) {
//        //        /*
//        //         * Update for overlay yoking
//        //         */
//        //        if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//        //            const int32_t yokeMapIndex = MapYokingGroupEnum::getSelectedMapIndex(m_mapYokingGroup);
//        //            if ((yokeMapIndex >= 0)
//        //                && (yokeMapIndex < selectedMapFileOut->getNumberOfMaps())) {
//        //                m_selectedMapIndex = yokeMapIndex;
//        //            }
//        //            else if (yokeMapIndex >= selectedMapFileOut->getNumberOfMaps()) {
//        //                m_selectedMapIndex = selectedMapFileOut->getNumberOfMaps() - 1;
//        //            }
//        //        }
//        //
//        selectedMapIndexOut = m_selectedMapIndex;  //m_selectedMapFile->getMapIndexFromUniqueID(selectedMapUniqueIDOut);
//    }
//    
//    /*
//     * Update the compound data type if this is the FIRST OVERLAY
//     */
//    if (m_overlayIndex == 0) {
//        if (m_selectedMapFile != NULL) {
//            ChartableTwoFileDelegate* chartFile = m_selectedMapFile->getChartingDelegate();
//            CaretAssert(chartFile);
//            chartFile->getChartCompoundDataTypeForChartDataType(m_chartDataType,
//                                                                        m_chartCompoundDataType);
//        }
//        CaretAssert(m_parentChartTwoOverlaySet);
//        m_parentChartTwoOverlaySet->firstOverlaySelectionChanged();
//    }
//    //If selected type changes, need to update other overlays in the overlay set with
//    //the selected data type}
//}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param mapFilesOut
 *    Contains all map files that can be selected.
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedFileMapNamesOut
 *    Optional output, if not NULL, with map names
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void
ChartTwoOverlay::getSelectionDataPrivate(std::vector<CaretMappableDataFile*>& mapFilesOut,
                                      CaretMappableDataFile* &selectedMapFileOut,
                                      std::vector<AString>* selectedFileMapNamesOut,
                                      int32_t& selectedMapIndexOut) const
{
    mapFilesOut.clear();
    selectedMapFileOut = NULL;
    selectedMapIndexOut = -1;
    if (selectedFileMapNamesOut != NULL) {
        selectedFileMapNamesOut->clear();
    }
    
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
        ChartableTwoFileDelegate* chartingFile = mapFile->getChartingDelegate();
        if (chartingFile->isChartingSupported()) {
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
    
//    /*
//     * If selected data file is valid, see if selected
//     * map is still valid.  If not, use first map.
//     */
//    if (m_selectedMapFile != NULL) {
//        if (m_selectedMapIndex >= m_selectedMapFile->getNumberOfMaps()) {
//            m_selectedMapIndex = m_selectedMapFile->getNumberOfMaps() - 1;
//        }
//        if (m_selectedMapIndex < 0) {
//            m_selectedMapIndex = 0;
//        }
//    }
//    else {
//        /*
//         * Use first map in first file that has one or more maps.
//         */
//        if (m_selectedMapFile == NULL) {
//            if (mapFilesOut.empty() == false) {
//                for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
//                     iter != mapFilesOut.end();
//                     iter++) {
//                    CaretMappableDataFile* mapTypeFile = *iter;
//                    if (mapTypeFile->getNumberOfMaps() > 0) {
//                        m_selectedMapFile = mapTypeFile;
//                        m_selectedMapIndex = 0;
//                        break;
//                    }
//                }
//            }
//        }
//    }

    /*
     * If no file selected, select the first valid file
     */
    if (m_selectedMapFile == NULL) {
        if ( ! mapFilesOut.empty()) {
            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
                 iter != mapFilesOut.end();
                 iter++) {
                CaretMappableDataFile* mapTypeFile = *iter;
                m_selectedMapFile = mapTypeFile;
                switch (m_chartDataType) {
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                        m_selectedHistogramMapIndex = 0;
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                        break;
                }
                break;
            }
        }
    }
    
    if (m_selectedMapFile != NULL) {
        int32_t numMaps = 0;
        
        switch (m_chartDataType) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            {
                numMaps = m_selectedMapFile->getNumberOfMaps();
                if (selectedFileMapNamesOut != NULL) {
                    for (int32_t i = 0; i < numMaps; i++) {
                        selectedFileMapNamesOut->push_back(m_selectedMapFile->getMapName(i));
                    }
                }
                if (m_selectedHistogramMapIndex >= numMaps) {
                    m_selectedHistogramMapIndex = numMaps - 1;
                }
                if (m_selectedHistogramMapIndex < 0) {
                    m_selectedHistogramMapIndex = 0;
                }
                selectedMapIndexOut = m_selectedHistogramMapIndex;
            }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            {
                ChartableTwoFileDelegate* chartDelegate        = m_selectedMapFile->getChartingDelegate();
                const ChartableTwoFileMatrixChart* matrixChart = chartDelegate->getMatrixCharting();
                CaretAssert(matrixChart);
                int32_t numRows = 0;
                int32_t numCols = 0;
                matrixChart->getMatrixDimensions(numRows, numCols);
                
                ChartTwoMatrixLoadingDimensionEnum::Enum rowColumnDimension;
                std::vector<int32_t> columnIndices;
                std::vector<int32_t> rowIndices;
                matrixChart->getSelectedRowColumnIndices(m_parentChartTwoOverlaySet->m_tabIndex,
                                                         rowColumnDimension,
                                                         rowIndices,
                                                         columnIndices);
                
                AString namePrefix;
                switch (rowColumnDimension) {
                    case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                        numMaps = numCols;
                        namePrefix = "Column ";
                        if ( ! columnIndices.empty()) {
                            selectedMapIndexOut = columnIndices[0];
                        }
                        break;
                    case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                        numMaps = numRows;
                        namePrefix = "Row ";
                        if ( ! rowIndices.empty()) {
                            selectedMapIndexOut = rowIndices[0];
                        }
                        break;
                }
                
                if (selectedFileMapNamesOut != NULL) {
                    for (int32_t i = 0; i < numMaps; i++) {
                        selectedFileMapNamesOut->push_back(namePrefix + QString::number(i));
                    }
                }
                
                
            }
                break;
        }
        
//        /*
//         * If selected data file is valid, see if selected
//         * map is still valid.  If not, use first map.
//         */
//        if (m_selectedMapFile != NULL) {
//            if (m_selectedHistogramMapIndex >= numMaps) {
//                m_selectedHistogramMapIndex = numMaps - 1;
//            }
//            if (m_selectedHistogramMapIndex < 0) {
//                m_selectedHistogramMapIndex = 0;
//            }
//        }
    }

    selectedMapFileOut = m_selectedMapFile;
//    if (selectedMapFileOut != NULL) {
//        //        /*
//        //         * Update for overlay yoking
//        //         */
//        //        if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
//        //            const int32_t yokeMapIndex = MapYokingGroupEnum::getSelectedMapIndex(m_mapYokingGroup);
//        //            if ((yokeMapIndex >= 0)
//        //                && (yokeMapIndex < numMaps)) {
//        //                m_selectedMapIndex = yokeMapIndex;
//        //            }
//        //            else if (yokeMapIndex >= numMaps) {
//        //                m_selectedMapIndex = numMaps - 1;
//        //            }
//        //        }
//        //
//        selectedMapIndexOut = m_selectedHistogramMapIndex;  //m_selectedMapFile->getMapIndexFromUniqueID(selectedMapUniqueIDOut);
//    }
    
    /*
     * Update the compound data type if this is the FIRST OVERLAY
     */
    if (m_overlayIndex == 0) {
        if (m_selectedMapFile != NULL) {
            const ChartableTwoFileDelegate* chartFile = m_selectedMapFile->getChartingDelegate();
            CaretAssert(chartFile);
            chartFile->getChartCompoundDataTypeForChartDataType(m_chartDataType,
                                                                m_chartCompoundDataType);
        }
        CaretAssert(m_parentChartTwoOverlaySet);
        m_parentChartTwoOverlaySet->firstOverlaySelectionChanged();
    }
    //If selected type changes, need to update other overlays in the overlay set with
    //the selected data type}
}

/**
 * Set the selected map file and map.
 * @param selectedMapFile
 *    File that is selected.
 * @param selectedMapIndex
 *    Index of map for selection.  If invalid, try the current map
 *    index if it is valid.  Otherwise, use the first map index.
 */
void
ChartTwoOverlay::setSelectionData(CaretMappableDataFile* selectedMapFile,
                               const int32_t selectedMapIndex)
{
    m_selectedMapFile = selectedMapFile;
    if (m_selectedMapFile != NULL) {
        switch (m_chartDataType) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                if (selectedMapIndex >= 0) {
                    m_selectedHistogramMapIndex = selectedMapIndex;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            {
                ChartableTwoFileDelegate* chartDelegate = m_selectedMapFile->getChartingDelegate();
                CaretAssert(chartDelegate);
                ChartableTwoFileMatrixChart* matrixChart   = chartDelegate->getMatrixCharting();
                CaretAssert(matrixChart);
                matrixChart->setSelectedRowColumnIndex(m_parentChartTwoOverlaySet->m_tabIndex,
                                                       selectedMapIndex);
            }
                break;
        }
        
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
    
    
    /*
     * By calling getSelectionData(), it will validate the
     * selected file and map index and update if needed 
     * (such as a valid map index).
     */
    CaretMappableDataFile* filePointer = NULL;
    int32_t mapIndex = -1;
    getSelectionData(filePointer, mapIndex);
}

/*
 switch (m_chartDataType) {
 case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
 break;
 case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
 break;
 case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
 break;
 case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
 break;
 }
 */
/**
 * @return Is the all maps  supported?
 */
bool
ChartTwoOverlay::isAllMapsSupported() const
{
    bool supportedFlag = false;
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
        {
            CaretMappableDataFile* cmdf = NULL;
            int32_t mapIndex = 0;
            
            getSelectionData(cmdf,
                             mapIndex);
            
            if (cmdf != NULL) {
                if (cmdf->getNumberOfMaps() > 1) {
                    supportedFlag = true;
                }
            }
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    
    return supportedFlag;
}

/**
 * @return All maps selected.
 */
bool
ChartTwoOverlay::isAllMapsSelected() const
{
    return m_allHistogramMapsSelectedFlag;
}

/**
 * Set all maps selected.
 *
 * @param status
 *     New status.
 */
void
ChartTwoOverlay::setAllMapsSelected(const bool status)
{
    m_allHistogramMapsSelectedFlag = status;
}

/**
 * @return The matrix triangular viewing mode.
 */
ChartTwoMatrixTriangularViewingModeEnum::Enum
ChartTwoOverlay::getMatrixTriangularViewingMode() const
{
    return m_matrixTriangularViewingMode;
}

/**
 * Set the matrix triangular viewing mode.
 * 
 * @param mode
 *     New triangular viewing mode.
 */
void
ChartTwoOverlay::setMatrixTriangularViewingMode(const ChartTwoMatrixTriangularViewingModeEnum::Enum mode)
{
    m_matrixTriangularViewingMode = mode;
}

/**
 * @return Is the matrix triangular view mode supported?
 */
bool
ChartTwoOverlay::isMatrixTriangularViewingModeSupported() const
{
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            return true;
            break;
    }
    
    return false;
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
ChartTwoOverlay::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoOverlay",
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
ChartTwoOverlay::restoreFromScene(const SceneAttributes* sceneAttributes,
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

