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

#include <QKeyEvent>
#include <QWheelEvent>

#include "KeyEvent.h"

using namespace caret;

/**
 * \class caret::KeyEvent
 * \brief Event issued when key is moved or buttons are pressed.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param openGLWidget
 *    OpenGL Widget in which key activity took place.
 * @param browserWindowIndex
 *    Index of the browser winddow in which key activity took place.
 * @param keyCode
 *    The key that was pressed.  See Qt::Key for list of codes.
 */
KeyEvent::KeyEvent(BrainOpenGLWidget* openGLWidget,
                   const int32_t browserWindowIndex,
                   const int32_t keyCode,
                   const bool firstKeyPressFlag)
: CaretObject(),
m_openGLWidget(openGLWidget),
m_browserWindowIndex(browserWindowIndex),
m_keyCode(keyCode),
m_firstKeyPressFlag(firstKeyPressFlag)
{
}

/**
 * Destructor
 */
KeyEvent::~KeyEvent()
{
}

/**
 * Initialize all members.
 */
void
KeyEvent::initializeMembersKeyEvent()
{
}

/**
 * @return The OpenGL Widget in which the key event occurred.
 */
BrainOpenGLWidget*
KeyEvent::getOpenGLWidget() const
{
    return m_openGLWidget;
}

/**
 * Get a string showing the contents of this key event.
 * @return String describing the key status.
 */
AString
KeyEvent::toString() const
{
    const AString msg = 
     ", keyCord=" + AString::number(m_keyCode);
    
    return msg;
}

/**
 * @return Index of the browser window in which the
 * event took place.
 */
int32_t 
KeyEvent::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

/**
 * Get the key code.  See Qt::Key for list of codes.
 * @return  The key code.
 *
 */
int32_t
KeyEvent::getKeyCode() const
{
    return m_keyCode;
}

/**
 * @return Is this the first (of possibly many) key press events?
 * If a key is held down for a period of time, this method
 * will return true for only the first key press events.
 *
 */
bool
KeyEvent::isFirstKeyPressFlag() const
{
    return m_firstKeyPressFlag;
}

