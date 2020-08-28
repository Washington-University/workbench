
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
#include "ChartTwoCartesianOrientedAxes.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoTitle.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileLineLayerChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "EventBrowserTabGet.h"
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
ChartTwoOverlaySetInterface(),
m_chartDataType(chartDataType),
m_name(name),
m_tabIndex(tabIndex)
{
    m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    
    m_horizontalAxes.reset(new ChartTwoCartesianOrientedAxes(this,
                                                             ChartTwoAxisOrientationTypeEnum::HORIZONTAL));
    m_verticalAxes.reset(new ChartTwoCartesianOrientedAxes(this,
                                                             ChartTwoAxisOrientationTypeEnum::VERTICAL));

    ChartTwoCartesianAxis* chartAxisLeft   = m_verticalAxes->getLeftOrBottomAxis();
    ChartTwoCartesianAxis* chartAxisRight  = m_verticalAxes->getRightOrTopAxis();
    ChartTwoCartesianAxis* chartAxisBottom = m_horizontalAxes->getLeftOrBottomAxis();
    ChartTwoCartesianAxis* chartAxisTop    = m_horizontalAxes->getRightOrTopAxis();

    chartAxisLeft->setDisplayedByUser(true);
    chartAxisRight->setDisplayedByUser(false);
    chartAxisBottom->setDisplayedByUser(true);
    chartAxisTop->setDisplayedByUser(false);
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
        {
            m_horizontalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::DATA);
            m_verticalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::AUTO);
            chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisTop->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
        {
            chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            
            /*
             * X-axis for line series shows full extent of data
             */
            m_horizontalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::DATA);
            m_verticalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::AUTO);
            chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisTop->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
        {
            chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            
            /*
             * X-axis for line series shows full extent of data
             */
            m_horizontalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::DATA);
            m_verticalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::AUTO);
            chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisTop->setUnits(CaretUnitsTypeEnum::NONE);
        }
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            chartAxisLeft->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisRight->setUnits(CaretUnitsTypeEnum::NONE);
            
            /*
             * X- and Y-axis for line series shows full extent of data
             */
            m_horizontalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::DATA);
            m_verticalAxes->initializeScaleRangeMode(ChartTwoAxisScaleRangeModeEnum::DATA);
            chartAxisBottom->setUnits(CaretUnitsTypeEnum::NONE);
            chartAxisTop->setUnits(CaretUnitsTypeEnum::NONE);
            break;
    }
    
    m_title = std::unique_ptr<ChartTwoTitle>(new ChartTwoTitle());
    
    m_sceneAssistant  = new SceneClassAssistant();
    m_sceneAssistant->add("m_horizontalAxes",
                          "ChartTwoCartesianOrientedAxes",
                          m_horizontalAxes.get());
    m_sceneAssistant->add("m_verticalAxes",
                          "ChartTwoCartesianOrientedAxes",
                          m_verticalAxes.get());
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
    *m_horizontalAxes  = *overlaySet->m_horizontalAxes;
    *m_verticalAxes    = *overlaySet->m_verticalAxes;
    
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
    
    m_horizontalAxes->copyAxes(overlaySet->m_horizontalAxes.get());
    m_verticalAxes->copyAxes(overlaySet->m_verticalAxes.get());
    
    m_axisLineThickness = overlaySet->m_axisLineThickness;
}

/**
 * @return The chart data type for this chart overlay.
 */
