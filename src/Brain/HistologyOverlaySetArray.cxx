
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

#define __HISTOLOGY_OVERLAY_SET_ARRAY_DECLARE__
#include "HistologyOverlaySetArray.h"
#undef __HISTOLOGY_OVERLAY_SET_ARRAY_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "EventBrowserTabDelete.h"
#include "EventManager.h"
#include "HistologyOverlaySet.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::OverlaySetArray 
 * \brief Maintains an array of overlay sets for use with a model
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param name
 *    Name of model using this overlay set.  This name is displayed
 *    if there is an attempt to yoke models with incompatible overlays.
 */
HistologyOverlaySetArray::HistologyOverlaySetArray(const AString& name)
: CaretObject(),
m_name(name)
{
    m_overlaySets.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySets[i] = new HistologyOverlaySet(name,
                                               i);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
}

/**
 * Destructor.
 */
HistologyOverlaySetArray::~HistologyOverlaySetArray()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (std::vector<HistologyOverlaySet*>::iterator iter = m_overlaySets.begin();
         iter != m_overlaySets.end();
         iter++) {
        delete *iter;
    }
    m_overlaySets.clear();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologyOverlaySetArray::toString() const
{
    return "OverlaySetArray";
}

/**
 * @return The number of overlay sets.
 */
int32_t
HistologyOverlaySetArray::getNumberOfOverlaySets()
{
    return m_overlaySets.size();
}

/**
 * Get the overlay set at the given index.
 *
 * @param indx
 *    Index of overlay set.
 * @return
 *    Overlay set at given index.
 */
HistologyOverlaySet*
HistologyOverlaySetArray::getOverlaySet(const int32_t indx)
{
    CaretAssertVectorIndex(m_overlaySets, indx);
    
    return m_overlaySets[indx];
}

/**
 * Initialize the overlay selections.
 */
void
HistologyOverlaySetArray::initializeOverlaySelections()
{
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        CaretAssertVectorIndex(m_overlaySets, iTab);
        m_overlaySets[iTab]->initializeOverlays();
    }
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
HistologyOverlaySetArray::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* deleteTabEvent = dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(deleteTabEvent);

        /*
         * Since tab is being deleted, reset any overlay yoking for the tab.
         */
        const int32_t tabIndex = deleteTabEvent->getBrowserTabIndex();
        if ((tabIndex > 0)
            && (tabIndex < getNumberOfOverlaySets())) {
            m_overlaySets[tabIndex]->resetOverlayYokingToOff();
        }
        
        deleteTabEvent->setEventProcessed();
    }
}

/**
 * Copy the overlay set from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
HistologyOverlaySetArray::copyOverlaySet(const int32_t sourceTabIndex,
                    const int32_t destinationTabIndex)
{
    CaretAssertVectorIndex(m_overlaySets, sourceTabIndex);
    CaretAssertVectorIndex(m_overlaySets, destinationTabIndex);
    
    const HistologyOverlaySet* sourceOverlaySet = m_overlaySets[sourceTabIndex];
    HistologyOverlaySet* destinationOverlaySet = m_overlaySets[destinationTabIndex];
    destinationOverlaySet->copyHistologyOverlaySet(sourceOverlaySet);
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param tabIndices
 *    Tab indices that are valid.
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
HistologyOverlaySetArray::saveTabIndicesToScene(const std::vector<int32_t>& tabIndices,
                                               const SceneAttributes* sceneAttributes,
                                               const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologyOverlaySetArray",
                                            1);
    
    /*
     * Save overlay sets for tabs
     */
    SceneObjectMapIntegerKey* overlaySetMap = new SceneObjectMapIntegerKey("m_histologyOverlaySetMAP",
                                                                           SceneObjectDataTypeEnum::SCENE_CLASS);
    
    for (const auto tabIndex : tabIndices) {
        overlaySetMap->addClass(tabIndex, m_overlaySets[tabIndex]->saveToScene(sceneAttributes,
                                                                                    "m_histologyOverlaySet"));
    }
    sceneClass->addChild(overlaySetMap);

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
HistologyOverlaySetArray::restoreFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const SceneObjectMapIntegerKey* overlaySetMap = sceneClass->getMapIntegerKey("m_histologyOverlaySetMAP");
    if (overlaySetMap != NULL) {
        const std::vector<int32_t> tabIndices = overlaySetMap->getKeys();
        for (const auto tabIndex : tabIndices) {
            const SceneClass* sceneClass = overlaySetMap->classValue(tabIndex);
            CaretAssertVectorIndex(m_overlaySets, tabIndex);
            m_overlaySets[tabIndex]->restoreFromScene(sceneAttributes,
                                                           sceneClass);
        }
    }
}
