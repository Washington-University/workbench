
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __DRAWING_VIEWPORT_CONTENT_MANAGER_DECLARE__
#include "DrawingViewportContentManager.h"
#undef __DRAWING_VIEWPORT_CONTENT_MANAGER_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DrawingViewportContentWindow.h"
#include "EventDrawingViewportContentAdd.h"
#include "EventDrawingViewportContentClear.h"
#include "EventDrawingViewportContentGet.h"
#include "EventManager.h"

using namespace caret;


    
/**
 * \class caret::DrawingViewportContentManager 
 * \brief Manager for drawing viewports
 * \ingroup Brain
 *
 * This manager provides access to viewports after their content has been drawn.
 * Annotations in particular need access to viewports to get coordinates when drawing
 * a new annotation and for when the line thickness is a percentage of the viewport
 * height.
 */

/**
 * Constructor.
 */
DrawingViewportContentManager::DrawingViewportContentManager()
: CaretObject()
{
    m_windowViewports.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS);
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_ADD);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_GET);
}

/**
 * Destructor.
 */
DrawingViewportContentManager::~DrawingViewportContentManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DrawingViewportContentManager::toString() const
{
    return "DrawingViewportContentManager";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
DrawingViewportContentManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_ADD) {
        EventDrawingViewportContentAdd* edvc(dynamic_cast<EventDrawingViewportContentAdd*>(event));
        CaretAssert(edvc);
        addViewport(edvc->getDrawingViewportContent());
        event->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_CLEAR) {
        EventDrawingViewportContentClear* edvc(dynamic_cast<EventDrawingViewportContentClear*>(event));
        CaretAssert(edvc);
        clearWindow(edvc->getWindowIndex());
        event->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_GET) {
        EventDrawingViewportContentGet* edvc(dynamic_cast<EventDrawingViewportContentGet*>(event));
        CaretAssert(edvc);
        getViewportTypeAtMouse(edvc);
        event->setEventProcessed();
    }
}

/**
 * Add  viewport content
 */
void
DrawingViewportContentManager::addViewport(DrawingViewportContentBase* viewportContent)
{
    CaretAssert(viewportContent);

    const int32_t windowIndex(viewportContent->getWindowIndex());
    if (m_debuFlag) {
        std::cout << "Add viewport: " << viewportContent->toString() << std::endl;
    }
    
    if ((windowIndex >= 0)
        && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)) {
        switch (viewportContent->getViewportContentType()) {
            case DrawingViewportContentTypeEnum::INVALID:
                CaretAssert(0);
                break;
            case DrawingViewportContentTypeEnum::MODEL: /* Yes, fallthrough for model*/
            case DrawingViewportContentTypeEnum::TAB:
            {
                /*
                 * Add BOTH model and tab to a window
                 */
                CaretAssertVectorIndex(m_windowViewports, windowIndex);
                if (m_windowViewports[windowIndex]) {
                    m_windowViewports[windowIndex]->addChildViewport(viewportContent);
                }
                else {
                    const AString msg("Trying to add tab to window without viewport, window="
                                      + AString::number(windowIndex));
                    CaretAssertMessage(0, msg);
                    CaretLogSevere(msg);
                }
            }
                break;
            case DrawingViewportContentTypeEnum::WINDOW:
            {
                DrawingViewportContentWindow* window(viewportContent->castToWindow());
                CaretAssert(window);
                CaretAssertVectorIndex(m_windowViewports, windowIndex);
                m_windowViewports[windowIndex].reset(window);
            }
                break;
        }
    }
    else {
        const AString msg("Invalid window index on viewport: "
                          + viewportContent->toString());
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
    
}

/**
 * Clear all viewports associated with the window
 * @param windowIndex
 */
void
DrawingViewportContentManager::clearWindow(const int32_t windowIndex)
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
    if (m_debuFlag) {
        std::cout << "Clearing window: " << windowIndex << std::endl;
    }
    
    CaretAssertVectorIndex(m_windowViewports, windowIndex);
    m_windowViewports[windowIndex].reset();
}

/**
 * Find the viewport described by the event
 */
void
DrawingViewportContentManager::getViewportTypeAtMouse(EventDrawingViewportContentGet* edvc)
{
    const int32_t windowIndex(edvc->getWindowIndex());
    
    CaretAssertVectorIndex(m_windowViewports, windowIndex);
    if (m_windowViewports[windowIndex]) {
        const DrawingViewportContentBase* viewportContent(m_windowViewports[windowIndex]->getViewportTypeAtMouse(edvc->getContentType(), edvc->getMouseXY()));
        edvc->setDrawingViewportContent(viewportContent);
    }
    else {
        const AString msg("Requesting viewport for non-existant window="
                          + AString::number(windowIndex));
        CaretLogSevere(msg);
        CaretAssertMessage(0, msg);
    }
}
