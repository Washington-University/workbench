#ifndef __BRAIN_BROWSER_WINDOW_TOOLBAR_H__
#define __BRAIN_BROWSER_WINDOW_TOOLBAR_H__

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

#include <stdint.h>

#include <QStack>
#include <QToolBar>

#include "BrainBrowserWindowToolBarOrientation.h"
#include "EnumComboBoxTemplate.h"
#include "EventListenerInterface.h"
#include "ModelTypeEnum.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

class QAbstractButton;
class QAction;
class QActionGroup;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QHBoxLayout;
class QIcon;
class QLabel;
class QMainWindow;
class QMenu;
class QRadioButton;
class QSpinBox;
class QToolButton;
class QVBoxLayout;

namespace caret {
    
    class BrainBrowserWindowToolBarAllSurface;
    class BrainBrowserWindowToolBarChartAxes;
    class BrainBrowserWindowToolBarChartAttributes;
    class BrainBrowserWindowToolBarChartTwoAttributes;
    class BrainBrowserWindowToolBarChartTwoOrientation;
    class BrainBrowserWindowToolBarChartTwoOrientedAxes;
    class BrainBrowserWindowToolBarChartTwoType;
    class BrainBrowserWindowToolBarChartType;
    class BrainBrowserWindowToolBarHistology;
    class BrainBrowserWindowToolBarImage;
    class BrainBrowserWindowToolBarSlicePlane;
    class BrainBrowserWindowToolBarSliceSelection;
    class BrainBrowserWindowToolBarSurface;
    class BrainBrowserWindowToolBarSurfaceMontage;
    class BrainBrowserWindowToolBarTab;
    class BrainBrowserWindowToolBarView;
    class BrainBrowserWindowToolBarVolumeMontage;
    class BrainBrowserWindowToolBarVolumeMPR;
    class BrainBrowserWindow;
    class BrowserTabContent;
    class EventBrowserTabReopenClosed;
    class EventBrowserWindowTileTabOperation;
    class Model;
    class ModelSurface;
    class ModelVolumeInterface;
    class SceneAttributes;
    class SceneClass;
    class Surface;
    class SurfaceSelectionViewController;
    class StructureSurfaceSelectionControl;
    class UserInputModeAnnotations;
    class UserInputModeBorders;
    class UserInputModeFoci;
    class UserInputModeImage;
    class UserInputModeTileTabsLayout;
    class UserInputModeView;
    class UserInputModeVolumeEdit;
    class UserInputModeAbstract;
    class WuQTabBar;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBar : public QToolBar, public EventListenerInterface, public SceneableInterface   {
      
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBar(const int32_t browserWindowIndex,
                                  BrowserTabContent* initialBrowserTabContent,
                                  QAction* overlayToolBoxAction,
                                  QAction* layersToolBoxAction,
                                  QToolButton* toolBarLockWindowAndAllTabAspectRatioButton,
                                  const QString& objectNamePrefix,
                                  BrainBrowserWindow* parentBrainBrowserWindow);
        
        ~BrainBrowserWindowToolBar();
        
        BrowserTabContent* addNewTab();
        
        void reopenLastClosedTab(EventBrowserTabReopenClosed& reopenTabEvent);
        
        void addNewDuplicatedTab(BrowserTabContent* browserTabContentToBeCloned);
        
        void addNewTabWithContent(BrowserTabContent* browserTabContent);
        
        void addDefaultTabsAfterLoadingSpecFile();
        
        void receiveEvent(Event* event);
        
        int32_t getNumberOfTabs() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        enum WidgetPlacement {
            WIDGET_PLACEMENT_NONE,
            WIDGET_PLACEMENT_BOTTOM,
            WIDGET_PLACEMENT_LEFT,
            WIDGET_PLACEMENT_RIGHT,
            WIDGET_PLACEMENT_TOP
        };
        
        static QWidget* createToolWidget(const QString& name,
                                         QWidget* childWidget,
                                         const WidgetPlacement verticalBarPlacement,
                                         const WidgetPlacement contentPlacement,
                                         const int horizontalStretching);
        
    signals:
        void viewedModelChanged();
        
    private:        
        
        BrainBrowserWindowToolBar(const BrainBrowserWindowToolBar&);
        BrainBrowserWindowToolBar& operator=(const BrainBrowserWindowToolBar&);
        
