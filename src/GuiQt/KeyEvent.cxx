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

#include <QCursor>
#include <QKeyEvent>
#include <QWheelEvent>

#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "KeyEvent.h"

using namespace caret;

/**
 * \class caret::KeyEvent
 * \brief Event issued when key is moved or buttons are pressed.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param viewportContent
 *    Viewport content at location of mouse
 * @param openGLWidget
 *    OpenGL Widget in which key activity took place.
 * @param browserWindowIndex
 *    Index of the browser winddow in which key activity took place.
 * @param keyCode
 *    The key that was pressed.  See Qt::Key for list of codes.
 * @param mouseX
 *    Mouse location X
 * @param mouseY
 *    Mouse location Y
 * @param mouseValidFlag
 *    Mouse location valid
 * @param firstKeyPressFlag
 *    True if this is the first key press when key is held down for a period of time
 * @param shiftKeyDownFlag
 *    True if the shift key is down
 */
KeyEvent::KeyEvent(const BrainOpenGLViewportContent* viewportContent,
                   BrainOpenGLWidget* openGLWidget,
                   const int32_t browserWindowIndex,
                   const int32_t keyCode,
                   const int32_t mouseX,
                   const int32_t mouseY,
                   const bool mouseValidFlag,
                   const bool firstKeyPressFlag,
                   const bool shiftKeyDownFlag)
: CaretObject(),
m_openGLWidget(openGLWidget),
m_browserWindowIndex(browserWindowIndex),
m_keyCode(keyCode),
m_mouseX(mouseX),
m_mouseY(mouseY),
m_mouseXYValid(mouseValidFlag),
m_firstKeyPressFlag(firstKeyPressFlag),
m_shiftKeyDownFlag(shiftKeyDownFlag)
{
    CaretAssert(m_openGLWidget);
 
    /*
     * MUST copy viewport content as it may be deleted by caller
     * prior to this instance being deleted
     */
    m_viewportContent = NULL;
    if (viewportContent != NULL) {
        m_viewportContent = new BrainOpenGLViewportContent(*viewportContent);
    }
    
//    const QPoint mousePos = m_openGLWidget->mapFromGlobal(QCursor::pos());
//    m_mouseX = mousePos.x();
//    m_mouseY = m_openGLWidget->height() - mousePos.y();
//    if ((m_mouseX >= 0)
//        && (m_mouseX < m_openGLWidget->width())
//        && (m_mouseY >= 0)
//        && (m_mouseY < m_openGLWidget->height())) {
//        m_mouseXYValid = true;
//    }
//    else {
//        m_mouseXYValid = false;
//    }
}

/**
 * Destructor
 */
KeyEvent::~KeyEvent()
{
    if (m_viewportContent != NULL) {
        delete m_viewportContent;
    }
}

/**
 * Initialize all members.
 */
void
KeyEvent::initializeMembersKeyEvent()
{
}

/**
 * @return The viewport content in which the mouse was pressed.
 */
BrainOpenGLViewportContent*
KeyEvent::getViewportContent() const
{
    return m_viewportContent;
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
/**
 * @return Is this the SHIFT key down?
 *
 */
bool
KeyEvent::isShiftKeyDownFlag() const
{
    return m_shiftKeyDownFlag;
}

/*
 * Get the mouse X/Y cooordinate in the OpenGL widget (origin is bottom left)
 * @param mouseXYOut
 *   Output with mouse X/Y
 * @return True if mouse X/Y is valid, else false
 */
bool
KeyEvent::getMouseXY(std::array<int32_t, 2>& mouseXYOut) const
{
    mouseXYOut[0] = m_mouseX;
    mouseXYOut[1] = m_mouseY;
    return m_mouseXYValid;
}


