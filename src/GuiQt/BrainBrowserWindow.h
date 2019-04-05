
#ifndef __BRAIN_BROWSER_WINDOW_H__
#define __BRAIN_BROWSER_WINDOW_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <set>
#include <stdint.h>

#include <QImage>
#include <QMainWindow>

#include "AString.h"
#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"
#include "TileTabsConfigurationModeEnum.h"

class QAction;
class QActionGroup;
class QMenu;
class QToolButton;
class QDockWidget;

namespace caret {
    class BrainBrowserWindowToolBar;
    class BrainBrowserWindowOrientedToolBox;
    class BrainOpenGLWidget;
    class BrowserWindowContent;
    class BrowserTabContent;
    class EventTileTabsConfigurationModification;
    class PlainTextStringBuilder;
    class SceneClassAssistant;
    class TileTabsConfiguration;

    
    /**
     * The brain browser window is the viewer for
     * brain models.  It may contain multiple tabs
     * with each tab displaying brain models.
     */ 
    class BrainBrowserWindow : public QMainWindow, public EventListenerInterface, public SceneableInterface  {
        Q_OBJECT
        
    public:
        virtual ~BrainBrowserWindow();
        
        virtual void receiveEvent(Event* event);
        
        BrowserTabContent* getBrowserTabContent();

        const BrowserTabContent* getBrowserTabContent() const;
        
        BrowserTabContent* getBrowserTabContent(int tabIndex);

        BrowserWindowContent* getBrowerWindowContent();
        
        const BrowserWindowContent* getBrowerWindowContent() const;
        
        QMenu* createPopupMenu();
        
        void getAllTabContent(std::vector<BrowserTabContent*>& allTabContent) const;
        
        void getAllTabContentIndices(std::vector<int32_t>& allTabContentIndices) const;
        
        void removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent);
        
        int32_t getBrowserWindowIndex() const;

        bool isTileTabsSelected() const;
        
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
        
        void loadSceneFromCommandLine(const AString& sceneFileName,
                                      const AString& sceneNameOrNumber);
        
        bool loadFilesFromNetwork(QWidget* parentForDialogs,
                                  const std::vector<AString>& filenames,
                                  const std::vector<DataFileTypeEnum::Enum> dataFileTypes,
                                  const AString& username,
                                  const AString& password);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        void resetGraphicsWidgetMinimumSize();
        
        void setGraphicsWidgetFixedSize(const int32_t width,
                                        const int32_t height);
        
        void getGraphicsWidgetSize(int32_t& xOut,
                                   int32_t& yOut,
                                   int32_t& widthOut,
                                   int32_t& heightOut,
                                   int32_t& graphicsWidthOut,
                                   int32_t& graphicsHeightOut,
                                   const bool applyLockedAspectRatiosFlag) const;
        
        AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        static int32_t loadRecentSpecFileMenu(QMenu* recentSpecFileMenu);
        
        static int32_t loadRecentSceneFileMenu(QMenu* recentSpecFileMenu);
        
        float getOpenGLWidgetAspectRatio() const;
        
        bool changeInputModeToAnnotationsWarningDialog();
        
        bool isWindowAspectRatioLocked() const;
        
        float getAspectRatio() const;
        
        bool hasValidOpenGL();
        
        bool isOpenGLContextSharingValid() const;

        AString getTileTabsConfigurationLabelText(const TileTabsConfigurationModeEnum::Enum configurationMode,
                                              const bool includeRowsAndColumns) const;

        void resizeDockWidgets(const QList<QDockWidget *> &docks, const QList<int> &sizes, Qt::Orientation orientation);
        
        static void setEnableMacDuplicateMenuBar(bool status);
        
    protected:
        void closeEvent(QCloseEvent* event);
        void keyPressEvent(QKeyEvent* event);
        
