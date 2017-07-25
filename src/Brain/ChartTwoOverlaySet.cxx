
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

#define __CHART_TWO_OVERLAY_SET_DECLARE__
#include "ChartTwoOverlaySet.h"
#undef __CHART_TWO_OVERLAY_SET_DECLARE__

#include "AnnotationPercentSizeText.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "EventBrowserTabGet.h"
#include "EventChartTwoAttributesChanged.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"


using namespace caret;


    
/**
 * \class caret::ChartTwoOverlaySet 
 * \brief A set of chart overlays.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *     Type of charts allowed in this overlay
 * @param name
 *     Name of the overlay set.
 * @param tabIndex
 *     Index of tab in which this overlay set is used.
 */
ChartTwoOverlaySet::ChartTwoOverlaySet(const ChartTwoDataTypeEnum::Enum chartDataType,
                                 const AString& name,
                                 const int32_t tabIndex)
: CaretObject(),
m_chartDataType(chartDataType),
m_name(name),
m_tabIndex(tabIndex)
{
    m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    
    m_chartAxisLeft   = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT));
    m_chartAxisRight  = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT));
    m_chartAxisBottom = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM));
    
    m_chartAxisLeft->setEnabledByChart(false);
    m_chartAxisRight->setEnabledByChart(false);
    m_chartAxisBottom->setEnabledByChart(false);
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
        {
            m_chartAxisLeft->setEnabledByChart(true);
            m_chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            float rangeMin = 0.0, rangeMax = 0.0;
            m_chartAxisLeft->getRange(rangeMin, rangeMax);
            rangeMin = 0.0;
            m_chartAxisLeft->setRange(rangeMin, rangeMax);
            
            m_chartAxisRight->setEnabledByChart(false);
            m_chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            m_chartAxisRight->getRange(rangeMin, rangeMax);
            rangeMin = 0.0;
            m_chartAxisRight->setRange(rangeMin, rangeMax);
            
            m_chartAxisBottom->setEnabledByChart(true);
            m_chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
        {
            m_chartAxisLeft->setEnabledByChart(true);
            m_chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            float rangeMin = 0.0, rangeMax = 0.0;
            m_chartAxisLeft->getRange(rangeMin, rangeMax);
            rangeMin = 0.0;
            m_chartAxisLeft->setRange(rangeMin, rangeMax);
            
            m_chartAxisRight->setEnabledByChart(false);
            m_chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            m_chartAxisRight->getRange(rangeMin, rangeMax);
            rangeMin = 0.0;
            m_chartAxisRight->setRange(rangeMin, rangeMax);
            
            m_chartAxisBottom->setEnabledByChart(true);
            m_chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    
    m_sceneAssistant  = new SceneClassAssistant();
    m_sceneAssistant->add("m_chartAxisLeft",
                          "ChartTwoCartesianAxis",
                          m_chartAxisLeft.get());
    m_sceneAssistant->add("m_chartAxisRight",
                          "ChartTwoCartesianAxis",
                          m_chartAxisRight.get());
    m_sceneAssistant->add("m_chartAxisBottom",
                          "ChartTwoCartesianAxis",
                          m_chartAxisBottom.get());
    m_sceneAssistant->add("m_numberOfDisplayedOverlays",
                          &m_numberOfDisplayedOverlays);
    
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays.push_back(std::make_shared<ChartTwoOverlay>(this,
                                                                    m_chartDataType,
                                                                    m_tabIndex,
                                                                    i));
        CaretAssertVectorIndex(m_overlays, i);
        m_overlays[i]->setWeakPointerToSelf(m_overlays[i]);
    }

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CHART_TWO_ATTRIBUTES_CHANGED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
}

/**
 * Destructor.
 */
ChartTwoOverlaySet::~ChartTwoOverlaySet()
{
    EventManager::get()->removeAllEventsFromListener(this);
    m_overlays.clear();
    delete m_sceneAssistant;
}