ChartTwoDataTypeEnum::Enum
ChartTwoOverlaySet::getChartTwoDataType() const
{
    return m_chartDataType;
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
 * Assign to this overlay that is not used by any other overlays.
 * Note: The maximum number of overlay is greater than the number
 * of CaretColors so when many overlays are enabled, a unque
 * color may not be available.
 *
 * @param chartOverlay
 *    Overlay that is assigned a color not used by other overlays
 */
void
ChartTwoOverlaySet::assignUnusedColor(ChartTwoOverlay* chartOverlay)
{
    CaretAssert(chartOverlay);
    
    /*
     * Get colors used by other overlays
     */
    std::set<CaretColorEnum::Enum> usedColors;
    for (int32_t i = 0; i < m_numberOfDisplayedOverlays; i++) {
        const ChartTwoOverlay* cto = getOverlay(i);
        CaretAssert(cto);
        if (cto != chartOverlay) {
            usedColors.insert(cto->getLineLayerColor().getCaretColorEnum());
        }
    }
    
    /*
     * Assing a color not used by any other chart layers
     */
    std::vector<CaretColorEnum::Enum> allColors;
    CaretColorEnum::getColorEnumsNoBlackOrWhite(allColors);
    CaretAssert( ! allColors.empty());
    for (const auto color : allColors) {
        if (usedColors.find(color) == usedColors.end()) {
            CaretColor caretColor;
            caretColor.setCaretColorEnum(color);
            chartOverlay->setLineLayerColor(caretColor);
            break;
        }
    }
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
        
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex+1);
        m_overlays[overlayIndex]->copyData(m_overlays[overlayIndex+1].get());
        assignUnusedColor(m_overlays[overlayIndex].get());
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
        
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex+1);
        m_overlays[overlayIndex+1]->copyData(m_overlays[overlayIndex].get());
        assignUnusedColor(m_overlays[overlayIndex+1].get());
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
    
    const ChartTwoCompoundDataType* cdt = m_overlays[0]->getChartTwoCompoundDataType();
    
    for (int32_t i = 1; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->setChartTwoCompoundDataType(*cdt);
    }
    
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
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
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
                            minimumValueOut = std::min(minimumValueOut, boundingBox.getMinY());
                            maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxY());
                            break;
                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                            minimumValueOut = std::min(minimumValueOut, boundingBox.getMinY());
                            maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxY());
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
 * Get the minimum and maximum values for the given chart axis orientation
 * from the ENABLED overlays in this chart overlay set.
 *
 * @param axisOrientationType
 *    Location of axis.
 * @param minimumValueOut
 *    Output with minimum value for axis.
 * @param maximumValueOut
 *    Output with maximum value for axis.
 * @return
 *    True if output values are valid, else false.
 */
bool
ChartTwoOverlaySet::getDataRangeForAxisOrientation(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientationType,
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
                    switch (axisOrientationType) {
                        case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
                            minimumValueOut = std::min(minimumValueOut, boundingBox.getMinX());
                            maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxX());
                            break;
                        case ChartTwoAxisOrientationTypeEnum::VERTICAL:
                            minimumValueOut = std::min(minimumValueOut, boundingBox.getMinY());
                            maximumValueOut = std::max(maximumValueOut, boundingBox.getMaxY());
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
            axisSupportedFlag = true;
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
    
    float xMinBottom = 0.0f;
    float xMaxBottom = 0.0f;
    float yMinLeft = 0.0f;
    float yMaxLeft = 0.0f;
    
    if (getDataRangeForAxisOrientation(ChartTwoAxisOrientationTypeEnum::HORIZONTAL,
                                       xMinBottom,
                                       xMaxBottom)) {
        
        if (m_horizontalAxes->getLeftOrBottomAxis()->isDisplayedByUser()) {
            axesOut.push_back(m_horizontalAxes->getLeftOrBottomAxis());
        }
        if (m_horizontalAxes->getRightOrTopAxis()->isDisplayedByUser()) {
            axesOut.push_back(m_horizontalAxes->getRightOrTopAxis());
        }
    }
    if (getDataRangeForAxisOrientation(ChartTwoAxisOrientationTypeEnum::VERTICAL,
                                       yMinLeft,
                                       yMaxLeft)) {
        if (m_verticalAxes->getLeftOrBottomAxis()->isDisplayedByUser()) {
            axesOut.push_back(m_verticalAxes->getLeftOrBottomAxis());
        }
        if (m_verticalAxes->getRightOrTopAxis()->isDisplayedByUser()) {
            axesOut.push_back(m_verticalAxes->getRightOrTopAxis());
        }
    }
}