    private slots:        
        void processAboutWorkbench();
        void processInformationDialog();
        void processNewWindow();
        void processNewTab();
        void processDuplicateTab();
        void processDataFileLocationOpen();
        void processDataFileOpen();
        void processManageSaveLoadedFiles();
        void processCaptureImage();
        void processMovieRecording();
        void processRecordMovie();
        void processEditPreferences();
        void processCloseAllFiles();
        void processCloseWindow();
        void processExitProgram();
        void processMoveAllTabsToOneWindow();
        void processViewFullScreenSelected();
        void processViewTileTabs();
        void processViewTileTabsConfigurationDialog();
        void processShowHelpInformation();
        void processShowIdentifyBrainordinateDialog();
        void processGapsAndMargins();
        
        void processViewTileTabsLoadUserConfigurationMenuAboutToShow();
        void processViewTileTabsLoadUserConfigurationMenuItemTriggered(QAction* action);
        void processViewTileTabsAutomaticCustomTriggered(QAction* action);
        

        void processMoveOverlayToolBoxToLeft();
        void processMoveOverlayToolBoxToBottom();
        void processMoveOverlayToolBoxToFloat();
        void processHideOverlayToolBox();
        
        void processMoveFeaturesToolBoxToRight();
        void processMoveFeaturesToolBoxToFloat();
        void processHideFeaturesToolBox();
        
        void processMoveSelectedTabToWindowMenuAboutToBeDisplayed();
        void processMoveSelectedTabToWindowMenuSelection(QAction*);
        
        void processRecentSceneFileMenuAboutToBeDisplayed();
        void processRecentSceneFileMenuSelection(QAction*);
        
        void processRecentSpecFileMenuAboutToBeDisplayed();
        void processRecentSpecFileMenuSelection(QAction*);
        
        void processShowOverlayToolBox(bool);
        void processShowFeaturesToolBox(bool);
        void processOverlayHorizontalToolBoxVisibilityChanged(bool);
        void processOverlayVerticalToolBoxVisibilityChanged(bool);
        
        void processFileMenuAboutToShow();
        void processDataMenuAboutToShow();
        void processViewMenuAboutToShow();
        
        void processSurfaceMenuInformation();
        void processSurfaceMenuPrimaryAnatomical();
        
        void processConnectToAllenDataBase();
        void processConnectToConnectomeDataBase();
        
        void processHcpWebsiteInBrowser();
        void processHcpFeatureRequestWebsiteInBrowser();
        void processReportWorkbenchBug();
        
        void processShowSurfacePropertiesDialog();
        void processShowVolumePropertiesDialog();
        
        void processDevelopGraphicsTiming();
        
        void processDevelopExportVtkFile();
        void developerMenuAboutToShow();
        void developerMenuFlagTriggered(QAction*);
        
        void processProjectFoci();
        void processSplitBorderFiles();
        
        void processWindowMenuLockWindowAspectRatioTriggered(bool checked);
        void processWindowMenuLockAllTabAspectRatioTriggered(bool checked);
        void processToolBarLockWindowAndAllTabAspectTriggered(bool checked);
        void processToolBarLockWindowAndAllTabAspectMenu(const QPoint& pos);

        void processEditMenuItemTriggered(QAction* action);
        void processEditMenuAboutToShow();
        
        void aspectRatioDialogUpdateForTab(const double aspectRatio);
        void aspectRatioDialogUpdateForWindow(const double aspectRatio);
        
    private:
        /** Contains status of components such as enter/exit full screen */
        struct WindowComponentStatus {
            bool isFeaturesToolBoxDisplayed;
            bool isOverlayToolBoxDisplayed;
            bool isToolBarDisplayed;
            QByteArray windowState;
            QByteArray windowGeometry;
            QByteArray featuresGeometry;
        };
        
        enum CreateDefaultTabsMode {
            CREATE_DEFAULT_TABS_YES,
            CREATE_DEFAULT_TABS_NO
        };
        
        enum class AspectRatioMode {
            TAB,
            WINDOW
        };
        
