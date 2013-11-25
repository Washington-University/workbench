
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
    m_parentToolBar->updateGraphicsWindow();
    
}

/**
 * Update the user interface.
 */
void
BrainBrowserWindowToolBarComponent::updateUserInterface()
{
    m_parentToolBar->updateUserInterface();
}