/**
 * @return Pointer to the horizontal axes
 */
ChartTwoCartesianOrientedAxes*
ChartTwoOverlaySet::getHorizontalAxes()
{
    return m_horizontalAxes.get();
}

/**
 * @return Pointer to the horizontal axes (const method)
 */
const ChartTwoCartesianOrientedAxes*
ChartTwoOverlaySet::getHorizontalAxes() const
{
    return m_horizontalAxes.get();
}

/**
 * @return Pointer to the vertical axes
 */
ChartTwoCartesianOrientedAxes*
ChartTwoOverlaySet::getVerticalAxes()
{
    return m_verticalAxes.get();
}

/**
 * @return Pointer to the vertical axes (const method)
 */
const ChartTwoCartesianOrientedAxes*
ChartTwoOverlaySet::getVerticalAxes() const
{
    return m_verticalAxes.get();
}

/**
 * Apply mouse translation to the current chart's axes
 * @param viewport
 *    Viewport containing chart
 * @param mouseDX
 *   The change in mouse X
 * @param mouseDY
 *   The change in mouse Y
 */
void
ChartTwoOverlaySet::applyMouseTranslation(const int32_t viewport[4],
                                          const float mouseDX,
                                          const float mouseDY)
{
    m_horizontalAxes->applyMouseTranslation(viewport,
                                            mouseDX,
                                            mouseDY);
    m_verticalAxes->applyMouseTranslation(viewport,
                                          mouseDX,
                                          mouseDY);
}

/**
 * Apply mouse scaling to the current chart's axes
 * @param viewport
 *    Viewport containing chart
 * @param mouseX
 *   The position of mouse along x-axis
 * @param mouseY
 *   The position of mouse along y-axis
 * @param mouseDY
 *   The change in mouse Y
 */
void
ChartTwoOverlaySet::applyMouseScaling(const int32_t viewport[4],
                                      const float mouseX,
                                      const float mouseY,
                                      const float mouseDY)
{
    m_horizontalAxes->applyMouseScaling(viewport,
                                        mouseX,
                                        mouseDY);
    m_verticalAxes->applyMouseScaling(viewport,
                                      mouseY,
                                      mouseDY);
}

/**
 * Apply chart two bounds selection as user drags the mouse
 * @param viewport
 * Chart  viewport
 * @param x1
 * X from first pair of coordinates
 * @param y1
 * Y from first pair of coordinates
 * @param x2
 * X from second pair of coordinates
 * @param y2
 * Y from second pair of coordinates
 */
void
ChartTwoOverlaySet::applyChartTwoAxesBoundSelection(const int32_t viewport[4],
                                                    const int32_t x1,
                                                    const int32_t y1,
                                                    const int32_t x2,
                                                    const int32_t y2)
{
    setChartTwoAxesBoundSelection(viewport,
                                  x1, y1, x2, y2);
    m_chartSelectionBoundsValid = true;
}

/**
 * Finalize chart two bounds selection to set the bounds of the chart
 * @param viewport
 * Chart viewport
 * @param x1
 * X from first pair of coordinates
 * @param y1
 * Y from first pair of coordinates
 * @param x2
 * X from second pair of coordinates
 * @param y2
 * Y from second pair of coordinates
 */
