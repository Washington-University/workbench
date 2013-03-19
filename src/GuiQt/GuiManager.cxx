
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

#include <algorithm>
#include <list>

#include <QAction>
#include <QApplication>
#include <QWebView>

#define __GUI_MANAGER_DEFINE__
#include "GuiManager.h"
#undef __GUI_MANAGER_DEFINE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CursorManager.h"
#include "CustomViewDialog.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserWindowNew.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapSettingsEditorDialogRequest.h"
#include "EventModelGetAll.h"
#include "EventOperatingSystemRequestOpenDataFile.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "FociPropertiesEditorDialog.h"
#include "ImageFile.h"
#include "ImageCaptureDialog.h"
#include "InformationDisplayDialog.h"
#include "ManageLoadedFilesDialog.h"
#include "MapSettingsEditorDialog.h"
#include "MovieDialog.h"
#include "PreferencesDialog.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneDialog.h"
#include "SceneWindowGeometry.h"
#include "SessionManager.h"
#include "SurfacePropertiesEditorDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 * @param parent
 *   Parent of this object.
 */
GuiManager::GuiManager(QObject* parent)
: QObject(parent)
{
    this->nameOfApplication = "Connectome Workbench";
    //this->brainOpenGL = NULL;
    this->allowBrowserWindowsToCloseWithoutConfirmation = false;
    
    m_customViewDialog = NULL;
    this->imageCaptureDialog = NULL;
    this->movieDialog = NULL;
    m_informationDisplayDialog = NULL;
    this->preferencesDialog = NULL;  
    this->connectomeDatabaseWebView = NULL;
    this->sceneDialog = NULL;
    m_surfacePropertiesEditorDialog = NULL;
    
    this->cursorManager = new CursorManager();
    
    /*
     * Information window.
     */
    QIcon infoDisplayIcon;
    const bool infoDisplayIconValid =
    WuQtUtilities::loadIcon(":/toolbar_info_icon.png", 
                            infoDisplayIcon);
    

    m_informationDisplayDialogEnabledAction =
    WuQtUtilities::createAction("Information...",
                                "Enables display of the Information Window\n"
                                "when new information is available",
                                this,
                                this,
                                SLOT(showHideInfoWindowSelected(bool))); 
    if (infoDisplayIconValid) {
        m_informationDisplayDialogEnabledAction->setIcon(infoDisplayIcon);
        m_informationDisplayDialogEnabledAction->setIconVisibleInMenu(false);
    }
    else {
        m_informationDisplayDialogEnabledAction->setIconText("Info");
    }
    
    m_informationDisplayDialogEnabledAction->blockSignals(true);
    m_informationDisplayDialogEnabledAction->setCheckable(true);
    m_informationDisplayDialogEnabledAction->setChecked(true);
    this->showHideInfoWindowSelected(m_informationDisplayDialogEnabledAction->isChecked());
    m_informationDisplayDialogEnabledAction->setIconText("Info"); 
    m_informationDisplayDialogEnabledAction->blockSignals(false);
        
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_NEW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_TIME_COURSE_DIALOG);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_SCALAR_DATA_COLOR_MAPPING_EDITOR_SHOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE);
}

/**
 * Destructor.
 */
GuiManager::~GuiManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete this->cursorManager;
    
    if (this->connectomeDatabaseWebView != NULL) {
        delete this->connectomeDatabaseWebView;
    }
    
    FociPropertiesEditorDialog::deleteStaticMembers();
    
//    if (this->brainOpenGL != NULL) {
//        delete this->brainOpenGL;
//        this->brainOpenGL = NULL;
//    }
    
}

/**
 * Get the GUI Manager.
 */
GuiManager* 
GuiManager::get()
{
    if (GuiManager::singletonGuiManager == NULL) {
        GuiManager::singletonGuiManager = new GuiManager();
        WuQtUtilities::sendListOfResourcesToCaretLogger();
    }
    return GuiManager::singletonGuiManager;
}

/*
 * Create the singleton GUI Manager.
 */
void 
GuiManager::createGuiManager()
{
    CaretAssertMessage((GuiManager::singletonGuiManager == NULL),
                       "GUI manager has already been created.");
    
    GuiManager::singletonGuiManager = new GuiManager();
}

/*
 * Delete the singleton GUI Manager.
 */
void 
GuiManager::deleteGuiManager()
{
    CaretAssertMessage((GuiManager::singletonGuiManager != NULL), 
                       "GUI manager does not exist, cannot delete it.");
    
    delete GuiManager::singletonGuiManager;
    GuiManager::singletonGuiManager = NULL;
}

/**
 * Beep to alert the user.
 */
void 
GuiManager::beep(const int32_t numTimesToBeep)
{
    for (int32_t i = 0; i < numTimesToBeep; i++) {
        SystemUtilities::sleepSeconds(0.25);
        QApplication::beep();
    }
}

/**
 * @return The brain.
 */
Brain* 
GuiManager::getBrain()
{
    return SessionManager::get()->getBrain(0);
}

/**
 * Get the Brain OpenGL for drawing with OpenGL.
 *
 * @return 
 *    Point to the brain.
 */
//BrainOpenGL* 
//GuiManager::getBrainOpenGL()
//{
//    if (this->brainOpenGL == NULL) {
//        this->brainOpenGL = BrainOpenGL::getBrainOpenGL();
//    }
//    
//    return this->brainOpenGL;
//}

