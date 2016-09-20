
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
#if QT_VERSION >= 0x050000
#else // QT_VERSION
#include <QCleanlooksStyle>
#endif // QT_VERSION
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QRadioButton>
#include <QSpinBox>
#include <QStyleFactory>
#include <QTabBar>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>

#include "AnnotationManager.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainBrowserWindowToolBarChartAttributes.h"
#include "BrainBrowserWindowToolBarChartAxes.h"
#include "BrainBrowserWindowToolBarChartType.h"
#include "BrainBrowserWindowToolBarClipping.h"
#include "BrainBrowserWindowToolBarSlicePlane.h"
#include "BrainBrowserWindowToolBarSliceSelection.h"
#include "BrainBrowserWindowToolBarSurfaceMontage.h"
#include "BrainBrowserWindowToolBarTab.h"
#include "BrainBrowserWindowToolBarVolumeMontage.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretFunctionName.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CursorDisplayScoped.h"
#include "DisplayPropertiesBorders.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabGetAllViewed.h"
#include "EventBrowserTabNew.h"
#include "EventBrowserWindowContentGet.h"
#include "EventBrowserWindowCreateTabs.h"
#include "EventBrowserWindowNew.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateYokedWindows.h"
#include "GuiManager.h"
#include "Model.h"
#include "ModelChart.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelSurfaceSelector.h"
#include "ModelTransform.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "OverlaySet.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneIntegerArray.h"
#include "ScenePrimitiveArray.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceSelectionViewController.h"
#include "StructureSurfaceSelectionControl.h"
#include "UserInputModeAbstract.h"
#include "VolumeFile.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSurfaceOutlineSetModel.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
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
 * @param windowAspectRatioLockedAction
 *    Action to lock window's aspect ratio.
 * @param tabAspectRatioLockedAction
 *    Action to lock tab's aspect ratio.
 * @param parent
 *    Parent for this toolbar.
 */
BrainBrowserWindowToolBar::BrainBrowserWindowToolBar(const int32_t browserWindowIndex,
                                                     BrowserTabContent* initialBrowserTabContent,
                                                     QAction* overlayToolBoxAction,
                                                     QAction* layersToolBoxAction,
                                                     QAction* windowAspectRatioLockedAction,
                                                     QAction* tabAspectRatioLockedAction,
                                                     BrainBrowserWindow* parentBrainBrowserWindow)