/**
 * Copy the given overlay set to this overlay set.
 * @param overlaySet
 *    Overlay set that is copied.
 */
void
ChartTwoOverlaySet::copyOverlaySet(const ChartTwoOverlaySet* overlaySet)
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->copyData(overlaySet->getOverlay(i));
    }
    *m_chartAxisLeft   = *overlaySet->m_chartAxisLeft;
    *m_chartAxisRight  = *overlaySet->m_chartAxisRight;
    *m_chartAxisBottom = *overlaySet->m_chartAxisBottom;
    
    m_numberOfDisplayedOverlays = overlaySet->m_numberOfDisplayedOverlays;
}

/**
 * Copy the cartesian axes from the given overlay set to this overlay set.
 *
 * @param overlaySet
 *     Overlay from which axes are copied.
 */
void
ChartTwoOverlaySet::copyCartesianAxes(const ChartTwoOverlaySet* overlaySet)
{
    CaretAssert(m_chartDataType == overlaySet->m_chartDataType);
    
    *m_chartAxisLeft   = *overlaySet->m_chartAxisLeft;
    *m_chartAxisRight  = *overlaySet->m_chartAxisRight;
    *m_chartAxisBottom = *overlaySet->m_chartAxisBottom;
}


/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
ChartTwoOverlay*
ChartTwoOverlaySet::getPrimaryOverlay()
{
    return m_overlays[0].get();
}

/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
const ChartTwoOverlay*
ChartTwoOverlaySet::getPrimaryOverlay() const
{
    return m_overlays[0].get();
}

/**
 * Get the overlay at the specified index.  If caller needs
 * to store the pointer there is a risk that the overlay 
 * may be destroyed.  Consider using getOverlayWeakPointer()
 * instead.
 *
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
const ChartTwoOverlay*
ChartTwoOverlaySet::getOverlay(const int32_t overlayNumber) const
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayNumber);
    return m_overlays[overlayNumber].get();
}

/**
 * Get the overlay at the specified index.  If caller needs
 * to store the pointer there is a risk that the overlay
 * may be destroyed.  Consider using getOverlayWeakPointer()
 * instead.
 *
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
ChartTwoOverlay*
ChartTwoOverlaySet::getOverlay(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayNumber);
    return m_overlays[overlayNumber].get();
}

/**
 * Get the a "weak pointer" to the chart overlay at the given
 * index.  Caller can store this pointer and test to see
 * if the chart overlay is still valid.
 *
 * @param overlayNumber
 *   Index of the overlay.
 * @return Weak pointer to chart verlay at the given index.
 */
std::weak_ptr<ChartTwoOverlay>
ChartTwoOverlaySet::getOverlayWeakPointer(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayNumber);
    return m_overlays[overlayNumber];
}


/**
 * Get a text description of the window's content.
 *
 * @param descriptionOut
 *    Description of the window's content.
 */
void
ChartTwoOverlaySet::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Overlay Set");
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        if (getOverlay(i)->isEnabled()) {
            descriptionOut.pushIndentation();
            
            descriptionOut.pushIndentation();
            getOverlay(i)->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
            
            descriptionOut.popIndentation();
        }
    }
}

/**
 * Find the top-most displayed and enabled overlay containing the given data file.
 *
 * @param mapFile
 *     File for which overlay is requested.
 */
ChartTwoOverlay*
ChartTwoOverlaySet::getDisplayedOverlayContainingDataFile(const CaretMappableDataFile* mapFile)
{
    if (mapFile == NULL) {
        return NULL;
    }
    
    ChartTwoOverlay* overlayOut = NULL;
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        ChartTwoOverlay* overlay = getOverlay(i);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* overlayMapFile = NULL;
            ChartTwoOverlay::SelectedIndexType selectionType = ChartTwoOverlay::SelectedIndexType::INVALID;
            int32_t dataFileIndex = -1;
            overlay->getSelectionData(overlayMapFile,
                                            selectionType,
                                            dataFileIndex);
            if (mapFile == overlayMapFile) {
                overlayOut = overlay;
            }
        }
    }
    
    return overlayOut;
}

