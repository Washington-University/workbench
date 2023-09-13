
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
#include "DrawingViewportContentBase.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDrawingViewportContentGet 
 * \brief Get viewport content after drawing has taken place
 * \ingroup Brain
 */

/**
 * Constructor for getting a specific content type in a window at a mouse position
 * @param contentType
 *    Content type requested
 * @param windowIndex
 *    Index of window
 * @param mouseXY
 *    Location of mouse in window
 */
EventDrawingViewportContentGet::EventDrawingViewportContentGet(const DrawingViewportContentTypeEnum::Enum contentType,
                                                               const int32_t windowIndex,
                                                               const Vector3D& mouseXY)
: Event(EventTypeEnum::EVENT_DRAWING_VIEWPORT_CONTENT_GET),
m_contentType(contentType),
m_windowIndex(windowIndex),
m_mouseXY(mouseXY)
{
    
}

/**
 * Destructor.
 */
EventDrawingViewportContentGet::~EventDrawingViewportContentGet()
{
}

/**
 * @return The drawing viewport content after the event is processed (NULL if not available)
 */
const DrawingViewportContentBase*
EventDrawingViewportContentGet::getDrawingViewportContent() const
{
    return m_drawingViewportContent;
}

/**
 * @return The MODEL drawing viewport  after this event is processed (NULL if failure or
 * if drawing viewport is not a model drawing viewport)
 */
const DrawingViewportContentModel*
EventDrawingViewportContentGet::getDrawingViewportContentModel() const
{
    if (m_drawingViewportContent != NULL) {
        return m_drawingViewportContent->castToModel();
    }
    return NULL;
}

/**
 * @return The TAB drawing viewport  after this event is processed (NULL if failure or
 * if drawing viewport is not a tab drawing viewport)
 */
const DrawingViewportContentTab*
EventDrawingViewportContentGet::getDrawingViewportContentTab() const
{
    if (m_drawingViewportContent != NULL) {
        return m_drawingViewportContent->castToTab();
    }
    return NULL;
}

/**
 * @return The WINDOW drawing viewport  after this event is processed (NULL if failure or
 * if drawing viewport is not a window drawing viewport)
 */
const DrawingViewportContentWindow*
EventDrawingViewportContentGet::getDrawingViewportContentWindow() const
{
    if (m_drawingViewportContent != NULL) {
        return m_drawingViewportContent->castToWindow();
    }
    return NULL;
}

/**
 * Set the drawing viewport content
 * @param drawingViewportContent
 *    The viewport content
 */
void
EventDrawingViewportContentGet::setDrawingViewportContent(const DrawingViewportContentBase* drawingViewportContent)\
{
    m_drawingViewportContent = drawingViewportContent;
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
 * @return Location of the mouse in the window
 */
const Vector3D
EventDrawingViewportContentGet::getMouseXY() const
{
    return m_mouseXY;
}

