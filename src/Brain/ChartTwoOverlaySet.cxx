
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
#include "BoundingBox.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoTitle.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileLineLayerChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "EventBrowserTabGet.h"
#include "EventChartTwoAttributesChanged.h"
#include "EventChartTwoAxisGetDataRange.h"
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
    
    m_chartAxisLeft   = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(this, ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT));
    m_chartAxisRight  = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(this, ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT));
    m_chartAxisBottom = std::unique_ptr<ChartTwoCartesianAxis>(new ChartTwoCartesianAxis(this, ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM));
    
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
            
            m_chartAxisRight->setEnabledByChart(false);
            m_chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            
            m_chartAxisBottom->setEnabledByChart(true);
            m_chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
        {
            m_chartAxisLeft->setEnabledByChart(true);
            m_chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            
            m_chartAxisRight->setEnabledByChart(false);
            m_chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            
            /*
             * X-axis for line series shows full extent of data
             */
            m_chartAxisBottom->setScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::DATA);
            m_chartAxisBottom->setEnabledByChart(true);
            m_chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
        {
            m_chartAxisLeft->setEnabledByChart(true);
            m_chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            
            m_chartAxisRight->setEnabledByChart(false);
            m_chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            
            /*
             * X-axis for line series shows full extent of data
             */
            m_chartAxisBottom->setScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::DATA);
            m_chartAxisBottom->setEnabledByChart(true);
            m_chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    
    m_title = std::unique_ptr<ChartTwoTitle>(new ChartTwoTitle());
    
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
    
    m_sceneAssistant->add("m_title",
                          "ChartTwoTitle",
                          m_title.get());
    m_sceneAssistant->add("m_numberOfDisplayedOverlays",
                          &m_numberOfDisplayedOverlays);
    m_sceneAssistant->add("m_axisLineThickness",
                          &m_axisLineThickness);
    
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays.push_back(std::make_shared<ChartTwoOverlay>(this,
                                                                    m_chartDataType,
                                                                    m_tabIndex,
                                                                    i));
        CaretAssertVectorIndex(m_overlays, i);
        m_overlays[i]->setWeakPointerToSelf(m_overlays[i]);
    }
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_CHART_LABEL_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CHART_TWO_ATTRIBUTES_CHANGED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CHART_TWO_AXIS_GET_DATA_RANGE);
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
    *m_title           = *overlaySet->m_title;
    m_axisLineThickness = overlaySet->m_axisLineThickness;
    
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
    m_axisLineThickness = overlaySet->m_axisLineThickness;
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
 * @return The displayed overlays which are the overlays
 * shown in the Overlay Toolbox and this includes
 * overlays that the user may have set to disabled.
 */
std::vector<ChartTwoOverlay*>
ChartTwoOverlaySet::getDisplayedOverlays() const
{
    std::vector<ChartTwoOverlay*> displayedOverlays;
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        CaretAssertVectorIndex(m_overlays, i);
        displayedOverlays.push_back(m_overlays[i].get());
    }
    
    return displayedOverlays;
}

/**
 * @return The displayed overlays but only those
 * that are enabled by the user.
 */
