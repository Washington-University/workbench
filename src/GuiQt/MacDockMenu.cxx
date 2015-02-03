
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

#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "EventBrowserWindowNew.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"

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
MacDockMenu::MacDockMenu(QWidget* parent)
: QMenu(parent)
{
    const int32_t firstRecentSpecFileIndex = actions().size();
    const int32_t numRecentSpecFiles = BrainBrowserWindow::loadRecentSpecFileMenu(this);
    if (numRecentSpecFiles > 0) {
        addSeparator();
    }
    
    QList<QAction*> menuActions = actions();
    
    const int32_t numberOfRecentSpecFileActions = menuActions.size() - firstRecentSpecFileIndex;
    for (int32_t i = firstRecentSpecFileIndex; i < numberOfRecentSpecFileActions; i++) {
        m_recentSpecFileActions.push_back(menuActions.at(i));
    }
    
    m_newBrowserWindowAction = addAction("New Window");
    
    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(menuActionTriggered(QAction*)));
}

/**
 * Destructor.
 */
MacDockMenu::~MacDockMenu()
{
    std::cout << "Mac Dock Menu Deleted" << std::endl;
}

/**
 * Called when an item in the menu is selected.
 * 
 * @param action
 *     Menu's action that was triggered.
 */
void
MacDockMenu::menuActionTriggered(QAction* action)
{
    if (action == m_newBrowserWindowAction) {
        createNewBrowserWindow();
    }
    else if (std::find(m_recentSpecFileActions.begin(),
                       m_recentSpecFileActions.end(),
                       action) != m_recentSpecFileActions.end()) {
        const AString specFileName = action->data().toString();
        std::cout << "Recent spec file: " << qPrintable(specFileName) << std::endl;
        std::vector<AString> fileNamesToLoad;
        fileNamesToLoad.push_back(specFileName);
        
        BrainBrowserWindow* browserWindow = GuiManager::get()->getActiveBrowserWindow();
        browserWindow->loadFilesFromCommandLine(fileNamesToLoad,
                                                BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG);
    }
    else if (action != NULL) {
        CaretAssertMessage(0, ("Mac Dock menu action not processed: "
                               + action->text()));
    }
}

/**
 * Factory method to create/update the Mac Dock Menu.
 * On non-Mac platforms, this method does nothing.
 */
void
MacDockMenu::createUpdateMacDockMenu()
{
#ifdef CARET_OS_MACOSX
    MacDockMenu* menu = new MacDockMenu();
    qt_mac_set_dock_menu(menu);
#endif // CARET_OS_MACOSX      const AString specFileName = itemAction->data().toString();

}



/**
 * Gets called to create a new browser window.
 */
void
MacDockMenu::createNewBrowserWindow()
{
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW,
                                    true);
    BrainBrowserWindow* browserWindow = GuiManager::get()->getActiveBrowserWindow();
    EventBrowserWindowNew eventNewBrowser(browserWindow, NULL);
    EventManager::get()->sendEvent(eventNewBrowser.getPointer());
    if (eventNewBrowser.isError()) {
        QMessageBox::critical(browserWindow,
                              "",
                              eventNewBrowser.getErrorMessage());
        return;
    }
    const int32_t newWindowIndex = eventNewBrowser.getBrowserWindowCreated()->getBrowserWindowIndex();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(newWindowIndex).getPointer());
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW,
                                    false);
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(newWindowIndex).getPointer());
}