/**
 * Add a displayed overlay.  If the maximum
 * number of surface overlays is reached,
 * this method has no effect.
 */
void
ChartTwoOverlaySet::addDisplayedOverlay()
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
ChartTwoOverlaySet::getNumberOfDisplayedOverlays() const
{
    return m_numberOfDisplayedOverlays;
}

/**
 * Insert an overlay below this overlay
 * @param overlayIndex
 *     Index of overlay for which an overlay is added below
 */
void
ChartTwoOverlaySet::insertOverlayAbove(const int32_t overlayIndex)
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
ChartTwoOverlaySet::insertOverlayBelow(const int32_t overlayIndex)
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
ChartTwoOverlaySet::removeDisplayedOverlay(const int32_t overlayIndex)
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayIndex);
    m_overlays[overlayIndex]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    
    if (m_numberOfDisplayedOverlays > BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays--;
        for (int32_t i = overlayIndex; i < m_numberOfDisplayedOverlays; i++) {
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i+1);
            m_overlays[i]->copyData(m_overlays[i+1].get());
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
ChartTwoOverlaySet::moveDisplayedOverlayUp(const int32_t overlayIndex)
{
    if (overlayIndex > 0) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
        m_overlays[overlayIndex]->swapData(m_overlays[overlayIndex - 1].get());
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
ChartTwoOverlaySet::moveDisplayedOverlayDown(const int32_t overlayIndex)
{
    const int32_t nextOverlayIndex = overlayIndex + 1;
    if (nextOverlayIndex < m_numberOfDisplayedOverlays) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, nextOverlayIndex);
        m_overlays[overlayIndex]->swapData(m_overlays[nextOverlayIndex].get());
    }
}

/**
 * Initialize the overlays.
 */
void
ChartTwoOverlaySet::initializeOverlays()
{
    /*
     * This method could be used to choose specific file types
     * for the default overlays similar to that in OverlaySet.cxx.
     */
}

/**
 * Called by first overlay when the first overlay's selection changes.
 * All other overlays are set to use the same chart compound data type
 * so that the charts in the tab are compatible
 */
void
ChartTwoOverlaySet::firstOverlaySelectionChanged()
{
    if (m_inFirstOverlayChangedMethodFlag) {
        return;
    }
    
    m_inFirstOverlayChangedMethodFlag = true;
    
    ChartTwoCompoundDataType cdt = m_overlays[0]->getChartTwoCompoundDataType();
    
    for (int32_t i = 1; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->setChartTwoCompoundDataType(cdt);
    }
    
    PlainTextStringBuilder description;
    getDescriptionOfContent(description);
    
    m_inFirstOverlayChangedMethodFlag = false;
}


/**
 * Reset the yoking status of all overlays to off.
 */
