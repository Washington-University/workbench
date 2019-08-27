
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

#define __USER_INPUT_MODE_VIEW_CONTEXT_TILE_TABS_SUB_MENU_DECLARE__
#include "UserInputModeViewContextTileTabsSubMenu.h"
#undef __USER_INPUT_MODE_VIEW_CONTEXT_TILE_TABS_SUB_MENU_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserWindowTileTabOperation.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MouseEvent.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeViewContextTileTabsSubMenu
 * \brief Menu for tile tabs operations in a window context menu
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param mouseEvent
 *     The mouse event that caused menu display
 * @param parentWidget
 *     Parent widget on which error message dialogs are displayed
 * @param viewportContent
 *     Content of the viewport under the mouse.
 */
UserInputModeViewContextTileTabsSubMenu::UserInputModeViewContextTileTabsSubMenu(const MouseEvent& mouseEvent,
                                                           QWidget* parentWidget,
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
    
    int32_t tabViewport[4];
    viewportContent->getTabViewportBeforeApplyingMargins(tabViewport);
    
    viewportContent->getWindowViewport(m_windowViewport);

    m_mouseWindowX = mouseEvent.getX();
    m_mouseWindowY = mouseEvent.getY();
    
    EventBrowserTabGet tabContentEvent(m_tabIndex);
    EventManager::get()->sendEvent(tabContentEvent.getPointer());
    BrowserTabContent* tabContent = tabContentEvent.getBrowserTab();
    
    if (bbw->isTileTabsSelected()) {
        switch (bbw->getBrowerWindowContent()->getTileTabsConfigurationMode()) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                break;
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                if (tabContent != NULL) {
                    addItemToMenu("Insert New Tab Before This Tab",
                                  EventBrowserWindowTileTabOperation::OPERATION_GRID_NEW_TAB_BEFORE);
                    addItemToMenu("Insert New Tab After This Tab",
                                  EventBrowserWindowTileTabOperation::OPERATION_GRID_NEW_TAB_AFTER);
                }
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                addItemToMenu("Insert Tab at Mouse",
                              EventBrowserWindowTileTabOperation::OPERATION_MANUAL_NEW_TAB);

                if (tabContent != NULL) {
                    if (actions().count() > 0) {
                        addSeparator();
                    }
                    addItemToMenu("Bring to Front",
                                  EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_TO_FRONT);
                    addItemToMenu("Bring Forward",
                                  EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_FORWARD);
                    addItemToMenu("Send to Back",
                                  EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_TO_BACK);
                    addItemToMenu("Send Backward",
                                  EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_BACKWARD);
                }
                break;
        }
        
        if (tabContent != NULL) {
            if (actions().count() > 0) {
                addSeparator();
            }
            addItemToMenu("Select This Tab",
                          EventBrowserWindowTileTabOperation::OPERATION_SELECT_TAB);
        }
    }
    
    QObject::connect(this, &QMenu::triggered,
                     this, &UserInputModeViewContextTileTabsSubMenu::actionTriggered);
}

/**
 * Destructor.
 */
UserInputModeViewContextTileTabsSubMenu::~UserInputModeViewContextTileTabsSubMenu()
{
}

/**
 * @return Is this menu valid?
 */
bool
UserInputModeViewContextTileTabsSubMenu::isValid() const
{
    return (actions().size() > 0);
}

/**
 * Add an operation to the menu with the given text
 *
 * @param text
 *     Text for menu
 * @param operation
 *     The operation
 */
void
UserInputModeViewContextTileTabsSubMenu::addItemToMenu(const QString& text,
                                            const EventBrowserWindowTileTabOperation::Operation operation)
{
    QAction* action = addAction(text);
    action->setData(static_cast<int>(operation));
}



/**
 * Gets called when an action is selected from the menu.
 * 
 * @param action
 *      Action selected by user.
 */
void
UserInputModeViewContextTileTabsSubMenu::actionTriggered(QAction* action)
{
    if (action == NULL) {
        return;
    }
    
    const int operationInt = action->data().toInt();
    const EventBrowserWindowTileTabOperation::Operation operation = static_cast<EventBrowserWindowTileTabOperation::Operation>(operationInt);
        std::vector<BrowserTabContent*> emptyBrowserTabs;
        EventBrowserWindowTileTabOperation tileTabOperation(operation,
                                                            m_parentWidget,
                                                            m_windowIndex,
                                                            m_tabIndex,
                                                            m_windowViewport,
                                                            m_mouseWindowX,
                                                            m_mouseWindowY,
                                                            emptyBrowserTabs);
        EventManager::get()->sendEvent(tileTabOperation.getPointer());
}

