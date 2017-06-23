
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

#define __CHART_TWO_OVERLAY_SET_ARRAY_DECLARE__
#include "ChartTwoOverlaySetArray.h"
#undef __CHART_TWO_OVERLAY_SET_ARRAY_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "ChartTwoOverlaySet.h"
#include "EventBrowserTabDelete.h"
#include "EventManager.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
using namespace caret;


    
/**
 * \class caret::ChartTwoOverlaySetArray 
 * \brief Maintains an array of chart overlay sets for use with a model
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *     Type of charts allowed in this overlay
 * @param name
 *    Name of model using this chart overlay set.  This name is displayed
 *    if there is an attempt to yoke models with incompatible chart overlays.
 */
ChartTwoOverlaySetArray::ChartTwoOverlaySetArray(const ChartTwoDataTypeEnum::Enum chartDataType,
                                           const AString& name)
: CaretObject(),
EventListenerInterface(),
SceneableInterface(),
m_name(name)
{
    m_chartOverlaySets.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    for (int32_t tabIndex = 0; tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; tabIndex++) {
        m_chartOverlaySets[tabIndex] = new ChartTwoOverlaySet(chartDataType,
                                                           name,
                                                           tabIndex);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
}

/**
 * Destructor.
 */
ChartTwoOverlaySetArray::~ChartTwoOverlaySetArray()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (std::vector<ChartTwoOverlaySet*>::iterator iter =m_chartOverlaySets.begin();
         iter !=m_chartOverlaySets.end();
         iter++) {
        delete *iter;
    }
    m_chartOverlaySets.clear();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoOverlaySetArray::toString() const
{
    return "ChartTwoOverlaySetArray";
}

/**
 * @return The number of chart overlay sets.
 */
int32_t
ChartTwoOverlaySetArray::getNumberOfChartOverlaySets()
{
    return m_chartOverlaySets.size();
}

/**
 * Get the chart overlay set at the given index.
 *
 * @param tabIndex
 *    Index of chart overlay tab.
 * @return
 *    Overlay set at given index.
 */
ChartTwoOverlaySet*
ChartTwoOverlaySetArray::getChartTwoOverlaySet(const int32_t tabIndex)
{
    CaretAssertVectorIndex(m_chartOverlaySets, tabIndex);
    
    return m_chartOverlaySets[tabIndex];
}

/**
 * Initialize the chart overlay selections.
 */
void
ChartTwoOverlaySetArray::initializeOverlaySelections()
{
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        CaretAssertVectorIndex(m_chartOverlaySets, iTab);
        m_chartOverlaySets[iTab]->initializeOverlays();
    }
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartTwoOverlaySetArray::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* deleteTabEvent = dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(deleteTabEvent);

        /*
         * Since tab is being deleted, reset any chart overlay yoking for the tab.
         */
        const int32_t tabIndex = deleteTabEvent->getBrowserTabIndex();
        if ((tabIndex > 0)
            && (tabIndex < getNumberOfChartOverlaySets())) {
           m_chartOverlaySets[tabIndex]->resetOverlayYokingToOff();
        }
        
        deleteTabEvent->setEventProcessed();
    }
}

/**
 * Copy the chart overlay set from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ChartTwoOverlaySetArray::copyChartOverlaySet(const int32_t sourceTabIndex,
                    const int32_t destinationTabIndex)
{
    CaretAssertVectorIndex(m_chartOverlaySets, sourceTabIndex);
    CaretAssertVectorIndex(m_chartOverlaySets, destinationTabIndex);
    
    const ChartTwoOverlaySet* sourceChartOverlaySet =m_chartOverlaySets[sourceTabIndex];
    ChartTwoOverlaySet* destinationChartOverlaySet =m_chartOverlaySets[destinationTabIndex];
    destinationChartOverlaySet->copyOverlaySet(sourceChartOverlaySet);
}

/**
 * Copy the chart overlay set cartesian axes from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ChartTwoOverlaySetArray::copyChartOverlaySetCartesianAxes(const int32_t sourceTabIndex,
                                                          const int32_t destinationTabIndex)
{
    CaretAssertVectorIndex(m_chartOverlaySets, sourceTabIndex);
    CaretAssertVectorIndex(m_chartOverlaySets, destinationTabIndex);
    
    const ChartTwoOverlaySet* sourceChartOverlaySet =m_chartOverlaySets[sourceTabIndex];
    ChartTwoOverlaySet* destinationChartOverlaySet =m_chartOverlaySets[destinationTabIndex];
    
    destinationChartOverlaySet->copyCartesianAxes(sourceChartOverlaySet);    
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
ChartTwoOverlaySetArray::saveToScene(const SceneAttributes* sceneAttributes,
                             const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoOverlaySetArray",
                                            1);
    
    std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    /*
     * Save overlay sets for tabs
     */
    SceneObjectMapIntegerKey* overlaySetMap = new SceneObjectMapIntegerKey("m_chartOverlaySetMAP",
                                                                            SceneObjectDataTypeEnum::SCENE_CLASS);
    
    for (const auto tabIndex : tabIndices) {
        overlaySetMap->addClass(tabIndex, m_chartOverlaySets[tabIndex]->saveToScene(sceneAttributes,
                                                                                    "m_chartOverlaySet"));
    }
    sceneClass->addChild(overlaySetMap);

    
    
    
//    const int32_t numOverlaySetsToSave = getNumberOfChartOverlaySets();
//    
//    std::vector<SceneClass*> overlaySetVector;
//    for (int i = 0; i < numOverlaySetsToSave; i++) {
//        overlaySetVector.push_back(m_chartOverlaySets[i]->saveToScene(sceneAttributes,
//                                                                      "m_chartOverlaySet"));
//    }
//    
//    SceneClassArray* overlaySetArray = new SceneClassArray("m_chartOverlaySets",
//                                                             overlaySetVector);
//    sceneClass->addChild(overlaySetArray);
    
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
ChartTwoOverlaySetArray::restoreFromScene(const SceneAttributes* sceneAttributes,
                                  const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const SceneObjectMapIntegerKey* overlaySetMap = sceneClass->getMapIntegerKey("m_chartOverlaySetMAP");
    if (overlaySetMap != NULL) {
        const std::vector<int32_t> tabIndices = overlaySetMap->getKeys();
        for (const auto tabIndex : tabIndices) {
            const SceneClass* sceneClass = overlaySetMap->classValue(tabIndex);
            CaretAssertVectorIndex(m_chartOverlaySets, tabIndex);
            m_chartOverlaySets[tabIndex]->restoreFromScene(sceneAttributes,
                                                           sceneClass);
        }
    }

    
//    const SceneClassArray* overlaySetArray = sceneClass->getClassArray("m_chartOverlaySets");
//    if (overlaySetArray != NULL) {
//        const int32_t numOverlaySets = std::min(overlaySetArray->getNumberOfArrayElements(),
//                                             (int32_t)BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
//        for (int32_t i = 0; i < numOverlaySets; i++) {
//            m_chartOverlaySets[i]->restoreFromScene(sceneAttributes,
//                                                    overlaySetArray->getClassAtIndex(i));
//        }
//    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