/**
 * See if a brain browser window can be closed.  If there is only
 * one brain browser window, the user will be warned that any 
 * changes to files will be lost and the application will exit.
 * If there is more than one brain browser window open and the 
 * window being closed contains more than one tab, the user will
 * be warned.
 *
 * @param brainBrowserWindow
 *   Brain browser window that will be closed.
 * @param numberOfOpenTabs
 *   Number of tabs in window.
 * @return 
 *   True if window should be closed, else false.
 */
bool 
GuiManager::allowBrainBrowserWindowToClose(BrainBrowserWindow* brainBrowserWindow,
                                           const int32_t numberOfOpenTabs)
{
    bool isBrowserWindowAllowedToClose = false;

    if (this->allowBrowserWindowsToCloseWithoutConfirmation) {
        isBrowserWindowAllowedToClose = true;
    }
    else {
        if (this->getNumberOfOpenBrainBrowserWindows() > 1) {
            /*
             * Warn if multiple tabs in window
             */
            if (numberOfOpenTabs > 1) {
                QString tabMessage = QString::number(numberOfOpenTabs) + " tabs are open.";
                isBrowserWindowAllowedToClose =
                WuQMessageBox::warningCloseCancel(brainBrowserWindow, 
                                                  "Are you sure you want to close this window?", 
                                                  tabMessage);
            }
            else {
                isBrowserWindowAllowedToClose = true;
            }
        }
        else {
            isBrowserWindowAllowedToClose = this->exitProgram(brainBrowserWindow);
        }
    }
    
    if (isBrowserWindowAllowedToClose) {
        for (int32_t i = 0; i < static_cast<int32_t>(m_brainBrowserWindows.size()); i++) {
            if (m_brainBrowserWindows[i] == brainBrowserWindow) {
                this->reparentNonModalDialogs(m_brainBrowserWindows[i]);
                m_brainBrowserWindows[i] = NULL;
            }
        }
    }
    
    return isBrowserWindowAllowedToClose;
}

/**
 * Get the number of brain browser windows.
 *
 * @return Number of brain browser windows that are valid.
 */
int32_t 
GuiManager::getNumberOfOpenBrainBrowserWindows() const
{
    int32_t numberOfWindows = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(m_brainBrowserWindows.size()); i++) {
        if (m_brainBrowserWindows[i] != NULL) {
            numberOfWindows++;
        }
    }
    return numberOfWindows;
}


/**
 * Get all of the brain browser windows.
 *
 * @return 
 *   Vector containing all open brain browser windows.
 */
std::vector<BrainBrowserWindow*> 
GuiManager::getAllOpenBrainBrowserWindows() const
{ 
    std::vector<BrainBrowserWindow*> windows;
    
    int32_t numWindows = static_cast<int32_t>(m_brainBrowserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        if (m_brainBrowserWindows[i] != NULL) {
            windows.push_back(m_brainBrowserWindows[i]);
        }
    }
    
    return windows; 
}

/**
 * @return Return the active browser window.  If no browser window is active,
 * the browser window with the lowest index is returned.  If no browser
 * window is open (which likely should never occur), NULL is returned.
 *
 * To verify that the returned window was the active window, call its
 * "isActiveWindow()" method.
 */
BrainBrowserWindow* 
GuiManager::getActiveBrowserWindow() const
{
    BrainBrowserWindow* firstWindowFound = NULL;
    int32_t numWindows = static_cast<int32_t>(m_brainBrowserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        BrainBrowserWindow* bbw = m_brainBrowserWindows[i];
        if (bbw != NULL) {
            if (firstWindowFound == NULL) {
                firstWindowFound = bbw;
            }
            if (bbw->isActiveWindow()) {
                return bbw;
            }
        }
    }    
    return firstWindowFound;
}


/**
 * Get the brain browser window with the given window index.
 * Note that as browser windows are opened or closed, a window's
 * index NEVER changes.  Thus, a NULL value may be returned for 
 * a window index referring to a window that was closed.
 *
 * @param browserWindowIndex
 *    Index of the window.
 * @return
 *    Pointer to window at given index or NULL in cases where
 *    the window was closed.
 */
BrainBrowserWindow* 
GuiManager::getBrowserWindowByWindowIndex(const int32_t browserWindowIndex)
{
    if (browserWindowIndex < static_cast<int32_t>(m_brainBrowserWindows.size())) {
        return m_brainBrowserWindows[browserWindowIndex];
    }
    return NULL;
}

/**
 * Create a new BrainBrowser Window.
 * @param parent
 *    Optional parent that is used only for window placement.
 * @param browserTabContent
 *    Optional tab for initial windwo tab.
 * @param createDefaultTabs
 *    If true, create the default tabs in the new window.
 */
BrainBrowserWindow*
GuiManager::newBrainBrowserWindow(QWidget* parent,
                                  BrowserTabContent* browserTabContent,
                                  const bool createDefaultTabs)
{
    /*
     * If no tabs can be created, do not create a new window.
     */
    EventBrowserTabGetAll getAllTabs;
    EventManager::get()->sendEvent(getAllTabs.getPointer());
    if (getAllTabs.getNumberOfBrowserTabs() == BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS) {
        return NULL;
    }
    
    int32_t windowIndex = -1;
    
    int32_t numWindows = static_cast<int32_t>(m_brainBrowserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        if (m_brainBrowserWindows[i] == NULL) {
            windowIndex = i;
            break;
        }
    }
    
    BrainBrowserWindow* bbw = NULL; 
    
    BrainBrowserWindow::CreateDefaultTabsMode tabsMode = (createDefaultTabs
                                                          ? BrainBrowserWindow::CREATE_DEFAULT_TABS_YES
                                                          : BrainBrowserWindow::CREATE_DEFAULT_TABS_NO);
    
    if (windowIndex < 0) {
        windowIndex = m_brainBrowserWindows.size();
        bbw = new BrainBrowserWindow(windowIndex, browserTabContent, tabsMode);
        m_brainBrowserWindows.push_back(bbw);
    }
    else {
        bbw = new BrainBrowserWindow(windowIndex, browserTabContent, tabsMode);
        m_brainBrowserWindows[windowIndex] = bbw;
    }
    
    if (parent != NULL) {
        WuQtUtilities::moveWindowToOffset(parent, bbw, 20, 20);
    }
    
    bbw->show();
    
    return bbw;
}

