
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

#define __EVENT_DRAWING_VIEWPORT_CONTENT_GET_DECLARE__
#include "EventDrawingViewportContentGet.h"
#undef __EVENT_DRAWING_VIEWPORT_CONTENT_GET_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DrawingViewportContent.h"
#include "EventBrowserTabIndexGetWindowIndex.h"
#include "EventManager.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDrawingViewportContentGet 
 * \brief Get viewport content after drawing has taken place
 * \ingroup Brain
 */

/**
 * @return New instance to get top-most model at window XY
 * @param windowIndex
 *    Index of window
 * @param windowXY
 *    Position in window
 */
std::unique_ptr<EventDrawingViewportContentGet>
EventDrawingViewportContentGet::newInstanceGetTopModelViewport(const int32_t windowIndex,
                                                               const Vector3D& windowXY)
{
    const int32_t invalidTabIndex(-1);
    std::unique_ptr<EventDrawingViewportContentGet> ptr(new EventDrawingViewportContentGet(Mode::MODEL_TOP_VIEWPORT,
                                                                                           DrawingViewportContentTypeEnum::INVALID,
                                                                                           windowIndex,
                                                                                           invalidTabIndex,
                                                                                           windowXY));
    return ptr;
}

/**
 * @return New instance to get content type at window XY
 * @param windowIndex
 *    Index of window
 * @param windowXY
 *    Position in window
 * @param contentType
 *    The content type
 */
std::unique_ptr<EventDrawingViewportContentGet>
EventDrawingViewportContentGet::newInstanceGetContentType(const int32_t windowIndex,
                                                          const Vector3D& windowXY,
                                                          const DrawingViewportContentTypeEnum::Enum contentType)
{
    std::unique_ptr<EventDrawingViewportContentGet> ptr(new EventDrawingViewportContentGet(Mode::MATCH_CONTENT_TYPE,
                                                                                           contentType,
                                                                                           windowIndex,
                                                                                           -1, /* invalid tab index */
                                                                                           windowXY));
    return ptr;
}

/**
 * New instance that prints all viewports at window XY
 * @param windowIndex
 *    Index of window
 * @param windowXY
 *    Position in window
 */
std::unique_ptr<EventDrawingViewportContentGet>
EventDrawingViewportContentGet::newInstancePrintAllAtWindowXY(const int32_t windowIndex,
                                                              const Vector3D& windowXY)
{
    std::unique_ptr<EventDrawingViewportContentGet> ptr(new EventDrawingViewportContentGet(Mode::TESTING,
                                                                                           DrawingViewportContentTypeEnum::INVALID,
                                                                                           windowIndex,
                                                                                           -1, /* invalid tab index */
                                                                                           windowXY));
    return ptr;
}

/**
 * @return All montage slice drawing viewports in the given tab
 * @param tabIndex
 *    Index of tab
 */
std::vector<const DrawingViewportContent*>
EventDrawingViewportContentGet::getVolumeMontageSlicesInTab(const int32_t tabIndex)
{
    EventBrowserTabIndexGetWindowIndex windowIndexEvent(tabIndex);
    EventManager::get()->sendEvent(windowIndexEvent.getPointer());
    const int32_t windowIndex(windowIndexEvent.getWindowIndex());

    if (windowIndex >= 0) {
        const Vector3D invalidWindowXY;
        EventDrawingViewportContentGet contentEvent(Mode::VOLUME_MONTAGE_SLICES,
                                                    DrawingViewportContentTypeEnum::MODEL_VOLUME_SLICE,
                                                    windowIndex,
                                                    tabIndex,
                                                    invalidWindowXY);
        EventManager::get()->sendEvent(contentEvent.getPointer());
        
        return contentEvent.getAllDrawingViewportContent();
    }
    
    CaretLogSevere("Failed to find window containing tab with index="
                   + AString::number(tabIndex));
    std::vector<const DrawingViewportContent*> emptyContent;
    return emptyContent;
}

/**
 * Constructor for getting a specific content type in a window at a window position
 * @param contentType
 *    Content type requested
 * @param windowIndex
 *    Index of window
 * @param windowXY
 *    Location in window
 */
EventDrawingViewportContentGet::EventDrawingViewportContentGet(const DrawingViewportContentTypeEnum::Enum contentType,
                                                               const int32_t windowIndex,
                                                               const Vector3D& windowXY)
: EventDrawingViewportContentGet(Mode::MATCH_CONTENT_TYPE,
                                 contentType,
                                 windowIndex,
                                 -1, /* invalid tab index */
                                 windowXY)
{
    
}

/**
 * Constructor for getting a specific content type in a window  position
 * @param contentType
 *    Content type requested
 * @param windowIndex
 *    Index of window
 * @param windowXY
 *    Location in window
 */
EventDrawingViewportContentGet::EventDrawingViewportContentGet(const int32_t windowIndex,
                                                               const Vector3D& windowXY)
: EventDrawingViewportContentGet(Mode::TESTING,
                                 DrawingViewportContentTypeEnum::INVALID,
                                 windowIndex,
                                 -1, /* invalid tab index */
                                 windowXY)
{
    
}

/**
 * Constructor for getting a specific content type in a window  position
 * @param mode
 *    The mode
 * @param contentType
 *    Content type requested
 * @param windowIndex
 *    Index of window
 * @param windowXY
 *    Location of XY in window
 */
EventDrawingViewportContentGet::EventDrawingViewportContentGet(const Mode mode,
                                                               const DrawingViewportContentTypeEnum::Enum contentType,
                                                               const int32_t windowIndex,
                                                               const int32_t tabIndex,
                                                               const Vector3D& windowXY)
: Event(EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_GET),
m_mode(mode),
m_contentType(contentType),
m_windowIndex(windowIndex),
m_tabIndex(tabIndex),
m_windowXY(windowXY)
{
    
}

/**
 * Destructor.
 */
EventDrawingViewportContentGet::~EventDrawingViewportContentGet()
{
}

/**
 * @return The mode
 */
EventDrawingViewportContentGet::Mode
EventDrawingViewportContentGet::getMode() const
{
    return m_mode;
}


/**
 * @return Content type desired
 */
DrawingViewportContentTypeEnum::Enum
EventDrawingViewportContentGet::getContentType() const
{
    return m_contentType;
}

/**
 * @return Index of the window
 */
int32_t
EventDrawingViewportContentGet::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return Location of  in the window
 */
const Vector3D
EventDrawingViewportContentGet::getWindowXY() const
{
    return m_windowXY;
}

/**
 * @return Index of tab
 */
int32_t
EventDrawingViewportContentGet::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * The drawing viewport content
 */
const DrawingViewportContent*
EventDrawingViewportContentGet::getDrawingViewportContent() const
{
    if (m_drawingViewportContent.empty()) {
        return NULL;
    }
    if (m_drawingViewportContent.size() > 1) {
        CaretLogSevere("Should not call this method, there is more than one DrawingViewportContent");
    }
    CaretAssertVectorIndex(m_drawingViewportContent, 0);
    return m_drawingViewportContent[0];
}

/**
 * @return All drawing viewport content
 */
std::vector<const DrawingViewportContent*>
EventDrawingViewportContentGet::getAllDrawingViewportContent() const
{
    return m_drawingViewportContent;
}

/**
 * Add the draiwng viewport content
 * @param drawingViewportContent
 *   The content
 */
void
EventDrawingViewportContentGet::addDrawingViewportContent(const DrawingViewportContent* drawingViewportContent)
{
    m_drawingViewportContent.push_back(drawingViewportContent);
}

