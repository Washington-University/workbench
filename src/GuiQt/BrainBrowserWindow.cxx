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
#include "EventBrowserWindowCreateTabs.h"
#include "EventDataFileRead.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FociProjectionDialog.h"
#include "GuiManager.h"
#include "ManageLoadedFilesDialog.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "ProgressReportingDialog.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneFile.h"
#include "SceneWindowGeometry.h"
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
                                       const CreateDefaultTabsMode createDefaultTabsMode,
                                       QWidget* parent,
                                       Qt::WindowFlags flags)
: QMainWindow(parent, flags)
{
    if (BrainBrowserWindow::s_firstWindowFlag) {
        BrainBrowserWindow::s_firstWindowFlag = false;
    }
    
    m_screenMode = BrainBrowserWindowScreenModeEnum::NORMAL;
    
    GuiManager* guiManager = GuiManager::get();
    
    setAttribute(Qt::WA_DeleteOnClose);
    
    m_browserWindowIndex = browserWindowIndex;
    
    setWindowTitle(guiManager->applicationName() 
                         + " "
                         + AString::number(m_browserWindowIndex + 1));
    
    m_openGLWidget = new BrainOpenGLWidget(this,
                                               browserWindowIndex);
    
    const int openGLSizeX = 500;
    const int openGLSizeY = (WuQtUtilities::isSmallDisplay() ? 200 : 375);
    m_openGLWidget->setMinimumSize(openGLSizeX, 
                                       openGLSizeY);
    
    setCentralWidget(m_openGLWidget);
    
    m_overlayVerticalToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Overlay ToolBox",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_OVERLAYS_VERTICAL,
                                          this);
    m_overlayVerticalToolBox->setAllowedAreas(Qt::LeftDockWidgetArea);
    
    m_overlayHorizontalToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Overlay ToolBox ",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_OVERLAYS_HORIZONTAL,
                                          this);
    m_overlayHorizontalToolBox->setAllowedAreas(Qt::BottomDockWidgetArea);

    if (WuQtUtilities::isSmallDisplay()) {
        m_overlayActiveToolBox = m_overlayVerticalToolBox;
        addDockWidget(Qt::LeftDockWidgetArea, m_overlayVerticalToolBox);
        m_overlayHorizontalToolBox->setVisible(false);
        //m_overlayHorizontalToolBox->toggleViewAction()->trigger();
    }
    else {
        m_overlayActiveToolBox = m_overlayHorizontalToolBox;
        addDockWidget(Qt::BottomDockWidgetArea, m_overlayHorizontalToolBox);
        m_overlayVerticalToolBox->setVisible(false);
        //m_overlayVerticalToolBox->toggleViewAction()->trigger();
    }
    
    QObject::connect(m_overlayHorizontalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayHorizontalToolBoxVisibilityChanged(bool)));
    QObject::connect(m_overlayVerticalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayVerticalToolBoxVisibilityChanged(bool)));
    
    m_featuresToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Features ToolBox",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_FEATURES,
                                          this);
    m_featuresToolBox->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_featuresToolBox);
    
    createActionsUsedByToolBar();
    m_overlayToolBoxAction->blockSignals(true);
    m_overlayToolBoxAction->setChecked(true);
    m_overlayToolBoxAction->blockSignals(false);
    m_featuresToolBoxAction->blockSignals(true);
    m_featuresToolBoxAction->setChecked(true);
    m_featuresToolBoxAction->blockSignals(false);
    
    m_toolbar = new BrainBrowserWindowToolBar(m_browserWindowIndex,
                                                  browserTabContent,
                                                  m_overlayToolBoxAction,
                                                  m_featuresToolBoxAction,
                                                  this);
    m_showToolBarAction = m_toolbar->toolBarToolButtonAction;
    addToolBar(m_toolbar);
    
    createActions();
    
    createMenus();
     
    m_toolbar->updateToolBar();

    processShowOverlayToolBox(m_overlayToolBoxAction->isChecked());
    processHideFeaturesToolBox();
    
    if (browserTabContent == NULL) {
        if (createDefaultTabsMode == CREATE_DEFAULT_TABS_YES) {
            m_toolbar->addDefaultTabsAfterLoadingSpecFile();
        }
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_defaultWindowComponentStatus.isFeaturesToolBoxDisplayed = m_featuresToolBoxAction->isChecked();
    m_defaultWindowComponentStatus.isOverlayToolBoxDisplayed  = m_overlayActiveToolBox->isVisible();
    m_defaultWindowComponentStatus.isToolBarDisplayed = m_showToolBarAction->isChecked();
}
/**
 * Destructor.
 */
BrainBrowserWindow::~BrainBrowserWindow()
{
    delete m_sceneAssistant;
}

/**
 * @return The screen mode (normal, full screen, montage, etc)
 */
BrainBrowserWindowScreenModeEnum::Enum 
BrainBrowserWindow::getScreenMode() const
{
    return m_screenMode;
}

/**
 * @return the index of this browser window.
 */