        BrowserTabContent* getTabContentFromSelectedTab();
        BrowserTabContent* getTabContentFromTab(const int tabIndex);
        
        int32_t getTabBarIndexWithBrowserTabIndex(const int32_t browserTabIndex);
        
        Model* getDisplayedModel();
        
        int32_t loadIntoTab(const int32_t tabIndex,
                            Model* controller);

        void updateGraphicsWindow();
        void updateGraphicsWindowAndYokedWindows();
        void updateUserInterface();
        void updateToolBox();
        void updateAllTabNames();
        void updateTabName(const int32_t tabIndex);
        void emitViewModelChangedSignal(); /* for use by friend classes */
        
        QWidget* createViewWidget();
        QWidget* createOrientationWidget();
        QWidget* createWholeBrainSurfaceOptionsWidget();
        QWidget* createVolumeIndicesWidget();
        QWidget* createModeWidget();
        QWidget* createTabOptionsWidget(QToolButton* toolBarLockWindowAndAllTabAspectRatioButton);
        QWidget* createChartAxesWidget();
        QWidget* createChartAttributesWidget();
        QWidget* createChartTwoOrientationWidget();
        QWidget* createChartTwoAttributesWidget();
        QWidget* createChartTwoOrientedAxisWidget();
        QWidget* createChartTypeWidget();
        QWidget* createChartTypeTwoWidget();
        QWidget* createHistologyWidget();
        QWidget* createImageWidget();
        QWidget* createSingleSurfaceOptionsWidget();
        QWidget* createSurfaceMontageOptionsWidget();
        QWidget* createVolumeMontageWidget();
        QWidget* createVolumeMprWidget();
        QWidget* createVolumePlaneWidget();
        
        ModelTypeEnum::Enum updateViewWidget(BrowserTabContent* browserTabContent);
        void updateOrientationWidget(BrowserTabContent* browserTabContent);
        void updateWholeBrainSurfaceOptionsWidget(BrowserTabContent* browserTabContent);
        void updateVolumeIndicesWidget(BrowserTabContent* browserTabContent);
        void updateModeWidget(BrowserTabContent* browserTabContent);
        void updateTabOptionsWidget(BrowserTabContent* browserTabContent);
        void updateSingleSurfaceOptionsWidget(BrowserTabContent* browserTabContent);
        void updateSurfaceMontageOptionsWidget(BrowserTabContent* browserTabContent);
        void updateChartAxesWidget(BrowserTabContent* browserTabContent);
        void updateChartAttributesWidget(BrowserTabContent* browserTabContent);
        void updateChartTwoAttributesWidget(BrowserTabContent* browserTabContent);
        void updateChartTwoOrientedAxesWidget(BrowserTabContent* browserTabContent);
        void updateChartTwoOrientationWidget(BrowserTabContent* browserTabContent);
        void updateChartTypeWidget(BrowserTabContent* browserTabContent);
        void updateChartTypeTwoWidget(BrowserTabContent* browserTabContent);
        void updateHistologyWidget(BrowserTabContent* browserTabContent);
        void updateImageWidget(BrowserTabContent* browserTabContent);
        void updateVolumeMontageWidget(BrowserTabContent* browserTabContent);
        void updateVolumeMprWidget(BrowserTabContent* browserTabConent);
        void updateVolumePlaneWidget(BrowserTabContent* browserTabContent);
        
        QWidget* viewWidget;
        QWidget* orientationWidget;
        QWidget* wholeBrainSurfaceOptionsWidget;
        QWidget* volumeIndicesWidget;
        QWidget* modeWidget;
        QWidget* tabMiscWidget;
        QWidget* singleSurfaceSelectionWidget;
        QWidget* surfaceMontageSelectionWidget;
        QWidget* volumeMontageWidget;
        QWidget* volumeMprWidget;
        QWidget* volumePlaneWidget;
        QWidget* chartTypeWidget;
        QWidget* chartTypeTwoWidget;
        QWidget* chartAxesWidget;
        QWidget* chartAttributesWidget;
        QWidget* chartTwoOrientationWidget;
        QWidget* chartTwoAttributesWidget;
        QWidget* chartTwoOrientedAxesWidget;
        QWidget* histologyWidget;
        QWidget* imageWidget;