void
ChartTwoOverlaySet::finalizeChartTwoAxesBoundSelection(const int32_t viewport[4],
                                                       const int32_t x1,
                                                       const int32_t y1,
                                                       const int32_t x2,
                                                       const int32_t y2)
{
    if ( ! m_chartSelectionBoundsValid) {
        return;
    }
    
    /*
     * x2 and y2 are coordinates of where mouse button was released.
     * Test to verify this coordinate is inside the chart axes.
     * User can cancel this operation by releasing the mouse outside
     * of the axes.
     */
    const int32_t extraPixels(3);
    const int32_t vpMinX(viewport[0] - extraPixels);
    const int32_t vpMaxX(viewport[0] + viewport[2] + extraPixels);
    const int32_t vpMinY(viewport[1] - extraPixels);
    const int32_t vpMaxY(viewport[1] + viewport[3] + extraPixels);
    if ((x2 >= vpMinX)
        && (x2 <= vpMaxX)
        && (y2 >= vpMinY)
        && (y2 <= vpMaxY)) {
        setChartTwoAxesBoundSelection(viewport,
                                      x1, y1, x2, y2);
        const float minX(m_chartSelectionBounds[0]);
        const float minY(m_chartSelectionBounds[1]);
        const float maxX(m_chartSelectionBounds[2]);
        const float maxY(m_chartSelectionBounds[3]);
        
        if ((minX < maxX)
            && (minY < maxY)) {
            if (m_horizontalAxes->isTransformationEnabled()) {
                m_horizontalAxes->setUserScaleMinimumValueFromGUI(minX);
                m_horizontalAxes->setUserScaleMaximumValueFromGUI(maxX);
            }
            if (m_verticalAxes->isTransformationEnabled()) {
                m_verticalAxes->setUserScaleMinimumValueFromGUI(minY);
                m_verticalAxes->setUserScaleMaximumValueFromGUI(maxY);
            }
        }
    }
    else {
        /*
         * Nothing, outside chart axes
         */
    }
    
    m_chartSelectionBoundsValid = false;
}

void
ChartTwoOverlaySet::setChartTwoAxesBoundSelection(const int32_t viewport[4],
                                   const int32_t x1,
                                   const int32_t y1,
                                   const int32_t x2,
                                   const int32_t y2)
{
    const int32_t vpX(viewport[0]);
    const int32_t vpY(viewport[1]);
    const int32_t viewportWidth(viewport[2]);
    const int32_t viewportHeight(viewport[3]);
    float xMin(m_horizontalAxes->getAxesCoordinateFromViewportCoordinate(viewportWidth,
                                                                         viewportHeight,
                                                                         x1 - vpX));
    float xMax(m_horizontalAxes->getAxesCoordinateFromViewportCoordinate(viewportWidth,
                                                                         viewportHeight,
                                                                         x2 - vpX));
    float yMin(m_verticalAxes->getAxesCoordinateFromViewportCoordinate(viewportWidth,
                                                                       viewportHeight,
                                                                       y1 - vpY));
    float yMax(m_verticalAxes->getAxesCoordinateFromViewportCoordinate(viewportWidth,
                                                                       viewportHeight,
                                                                       y2 - vpY));
    
    if (xMax < xMin) std::swap(xMax, xMin);
    if (yMax < yMin) std::swap(yMax, yMin);
    
    m_chartSelectionBounds[0] = xMin;
    m_chartSelectionBounds[1] = yMin;
    m_chartSelectionBounds[2] = xMax;
    m_chartSelectionBounds[3] = yMax;
}

/**
 * Get chart two bounds selection as user drags the mouse
 * @param minX
 * X from first pair of coordinates
 * @param minY
 * Y from first pair of coordinates
 * @param maxX
 * X from second pair of coordinates
 * @param maxY
 * Y from second pair of coordinates
 * @return True if valid
 */
