
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

#include <algorithm>
#include <cmath>
#include <limits>

#include <QActionGroup>
#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QMenu>
#include <QRadioButton>
#include <QSpinBox>
#include <QStyleFactory>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>

#include "AnnotationBrowserTab.h"
#include "AnnotationCoordinate.h"
#include "AnnotationManager.h"
#include "AnnotationStackingOrderOperation.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainBrowserWindowToolBarAllSurface.h"
#include "BrainBrowserWindowToolBarChartAttributes.h"
#include "BrainBrowserWindowToolBarChartAxes.h"
#include "BrainBrowserWindowToolBarChartTwoAttributes.h"
#include "BrainBrowserWindowToolBarChartTwoOrientation.h"
#include "BrainBrowserWindowToolBarChartTwoOrientedAxes.h"
#include "BrainBrowserWindowToolBarChartTwoType.h"
#include "BrainBrowserWindowToolBarChartType.h"
#include "BrainBrowserWindowToolBarHistology.h"
#include "BrainBrowserWindowToolBarImage.h"
#include "BrainBrowserWindowToolBarOrientation.h"
#include "BrainBrowserWindowToolBarSlicePlane.h"
#include "BrainBrowserWindowToolBarSliceSelection.h"
#include "BrainBrowserWindowToolBarSurface.h"
#include "BrainBrowserWindowToolBarSurfaceMontage.h"
#include "BrainBrowserWindowToolBarTab.h"
#include "BrainBrowserWindowToolBarTabPopUpMenu.h"
#include "BrainBrowserWindowToolBarView.h"
#include "BrainBrowserWindowToolBarVolumeMPR.h"
#include "BrainBrowserWindowToolBarVolumeMontage.h"
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretFunctionName.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CursorDisplayScoped.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesBorders.h"
#include "EventBrowserTabClose.h"
#include "EventBrowserTabCloseInToolBar.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabDeleteInToolBar.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabGetAllViewed.h"
#include "EventBrowserTabNew.h"
#include "EventBrowserTabNewClone.h"
#include "EventBrowserTabNewInGUI.h"
#include "EventBrowserTabReopenClosed.h"
#include "EventBrowserTabSelectInWindow.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventBrowserWindowCreateTabs.h"
#include "EventBrowserWindowNew.h"
#include "EventBrowserWindowTileTabOperation.h"
#include "EventUserInputModeGet.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "EventSceneActive.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateYokedWindows.h"
#include "GuiManager.h"
#include "LockAspectWarningDialog.h"
#include "Model.h"
#include "ModelChart.h"
#include "ModelChartTwo.h"
#include "ModelHistology.h"
#include "ModelMedia.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelSurfaceSelector.h"
#include "ModelTransform.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "OverlaySet.h"
#include "RecentSceneMenu.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneIntegerArray.h"
#include "ScenePrimitiveArray.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceSelectionViewController.h"
#include "StructureSurfaceSelectionControl.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "UserInputModeAbstract.h"
#include "UserInputModeAnnotations.h"
#include "UserInputModeAnnotationsWidget.h"
#include "UserInputModeBorders.h"
#include "UserInputModeBordersWidget.h"
#include "UserInputModeFoci.h"
#include "UserInputModeFociWidget.h"
#include "UserInputModeImage.h"
#include "UserInputModeImageWidget.h"
#include "UserInputModeTileTabsLayout.h"
#include "UserInputModeView.h"
#include "UserInputModeVolumeEdit.h"
#include "UserInputModeVolumeEditWidget.h"
#include "VolumeFile.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSurfaceOutlineSetModel.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQTabBar.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"


using namespace caret;

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of the parent browser window.
 * @param initialBrowserTabContent
 *    Content of default tab (may be NULL in which cast
 *    new content is created).
 * @param overlayToolBoxAction
 *    Action to show overlay tool box.
 * @param layersToolBoxAction
 *    Action to show layers tool box.
 * @param toolBarLockWindowAndAllTabAspectRatioButton
 *    Button to lock window's aspect ratio and aspect ratio of all tabs.
 * @param objectNamePrefix
 *    Prefix for object name
 * @param parent
 *    Parent for this toolbar.
 */
BrainBrowserWindowToolBar::BrainBrowserWindowToolBar(const int32_t browserWindowIndex,
                                                     BrowserTabContent* initialBrowserTabContent,
                                                     QAction* overlayToolBoxAction,
                                                     QAction* layersToolBoxAction,
                                                     QToolButton* toolBarLockWindowAndAllTabAspectRatioButton,
                                                     const QString& objectNamePrefix,
                                                     BrainBrowserWindow* parentBrainBrowserWindow)