int32_t 
BrainBrowserWindow::getBrowserWindowIndex() const 
{ 
    return m_browserWindowIndex; 
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
                                                   m_toolbar->tabBar->count())) {
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
    m_overlayToolBoxAction = 
    WuQtUtilities::createAction("Overlay ToolBox",
                                "Overlay ToolBox",
                                this,
                                this,
                                SLOT(processShowOverlayToolBox(bool)));
    m_overlayToolBoxAction->setCheckable(true);
    if (overlayToolBoxIconValid) {
        m_overlayToolBoxAction->setIcon(overlayToolBoxIcon);
        m_overlayToolBoxAction->setIconVisibleInMenu(false);
    }
    else {
        m_overlayToolBoxAction->setIconText("OT");
    }

    /*
     * Note: The name of a dock widget becomes its
     * name in the toggleViewAction().  So, use
     * a separate action here so that the name in 
     * the menu is as set here.
     */
    m_featuresToolBoxAction = m_featuresToolBox->toggleViewAction();
    m_featuresToolBoxAction->setCheckable(true);
    QObject::connect(m_featuresToolBoxAction, SIGNAL(triggered(bool)),
                     this, SLOT(processShowFeaturesToolBox(bool)));
    if (featuresToolBoxIconValid) {
        m_featuresToolBoxAction->setIcon(featuresToolBoxIcon);
        m_featuresToolBoxAction->setIconVisibleInMenu(false);
    }
    else {
        m_featuresToolBoxAction->setIconText("LT");
    }
    
    /*
     * Scene window.
     */
    QIcon clapBoardIcon;
    const bool clapBoardIconValid = WuQtUtilities::loadIcon(":/toolbar_clapboard_icon.png", 
                                                            clapBoardIcon);
    
    m_showSceneDialogAction = WuQtUtilities::createAction("Scenes...",
                                                          "Displays the Scene Window",
                                                          this,
                                                          this,
                                                          SLOT(processShowSceneDialog()));
    if (clapBoardIconValid) {
        m_showSceneDialogAction->setIcon(clapBoardIcon);
        m_showSceneDialogAction->setIconVisibleInMenu(false);
    }
    else {
        m_showSceneDialogAction->setIconText("Scenes");
    }
    m_showSceneDialogAction->setCheckable(false);
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
 * Show the surface properties editor dialog.
 */
void
BrainBrowserWindow::processShowSurfacePropertiesDialog()
{
    GuiManager::get()->processShowSurfacePropertiesEditorDialog(this);
}

/**
 * Create actions for this window.
 * NOTE: This is called AFTER the toolbar is created.
 */
void 
BrainBrowserWindow::createActions()
{
    CaretAssert(m_toolbar);
    
    GuiManager* guiManager = GuiManager::get();
    
    m_aboutWorkbenchAction =
    WuQtUtilities::createAction("About Workbench...",
                                "Information about Workbench",
                                this,
                                this,
                                SLOT(processAboutWorkbench()));
    
    m_newWindowAction =
    WuQtUtilities::createAction("New Window",
                                "Creates a new window for viewing brain models",
                                Qt::CTRL+Qt::Key_N,
                                this,
                                this,
                                SLOT(processNewWindow()));
    
    m_newTabAction =
    WuQtUtilities::createAction("New Tab", 
                                "Create a new tab (window pane) in the window",
                                Qt::CTRL + Qt::Key_T,
                                this,
                                this,
                                SLOT(processNewTab()));
    
    m_openFileAction =
    WuQtUtilities::createAction("Open File...", 
                                "Open a data file including a spec file located on the computer",
                                Qt::CTRL+Qt::Key_O,
                                this,
                                this,
                                SLOT(processDataFileOpen()));
    
    m_openLocationAction = 
    WuQtUtilities::createAction("Open Location...", 
                                "Open a data file including a spec file located on a web server (http)",
                                Qt::CTRL+Qt::Key_L,
                                this,
                                this,
                                SLOT(processDataFileLocationOpen()));
    
    m_openFileViaSpecFileAction =
    WuQtUtilities::createAction("Open File via Spec File...", 
                                "Open a data file listed in the Spec File",
                                Qt::CTRL + Qt::SHIFT + Qt::Key_O,
                                this,
                                this,
                                SLOT(processDataFileOpenFromSpecFile()));
    
    
    m_manageFilesAction =
    WuQtUtilities::createAction("Save/Manage Files...", 
                                "Save and Manage Loaded Files",
                                Qt::CTRL + Qt::Key_S,
                                this,
                                this,
                                SLOT(processManageSaveLoadedFiles()));
    
    m_closeSpecFileAction =
    WuQtUtilities::createAction("Close Spec File",
                                "Close the Spec File",
                                this,
                                this,
                                SLOT(processCloseSpecFile()));
    
    m_closeTabAction =
    WuQtUtilities::createAction("Close Tab",
                                "Close the active tab (window pane) in the window",
                                Qt::CTRL + Qt::Key_W,
                                this,
                                m_toolbar,
                                SLOT(closeSelectedTab()));
    
    m_closeWindowAction = 
    WuQtUtilities::createAction("Close Window",
                                "Close the window",
                                Qt::CTRL + Qt::SHIFT + Qt::Key_W,
                                this,
                                this,
                                SLOT(close()));
    
    m_captureImageAction =
    WuQtUtilities::createAction("Capture Image...",
                                "Capture an Image of the windows content",
                                this,
                                this,
                                SLOT(processCaptureImage()));

    m_recordMovieAction = 
    WuQtUtilities::createAction("Animation Control...",
                                "Animate Brain Surface",
                                this,
                                this,
                                SLOT(processRecordMovie()));
    
    m_preferencesAction = 
    WuQtUtilities::createAction("Preferences...",
                                "Edit the preferences",
                                this,
                                this,
                                SLOT(processEditPreferences()));
    
    m_exitProgramAction =
    WuQtUtilities::createAction("Exit", 
                                "Exit (quit) the program",
                                Qt::CTRL+Qt::Key_Q, 
                                this,
                                this,
                                SLOT(processExitProgram()));
    
    m_fociProjectAction =
    WuQtUtilities::createAction("Project Foci...",
                                "Project Foci to Surfaces",
                                this,
                                this,
                                SLOT(processProjectFoci()));
    
    m_viewScreenNormalAction = WuQtUtilities::createAction("Normal",
                                                               "Normal Viewing", 
                                                               Qt::Key_Escape, 
                                                               this);
    m_viewScreenNormalAction->setCheckable(true);
    
    m_viewScreenFullAction = WuQtUtilities::createAction("Full Screen", 
                                                             "View using all of screen", 
                                                             Qt::CTRL+Qt::Key_F, 
                                                             this);
    m_viewScreenFullAction->setCheckable(true);
    
    m_viewScreenMontageTabsAction = WuQtUtilities::createAction("Tile Tabs", 
                                                                    "View all tabs in a tiled layout", 
                                                                    Qt::CTRL+Qt::Key_M, 
                                                                    this);
    m_viewScreenMontageTabsAction->setCheckable(true);
    
    m_viewScreenFullMontageTabsAction = WuQtUtilities::createAction("Tile Tabs (Full Screen)", 
                                                                        "View all tabs in a tiled layout using all of screen", 
                                                                        Qt::CTRL+Qt::SHIFT+Qt::Key_M, 
                                                                        this);
    m_viewScreenFullMontageTabsAction->setCheckable(true);
    
    m_viewScreenActionGroup = new QActionGroup(this);
    QObject::connect(m_viewScreenActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(processViewScreenActionGroupSelection(QAction*)));
    m_viewScreenActionGroup->setExclusive(true);
    m_viewScreenActionGroup->addAction(m_viewScreenNormalAction);
    m_viewScreenActionGroup->addAction(m_viewScreenFullAction);
    m_viewScreenActionGroup->addAction(m_viewScreenMontageTabsAction);
    m_viewScreenActionGroup->addAction(m_viewScreenFullMontageTabsAction);
    
    m_viewScreenActionGroup->blockSignals(true);
    m_viewScreenNormalAction->setChecked(true);
    m_viewScreenActionGroup->blockSignals(false);
        
    m_nextTabAction =
    WuQtUtilities::createAction("Next Tab",
                                "Move to the next tab",
                                Qt::CTRL + Qt::Key_Right,
                                this,
                                m_toolbar,
                                SLOT(nextTab()));
    
    m_previousTabAction =
    WuQtUtilities::createAction("Previous Tab",
                                "Move to the previous tab",
                                Qt::CTRL + Qt::Key_Left,
                                this,
                                m_toolbar,
                                SLOT(previousTab()));
    
    m_renameSelectedTabAction =
    WuQtUtilities::createAction("Rename Selected Tab...",
                                "Change the name of the selected tab",
                                this,
                                m_toolbar,
                                SLOT(renameTab()));
    
    m_moveTabsInWindowToNewWindowsAction =
    WuQtUtilities::createAction("Move All Tabs in Current Window to New Windows",
                                "Move all but the left most tab to new windows",
                                this,
                                m_toolbar,
                                SLOT(moveTabsToNewWindows()));
    
    m_moveTabsFromAllWindowsToOneWindowAction =
    WuQtUtilities::createAction("Move All Tabs From All Windows Into Selected Window",
                                "Move all tabs from all windows into selected window",
                                this,
                                this,
                                SLOT(processMoveAllTabsToOneWindow()));
    
    m_bringAllToFrontAction =
    WuQtUtilities::createAction("Bring All To Front",
                                "Move all windows on top of other application windows",
                                this,
                                guiManager,
                                SLOT(processBringAllWindowsToFront()));
    
    m_informationDialogAction =
    WuQtUtilities::createAction("Information Window",
                                "Show the Informaiton Window",
                                this,
                                guiManager,
                                SLOT(processShowInformationWindow()));
    
    m_helpHcpWebsiteAction =
    WuQtUtilities::createAction("Go to HCP Website...",
                                "Load the HCP Website in your computer's web browser",
                                this,
                                this,
                                SLOT(processHcpWebsiteInBrowser()));
    
    m_helpOnlineAction =
    WuQtUtilities::createAction("Show Help (Online)...",
                                "Show the Help Window",
                                this,
                                guiManager,
                                SLOT(processShowHelpOnlineWindow()));
    m_helpOnlineAction->setEnabled(false);
    
    m_helpSearchOnlineAction =
    WuQtUtilities::createAction("Search Help (Online)...",
                                "Show the Search Helper Window",
                                this,
                                guiManager,
                                SLOT(processShowSearchHelpOnlineWindow()));
    m_helpSearchOnlineAction->setEnabled(false);

    m_helpGraphicsTimingAction =
    WuQtUtilities::createAction("Time Graphics Update",
                                "Show the average time for updating the windows graphics",
                                this,
                                this,
                                SLOT(processGraphicsTiming()));
    
    m_connectToAllenDatabaseAction =
    WuQtUtilities::createAction("Allen Brain Institute Database...",
                                "Open a connection to the Allen Brain Institute Database",
                                this,
                                this,
                                SLOT(processConnectToAllenDataBase()));
    m_connectToAllenDatabaseAction->setEnabled(false);
    
    m_connectToConnectomeDatabaseAction =
    WuQtUtilities::createAction("Human Connectome Project Database...",
                                "Open a connection to the Human Connectome Project Database",
                                this,
                                this,
                                SLOT(processConnectToConnectomeDataBase()));
    m_connectToConnectomeDatabaseAction->setEnabled(false);
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
    QMenuBar* menubar = menuBar();
    menubar->addMenu(createMenuFile());
    menubar->addMenu(createMenuView());
    QMenu* dataMenu = createMenuData();
    if (dataMenu != NULL) {
        menubar->addMenu(dataMenu);
    }
    menubar->addMenu(createMenuSurface());
    QMenu* volumeMenu = createMenuVolume();
    if (volumeMenu != NULL) {
        menubar->addMenu(volumeMenu);
    }
    menubar->addMenu(createMenuConnect());
    menubar->addMenu(createMenuWindow());
    menubar->addMenu(createMenuHelp());    
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

    menu->addAction(m_aboutWorkbenchAction);
    menu->addAction(m_preferencesAction);
#ifndef CARET_OS_MACOSX
    menu->addSeparator();
#endif // CARET_OS_MACOSX
    menu->addAction(m_newWindowAction);
    menu->addAction(m_newTabAction);
    menu->addSeparator();
    menu->addAction(m_openFileAction);
    menu->addAction(m_openLocationAction);
    m_recentSpecFileMenu = menu->addMenu("Open Recent Spec File");
    QObject::connect(m_recentSpecFileMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processRecentSpecFileMenuAboutToBeDisplayed()));
    QObject::connect(m_recentSpecFileMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processRecentSpecFileMenuSelection(QAction*)));
    menu->addAction(m_openFileViaSpecFileAction);
    menu->addAction(m_manageFilesAction);
    menu->addAction(m_closeSpecFileAction);
    menu->addSeparator();
    menu->addAction(m_recordMovieAction);
    menu->addAction(m_captureImageAction);
    menu->addSeparator();
    menu->addAction(m_closeTabAction);
    menu->addAction(m_closeWindowAction);
    menu->addSeparator();