bool
ChartTwoOverlaySet::getChartSelectionBounds(float& minX,
                                            float& minY,
                                            float& maxX,
                                            float& maxY) const
{
    minX = m_chartSelectionBounds[0];
    minY = m_chartSelectionBounds[1];
    maxX = m_chartSelectionBounds[2];
    maxY = m_chartSelectionBounds[3];
    
    return m_chartSelectionBoundsValid;
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
                        label = mapFile->getChartingDelegate()->getHistogramCharting()->getBottomTopAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        label = mapFile->getChartingDelegate()->getHistogramCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        label = mapFile->getChartingDelegate()->getHistogramCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        label = mapFile->getChartingDelegate()->getHistogramCharting()->getBottomTopAxisTitle();
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        label = mapFile->getChartingDelegate()->getLineLayerCharting()->getBottomTopAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        label = mapFile->getChartingDelegate()->getLineLayerCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        label = mapFile->getChartingDelegate()->getLineLayerCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        label = mapFile->getChartingDelegate()->getLineLayerCharting()->getBottomTopAxisTitle();
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        label = mapFile->getChartingDelegate()->getLineSeriesCharting()->getBottomTopAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        label = mapFile->getChartingDelegate()->getLineSeriesCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        label = mapFile->getChartingDelegate()->getLineSeriesCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        label = mapFile->getChartingDelegate()->getLineSeriesCharting()->getBottomTopAxisTitle();
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        label = mapFile->getChartingDelegate()->getMatrixCharting()->getBottomTopAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        label = mapFile->getChartingDelegate()->getMatrixCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        label = mapFile->getChartingDelegate()->getMatrixCharting()->getLeftRightAxisTitle();
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        label = mapFile->getChartingDelegate()->getMatrixCharting()->getBottomTopAxisTitle();
                        break;
                }
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
                        mapFile->getChartingDelegate()->getHistogramCharting()->setBottomTopAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        mapFile->getChartingDelegate()->getHistogramCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        mapFile->getChartingDelegate()->getHistogramCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        mapFile->getChartingDelegate()->getHistogramCharting()->setBottomTopAxisTitle(label);
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        mapFile->getChartingDelegate()->getLineLayerCharting()->setBottomTopAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        mapFile->getChartingDelegate()->getLineLayerCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        mapFile->getChartingDelegate()->getLineLayerCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        mapFile->getChartingDelegate()->getLineLayerCharting()->setBottomTopAxisTitle(label);
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        mapFile->getChartingDelegate()->getLineSeriesCharting()->setBottomTopAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        mapFile->getChartingDelegate()->getLineSeriesCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        mapFile->getChartingDelegate()->getLineSeriesCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        mapFile->getChartingDelegate()->getLineSeriesCharting()->setBottomTopAxisTitle(label);
                        break;
                }
                break;
            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                switch (axis->getAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        mapFile->getChartingDelegate()->getMatrixCharting()->setBottomTopAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        mapFile->getChartingDelegate()->getMatrixCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        mapFile->getChartingDelegate()->getMatrixCharting()->setLeftRightAxisTitle(label);
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        mapFile->getChartingDelegate()->getMatrixCharting()->setBottomTopAxisTitle(label);
                        break;
                }
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
 * Increment (decrement if negative) any line chart layer points the are active
 * @param incrementValue
 * Amount to advance point index.
 */
void
ChartTwoOverlaySet::incrementOverlayActiveLineChartPoint(const int32_t incrementValue)
{
    const int32_t numOverlays = static_cast<int32_t>(m_overlays.size());
    for (int32_t i = 0; i < numOverlays; i++) {
        if (m_overlays[i] != NULL) {
            switch (m_overlays[i]->getLineChartActiveMode()) {
                case ChartTwoOverlayActiveModeEnum::ACTIVE:
                    m_overlays[i]->incrementSelectedLineChartPointIndex(incrementValue);
                    break;
                case ChartTwoOverlayActiveModeEnum::OFF:
                    break;
                case ChartTwoOverlayActiveModeEnum::ON:
                    break;
            }
        }
    }
}

/**
 * Select or deselect the active line chart layers
 * @param chartTwoOverlay
 *    If non NULL, set the given layer as the active layer and change any other active layers to on.
 *    If NULL, change any active layer to on (no layers are active)
 * @param lineSegmentPointIndex
 *    Index of selected line segment point
 */