: QToolBar(parentBrainBrowserWindow),
m_parentBrainBrowserWindow(parentBrainBrowserWindow)
{
    this->browserWindowIndex = browserWindowIndex;
    m_tabIndexForTileTabsHighlighting = -1;
    
    this->isContructorFinished = false;
    this->isDestructionInProgress = false;

    /*
     * Needed for saving and restoring window state in main window
     */
    m_objectNamePrefix = (objectNamePrefix
                          + ":ToolBar");
    setObjectName(m_objectNamePrefix);
    
    /*
     * Create tab bar that displays models.
     */
    this->tabBar = new WuQTabBar();
    if (WuQtUtilities::isSmallDisplay()) {
        this->tabBar->setStyleSheet("QTabBar::tab:selected {"
                                    "    font: bold;"
                                    "}  " 
                                    "QTabBar::tab {"
                                    "    font: italic"
                                    "}");
    }
    else {
        this->tabBar->setStyleSheet("QTabBar::tab:selected {"
                                    "    font: bold 14px;"
                                    "}  " 
                                    "QTabBar::tab {"
                                    "    font: italic"
                                    "}");
    }

    this->tabBar->setShape(QTabBar::RoundedNorth);
    this->tabBar->setMovable(true);
#ifdef Q_OS_MACX
    /*
     * Adding a parent to the style will result in it
     * being destroyed when this instance is destroyed.
     * The style must remain valid until the destruction
     * of this instance.  It cannot be declared statically.
     */
    /*
     * "Cleanlooks Style" removed in Qt5.  Fusion style
     * seems to have same effect on toolbar so that 
     * tabs do not get shrunk too small and so the
     * scroll tabs arrows are available.
     */
    QStyle* fusionStyle = QStyleFactory::create("Fusion");
    if (fusionStyle != NULL) {
        fusionStyle->setParent(this);
        this->tabBar->setStyle(fusionStyle);
    }
#endif // Q_OS_MACX
    QObject::connect(this->tabBar, SIGNAL(currentChanged(int)),
                     this, SLOT(selectedTabChanged(int)));
    QObject::connect(this->tabBar, SIGNAL(tabCloseRequested(int)),
                     this, SLOT(tabCloseSelected(int)));
    QObject::connect(this->tabBar, SIGNAL(tabMoved(int,int)),
                     this, SLOT(tabMoved(int,int)));
    this->tabBar->setObjectName(m_objectNamePrefix
                                + ":Tab");
    
    WuQMacroManager::instance()->addMacroSupportToObjectWithToolTip(qobject_cast<QTabBar*>(this->tabBar),
                                                                    "ToolBar Tab",
                                                                    "Select Tab");

    /*
     * Add context menu
     * From the position, use QTabBar::tabAt(const QPoint& position) to get the
     * tab that the user has clicked and if over a tab, pop-up a menu 
     * for the tab with Create New Tab and Place Right, Duplicate and Place on Right
     */
    QObject::connect(this->tabBar, &QTabBar::customContextMenuRequested,
                     this, &BrainBrowserWindowToolBar::showTabMenu);
    this->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    
    /*
     * Custom view action
     */
    this->customViewMenu = new QMenu();
    QObject::connect(this->customViewMenu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBar::customViewMenuAboutToShow);
    QObject::connect(this->customViewMenu, &QMenu::triggered,
                     this, &BrainBrowserWindowToolBar::customViewMenuTriggered);
    const QString customToolTip = ("Pressing the \"Custom\" button displays a dialog for creating and editing orientations.\n"
                                   "Note that custom orientations are stored in your Workbench's preferences and thus\n"
                                   "will be availble in any concurrent or future instances of Workbench.\n"
                                   "Press arrow on right to select and apply existing custom view.");
    this->customViewAction = new QAction("Custom");
    this->customViewAction->setToolTip(customToolTip);
    this->customViewAction->setMenu(this->customViewMenu);
    QObject::connect(this->customViewAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBar::customViewActionTriggered);
    this->customViewAction->setObjectName(m_objectNamePrefix
                                          + ":CustomView");
    WuQMacroManager::instance()->addMacroSupportToObject(this->customViewAction,
                                                         "Display custom view dialog");
    
    /*
     * Actions at right side of toolbar
     */
    QToolButton* informationDialogToolButton = new QToolButton();
    informationDialogToolButton->setDefaultAction(GuiManager::get()->getInformationDisplayDialogEnabledAction());
    
    QToolButton* identifyDialogToolButton = new QToolButton();
    identifyDialogToolButton->setDefaultAction(GuiManager::get()->getIdentifyBrainordinateDialogDisplayAction());
    
    QToolButton* helpDialogToolButton = new QToolButton();
    helpDialogToolButton->setDefaultAction(GuiManager::get()->getHelpViewerDialogDisplayAction());

    /*
     * Scene button action
     */
    QAction* sceneButtonAction = new QAction(this);
    sceneButtonAction->setText("");
    sceneButtonAction->setIcon(GuiManager::get()->getSceneDialogDisplayAction()->icon());
    sceneButtonAction->setMenu(new RecentSceneMenu(m_parentBrainBrowserWindow));
    QObject::connect(sceneButtonAction, &QAction::triggered,
                     [=](bool) { sceneToolButtonClicked(); });

    /*
     * Scene button
     */
    QToolButton* sceneDialogToolButton = new QToolButton();
    sceneDialogToolButton->setDefaultAction(sceneButtonAction);

    /*
     * Movie button
     */
    QIcon movieIcon;
    QString movieButtonText;
    if ( ! WuQtUtilities::loadIcon(":/ToolBar/movie.png",
                                   movieIcon)) {
        movieButtonText = "Movie";
    }
    const QString movieButtonToolTip("Show movie recording window");
    m_movieToolButton = new QToolButton();
    m_movieToolButton->setText(movieButtonText);
    m_movieToolButton->setIcon(movieIcon);
    m_movieToolButton->setToolTip(movieButtonToolTip);
    QObject::connect(m_movieToolButton, &QToolButton::clicked,
                     parentBrainBrowserWindow, &BrainBrowserWindow::processMovieRecording);
    
    /*
     * Macros button
     */
    QIcon macrosIcon;
    QAction* macrosAction = new QAction();
    if (WuQtUtilities::loadIcon(":/ToolBar/macro.png",
                                macrosIcon)) {
        macrosAction->setIcon(macrosIcon);
    }
    else {
        macrosAction->setText("M");
    }
    macrosAction->setToolTip("Show macros window");
    QObject::connect(macrosAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBar::showMacroDialog);
    QToolButton* macrosToolButton = new QToolButton();
    macrosToolButton->setDefaultAction(macrosAction);
    macrosAction->setParent(macrosToolButton);
    
    /*
     * Toolbar action and tool button at right of the tab bar
     */
    QIcon toolBarIcon;
    const bool toolBarIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/toolbar.png", 
                            toolBarIcon);
    
    this->toolBarToolButtonAction =
    WuQtUtilities::createAction("Toolbar", 
                                "Show or hide the toolbar",
                                this,
                                this,
                                SLOT(showHideToolBar(bool)));
    if (toolBarIconValid) {
        this->toolBarToolButtonAction->setIcon(toolBarIcon);
        this->toolBarToolButtonAction->setIconVisibleInMenu(false);
    }
    this->toolBarToolButtonAction->setIconVisibleInMenu(false);
    this->toolBarToolButtonAction->blockSignals(true);
    this->toolBarToolButtonAction->setCheckable(true);
    this->toolBarToolButtonAction->setChecked(true);
    this->showHideToolBar(this->toolBarToolButtonAction->isChecked());
    this->toolBarToolButtonAction->blockSignals(false);
    QToolButton* toolBarToolButton = new QToolButton();
    toolBarToolButton->setDefaultAction(this->toolBarToolButtonAction);
    toolBarToolButtonAction->setObjectName(m_objectNamePrefix
                                           + ":ShowToolBar");
    WuQMacroManager::instance()->addMacroSupportToObject(toolBarToolButtonAction,
                                                         "Show toolbar");
    
    /*
     * Toolbox control at right of the tab bar
     */
    QToolButton* overlayToolBoxToolButton = new QToolButton();
    overlayToolBoxToolButton->setDefaultAction(overlayToolBoxAction);
    
    QToolButton* layersToolBoxToolButton = new QToolButton();
    layersToolBoxToolButton->setDefaultAction(layersToolBoxAction);
    
    QToolButton* dataToolTipsToolButton = new QToolButton();
    dataToolTipsToolButton->setDefaultAction(GuiManager::get()->getDataToolTipsAction(dataToolTipsToolButton));
    
    /*
     * Make all tool buttons the same height
     */
    WuQtUtilities::matchWidgetHeights(macrosToolButton,
                                      m_movieToolButton,
                                      helpDialogToolButton,
                                      informationDialogToolButton,
                                      identifyDialogToolButton,
                                      sceneDialogToolButton,
                                      toolBarToolButton,
                                      overlayToolBoxToolButton,
                                      layersToolBoxToolButton,
                                      dataToolTipsToolButton);
    
    WuQtUtilities::setToolButtonStyleForQt5Mac(macrosToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_movieToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(helpDialogToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(informationDialogToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(identifyDialogToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(sceneDialogToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolBarToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(overlayToolBoxToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(layersToolBoxToolButton);
    WuQtUtilities::setToolButtonStyleForQt5Mac(dataToolTipsToolButton);
    
    /*
     * Tab bar and controls at far right side of toolbar
     */
    this->tabBarWidget = new QWidget();
    QHBoxLayout* tabBarLayout = new QHBoxLayout(this->tabBarWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(tabBarLayout, 2, 1);
    tabBarLayout->addWidget(this->tabBar, 100);
    tabBarLayout->addWidget(dataToolTipsToolButton);
    tabBarLayout->addWidget(helpDialogToolButton);
    tabBarLayout->addWidget(informationDialogToolButton);
    tabBarLayout->addWidget(identifyDialogToolButton);
    tabBarLayout->addWidget(m_movieToolButton);
    tabBarLayout->addWidget(macrosToolButton);
    tabBarLayout->addWidget(sceneDialogToolButton);
    tabBarLayout->addWidget(toolBarToolButton);
    tabBarLayout->addWidget(overlayToolBoxToolButton);
    tabBarLayout->addWidget(layersToolBoxToolButton);
    
    /*
     * Create the toolbar's widgets.
     */
    this->viewWidget = this->createViewWidget();
    this->orientationWidget = this->createOrientationWidget();
    this->chartAxesWidget = createChartAxesWidget();
    this->chartAttributesWidget = createChartAttributesWidget();
    this->chartTwoOrientationWidget = createChartTwoOrientationWidget();
    this->chartTwoAttributesWidget = createChartTwoAttributesWidget();
    this->chartTwoOrientedAxesWidget = createChartTwoOrientedAxisWidget();
    this->chartTypeWidget = createChartTypeWidget();
    this->chartTypeTwoWidget = createChartTypeTwoWidget();
    this->wholeBrainSurfaceOptionsWidget = this->createWholeBrainSurfaceOptionsWidget();
    this->volumeIndicesWidget = this->createVolumeIndicesWidget();
    this->modeWidget = this->createModeWidget();
    this->tabMiscWidget = this->createTabOptionsWidget(toolBarLockWindowAndAllTabAspectRatioButton);
    this->singleSurfaceSelectionWidget = this->createSingleSurfaceOptionsWidget();
    this->surfaceMontageSelectionWidget = this->createSurfaceMontageOptionsWidget();
    this->volumeMontageWidget = this->createVolumeMontageWidget();
    this->volumeMprWidget = this->createVolumeMprWidget();
    this->volumePlaneWidget = this->createVolumePlaneWidget();
    this->histologyWidget = this->createHistologyWidget();
    this->imageWidget = this->createImageWidget();

    this->userInputAnnotationsModeProcessor = new UserInputModeAnnotations(browserWindowIndex);
    this->userInputBordersModeProcessor = new UserInputModeBorders(browserWindowIndex);
    this->userInputFociModeProcessor = new UserInputModeFoci(browserWindowIndex);
    this->userInputImageModeProcessor = new UserInputModeImage(browserWindowIndex);
    this->userInputVolumeEditModeProcessor = new UserInputModeVolumeEdit(browserWindowIndex);
    this->userInputTileTabsManualLayoutProcessor = new UserInputModeTileTabsLayout(browserWindowIndex);
    this->userInputViewModeProcessor = new UserInputModeView(browserWindowIndex);
    this->selectedUserInputProcessor = this->userInputViewModeProcessor;
    this->selectedUserInputProcessor->initialize();

    this->annotateModeWidget = createToolWidget("",
                                                this->userInputAnnotationsModeProcessor->getWidgetForToolBar(),
                                                WIDGET_PLACEMENT_LEFT,
                                                WIDGET_PLACEMENT_TOP,
                                                0);
    this->bordersModeWidget = this->userInputBordersModeProcessor->getWidgetForToolBar();
    this->fociModeWidget = createToolWidget("Foci Operations",
                                            this->userInputFociModeProcessor->getWidgetForToolBar(),
                                            WIDGET_PLACEMENT_LEFT,
                                            WIDGET_PLACEMENT_TOP,
                                            0);
    this->imageModeWidget = createToolWidget("Image Operations",
                                             this->userInputImageModeProcessor->getWidgetForToolBar(),
                                             WIDGET_PLACEMENT_LEFT,
                                             WIDGET_PLACEMENT_TOP,
                                             0);
    this->tileModeWidget = createToolWidget("",
                                            this->userInputTileTabsManualLayoutProcessor->getWidgetForToolBar(),
                                            WIDGET_PLACEMENT_LEFT,
                                            WIDGET_PLACEMENT_TOP,
                                            0);
    
    this->volumeModeWidget = this->userInputVolumeEditModeProcessor->getWidgetForToolBar();
    

    /*
     * Layout the toolbar's widgets.
     */
    m_toolbarWidget = new QWidget();
    this->toolbarWidgetLayout = new QHBoxLayout(m_toolbarWidget);
    this->toolbarWidgetLayout->setContentsMargins(0, 0, 0, 0);
    this->toolbarWidgetLayout->setSpacing(0);
    
    this->toolbarWidgetLayout->addWidget(this->modeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->viewWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->orientationWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->wholeBrainSurfaceOptionsWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->singleSurfaceSelectionWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->surfaceMontageSelectionWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumePlaneWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeMontageWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeMprWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeIndicesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartTypeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartTypeTwoWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartAxesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartTwoOrientationWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartTwoAttributesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartTwoOrientedAxesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartAttributesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->imageWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->histologyWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->annotateModeWidget, 0, Qt::AlignLeft);

    this->toolbarWidgetLayout->addWidget(this->bordersModeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->fociModeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->imageModeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->tileModeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeModeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->tabMiscWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addStretch();

    /*
     * Arrange the tabbar and the toolbar vertically.
     */
    QWidget* w = new QWidget();
    m_toolBarMainLayout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutSpacingAndMargins(m_toolBarMainLayout, 1, 0);
    m_toolBarMainLayout->addWidget(this->tabBarWidget);
    m_toolBarMainLayout->addWidget(m_toolbarWidget);
    
    this->addWidget(w);
    
    if (initialBrowserTabContent != NULL) {
        insertTabContentPrivate(InsertTabMode::APPEND,
                                initialBrowserTabContent,
                                -1);
    }
    else {
        AString errorMessage;
        BrowserTabContent* tabContent = this->createNewTab(errorMessage);
        if (tabContent != NULL) {
            insertTabContentPrivate(InsertTabMode::APPEND,
                                    tabContent,
                                    -1);
        }
    }
    
    /*
     * The height of the toolbar is set here.  Prior to calling
     * updateToolBar(), all widgets still have a 'visible' status
     * and thus the sizeHint().height() is the maximum height.
     */
    const int32_t toolBarHeight = m_toolbarWidget->sizeHint().height();
    m_toolbarWidget->setFixedHeight(toolBarHeight);
    
    this->updateToolBar();
    
    this->isContructorFinished = true;
    m_tileTabsHighlightingTimerEnabledFlag = true;
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_CLOSE_IN_TOOL_BAR);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE_IN_TOOL_BAR);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW_IN_GUI);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL_VIEWED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_SELECT_IN_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_DRAWING_CONTENT_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_CREATE_TABS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_USER_INPUT_MODE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);

    QObject::connect(this->tabBar, &WuQTabBar::mousePressedSignal,
                     this, &BrainBrowserWindowToolBar::tabBarMousePressedSlot);
    QObject::connect(this->tabBar, &WuQTabBar::mouseReleasedSignal,
                     this, &BrainBrowserWindowToolBar::tabBarMouseReleasedSlot);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBar::~BrainBrowserWindowToolBar()
{
    this->isDestructionInProgress = true;
    

    EventManager::get()->removeAllEventsFromListener(this);
    
    this->modeWidgetGroup->clear();
    
    for (int i = (this->tabBar->count() - 1); i >= 0; i--) {
        this->tabClosed(i,
                        RemoveTabMode::DELETE_TAB_CONTENT);
    }

    delete this->userInputViewModeProcessor;
    delete this->userInputAnnotationsModeProcessor;
    delete this->userInputBordersModeProcessor;
    delete this->userInputFociModeProcessor;
    delete this->userInputImageModeProcessor;
    delete this->userInputTileTabsManualLayoutProcessor;
    delete this->userInputVolumeEditModeProcessor;
    this->selectedUserInputProcessor = NULL; /* DO NOT DELETE since it does not own the object to which it points */
    
    this->isDestructionInProgress = false;
}

/**
 * Create a new tab.  NOTE: This method only creates
 * a new tab, it DOES NOT add the tab to the toolbar.
 * @param errorMessage
 *     If fails to create new tab, it will contain a message
 *     describing the error.
 * @return 
 *     Pointer to content of new tab or NULL if unable to
 *     create the new tab.
 */
BrowserTabContent* 
BrainBrowserWindowToolBar::createNewTab(AString& errorMessage)
{
    errorMessage = "";
    
    EventBrowserTabNew newTabEvent;
    EventManager::get()->sendEvent(newTabEvent.getPointer());
    
    if (newTabEvent.isError()) {
        errorMessage = newTabEvent.getErrorMessage();
        return NULL;
    }
    
    BrowserTabContent* tabContent = newTabEvent.getBrowserTab();
    Brain* brain = GuiManager::get()->getBrain();
    tabContent->getVolumeSurfaceOutlineSet()->selectSurfacesAfterSpecFileLoaded(brain, 
                                                                                false);
    
    return tabContent;
}


/**
 * Add a new tab and clone the content of the given tab.
 * @param browserTabContentToBeCloned
 *    Tab Content that is to be cloned into the new tab.
 */
void 
BrainBrowserWindowToolBar::addNewDuplicatedTab(BrowserTabContent* browserTabContentToBeCloned)
{
    insertAndCloneTabContentAtTabBarIndex(browserTabContentToBeCloned,
                                        -1);
}

/**
 * Clone and insert the cloned tab at the given index in the tab bar.
 *
 * @param browserTabContentToBeCloned
 *    Tab Content that is to be cloned into the new tab.
 * @param tabBarIndex
 *     Index in the tab bar.  If invalid, tab is appended to the end of the toolbar.
 */
void
BrainBrowserWindowToolBar::insertAndCloneTabContentAtTabBarIndex(const BrowserTabContent* browserTabContentToBeCloned,
                                                               const int32_t tabBarIndex)
{
    if ( ! allowAddingNewTab()) {
        return;
    }
    
    /*
     * Wait cursor
     */
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    EventBrowserTabNewClone cloneTabEvent(browserTabContentToBeCloned->getTabNumber());
    EventManager::get()->sendEvent(cloneTabEvent.getPointer());
    if (cloneTabEvent.isError()) {
        cursor.restoreCursor();
        QMessageBox::critical(this,
                              "",
                              cloneTabEvent.getErrorMessage());
        return;
    }
    
    BrowserTabContent* tabContent = cloneTabEvent.getNewBrowserTab();
    CaretAssert(tabContent);
    
    if (tabBarIndex >= 0){
        insertTabContentPrivate(InsertTabMode::AT_TAB_BAR_INDEX,
                                tabContent,
                                tabBarIndex);
    }
    else {
        insertTabContentPrivate(InsertTabMode::APPEND,
                                tabContent,
                                -1);
    }
    
    this->updateToolBar();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Add a new tab containing the given content.
 * @param tabContent
 *    Content for new tab.
 */
void 
BrainBrowserWindowToolBar::addNewTabWithContent(BrowserTabContent* tabContent)
{
    insertTabContentPrivate(InsertTabMode::APPEND,
                            tabContent,
                            -1);
}

/**
 * Reopen the last closed tab in this window
 * @param reopenTabEvent
 * The reopen event
 */
void
BrainBrowserWindowToolBar::reopenLastClosedTab(EventBrowserTabReopenClosed& reopenTabEvent)
{
    if (reopenTabEvent.isError()) {
        return;
    }
    else {
        BrowserTabContent* tabContent = reopenTabEvent.getBrowserTabContent();
        CaretAssert(tabContent);
        const int32_t tabBarPosition = tabContent->getClosedTabWindowTabBarPositionIndex();
        const int32_t windowIndex    = tabContent->getClosedTabWindowIndex();
        if (windowIndex == this->browserWindowIndex) {
            insertTabContentPrivate(InsertTabMode::AT_TAB_BAR_INDEX,
                                    tabContent,
                                    tabBarPosition);
        }
        else {
            insertTabContentPrivate(InsertTabMode::APPEND,
                                    tabContent,
                                    -1);
        }
    }

    this->updateToolBar();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}


/**
 * Replace the current browser tabs with the given browser tabs.  Tabs will be added
 * or removed as needed.
 *
 * @param browserTabs
 *     Browser tabs for this window.
 */
void
BrainBrowserWindowToolBar::replaceBrowserTabs(const std::vector<BrowserTabContent*>& browserTabs)
{
    const int32_t numTabs = static_cast<int32_t>(browserTabs.size());
    if (numTabs <= 0) {
        return;
    }
    
    QSignalBlocker blocker(this->tabBar);
    
    int32_t selectedTabIndex = this->tabBar->currentIndex();

    /*
     * Remove BrowserTabContent from all tabs since tabs may be
     * closed and closing the tab will try to delete the BrowserTabContent
     * in the tab but this BrowserTabContent may still be valid.
     */
    const int32_t beforeNumTabs = this->tabBar->count();
    for (int32_t iTab = 0; iTab < beforeNumTabs; iTab++) {
        this->tabBar->setTabData(iTab,
                                 QVariant::fromValue((void*)NULL));
    }
    
    /*
     * Add/remove tabs as needed
     */
    if (beforeNumTabs < numTabs) {
        const int32_t numToAdd = numTabs - beforeNumTabs;
        for (int32_t i = 0; i < numToAdd; i++) {
            this->tabBar->addTab("");
        }
    }
    else if (beforeNumTabs > numTabs) {
        const int32_t numToRemove = beforeNumTabs - numTabs;
        for (int32_t i = 0; i < numToRemove; i++) {
            const int32_t lastTabIndex = this->tabBar->count() - 1;
            this->tabBar->removeTab(lastTabIndex);
        }
    }
    CaretAssert(this->tabBar->count() == numTabs);
    
    /*
     * Set content and update name for each tab
     */
    for (int32_t iTab = 0; iTab < numTabs; iTab++) {
        CaretAssertVectorIndex(browserTabs, iTab);
        BrowserTabContent* btc = browserTabs[iTab];
        CaretAssert(btc);
        this->tabBar->setTabData(iTab,
                                 QVariant::fromValue((void*)btc));
        this->updateTabName(iTab);
    }
    
    
    const int32_t numOpenTabs = this->tabBar->count();
    this->tabBar->setTabsClosable(numOpenTabs > 1);
    
    if (selectedTabIndex >= numTabs) {
        selectedTabIndex = numTabs - 1;
    }
    if (selectedTabIndex < 0) {
        selectedTabIndex = 0;
    }
    this->tabBar->setCurrentIndex(selectedTabIndex);
}

/**
 * Adds a new tab.
 *
 * @return
 *     Pointer to new tab content or NULL if tab could not be inserted
 */
BrowserTabContent*
BrainBrowserWindowToolBar::addNewTab()
{
    return insertNewTabAtTabBarIndex(-1);
}

/**
 * Insert a new tab at the given index in the tab bar.
 *
 * @param tabBarIndex
 *     Index in the tab bar.  If invalid, tab is appended to the end of the toolbar.
 * @return
 *     Pointer to new tab content or NULL if tab could not be inserted
 */
BrowserTabContent*
BrainBrowserWindowToolBar::insertNewTabAtTabBarIndex(const int32_t tabBarIndex)
{
    if ( ! allowAddingNewTab()) {
        return NULL;
    }
    
    /*
     * Wait cursor
     */
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    AString errorMessage;
    BrowserTabContent* tabContent = this->createNewTab(errorMessage);
    if (tabContent == NULL) {
        cursor.restoreCursor();
        QMessageBox::critical(this,
                              "",
                              errorMessage);
        return NULL;
    }
    
    if (tabBarIndex >= 0){
        insertTabContentPrivate(InsertTabMode::AT_TAB_BAR_INDEX,
                                tabContent,
                                tabBarIndex);
    }
    else {
        insertTabContentPrivate(InsertTabMode::APPEND,
                                tabContent,
                                -1);
    }
    
    this->updateToolBar();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
    
    return tabContent;
}


/**
 * Insert tab content into a tab in the toolbar.
 *
 * @param insertTabMode
 *     Mode for inserting the tab.
 * @param browserTabContent
 *     The tab content.
 * @param tabBarIndex
 *     Index of tab bar of where tab is inserted for mode AT_TAB_BAR_INDEX
 */
void
BrainBrowserWindowToolBar::insertTabContentPrivate(const InsertTabMode insertTabMode,
                                                   BrowserTabContent* browserTabContent,
                                                   const int32_t tabBarIndex)
{
    CaretAssert(browserTabContent);
    
    this->tabBar->blockSignals(true);
    
    int32_t newTabIndex = -1;
    
    switch (insertTabMode) {
        case InsertTabMode::APPEND:
            newTabIndex = this->tabBar->addTab("NewTab");
            break;
        case InsertTabMode::AT_TAB_BAR_INDEX:
            if ((tabBarIndex >= 0) && (tabBarIndex <= this->tabBar->count())) {
                newTabIndex = this->tabBar->insertTab(tabBarIndex,
                                                      "NewTab");
            }
            break;
    }

    if (newTabIndex >= 0) {
        this->tabBar->setTabData(newTabIndex,
                                 QVariant::fromValue((void*)browserTabContent));
        
        const int32_t numOpenTabs = this->tabBar->count();
        this->tabBar->setTabsClosable(numOpenTabs > 1);
        
        this->updateTabName(newTabIndex);
        
        this->tabBar->setCurrentIndex(newTabIndex);
    }
    
    this->tabBar->blockSignals(false);
}

/**
 * When adding a new tab or duplicating a tab, the tab
 * may not be visible when Tile Tabs is enabled and there
 * is no available space in the tile tabs configuration.
 *
 * @return
 *     True if new tab should be created, else false.
 */
bool
BrainBrowserWindowToolBar::allowAddingNewTab()
{
    static bool m_doNotShowDialogAgainFlag = false;
    
    if (m_doNotShowDialogAgainFlag) {
        return true;
    }
    
    CaretAssert(m_parentBrainBrowserWindow);
    BrowserWindowContent* browserWindowContent = m_parentBrainBrowserWindow->getBrowerWindowContent();
    CaretAssert(browserWindowContent);
    
    /*
     * Is tile tabs off?
     */
    if ( ! browserWindowContent->isTileTabsEnabled()) {
        return true;
    }
    
    /*
     * Automatic configuration and manual configuration always show all tabs
     */
    switch (browserWindowContent->getTileTabsConfigurationMode()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            return true;
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            return true;
            break;
    }
    
    /*
     * Is there space in the current configuration?
     */
    const TileTabsLayoutGridConfiguration* tileTabs = browserWindowContent->getCustomGridTileTabsConfiguration();
    CaretAssert(tileTabs);
    const int32_t tileTabsCount = tileTabs->getNumberOfColumns() * tileTabs->getNumberOfRows();
    if (getNumberOfTabs() < tileTabsCount) {
        return true;
    }
    
    /*
     * Selected configuration is full, all user to continue/cancel
     */
    const AString doNotShowAgainMsg("Remember my choice and do not show this dialog again");
    
    const AString msg("<html>"
                      "Tile tabs is enabled and there is no space in the selected tile "
                      "tabs configuration for the new tab.  If you choose to continue, "
                      "either the new tab will not be visible or an existing tab will "
                      "become invisible until the tile tabs configuration is changed."
                      "<p>"
                      "Do you want to create the new tab?"
                      "<br>"
                      "</html>");
    
    
    if (WuQMessageBox::warningYesNoWithDoNotShowAgain(this,
                                                      "NewTabNotDisplayedDueToTileTabs",
                                                      msg)) {
        return true;
    }
    
    return false;
}

/**
 * Show the macro dialog
 */
void
BrainBrowserWindowToolBar::showMacroDialog()
{
    CaretAssert(m_parentBrainBrowserWindow);
    WuQMacroManager::instance()->showMacrosDialog(m_parentBrainBrowserWindow);
}

/**
 * Shows/hides the toolbar.
 */
void
BrainBrowserWindowToolBar::showHideToolBar(bool showIt)
{
    if (this->isContructorFinished) {
        m_toolbarWidget->setVisible(showIt);
    }
    
    this->toolBarToolButtonAction->blockSignals(true);
    if (showIt) {
        this->toolBarToolButtonAction->setToolTip("Hide Toolbar");
        this->toolBarToolButtonAction->setChecked(true);
    }
    else {
        this->toolBarToolButtonAction->setToolTip("Show Toolbar");
        this->toolBarToolButtonAction->setChecked(false);
    }
    this->toolBarToolButtonAction->blockSignals(false);
}


/**
 * Add the default tabs after loading a spec file.
 */
void 
BrainBrowserWindowToolBar::addDefaultTabsAfterLoadingSpecFile()
{
    EventModelGetAll eventAllModels;
    EventManager::get()->sendEvent(eventAllModels.getPointer());
    
    const std::vector<Model*> allModels =
       eventAllModels.getModels();

    ModelSurface* leftSurfaceModel = NULL;
    ModelSurface* leftSurfaceInflated = NULL;
    ModelSurface* leftSurfaceVeryInflated = NULL;
    int32_t leftSurfaceTypeCode = 1000000;
    
    ModelSurface* rightSurfaceModel = NULL;
    ModelSurface* rightSurfaceInflated = NULL;
    ModelSurface* rightSurfaceVeryInflated = NULL;
    int32_t rightSurfaceTypeCode = 1000000;

    ModelSurface* cerebellumSurfaceModel = NULL;
    ModelSurface* cerebellumSurfaceInflated = NULL;
    ModelSurface* cerebellumSurfaceVeryInflated = NULL;
    int32_t cerebellumSurfaceTypeCode = 1000000;
    
    ModelChart* chartModel = NULL;
    ModelChartTwo* chartTwoModel = NULL;
    ModelHistology* histologyModel(NULL);
    ModelMedia* mediaModel(NULL);
    ModelSurfaceMontage* surfaceMontageModel = NULL;
    ModelVolume* volumeModel = NULL;
    ModelWholeBrain* wholeBrainModel = NULL;
    
    const bool includeSingleSurfacesFlag = false;
    const bool includeOldChartFlag = false;
    
    for (std::vector<Model*>::const_iterator iter = allModels.begin();
         iter != allModels.end();
         iter++) {
        ModelSurface* surfaceModel =
            dynamic_cast<ModelSurface*>(*iter);
        if (surfaceModel != NULL) {
            if (includeSingleSurfacesFlag) {
                Surface* surface = surfaceModel->getSurface();
                StructureEnum::Enum structure = surface->getStructure();
                SurfaceTypeEnum::Enum surfaceType = surface->getSurfaceType();
                const int32_t surfaceTypeCode = SurfaceTypeEnum::toIntegerCode(surfaceType);
                
                switch (structure) {
                    case StructureEnum::CEREBELLUM:
                        if (surfaceTypeCode < cerebellumSurfaceTypeCode) {
                            cerebellumSurfaceModel = surfaceModel;
                            cerebellumSurfaceTypeCode = surfaceTypeCode;
                        }
                        if (surfaceType == SurfaceTypeEnum::INFLATED) {
                            cerebellumSurfaceInflated = surfaceModel;
                        }
                        else if (surfaceType == SurfaceTypeEnum::VERY_INFLATED) {
                            cerebellumSurfaceVeryInflated = surfaceModel;
                        }
                        break;
                    case StructureEnum::CORTEX_LEFT:
                        if (surfaceTypeCode < leftSurfaceTypeCode) {
                            leftSurfaceModel = surfaceModel;
                            leftSurfaceTypeCode = surfaceTypeCode;
                        }
                        if (surfaceType == SurfaceTypeEnum::INFLATED) {
                            leftSurfaceInflated = surfaceModel;
                        }
                        else if (surfaceType == SurfaceTypeEnum::VERY_INFLATED) {
                            leftSurfaceVeryInflated = surfaceModel;
                        }
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        if (surfaceTypeCode < rightSurfaceTypeCode) {
                            rightSurfaceModel = surfaceModel;
                            rightSurfaceTypeCode = surfaceTypeCode;
                        }
                        if (surfaceType == SurfaceTypeEnum::INFLATED) {
                            rightSurfaceInflated = surfaceModel;
                        }
                        else if (surfaceType == SurfaceTypeEnum::VERY_INFLATED) {
                            rightSurfaceVeryInflated = surfaceModel;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        else if (dynamic_cast<ModelSurfaceMontage*>(*iter) != NULL) {
            surfaceMontageModel = dynamic_cast<ModelSurfaceMontage*>(*iter);
        }
        else if (dynamic_cast<ModelVolume*>(*iter) != NULL) {
            volumeModel = dynamic_cast<ModelVolume*>(*iter);
        }
        else if (dynamic_cast<ModelWholeBrain*>(*iter) != NULL) {
            wholeBrainModel = dynamic_cast<ModelWholeBrain*>(*iter);
        }
        else if (dynamic_cast<ModelChart*>(*iter) != NULL) {
            if (includeOldChartFlag) {
                chartModel = dynamic_cast<ModelChart*>(*iter);
            }
        }
        else if (dynamic_cast<ModelChartTwo*>(*iter) != NULL) {
            chartTwoModel = dynamic_cast<ModelChartTwo*>(*iter);
        }
        else if (dynamic_cast<ModelMedia*>(*iter) != NULL) {
            mediaModel = dynamic_cast<ModelMedia*>(*iter);
        }
        else if (dynamic_cast<ModelHistology*>(*iter) != NULL) {
            histologyModel = dynamic_cast<ModelHistology*>(*iter);
        }
        else {
            CaretAssertMessage(0, AString("Unknown controller type: ") + (*iter)->getNameForGUI(true));
        }
    }
    
    if (cerebellumSurfaceInflated != NULL) {
        cerebellumSurfaceModel = cerebellumSurfaceInflated;
    }
    else if (cerebellumSurfaceVeryInflated != NULL) {
        cerebellumSurfaceModel = cerebellumSurfaceVeryInflated;
    }
    if (leftSurfaceInflated != NULL) {
        leftSurfaceModel = leftSurfaceInflated;
    }
    else if (leftSurfaceVeryInflated != NULL) {
        leftSurfaceModel = leftSurfaceVeryInflated;
    }
    if (rightSurfaceInflated != NULL) {
        rightSurfaceModel = rightSurfaceInflated;
    }
    else if (rightSurfaceVeryInflated != NULL) {
        rightSurfaceModel = rightSurfaceVeryInflated;
    }
    
    int32_t numberOfTabsNeeded = 0;
    if (surfaceMontageModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (volumeModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (wholeBrainModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (chartModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (chartTwoModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (leftSurfaceModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (rightSurfaceModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (cerebellumSurfaceModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (histologyModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (mediaModel != NULL) {
        numberOfTabsNeeded++;
    }
    
    const int32_t numberOfTabsToAdd = numberOfTabsNeeded - this->tabBar->count();
    for (int32_t i = 0; i < numberOfTabsToAdd; i++) {
        AString errorMessage;
        BrowserTabContent* tabContent = this->createNewTab(errorMessage);
        if (tabContent != NULL) {
            insertTabContentPrivate(InsertTabMode::APPEND,
                                    tabContent,
                                    -1);
        }
    }
    
    int32_t tabIndex = 0;
    tabIndex = loadIntoTab(tabIndex,
                           surfaceMontageModel);
    tabIndex = loadIntoTab(tabIndex,
                           volumeModel);
    tabIndex = loadIntoTab(tabIndex,
                           wholeBrainModel);
    tabIndex = loadIntoTab(tabIndex,
                           chartTwoModel);
    tabIndex = loadIntoTab(tabIndex,
                           chartModel);
    tabIndex = loadIntoTab(tabIndex,
                           leftSurfaceModel);
    tabIndex = loadIntoTab(tabIndex,
                           rightSurfaceModel);
    tabIndex = loadIntoTab(tabIndex,
                           cerebellumSurfaceModel);
    tabIndex = loadIntoTab(tabIndex,
                           histologyModel);
    tabIndex = loadIntoTab(tabIndex,
                           mediaModel);
    
    const int numTabs = this->tabBar->count();
    if (numTabs > 0) {
        this->tabBar->setCurrentIndex(0);

        Brain* brain = GuiManager::get()->getBrain();
        for (int32_t i = 0; i < numTabs; i++) {
            BrowserTabContent* btc = this->getTabContentFromTab(i);
            if (btc != NULL) {
                btc->getVolumeSurfaceOutlineSet()->selectSurfacesAfterSpecFileLoaded(brain,
                                                                                     true);
            }
        }
        
        /*
         * Set the default tab to whole brain, if present
         */
        int32_t surfaceTabIndex = -1;
        int32_t montageTabIndex = -1;
        int32_t wholeBrainTabIndex = -1;
        int32_t volumeTabIndex = -1;
        for (int32_t i = 0; i < numTabs; i++) {
            BrowserTabContent* btc = getTabContentFromTab(i);
            if (btc != NULL) {
                switch (btc->getSelectedModelType()) {
                    case ModelTypeEnum::MODEL_TYPE_INVALID:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
                        break;
                    case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE:
                        if (surfaceTabIndex < 0) {
                            surfaceTabIndex = i;
                        }
                        break;
                    case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                        if (montageTabIndex < 0) {
                            montageTabIndex = i;
                        }
                        break;
                    case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                        if (volumeTabIndex < 0) {
                            volumeTabIndex = i;
                        }
                        break;
                    case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                        if (wholeBrainTabIndex < 0) {
                            wholeBrainTabIndex = i;
                        }
                        break;
                    case ModelTypeEnum::MODEL_TYPE_CHART:
                        break;
                    case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                        break;
                }
            }
        }

        int32_t defaultTabIndex = 0;
        if (montageTabIndex >= 0) {
            defaultTabIndex = montageTabIndex;
        }
        else if (surfaceTabIndex >= 0) {
            defaultTabIndex = surfaceTabIndex;
        }
        else if (volumeTabIndex >= 0) {
            defaultTabIndex = volumeTabIndex;
        }
        else if (wholeBrainTabIndex >= 0) {
            defaultTabIndex = wholeBrainTabIndex;
        }
        
        this->tabBar->setCurrentIndex(defaultTabIndex);
    }
}

/**
 * Load a controller into the tab with the given index.
 * @param tabIndexIn
 *   Index of tab into which controller is loaded.  A
 *   new tab will be created, if needed.
 * @param controller
 *   Model that is to be displayed in the tab.  If
 *   NULL, this method does nothing.
 * @return
 *   Index of next tab after controller is displayed.
 *   If the input controller was NULL, the returned 
 *   value is identical to the input tab index.
 */
int32_t 
BrainBrowserWindowToolBar::loadIntoTab(const int32_t tabIndexIn,
                                       Model* controller)
{
    if (tabIndexIn < 0) {
        return -1;
    }
    
    int32_t tabIndex = tabIndexIn;
    
    if (controller != NULL) {
        if (tabIndex >= this->tabBar->count()) {
            AString errorMessage;
            BrowserTabContent* tabContent = this->createNewTab(errorMessage);
            if (tabContent == NULL) {
                return -1;
            }
            insertTabContentPrivate(InsertTabMode::APPEND,
                                    tabContent,
                                    -1);
            tabIndex = this->tabBar->count() - 1;
        }
        
        void* p = this->tabBar->tabData(tabIndex).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        btc->setSelectedModelType(controller->getModelType());
        
        ModelSurface* surfaceModel =
        dynamic_cast<ModelSurface*>(controller);
        if (surfaceModel != NULL) {
            btc->getSurfaceModelSelector()->setSelectedStructure(surfaceModel->getSurface()->getStructure());
            btc->getSurfaceModelSelector()->setSelectedSurfaceModel(surfaceModel);
            btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_SURFACE);
        }
        this->updateTabName(tabIndex);
        
        tabIndex++;
    }
 
    return tabIndex;
}

/**
 * Move all but the current tab to new windows.
 */
void 
BrainBrowserWindowToolBar::moveTabsToNewWindows()
{
    int32_t numTabs = this->tabBar->count();
    if (numTabs > 1) {
        const int32_t currentIndex = this->tabBar->currentIndex();
        
        QWidget* lastParent = this->parentWidget();
        if (lastParent == NULL) {
            lastParent = this;
        }
        for (int32_t i = (numTabs - 1); i >= 0; i--) {
            if (i != currentIndex) {
                void* p = this->tabBar->tabData(i).value<void*>();
                BrowserTabContent* btc = (BrowserTabContent*)p;

                EventBrowserWindowNew eventNewWindow(lastParent, btc);
                EventManager::get()->sendEvent(eventNewWindow.getPointer());
                if (eventNewWindow.isError()) {
                    QMessageBox::critical(this,
                                          "",
                                          eventNewWindow.getErrorMessage());
                    break;
                }
                else {
                    lastParent = eventNewWindow.getBrowserWindowCreated();
                    this->tabBar->setTabData(i, QVariant::fromValue((void*)NULL));
                    this->tabClosed(i,
                                    RemoveTabMode::INGORE_TAB_CONTENT);
                }
            }
        }
        
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Remove and return all tabs from this toolbar.
 * After this the window containing this toolbar 
 * will contain no tabs!
 *
 * @param allTabContent
 *    Will contain the content from the tabs upon return.
 */
void 
BrainBrowserWindowToolBar::removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent)
{
    allTabContent.clear();
    
    int32_t numTabs = this->tabBar->count();
    for (int32_t i = (numTabs - 1); i >= 0; i--) {
        void* p = this->tabBar->tabData(i).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        if (btc != NULL) {
            allTabContent.push_back(btc);
        }
        this->tabBar->setTabData(i, QVariant::fromValue((void*)NULL));
        this->tabClosed(i,
                        RemoveTabMode::INGORE_TAB_CONTENT);
    }
}

/**
 * Get content of all tabs
 *
 * @param allTabContent
 *    Will contain the content from the tabs upon return.
 */
void
BrainBrowserWindowToolBar::getAllTabContent(std::vector<BrowserTabContent*>& allTabContent) const
{
    allTabContent.clear();
    
    int32_t numTabs = this->tabBar->count();
    for (int32_t i = 0; i < numTabs; i++) {
        void* p = this->tabBar->tabData(i).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        if (btc != NULL) {
            allTabContent.push_back(btc);
        }
    }
}


/**
 * Remove the tab that contains the given tab content.
 * Note: The tab content is NOT deleted and the caller must
 * either delete it or move it into a window.
 * After this method completes, the windowo may contain no tabs.
 *
 * @param browserTabContent
 */
void 
BrainBrowserWindowToolBar::removeTabWithContent(BrowserTabContent* browserTabContent)
{
    int32_t numTabs = this->tabBar->count();
    for (int32_t i = 0; i < numTabs; i++) {
        void* p = this->tabBar->tabData(i).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        if (btc == browserTabContent) {
            this->tabBar->setTabData(i, QVariant::fromValue((void*)NULL));
            this->tabClosed(i,
                            RemoveTabMode::INGORE_TAB_CONTENT);
            if (this->tabBar->count() <= 0) {
                EventManager::get()->removeAllEventsFromListener(this);
            }
            break;
        }
    }    
}


/**
 * Select the next tab.
 */
void 
BrainBrowserWindowToolBar::nextTab()
{
    int32_t numTabs = this->tabBar->count();
    if (numTabs > 1) {
        int32_t tabIndex = this->tabBar->currentIndex();
        tabIndex++;
        if (tabIndex >= numTabs) {
            tabIndex = 0;
        }
        this->tabBar->setCurrentIndex(tabIndex);
    }
}

/**
 * Select the previous tab.
 */
void 
BrainBrowserWindowToolBar::previousTab()
{
    int32_t numTabs = this->tabBar->count();
    if (numTabs > 1) {
        int32_t tabIndex = this->tabBar->currentIndex();
        tabIndex--;
        if (tabIndex < 0) {
            tabIndex = numTabs - 1;
        }
        this->tabBar->setCurrentIndex(tabIndex);
    }
}

/**
 * Rename the current tab.
 */
void 
BrainBrowserWindowToolBar::renameTab()
{
    const int tabIndex = this->tabBar->currentIndex();
    if (tabIndex >= 0) {
        void* p = this->tabBar->tabData(tabIndex).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        AString currentName = btc->getUserTabName();
        bool ok = false;
        AString newName = QInputDialog::getText(this,
                                                "Set Tab Name",
                                                "New Name (empty to reset)",
                                                QLineEdit::Normal,
                                                currentName,
                                                &ok);
        if (ok) {
            btc->setUserTabName(newName);
            this->updateTabName(tabIndex);
        }
    }

    
}

/**
 * Update the names of all tabs.
 */
void
BrainBrowserWindowToolBar::updateAllTabNames()
{
    for (int32_t i = 0; i < this->tabBar->count(); i++) {
        updateTabName(i);
    }
}

/**
 * Update the name of the tab at the given index.  The 
 * name is obtained from the tabs browser content.
 *
 * @param tabIndex
 *   Index of tab.
 */
void 
BrainBrowserWindowToolBar::updateTabName(const int32_t tabIndex)
{
    int32_t tabIndexForUpdate = tabIndex;
    if (tabIndexForUpdate < 0) {
        tabIndexForUpdate = this->tabBar->currentIndex();
    }
    void* p = this->tabBar->tabData(tabIndexForUpdate).value<void*>();
    AString newName = "";
    BrowserTabContent* btc = (BrowserTabContent*)p;   
    if (btc != NULL) {
        newName = btc->getTabName();
    }
    this->tabBar->setTabText(tabIndexForUpdate, newName);
}

/**
 * Close the selected tab.  This method is typically
 * called by the BrowswerWindow's File Menu.
 */
void 
BrainBrowserWindowToolBar::closeSelectedTabFromFileMenu()
{
    tabCloseSelected(this->tabBar->currentIndex());
}

/**
 * Called when context menu requested for the tab bar.
 *
 * @param pos
 *     Postion of mouse in parent widget.
 */
void
BrainBrowserWindowToolBar::showTabMenu(const QPoint& pos)
{
    const int tabIndexUnderMouseIndex = this->tabBar->tabAt(pos);
    
    if (tabIndexUnderMouseIndex >= 0) {
        BrainBrowserWindowToolBarTabPopUpMenu menu(this,
                                                   tabIndexUnderMouseIndex);
        menu.exec(mapToGlobal(pos));
    }
    else {
        QMenu menu(this->tabBar);
        menu.move(mapToGlobal(pos));
        QAction* addAction = menu.addAction("Add New Tab");
        
        const QAction* selectedAction = menu.exec();
        if (selectedAction == addAction) {
            addNewTab();
        }
        else if (selectedAction != NULL) {
            CaretAssertMessage(0, "Has a new action been added to the menu");
        }
    }
}

/**
 * Called when the selected tab is changed.
 * @param index
 *    Index of selected tab.
 */
void 
BrainBrowserWindowToolBar::selectedTabChanged(int indx)
{
    this->updateTabName(indx);
    this->updateToolBar();
    this->updateToolBox();
    emit viewedModelChanged();
    
    if (m_parentBrainBrowserWindow != NULL) {
        if (m_parentBrainBrowserWindow->isTileTabsSelected()) {
            const BrowserTabContent* btc = getTabContentFromSelectedTab();
            if (btc != NULL) {
                if (m_tileTabsHighlightingTimerEnabledFlag) {
                    m_tabIndexForTileTabsHighlighting = btc->getTabNumber();
                    
                    /*
                     * Short timer that will reset the tab highlighting
                     */
                    if (m_tileTabsHighlightingTimer == NULL) {
                        m_tileTabsHighlightingTimer = new QTimer(this);
                        QObject::connect(m_tileTabsHighlightingTimer, &QTimer::timeout,
                                         this, &BrainBrowserWindowToolBar::resetTabIndexForTileTabsHighlighting);
                    }
                    
                    /*
                     * Note: There is no need to 'stop' the timer if it is running
                     * as the start() method will stop and restart the timer
                     */
                    const int timeInMilliseconds = 750;
                    m_tileTabsHighlightingTimer->setSingleShot(true);
                    m_tileTabsHighlightingTimer->start(timeInMilliseconds);
                }
            }
        }
    }

    /*
     * Graphics must be updated so border is visible around the selected tab
     */
    this->updateGraphicsWindow();
}

/**
 * Reset the tab index for tab tile highlighting.
 */
void
BrainBrowserWindowToolBar::resetTabIndexForTileTabsHighlighting()
{
    m_tabIndexForTileTabsHighlighting = -1;
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Called when tab bar mouse pressed.
 */
void
BrainBrowserWindowToolBar::tabBarMousePressedSlot(QMouseEvent* event)
{
    /*
     * Only block if LEFT button is used without modifiers.  Otherwise, if user
     * performs right-click with mouse to display menu, the "mouseReleased" is
     * never called by Qt.  In addition, we don't want to block signals when
     * the pop-up menu is displayed.
     *
     * Dragging tabs is slow because as the tab is moved, there are many graphics
     * and user-interface updates each time the tab changes.  So, disable these updates 
     * during the time the mouse is pressed and until it is released.  Note that we
     * block the "all windows graphics" update but not the individual window update.
     * The individual window graphics update is needed to draw the box around the selected tab.
     */
    if (event->button() == Qt::LeftButton) {
        EventManager::get()->blockEvent(EventTypeEnum::EVENT_USER_INTERFACE_UPDATE, true);
        EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS, true);
    }
}

/**
 * Called when tab bar mouse released.
 */
void
BrainBrowserWindowToolBar::tabBarMouseReleasedSlot(QMouseEvent* event)
{
    /*
     * Enable user-interface updates and graphics drawing since any tab
     * dragging has finished.
     */
    if (event->button() == Qt::LeftButton) {
        EventManager::get()->blockEvent(EventTypeEnum::EVENT_USER_INTERFACE_UPDATE, false);
        EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS, false);
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());

        /**
         * Causes graphics and user-interface to update.
         * A box is drawn around selected tab.
         */
        selectedTabChanged(tabBar->currentIndex());
    }
}

/**
 * Gets called when user closes a tab by clicking the tab's 'X'.
 *
 * @param tabIndex
 *     Index of the tab that was closed.
 */
void
BrainBrowserWindowToolBar::tabCloseSelected(int tabIndex)
{
    if ((tabIndex >= 0)
        && (tabIndex < this->tabBar->count())) {
        tabClosed(tabIndex,
                  RemoveTabMode::CLOSE_TAB_CONTENT_FOR_REOPENING);
    }
    this->updateGraphicsWindow();
}

/**
 * Close a tab.
 *
 * @param tabIndex
 *    Index of tab that was closed.
 * @param removeTabMode
 *    Mode for removing tab
 */
void
BrainBrowserWindowToolBar::tabClosed(int tabIndex,
                                     const RemoveTabMode removeTabMode)
{
    CaretAssertArrayIndex(this-tabBar->tabData(), this->tabBar->count(), tabIndex);
    this->removeTab(tabIndex,
                    removeTabMode);
    
    if (this->isDestructionInProgress == false) {
        this->updateToolBar();
        this->updateToolBox();
        emit viewedModelChanged();
    }
}

/**
 * Emit the viewedModelChanged() signal
 * This method is used by friend classes
 */
void
BrainBrowserWindowToolBar::emitViewModelChangedSignal()
{
    emit viewedModelChanged();
}

/**
 * Called when a tab has been moved.
 *
 * @param from
 *    Previous location of tab.
 * @param to
 *    New location of tab.
 */
void
BrainBrowserWindowToolBar::tabMoved(int /*from*/, int /*to*/)
{
    /* No need to redraw as the selectedTabChanged() is also called when a tab is moved */
}

/**
 * Remove the tab at the given index.
 * @param index
 *    Index of the tab
 * @param removeTabMode
 *    Mode for removing tab
 */
void 
BrainBrowserWindowToolBar::removeTab(int tabIndex,
                                     const RemoveTabMode removeTabMode)
{
    CaretAssertArrayIndex(this-tabBar->tabData(), this->tabBar->count(), tabIndex);
    
    void* p = this->tabBar->tabData(tabIndex).value<void*>();
    if (p != NULL) {
        BrowserTabContent* btc = (BrowserTabContent*)p;
        
        switch (removeTabMode) {
            case RemoveTabMode::CLOSE_TAB_CONTENT_FOR_REOPENING:
            {
                EventBrowserTabClose closeTabEvent(btc,
                                                   btc->getTabNumber(),
                                                   this->browserWindowIndex,
                                                   tabIndex);
                EventManager::get()->sendEvent(closeTabEvent.getPointer());
                if (closeTabEvent.isError()) {
                    WuQMessageBox::errorOk(this,
                                           closeTabEvent.getErrorMessage());
                }
            }
                break;
            case RemoveTabMode::DELETE_TAB_CONTENT:
            {
                EventBrowserTabDelete deleteTabEvent(btc,
                                                     btc->getTabNumber(),
                                                     this->browserWindowIndex);
                EventManager::get()->sendEvent(deleteTabEvent.getPointer());
                if (deleteTabEvent.isError()) {
                    WuQMessageBox::errorOk(this,
                                           deleteTabEvent.getErrorMessage());
                }
            }
                break;
            case RemoveTabMode::INGORE_TAB_CONTENT:
                break;
        }
    }
    
    this->tabBar->blockSignals(true);
    this->tabBar->removeTab(tabIndex);
    this->tabBar->blockSignals(false);

    const int numOpenTabs = this->tabBar->count();
    this->tabBar->setTabsClosable(numOpenTabs > 1);    
}

/**
 * Update the toolbar.
 */
void 
BrainBrowserWindowToolBar::updateToolBar()
{
    if (this->isDestructionInProgress) {
        return;
    }

    /*
     * If this is true, it indicates that one of the 'update' methods has mistakenly emitted a signal
     * or an update.
     */
    if (m_performingUpdateFlag) {
        const AString msg("During an update of the toolbar, a signal was issued and needs to be blocked.\n"
                          "Set a break on this line and go through the call stack to find where the\n"
                          "signal was emitted and block it.");
        CaretLogSevere(msg);
    }
    m_performingUpdateFlag = true;
    
    /*
     * If there are no models, close all but the first tab.
     */
    EventModelGetAll getAllModelsEvent;
    EventManager::get()->sendEvent(getAllModelsEvent.getPointer());
    if (getAllModelsEvent.getFirstModel() == NULL) {
        for (int i = (this->tabBar->count() - 1); i >= 0; i--) {
            this->removeTab(i,
                            RemoveTabMode::DELETE_TAB_CONTENT);
        }
    }
    
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    
    const ModelTypeEnum::Enum viewModel = this->updateViewWidget(browserTabContent);
    
    bool showOrientationWidget = false;
    bool showWholeBrainSurfaceOptionsWidget = false;
    bool showSingleSurfaceOptionsWidget = false;
    bool showSurfaceMontageOptionsWidget = false;
    bool showVolumeIndicesWidget = false;
    bool showVolumePlaneWidget = false;
    bool showVolumeMontageWidget = false;
    bool showVolumeMprWidget = false;
    
    bool showChartOneAxesWidget = false;
    bool showChartOneTypeWidget = false;
    bool showChartTwoTypeWidget = false;
    bool showChartOneAttributesWidget = false;
    bool showChartTwoOrientationWidget = false;
    bool showChartTwoAttributesWidget = false;
    bool showChartTwoAxesWidget = false;
    
    bool showHistologyWidget = false;
    bool showImageWidget = false;

    bool showModeWidget = true;
    bool showViewWidget = true;
    bool showTabMiscWidget = true;
    
    bool showAnnotateModeWidget(false);
    bool showBorderModeWidget(false);
    bool showFociModeWidget(false);
    bool showImageModeWidget(false);
    bool showTileModeWidget(false);
    bool showVolumeModeWidget(false);
    
    bool showViewModeWidgetsFlag(false);
    
    /*
     * Viewed models may not be compatible
     * with all user input modes
     */
    bool borderCompatibleViewFlag(false);
    bool fociCompatibleViewFlag(false);
    bool imageCompatibleViewFlag(false);
    bool volumeEditCompatibleViewFlag(false);
    switch (viewModel) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            fociCompatibleViewFlag = true;
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            borderCompatibleViewFlag = true;
            fociCompatibleViewFlag   = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            borderCompatibleViewFlag = true;
            fociCompatibleViewFlag   = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            fociCompatibleViewFlag       = true;
            imageCompatibleViewFlag      = true;
            volumeEditCompatibleViewFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            borderCompatibleViewFlag = true;
            fociCompatibleViewFlag   = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            break;
    }

    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const ToolBarWidthModeEnum::Enum widthMode = prefs->getToolBarWidthMode();
    bool wideFlag(false);
    switch (widthMode) {
        case ToolBarWidthModeEnum::STANDARD:
            wideFlag = false;
            break;
        case ToolBarWidthModeEnum::WIDE:
            wideFlag = true;
            break;
    }
    
    /*
     * Enable widgets for selected input mode
     */
    CaretAssert(this->selectedUserInputProcessor);
    switch (this->selectedUserInputProcessor->getUserInputMode()) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            if (wideFlag) {
                showViewModeWidgetsFlag = true;
            }
            showAnnotateModeWidget = true;
            break;
        case UserInputModeEnum::Enum::BORDERS:
            showViewModeWidgetsFlag = true;
            showBorderModeWidget    = borderCompatibleViewFlag;
            break;
        case UserInputModeEnum::Enum::FOCI:
            showViewModeWidgetsFlag = true;
            showFociModeWidget      = fociCompatibleViewFlag;
            break;
        case UserInputModeEnum::Enum::IMAGE:
            showViewModeWidgetsFlag = true;
            showImageModeWidget     = imageCompatibleViewFlag;
            break;
        case UserInputModeEnum::Enum::INVALID:
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            if (wideFlag) {
                showViewModeWidgetsFlag = true;
            }
            showTileModeWidget = true;
            break;
        case UserInputModeEnum::Enum::VIEW:
            showViewModeWidgetsFlag = true;
            break;
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            showViewModeWidgetsFlag = true;
            showVolumeModeWidget    = volumeEditCompatibleViewFlag;
            break;
    }
    
    /*
     * Note that view mode widgets are shown in other modes.
     */
    if (showViewModeWidgetsFlag) {
        showViewWidget   = true;
        
        switch (viewModel) {
            case ModelTypeEnum::MODEL_TYPE_INVALID:
                break;
            case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
                showHistologyWidget = true;
                showOrientationWidget = true;
                break;
            case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                showImageWidget = true;
                showOrientationWidget = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE:
                showOrientationWidget = true;
                showSingleSurfaceOptionsWidget = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                showOrientationWidget = true;
                showSurfaceMontageOptionsWidget = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                showVolumeIndicesWidget = true;
                showOrientationWidget = true;
                showVolumePlaneWidget = true;
                showVolumeMontageWidget = true;
                showVolumeMprWidget = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                showOrientationWidget = true;
                showWholeBrainSurfaceOptionsWidget = true;
                showVolumeIndicesWidget = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_CHART:
            {
                ModelChart* modelChart = browserTabContent->getDisplayedChartOneModel();
                if (modelChart != NULL) {
                    showChartOneTypeWidget = true;
                    switch (modelChart->getSelectedChartOneDataType(browserTabContent->getTabNumber())) {
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                            showChartOneAttributesWidget = true;
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                            showChartOneAttributesWidget = true;
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                            showChartOneAxesWidget = true;
                            showChartOneAttributesWidget = true;
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                            showChartOneAxesWidget = true;
                            showChartOneAttributesWidget = true;
                            break;
                        case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                            showChartOneAxesWidget = true;
                            showChartOneAttributesWidget = true;
                            break;
                    }
                }
            }
                break;
            case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            {
                ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
                if (modelChartTwo != NULL) {
                    switch (modelChartTwo->getSelectedChartTwoDataType(browserTabContent->getTabNumber())) {
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                            showChartTwoAxesWidget = true;
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                            showChartTwoAxesWidget = true;
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                            showChartTwoAxesWidget = true;
                            break;
                        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                            showChartTwoAxesWidget = true;
                            showChartTwoOrientationWidget = true;
                            showChartTwoAttributesWidget  = true;
                            break;
                    }
                    
                    showChartTwoTypeWidget = true;
                }
            }
                break;
        }
    }
    
    /*
     * WB-900
     * Do not show chart orientation widget since charts no longer use
     * transformation matrices but instead pan/zoom by adjusting the
     * axis min/max.  Need to work on conversion (if possible) from
     * matrix to axis min/max.
     */
    showChartTwoOrientationWidget = false;
    
    
    /*
     * Need to turn off display of all widgets, 
     * otherwise, the toolbar width may be overly
     * expanded with empty space as other widgets
     * are turned on and off.
     */
    this->modeWidget->setVisible(false);
    this->viewWidget->setVisible(false);
    this->orientationWidget->setVisible(false);
    this->wholeBrainSurfaceOptionsWidget->setVisible(false);
    this->singleSurfaceSelectionWidget->setVisible(false);
    this->surfaceMontageSelectionWidget->setVisible(false);
    this->chartTypeWidget->setVisible(false);
    this->chartTypeTwoWidget->setVisible(false);
    this->chartAxesWidget->setVisible(false);
    this->chartAttributesWidget->setVisible(false);
    this->chartTwoOrientationWidget->setVisible(false);
    this->chartTwoAttributesWidget->setVisible(false);
    this->chartTwoOrientedAxesWidget->setVisible(false);
    this->volumeIndicesWidget->setVisible(false);
    this->volumePlaneWidget->setVisible(false);
    this->volumeMontageWidget->setVisible(false);
    this->volumeMprWidget->setVisible(false);
    this->tabMiscWidget->setVisible(false);
    
    this->annotateModeWidget->setVisible(false);
    this->bordersModeWidget->setVisible(false);
    this->fociModeWidget->setVisible(false);
    this->imageModeWidget->setVisible(false);
    this->tileModeWidget->setVisible(false);
    this->volumeModeWidget->setVisible(false);

    updateToolBarComponents(browserTabContent);
    
    this->annotateModeWidget->setVisible(showAnnotateModeWidget);
    this->bordersModeWidget->setVisible(showBorderModeWidget);
    this->fociModeWidget->setVisible(showFociModeWidget);
    this->imageModeWidget->setVisible(showImageModeWidget);
    this->tileModeWidget->setVisible(showTileModeWidget);
    this->volumeModeWidget->setVisible(showVolumeModeWidget);
    
    this->modeWidget->setVisible(showModeWidget);
    this->viewWidget->setVisible(showViewWidget);
    this->orientationWidget->setVisible(showOrientationWidget);
    this->wholeBrainSurfaceOptionsWidget->setVisible(showWholeBrainSurfaceOptionsWidget);
    this->singleSurfaceSelectionWidget->setVisible(showSingleSurfaceOptionsWidget);
    this->surfaceMontageSelectionWidget->setVisible(showSurfaceMontageOptionsWidget);
    this->chartTypeWidget->setVisible(showChartOneTypeWidget);
    this->chartTypeTwoWidget->setVisible(showChartTwoTypeWidget);
    this->chartAxesWidget->setVisible(showChartOneAxesWidget);
    this->chartAttributesWidget->setVisible(showChartOneAttributesWidget);
    this->chartTwoOrientationWidget->setVisible(showChartTwoOrientationWidget);
    this->chartTwoAttributesWidget->setVisible(showChartTwoAttributesWidget);
    this->chartTwoOrientedAxesWidget->setVisible(showChartTwoAxesWidget);
    this->histologyWidget->setVisible(showHistologyWidget);
    this->imageWidget->setVisible(showImageWidget);
    this->volumeIndicesWidget->setVisible(showVolumeIndicesWidget);
    this->volumePlaneWidget->setVisible(showVolumePlaneWidget);
    this->volumeMontageWidget->setVisible(showVolumeMontageWidget);
    const bool allowVolumeMprWidgetFlag(false);
    if (allowVolumeMprWidgetFlag) {
        this->volumeMprWidget->setVisible(showVolumeMprWidget);
    }
    this->tabMiscWidget->setVisible(showTabMiscWidget);
    
    updateToolBarComponents(browserTabContent);
    
    this->updateAllTabNames();
    
    if (m_parentBrainBrowserWindow != NULL) {
        if (m_parentBrainBrowserWindow->isFullScreen()) {
            this->setVisible(false);
        }
        else {
            this->setVisible(true);
        }
    }
    
    m_performingUpdateFlag = false;
}

/**
 * Update the components in the toolbar with the given tab content.
 *
 * @param browserTabContent
 *    The current tab content.
 */
void
BrainBrowserWindowToolBar::updateToolBarComponents(BrowserTabContent* browserTabContent)
{
    if (browserTabContent != NULL) {
        this->updateOrientationWidget(browserTabContent);
        this->updateWholeBrainSurfaceOptionsWidget(browserTabContent);
        this->updateVolumeIndicesWidget(browserTabContent);
        this->updateSingleSurfaceOptionsWidget(browserTabContent);
        this->updateSurfaceMontageOptionsWidget(browserTabContent);
        this->updateChartTypeWidget(browserTabContent);
        this->updateChartTypeTwoWidget(browserTabContent);
        this->updateChartAxesWidget(browserTabContent);
        this->updateChartAttributesWidget(browserTabContent);
        this->updateChartTwoOrientationWidget(browserTabContent);
        this->updateChartTwoAttributesWidget(browserTabContent);
        this->updateChartTwoOrientedAxesWidget(browserTabContent);
        this->updateVolumeMontageWidget(browserTabContent);
        this->updateVolumeMprWidget(browserTabContent);
        this->updateVolumePlaneWidget(browserTabContent);
        this->updateModeWidget(browserTabContent);
        this->updateViewWidget(browserTabContent);
        this->updateTabOptionsWidget(browserTabContent);
        this->updateHistologyWidget(browserTabContent);
        this->updateImageWidget(browserTabContent);
    }
}

/**
 * Create the view widget.
 *
 * @return The view widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createViewWidget()
{
    m_viewToolBarComponent = new BrainBrowserWindowToolBarView(m_objectNamePrefix,
                                                               this);
    QWidget* w = this->createToolWidget("Display",
                                        m_viewToolBarComponent, 
                                        WIDGET_PLACEMENT_NONE,
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    return w;
}

/**
 * Update the view widget.
 * 
 * @param browserTabContent
 *   Content in the tab.
 * @return 
 *   An enumerated type indicating the type of model being viewed.
 */
ModelTypeEnum::Enum
BrainBrowserWindowToolBar::updateViewWidget(BrowserTabContent* browserTabContent)
{
    ModelTypeEnum::Enum modelType = ModelTypeEnum::MODEL_TYPE_INVALID;
    if (browserTabContent != NULL) {
        modelType = browserTabContent->getSelectedModelType();
    }
    m_viewToolBarComponent->updateContent(browserTabContent);
    return modelType;
}

/**
 * Create the orientation widget.
 *
 * @return  The orientation widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createOrientationWidget()
{
    m_orientationToolBarComponent = new BrainBrowserWindowToolBarOrientation(m_objectNamePrefix,
                                                                             this);
    QWidget* orientWidget = this->createToolWidget("Orientation", 
                                                   m_orientationToolBarComponent,
                                                   WIDGET_PLACEMENT_LEFT, 
                                                   WIDGET_PLACEMENT_TOP,
                                                   0);
    orientWidget->setVisible(false);
    
    return orientWidget;
}

/**
 * Update the orientation widget.
 * 
 * @param browserTabContent
 *   Content of browser tab.
 */
void 
BrainBrowserWindowToolBar::updateOrientationWidget(BrowserTabContent* browserTabContent)
{
    if (this->orientationWidget->isHidden()) {
        return;
    }
    
    m_orientationToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the whole brain surface options widget.
 *
 * @return The whole brain surface options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createWholeBrainSurfaceOptionsWidget()
{
    m_allSurfaceToolBarComponent = new BrainBrowserWindowToolBarAllSurface(m_objectNamePrefix,
                                                                           this);
    
    QWidget* w = this->createToolWidget("Surface Viewing", 
                                        m_allSurfaceToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the whole brain surface options widget.
 * 
 * @param browserTabContent
 *   Content of browser tab.
 */
void 
BrainBrowserWindowToolBar::updateWholeBrainSurfaceOptionsWidget(BrowserTabContent* browserTabContent)
{
    if (this->wholeBrainSurfaceOptionsWidget->isHidden()) {
        return;
    }
 
    m_allSurfaceToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the volume indices widget.
 *
 * @return  The volume indices widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumeIndicesWidget()
{
    m_sliceSelectionComponent = new BrainBrowserWindowToolBarSliceSelection(this,
                                                                            m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Slice Indices/Coords",
                                        m_sliceSelectionComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume indices widget.
 * 
 * @param browserTabContent
 *   Content of browser tab.
 */
void 
BrainBrowserWindowToolBar::updateVolumeIndicesWidget(BrowserTabContent* browserTabContent)
{
    if (this->volumeIndicesWidget->isHidden()) {
        return;
    }
    m_sliceSelectionComponent->updateContent(browserTabContent);
}

/**
 * Create the mode widget.
 *
 * @return The mode widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createModeWidget()
{
    /*
     * Annotations
     */
    this->modeInputModeAnnotationsRadioButton = new QRadioButton("Annotate");
    this->modeInputModeAnnotationsRadioButton->setToolTip("Perform annotate operations with mouse");
    this->modeInputModeAnnotationsRadioButton->setObjectName(m_objectNamePrefix
                                                  + ":Mode:Annotate");
    
    /*
     * Borders 
     */ 
    this->modeInputModeBordersRadioButton = new QRadioButton("Edit Borders");
    this->modeInputModeBordersRadioButton->setToolTip("Perform border operations with mouse");
    this->modeInputModeBordersRadioButton->setObjectName(m_objectNamePrefix
                                                        + ":Mode:Border");
    
    /*
     * Foci
     */
    this->modeInputModeFociRadioButton = new QRadioButton("Edit Foci");
    this->modeInputModeFociRadioButton->setToolTip("Perform foci operations with mouse");
    this->modeInputModeFociRadioButton->setObjectName(m_objectNamePrefix
                                                        + ":Mode:Foci");
    
    /*
     * Image
     */
    const bool showImageButtonFlag(false);
    this->modeInputModeImageRadioButton = NULL;
    if (showImageButtonFlag) {
        this->modeInputModeImageRadioButton = new QRadioButton("Image");
        this->modeInputModeImageRadioButton->setToolTip("Edit Image Control Points");
        this->modeInputModeImageRadioButton->setObjectName(m_objectNamePrefix
                                                            + ":Mode:Image");
    }
    
    /*
     * Tile tabs manual layout editing
     */
    const AString tileToolTip("<html>"
                              "Edit Tile Tabs Manual Configuration<br><br>"
                              "This button is enabled when: <br>"
                              "  * Tile Tabs is enabled (View Menu -> Enter Tile Tabs)<br>"
                              "  * Selected Tile Tab Active Configuration Type is Manual (View Menu -> Edit Tile Tabs Configuration)"
                              "</html>");
    this->modeInputModeTileTabsManualLayoutRadioButton = new QRadioButton("Tile Layout");
    this->modeInputModeTileTabsManualLayoutRadioButton->setToolTip(tileToolTip);
    this->modeInputModeTileTabsManualLayoutRadioButton->setObjectName(m_objectNamePrefix
                                                                 + "Mode:TileTabsManualLayout");
    
    /*
     * Volume Edit
     */
    this->modeInputVolumeEditRadioButton = new QRadioButton("Edit Voxels");
    this->modeInputVolumeEditRadioButton->setToolTip("Edit volume voxels");
    this->modeInputVolumeEditRadioButton->setObjectName(m_objectNamePrefix
                                                        + ":Mode:Volume");
    
    /*
     * View Mode
     */
    this->modeInputModeViewRadioButton = new QRadioButton("View");
    this->modeInputModeViewRadioButton->setToolTip("Perform viewing operations with mouse\n"
                                                                 "\n"
                                                                 "Identify: Click left mouse button (might cause rotation)\n"
#ifdef CARET_OS_MACOSX
                                                                 "Identify: Click left mouse button while keyboard shift and apple keys are down (prevents rotation)\n"
#else // CARET_OS_MACOSX
                                                                 "Identify: Click left mouse button while keyboard shift and controls keys are down (prevents rotation)\n"
#endif // CARET_OS_MACOSX
                                                                 "Pan:      Move mouse with left mouse button down and keyboard shift key down\n"
                                                                 "Rotate:   Move mouse with left mouse button down (Except Volume Orthogonal)\n"
                                                                 "Slice Scolling: Move mouse with left mouse button down (Volume Orthogonal Only)\n"
#ifdef CARET_OS_MACOSX
                                                                 "Zoom:     Move mouse with left mouse button down and keyboard apple key down"
#else // CARET_OS_MACOSX
                                                                 "Zoom:     Move mouse with left mouse button down and keyboard control key down"
#endif // CARET_OS_MACOSX
                                                                 );
    this->modeInputModeViewRadioButton->setObjectName(m_objectNamePrefix
                                                        + ":Mode:View");
    
    this->modeInputModeRadioButtonGroup = new QButtonGroup(this);
    this->modeInputModeRadioButtonGroup->addButton(this->modeInputModeAnnotationsRadioButton);
    this->modeInputModeRadioButtonGroup->addButton(this->modeInputModeBordersRadioButton);
    this->modeInputModeRadioButtonGroup->addButton(this->modeInputModeFociRadioButton);
    if (this->modeInputModeImageRadioButton != NULL) {
        this->modeInputModeRadioButtonGroup->addButton(this->modeInputModeImageRadioButton);
    }
    this->modeInputModeRadioButtonGroup->addButton(this->modeInputModeViewRadioButton);
    this->modeInputModeRadioButtonGroup->addButton(this->modeInputModeTileTabsManualLayoutRadioButton);
    this->modeInputModeRadioButtonGroup->addButton(this->modeInputVolumeEditRadioButton);
    QObject::connect(this->modeInputModeRadioButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
                     this, &BrainBrowserWindowToolBar::modeInputModeRadioButtonClicked);
    this->modeInputModeRadioButtonGroup->setExclusive(true);
    
    WuQMacroManager::instance()->addMacroSupportToObject(this->modeInputModeAnnotationsRadioButton,
                                                         "Select annotate mode");
    WuQMacroManager::instance()->addMacroSupportToObject(this->modeInputModeBordersRadioButton,
                                                         "Select border mode");
    WuQMacroManager::instance()->addMacroSupportToObject(this->modeInputModeFociRadioButton,
                                                         "Select foci mode");
    if (modeInputModeImageRadioButton != NULL) {
        WuQMacroManager::instance()->addMacroSupportToObject(this->modeInputModeImageRadioButton,
                                                             "Select image mode");
    }
    WuQMacroManager::instance()->addMacroSupportToObject(this->modeInputModeTileTabsManualLayoutRadioButton,
                                                         "Select Tile Tabs Manual Layout Editing");
    WuQMacroManager::instance()->addMacroSupportToObject(this->modeInputModeViewRadioButton,
                                                         "Select view mode");
    WuQMacroManager::instance()->addMacroSupportToObject(this->modeInputVolumeEditRadioButton,
                                                         "Select volume mode");
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
#ifdef CARET_OS_MACOSX
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 2);
#else
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
#endif
    layout->addWidget(this->modeInputModeViewRadioButton);
    layout->addWidget(this->modeInputModeTileTabsManualLayoutRadioButton);
    layout->addWidget(this->modeInputModeAnnotationsRadioButton);
    layout->addWidget(this->modeInputModeBordersRadioButton);
    layout->addWidget(this->modeInputModeFociRadioButton);
    if (this->modeInputModeImageRadioButton != NULL) {
        layout->addWidget(this->modeInputModeImageRadioButton);
    }
    layout->addWidget(this->modeInputVolumeEditRadioButton);
    
    this->modeWidgetGroup = new WuQWidgetObjectGroup(this);
    this->modeWidgetGroup->add(this->modeInputModeRadioButtonGroup);
    
    QWidget* w = this->createToolWidget("Mode", 
                                        widget, 
                                        WIDGET_PLACEMENT_RIGHT,
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    return w;
}

/**
 * Called when a tools input mode button is clicked.
 * @param action
 *    Action of tool button that was clicked.
 */
void 
BrainBrowserWindowToolBar::modeInputModeRadioButtonClicked(QAbstractButton* button)
{
    BrowserTabContent* tabContent = this->getTabContentFromSelectedTab();
    if (tabContent == NULL) {
        return;
    }

    const UserInputModeEnum::Enum currentMode = this->selectedUserInputProcessor->getUserInputMode();
    
    UserInputModeEnum::Enum inputMode = UserInputModeEnum::Enum::INVALID;
    
    if (button == this->modeInputModeAnnotationsRadioButton) {
        if (currentMode !=  UserInputModeEnum::Enum::ANNOTATIONS) {
            CaretAssert(m_parentBrainBrowserWindow);
            if ( ! m_parentBrainBrowserWindow->changeInputModeToAnnotationsWarningDialog()) {
                /*
                 * Since mode is rejected, need to update toolbar
                 */
                updateModeWidget(getTabContentFromSelectedTab());
                return;
            }
        }
        inputMode = UserInputModeEnum::Enum::ANNOTATIONS;
    }
    else if (button == this->modeInputModeBordersRadioButton) {
        inputMode = UserInputModeEnum::Enum::BORDERS;
        
        /*
         * If borders are not displayed, display them
         */
        DisplayPropertiesBorders* dpb = GuiManager::get()->getBrain()->getDisplayPropertiesBorders();
        const int32_t browserTabIndex = tabContent->getTabNumber();
        const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(browserTabIndex);
        if (dpb->isDisplayed(displayGroup,
                             browserTabIndex) == false) {
            dpb->setDisplayed(displayGroup, 
                              browserTabIndex,
                              true);
        }
    }
    else if (button == this->modeInputModeFociRadioButton) {
        inputMode = UserInputModeEnum::Enum::FOCI;
    }
    else if ((button == this->modeInputModeImageRadioButton)
             && (this->modeInputModeImageRadioButton != NULL)) {
        inputMode = UserInputModeEnum::Enum::IMAGE;
    }
    else if (button == this->modeInputModeTileTabsManualLayoutRadioButton) {
        inputMode = UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING;
        
        CaretAssert(m_parentBrainBrowserWindow);
        BrowserWindowContent* browserWindowContent = m_parentBrainBrowserWindow->getBrowerWindowContent();
        CaretAssert(browserWindowContent);
        if ( ! browserWindowContent->isTileTabsEnabled()) {
            browserWindowContent->setTileTabsEnabled(true);
        }
    }
    else if (button == this->modeInputVolumeEditRadioButton) {
        inputMode = UserInputModeEnum::Enum::VOLUME_EDIT;
    }
    else if (button == this->modeInputModeViewRadioButton) {
        inputMode = UserInputModeEnum::Enum::VIEW;
    }
    else {
        CaretAssertMessage(0, "Tools input mode action is invalid, new action added???");
    }
    
    EventManager::get()->sendEvent(EventGetOrSetUserInputModeProcessor(this->browserWindowIndex,
                                                                       inputMode).getPointer());
    this->updateUserInterface();
    this->updateGraphicsWindow();

    this->updateModeWidget(tabContent);
    this->updateDisplayedModeUserInputWidget();
}

/**
 * Update the tools widget.
 * 
 * @param browserTabContent
 *   Content of browser tab.
 */
void 
BrainBrowserWindowToolBar::updateModeWidget(BrowserTabContent* /*browserTabContent*/)
{
    if (this->modeWidget->isHidden()) {
        return;
    }
    
    CaretAssert(m_parentBrainBrowserWindow);
    
    this->modeWidgetGroup->blockAllSignals(true);
    
    switch (this->selectedUserInputProcessor->getUserInputMode()) {
        case UserInputModeEnum::Enum::INVALID:
            /* may get here when program is exiting and widgets are being destroyed */
            break;
        case UserInputModeEnum::Enum::ANNOTATIONS:
            this->modeInputModeAnnotationsRadioButton->setChecked(true);
            break;
        case UserInputModeEnum::Enum::BORDERS:
            this->modeInputModeBordersRadioButton->setChecked(true);
            break;
        case UserInputModeEnum::Enum::FOCI:
            this->modeInputModeFociRadioButton->setChecked(true);
            break;
        case UserInputModeEnum::Enum::IMAGE:
            if (this->modeInputModeImageRadioButton != NULL) {
                this->modeInputModeImageRadioButton->setChecked(true);
            }
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            this->modeInputModeTileTabsManualLayoutRadioButton->setChecked(true);
            break;
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            this->modeInputVolumeEditRadioButton->setChecked(true);
            break;
        case UserInputModeEnum::Enum::VIEW:
            this->modeInputModeViewRadioButton->setChecked(true);
            break;
    }

    this->modeWidgetGroup->blockAllSignals(false);

    this->updateDisplayedModeUserInputWidget();
}

void
BrainBrowserWindowToolBar::updateDisplayedModeUserInputWidget()
{
    switch (this->selectedUserInputProcessor->getUserInputMode()) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            /*
             * Delete all selected annotations and update graphics and UI.
             */
            AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
            annotationManager->deselectAllAnnotationsForEditing(this->browserWindowIndex);
            break;
    }
}

/**
 * Create the tab options widget.
 *
 * @param lockWindowAndAllTabAspectAction
 *    Action for locking the window's aspect ratio and the aspect ratio of all tabs
 *
 * @return  Button to lock window and tab's aspect ratios.
 */
QWidget* 
BrainBrowserWindowToolBar::createTabOptionsWidget(QToolButton* toolBarLockWindowAndAllTabAspectRatioButton)
{
    m_tabOptionsComponent = new BrainBrowserWindowToolBarTab(this->browserWindowIndex,
                                                             toolBarLockWindowAndAllTabAspectRatioButton,
                                                             this,
                                                             m_objectNamePrefix);
    
    QWidget* w = this->createToolWidget("Misc",
                                        m_tabOptionsComponent, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    return w;
}

/**
 * Update the tab widget.
 * 
 * @param browserTabContent
 *   Content of browser tab.
 */
void 
BrainBrowserWindowToolBar::updateTabOptionsWidget(BrowserTabContent* browserTabContent)
{
    if (this->tabMiscWidget->isHidden()) {
        return;
    }
    
    m_tabOptionsComponent->updateContent(browserTabContent);
}


/**
 * Create the chart type widget.
 *
 * @return
 *    Widget containing the chart options.
 */
QWidget*
BrainBrowserWindowToolBar::createChartTypeWidget()
{
    m_chartTypeToolBarComponent = new BrainBrowserWindowToolBarChartType(this,
                                                                         m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Chart Type (OLD)",
                                        m_chartTypeToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the chart type widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateChartTypeWidget(BrowserTabContent* browserTabContent)
{
    if (this->chartTypeWidget->isHidden()) {
        return;
    }
    
    m_chartTypeToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the chart type two widget.
 *
 * @return
 *    Widget containing the chart type.
 */
QWidget*
BrainBrowserWindowToolBar::createChartTypeTwoWidget()
{
    m_chartTwoTypeToolBarComponent = new BrainBrowserWindowToolBarChartTwoType(this,
                                                                               m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Chart Type",
                                        m_chartTwoTypeToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the chart type widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateChartTypeTwoWidget(BrowserTabContent* browserTabContent)
{
    if (this->chartTypeTwoWidget->isHidden()) {
        return;
    }
    
    m_chartTwoTypeToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the chart axes widget.
 *
 * @return
 *    Widget containing the chart axes.
 */
QWidget*
BrainBrowserWindowToolBar::createChartAxesWidget()
{
    m_chartAxisToolBarComponent = new BrainBrowserWindowToolBarChartAxes(this,
                                                                         m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Chart Axes",
                                        m_chartAxisToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the chart axes widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateChartAxesWidget(BrowserTabContent* browserTabContent)
{
    if (this->chartAxesWidget->isHidden()) {
        return;
    }
    
    m_chartAxisToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the chart attributes widget.
 *
 * @return
 *    Widget containing the chart attributes.
 */
QWidget*
BrainBrowserWindowToolBar::createChartAttributesWidget()
{
    m_chartAttributesToolBarComponent = new BrainBrowserWindowToolBarChartAttributes(this,
                                                                                     m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Chart Attributes",
                                        m_chartAttributesToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the chart attributes widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateChartAttributesWidget(BrowserTabContent* browserTabContent)
{
    if (this->chartAttributesWidget->isHidden()) {
        return;
    }
    m_chartAttributesToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the chart two orientation widget.
 *
 * @return
 *    Widget containing the two chart orientation.
 */
QWidget*
BrainBrowserWindowToolBar::createChartTwoOrientationWidget()
{
    m_chartTwoOrientationToolBarComponent = new BrainBrowserWindowToolBarChartTwoOrientation(this,
                                                                                             m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Chart<BR>Orientation",
                                        m_chartTwoOrientationToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the chart orientation widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateChartTwoOrientationWidget(BrowserTabContent* browserTabContent)
{
    if (this->chartTwoOrientationWidget->isHidden()) {
        return;
    }
    m_chartTwoOrientationToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the chart two attributes widget.
 *
 * @return
 *    Widget containing the chart two attributes.
 */
QWidget*
BrainBrowserWindowToolBar::createChartTwoAttributesWidget()
{
    this->m_chartTwoAttributesToolBarComponent = new BrainBrowserWindowToolBarChartTwoAttributes(this,
                                                                                                 m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Chart<BR>Attributes",
                                        this->m_chartTwoAttributesToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the chart attributes widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateChartTwoAttributesWidget(BrowserTabContent* browserTabContent)
{
    if (this->chartTwoAttributesWidget->isHidden()) {
        return;
    }
    m_chartTwoAttributesToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the chart two oriented axes widget.
 *
 * @return
 *    Widget containing the chart two oriented axes.
 */
QWidget*
BrainBrowserWindowToolBar::createChartTwoOrientedAxisWidget()
{
    this->m_chartTwoOrientedAxesToolBarComponent = new BrainBrowserWindowToolBarChartTwoOrientedAxes(this,
                                                                                                     m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Chart Axes",
                                        this->m_chartTwoOrientedAxesToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the chart oriented axes widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateChartTwoOrientedAxesWidget(BrowserTabContent* browserTabContent)
{
    if (this->chartTwoOrientedAxesWidget->isHidden()) {
        return;
    }
    m_chartTwoOrientedAxesToolBarComponent->updateContent(browserTabContent);
}


/**
 * Create the image resolution widget.
 *
 * @return
 *    Widget containing the image options.
 */
QWidget*
BrainBrowserWindowToolBar::createImageWidget()
{
    m_imageToolBarComponent = new BrainBrowserWindowToolBarImage(this,
                                                                 m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Image",
                                        m_imageToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Create the histology resolution widget.
 *
 * @return
 *    Widget containing histology widget options.
 */
QWidget*
BrainBrowserWindowToolBar::createHistologyWidget()
{
    m_histologyToolBarComponent = new BrainBrowserWindowToolBarHistology(this,
                                                                        m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Histology",
                                        m_histologyToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the image  widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateImageWidget(BrowserTabContent* browserTabContent)
{
    if (this->imageWidget->isHidden()) {
        return;
    }
    
    m_imageToolBarComponent->updateContent(browserTabContent);
}

/**
 * Update the histology  widget.
 *
 * @param browserTabContent
 *   The active model display (may be NULL).
 */
void
BrainBrowserWindowToolBar::updateHistologyWidget(BrowserTabContent* browserTabContent)
{
    if (this->histologyWidget->isHidden()) {
        return;
    }
    
    m_histologyToolBarComponent->updateContent(browserTabContent);
}


/**
 * Create the single surface options widget.
 *
 * @return  The single surface options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createSingleSurfaceOptionsWidget()
{
    m_surfaceToolBarComponent = new BrainBrowserWindowToolBarSurface(m_objectNamePrefix,
                                                                     this);
    QWidget* w = this->createToolWidget("Selection",
                                        m_surfaceToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP, 
                                        100);
    w->setVisible(false);
    return w;
}
/**
 * Update the single surface options widget.
 * 
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateSingleSurfaceOptionsWidget(BrowserTabContent* browserTabContent)
{
    if (this->singleSurfaceSelectionWidget->isHidden()) {
        return;
    }
    
    m_surfaceToolBarComponent->updateContent(browserTabContent);
}

/**
 * @return Create and return the surface montage options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createSurfaceMontageOptionsWidget()
{
    m_surfaceMontageToolBarComponent = new BrainBrowserWindowToolBarSurfaceMontage(this,
                                                                                   m_objectNamePrefix);
    QWidget* w = this->createToolWidget("Montage Selection", 
                                        m_surfaceMontageToolBarComponent, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP, 
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the surface montage options widget.
 * 
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateSurfaceMontageOptionsWidget(BrowserTabContent* browserTabContent)
{
    if (this->surfaceMontageSelectionWidget->isHidden()) {
        return;
    }

    m_surfaceMontageToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the volume montage widget.
 *
 * @return The volume montage widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumeMontageWidget()
{
    m_volumeMontageComponent = new BrainBrowserWindowToolBarVolumeMontage(m_objectNamePrefix,
                                                                          this);
    
    
    QWidget* w = this->createToolWidget("Montage", 
                                        m_volumeMontageComponent,
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume montage widget.
 * 
 * @param browserTabContent
 *   Content of browser tab.
 */
void 
BrainBrowserWindowToolBar::updateVolumeMontageWidget(BrowserTabContent* browserTabContent)
{
    if (this->volumeMontageWidget->isHidden()) {
        return;
    }

    m_volumeMontageComponent->updateContent(browserTabContent);
}

/**
 * Create the volume mpr widget.
 *
 * @return The volume montage widget.
 */
QWidget*
BrainBrowserWindowToolBar::createVolumeMprWidget()
{
    m_volumeMprComponent = new BrainBrowserWindowToolBarVolumeMPR(this,
                                                                  m_objectNamePrefix);
    
    
    QWidget* w = this->createToolWidget("MPR",
                                        m_volumeMprComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume mpr widget.
 *
 * @param browserTabContent
 *   Content of browser tab.
 */
void
BrainBrowserWindowToolBar::updateVolumeMprWidget(BrowserTabContent* browserTabContent)
{
    if (this->volumeMprWidget->isHidden()) {
        return;
    }
    
    m_volumeMprComponent->updateContent(browserTabContent);
}

/**
 * Create the volume plane widget.
 *
 * @return The volume plane widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumePlaneWidget()
{
    m_slicePlaneComponent = new BrainBrowserWindowToolBarSlicePlane(m_objectNamePrefix,
                                                                    this);
    QWidget* w = this->createToolWidget("Slice Plane", 
                                        m_slicePlaneComponent, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume plane orientation widget.
 * 
 * @param browserTabContent
 *   Content of browser tab.
 */
void 
BrainBrowserWindowToolBar::updateVolumePlaneWidget(BrowserTabContent* browserTabContent)
{
    if (this->volumePlaneWidget->isHidden()) {
        return;
    }

    m_slicePlaneComponent->updateContent(browserTabContent);
}

/**
 * Create a tool widget which is a group of widgets with
 * a descriptive label added.
 *
 * @param name
 *    Name for the descriptive label.  For a multi-line label,
 *    separate the lines with an HTML "<BR>" tag.
 * @param childWidget
 *    Child widget that is in the tool widget.
 * @param verticalBarPlacement
 *    Where to place a vertical bar.  Values other than right or
 *    left are ignored in which case no vertical bar is displayed.
 * @param contentPlacement
 *    Where to place widget which must be top or bottom.
 * @return The tool widget.
 */
QWidget*
BrainBrowserWindowToolBar::createToolWidget(const QString& name,
                                            QWidget* childWidget,
                                            const WidgetPlacement verticalBarPlacement,
                                            const WidgetPlacement contentPlacement,
                                            const int /*horizontalStretching*/)
{
    QLabel* nameLabel(NULL);
    if ( ! name.isEmpty()) {
        nameLabel = new QLabel("<html><center>" + name + "</center></html>");
        nameLabel->setFixedHeight(nameLabel->sizeHint().height());
    }
    
    int32_t columnWidget(-1);
    int32_t columnVerticalBar(-1);
    int32_t columnCount(-1);
    switch(verticalBarPlacement) {
        case WIDGET_PLACEMENT_LEFT:
            columnVerticalBar = 0;
            columnWidget      = 1;
            columnCount       = 2;
            break;
        case WIDGET_PLACEMENT_RIGHT:
            columnVerticalBar = 1;
            columnWidget      = 0;
            columnCount       = 2;
            break;
        default:
            columnWidget = 0;
            columnCount  = 1;
            break;
    }
    CaretAssert(columnWidget >= 0);
    CaretAssert(columnCount  >= 1);
    
    int32_t rowWidget(-1);
    int32_t rowEmpty(-1);
    int32_t rowLabel(-1);
    int32_t rowCount(-1);
    switch (contentPlacement) {
        case WIDGET_PLACEMENT_BOTTOM:
            rowEmpty  = 0;
            rowWidget = 1;
            if (nameLabel != NULL) {
                rowLabel  = 2;
                rowCount  = 3;
            }
            else {
                rowCount = 2;
            }
            break;
        case WIDGET_PLACEMENT_TOP:
            rowEmpty  = 1;
            rowWidget = 0;
            if (nameLabel != NULL) {
                rowLabel  = 2;
                rowCount  = 3;
            }
            else {
                rowCount  = 3;
            }
            break;
        case WIDGET_PLACEMENT_NONE:
            rowWidget = 0;
            if (nameLabel != NULL) {
                rowLabel  = 1;
                rowCount  = 2;
            }
            else {
                rowCount = 1;
            }
            break;
        default:
            CaretAssert(0);
            break;
    }
    CaretAssert(rowWidget >= 0);
    CaretAssert(rowCount  >= 1);
    
    QWidget* w = new QWidget();
    QGridLayout* layout = new QGridLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    if (columnVerticalBar >= 0) {
        layout->setHorizontalSpacing(2);
    }
    else {
        layout->setHorizontalSpacing(0);
    }
    layout->setVerticalSpacing(0);
    
    for (int32_t iRow = 0; iRow < rowCount; iRow++) {
        layout->setRowStretch(iRow, 0);
    }
    if (rowEmpty >= 0) {
        layout->setRowStretch(rowEmpty, 100);
    }
    if (columnVerticalBar >= 0) {
        layout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                          0, columnVerticalBar, rowCount, 1);
    }
    
    layout->addWidget(childWidget,
                      rowWidget, columnWidget);
    if (nameLabel != NULL) {
        CaretAssert(rowLabel >= 0);
        layout->addWidget(nameLabel,
                          rowLabel, 0, 1, columnCount, Qt::AlignHCenter);
    }
    
    QMargins m(w->contentsMargins());
    m.setLeft(0);
    m.setRight(0);
    w->setContentsMargins(m);

    return w;
}

/**
 * Update the graphics windows for the selected tab.
 */
void 
BrainBrowserWindowToolBar::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(
            EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Update the user-interface.
 */
void 
BrainBrowserWindowToolBar::updateUserInterface()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
}

/**
 * Update the toolbox for the window
 */
void 
BrainBrowserWindowToolBar::updateToolBox()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).addToolBox().getPointer());
}


/**
 * Called when the scene tool button is clicked to show scene dialog
 */
void
BrainBrowserWindowToolBar::sceneToolButtonClicked()
{
    GuiManager::get()->getSceneDialogDisplayAction()->trigger();
}

/**
 * Called when item is selected from custom view menu
 */
void
BrainBrowserWindowToolBar::customViewMenuTriggered(QAction* action)
{
    if (action != NULL) {
        const QString customViewName(action->text());
        
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        ModelTransform modelTransform;
        if (prefs->getCustomView(customViewName, modelTransform)) {
            BrowserTabContent* btc = this->getTabContentFromSelectedTab();
            btc->setTransformationsFromModelTransform(modelTransform);
            this->updateGraphicsWindowAndYokedWindows();
        }
    }
}

/**
 * Called when custom view menu is about to show
 */
void
BrainBrowserWindowToolBar::customViewMenuAboutToShow()
{
    this->customViewMenu->clear();
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->readCustomViews();
    const std::vector<std::pair<AString, AString> > customViewNameAndComments = prefs->getCustomViewNamesAndComments();
    
    const int32_t numViews = static_cast<int32_t>(customViewNameAndComments.size());
    for (int32_t i = 0; i < numViews; i++) {
        QAction* action = this->customViewMenu->addAction(customViewNameAndComments[i].first);
        action->setToolTip(WuQtUtilities::createWordWrappedToolTipText(customViewNameAndComments[i].second));
    }
}
/**
 * Called when custom view is triggered and displays Custom View Menu.
 */
void
BrainBrowserWindowToolBar::customViewActionTriggered()
{
    CaretAssert(m_parentBrainBrowserWindow);
    GuiManager::get()->processShowCustomViewDialog(m_parentBrainBrowserWindow);
}

/**
 * Receive events from the event manager.
 *  
 * @param event
 *   Event sent by event manager.
 */
void 
BrainBrowserWindowToolBar::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_CLOSE_IN_TOOL_BAR) {
        EventBrowserTabCloseInToolBar* closeTabInGuiEvent = dynamic_cast<EventBrowserTabCloseInToolBar*>(event);
        CaretAssert(closeTabInGuiEvent);
        
        BrowserTabContent* tabToClose = closeTabInGuiEvent->getBrowserTab();
        CaretAssert(tabToClose);
        
        for (int32_t iTab = 0; iTab < this->tabBar->count(); iTab++) {
            if (tabToClose == getTabContentFromTab(iTab)) {
                if ((iTab >= 0)
                    && (iTab < this->tabBar->count())) {
                    tabClosed(iTab,
                              RemoveTabMode::CLOSE_TAB_CONTENT_FOR_REOPENING);
                }
                this->updateGraphicsWindow();
                closeTabInGuiEvent->setEventProcessed();
                break;
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE_IN_TOOL_BAR) {
        EventBrowserTabDeleteInToolBar* deleteTabInGuiEvent = dynamic_cast<EventBrowserTabDeleteInToolBar*>(event);
        CaretAssert(deleteTabInGuiEvent);

        BrowserTabContent* tabToDelete = deleteTabInGuiEvent->getBrowserTab();
        CaretAssert(tabToDelete);
        
        for (int32_t iTab = 0; iTab < this->tabBar->count(); iTab++) {
            if (tabToDelete == getTabContentFromTab(iTab)) {
                if ((iTab >= 0)
                    && (iTab < this->tabBar->count())) {
                    tabClosed(iTab,
                              RemoveTabMode::DELETE_TAB_CONTENT);
                }
                this->updateGraphicsWindow();
                deleteTabInGuiEvent->setEventProcessed();
                break;
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_NEW_IN_GUI) {
        EventBrowserTabNewInGUI* newTabEvent = dynamic_cast<EventBrowserTabNewInGUI*>(event);
        CaretAssert(newTabEvent);
        
        BrowserTabContent* newTabContent = addNewTab();
        newTabEvent->setBrowserTab(newTabContent);
        newTabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_DRAWING_CONTENT_GET) {
        EventBrowserWindowDrawingContent* getModelEvent =
            dynamic_cast<EventBrowserWindowDrawingContent*>(event);
        CaretAssert(getModelEvent);
        
        if (getModelEvent->getBrowserWindowIndex() == this->browserWindowIndex) {
            if (m_parentBrainBrowserWindow != NULL) {
                const int32_t numTabs = this->tabBar->count();
                for (int32_t i = 0; i < numTabs; i++) {
                    BrowserTabContent* btc = this->getTabContentFromTab(i);
                    getModelEvent->addBrowserTab(btc);
                }
                
                BrowserWindowContent* windowContent = m_parentBrainBrowserWindow->getBrowerWindowContent();
                getModelEvent->setBrowserWindowContent(windowContent);
                
                if (windowContent->isTileTabsEnabled()) {
                    /*
                     * Tab that is highlighted so user knows which tab
                     * any changes in toolbar/toolboxes apply to.
                     */
                    getModelEvent->setTabIndexForTileTabsHighlighting(m_tabIndexForTileTabsHighlighting);
                }
                
                getModelEvent->setSelectedBrowserTabContent(this->getTabContentFromSelectedTab());
            }
            getModelEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_USER_INPUT_MODE) {
        EventUserInputModeGet* modeEvent = dynamic_cast<EventUserInputModeGet*>(event);
        CaretAssert(modeEvent);
        
        if (modeEvent->getWindowIndex() == this->browserWindowIndex) {
            modeEvent->setUserInputMode(this->selectedUserInputProcessor->getUserInputMode());
            modeEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE) {
        EventGetOrSetUserInputModeProcessor* inputModeEvent =
        dynamic_cast<EventGetOrSetUserInputModeProcessor*>(event);
        CaretAssert(inputModeEvent);
        
        if (inputModeEvent->getWindowIndex() == this->browserWindowIndex) {
            if (inputModeEvent->isGetUserInputMode()) {
                inputModeEvent->setUserInputProcessor(this->selectedUserInputProcessor);
            }
            else if (inputModeEvent->isSetUserInputMode()) {
                UserInputModeAbstract* newUserInputProcessor = NULL;
                switch (inputModeEvent->getUserInputMode()) {
                    case UserInputModeEnum::Enum::INVALID:
                        CaretAssertMessage(0, "INVALID is NOT allowed for user input mode");
                        break;
                    case UserInputModeEnum::Enum::ANNOTATIONS:
                        newUserInputProcessor = this->userInputAnnotationsModeProcessor;
                        break;
                    case UserInputModeEnum::Enum::BORDERS:
                        newUserInputProcessor = this->userInputBordersModeProcessor;
                        break;
                    case UserInputModeEnum::Enum::FOCI:
                        newUserInputProcessor = this->userInputFociModeProcessor;
                        break;
                    case UserInputModeEnum::Enum::IMAGE:
                        newUserInputProcessor = this->userInputImageModeProcessor;
                        break;
                    case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
                        newUserInputProcessor = this->userInputTileTabsManualLayoutProcessor;
                        break;
                    case UserInputModeEnum::Enum::VOLUME_EDIT:
                        newUserInputProcessor = this->userInputVolumeEditModeProcessor;
                        break;
                    case UserInputModeEnum::Enum::VIEW:
                        newUserInputProcessor = this->userInputViewModeProcessor;
                        break;
                }
                
                if ((newUserInputProcessor == this->userInputAnnotationsModeProcessor)
                    || (this->selectedUserInputProcessor == this->userInputAnnotationsModeProcessor)) {
                    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
                    CaretAssert(annMan);
                    annMan->deselectAllAnnotationsForEditing(this->browserWindowIndex);
                }
                if (newUserInputProcessor != NULL) {
                    if (newUserInputProcessor != this->selectedUserInputProcessor) {
                        this->selectedUserInputProcessor->finish();
                        this->selectedUserInputProcessor = newUserInputProcessor;
                        this->selectedUserInputProcessor->initialize();
                    }
                }
            }
            inputModeEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isToolBarUpdate()
            && uiEvent->isUpdateForWindow(this->browserWindowIndex)) {
            this->updateToolBar();
            uiEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_CREATE_TABS) {
        EventBrowserWindowCreateTabs* tabEvent =
        dynamic_cast<EventBrowserWindowCreateTabs*>(event);
        CaretAssert(tabEvent);
        
        EventModelGetAll eventAllModels;
        EventManager::get()->sendEvent(eventAllModels.getPointer());
        const bool haveModels = (eventAllModels.getModels().empty() == false);
        
        if (haveModels) {
            switch (tabEvent->getMode()) {
                case EventBrowserWindowCreateTabs::MODE_LOADED_DATA_FILE:
                    if (tabBar->count() == 0) {
                        AString errorMessage;
                        BrowserTabContent* tabContent = createNewTab(errorMessage);
                        if (errorMessage.isEmpty() == false) {
                            CaretLogSevere(errorMessage);
                        }
                        if (tabContent != NULL) {
                            insertTabContentPrivate(InsertTabMode::APPEND,
                                                    tabContent,
                                                    -1);
                        }
                    }
                    break;
                case EventBrowserWindowCreateTabs::MODE_LOADED_SPEC_FILE:
                    this->addDefaultTabsAfterLoadingSpecFile();
                    break;
            }
        }
        tabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_TILE_TAB_OPERATION) {
        EventBrowserWindowTileTabOperation* tileTabsEvent =
        dynamic_cast<EventBrowserWindowTileTabOperation*>(event);
        CaretAssert(tileTabsEvent);
        processTileTabOperationEvent(tileTabsEvent);
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS) {
        EventUpdateYokedWindows* yokeUpdateEvent =
        dynamic_cast<EventUpdateYokedWindows*>(event);
        CaretAssert(yokeUpdateEvent);
        
        BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
        if (browserTabContent != NULL) {
            if (this->browserWindowIndex != yokeUpdateEvent->getBrowserWindowIndexThatIssuedEvent()) {
                if (yokeUpdateEvent->isBrainOrChartModelYoking(browserTabContent->getBrainModelYokingGroup(),
                                                               browserTabContent->getChartModelYokingGroup())) {
                    this->updateToolBar();
                    this->updateGraphicsWindow();
                }
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL_VIEWED) {
        EventBrowserTabGetAllViewed* viewedTabsEvent = dynamic_cast<EventBrowserTabGetAllViewed*>(event);
        CaretAssert(viewedTabsEvent);
        
        if (m_parentBrainBrowserWindow != NULL) {
            if (m_parentBrainBrowserWindow->isTileTabsSelected()) {
                const int32_t numTabs = this->tabBar->count();
                for (int32_t i = 0; i < numTabs; i++) {
                    BrowserTabContent* btc = this->getTabContentFromTab(i);
                    viewedTabsEvent->addViewedBrowserTab(btc);
                }
            }
            else {
                BrowserTabContent* btc = getTabContentFromSelectedTab();
                if (btc != NULL) {
                    viewedTabsEvent->addViewedBrowserTab(btc);
                }
            }
        }
        
        viewedTabsEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_SELECT_IN_WINDOW) {
        EventBrowserTabSelectInWindow* selectTabEvent = dynamic_cast<EventBrowserTabSelectInWindow*>(event);
        CaretAssert(selectTabEvent);
        
        const int32_t browserTabIndex = selectTabEvent->getBrowserTabIndex();
        const int32_t tabIndex = getTabBarIndexWithBrowserTabIndex(browserTabIndex);
        if (tabIndex >= 0) {
            this->tabBar->setCurrentIndex(tabIndex);
            selectedTabChanged(tabIndex);
            selectTabEvent->setEventProcessed();
        }
    }
    else {
        
    }
}

/**
 * Process a tile tab operation event.
 *
 * @param tileTabOperation
 */
void
BrainBrowserWindowToolBar::processTileTabOperationEvent(EventBrowserWindowTileTabOperation* tileTabsEvent)
{
    if (tileTabsEvent->getWindowIndex() == this->browserWindowIndex) {
        const int32_t browserTabIndex = tileTabsEvent->getBrowserTabIndex();
        int32_t tabBarIndex = getTabBarIndexWithBrowserTabIndex(browserTabIndex);
        const EventBrowserWindowTileTabOperation::Operation operation = tileTabsEvent->getOperation();
        switch (operation) {
            case EventBrowserWindowTileTabOperation::OPERATION_GRID_NEW_TAB_AFTER:
                if (tabBarIndex >= 0) {
                    insertNewTabAtTabBarIndex(tabBarIndex + 1);
                }
                break;
            case EventBrowserWindowTileTabOperation::OPERATION_GRID_NEW_TAB_BEFORE:
                if (tabBarIndex >= 0) {
                    insertNewTabAtTabBarIndex(tabBarIndex);
                }
                break;
            case EventBrowserWindowTileTabOperation::OPERATION_MANUAL_NEW_TAB:
            {
                BrowserTabContent* btc = addNewTab();
                if (btc != NULL) {
                    int32_t windowViewport[4];
                    tileTabsEvent->getWindowViewport(windowViewport);
                    
                    const float windowWidth = windowViewport[2];
                    const float windowHeight = windowViewport[3];

                    if ((windowWidth > 0)
                        && (windowHeight > 0)) {
                        const float mouseX(tileTabsEvent->getMouseX());
                        const float mouseY(tileTabsEvent->getMouseY());
                        
                        AnnotationBrowserTab* tabAnnotation = btc->getManualLayoutBrowserTabAnnotation();
                        CaretAssert(tabAnnotation);
                        float xyz[3];
                        tabAnnotation->getCoordinate()->getXYZ(xyz);
                        xyz[0] = (mouseX / windowWidth) * 100.0;
                        xyz[1] = (mouseY / windowHeight) * 100.0;
                        tabAnnotation->getCoordinate()->setXYZ(xyz);
                    }
                    
                    updateGraphicsWindow();
                }
            }
                break;
            case EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_TO_FRONT:
            case EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_FORWARD:
            case EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_TO_BACK:
            case EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_BACKWARD:
            {
                AnnotationStackingOrderTypeEnum::Enum orderingOperation = AnnotationStackingOrderTypeEnum::BRING_TO_FRONT;
                switch (operation) {
                    case EventBrowserWindowTileTabOperation::OPERATION_GRID_NEW_TAB_AFTER:
                    case EventBrowserWindowTileTabOperation::OPERATION_GRID_NEW_TAB_BEFORE:
                    case EventBrowserWindowTileTabOperation::OPERATION_MANUAL_NEW_TAB:
                        CaretAssert(0);
                        break;
                    case EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_TO_FRONT:
                        orderingOperation = AnnotationStackingOrderTypeEnum::BRING_TO_FRONT;
                        break;
                    case EventBrowserWindowTileTabOperation::OPERATION_ORDER_BRING_FORWARD:
                        orderingOperation = AnnotationStackingOrderTypeEnum::BRING_FORWARD;
                        break;
                    case EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_TO_BACK:
                        orderingOperation = AnnotationStackingOrderTypeEnum::SEND_TO_BACK;
                        break;
                    case EventBrowserWindowTileTabOperation::OPERATION_ORDER_SEND_BACKWARD:
                        orderingOperation = AnnotationStackingOrderTypeEnum::SEND_BACKWARD;
                        break;
                    case EventBrowserWindowTileTabOperation::OPERATION_SELECT_TAB:
                    case EventBrowserWindowTileTabOperation::OPERATION_REPLACE_TABS:
                        CaretAssert(0);
                        break;
                }
                
                std::vector<BrowserTabContent*> allTabContent;
                getAllTabContent(allTabContent);
                std::vector<Annotation*> annotations;
                Annotation* selectedAnnotation(NULL);
                for (auto tc : allTabContent) {
                    AnnotationBrowserTab* abt = tc->getManualLayoutBrowserTabAnnotation();
                    CaretAssert(abt);
                    if (tc->getTabNumber() == browserTabIndex) {
                        selectedAnnotation = abt;
                    }
                    annotations.push_back(tc->getManualLayoutBrowserTabAnnotation());
                }
                
                if (selectedAnnotation != NULL) {
                    AString errorMessage;
                    AnnotationStackingOrderOperation modifier(AnnotationStackingOrderOperation::Mode::MODE_APPLY_NEW_ORDER_TO_ANNOTAIONS,
                                                              annotations,
                                                              selectedAnnotation,
                                                              this->browserWindowIndex);
                    if (! modifier.runOrdering(orderingOperation,
                                               errorMessage)) {
                        WuQMessageBox::errorOk(this,
                                               errorMessage);
                    }
                }
                else {
                    WuQMessageBox::errorOk(this,
                                           ("PROGRAM ERROR: Unable to find tab with number "
                                            + AString::number(browserTabIndex + 1)));
                }
            }
                break;
            case EventBrowserWindowTileTabOperation::OPERATION_SELECT_TAB:
                if (tabBarIndex >= 0) {
                    /*
                     * Highlighting places outline around tab in graphics region
                     */
                    const bool highlightTabWithOutlineFlag(true);
                    if ( ! highlightTabWithOutlineFlag) {
                        m_tileTabsHighlightingTimerEnabledFlag = false;
                    }
                    this->tabBar->setCurrentIndex(tabBarIndex);
                    if ( ! highlightTabWithOutlineFlag) {
                        m_tileTabsHighlightingTimerEnabledFlag = true;
                    }
                }
                break;
            case EventBrowserWindowTileTabOperation::OPERATION_REPLACE_TABS:
                replaceBrowserTabs(tileTabsEvent->getBrowserTabsForReplaceOperation());
                break;
        }
        
        tileTabsEvent->setEventProcessed();
        
        updateGraphicsWindow();
    }
}


/**
 * Get the tab bar index containing the browser tab index.
 *
 * @param browserTabIndex
 *     Index of the browser tab.
 * @return 
 *     Index in tab bar containing the browser tab index or -1 if not found.
 */
int32_t
BrainBrowserWindowToolBar::getTabBarIndexWithBrowserTabIndex(const int32_t browserTabIndex)
{
    const int numTabs = this->tabBar->count();
    if (numTabs > 0) {
        for (int32_t i = 0; i < numTabs; i++) {
            const BrowserTabContent* btc = getTabContentFromTab(i);
            if (btc->getTabNumber() == browserTabIndex){
                return i;
            }
        }
    }
    
    return -1;
}

/**
 * If this window is yoked, update all windows since they may
 * be yoked to this window.  If NOT yoked, just update this window.
 */
void
BrainBrowserWindowToolBar::updateGraphicsWindowAndYokedWindows()
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        if (browserTabContent->isBrainModelYoked()) {
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            EventManager::get()->sendEvent(EventUpdateYokedWindows(this->browserWindowIndex,
                                                                   browserTabContent->getBrainModelYokingGroup(),
                                                                   browserTabContent->getChartModelYokingGroup()).getPointer());
        }
        else {
            updateGraphicsWindow();
        }
    }
}

/**
 * Get the content in the browser tab.
 * @return
 *    Browser tab contents in the selected tab or NULL if none.
 */
BrowserTabContent* 
BrainBrowserWindowToolBar::getTabContentFromSelectedTab()
{
    const int tabIndex = this->tabBar->currentIndex();
    BrowserTabContent* btc = this->getTabContentFromTab(tabIndex);
    return btc;
}

/**
 * Get the content in the given browser tab
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Browser tab contents in the selected tab or NULL if none.
 */
BrowserTabContent* 
BrainBrowserWindowToolBar::getTabContentFromTab(const int tabIndex)
{
    if ((tabIndex >= 0) && (tabIndex < this->tabBar->count())) {
        void* p = this->tabBar->tabData(tabIndex).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        return btc;
    }
    
    return NULL;
}

/**
 * Get the model displayed in the selected tab.
 * @return
 *     Model in the selected tab or NULL if 
 *     no model is displayed.
 */
Model* 
BrainBrowserWindowToolBar::getDisplayedModel()
{
    Model* mdc = NULL;
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    if (btc != NULL) {
        mdc = btc->getModelForDisplay();
    }
    
    return mdc;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
BrainBrowserWindowToolBar::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainBrowserWindowToolBar",
                                            1);
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    

    /*
     * Determine tabs in this toolbar that should be saved to scene
     */
    const std::vector<int32_t> tabIndicesForScene = sceneAttributes->getIndicesOfTabsForSavingToScene();
    std::vector<int32_t> tabIndicesToSave;
    const int numTabsInToolbar = this->tabBar->count();
    if (numTabsInToolbar > 0) {
        for (int32_t i = 0; i < numTabsInToolbar; i++) {
            BrowserTabContent* btc = this->getTabContentFromTab(i);
            const int32_t tabIndex = btc->getTabNumber();
            if (std::find(tabIndicesForScene.begin(),
                          tabIndicesForScene.end(),
                          tabIndex) != tabIndicesForScene.end()) {
                tabIndicesToSave.push_back(tabIndex);
            }
        }
    }
    
    /*
     * Save the tabs
     */
    const int numTabs = static_cast<int32_t>(tabIndicesToSave.size());
    if (numTabs > 0) {
        SceneIntegerArray* sceneTabIndexArray = new SceneIntegerArray("tabIndices",
                                                                      numTabs);
        for (int32_t i = 0; i < numTabs; i++) {
            const int32_t tabIndex = tabIndicesToSave[i];
            sceneTabIndexArray->setValue(i,
                                         tabIndex);
        }
        sceneClass->addChild(sceneTabIndexArray);
    }

    /*
     * Add selected tab to scene
     */
    int32_t selectedTabIndex = -1;
    BrowserTabContent* selectedTab = getTabContentFromSelectedTab();
    if (selectedTab != NULL) {
        selectedTabIndex = selectedTab->getTabNumber();
    }
    sceneClass->addInteger("selectedTabIndex", selectedTabIndex);
    
    /*
     * Toolbar visible
     */
    sceneClass->addBoolean("toolBarVisible", 
                           m_toolbarWidget->isVisible());
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
BrainBrowserWindowToolBar::restoreFromScene(const SceneAttributes* sceneAttributes,
                                     const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
    
    /*
     * Close any tabs
     */
    const int32_t numberOfOpenTabs = this->tabBar->count();
    for (int32_t iClose = (numberOfOpenTabs - 1); iClose >= 0; iClose--) {
        this->tabClosed(iClose,
                        RemoveTabMode::DELETE_TAB_CONTENT);
    }
    
    /*
     * Index of selected browser tab (NOT the tabBar)
     */
    const int32_t selectedTabIndex = sceneClass->getIntegerValue("selectedTabIndex", -1);

    /*
     * Create new tabs
     */
    int32_t defaultTabBarIndex = 0;
    const ScenePrimitiveArray* sceneTabIndexArray = sceneClass->getPrimitiveArray("tabIndices");
    if (sceneTabIndexArray != NULL) {
        const int32_t numValidTabs = sceneTabIndexArray->getNumberOfArrayElements();
        for (int32_t iTab = 0; iTab < numValidTabs; iTab++) {
            const int32_t tabIndex = sceneTabIndexArray->integerValue(iTab);
            
            EventBrowserTabGet getTabContent(tabIndex);
            EventManager::get()->sendEvent(getTabContent.getPointer());
            BrowserTabContent* tabContent = getTabContent.getBrowserTab();
            if (tabContent != NULL) {
                if (tabContent->getTabNumber() == selectedTabIndex) {
                    defaultTabBarIndex = iTab;
                }
                CaretAssert(iTab >= 0);
                insertTabContentPrivate(InsertTabMode::AT_TAB_BAR_INDEX,
                                        tabContent,
                                        iTab);
            }
            else {
                sceneAttributes->addToErrorMessage("Toolbar in window "
                                                   + AString::number(this->browserWindowIndex)
                                                   + " failed to restore tab " 
                                                   + AString::number(selectedTabIndex));
            }
        }
    }
    
    /*
     * Select tab
     */
    if ((defaultTabBarIndex >= 0) 
        && (defaultTabBarIndex < tabBar->count())) {
        tabBar->setCurrentIndex(defaultTabBarIndex);
    }
    
    /*
     * Show hide toolbar
     */
    const bool showToolBar = sceneClass->getBooleanValue("toolBarVisible",
                                                         true);
    showHideToolBar(showToolBar);
}

/**
 * @return Number of tabs in the tab bar.
 */
int32_t
BrainBrowserWindowToolBar::getNumberOfTabs() const
{
    return this->tabBar->count();
}