: QToolBar(parentBrainBrowserWindow)
{
    this->browserWindowIndex = browserWindowIndex;
    this->updateCounter = 0;
    m_tabIndexForTileTabsHighlighting = -1;
    
    this->isContructorFinished = false;
    this->isDestructionInProgress = false;

    this->viewOrientationLeftIcon = NULL;
    this->viewOrientationRightIcon = NULL;
    this->viewOrientationAnteriorIcon = NULL;
    this->viewOrientationPosteriorIcon = NULL;
    this->viewOrientationDorsalIcon = NULL;
    this->viewOrientationVentralIcon = NULL;
    this->viewOrientationLeftLateralIcon = NULL;
    this->viewOrientationLeftMedialIcon = NULL;
    this->viewOrientationRightLateralIcon = NULL;
    this->viewOrientationRightMedialIcon = NULL;

    /*
     * Needed for saving and restoring window state in main window
     */
    setObjectName("BrainBrowserWindowToolBar_"
                  + AString::number(browserWindowIndex));
    
    /*
     * Create tab bar that displays models.
     */
    this->tabBar = new QTabBar();
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
#if QT_VERSION >= 0x050000
    QStyle* cleanLooksStyle = QStyleFactory::create("Cleanlooks");
    if (cleanLooksStyle != NULL) {
        cleanLooksStyle->setParent(this);
        this->tabBar->setStyle(cleanLooksStyle);
    }
#else // QT_VERSION
    QCleanlooksStyle* cleanLooksStyle = new QCleanlooksStyle();
    cleanLooksStyle->setParent(this);
    this->tabBar->setStyle(cleanLooksStyle);
#endif // QT_VERSION
#endif // Q_OS_MACX
    QObject::connect(this->tabBar, SIGNAL(currentChanged(int)),
                     this, SLOT(selectedTabChanged(int)));
    QObject::connect(this->tabBar, SIGNAL(tabCloseRequested(int)),
                     this, SLOT(tabCloseSelected(int)));
    QObject::connect(this->tabBar, SIGNAL(tabMoved(int,int)),
                     this, SLOT(tabMoved(int,int)));
    
    /*
     * Custom view action
     */
    const QString customToolTip = ("Pressing the \"Custom\" button displays a dialog for creating and editing orientations.\n"
                                   "Note that custom orientations are stored in your Workbench's preferences and thus\n"
                                   "will be availble in any concurrent or future instances of Workbench.");
    this->customViewAction = WuQtUtilities::createAction("Custom",
                                                         customToolTip,
                                                         this,
                                                         this,
                                                         SLOT(customViewActionTriggered()));
    
    /*
     * Actions at right side of toolbar
     */
    QToolButton* informationDialogToolButton = new QToolButton();
    informationDialogToolButton->setDefaultAction(GuiManager::get()->getInformationDisplayDialogEnabledAction());
    
    QToolButton* identifyDialogToolButton = new QToolButton();
    identifyDialogToolButton->setDefaultAction(GuiManager::get()->getIdentifyBrainordinateDialogDisplayAction());
    
    QToolButton* helpDialogToolButton = new QToolButton();
    helpDialogToolButton->setDefaultAction(GuiManager::get()->getHelpViewerDialogDisplayAction());
    
    QToolButton* sceneDialogToolButton = new QToolButton();
    sceneDialogToolButton->setDefaultAction(GuiManager::get()->getSceneDialogDisplayAction());
    
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
    
    /*
     * Toolbox control at right of the tab bar
     */
    QToolButton* overlayToolBoxToolButton = new QToolButton();
    overlayToolBoxToolButton->setDefaultAction(overlayToolBoxAction);
    
    QToolButton* layersToolBoxToolButton = new QToolButton();
    layersToolBoxToolButton->setDefaultAction(layersToolBoxAction);
    
    /*
     * Make all tool buttons the same height
     */
    WuQtUtilities::matchWidgetHeights(helpDialogToolButton,
                                      informationDialogToolButton,
                                      identifyDialogToolButton,
                                      sceneDialogToolButton,
                                      toolBarToolButton,
                                      overlayToolBoxToolButton,
                                      layersToolBoxToolButton);
    
    /*
     * Tab bar and controls at far right side of toolbar
     */
    this->tabBarWidget = new QWidget();
    QHBoxLayout* tabBarLayout = new QHBoxLayout(this->tabBarWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(tabBarLayout, 2, 1);
    tabBarLayout->addWidget(this->tabBar, 100);
    tabBarLayout->addWidget(helpDialogToolButton);
    tabBarLayout->addWidget(informationDialogToolButton);
    tabBarLayout->addWidget(identifyDialogToolButton);
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
    this->chartTypeWidget = createChartTypeWidget();
    this->wholeBrainSurfaceOptionsWidget = this->createWholeBrainSurfaceOptionsWidget();
    this->volumeIndicesWidget = this->createVolumeIndicesWidget();
    this->modeWidget = this->createModeWidget();
    this->windowWidget = this->createTabOptionsWidget(windowAspectRatioLockedAction,
                                                      tabAspectRatioLockedAction);
    this->singleSurfaceSelectionWidget = this->createSingleSurfaceOptionsWidget();
    this->surfaceMontageSelectionWidget = this->createSurfaceMontageOptionsWidget();
    m_clippingOptionsWidget = createClippingOptionsWidget();
    this->volumeMontageWidget = this->createVolumeMontageWidget();
    this->volumePlaneWidget = this->createVolumePlaneWidget();
    
    /*
     * Layout the toolbar's widgets.
     */
    m_toolbarWidget = new QWidget();
    this->toolbarWidgetLayout = new QHBoxLayout(m_toolbarWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(this->toolbarWidgetLayout, 2, 1);
    
    this->toolbarWidgetLayout->addWidget(this->viewWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->orientationWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->wholeBrainSurfaceOptionsWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->singleSurfaceSelectionWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->surfaceMontageSelectionWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumePlaneWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeMontageWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeIndicesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartTypeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartAxesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->chartAttributesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->modeWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(m_clippingOptionsWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->windowWidget, 0, Qt::AlignLeft);

    this->toolbarWidgetLayout->addStretch();

    /*
     * Widget below toolbar for user input mode mouse controls
     */
    this->userInputControlsWidgetLayout = new QHBoxLayout();
    this->userInputControlsWidgetLayout->addSpacing(5);
    WuQtUtilities::setLayoutSpacingAndMargins(this->userInputControlsWidgetLayout, 0, 0);
    this->userInputControlsWidget = new QWidget();
    QVBoxLayout* userInputLayout = new QVBoxLayout(this->userInputControlsWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(userInputLayout, 2, 0);
    userInputLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    userInputLayout->addLayout(this->userInputControlsWidgetLayout);
    userInputControlsWidgetActiveInputWidget = NULL;
    
    /*
     * Arrange the tabbar and the toolbar vertically.
     */
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 1, 0);
    layout->addWidget(this->tabBarWidget);
    layout->addWidget(m_toolbarWidget);
    layout->addWidget(this->userInputControlsWidget);
    
    this->addWidget(w);
    
    if (initialBrowserTabContent != NULL) {
        this->addNewTabWithContent(initialBrowserTabContent);
    }
    else {
        AString errorMessage;
        this->createNewTab(errorMessage);
    }
    
    this->updateToolBar();
    
    this->isContructorFinished = true;
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL_VIEWED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_CREATE_TABS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBar::~BrainBrowserWindowToolBar()
{
    this->isDestructionInProgress = true;
    
    if (this->viewOrientationLeftIcon != NULL) {
        delete this->viewOrientationLeftIcon;
        this->viewOrientationLeftIcon = NULL;
    }
    if (this->viewOrientationRightIcon != NULL) {
        delete this->viewOrientationRightIcon;
        this->viewOrientationRightIcon = NULL;
    }

    if (this->viewOrientationAnteriorIcon != NULL) {
        delete this->viewOrientationAnteriorIcon;
        this->viewOrientationAnteriorIcon = NULL;
    }

    if (this->viewOrientationPosteriorIcon != NULL) {
        delete this->viewOrientationPosteriorIcon;
        this->viewOrientationPosteriorIcon = NULL;
    }

    if (this->viewOrientationDorsalIcon != NULL) {
        delete this->viewOrientationDorsalIcon;
        this->viewOrientationDorsalIcon = NULL;
    }

    if (this->viewOrientationVentralIcon != NULL) {
        delete this->viewOrientationVentralIcon;
        this->viewOrientationVentralIcon = NULL;
    }

    if (this->viewOrientationLeftLateralIcon != NULL) {
        delete this->viewOrientationLeftLateralIcon;
        this->viewOrientationLeftLateralIcon = NULL;
    }

    if (this->viewOrientationLeftMedialIcon != NULL) {
        delete this->viewOrientationLeftMedialIcon;
        this->viewOrientationLeftMedialIcon = NULL;
    }

    if (this->viewOrientationRightLateralIcon != NULL) {
        delete this->viewOrientationRightLateralIcon;
        this->viewOrientationRightLateralIcon = NULL;
    }

    if (this->viewOrientationRightMedialIcon != NULL) {
        delete this->viewOrientationRightMedialIcon;
        this->viewOrientationRightMedialIcon = NULL;
    }

    EventManager::get()->removeAllEventsFromListener(this);
    
    this->viewWidgetGroup->clear();
    this->orientationWidgetGroup->clear();
    this->wholeBrainSurfaceOptionsWidgetGroup->clear();
    this->modeWidgetGroup->clear();
    this->singleSurfaceSelectionWidgetGroup->clear();
    
    for (int i = (this->tabBar->count() - 1); i >= 0; i--) {
        this->tabClosed(i);
    }

    this->isDestructionInProgress = false;
}

/**
 * Create a new tab.
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
    this->addNewTabWithContent(tabContent);
    
    return tabContent;
}


/**
 * Add a new tab and clone the content of the given tab.
 * @param browserTabContentToBeCloned
 *    Tab Content that is to be cloned into the new tab.
 */
void 
BrainBrowserWindowToolBar::addNewTabCloneContent(BrowserTabContent* browserTabContentToBeCloned)
{
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
        return;
    }
    
    if (browserTabContentToBeCloned != NULL) {
        /*
         * New tab is clone of tab that was displayed when the new tab was created.
         */
        tabContent->cloneBrowserTabContent(browserTabContentToBeCloned);
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
    addOrInsertNewTab(tabContent,
                      -1);
}

/**
 * Adds a new tab.
 */
void
BrainBrowserWindowToolBar::addNewTab()
{
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
        return;
    }

    this->updateToolBar();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(this->browserWindowIndex).getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}


/**
 * Insert the tab content at the given index.
 *
 * @param browserTabContent
 *    Content of the tab.
 * @param insertAtIndex
 *    Insert the tab at the given index in the tab bar.  Must be greater than or
 *    equal to zero.
 */
void
BrainBrowserWindowToolBar::insertTabAtIndex(BrowserTabContent* browserTabContent,
                      const int32_t insertAtIndex)
{
    CaretAssert(insertAtIndex >= 0);
    
    addOrInsertNewTab(browserTabContent,
                      insertAtIndex);
}

/**
 * Add or Insert the tab content at the given index.
 *
 * @param browserTabContent
 *    Content of the tab.
 * @param insertAtIndex
 *    If greater than or equal to zero, insert the tab at this index.  
 *    Otherwise, use the index from the tab content or just append.
 */
void
BrainBrowserWindowToolBar::addOrInsertNewTab(BrowserTabContent* browserTabContent,
                       const int32_t insertAtIndex)
{
    CaretAssert(browserTabContent);

    this->tabBar->blockSignals(true);

    int32_t newTabIndex = -1;
    if (insertAtIndex >= 0) {
        newTabIndex = this->tabBar->insertTab(insertAtIndex,
                                              "NewTab");
    }
    else {
        const int32_t tabContentIndex = browserTabContent->getTabNumber();
        
        const int32_t numTabs = this->tabBar->count();
        if (numTabs <= 0) {
            newTabIndex = this->tabBar->addTab("NewTab");
        }
        else {
            int insertIndex = 0;
            for (int32_t i = 0; i < numTabs; i++) {
                if (tabContentIndex > this->getTabContentFromTab(i)->getTabNumber()) {
                    insertIndex = i + 1;
                }
            }
            if (insertIndex >= numTabs) {
                newTabIndex = this->tabBar->addTab("NewTab");
            }
            else {
                this->tabBar->insertTab(insertIndex, "NewTab");
                newTabIndex = insertIndex;
            }
        }
    }

    this->tabBar->setTabData(newTabIndex,
                             qVariantFromValue((void*)browserTabContent));

    const int32_t numOpenTabs = this->tabBar->count();
    this->tabBar->setTabsClosable(numOpenTabs > 1);

    this->updateTabName(newTabIndex);
    
    this->tabBar->setCurrentIndex(newTabIndex);
    
    this->tabBar->blockSignals(false);
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
    ModelSurfaceMontage* surfaceMontageModel = NULL;
    ModelVolume* volumeModel = NULL;
    ModelWholeBrain* wholeBrainModel = NULL;
    
    for (std::vector<Model*>::const_iterator iter = allModels.begin();
         iter != allModels.end();
         iter++) {
        ModelSurface* surfaceModel =
            dynamic_cast<ModelSurface*>(*iter);
        if (surfaceModel != NULL) {
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
        else if (dynamic_cast<ModelSurfaceMontage*>(*iter) != NULL) {
            surfaceMontageModel = dynamic_cast<ModelSurfaceMontage*>(*iter);
        }
        else if (dynamic_cast<ModelVolume*>(*iter) != NULL) {
            volumeModel = dynamic_cast<ModelVolume*>(*iter);
        }
        else if (dynamic_cast<ModelWholeBrain*>(*iter) != NULL) {
            wholeBrainModel = dynamic_cast<ModelWholeBrain*>(*iter);
        }
        else if (dynamic_cast<ModelChart*>(*iter)) {
            chartModel = dynamic_cast<ModelChart*>(*iter);
        }
        else {
            CaretAssertMessage(0, AString("Unknow controller type: ") + (*iter)->getNameForGUI(true));
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
    if (leftSurfaceModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (rightSurfaceModel != NULL) {
        numberOfTabsNeeded++;
    }
    if (cerebellumSurfaceModel != NULL) {
        numberOfTabsNeeded++;
    }
    
    const int32_t numberOfTabsToAdd = numberOfTabsNeeded - this->tabBar->count();
    for (int32_t i = 0; i < numberOfTabsToAdd; i++) {
        AString errorMessage;
        this->createNewTab(errorMessage);
    }
    
    int32_t tabIndex = 0;
    tabIndex = loadIntoTab(tabIndex,
                           surfaceMontageModel);
    tabIndex = loadIntoTab(tabIndex,
                           volumeModel);
    tabIndex = loadIntoTab(tabIndex,
                           wholeBrainModel);
    tabIndex = loadIntoTab(tabIndex,
                           chartModel);
    tabIndex = loadIntoTab(tabIndex,
                           leftSurfaceModel);
    tabIndex = loadIntoTab(tabIndex,
                           rightSurfaceModel);
    tabIndex = loadIntoTab(tabIndex,
                           cerebellumSurfaceModel);
    
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
            if (this->createNewTab(errorMessage) == NULL) {
                return -1;
            }
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
                    this->tabBar->setTabData(i, qVariantFromValue((void*)NULL));
                    this->tabClosed(i);
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
        this->tabBar->setTabData(i, qVariantFromValue((void*)NULL));
        this->tabClosed(i);
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
            this->tabBar->setTabData(i, qVariantFromValue((void*)NULL));
            this->tabClosed(i);
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
        AString currentName = btc->getUserName();
        bool ok = false;
        AString newName = QInputDialog::getText(this,
                                                "Set Tab Name",
                                                "New Name (empty to reset)",
                                                QLineEdit::Normal,
                                                currentName,
                                                &ok);
        if (ok) {
            btc->setUserName(newName);
            this->updateTabName(tabIndex);
        }
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
        newName = btc->getName();
    }
    this->tabBar->setTabText(tabIndexForUpdate, newName);
}

/**
 * Close the selected tab.  This method is typically
 * called by the BrowswerWindow's File Menu.
 */
void 
BrainBrowserWindowToolBar::closeSelectedTab()
{
    tabCloseSelected(this->tabBar->currentIndex());
//    const int tabIndex = this->tabBar->currentIndex();
//    if (this->tabBar->count() > 1) {
//        this->tabClosed(tabIndex);
//    }
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
    
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->browserWindowIndex);
    if (browserWindow != NULL) {
        if (browserWindow->isTileTabsSelected()) {
            const BrowserTabContent* btc = getTabContentFromSelectedTab();
            if (btc != NULL) {
                m_tabIndexForTileTabsHighlighting = btc->getTabNumber();
                
                /*
                 * Short timer that will reset the tab highlighting
                 */
                const int timeInMilliseconds = 750;
                QTimer::singleShot(timeInMilliseconds,
                                   this, SLOT(resetTabIndexForTileTabsHighlighting()));
            }
        }
    }
    
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
        tabClosed(tabIndex);
    }
    this->updateGraphicsWindow();
}

/**
 * Close a tab.
 *
 * @param tabIndex
 *    Index of tab that was closed.
 */
void
BrainBrowserWindowToolBar::tabClosed(int tabIndex)
{
    CaretAssertArrayIndex(this-tabBar->tabData(), this->tabBar->count(), tabIndex);
    this->removeTab(tabIndex);
    
    if (this->isDestructionInProgress == false) {
        this->updateToolBar();
        this->updateToolBox();
        emit viewedModelChanged();
    }
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
    this->updateGraphicsWindow();
}

/**
 * Remove the tab at the given index.
 * @param index
 */
void 
BrainBrowserWindowToolBar::removeTab(int tabIndex)
{
    CaretAssertArrayIndex(this-tabBar->tabData(), this->tabBar->count(), tabIndex);
    
    void* p = this->tabBar->tabData(tabIndex).value<void*>();
    if (p != NULL) {
        BrowserTabContent* btc = (BrowserTabContent*)p;
        
        EventBrowserTabDelete deleteTabEvent(btc);
        EventManager::get()->sendEvent(deleteTabEvent.getPointer());
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
     * If there are no models, close all but the first tab.
     */
    EventModelGetAll getAllModelsEvent;
    EventManager::get()->sendEvent(getAllModelsEvent.getPointer());
    if (getAllModelsEvent.getFirstModel() == NULL) {
        for (int i = (this->tabBar->count() - 1); i >= 0; i--) {
            this->removeTab(i);
        }
    }
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    
    const ModelTypeEnum::Enum viewModel = this->updateViewWidget(browserTabContent);
    
    bool showOrientationWidget = false;
    bool showWholeBrainSurfaceOptionsWidget = false;
    bool showSingleSurfaceOptionsWidget = false;
    bool showSurfaceMontageOptionsWidget = false;
    bool showClippingOptionsWidget = true;
    bool showVolumeIndicesWidget = false;
    bool showVolumePlaneWidget = false;
    bool showVolumeMontageWidget = false;
    
    bool showChartAxesWidget = false;
    bool showChartTypeWidget = false;
    bool showChartAttributesWidget = false;
    
    bool showModeWidget = true;
    bool showWindowWidget = true;
    
    switch (viewModel) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
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
            showVolumePlaneWidget = true;
            showVolumeMontageWidget = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            showOrientationWidget = true;
            showWholeBrainSurfaceOptionsWidget = true;
            showVolumeIndicesWidget = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
        {
            showChartTypeWidget = true;
            showClippingOptionsWidget = false;
            
            ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
            if (modelChart != NULL) {
                switch (modelChart->getSelectedChartDataType(browserTabContent->getTabNumber())) {
                    case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                        showChartAttributesWidget = true;
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                        showChartAttributesWidget = true;
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                        showChartAxesWidget = true;
                        showChartAttributesWidget = true;
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                        showChartAxesWidget = true;
                        showChartAttributesWidget = true;
                        break;
                    case ChartDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                        showChartAxesWidget = true;
                        showChartAttributesWidget = true;
                        break;
                }
            }
        }
            break;
    }
    
    /*
     * Need to turn off display of all widgets, 
     * otherwise, the toolbar width may be overly
     * expanded with empty space as other widgets
     * are turned on and off.
     */
    this->orientationWidget->setVisible(false);
    this->wholeBrainSurfaceOptionsWidget->setVisible(false);
    this->singleSurfaceSelectionWidget->setVisible(false);
    this->surfaceMontageSelectionWidget->setVisible(false);
    this->chartTypeWidget->setVisible(false);
    this->chartAxesWidget->setVisible(false);
    this->chartAttributesWidget->setVisible(false);
    this->volumeIndicesWidget->setVisible(false);
    this->volumePlaneWidget->setVisible(false);
    this->volumeMontageWidget->setVisible(false);
    this->modeWidget->setVisible(false);
    this->windowWidget->setVisible(false);
    m_clippingOptionsWidget->setVisible(false);
    
    this->orientationWidget->setVisible(showOrientationWidget);
    this->wholeBrainSurfaceOptionsWidget->setVisible(showWholeBrainSurfaceOptionsWidget);
    this->singleSurfaceSelectionWidget->setVisible(showSingleSurfaceOptionsWidget);
    this->surfaceMontageSelectionWidget->setVisible(showSurfaceMontageOptionsWidget);
    this->chartTypeWidget->setVisible(showChartTypeWidget);
    this->chartAxesWidget->setVisible(showChartAxesWidget);
    this->chartAttributesWidget->setVisible(showChartAttributesWidget);
    this->volumeIndicesWidget->setVisible(showVolumeIndicesWidget);
    this->volumePlaneWidget->setVisible(showVolumePlaneWidget);
    this->volumeMontageWidget->setVisible(showVolumeMontageWidget);
    this->modeWidget->setVisible(showModeWidget);
    m_clippingOptionsWidget->setVisible(showClippingOptionsWidget);
    this->windowWidget->setVisible(showWindowWidget);

    updateToolBarComponents(browserTabContent);
    
    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    if (this->updateCounter != 0) {
        CaretLogSevere("Update counter is non-zero at end of updateToolBar()");
    }
    
    this->updateTabName(-1);
    
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->browserWindowIndex);
    if (browserWindow != NULL) {
        if (browserWindow->isFullScreen()) {
            this->setVisible(false);
        }
        else {
            this->setVisible(true);
        }
    }
    
    /*
     * Try to avoid resizing of Toolbar widget (view, orientation, etc)
     * height when models are changed. Let it grow but never shrink.
     */
    if (isVisible()) {
        if (m_toolbarWidget->isVisible()) {
            const int sizeHintHeight = m_toolbarWidget->sizeHint().height();
            const int actualHeight = m_toolbarWidget->height();
            if (sizeHintHeight >= actualHeight) {
                m_toolbarWidget->setFixedHeight(sizeHintHeight);
            }
        }
    }
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
        this->updateChartAxesWidget(browserTabContent);
        this->updateChartAttributesWidget(browserTabContent);
        this->updateVolumeMontageWidget(browserTabContent);
        this->updateVolumePlaneWidget(browserTabContent);
        this->updateModeWidget(browserTabContent);
        this->updateTabOptionsWidget(browserTabContent);
        this->updateClippingOptionsWidget(browserTabContent);
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
    this->viewModeChartRadioButton = new QRadioButton("Chart");
    this->viewModeSurfaceRadioButton = new QRadioButton("Surface");
    this->viewModeSurfaceMontageRadioButton = new QRadioButton("Montage");
    this->viewModeVolumeRadioButton = new QRadioButton("Volume");
    this->viewModeWholeBrainRadioButton = new QRadioButton("All");
    
//    this->viewModeChartRadioButton->setVisible(false);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(this->viewModeChartRadioButton);
    layout->addWidget(this->viewModeSurfaceMontageRadioButton);
    layout->addWidget(this->viewModeVolumeRadioButton);
    layout->addWidget(this->viewModeWholeBrainRadioButton);
    layout->addWidget(this->viewModeSurfaceRadioButton);
    layout->addStretch();

    QButtonGroup* viewModeRadioButtonGroup = new QButtonGroup(this);
    viewModeRadioButtonGroup->addButton(this->viewModeChartRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeSurfaceRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeSurfaceMontageRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeVolumeRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeWholeBrainRadioButton);
    QObject::connect(viewModeRadioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(viewModeRadioButtonClicked(QAbstractButton*)));
    
    this->viewWidgetGroup = new WuQWidgetObjectGroup(this);
    this->viewWidgetGroup->add(this->viewModeChartRadioButton);
    this->viewWidgetGroup->add(this->viewModeSurfaceRadioButton);
    this->viewWidgetGroup->add(this->viewModeSurfaceMontageRadioButton);
    this->viewWidgetGroup->add(this->viewModeVolumeRadioButton);
    this->viewWidgetGroup->add(this->viewModeWholeBrainRadioButton);
    
    QWidget* w = this->createToolWidget("View", 
                                        widget, 
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
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->viewWidgetGroup->blockAllSignals(true);
    
    /*
     * Enable buttons for valid types
     */
    if (browserTabContent != NULL) {
        this->viewModeSurfaceRadioButton->setEnabled(browserTabContent->isSurfaceModelValid());
        this->viewModeSurfaceMontageRadioButton->setEnabled(browserTabContent->isSurfaceMontageModelValid());
        this->viewModeVolumeRadioButton->setEnabled(browserTabContent->isVolumeSliceModelValid());
        this->viewModeWholeBrainRadioButton->setEnabled(browserTabContent->isWholeBrainModelValid());
        this->viewModeChartRadioButton->setEnabled(browserTabContent->isChartModelValid());
    }
    
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            this->viewModeSurfaceRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            this->viewModeSurfaceMontageRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            this->viewModeVolumeRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            this->viewModeWholeBrainRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            this->viewModeChartRadioButton->setChecked(true);
            break;
    }
    
    this->viewWidgetGroup->blockAllSignals(false);

    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
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
    this->viewOrientationLeftIcon = WuQtUtilities::loadIcon(":/ToolBar/view-left.png");
    this->viewOrientationRightIcon = WuQtUtilities::loadIcon(":/ToolBar/view-right.png");
    this->viewOrientationAnteriorIcon = WuQtUtilities::loadIcon(":/ToolBar/view-anterior.png");
    this->viewOrientationPosteriorIcon = WuQtUtilities::loadIcon(":/ToolBar/view-posterior.png");
    this->viewOrientationDorsalIcon = WuQtUtilities::loadIcon(":/ToolBar/view-dorsal.png");
    this->viewOrientationVentralIcon = WuQtUtilities::loadIcon(":/ToolBar/view-ventral.png");
    this->viewOrientationLeftLateralIcon = WuQtUtilities::loadIcon(":/ToolBar/view-left-lateral.png");
    this->viewOrientationLeftMedialIcon = WuQtUtilities::loadIcon(":/ToolBar/view-left-medial.png");
    this->viewOrientationRightLateralIcon = WuQtUtilities::loadIcon(":/ToolBar/view-right-lateral.png");
    this->viewOrientationRightMedialIcon = WuQtUtilities::loadIcon(":/ToolBar/view-right-medial.png");
    
    this->orientationLeftOrLateralToolButtonAction = WuQtUtilities::createAction("L", 
                                                                        "View from a LEFT perspective", 
                                                                        this, 
                                                                        this, 
                                                                        SLOT(orientationLeftOrLateralToolButtonTriggered(bool)));
    if (this->viewOrientationLeftIcon != NULL) {
        this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationLeftIcon);
    }
    else {
        this->orientationLeftOrLateralToolButtonAction->setIconText("L");
    }
    
    this->orientationRightOrMedialToolButtonAction = WuQtUtilities::createAction("R", 
                                                                         "View from a RIGHT perspective", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(orientationRightOrMedialToolButtonTriggered(bool)));
    if (this->viewOrientationRightIcon != NULL) {
        this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationRightIcon);
    }
    else {
        this->orientationRightOrMedialToolButtonAction->setIconText("R");
    }
    
    this->orientationAnteriorToolButtonAction = WuQtUtilities::createAction("A", 
                                                                            "View from an ANTERIOR perspective", 
                                                                            this, 
                                                                            this, 
                                                                            SLOT(orientationAnteriorToolButtonTriggered(bool)));
    if (this->viewOrientationAnteriorIcon != NULL) {
        this->orientationAnteriorToolButtonAction->setIcon(*this->viewOrientationAnteriorIcon);
    }
    else {
        this->orientationAnteriorToolButtonAction->setIconText("A");
    }
    
    this->orientationPosteriorToolButtonAction = WuQtUtilities::createAction("P", 
                                                                             "View from a POSTERIOR perspective", 
                                                                             this, 
                                                                             this, 
                                                                             SLOT(orientationPosteriorToolButtonTriggered(bool)));
    if (this->viewOrientationPosteriorIcon != NULL) {
        this->orientationPosteriorToolButtonAction->setIcon(*this->viewOrientationPosteriorIcon);
    }
    else {
        this->orientationPosteriorToolButtonAction->setIconText("P");
    }
    
    this->orientationDorsalToolButtonAction = WuQtUtilities::createAction("D", 
                                                                          "View from a DORSAL perspective", 
                                                                          this, 
                                                                          this, 
                                                                          SLOT(orientationDorsalToolButtonTriggered(bool)));
    if (this->viewOrientationDorsalIcon != NULL) {
        this->orientationDorsalToolButtonAction->setIcon(*this->viewOrientationDorsalIcon);
    }
    else {
        this->orientationDorsalToolButtonAction->setIconText("D");
    }
    
    this->orientationVentralToolButtonAction = WuQtUtilities::createAction("V", 
                                                                           "View from a VENTRAL perspective", 
                                                                           this, 
                                                                           this, 
                                                                           SLOT(orientationVentralToolButtonTriggered(bool)));
    if (this->viewOrientationVentralIcon != NULL) {
        this->orientationVentralToolButtonAction->setIcon(*this->viewOrientationVentralIcon);
    }
    else {
        this->orientationVentralToolButtonAction->setIconText("V");
    }
    
    
    this->orientationLateralMedialToolButtonAction = WuQtUtilities::createAction("LM",
                                                                                 "View from a Lateral/Medial perspective", 
                                                                                 this, 
                                                                                 this, 
                                                                                 SLOT(orientationLateralMedialToolButtonTriggered(bool)));
    
    this->orientationDorsalVentralToolButtonAction = WuQtUtilities::createAction("DV",
                                                                                    "View from a Dorsal/Ventral perspective", 
                                                                                    this, 
                                                                                    this, 
                                                                                    SLOT(orientationDorsalVentralToolButtonTriggered(bool)));
    
    this->orientationAnteriorPosteriorToolButtonAction = WuQtUtilities::createAction("AP", 
                                                                                        "View from a Anterior/Posterior perspective", 
                                                                                        this, 
                                                                                        this, 
                                                                                        SLOT(orientationAnteriorPosteriorToolButtonTriggered(bool)));
    
    this->orientationResetToolButtonAction = WuQtUtilities::createAction("R\nE\nS\nE\nT",
                                                                         "Reset the view to dorsal and remove any panning or zooming", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(orientationResetToolButtonTriggered(bool)));
    
    this->orientationLeftOrLateralToolButton = new QToolButton();
    this->orientationLeftOrLateralToolButton->setDefaultAction(this->orientationLeftOrLateralToolButtonAction);
    
    this->orientationRightOrMedialToolButton = new QToolButton();
    this->orientationRightOrMedialToolButton->setDefaultAction(this->orientationRightOrMedialToolButtonAction);
    
    this->orientationAnteriorToolButton = new QToolButton();
    this->orientationAnteriorToolButton->setDefaultAction(this->orientationAnteriorToolButtonAction);
    
    this->orientationPosteriorToolButton = new QToolButton();
    this->orientationPosteriorToolButton->setDefaultAction(this->orientationPosteriorToolButtonAction);
    
    this->orientationDorsalToolButton = new QToolButton();
    this->orientationDorsalToolButton->setDefaultAction(this->orientationDorsalToolButtonAction);
    
    this->orientationVentralToolButton = new QToolButton();
    this->orientationVentralToolButton->setDefaultAction(this->orientationVentralToolButtonAction);
    
    this->orientationLateralMedialToolButton = new QToolButton();
    this->orientationLateralMedialToolButton->setDefaultAction(this->orientationLateralMedialToolButtonAction);
    
    this->orientationDorsalVentralToolButton = new QToolButton();
    this->orientationDorsalVentralToolButton->setDefaultAction(this->orientationDorsalVentralToolButtonAction);
    
    this->orientationAnteriorPosteriorToolButton = new QToolButton();
    this->orientationAnteriorPosteriorToolButton->setDefaultAction(this->orientationAnteriorPosteriorToolButtonAction);
    
    WuQtUtilities::matchWidgetWidths(this->orientationLateralMedialToolButton,
                                     this->orientationDorsalVentralToolButton,
                                     this->orientationAnteriorPosteriorToolButton);
    
    QToolButton* orientationResetToolButton = new QToolButton();
    orientationResetToolButton->setDefaultAction(this->orientationResetToolButtonAction);

    this->orientationCustomViewSelectToolButton = new QToolButton();
    this->orientationCustomViewSelectToolButton->setDefaultAction(this->customViewAction);
    this->orientationCustomViewSelectToolButton->setSizePolicy(QSizePolicy::Minimum,
                                                               QSizePolicy::Fixed);
    
    QGridLayout* buttonGridLayout = new QGridLayout();
    buttonGridLayout->setColumnStretch(3, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(buttonGridLayout, 0, 0);
    buttonGridLayout->addWidget(this->orientationLeftOrLateralToolButton,      0, 0);
    buttonGridLayout->addWidget(this->orientationRightOrMedialToolButton,     0, 1);
    buttonGridLayout->addWidget(this->orientationDorsalToolButton,    1, 0);
    buttonGridLayout->addWidget(this->orientationVentralToolButton,   1, 1);
    buttonGridLayout->addWidget(this->orientationAnteriorToolButton,  2, 0);
    buttonGridLayout->addWidget(this->orientationPosteriorToolButton, 2, 1);
    buttonGridLayout->addWidget(this->orientationLateralMedialToolButton, 0, 2);
    buttonGridLayout->addWidget(this->orientationDorsalVentralToolButton, 1, 2);
    buttonGridLayout->addWidget(this->orientationAnteriorPosteriorToolButton, 2, 2);
    buttonGridLayout->addWidget(this->orientationCustomViewSelectToolButton, 3, 0, 1, 5, Qt::AlignHCenter);
    buttonGridLayout->addWidget(orientationResetToolButton, 0, 4, 3, 1);
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(buttonGridLayout);
    
    this->orientationWidgetGroup = new WuQWidgetObjectGroup(this);
    this->orientationWidgetGroup->add(this->orientationLeftOrLateralToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationRightOrMedialToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationAnteriorToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationPosteriorToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationDorsalToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationVentralToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationResetToolButtonAction);

    QWidget* orientWidget = this->createToolWidget("Orientation", 
                                                   w, 
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
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    this->orientationWidgetGroup->blockAllSignals(true);
    
    const Model* mdc = this->getDisplayedModel();
    if (mdc != NULL) {
        const ModelSurface* mdcs = dynamic_cast<const ModelSurface*>(mdc);
        const ModelSurfaceMontage* mdcsm = dynamic_cast<const ModelSurfaceMontage*>(mdc);
        const ModelVolume* mdcv = dynamic_cast<const ModelVolume*>(mdc);
        const ModelWholeBrain* mdcwb = dynamic_cast<const ModelWholeBrain*>(mdc);
        
        bool rightFlag = false;
        bool leftFlag = false;
        bool leftRightFlag = false;
        
        bool enableDualViewOrientationButtons = false;
        bool showDualViewOrientationButtons = false;
        bool showSingleViewOrientationButtons = false;
        
        if (mdcs != NULL) {
            const Surface* surface = mdcs->getSurface();
            const StructureEnum::Enum structure = surface->getStructure();
            if (StructureEnum::isLeft(structure)) {
                leftFlag = true;
            }
            else if (StructureEnum::isRight(structure)) {
                rightFlag = true;
            }
            else {
                leftRightFlag = true;
            }
            
            showSingleViewOrientationButtons = true;
        }
        else if (mdcsm != NULL) {
            AString latMedLeftRightText = "LM";
            AString latMedLeftRightToolTipText = "View from a Lateral/Medial perspective";
            switch (mdcsm->getSelectedConfigurationType(tabIndex)) {
                case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
                    latMedLeftRightText = "LR";
                    latMedLeftRightToolTipText = "View from a Right/Left Perspective";
                    enableDualViewOrientationButtons = true;
                    break;
                case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
                    enableDualViewOrientationButtons = true;
                    break;
                case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
                    break;
            }
            
            this->orientationLateralMedialToolButtonAction->setText(latMedLeftRightText);
            WuQtUtilities::setToolTipAndStatusTip(this->orientationLateralMedialToolButtonAction,
                                                  latMedLeftRightToolTipText);
            
            showDualViewOrientationButtons = true;
        }
        else if (mdcv != NULL) {
            /* nothing */
        }
        else if (mdcwb != NULL) {
            leftRightFlag = true;
            showSingleViewOrientationButtons = true;
        }
        else {
            CaretAssertMessage(0, "Unknown model display controller type");
        }
        
        if (rightFlag || leftFlag) {
            if (rightFlag) {
                if (this->viewOrientationRightLateralIcon != NULL) {
                    this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationRightLateralIcon);
                }
                else {
                    this->orientationLeftOrLateralToolButtonAction->setIconText("L");
                }
                if (this->viewOrientationRightMedialIcon != NULL) {
                    this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationRightMedialIcon);
                }
                else {
                    this->orientationRightOrMedialToolButtonAction->setIconText("M");
                }
            }
            else if (leftFlag) {
                if (this->viewOrientationLeftLateralIcon != NULL) {
                    this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationLeftLateralIcon);
                }
                else {
                    this->orientationLeftOrLateralToolButtonAction->setIconText("L");
                }
                if (this->viewOrientationLeftMedialIcon != NULL) {
                    this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationLeftMedialIcon);
                }
                else {
                    this->orientationRightOrMedialToolButtonAction->setIconText("M");
                }
            }
            WuQtUtilities::setToolTipAndStatusTip(this->orientationLeftOrLateralToolButtonAction, 
                                                  "View from a LATERAL perspective");
            WuQtUtilities::setToolTipAndStatusTip(this->orientationRightOrMedialToolButtonAction, 
                                                  "View from a MEDIAL perspective");
        }
        else if (leftRightFlag) {
            if (this->viewOrientationLeftIcon != NULL) {
                this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationLeftIcon);
            }
            else {
                this->orientationLeftOrLateralToolButtonAction->setIconText("L");
            }
            if (this->viewOrientationRightIcon != NULL) {
                this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationRightIcon);
            }
            else {
                this->orientationRightOrMedialToolButtonAction->setIconText("R");
            }
            WuQtUtilities::setToolTipAndStatusTip(this->orientationLeftOrLateralToolButtonAction, 
                                                  "View from a LEFT perspective");
            WuQtUtilities::setToolTipAndStatusTip(this->orientationRightOrMedialToolButtonAction, 
                                                  "View from a RIGHT perspective");
        }

        /*
         * The dual view buttons are not need for a flat map montage.
         * However, if they are hidden, their space is not reallocated and the 
         * Reset button remains on the right and it looks weird.  So, 
         * display them but disable them when a flat map montage.
         */
        this->orientationLateralMedialToolButton->setVisible(showDualViewOrientationButtons);
        this->orientationDorsalVentralToolButton->setVisible(showDualViewOrientationButtons);
        this->orientationAnteriorPosteriorToolButton->setVisible(showDualViewOrientationButtons);

        this->orientationLateralMedialToolButton->setEnabled(enableDualViewOrientationButtons);
        this->orientationDorsalVentralToolButton->setEnabled(enableDualViewOrientationButtons);
        this->orientationAnteriorPosteriorToolButton->setEnabled(enableDualViewOrientationButtons);
        
        
        this->orientationLeftOrLateralToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationRightOrMedialToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationDorsalToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationVentralToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationAnteriorToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationPosteriorToolButton->setVisible(showSingleViewOrientationButtons);
    }
    this->orientationWidgetGroup->blockAllSignals(false);
        
    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