void
ChartTwoOverlaySet::selectOverlayActiveLineChart(ChartTwoOverlay* chartTwoOverlay,
                                                 const int32_t lineSegmentPointIndex)
{
    const int32_t numOverlays = static_cast<int32_t>(m_overlays.size());
    for (int32_t i = 0; i < numOverlays; i++) {
        if (m_overlays[i] != NULL) {
            /*
             * Change any active overlays to on
             */
            switch (m_overlays[i]->getLineChartActiveMode()) {
                case ChartTwoOverlayActiveModeEnum::ACTIVE:
                    m_overlays[i]->setLineChartActiveMode(ChartTwoOverlayActiveModeEnum::ON);
                    break;
                case ChartTwoOverlayActiveModeEnum::OFF:
                    break;
                case ChartTwoOverlayActiveModeEnum::ON:
                    break;
            }

            /*
             * Make given chart overlay active
             */
            if (chartTwoOverlay == m_overlays[i].get()) {
                chartTwoOverlay->setLineChartActiveMode(ChartTwoOverlayActiveModeEnum::ACTIVE);
                chartTwoOverlay->setSelectedLineChartPointIndex(lineSegmentPointIndex);
            }
        }
    }
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
                                            2);
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
 * Restore the range scale from an old scene
 * @param axisOrientationType
 *    The axis orientation
 * @param leftOrBottomAxis
 *    The left or bottom axis
 * @param rightOrTopAxis
 *    The right or top axis
 */