/**
 * Exit the program.
 * @param
 *    Parent over which dialogs are displayed for saving/verifying.
 * return 
 *    true if application should exit, else false.
 */
bool 
GuiManager::exitProgram(QWidget* parent)
{
    bool okToExit = false;
    
    /*
     * Are files modified?
     */
    const bool areFilesModified = this->getBrain()->areFilesModified(true,
                                                               false);
//    std::vector<CaretDataFile*> dataFiles;
//    this->getBrain()->getAllDataFiles(dataFiles);
//    for (std::vector<CaretDataFile*>::iterator iter = dataFiles.begin();
//         iter != dataFiles.end();
//         iter++) {
//        CaretDataFile* cdf = *iter;
//
//        /**
//         * Do not check connectivity files for modified status
//         */ 
//        bool checkIfModified = true;
//        switch (cdf->getDataFileType()) {
//            case DataFileTypeEnum::CONNECTIVITY_DENSE:
//            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
//                checkIfModified = false;
//                break;
//            default:
//                break;
//        }
//        
//        if (checkIfModified) {
//            if (cdf->isModified()) {
//                areFilesModified = true;
//                break;
//            }
//        }
//    }
         
    if (areFilesModified) {
        WuQMessageBox::StandardButton buttonPressed = 
        WuQMessageBox::saveDiscardCancel(parent, 
                                         "Files are modified.", 
                                         "Do you want to save changes?");
        
        switch (buttonPressed) {
            case QMessageBox::Save:
            {
                ManageLoadedFilesDialog manageLoadedFile(parent,
                                                         this->getBrain(),
                                                         true);
                if (manageLoadedFile.exec() == ManageLoadedFilesDialog::Accepted) {
                    okToExit = true;
                }
            }
                break;
            case QMessageBox::Discard:
                okToExit = true;
                break;
            case QMessageBox::Cancel:
                break;
            default:
                CaretAssert(0);
                break;
        }
        
    }
    else {
        okToExit =
        WuQMessageBox::warningOkCancel(parent,
                                       "<html>Closing this window will<br>exit the application.</html>");
    }
    
    if (okToExit) {
        std::vector<BrainBrowserWindow*> bws = this->getAllOpenBrainBrowserWindows();
        for (int i = 0; i < static_cast<int>(bws.size()); i++) {
            bws[i]->deleteLater();
        }
        
        QCoreApplication::instance()->quit();
    }    
    
    return okToExit;
}

/**
 * Get the model displayed in the given browser window.
 * @param browserWindowIndex
 *    Index of browser window.
 * @return
 *    Model in the browser window.  May be NULL if no data loaded.
 */
Model*
GuiManager::getModelInBrowserWindow(const int32_t browserWindowIndex)
{
    BrowserTabContent* browserTabContent = getBrowserTabContentForBrowserWindow(browserWindowIndex,
                                                                                true);
    Model* model = NULL;
    if (browserTabContent != NULL) {
        model = browserTabContent->getModelControllerForDisplay();
    }
    return model;
}


/**
 * Get the browser tab content in a browser window.
 * @param browserWindowIndex
 *    Index of browser window.
 * @param allowInvalidBrowserWindowIndex
 *    In some instance, such as GUI construction or destruction, the window is not
 *    fully created or deleted, thus "this->brainBrowserWindows" is invalid for
 *    the given index.  If this parameter is true, NULL will be 
 *    returned in this case.
 * @return
 *    Browser tab content in the browser window.  Value may be NULL
 *    is allowInvalidBrowserWindowIndex is true
 */
BrowserTabContent* 
GuiManager::getBrowserTabContentForBrowserWindow(const int32_t browserWindowIndex,
                                                 const bool allowInvalidBrowserWindowIndex)
{
    if (allowInvalidBrowserWindowIndex) {
        if (browserWindowIndex >= static_cast<int32_t>(m_brainBrowserWindows.size())) {
            return NULL;
        }
    }
    
    CaretAssertVectorIndex(m_brainBrowserWindows, browserWindowIndex);
    BrainBrowserWindow* browserWindow = m_brainBrowserWindows[browserWindowIndex];
    if (allowInvalidBrowserWindowIndex) {
        if (browserWindow == NULL) {
            return NULL;
        }
    }
    CaretAssert(browserWindow);
    
    BrowserTabContent* tabContent = browserWindow->getBrowserTabContent();
    //CaretAssert(tabContent);
    return tabContent;
}

/**
 * Called when bring all windows to front is selected.
 */
void 
GuiManager::processBringAllWindowsToFront()
{
    for (int32_t i = 0; i < static_cast<int32_t>(m_brainBrowserWindows.size()); i++) {
        if (m_brainBrowserWindows[i] != NULL) {
            m_brainBrowserWindows[i]->show();
            m_brainBrowserWindows[i]->activateWindow();
        }
    }
    
    for (int32_t i = 0; i < static_cast<int32_t>(nonModalDialogs.size()); i++) {
        if (nonModalDialogs[i] != NULL) {
            if (nonModalDialogs[i]->isVisible()) {
                nonModalDialogs[i]->raise();
            }
        }
    }
}