/**
 * Create the whole brain surface options widget.
 *
 * @return The whole brain surface options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createWholeBrainSurfaceOptionsWidget()
{
    
    this->wholeBrainSurfaceTypeComboBox = WuQFactory::newComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceTypeComboBox,
                                          "Select the geometric type of surface for display");
    QObject::connect(this->wholeBrainSurfaceTypeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(wholeBrainSurfaceTypeComboBoxIndexChanged(int)));
    
    /*
     * Left
     */
    this->wholeBrainSurfaceLeftCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceLeftCheckBox,
                                          "Enable/Disable display of the left cortical surface");
    QObject::connect(this->wholeBrainSurfaceLeftCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceLeftCheckBoxStateChanged(int)));
    
    QAction* leftSurfaceAction = WuQtUtilities::createAction("Left", 
                                                             "Select the whole brain left surface", 
                                                             this, 
                                                             this, 
                                                             SLOT(wholeBrainSurfaceLeftToolButtonTriggered(bool)));
    QToolButton* wholeBrainLeftSurfaceToolButton = new QToolButton();
    wholeBrainLeftSurfaceToolButton->setDefaultAction(leftSurfaceAction);
    
    /*
     * Right
     */
    this->wholeBrainSurfaceRightCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceRightCheckBox,
                                          "Enable/Disable display of the right cortical surface");
    QObject::connect(this->wholeBrainSurfaceRightCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceRightCheckBoxStateChanged(int)));
    
    QAction* rightSurfaceAction = WuQtUtilities::createAction("Right", 
                                                             "Select the whole brain right surface", 
                                                             this,
                                                             this, 
                                                             SLOT(wholeBrainSurfaceRightToolButtonTriggered(bool)));
    QToolButton* wholeBrainRightSurfaceToolButton = new QToolButton();
    wholeBrainRightSurfaceToolButton->setDefaultAction(rightSurfaceAction);
    
    /*
     * Cerebellum
     */
    this->wholeBrainSurfaceCerebellumCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceCerebellumCheckBox,
                                          "Enable/Disable display of the cerebellum surface");
    QObject::connect(this->wholeBrainSurfaceCerebellumCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceCerebellumCheckBoxStateChanged(int)));
    
    QAction* cerebellumSurfaceAction = WuQtUtilities::createAction("Cerebellum", 
                                                              "Select the whole brain cerebellum surface", 
                                                              this, 
                                                              this, 
                                                              SLOT(wholeBrainSurfaceCerebellumToolButtonTriggered(bool)));
    QToolButton* wholeBrainCerebellumSurfaceToolButton = new QToolButton();
    wholeBrainCerebellumSurfaceToolButton->setDefaultAction(cerebellumSurfaceAction);

    /*
     * Left/Right separation
     */
    const int separationSpinngerWidth = 48;
    this->wholeBrainSurfaceSeparationLeftRightSpinBox = WuQFactory::newDoubleSpinBox();
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setDecimals(0);
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setFixedWidth(separationSpinngerWidth);
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setMinimum(-100000.0);
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setMaximum(100000.0);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationLeftRightSpinBox,
                                          "Adjust the separation of the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationLeftRightSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double)));
    
    /*
     * Cerebellum separation
     */
    this->wholeBrainSurfaceSeparationCerebellumSpinBox = WuQFactory::newDoubleSpinBox();
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setDecimals(0);
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setFixedWidth(separationSpinngerWidth);
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setMinimum(-100000.0);
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setMaximum(100000.0);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationCerebellumSpinBox,
                                          "Adjust the separation of the cerebellum from the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationCerebellumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double)));
    
    

    QLabel* columnTwoSpaceLabel = new QLabel(" ");
    wholeBrainLeftSurfaceToolButton->setText("L");
    wholeBrainRightSurfaceToolButton->setText("R");
    wholeBrainCerebellumSurfaceToolButton->setText("C");
    
    bool originalFlag = false;
    QGridLayout* gridLayout = new QGridLayout();
    if (originalFlag) {
        gridLayout->setVerticalSpacing(2);
        gridLayout->setHorizontalSpacing(2);
        gridLayout->addWidget(this->wholeBrainSurfaceTypeComboBox, 0, 0, 1, 6);
        gridLayout->addWidget(this->wholeBrainSurfaceLeftCheckBox, 1, 0);
        gridLayout->addWidget(wholeBrainLeftSurfaceToolButton, 1, 1);
        gridLayout->addWidget(columnTwoSpaceLabel, 1, 2);
        gridLayout->addWidget(this->wholeBrainSurfaceRightCheckBox, 1, 3);
        gridLayout->addWidget(wholeBrainRightSurfaceToolButton, 1, 4);
        gridLayout->addWidget(this->wholeBrainSurfaceSeparationLeftRightSpinBox, 1, 5);
        gridLayout->addWidget(this->wholeBrainSurfaceCerebellumCheckBox, 2, 0);
        gridLayout->addWidget(wholeBrainCerebellumSurfaceToolButton, 2, 1);
        gridLayout->addWidget(this->wholeBrainSurfaceSeparationCerebellumSpinBox, 2, 5);
    }
    else {
        gridLayout->setVerticalSpacing(2);
        gridLayout->setHorizontalSpacing(2);
        gridLayout->addWidget(this->wholeBrainSurfaceTypeComboBox, 0, 0, 1, 6);
        gridLayout->addWidget(this->wholeBrainSurfaceLeftCheckBox, 1, 0);
        gridLayout->addWidget(wholeBrainLeftSurfaceToolButton, 1, 1);
        gridLayout->addWidget(this->wholeBrainSurfaceRightCheckBox, 2, 0);
        gridLayout->addWidget(wholeBrainRightSurfaceToolButton, 2, 1);
        gridLayout->addWidget(this->wholeBrainSurfaceCerebellumCheckBox, 3, 0);
        gridLayout->addWidget(wholeBrainCerebellumSurfaceToolButton, 3, 1);
        gridLayout->addWidget(this->wholeBrainSurfaceSeparationLeftRightSpinBox, 1, 2, 2, 1);
        gridLayout->addWidget(this->wholeBrainSurfaceSeparationCerebellumSpinBox, 3, 2);
    }
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(gridLayout);
    
    this->wholeBrainSurfaceOptionsWidgetGroup = new WuQWidgetObjectGroup(this);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceTypeComboBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceLeftCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainLeftSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceRightCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainRightSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceCerebellumCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainCerebellumSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceSeparationLeftRightSpinBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceSeparationCerebellumSpinBox);
    
    QWidget* w = this->createToolWidget("Surface Viewing", 
                                        widget, 
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
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
 
    ModelWholeBrain* wholeBrainModel = browserTabContent->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        const int32_t tabNumber = browserTabContent->getTabNumber();
        
        this->wholeBrainSurfaceOptionsWidgetGroup->blockAllSignals(true);
        
        std::vector<SurfaceTypeEnum::Enum> availableSurfaceTypes;
        wholeBrainModel->getAvailableSurfaceTypes(availableSurfaceTypes);
        
        const SurfaceTypeEnum::Enum selectedSurfaceType = wholeBrainModel->getSelectedSurfaceType(tabNumber);
        
        int32_t defaultIndex = 0;
        this->wholeBrainSurfaceTypeComboBox->clear();
        int32_t numSurfaceTypes = static_cast<int32_t>(availableSurfaceTypes.size());
        for (int32_t i = 0; i < numSurfaceTypes; i++) {
            const SurfaceTypeEnum::Enum st = availableSurfaceTypes[i];
            if (st == selectedSurfaceType) {
                defaultIndex = this->wholeBrainSurfaceTypeComboBox->count();
            }
            const AString name = SurfaceTypeEnum::toGuiName(st);
            const int integerCode = SurfaceTypeEnum::toIntegerCode(st);
            this->wholeBrainSurfaceTypeComboBox->addItem(name,
                                                         integerCode);
        }
        if (defaultIndex < this->wholeBrainSurfaceTypeComboBox->count()) {
            this->wholeBrainSurfaceTypeComboBox->setCurrentIndex(defaultIndex);
        }
        
        this->wholeBrainSurfaceLeftCheckBox->setChecked(browserTabContent->isWholeBrainLeftEnabled());
        this->wholeBrainSurfaceRightCheckBox->setChecked(browserTabContent->isWholeBrainRightEnabled());
        this->wholeBrainSurfaceCerebellumCheckBox->setChecked(browserTabContent->isWholeBrainCerebellumEnabled());
        
        this->wholeBrainSurfaceSeparationLeftRightSpinBox->setValue(browserTabContent->getWholeBrainLeftRightSeparation());
        this->wholeBrainSurfaceSeparationCerebellumSpinBox->setValue(browserTabContent->getWholeBrainCerebellumSeparation());
        
        this->wholeBrainSurfaceOptionsWidgetGroup->blockAllSignals(false);
    }
    
    
    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

