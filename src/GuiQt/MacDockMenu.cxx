
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include <QMessageBox>

#define __MAC_DOCK_MENU_DECLARE__
#include "MacDockMenu.h"
#undef __MAC_DOCK_MENU_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserWindowNew.h"
#include "EventGraphicsPaintSoonOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SpecFile.h"
#include "SystemUtilities.h"

using namespace caret;


#ifdef CARET_OS_MACOSX
/*
 * Not in Qt Header Files.
 * See http://qt-project.org/doc/qt-4.8/exportedfunctions.html
 */
extern void qt_mac_set_dock_menu(QMenu *menu);
#endif // CARET_OS_MACOSX


/**
 * \class caret::MacDockMenu
 * \brief Menu that is displayed Mac's Dock Menu.
 * \ingroup GuiQt
 *
 * The Mac Dock Menu is displayed when the user either holds the mouse button
 * down or Control-Clicks the icon for a running wb_view instance in the 
 * Mac's Dock.
 */


/**
 * Constructor.
 */
MacDockMenu::MacDockMenu()
: QMenu(0)
{
    /*
     * Will update menu when is "about to be shown
     */
    QObject::connect(this, &QMenu::aboutToShow,
                     this, &MacDockMenu::menuAboutToShow);
        
    /**
     * Tell Qt that this is the Dock Menu (a QMenu method)
     */
#ifdef CARET_OS_MACOSX
    setAsDockMenu();
#endif // CARET_OS_MACOSX
}

/**
 * Destructor.
 */
MacDockMenu::~MacDockMenu()
{
}

/**
 * Called when menu is about to show
 */
void
MacDockMenu::menuAboutToShow()
{
    /*
     * Remove all menu items
     */
    clear();
    
    const AString exeName(SystemUtilities::getExecutableName());
    QAction* startNewWbViewAction = addAction("Start New " + exeName);
    QObject::connect(startNewWbViewAction, &QAction::triggered,
                     this, &MacDockMenu::startNewWbViewMenuItemTriggered);
    
    addBrowserWindowActions();
}

/**
 * Called when start new wb_View menu item is triggered
 */
void
MacDockMenu::startNewWbViewMenuItemTriggered()
{
    QStringList noParameters;
    QString noWorkingDirectory;
    QWidget* parentWidget = GuiManager::get()->getActiveBrowserWindow();
    
    GuiManager::startNewWbViewInstance(noParameters,
                                       noWorkingDirectory,
                                       parentWidget);
}

/**
 * Add actions for selecting browser windows
 */
void
MacDockMenu::addBrowserWindowActions()
{
    /*
     * Only if 2 or more windows open (clicking Dock icon will show single window)
     */
    const std::vector<BrainBrowserWindow*> browserWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    if (browserWindows.size() >= 2) {
        /*
         * Separator from new wb_view item
         */
        addSeparator();
        
        for (const auto bbw : browserWindows) {
            AString title(bbw->windowTitle());
            const BrowserTabContent* selectedTab = bbw->getBrowserTabContent();
            if (selectedTab != NULL) {
                title.append(" "
                             + selectedTab->getTabName());
            }
            const int32_t windowIndex(bbw->getBrowserWindowIndex());
            
            QAction* action = addAction(title);
            QObject::connect(action, &QAction::triggered,
                             this, [=]() { browserWindowActionSelected(windowIndex); });
        }
    }
}

/**
 * Called when a browser window action is selected
 * @param browserWindowIndex
 *    Index of window
 */
void
MacDockMenu::browserWindowActionSelected(const int32_t browserWindowIndex)
{
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
    if (bbw != NULL) {
        bbw->activateWindow();
        bbw->raise();
    }
}

