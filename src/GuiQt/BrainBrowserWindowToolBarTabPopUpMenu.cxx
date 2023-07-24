
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_POP_UP_MENU_DECLARE__
#include "BrainBrowserWindowToolBarTabPopUpMenu.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_POP_UP_MENU_DECLARE__

#include "AnnotationBrowserTab.h"
#include "AnnotationManager.h"
#include "Brain.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserWindowContent.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQTabBar.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarTabPopUpMenu 
 * \brief Pop up (context-sensitive) menu for tab in toolbar to add new tabs
 * \ingroup GuiQt
 *
 * This menu is displayed when the user performs a right-click
 * with the mouse over a tab in the brain browswer window toolbar.
 * The menu allows users to add/duplicate/remove tabs.
 */

/**
 * Constructor.
 * 
 * @param toolBar
 *     The toolbar.
 * @param tabIndexUnderMouse
 *     Index of tab over which mouse was clicked
 * @param parent
 *     Optional parent widget.
 */
BrainBrowserWindowToolBarTabPopUpMenu::BrainBrowserWindowToolBarTabPopUpMenu(BrainBrowserWindowToolBar* toolBar,
                                                                             const int32_t tabIndexUnderMouse,
                                                                             QWidget* parent)
: QMenu(parent),
m_toolBar(toolBar),
m_tabIndexUnderMouse(tabIndexUnderMouse)
{
    CaretAssert(m_toolBar);
    CaretAssert(m_toolBar->tabBar);
    
    m_numberOfTabs   = m_toolBar->tabBar->count();
    m_activeTabIndex = m_toolBar->tabBar->currentIndex();
    
    m_browserWindowIndex = m_toolBar->browserWindowIndex;
    std::unique_ptr<EventBrowserWindowContent> windowContentEvent = EventBrowserWindowContent::getWindowContent(m_browserWindowIndex);
    EventManager::get()->sendEvent(windowContentEvent->getPointer());
    const BrowserWindowContent* browserWindowContent = windowContentEvent->getBrowserWindowContent();
    
    if (m_tabIndexUnderMouse >= 0) {
        BrowserTabContent* tabContent = m_toolBar->getTabContentFromTab(m_tabIndexUnderMouse);
        m_selectedBrowserTabAnnotation = NULL;
        m_browserTabContentIndex = -1;
        if (tabContent != NULL) {
            m_browserTabContentIndex = tabContent->getTabNumber();

            m_selectedBrowserTabAnnotation = tabContent->getManualLayoutBrowserTabAnnotation();
        }
    }
    
    if (browserWindowContent != NULL) {
        if (browserWindowContent->isManualModeTileTabsConfigurationEnabled()) {
            if (m_selectedBrowserTabAnnotation != NULL) {
                addItem(MenuItem::MANUAL_LAYOUT_SELECT_FOR_EDITING);

                if (m_selectedBrowserTabAnnotation->isBrowserTabDisplayed()) {
                    addItem(MenuItem::MANUAL_LAYOUT_SET_VISIBLE,
                            "Hide Tab Content in Window");
                }
                else {
                    addItem(MenuItem::MANUAL_LAYOUT_SET_VISIBLE,
                            "Show Tab Content in Window");
                }
                addSeparator();
            }
        }
    }
    
    addItem(MenuItem::CREATE_NEW_TAB_BEFORE);
    addItem(MenuItem::CREATE_NEW_TAB_AFTER);
    addSeparator();
    addItem(MenuItem::DUPLICATE_TAB_AT_BEGINNING);
    addItem(MenuItem::DUPLICATE_TAB_BEFORE);
    addItem(MenuItem::DUPLICATE_TAB_AFTER);
    addItem(MenuItem::DUPLICATE_TAB_AT_END);
    addSeparator();
    addItem(MenuItem::MOVE_TAB_TO_BEGINNING);
    addItem(MenuItem::MOVE_TAB_BEFORE);
    addItem(MenuItem::MOVE_TAB_AFTER);
    addItem(MenuItem::MOVE_TAB_TO_END);
    addSeparator();
    addItem(MenuItem::DELETE_TAB);
    
    QObject::connect(this, &QMenu::triggered,
                     this, &BrainBrowserWindowToolBarTabPopUpMenu::menuItemSelected);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarTabPopUpMenu::~BrainBrowserWindowToolBarTabPopUpMenu()
{
}

/**
 * Add an item to the menu.
 *
 * @param menuItem
 *     Item to be added to menu.
 */
void
BrainBrowserWindowToolBarTabPopUpMenu::addItem(const MenuItem menuItem,
                                               const QString& overrideMenuItemText)
{
    
    QString thisTabName = "This Tab";
    QString activeTabName = "Active Tab";
    
    const int32_t activeTabIndex = m_toolBar->tabBar->currentIndex();
    const bool includeNamesInMenuFlag = false;
    if (includeNamesInMenuFlag) {
        if (m_tabIndexUnderMouse >= 0) {
            thisTabName = ("Tab \"" + m_toolBar->tabBar->tabText(m_tabIndexUnderMouse) + "\"");
        }
        if (activeTabIndex >= 0) {
            activeTabName = ("Tab \"" + m_toolBar->tabBar->tabText(activeTabIndex) + "\"");
        }
    }
    
    QString text;
    switch (menuItem) {
        case MenuItem::NONE:
            CaretAssert(0);
            break;
        case MenuItem::CREATE_NEW_TAB_BEFORE:
            text = "Create New Tab Before " + thisTabName;
            break;
        case MenuItem::CREATE_NEW_TAB_AFTER:
            text = "Create New Tab After " + thisTabName;
            break;
        case MenuItem::DUPLICATE_TAB_AT_BEGINNING:
            text = "Duplicate " + thisTabName + " at Beginning";
            break;
        case MenuItem::DUPLICATE_TAB_BEFORE:
            text = "Duplicate " + thisTabName + " Before " + activeTabName;
            break;
        case MenuItem::DUPLICATE_TAB_AFTER:
            text = "Duplicate " + thisTabName + " After " + activeTabName;
            break;
        case MenuItem::DUPLICATE_TAB_AT_END:
            text = "Duplicate " + thisTabName + " at End";
            break;
        case MenuItem::MANUAL_LAYOUT_SELECT_FOR_EDITING:
            text = "Select for Manual Layout Editing";
            break;
        case MenuItem::MANUAL_LAYOUT_SET_VISIBLE:
            text = "Visible in Manual Layout";
            break;
        case MenuItem::MOVE_TAB_TO_BEGINNING:
            text = "Move " + thisTabName + " to Beginning";
            break;
        case MenuItem::MOVE_TAB_BEFORE:
            text = "Move " + thisTabName + " to Before " + activeTabName;
            break;
        case MenuItem::MOVE_TAB_AFTER:
            text = "Move " + thisTabName + " to After " + activeTabName;
            break;
        case MenuItem::MOVE_TAB_TO_END:
            text = "Move " + thisTabName + " to End";
            break;
        case MenuItem::DELETE_TAB:
            text = "Delete " + thisTabName;
            break;
    }
    
    if ( ! overrideMenuItemText.isEmpty()) {
        text = overrideMenuItemText;
    }
    
    QAction* action = addAction(text);
    action->setData(static_cast<int>(menuItem));
    action->setEnabled(isEnabled(menuItem));
}

/**
 * Called when a menu item is selected.
 *
 * @param action
 *     Action for item in menu that was selected.
 */
void
BrainBrowserWindowToolBarTabPopUpMenu::menuItemSelected(QAction* action)
{
    if (action != NULL) {
        bool updateGraphicsFlag(false);
        bool updateUserIterfaceFlag(false);
        
        const BrowserTabContent* activeTabContent = m_toolBar->getTabContentFromTab(m_activeTabIndex);
        
        MenuItem menuItem = static_cast<MenuItem>(action->data().toInt());
        
        CaretAssert(isEnabled(menuItem));
        
        int32_t createNewIndex   = -1;
        int32_t deleteIndex      = -1;
        int32_t duplicateToIndex = -1;
        int32_t moveToIndex      = -1;
        
        switch (menuItem) {
            case MenuItem::NONE:
                CaretAssert(0);
                break;
            case MenuItem::CREATE_NEW_TAB_BEFORE:
                createNewIndex = m_tabIndexUnderMouse;
                break;
            case MenuItem::CREATE_NEW_TAB_AFTER:
                createNewIndex = m_tabIndexUnderMouse + 1;
                break;
            case MenuItem::DUPLICATE_TAB_AT_BEGINNING:
                duplicateToIndex = 0;
                break;
            case MenuItem::DUPLICATE_TAB_BEFORE:
                duplicateToIndex = m_activeTabIndex;
                break;
            case MenuItem::DUPLICATE_TAB_AFTER:
                duplicateToIndex = m_activeTabIndex + 1;
                break;
            case MenuItem::DUPLICATE_TAB_AT_END:
                duplicateToIndex = m_numberOfTabs;
                break;
            case MenuItem::MANUAL_LAYOUT_SELECT_FOR_EDITING:
                if (m_selectedBrowserTabAnnotation != NULL) {
                    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
                    CaretAssert(annMan);
                    annMan->selectAnnotationForEditing(m_browserWindowIndex,
                                                       AnnotationManager::SELECTION_MODE_EXTENDED,
                                                       true,
                                                       m_selectedBrowserTabAnnotation);
                    updateGraphicsFlag = true;
                    updateUserIterfaceFlag = true;
                }
                break;
            case MenuItem::MANUAL_LAYOUT_SET_VISIBLE:
                if (m_selectedBrowserTabAnnotation != NULL) {
                    m_selectedBrowserTabAnnotation->setBrowserTabDisplayed( ! m_selectedBrowserTabAnnotation->isBrowserTabDisplayed());
                    updateGraphicsFlag = true;
                    updateUserIterfaceFlag = true;
                }
                break;
            case MenuItem::MOVE_TAB_TO_BEGINNING:
                moveToIndex = 0;
                break;
            case MenuItem::MOVE_TAB_BEFORE:
                if (m_tabIndexUnderMouse < m_activeTabIndex) {
                    moveToIndex = m_activeTabIndex - 1;
                }
                else {
                    moveToIndex = m_activeTabIndex;
                }
                break;
            case MenuItem::MOVE_TAB_AFTER:
                if (m_tabIndexUnderMouse < m_activeTabIndex) {
                    moveToIndex = m_activeTabIndex;
                }
                else {
                    moveToIndex = m_activeTabIndex + 1;
                }
                break;
            case MenuItem::MOVE_TAB_TO_END:
                moveToIndex = m_numberOfTabs - 1;
                break;
            case MenuItem::DELETE_TAB:
                deleteIndex = m_tabIndexUnderMouse;
                break;
        }
        
        if (createNewIndex >= 0) {
            m_toolBar->insertNewTabAtTabBarIndex(createNewIndex);
        }
        else if (deleteIndex >= 0) {
            m_toolBar->tabCloseSelected(deleteIndex);
        }
        else if (duplicateToIndex >= 0) {
            BrowserTabContent* tabContent = m_toolBar->getTabContentFromTab(m_tabIndexUnderMouse);
            CaretAssert(tabContent);
            m_toolBar->insertAndCloneTabContentAtTabBarIndex(tabContent, duplicateToIndex);
        }
        else if (moveToIndex >= 0) {
            if (moveToIndex != m_tabIndexUnderMouse) {
                m_toolBar->tabBar->moveTab(m_tabIndexUnderMouse, moveToIndex);
            }
        }
        
        /*
         * Ensure active tab remains active.  Note that number of tabs may have changed.
         */
        int32_t updatedActiveTabIndex = -1;
        const int32_t numTabs = m_toolBar->tabBar->count();
        for (int32_t iTab = 0; iTab < numTabs; iTab++) {
            if (activeTabContent == m_toolBar->getTabContentFromTab(iTab)) {
                updatedActiveTabIndex = iTab;
                break;
            }
        }
        if (updatedActiveTabIndex >= 0) {
            m_toolBar->tabBar->setCurrentIndex(updatedActiveTabIndex);
        }
        
        if (updateGraphicsFlag) {
            EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
        }
        if (updateUserIterfaceFlag) {
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        }
    }
}

/**
 * @return True if the menu item is enabled.
 * Items are enabled if action would do nothing such as
 * moving a tab to itself.
 *
 * @param item
 *     Item for testing enabled.
 */
bool
BrainBrowserWindowToolBarTabPopUpMenu::isEnabled(const MenuItem menuItem) const
{
    bool enabledFlag = false;
    
    BrowserTabContent* tabContent = m_toolBar->getTabContentFromTab(m_tabIndexUnderMouse);
    
    switch (menuItem) {
        case MenuItem::NONE:
            CaretAssert(0);
            break;
        case MenuItem::CREATE_NEW_TAB_BEFORE:
            enabledFlag = true;
            break;
        case MenuItem::CREATE_NEW_TAB_AFTER:
            enabledFlag = true;
            break;
        case MenuItem::DUPLICATE_TAB_AT_BEGINNING:
        case MenuItem::DUPLICATE_TAB_BEFORE:
        case MenuItem::DUPLICATE_TAB_AFTER:
        case MenuItem::DUPLICATE_TAB_AT_END:
            if (tabContent != NULL) {
                enabledFlag = true;
            }
            break;
        case MenuItem::MANUAL_LAYOUT_SELECT_FOR_EDITING:
        {
            EventGetOrSetUserInputModeProcessor inputProcessorEvent(m_browserWindowIndex);
            EventManager::get()->sendEvent(inputProcessorEvent.getPointer());
            switch (inputProcessorEvent.getUserInputMode()) {
                case UserInputModeEnum::Enum::ANNOTATIONS:
                    break;
                case UserInputModeEnum::Enum::BORDERS:
                    break;
                case UserInputModeEnum::Enum::FOCI:
                    break;
                case UserInputModeEnum::Enum::IMAGE:
                    break;
                case UserInputModeEnum::Enum::INVALID:
                    break;
                case UserInputModeEnum::Enum::SAMPLES_EDITING:
                    break;
                case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
                    if (m_selectedBrowserTabAnnotation != NULL) {
                        enabledFlag = true;
                    }
                    break;
                case UserInputModeEnum::Enum::VIEW:
                    break;
                case UserInputModeEnum::Enum::VOLUME_EDIT:
                    break;
            }
        }
            break;
        case MenuItem::MANUAL_LAYOUT_SET_VISIBLE:
            enabledFlag = (m_selectedBrowserTabAnnotation != NULL);
            break;
        case MenuItem::MOVE_TAB_TO_BEGINNING:
            if (m_tabIndexUnderMouse > 0) {
                enabledFlag = true;
            }
            break;
        case MenuItem::MOVE_TAB_BEFORE:
            if ((m_tabIndexUnderMouse != m_activeTabIndex)
                && (m_tabIndexUnderMouse != (m_activeTabIndex - 1))) {
                enabledFlag = true;
            }
            break;
        case MenuItem::MOVE_TAB_AFTER:
            if ((m_tabIndexUnderMouse != m_activeTabIndex)
                && (m_tabIndexUnderMouse != (m_activeTabIndex + 1))) {
                enabledFlag = true;
            }
            break;
        case MenuItem::MOVE_TAB_TO_END:
            if (m_tabIndexUnderMouse != (m_numberOfTabs - 1)) {
                enabledFlag = true;
            }
            break;
        case MenuItem::DELETE_TAB:
            enabledFlag = true;
            break;
    }
    
    return enabledFlag;
}