/**
 * Create the volume indices widget.
 *
 * @return  The volume indices widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumeIndicesWidget()
{
    m_sliceSelectionComponent = new BrainBrowserWindowToolBarSliceSelection(this);
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
    this->modeInputModeAnnotationsAction = WuQtUtilities::createAction("Annotate",
                                                                      "Perform annotate operations with mouse",
                                                                      this);
    this->modeInputModeAnnotationsAction->setCheckable(true);
    QToolButton* inputModeAnnotationsToolButton = new QToolButton();
    inputModeAnnotationsToolButton->setDefaultAction(this->modeInputModeAnnotationsAction);
    
    /*
     * Borders 
     */ 
    this->modeInputModeBordersAction = WuQtUtilities::createAction("Border",
                                                                    "Perform border operations with mouse",
                                                                    this);
    this->modeInputModeBordersAction->setCheckable(true);
    QToolButton* inputModeBordersToolButton = new QToolButton();
    inputModeBordersToolButton->setDefaultAction(this->modeInputModeBordersAction);
    
    /*
     * Foci
     */
    this->modeInputModeFociAction = WuQtUtilities::createAction("Foci",
                                                                 "Perform foci operations with mouse",
                                                                 this);
    this->modeInputModeFociAction->setCheckable(true);
    QToolButton* inputModeFociToolButton = new QToolButton();
    inputModeFociToolButton->setDefaultAction(this->modeInputModeFociAction);
    
    /*
     * Image
     */
    this->modeInputModeImageAction = WuQtUtilities::createAction("Image",
                                                                 "Edit Image Control Points",
                                                                 this);
    this->modeInputModeImageAction->setCheckable(true);
    QToolButton* inputModeImageToolButton = new QToolButton();
    inputModeImageToolButton->setDefaultAction(this->modeInputModeImageAction);
    
    /*
     * Volume Edit
     */
    this->modeInputVolumeEditAction = WuQtUtilities::createAction("Volume",
                                                                  "Edit volume voxels",
                                                                  this);
    this->modeInputVolumeEditAction->setCheckable(true);
    QToolButton* inputModeVolumeEditButton = new QToolButton();
    inputModeVolumeEditButton->setDefaultAction(this->modeInputVolumeEditAction);
    
    /*
     * View Mode
     */
    this->modeInputModeViewAction = WuQtUtilities::createAction("View",
                                                                 "Perform viewing operations with mouse\n"
                                                                 "\n"
                                                                 "Identify: Click left mouse button (might cause rotation)\n"
#ifdef CARET_OS_MACOSX
                                                                 "Identify: Click left mouse button while keyboard shift and apple keys are down (prevents rotation)\n"
#else // CARET_OS_MACOSX
                                                                 "Identify: Click left mouse button while keyboard shift and controls keys are down (prevents rotation)\n"
#endif // CARET_OS_MACOSX
                                                                 "Pan:      Move mouse with left mouse button down and keyboard shift key down\n"
                                                                 "Rotate:   Move mouse with left mouse button down\n"
#ifdef CARET_OS_MACOSX
                                                                 "Zoom:     Move mouse with left mouse button down and keyboard apple key down",
#else // CARET_OS_MACOSX
                                                                 "Zoom:     Move mouse with left mouse button down and keyboard control key down",
#endif // CARET_OS_MACOSX
                                                                 this);
    this->modeInputModeViewAction->setCheckable(true);
    QToolButton* inputModeViewToolButton = new QToolButton();
    inputModeViewToolButton->setDefaultAction(this->modeInputModeViewAction);
    
    WuQtUtilities::matchWidgetWidths(inputModeAnnotationsToolButton,
                                     inputModeBordersToolButton,
                                     //inputModeFociToolButton,
                                     //inputModeImageToolButton,
                                     inputModeViewToolButton,
                                     inputModeVolumeEditButton);
