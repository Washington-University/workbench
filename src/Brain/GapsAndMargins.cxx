
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __GAPS_AND_MARGINS_DECLARE__
#include "GapsAndMargins.h"
#undef __GAPS_AND_MARGINS_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabNew.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class caret::GapsAndMargins 
 * \brief Gaps for surface montage and volume montage.  Margins for tabs.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
GapsAndMargins::GapsAndMargins()
: CaretObject()
{
    reset();
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->addArray("m_tabMarginsBottom",
                               m_tabMarginsBottom,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->addArray("m_tabMarginsLeft",
                               m_tabMarginsLeft,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->addArray("m_tabMarginsRight",
                               m_tabMarginsRight,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->addArray("m_tabMarginsTop",
                               m_tabMarginsTop,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                               0.0);
    
    m_sceneAssistant->addArray("m_surfaceMontageHorizontalGaps",
                               m_surfaceMontageHorizontalGaps,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS,
                               0.0);
    
    m_sceneAssistant->addArray("m_surfaceMontageVerticalGaps",
                               m_surfaceMontageVerticalGaps,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS,
                               0.0);
    
    m_sceneAssistant->addArray("m_volumeMontageHorizontalGaps",
                               m_volumeMontageHorizontalGaps,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS,
                               0.0);
    
    m_sceneAssistant->addArray("m_volumeMontageVerticalGaps",
                               m_volumeMontageVerticalGaps,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS,
                               0.0);
    
    /*
     * Use processed event listener because we need to know the index
     * of the tab that was created
     */
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW);
}

/**
 * Destructor.
 */
GapsAndMargins::~GapsAndMargins()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
GapsAndMargins::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_NEW) {
        EventBrowserTabNew* newTabEvent = dynamic_cast<EventBrowserTabNew*>(event);
        CaretAssert(newTabEvent);
        
        const BrowserTabContent* tab = newTabEvent->getBrowserTab();
        CaretAssert(tab);
        if (tab != NULL) {
            const int32_t tabIndex = tab->getTabNumber();
            CaretAssertArrayIndex(m_tabMarginsLeft,   BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            CaretAssertArrayIndex(m_tabMarginsRight,  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            CaretAssertArrayIndex(m_tabMarginsBottom, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            CaretAssertArrayIndex(m_tabMarginsTop,    BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            m_tabMarginsLeft[tabIndex]   = 0.0;
            m_tabMarginsRight[tabIndex]  = 0.0;
            m_tabMarginsBottom[tabIndex] = 0.0;
            m_tabMarginsTop[tabIndex]    = 0.0;
        }
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GapsAndMargins::toString() const
{
    return "GapsAndMargins";
}

void
GapsAndMargins::reset()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_tabMarginsBottom[i] = 0.0;
        m_tabMarginsLeft[i]   = 0.0;
        m_tabMarginsRight[i]  = 0.0;
        m_tabMarginsTop[i]    = 0.0;
    }

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        m_surfaceMontageHorizontalGaps[i] = 0.0;
        m_surfaceMontageVerticalGaps[i]   = 0.0;
        m_volumeMontageHorizontalGaps[i]  = 0.0;
        m_volumeMontageVerticalGaps[i]    = 0.0;
    }
}

/**
 * Get the LEFT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Left margin for the tab.
 */
float
GapsAndMargins::getMarginLeftForTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsLeft, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsLeft[tabIndex];
}

/**
 * Get the RIGHT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Right margin for the tab.
 */
float
GapsAndMargins::getMarginRightForTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsRight, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsRight[tabIndex];
}

/**
 * Get the BOTTOM margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Bottom margin for the tab.
 */
float
GapsAndMargins::getMarginBottomForTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsBottom, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsBottom[tabIndex];
}

/**
 * Get the TOP margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Top margin for the tab.
 */
float
GapsAndMargins::getMarginTopForTab(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_tabMarginsTop, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_tabMarginsTop[tabIndex];
}

/**
 * Set the LEFT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Left margin for the tab.
 */
void
GapsAndMargins::setMarginLeftForTab(const int32_t tabIndex,
                                 const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsLeft, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsLeft[tabIndex] = margin;
}

/**
 * Set the RIGHT margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Right margin for the tab.
 */
void
GapsAndMargins::setMarginRightForTab(const int32_t tabIndex,
                                  const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsRight, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsRight[tabIndex] = margin;
}

/**
 * Set the BOTTOM margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Bottom margin for the tab.
 */
void
GapsAndMargins::setMarginBottomForTab(const int32_t tabIndex,
                                  const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsBottom, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsBottom[tabIndex] = margin;
}

/**
 * Set the Top margin for the given tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param margin
 *     Top margin for the tab.
 */
