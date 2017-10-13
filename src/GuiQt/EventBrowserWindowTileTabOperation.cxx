
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION_DECLARE__
#include "EventBrowserWindowTileTabOperation.h"
#undef __EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION_DECLARE__

#include <QWidget>

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventTypeEnum.h"
#include "WuQMessageBox.h"
using namespace caret;


    
/**
 * \class caret::EventBrowserWindowTileTabOperation 
 * \brief Operations for tab changes
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param operation
 *    The operation
 * @param parentWidget
 *    Parent widget for error dialogs.
 * @param windowIndex
 *    Index of the window.
 * @param browserTabIndex
 *    Index of the browser tab.
 */
EventBrowserWindowTileTabOperation::EventBrowserWindowTileTabOperation(const Operation operation,
                                                                       QWidget* parentWidget,
                                                                       const int32_t windowIndex,
                                                                       const int32_t browserTabIndex)
: Event(EventTypeEnum::EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION),
m_operation(operation),
m_parentWidget(parentWidget),
m_windowIndex(windowIndex),
m_browserTabIndex(browserTabIndex)
{
    CaretAssert(m_parentWidget);
    CaretAssert(m_windowIndex >= 0);
    CaretAssert(m_browserTabIndex >= 0);
}

/**
 * Destructor.
 */
EventBrowserWindowTileTabOperation::~EventBrowserWindowTileTabOperation()
{
}

/**
 * Operation that selects the given tab in the given window.
 *
 * @param parentWidget
 *    Parent widget for error dialogs.
 * @param windowIndex
 *    Index of the window.
 * @param browserTabIndex
 *    Index of the browser tab.
 */
void
EventBrowserWindowTileTabOperation::selectTabInWindow(QWidget* parentWidget,
                                                      const int32_t windowIndex,
                                                      const int32_t browserTabIndex)
{
    EventBrowserWindowTileTabOperation tabOperation(Operation::OPERATION_SELECT_TAB,
                                                    parentWidget,
                                                    windowIndex,
                                                    browserTabIndex);
    
    EventManager::get()->sendEvent(tabOperation.getPointer());
    
    if (tabOperation.getEventProcessCount() <= 0) {
        WuQMessageBox::errorOk(parentWidget,
                               "Tab not created, invalid window or tab index");
    }
}

/**
 * @return The mode
 */
EventBrowserWindowTileTabOperation::Operation
EventBrowserWindowTileTabOperation::getOperation() const
{
    return m_operation;
}

/**
 * @return The window index.
 */
int32_t
EventBrowserWindowTileTabOperation::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return The browser tab index.
 */
int32_t
EventBrowserWindowTileTabOperation::getBrowserTabIndex() const
{
    return m_browserTabIndex;
}


