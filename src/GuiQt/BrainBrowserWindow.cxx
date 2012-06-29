/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabBar>
#include <QUrl>

#define __BRAIN_BROWSER_WINDOW_DECLARE__
#include "BrainBrowserWindow.h"
#undef __BRAIN_BROWSER_WINDOW_DECLARE__

#include "AboutWorkbenchDialog.h"
#include "Brain.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretFileDialogExtendable.h"
#include "CaretFileRemoteDialog.h"
#include "CaretPreferences.h"
#include "CursorDisplayScoped.h"
#include "DisplayPropertiesVolume.h"
#include "EventBrowserWindowNew.h"
#include "CaretLogger.h"
#include "ElapsedTimer.h"
#include "EventDataFileRead.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ManageLoadedFilesDialog.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SessionManager.h"
#include "SpecFile.h"
#include "SpecFileCreateAddToDialog.h"
#include "SpecFileDialog.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "SurfaceSelectionViewController.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index for this window.
 * @param browserTabContent  
 *    If not NULL, this is the tab displayed in the window.
 *    If NULL, a new tab is created.
 * @param parent
 *    Parent of this object
 * @param flags
 *    Flags for Qt.
 */
BrainBrowserWindow::BrainBrowserWindow(const int browserWindowIndex,
                                       BrowserTabContent* browserTabContent,
                                       QWidget* parent,
                                       Qt::WindowFlags flags)
: QMainWindow(parent, flags)
{
    if (BrainBrowserWindow::firstWindowFlag) {
        BrainBrowserWindow::firstWindowFlag = false;
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        BrainBrowserWindow::previousOpenFileAddToSpecFileSelection = prefs->isDataFileAddToSpecFileEnabled();
    }
    
    this->screenMode = BrainBrowserWindowScreenModeEnum::NORMAL;
    
    GuiManager* guiManager = GuiManager::get();
    
    this->setAttribute(Qt::WA_DeleteOnClose);
    
    this->browserWindowIndex = browserWindowIndex;
    
    this->setWindowTitle(guiManager->applicationName() 
                         + " "
                         + AString::number(this->browserWindowIndex + 1));
    
    this->openGLWidget = new BrainOpenGLWidget(this,
                                               browserWindowIndex);
    
    const int openGLSizeX = 500;
    const int openGLSizeY = (WuQtUtilities::isSmallDisplay() ? 200 : 375);
    this->openGLWidget->setMinimumSize(openGLSizeX, 
                                       openGLSizeY);
    
    this->setCentralWidget(this->openGLWidget);
    
    this->overlayVerticalToolBox = 
    new BrainBrowserWindowOrientedToolBox(this->browserWindowIndex,
                                          "Overlay ToolBox",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_OVERLAYS_VERTICAL,
                                          this);
    this->overlayVerticalToolBox->setAllowedAreas(Qt::LeftDockWidgetArea);
    
    this->overlayHorizontalToolBox = 
    new BrainBrowserWindowOrientedToolBox(this->browserWindowIndex,
                                          "Overlay ToolBox ",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_OVERLAYS_HORIZONTAL,
                                          this);
    this->overlayHorizontalToolBox->setAllowedAreas(Qt::BottomDockWidgetArea);

    if (WuQtUtilities::isSmallDisplay()) {
        this->overlayActiveToolBox = this->overlayVerticalToolBox;
        this->addDockWidget(Qt::LeftDockWidgetArea, this->overlayVerticalToolBox);
        this->overlayHorizontalToolBox->setVisible(false);
        //this->overlayHorizontalToolBox->toggleViewAction()->trigger();
    }
    else {
        this->overlayActiveToolBox = this->overlayHorizontalToolBox;
        this->addDockWidget(Qt::BottomDockWidgetArea, this->overlayHorizontalToolBox);
        this->overlayVerticalToolBox->setVisible(false);
        //this->overlayVerticalToolBox->toggleViewAction()->trigger();
    }
    
    QObject::connect(this->overlayHorizontalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayHorizontalToolBoxVisibilityChanged(bool)));
    QObject::connect(this->overlayVerticalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayVerticalToolBoxVisibilityChanged(bool)));
    
    this->featuresToolBox = 
    new BrainBrowserWindowOrientedToolBox(this->browserWindowIndex,
                                          "Features ToolBox",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_LAYERS,
                                          this);
    this->featuresToolBox->setAllowedAreas(Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, this->featuresToolBox);
    
    this->createActionsUsedByToolBar();
    this->overlayToolBoxAction->blockSignals(true);
    this->overlayToolBoxAction->setChecked(true);
    this->overlayToolBoxAction->blockSignals(false);
    this->featuresToolBoxAction->blockSignals(true);
    this->featuresToolBoxAction->setChecked(true);
    this->featuresToolBoxAction->blockSignals(false);
    
    this->toolbar = new BrainBrowserWindowToolBar(this->browserWindowIndex,
                                                  browserTabContent,
                                                  this->overlayToolBoxAction,
                                                  this->featuresToolBoxAction,
                                                  this);
    this->showToolBarAction = this->toolbar->toolBarToolButtonAction;
    this->addToolBar(this->toolbar);
    
    this->createActions();
    
    this->createMenus();
     
    this->toolbar->updateToolBar();

    this->processShowOverlayToolBox(this->overlayToolBoxAction->isChecked());
    this->processHideFeaturesToolBox();
    
    if (browserTabContent == NULL) {
        this->toolbar->addDefaultTabsAfterLoadingSpecFile();
    }
}

/**
 * Destructor.
 */
BrainBrowserWindow::~BrainBrowserWindow()
{
}

/**
 * @return The screen mode (normal, full screen, montage, etc)
 */
BrainBrowserWindowScreenModeEnum::Enum 
BrainBrowserWindow::getScreenMode() const
{
    return this->screenMode;
}

/**
 * @return the index of this browser window.
 */
int32_t 
BrainBrowserWindow::getBrowserWindowIndex() const 
{ 
    return this->browserWindowIndex; 
}

/**
 * Called when the window is requested to close.
 *
 * @param event
 *     CloseEvent that may or may not be accepted
 *     allowing the window to close.
 */