#ifndef CARET_OS_MACOSX
    menu->addSeparator();
#endif // CARET_OS_MACOSX
    menu->addAction(m_exitProgramAction);
    
    return menu;
}

/**
 * Called to display the overlay toolbox.
 */
void 
BrainBrowserWindow::processShowOverlayToolBox(bool status)
{
    m_overlayActiveToolBox->setVisible(status);
    m_overlayToolBoxAction->blockSignals(true);
    m_overlayToolBoxAction->setChecked(status);
    m_overlayToolBoxAction->blockSignals(false);
    if (status) {
        m_overlayToolBoxAction->setToolTip("Hide Overlay Toolbox");
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBox().getPointer());
    }
    else {
        m_overlayToolBoxAction->setToolTip("Show Overlay Toolbox");
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
        if (m_overlayActiveToolBox == m_overlayHorizontalToolBox) {
            m_overlayToolBoxAction->blockSignals(true);
            m_overlayToolBoxAction->setChecked(false);
            m_overlayToolBoxAction->blockSignals(false);
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
        if (m_overlayActiveToolBox == m_overlayVerticalToolBox) {
            m_overlayToolBoxAction->blockSignals(true);
            m_overlayToolBoxAction->setChecked(false);
            m_overlayToolBoxAction->blockSignals(false);
        }
    }
}

/**
 * Called when File Menu is about to show.
 */
void 
BrainBrowserWindow::processFileMenuAboutToShow()
{
    const bool enabled = GuiManager::get()->getBrain()->isSpecFileValid();
    
    m_openFileViaSpecFileAction->setEnabled(enabled);
}

/**
 * Called when Open Recent Spec File Menu is about to be displayed
 * and creates the content of the menu.
 */
void 
BrainBrowserWindow::processRecentSpecFileMenuAboutToBeDisplayed()
{
    m_recentSpecFileMenu->clear();
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    std::vector<AString> recentSpecFiles;
    prefs->getPreviousSpecFiles(recentSpecFiles);
    
    const int32_t numRecentSpecFiles = static_cast<int>(recentSpecFiles.size());
    for (int32_t i = 0; i < numRecentSpecFiles; i++) {
        AString actionName;
        AString actionFullPath;
        if (DataFile::isFileOnNetwork(recentSpecFiles[i])) {
            actionName     = recentSpecFiles[i];
            actionFullPath = recentSpecFiles[i];
        }
        else {
            FileInformation fileInfo(recentSpecFiles[i]);
            QString path = fileInfo.getPathName();
            QString name = fileInfo.getFileName();
            if (path.isEmpty() == false) {
                name += (" (" + path + ")");
            }
            actionName = name;
            actionFullPath = fileInfo.getFilePath();
        }
        
        QAction* action = new QAction(actionName,
                                      m_recentSpecFileMenu);
        action->setData(actionFullPath);
        m_recentSpecFileMenu->addAction(action);
    } 
    
    if (numRecentSpecFiles > 0) {
        m_recentSpecFileMenu->addSeparator();
        QAction* action = new QAction("Clear Menu",
                                      m_recentSpecFileMenu);
        action->setData("CLEAR_CLEAR");
        m_recentSpecFileMenu->addAction(action);
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
        
        if (errorMessages.isEmpty()) {
            SpecFileDialog* sfd = SpecFileDialog::createForLoadingSpecFile(&specFile,
                                                                           this);
            if (sfd->exec() == QDialog::Accepted) {
                EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
                                                             &specFile);
                
                ProgressReportingDialog::runEvent(&readSpecFileEvent,
                                                  this,
                                                  specFile.getFileNameNoPath());
                
                if (readSpecFileEvent.isError()) {
                    if (errorMessages.isEmpty() == false) {
                        errorMessages += "\n";
                    }
                    errorMessages += readSpecFileEvent.getErrorMessage();
                }
            }
            
            delete sfd;
            sfd = NULL;
            
            m_toolbar->addDefaultTabsAfterLoadingSpecFile();
        }
        
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
    
    menu->addAction(m_showToolBarAction);
    menu->addMenu(createMenuViewMoveFeaturesToolBox());
    menu->addMenu(createMenuViewMoveOverlayToolBox());
    menu->addSeparator();

    /*menu->addAction(m_recordMovieAction);
    menu->addSeparator();*/
    
    QMenu* screenMenu = new QMenu("Screen");
    screenMenu->addAction(m_viewScreenNormalAction);
    screenMenu->addAction(m_viewScreenFullAction);
    screenMenu->addAction(m_viewScreenMontageTabsAction);
    screenMenu->addAction(m_viewScreenFullMontageTabsAction);

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
    
    menu->addAction(m_connectToAllenDatabaseAction);
    menu->addAction(m_connectToConnectomeDatabaseAction);
    
    return menu;
}

/**
 * Create the data menu.
 * @return the data menu.
 */