/**
 * Called when show help online is selected.
 */ 
void 
GuiManager::processShowHelpOnlineWindow()
{
    
}

/**
 * Called when search help online is selected.
 */
void 
GuiManager::processShowSearchHelpOnlineWindow()
{
    
}

/**
 * @return Name of the application.
 */
QString 
GuiManager::applicationName() const
{
    return this->nameOfApplication;
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
GuiManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_NEW) {
        EventBrowserWindowNew* eventNewBrowser =
            dynamic_cast<EventBrowserWindowNew*>(event);
        CaretAssert(eventNewBrowser);
        
        BrainBrowserWindow* bbw = this->newBrainBrowserWindow(eventNewBrowser->getParent(), 
                                                              eventNewBrowser->getBrowserTabContent(),
                                                              true);
        if (bbw == NULL) {
            eventNewBrowser->setErrorMessage("Workench is exhausted.  It cannot create any more windows.");
            eventNewBrowser->setEventProcessed();
            return;
        }
        
        eventNewBrowser->setBrowserWindowCreated(bbw);
        eventNewBrowser->setEventProcessed();
        
        /*
         * Initialize the size of the window
         */
        const int w = bbw->width();
        const int preferredMaxHeight = (WuQtUtilities::isSmallDisplay()
                                        ? 550
                                        : 850);
        const int h = std::min(bbw->height(), 
                               preferredMaxHeight);
        bbw->resize(w, h);
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS) {
        EventUpdateInformationWindows* infoEvent =
        dynamic_cast<EventUpdateInformationWindows*>(event);
        CaretAssert(infoEvent);
        
        bool showInfoDialog = infoEvent->isImportant();
        
        if (showInfoDialog) {
            this->processShowInformationDisplayDialog(false);
        }
        
        infoEvent->setEventProcessed();
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_UPDATE_TIME_COURSE_DIALOG) {
        this->processUpdateTimeCourseDialogs();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_SCALAR_DATA_COLOR_MAPPING_EDITOR_SHOW) {
        EventMapSettingsEditorDialogRequest* mapEditEvent =
        dynamic_cast<EventMapSettingsEditorDialogRequest*>(event);
        CaretAssert(mapEditEvent);
        
        const int browserWindowIndex = mapEditEvent->getBrowserWindowIndex();
        CaretAssertVectorIndex(m_brainBrowserWindows, browserWindowIndex);
        BrainBrowserWindow* browserWindow = m_brainBrowserWindows[browserWindowIndex];
        CaretAssert(browserWindow);
        
//        CaretMappableDataFile* mapFile = mapEditEvent->getCaretMappableDataFile();
//        const int mapIndex = mapEditEvent->getMapIndex();
        Overlay* overlay = mapEditEvent->getOverlay();
        
        MapSettingsEditorDialog* mapEditor = NULL;
        for (std::set<MapSettingsEditorDialog*>::iterator mapEditorIter = m_mappingSettingsEditors.begin();
             mapEditorIter != m_mappingSettingsEditors.end();
             mapEditorIter++) {
            MapSettingsEditorDialog* med = *mapEditorIter;
            if (med->isDoNotReplaceSelected() == false) {
                mapEditor = med;
                break;
            }
        }
        
        bool placeInDefaultLocation = false;
        if (mapEditor == NULL) {
            mapEditor = new MapSettingsEditorDialog(browserWindow);
            m_mappingSettingsEditors.insert(mapEditor);
            this->nonModalDialogs.push_back(mapEditor);
            placeInDefaultLocation = true;
        }
        else {
            if (mapEditor->isHidden()) {
                placeInDefaultLocation = true;
            }
        }
        
        mapEditor->updateDialogContent(overlay);
        mapEditor->show();
        mapEditor->raise();
        mapEditor->activateWindow();
        if (placeInDefaultLocation) {
            WuQtUtilities::moveWindowToSideOfParent(browserWindow,
                                                    mapEditor);
        }
        mapEditEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_OPERATING_SYSTEM_REQUEST_OPEN_DATA_FILE) {
        EventOperatingSystemRequestOpenDataFile* openFileEvent =
        dynamic_cast<EventOperatingSystemRequestOpenDataFile*>(event);
        CaretAssert(openFileEvent);
        
        BrainBrowserWindow* bbw = getActiveBrowserWindow();
        if (bbw != NULL) {
            std::vector<AString> filenamesVector;
            std::vector<DataFileTypeEnum::Enum> dataFileTypeVectorNotUsed;
            filenamesVector.push_back(openFileEvent->getDataFileName());
            bbw->loadFiles(bbw,
                           filenamesVector,
                           dataFileTypeVectorNotUsed,
                           BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG,
                           BrainBrowserWindow::ADD_DATA_FILE_TO_SPEC_FILE_NO,
                           "",
                           "");
        }
        else {
            /*
             * Browser window has not yet been created.
             * After it is created, the file will be opened.
             */
            m_nameOfDataFileToOpenAfterStartup = openFileEvent->getDataFileName();
            //CaretLogSevere("No browser window open for loading file from operating system.");
            //CaretAssert(0);
        }
    }
}

/**
 * Remove the tab content from all browser windows except for the given
 * browser windows, close the other browser windows, and then return
 * the tab content.
 *
 * @param browserWindow
 *    Browser window that gets tab content from all other windows.
 * @param tabContents
 *    Tab content from all other windows.
 */
