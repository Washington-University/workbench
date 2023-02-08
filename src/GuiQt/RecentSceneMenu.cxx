
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __RECENT_SCENE_MENU_DECLARE__
#include "RecentSceneMenu.h"
#undef __RECENT_SCENE_MENU_DECLARE__

#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "FileInformation.h"
#include "SessionManager.h"

using namespace caret;
    
/**
 * \class caret::RecentSceneMenu 
 * \brief Menu for loading a recent scene
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param menuLocation
 *    Location of menu
 * @param brainBrowserWindow
 *    Parent brain browser window
 * @param parent
 *    The parent widget
 */
RecentSceneMenu::RecentSceneMenu(const MenuLocation menuLocation,
                                 BrainBrowserWindow* brainBrowserWindow,
                                 QWidget* parentWidget)
: QMenu(parentWidget),
m_menuLocation(menuLocation),
m_brainBrowserWindow(brainBrowserWindow)
{
    CaretAssert(m_brainBrowserWindow);
    
    setTitle("Open Recent Scene");
    
    QObject::connect(this, &QMenu::aboutToShow,
                     this, &RecentSceneMenu::menuAboutToShow);
    QObject::connect(this, &QMenu::triggered,
                     this, &RecentSceneMenu::menuTriggered);
}

/**
 * Destructor.
 */
RecentSceneMenu::~RecentSceneMenu()
{
}

/**
 * Called when a menu item is selected
 * @param action
 *    Menu item selected or NULL if no selectionl
 */
void
RecentSceneMenu::menuTriggered(QAction* action)
{
    if (action == NULL) {
        return;
    }
    
    QVariant variant(action->data());
    
    bool validFlag(false);
    RecentSceneInfoContainer rsic(variant,
                                  validFlag);
    
    if (validFlag) {
        m_brainBrowserWindow->loadRecentScene(rsic.getSceneFileName(),
                                              rsic.getSceneName());
    }
}

/**
 * Called when the menu is about to show.
 */
void
RecentSceneMenu::menuAboutToShow()
{
    clear();
    
    /*
     * Note: On MacOS, tooltips do not work on menus
     * in the menubar but do work in menus in tool buttons
     * in the toolbar.
     */
    bool addToolTips(false);
    switch (m_menuLocation) {
        case MenuLocation::FILE_MENU:
            break;
        case MenuLocation::TOOLBAR_SCENE_BUTTON:
            addToolTips = true;
            break;
    }
    
    std::vector<RecentSceneInfoContainer> recentSceneInfo;
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    preferences->getMostRecentScenes(recentSceneInfo);
    for (auto& rsi : recentSceneInfo) {
        QAction* action(addAction(rsi.getSceneName()));
        action->setData(rsi.toQVariant());
        
        if (addToolTips) {
            FileInformation fileInfo(rsi.getSceneFileName());
            action->setToolTip("Loaded from " + fileInfo.getFileName());
        }
    }
    
    if (addToolTips) {
        setToolTipsVisible(true);
    }
}