QMenu* 
BrainBrowserWindow::createMenuData()
{
    QMenu* menu = new QMenu("Data", this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(processDataMenuAboutToShow()));
    
    menu->addAction(m_fociProjectAction);
    
    return menu;
}

/**
 * Called when Data Menu is about to show.
 */
void
BrainBrowserWindow::processDataMenuAboutToShow()
{
    bool haveFociFiles = (GuiManager::get()->getBrain()->getNumberOfFociFiles() > 0);
    m_fociProjectAction->setEnabled(haveFociFiles);
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
    
    menu->addAction("Properties...",
                    this,
                    SLOT(processShowSurfacePropertiesDialog()));
    
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
    BrowserTabContent* btc = getBrowserTabContent();  
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
//    QMenu* menu = new QMenu("Volume", this);
//    return menu;
    return NULL;
}

/**
 * Create the window menu.
 * @return the window menu.
 */
QMenu* 
BrainBrowserWindow::createMenuWindow()
{
    m_moveSelectedTabToWindowMenu = new QMenu("Move Selected Tab to Window");
    QObject::connect(m_moveSelectedTabToWindowMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processMoveSelectedTabToWindowMenuAboutToBeDisplayed()));
    QObject::connect(m_moveSelectedTabToWindowMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processMoveSelectedTabToWindowMenuSelection(QAction*)));
    
    QMenu* menu = new QMenu("Window", this);
    
    menu->addAction(m_nextTabAction);
    menu->addAction(m_previousTabAction);
    menu->addAction(m_renameSelectedTabAction);
    menu->addSeparator();
    menu->addAction(m_moveTabsInWindowToNewWindowsAction);
    menu->addAction(m_moveTabsFromAllWindowsToOneWindowAction);
    menu->addMenu(m_moveSelectedTabToWindowMenu);
    menu->addSeparator();
    menu->addAction(m_informationDialogAction);
    menu->addAction(m_showSceneDialogAction);
    menu->addSeparator();
    menu->addAction(m_bringAllToFrontAction);
    
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
    
    menu->addAction(m_helpHcpWebsiteAction);
    menu->addSeparator();
    menu->addAction(m_helpOnlineAction);
    menu->addAction(m_helpSearchOnlineAction);
    menu->addSeparator();
    menu->addAction(m_helpGraphicsTimingAction);
    
    return menu;
}

/**
 * Time the graphics drawing.
 */