std::vector<ChartTwoOverlay*>
ChartTwoOverlaySet::getEnabledOverlays() const
{
    std::vector<ChartTwoOverlay*> enabledOverlays;
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        CaretAssertVectorIndex(m_overlays, i);
        if (m_overlays[i]->isEnabled()) {
            enabledOverlays.push_back(m_overlays[i].get());
        }
    }
    
    return enabledOverlays;
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
    descriptionOut.pushIndentation();
    descriptionOut.addLine("    Title: " +  (m_title->isDisplayed() ? m_title->getText() : "Disabled by user"));
    descriptionOut.popIndentation();
    
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
                    const YokingGroupEnum::Enum tabYoking = btc->getChartModelYokingGroup();
                    
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
            case EventChartTwoAttributesChanged::Mode::LINE_THICKESS:
            {
                YokingGroupEnum::Enum yokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
                ChartTwoDataTypeEnum::Enum chartTwoDataType = ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID;
                float lineThickness = 0.0f;
                attributeEvent->getLineThicknessChanged(yokingGroup,
                                                        chartTwoDataType,
                                                        lineThickness);
                
                /*
                 * Only tabs in the windows are valid
                 */
                EventBrowserTabGet tabEvent(m_tabIndex);
                EventManager::get()->sendEvent(tabEvent.getPointer());
                const BrowserTabContent* btc = tabEvent.getBrowserTab();
                if (btc != NULL) {
                    const YokingGroupEnum::Enum tabYoking = btc->getChartModelYokingGroup();
                    
                    if ((yokingGroup != YokingGroupEnum::YOKING_GROUP_OFF)
                        && (yokingGroup == tabYoking)
                        && (m_chartDataType == chartTwoDataType)) {
                        m_axisLineThickness = lineThickness;
                    }
                }
            }
                break;
            case EventChartTwoAttributesChanged::Mode::TITLE:
            {
                YokingGroupEnum::Enum yokingGroup = YokingGroupEnum::YOKING_GROUP_OFF;
                ChartTwoDataTypeEnum::Enum chartTwoDataType = ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID;
                ChartTwoTitle* chartTitle = NULL;
                attributeEvent->getTitleChanged(yokingGroup,
                                                chartTwoDataType,
                                                chartTitle);
                
                /*
                 * Only tabs in the windows are valid
                 */
                EventBrowserTabGet tabEvent(m_tabIndex);
                EventManager::get()->sendEvent(tabEvent.getPointer());
                const BrowserTabContent* btc = tabEvent.getBrowserTab();
                if (btc != NULL) {
                    const YokingGroupEnum::Enum tabYoking = btc->getChartModelYokingGroup();
                    
                    if ((yokingGroup != YokingGroupEnum::YOKING_GROUP_OFF)
                        && (yokingGroup == tabYoking)
                        && (m_chartDataType == chartTwoDataType)) {
                        *m_title = *chartTitle;
                    }
                }
            }
                break;
        }
        
        attributeEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_CHART_TWO_AXIS_GET_DATA_RANGE) {
        EventChartTwoAxisGetDataRange* rangeEvent = dynamic_cast<EventChartTwoAxisGetDataRange*>(event);
        CaretAssert(rangeEvent);
        
        if (rangeEvent->getChartOverlaySet() == this) {
            float minimumValue = 0.0f;
            float maximumValue = 0.0f;
            if (getDataRangeForAxis(rangeEvent->getChartAxisLocation(),
                                    minimumValue,
                                    maximumValue)) {
                rangeEvent->setMinimumAndMaximumValues(minimumValue,
                                                       maximumValue);
                rangeEvent->setEventProcessed();
            }
        }
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
                                
                                switch (m_chartDataType) {
                                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                                        break;
                                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                                        if (yokingGroupMapIndex < mapFile->getNumberOfMaps()) {
                                            overlay->setSelectionData(mapFile,
                                                                      yokingGroupMapIndex);
                                        }
                                        break;
                                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                                        if (yokingGroupMapIndex < mapFile->getNumberOfMaps()) {
                                            overlay->setSelectionData(mapFile,
                                                                      yokingGroupMapIndex);
                                        }
                                        break;
                                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                                        break;
                                    case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                                        break;
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
 * Get the minimum and maximum values for the given chart axis
 * from the ENABLED overlays in this chart overlay set.
 *
 * @param chartAxisLocation
 *    Location of axis.
 * @param minimumValueOut
 *    Output with minimum value for axis.
 * @param maximumValueOut
 *    Output with maximum value for axis.
 * @return
 *    True if output values are valid, else false.
 */
bool
ChartTwoOverlaySet::getDataRangeForAxis(const ChartAxisLocationEnum::Enum chartAxisLocation,
                                        float& minimumValueOut,
                                        float& maximumValueOut) const
{
    minimumValueOut = 0.0f;
    maximumValueOut = 0.0f;
    if ( ! isAxesSupportedByChartDataType()) {
        return false;
    }
    
    minimumValueOut =  std::numeric_limits<float>::max();
    maximumValueOut = -std::numeric_limits<float>::max();

    std::vector<ChartTwoOverlay*> enabledOverlays = getEnabledOverlays();
    for (auto overlay : enabledOverlays) {
        if (overlay->isEnabled()) {
            CaretMappableDataFile* mapFile = NULL;
            ChartTwoOverlay::SelectedIndexType selectedIndexType = ChartTwoOverlay::SelectedIndexType::INVALID;
            int32_t selectedIndex = -1;
            overlay->getSelectionData(mapFile,
                                      selectedIndexType,
                                      selectedIndex);
            if (mapFile != NULL) {
                BoundingBox boundingBox;
                if (overlay->getBounds(boundingBox)) {
                    switch (chartAxisLocation) {
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                            minimumValueOut = std::min(minimumValueOut, boundingBox.getMinX());
                            maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxX());
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                            if (overlay->getCartesianVerticalAxisLocation() == ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT) {
                                minimumValueOut = std::min(minimumValueOut, boundingBox.getMinY());
                                maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxY());
                            }
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                            if (overlay->getCartesianVerticalAxisLocation() == ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT) {
                                minimumValueOut = std::min(minimumValueOut, boundingBox.getMinY());
                                maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxY());
                            }
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                            minimumValueOut = std::min(minimumValueOut, boundingBox.getMinX());
                            maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxX());
                            break;
                    }
                }
            }
        }
    }
    
    if (minimumValueOut < maximumValueOut) {
        return true;
    }
    
    minimumValueOut = 0.0f;
    maximumValueOut = 0.0f;
    
    return false;
}

/**
 * @return Are axes supported by the chart data type for this overlay set?
 */
