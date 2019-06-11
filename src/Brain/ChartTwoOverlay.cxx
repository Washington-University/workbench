
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
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "ChartTwoLineSeriesHistory.h"
#include "ChartTwoOverlaySet.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventChartOverlayValidate.h"
#include "EventManager.h"
#include "Histogram.h"
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
                                 const int32_t tabIndex,
                                 const int32_t overlayIndex)
: CaretObject(),
m_parentChartTwoOverlaySet(parentChartTwoOverlaySet),
m_chartDataType(chartDataType),
m_tabIndex(tabIndex),
m_overlayIndex(overlayIndex)
{
    CaretAssert(m_parentChartTwoOverlaySet);
    
    m_name = "Overlay " + AString::number(overlayIndex + 1);
    m_enabled = (m_overlayIndex == 0);
    m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    
    m_colorBar = std::unique_ptr<AnnotationColorBar>(new AnnotationColorBar(AnnotationAttributesDefaultTypeEnum::NORMAL));
    m_colorBar->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
    
    m_matrixTriangularViewingMode = ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL;
    m_cartesianVerticalAxisLocation = ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT;
    
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
    m_sceneAssistant->add<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>("m_cartesianVerticalAxisLocation",
                                                                              &m_cartesianVerticalAxisLocation);
    m_sceneAssistant->add("m_selectedHistogramMapIndex", &m_selectedHistogramMapIndex);
    m_sceneAssistant->add("m_allHistogramMapsSelectedFlag", &m_allHistogramMapsSelectedFlag);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_CHART_OVERLAY_VALIDATE);
}

/**
 * Destructor.
 */
ChartTwoOverlay::~ChartTwoOverlay()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return Weak pointer to instance.
 * This weak pointer stored by classes (such as those in the GUI)
 * safely test if an instance is still valid and can be safely accessed.
 */
std::weak_ptr<ChartTwoOverlay>
ChartTwoOverlay::getWeakPointerToSelf()
{
    return m_weakPointerToSelf;
}

/**
 * Set the weak pointer for this instance.
 * This is called by the parent ChartTwoOverlaySet.
 *
 * @param weakPointerToSelf
 *     Weak pointer to this instance.
 */