void
BrainBrowserWindow::processGraphicsTiming()
{
    ElapsedTimer et;
    et.start();
    
    const int32_t numTimes = 5;
    for (int32_t i = 0; i < numTimes; i++) {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
    
    const float time = et.getElapsedTimeSeconds() / numTimes;
    const AString timeString = AString::number(time, 'f', 5);
    
    const AString msg = ("Time to draw graphics (seconds): "
                         + timeString);
    WuQMessageBox::informationOk(this, msg);
}

/**
 * Project foci.
 */
void
BrainBrowserWindow::processProjectFoci()
{    
    FociProjectionDialog fpd(this);
    fpd.exec();
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
 * Called when record movie is selected.
 */
void 
BrainBrowserWindow::processRecordMovie()
{
    GuiManager::get()->processShowMovieDialog(this);
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
    if (s_previousOpenFileNameFilter.isEmpty()) {
        s_previousOpenFileNameFilter = 
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
    addFileToSpecFileCheckBox->setChecked(false);
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
    fd.selectNameFilter(s_previousOpenFileNameFilter);
    fd.addWidget(extraWidget);
    if (s_previousOpenFileDirectory.isEmpty() == false) {
        FileInformation fileInfo(s_previousOpenFileDirectory);
        if (fileInfo.exists()) {
            fd.setDirectory(s_previousOpenFileDirectory);
        }
    }
    
    AString errorMessages;
    
    if (fd.exec() == CaretFileDialogExtendable::Accepted) {
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
                //addDataFileToSpecFileMode = ADD_DATA_FILE_TO_SPEC_FILE_YES;
            }
            else {
                if (addFileToSpecFileCheckBox->isChecked()) {
                    Brain* brain = GuiManager::get()->getBrain();
                    FileInformation fileInfo(brain->getSpecFileName());
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
                loadFiles(filenamesVector,
                          LOAD_SPEC_FILE_WITH_DIALOG,
                          addDataFileToSpecFileMode,
                          "",
                          "");
            }
        }
        s_previousOpenFileNameFilter = fd.selectedNameFilter();
        s_previousOpenFileDirectory  = fd.directory().absolutePath();
    }
}

/**
 * Load files that are on the network
 * @param
 *    List of filenames to read.
 * @param username
 *    Username for network file reading
 * @param password
 *    Password for network file reading
 */
void
BrainBrowserWindow::loadFilesFromNetwork(const std::vector<AString>& filenames,
                                         const AString& username,
                                         const AString& password)
{    
//    this->loadFilesFromCommandLine(filenames,
//                                            BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE);
    loadFiles(filenames,
              BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE,
              ADD_DATA_FILE_TO_SPEC_FILE_NO,
              username,
              password);
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
    loadFiles(filenames,
              loadSpecFileMode,
              ADD_DATA_FILE_TO_SPEC_FILE_NO,
              "",
              "");
}

/**
 * Load the scene file and the scene with the given name or number
 * @param sceneFileName
 *    Name of scene file.
 * @param sceneNameOrNumber
 *    Name or number of scene.  Name takes precedence over number. 
 *    Scene numbers start at one.
 */
void
BrainBrowserWindow::loadSceneFromCommandLine(const AString& sceneFileName,
                                             const AString& sceneNameOrNumber)
{
    std::vector<AString> filenames;
    filenames.push_back(sceneFileName);
    
    loadFilesFromCommandLine(filenames,
                             LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE);
    
    bool haveSceneFileError = true;
    bool haveSceneError = true;
    FileInformation fileInfo(sceneFileName);
    const AString nameNoExt = fileInfo.getFileName();
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numSceneFiles = brain->getNumberOfSceneFiles();
    for (int32_t i = 0; i < numSceneFiles; i++) {
        SceneFile* sf = brain->getSceneFile(i);
        if (sf->getFileName().contains(sceneFileName)) {
            haveSceneFileError = false;
            Scene* scene = sf->getSceneWithName(sceneNameOrNumber);
            if (scene == NULL) {
                bool isValidNumber = false;
                int sceneNumber = sceneNameOrNumber.toInt(&isValidNumber);
                if (isValidNumber) {
                    sceneNumber--;  // convert to index (numbers start at one)
                    if ((sceneNumber >= 0)
                        && (sceneNumber < sf->getNumberOfScenes())) {
                        scene = sf->getSceneAtIndex(sceneNumber);
                    }
                }
            }
            
            if (scene != NULL) {
                GuiManager::get()->processShowSceneDialogAndScene(this,
                                                                  sf,
                                                                  scene);
                haveSceneError = false;
                break;
            }
        }
    }
    
    if (haveSceneFileError) {
        const AString msg = ("No scene file named \""
                             + sceneFileName
                             + "\" was loaded.");
        WuQMessageBox::errorOk(this, msg);
    }
    else if (haveSceneError) {
        const AString msg = ("No scene with name/number \""
                             + sceneNameOrNumber
                             + "\" found in scene file.");
        WuQMessageBox::errorOk(this, msg);
    }
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
 * @param username
 *    Username for network file reading
 * @param password
 *    Password for network file reading
 */
void 
BrainBrowserWindow::loadFiles(const std::vector<AString>& filenames,
                              const LoadSpecFileMode loadSpecFileMode,
                              const AddDataFileToSpecFileMode addDataFileToSpecFileMode,
                              const AString& username,
                              const AString& password)
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
    
    /*
     * Load files in this order:
     * (1) Spec File - Limit to one.
     * (2) Volume File
     * (3) Surface File
     * (4) All other files.
     */
    std::vector<AString> filenamesToLoad;
//    if (specFileName.isEmpty() == false) {
//        filenamesToLoad.push_back(specFileName);
//    }
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
    bool sceneFileWasLoaded = false;
    
    /*
     * Load spec file (before data files)
     */
    if (specFileName.isEmpty() == false) {
        SpecFile specFile;
        try {
            specFile.readFile(specFileName);
        }
        catch (const DataFileException& e) {
            errorMessages += e.whatString();
            QMessageBox::critical(this,
                                  "ERROR",
                                  errorMessages);
            return;
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
                if (username.isEmpty() == false) {
                    readSpecFileEvent.setUsernameAndPassword(username,
                                                             password);
                }
                
                ProgressReportingDialog::runEvent(&readSpecFileEvent,
                                                  this,
                                                  specFile.getFileNameNoPath());
                
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
                 * Allow user to choose files listed in the spec file
                 */
                SpecFileDialog* sfd = SpecFileDialog::createForLoadingSpecFile(&specFile,
                                                                               this);
                if (sfd->exec() == QDialog::Accepted) {
                    timer.reset();
                    specFileTimeStart = timer.getElapsedTimeSeconds();
                    
                    EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
                                                                 &specFile);
                    if (username.isEmpty() == false) {
                        readSpecFileEvent.setUsernameAndPassword(username,
                                                                 password);
                    }
                    
                    ProgressReportingDialog::runEvent(&readSpecFileEvent,
                                                      this,
                                                      specFile.getFileNameNoPath());
                    
                    if (readSpecFileEvent.isError()) {
                        if (errorMessages.isEmpty() == false) {
                            errorMessages += "\n";
                        }
                        errorMessages += readSpecFileEvent.getErrorMessage();
                    }
                    specFileTimeEnd = timer.getElapsedTimeSeconds();
                    
                    createDefaultTabsFlag = true;
                }
                
                delete sfd;
            }
                break;
        }
        
        sceneFileWasLoaded = specFile.areAllSelectedFilesSceneFiles();
    }
    
    /*
     * Prepare to load any data files
     */
    bool addDataFileToSpecFile = false;
    switch (addDataFileToSpecFileMode) {
        case ADD_DATA_FILE_TO_SPEC_FILE_NO:
            addDataFileToSpecFile = false;
            break;
        case ADD_DATA_FILE_TO_SPEC_FILE_YES:
            addDataFileToSpecFile = true;
            break;
    }
    EventDataFileRead loadFilesEvent(GuiManager::get()->getBrain(),
                                    addDataFileToSpecFile);
    if (username.isEmpty() == false) {
        loadFilesEvent.setUsernameAndPassword(username,
                                             password);
    }
    
    /*
     * Add data files to data file loading event (after loading spec file)
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
            loadFilesEvent.addDataFile(fileType,
                                       name);
        }
        else {
            if (errorMessages.isEmpty() == false) {
                errorMessages += "\n";
            }
            errorMessages += ("Extension for " + name + " does not match a suppported file type");
        }
    }
    
    /*
     * Now, load the data files
     */
    const int32_t numberOfValidFiles = loadFilesEvent.getNumberOfDataFilesToRead();
    if (numberOfValidFiles > 0) {
        ProgressReportingDialog::runEvent(&loadFilesEvent,
                                          this,
                                          "Loading Data Files");
        errorMessages.appendWithNewLine(loadFilesEvent.getErrorMessage());
        
        /*
         * Check for errors
         */
        for (int32_t i = 0; i < numberOfValidFiles; i++) {
            const AString& dataFileName = loadFilesEvent.getDataFileName(i);
            const DataFileTypeEnum::Enum dataFileType = loadFilesEvent.getDataFileType(i);
            
            const AString shortName = FileInformation(dataFileName).getFileName();
            
            if (loadFilesEvent.isFileErrorInvalidStructure(i)) {
                /*
                 * Allow user to specify the structure
                 */
                WuQDataEntryDialog ded("Structure",
                                       this);
                StructureEnumComboBox* ssc = ded.addStructureEnumComboBox("");
                ded.setTextAtTop(("File \""
                                  + shortName
                                  + "\"\nhas missing or invalid structure, select it's structure."
                                  "\nAfter loading, save file with File Menu->Save Manage Files"
                                  "\nto prevent this error."),
                                 false);
                if (ded.exec() == WuQDataEntryDialog::Accepted) {
                    EventDataFileRead loadFileEventStructure(GuiManager::get()->getBrain(),
                                                             addDataFileToSpecFile);
                    loadFileEventStructure.addDataFile(ssc->getSelectedStructure(),
                                                       dataFileType,
                                                       dataFileName);
                    if (username.isEmpty() == false) {
                        loadFileEventStructure.setUsernameAndPassword(username,
                                                                      password);
                    }
                    
                    ProgressReportingDialog::runEvent(&loadFileEventStructure,
                                                      this,
                                                      ("Loading " + shortName));
                    if (loadFileEventStructure.isError()) {
                        errorMessages.appendWithNewLine(loadFileEventStructure.getErrorMessage());
                    }
                }
                else {
                    errorMessages.appendWithNewLine("File \""
                                                       + shortName
                                                       + "\" not loaded due to invalid structure.");
                }
            }
            else if (loadFilesEvent.isFileError(i) == false) {
                if (dataFileType == DataFileTypeEnum::SCENE) {
                    sceneFileWasLoaded = true;
                }                
            }
        }
    }
    
    
    
    
//    /*
//     * Load each file.
//     */
//    //const int32_t numFilesToLoad = static_cast<int32_t>(filenamesToLoad.size());
//    for (int32_t i = 0; i < numFilesToLoad; i++) {
//        AString name = filenamesToLoad[i];
//        
//        //FileInformation fileInfo(name);
//        //if (fileInfo.isAbsolute()) {
//        //    prefs->addToPreviousOpenFileDirectories(fileInfo.getPathName());
//        //}
//        
//        bool isValidType = false;
//        DataFileTypeEnum::Enum fileType = DataFileTypeEnum::fromFileExtension(name, &isValidType);
//        if (isValidType) {
//            if (fileType == DataFileTypeEnum::SPECIFICATION) {
//                SpecFile specFile;
//                try {
//                    specFile.readFile(name);
//                }
//                catch (const DataFileException& e) {
//                    errorMessages += e.whatString();
//                    cursor.restoreCursor();
//                    QMessageBox::critical(this,
//                                          "ERROR",
//                                          errorMessages);
//                    return;
//                }
//                
//                switch (loadSpecFileMode) {
//                    case LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE:
//                    {
//                        timer.reset(); // resets timer
//                        specFileTimeStart = timer.getElapsedTimeSeconds();
//                        
//                        /*
//                         * Load all files listed in spec file
//                         */
//                        specFile.setAllFilesSelected(true);
//                        
//                        EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
//                                                                     &specFile);
//                        if (username.isEmpty() == false) {
//                            readSpecFileEvent.setUsernameAndPassword(username,
//                                                                     password);
//                        }
//                        
//                        ProgressReportingDialog::runEvent(&readSpecFileEvent,
//                                                          this,
//                                                          specFile.getFileNameNoPath());
//                        //EventManager::get()->sendEvent(readSpecFileEvent.getPointer());
//                        
//                        if (readSpecFileEvent.isError()) {
//                            if (errorMessages.isEmpty() == false) {
//                                errorMessages += "\n";
//                            }
//                            errorMessages += readSpecFileEvent.getErrorMessage();
//                        }
//                        specFileTimeEnd = timer.getElapsedTimeSeconds();
//                    }
//                        break;
//                    case LOAD_SPEC_FILE_WITH_DIALOG:
//                    case LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE:
//                    {
//                        /*
//                         * Remove wait cursor
//                         */
//                        cursor.restoreCursor();
//                        
//                        /*
//                         * Allow user to choose files listed in the spec file
//                         */
//                        SpecFileDialog* sfd = SpecFileDialog::createForLoadingSpecFile(&specFile,
//                                                                                       this);
//                        if (sfd->exec() == QDialog::Accepted) {
//                            /*
//                             * Redisplay wait cursor
//                             */
//                            cursor.showWaitCursor();
//                            
//                            timer.reset();
//                            specFileTimeStart = timer.getElapsedTimeSeconds();
//                            
//                            EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
//                                                                         &specFile);
//                            if (username.isEmpty() == false) {
//                                readSpecFileEvent.setUsernameAndPassword(username,
//                                                                         password);
//                            }
//                            
//                            ProgressReportingDialog::runEvent(&readSpecFileEvent,
//                                                              this,
//                                                              specFile.getFileNameNoPath());
//                            //EventManager::get()->sendEvent(readSpecFileEvent.getPointer());
//                            
//                            if (readSpecFileEvent.isError()) {
//                                if (errorMessages.isEmpty() == false) {
//                                    errorMessages += "\n";
//                                }
//                                errorMessages += readSpecFileEvent.getErrorMessage();
//                            }
//                            specFileTimeEnd = timer.getElapsedTimeSeconds();
//                            
//                            createDefaultTabsFlag = true;
//                        }
//                        else {
//                            /*
//                             * Redisplay wait cursor
//                             */
//                            cursor.showWaitCursor();
//                        }
//                        
//                        delete sfd;
//                    }
//                        break;
//                }
//                
//                sceneFileWasLoaded = specFile.areAllSelectedFilesSceneFiles();
//            }
//            else {
//                bool addDataFileToSpecFile = false;
//                switch (addDataFileToSpecFileMode) {
//                    case ADD_DATA_FILE_TO_SPEC_FILE_NO:
//                        addDataFileToSpecFile = false;
//                        break;
//                    case ADD_DATA_FILE_TO_SPEC_FILE_YES:
//                        addDataFileToSpecFile = true;
//                        break;
//                }
//                
//                EventDataFileRead loadFileEvent(GuiManager::get()->getBrain(),
//                                                addDataFileToSpecFile);
//                loadFileEvent.addDataFile(fileType,
//                                          name);
//                if (username.isEmpty() == false) {
//                    loadFileEvent.setUsernameAndPassword(username,
//                                                         password);
//                }
//                
//                EventManager::get()->sendEvent(loadFileEvent.getPointer());
//                
//                if (fileType == DataFileTypeEnum::SCENE) {
//                    sceneFileWasLoaded = true;
//                }
//                if (loadFileEvent.isError()) {
//                    AString loadErrorMessage = "";
//                    
//                    if (loadFileEvent.isFileErrorInvalidStructure(0)) {
//                        /*
//                         * Remove wait cursor
//                         */
//                        cursor.restoreCursor();
//                        
//                        WuQDataEntryDialog ded("Structure",
//                                               this);
//                        StructureEnumComboBox* ssc = ded.addStructureEnumComboBox("");
//                        ded.setTextAtTop(("File \""
//                                          + FileInformation(name).getFileName()
//                                          + "\"\nhas missing or invalid structure, select it's structure."
//                                          "\nAfter loading, save file with File Menu->Save Manage Files"
//                                          "\nto prevent this error."),
//                                         false);
//                        if (ded.exec() == WuQDataEntryDialog::Accepted) {
//                            /*
//                             * Redisplay wait cursor
//                             */
//                            cursor.showWaitCursor();
//
//                            EventDataFileRead loadFileEventStructure(GuiManager::get()->getBrain(),
//                                                                     addDataFileToSpecFile);
//                            loadFileEventStructure.addDataFile(ssc->getSelectedStructure(),
//                                                               fileType,
//                                                               name);
//                            if (username.isEmpty() == false) {
//                                loadFileEventStructure.setUsernameAndPassword(username,
//                                                                              password);
//                            }
//                            
//                            EventManager::get()->sendEvent(loadFileEventStructure.getPointer());
//                            if (loadFileEventStructure.isError()) {
//                                loadErrorMessage = loadFileEventStructure.getErrorMessage();
//                            }
//                        }
//                        else {
//                            /*
//                             * Redisplay wait cursor
//                             */
//                            cursor.showWaitCursor();
//                        }
//                    }
//                    else {
//                        loadErrorMessage = loadFileEvent.getErrorMessage();
//                    }
//                    if (loadErrorMessage.isEmpty() == false) {
//                        if (errorMessages.isEmpty() == false) {
//                            errorMessages += "\n";
//                        }
//                        errorMessages += loadErrorMessage;
//                    }
//                }                    
//            }
//        }
//        else {
//            if (errorMessages.isEmpty() == false) {
//                errorMessages += "\n";
//            }
//            errorMessages += ("Extension for " + name + " does not match a suppported file type");
//        }
//    }
    
    const float specFileTime = specFileTimeEnd - specFileTimeStart;
    
    const float createTabsStartTime = timer.getElapsedTimeSeconds();
    const EventBrowserWindowCreateTabs::Mode tabMode = (createDefaultTabsFlag ?
                                                  EventBrowserWindowCreateTabs::MODE_LOADED_SPEC_FILE :
                                                  EventBrowserWindowCreateTabs::MODE_LOADED_DATA_FILE);
    EventBrowserWindowCreateTabs createTabsEvent(tabMode);
    EventManager::get()->sendEvent(createTabsEvent.getPointer());
    
//    if (createDefaultTabsFlag) {
//        m_toolbar->addDefaultTabsAfterLoadingSpecFile();
//    }
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
    
    if (errorMessages.isEmpty() == false) {
        QMessageBox::critical(this, 
                              "ERROR", 
                              errorMessages);
    }
    
    if (sceneFileWasLoaded) {
        GuiManager::get()->processShowSceneDialog(this);
    }
}