        UserInputModeAbstract* selectedUserInputProcessor = NULL;
        UserInputModeAnnotations* userInputAnnotationsModeProcessor;
        UserInputModeView* userInputViewModeProcessor;
        UserInputModeBorders* userInputBordersModeProcessor;
        UserInputModeFoci* userInputFociModeProcessor;
        UserInputModeImage* userInputImageModeProcessor;
        UserInputModeTileTabsLayout* userInputTileTabsManualLayoutProcessor;
        UserInputModeVolumeEdit* userInputVolumeEditModeProcessor;
        
        /* DO NOT delete these as the corresponding processor is the parent */
        QWidget* annotateModeWidget;
        QWidget* bordersModeWidget;
        QWidget* fociModeWidget;
        QWidget* imageModeWidget;
        QWidget* tileModeWidget;
        QWidget* volumeModeWidget;
        
        WuQWidgetObjectGroup* modeWidgetGroup;
        
        QVBoxLayout* m_toolBarMainLayout;
        
        QWidget* fullToolBarWidget;
        QWidget* m_toolbarWidget;
        QHBoxLayout* toolbarWidgetLayout;
        
        QWidget* tabBarWidget;
        WuQTabBar* tabBar;
        
        void removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent);
        
        void getAllTabContent(std::vector<BrowserTabContent*>& allTabContent) const;
        
        void removeTabWithContent(BrowserTabContent* browserTabContent);
        
        void processTileTabOperationEvent(EventBrowserWindowTileTabOperation* tileTabsEvent);
        
    public slots:
        void closeSelectedTabFromFileMenu();

        void moveTabsToNewWindows();
        
        void nextTab();
        
        void previousTab();
        
        void renameTab();
        
        void updateToolBar();
        
        void updateToolBarComponents(BrowserTabContent* browserTabContent);
        
        void showHideToolBar(bool showIt);
        
        void showMacroDialog();
        
    private slots:
        void selectedTabChanged(int indx);
        void tabMoved(int, int);
        void tabCloseSelected(int);
        void showTabMenu(const QPoint& pos);
        void tabBarMousePressedSlot(QMouseEvent* event);
        void tabBarMouseReleasedSlot(QMouseEvent* event);
        
    private:
        enum class InsertTabMode {
            APPEND,
            AT_TAB_BAR_INDEX
        };
        
        /**
         * Remove tab mode
         */
        enum class RemoveTabMode {
            /** Close tab but allow it to be reopened later */
            CLOSE_TAB_CONTENT_FOR_REOPENING,
            /** Delete the tab, CANNOT be reopened */
            DELETE_TAB_CONTENT,
            /** Ignore tab content, caller may delete it or transfer it (content attached to QTab may be NULL too) */
            INGORE_TAB_CONTENT
        };
        
        bool allowAddingNewTab();
        
        void insertTabContentPrivate(const InsertTabMode insertTabMode,
                                     BrowserTabContent* browserTabContent,
                                     const int32_t tabBarIndex);

        void removeTab(int index,
                       const RemoveTabMode removeTabMode);
        
        void tabClosed(int index,
                       const RemoveTabMode removeTabMode);
        
        BrowserTabContent* insertNewTabAtTabBarIndex(int32_t tabBarIndex);
        void insertAndCloneTabContentAtTabBarIndex(const BrowserTabContent* tabContentToBeCloned,
                                                 const int32_t tabBarIndex);
        void replaceBrowserTabs(const std::vector<BrowserTabContent*>& browserTabs);
        
        BrowserTabContent* createNewTab(AString& errorMessage);
        
        QAction* customViewAction;
        
        QMenu* customViewMenu;

    private slots:
        void customViewActionTriggered();
        
        void customViewMenuAboutToShow();
        
        void customViewMenuTriggered(QAction* action);
        
        void sceneToolButtonClicked();
        
    private:
        
        QToolButton* m_movieToolButton = NULL;
        