void 
GuiManager::closeOtherWindowsAndReturnTheirTabContent(BrainBrowserWindow* browserWindow,
                                                      std::vector<BrowserTabContent*>& tabContents)
{
    tabContents.clear();
    
    const int32_t numWindows = m_brainBrowserWindows.size();
    for (int32_t i = 0; i < numWindows; i++) {
        BrainBrowserWindow* bbw = m_brainBrowserWindows[i];
        if (bbw != NULL) {
            if (bbw != browserWindow) {
                std::vector<BrowserTabContent*> tabs;
                bbw->removeAndReturnAllTabs(tabs);
                tabContents.insert(tabContents.end(), 
                                   tabs.begin(), 
                                   tabs.end());
                this->allowBrowserWindowsToCloseWithoutConfirmation = true;
                bbw->close();
                
                /*
                 * Should delete the windows that were closed!
                 * When a window is closed, Qt uses 'deleteLater'
                 * but we need them deleted now so that event listeners
                 * are shut down since the closed windows no longer
                 * have any content.
                 */
                QCoreApplication::sendPostedEvents(0,  QEvent::DeferredDelete);
                
                this->allowBrowserWindowsToCloseWithoutConfirmation = false;
            }
        }
    }
    
}

/**
 * Close all but the given window.
 * @param browserWindow
 *    Window that is NOT closed.
 */
void 
GuiManager::closeAllOtherWindows(BrainBrowserWindow* browserWindow)
{
    const int32_t numWindows = m_brainBrowserWindows.size();
    for (int32_t i = 0; i < numWindows; i++) {
        BrainBrowserWindow* bbw = m_brainBrowserWindows[i];
        if (bbw != NULL) {
            if (bbw != browserWindow) {
                this->allowBrowserWindowsToCloseWithoutConfirmation = true;
                bbw->close();
                
                /*
                 * Should delete the windows that were closed!
                 * When a window is closed, Qt uses 'deleteLater'
                 * but we need them deleted now so that event listeners
                 * are shut down since the closed windows no longer
                 * have any content.
                 */
                QCoreApplication::sendPostedEvents(0,  QEvent::DeferredDelete);
                
                this->allowBrowserWindowsToCloseWithoutConfirmation = false;
            }
        }
    }
}

/** 
 * Reparent non-modal dialogs that may need to be reparented if the 
 * original parent, a BrainBrowserWindow is closed in which case the
 * dialog is reparented to a different BrainBrowserWindow.
 *
 * @param closingBrainBrowserWindow
 *    Browser window that is closing.
 */
void 
GuiManager::reparentNonModalDialogs(BrainBrowserWindow* closingBrainBrowserWindow)
{
    BrainBrowserWindow* firstBrainBrowserWindow = NULL;
    
    for (int32_t i = 0; i < static_cast<int32_t>(m_brainBrowserWindows.size()); i++) {
        if (m_brainBrowserWindows[i] != NULL) {
            if (m_brainBrowserWindows[i] != closingBrainBrowserWindow) {
                firstBrainBrowserWindow = m_brainBrowserWindows[i];
                break;
            }
        }
    }
    
    if (firstBrainBrowserWindow != NULL) {
        const int32_t numNonModalDialogs = static_cast<int32_t>(this->nonModalDialogs.size());
        for (int32_t i = 0; i < numNonModalDialogs; i++) {
            QWidget* d = this->nonModalDialogs[i];
            if (d->parent() == closingBrainBrowserWindow) {
                d->setParent(firstBrainBrowserWindow, d->windowFlags());
                d->hide();
            }
        }
    }
}

/**
 * Show the surface properties editor dialog.
 * @param browserWindow
 *    Browser window on which dialog is displayed.
 */
void
GuiManager::processShowSurfacePropertiesEditorDialog(BrainBrowserWindow* browserWindow)
{
    bool wasCreatedFlag = false;
    
    if (this->m_surfacePropertiesEditorDialog == NULL) {
        m_surfacePropertiesEditorDialog = new SurfacePropertiesEditorDialog(browserWindow);
        this->nonModalDialogs.push_back(m_surfacePropertiesEditorDialog);
        m_surfacePropertiesEditorDialog->setSavePositionForNextTime(true);
        wasCreatedFlag = true;
    }
    m_surfacePropertiesEditorDialog->setVisible(true);
    m_surfacePropertiesEditorDialog->show();
    m_surfacePropertiesEditorDialog->activateWindow();
    
    if (wasCreatedFlag) {
        WuQtUtilities::moveWindowToSideOfParent(browserWindow,
                                                m_surfacePropertiesEditorDialog);
    }
}


/**
 * Show the scene dialog.  If dialog needs to be created, use the
 * given window as the parent.
 * @param browserWindowIn
 *    Parent of scene dialog if it needs to be created.
 */
void 
GuiManager::processShowSceneDialog(BrainBrowserWindow* browserWindowIn)
{
    bool wasCreatedFlag = false;

    BrainBrowserWindow* browserWindow = browserWindowIn;
    
    if (this->sceneDialog == NULL) {
            this->sceneDialog = new SceneDialog(browserWindow);
            this->nonModalDialogs.push_back(this->sceneDialog);
//            this->sceneDialog->resize(600, 200);
            this->sceneDialog->setSavePositionForNextTime(true);
        wasCreatedFlag = true;
    }
    this->sceneDialog->setVisible(true);
    this->sceneDialog->show();
    this->sceneDialog->activateWindow();
    
    if (wasCreatedFlag) {
        WuQtUtilities::moveWindowToSideOfParent(browserWindow,
                                                this->sceneDialog);
    }
}