//    inputModeFociToolButton->setContentsMargins(0, 0, 0, 0);
//    inputModeImageToolButton->setContentsMargins(0, 0, 0, 0);
    inputModeFociToolButton->setSizePolicy(QSizePolicy::Preferred,
                                           inputModeFociToolButton->sizePolicy().verticalPolicy());
    inputModeImageToolButton->setSizePolicy(QSizePolicy::Preferred,
                                           inputModeImageToolButton->sizePolicy().verticalPolicy());
//    inputModeFociToolButton->setFixedWidth(inputModeFociToolButton->sizeHint().width());
//    inputModeImageToolButton->setFixedWidth(inputModeImageToolButton->sizeHint().width());
    
    /*
     * Layout for input modes
     */
    QWidget* inputModeWidget = new QWidget();
    QGridLayout* inputModeLayout = new QGridLayout(inputModeWidget);
    int modeRow = 0;
    WuQtUtilities::setLayoutSpacingAndMargins(inputModeLayout, 2, 2);
    inputModeLayout->addWidget(inputModeAnnotationsToolButton, modeRow, 0, 1, 2, Qt::AlignHCenter);
    modeRow++;
    inputModeLayout->addWidget(inputModeBordersToolButton, modeRow, 0, 1, 2, Qt::AlignHCenter);
    modeRow++;
    inputModeLayout->addWidget(inputModeFociToolButton, modeRow, 0);
    inputModeLayout->addWidget(inputModeImageToolButton, modeRow, 1);
    modeRow++;
    inputModeLayout->addWidget(inputModeViewToolButton, modeRow, 0, 1, 2, Qt::AlignHCenter);
    modeRow++;
    inputModeLayout->addWidget(inputModeVolumeEditButton, modeRow, 0, 1, 2, Qt::AlignHCenter);
    modeRow++;
    
    this->modeInputModeActionGroup = new QActionGroup(this);
    this->modeInputModeActionGroup->addAction(this->modeInputModeAnnotationsAction);
    this->modeInputModeActionGroup->addAction(this->modeInputModeBordersAction);
    this->modeInputModeActionGroup->addAction(this->modeInputModeFociAction);
    this->modeInputModeActionGroup->addAction(this->modeInputModeImageAction);
    this->modeInputModeActionGroup->addAction(this->modeInputModeViewAction);
    this->modeInputModeActionGroup->addAction(this->modeInputVolumeEditAction);
    QObject::connect(this->modeInputModeActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(modeInputModeActionTriggered(QAction*)));
    this->modeInputModeActionGroup->setExclusive(true);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(inputModeWidget, 0, Qt::AlignHCenter);
    layout->addStretch();
    
    this->modeWidgetGroup = new WuQWidgetObjectGroup(this);
    this->modeWidgetGroup->add(this->modeInputModeActionGroup);
    
    QWidget* w = this->createToolWidget("Mode", 
                                        widget, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_NONE,
                                        0);
    return w;
}

