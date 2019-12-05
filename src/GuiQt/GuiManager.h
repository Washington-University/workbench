

#ifndef __GUI_MANAGER_H__
#define __GUI_MANAGER_H__

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
#include <vector>
#include <stdint.h>

#include <QObject>

#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QAction;
class QDialog;
class QMenu;
class QWidget;
class MovieDialog;
class WuQWebView;

namespace caret {    
    class Brain;
    class BrainBrowserWindow;
    class BrowserTabContent;
    class BugReportDialog;
    class ChartTwoLineSeriesHistoryDialog;
    class CursorManager;
    class CustomViewDialog;
    class GapsAndMarginsDialog;
    class HelpViewerDialog;
    class IdentifyBrainordinateDialog;
    class IdentificationDisplayDialog;
    class ImageFile;
    class ImageCaptureDialog;
    class InformationDisplayDialog;
    class MovieRecordingDialog;
    class OverlaySettingsEditorDialog;
    class Model;
    class PaletteColorMappingEditorDialog;
    class PreferencesDialog;
    class Scene;
    class SceneDialog;
    class SceneFile;
    class SelectionManager;
    class SpecFile;
    class SurfacePropertiesEditorDialog;
    class VolumePropertiesEditorDialog;
    class TileTabsConfigurationDialog;
    
    /**
     * Manages the graphical user-interface.
     */
    class GuiManager : public QObject, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT
        
    public:
        static GuiManager* get();
        
        static void createGuiManager();
        
        static void deleteGuiManager();
        
        static void beep();
        
        static void updateUserInterface();
        
        static void updateGraphicsAllWindows();
        
        static void updateGraphicsOneWindow(const int32_t windowIndex);
        
        static void updateSurfaceColoring();
        
        Brain* getBrain() const;
        
        int32_t getNumberOfOpenBrainBrowserWindows() const;
        
        BrainBrowserWindow* getActiveBrowserWindow() const;
        
        std::vector<BrainBrowserWindow*> getAllOpenBrainBrowserWindows() const;
        
        std::vector<int32_t> getAllOpenBrainBrowserWindowIndices() const;
        
        BrainBrowserWindow* getBrowserWindowByWindowIndex(const int32_t browserWindowIndex);
        
        bool allowBrainBrowserWindowToClose(BrainBrowserWindow* bbw,
                                            const int32_t numberOfOpenTabs);
        
        bool exitProgram(BrainBrowserWindow* parent);
        
        bool processShowOpenSpecFileDialog(SpecFile* specFile,
                                           BrainBrowserWindow* browserWindow);
        
        void processShowSaveManageFilesDialog(BrainBrowserWindow* browserWindow);
        
        
        BrowserTabContent* getBrowserTabContentForBrowserWindow(const int32_t browserWindowIndex,
                                                                const bool allowInvalidBrowserWindowIndex);
        
        Model* getModelInBrowserWindow(const int32_t browserWindowIndex);
        
        void receiveEvent(Event* event);

        const CursorManager* getCursorManager() const;
        
        QAction* getInformationDisplayDialogEnabledAction();
        
        QAction* getIdentifyBrainordinateDialogDisplayAction();
        
        QAction* getSceneDialogDisplayAction();
        
        QAction* getHelpViewerDialogDisplayAction();
        
        QAction* getDataToolTipsAction(QWidget* buttonWidget);
        
        void closeAllOtherWindows(BrainBrowserWindow* browserWindow);
        
        void closeOtherWindowsAndReturnTheirTabContent(BrainBrowserWindow* browserWindow,
                                                       std::vector<BrowserTabContent*>& tabContents);
        