/**
 * Show the scene dialog and load the given scene from the given scene file.
 * If displaying the scene had an error, the scene dialog will remain open.
 * Otherwise, the scene dialog is closed.
 *
 * @param browserWindow
 *    Parent of scene dialog if it needs to be created.
 * @param sceneFile
 *    Scene File that contains the scene.
 * @param scene
 *    Scene that is displayed.
 */
void
GuiManager::processShowSceneDialogAndScene(BrainBrowserWindow* browserWindow,
                                           SceneFile* sceneFile,
                                           Scene* scene)
{
    processShowSceneDialog(browserWindow);
    const bool sceneWasDisplayed = this->sceneDialog->displayScene(sceneFile,
                                    scene);
    if (sceneWasDisplayed) {
        this->sceneDialog->close();
    }
}

/**
 * @return The action that indicates the enabled status
 * for display of the information window.
 */
QAction* 
GuiManager::getInformationDisplayDialogEnabledAction()
{
    return m_informationDisplayDialogEnabledAction;
}

/**
 * Show the information window.
 */
void 
GuiManager::processShowInformationWindow()
{
    this->processShowInformationDisplayDialog(true);
}

void 
GuiManager::showHideInfoWindowSelected(bool status)
{
    QString text("Show Information Window");
    if (status) {
        text = "Hide Information Window";
        if (m_informationDisplayDialogEnabledAction->signalsBlocked() == false) {
            this->processShowInformationDisplayDialog(true);
        }
    }
    
    text += ("\n\n"
             "When this button is 'on', the information window\n"
             "is automatically displayed when an identification\n"
             "operation (mouse click over surface or volume slice)\n"
             "is performed.  ");
    m_informationDisplayDialogEnabledAction->setToolTip(text);
}


/**
 * Show the information display window.
 * @param forceDisplayOfDialog
 *   If true, the window will be displayed even if its display
 *   enabled status is off.
 */
void 
GuiManager::processShowInformationDisplayDialog(const bool forceDisplayOfDialog)
{
    if (m_informationDisplayDialog == NULL) {
        std::vector<BrainBrowserWindow*> bbws = this->getAllOpenBrainBrowserWindows();
        if (bbws.empty() == false) {
            BrainBrowserWindow* parentWindow = bbws[0];
            m_informationDisplayDialog = new InformationDisplayDialog(parentWindow);
            this->nonModalDialogs.push_back(m_informationDisplayDialog);
            
            m_informationDisplayDialog->resize(600, 200);
            m_informationDisplayDialog->setSavePositionForNextTime(true);
            WuQtUtilities::moveWindowToSideOfParent(parentWindow,
                                                    m_informationDisplayDialog);
        }
    }
    
    if (forceDisplayOfDialog
        || m_informationDisplayDialogEnabledAction->isChecked()) {
        if (m_informationDisplayDialog != NULL) {
            m_informationDisplayDialog->setVisible(true);
            m_informationDisplayDialog->show();
            m_informationDisplayDialog->activateWindow();
        }
    }
}

/**
 * Show the custom view dialog.
 * @param browserWindow
 *    Window on which dialog was requested.
 */
void
GuiManager::processShowCustomViewDialog(BrainBrowserWindow* browserWindow)
{
    if (m_customViewDialog == NULL) {
        m_customViewDialog = new CustomViewDialog(browserWindow);
        this->nonModalDialogs.push_back(m_customViewDialog);
    }
    
    const int32_t browserWindowIndex = browserWindow->getBrowserWindowIndex();
    m_customViewDialog->updateContent(browserWindowIndex);
    m_customViewDialog->setVisible(true);
    m_customViewDialog->show();
    m_customViewDialog->activateWindow();
    
}

/**
 * Show the image capture window.
 * @param browserWindow
 *    Window on which dialog was requested.
 */
void 
GuiManager::processShowImageCaptureDialog(BrainBrowserWindow* browserWindow)
{
    if (this->imageCaptureDialog == NULL) {
        this->imageCaptureDialog = new ImageCaptureDialog(browserWindow);
        this->nonModalDialogs.push_back(this->imageCaptureDialog);
    }
    this->imageCaptureDialog->updateDialog();
    this->imageCaptureDialog->setBrowserWindowIndex(browserWindow->getBrowserWindowIndex());
    this->imageCaptureDialog->setVisible(true);
    this->imageCaptureDialog->show();
    this->imageCaptureDialog->activateWindow();
}

/**
 * Show the record movie window.
 * @param browserWindow
 *    Window on which dialog was requested.
 */
void 
GuiManager::processShowMovieDialog(BrainBrowserWindow* browserWindow)
{
    if (this->movieDialog == NULL) {
        this->movieDialog = new MovieDialog(browserWindow);
        this->nonModalDialogs.push_back(this->movieDialog);
    }
    //this->movieDialog->updateDialog();
    //this->movieDialog->setBrowserWindowIndex(browserWindow->getBrowserWindowIndex());
    this->movieDialog->setVisible(true);
    this->movieDialog->show();
    this->movieDialog->activateWindow();
}



/**
 * Show the preferences window.
 * @param browserWindow
 *    Window on which dialog was requested.
 */
