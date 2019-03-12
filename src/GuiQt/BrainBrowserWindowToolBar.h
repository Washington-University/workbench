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
class QMenu;
class QRadioButton;
class QSpinBox;
class QToolButton;

namespace caret {
    
    class BrainBrowserWindowToolBarChartAxes;
    class BrainBrowserWindowToolBarChartAttributes;
    class BrainBrowserWindowToolBarChartTwoAttributes;
    class BrainBrowserWindowToolBarChartTwoAxes;
    class BrainBrowserWindowToolBarChartTwoOrientation;
    class BrainBrowserWindowToolBarChartTwoTitle;
    class BrainBrowserWindowToolBarChartTwoType;
    class BrainBrowserWindowToolBarChartType;
    class BrainBrowserWindowToolBarClipping;
    class BrainBrowserWindowToolBarSlicePlane;
    class BrainBrowserWindowToolBarSliceSelection;
    class BrainBrowserWindowToolBarSurfaceMontage;
    class BrainBrowserWindowToolBarTab;
    class BrainBrowserWindowToolBarVolumeMontage;
    class BrainBrowserWindow;
    class BrowserTabContent;
    class Model;
    class ModelSurface;
    class ModelVolumeInterface;
    class SceneAttributes;
    class SceneClass;
    class Surface;
    class SurfaceSelectionViewController;
    class StructureSurfaceSelectionControl;
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
        
        void addNewTab();
        
        void addNewDuplicatedTab(BrowserTabContent* browserTabContentToBeCloned);
        
        void addNewTabWithContent(BrowserTabContent* browserTabContent);
        
        void addDefaultTabsAfterLoadingSpecFile();
        
        void receiveEvent(Event* event);
        
        int32_t getNumberOfTabs() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    signals:
        void viewedModelChanged();
        
    private:        
        enum WidgetPlacement {
            WIDGET_PLACEMENT_NONE,
            WIDGET_PLACEMENT_BOTTOM,
            WIDGET_PLACEMENT_LEFT,
            WIDGET_PLACEMENT_RIGHT,
            WIDGET_PLACEMENT_TOP
        };
        
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
        QWidget* createChartTwoAxesWidget();
        QWidget* createChartTwoTitleWidget();
        QWidget* createChartTypeWidget();
        QWidget* createChartTypeTwoWidget();
        QWidget* createSingleSurfaceOptionsWidget();
        QWidget* createSurfaceMontageOptionsWidget();
        QWidget* createClippingOptionsWidget();
        QWidget* createVolumeMontageWidget();
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
        void updateChartTwoAxesWidget(BrowserTabContent* browserTabContent);
        void updateChartTwoOrientationWidget(BrowserTabContent* browserTabContent);
        void updateChartTwoTitleWidget(BrowserTabContent* browserTabContent);
        void updateChartTypeWidget(BrowserTabContent* browserTabContent);
        void updateChartTypeTwoWidget(BrowserTabContent* browserTabContent);
        void updateVolumeMontageWidget(BrowserTabContent* browserTabContent);
        void updateVolumePlaneWidget(BrowserTabContent* browserTabContent);
        void updateClippingOptionsWidget(BrowserTabContent* browserTabContent);
        
        QWidget* createToolWidget(const QString& name,
                                  QWidget* childWidget,
                                  const WidgetPlacement verticalBarPlacement,
                                  const WidgetPlacement contentPlacement,
                                  const int horizontalStretching);
        
        QWidget* viewWidget;
        QWidget* orientationWidget;
        QWidget* wholeBrainSurfaceOptionsWidget;
        QWidget* volumeIndicesWidget;
        QWidget* modeWidget;
        QWidget* windowWidget;
        QWidget* singleSurfaceSelectionWidget;
        QWidget* surfaceMontageSelectionWidget;
        QWidget* m_clippingOptionsWidget;
        QWidget* volumeMontageWidget;
        QWidget* volumePlaneWidget;
        QWidget* chartTypeWidget;
        QWidget* chartTypeTwoWidget;
        QWidget* chartAxesWidget;
        QWidget* chartAttributesWidget;
        QWidget* chartTwoOrientationWidget;
        QWidget* chartTwoAttributesWidget;
        QWidget* chartTwoAxesWidget;
        QWidget* chartTwoTitleWidget;
        
        WuQWidgetObjectGroup* viewWidgetGroup;
        WuQWidgetObjectGroup* orientationWidgetGroup;
        WuQWidgetObjectGroup* wholeBrainSurfaceOptionsWidgetGroup;
        WuQWidgetObjectGroup* modeWidgetGroup;
        WuQWidgetObjectGroup* singleSurfaceSelectionWidgetGroup;
        
        QWidget* fullToolBarWidget;
        QWidget* m_toolbarWidget;
        QHBoxLayout* toolbarWidgetLayout;
        
        QWidget* tabBarWidget;
        WuQTabBar* tabBar;
        
