

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

#include <vector>
#include <stdint.h>

#include <QObject>

#include "EventListenerInterface.h"

class QWidget;

namespace caret {
    
    class Brain;
    class BrainBrowserWindow;
    class BrainOpenGL;
    class BrowserTabContent;
    
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
        
        QVector<BrainBrowserWindow*> getAllBrainBrowserWindows() const;
        
        bool allowBrainBrowserWindowToClose(BrainBrowserWindow* bbw,
                                            const int32_t numberOfOpenTabs);
        
        bool exitProgram(QWidget* parent);
        
        QString applicationName() const;
        
        BrainOpenGL* getBrainOpenGL();
        
        BrowserTabContent* getBrowserTabContentForBrowserWindow(const int32_t browserWindowIndex);
        
        void receiveEvent(Event* event);
        
    public slots:
        void processBringAllWindowsToFront();
        void processShowIdentifyWindow();
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
        
        /* Performs OpenGL drawing commands */
        BrainOpenGL* brainOpenGL;
    };
    
#ifdef __GUI_MANAGER_DEFINE__
    GuiManager* GuiManager::singletonGuiManager = NULL;
#endif // __GUI_MANAGER_DEFINE__
}

#endif // __GUI_MANAGER_H__