    private:
        BrainBrowserWindowToolBarAllSurface* m_allSurfaceToolBarComponent;
        BrainBrowserWindowToolBarChartAxes* m_chartAxisToolBarComponent;
        BrainBrowserWindowToolBarChartTwoType* m_chartTwoTypeToolBarComponent;
        BrainBrowserWindowToolBarChartType* m_chartTypeToolBarComponent;
        BrainBrowserWindowToolBarChartAttributes* m_chartAttributesToolBarComponent;
        BrainBrowserWindowToolBarChartTwoOrientedAxes* m_chartTwoOrientedAxesToolBarComponent;
        BrainBrowserWindowToolBarChartTwoOrientation* m_chartTwoOrientationToolBarComponent;
        BrainBrowserWindowToolBarChartTwoAttributes* m_chartTwoAttributesToolBarComponent;
        BrainBrowserWindowToolBarHistology* m_histologyToolBarComponent;
        BrainBrowserWindowToolBarImage* m_imageToolBarComponent;
        BrainBrowserWindowToolBarOrientation* m_orientationToolBarComponent;
        BrainBrowserWindowToolBarSurface* m_surfaceToolBarComponent;
        BrainBrowserWindowToolBarSurfaceMontage* m_surfaceMontageToolBarComponent;
        BrainBrowserWindowToolBarView* m_viewToolBarComponent;
        BrainBrowserWindowToolBarSlicePlane* m_slicePlaneComponent;
        BrainBrowserWindowToolBarSliceSelection* m_sliceSelectionComponent;
        BrainBrowserWindowToolBarVolumeMontage* m_volumeMontageComponent;
        BrainBrowserWindowToolBarVolumeMPR* m_volumeMprComponent;
        BrainBrowserWindowToolBarTab* m_tabOptionsComponent;
        
    private slots:
        void modeInputModeRadioButtonClicked(QAbstractButton*);
        
    private:
        void updateDisplayedModeUserInputWidget();
        QButtonGroup* modeInputModeRadioButtonGroup;
        QRadioButton* modeInputModeAnnotationsRadioButton;
        QRadioButton* modeInputModeBordersRadioButton;
        QRadioButton* modeInputModeFociRadioButton;
        QRadioButton* modeInputModeImageRadioButton;
        QRadioButton* modeInputModeViewRadioButton;
        QRadioButton* modeInputModeTileTabsManualLayoutRadioButton;
        QRadioButton* modeInputVolumeEditRadioButton;
        
    private:
        QAction* toolBarToolButtonAction;
        QAction* toolBoxToolButtonAction;
    
        BrainBrowserWindow* m_parentBrainBrowserWindow = NULL;
        
        int32_t browserWindowIndex;
        
    private slots:
        void resetTabIndexForTileTabsHighlighting();
        
    private:
        friend class BrainBrowserWindow;
        friend class BrainBrowserWindowToolBarAllSurface;
        friend class BrainBrowserWindowToolBarChartAxes;
        friend class BrainBrowserWindowToolBarChartTwoOrientation;
        friend class BrainBrowserWindowToolBarChartTwoAttributes;
        friend class BrainBrowserWindowToolBarChartTwoType;
        friend class BrainBrowserWindowToolBarChartType;
        friend class BrainBrowserWindowToolBarComponent;
        friend class BrainBrowserWindowToolBarHistology;
        friend class BrainBrowserWindowToolBarImage;
        friend class BrainBrowserWindowToolBarMode;
        friend class BrainBrowserWindowToolBarOrientation;
        friend class BrainBrowserWindowToolBarSurfaceMontage;
        friend class BrainBrowserWindowToolBarSlicePlane;
        friend class BrainBrowserWindowToolBarSliceSelection;
        friend class BrainBrowserWindowToolBarSurface;
        friend class BrainBrowserWindowToolBarTab;
        friend class BrainBrowserWindowToolBarView;
        friend class BrainBrowserWindowToolBarVolumeMontage;
        
        /**
         * When a tab is selected in Tile Tabs viewing,
         * the graphics window content of the tab is 
         * highlighted for a short time by drawing a
         * box around it.
         */
        int32_t m_tabIndexForTileTabsHighlighting;
        QTimer* m_tileTabsHighlightingTimer = NULL;
        bool m_tileTabsHighlightingTimerEnabledFlag = true;
        
        QString m_objectNamePrefix;
        
        bool isContructorFinished;
        bool isDestructionInProgress;
        
        /**
         * Tracks when update is performed to catch incorrectly emitted signals.
         */
        bool m_performingUpdateFlag = false;
        
        const int32_t TAB_INDEX_APPEND_TO_TOOLBAR = 10000;
        
        friend class BrainBrowserWindowToolBarTabPopUpMenu;
    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOLBAR_DECLARE__
#endif // __BRAIN_BROWSER_WINDOW_TOOLBAR_DECLARE__
}

#endif // __BRAIN_BROWSER_WINDOW_TOOLBAR_H__

