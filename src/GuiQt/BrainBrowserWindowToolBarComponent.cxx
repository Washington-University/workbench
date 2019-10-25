
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_COMPONENT_DECLARE__
#include "BrainBrowserWindowToolBarComponent.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_COMPONENT_DECLARE__

#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarComponent 
 * \brief Abstract class for brain browser window tool bar components.
 * \ingroup GuiQt
 *
 */

/**
 * Constructor.
 * 
 * @param parent
 *    Parent widget.
 */
BrainBrowserWindowToolBarComponent::BrainBrowserWindowToolBarComponent(BrainBrowserWindowToolBar* parentToolBar)
: QWidget(parentToolBar),
m_parentToolBar(parentToolBar)
{
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarComponent::~BrainBrowserWindowToolBarComponent()
{
    m_widgetGroup->clear();
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
BrainBrowserWindowToolBarComponent::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Add a widget/object to the widget group so that all items can be
 * enabled or disabled.
 *
 * @param qObject
 *    QObject or QWidget subclass added to widget group.
 */
void
BrainBrowserWindowToolBarComponent::addToWidgetGroup(QObject* qObject)
{
    CaretAssert(qObject);
    m_widgetGroup->add(qObject);
}

/**
 * Block/unblock signals for components in the widget group.
 *
 * @param blocked
 *     New status for blocked (true/false).
 */
void
BrainBrowserWindowToolBarComponent::blockAllSignals(const bool blocked)
{
    m_widgetGroup->blockAllSignals(blocked);
}

/**
 * @return The browser tab content for the selected tab.
 */
BrowserTabContent*
BrainBrowserWindowToolBarComponent::getTabContentFromSelectedTab()
{
    return m_parentToolBar->getTabContentFromSelectedTab();
}

/**
 * Invalidate surface coloring and update the graphics in the 
 * window containing this toolbar.
 */
void
BrainBrowserWindowToolBarComponent::invalidateColoringAndUpdateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    updateGraphicsWindow();
}

/**
 * Update the graphics window.
 */
void
BrainBrowserWindowToolBarComponent::updateGraphicsWindow()
{
    m_parentToolBar->updateGraphicsWindow();
}

/**
 * If this window is yoked, update all windows since they may
 * be yoked to this window.  If NOT yoked, just update this window.
 */
void
BrainBrowserWindowToolBarComponent::updateGraphicsWindowAndYokedWindows()
{
    m_parentToolBar->updateGraphicsWindowAndYokedWindows();
}

/**
 * Update the user interface.
 */
void
BrainBrowserWindowToolBarComponent::updateUserInterface()
{
    m_parentToolBar->updateUserInterface();
}

/**
 * @return Parent window toolbar
 */
BrainBrowserWindowToolBar*
BrainBrowserWindowToolBarComponent::getParentToolBar()
{
    return m_parentToolBar;
}

