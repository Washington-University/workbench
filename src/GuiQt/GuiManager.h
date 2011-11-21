

#ifndef __GUI_MANAGER_H__
#define __GUI_MANAGER_H__

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

#include <set>
#include <vector>
#include <stdint.h>

#include <QObject>

#include "EventListenerInterface.h"

class QDialog;
class QImage;
class QWidget;

namespace caret {
    
    class Brain;
    class BrainBrowserWindow;
    class BrowserTabContent;
    class ImageCaptureDialog;
    class PreferencesDialog;
    
    /**
     * Manages the graphical user-interface.
     */
    class GuiManager : public QObject, public EventListenerInterface {
        
        Q_OBJECT
        
    public:
        static GuiManager* get();
        
        static void createGuiManager();
        
        static void deleteGuiManager();
        
        Brain* getBrain();
        
        int32_t getNumberOfBrainBrowserWindows() const;
        
        std::vector<BrainBrowserWindow*> getAllBrainBrowserWindows() const;
        
        bool allowBrainBrowserWindowToClose(BrainBrowserWindow* bbw,
                                            const int32_t numberOfOpenTabs);
        
        bool exitProgram(QWidget* parent);
        
        QString applicationName() const;
        
        //BrainOpenGL* getBrainOpenGL();
        
        BrowserTabContent* getBrowserTabContentForBrowserWindow(const int32_t browserWindowIndex,
                                                                const bool allowInvalidBrowserWindowIndex);
        
        void receiveEvent(Event* event);
        
        void closeOtherWindowsAndReturnTheirTabContent(BrainBrowserWindow* browserWindow,
                                                       std::vector<BrowserTabContent*>& tabContents);
        
        void processShowImageCaptureDialog(BrainBrowserWindow* browserWindow);
        void processShowPreferencesDialog(BrainBrowserWindow* browserWindow);
        
        bool captureImageOfBrowserWindowGraphicsArea(const int32_t browserWindowIndex,
                                                     const int32_t imageSizeX,
                                                     const int32_t imageSizeY,
                                                     QImage& imageOut);
    public slots:
        void processBringAllWindowsToFront();
        void processShowDataDisplayWindow();
        void processShowHelpOnlineWindow();
        void processShowSearchHelpOnlineWindow();
        
    private:
        GuiManager(QObject* parent = 0);
        
        virtual ~GuiManager();
        
        
        GuiManager(const GuiManager&);
        GuiManager& operator=(const GuiManager&);
        
        BrainBrowserWindow* newBrainBrowserWindow(QWidget* parent,
                                                  BrowserTabContent* browserTabContent);
        
        void reparentNonModalDialogs(BrainBrowserWindow* closingBrainBrowserWindow);
        
        /** One instance of the GuiManager */
        static GuiManager* singletonGuiManager;
        
        /** 
         * Contains pointers to Brain Browser windows
         * As BrainBrowser windows are closed, some of
         * the elements may be NULL.
         */
        std::vector<BrainBrowserWindow*> brainBrowserWindows;
        
        /** Name of application */
        QString nameOfApplication;
        
        /** Skips confirmation of browser window closing when all tabs are moved to one window */
        bool allowBrowserWindowsToCloseWithoutConfirmation;
        
        /* Performs OpenGL drawing commands */
        //BrainOpenGL* brainOpenGL;
        
        ImageCaptureDialog* imageCaptureDialog;
        
        PreferencesDialog* preferencesDialog;
        
        /** 
         * Tracks non-modal dialogs that are created only one time
         * and may need to be reparented if the original parent, a
         * BrainBrowserWindow is closed in which case the dialog
         * is reparented to a different BrainBrowserWindow.
         */
        std::vector<QDialog*> nonModalDialogs;
    };
    
#ifdef __GUI_MANAGER_DEFINE__
    GuiManager* GuiManager::singletonGuiManager = NULL;
#endif // __GUI_MANAGER_DEFINE__
}

#endif // __GUI_MANAGER_H__
