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
#include "ModelDisplayControllerTypeEnum.h"

class QAbstractButton;
class QActionGroup;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QHBoxLayout;
class QLabel;
class QMenu;
class QRadioButton;
class QSpinBox;
class QTabBar;
class QToolButton;

namespace caret {
    
    class BrowserTabContent;
    class ModelDisplayController;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBar : public QToolBar, public EventListenerInterface   {
      
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBar(const int32_t browserWindowIndex,
                                  BrowserTabContent* initialBrowserTabContent,
                                  QAction* toolsToolBoxToolButtonAction,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowToolBar();
        
        QAction* getShowToolBoxAction();
        
        void receiveEvent(Event* event);
        
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
        
        ModelDisplayController* getDisplayedModelController();
        
        void updateGraphicsWindow();
        
        QWidget* createViewWidget();
        QWidget* createOrientationWidget();
        QWidget* createWholeBrainSurfaceOptionsWidget();
        QWidget* createVolumeIndicesWidget();
        QWidget* createToolsWidget();
        QWidget* createWindowWidget();
        QWidget* createSingleSurfaceOptionsWidget();
        QWidget* createVolumeMontageWidget();
        QWidget* createVolumePlaneWidget();
        
        ModelDisplayControllerTypeEnum::Enum updateViewWidget(BrowserTabContent* browserTabContent);
        void updateOrientationWidget(BrowserTabContent* browserTabContent);
        void updateWholeBrainSurfaceOptionsWidget(BrowserTabContent* browserTabContent);
        void updateVolumeIndicesWidget(BrowserTabContent* browserTabContent);
        void updateToolsWidget(BrowserTabContent* browserTabContent);
        void updateWindowWidget(BrowserTabContent* browserTabContent);
        void updateSingleSurfaceOptionsWidget(BrowserTabContent* browserTabContent);
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
        QWidget* singleSurfaceVerticalBarWidget;
        QWidget* singleSurfaceSelectionWidget;
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
        WuQWidgetObjectGroup* volumeMontageWidgetGroup;
        WuQWidgetObjectGroup* volumePlaneWidgetGroup;
        
        QWidget* toolbarWidget;
        QHBoxLayout* toolbarWidgetLayout;
        
        QTabBar* tabBar;

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
        
    public slots:
        void addNewTab();

        void addNewTab(BrowserTabContent* browserTabContent);
        
        void closeSelectedTab();

        void moveTabsToNewWindows();
        
        void nextTab();
        
        void previousTab();
        
        void renameTab();
        
        void updateToolBar();
        
    private slots:
        void selectedTabChanged(int indx);
        void tabClosed(int index);
        
    private:
        QRadioButton* viewModeSurfaceRadioButton;
        QRadioButton* viewModeVolumeRadioButton;
        QRadioButton* viewModeWholeBrainRadioButton;
        
    private slots:
        void viewModeRadioButtonClicked(QAbstractButton*);
        
    private:
        QAction* orientationLeftToolButtonAction;
        QAction* orientationRightToolButtonAction;
        QAction* orientationAnteriorToolButtonAction;
        QAction* orientationPosteriorToolButtonAction;
        QAction* orientationDorsalToolButtonAction;
        QAction* orientationVentralToolButtonAction;
        QAction* orientationResetToolButtonAction;
        QAction* orientationUserViewOneToolButtonAction;
        QAction* orientationUserViewTwoToolButtonAction;
        QAction* orientationUserViewSelectToolButtonAction;
        QMenu* orientationUserViewSelectToolButtonMenu;
        
    private slots:
        void orientationLeftToolButtonTriggered(bool checked);
        void orientationRightToolButtonTriggered(bool checked);
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
        QMenu* wholeBrainSurfaceLeftMenu;
        QMenu* wholeBrainSurfaceRightMenu;
        QMenu* wholeBrainSurfaceCerebellumMenu;
        QDoubleSpinBox* wholeBrainSurfaceSeparationLeftRightSpinBox;
        QDoubleSpinBox* wholeBrainSurfaceSeparationCerebellumSpinBox;

    private slots:
        void wholeBrainSurfaceTypeComboBoxIndexChanged(int indx);
        void wholeBrainSurfaceLeftCheckBoxStateChanged(int state);
        void wholeBrainSurfaceRightCheckBoxStateChanged(int state);
        void wholeBrainSurfaceCerebellumCheckBoxStateChanged(int state);
        void wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double d);
        void wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double d);
        void wholeBrainSurfaceLeftMenuTriggered(QAction*);
        void wholeBrainSurfaceRightMenuTriggered(QAction*);
        void wholeBrainSurfaceCerebellumMenuTriggered(QAction*);
    
    private:
        QCheckBox* volumeIndicesParasagittalCheckBox;
        QCheckBox* volumeIndicesCoronalCheckBox;
        QCheckBox* volumeIndicesAxialCheckBox;
        QSpinBox*  volumeIndicesParasagittalSpinBox;
        QSpinBox*  volumeIndicesCoronalSpinBox;
        QSpinBox*  volumeIndicesAxialSpinBox;
        
    private slots:
        void volumeIndicesParasagittalCheckBoxStateChanged(int state);
        void volumeIndicesCoronalCheckBoxStateChanged(int state);
        void volumeIndicesAxialCheckBoxStateChanged(int state);
        void volumeIndicesParasagittalSpinBoxValueChanged(int i);
        void volumeIndicesCoronalSpinBoxValueChanged(int i);
        void volumeIndicesAxialSpinBoxValueChanged(int i);
        
    private:
        
        QComboBox* windowYokeToTabComboBox;
        QCheckBox* windowYokeMirroredCheckBox;
    
    private slots:
        void windowYokeToTabComboBoxIndexChanged(int indx);
        void windowYokeMirroredCheckBoxStateChanged(int state);
        
    private:
        QComboBox* surfaceSurfaceSelectionComboBox;
        
    private slots:
        void surfaceSurfaceSelectionComboBoxIndexChanged(int indx);
        
    private:
        QAction* volumePlaneParasagittalToolButtonAction;
        QAction* volumePlaneCoronalToolButtonAction;
        QAction* volumePlaneAxialToolButtonAction;
        QAction* volumePlaneAllToolButtonAction;
        QAction* volumePlaneObliqueToolButtonAction;
        QAction* volumePlaneMontageToolButtonAction;
        QAction* volumePlaneResetToolButtonAction;
        
        QActionGroup* volumePlaneActionGroup;
        
    private slots:
        void volumePlaneActionGroupTriggered(QAction*);
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
    
    private:
        int32_t browserWindowIndex;
        
    private:
        friend class BrainBrowserWindow;
        
        bool isContructorFinished;
    };
}

#endif // __BRAIN_BROWSER_WINDOW_TOOLBAR_H__