void 
BrainBrowserWindow::closeEvent(QCloseEvent* event)
{
    /*
     * The GuiManager may warn user about closing the 
     * window and the user may cancel closing of the window.
     */
    GuiManager* guiManager = GuiManager::get();
    if (guiManager->allowBrainBrowserWindowToClose(this,
                                                   this->toolbar->tabBar->count())) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

/**
 * Create actions for this window.
 * NOTE: This is called BEFORE the toolbar is created.
 */
void 
BrainBrowserWindow::createActionsUsedByToolBar()
{
    QIcon featuresToolBoxIcon;
    const bool featuresToolBoxIconValid = WuQtUtilities::loadIcon(":/toolbox.png", 
                                                         featuresToolBoxIcon);
    
    QIcon overlayToolBoxIcon;
    const bool overlayToolBoxIconValid = WuQtUtilities::loadIcon(":/layers_toolbox_icon.png",
                                                                  overlayToolBoxIcon);
    
    /*
     * Note: The name of a dock widget becomes its
     * name in the toggleViewAction().  So, use
     * a separate action here so that the name in 
     * the menu is as set here.
     */
    this->overlayToolBoxAction = 
    WuQtUtilities::createAction("Overlay ToolBox",
                                "Overlay ToolBox",
                                this,
                                this,
                                SLOT(processShowOverlayToolBox(bool)));
    this->overlayToolBoxAction->setCheckable(true);
    if (overlayToolBoxIconValid) {
        this->overlayToolBoxAction->setIcon(overlayToolBoxIcon);
        this->overlayToolBoxAction->setIconVisibleInMenu(false);
    }
    else {
        this->overlayToolBoxAction->setIconText("OT");
    }

    /*
     * Note: The name of a dock widget becomes its
     * name in the toggleViewAction().  So, use
     * a separate action here so that the name in 
     * the menu is as set here.
     */
    this->featuresToolBoxAction = this->featuresToolBox->toggleViewAction();
    this->featuresToolBoxAction->setCheckable(true);
    QObject::connect(this->featuresToolBoxAction, SIGNAL(triggered(bool)),
                     this, SLOT(processShowFeaturesToolBox(bool)));
    if (featuresToolBoxIconValid) {
        this->featuresToolBoxAction->setIcon(featuresToolBoxIcon);
        this->featuresToolBoxAction->setIconVisibleInMenu(false);
    }
    else {
        this->featuresToolBoxAction->setIconText("LT");
    }
    
    /*
     * Scene window.
     */
    QIcon clapBoardIcon;
    const bool clapBoardIconValid = WuQtUtilities::loadIcon(":/toolbar_clapboard_icon.png", 
                                                            clapBoardIcon);
    
    this->showSceneDialogAction = WuQtUtilities::createAction("Scenes...",
                                                          "Displays the Scene Window",
                                                          this,
                                                          this,
                                                          SLOT(processShowSceneDialog()));
    if (clapBoardIconValid) {
        this->showSceneDialogAction->setIcon(clapBoardIcon);
        this->showSceneDialogAction->setIconVisibleInMenu(false);
    }
    else {
        this->showSceneDialogAction->setIconText("Scenes");
    }
    this->showSceneDialogAction->setCheckable(false);
    //    this->sceneDialogAction->blockSignals(true);
    //    this->sceneDialogAction->setCheckable(true);
    //    this->sceneDialogAction->setChecked(false);
    //    this->showHideSceneWindow(this->sceneDialogAction->isChecked());
    //    this->sceneDialogAction->blockSignals(false);

}

/**
 * Show the scene dialog.
 */
void 
BrainBrowserWindow::processShowSceneDialog()
{
    GuiManager::get()->processShowSceneDialog(this);
}

/**
 * Create actions for this window.
 * NOTE: This is called AFTER the toolbar is created.
 */
void 
BrainBrowserWindow::createActions()
{
    CaretAssert(this->toolbar);
    
    GuiManager* guiManager = GuiManager::get();
    
    this->aboutWorkbenchAction =
    WuQtUtilities::createAction("About Workbench...",
                                "Information about Workbench",
                                this,
                                this,
                                SLOT(processAboutWorkbench()));
    
    this->newWindowAction =
    WuQtUtilities::createAction("New Window",
                                "Creates a new window for viewing brain models",
                                Qt::CTRL+Qt::Key_N,
                                this,
                                this,
                                SLOT(processNewWindow()));
    
    this->newTabAction =
    WuQtUtilities::createAction("New Tab", 
                                "Create a new tab (window pane) in the window",
                                Qt::CTRL + Qt::Key_T,
                                this,
                                this,
                                SLOT(processNewTab()));
    
    this->openFileAction =
    WuQtUtilities::createAction("Open File...", 
                                "Open a data file including a spec file located on the computer",
                                Qt::CTRL+Qt::Key_O,
                                this,
                                this,
                                SLOT(processDataFileOpen()));
    
    this->openLocationAction = 
    WuQtUtilities::createAction("Open Location...", 
                                "Open a data file including a spec file located on a web server (http)",
                                Qt::CTRL+Qt::Key_L,
                                this,
                                this,
                                SLOT(processDataFileLocationOpen()));
    
    this->openFileViaSpecFileAction =
    WuQtUtilities::createAction("Open File via Spec File...", 
                                "Open a data file listed in the Spec File",
                                Qt::CTRL + Qt::SHIFT + Qt::Key_O,
                                this,
                                this,
                                SLOT(processDataFileOpenFromSpecFile()));
    
    
    this->manageFilesAction =
    WuQtUtilities::createAction("Save/Manage Files...", 
                                "Save and Manage Loaded Files",
                                Qt::CTRL + Qt::Key_S,
                                this,
                                this,
                                SLOT(processManageSaveLoadedFiles()));
    
    this->closeSpecFileAction =
    WuQtUtilities::createAction("Close Spec File",
                                "Close the Spec File",
                                this,
                                this,
                                SLOT(processCloseSpecFile()));
    
    this->closeTabAction =
    WuQtUtilities::createAction("Close Tab",
                                "Close the active tab (window pane) in the window",
                                Qt::CTRL + Qt::Key_W,
                                this,
                                this->toolbar,
                                SLOT(closeSelectedTab()));
    
    this->closeWindowAction = 
    WuQtUtilities::createAction("Close Window",
                                "Close the window",
                                Qt::CTRL + Qt::SHIFT + Qt::Key_W,
                                this,
                                this,
                                SLOT(close()));
    
    this->captureImageAction =
    WuQtUtilities::createAction("Capture Image...",
                                "Capture an Image of the windows content",
                                this,
                                this,
                                SLOT(processCaptureImage()));
    
    this->preferencesAction = 
    WuQtUtilities::createAction("Preferences...",
                                "Edit the preferences",
                                this,
                                this,
                                SLOT(processEditPreferences()));
    
    this->exitProgramAction =
    WuQtUtilities::createAction("Exit", 
                                "Exit (quit) the program",
                                Qt::CTRL+Qt::Key_Q, 
                                this,
                                this,
                                SLOT(processExitProgram()));
    
    this->viewScreenNormalAction = WuQtUtilities::createAction("Normal", 
                                                               "Normal Viewing", 
                                                               Qt::Key_Escape, 
                                                               this);
    this->viewScreenNormalAction->setCheckable(true);
    
    this->viewScreenFullAction = WuQtUtilities::createAction("Full Screen", 
                                                             "View using all of screen", 
                                                             Qt::CTRL+Qt::Key_F, 
                                                             this);
    this->viewScreenFullAction->setCheckable(true);
    
    this->viewScreenMontageTabsAction = WuQtUtilities::createAction("Tab Montage", 
                                                                    "View all tabs in a grid layout", 
                                                                    Qt::CTRL+Qt::Key_M, 
                                                                    this);
    this->viewScreenMontageTabsAction->setCheckable(true);
    
    this->viewScreenFullMontageTabsAction = WuQtUtilities::createAction("Tab Montage (Full Screen)", 
                                                                        "View all tabs in a grid layout using all of screen", 
                                                                        Qt::CTRL+Qt::SHIFT+Qt::Key_M, 
                                                                        this);
    this->viewScreenFullMontageTabsAction->setCheckable(true);
    
    this->viewScreenActionGroup = new QActionGroup(this);
    QObject::connect(this->viewScreenActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(processViewScreenActionGroupSelection(QAction*)));
    this->viewScreenActionGroup->setExclusive(true);
    this->viewScreenActionGroup->addAction(this->viewScreenNormalAction);
    this->viewScreenActionGroup->addAction(this->viewScreenFullAction);
    this->viewScreenActionGroup->addAction(this->viewScreenMontageTabsAction);
    this->viewScreenActionGroup->addAction(this->viewScreenFullMontageTabsAction);
    
    this->viewScreenActionGroup->blockSignals(true);
    this->viewScreenNormalAction->setChecked(true);
    this->viewScreenActionGroup->blockSignals(false);
        
    this->nextTabAction =
    WuQtUtilities::createAction("Next Tab",
                                "Move to the next tab",
                                Qt::CTRL + Qt::Key_Right,
                                this,
                                this->toolbar,
                                SLOT(nextTab()));
    
    this->previousTabAction =
    WuQtUtilities::createAction("Previous Tab",
                                "Move to the previous tab",
                                Qt::CTRL + Qt::Key_Left,
                                this,
                                this->toolbar,
                                SLOT(previousTab()));
    
    this->renameSelectedTabAction =
    WuQtUtilities::createAction("Rename Selected Tab...",
                                "Change the name of the selected tab",
                                this,
                                this->toolbar,
                                SLOT(renameTab()));
    
    this->moveTabsInWindowToNewWindowsAction =
    WuQtUtilities::createAction("Move All Tabs in Current Window to New Windows",
                                "Move all but the left most tab to new windows",
                                this,
                                this->toolbar,
                                SLOT(moveTabsToNewWindows()));
    
    this->moveTabsFromAllWindowsToOneWindowAction =
    WuQtUtilities::createAction("Move All Tabs From All Windows Into Selected Window",
                                "Move all tabs from all windows into selected window",
                                this,
                                this,
                                SLOT(processMoveAllTabsToOneWindow()));
    
    this->bringAllToFrontAction =
    WuQtUtilities::createAction("Bring All To Front",
                                "Move all windows on top of other application windows",
                                this,
                                guiManager,
                                SLOT(processBringAllWindowsToFront()));
    
    this->informationDialogAction =
    WuQtUtilities::createAction("Information Window",
                                "Show the Informaiton Window",
                                this,
                                guiManager,
                                SLOT(processShowInformationWindow()));
    
    this->helpHcpWebsiteAction =
    WuQtUtilities::createAction("Go to HCP Website...",
                                "Load the HCP Website in your computer's web browser",
                                this,
                                this,
                                SLOT(processHcpWebsiteInBrowser()));
    
    this->helpOnlineAction =
    WuQtUtilities::createAction("Show Help (Online)...",
                                "Show the Help Window",
                                this,
                                guiManager,
                                SLOT(processShowHelpOnlineWindow()));
    this->helpOnlineAction->setEnabled(false);
    
    this->helpSearchOnlineAction =
    WuQtUtilities::createAction("Search Help (Online)...",
                                "Show the Search Helper Window",
                                this,
                                guiManager,
                                SLOT(processShowSearchHelpOnlineWindow()));
    this->helpSearchOnlineAction->setEnabled(false);

    this->connectToAllenDatabaseAction =
    WuQtUtilities::createAction("Allen Brain Institute Database..",
                                "Open a connection to the Allen Brain Institute Database",
                                this,
                                this,
                                SLOT(processConnectToAllenDataBase()));
    
    this->connectToConnectomeDatabaseAction =
    WuQtUtilities::createAction("Human Connectome Project Database...",
                                "Open a connection to the Human Connectome Project Database",
                                this,
                                this,
                                SLOT(processConnectToConnectomeDataBase()));
}

/**
 * Create menus for this window.
 */
void 
BrainBrowserWindow::createMenus()
{
    /*
     * Create the menu bar and add menus to it.
     */
    QMenuBar* menuBar = this->menuBar();
    menuBar->addMenu(this->createMenuFile());
    menuBar->addMenu(this->createMenuView());
    QMenu* dataMenu = this->createMenuData();
    if (dataMenu != NULL) {
        menuBar->addMenu(dataMenu);
    }
    menuBar->addMenu(this->createMenuSurface());
    menuBar->addMenu(this->createMenuVolume());
    menuBar->addMenu(this->createMenuConnect());
    menuBar->addMenu(this->createMenuWindow());
    menuBar->addMenu(this->createMenuHelp());    
}

/**
 * Create the file menu.
 * @return the file menu.
 */
QMenu* 
BrainBrowserWindow::createMenuFile()
{
    QMenu* menu = new QMenu("File", this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(processFileMenuAboutToShow()));

    menu->addAction(this->aboutWorkbenchAction);
    menu->addAction(this->preferencesAction);
#ifndef CARET_OS_MACOSX
    menu->addSeparator();
#endif // CARET_OS_MACOSX
    menu->addAction(this->newWindowAction);
    menu->addAction(this->newTabAction);
    menu->addSeparator();
    menu->addAction(this->openFileAction);
    menu->addAction(this->openLocationAction);
    this->recentSpecFileMenu = menu->addMenu("Open Recent Spec File");
    QObject::connect(this->recentSpecFileMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processRecentSpecFileMenuAboutToBeDisplayed()));
    QObject::connect(this->recentSpecFileMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processRecentSpecFileMenuSelection(QAction*)));
    menu->addAction(this->openFileViaSpecFileAction);
    menu->addAction(this->manageFilesAction);
    menu->addAction(this->closeSpecFileAction);
    menu->addSeparator();
    menu->addAction(this->captureImageAction);
    menu->addSeparator();
    menu->addAction(this->closeTabAction);
    menu->addAction(this->closeWindowAction);
    menu->addSeparator();