/**
 * Called when open data file from spec file is selected.
 */
void 
BrainBrowserWindow::processDataFileOpenFromSpecFile()
{
    const bool valid = GuiManager::get()->getBrain()->isSpecFileValid();
    if (valid) {
        try {
            SpecFile sf;
            sf.readFile(GuiManager::get()->getBrain()->getSpecFileName());
            SpecFileDialog::displayFastOpenDataFile(&sf,
                                                    this);
        }
        catch (const DataFileException& e) {
            QMessageBox::critical(this, 
                                  "ERROR", 
                                  e.whatString());
        }
    }
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
    
    BrainBrowserWindowScreenModeEnum::Enum previousScreenMode = m_screenMode;
    
    m_screenMode = BrainBrowserWindowScreenModeEnum::NORMAL;
    if (action == m_viewScreenNormalAction) {
        m_screenMode = BrainBrowserWindowScreenModeEnum::NORMAL;
    }
    else if (action == m_viewScreenFullAction) {
        m_screenMode = BrainBrowserWindowScreenModeEnum::FULL_SCREEN;
        showInFullScreen = true;
    }
    else if (action == m_viewScreenMontageTabsAction) {
        m_screenMode = BrainBrowserWindowScreenModeEnum::TAB_MONTAGE;
    }
    else if (action == m_viewScreenFullMontageTabsAction) {
        m_screenMode = BrainBrowserWindowScreenModeEnum::TAB_MONTAGE_FULL_SCREEN;
        showInFullScreen = true;
    }
    else {
        CaretAssert(0);
    }
    
    if (m_screenMode == previousScreenMode) {
        return;
    }
    
    if (previousScreenMode == BrainBrowserWindowScreenModeEnum::NORMAL) {
        if (m_screenMode != BrainBrowserWindowScreenModeEnum::NORMAL) {
        saveWindowComponentStatus(m_normalWindowComponentStatus);
        }
    }
    
    if (showInFullScreen) {
        if (isFullScreen() == false) {
            showFullScreen();
        }
    }
    else {
        if (isFullScreen()) {
            showNormal();
        }
    }    
    
    if (m_screenMode == BrainBrowserWindowScreenModeEnum::NORMAL) {
        restoreWindowComponentStatus(m_normalWindowComponentStatus);
    }
    
    m_toolbar->updateToolBar();
    if (m_screenMode != BrainBrowserWindowScreenModeEnum::NORMAL) {
        m_overlayToolBoxAction->blockSignals(true);
        m_overlayToolBoxAction->setChecked(true);
        m_overlayToolBoxAction->blockSignals(false);
        m_overlayToolBoxAction->trigger();

        m_featuresToolBoxAction->blockSignals(true);
        m_featuresToolBoxAction->setChecked(true);
        m_featuresToolBoxAction->blockSignals(false);
        m_featuresToolBoxAction->trigger();
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Restore the status of window components.
 * @param wcs
 *    Window component status that is restored.
 */
void 
BrainBrowserWindow::restoreWindowComponentStatus(const WindowComponentStatus& wcs)
{
    m_showToolBarAction->setEnabled(true);
    if (wcs.isToolBarDisplayed) {
        m_showToolBarAction->setChecked(false);
        m_showToolBarAction->trigger();
    }
    else {
        m_showToolBarAction->setChecked(true);
        m_showToolBarAction->trigger();
    }
    
    m_overlayToolBoxAction->setEnabled(true);
    if (wcs.isOverlayToolBoxDisplayed) {
        m_overlayToolBoxAction->blockSignals(true);
        m_overlayToolBoxAction->setChecked(false);
        m_overlayToolBoxAction->blockSignals(false);
        m_overlayToolBoxAction->trigger();
    }
    else {
        m_overlayToolBoxAction->blockSignals(true);
        m_overlayToolBoxAction->setChecked(true);
        m_overlayToolBoxAction->blockSignals(false);
        m_overlayToolBoxAction->trigger();
    }
    
    m_featuresToolBoxAction->setEnabled(true);
    if (wcs.isFeaturesToolBoxDisplayed) {
        m_featuresToolBoxAction->blockSignals(true);
        m_featuresToolBoxAction->setChecked(false);
        m_featuresToolBoxAction->blockSignals(false);
        m_featuresToolBoxAction->trigger();
    }
    else {
        m_featuresToolBoxAction->blockSignals(true);
        m_featuresToolBoxAction->setChecked(true);
        m_featuresToolBoxAction->blockSignals(false);
        m_featuresToolBoxAction->trigger();
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
    wcs.isToolBarDisplayed = m_showToolBarAction->isChecked();
    wcs.isOverlayToolBoxDisplayed = m_overlayToolBoxAction->isChecked();
    wcs.isFeaturesToolBoxDisplayed  = m_featuresToolBoxAction->isChecked();
    m_showToolBarAction->setEnabled(false);
    m_overlayToolBoxAction->setEnabled(false);
    m_featuresToolBoxAction->setEnabled(false);
}

/**
 * Adds a new tab to the window.
 */
void 
BrainBrowserWindow::processNewTab()
{
    BrowserTabContent* previousTabContent = getBrowserTabContent();
    m_toolbar->addNewTabCloneContent(previousTabContent);
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
        m_toolbar->addNewTab(otherTabContent[i]);
        m_toolbar->updateToolBar();
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Called when the move tab to window is about to be displayed.
 */
void 
BrainBrowserWindow::processMoveSelectedTabToWindowMenuAboutToBeDisplayed()
{
    m_moveSelectedTabToWindowMenu->clear();

    if (getBrowserTabContent() == NULL) {
        return;
    }
    
    /*
     * Allow movement of tab to new window ONLY if this window
     * contains more than one tab.
     */
    if (m_toolbar->tabBar->count() > 1) {
        QAction* toNewWindowAction = new QAction("New Window",
                                                 m_moveSelectedTabToWindowMenu);
        toNewWindowAction->setData(qVariantFromValue((void*)NULL));
        m_moveSelectedTabToWindowMenu->addAction(toNewWindowAction);
    }
    
    std::vector<BrainBrowserWindow*> browserWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    for (int32_t i = 0; i < static_cast<int32_t>(browserWindows.size()); i++) {
        if (browserWindows[i] != this) {
            QAction* action = new QAction(browserWindows[i]->windowTitle(),
                                          m_moveSelectedTabToWindowMenu);
            action->setData(qVariantFromValue((void*)browserWindows[i]));
            m_moveSelectedTabToWindowMenu->addAction(action);
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
        
        BrowserTabContent* btc = getBrowserTabContent();
        
            
        if (moveToBrowserWindow != NULL) {
            m_toolbar->removeTabWithContent(btc);
            moveToBrowserWindow->m_toolbar->addNewTab(btc);
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
            m_toolbar->removeTabWithContent(btc);
        }
        
        if (m_toolbar->tabBar->count() <= 0) {
            close();
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
    moveOverlayToolBox(Qt::LeftDockWidgetArea);
}

/**
 * Called to move the overlay toolbox to the bottom side of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToBottom()
{
    moveOverlayToolBox(Qt::BottomDockWidgetArea);
}

/**
 * Called to move the overlay toolbox to float outside of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToFloat()
{
moveOverlayToolBox(Qt::NoDockWidgetArea);
}

/**
 * Called to hide the overlay toolbox.
 */
void 
BrainBrowserWindow::processHideOverlayToolBox()
{
    processShowOverlayToolBox(false);
}

/**
 * Called to move the layers toolbox to the right side of the window.
 */
void 
BrainBrowserWindow::processMoveFeaturesToolBoxToRight()
{
    moveFeaturesToolBox(Qt::RightDockWidgetArea);
}

/**
 * Called to move the layers toolbox to float outside of the window.
 */
void 
BrainBrowserWindow::processMoveFeaturesToolBoxToFloat()
{
    moveFeaturesToolBox(Qt::NoDockWidgetArea);
}

/**
 * Called to hide the layers tool box.
 */
void 
BrainBrowserWindow::processHideFeaturesToolBox()
{
    if (m_featuresToolBoxAction->isChecked()) {
        m_featuresToolBoxAction->trigger();
    }
}

/**
 * Called to display the layers toolbox.
 */
void 
BrainBrowserWindow::processShowFeaturesToolBox(bool status)
{
    if (status) {
        m_featuresToolBoxAction->setToolTip("Hide Features Toolbox");
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBox().getPointer());
    }
    else {
        m_featuresToolBoxAction->setToolTip("Show Features Toolbox");
    }
//    if (status) {
//        AString title = m_featuresToolBoxTitle;
//        if (m_featuresToolBox->isFloating()) {
//            title += (" "
//                      + AString::number(m_browserWindowIndex + 1));
//        }
//        m_featuresToolBox->setWindowTitle(title);
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
            m_featuresToolBox->setFloating(false);
            addDockWidget(Qt::RightDockWidgetArea, 
                                m_featuresToolBox);
            if (m_featuresToolBoxAction->isChecked() == false) {
                m_featuresToolBoxAction->trigger();
            }
            break;
        case Qt::TopDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on top");
            break;
        case Qt::BottomDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on bottom");
            break;
        default:
            m_featuresToolBox->setFloating(true);
            if (m_featuresToolBoxAction->isChecked() == false) {
                m_featuresToolBoxAction->trigger();
            }
            break;
    }
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
            m_overlayHorizontalToolBox->setVisible(false);
            m_overlayVerticalToolBox->setFloating(false);
            addDockWidget(Qt::LeftDockWidgetArea, 
                                m_overlayVerticalToolBox);
            m_overlayVerticalToolBox->setVisible(true);
            m_overlayActiveToolBox = m_overlayVerticalToolBox;
            isVisible = true;
            break;
        case Qt::RightDockWidgetArea:
            CaretAssertMessage(0, "Overlay toolbox not allowed on right");
            break;
        case Qt::TopDockWidgetArea:
            CaretAssertMessage(0, "Overlay toolbox not allowed on top");
            break;
        case Qt::BottomDockWidgetArea:
            m_overlayVerticalToolBox->setVisible(false);
            m_overlayHorizontalToolBox->setFloating(false);
            addDockWidget(Qt::BottomDockWidgetArea, 
                                m_overlayHorizontalToolBox);
            m_overlayHorizontalToolBox->setVisible(true);
            m_overlayActiveToolBox = m_overlayHorizontalToolBox;
            isVisible = true;
            break;
        default:
            m_overlayActiveToolBox->setVisible(true);
            m_overlayActiveToolBox->setFloating(true);
            isVisible = true;
            break;
    }

    processShowOverlayToolBox(isVisible);
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
    m_toolbar->removeAndReturnAllTabs(allTabContent);
}

/**
 * @return Return the active browser tab content in
 * this browser window.
 */
BrowserTabContent* 
BrainBrowserWindow::getBrowserTabContent()
{
    return m_toolbar->getTabContentFromSelectedTab();
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
    QImage image = m_openGLWidget->captureImage(imageSizeX, 
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
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);

    /*
     * Save toolbar
     */
    sceneClass->addClass(m_toolbar->saveToScene(sceneAttributes, 
                                                "m_toolbar"));

    /*
     * Save overlay toolbox
     */
    {
        AString orientationName = "";
        if (m_overlayActiveToolBox == m_overlayHorizontalToolBox) {
            orientationName = "horizontal";
        }
        else if (m_overlayActiveToolBox == m_overlayVerticalToolBox) {
            orientationName = "vertical";
        }
        
        SceneClass* overlayToolBoxClass = new SceneClass("overlayToolBox",
                                                         "OverlayToolBox",
                                                         1);
        overlayToolBoxClass->addString("orientation", 
                                       orientationName);
        overlayToolBoxClass->addBoolean("floating", 
                                        m_overlayActiveToolBox->isFloating());
        overlayToolBoxClass->addBoolean("visible", 
                                        m_overlayActiveToolBox->isVisible());
        sceneClass->addClass(overlayToolBoxClass);
        
        sceneClass->addClass(m_overlayActiveToolBox->saveToScene(sceneAttributes,
                                                                 "m_overlayActiveToolBox"));
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
    
    /*
     * Save features toolbox
     */
    {
        SceneClass* featureToolBoxClass = new SceneClass("featureToolBox",
                                                         "FeatureToolBox",
                                                         1);
        featureToolBoxClass->addBoolean("floating",
                                        m_featuresToolBox->isFloating());
        featureToolBoxClass->addBoolean("visible",
                                        m_featuresToolBox->isVisible());
        sceneClass->addClass(featureToolBoxClass);
        sceneClass->addClass(m_featuresToolBox->saveToScene(sceneAttributes,
                                                            "m_featuresToolBox"));
    }
    
    /*
     * Screen mode (normal, full, etc)
     */
    sceneClass->addEnumeratedType<BrainBrowserWindowScreenModeEnum, BrainBrowserWindowScreenModeEnum::Enum>("m_screenMode",
                                                                                                            m_screenMode);
    
    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                         "geometry"));
    
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
    if (sceneClass == NULL) {
        return;
    }
    
    /*
     * Screen mode
     * Note: m_screenMode must be set to a different value than 
     * the value passed to processViewScreenActionGroupSelection().
     */
    m_normalWindowComponentStatus = m_defaultWindowComponentStatus;
    const BrainBrowserWindowScreenModeEnum::Enum newScreenMode = 
    sceneClass->getEnumeratedTypeValue<BrainBrowserWindowScreenModeEnum, BrainBrowserWindowScreenModeEnum::Enum>("m_screenMode", 
                                                                                                                 BrainBrowserWindowScreenModeEnum::NORMAL);
    m_screenMode = BrainBrowserWindowScreenModeEnum::NORMAL;
    switch (newScreenMode) {
        case BrainBrowserWindowScreenModeEnum::NORMAL:
            m_screenMode = BrainBrowserWindowScreenModeEnum::FULL_SCREEN;
            processViewScreenActionGroupSelection(m_viewScreenNormalAction);
            break;
        case BrainBrowserWindowScreenModeEnum::FULL_SCREEN:
            processViewScreenActionGroupSelection(m_viewScreenFullAction);
            break;
        case BrainBrowserWindowScreenModeEnum::TAB_MONTAGE:
            processViewScreenActionGroupSelection(m_viewScreenMontageTabsAction);
            break;
        case BrainBrowserWindowScreenModeEnum::TAB_MONTAGE_FULL_SCREEN:
            processViewScreenActionGroupSelection(m_viewScreenFullMontageTabsAction);
            break;
    }
    
    /*
     * Restore toolbar
     */
    const SceneClass* toolbarClass = sceneClass->getClass("m_toolbar");
    if (toolbarClass != NULL) {
        m_toolbar->restoreFromScene(sceneAttributes,
                                    toolbarClass);
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    
    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    swg.restoreFromScene(sceneAttributes, sceneClass->getClass("geometry"));
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    QApplication::processEvents();
    
    /*
     * Restore feature toolbox
     */
    const SceneClass* featureToolBoxClass = sceneClass->getClass("featureToolBox");
    if (featureToolBoxClass != NULL) {
        const bool toolBoxVisible = featureToolBoxClass->getBooleanValue("visible",
                                                                         true);
        const bool toolBoxFloating = featureToolBoxClass->getBooleanValue("floating",
                                                                          false);
        
        if (toolBoxVisible) {
            if (toolBoxFloating) {
                processMoveFeaturesToolBoxToFloat();
            }
            else {
                processMoveFeaturesToolBoxToRight();
            }
        }
        m_featuresToolBoxAction->blockSignals(true);
        m_featuresToolBoxAction->setChecked(! toolBoxVisible);
        m_featuresToolBoxAction->blockSignals(false);
        m_featuresToolBoxAction->trigger();
        //processShowFeaturesToolBox(toolBoxVisible);
        m_featuresToolBox->restoreFromScene(sceneAttributes,
                                            sceneClass->getClass("m_featuresToolBox"));
    }
    
    /*
     * Restore overlay toolbox
     */
    const SceneClass* overlayToolBoxClass = sceneClass->getClass("overlayToolBox");
    if (overlayToolBoxClass != NULL) {
        const AString orientationName = overlayToolBoxClass->getStringValue("orientation",
                                                                            "horizontal");
        const bool toolBoxVisible = overlayToolBoxClass->getBooleanValue("visible",
                                                                         true);
        const bool toolBoxFloating = overlayToolBoxClass->getBooleanValue("floating",
                                                                          false);
        if (orientationName == "horizontal") {
            processMoveOverlayToolBoxToBottom();
        }
        else {
            processMoveOverlayToolBoxToLeft();
        }
        if (toolBoxFloating) {
            processMoveOverlayToolBoxToFloat();
        }
        processShowOverlayToolBox(toolBoxVisible);
        m_overlayActiveToolBox->restoreFromScene(sceneAttributes,
                                                 sceneClass->getClass("m_overlayActiveToolBox"));
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
}

void
BrainBrowserWindow::getViewportSize(int &w, int &h)
{
    m_openGLWidget->getViewPortSize(w,h);
}