void
GapsAndMargins::setMarginTopForTab(const int32_t tabIndex,
                                   const float margin)
{
    CaretAssertArrayIndex(m_tabMarginsTop, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_tabMarginsTop[tabIndex] = margin;
}

/**
 * @return The surface montage horizontal gap.
 *
 * @param windowIndex
 *    Index of window.
 */
float
GapsAndMargins::getSurfaceMontageHorizontalGapForWindow(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_surfaceMontageHorizontalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_surfaceMontageHorizontalGaps[windowIndex];
}

/**
 * @return The surface montage vertical gap.
 *
 * @param windowIndex
 *    Index of window.
 */
float
GapsAndMargins::getSurfaceMontageVerticalGapForWindow(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_surfaceMontageVerticalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_surfaceMontageVerticalGaps[windowIndex];
}

/**
 * @return The volume montage horizontal gap.
 *
 * @param windowIndex
 *    Index of window.
 */
float
GapsAndMargins::getVolumeMontageHorizontalGapForWindow(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_volumeMontageHorizontalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_volumeMontageHorizontalGaps[windowIndex];
}

/**
 * @return The volume montage vertical gap.
 *
 * @param windowIndex
 *    Index of window.
 */
float
GapsAndMargins::getVolumeMontageVerticalGapForWindow(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_volumeMontageVerticalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_volumeMontageVerticalGaps[windowIndex];
}

/**
 * Set the surface montage horizontal gap.
 *
 * @param windowIndex
 *    Index of window.
 * @param gap
 *    New value for horizontal gap.
 */
void
GapsAndMargins::setSurfaceMontageHorizontalGapForWindow(const int32_t windowIndex,
                                                        const float gap)
{
    CaretAssertArrayIndex(m_surfaceMontageHorizontalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_surfaceMontageHorizontalGaps[windowIndex] = gap;
}

/**
 * Set the surface montage vertical gap.
 *
 * @param windowIndex
 *    Index of window.
 * @param gap
 *    New value for vertical gap.
 */
void
GapsAndMargins::setSurfaceMontageVerticalGapForWindow(const int32_t windowIndex,
                                                      const float gap)
{
    CaretAssertArrayIndex(m_surfaceMontageVerticalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_surfaceMontageVerticalGaps[windowIndex] = gap;
}

/**
 * Set the volume montage horizontal gap.
 *
 * @param windowIndex
 *    Index of window.
 * @param gap
 *    New value for horizontal gap.
 */
void
GapsAndMargins::setVolumeMontageHorizontalGapForWindow(const int32_t windowIndex,
                                                       const float gap)
{
    CaretAssertArrayIndex(m_volumeMontageHorizontalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_volumeMontageHorizontalGaps[windowIndex] = gap;
}

/**
 * Set the volume montage vertical gap.
 *
 * @param windowIndex
 *    Index of window.
 * @param gap
 *    New value for vertical gap.
 */
void
GapsAndMargins::setVolumeMontageVerticalGapForWindow(const int32_t windowIndex,
                                                     const float gap)
{
    CaretAssertArrayIndex(m_volumeMontageVerticalGaps, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_volumeMontageVerticalGaps[windowIndex] = gap;
}

/**
 * Get the margins for drawing a tab.
 *
 * @param tabIndex
 *     Index of the tab.
 * @param viewportWidth
 *     Width of viewport.
 * @param viewportHeight
 *     Height of viewport.
 * @param MarginOut
 *     Margin for
 * @param MarginOut
 *     Margin for
 * @param MarginOut
 *     Margin for
 * @param MarginOut
 *     Margin for
 */
void
GapsAndMargins::getMarginsInPixelsForDrawing(const int32_t tabIndex,
                                             const int32_t viewportWidth,
                                             const int32_t viewportHeight,
                                             int32_t& leftMarginOut,
                                             int32_t& rightMarginOut,
                                             int32_t& bottomMarginOut,
                                             int32_t& topMarginOut) const
{
    leftMarginOut   = 0;
    rightMarginOut  = 0;
    bottomMarginOut = 0;
    topMarginOut    = 0;

    topMarginOut    = static_cast<int32_t>(viewportHeight * (getMarginTopForTab(tabIndex)    / 100.0));
    bottomMarginOut = static_cast<int32_t>(viewportHeight * (getMarginBottomForTab(tabIndex) / 100.0));
    leftMarginOut   = static_cast<int32_t>(viewportWidth  * (getMarginLeftForTab(tabIndex)   / 100.0));
    rightMarginOut  = static_cast<int32_t>(viewportWidth  * (getMarginRightForTab(tabIndex)  / 100.0));
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
GapsAndMargins::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "GapsAndMargins",
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
GapsAndMargins::restoreFromScene(const SceneAttributes* sceneAttributes,
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