        void processShowBugReportDialog(BrainBrowserWindow* browserWindow,
                                        const AString& openGLInformation);
        void processShowCustomViewDialog(BrainBrowserWindow* browserWindow);
        void processShowGapsAndMarginsDialog(BrainBrowserWindow* browserWindow);
        void processShowImageCaptureDialog(BrainBrowserWindow* browserWindow);
        void processShowMovieRecordingDialog(BrainBrowserWindow* browserWindow);
        void processShowMovieDialog(BrainBrowserWindow* browserWindow);
        void processShowPreferencesDialog(BrainBrowserWindow* browserWindow);
        void processShowInformationDisplayDialog(const bool forceDisplayOfDialog);
        void processShowTileTabsConfigurationDialog(BrainBrowserWindow* browserWindow);
        void showHideIdentfyBrainordinateDialog(const bool status,
                                                BrainBrowserWindow* parentBrainBrowserWindow);
        
        void processShowSceneDialog(BrainBrowserWindow* browserWindow);
        
        void processShowSurfacePropertiesEditorDialog(BrainBrowserWindow* browserWindow);
        
        void processShowVolumePropertiesEditorDialog(BrainBrowserWindow* browserWindow);
        
        void processShowSceneDialogAndScene(BrainBrowserWindow* browserWindow,
                                            SceneFile* sceneFile,
                                            Scene* scene,
                                            const bool showSceneDialogFlag);
        
        void processShowAllenDataBaseWebView(BrainBrowserWindow* browserWindow);
        void processShowConnectomeDataBaseWebView(BrainBrowserWindow* browserWindow);
        
        void updateAnimationStartTime(double value);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        AString getNameOfDataFileToOpenAfterStartup() const;

        void processIdentification(const int32_t tabIndex,
                                   SelectionManager* selectionManager,
                                   QWidget* parentWidget);
        
        /*
         * Mode used when testing for modified files
         */
        enum TestModifiedMode {
            /** Testing when user is exiting Workbench */
            TEST_FOR_MODIFIED_FILES_MODE_FOR_EXIT,
            /** Testing when a scene is added for modified but not palette modified */
            TEST_FOR_MODIFIED_FILES_EXCLUDING_PALETTES_MODE_FOR_SCENE_ADD,
            /** Testing when a scene is added for modified palettes only */
            TEST_FOR_MODIFIED_FILES_PALETTE_ONLY_MODE_FOR_SCENE_ADD,
            /** Testing when a scene is shown */
            TEST_FOR_MODIFIED_FILES_MODE_FOR_SCENE_SHOW
        };
        
        bool testForModifiedFiles(const TestModifiedMode testModifiedMode,
                                  AString& textMesssageOut,
                                  AString& modifiedFilesMessageOut) const;
        
    public slots:
        void processBringAllWindowsToFront();
        void processShowInformationWindow();
        void processTileWindows();
        
        void showHideInfoWindowSelected(bool);
        
        void showIdentifyBrainordinateDialogActionToggled(bool);
        
        void sceneDialogDisplayActionTriggered(bool);
        
        void showHelpDialogActionToggled(bool);
        
    private slots:
        void helpDialogWasClosed();
        void sceneDialogWasClosed();
        void identifyBrainordinateDialogWasClosed();
        void dataToolTipsActionTriggered(bool);
        
    private:
        GuiManager(QObject* parent = 0);
        
        virtual ~GuiManager();
        
        
        GuiManager(const GuiManager&);
        GuiManager& operator=(const GuiManager&);
        
        void initializeGuiManager();
        
        BrainBrowserWindow* newBrainBrowserWindow(QWidget* parent,
                                                  const int32_t useWindowIndex,
                                                  BrowserTabContent* browserTabContent,
                                                  const bool createDefaultTabs,
                                                  AString& errorMessageOut);
        
        void reparentNonModalDialogs(BrainBrowserWindow* closingBrainBrowserWindow);
        
        void showHideSceneDialog(const bool status,
                                 BrainBrowserWindow* parentBrainBrowserWindow);
        
        void showHideHelpDialog(const bool status,
                                const BrainBrowserWindow* parentBrainBrowserWindow);
        
        void addNonModalDialog(QWidget* dialog);
        
        void updateNonModalDialogs();
        