#ifndef CARET_OS_MACOSX
    menu->addSeparator();
#endif // CARET_OS_MACOSX
    menu->addAction(this->exitProgramAction);
    
    return menu;
}

/**
 * Called to display the overlay toolbox.
 */
void 
BrainBrowserWindow::processShowOverlayToolBox(bool status)
{
    this->overlayActiveToolBox->setVisible(status);
    this->overlayToolBoxAction->blockSignals(true);
    this->overlayToolBoxAction->setChecked(status);
    this->overlayToolBoxAction->blockSignals(false);
    if (status) {
        this->overlayToolBoxAction->setToolTip("Hide Overlay Toolbox");
    }
    else {
        this->overlayToolBoxAction->setToolTip("Show Overlay Toolbox");
    }
}

/**
 * Called when visibility of horizontal overlay toolbox is changed.
 * @param visible
 *    New visibility status.
 */
void 
BrainBrowserWindow::processOverlayHorizontalToolBoxVisibilityChanged(bool visible)
{
    if (visible == false) {
        if (this->overlayActiveToolBox == this->overlayHorizontalToolBox) {
            this->overlayToolBoxAction->blockSignals(true);
            this->overlayToolBoxAction->setChecked(false);
            this->overlayToolBoxAction->blockSignals(false);
        }
    }
}

/**
 * Called when visibility of vertical overlay toolbox is changed.
 * @param visible
 *    New visibility status.
 */
void 
BrainBrowserWindow::processOverlayVerticalToolBoxVisibilityChanged(bool visible)
{
    if (visible == false) {
        if (this->overlayActiveToolBox == this->overlayVerticalToolBox) {
            this->overlayToolBoxAction->blockSignals(true);
            this->overlayToolBoxAction->setChecked(false);
            this->overlayToolBoxAction->blockSignals(false);
        }
    }
}

/**
 * Called when File Menu is about to show.
 */
void 
BrainBrowserWindow::processFileMenuAboutToShow()
{
    const int32_t numFiles = GuiManager::get()->getBrain()->getSpecFile()->getNumberOfFiles();
    this->openFileViaSpecFileAction->setEnabled(numFiles > 0);
}

/**
 * Called when Open Recent Spec File Menu is about to be displayed
 * and creates the content of the menu.
 */
void 
BrainBrowserWindow::processRecentSpecFileMenuAboutToBeDisplayed()
{
    this->recentSpecFileMenu->clear();
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    std::vector<AString> recentSpecFiles;
    prefs->getPreviousSpecFiles(recentSpecFiles);
    
    const int32_t numRecentSpecFiles = static_cast<int>(recentSpecFiles.size());
    for (int32_t i = 0; i < numRecentSpecFiles; i++) {
        FileInformation fileInfo(recentSpecFiles[i]);
        QString path = fileInfo.getPathName();
        QString name = fileInfo.getFileName();
        if (path.isEmpty() == false) {
            name += (" (" + path + ")");
        }
        QString fullPath = fileInfo.getFilePath();
        
        QAction* action = new QAction(name,
                                      this->recentSpecFileMenu);
        action->setData(fullPath);
        this->recentSpecFileMenu->addAction(action);
    } 
    
    if (numRecentSpecFiles > 0) {
        this->recentSpecFileMenu->addSeparator();
        QAction* action = new QAction("Clear Menu",
                                      this->recentSpecFileMenu);
        action->setData("CLEAR_CLEAR");
        this->recentSpecFileMenu->addAction(action);
    }
}

/**
 * Called when an item is selected from the recent spec file
 * menu.
 * @param itemAction
 *    Action of the menu item that was selected.
 */
void 
BrainBrowserWindow::processRecentSpecFileMenuSelection(QAction* itemAction)
{
    AString errorMessages;
    
    const AString specFileName = itemAction->data().toString();
    if (specFileName == "CLEAR_CLEAR") {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->clearPreviousSpecFiles();
        return;
    }
    
    if (specFileName.isEmpty() == false) {
        SpecFile specFile;
        try {
            specFile.readFile(specFileName);
        }
        catch (const DataFileException& e) {
            errorMessages += e.whatString();
        }
        
        SpecFileDialog* sfd = SpecFileDialog::createForLoadingSpecFile(&specFile,
                                                                       this);
        if (sfd->exec() == QDialog::Accepted) {
            CursorDisplayScoped cursor;
            cursor.showWaitCursor();
            
            EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
                                                         &specFile);
            
            EventManager::get()->sendEvent(readSpecFileEvent.getPointer());
            
            if (readSpecFileEvent.isError()) {
                if (errorMessages.isEmpty() == false) {
                    errorMessages += "\n";
                }
                errorMessages += readSpecFileEvent.getErrorMessage();
            }
        }
        
        delete sfd;
        sfd = NULL;
        
        this->toolbar->addDefaultTabsAfterLoadingSpecFile();
        
        if (errorMessages.isEmpty() == false) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  errorMessages);
        }
        
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when view menu is about to show.
 */
void 
BrainBrowserWindow::processViewMenuAboutToShow()
{
}

/**
 * Create the view menu.
 * @return the view menu.
 */
QMenu* 
BrainBrowserWindow::createMenuView()
{
    QMenu* menu = new QMenu("View", this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(processViewMenuAboutToShow()));
    
    menu->addAction(this->showToolBarAction);
    menu->addMenu(this->createMenuViewMoveFeaturesToolBox());
    menu->addMenu(this->createMenuViewMoveOverlayToolBox());
    menu->addSeparator();
    
    QMenu* screenMenu = new QMenu("Screen");
    screenMenu->addAction(this->viewScreenNormalAction);
    screenMenu->addAction(this->viewScreenFullAction);
    screenMenu->addAction(this->viewScreenMontageTabsAction);
    screenMenu->addAction(this->viewScreenFullMontageTabsAction);

    menu->addMenu(screenMenu);
    
    return menu;
}