/**
 * Called when a tools input mode button is clicked.
 * @param action
 *    Action of tool button that was clicked.
 */
void 
BrainBrowserWindowToolBar::modeInputModeActionTriggered(QAction* action)
{
    BrowserTabContent* tabContent = this->getTabContentFromSelectedTab();
    if (tabContent == NULL) {
        return;
    }

    UserInputModeAbstract::UserInputMode inputMode = UserInputModeAbstract::INVALID;
    
    if (action == this->modeInputModeAnnotationsAction) {
        inputMode = UserInputModeAbstract::ANNOTATIONS;
    }
    else if (action == this->modeInputModeBordersAction) {
        inputMode = UserInputModeAbstract::BORDERS;
        
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
            this->updateUserInterface();
            this->updateGraphicsWindow();
        }
    }
    else if (action == this->modeInputModeFociAction) {
        inputMode = UserInputModeAbstract::FOCI;
    }
    else if (action == this->modeInputModeImageAction) {
        inputMode = UserInputModeAbstract::IMAGE;
    }
    else if (action == this->modeInputVolumeEditAction) {
        inputMode = UserInputModeAbstract::VOLUME_EDIT;
    }
    else if (action == this->modeInputModeViewAction) {
        inputMode = UserInputModeAbstract::VIEW;
    }
    else {
        CaretAssertMessage(0, "Tools input mode action is invalid, new action added???");
    }
    
    EventManager::get()->sendEvent(EventGetOrSetUserInputModeProcessor(this->browserWindowIndex,
                                                                       inputMode).getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
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
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->modeWidgetGroup->blockAllSignals(true);
    
    EventGetOrSetUserInputModeProcessor getInputModeEvent(this->browserWindowIndex);
    EventManager::get()->sendEvent(getInputModeEvent.getPointer());

    switch (getInputModeEvent.getUserInputMode()) {
        case UserInputModeAbstract::INVALID:
            /* may get here when program is exiting and widgets are being destroyed */
            break;
        case UserInputModeAbstract::ANNOTATIONS:
            this->modeInputModeAnnotationsAction->setChecked(true);
            break;
        case UserInputModeAbstract::BORDERS:
            this->modeInputModeBordersAction->setChecked(true);
            break;
        case UserInputModeAbstract::FOCI:
            this->modeInputModeFociAction->setChecked(true);
            break;
        case UserInputModeAbstract::IMAGE:
            this->modeInputModeImageAction->setChecked(true);
            break;
        case UserInputModeAbstract::VOLUME_EDIT:
            this->modeInputVolumeEditAction->setChecked(true);
            break;
        case UserInputModeAbstract::VIEW:
            this->modeInputModeViewAction->setChecked(true);
            break;
    }
    
    this->modeWidgetGroup->blockAllSignals(false);

    this->updateDisplayedModeUserInputWidget();
    
    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

void
BrainBrowserWindowToolBar::updateDisplayedModeUserInputWidget()
{
    EventGetOrSetUserInputModeProcessor getInputModeEvent(this->browserWindowIndex);
    EventManager::get()->sendEvent(getInputModeEvent.getPointer());
    
    UserInputModeAbstract* userInputProcessor = getInputModeEvent.getUserInputProcessor();
    QWidget* userInputWidget = userInputProcessor->getWidgetForToolBar();
    
    /*
     * If a widget is display and needs to change,
     * remove the old widget.
     */
    if (this->userInputControlsWidgetActiveInputWidget != NULL) {
        if (userInputWidget != this->userInputControlsWidgetActiveInputWidget) {
            /*
             * Remove the current input widget:
             * (1) Set its visibility to false
             * (2) Remove the widget from the toolbar's layout
             * (3) Set its parent to NULL.
             *
             * Why is the parent set to NULL?
             * 
             * When a widget is put into a layout, the widget is put into
             * a QWidgetItem (subclass of QLayoutItem).  
             *
             * QLayout::removeWidget() will delete the QWidgetItem but
             * it does not reset the parent for the widget that was in
             * QWidgetItem.  So the user will need to delete the widget
             * unless it is placed into a layout.
             *
             * After removing the widget, set the widget's parent to NULL.
             * As a result, when the input receiver (owner of the widget) 
             * is deleted, it can examine the parent, and, if the parent
             * is NULL, it can delete the widget preventing a memory link
             * and a possible crash.
             */
            this->userInputControlsWidgetActiveInputWidget->setVisible(false);
            this->userInputControlsWidgetLayout->removeWidget(this->userInputControlsWidgetActiveInputWidget);
            this->userInputControlsWidgetActiveInputWidget->setParent(NULL);
            this->userInputControlsWidgetActiveInputWidget = NULL;
        }
    }
    if (this->userInputControlsWidgetActiveInputWidget == NULL) {
        if (userInputWidget != NULL) {
            this->userInputControlsWidgetActiveInputWidget = userInputWidget;
            this->userInputControlsWidgetActiveInputWidget->setVisible(true);
            this->userInputControlsWidgetLayout->addWidget(this->userInputControlsWidgetActiveInputWidget);
            this->userInputControlsWidget->setVisible(true);
            this->userInputControlsWidgetLayout->update();
        }
        else {
            this->userInputControlsWidget->setVisible(false);
        }
    }
    
    if (userInputProcessor->getUserInputMode() != UserInputModeAbstract::ANNOTATIONS) {
        /*
         * Delete all selected annotations and update graphics and UI.
         */
        AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
        annotationManager->deselectAllAnnotationsForEditing(this->browserWindowIndex);
    }
}

/**
 * Create the tab options widget.
 *
 * @param windowAspectRatioLockedAction
 *    Action for locking the window's aspect ratio.
 * @param tabAspectRatioLockedAction
 *    Action for locking the tab's aspect ratio.
 *
 * @return  The tab options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createTabOptionsWidget(QAction* windowAspectRatioLockedAction,
                                                  QAction* tabAspectRatioLockedAction)
{
    m_tabOptionsComponent = new BrainBrowserWindowToolBarTab(this->browserWindowIndex,
                                                             windowAspectRatioLockedAction,
                                                             tabAspectRatioLockedAction,
                                                             this);
    
    QWidget* w = this->createToolWidget("Tab",
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
    if (this->windowWidget->isHidden()) {
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
    m_chartTypeToolBarComponent = new BrainBrowserWindowToolBarChartType(this);
    QWidget* w = this->createToolWidget("Chart Type",
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
 * Create the chart axes widget.
 *
 * @return
 *    Widget containing the chart axes.
 */
QWidget*
BrainBrowserWindowToolBar::createChartAxesWidget()
{
    m_chartAxisToolBarComponent = new BrainBrowserWindowToolBarChartAxes(this);
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
    m_chartAttributesToolBarComponent = new BrainBrowserWindowToolBarChartAttributes(this);
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
 * Create the single surface options widget.
 *
 * @return  The single surface options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createSingleSurfaceOptionsWidget()
{
    QLabel* structureSurfaceLabel = new QLabel("Brain Structure and Surface: ");
    
    this->surfaceSurfaceSelectionControl = new StructureSurfaceSelectionControl(false);
    QObject::connect(this->surfaceSurfaceSelectionControl, 
                     SIGNAL(selectionChanged(const StructureEnum::Enum,
                                             ModelSurface*)),
                     this,
                     SLOT(surfaceSelectionControlChanged(const StructureEnum::Enum,
                                                         ModelSurface*)));
    
    this->surfaceSurfaceSelectionControl->setMinimumWidth(150);
    this->surfaceSurfaceSelectionControl->setMaximumWidth(1200);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(structureSurfaceLabel);
    layout->addWidget(this->surfaceSurfaceSelectionControl);
    layout->addStretch();
    
    this->singleSurfaceSelectionWidgetGroup = new WuQWidgetObjectGroup(this);
    this->singleSurfaceSelectionWidgetGroup->add(this->surfaceSurfaceSelectionControl);

    QWidget* w = this->createToolWidget("Selection", 
                                        widget, 
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
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->singleSurfaceSelectionWidgetGroup->blockAllSignals(true);
    
    this->surfaceSurfaceSelectionControl->updateControl(browserTabContent->getSurfaceModelSelector());
    
    this->singleSurfaceSelectionWidgetGroup->blockAllSignals(false);
    
    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

/**
 * @return Create and return the surface montage options widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createSurfaceMontageOptionsWidget()
{
    m_surfaceMontageToolBarComponent = new BrainBrowserWindowToolBarSurfaceMontage(this);
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
 * @return Create and return the clipping options component.
 */
QWidget*
BrainBrowserWindowToolBar::createClippingOptionsWidget()
{
    m_clippingToolBarComponent = new BrainBrowserWindowToolBarClipping(this->browserWindowIndex,
                                                                       this);
    QWidget* w = this->createToolWidget("Clipping",
                                        m_clippingToolBarComponent,
                                        WIDGET_PLACEMENT_LEFT,
                                        WIDGET_PLACEMENT_TOP,
                                        100);
    w->setVisible(false);
    return w;
}

/**
 * Update the clipping options widget.
 *
 * @param browserTabContent
 *   The active browser tab.
 */
void
BrainBrowserWindowToolBar::updateClippingOptionsWidget(BrowserTabContent* browserTabContent)
{
    if (m_clippingOptionsWidget->isHidden()) {
        return;
    }
    
    m_clippingToolBarComponent->updateContent(browserTabContent);
}

/**
 * Create the volume montage widget.
 *
 * @return The volume montage widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumeMontageWidget()
{
    m_volumeMontageComponent = new BrainBrowserWindowToolBarVolumeMontage(this);
    
    
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

    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    m_volumeMontageComponent->updateContent(browserTabContent);

    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

/**
 * Create the volume plane widget.
 *
 * @return The volume plane widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumePlaneWidget()
{
    m_slicePlaneComponent = new BrainBrowserWindowToolBarSlicePlane(this);
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
 *    Name for the descriptive labe.
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
    QLabel* nameLabel = new QLabel("<html>" + name + "</html>");
    
    QWidget* w = new QWidget();
    QGridLayout* layout = new QGridLayout(w);
    layout->setColumnStretch(0, 100);
    layout->setColumnStretch(1, 100);    
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    switch (contentPlacement) {
        case WIDGET_PLACEMENT_BOTTOM:
            layout->setRowStretch(0, 100);
            layout->setRowStretch(1, 0);
            layout->addWidget(childWidget, 1, 0, 1, 2);
            break;
        case WIDGET_PLACEMENT_TOP:
            layout->setRowStretch(1, 100);
            layout->setRowStretch(0, 0);
            layout->addWidget(childWidget, 0, 0, 1, 2);
            break;
        case WIDGET_PLACEMENT_NONE:
            layout->setRowStretch(0, 0);
            layout->addWidget(childWidget, 0, 0, 1, 2);
            break;
        default:
            CaretAssert(0);
    }
    layout->addWidget(nameLabel, 2, 0, 1, 2, Qt::AlignHCenter);
    
    const bool addVerticalBarOnLeftSide = (verticalBarPlacement == WIDGET_PLACEMENT_LEFT);
    const bool addVerticalBarOnRightSide = (verticalBarPlacement == WIDGET_PLACEMENT_RIGHT);
    
    if (addVerticalBarOnLeftSide
        || addVerticalBarOnRightSide) {
        QWidget* w2 = new QWidget();
        QHBoxLayout* horizLayout = new QHBoxLayout(w2);
        WuQtUtilities::setLayoutSpacingAndMargins(horizLayout, 0, 0);
        if (addVerticalBarOnLeftSide) {
            horizLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0);
            horizLayout->addSpacing(3);
        }
        const int widgetStretchFactor = 100;
        horizLayout->addWidget(w, widgetStretchFactor);
        if (addVerticalBarOnRightSide) {
            horizLayout->addSpacing(3);
            horizLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0);
        }
        w = w2;
    }
 
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
 * Called when a view mode is selected.
 */
void 
BrainBrowserWindowToolBar::viewModeRadioButtonClicked(QAbstractButton*)
{
    CaretLogEntering();
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    if (btc == NULL) {
        return;
    }
    
    if (this->viewModeChartRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_CHART);
    }
    else if (this->viewModeSurfaceRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_SURFACE);
    }
    else if (this->viewModeSurfaceMontageRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE);
    }
    else if (this->viewModeVolumeRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES);
    }
    else if (this->viewModeWholeBrainRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN);
    }
    else {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_INVALID);
    }
    
    this->checkUpdateCounter();
    this->updateToolBar();
    this->updateTabName(-1);
    this->updateToolBox();
    emit viewedModelChanged();
    this->updateGraphicsWindow();
}

