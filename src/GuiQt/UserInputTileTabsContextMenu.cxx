
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

#define __USER_INPUT_TILE_TABS_CONTEXT_MENU_DECLARE__
#include "UserInputTileTabsContextMenu.h"
#undef __USER_INPUT_TILE_TABS_CONTEXT_MENU_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserWindowTileTabOperation.h"
#include "EventManager.h"
#include "GuiManager.h"

using namespace caret;


    
/**
 * \class caret::UserInputTileTabsContextMenu 
 * \brief Menu for tile tabs operations in a window context menu
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentWidget
 *     Parent widget on which error message dialogs are displayed
 * @param viewportContent
 *     Content of the viewport under the mouse.
 */
UserInputTileTabsContextMenu::UserInputTileTabsContextMenu(QWidget* parentWidget,
                                                           BrainOpenGLViewportContent* viewportContent)
: QMenu("Tabs"),
m_parentWidget(parentWidget),
m_windowIndex(viewportContent->getWindowIndex()),
m_tabIndex(viewportContent->getTabIndex())
{
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_windowIndex);
    CaretAssert(bbw);
    CaretAssert(m_windowIndex >= 0);
    CaretAssert(m_tabIndex >= 0);
    
    EventBrowserTabGet tabContentEvent(m_tabIndex);
    EventManager::get()->sendEvent(tabContentEvent.getPointer());
    BrowserTabContent* tabContent = tabContentEvent.getBrowserTab();
    if (tabContent != NULL) {
        const AString tabNumberText(AString::number(m_tabIndex + 1));
        
        if (bbw->isTileTabsSelected()) {
            m_createNewTabBeforeAction = addAction("Create New Tab Before This Tab");
            m_createNewTabAfterAction = addAction("Create New Tab After This Tab");
            m_selectTabAction = addAction("Select This Tab");
        }
        
        QObject::connect(this, &QMenu::triggered,
                         this, &UserInputTileTabsContextMenu::actionTriggered);
    }
}

/**
 * Destructor.
 */
UserInputTileTabsContextMenu::~UserInputTileTabsContextMenu()
{
}

/**
 * @return Is this menu valid?
 */
bool
UserInputTileTabsContextMenu::isValid() const
{
    return (actions().size() > 0);
}


/**
 * Gets called when an action is selected from the menu.
 * 
 * @param action
 *      Action selected by user.
 */
void
UserInputTileTabsContextMenu::actionTriggered(QAction* action)
{
    EventBrowserWindowTileTabOperation::Operation operation = EventBrowserWindowTileTabOperation::OPERATION_SELECT_TAB;
    bool validOperationFlag = false;
    
    if (action == m_createNewTabBeforeAction) {
        operation = EventBrowserWindowTileTabOperation::OPERATION_NEW_TAB_BEFORE;
        validOperationFlag = true;
    }
    else if (action == m_createNewTabAfterAction) {
        operation = EventBrowserWindowTileTabOperation::OPERATION_NEW_TAB_AFTER;
        validOperationFlag = true;
    }
    else if (action == m_selectTabAction) {
        operation = EventBrowserWindowTileTabOperation::OPERATION_SELECT_TAB;
        validOperationFlag = true;
    }
    else if (action != NULL) {
        CaretAssertMessage(0, "Invalid menu action.  Has a new menu action been added?");
    }

    if (validOperationFlag) {
        std::vector<BrowserTabContent*> emptyBrowserTabs;
        EventBrowserWindowTileTabOperation tileTabOperation(operation,
                                                            m_parentWidget,
                                                            m_windowIndex,
                                                            m_tabIndex,
                                                            emptyBrowserTabs);
        EventManager::get()->sendEvent(tileTabOperation.getPointer());
    }
}