/**
 * @return Create and return the overlay toolbox menu.
 */
QMenu* 
BrainBrowserWindow::createMenuViewMoveFeaturesToolBox()
{
    QMenu* menu = new QMenu("Features Toolbox", this);
    
    menu->addAction("Attach to Right", this, SLOT(processMoveFeaturesToolBoxToRight()));
    menu->addAction("Detach", this, SLOT(processMoveFeaturesToolBoxToFloat()));
    menu->addAction("Hide", this, SLOT(processHideFeaturesToolBox()));
    
    return menu;
}

/**
 * @return Create and return the overlay toolbox menu.
 */
QMenu* 
BrainBrowserWindow::createMenuViewMoveOverlayToolBox()
{
    QMenu* menu = new QMenu("Overlay Toolbox", this);
    
    menu->addAction("Attach to Bottom", this, SLOT(processMoveOverlayToolBoxToBottom()));
    menu->addAction("Attach to Left", this, SLOT(processMoveOverlayToolBoxToLeft()));
    menu->addAction("Detach", this, SLOT(processMoveOverlayToolBoxToFloat()));
    menu->addAction("Hide", this, SLOT(processHideOverlayToolBox()));
    
    return menu;
}

/**
 * Create the connect menu.
 * @return the connect menu.
 */
QMenu* 
BrainBrowserWindow::createMenuConnect()
{
    QMenu* menu = new QMenu("Connect", this);
    
    menu->addAction(this->connectToAllenDatabaseAction);
    menu->addAction(this->connectToConnectomeDatabaseAction);
    
    return menu;
}

/**
 * Create the data menu.
 * @return the data menu.
 */
QMenu* 
BrainBrowserWindow::createMenuData()
{
/*
    QMenu* menu = new QMenu("Data", this);
    return menu;
*/
    return NULL;
}

/**
 * Create the surface menu.
 * @return the surface menu.
 */
QMenu* 
BrainBrowserWindow::createMenuSurface()
{
    QMenu* menu = new QMenu("Surface", this);
    
    menu->addAction("Information...", 
                    this, 
                    SLOT(processSurfaceMenuInformation()));
    
    menu->addAction("Volume Interaction...", 
                    this, 
                    SLOT(processSurfaceMenuVolumeInteraction()));
    
    return menu;
}

/**
 * Called when Volume Interaction is selected from the surface menu.
 */
void 
BrainBrowserWindow::processSurfaceMenuVolumeInteraction()
{
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numBrainStructures = brain->getNumberOfBrainStructures();
    if (numBrainStructures <= 0) {
        return;
    }
    
    WuQDataEntryDialog ded("Volume Interaction Surfaces",
                           this);
    std::vector<SurfaceSelectionViewController*> surfaceSelectionControls;
    for (int32_t i = 0; i < numBrainStructures; i++) {
        BrainStructure* bs = brain->getBrainStructure(i);
        SurfaceSelectionViewController* ssc = ded.addSurfaceSelectionViewController(StructureEnum::toGuiName(bs->getStructure()), 
                                                                                    bs);
        ssc->setSurface(bs->getVolumeInteractionSurface());
        surfaceSelectionControls.push_back(ssc);
    }
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        for (int32_t i = 0; i < numBrainStructures; i++) {
            BrainStructure* bs = brain->getBrainStructure(i);
            bs->setVolumeInteractionSurface(surfaceSelectionControls[i]->getSurface());
        }
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when Information is selected from the surface menu.
 */
void 
BrainBrowserWindow::processSurfaceMenuInformation()
{
    BrowserTabContent* btc = this->getBrowserTabContent();  
    if (btc != NULL) {
        AString txt = "";
        
        Model* mdc = btc->getModelControllerForDisplay();
        ModelSurface* mdcs = dynamic_cast<ModelSurface*>(mdc);
        if (mdcs != NULL) {
            txt += mdcs->getSurface()->getInformation();
        }
        
        ModelWholeBrain* mdcwb = dynamic_cast<ModelWholeBrain*>(mdc);
        if (mdcwb != NULL) {
            std::vector<StructureEnum::Enum> allStructures;
            StructureEnum::getAllEnums(allStructures);
            
            for (std::vector<StructureEnum::Enum>::iterator iter = allStructures.begin();
                 iter != allStructures.end();
                 iter++) {
                const Surface* surface = mdcwb->getSelectedSurface(*iter, btc->getTabNumber());
                if (surface != NULL) {
                    txt += surface->getInformation();
                }
            }
        }
        
        if (txt.isEmpty() == false) {
            WuQMessageBox::informationOk(this,
                                         txt);
        }
    }
}

/**
 * Create the volume menu.
 * @return the volume menu.
 */
QMenu* 
BrainBrowserWindow::createMenuVolume()
{
    QMenu* menu = new QMenu("Volume", this);
    
    return menu;
}

/**
 * Create the window menu.
 * @return the window menu.
 */
QMenu* 
BrainBrowserWindow::createMenuWindow()
{
    this->moveSelectedTabToWindowMenu = new QMenu("Move Selected Tab to Window");
    QObject::connect(this->moveSelectedTabToWindowMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processMoveSelectedTabToWindowMenuAboutToBeDisplayed()));
    QObject::connect(this->moveSelectedTabToWindowMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processMoveSelectedTabToWindowMenuSelection(QAction*)));
    
    QMenu* menu = new QMenu("Window", this);
    
    menu->addAction(this->nextTabAction);
    menu->addAction(this->previousTabAction);
    menu->addAction(this->renameSelectedTabAction);
    menu->addSeparator();
    menu->addAction(this->moveTabsInWindowToNewWindowsAction);
    menu->addAction(this->moveTabsFromAllWindowsToOneWindowAction);
    menu->addMenu(this->moveSelectedTabToWindowMenu);
    menu->addSeparator();
    menu->addAction(this->informationDialogAction);
    menu->addAction(this->showSceneDialogAction);
    menu->addSeparator();
    menu->addAction(this->bringAllToFrontAction);
    
    return menu;
}

/**
 * Create the help menu.
 * @return the help menu.
 */
QMenu* 
BrainBrowserWindow::createMenuHelp()
{
    QMenu* menu = new QMenu("Help", this);
    
    menu->addAction(this->helpHcpWebsiteAction);
    menu->addSeparator();
    menu->addAction(this->helpOnlineAction);
    menu->addAction(this->helpSearchOnlineAction);
    
    return menu;
}

/**
 * Called when capture image is selected.
 */
void 
BrainBrowserWindow::processCaptureImage()
{
    GuiManager::get()->processShowImageCaptureDialog(this);
}

/**
 * Called when capture image is selected.
 */
void 
BrainBrowserWindow::processEditPreferences()
{
    GuiManager::get()->processShowPreferencesDialog(this);
}

/**
 * Called when information dialog is selected.
 */
void 
BrainBrowserWindow::processInformationDialog()
{
    GuiManager::get()->processShowInformationDisplayDialog(this);
}

/**
 * Called when close spec file is selected.
 */
void 
BrainBrowserWindow::processCloseSpecFile()
{
    Brain* brain = GuiManager::get()->getBrain();
    brain->resetBrain();
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    GuiManager::get()->closeAllOtherWindows(this);
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());    
}

/**
 * Create a new window.
 */