void
ChartTwoOverlaySet::resetOverlayYokingToOff()
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoOverlaySet::toString() const
{
    return "ChartTwoOverlaySet";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartTwoOverlaySet::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_CHART_TWO_ATTRIBUTES_CHANGED) {
        EventChartTwoAttributesChanged* attributeEvent = dynamic_cast<EventChartTwoAttributesChanged*>(event);
        CaretAssert(attributeEvent);
        
        switch (attributeEvent->getMode()) {
            case EventChartTwoAttributesChanged::Mode::INVALID:
                break;
            case EventChartTwoAttributesChanged::Mode::CARTESIAN_AXIS:
            {
                YokingGroupEnum::Enum yokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
                ChartTwoDataTypeEnum::Enum chartTwoDataType = ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID;
                ChartTwoCartesianAxis* cartesianAxis = NULL;
                attributeEvent->getCartesianAxisChanged(yokingGroup,
                                                        chartTwoDataType,
                                                        cartesianAxis);
                
                /*
                 * Only tabs in the windows are valid
                 */
                EventBrowserTabGet tabEvent(m_tabIndex);
                EventManager::get()->sendEvent(tabEvent.getPointer());
                const BrowserTabContent* btc = tabEvent.getBrowserTab();
                if (btc != NULL) {
                    const YokingGroupEnum::Enum tabYoking = btc->getYokingGroup();
                    
                    if ((yokingGroup != YokingGroupEnum::YOKING_GROUP_OFF)
                        && (yokingGroup == tabYoking)
                        && (m_chartDataType == chartTwoDataType)) {
                        switch (cartesianAxis->getAxisLocation()) {
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                                *m_chartAxisBottom = *cartesianAxis;
                                break;
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                                *m_chartAxisLeft = *cartesianAxis;
                                break;
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                                *m_chartAxisRight = *cartesianAxis;
                                break;
                            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                                CaretAssert(0);
                                break;
                        }
                    }
                }
            }
                break;
        }
        
        attributeEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
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
                ChartTwoOverlay* overlay = getOverlay(j);
                if (overlay->isMapYokingSupported()) {
                    CaretMappableDataFile* mapFile = NULL;
                    ChartTwoOverlay::SelectedIndexType indexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                    int32_t mapIndex = -1;
                    overlay->getSelectionData(mapFile,
                                              indexType,
                                              mapIndex);
                    if (mapFile != NULL) {
                        if (overlay->isMapYokingSupported()) {
                            mapYokeEvent->addMapYokedFile(mapFile, overlay->getMapYokingGroup(), m_tabIndex);
                        }
                    }
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
            const CaretMappableDataFile* eventMapFile = selectMapEvent->getCaretMappableDataFile();
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                ChartTwoOverlay* overlay = getOverlay(j);
                if (overlay->isMapYokingSupported()) {
                    if (overlay->getMapYokingGroup() == selectMapEvent->getMapYokingGroup()) {
                        CaretMappableDataFile* mapFile = NULL;
                        ChartTwoOverlay::SelectedIndexType indexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                        int32_t mapIndex = -1;
                        overlay->getSelectionData(mapFile,
                                                  indexType,
                                                  mapIndex);
                        
                        if (mapFile != NULL) {
                            if (overlay->isMapYokingSupported()) {
                                if (yokingGroupMapIndex < mapFile->getNumberOfMaps()) {
                                    overlay->setSelectionData(mapFile,
                                                              yokingGroupMapIndex);
                                }
                                
                                if (mapFile == eventMapFile) {
                                    overlay->setEnabled(yokingGroupSelectedStatus);
                                }
                            }
                        }
                    }
                }
            }
            
            selectMapEvent->setEventProcessed();
        }
    }
}

/**
 * Get the displayed chart axes.
 *
 * @param axesOut
 *     Output containing the displayed axes.
 * @param leftAxisLabelOut
 *     Label for left axis (may be NULL if axis not displayed).
 * @param rightAxisLabelOut
 *     Label for right axis (may be NULL if axis not displayed).
 * @param bottomAxisLabelOut
 *     Label for bottom axis (may be NULL if axis not displayed).
 */
