
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
#include "DrawingViewportContent.h"
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
        EventDrawingViewportContentAdd* addEvent(dynamic_cast<EventDrawingViewportContentAdd*>(event));
        CaretAssert(addEvent);
        
        const int32_t numItems(addEvent->getNumberOfDrawingViewportContent());
        for (int32_t i = 0; i < numItems; i++) {
            std::unique_ptr<DrawingViewportContent> dvc(addEvent->takeDrawingViewportContent(i));
            addViewport(dvc);
        }
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
        switch (edvc->getMode()) {
            case EventDrawingViewportContentGet::Mode::MATCH_CONTENT_TYPE:
                getViewportTypeInWindow(edvc);
                event->setEventProcessed();
                break;
            case EventDrawingViewportContentGet::Mode::MODEL_TOP_VIEWPORT:
                getTopMostModelInWindow(edvc);
                event->setEventProcessed();
                break;
            case EventDrawingViewportContentGet::Mode::TESTING:
                getAllViewportsInWindow(edvc);
                event->setEventProcessed();
                break;
            case EventDrawingViewportContentGet::Mode::VOLUME_MONTAGE_SLICES:
                getMontageVolumeSlices(edvc);
                break;
        }
    }
}

/**
 * Add  viewport content
 * @param viewport
 *   Viewport to add
 */
void
DrawingViewportContentManager::addViewport(std::unique_ptr<DrawingViewportContent>& viewportContent)
{
    CaretAssert(viewportContent->getViewportContentType() != DrawingViewportContentTypeEnum::INVALID);
    
    const int32_t windowIndex(viewportContent->getWindowIndex());
    if (m_debuFlag) {
        std::cout << "Add viewport: " << viewportContent->toString() << std::endl;
    }
    
    if ((windowIndex >= 0)
        && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)) {
        m_windowViewportContent[windowIndex].push_back(std::move(viewportContent));
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
 *    The window index
 */
void
DrawingViewportContentManager::clearWindow(const int32_t windowIndex)
{
    CaretAssert((windowIndex >= 0)
                && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
    if (m_debuFlag) {
        std::cout << "Clearing window: " << windowIndex << std::endl;
    }
    
    m_windowViewportContent[windowIndex].clear();
}

/**
 * Find the viewport described by the event
 * @param edvc
 *    The content event
 */
void
DrawingViewportContentManager::getViewportTypeInWindow(EventDrawingViewportContentGet* edvc)
{
    const DrawingViewportContentTypeEnum::Enum contentType(edvc->getContentType());
    const int32_t windowIndex(edvc->getWindowIndex());
    const Vector3D windowXY(edvc->getWindowXY());
    
    CaretAssertArrayIndex(m_windowViewportContent, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    std::vector<std::unique_ptr<DrawingViewportContent>>& windowContent(m_windowViewportContent[windowIndex]);
    for (const auto& dvc : windowContent) {
        if (dvc->containsWindowXY(windowXY)
            && (dvc->getViewportContentType() == contentType)) {
            edvc->addDrawingViewportContent(dvc.get());
            break;
        }
    }
}

/**
 * Find the top-most model described by the event
 * @param edvc
 *    The content event
 */
void
DrawingViewportContentManager::getTopMostModelInWindow(EventDrawingViewportContentGet* edvc)
{
    const int32_t windowIndex(edvc->getWindowIndex());
    const Vector3D windowXY(edvc->getWindowXY());
    
    DrawingViewportContent* topDrawingViewportContent(NULL);
    CaretAssertArrayIndex(m_windowViewportContent, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    std::vector<std::unique_ptr<DrawingViewportContent>>& windowContent(m_windowViewportContent[windowIndex]);
    for (auto& dvc : windowContent) {
        if (dvc->containsWindowXY(windowXY)) {
            topDrawingViewportContent = dvc.get();
        }
    }
    edvc->addDrawingViewportContent(topDrawingViewportContent);
}


/**
 * Get ALL the viewport described by the event
 * @param edvc
 *    The content event
 */
void
DrawingViewportContentManager::getAllViewportsInWindow(EventDrawingViewportContentGet* edvc)
{
    const int32_t windowIndex(edvc->getWindowIndex());
    
    const Vector3D windowXY(edvc->getWindowXY());
    CaretAssertArrayIndex(m_windowViewportContent, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    std::vector<std::unique_ptr<DrawingViewportContent>>& windowContent(m_windowViewportContent[windowIndex]);
    for (auto& dvc : windowContent) {
        if (dvc->containsWindowXY(windowXY)) {
            edvc->addDrawingViewportContent(dvc.get());
            std::cout << dvc->toString() << std::endl;
        }
    }
}

/**
 * Get all montage viewports
 * @param edvc
 *    The content event
 */
void
DrawingViewportContentManager::getMontageVolumeSlices(EventDrawingViewportContentGet* edvc)
{
    const int32_t windowIndex(edvc->getWindowIndex());
    const int32_t tabIndex(edvc->getTabIndex());
    CaretAssertArrayIndex(m_windowViewportContent, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    std::vector<std::unique_ptr<DrawingViewportContent>>& windowContent(m_windowViewportContent[windowIndex]);
    for (auto& dvc : windowContent) {
        if (dvc->getTabIndex() == tabIndex) {
            if (dvc->getViewportContentType() == DrawingViewportContentTypeEnum::MODEL_VOLUME_SLICE) {
                edvc->addDrawingViewportContent(dvc.get());
            }
        }
    }
}



