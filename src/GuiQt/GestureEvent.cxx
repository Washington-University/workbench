
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __GESTURE_EVENT_DECLARE__
#include "GestureEvent.h"
#undef __GESTURE_EVENT_DECLARE__

#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GestureEvent 
 * \brief Contains information about a gesture event, typically from a track pad
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param viewportContent
 *    Content of viewport.
 * @param openGLWidget
 *    OpenGL Widget in which mouse activity took place.
 * @param browserWindowIndex
 *    Index of the browser winddow in which mouse activity took place.
 * @param startCenterX
 *    X-coord at start of gesture
 * @param startCenterY
 *    Y-coord at start of gesture
 * @param state
 *    State of the gesture
 * @param type
 *    Type of the gesture
 * @param dataValue
 *    Data value (pinch or rotation)
 */
GestureEvent::GestureEvent(const BrainOpenGLViewportContent* viewportContent,
                           BrainOpenGLWidget* openGLWidget,
                           const int32_t browserWindowIndex,
                           const int32_t startCenterX,
                           const int32_t startCenterY,
                           const State state,
                           const Type type,
                           const float dataValue)
: CaretObject(),
m_openGLWidget(openGLWidget),
m_browserWindowIndex(browserWindowIndex),
m_startCenterX(startCenterX),
m_startCenterY(startCenterY),
m_state(state),
m_type(type),
m_dataValue(dataValue)
{
    if (viewportContent != NULL) {
        m_viewportContent.reset(new BrainOpenGLViewportContent(*viewportContent));
    }
}

/**
 * Destructor.
 */
GestureEvent::~GestureEvent()
{
}

/**
 * @return The viewport content in which the mouse was pressed.
 */
BrainOpenGLViewportContent*
GestureEvent::getViewportContent() const
{
    return m_viewportContent.get();
}

/**
 * @return The OpenGL Widget in which the mouse event occurred.
 */
BrainOpenGLWidget*
GestureEvent::getOpenGLWidget() const
{
    return m_openGLWidget;
}

/**
 * @return Index of the browser window in which the
 * event took place.
 */
int32_t
GestureEvent::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * Get the X-coordinate of the mouse.
 * @return The X-coordinate.
 *
 */
int32_t
GestureEvent::getStartCenterX() const
{
    return m_startCenterX;
}

/**
 * Get the Y-coordinate of the mouse.
 * Origin is at the BOTTOM of the widget !!!
 * @return The Y-coordinate.
 *
 */
int32_t
GestureEvent::getStartCenterY() const
{
    return m_startCenterY;
}

/**
 * Get the Global X-coordinate of where the mouse was pressed.
 * @return The Global X-coordinate.
 *
 * @param x
 *     X-coordinate in widget (0 is left side of widget)
 * @param y
 *     Y-coordinate in widget (0 is bottom side of widget)
 * @param outGlobalX
 *     Output with global X-coordinate
 * @param outGlobalY
 *     Output with global Y-coordinate
 */
void
GestureEvent::getGlobalXY(const int32_t x,
                          const int32_t y,
                          int32_t& outGlobalX,
                          int32_t& outGlobalY) const
{
    CaretAssert(m_openGLWidget);
    const int32_t yOriginTop = m_openGLWidget->height() - y;
    const QPoint globalPoint = m_openGLWidget->mapToGlobal(QPoint(x,
                                                                  yOriginTop));
    outGlobalX = globalPoint.x();
    outGlobalY = globalPoint.y();
}

/**
 * @return The State of the gesture
 */
GestureEvent::State
GestureEvent::getState() const
{
    return m_state;
}

/**
 * @return The Type of the gesture
 */
GestureEvent::Type
GestureEvent::getType() const
{
    return m_type;
}

/**
 * @return The data value (Pinch is scale factor, bigger if > 1, smaller if < 1;
 *                    Rotate is change in rotation angle)
 */
float
GestureEvent::getValue() const
{
    return m_dataValue;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GestureEvent::toString() const
{
    return "GestureEvent";
}

