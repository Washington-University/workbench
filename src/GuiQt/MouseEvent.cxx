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
 *    Mouse X-coordinate (left == 0)
 * @param y
 *    Mouse Y-coordinate (bottom == 0)
 * @param dx
 *    Change in mouse X-coordinate
 * @param dy
 *    Change in mouse Y-coordinate
 */
MouseEvent::MouseEvent(const MouseEventTypeEnum::Enum mouseEventType,
                       const Qt::KeyboardModifiers keyModifiers, 
                       const int x,
                       const int y,
                       const int dx,
                       const int dy)
: CaretObject()
{
    this->initializeMembersMouseEvent();

    this->mouseEventType = mouseEventType;
    this->x = x;
    this->y = y;
    this->dx = dx;
    this->dy = dy;

    if (keyModifiers == Qt::NoButton) {
        // nothing
    }
    else if (keyModifiers == Qt::ControlModifier) {
        this->keyDownControl = true;           
    }
    else if (keyModifiers == Qt::ShiftModifier) {
        this->keyDownShift = true;            
    }
    else if ((keyModifiers & Qt::ControlModifier)
             && (keyModifiers & Qt::ShiftModifier)) {
        this->keyDownControlAndShift = true;           
    }
    else {
        /*
         * Combinations of keys NOT allowed.
         */
        this->mouseEventType = MouseEventTypeEnum::INVALID;
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
    this->initializeMembersMouseEvent();
    
    this->mouseEventType = MouseEventTypeEnum::WHEEL_MOVED;
    this->x = event.x();
    this->y = event.y();
    this->wheelRotation = event.delta();
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
    this->mouseEventType = MouseEventTypeEnum::INVALID;
    this->x = 0;
    this->y = 0;
    this->dx = 0;
    this->dy = 0;
    this->wheelRotation = 0;
    
    this->setNoKeysDown();
    
    this->graphicsUpdateOneWindowRequested = false;
    this->graphicsUpdateAllWindowsRequested = false;
    this->userInterfaceUpdateRequested = false;
}

void 
MouseEvent::setNoKeysDown()
{
    this->keyDownControlAndShift = false;
    this->keyDownControl = false;
    this->keyDownShift = false;
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
    return (this->mouseEventType != MouseEventTypeEnum::INVALID);
}

/**
 * Get a string showing the contents of this mouse event.
 * @return String describing the mouse status.
 */
AString
MouseEvent::toString() const
{
    const AString msg = 
    "type=" + MouseEventTypeEnum::toName(this->mouseEventType)
    + ", x=" + AString::number(this->x)
    + ", y=" + AString::number(this->y)
    + ", dx=" + AString::number(this->dx)
    + ", dy=" + AString::number(this->dy);
    + ", keyDownControlAndShift=" + AString::fromBool(this->keyDownControlAndShift)
    + ", keyDownControl=" + AString::fromBool(this->keyDownControl)
    + ", keyDownShift=" + AString::fromBool(this->keyDownShift)
    + ", wheelRotation=" + AString::number(this->wheelRotation);
    
    return msg;
}

/**
 * Get the change in the X-coordinate.
 * @return change in the X-coordinate.
 *
 */
int32_t
MouseEvent::getDx() const
{
    return this->dx;
}

/**
 * Get the change in the Y-coordinate.
 * @return change in the Y-coordinate.
 *
 */
int32_t
MouseEvent::getDy() const
{
    return this->dy;
}

/**
 * Get the type of mouse event.
 * @return Type of mouse event.
 *
 */
MouseEventTypeEnum::Enum
MouseEvent::getMouseEventType() const
{
    return this->mouseEventType;
}

/**
 * Get the X-coordinate of the mouse.
 * @return The X-coordinate.
 *
 */
int32_t
MouseEvent::getX() const
{
    return this->x;
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
    return this->y;
}

/**
 * Get the amount of rotation in the mouse wheel.
 * @return  Amount mouse wheel rotated.
 *
 */
int32_t
MouseEvent::getWheelRotation() const
{
    return this->wheelRotation;
}

/**
 * @return Are any keys down?
 */
bool 
MouseEvent::isAnyKeyDown() const
{
    const bool anyKeyDown = (this->keyDownControlAndShift
                             || this->keyDownControl
                             || this->keyDownShift);
    
    return anyKeyDown;
}

/**
 * @return Is the CONTROL AND SHIFT key down?
 */
bool 
MouseEvent::isControlAndShiftKeyDown() const
{
    return this->keyDownControlAndShift;
}

/**
 * @return Is the CONTROL key down?
 */
bool 
MouseEvent::isControlKeyDown() const
{
    return this->keyDownControl;
}

/**
 * @return Is the SHIFT key down?
 */
bool 
MouseEvent::isShiftKeyDown() const
{
    return this->keyDownShift;
}

/**
 * @return Is a graphics update of the window
 * after processing the mouse event requested?
 */
bool 
MouseEvent::isGraphicsUpdateOneWindowRequested() const
{
    return this->graphicsUpdateOneWindowRequested;
}

/**
 * Request a graphics update of the window after
 * the mouse event has been processed.
 */
void 
MouseEvent::setGraphicsUpdateOneWindowRequested()
{
    this->graphicsUpdateOneWindowRequested = true;
}

/**
 * @return Is a graphics update of all windows
 * after processing the mouse event requested?
 */
bool 
MouseEvent::isGraphicsUpdateAllWindowsRequested() const
{
    return this->graphicsUpdateAllWindowsRequested;
}

/**
 * Request a graphics update of all windows after
 * the mouse event has been processed.
 */
void 
MouseEvent::setGraphicsUpdateAllWindowsRequested()
{
    this->graphicsUpdateAllWindowsRequested = true;
}

/**
 * @return Is a user-interface update after
 * the processing the mouse event requested?
 */
bool 
MouseEvent::isUserInterfaceUpdateRequested() const
{
    return this->userInterfaceUpdateRequested;
}

/**
 * Request a user-interface update after the
 * mouse event has been processed.
 */
void 
MouseEvent::setUserInterfaceUpdateRequested()
{
    this->userInterfaceUpdateRequested = true;
}


