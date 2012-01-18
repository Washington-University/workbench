
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

#include <QApplication>

#define __GUI_MANAGER_DEFINE__
#include "GuiManager.h"
#undef __GUI_MANAGER_DEFINE__

#include "BrainBrowserWindow.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "DisplayControlDialog.h"
#include "EventBrowserWindowNew.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "ImageFile.h"
#include "ImageCaptureDialog.h"
#include "PreferencesDialog.h"
#include "SessionManager.h"

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
    
    this->displayControlDialog = NULL;
    this->imageCaptureDialog = NULL;
    this->preferencesDialog = NULL;    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_NEW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_TIME_COURSE_DIALOG);
}

/**
 * Destructor.
 */
GuiManager::~GuiManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
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
        for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
            if (this->brainBrowserWindows[i] == brainBrowserWindow) {
                this->reparentNonModalDialogs(this->brainBrowserWindows[i]);
                this->brainBrowserWindows[i] = NULL;
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
    for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
        if (this->brainBrowserWindows[i] != NULL) {
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
    
    int32_t numWindows = static_cast<int32_t>(this->brainBrowserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        if (this->brainBrowserWindows[i] != NULL) {
            windows.push_back(this->brainBrowserWindows[i]);
        }
    }
    
    return windows; 
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
    if (browserWindowIndex < static_cast<int32_t>(this->brainBrowserWindows.size())) {
        return this->brainBrowserWindows[browserWindowIndex];
    }
    return NULL;
}

/**
 * Create a new BrainBrowser Window.
 * @param parent
 *    Optional parent that is used only for window placement.
 * @param browserTabContent
 *    Optional tab for initial windwo tab.
 */
BrainBrowserWindow*
GuiManager::newBrainBrowserWindow(QWidget* parent,
                                  BrowserTabContent* browserTabContent)
{
    int32_t windowIndex = -1;
    
    int32_t numWindows = static_cast<int32_t>(this->brainBrowserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        if (this->brainBrowserWindows[i] == NULL) {
            windowIndex = i;
            break;
        }
    }
    
    BrainBrowserWindow* bbw = NULL; 
    
    if (windowIndex < 0) {
        windowIndex = this->brainBrowserWindows.size();
        bbw = new BrainBrowserWindow(windowIndex, browserTabContent);
        this->brainBrowserWindows.push_back(bbw);
    }
    else {
        bbw = new BrainBrowserWindow(windowIndex, browserTabContent);
        this->brainBrowserWindows[windowIndex] = bbw;
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
    const bool areFilesModified = false;
    if (areFilesModified) {
        WuQMessageBox::StandardButton buttonPressed = 
        WuQMessageBox::saveDiscardCancel(parent, 
                                         "Files are modified.", 
                                         "Do you want to save changes?");
        
        switch (buttonPressed) {
            case QMessageBox::Save:
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
        if (browserWindowIndex >= static_cast<int32_t>(this->brainBrowserWindows.size())) {
            return NULL;
        }
    }
    
    CaretAssertVectorIndex(this->brainBrowserWindows, browserWindowIndex);
    BrainBrowserWindow* browserWindow = brainBrowserWindows[browserWindowIndex];
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
    for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
        if (this->brainBrowserWindows[i] != NULL) {
            this->brainBrowserWindows[i]->show();
            this->brainBrowserWindows[i]->activateWindow();
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
                                                              eventNewBrowser->getBrowserTabContent());
        eventNewBrowser->setBrowserWindowCreated(bbw);
        
        eventNewBrowser->setEventProcessed();
    }
    else if(event->getEventType() == EventTypeEnum::EVENT_UPDATE_TIME_COURSE_DIALOG) {
        this->processUpdateTimeCourseDialogs();
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
    
    const int32_t numWindows = this->brainBrowserWindows.size();
    for (int32_t i = 0; i < numWindows; i++) {
        BrainBrowserWindow* bbw = this->brainBrowserWindows[i];
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
    
    for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
        if (this->brainBrowserWindows[i] != NULL) {
            if (this->brainBrowserWindows[i] != closingBrainBrowserWindow) {
                firstBrainBrowserWindow = this->brainBrowserWindows[i];
                break;
            }
        }
    }
    
    if (firstBrainBrowserWindow != NULL) {
        const int32_t numNonModalDialogs = static_cast<int32_t>(this->nonModalDialogs.size());
        for (int32_t i = 0; i < numNonModalDialogs; i++) {
            QDialog* d = this->nonModalDialogs[i];
            if (d->parent() == closingBrainBrowserWindow) {
                d->setParent(firstBrainBrowserWindow, d->windowFlags());
                d->hide();
            }
        }
    }
}

/**
 * Show the image capture window.
 */
void 
GuiManager::processShowImageCaptureDialog(BrainBrowserWindow* browserWindow)
{
    if (this->imageCaptureDialog == NULL) {
        this->imageCaptureDialog = new ImageCaptureDialog(browserWindow);
        this->nonModalDialogs.push_back(this->imageCaptureDialog);
    }
    this->imageCaptureDialog->updateDialog();
    this->imageCaptureDialog->setVisible(true);
    this->imageCaptureDialog->show();
    this->imageCaptureDialog->activateWindow();
}

/**
 * Show the preferences window.
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
 * Show the preferences window.
 */
void 
GuiManager::processShowDisplayControlDialog(BrainBrowserWindow* browserWindow)
{
    if (this->displayControlDialog == NULL) {
        this->displayControlDialog = new DisplayControlDialog(browserWindow);
        this->nonModalDialogs.push_back(this->displayControlDialog);
    }
    this->displayControlDialog->updateDialog();
    this->displayControlDialog->setVisible(true);
    this->displayControlDialog->show();
    this->displayControlDialog->activateWindow();
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

    for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
        if (this->brainBrowserWindows[i] != NULL && this->brainBrowserWindows[i]->isVisible()) {
            if (this->brainBrowserWindows[i] != NULL) {
                browserWindow = this->brainBrowserWindows[i];
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
                                                    ImageFile& imageFileOut)
{
    bool valid = false;
    
    const int32_t numBrowserWindows = static_cast<int32_t>(this->brainBrowserWindows.size());
    if ((browserWindowIndex >= 0) 
        && (browserWindowIndex < numBrowserWindows)) {
        BrainBrowserWindow* bbw = this->brainBrowserWindows[browserWindowIndex];
        if (bbw != NULL) {
            QImage image = bbw->captureImageOfGraphicsArea(imageSizeX, imageSizeY);
            imageFileOut.setFromQImage(image);
            valid = true;
        }
    }

    /*
     * Image capture sometimes messes up window so redraw it.
     */
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(browserWindowIndex).getPointer());

    return valid;
}