void 
GuiManager::processShowPreferencesDialog(BrainBrowserWindow* browserWindow)
{
    if (this->preferencesDialog == NULL) {
        this->preferencesDialog = new PreferencesDialog(browserWindow);
        this->nonModalDialogs.push_back(this->preferencesDialog);
    }
    this->preferencesDialog->updateDialog();
    this->preferencesDialog->setVisible(true);
    this->preferencesDialog->show();
    this->preferencesDialog->activateWindow();
}

/**
 * Show the allen database web view.
 * @param browserWindow
 *    If the web view needs to be created, use this as parent.
 */
void 
GuiManager::processShowAllenDataBaseWebView(BrainBrowserWindow* browserWindow)
{
    WuQMessageBox::informationOk(browserWindow, 
                                 "Allen Database connection not yet implemented");
}

/**
 * Show the connectome database web view.
 * @param browserWindow
 *    If the web view needs to be created, use this as parent.
 */
void 
GuiManager::processShowConnectomeDataBaseWebView(BrainBrowserWindow* /*browserWindow*/)
{
    if (this->connectomeDatabaseWebView == NULL) {
        this->connectomeDatabaseWebView = new WuQWebView();
        this->connectomeDatabaseWebView->load(QUrl("https://db.humanconnectome.org/"));
        this->nonModalDialogs.push_back(this->connectomeDatabaseWebView);
    }
    this->connectomeDatabaseWebView->show();
//    this->connectomeDatabaseWebView->activateWindow();
}

/**
 * Show Timeseries Time Course
 */
void GuiManager::processUpdateTimeCourseDialogs()
{
    //if(!this->timeCourseDialog) this->timeCourseDialog = this->getTimeCourseDialog();
    QList<TimeCourseDialog *> list = this->timeCourseDialogs.values();
    for(int i=0;i<list.size();i++)
    {
        list[i]->updateDialog();
    }    
}

/**
  * Allows Connectivity Manager to update the Time Course Dialog
  */
TimeCourseDialog * GuiManager::getTimeCourseDialog(void *id)
{
    if(timeCourseDialogs.contains(id)) return timeCourseDialogs.value(id);
    BrainBrowserWindow* browserWindow = NULL;

    for (int32_t i = 0; i < static_cast<int32_t>(m_brainBrowserWindows.size()); i++) {
        if (m_brainBrowserWindows[i] != NULL && m_brainBrowserWindows[i]->isVisible()) {
            if (m_brainBrowserWindows[i] != NULL) {
                browserWindow = m_brainBrowserWindows[i];
                break;
            }
        }
    }

    if(browserWindow == NULL) return NULL;//not the best error checking but at least it
                                     //won't crash
    if (this->timeCourseDialogs[id] == NULL) {
        this->timeCourseDialogs.insert(id, new TimeCourseDialog(browserWindow));
        this->nonModalDialogs.push_back(this->timeCourseDialogs[id]);
    }
    return this->timeCourseDialogs[id];
}

/**
 * Adds time lines to all corresponding time course dialogs
 */
void GuiManager::addTimeLines(QList <TimeLine> &tlV)
{
    for(int i =0;i<tlV.size();i++)
    {
        this->getTimeCourseDialog(tlV[i].id)->addTimeLine(tlV[i]);
    }
}

/**
 * Removes Time Course Dialog from GuiManager and calls destroy on object
 */
void GuiManager::removeTimeCourseDialog(void *id)
{
    this->timeCourseDialogs.remove(id);
    //this->nonModalDialogs remove
}

/**
 * sets animation start time for Time Course Dialogs
 */
void GuiManager::updateAnimationStartTime(double /*value*/)
{
       
}

/**
 * Capture an image of the browser window's graphics area.
 * If either of the image dimensions
 * is zero, the image will be the size of the graphcis 
 * area.
 *
 * @param browserWindowIndex
 *    Index of the browser window.
 * @param imageSizeX
 *    Desired X size of image.
 * @param imageSizeY
 *    Desired X size of image.
 * @param imageFileOut
 *    ImageFile that will contain the captured image.
 * @return 
 *    true if the browser window index was valid, else false.
 */
bool 
GuiManager::captureImageOfBrowserWindowGraphicsArea(const int32_t browserWindowIndex,
                                                    const int32_t imageSizeX,
                                                    const int32_t imageSizeY,
                                                    ImageFile& imageFileOut,
                                                    bool updateWindow    )
{
    bool valid = false;
    
    const int32_t numBrowserWindows = static_cast<int32_t>(m_brainBrowserWindows.size());
    if ((browserWindowIndex >= 0) 
        && (browserWindowIndex < numBrowserWindows)) {
        BrainBrowserWindow* bbw = m_brainBrowserWindows[browserWindowIndex];
        if (bbw != NULL) {
            QImage image = bbw->captureImageOfGraphicsArea(imageSizeX, imageSizeY);
            imageFileOut.setFromQImage(image);
            valid = true;
        }
    }

    /*
     * Image capture sometimes messes up window so redraw it.
     */
    if(updateWindow) EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(browserWindowIndex).getPointer());

    return valid;
}

/**
 * @return The cursor manager.
 */