void
ChartTwoOverlaySet::getDisplayedChartAxes(std::vector<ChartTwoCartesianAxis*>& axesOut,
                                          AnnotationPercentSizeText* &leftAxisLabelOut,
                                          AnnotationPercentSizeText* &rightAxisLabelOut,
                                          AnnotationPercentSizeText* &bottomAxisLabelOut) const
{
    axesOut.clear();
    leftAxisLabelOut   = NULL;
    rightAxisLabelOut  = NULL;
    bottomAxisLabelOut = NULL;
    
    AString lineSeriesDataTypeName;
    
    bool showAxesFlag = false;
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            showAxesFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
        {
            const ChartTwoOverlay* primaryOverlay = getPrimaryOverlay();
            if (primaryOverlay != NULL) {
                CaretMappableDataFile* mapFile = NULL;
                ChartTwoOverlay::SelectedIndexType indexType;
                int32_t mapIndex;
                primaryOverlay->getSelectionData(mapFile, indexType, mapIndex);
                if (mapFile != NULL) {
                    const NiftiTimeUnitsEnum::Enum units = mapFile->getMapIntervalUnits();
                    switch (units) {
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_HZ:
                            lineSeriesDataTypeName = "Hertz";
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_MSEC:
                            lineSeriesDataTypeName = "milliseconds";
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_PPM:
                            lineSeriesDataTypeName = "parts per million";
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_SEC:
                            lineSeriesDataTypeName = "seconds";
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN:
                            lineSeriesDataTypeName = "data";
                            break;
                        case NiftiTimeUnitsEnum::NIFTI_UNITS_USEC:
                            lineSeriesDataTypeName = "microseconds";
                            break;
                    }
                }
            }
        }
            showAxesFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }

    bool showBottomFlag = false;
    bool showLeftFlag   = false;
    bool showRightFlag  = false;
    
    if (showAxesFlag) {
        for (auto overlay : m_overlays) {
            if (overlay->isEnabled()) {
                CaretMappableDataFile* mapFile = NULL;
                ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
                int32_t selectedIndex = -1;
                overlay->getSelectionData(mapFile,
                                          selectedIndexType,
                                          selectedIndex);
                if (mapFile != NULL) {
                    showBottomFlag = true;
                    switch (overlay->getCartesianVerticalAxisLocation()) {
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                            CaretAssert(0);
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                            showLeftFlag = true;
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                            showRightFlag = true;
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                            CaretAssert(0);
                            break;
                    }
                }
            }
        }
    }
    
    m_chartAxisBottom->setEnabledByChart(showBottomFlag);
    m_chartAxisLeft->setEnabledByChart(showLeftFlag);
    m_chartAxisRight->setEnabledByChart(showRightFlag);
    
    if (m_chartAxisBottom->isEnabledByChart()) {
        axesOut.push_back(m_chartAxisBottom.get());
        
        bottomAxisLabelOut = getAxisLabel(m_chartAxisBottom.get());
    }
    if (m_chartAxisLeft->isEnabledByChart()) {
        axesOut.push_back(m_chartAxisLeft.get());
        
        leftAxisLabelOut = getAxisLabel(m_chartAxisLeft.get());
    }
    if (m_chartAxisRight->isEnabledByChart()) {
        axesOut.push_back(m_chartAxisRight.get());
        
        rightAxisLabelOut = getAxisLabel(m_chartAxisRight.get());
    }
}

/**
 * Get the annotation for the axis label.
 *
 * @param axis
 *     The cartesian axis.
 * @return 
 *     Pointer to annotation containing label or NULL if invalid.
 */
AnnotationPercentSizeText*
ChartTwoOverlaySet::getAxisLabel(const ChartTwoCartesianAxis* axis) const
{
    AnnotationPercentSizeText* label = NULL;
    
    const int32_t overlayTitleIndex = axis->getLabelOverlayIndex(m_numberOfDisplayedOverlays);
    const ChartTwoOverlay* chartOverlay = getOverlay(overlayTitleIndex);
    CaretMappableDataFile* mapFile = chartOverlay->getSelectedMapFile();
    if (mapFile != NULL) {
        switch (m_chartDataType) {
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        label = mapFile->getChartingDelegate()->getHistogramCharting()->getBottomAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        label = mapFile->getChartingDelegate()->getHistogramCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        label = mapFile->getChartingDelegate()->getHistogramCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        CaretAssert(0);
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        label = mapFile->getChartingDelegate()->getLineSeriesCharting()->getBottomAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        label = mapFile->getChartingDelegate()->getLineSeriesCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        label = mapFile->getChartingDelegate()->getLineSeriesCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        CaretAssert(0);
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                break;
        }
    }
    return label;
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
ChartTwoOverlaySet::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoOverlaySet",
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
ChartTwoOverlaySet::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
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
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
            m_overlays[i]->restoreFromScene(sceneAttributes,
                                            overlayClassArray->getClassAtIndex(i));
        }
    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