void
ChartTwoOverlay::setWeakPointerToSelf(std::weak_ptr<ChartTwoOverlay> weakPointerToSelf)
{
    m_weakPointerToSelf = weakPointerToSelf;
    CaretAssert( ! m_weakPointerToSelf.expired());
    CaretAssert(m_weakPointerToSelf.lock().get() == this);
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
    if (event->getEventType() == EventTypeEnum::EVENT_CHART_OVERLAY_VALIDATE) {
        EventChartOverlayValidate* eov = dynamic_cast<EventChartOverlayValidate*>(event);
        CaretAssert(eov);
        eov->testValidChartOverlay(this);
        eov->setEventProcessed();
    }
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
            SelectedIndexType indexType = SelectedIndexType::INVALID;
            int32_t index = 0;
            me->getSelectionData(mapFile,
                                 indexType,
                                 index);
            if (mapFile != NULL) {
                descriptionOut.addLine("Overlay Index: " + AString::number(m_overlayIndex));
                descriptionOut.addLine("File: "+
                                       mapFile->getFileNameNoPath());
                if (mapFile->hasMapAttributes()) {
                    switch (indexType) {
                        case SelectedIndexType::INVALID:
                            break;
                        case SelectedIndexType::COLUMN:
                            descriptionOut.addLine("Column "
                                                   + AString::number(index));
                            break;
                        case SelectedIndexType::MAP:
                            if ((index >= 0)
                                && (index < mapFile->getNumberOfMaps())) {
                                descriptionOut.addLine("Map Index: "
                                                       + AString::number(index + 1));
                                descriptionOut.addLine("Map Name: "
                                                       + mapFile->getMapName(index));
                            }
                            break;
                        case SelectedIndexType::ROW:
                            descriptionOut.addLine("Row "
                                                   + AString::number(index));
                            break;
                    }
                }
                
                const ChartableTwoFileDelegate* chartDelegate = mapFile->getChartingDelegate();
                ChartTwoCompoundDataType cdt;
                chartDelegate->getChartTwoCompoundDataTypeForChartTwoDataType(getChartTwoDataType(),
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
ChartTwoOverlay::getChartTwoDataType() const
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
        
    if (chartCompoundDataType.getChartTwoDataType() != ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID) {
        CaretAssert(m_chartDataType == chartCompoundDataType.getChartTwoDataType());
    }
    m_chartCompoundDataType = chartCompoundDataType;
}

/**
 * @return The tab index.
 */
int32_t
ChartTwoOverlay::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * @return Enabled status for this surface overlay.
 */
bool
ChartTwoOverlay::isEnabled() const
{
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
    m_enabled = enabled;
}

/**
 * @return line-series loading enabled
 */
bool
ChartTwoOverlay::isLineSeriesLoadingEnabled() const
{
    CaretMappableDataFile* mapFile = NULL;
    SelectedIndexType indexType = SelectedIndexType::INVALID;
    int32_t mapIndex = -1;
    getSelectionData(mapFile,
                     indexType,
                     mapIndex);
    
    const ChartableTwoFileLineSeriesChart* lineSeriesChart = mapFile->getChartingDelegate()->getLineSeriesCharting();
    const ChartTwoLineSeriesHistory* lineSeriesHistory = lineSeriesChart->getHistory();
    return lineSeriesHistory->isLoadingEnabled();
}

/**
 * Set line-series loading enabled
 *
 * @param lineSeriesLoadingEnabled
 *    New value for line-series loading enabled
 */
void
ChartTwoOverlay::setLineSeriesLoadingEnabled(const bool lineSeriesLoadingEnabled)
{
    CaretMappableDataFile* mapFile = NULL;
    SelectedIndexType indexType = SelectedIndexType::INVALID;
    int32_t mapIndex = -1;
    getSelectionData(mapFile,
                     indexType,
                     mapIndex);
    
    ChartableTwoFileLineSeriesChart* lineSeriesChart = mapFile->getChartingDelegate()->getLineSeriesCharting();
    ChartTwoLineSeriesHistory* lineSeriesHistory = lineSeriesChart->getHistory();
    lineSeriesHistory->setLoadingEnabled(lineSeriesLoadingEnabled);
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
    
    m_enabled = overlay->m_enabled;
    
    m_mapYokingGroup = overlay->m_mapYokingGroup;
    
    *m_colorBar = *overlay->m_colorBar;
    m_matrixTriangularViewingMode = overlay->m_matrixTriangularViewingMode;
    m_cartesianVerticalAxisLocation = overlay->m_cartesianVerticalAxisLocation;
    
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
                                                                                                        overlay->m_tabIndex,
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
 * @return Is map yoking supported ?
 * 
 * NOTE: Within this class, do not use this method.
 * Instead, use isMapYokingSupportedPrivate().
 */
bool
ChartTwoOverlay::isMapYokingSupported() const
{
    CaretMappableDataFile* mapFile = NULL;
    SelectedIndexType indexType = SelectedIndexType::INVALID;
    int32_t mapIndex = -1;
    getSelectionData(mapFile, indexType, mapIndex);

    return isMapYokingSupportedPrivate(mapFile);
}

/**
 * Is map yoking supported for the given map file?
 * This is a private method and used within this class.
 * as use of the public method could cause stack
 * overlow by use of isMapYokingSupported() and
 * getSelectionData().
 *
 * @param mapFile
 *     The map file.  A NULL value is allowed.
 * @return 
 *     True if map yoking is supported, else false.
 */
bool
ChartTwoOverlay::isMapYokingSupportedPrivate(const CaretMappableDataFile* mapFile) const
{
    if (mapFile == NULL) {
        return false;
    }
    
    if (mapFile->getNumberOfMaps() < 2) {
        return false;
    }
    
    bool supportedFlag = false;
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            if (mapFile->isSurfaceMappable()
                || mapFile->isVolumeMappable()) {
                supportedFlag = true;
            }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            if (mapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
                supportedFlag = true;
            }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            if (mapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
                supportedFlag = true;
            }
            break;
    }
    
    return supportedFlag;
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
 * Get a bounding box for data displayed within this overlay.
 * Bounds are provided for histogram and line-series charts only.
 * 
 * @param boundingBox
 *     Upon exit contains bounds for data within this overlay
 * @return
 *     True if the bounds are valid, else false.
 */
bool
ChartTwoOverlay::getBounds(BoundingBox& boundingBoxOut) const
{
    boundingBoxOut.resetForUpdate();

    if ( ! isEnabled()) {
        return false;
    }
    
    CaretMappableDataFile* mapFile = NULL;
    SelectedIndexType selectedIndexType = SelectedIndexType::INVALID;
    int32_t selectedIndex = -1;
    getSelectionData(mapFile,
                     selectedIndexType,
                     selectedIndex);
    
    if (mapFile == NULL) {
        return false;
    }
    
    ChartableTwoFileDelegate* chartDelegate = mapFile->getChartingDelegate();
    
    bool validFlag = false;
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
        {
            ChartableTwoFileHistogramChart* histogramChart = chartDelegate->getHistogramCharting();
            const Histogram* histogram = histogramChart->getHistogramForChartDrawing(selectedIndex,
                                                                                     (isAllMapsSupported()
                                                                                      && isAllMapsSelected()));
            //CaretAssert(histogram);
            if (histogram != NULL) {
                float histogramMinX = 0.0, histogramMaxX = 0.0, histogramMaxY = 0.0;
                histogram->getRangeAndMaxDisplayHeight(histogramMinX, histogramMaxX, histogramMaxY);
                if (histogramMaxX > histogramMinX) {
                    boundingBoxOut.set(histogramMinX, histogramMaxX,
                                       0.0f, histogramMaxY,
                                       0.0f, 0.0f);
                    validFlag = true;
                }
            }
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
        {
            const ChartableTwoFileLineSeriesChart* lineSeriesChart = chartDelegate->getLineSeriesCharting();
            const ChartTwoLineSeriesHistory* lineSeriesHistory = lineSeriesChart->getHistory();
            validFlag = lineSeriesHistory->getBounds(boundingBoxOut);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    
    return validFlag;
}

/**
 * @return The selected map file (NULL if not map file available)
 */
CaretMappableDataFile*
ChartTwoOverlay::getSelectedMapFile() const
{
    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* selectedMapFile = NULL;
    SelectedIndexType selectedIndexType = SelectedIndexType::INVALID;
    int32_t selectedIndex = -1;
    getSelectionData(mapFiles,
                     selectedMapFile,
                     selectedIndexType,
                     selectedIndex);
    
    return selectedMapFile;
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedIndexTypeOut
 *    Type of index selection
 * @param selectedIndexOut
 *    Selected index in the selected file.
 */
void
ChartTwoOverlay::getSelectionData(CaretMappableDataFile* &selectedMapFileOut,
                                  SelectedIndexType& selectedIndexTypeOut,
                                  int32_t& selectedIndexOut) const
{
    std::vector<CaretMappableDataFile*> mapFiles;
    getSelectionData(mapFiles,
                     selectedMapFileOut,
                     selectedIndexTypeOut,
                     selectedIndexOut);
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
 * @param selectedIndexTypeOut
 *    Type of index selection
 * @param selectedIndexOut
 *    Selected index in the selected file.
 */
void
ChartTwoOverlay::getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                                  CaretMappableDataFile* &selectedMapFileOut,
                                  std::vector<AString>& selectedFileMapNamesOut,
                                  SelectedIndexType& selectedIndexTypeOut,
                                  int32_t& selectedIndexOut) const
{
    getSelectionDataPrivate(mapFilesOut,
                            selectedMapFileOut,
                            &selectedFileMapNamesOut,
                            selectedIndexTypeOut,
                            selectedIndexOut);    
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param mapFilesOut
 *    Contains all map files that can be selected.
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedIndexTypeOut
 *    Type of index selection
 * @param selectedIndexOut
 *    Selected index in the selected file.
 */
void
ChartTwoOverlay::getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                               CaretMappableDataFile* &selectedMapFileOut,
                                  SelectedIndexType& selectedIndexTypeOut,
                                  int32_t& selectedIndexOut) const
{
    getSelectionDataPrivate(mapFilesOut,
                            selectedMapFileOut,
                            NULL,
                            selectedIndexTypeOut,
                            selectedIndexOut);
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
 *    Optional output, if not NULL, with map names
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void
ChartTwoOverlay::getSelectionDataPrivate(std::vector<CaretMappableDataFile*>& mapFilesOut,
                                         CaretMappableDataFile* &selectedMapFileOut,
                                         std::vector<AString>* selectedFileMapNamesOut,
                                         SelectedIndexType& selectedIndexTypeOut,
                                         int32_t& selectedIndexOut) const
{
    mapFilesOut.clear();
    selectedMapFileOut = NULL;
    selectedIndexTypeOut = SelectedIndexType::INVALID;
    selectedIndexOut = -1;
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
        if (chartingFile->isChartingTwoSupported()) {
            bool useIt = false;
            
            std::vector<ChartTwoCompoundDataType> chartCompoundDataTypes;
            chartingFile->getSupportedChartTwoCompoundDataTypes(chartCompoundDataTypes);
            
            for (auto& compoundType : chartCompoundDataTypes) {
                if (m_chartDataType == compoundType.getChartTwoDataType()) {
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
                            
                            /*
                             * If file is a scalar data series, and the same scalar data series file
                             * is enabled in a "higher" chart overlay, do not show the file in this
                             * overlay.  Updated to only hide this file in disabled overlays.
                             */
                            const bool enableSdsFilterFlag(true);
                            if (enableSdsFilterFlag) {
                                if (mapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
                                    if ( ! isEnabled()) {
                                        for (int32_t io = 0; io < m_overlayIndex; io++) {
                                            const ChartTwoOverlay* otherOverlay = m_parentChartTwoOverlaySet->getOverlay(io);
                                            CaretAssert(otherOverlay);
                                            if (otherOverlay->isEnabled()) {
                                                if (otherOverlay->getSelectedMapFile() == mapFile) {
                                                    useIt = false;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
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
        
        ChartableTwoFileMatrixChart* matrixChart = NULL;
        
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
                selectedIndexTypeOut = SelectedIndexType::MAP;
                selectedIndexOut     = m_selectedHistogramMapIndex;
            }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            {
                /*
                 * Row/Column Scalar files use the matrix for line chart tracking
                 */
                ChartableTwoFileDelegate* chartDelegate = m_selectedMapFile->getChartingDelegate();
                ChartableTwoFileLineSeriesChart* lineChart = chartDelegate->getLineSeriesCharting();
                CaretAssert(lineChart);
                
                switch (lineChart->getLineSeriesContentType()) {
                    case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED:
                        break;
                    case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA:
                        break;
                    case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA:
                        matrixChart = chartDelegate->getMatrixCharting();
                        break;
                }
            }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            {
                ChartableTwoFileDelegate* chartDelegate = m_selectedMapFile->getChartingDelegate();
                matrixChart = chartDelegate->getMatrixCharting();
//                const ChartableTwoFileMatrixChart* matrixChart = chartDelegate->getMatrixCharting();
//                CaretAssert(matrixChart);
//                int32_t numRows = 0;
//                int32_t numCols = 0;
//                matrixChart->getMatrixDimensions(numRows, numCols);
//                
//                ChartTwoMatrixLoadingDimensionEnum::Enum rowColumnDimension;
//                std::vector<int32_t> columnIndices;
//                std::vector<int32_t> rowIndices;
//                matrixChart->getSelectedRowColumnIndices(m_parentChartTwoOverlaySet->m_tabIndex,
//                                                         rowColumnDimension,
//                                                         rowIndices,
//                                                         columnIndices);
//                
//                switch (rowColumnDimension) {
//                    case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
//                        numMaps = numCols;
//                        if ( ! columnIndices.empty()) {
//                            selectedIndexTypeOut = SelectedIndexType::COLUMN;
//                            selectedIndexOut     = columnIndices[0];
//                        }
//                        if (matrixChart->hasColumnSelection()) {
//                            if (selectedFileMapNamesOut != NULL) {
//                                for (int32_t i = 0; i < numMaps; i++) {
//                                    selectedFileMapNamesOut->push_back(matrixChart->getColumnName(i));
//                                }
//                            }
//                        }
//                        break;
//                    case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
//                        numMaps = numRows;
//                        if ( ! rowIndices.empty()) {
//                            selectedIndexTypeOut = SelectedIndexType::ROW;
//                            selectedIndexOut     = rowIndices[0];
//                        }
//                        if (matrixChart->hasRowSelection()) {
//                            if (selectedFileMapNamesOut != NULL) {
//                                for (int32_t i = 0; i < numMaps; i++) {
//                                    selectedFileMapNamesOut->push_back(matrixChart->getRowName(i));
//                                }
//                            }
//                        }
//                        break;
//                }
            }
                break;
        }
        
        if (matrixChart != NULL) {
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
            
            switch (rowColumnDimension) {
                case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                    numMaps = numCols;
                    if ( ! columnIndices.empty()) {
                        selectedIndexTypeOut = SelectedIndexType::COLUMN;
                        selectedIndexOut     = columnIndices[0];
                    }
                    if (matrixChart->hasColumnSelection()) {
                        if (selectedFileMapNamesOut != NULL) {
                            for (int32_t i = 0; i < numMaps; i++) {
                                selectedFileMapNamesOut->push_back(matrixChart->getColumnName(i));
                            }
                        }
                    }
                    break;
                case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                    numMaps = numRows;
                    if ( ! rowIndices.empty()) {
                        selectedIndexTypeOut = SelectedIndexType::ROW;
                        selectedIndexOut     = rowIndices[0];
                    }
                    if (matrixChart->hasRowSelection()) {
                        if (selectedFileMapNamesOut != NULL) {
                            for (int32_t i = 0; i < numMaps; i++) {
                                selectedFileMapNamesOut->push_back(matrixChart->getRowName(i));
                            }
                        }
                    }
                    break;
            }
        }
        
    }

    selectedMapFileOut = m_selectedMapFile;
    
    /*
     * Update the compound data type if this is the FIRST OVERLAY
     */
    if (m_overlayIndex == 0) {
        if (m_selectedMapFile != NULL) {
            const ChartableTwoFileDelegate* chartFile = m_selectedMapFile->getChartingDelegate();
            CaretAssert(chartFile);
            chartFile->getChartTwoCompoundDataTypeForChartTwoDataType(m_chartDataType,
                                                                m_chartCompoundDataType);
        }
        CaretAssert(m_parentChartTwoOverlaySet);
        m_parentChartTwoOverlaySet->firstOverlaySelectionChanged();
    }
    
    if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        if ( ! isMapYokingSupportedPrivate(m_selectedMapFile)) {
            m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
        }
    }
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
        ChartableTwoFileLineSeriesChart* lineSeriesChart = NULL;
        ChartableTwoFileMatrixChart* matrixChart = NULL;
        
        switch (m_chartDataType) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                if (selectedMapIndex >= 0) {
                    m_selectedHistogramMapIndex = selectedMapIndex;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            {
                /*
                 * Row/Column Scalar files use the matrix for line chart tracking
                 */
                ChartableTwoFileDelegate* chartDelegate = m_selectedMapFile->getChartingDelegate();
                ChartableTwoFileLineSeriesChart* lineChart = chartDelegate->getLineSeriesCharting();
                CaretAssert(lineChart);
                
                switch (lineChart->getLineSeriesContentType()) {
                    case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED:
                        break;
                    case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA:
                        break;
                    case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA:
                        lineSeriesChart = lineChart;
                        matrixChart     = chartDelegate->getMatrixCharting();
                        break;
                }
            }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            {
                ChartableTwoFileDelegate* chartDelegate = m_selectedMapFile->getChartingDelegate();
                CaretAssert(chartDelegate);
                matrixChart   = chartDelegate->getMatrixCharting();

                if (m_selectedMapFile->getDataFileType() == DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES) {
                    ChartableTwoFileLineSeriesChart* lineChart = chartDelegate->getLineSeriesCharting();
                    if (lineChart != NULL) {
                        switch (lineChart->getLineSeriesContentType()) {
                            case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_UNSUPPORTED:
                                break;
                            case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_BRAINORDINATE_DATA:
                                break;
                            case ChartTwoLineSeriesContentTypeEnum::LINE_SERIES_CONTENT_ROW_SCALAR_DATA:
                                lineSeriesChart = lineChart;
                                break;
                        }
                    }
                }
            }
                break;
        }
        
        if (matrixChart != NULL) {
            matrixChart->setSelectedRowColumnIndex(m_parentChartTwoOverlaySet->m_tabIndex,
                                                   selectedMapIndex);
            if (lineSeriesChart) {
                lineSeriesChart->loadDataForRowOrColumn(m_tabIndex,
                                                        selectedMapIndex);
            }
        }
        
        if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            if (m_selectedMapFile == NULL) {
                m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
            }
        }
    }
    
    
    /*
     * By calling getSelectionData(), it will validate the
     * selected file and map index and update if needed 
     * (such as a valid map index).
     */
    CaretMappableDataFile* mapFile = NULL;
    ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
    int32_t selectedIndex = -1;
    getSelectionData(mapFile,
                     selectedIndexType,
                     selectedIndex);
}

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
            SelectedIndexType indexType = SelectedIndexType::INVALID;
            int32_t mapIndex = 0;
            getSelectionData(cmdf,
                             indexType,
                             mapIndex);
            
            if (cmdf != NULL) {
                if (cmdf->getNumberOfMaps() > 1) {
                    supportedFlag = true;
                }
                else {
                    /*
                     * Parcel type files (dpconn, pconn, and pdconn) are a matrix
                     * type file in which one row is loaded as the current map.
                     * Allow "all maps" so that the histogram of all data can
                     * be viewed as a chart histogram overlay.
                     */
                    switch (cmdf->getDataFileType()) {
                        case DataFileTypeEnum::ANNOTATION:
                            break;
                        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                            break;
                        case DataFileTypeEnum::BORDER:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_DENSE:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                            supportedFlag = true;
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                            supportedFlag = true;
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                            supportedFlag = true;
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                            break;
                        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                            break;
                        case DataFileTypeEnum::FOCI:
                            break;
                        case DataFileTypeEnum::IMAGE:
                            break;
                        case DataFileTypeEnum::LABEL:
                            break;
                        case DataFileTypeEnum::METRIC:
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
                    }
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
    if ( ! isMatrixTriangularViewingModeSupported()) {
        if (m_matrixTriangularViewingMode != ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL) {
            m_matrixTriangularViewingMode = ChartTwoMatrixTriangularViewingModeEnum::MATRIX_VIEW_FULL;
        }
    }
    
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
        {
            ChartableTwoFileDelegate* chartDelegate        = m_selectedMapFile->getChartingDelegate();
            const ChartableTwoFileMatrixChart* matrixChart = chartDelegate->getMatrixCharting();
            return matrixChart->isMatrixTriangularViewingModeSupported();
        }
    }
    
    return false;
}

/**
 * @return Location of vertical cartesian axis
 */
ChartAxisLocationEnum::Enum
ChartTwoOverlay::getCartesianVerticalAxisLocation() const
{
    validateCartesianVerticalAxisLocation();
    return m_cartesianVerticalAxisLocation;
}

/**
 * Set Location of vertical cartesian axis
 *
 * @param cartesianVerticalAxisLocation
 *    New value for Location of vertical cartesian axis
 */
void
ChartTwoOverlay::setCartesianVerticalAxisLocation(const ChartAxisLocationEnum::Enum cartesianVerticalAxisLocation)
{
    m_cartesianVerticalAxisLocation = cartesianVerticalAxisLocation;
    validateCartesianVerticalAxisLocation();
}

/**
 * Validate cartesian vertical axis to valid locations (left and right)
 */
void
ChartTwoOverlay::validateCartesianVerticalAxisLocation() const
{
    
    switch (m_cartesianVerticalAxisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
            m_cartesianVerticalAxisLocation = ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT;
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            m_cartesianVerticalAxisLocation = ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT;
            break;
    }
}


/**
 * @return Is the cartesian vertical axis location supported?
 */
bool
ChartTwoOverlay::isCartesianVerticalAxisLocationSupported() const
{
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            return true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            return true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
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
    
    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* selectedMapFile = NULL;
    //AString selectedMapUniqueID;
    SelectedIndexType selectedIndexType = SelectedIndexType::INVALID;
    int32_t selectedMapIndex;
    getSelectionData(mapFiles,
                     selectedMapFile,
                     selectedIndexType,
                     selectedMapIndex);
    
    AString sceneSelectedMapFileNameWithPath;
    AString sceneSelectedMapFileNameNoPath;
    AString sceneSelectedMapName;
    int32_t sceneSelectedMapIndex = selectedMapIndex;
    
    /*
     * NOTE:Some of the connectivity matrix files may not have a valid
     * selection index (-1) until the user identifies a brainordinate
     */
    if (selectedMapFile != NULL) {
        sceneSelectedMapFileNameWithPath = selectedMapFile->getFileName();
        sceneSelectedMapFileNameNoPath = selectedMapFile->getFileNameNoPath();
        if ((selectedMapIndex >= 0)
            && (selectedMapIndex < selectedMapFile->getNumberOfMaps())) {
            sceneSelectedMapName = selectedMapFile->getMapName(selectedMapIndex);
        }
    }

    sceneClass->addPathName("sceneSelectedMapFileNameWithPath",
                            sceneSelectedMapFileNameWithPath);
    sceneClass->addString("sceneSelectedMapFileNameNoPath",
                          sceneSelectedMapFileNameNoPath);
    sceneClass->addString("sceneSelectedMapName",
                          sceneSelectedMapName);
    sceneClass->addInteger("sceneSelectedMapIndex",
                           sceneSelectedMapIndex);

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
    const MapYokingGroupEnum::Enum mapYokingGroupFromScene = m_mapYokingGroup;
    
    /*
     * Making a call to getSelectionData() to get the availble
     * map files
     */
    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* unusedSelectedMapFile = NULL;
    SelectedIndexType unusedSelectedIndexType = SelectedIndexType::INVALID;
    int32_t unusedSelectedMapIndex;
    getSelectionData(mapFiles,
                     unusedSelectedMapFile,
                     unusedSelectedIndexType,
                     unusedSelectedMapIndex);

    const AString selectedMapFileNameWithPath = sceneClass->getPathNameValue("sceneSelectedMapFileNameWithPath");
    
    const AString selectedMapFileName = sceneClass->getStringValue("sceneSelectedMapFileNameNoPath",
                                                                   "");
    const AString selectedMapName = sceneClass->getStringValue("sceneSelectedMapName",
                                                               "");
    const int32_t selectedMapIndex = sceneClass->getIntegerValue("sceneSelectedMapIndex",
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
        if (foundMapNameIndex >= 0) {
            if (foundMapNameFile != NULL) {
                setSelectionData(foundMapNameFile,
                                 foundMapNameIndex);
                found = true;
            }
        }
    }
    
    if ( ! found) {
        /*
         * If not found by unique ID, try to find map by name
         */
        if ( ! selectedMapName.isEmpty()) {
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
     * NOTE:Some of the connectivity matrix files may not have a valid
     * selection index (-1) until the user identifies a brainordinate
     */
    if ( ! found) {
        if (matchedMapFile != NULL) {
            if (matchedMapFile->getNumberOfMaps() > 0) {
                int32_t defaultMapIndex = -1;
                switch (m_chartDataType) {
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                        defaultMapIndex = 0;
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                        defaultMapIndex = 0;
                        break;
                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                        break;
                }
                setSelectionData(matchedMapFile,
                                 defaultMapIndex);
            }
        }
    }
    
    if (mapYokingGroupFromScene != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        /*
         * Need to update selections and then apply yoking as 
         * yoking is cleared when the the previous file
         * was not found.
         */
        CaretMappableDataFile* mapFile = NULL;
        SelectedIndexType indexType = SelectedIndexType::INVALID;
        int32_t mapIndex = -1;
        getSelectionData(mapFile, indexType, mapIndex);
        setMapYokingGroup(mapYokingGroupFromScene);
    }
    
    
    
    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

