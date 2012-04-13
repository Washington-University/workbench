#ifndef __BRAIN_BROWSER_WINDOW_TOOLBAR_H__
#define __BRAIN_BROWSER_WINDOW_TOOLBAR_H__

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

#include <QStack>
#include <QToolBar>

#include "EventListenerInterface.h"
#include "ModelTypeEnum.h"
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
class QTabBar;
class QToolButton;

namespace caret {
    
    class BrainBrowserWindow;
    //class BrainBrowserWindowToolBox;
    class BrowserTabContent;
    class Model;
    class ModelSurface;
    class Surface;
    class SurfaceSelectionViewController;
    class StructureSurfaceSelectionControl;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBar : public QToolBar, public EventListenerInterface   {
      
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBar(const int32_t browserWindowIndex,
                                  BrowserTabContent* initialBrowserTabContent,
                                  QAction* toolBoxToggleAction,
                                  BrainBrowserWindow* parentBrainBrowserWindow);
        
        ~BrainBrowserWindowToolBar();
        
        QAction* getShowToolBoxAction();
        
        void addDefaultTabsAfterLoadingSpecFile();
        
        void receiveEvent(Event* event);
        
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
        
        Model* getDisplayedModelController();
        
        int32_t loadIntoTab(const int32_t tabIndex,
                            Model* controller);

        void updateGraphicsWindow();
        void updateUserInterface();
        void updateToolBox();
        void updateTabName(const int32_t tabIndex);
        
        QWidget* createViewWidget();
        QWidget* createOrientationWidget();
        QWidget* createWholeBrainSurfaceOptionsWidget();
        QWidget* createVolumeIndicesWidget();
        QWidget* createToolsWidget();
        QWidget* createWindowWidget();
        QWidget* createSingleSurfaceOptionsWidget();
        QWidget* createSurfaceMontageOptionsWidget();
        QWidget* createVolumeMontageWidget();
        QWidget* createVolumePlaneWidget();
        
        ModelTypeEnum::Enum updateViewWidget(BrowserTabContent* browserTabContent);
        void updateOrientationWidget(BrowserTabContent* browserTabContent);
        void updateWholeBrainSurfaceOptionsWidget(BrowserTabContent* browserTabContent);
        void updateVolumeIndicesWidget(BrowserTabContent* browserTabContent);
        void updateToolsWidget(BrowserTabContent* browserTabContent);
        void updateWindowWidget(BrowserTabContent* browserTabContent);
        void updateSingleSurfaceOptionsWidget(BrowserTabContent* browserTabContent);
        void updateSurfaceMontageOptionsWidget(BrowserTabContent* browserTabContent);
        void updateVolumeMontageWidget(BrowserTabContent* browserTabContent);
        void updateVolumePlaneWidget(BrowserTabContent* browserTabContent);
        
        QWidget* createToolWidget(const QString& name,
                                  QWidget* childWidget,
                                  const WidgetPlacement verticalBarPlacement,
                                  const WidgetPlacement contentPlacement,
                                  const int horizontalStretching);
        
        QWidget* viewWidget;
        QWidget* orientationWidget;
        QWidget* wholeBrainSurfaceOptionsWidget;
        QWidget* volumeIndicesWidget;
        QWidget* toolsWidget;
        QWidget* windowWidget;
        QWidget* singleSurfaceSelectionWidget;
        QWidget* surfaceMontageSelectionWidget;
        QWidget* volumeMontageWidget;
        QWidget* volumePlaneWidget;
        //QWidget* spacerWidget;
        
        WuQWidgetObjectGroup* viewWidgetGroup;
        WuQWidgetObjectGroup* orientationWidgetGroup;
        WuQWidgetObjectGroup* wholeBrainSurfaceOptionsWidgetGroup;
        WuQWidgetObjectGroup* volumeIndicesWidgetGroup;
        WuQWidgetObjectGroup* toolsWidgetGroup;
        WuQWidgetObjectGroup* windowWidgetGroup;
        WuQWidgetObjectGroup* singleSurfaceSelectionWidgetGroup;
        WuQWidgetObjectGroup* surfaceMontageSelectionWidgetGroup;
        WuQWidgetObjectGroup* volumeMontageWidgetGroup;
        WuQWidgetObjectGroup* volumePlaneWidgetGroup;
        
        QWidget* fullToolBarWidget;
        QWidget* toolbarWidget;
        QHBoxLayout* toolbarWidgetLayout;
        