        BrainBrowserWindow(const int browserWindowIndex,
                           BrowserTabContent* browserTabContent,
                           const CreateDefaultTabsMode createDefaultTabsMode,
                           QWidget* parent = 0,
                           Qt::WindowFlags flags = 0);
        
        BrainBrowserWindow(const BrainBrowserWindow&);
        BrainBrowserWindow& operator=(const BrainBrowserWindow&);
        
        bool loadFiles(QWidget* parentForDialogs,
                       const std::vector<AString>& filenames,
                       const std::vector<DataFileTypeEnum::Enum> dataFileTypes,
                       const LoadSpecFileMode loadSpecFileMode,
                       const AString& username,
                       const AString& password);
        
        void createActions();
        void createActionsUsedByToolBar();
        void createMenus();
        
        QMenu* createMenuDevelop();
        QMenu* createMenuEdit();
        QMenu* createMenuFile();
        QMenu* createMenuView();
        QMenu* createMenuViewMoveOverlayToolBox();
        QMenu* createMenuViewMoveFeaturesToolBox();
        QMenu* createMenuViewTileTabsLoadUserConfiguration();
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
        
        void openSpecFile(const AString& specFileName);
        
        void processViewFullScreen(bool showFullScreenDisplay,
                                   const bool saveRestoreWindowStatus);
        
        void setViewTileTabs(const bool newStatus);
        
        bool isMacOptionKeyDown() const;
        
        void showDataFileReadWarningsDialog();
        
        void lockWindowAspectRatio(const bool checked);
        void lockAllTabAspectRatios(const bool checked);
        void updateActionsForLockingAspectRatios();
        
        float getAspectRatioFromDialog(const AspectRatioMode aspectRatioMode,
                                       const QString& title,
                                       const float aspectRatio,
                                       QWidget* parent) const;
        
        void saveBrowserWindowContentForScene();
        
        void modifyTileTabsConfiguration(EventTileTabsConfigurationModification* modEvent);

        /** Index of this window */
        const int32_t m_browserWindowIndex;
        
        BrowserWindowContent* m_browserWindowContent = NULL;
        
        BrainOpenGLWidget* m_openGLWidget;
        
        BrainBrowserWindowToolBar* m_toolbar;
        
        QAction* m_aboutWorkbenchAction;
        
        QAction* m_newWindowAction;
        
        QAction* m_newTabAction;
        
        QAction* m_duplicateTabAction;
        
        QAction* m_openFileAction;
        
        QAction* m_openLocationAction;
        
        QAction* m_manageFilesAction;
        
        QAction* m_closeSpecFileAction;
        
        QAction* m_closeTabAction;
        
        QAction* m_closeWindowAction;
        AString m_closeWindowActionConfirmTitle;
        AString m_closeWindowActionNoConfirmTitle;
        bool m_closeWithoutConfirmationFlag;
        
        QAction* m_captureImageAction;

        QAction* m_movieRecordingAction;
        
        QAction* m_recordMovieAction;
        
        QAction* m_preferencesAction;
        
        QAction* m_exitProgramAction;
        
        QAction* m_showToolBarAction;
        
        QMenu* m_viewMoveFeaturesToolBoxMenu;
        QMenu* m_viewMoveOverlayToolBoxMenu;
        QMenu* m_viewTileTabsLoadUserConfigurationMenu;
        
        QAction* m_viewFullScreenAction;
        QAction* m_viewTileTabsAction;
        
        QAction* m_viewTileTabsConfigurationDialogAction;
        QAction* m_viewAutomaticTileTabsConfigurationAction;
        QAction* m_viewCustomTileTabsConfigurationAction;
        std::vector<std::pair<QAction*, TileTabsConfiguration*>> m_viewCustomTileTabsConfigurationActions;
        
        QAction* m_gapsAndMarginsAction;
        
        QAction* m_nextTabAction;
        
        QAction* m_previousTabAction;
        
        QAction* m_renameSelectedTabAction;
        
