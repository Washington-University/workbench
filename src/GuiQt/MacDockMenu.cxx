
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
#include "CaretAssert.h"
#include "EventBrowserWindowNew.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SpecFile.h"

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
    /*
     * Name of spec file
     */
    const AString specFileName = GuiManager::get()->getBrain()->getSpecFile()->getFileNameNoPath();
    
    /*
     * Open Windows
     */
    const std::vector<BrainBrowserWindow*> browserWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    if ( ! browserWindows.empty()) {
        for (std::vector<BrainBrowserWindow*>::const_iterator bwIter = browserWindows.begin();
             bwIter != browserWindows.end();
             bwIter++) {
            const BrainBrowserWindow* bbw = *bwIter;
            const AString title = (bbw->windowTitle()
                                   + " "
                                   + specFileName);
            
            
            QAction* action = addAction(title);
            action->setData(bbw->getBrowserWindowIndex());
            
            m_browserWindowActions.push_back(action);
        }
        
        addSeparator();
    }
    
    /*
     * Create New Window
     */
    m_newBrowserWindowAction = addAction("New Window");
    
    /*
     * Connect the menu to a slot for processing menu selections
     */
    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(menuActionTriggered(QAction*)));
}

/**
 * Destructor.
 */
MacDockMenu::~MacDockMenu()
{
    std::cout << "Deleting Mac Dock Menu" << std::endl;
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
        std::vector<AString> fileNamesToLoad;
        fileNamesToLoad.push_back(specFileName);
        
        BrainBrowserWindow* browserWindow = GuiManager::get()->getActiveBrowserWindow();
        browserWindow->loadFilesFromCommandLine(fileNamesToLoad,
                                                BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG);
    }
    else if (std::find(m_browserWindowActions.begin(),
                       m_browserWindowActions.end(),
                       action) != m_browserWindowActions.end()) {
        const int32_t browserWindowIndex = action->data().toInt();
        
        BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
        if (bbw != NULL) {
            bbw->activateWindow();
            bbw->raise();
        }
    }
    else if (action != NULL) {
        CaretAssertMessage(0, ("Mac Dock menu action not processed: "
                               + action->text()));
    }
}

/**
 * Factory method to create/update the Mac Dock Menu.
 * On non-Mac platforms, this method does nothing.
 *
 * With Qt Menus, the "aboutToShow()" signal is used to
 * update the content of the menu so that the menu
 * can be updated just before it is displayed.  However,
 * when a menu is added to the Dock, it is converted
 * to a platform native menu so the "aboutToShow()" signal
 * does not get issued.  So, in Workbench, a Workbench event
 * is issued when something happens that requires an 
 * update to the Dock Menu.  This results in a new menu
 * being created and replacing the current Dock menu.
 */
void
MacDockMenu::createUpdateMacDockMenu()
{
    const bool DISABLE_DOCK_MENU = true;
    if (DISABLE_DOCK_MENU) {
        return;
    }
#ifdef CARET_OS_MACOSX
    BrainBrowserWindow* browserWindow = GuiManager::get()->getActiveBrowserWindow();
    if (browserWindow == NULL) {
        return;
    }
    
    MacDockMenu* menu = new MacDockMenu();
    qt_mac_set_dock_menu(menu);
    
    /*
     * Previously created menus probably should be deleted but 
     * it is not clear if QT takes ownership of the menu
     * which could result in a "double delete".  In Qt 4.8.x,
     * the code does not show the menu being deleted at
     * any time but if that changes a crash would result.
     *
     * Another problem is that selection from the Dock
     * menu may cause the Dock menu to get recreated.
     */
//    if (s_previousMacDockMenu != NULL) {
//        delete s_previousMacDockMenu;
//    }
//    s_previousMacDockMenu = menu;
#endif // CARET_OS_MACOSX

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