        QWidget* tabBarWidget;
        QTabBar* tabBar;
        
        /** Widget displayed at bottom of toolbar for mouse input controls */
        QWidget* userInputControlsWidget;

        /** Layout for widget displayed at bottom of toolbar for mouse input controls */
        QHBoxLayout* userInputControlsWidgetLayout;

        /** Is set to the user input widget provided by the user input processor */
        QWidget* userInputControlsWidgetActiveInputWidget;
        
        /**
         * When updating, no signals should be emitted.  This variable
         * is incremented at the beginning of an update method and
         * decremented at the end of the update method.  If it is 
         * non-zero in a slot method, then a signal was emitted during
         * the update and the widget that emitted the signal should
         * have its signal blocked.
         */
        void incrementUpdateCounter(const char* methodName);
        void decrementUpdateCounter(const char* methodName);
        void checkUpdateCounter();
        int updateCounter;
        
        void removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent);
        void removeTabWithContent(BrowserTabContent* browserTabContent);
        
    public slots:
        void addNewTab();

        void addNewTab(BrowserTabContent* browserTabContent);
        
        void closeSelectedTab();

        void moveTabsToNewWindows();
        
        void nextTab();
        
        void previousTab();
        
        void renameTab();
        
        void updateToolBar();
        
        void showHideToolBar(bool showIt);
        
    private slots:
        void selectedTabChanged(int indx);
        void tabClosed(int index);
        
    private:
        void removeTab(int index);
        
        QRadioButton* viewModeSurfaceRadioButton;
        QRadioButton* viewModeSurfaceMontageRadioButton;
        QRadioButton* viewModeVolumeRadioButton;
        QRadioButton* viewModeWholeBrainRadioButton;
        
    private slots:
        void viewModeRadioButtonClicked(QAbstractButton*);
        
    private:
        QAction* orientationLeftOrLateralToolButtonAction;
        QAction* orientationRightOrMedialToolButtonAction;
        QAction* orientationAnteriorToolButtonAction;
        QAction* orientationPosteriorToolButtonAction;
        QAction* orientationDorsalToolButtonAction;
        QAction* orientationVentralToolButtonAction;
        QAction* orientationResetToolButtonAction;
        QAction* orientationUserViewOneToolButtonAction;
        QAction* orientationUserViewTwoToolButtonAction;
        QAction* orientationUserViewSelectToolButtonAction;
        QMenu* orientationUserViewSelectToolButtonMenu;
        QToolButton* orientationUserViewSelectToolButton;
        
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
    private slots:
        void orientationLeftOrLateralToolButtonTriggered(bool checked);
        void orientationRightOrMedialToolButtonTriggered(bool checked);
        void orientationAnteriorToolButtonTriggered(bool checked);
        void orientationPosteriorToolButtonTriggered(bool checked);
        void orientationDorsalToolButtonTriggered(bool checked);
        void orientationVentralToolButtonTriggered(bool checked);
        void orientationResetToolButtonTriggered(bool checked);
        void orientationUserViewOneToolButtonTriggered(bool checked);
        void orientationUserViewTwoToolButtonTriggered(bool checked);

        void orientationUserViewSelectToolButtonMenuAboutToShow();
        void orientationUserViewSelectToolButtonMenuTriggered(QAction* action);
        
    
    private:
        QComboBox*      wholeBrainSurfaceTypeComboBox;
        QCheckBox*      wholeBrainSurfaceLeftCheckBox;
        QCheckBox*      wholeBrainSurfaceRightCheckBox;
        QCheckBox*      wholeBrainSurfaceCerebellumCheckBox;
        QDoubleSpinBox* wholeBrainSurfaceSeparationLeftRightSpinBox;
        QDoubleSpinBox* wholeBrainSurfaceSeparationCerebellumSpinBox;

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
    
    private:
        QCheckBox* volumeIndicesParasagittalCheckBox;
        QCheckBox* volumeIndicesCoronalCheckBox;
        QCheckBox* volumeIndicesAxialCheckBox;
        QSpinBox*  volumeIndicesParasagittalSpinBox;
        QSpinBox*  volumeIndicesCoronalSpinBox;
        QSpinBox*  volumeIndicesAxialSpinBox;
        QDoubleSpinBox* volumeIndicesXcoordSpinBox;
        QDoubleSpinBox* volumeIndicesYcoordSpinBox;
        QDoubleSpinBox* volumeIndicesZcoordSpinBox;
        