bool
ChartTwoOverlaySet::isAxesSupportedByChartDataType() const
{
    bool axisSupportedFlag = false;
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            axisSupportedFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            axisSupportedFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            axisSupportedFlag = true;
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            break;
    }
    
    return axisSupportedFlag;
}


/**
 * Get the displayed chart axes.
 *
 * @param axesOut
 *     Output containing the displayed axes.
 */
void
ChartTwoOverlaySet::getDisplayedChartAxes(std::vector<ChartTwoCartesianAxis*>& axesOut) const
{
    axesOut.clear();
    
    AString lineSeriesDataTypeName;
    
    
    float xMin = 0.0f;
    float xMax = 0.0f;
    float yMinLeft = 0.0f;
    float yMaxLeft = 0.0f;
    float yMinRight = 0.0f;
    float yMaxRight = 0.0f;
    
    bool showBottomFlag = getDataRangeForAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM,
                                              xMin, xMax);
    bool showLeftFlag   = getDataRangeForAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT,
                                              yMinLeft, yMaxLeft);
    bool showRightFlag  = getDataRangeForAxis(ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT,
                                              yMinRight, yMaxRight);
    
    m_chartAxisBottom->setEnabledByChart(showBottomFlag);
    m_chartAxisLeft->setEnabledByChart(showLeftFlag);
    m_chartAxisRight->setEnabledByChart(showRightFlag);
    
    if (m_chartAxisBottom->isEnabledByChart()) {
        axesOut.push_back(m_chartAxisBottom.get());
    }
    if (m_chartAxisLeft->isEnabledByChart()) {
        axesOut.push_back(m_chartAxisLeft.get());
    }
    if (m_chartAxisRight->isEnabledByChart()) {
        axesOut.push_back(m_chartAxisRight.get());
    }
}

/**
 * Get the text for the axis label.
 *
 * @param axis
 *     The cartesian axis.
 * @return 
 *     Text for the axis label.
 */
AString
ChartTwoOverlaySet::getAxisLabel(const ChartTwoCartesianAxis* axis) const
{
    if (axis == NULL) {
        return "";
    }
    
    AString label;
    
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
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        label = mapFile->getChartingDelegate()->getLineLayerCharting()->getBottomAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        label = mapFile->getChartingDelegate()->getLineLayerCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        label = mapFile->getChartingDelegate()->getLineLayerCharting()->getLeftRightAxisTitle();
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
 * Set the text for the axis label.
 *
 * @param axis
 *     The cartesian axis.
 * @param label
 *     New text for the axis label.
 */
void
ChartTwoOverlaySet::setAxisLabel(const ChartTwoCartesianAxis* axis,
                                 const AString& label)
{
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
                        mapFile->getChartingDelegate()->getHistogramCharting()->setBottomAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        mapFile->getChartingDelegate()->getHistogramCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        mapFile->getChartingDelegate()->getHistogramCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        CaretAssert(0);
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        mapFile->getChartingDelegate()->getLineLayerCharting()->setBottomAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        mapFile->getChartingDelegate()->getLineLayerCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        mapFile->getChartingDelegate()->getLineLayerCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        CaretAssert(0);
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        mapFile->getChartingDelegate()->getLineSeriesCharting()->setBottomAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        mapFile->getChartingDelegate()->getLineSeriesCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        mapFile->getChartingDelegate()->getLineSeriesCharting()->setLeftRightAxisTitle(label);
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
}


/**
 * @return The chart title.
 */
ChartTwoTitle*
ChartTwoOverlaySet::getChartTitle()
{
    return m_title.get();
}

/**
 * @return The chart title.
 */
const ChartTwoTitle*
ChartTwoOverlaySet::getChartTitle() const
{
    return m_title.get();
}

/**
 * @return Thickness of box around chart and tick marks on axes
 */
float
ChartTwoOverlaySet::getAxisLineThickness() const
{
    return m_axisLineThickness;
}

/**
 * Set Thickness of box around chart and tick marks on axes
 *
 * @param axisLineThickness
 *    New value for Thickness of box around chart and tick marks on axes
 */
void
ChartTwoOverlaySet::setAxisLineThickness(const float axisLineThickness)
{
    m_axisLineThickness = axisLineThickness;
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
    
    m_title->reset();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    /*
     * Title was originally saved as text and boolean
     */
    const SceneClass* oldChartTitleClass = sceneClass->getClass("m_chartTitle");
    if (oldChartTitleClass != NULL) {
        if (oldChartTitleClass->getClassName() == "AnnotationPercentSizeText") {
            std::unique_ptr<AnnotationPercentSizeText> title = std::unique_ptr<AnnotationPercentSizeText>(new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL,
                                                                                                                                        AnnotationTextFontSizeTypeEnum::PERCENTAGE_OF_VIEWPORT_HEIGHT));
            title->setText("");
            title->restoreFromScene(sceneAttributes, oldChartTitleClass);
            
            m_title->setText(title->getText());
            m_title->setDisplayed(sceneClass->getBooleanValue("m_chartTitleDisplayedFlag",
                                                              false));
        }
    }
    
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