/**
 * Called when orientation left or lateral button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationLeftOrLateralToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
        btc->leftView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation right or medial button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationRightOrMedialToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
        btc->rightView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation anterior button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationAnteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->anteriorView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation posterior button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationPosteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->posteriorView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation dorsal button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationDorsalToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->dorsalView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation ventral button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationVentralToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->ventralView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation reset button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationResetToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->resetView();
    
    Model* mdc = btc->getModelForDisplay();
    if (mdc != NULL) {
        this->updateVolumeIndicesWidget(btc);
        this->updateGraphicsWindow();
        this->updateOtherYokedWindows();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation lateral/medial button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationLateralMedialToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->leftView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation dorsal/ventral button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationDorsalVentralToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->dorsalView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation anterior/posterior button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationAnteriorPosteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->anteriorView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    
    this->checkUpdateCounter();
}

/**
 * Called when custom view is triggered and displays Custom View Menu.
 */
void
BrainBrowserWindowToolBar::customViewActionTriggered()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->readCustomViews();
    const std::vector<std::pair<AString, AString> > customViewNameAndComments = prefs->getCustomViewNamesAndComments();
    
    QMenu menu;
    
    QAction* editAction = menu.addAction("Create and Edit...");
    editAction->setToolTip("Add and delete Custom Views.\n"
                           "Edit model transformations.");
    
    const int32_t numViews = static_cast<int32_t>(customViewNameAndComments.size());
    if (numViews > 0) {
        menu.addSeparator();
    }
    for (int32_t i = 0; i < numViews; i++) {
        QAction* action = menu.addAction(customViewNameAndComments[i].first);
        action->setToolTip(WuQtUtilities::createWordWrappedToolTipText(customViewNameAndComments[i].second));
    }
    
    QAction* selectedAction = menu.exec(QCursor::pos());
    if (selectedAction != NULL) {
        if (selectedAction == editAction) {
            BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
            GuiManager::get()->processShowCustomViewDialog(bbw);
        }
        else {
            const AString customViewName = selectedAction->text();
            
            ModelTransform modelTransform;
            if (prefs->getCustomView(customViewName, modelTransform)) {
                BrowserTabContent* btc = this->getTabContentFromSelectedTab();
                btc->setTransformationsFromModelTransform(modelTransform);
                this->updateGraphicsWindow();
                this->updateOtherYokedWindows();
            }
        }
    }
}

/**
 * Called when the whole brain surface type combo box is changed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceTypeComboBoxIndexChanged(int /*indx*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }

    int32_t comboBoxIndex = this->wholeBrainSurfaceTypeComboBox->currentIndex();
    if (comboBoxIndex >= 0) {
        const int32_t integerCode = this->wholeBrainSurfaceTypeComboBox->itemData(comboBoxIndex).toInt();
        bool isValid = false;
        const SurfaceTypeEnum::Enum surfaceType = SurfaceTypeEnum::fromIntegerCode(integerCode, &isValid);
        if (isValid) {
            wholeBrainModel->setSelectedSurfaceType(tabIndex, surfaceType);
            this->updateVolumeIndicesWidget(btc); /* slices may get deselected */
            this->updateGraphicsWindow();
            this->updateOtherYokedWindows();
        }
    }
}