        void addParentLessNonModalDialog(QWidget* dialog);
        
        QPixmap createDataToolTipsIcon(const QWidget* widget);
        
        /** One instance of the GuiManager */
        static GuiManager* singletonGuiManager;
        
        /** Contains pointers to Brain Browser windows */
        std::vector<BrainBrowserWindow*> m_brainBrowserWindows;
        
        /** Skips confirmation of browser window closing when all tabs are moved to one window */
        bool allowBrowserWindowsToCloseWithoutConfirmation;
        
        /* Editor for overlay settings. */
        std::set<OverlaySettingsEditorDialog*> m_overlaySettingsEditors;
        
        /** Editor for palette color mapping editing */
        PaletteColorMappingEditorDialog* m_paletteColorMappingEditor;
        
        ChartTwoLineSeriesHistoryDialog* m_chartTwoLineSeriesHistoryDialog;
        
        TileTabsConfigurationDialog* m_tileTabsConfigurationDialog;
        
        CustomViewDialog* m_customViewDialog;
        
        ImageCaptureDialog* imageCaptureDialog;

        MovieRecordingDialog* m_movieRecordingDialog;
        
        GapsAndMarginsDialog* m_gapsAndMarginsDialog;
        
        MovieDialog* movieDialog;
        
        PreferencesDialog* preferencesDialog;       
        
        InformationDisplayDialog* m_informationDisplayDialog;
        
        IdentifyBrainordinateDialog* m_identifyBrainordinateDialog;
        
        IdentificationDisplayDialog* m_identificationDisplayDialog = NULL;
        
        SceneDialog* sceneDialog;
        
        QAction* m_sceneDialogDisplayAction;
        
        SurfacePropertiesEditorDialog* m_surfacePropertiesEditorDialog;
        
        VolumePropertiesEditorDialog* m_volumePropertiesEditorDialog;
        
        WuQWebView* connectomeDatabaseWebView;
        
        CursorManager* cursorManager;
        
        QAction* m_informationDisplayDialogEnabledAction;
        
        QAction* m_identifyBrainordinateDialogEnabledAction;
        
        QAction* m_dataToolTipsEnabledAction;
        
        BugReportDialog* m_bugReportDialog;
        
        QAction* m_helpViewerDialogDisplayAction;
        
        HelpViewerDialog* m_helpViewerDialog;
        
        /** 
         * Tracks non-modal dialogs that are created only one time
         * and may need to be reparented if the original parent, a
         * BrainBrowserWindow is closed in which case the dialog
         * is reparented to a different BrainBrowserWindow.
         *
         * On Linux and Windows, behavior is that when a dialog becomes active,
         * its parent window is brought forward to be directly behind the 
         * dialog.  When there are multiple browser windows, this may cause
         * the parent browser window to be brought forward and obscure
         * a browser window (not a parent of the dialog) that the user
         * was viewing.  In this case, it may be desirable to make 
         * the dialog parent-less.
         */
        std::set<QWidget*> nonModalDialogs;
        
        /**
         * Tracks non-modal dialogs WITHOUT parents.  Dialogs without parents
         * are undesirable on Mac because when the parent-less dialog is
         * the active window, the menu bar will disappear.  See the
         * comment for 'nonModalDialogs' for more information.
         */
        std::set<QWidget*> m_parentlessNonModalDialogs;
        
        /**
         * If Workbench is started by double-clicking a data file in
         * the Mac OSX Finder, this will contain the name of the data
         * file.  When the event is received, Workbench has not yet
         * created windows.  After creating the first Browser Window,
         * the values of this string is requested, and if valid,
         * the data file is opened.
         */
        AString m_nameOfDataFileToOpenAfterStartup;
    };
    
#ifdef __GUI_MANAGER_DEFINE__
    GuiManager* GuiManager::singletonGuiManager = NULL;
#endif // __GUI_MANAGER_DEFINE__
}

#endif // __GUI_MANAGER_H__