void 
BrainBrowserWindow::processNewWindow()
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();

    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW, 
                                    true);
    EventBrowserWindowNew eventNewBrowser(this, NULL);
    EventManager::get()->sendEvent(eventNewBrowser.getPointer());
    if (eventNewBrowser.isError()) {
        cursor.restoreCursor();
        QMessageBox::critical(this,
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

/**
 * Display about workbench dialog.
 */
void 
BrainBrowserWindow::processAboutWorkbench()
{
    AboutWorkbenchDialog awd(this);
    awd.exec();
}

/**
 * Called when open location is selected.
 */
void 
BrainBrowserWindow::processDataFileLocationOpen()
{
    CaretFileRemoteDialog fileRemoteDialog(this);
    fileRemoteDialog.exec();
}

/**
 * Called when open data file is selected.
 */
void 
BrainBrowserWindow::processDataFileOpen()
{
    if (this->previousOpenFileNameFilter.isEmpty()) {
        this->previousOpenFileNameFilter = 
            DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION);
    }
    
    /*
     * Get all file filters.
     */
    std::vector<DataFileTypeEnum::Enum> dataFileTypes;
    DataFileTypeEnum::getAllEnums(dataFileTypes, false, false);
    QStringList filenameFilterList;
    filenameFilterList.append("Any File (*)");
    for (std::vector<DataFileTypeEnum::Enum>::const_iterator iter = dataFileTypes.begin();
         iter != dataFileTypes.end();
         iter++) {
        AString filterName = DataFileTypeEnum::toQFileDialogFilter(*iter);
        filenameFilterList.append(filterName);
    }
    
    /*
     * Widget for adding to file dialog
     */
    QCheckBox* addFileToSpecFileCheckBox = new QCheckBox("Add Opened Data File to Spec File");
    addFileToSpecFileCheckBox->setToolTip("If this box is checked, the data file(s) opened\n"
                                          "will be added to the currently loaded Spec File.\n"
                                          "If there is not a valid Spec File loaded, you\n"
                                          "will be prompted to create or select a Spec File.");
    addFileToSpecFileCheckBox->setChecked(previousOpenFileAddToSpecFileSelection);
    QWidget* extraWidget = new QWidget();
    QVBoxLayout* extraLayout = new QVBoxLayout(extraWidget);
    extraLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    extraLayout->addWidget(addFileToSpecFileCheckBox, 0, Qt::AlignLeft);
    
    /*
     * Setup file selection dialog.
     */
    CaretFileDialogExtendable fd(this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::ExistingFiles);
    fd.setViewMode(CaretFileDialog::List);
    fd.selectNameFilter(this->previousOpenFileNameFilter);
    fd.addWidget(extraWidget);
    
    AString errorMessages;
    
    if (fd.exec()) {
        QStringList selectedFiles = fd.selectedFiles();
        if (selectedFiles.empty() == false) {            
            /*
             * Load the files.
             */
            bool isLoadingSpecFile = false;
            std::vector<AString> filenamesVector;
            QStringListIterator nameIter(selectedFiles);
            while (nameIter.hasNext()) {
                const QString name = nameIter.next();
                filenamesVector.push_back(name);
                if (name.endsWith(DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION))) {
                    isLoadingSpecFile = true;
                }
            }
            
            bool isLoadDataFiles = true;
                        
            AddDataFileToSpecFileMode addDataFileToSpecFileMode = ADD_DATA_FILE_TO_SPEC_FILE_NO;
            if (isLoadingSpecFile) {
                addDataFileToSpecFileMode = ADD_DATA_FILE_TO_SPEC_FILE_YES;
            }
            else {
                if (addFileToSpecFileCheckBox->isChecked()) {
                    Brain* brain = GuiManager::get()->getBrain();
                    FileInformation fileInfo(brain->getSpecFile()->getFileName());
                    if (fileInfo.exists()) {
                        addDataFileToSpecFileMode = ADD_DATA_FILE_TO_SPEC_FILE_YES;
                    }
                    else {
                        SpecFileCreateAddToDialog createAddToSpecFileDialog(GuiManager::get()->getBrain(),
                                                                            SpecFileCreateAddToDialog::MODE_OPEN,
                                                                            this);
                        
                        if (createAddToSpecFileDialog.exec() == SpecFileCreateAddToDialog::Accepted) {
                            if (createAddToSpecFileDialog.isAddToSpecFileSelected()) {
                                addDataFileToSpecFileMode = ADD_DATA_FILE_TO_SPEC_FILE_YES;
                            }
                        }
                        else {
                            isLoadDataFiles = false;
                        }
                    }
                }                
            }
            
            if (isLoadDataFiles) {
                this->loadFiles(filenamesVector,
                                LOAD_SPEC_FILE_WITH_DIALOG,
                                addDataFileToSpecFileMode);
            }
        }
        previousOpenFileNameFilter = fd.selectedNameFilter();
        previousOpenFileAddToSpecFileSelection = addFileToSpecFileCheckBox->isChecked();
    }
}

/**
 * Load the files that were specified on the command line.
 * @param filenames
 *    Names of files on the command line.
 * @param loadSpecFileMode
 *    Specifies handling of SpecFiles
 */
void 
BrainBrowserWindow::loadFilesFromCommandLine(const std::vector<AString>& filenames,
                                             const LoadSpecFileMode loadSpecFileMode)
{
    this->loadFiles(filenames,
                    loadSpecFileMode,
                    ADD_DATA_FILE_TO_SPEC_FILE_NO);
}

/**
 * Load data files.  If there are errors, an error message dialog
 * will be displayed.
 *
 * @param filenames
 *    Names of files.
 * @param loadSpecFileMode
 *    Specifies handling of SpecFiles
 * @param commandLineFlag
 *    True if files are being loaded from the command line.
 */
