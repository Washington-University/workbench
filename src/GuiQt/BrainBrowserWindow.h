
#ifndef __BRAIN_BROWSER_WINDOW_H__
#define __BRAIN_BROWSER_WINDOW_H__

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

#include <stdint.h>

#include <QImage>
#include <QMainWindow>

#include "AString.h"
#include "BrainBrowserWindowScreenModeEnum.h"
#include "DataFileException.h"
#include "DataFileTypeEnum.h"

class QAction;
class QActionGroup;
class QDockWidget;
class QMenu;

namespace caret {
    class BrainBrowserWindowToolBar;
    class BrainOpenGLWidget;
    class BrowserTabContent;

    
    /**
     * The brain browser window is the viewer for
     * brain models.  It may contain multiple tabs
     * with each tab displaying brain models.
     */ 
    class BrainBrowserWindow : public QMainWindow  {
        Q_OBJECT
        
    public:
        virtual ~BrainBrowserWindow();
        
        BrowserTabContent* getBrowserTabContent();
        
        QMenu* createPopupMenu();
        
        void removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent);
        
        int32_t getBrowserWindowIndex() const;

        QImage captureImageOfGraphicsArea(const int32_t imageSizeX,
                                          const int32_t imageSizeY);
        
        BrainBrowserWindowScreenModeEnum::Enum getScreenMode() const;
        
        /**
         * Mode for loading spec files
         */
        enum LoadSpecFileMode {
            /** Do not show spec file dialog, just load all files listed in spec file listed on command line at program startup */
            LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE,
            /** Show spec file in spec file dialog for user selections */
            LOAD_SPEC_FILE_WITH_DIALOG,
            /** Show spec file in spec file dialog for user selections from spec file listed on command line at program startup */
            LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE
        };
        
        void loadFilesFromCommandLine(const std::vector<AString>& filenames,
                                      const LoadSpecFileMode loadSpecFileMode);
        
    protected:
        void closeEvent(QCloseEvent* event);
        
    private slots:        
        void processAboutWorkbench();
        void processInformationDialog();
        void processNewWindow();
        void processNewTab();
        void processDataFileLocationOpen();
        void processDataFileOpen();
        void processDataFileOpenFromSpecFile();
        void processManageSaveLoadedFiles();
        void processCaptureImage();
        void processEditPreferences();
        void processCloseSpecFile();
        void processExitProgram();
        void processViewScreenActionGroupSelection(QAction*);
        void processMoveAllTabsToOneWindow();
        
        void processMoveOverlayToolBoxToLeft();
        void processMoveOverlayToolBoxToBottom();
        void processMoveOverlayToolBoxToFloat();
        void processHideOverlayToolBox();
        
        void processMoveFeaturesToolBoxToRight();
        void processMoveFeaturesToolBoxToFloat();
        void processHideFeaturesToolBox();
        
        void processMoveSelectedTabToWindowMenuAboutToBeDisplayed();
        void processMoveSelectedTabToWindowMenuSelection(QAction*);
        
        void processRecentSpecFileMenuAboutToBeDisplayed();
        void processRecentSpecFileMenuSelection(QAction*);
        
        void processShowOverlayToolBox(bool);
        void processShowFeaturesToolBox(bool);
        void processOverlayHorizontalToolBoxVisibilityChanged(bool);
        void processOverlayVerticalToolBoxVisibilityChanged(bool);
        
        void shrinkToolbox();
        
        void processFileMenuAboutToShow();
        void processViewMenuAboutToShow();
        
        void processSurfaceMenuInformation();
        void processSurfaceMenuVolumeInteraction();
        
        void processConnectToAllenDataBase();
        void processConnectToConnectomeDataBase();
        
        void processHcpWebsiteInBrowser();
                                       
    private:
        // Contains status of components such as enter/exit full screen
        struct WindowComponentStatus {
            bool isFeaturesToolBoxDisplayed;
            bool isOverlayToolBoxDisplayed;
            bool isToolBarDisplayed;
        };
        
        enum AddDataFileToSpecFileMode {
            ADD_DATA_FILE_TO_SPEC_FILE_YES,
            ADD_DATA_FILE_TO_SPEC_FILE_NO
        };
        