void
ChartTwoOverlaySet::updateRangeScaleFromVersionOneScene(const ChartTwoAxisOrientationTypeEnum::Enum axisOrientationType,
                                                        ChartTwoCartesianAxis* leftOrBottomAxis,
                                                        ChartTwoCartesianAxis* rightOrTopAxis)
{
    ChartTwoAxisScaleRangeModeEnum::Enum rangeMode = ChartTwoAxisScaleRangeModeEnum::AUTO;
    float minRange(0.0);
    float maxRange(0.0);
    
    /*
     * Restore from left/bottom first
     */
    bool leftBottomValidFlag(false);
    if (leftOrBottomAxis != NULL) {
        /*
         * Neither axis may be on, so default to left's range
         */
        rangeMode = leftOrBottomAxis->getSceneScaleRangeMode();
        minRange  = leftOrBottomAxis->getSceneUserScaleMinimumValue();
        maxRange  = leftOrBottomAxis->getSceneUserScaleMaximumValue();
        if (leftOrBottomAxis->isDisplayedByUser()) {
            /*
             * Use the left axis for range
             */
            leftBottomValidFlag = true;
        }
    }
    
    /*
     * If left NOT valid
     */
    if ( ! leftBottomValidFlag) {
        /*
         * Only use right axis if left is not used (NULL)
         * or left axis is off and right is on
         */
        if (rightOrTopAxis != NULL) {
            if (rightOrTopAxis->isDisplayedByUser()
                || (leftOrBottomAxis == NULL)) {
                rangeMode = rightOrTopAxis->getSceneScaleRangeMode();
                minRange  = rightOrTopAxis->getSceneUserScaleMinimumValue();
                maxRange  = rightOrTopAxis->getSceneUserScaleMaximumValue();
            }
        }
    }
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            /*
             * Use 'data' for range for matrices in version one scenes
             */
            rangeMode = ChartTwoAxisScaleRangeModeEnum::DATA;
            break;
    }
    
    switch (axisOrientationType) {
        case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
            m_horizontalAxes->setRangeModeAndUserScaleFromVersionOneScene(rangeMode,
                                                                          minRange,
                                                                          maxRange);
            break;
        case ChartTwoAxisOrientationTypeEnum::VERTICAL:
            m_verticalAxes->setRangeModeAndUserScaleFromVersionOneScene(rangeMode,
                                                                          minRange,
                                                                          maxRange);
            break;
    }
    
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            /*
             * Version one matrices did not have axes
             */
            switch (axisOrientationType) {
                case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
                    m_horizontalAxes->getLeftOrBottomAxis()->setDisplayedByUser(false);
                    m_horizontalAxes->getRightOrTopAxis()->setDisplayedByUser(false);
                    break;
                case ChartTwoAxisOrientationTypeEnum::VERTICAL:
                    m_verticalAxes->getLeftOrBottomAxis()->setDisplayedByUser(false);
                    m_verticalAxes->getRightOrTopAxis()->setDisplayedByUser(false);
                    break;
            }
            break;
    }
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
    m_verticalAxes->reset();
    m_horizontalAxes->reset();
    

    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    if (sceneClass->getVersionNumber() >= 2) {
    }
    else {
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

    /**
     * For version 1 need use obsolete axis location (right or left) to set display
     * status of left and right axes
     */
    if (sceneClass->getVersionNumber() == 1) {
        bool leftAxisOnFlag(false);
        bool rightAxisOnFlag(false);
        for (int32_t i = 0; i < getNumberOfDisplayedOverlays(); i++) {
            const ChartTwoOverlay* overlay = getOverlay(i);
            if (overlay->isEnabled()) {
                switch (overlay->getSceneCartesianVerticalAxisLocation()) {
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                        leftAxisOnFlag = true;
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                        rightAxisOnFlag = true;
                        break;
                    case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                        break;
                }
            }
        }
        
        /*
         * Restore the axes from the older separate axes
         */
        ChartTwoCartesianAxis* rightAxis(NULL);
        const SceneClass* v1RightAxis = sceneClass->getClass("m_chartAxisRight");
        if (v1RightAxis != NULL) {
            rightAxis = m_verticalAxes->getRightOrTopAxis();
            rightAxis->restoreFromScene(sceneAttributes,
                                        v1RightAxis);
        }
        
        const SceneClass* v1LeftAxis = sceneClass->getClass("m_chartAxisLeft");
        ChartTwoCartesianAxis* leftAxis(NULL);
        if (v1LeftAxis != NULL) {
            leftAxis = m_verticalAxes->getLeftOrBottomAxis();
            leftAxis->restoreFromScene(sceneAttributes,
                                       v1LeftAxis);
        }
        
        /*
         * Must do after restoring axes
         */
        m_verticalAxes->getLeftOrBottomAxis()->setDisplayedByUser(leftAxisOnFlag);
        m_verticalAxes->getRightOrTopAxis()->setDisplayedByUser(rightAxisOnFlag);
        if (leftAxisOnFlag
            && (leftAxis != NULL)) {
            updateRangeScaleFromVersionOneScene(ChartTwoAxisOrientationTypeEnum::VERTICAL,
                                                leftAxis,
                                                NULL); /* right axis */
        }
        else if (rightAxisOnFlag
                 && (rightAxis != NULL)) {
            updateRangeScaleFromVersionOneScene(ChartTwoAxisOrientationTypeEnum::VERTICAL,
                                                NULL,  /* left axis */
                                                rightAxis);
        }
        
        const SceneClass* v1TopAxis = sceneClass->getClass("m_chartAxisTop");
        ChartTwoCartesianAxis* topAxis(NULL);
        if (v1TopAxis != NULL) {
            topAxis = m_horizontalAxes->getRightOrTopAxis();
            topAxis->restoreFromScene(sceneAttributes,
                                      v1TopAxis);
        }
        
        /*
         * There was no top axis option in version 1 scenes
         * so turn top axis off
         */
        m_horizontalAxes->getRightOrTopAxis()->setDisplayedByUser(false);
        
        const SceneClass* v1BottomAxis = sceneClass->getClass("m_chartAxisBottom");
        ChartTwoCartesianAxis* bottomAxis(NULL);
        if (v1BottomAxis != NULL) {
            bottomAxis = m_horizontalAxes->getLeftOrBottomAxis();
            bottomAxis->restoreFromScene(sceneAttributes,
                                         v1BottomAxis);
        }
        
        updateRangeScaleFromVersionOneScene(ChartTwoAxisOrientationTypeEnum::HORIZONTAL,
                                            bottomAxis,
                                            topAxis);

    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

