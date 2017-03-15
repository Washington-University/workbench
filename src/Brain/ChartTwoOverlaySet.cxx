
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

#include "CaretAssert.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "EventManager.h"
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
    
    m_chartAxisLeft->setVisible(false);
    m_chartAxisRight->setVisible(false);
    m_chartAxisBottom->setVisible(false);
    switch (m_chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            m_chartAxisLeft->setVisible(true);
            m_chartAxisLeft->setLabelText("Counts");
            m_chartAxisLeft->setUnits(ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            m_chartAxisRight->setVisible(false);
            m_chartAxisRight->setLabelText("Counts");
            m_chartAxisRight->setUnits(ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            m_chartAxisBottom->setVisible(true);
            m_chartAxisBottom->setLabelText("Data");
            m_chartAxisBottom->setUnits(ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            m_chartAxisLeft->setVisible(true);
            m_chartAxisBottom->setVisible(true);
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
        m_overlays[i] = new ChartTwoOverlay(this,
                                         m_chartDataType,
                                         i);
    }
//    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
//    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
}

/**
 * Destructor.
 */
ChartTwoOverlaySet::~ChartTwoOverlaySet()
{
    EventManager::get()->removeAllEventsFromListener(this);
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        delete m_overlays[i];
    }
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
    m_numberOfDisplayedOverlays = overlaySet->m_numberOfDisplayedOverlays;
}

/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
ChartTwoOverlay*
ChartTwoOverlaySet::getPrimaryOverlay()
{
    return m_overlays[0];
}

/**
 * Get the overlay at the specified index.
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
    return m_overlays[overlayNumber];
}

/**
 * Get the overlay at the specified index.
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
            
//            descriptionOut.addLine("Overlay "
//                                   + AString::number(i + 1)
//                                   + ": ");
            
            descriptionOut.pushIndentation();
            getOverlay(i)->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
            
            descriptionOut.popIndentation();
        }
    }
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
 * Sets the number of displayed overlays.
 * @param numberOfDisplayedOverlays
 *   Number of overlays for display.
 */
void
ChartTwoOverlaySet::setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays)
{
    const int32_t oldNumberOfDisplayedOverlays = m_numberOfDisplayedOverlays;
    m_numberOfDisplayedOverlays = numberOfDisplayedOverlays;
    if (m_numberOfDisplayedOverlays < BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    }
    if (m_numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
    
    /*
     * If one overlay added (probably through GUI),
     * shift all overlays down one position so that
     * new overlay appears at the top
     */
    const int32_t numberOfOverlaysAdded = m_numberOfDisplayedOverlays - oldNumberOfDisplayedOverlays;
    if (numberOfOverlaysAdded == 1) {
        for (int32_t i = (m_numberOfDisplayedOverlays - 1); i >= 0; i--) {
            moveDisplayedOverlayDown(i);
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
            m_overlays[i]->copyData(m_overlays[i+1]);
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
        m_overlays[overlayIndex]->swapData(m_overlays[overlayIndex - 1]);
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
        m_overlays[overlayIndex]->swapData(m_overlays[nextOverlayIndex]);
    }
}

/**
 * Initialize the overlays.
 */
void
ChartTwoOverlaySet::initializeOverlays()
{
//    bool isMatchToVolumeUnderlay = false;
//    bool isMatchToVolumeOverlays = false;
//    
//    switch (m_includeVolumeFiles) {
//        case Overlay::INCLUDE_VOLUME_FILES_NO:
//            break;
//        case Overlay::INCLUDE_VOLUME_FILES_YES:
//            /*
//             * If no surface structures, then it must be volume slice view
//             * so allow volumes to be in the overlays.
//             */
//            if (m_includeSurfaceStructures.empty()) {
//                isMatchToVolumeOverlays = true;
//            }
//            isMatchToVolumeUnderlay = true;
//            break;
//    }
//    
//    /*
//     * Underlays consist of anatomical type data
//     */
//    std::vector<CaretMappableDataFile*> underlayMapFiles;
//    std::vector<int32_t> underlayMapIndices;
//    findUnderlayFiles(m_includeSurfaceStructures,
//                      isMatchToVolumeUnderlay,
//                      underlayMapFiles,
//                      underlayMapIndices);
//    
//    /*
//     * Middle layers are Cifti labels or Gifti Labels
//     * that do not contain shape data
//     */
//    std::vector<CaretMappableDataFile*> middleLayerMapFiles;
//    std::vector<int32_t> middleLayerMapIndices;
//    findMiddleLayerFiles(m_includeSurfaceStructures,
//                         isMatchToVolumeOverlays,
//                         middleLayerMapFiles,
//                         middleLayerMapIndices);
//    
//    /*
//     * Overlays consist of Cifti scalars or Gifti Metric
//     */
//    std::vector<CaretMappableDataFile*> overlayMapFiles;
//    std::vector<int32_t> overlayMapIndices;
//    findOverlayFiles(m_includeSurfaceStructures,
//                     isMatchToVolumeOverlays,
//                     overlayMapFiles,
//                     overlayMapIndices);
//    
//    const int32_t numberOfUnderlayFiles = static_cast<int32_t>(underlayMapFiles.size());
//    
//    /*
//     * Number of overlay that are displayed.
//     */
//    const int32_t numberOfDisplayedOverlays = getNumberOfDisplayedOverlays();
//    
//    
//    /*
//     * Track overlay that were initialized
//     */
//    std::vector<bool> overlayInitializedFlag(numberOfDisplayedOverlays,
//                                             false);
//    
//    /*
//     * Put in the shape files at the bottom
//     * Note that highest overlay index is bottom
//     */
//    int32_t overlayIndexForUnderlay = (numberOfDisplayedOverlays - 1);
//    for (int32_t underlayFileIndex = 0; underlayFileIndex < numberOfUnderlayFiles; underlayFileIndex++) {
//        if (overlayIndexForUnderlay >= 0) {
//            Overlay* overlay = getOverlay(overlayIndexForUnderlay);
//            overlay->setSelectionData(underlayMapFiles[underlayFileIndex],
//                                      underlayMapIndices[underlayFileIndex]);
//            overlayInitializedFlag[overlayIndexForUnderlay] = true;
//            overlayIndexForUnderlay--;
//        }
//        else {
//            break;
//        }
//    }
//    
//    /*
//     * Combine overlay and middle layer files
//     */
//    std::vector<CaretMappableDataFile*> upperLayerFiles;
//    std::vector<int32_t> upperLayerIndices;
//    upperLayerFiles.insert(upperLayerFiles.end(),
//                           overlayMapFiles.begin(),
//                           overlayMapFiles.end());
//    upperLayerIndices.insert(upperLayerIndices.end(),
//                             overlayMapIndices.begin(),
//                             overlayMapIndices.end());
//    upperLayerFiles.insert(upperLayerFiles.end(),
//                           middleLayerMapFiles.begin(),
//                           middleLayerMapFiles.end());
//    upperLayerIndices.insert(upperLayerIndices.end(),
//                             middleLayerMapIndices.begin(),
//                             middleLayerMapIndices.end());
//    CaretAssert(upperLayerFiles.size() == upperLayerIndices.size());
//    
//    const int32_t numberOfUpperFiles = static_cast<int32_t>(upperLayerFiles.size());
//    
//    /*
//     * Put in overlay and middle layer files
//     */
//    for (int32_t upperFileIndex = 0; upperFileIndex < numberOfUpperFiles; upperFileIndex++) {
//        /*
//         * Find available overlay
//         */
//        int32_t upperLayerOverlayIndex = -1;
//        for (int32_t overlayIndex = 0; overlayIndex < numberOfDisplayedOverlays; overlayIndex++) {
//            if (overlayInitializedFlag[overlayIndex] == false) {
//                upperLayerOverlayIndex = overlayIndex;
//                break;
//            }
//        }
//        
//        if (upperLayerOverlayIndex >= 0) {
//            Overlay* upperLayerOverlay = getOverlay(upperLayerOverlayIndex);
//            upperLayerOverlay->setSelectionData(upperLayerFiles[upperFileIndex],
//                                                upperLayerIndices[upperFileIndex]);
//            overlayInitializedFlag[upperLayerOverlayIndex] = true;
//        }
//        else {
//            break;
//        }
//    }
//    
//    /*
//     * Disable overlays that were not initialized
//     */
//    for (int32_t i = 0; i < numberOfDisplayedOverlays; i++) {
//        CaretAssertVectorIndex(overlayInitializedFlag, i);
//        getOverlay(i)->setEnabled(overlayInitializedFlag[i]);
//    }
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

    
    
    
//    std::cout << qPrintable("First Chart Overlay in tab "
//                            + AString::number(m_tabIndex + 1)
//                            + ":\n"
//                            +  cdt.toString()) << std::endl;
    
    for (int32_t i = 1; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
        m_overlays[i]->setChartTwoCompoundDataType(cdt);
//        if (m_overlays[i]->isEnabled()) {
//            std::cout << "   Overlay " << i << qPrintable(m_overlays[i]->toString()) << std::endl;
//        }
    }
    
    PlainTextStringBuilder description;
    getDescriptionOfContent(description);
    //std::cout << "First Overlay Changed: " << qPrintable(description.getText()) << std::endl;
    
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
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * @return The chart left-axis.
 */
ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisLeft()
{
    return m_chartAxisLeft.get();
}

/**
 * @return The chart left-axis (const method)
 */
const ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisLeft() const
{
    return m_chartAxisLeft.get();
}

/**
 * @return The chart right-axis.
 */
ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisRight()
{
    return m_chartAxisRight.get();
}

/**
 * @return The chart right-axis (const method)
 */
const ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisRight() const
{
    return m_chartAxisRight.get();
}

/**
 * @return The chart bottom-axis.
 */
ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisBottom()
{
    return m_chartAxisBottom.get();
}

std::vector<ChartTwoCartesianAxis*>
ChartTwoOverlaySet::getDisplayedChartAxes() const
{
    std::vector<ChartTwoCartesianAxis*> axes;
    
    if (m_chartAxisBottom->isVisible()) {
        axes.push_back(m_chartAxisBottom.get());
    }
    if (m_chartAxisLeft->isVisible()) {
        axes.push_back(m_chartAxisLeft.get());
    }
    if (m_chartAxisRight->isVisible()) {
        axes.push_back(m_chartAxisRight.get());
    }
    
    return axes;
}


/**
 * @return The chart bottom-axis (const method)
 */
const ChartTwoCartesianAxis*
ChartTwoOverlaySet::getChartAxisBottom() const
{
    return m_chartAxisBottom.get();
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