void 
BrainBrowserWindow::loadFiles(const std::vector<AString>& filenames,
                              const LoadSpecFileMode loadSpecFileMode,
                              const AddDataFileToSpecFileMode addDataFileToSpecFileMode)
{
    /*
     * Pick out specific file types.
     */
    AString specFileName;
    std::vector<AString> volumeFileNames;
    std::vector<AString> surfaceFileNames;
    std::vector<AString> otherFileNames;
    const int32_t numFiles = static_cast<int32_t>(filenames.size());
    for (int32_t i = 0; i < numFiles; i++) {
        const AString name = filenames[i];
        bool isValidType = false;
        DataFileTypeEnum::Enum fileType = DataFileTypeEnum::fromFileExtension(name, &isValidType);
        if (isValidType) {
            switch (fileType) {
                case DataFileTypeEnum::SPECIFICATION:
                    if (specFileName.isEmpty() == false) {
                        QMessageBox::critical(this, 
                                              "ERROR", 
                                              "More than one spec file cannot be loaded");
                        return;

                    }
                    specFileName = name;
                    break;
                case DataFileTypeEnum::SURFACE:
                    surfaceFileNames.push_back(name);
                    break;
                case DataFileTypeEnum::VOLUME:
                    volumeFileNames.push_back(name);
                    break;
                default:
                    otherFileNames.push_back(name);
                    break;
            }
        }
    }
    
    /*
     * Display the wait cursor
     */
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();

    /*
     * Load files in this order:
     * (1) Spec File - Limit to one.
     * (2) Volume File
     * (3) Surface File
     * (4) All other files.
     */
    std::vector<AString> filenamesToLoad;
    if (specFileName.isEmpty() == false) {
        filenamesToLoad.push_back(specFileName);
    }
    filenamesToLoad.insert(filenamesToLoad.end(),
                           volumeFileNames.begin(),
                           volumeFileNames.end());
    filenamesToLoad.insert(filenamesToLoad.end(),
                           surfaceFileNames.begin(),
                           surfaceFileNames.end());
    filenamesToLoad.insert(filenamesToLoad.end(),
                           otherFileNames.begin(),
                           otherFileNames.end());
                           

    bool createDefaultTabsFlag = false;
    switch (loadSpecFileMode) {
        case LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE:
            createDefaultTabsFlag = true;
            break;
        case LOAD_SPEC_FILE_WITH_DIALOG:
            break;
        case LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE:
            createDefaultTabsFlag = true;
            break;
    }
    
    AString errorMessages;
    
    ElapsedTimer timer;
    timer.start();
    float specFileTimeStart = 0.0;
    float specFileTimeEnd   = 0.0;
    
    /*
     * Load each file.
     */
    const int32_t numFilesToLoad = static_cast<int32_t>(filenamesToLoad.size());
    for (int32_t i = 0; i < numFilesToLoad; i++) {
        AString name = filenamesToLoad[i];
        
        //FileInformation fileInfo(name);
        //if (fileInfo.isAbsolute()) {
        //    prefs->addToPreviousOpenFileDirectories(fileInfo.getPathName());
        //}
        
        bool isValidType = false;
        DataFileTypeEnum::Enum fileType = DataFileTypeEnum::fromFileExtension(name, &isValidType);
        if (isValidType) {
            if (fileType == DataFileTypeEnum::SPECIFICATION) {
                SpecFile specFile;
                try {
                    specFile.readFile(name);
                }
                catch (const DataFileException& e) {
                    errorMessages += e.whatString();
                }
                
                switch (loadSpecFileMode) {
                    case LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE:
                    {
                        timer.reset(); // resets timer
                        specFileTimeStart = timer.getElapsedTimeSeconds();
                        
                        /*
                         * Load all files listed in spec file
                         */
                        specFile.setAllFilesSelected(true);
                        
                        EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
                                                                     &specFile);
                        
                        EventManager::get()->sendEvent(readSpecFileEvent.getPointer());
                        
                        if (readSpecFileEvent.isError()) {
                            if (errorMessages.isEmpty() == false) {
                                errorMessages += "\n";
                            }
                            errorMessages += readSpecFileEvent.getErrorMessage();
                        }
                        specFileTimeEnd = timer.getElapsedTimeSeconds();
                    }
                        break;
                    case LOAD_SPEC_FILE_WITH_DIALOG:
                    case LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE:
                    {
                        /*
                         * Remove wait cursor
                         */
                        cursor.restoreCursor();
                        
                        /*
                         * Allow user to choose files listed in the spec file
                         */
                        SpecFileDialog* sfd = SpecFileDialog::createForLoadingSpecFile(&specFile,
                                                                                       this);
                        if (sfd->exec() == QDialog::Accepted) {
                            /*
                             * Redisplay wait cursor
                             */
                            cursor.showWaitCursor();
                            
                            timer.reset();
                            specFileTimeStart = timer.getElapsedTimeSeconds();
                            
                            EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
                                                                         &specFile);
                            
                            EventManager::get()->sendEvent(readSpecFileEvent.getPointer());
                            
                            if (readSpecFileEvent.isError()) {
                                if (errorMessages.isEmpty() == false) {
                                    errorMessages += "\n";
                                }
                                errorMessages += readSpecFileEvent.getErrorMessage();
                            }
                            specFileTimeEnd = timer.getElapsedTimeSeconds();
                            
                            createDefaultTabsFlag = true;
                        }
                        else {
                            /*
                             * Redisplay wait cursor
                             */
                            cursor.showWaitCursor();
                        }
                        
                        delete sfd;
                    }
                        break;
                }
            }
            else {
                bool addDataFileToSpecFile = false;
                switch (addDataFileToSpecFileMode) {
                    case ADD_DATA_FILE_TO_SPEC_FILE_NO:
                        addDataFileToSpecFile = false;
                        break;
                    case ADD_DATA_FILE_TO_SPEC_FILE_YES:
                        addDataFileToSpecFile = true;
                        break;
                }
                
                EventDataFileRead loadFileEvent(GuiManager::get()->getBrain(),
                                                fileType,
                                                name,
                                                addDataFileToSpecFile);
                
                EventManager::get()->sendEvent(loadFileEvent.getPointer());
                
                if (loadFileEvent.isError()) {
                    AString loadErrorMessage = "";
                    
                    if (loadFileEvent.isErrorInvalidStructure()) {
                        /*
                         * Remove wait cursor
                         */
                        cursor.restoreCursor();
                        
                        WuQDataEntryDialog ded("Structure",
                                               this);
                        StructureEnumComboBox* ssc = ded.addStructureEnumComboBox("");
                        ded.setTextAtTop(("File \""
                                          + FileInformation(name).getFileName()
                                          + "\"\nhas missing or invalid structure, select it's structure."
                                          "\nAfter loading, save file with File Menu->Save Manage Files"
                                          "\nto prevent this error."),
                                         false);
                        if (ded.exec() == WuQDataEntryDialog::Accepted) {
                            /*
                             * Redisplay wait cursor
                             */
                            cursor.showWaitCursor();

                            EventDataFileRead loadFileEventStructure(GuiManager::get()->getBrain(),
                                                                     ssc->getSelectedStructure(),
                                                                     fileType,
                                                                     name,
                                                                     addDataFileToSpecFile);
                            
                            EventManager::get()->sendEvent(loadFileEventStructure.getPointer());
                            if (loadFileEventStructure.isError()) {
                                loadErrorMessage = loadFileEventStructure.getErrorMessage();
                            }
                        }
                        else {
                            /*
                             * Redisplay wait cursor
                             */
                            cursor.showWaitCursor();
                        }
                    }
                    else {
                        loadErrorMessage = loadFileEvent.getErrorMessage();
                    }
                    if (loadErrorMessage.isEmpty() == false) {
                        if (errorMessages.isEmpty() == false) {
                            errorMessages += "\n";
                        }
                        errorMessages += loadErrorMessage;
                    }
                }                    
            }
        }
        else {
            if (errorMessages.isEmpty() == false) {
                errorMessages += "\n";
            }
            errorMessages += ("Extension for " + name + " does not match a Caret file type");
        }
    }
    
    const float specFileTime = specFileTimeEnd - specFileTimeStart;
    
    const float createTabsStartTime = timer.getElapsedTimeSeconds();
    if (createDefaultTabsFlag) {
        this->toolbar->addDefaultTabsAfterLoadingSpecFile();
    }
    const float createTabsTime = timer.getElapsedTimeSeconds() - createTabsStartTime;
    
    const float guiStartTime = timer.getElapsedTimeSeconds();
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    const float guiTime = timer.getElapsedTimeSeconds() - guiStartTime;
    
    if (specFileName.isEmpty() == false) {
        CaretLogInfo("Time to read files from spec file (in GUI) \""
                     + FileInformation(specFileName).getFileName()
                     + "\" was "
                     + AString::number(timer.getElapsedTimeSeconds())
                     + " seconds.\n  Time to read spec data files was "
                     + AString::number(specFileTime)
                     + " seconds.\n  Time to update GUI was "
                     + AString::number(guiTime)
                     + " seconds.\n  Time to create tabs was "
                     + AString::number(createTabsTime));
    }
    
    /*
     * Remove wait cursor
     */
    cursor.restoreCursor();
    
    if (errorMessages.isEmpty() == false) {
        QMessageBox::critical(this, 
                              "ERROR", 
                              errorMessages);
    }
    
}


/**
 * Called when open data file from spec file is selected.
 */
void 
BrainBrowserWindow::processDataFileOpenFromSpecFile()
{
    SpecFile* sf = GuiManager::get()->getBrain()->getSpecFile();
    SpecFileDialog::displayFastOpenDataFile(sf,
                                            this);
}

/**
 * Called when manage/save loaded files is selected.
 */
void 
BrainBrowserWindow::processManageSaveLoadedFiles()
{
    ManageLoadedFilesDialog manageLoadedFile(this,
                                             GuiManager::get()->getBrain(),
                                             false);
    manageLoadedFile.exec();
}

/**
 * Exit the program.
 */
void 
BrainBrowserWindow::processExitProgram()
{
    GuiManager::get()->exitProgram(this);
}

/**
 * Called when a view screen menu item is selected.
 */