/**
 * Called when whole brain surface left check box is toggled.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceLeftCheckBoxStateChanged(int /*state*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainLeftEnabled(this->wholeBrainSurfaceLeftCheckBox->isChecked());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when the left surface tool button is pressed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceLeftToolButtonTriggered(bool /*checked*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    const int32_t tabIndex = btc->getTabNumber();
    
    Brain* brain = GuiManager::get()->getBrain();
    BrainStructure* brainStructure = brain->getBrainStructure(StructureEnum::CORTEX_LEFT, false);
    if (brainStructure != NULL) {
        std::vector<Surface*> surfaces;
        brainStructure->getSurfacesOfType(wholeBrainModel->getSelectedSurfaceType(tabIndex),
                                          surfaces);
        
        const int32_t numSurfaces = static_cast<int32_t>(surfaces.size());
        if (numSurfaces > 0) {
            Surface* selectedSurface = wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_LEFT,
                                                                                tabIndex);
            QMenu menu;
            QActionGroup* actionGroup = new QActionGroup(&menu);
            actionGroup->setExclusive(true);
            for (int32_t i = 0; i < numSurfaces; i++) {
                QString name = surfaces[i]->getFileNameNoPath();
                QAction* action = actionGroup->addAction(name);
                action->setCheckable(true);
                if (surfaces[i] == selectedSurface) {
                    action->setChecked(true);
                }
                menu.addAction(action);
            }
            QAction* result = menu.exec(QCursor::pos());
            if (result != NULL) {
                QList<QAction*> actionList = actionGroup->actions();
                for (int32_t i = 0; i < numSurfaces; i++) {
                    if (result == actionList.at(i)) {
                        wholeBrainModel->setSelectedSurface(StructureEnum::CORTEX_LEFT,
                                                                 tabIndex, 
                                                                 surfaces[i]);
                        this->updateGraphicsWindow();
                        this->updateOtherYokedWindows();
                        break;
                    }
                }
            }
        }
    }
}

/** 
 * Called when the right surface tool button is pressed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceRightToolButtonTriggered(bool /*checked*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    const int32_t tabIndex = btc->getTabNumber();
    
    Brain* brain = GuiManager::get()->getBrain();
    BrainStructure* brainStructure = brain->getBrainStructure(StructureEnum::CORTEX_RIGHT, false);
    if (brainStructure != NULL) {
        std::vector<Surface*> surfaces;
        brainStructure->getSurfacesOfType(wholeBrainModel->getSelectedSurfaceType(tabIndex),
                                          surfaces);
        
        const int32_t numSurfaces = static_cast<int32_t>(surfaces.size());
        if (numSurfaces > 0) {
            Surface* selectedSurface = wholeBrainModel->getSelectedSurface(StructureEnum::CORTEX_RIGHT,
                                                                                tabIndex);
            QMenu menu;
            QActionGroup* actionGroup = new QActionGroup(&menu);
            actionGroup->setExclusive(true);
            for (int32_t i = 0; i < numSurfaces; i++) {
                QString name = surfaces[i]->getFileNameNoPath();
                QAction* action = actionGroup->addAction(name);
                action->setCheckable(true);
                if (surfaces[i] == selectedSurface) {
                    action->setChecked(true);
                }
                menu.addAction(action);
            }
            QAction* result = menu.exec(QCursor::pos());
            if (result != NULL) {
                QList<QAction*> actionList = actionGroup->actions();
                for (int32_t i = 0; i < numSurfaces; i++) {
                    if (result == actionList.at(i)) {
                        wholeBrainModel->setSelectedSurface(StructureEnum::CORTEX_RIGHT,
                                                                 tabIndex, 
                                                                 surfaces[i]);
                        this->updateGraphicsWindow();
                        this->updateOtherYokedWindows();
                        break;
                    }
                }
            }
        }
    }
}

/** 
 * Called when the cerebellum surface tool button is pressed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceCerebellumToolButtonTriggered(bool /*checked*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    const int32_t tabIndex = btc->getTabNumber();
    
    Brain* brain = GuiManager::get()->getBrain();
    BrainStructure* brainStructure = brain->getBrainStructure(StructureEnum::CEREBELLUM, false);
    if (brainStructure != NULL) {
        std::vector<Surface*> surfaces;
        brainStructure->getSurfacesOfType(wholeBrainModel->getSelectedSurfaceType(tabIndex),
                                          surfaces);
        
        const int32_t numSurfaces = static_cast<int32_t>(surfaces.size());
        if (numSurfaces > 0) {
            Surface* selectedSurface = wholeBrainModel->getSelectedSurface(StructureEnum::CEREBELLUM,
                                                                                tabIndex);
            QMenu menu;
            QActionGroup* actionGroup = new QActionGroup(&menu);
            actionGroup->setExclusive(true);
            for (int32_t i = 0; i < numSurfaces; i++) {
                QString name = surfaces[i]->getFileNameNoPath();
                QAction* action = actionGroup->addAction(name);
                action->setCheckable(true);
                if (surfaces[i] == selectedSurface) {
                    action->setChecked(true);
                }
                menu.addAction(action);
            }
            QAction* result = menu.exec(QCursor::pos());
            if (result != NULL) {
                QList<QAction*> actionList = actionGroup->actions();
                for (int32_t i = 0; i < numSurfaces; i++) {
                    if (result == actionList.at(i)) {
                        wholeBrainModel->setSelectedSurface(StructureEnum::CEREBELLUM,
                                                                 tabIndex, 
                                                                 surfaces[i]);
                        this->updateGraphicsWindow();
                        this->updateOtherYokedWindows();
                        break;
                    }
                }
            }
        }
    }
}

/**
 * Called when whole brain surface right checkbox is toggled.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceRightCheckBoxStateChanged(int /*state*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainRightEnabled(this->wholeBrainSurfaceRightCheckBox->isChecked());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when whole brain cerebellum check box is toggled.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceCerebellumCheckBoxStateChanged(int /*state*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainCerebellumEnabled(this->wholeBrainSurfaceCerebellumCheckBox->isChecked());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when whole brain separation left/right spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double /*d*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();

    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainLeftRightSeparation(this->wholeBrainSurfaceSeparationLeftRightSpinBox->value());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when whole brain left&right/cerebellum spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double /*d*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();

    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        return;
    }
    
    btc->setWholeBrainCerebellumSeparation(this->wholeBrainSurfaceSeparationCerebellumSpinBox->value());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when a single surface control is changed.
 * @param structure
 *      Structure that is selected.
 * @param surfaceModel
 *     Model that is selected.
 */
void 
BrainBrowserWindowToolBar::surfaceSelectionControlChanged(
                                    const StructureEnum::Enum structure,
                                    ModelSurface* surfaceModel)
{
    if (surfaceModel != NULL) {
        BrowserTabContent* btc = this->getTabContentFromSelectedTab();
        ModelSurfaceSelector* surfaceModelSelector = btc->getSurfaceModelSelector();
        surfaceModelSelector->setSelectedStructure(structure);
        surfaceModelSelector->setSelectedSurfaceModel(surfaceModel);
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        this->updateUserInterface();
        this->updateGraphicsWindow();
    }
    
    this->updateTabName(-1);
    
    this->checkUpdateCounter();    
}

void
BrainBrowserWindowToolBar::checkUpdateCounter()
{
    if (this->updateCounter != 0) {
        CaretLogWarning(AString("Update counter is non-zero, this indicates that signal needs to be blocked during update, value=")
                        + AString::number(updateCounter));
    }
}

void 
BrainBrowserWindowToolBar::incrementUpdateCounter(const char* /*methodName*/)
{
    this->updateCounter++;
}

void 
BrainBrowserWindowToolBar::decrementUpdateCounter(const char* /*methodName*/)
{
    this->updateCounter--;
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
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT_GET) {
        EventBrowserWindowContentGet* getModelEvent =
            dynamic_cast<EventBrowserWindowContentGet*>(event);
        CaretAssert(getModelEvent);
        
        if (getModelEvent->getBrowserWindowIndex() == this->browserWindowIndex) {
            BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->browserWindowIndex);
            if (browserWindow != NULL) {
                if (browserWindow->isTileTabsSelected()) {
                    const int32_t numTabs = this->tabBar->count();
                    for (int32_t i = 0; i < numTabs; i++) {
                        BrowserTabContent* btc = this->getTabContentFromTab(i);
                        getModelEvent->addTabContentToDraw(btc);
                    }
                    
                    /*
                     * Tab that is highlighted so user knows which tab
                     * any changes in toolbar/toolboxes apply to.
                     */
                    getModelEvent->setTabIndexForTileTabsHighlighting(m_tabIndexForTileTabsHighlighting);
                    
                    getModelEvent->setTileTabsConfiguration(browserWindow->getSelectedTileTabsConfiguration());
                }
                else {
                    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
                    getModelEvent->addTabContentToDraw(btc);
                }
                
                getModelEvent->setSelectedBrowserTabContent(this->getTabContentFromSelectedTab());
            }
            getModelEvent->setEventProcessed();
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
                        createNewTab(errorMessage);
                        if (errorMessage.isEmpty() == false) {
                            CaretLogSevere(errorMessage);
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
    else if (event->getEventType() == EventTypeEnum::EVENT_UPDATE_YOKED_WINDOWS) {
        EventUpdateYokedWindows* yokeUpdateEvent =
            dynamic_cast<EventUpdateYokedWindows*>(event);
        CaretAssert(yokeUpdateEvent);
        
        BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
        if (browserTabContent != NULL) {
            if (this->browserWindowIndex != yokeUpdateEvent->getBrowserWindowIndexThatIssuedEvent()) {
                if (browserTabContent->getYokingGroup() == yokeUpdateEvent->getYokingGroup()) {
                    this->updateToolBar();
                    this->updateGraphicsWindow();
                }
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET_ALL_VIEWED) {
        EventBrowserTabGetAllViewed* viewedTabsEvent = dynamic_cast<EventBrowserTabGetAllViewed*>(event);
        CaretAssert(viewedTabsEvent);
        
        BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->browserWindowIndex);
        if (browserWindow != NULL) {
            if (browserWindow->isTileTabsSelected()) {
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
    else {
        
    }
    
//    CaretLogFinest("Toolbar width/height: "
//                   + AString::number(width())
//                   + "/"
//                   + AString::number(height()));
}

/**
 * If this windows is yoked, issue an event to update other
 * windows that are using the same yoking.
 */
void
BrainBrowserWindowToolBar::updateOtherYokedWindows()
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        if (browserTabContent->isYoked()) {
            EventManager::get()->sendEvent(EventUpdateYokedWindows(this->browserWindowIndex,
                                                                   browserTabContent->getYokingGroup()).getPointer());
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
        this->tabClosed(iClose);
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
                this->insertTabAtIndex(tabContent,
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