const 
CursorManager* 
GuiManager::getCursorManager() const
{
    return this->cursorManager;
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
GuiManager::saveToScene(const SceneAttributes* sceneAttributes,
                        const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "GuiManager",
                                            1);
    
    /*
     * Save session manager (brain, etc)
     */
    sceneClass->addClass(SessionManager::get()->saveToScene(sceneAttributes, 
                                                            "m_sessionManager"));
    

    /*
     * Save windows
     */
    std::vector<SceneClass*> browserWindowClasses;
    const int32_t numBrowserWindows = static_cast<int32_t>(m_brainBrowserWindows.size());
    for (int32_t i = 0; i < numBrowserWindows; i++) {
        BrainBrowserWindow* bbw = m_brainBrowserWindows[i];
        if (bbw != NULL) {
            browserWindowClasses.push_back(bbw->saveToScene(sceneAttributes,
                                                  "m_brainBrowserWindows"));
        }
    }
    SceneClassArray* browserWindowArray = new SceneClassArray("m_brainBrowserWindows",
                                                              browserWindowClasses);
    sceneClass->addChild(browserWindowArray);

    /*
     * Save information window
     */
    if (m_informationDisplayDialog != NULL) {
        sceneClass->addClass(m_informationDisplayDialog->saveToScene(sceneAttributes,
                                                                     "m_informationDisplayDialog"));
    }
    
    /*
     * Save surface properties window
     */
    if (m_surfacePropertiesEditorDialog != NULL) {
        sceneClass->addClass(m_surfacePropertiesEditorDialog->saveToScene(sceneAttributes,
                                                                          "m_surfacePropertiesEditorDialog"));
    }
    
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
GuiManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                             const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    /*
     * Invalid first window position used when 
     * positioning other windows
     */
    SceneWindowGeometry::setFirstBrowserWindowCoordinatesInvalid();
        
    /*
     * Reset the brain
     */
    Brain* brain = GuiManager::get()->getBrain();
    brain->resetBrainKeepSceneFiles();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());    

    /*
     * Close all but one window
     */
    BrainBrowserWindow* firstBrowserWindow = getActiveBrowserWindow();;
    if (firstBrowserWindow != NULL) {
        closeAllOtherWindows(firstBrowserWindow);
    }
    
    /*
     * Update the windows
     */
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());    
    
    /*
     * Block graphics update events
     */
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS, 
                                    true);
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW,
                                    true);
    
    /*
     * Restore session manager
     */
    SessionManager::get()->restoreFromScene(sceneAttributes, 
                                            sceneClass->getClass("m_sessionManager"));

    /*
     * See if models available (user may have cancelled scene loading.
     */
    EventModelGetAll getAllModelsEvent;
    EventManager::get()->sendEvent(getAllModelsEvent.getPointer());
    const bool haveModels = (getAllModelsEvent.getModels().empty() == false);

    if (haveModels) {
        /*
         * Get open windows
         */
        std::list<BrainBrowserWindow*> availableWindows;
        const int32_t numBrowserWindows = static_cast<int32_t>(m_brainBrowserWindows.size());
        for (int32_t i = 0; i < numBrowserWindows; i++) {
            if (m_brainBrowserWindows[i] != NULL) {
                availableWindows.push_back(m_brainBrowserWindows[i]);
            }
        }
        
        /*
         * Restore windows
         */
        const SceneClassArray* browserWindowArray = sceneClass->getClassArray("m_brainBrowserWindows");
        if (browserWindowArray != NULL) {
            const int32_t numBrowserClasses = browserWindowArray->getNumberOfArrayElements();
            for (int32_t i = 0; i < numBrowserClasses; i++) {
                const SceneClass* browserClass = browserWindowArray->getClassAtIndex(i);
                BrainBrowserWindow* bbw = NULL;
                if (availableWindows.empty() == false) {
                    bbw = availableWindows.front();
                    availableWindows.pop_front();
                }
                else {
                    bbw = newBrainBrowserWindow(NULL,
                                                NULL,
                                                false);
                }
                if (bbw != NULL) {
                    bbw->restoreFromScene(sceneAttributes,
                                          browserClass);
                }
            }
        }
        
        /*
         * Close windows not needed
         */
        //    for (std::list<BrainBrowserWindow*>::iterator iter = availableWindows.begin();
        //         iter != availableWindows.end();
        //         iter++) {
        //        BrainBrowserWindow* bbw = *iter;
        //        bbw->close();
        //    }
        
        /*
         * Restore information window
         */
        const SceneClass* infoWindowClass = sceneClass->getClass("m_informationDisplayDialog");
        if (infoWindowClass != NULL) {
            if (m_informationDisplayDialog == NULL) {
                processShowInformationWindow();
            }
            else if (m_informationDisplayDialog->isVisible() == false) {
                processShowInformationWindow();
            }
            m_informationDisplayDialog->restoreFromScene(sceneAttributes,
                                                         infoWindowClass);
        }
        else {
            if (m_informationDisplayDialog != NULL) {
                /*
                 * Will clear text
                 */
                m_informationDisplayDialog->restoreFromScene(sceneAttributes,
                                                             NULL);
            }
        }
        
        /*
         * Restore surface properties
         */
        const SceneClass* surfPropClass = sceneClass->getClass("m_surfacePropertiesEditorDialog");
        if (surfPropClass != NULL) {
            if (m_surfacePropertiesEditorDialog == NULL) {
                processShowSurfacePropertiesEditorDialog(firstBrowserWindow);
            }
            else if (m_surfacePropertiesEditorDialog->isVisible() == false) {
                processShowSurfacePropertiesEditorDialog(firstBrowserWindow);
            }
            m_surfacePropertiesEditorDialog->restoreFromScene(sceneAttributes,
                                                              surfPropClass);
        }
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    /*
     * Unblock graphics updates
     */
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS, 
                                    false);
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW,
                                    false);

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Get the name of the data file that may have been set
 * if Workbench was started by double-clicking a file
 * in the Mac OSX finder.
 */
AString
GuiManager::getNameOfDataFileToOpenAfterStartup() const
{
    return m_nameOfDataFileToOpenAfterStartup;
}

