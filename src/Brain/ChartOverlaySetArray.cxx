
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
#include "ChartOverlaySetArray.h"
#undef __OVERLAY_SET_ARRAY_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "EventBrowserTabDelete.h"
#include "EventManager.h"
#include "ChartOverlaySet.h"

using namespace caret;


    
/**
 * \class caret::ChartOverlaySetArray 
 * \brief Maintains an array of chart overlay sets for use with a model
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param name
 *    Name of model using this chart overlay set.  This name is displayed
 *    if there is an attempt to yoke models with incompatible chart overlays.
 */
ChartOverlaySetArray::ChartOverlaySetArray(const AString& name)
: CaretObject(),
m_name(name)
{
   m_chartOverlaySets.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartOverlaySets[i] = new ChartOverlaySet(ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES,
                                                    name,
                                                    i);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
}

/**
 * Destructor.
 */
ChartOverlaySetArray::~ChartOverlaySetArray()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (std::vector<ChartOverlaySet*>::iterator iter =m_chartOverlaySets.begin();
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
ChartOverlaySetArray::toString() const
{
    return "ChartOverlaySetArray";
}

/**
 * @return The number of chart overlay sets.
 */
int32_t
ChartOverlaySetArray::getNumberOfChartOverlaySets()
{
    return m_chartOverlaySets.size();
}

/**
 * Get the chart overlay set at the given index.
 *
 * @param indx
 *    Index of chart overlay set.
 * @return
 *    Overlay set at given index.
 */
ChartOverlaySet*
ChartOverlaySetArray::getChartOverlaySet(const int32_t indx)
{
    CaretAssertVectorIndex(m_chartOverlaySets, indx);
    
    return m_chartOverlaySets[indx];
}

/**
 * Initialize the chart overlay selections.
 */
void
ChartOverlaySetArray::initializeOverlaySelections()
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
ChartOverlaySetArray::receiveEvent(Event* event)
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
ChartOverlaySetArray::copyChartOverlaySet(const int32_t sourceTabIndex,
                    const int32_t destinationTabIndex)
{
    CaretAssertVectorIndex(m_chartOverlaySets, sourceTabIndex);
    CaretAssertVectorIndex(m_chartOverlaySets, destinationTabIndex);
    
    const ChartOverlaySet* sourceChartOverlaySet =m_chartOverlaySets[sourceTabIndex];
    ChartOverlaySet* destinationChartOverlaySet =m_chartOverlaySets[destinationTabIndex];
    destinationChartOverlaySet->copyOverlaySet(sourceChartOverlaySet);
}

