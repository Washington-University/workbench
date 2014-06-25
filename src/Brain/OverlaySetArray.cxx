
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

#define __OVERLAY_SET_ARRAY_DECLARE__
#include "OverlaySetArray.h"
#undef __OVERLAY_SET_ARRAY_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "EventBrowserTabDelete.h"
#include "EventManager.h"
#include "OverlaySet.h"

using namespace caret;


    
/**
 * \class caret::OverlaySetArray 
 * \brief Maintains an array of overlay sets for use with a model
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param includeSurfaceStructures
 *    Surface structures for files available in this overlay.
 * @param includeVolumeFiles
 *    Include (or not) volume files.
 * @param name
 *    Name of model using this overlay set.  This name is displayed
 *    if there is an attempt to yoke models with incompatible overlays.
 */
OverlaySetArray::OverlaySetArray(const std::vector<StructureEnum::Enum>& includeSurfaceStructures,
                const Overlay::IncludeVolumeFiles includeVolumeFiles,
                const AString& name)
: CaretObject(),
m_name(name)
{
    m_overlaySets.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySets[i] = new OverlaySet(name,
                                          i,
                                          includeSurfaceStructures,
                                          includeVolumeFiles);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
}

/**
 * Destructor.
 */
OverlaySetArray::~OverlaySetArray()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (std::vector<OverlaySet*>::iterator iter = m_overlaySets.begin();
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
OverlaySetArray::toString() const
{
    return "OverlaySetArray";
}

/**
 * @return The number of overlay sets.
 */
int32_t
OverlaySetArray::getNumberOfOverlaySets()
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
OverlaySet*
OverlaySetArray::getOverlaySet(const int32_t indx)
{
    CaretAssertVectorIndex(m_overlaySets, indx);
    
    return m_overlaySets[indx];
}

/**
 * Initialize the overlay selections.
 */
void
OverlaySetArray::initializeOverlaySelections()
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
OverlaySetArray::receiveEvent(Event* event)
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
OverlaySetArray::copyOverlaySet(const int32_t sourceTabIndex,
                    const int32_t destinationTabIndex)
{
    CaretAssertVectorIndex(m_overlaySets, sourceTabIndex);
    CaretAssertVectorIndex(m_overlaySets, destinationTabIndex);
    
    const OverlaySet* sourceOverlaySet = m_overlaySets[sourceTabIndex];
    OverlaySet* destinationOverlaySet = m_overlaySets[destinationTabIndex];
    destinationOverlaySet->copyOverlaySet(sourceOverlaySet);
}

