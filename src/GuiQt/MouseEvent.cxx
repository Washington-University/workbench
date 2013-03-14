/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QMouseEvent>
#include <QWheelEvent>

#include "MouseEvent.h"

using namespace caret;

/**
 * Constructor.
 * @param mouseEventType
 *    Type of mouse event.
 * @param keyModifiers
 *    Mouse key modifiers.
 * @param x
 *    Current mouse X-coordinate (left == 0)
 * @param y
 *    Current mouse Y-coordinate (bottom == 0)
 * @param dx
 *    Change in mouse X-coordinate since last mouse event
 * @param dy
 *    Change in mouse Y-coordinate since last mouse event
 * @param mousePressX
 *    X-coordinate of mouse when button was pressed
 * @param mousePressY
 *    Y-coordinate of mouse when button was pressed
 */
MouseEvent::MouseEvent(BrainOpenGLViewportContent* viewportContent,
                       BrainOpenGLWidget* openGLWidget,
                       const int32_t browserWindowIndex,
                       const MouseEventTypeEnum::Enum mouseEventType,
                       const Qt::KeyboardModifiers keyModifiers, 
                       const int32_t x,
                       const int32_t y,
                       const int32_t dx,
                       const int32_t dy,
                       const int32_t mousePressX,
                       const int32_t mousePressY)
: CaretObject()
{
    initializeMembersMouseEvent();

    m_viewportContent = viewportContent;
    m_openGLWidget    = openGLWidget;
    m_browserWindowIndex = browserWindowIndex;
    m_mouseEventType = mouseEventType;
    m_x = x;
    m_y = y;
    m_dx = dx;
    m_dy = dy;
    m_pressX = mousePressX;
    m_pressY = mousePressY;

    if (keyModifiers == Qt::NoButton) {
        // nothing
    }
    else if (keyModifiers == Qt::ControlModifier) {
        m_keyDownControl = true;           
    }
    else if (keyModifiers == Qt::ShiftModifier) {
        m_keyDownShift = true;            
    }
    else if ((keyModifiers & Qt::ControlModifier)
             && (keyModifiers & Qt::ShiftModifier)) {
        m_keyDownControlAndShift = true;           
    }
    else {
        /*
         * Combinations of keys NOT allowed.
         */
        m_mouseEventType = MouseEventTypeEnum::INVALID;
    }
}

/**
 * Constructor.
 * @param event
 *    The mouse wheel event.
 */
MouseEvent::MouseEvent(const QWheelEvent& event)
: CaretObject()
{
    initializeMembersMouseEvent();
    
    m_mouseEventType = MouseEventTypeEnum::WHEEL_MOVED;
    m_x = event.x();
    m_y = event.y();
    m_wheelRotation = event.delta();
}

/**
 * Destructor
 */
MouseEvent::~MouseEvent()
{
}

/**
 * Initialize all members.
 */
void
MouseEvent::initializeMembersMouseEvent()
{
    m_viewportContent = NULL;
    m_openGLWidget = NULL;
    m_mouseEventType = MouseEventTypeEnum::INVALID;
    m_browserWindowIndex = -1;
    m_x = 0;
    m_y = 0;
    m_dx = 0;
    m_dy = 0;
    m_pressX = 0;
    m_pressY = 0;
    m_wheelRotation = 0;
    m_keyDownControl = false;
    m_keyDownControlAndShift = false;
    m_keyDownShift = false;
    setNoKeysDown();
}

void 
MouseEvent::setNoKeysDown()
{
    m_keyDownControlAndShift = false;
    m_keyDownControl = false;
    m_keyDownShift = false;
}

/**
 * Is this mouse event valid?
 *
 * Some key combinations are not allowed and they
 * will cause the event to be invalid.
 *
 * @return true if mouse event is valid, else false.
 */
bool 
MouseEvent::isValid() const
{
    return (m_mouseEventType != MouseEventTypeEnum::INVALID);
}

/**
 * @return The viewport content in which the mouse was pressed.
 */
BrainOpenGLViewportContent*
MouseEvent::getViewportContent() const
{
    return m_viewportContent;
}

/**
 * @return The OpenGL Widget in which the mouse event occurred.
 */
BrainOpenGLWidget*
MouseEvent::getOpenGLWidget() const
{
    return m_openGLWidget;
}

/**
 * Get a string showing the contents of this mouse event.
 * @return String describing the mouse status.
 */
AString
MouseEvent::toString() const
{
    const AString msg = 
    "type=" + MouseEventTypeEnum::toName(m_mouseEventType)
    + ", x=" + AString::number(m_x)
    + ", y=" + AString::number(m_y)
    + ", dx=" + AString::number(m_dx)
    + ", dy=" + AString::number(m_dy);
    + ", keyDownControlAndShift=" + AString::fromBool(m_keyDownControlAndShift)
    + ", keyDownControl=" + AString::fromBool(m_keyDownControl)
    + ", keyDownShift=" + AString::fromBool(m_keyDownShift)
    + ", wheelRotation=" + AString::number(m_wheelRotation);
    
    return msg;
}

/**
 * @return Index of the browser window in which the
 * event took place.
 */
int32_t 
MouseEvent::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * Get the change in the X-coordinate.
 * @return change in the X-coordinate.
 *
 */
int32_t
MouseEvent::getDx() const
{
    return m_dx;
}

/**
 * Get the change in the Y-coordinate.
 * @return change in the Y-coordinate.
 *
 */
int32_t
MouseEvent::getDy() const
{
    return m_dy;
}

/**
 * Get the type of mouse event.
 * @return Type of mouse event.
 *
 */
MouseEventTypeEnum::Enum
MouseEvent::getMouseEventType() const
{
    return m_mouseEventType;
}

/**
 * Get the X-coordinate of the mouse.
 * @return The X-coordinate.
 *
 */
int32_t
MouseEvent::getX() const
{
    return m_x;
}

/**
 * Get the Y-coordinate of the mouse.
 * Origin is at the BOTTOM of the widget !!!
 * @return The Y-coordinate.
 *
 */
int32_t
MouseEvent::getY() const
{
    return m_y;
}

/**
 * Get the X-coordinate of where the mouse was pressed.
 * @return The X-coordinate.
 *
 */
int32_t
MouseEvent::getPressedX() const
{
    return m_pressX;
}

/**
 * Get the Y-coordinate of where the mouse was pressed.
 * Origin is at the BOTTOM of the widget !!!
 * @return The Y-coordinate.
 *
 */
int32_t
MouseEvent::getPressedY() const
{
    return m_pressY;
}

/**
 * Get the amount of rotation in the mouse wheel.
 * @return  Amount mouse wheel rotated.
 *
 */
int32_t
MouseEvent::getWheelRotation() const
{
    return m_wheelRotation;
}

/**
 * @return Are any keys down?
 */
bool 
MouseEvent::isAnyKeyDown() const
{
    const bool anyKeyDown = (m_keyDownControlAndShift
                             || m_keyDownControl
                             || m_keyDownShift);
    
    return anyKeyDown;
}

/**
 * @return Is the CONTROL AND SHIFT key down?
 */
bool 
MouseEvent::isControlAndShiftKeyDown() const
{
    return m_keyDownControlAndShift;
}

/**
 * @return Is the CONTROL key down?
 */
bool 
MouseEvent::isControlKeyDown() const
{
    return m_keyDownControl;
}

/**
 * @return Is the SHIFT key down?
 */
bool 
MouseEvent::isShiftKeyDown() const
{
    return m_keyDownShift;
}