        void readVolumeSliceCoordinatesAndUpdateSliceIndices();
        void readVolumeSliceIndicesAndUpdateSliceCoordinates();
        void updateSliceIndicesAndCoordinatesRanges();
        
    private slots:
        void volumeIndicesOriginActionTriggered();
        void volumeIndicesParasagittalCheckBoxStateChanged(int state);
        void volumeIndicesCoronalCheckBoxStateChanged(int state);
        void volumeIndicesAxialCheckBoxStateChanged(int state);
        void volumeIndicesParasagittalSpinBoxValueChanged(int i);
        void volumeIndicesCoronalSpinBoxValueChanged(int i);
        void volumeIndicesAxialSpinBoxValueChanged(int i);
        void volumeIndicesXcoordSpinBoxValueChanged(double d);
        void volumeIndicesYcoordSpinBoxValueChanged(double d);
        void volumeIndicesZcoordSpinBoxValueChanged(double d);
        
    private:
        
        QComboBox* windowYokeGroupComboBox;
    
    private slots:
        void windowYokeToGroupComboBoxIndexChanged(int indx);
        
    private:
        StructureSurfaceSelectionControl* surfaceSurfaceSelectionControl;
        
    private slots:
        void surfaceSelectionControlChanged(const StructureEnum::Enum,
                                            ModelSurface*);
        
    private:
        SurfaceSelectionViewController* surfaceMontageLeftSurfaceViewController;
        SurfaceSelectionViewController* surfaceMontageLeftSecondSurfaceViewController;
        SurfaceSelectionViewController* surfaceMontageRightSurfaceViewController;
        SurfaceSelectionViewController* surfaceMontageRightSecondSurfaceViewController;
        QCheckBox* surfaceMontageDualConfigurationCheckBox;
        
    private slots:
        void surfaceMontageLeftSurfaceSelected(Surface*);
        void surfaceMontageLeftSecondSurfaceSelected(Surface*);
        void surfaceMontageRightSurfaceSelected(Surface*);
        void surfaceMontageRightSecondSurfaceSelected(Surface*);
        void surfaceMontageDualConfigurationCheckBoxSelected(bool);
        
    private:
        QAction* volumePlaneParasagittalToolButtonAction;
        QAction* volumePlaneCoronalToolButtonAction;
        QAction* volumePlaneAxialToolButtonAction;
        QAction* volumePlaneAllToolButtonAction;
        QAction* volumePlaneViewOrthogonalToolButtonAction;
        QAction* volumePlaneViewObliqueToolButtonAction;
        QAction* volumePlaneViewMontageToolButtonAction;
        QAction* volumePlaneResetToolButtonAction;
        
        QActionGroup* volumePlaneActionGroup;
        QActionGroup* volumePlaneViewActionGroup;
        
    private slots:
        void volumePlaneActionGroupTriggered(QAction*);
        void volumePlaneViewActionGroupTriggered(QAction*);
        void volumePlaneResetToolButtonTriggered(bool checked);
        
        
    private:
        QSpinBox* montageRowsSpinBox;
        QSpinBox* montageColumnsSpinBox;
        QSpinBox* montageSpacingSpinBox;
        
    private slots:
        void montageRowsSpinBoxValueChanged(int i);
        void montageColumnsSpinBoxValueChanged(int i);
        void montageSpacingSpinBoxValueChanged(int i);
        
    private:
        QAction* toolsToolBoxToolButtonAction;

    private slots:
        void toolsConnectToDatabaseActionTriggered(bool);
        void toolsInputModeActionTriggered(QAction*);
        
    private:
        void updateDisplayedToolsUserInputWidget();
        QMenu* toolsConnectMenu;
        QAction* toolsConnectToAllenDatabaseAction;
        QAction* toolsConnectToConnectomeDatabaseAction;
        
        QActionGroup* toolsInputModeActionGroup;
        QAction* toolsInputModeBordersAction;
        QAction* toolsInputModeViewAction;
        
    private:
        //BrainBrowserWindowToolBox* toolBox;
        QAction* toolBarToolButtonAction;
        QAction* toolBoxToolButtonAction;
    
    private:
        int32_t browserWindowIndex;
        
    private:
        friend class BrainBrowserWindow;
        
        bool isContructorFinished;
        bool isDestructionInProgress;
        
        int32_t indexOfNewestAddedOrInsertedTab;
    };
}

#endif // __BRAIN_BROWSER_WINDOW_TOOLBAR_H__