void
BrainBrowserWindow::processViewScreenActionGroupSelection(QAction* action)
{
    bool showInFullScreen = false;
    
    BrainBrowserWindowScreenModeEnum::Enum previousScreenMode = this->screenMode;
    
    this->screenMode = BrainBrowserWindowScreenModeEnum::NORMAL;
    if (action == this->viewScreenNormalAction) {
        this->screenMode = BrainBrowserWindowScreenModeEnum::NORMAL;
    }
    else if (action == this->viewScreenFullAction) {
        this->screenMode = BrainBrowserWindowScreenModeEnum::FULL_SCREEN;
        showInFullScreen = true;
    }
    else if (action == this->viewScreenMontageTabsAction) {
        this->screenMode = BrainBrowserWindowScreenModeEnum::TAB_MONTAGE;
    }
    else if (action == this->viewScreenFullMontageTabsAction) {
        this->screenMode = BrainBrowserWindowScreenModeEnum::TAB_MONTAGE_FULL_SCREEN;
        showInFullScreen = true;
    }
    else {
        CaretAssert(0);
    }
    
    if (this->screenMode == previousScreenMode) {
        return;
    }
    
    if (previousScreenMode == BrainBrowserWindowScreenModeEnum::NORMAL) {
        if (this->screenMode != BrainBrowserWindowScreenModeEnum::NORMAL) {
            this->saveWindowComponentStatus(this->normalWindowComponentStatus);
        }
    }
    
    if (showInFullScreen) {
        if (this->isFullScreen() == false) {
            this->showFullScreen();
        }
    }
    else {
        if (this->isFullScreen()) {
            this->showNormal();
        }
    }    
    
    if (this->screenMode == BrainBrowserWindowScreenModeEnum::NORMAL) {
        this->restoreWindowComponentStatus(this->normalWindowComponentStatus);
    }
    
    this->toolbar->updateToolBar();
    if (this->screenMode != BrainBrowserWindowScreenModeEnum::NORMAL) {
        this->overlayToolBoxAction->blockSignals(true);
        this->overlayToolBoxAction->setChecked(true);
        this->overlayToolBoxAction->blockSignals(false);
        this->overlayToolBoxAction->trigger();

        this->featuresToolBoxAction->blockSignals(true);
        this->featuresToolBoxAction->setChecked(true);
        this->featuresToolBoxAction->blockSignals(false);
        this->featuresToolBoxAction->trigger();
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Restore the status of window components.
 * @param wcs
 *    Window component status that is restored.
 */
void 
BrainBrowserWindow::restoreWindowComponentStatus(const WindowComponentStatus& wcs)
{
    this->showToolBarAction->setEnabled(true);
    if (wcs.isToolBarDisplayed) {
        this->showToolBarAction->setChecked(false);
        this->showToolBarAction->trigger();
    }
    else {
        this->showToolBarAction->setChecked(true);
        this->showToolBarAction->trigger();
    }
    
    this->overlayToolBoxAction->setEnabled(true);
    if (wcs.isOverlayToolBoxDisplayed) {
        this->overlayToolBoxAction->blockSignals(true);
        this->overlayToolBoxAction->setChecked(false);
        this->overlayToolBoxAction->blockSignals(false);
        this->overlayToolBoxAction->trigger();
    }
    else {
        this->overlayToolBoxAction->blockSignals(true);
        this->overlayToolBoxAction->setChecked(true);
        this->overlayToolBoxAction->blockSignals(false);
        this->overlayToolBoxAction->trigger();
    }
    
    this->featuresToolBoxAction->setEnabled(true);
    if (wcs.isFeaturesToolBoxDisplayed) {
        this->featuresToolBoxAction->blockSignals(true);
        this->featuresToolBoxAction->setChecked(false);
        this->featuresToolBoxAction->blockSignals(false);
        this->featuresToolBoxAction->trigger();
    }
    else {
        this->featuresToolBoxAction->blockSignals(true);
        this->featuresToolBoxAction->setChecked(true);
        this->featuresToolBoxAction->blockSignals(false);
        this->featuresToolBoxAction->trigger();
    }
}

/**
 * Save the status of window components.
 * @param wcs
 *    Will contains status after exit.
 * @param hideComponents
 *    If true, any components (toolbar/toolbox) will be hidden.
 */
void 
BrainBrowserWindow::saveWindowComponentStatus(WindowComponentStatus& wcs)
{
    wcs.isToolBarDisplayed = this->showToolBarAction->isChecked();
    wcs.isOverlayToolBoxDisplayed = this->overlayToolBoxAction->isChecked();
    wcs.isFeaturesToolBoxDisplayed  = this->featuresToolBoxAction->isChecked();
    this->showToolBarAction->setEnabled(false);
    this->overlayToolBoxAction->setEnabled(false);
    this->featuresToolBoxAction->setEnabled(false);
}

/**
 * Adds a new tab to the window.
 */
void 
BrainBrowserWindow::processNewTab()
{
    BrowserTabContent* previousTabContent = this->getBrowserTabContent();
    this->toolbar->addNewTabCloneContent(previousTabContent);
    
//    /*
//     * Wait cursor
//     */
//    CursorDisplayScoped cursor;
//    cursor.showWaitCursor();
//    
//    BrowserTabContent* previousTabContent = this->getBrowserTabContent();
//    
//    this->toolbar->addNewTab();
//    this->toolbar->updateToolBar();
//
//    if (previousTabContent != NULL) {
//        /*
//         * New tab is clone of tab that was displayed when the new tab was created.
//         */
//        BrowserTabContent* newTabContent = this->getBrowserTabContent();
//        newTabContent->cloneBrowserTabContent(previousTabContent);
//    }
//    
//    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
//    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
//    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Called when move all tabs to one window is selected.
 */
void 
BrainBrowserWindow::processMoveAllTabsToOneWindow()
{
    /*
     * Wait cursor
     */
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    std::vector<BrowserTabContent*> otherTabContent;
    GuiManager::get()->closeOtherWindowsAndReturnTheirTabContent(this,
                                                                 otherTabContent);
    
    const int32_t numOtherTabs = static_cast<int32_t>(otherTabContent.size());
    for (int32_t i = 0; i < numOtherTabs; i++) {
        this->toolbar->addNewTab(otherTabContent[i]);
        this->toolbar->updateToolBar();
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Called when the move tab to window is about to be displayed.
 */
void 
BrainBrowserWindow::processMoveSelectedTabToWindowMenuAboutToBeDisplayed()
{
    this->moveSelectedTabToWindowMenu->clear();

    if (this->getBrowserTabContent() == NULL) {
        return;
    }
    
    /*
     * Allow movement of tab to new window ONLY if this window
     * contains more than one tab.
     */
    if (this->toolbar->tabBar->count() > 1) {
        QAction* toNewWindowAction = new QAction("New Window",
                                                 this->moveSelectedTabToWindowMenu);
        toNewWindowAction->setData(qVariantFromValue((void*)NULL));
        this->moveSelectedTabToWindowMenu->addAction(toNewWindowAction);
    }
    
    std::vector<BrainBrowserWindow*> browserWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    for (int32_t i = 0; i < static_cast<int32_t>(browserWindows.size()); i++) {
        if (browserWindows[i] != this) {
            QAction* action = new QAction(browserWindows[i]->windowTitle(),
                                          this->moveSelectedTabToWindowMenu);
            action->setData(qVariantFromValue((void*)browserWindows[i]));
            this->moveSelectedTabToWindowMenu->addAction(action);
        }
    }    
}

/**
 * Called when move tab to window menu item is selected.
 * This window may close if there are no more tabs after
 * the tab is removed.
 * @param action
 *    Action from menu item that was selected.
 */
void 
BrainBrowserWindow::processMoveSelectedTabToWindowMenuSelection(QAction* action)
{
    if (action != NULL) {
        /*
         * Wait cursor
         */
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        void* p = action->data().value<void*>();
        BrainBrowserWindow* moveToBrowserWindow = (BrainBrowserWindow*)p;
        
        BrowserTabContent* btc = this->getBrowserTabContent();
        
            
        if (moveToBrowserWindow != NULL) {
            this->toolbar->removeTabWithContent(btc);
            moveToBrowserWindow->toolbar->addNewTab(btc);
        }
        else {
            EventBrowserWindowNew newWindow(this,
                                            btc);
            EventManager::get()->sendEvent(newWindow.getPointer());
            if (newWindow.isError()) {
                cursor.restoreCursor();
                QMessageBox::critical(this,
                                      "",
                                      newWindow.getErrorMessage());
                return;
            }
            this->toolbar->removeTabWithContent(btc);
        }
        
        if (this->toolbar->tabBar->count() <= 0) {
            this->close();
        }
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

/**
 * Called to move the overlay toolbox to the left side of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToLeft()
{
    this->moveOverlayToolBox(Qt::LeftDockWidgetArea);
}

/**
 * Called to move the overlay toolbox to the bottom side of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToBottom()
{
    this->moveOverlayToolBox(Qt::BottomDockWidgetArea);
}

/**
 * Called to move the overlay toolbox to float outside of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToFloat()
{
    this->moveOverlayToolBox(Qt::NoDockWidgetArea);
}

/**
 * Called to hide the overlay toolbox.
 */
void 
BrainBrowserWindow::processHideOverlayToolBox()
{
    this->processShowOverlayToolBox(false);
}

/**
 * Called to move the layers toolbox to the right side of the window.
 */
void 
BrainBrowserWindow::processMoveFeaturesToolBoxToRight()
{
    this->moveFeaturesToolBox(Qt::RightDockWidgetArea);
}

/**
 * Called to move the layers toolbox to float outside of the window.
 */
void 
BrainBrowserWindow::processMoveFeaturesToolBoxToFloat()
{
    this->moveFeaturesToolBox(Qt::NoDockWidgetArea);
}

/**
 * Called to hide the layers tool box.
 */
void 
BrainBrowserWindow::processHideFeaturesToolBox()
{
    if (this->featuresToolBoxAction->isChecked()) {
        this->featuresToolBoxAction->trigger();
    }
}

/**
 * Called to display the layers toolbox.
 */
void 
BrainBrowserWindow::processShowFeaturesToolBox(bool status)
{
    if (status) {
        this->featuresToolBoxAction->setToolTip("Hide Features Toolbox");
    }
    else {
        this->featuresToolBoxAction->setToolTip("Show Features Toolbox");
    }
//    if (status) {
//        AString title = this->featuresToolBoxTitle;
//        if (this->featuresToolBox->isFloating()) {
//            title += (" "
//                      + AString::number(this->browserWindowIndex + 1));
//        }
//        this->featuresToolBox->setWindowTitle(title);
//    }
}

/**
 * Move the overlay toolbox to the desired location.
 * @param area
 *    DockWidget location.
 */
void 
BrainBrowserWindow::moveFeaturesToolBox(Qt::DockWidgetArea area)
{
    switch (area) {
        case Qt::LeftDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on left");
            break;
        case Qt::RightDockWidgetArea:
            this->featuresToolBox->setFloating(false);
            this->addDockWidget(Qt::RightDockWidgetArea, 
                                this->featuresToolBox);
            if (this->featuresToolBoxAction->isChecked() == false) {
                this->featuresToolBoxAction->trigger();
            }
            break;
        case Qt::TopDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on top");
            break;
        case Qt::BottomDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on bottom");
            break;
        default:
            this->featuresToolBox->setFloating(true);
            if (this->featuresToolBoxAction->isChecked() == false) {
                this->featuresToolBoxAction->trigger();
            }
            break;
    }
    
//    this->processShowFeaturesToolBox(true);
}

/**
 * Move the overlay toolbox to the desired location.
 * @param area
 *    DockWidget location.
 */
void 
BrainBrowserWindow::moveOverlayToolBox(Qt::DockWidgetArea area)
{
    bool isVisible = false;
    switch (area) {
        case Qt::LeftDockWidgetArea:
            this->overlayHorizontalToolBox->setVisible(false);
            this->overlayVerticalToolBox->setFloating(false);
            this->addDockWidget(Qt::LeftDockWidgetArea, 
                                this->overlayVerticalToolBox);
            this->overlayVerticalToolBox->setVisible(true);
            this->overlayActiveToolBox = this->overlayVerticalToolBox;
            isVisible = true;
            break;
        case Qt::RightDockWidgetArea:
            CaretAssertMessage(0, "Overlay toolbox not allowed on right");
            break;
        case Qt::TopDockWidgetArea:
            CaretAssertMessage(0, "Overlay toolbox not allowed on top");
            break;
        case Qt::BottomDockWidgetArea:
            this->overlayVerticalToolBox->setVisible(false);
            this->overlayHorizontalToolBox->setFloating(false);
            this->addDockWidget(Qt::BottomDockWidgetArea, 
                                this->overlayHorizontalToolBox);
            this->overlayHorizontalToolBox->setVisible(true);
            this->overlayActiveToolBox = this->overlayHorizontalToolBox;
            isVisible = true;
            break;
        default:
            this->overlayActiveToolBox->setVisible(true);
            this->overlayActiveToolBox->setFloating(true);
            isVisible = true;
            break;
    }
    
    this->processShowOverlayToolBox(isVisible);

//    if (this->toolBox->isVisible() == false) {
//        this->viewMenuShowToolBoxAction->trigger();
//    }
    /*
     * This code will allow the region of the main window
     * containing the dock widget to shrink without changing 
     * the vertical size of the OpenGL graphics widget
     * and without changing the width of the main window.
     *
    const int centralMinHeight = this->centralWidget()->minimumHeight();
    const int centralMaxHeight = this->centralWidget()->maximumHeight();
    this->centralWidget()->setFixedHeight(this->centralWidget()->height());
    const int minWidth = this->minimumWidth();
    const int maxWidth = this->maximumWidth();
    this->setFixedWidth(this->width());
    this->adjustSize();
    this->setMinimumWidth(minWidth);
    this->setMaximumWidth(maxWidth);
    this->centralWidget()->setMinimumHeight(centralMinHeight);
    this->centralWidget()->setMaximumHeight(centralMaxHeight);
*/
    /*
    switch (area) {
        case Qt::LeftDockWidgetArea:
            break;
        case Qt::RightDockWidgetArea:
            break;
        case Qt::TopDockWidgetArea:
            this->toolBox->setFloating(false);
            this->addDockWidget(Qt::TopDockWidgetArea, this->toolBox);
            break;
        case Qt::BottomDockWidgetArea:
            this->toolBox->setFloating(false);
            this->addDockWidget(Qt::BottomDockWidgetArea, this->toolBox);
            break;
        default:
            this->toolBox->setFloating(true);
            break;
    }
*/
}

/**
 * Shrink the toolbox after a control is removed.
 */
void 
BrainBrowserWindow::shrinkToolbox()
{
//    if (dynamic_cast<BrainBrowserWindowOrientedToolBox*>(this->toolBox) != NULL) {
//        return;
//    }
//    
//    if (this->toolBox->isFloating() == false) {
//        switch (this->dockWidgetArea(this->toolBox)) {
//            case Qt::LeftDockWidgetArea:
//                this->toolBox->setFloating(false);
//                this->addDockWidget(Qt::LeftDockWidgetArea, 
//                                    this->toolBox,
//                                    Qt::Horizontal);
//                break;
//            case Qt::RightDockWidgetArea:
//                this->toolBox->setFloating(false);
//                this->addDockWidget(Qt::LeftDockWidgetArea, 
//                                    this->toolBox,
//                                    Qt::Horizontal);
//                break;
//            case Qt::TopDockWidgetArea:
//                this->toolBox->setFloating(false);
//                this->addDockWidget(Qt::TopDockWidgetArea, 
//                                    this->toolBox,
//                                    Qt::Horizontal);
//                break;
//            case Qt::BottomDockWidgetArea:
//                this->toolBox->setFloating(false);
//                this->addDockWidget(Qt::BottomDockWidgetArea, 
//                                    this->toolBox,
//                                    Qt::Horizontal);
//                break;
//            default:
//                this->toolBox->setFloating(true);
//                break;
//        }
//    }
//     /*
//     * This code will allow the region of the main window
//     * containing the dock widget to shrink without changing 
//     * the vertical size of the OpenGL graphics widget
//     * and without changing the width of the main window.
//     */
//     const int centralMinHeight = this->centralWidget()->minimumHeight();
//     const int centralMaxHeight = this->centralWidget()->maximumHeight();
//     this->centralWidget()->setFixedHeight(this->centralWidget()->height());
//     const int minWidth = this->minimumWidth();
//     const int maxWidth = this->maximumWidth();
//     this->setFixedWidth(this->width());
//     this->toolBox->adjustSize();
//     this->setFixedWidth(this->width());
//     this->adjustSize();
//     this->setMinimumWidth(minWidth);
//     this->setMaximumWidth(maxWidth);
//     this->centralWidget()->setMinimumHeight(centralMinHeight);
//     this->centralWidget()->setMaximumHeight(centralMaxHeight);
}

/**
 * Remove and return all tabs from this toolbar.
 * After this the window containing this toolbar 
 * will contain no tabs!
 *
 * @param allTabContent
 *    Will contain the content from the tabs upon return.
 */
void 
BrainBrowserWindow::removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent)
{
    this->toolbar->removeAndReturnAllTabs(allTabContent);
}

/**
 * @return Return the active browser tab content in
 * this browser window.
 */
BrowserTabContent* 
BrainBrowserWindow::getBrowserTabContent()
{
    return this->toolbar->getTabContentFromSelectedTab();
}

/**
 * Returns a popup menu for the main window.
 * Overrides that in QMainWindow and prevents the 
 * default context menu from appearing.
 *
 * @return Context menu for display or NULL if
 * nothing available. 
 */
QMenu* 
BrainBrowserWindow::createPopupMenu()
{
    return NULL;
}

/**
 * Capture an image of the window's graphics area using 
 * the given image size.  If either of the image dimensions
 * is zero, the image will be the size of the graphcis 
 * area.
 *
 * @param imageSizeX
 *    Desired X size of image.
 * @param imageSizeY
 *    Desired X size of image.
 * @return
 *    An image of the graphics area.
 */
QImage 
BrainBrowserWindow::captureImageOfGraphicsArea(const int32_t imageSizeX,
                                               const int32_t imageSizeY)
{
    QImage image = this->openGLWidget->captureImage(imageSizeX, 
                                                    imageSizeY);
    
    return image;
}

/**
 * Open a connection to the allen brain institute database.
 */
void 
BrainBrowserWindow::processConnectToAllenDataBase()
{
    GuiManager::get()->processShowAllenDataBaseWebView(this);
}

/**
 * Open a connection to the human connectome project database.
 */
void 
BrainBrowserWindow::processConnectToConnectomeDataBase()
{
    GuiManager::get()->processShowConnectomeDataBaseWebView(this);
}

/**
 * Load the HCP Website into the user's web browser.
 */
void 
BrainBrowserWindow::processHcpWebsiteInBrowser()
{
    QUrl url("https://humanconnectome.org");
    QDesktopServices::openUrl(url);
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
BrainBrowserWindow::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainBrowserWindow",
                                            1);
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
BrainBrowserWindow::restoreFromScene(const SceneAttributes* sceneAttributes,
                             const SceneClass* sceneClass)
{
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
    
}