        /** Widget displayed at bottom of toolbar for mouse input controls */
        QWidget* userInputControlsWidget;

        /** Layout for widget displayed at bottom of toolbar for mouse input controls */
        QHBoxLayout* userInputControlsWidgetLayout;

        /** Is set to the user input widget provided by the user input processor */
        QWidget* userInputControlsWidgetActiveInputWidget;
        
        void removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent);
        
        void getAllTabContent(std::vector<BrowserTabContent*>& allTabContent) const;
        
        void removeTabWithContent(BrowserTabContent* browserTabContent);
        
    public slots:
        void closeSelectedTab();

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
        void tabBarMousePressedSlot();
        void tabBarMouseReleasedSlot();
        
    private:
        enum class InsertTabMode {
            APPEND,
            AT_TAB_BAR_INDEX
        };
        
        bool allowAddingNewTab();
        
        void insertTabContentPrivate(const InsertTabMode insertTabMode,
                                     BrowserTabContent* browserTabContent,
                                     const int32_t tabBarIndex);
        
        void removeTab(int index);
        void tabClosed(int index);
        
        void insertNewTabAtTabBarIndex(int32_t tabBarIndex);
        void insertAndCloneTabContentAtTabBarIndex(const BrowserTabContent* tabContentToBeCloned,
                                                 const int32_t tabBarIndex);
        void replaceBrowserTabs(const std::vector<BrowserTabContent*>& browserTabs);
        
        BrowserTabContent* createNewTab(AString& errorMessage);
        
        QRadioButton* viewModeSurfaceRadioButton;
        QRadioButton* viewModeSurfaceMontageRadioButton;
        QRadioButton* viewModeVolumeRadioButton;
        QRadioButton* viewModeWholeBrainRadioButton;
        QRadioButton* viewModeChartOneRadioButton;
        QRadioButton* viewModeChartTwoRadioButton;
        
        QAction* customViewAction;

    private slots:
        void viewModeRadioButtonClicked(QAbstractButton*);
        
        void customViewActionTriggered();
        
        void sceneToolButtonMenuTriggered(QAction* action);
        
        void sceneToolButtonMenuAboutToShow();
        
        void sceneToolButtonClicked();
        
        void createMovieToolButtonMenuItemTriggered();
        
    private:
        QAction* orientationLateralMedialToolButtonAction;
        QAction* orientationDorsalVentralToolButtonAction;
        QAction* orientationAnteriorPosteriorToolButtonAction;
        
        QToolButton* orientationLateralMedialToolButton;
        QToolButton* orientationDorsalVentralToolButton;
        QToolButton* orientationAnteriorPosteriorToolButton;
        
        QAction* orientationLeftOrLateralToolButtonAction;
        QAction* orientationRightOrMedialToolButtonAction;
        QAction* orientationAnteriorToolButtonAction;
        QAction* orientationPosteriorToolButtonAction;
        QAction* orientationDorsalToolButtonAction;
        QAction* orientationVentralToolButtonAction;
        
        QToolButton* orientationLeftOrLateralToolButton;
        QToolButton* orientationRightOrMedialToolButton;
        QToolButton* orientationAnteriorToolButton;
        QToolButton* orientationPosteriorToolButton;
        QToolButton* orientationDorsalToolButton;
        QToolButton* orientationVentralToolButton;

        QAction* orientationResetToolButtonAction;
        QToolButton* orientationCustomViewSelectToolButton;
        
        QIcon* viewOrientationLeftIcon;
        QIcon* viewOrientationRightIcon;
        QIcon* viewOrientationAnteriorIcon;
        QIcon* viewOrientationPosteriorIcon;
        QIcon* viewOrientationDorsalIcon;
        QIcon* viewOrientationVentralIcon;
        QIcon* viewOrientationLeftLateralIcon;
        QIcon* viewOrientationLeftMedialIcon;
        QIcon* viewOrientationRightLateralIcon;
        QIcon* viewOrientationRightMedialIcon;
        
        QMenu* m_sceneToolButtonMenu = NULL;
        
        QToolButton* m_movieToolButton = NULL;
        
    private slots:
        void orientationLeftOrLateralToolButtonTriggered(bool checked);
        void orientationRightOrMedialToolButtonTriggered(bool checked);
        void orientationAnteriorToolButtonTriggered(bool checked);
        void orientationPosteriorToolButtonTriggered(bool checked);
        void orientationDorsalToolButtonTriggered(bool checked);
        void orientationVentralToolButtonTriggered(bool checked);
        void orientationResetToolButtonTriggered(bool checked);
        
        void orientationLateralMedialToolButtonTriggered(bool checked);
        void orientationDorsalVentralToolButtonTriggered(bool checked);
        void orientationAnteriorPosteriorToolButtonTriggered(bool checked);

    
    private:
        QComboBox*      wholeBrainSurfaceTypeComboBox;
        QCheckBox*      wholeBrainSurfaceLeftCheckBox;
        QCheckBox*      wholeBrainSurfaceRightCheckBox;
        QCheckBox*      wholeBrainSurfaceCerebellumCheckBox;
        QMenu*          wholeBrainSurfaceLeftMenu;
        QMenu*          wholeBrainSurfaceRightMenu;
        QMenu*          wholeBrainSurfaceCerebellumMenu;
        QDoubleSpinBox* wholeBrainSurfaceSeparationLeftRightSpinBox;
        QDoubleSpinBox* wholeBrainSurfaceSeparationCerebellumSpinBox;
        QCheckBox*      wholeBrainSurfaceMatchCheckBox;
        void updateAllWholeBrainSurfaceMenus();
        void updateWholeBrainSurfaceMenu(QMenu* menu,
                                         const StructureEnum::Enum structure);

    private slots:
        void wholeBrainSurfaceTypeComboBoxIndexChanged(int indx);
        void wholeBrainSurfaceLeftCheckBoxStateChanged(int state);
        void wholeBrainSurfaceRightCheckBoxStateChanged(int state);
        void wholeBrainSurfaceCerebellumCheckBoxStateChanged(int state);
        void wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double d);
        void wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double d);
        void wholeBrainSurfaceLeftToolButtonTriggered(bool checked);
        void wholeBrainSurfaceRightToolButtonTriggered(bool checked);
        void wholeBrainSurfaceCerebellumToolButtonTriggered(bool checked);
        void wholeBrainSurfaceMatchCheckBoxClicked(bool checked);
        
        void wholeBrainSurfaceLeftMenuTriggered(QAction*);
        void wholeBrainSurfaceRightMenuTriggered(QAction*);
        void wholeBrainSurfaceCerebellumMenuTriggered(QAction*);

    private:
        StructureSurfaceSelectionControl* surfaceSurfaceSelectionControl;
        
    private slots:
        void surfaceSelectionControlChanged(const StructureEnum::Enum,
                                            ModelSurface*);
        
    private:
        BrainBrowserWindowToolBarChartAxes* m_chartAxisToolBarComponent;
        BrainBrowserWindowToolBarChartTwoType* m_chartTwoTypeToolBarComponent;
        BrainBrowserWindowToolBarChartType* m_chartTypeToolBarComponent;
        BrainBrowserWindowToolBarChartAttributes* m_chartAttributesToolBarComponent;
        BrainBrowserWindowToolBarChartTwoAxes* m_chartTwoAxesToolBarComponent;
        BrainBrowserWindowToolBarChartTwoOrientation* m_chartTwoOrientationToolBarComponent;
        BrainBrowserWindowToolBarChartTwoAttributes* m_chartTwoAttributesToolBarComponent;
        BrainBrowserWindowToolBarChartTwoTitle* m_chartTwoTitleToolBarComponent;

        BrainBrowserWindowToolBarSurfaceMontage* m_surfaceMontageToolBarComponent;
        
        BrainBrowserWindowToolBarClipping* m_clippingToolBarComponent;
        BrainBrowserWindowToolBarSlicePlane* m_slicePlaneComponent;
        BrainBrowserWindowToolBarSliceSelection* m_sliceSelectionComponent;
        BrainBrowserWindowToolBarVolumeMontage* m_volumeMontageComponent;
        BrainBrowserWindowToolBarTab* m_tabOptionsComponent;
        
    private slots:
        void modeInputModeActionTriggered(QAction*);
        
    private:
        void updateDisplayedModeUserInputWidget();
        QActionGroup* modeInputModeActionGroup;
        QAction* modeInputModeAnnotationsAction;
        QAction* modeInputModeBordersAction;
        QAction* modeInputModeFociAction;
        QAction* modeInputModeImageAction;
        QAction* modeInputModeViewAction;
        QAction* modeInputVolumeEditAction;
        
    private:
        QAction* toolBarToolButtonAction;
        QAction* toolBoxToolButtonAction;
    
        int32_t browserWindowIndex;
        
    private slots:
        void resetTabIndexForTileTabsHighlighting();
        
    private:
        friend class BrainBrowserWindow;
        friend class BrainBrowserWindowToolBarChartAxes;
        friend class BrainBrowserWindowToolBarChartTwoAxes;
        friend class BrainBrowserWindowToolBarChartTwoOrientation;
        friend class BrainBrowserWindowToolBarChartTwoAttributes;

        friend class BrainBrowserWindowToolBarChartTwoType;
        friend class BrainBrowserWindowToolBarChartType;
        friend class BrainBrowserWindowToolBarClipping;
        friend class BrainBrowserWindowToolBarComponent;
        friend class BrainBrowserWindowToolBarSurfaceMontage;
        friend class BrainBrowserWindowToolBarSlicePlane;
        friend class BrainBrowserWindowToolBarSliceSelection;
        friend class BrainBrowserWindowToolBarTab;
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
}

#endif // __BRAIN_BROWSER_WINDOW_TOOLBAR_H__