        BrainBrowserWindow(const int browserWindowIndex,
                           BrowserTabContent* browserTabContent,
                           QWidget* parent = 0,
                           Qt::WindowFlags flags = 0);
        
        BrainBrowserWindow(const BrainBrowserWindow&);
        BrainBrowserWindow& operator=(const BrainBrowserWindow&);
        
        void loadFiles(const std::vector<AString>& filenames,
                       const LoadSpecFileMode loadSpecFileMode,
                       const AddDataFileToSpecFileMode addDataFileToSpecFileMode);
        
        void createActions();
        void createActionsUsedByToolBar();
        void createMenus();
        
        QMenu* createMenuFile();
        QMenu* createMenuView();
        QMenu* createMenuViewMoveOverlayToolBox();
        QMenu* createMenuViewMoveFeaturesToolBox();
        QMenu* createMenuConnect();
        QMenu* createMenuData();
        QMenu* createMenuSurface();
        QMenu* createMenuVolume();
        QMenu* createMenuWindow();
        QMenu* createMenuHelp();
        
        void moveOverlayToolBox(Qt::DockWidgetArea area);
        void moveFeaturesToolBox(Qt::DockWidgetArea area);
        
        void restoreWindowComponentStatus(const WindowComponentStatus& wcs);
        void saveWindowComponentStatus(WindowComponentStatus& wcs);
        
        void openSpecFile(const AString& specFileName) throw (DataFileException);
        
        /** Index of this window */
        int32_t browserWindowIndex;
        
        BrainOpenGLWidget* openGLWidget;
        
        BrainBrowserWindowToolBar* toolbar;
        
        QAction* aboutWorkbenchAction;
        
        QAction* newWindowAction;
        
        QAction* newTabAction;
        
        QAction* openFileAction;
        
        QAction* openLocationAction;
        
        QAction* openFileViaSpecFileAction;
        
        QAction* manageFilesAction;
        
        QAction* closeSpecFileAction;
        
        QAction* closeTabAction;
        
        QAction* closeWindowAction;
        
        QAction* captureImageAction;
        
        QAction* preferencesAction;
        
        QAction* exitProgramAction;
        
        QAction* showToolBarAction;
        
        QActionGroup* viewScreenActionGroup;
        QAction* viewScreenNormalAction;
        QAction* viewScreenFullAction;
        QAction* viewScreenMontageTabsAction;
        QAction* viewScreenFullMontageTabsAction;        
        
        QAction* nextTabAction;
        
        QAction* previousTabAction;
        
        QAction* renameSelectedTabAction;
        
        QAction* moveTabsInWindowToNewWindowsAction;
        
        QAction* moveTabsFromAllWindowsToOneWindowAction;
        
        QAction* bringAllToFrontAction;
        
        QAction* informationDialogAction;
        
        QAction* connectToAllenDatabaseAction;
        QAction* connectToConnectomeDatabaseAction;

        QAction* helpHcpWebsiteAction;
        QAction* helpOnlineAction;
        QAction* helpSearchOnlineAction;
        
        QAction* overlayToolBoxAction;
        
        QAction* featuresToolBoxAction;
        
        QMenu* moveSelectedTabToWindowMenu;
        
        QMenu* recentSpecFileMenu;
        
        QDockWidget* overlayHorizontalToolBox;
        QDockWidget* overlayVerticalToolBox;
        QDockWidget* overlayActiveToolBox;
        QDockWidget* featuresToolBox;
        
        static AString previousOpenFileNameFilter;
        static bool previousOpenFileAddToSpecFileSelection;
        
        
        BrainBrowserWindowScreenModeEnum::Enum screenMode;
        
        WindowComponentStatus normalWindowComponentStatus;
                
        static bool firstWindowFlag;
        
        friend class BrainBrowserWindowToolBar;
        friend class GuiManager;
    };
#ifdef __BRAIN_BROWSER_WINDOW_DECLARE__
    AString BrainBrowserWindow::previousOpenFileNameFilter;
    bool BrainBrowserWindow::previousOpenFileAddToSpecFileSelection = true;
    bool BrainBrowserWindow::firstWindowFlag = true;
#endif // __BRAIN_BROWSER_WINDOW_DECLARE__
    
}

#endif // __BRAIN_BROWSER_WINDOW_H__