        QAction* m_moveTabsInWindowToNewWindowsAction;
        
        QAction* m_moveTabsFromAllWindowsToOneWindowAction;
        
        QAction* m_bringAllToFrontAction;
        QAction* m_tileWindowsAction;
        
        QAction* m_informationDialogAction;
        
        QAction* m_connectToAllenDatabaseAction;
        QAction* m_connectToConnectomeDatabaseAction;

        QAction* m_helpHcpWebsiteAction;
        QAction* m_helpHcpFeatureRequestAction;
        QAction* m_helpWorkbenchBugReportAction;
        
        QAction* m_developMenuAction;
        QActionGroup* m_developerFlagsActionGroup;
        QAction* m_developerGraphicsTimingAction;
        QAction* m_developerExportVtkFileAction;
        
        QAction* m_overlayToolBoxAction;
        
        QAction* m_windowMenuLockWindowAspectRatioAction;
        QAction* m_windowMenuLockAllTabAspectRatioAction;
        QAction* m_toolBarLockWindowAndAllTabAspectRatioAction;
        QToolButton* m_toolBarLockWindowAndAllTabAspectRatioButton;
        
        QAction* m_featuresToolBoxAction;
        
        QAction* m_dataFociProjectAction;
        QAction* m_dataBorderFilesSplitAction;
        
        QMenu* m_moveSelectedTabToWindowMenu;
        
        QMenu* m_recentSpecFileMenu;
        AString m_recentSpecFileMenuOpenConfirmTitle;
        AString m_recentSpecFileMenuLoadNoConfirmTitle;
        
        QMenu* m_recentSceneFileMenu;
        
        QMenu* m_editMenu;
        QAction* m_editMenuRedoAction;
        QAction* m_editMenuUndoAction;
        
        BrainBrowserWindowOrientedToolBox* m_overlayHorizontalToolBox;
        BrainBrowserWindowOrientedToolBox* m_overlayVerticalToolBox;
        BrainBrowserWindowOrientedToolBox* m_overlayActiveToolBox;
        BrainBrowserWindowOrientedToolBox* m_featuresToolBox;
        
        static AString s_previousOpenFileNameFilter;
        static AString s_previousOpenFileDirectory;
        static QByteArray s_previousOpenFileGeometry;
        
        WindowComponentStatus m_defaultWindowComponentStatus;
        WindowComponentStatus m_normalWindowComponentStatus;
        
        static bool s_firstWindowFlag;
        
        friend class BrainBrowserWindowToolBar;
        friend class GuiManager;
        
        SceneClassAssistant* m_sceneAssistant;
        
        /** X position from scene file for first window */
        static int32_t s_sceneFileFirstWindowX;

        /** Y position from scene file for first window */
        static int32_t s_sceneFileFirstWindowY;
        
        static std::set<BrainBrowserWindow*> s_brainBrowserWindows;
        
        QString m_objectNamePrefix;
        
        bool m_keyEventProcessingFlag = false;
        
        static bool s_enableMacDuplicateMenuBarFlag;
        
    };
#ifdef __BRAIN_BROWSER_WINDOW_DECLARE__
    std::set<BrainBrowserWindow*> BrainBrowserWindow::s_brainBrowserWindows;
    AString BrainBrowserWindow::s_previousOpenFileNameFilter;
    AString BrainBrowserWindow::s_previousOpenFileDirectory;
    QByteArray BrainBrowserWindow::s_previousOpenFileGeometry;
    
    bool BrainBrowserWindow::s_firstWindowFlag = true;
    int32_t BrainBrowserWindow::s_sceneFileFirstWindowX = -1;
    int32_t BrainBrowserWindow::s_sceneFileFirstWindowY = -1;

    bool BrainBrowserWindow::s_enableMacDuplicateMenuBarFlag = false;
#endif // __BRAIN_BROWSER_WINDOW_DECLARE__
    
}

#endif // __BRAIN_BROWSER_WINDOW_H__

