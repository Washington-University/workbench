
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

#include <QActionGroup>
#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCleanlooksStyle>
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
#include <QTabBar>
#include <QToolButton>

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretFunctionName.h"
#include "CaretLogger.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabNew.h"
#include "EventBrowserWindowContentGet.h"
#include "EventBrowserWindowNew.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "EventModelDisplayControllerGetAll.h"
#include "GuiManager.h"
#include "ModelDisplayController.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerSurfaceSelector.h"
#include "ModelDisplayControllerVolume.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "Surface.h"
#include "StructureSurfaceSelectionControl.h"
#include "VolumeFile.h"
#include "VolumeSliceViewModeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"
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
 * @param toolBoxToolButtonAction
 *    Action for the Toolbox button in this toolbar.
 * @param parent
 *    Parent for this toolbar.
 */
BrainBrowserWindowToolBar::BrainBrowserWindowToolBar(const int32_t browserWindowIndex,
                                                     BrowserTabContent* initialBrowserTabContent,
                                                     QAction* toolBoxToolButtonAction,
                                                     QWidget* parent)
: QToolBar(parent)
{
    this->browserWindowIndex = browserWindowIndex;
    this->toolsToolBoxToolButtonAction = toolBoxToolButtonAction;
    this->updateCounter = 0;
    
    this->isContructorFinished = false;

/*
    qApp->setStyleSheet("QTabBar::tab:selected {"
                        "    font: bold 14px"
                        "}  " 
                        "QTabBar::tab!selected {"
                        "    font: italic 10px"
                        "}");
*/
    /*
     * Create tab bar that displays models.
     */
    this->tabBar = new QTabBar();
    this->tabBar->setStyleSheet("QTabBar::tab:selected {"
                                "    font: bold 14px;"
                                //"    text-decoration: underline; "
                                "}  " 
                                //"QTabBar::tab:!selected {"
                                "QTabBar::tab {"
                                "    font: italic"
                                "}");
    /*
                                "QTabBar::tab:close-button {"
                                "image: url(close.png)"
                                "    subcontrol-position: left;"
                                "}");
     */

    //this->tabBar->setDocumentMode(true);
    this->tabBar->setShape(QTabBar::RoundedNorth);
#ifdef Q_OS_MACX
    this->tabBar->setStyle(new QCleanlooksStyle());
#endif // Q_OS_MACX
    QObject::connect(this->tabBar, SIGNAL(currentChanged(int)),
                     this, SLOT(selectedTabChanged(int)));
    QObject::connect(this->tabBar, SIGNAL(tabCloseRequested(int)),
                     this, SLOT(tabClosed(int)));
    
    /*
     * Create the toolbar's widgets.
     */
    this->viewWidget = this->createViewWidget();
    this->orientationWidget = this->createOrientationWidget();
    this->wholeBrainSurfaceOptionsWidget = this->createWholeBrainSurfaceOptionsWidget();
    this->volumeIndicesWidget = this->createVolumeIndicesWidget();
    this->toolsWidget = this->createToolsWidget();
    this->windowWidget = this->createWindowWidget();
    this->singleSurfaceSelectionWidget = this->createSingleSurfaceOptionsWidget();
    this->volumeMontageWidget = this->createVolumeMontageWidget();
    this->volumePlaneWidget = this->createVolumePlaneWidget();
    //this->spacerWidget = new QWidget;
    
    /*
     * Layout the toolbar's widgets.
     */
    this->toolbarWidget = new QWidget();
    this->toolbarWidgetLayout = new QHBoxLayout(this->toolbarWidget);
    this->toolbarWidgetLayout->setSpacing(2);
    
    this->toolbarWidgetLayout->addWidget(this->viewWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->orientationWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->wholeBrainSurfaceOptionsWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->singleSurfaceSelectionWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumePlaneWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeIndicesWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->volumeMontageWidget, 0, Qt::AlignLeft);
    
    //this->toolbarWidgetLayout->addWidget(this->spacerWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->toolsWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addWidget(this->windowWidget, 0, Qt::AlignLeft);
    
    this->toolbarWidgetLayout->addStretch();

    /*
     * Arrange the tabbar and the toolbar vertically.
     */
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutMargins(layout, 1, 2, 0);
    layout->addWidget(this->tabBar);
    layout->addWidget(this->toolbarWidget);
    
    this->addWidget(w);
    
    if (initialBrowserTabContent != NULL) {
        this->addNewTab(initialBrowserTabContent);
    }
    else {
        this->addNewTab();
    }
    
    //this->updateViewWidget(NULL);
    this->updateToolBar();
    
    this->isContructorFinished = true;
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_CONTENT_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBar::~BrainBrowserWindowToolBar()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int i = (this->tabBar->count() - 1); i >= 0; i--) {
        this->tabClosed(i);
    }
}

/**
 * Add a new tab.
 */
void 
BrainBrowserWindowToolBar::addNewTab()
{
    EventBrowserTabNew newTabEvent;
    EventManager::get()->sendEvent(newTabEvent.getPointer());
    
    if (newTabEvent.isError()) {
        QMessageBox::critical(this, "", newTabEvent.getErrorMessage());
        return;
    }
    
    BrowserTabContent* tabContent = newTabEvent.getBrowserTab();
    
    this->addNewTab(tabContent);

    
/*
    const int newTabIndex = this->tabBar->addTab("NewTab");
    this->tabBar->setTabData(newTabIndex, qVariantFromValue((void*)tabContent));
    
    const int numOpenTabs = this->tabBar->count();
    this->tabBar->setTabsClosable(numOpenTabs > 1);
    
    this->tabBar->setTabText(newTabIndex, tabContent->getName());
    
    this->tabBar->blockSignals(false);
    
    this->tabBar->setCurrentIndex(newTabIndex);
*/
}

/**
 * Add a new tab containing the given content.
 * @param tabContent
 *    Content for new tab.
 */
void 
BrainBrowserWindowToolBar::addNewTab(BrowserTabContent* tabContent)
{
    CaretAssert(tabContent);
    
    this->tabBar->blockSignals(true);
    
    const int newTabIndex = this->tabBar->addTab("NewTab");
    this->tabBar->setTabData(newTabIndex, qVariantFromValue((void*)tabContent));
    
    const int numOpenTabs = this->tabBar->count();
    this->tabBar->setTabsClosable(numOpenTabs > 1);
    
    //this->tabBar->setTabText(newTabIndex, tabContent->getName());
    this->updateTabName(newTabIndex);
    
    this->tabBar->setCurrentIndex(newTabIndex);
    
    this->tabBar->blockSignals(false);
    
    if (this->isContructorFinished) {
        this->updateUserInterface();
        this->updateGraphicsWindow();
    }
}

/**
 * Shows/hides the toolbar.
 */
void 
BrainBrowserWindowToolBar::showHideToolBar(bool showIt)
{
    this->toolbarWidget->setVisible(showIt);
}


/**
 * Add the default tabs after loading a spec file.
 */
void 
BrainBrowserWindowToolBar::addDefaultTabsAfterLoadingSpecFile()
{
    EventModelDisplayControllerGetAll eventAllControllers;
    EventManager::get()->sendEvent(eventAllControllers.getPointer());
    
    const std::vector<ModelDisplayController*> allControllers =
       eventAllControllers.getModelDisplayControllers();

    ModelDisplayControllerSurface* leftSurfaceController = NULL;
    int32_t leftSurfaceTypeCode = 1000000;
    
    ModelDisplayControllerSurface* rightSurfaceController = NULL;
    int32_t rightSurfaceTypeCode = 1000000;

    ModelDisplayControllerSurface* cerebellumSurfaceController = NULL;
    int32_t cerebellumSurfaceTypeCode = 1000000;
    
    ModelDisplayControllerVolume* volumeController = NULL;
    ModelDisplayControllerWholeBrain* wholeBrainController = NULL;
    
    for (std::vector<ModelDisplayController*>::const_iterator iter = allControllers.begin();
         iter != allControllers.end();
         iter++) {
        ModelDisplayControllerSurface* surfaceController =
            dynamic_cast<ModelDisplayControllerSurface*>(*iter);
        if (surfaceController != NULL) {
            Surface* surface = surfaceController->getSurface();
            StructureEnum::Enum structure = surface->getStructure();
            SurfaceTypeEnum::Enum surfaceType = surface->getSurfaceType();
            const int32_t surfaceTypeCode = SurfaceTypeEnum::toIntegerCode(surfaceType);
            
            switch (structure) {
                case StructureEnum::CEREBELLUM:
                    if (surfaceTypeCode < cerebellumSurfaceTypeCode) {
                        cerebellumSurfaceController = surfaceController;
                        cerebellumSurfaceTypeCode = surfaceTypeCode;
                    }
                    break;
                case StructureEnum::CORTEX_LEFT:
                    if (surfaceTypeCode < leftSurfaceTypeCode) {
                        leftSurfaceController = surfaceController;
                        leftSurfaceTypeCode = surfaceTypeCode;
                    }
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    if (surfaceTypeCode < rightSurfaceTypeCode) {
                        rightSurfaceController = surfaceController;
                        rightSurfaceTypeCode = surfaceTypeCode;
                    }
                    break;
                default:
                    break;
            }
        }
        else if (dynamic_cast<ModelDisplayControllerVolume*>(*iter) != NULL) {
            volumeController = dynamic_cast<ModelDisplayControllerVolume*>(*iter);
        }
        else if (dynamic_cast<ModelDisplayControllerWholeBrain*>(*iter) != NULL) {
            wholeBrainController = dynamic_cast<ModelDisplayControllerWholeBrain*>(*iter);
        }
        else {
            CaretAssertMessage(0, AString("Unknow controller type: ") + (*iter)->getNameForGUI(true));
        }
    }
    
    int32_t tabIndex = 0;
    
    tabIndex = loadIntoTab(tabIndex,
                           leftSurfaceController);
    tabIndex = loadIntoTab(tabIndex,
                           rightSurfaceController);
    tabIndex = loadIntoTab(tabIndex,
                           cerebellumSurfaceController);
    tabIndex = loadIntoTab(tabIndex,
                           volumeController);
    tabIndex = loadIntoTab(tabIndex,
                           wholeBrainController);
    
    if (this->tabBar->count() > 0) {
        this->tabBar->setCurrentIndex(0);
    }
}

/**
 * Load a controller into the tab with the given index.
 * @param tabIndexIn
 *   Index of tab into which controller is loaded.  A
 *   new tab will be created, if needed.
 * @param controller
 *   Controller that is to be displayed in the tab.  If
 *   NULL, this method does nothing.
 * @return
 *   Index of next tab after controller is displayed.
 *   If the input controller was NULL, the returned 
 *   value is identical to the input tab index.
 */
int32_t 
BrainBrowserWindowToolBar::loadIntoTab(const int32_t tabIndexIn,
                                       ModelDisplayController* controller)
{
    int32_t tabIndex = tabIndexIn;
    
    if (controller != NULL) {
        if (tabIndex >= this->tabBar->count()) {
            this->addNewTab();
            tabIndex = this->tabBar->count() - 1;
        }
        void* p = this->tabBar->tabData(tabIndex).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        btc->setSelectedModelType(controller->getControllerType());
        
        ModelDisplayControllerSurface* surfaceController =
        dynamic_cast<ModelDisplayControllerSurface*>(controller);
        if (surfaceController != NULL) {
            btc->getSurfaceModelSelector()->setSelectedStructure(surfaceController->getSurface()->getStructure());
            btc->getSurfaceModelSelector()->setSelectedSurfaceController(surfaceController);
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
    
    this->updateUserInterface();
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
    BrowserTabContent* btc = (BrowserTabContent*)p;    
    this->tabBar->setTabText(tabIndexForUpdate, btc->getName());

    /*
    QIcon coronalIcon;
    const bool coronalIconValid =
    WuQtUtilities::loadIcon(":/view-plane-coronal.png", 
                            coronalIcon);
    if (coronalIconValid) {
        this->tabBar->setTabIcon(tabIndex, coronalIcon);
    }
    */
}

/**
 * Close the selected tab.  This method is typically
 * called by the BrowswerWindow's File Menu.
 */
void 
BrainBrowserWindowToolBar::closeSelectedTab()
{
    const int tabIndex = this->tabBar->currentIndex();
    if (this->tabBar->count() > 1) {
        this->tabClosed(tabIndex);
        //this->tabBar->removeTab(tabIndex);
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
    this->updateGraphicsWindow();
    this->updateUserInterface();
    this->updateGraphicsWindow(); // yes, do a second time
}

void 
BrainBrowserWindowToolBar::tabClosed(int indx)
{
    CaretAssertArrayIndex(this-tabBar->tabData(), this->tabBar->count(), indx);
    
    void* p = this->tabBar->tabData(indx).value<void*>();
    if (p != NULL) {
        BrowserTabContent* btc = (BrowserTabContent*)p;
    
        EventBrowserTabDelete deleteTabEvent(btc);
        EventManager::get()->sendEvent(deleteTabEvent.getPointer());
    }
    
    this->tabBar->blockSignals(true);
    this->tabBar->removeTab(indx);
    this->tabBar->blockSignals(false);
    
    const int numOpenTabs = this->tabBar->count();
    this->tabBar->setTabsClosable(numOpenTabs > 2);
}


/**
 * Update the toolbar.
 */
void 
BrainBrowserWindowToolBar::updateToolBar()
{
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    
    const ModelDisplayControllerTypeEnum::Enum viewModel = this->updateViewWidget(browserTabContent);
    
    bool showOrientationWidget = false;
    bool showWholeBrainSurfaceOptionsWidget = false;
    bool showSingleSurfaceOptionsWidget = false;
    bool showVolumeIndicesWidget = false;
    bool showVolumePlaneWidget = false;
    bool showVolumeMontageWidget = false;
    
    bool showToolsWidget = true;
    bool showWindowWidget = true;
    
    int spacerWidgetStretchFactor = 0;
    int singleSurfaceWidgetStretchFactor = 0;
    
    switch (viewModel) {
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE:
            showOrientationWidget = true;
            showSingleSurfaceOptionsWidget = true;
            singleSurfaceWidgetStretchFactor = 100;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            showVolumeIndicesWidget = true;
            showVolumePlaneWidget = true;
            showVolumeMontageWidget = true;
            spacerWidgetStretchFactor = 100;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            showOrientationWidget = true;
            showWholeBrainSurfaceOptionsWidget = true;
            showVolumeIndicesWidget = true;
            spacerWidgetStretchFactor = 100;
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
    this->volumeIndicesWidget->setVisible(false);
    this->volumePlaneWidget->setVisible(false);
    this->volumeMontageWidget->setVisible(false);
    this->toolsWidget->setVisible(false);
    this->windowWidget->setVisible(false);
    
    //this->toolbarWidgetLayout->setStretchFactor(this->singleSurfaceSelectionWidget, 
    //                                          singleSurfaceWidgetStretchFactor);
    //this->toolbarWidgetLayout->setStretchFactor(this->spacerWidget, 
    //                                          spacerWidgetStretchFactor);
    
    this->orientationWidget->setVisible(showOrientationWidget);
    this->wholeBrainSurfaceOptionsWidget->setVisible(showWholeBrainSurfaceOptionsWidget);
    this->singleSurfaceSelectionWidget->setVisible(showSingleSurfaceOptionsWidget);
    this->singleSurfaceSelectionWidget->updateGeometry();
    this->volumeIndicesWidget->setVisible(showVolumeIndicesWidget);
    this->volumePlaneWidget->setVisible(showVolumePlaneWidget);
    this->volumeMontageWidget->setVisible(showVolumeMontageWidget);
    this->toolsWidget->setVisible(showToolsWidget);
    this->windowWidget->setVisible(showWindowWidget);

    this->updateOrientationWidget(browserTabContent);
    this->updateWholeBrainSurfaceOptionsWidget(browserTabContent);
    this->updateVolumeIndicesWidget(browserTabContent);
    this->updateSingleSurfaceOptionsWidget(browserTabContent);
    this->updateVolumeMontageWidget(browserTabContent);
    this->updateVolumePlaneWidget(browserTabContent);
    this->updateToolsWidget(browserTabContent);
    this->updateWindowWidget(browserTabContent);
    
    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    if (this->updateCounter != 0) {
        CaretLogSevere("Update counter is non-zero at end of updateToolBar()");
    }
    
}

/**
 * Get the action for showing the toolbox.
 */
QAction* 
BrainBrowserWindowToolBar::getShowToolBoxAction()
{
    return this->toolsToolBoxToolButtonAction;
}

/**
 * Create the view widget.
 *
 * @return The view widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createViewWidget()
{
    this->viewModeSurfaceRadioButton = new QRadioButton("Surface");
    this->viewModeVolumeRadioButton = new QRadioButton("Volume");
    this->viewModeWholeBrainRadioButton = new QRadioButton("Whole Brain");
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 2, 2);
    layout->addWidget(this->viewModeSurfaceRadioButton);
    layout->addWidget(this->viewModeVolumeRadioButton);
    layout->addWidget(this->viewModeWholeBrainRadioButton);
    layout->addStretch();

    QButtonGroup* viewModeRadioButtonGroup = new QButtonGroup(this);
    viewModeRadioButtonGroup->addButton(this->viewModeSurfaceRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeVolumeRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeWholeBrainRadioButton);
    QObject::connect(viewModeRadioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(viewModeRadioButtonClicked(QAbstractButton*)));
    
    this->viewWidgetGroup = new WuQWidgetObjectGroup(this);
    this->viewWidgetGroup->add(this->viewModeSurfaceRadioButton);
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
ModelDisplayControllerTypeEnum::Enum
BrainBrowserWindowToolBar::updateViewWidget(BrowserTabContent* browserTabContent)
{
    CaretAssert(browserTabContent);
    
    ModelDisplayControllerTypeEnum::Enum modelType = browserTabContent->getSelectedModelType();
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->viewWidgetGroup->blockSignals(true);
    
    /*
     * Enable buttons for valid types
     */
    this->viewModeSurfaceRadioButton->setEnabled(browserTabContent->isSurfaceModelValid());
    this->viewModeVolumeRadioButton->setEnabled(browserTabContent->isVolumeSliceModelValid());
    this->viewModeWholeBrainRadioButton->setEnabled(browserTabContent->isWholeBrainModelValid());
    
    switch (modelType) {
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE:
            this->viewModeSurfaceRadioButton->setChecked(true);
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            this->viewModeVolumeRadioButton->setChecked(true);
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            this->viewModeWholeBrainRadioButton->setChecked(true);
            break;
    }
    
    this->viewWidgetGroup->blockSignals(false);

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
    QIcon leftIcon;
    const bool leftIconValid =
        WuQtUtilities::loadIcon(":/view-left.png", 
                                leftIcon);
    
    QIcon rightIcon;
    const bool rightIconValid =
    WuQtUtilities::loadIcon(":/view-right.png", 
                            rightIcon);
    
    QIcon anteriorIcon;
    const bool anteriorIconValid =
    WuQtUtilities::loadIcon(":/view-anterior.png", 
                            anteriorIcon);
    
    QIcon posteriorIcon;
    const bool posteriorIconValid =
    WuQtUtilities::loadIcon(":/view-posterior.png", 
                            posteriorIcon);
    
    QIcon dorsalIcon;
    const bool dorsalIconValid =
    WuQtUtilities::loadIcon(":/view-dorsal.png", 
                            dorsalIcon);
    
    QIcon ventralIcon;
    const bool ventralIconValid =
    WuQtUtilities::loadIcon(":/view-ventral.png", 
                            ventralIcon);
    
    this->orientationLeftToolButtonAction = WuQtUtilities::createAction("L", 
                                                                        "View the brain from a LEFT perspective", 
                                                                        this, 
                                                                        this, 
                                                                        SLOT(orientationLeftToolButtonTriggered(bool)));
    if (leftIconValid) {
        this->orientationLeftToolButtonAction->setIcon(leftIcon);
    }
    
    this->orientationRightToolButtonAction = WuQtUtilities::createAction("R", 
                                                                         "View the brain from a RIGHT perspective", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(orientationRightToolButtonTriggered(bool)));
    if (rightIconValid) {
        this->orientationRightToolButtonAction->setIcon(rightIcon);
    }
    
    this->orientationAnteriorToolButtonAction = WuQtUtilities::createAction("A", 
                                                                            "View the brain from an ANTERIOR perspective", 
                                                                            this, 
                                                                            this, 
                                                                            SLOT(orientationAnteriorToolButtonTriggered(bool)));
    if (anteriorIconValid) {
        this->orientationAnteriorToolButtonAction->setIcon(anteriorIcon);
    }
    
    this->orientationPosteriorToolButtonAction = WuQtUtilities::createAction("P", 
                                                                             "View the brain from a POSTERIOR perspective", 
                                                                             this, 
                                                                             this, 
                                                                             SLOT(orientationPosteriorToolButtonTriggered(bool)));
    if (posteriorIconValid) {
        this->orientationPosteriorToolButtonAction->setIcon(posteriorIcon);
    }
    
    this->orientationDorsalToolButtonAction = WuQtUtilities::createAction("D", 
                                                                          "View the brain from a DORSAL perspective", 
                                                                          this, 
                                                                          this, 
                                                                          SLOT(orientationDorsalToolButtonTriggered(bool)));
    if (dorsalIconValid) {
        this->orientationDorsalToolButtonAction->setIcon(dorsalIcon);
    }
    
    this->orientationVentralToolButtonAction = WuQtUtilities::createAction("V", 
                                                                           "View the brain from a VENTRAL perspective", 
                                                                           this, 
                                                                           this, 
                                                                           SLOT(orientationVentralToolButtonTriggered(bool)));
    if (ventralIconValid) {
        this->orientationVentralToolButtonAction->setIcon(ventralIcon);
    }
    
    this->orientationResetToolButtonAction = WuQtUtilities::createAction("Reset", 
                                                                         "Reset the view to dorsal and remove any panning or zooming", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(orientationResetToolButtonTriggered(bool)));

    this->orientationUserViewOneToolButtonAction = WuQtUtilities::createAction("V1", 
                                                                                "Display the model using user view 1", 
                                                                                this, 
                                                                                this, 
                                                                                SLOT(orientationUserViewOneToolButtonTriggered(bool)));
    
    this->orientationUserViewTwoToolButtonAction = WuQtUtilities::createAction("V2", 
                                                                               "Display the model using user view 2", 
                                                                               this, 
                                                                               this, 
                                                                               SLOT(orientationUserViewTwoToolButtonTriggered(bool)));
    

    
    this->orientationUserViewSelectToolButtonMenu = new QMenu(this);
    QObject::connect(this->orientationUserViewSelectToolButtonMenu, SIGNAL(aboutToShow()),
                     this, SLOT(orientationUserViewSelectToolButtonMenuAboutToShow()));
    QObject::connect(this->orientationUserViewSelectToolButtonMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(orientationUserViewSelectToolButtonMenuTriggered(QAction*)));
    this->orientationUserViewSelectToolButtonMenu->addAction("View xx");
    this->orientationUserViewSelectToolButtonMenu->addAction("View yy");
    this->orientationUserViewSelectToolButtonMenu->addSeparator();
    this->orientationUserViewSelectToolButtonMenu->addAction("Set User View 1");
    this->orientationUserViewSelectToolButtonMenu->addAction("Set User View 2");
    this->orientationUserViewSelectToolButtonMenu->addSeparator();
    this->orientationUserViewSelectToolButtonMenu->addAction("Add View...");
    this->orientationUserViewSelectToolButtonMenu->addAction("Edit Views...");
    
    this->orientationUserViewSelectToolButtonAction = new QAction(this);
    this->orientationUserViewSelectToolButtonAction->setText("View");
    this->orientationUserViewSelectToolButtonAction->setMenu(this->orientationUserViewSelectToolButtonMenu);
    this->orientationUserViewSelectToolButtonAction->setToolTip("Add, edit, and select user-defined views");
    this->orientationUserViewSelectToolButtonAction->setStatusTip("Add, edit, and select user-defined views");


    QToolButton* orientationLeftToolButton = new QToolButton();
    orientationLeftToolButton->setDefaultAction(this->orientationLeftToolButtonAction);
    
    QToolButton* orientationRightToolButton = new QToolButton();
    orientationRightToolButton->setDefaultAction(this->orientationRightToolButtonAction);
    
    QToolButton* orientationAnteriorToolButton = new QToolButton();
    orientationAnteriorToolButton->setDefaultAction(this->orientationAnteriorToolButtonAction);
    
    QToolButton* orientationPosteriorToolButton = new QToolButton();
    orientationPosteriorToolButton->setDefaultAction(this->orientationPosteriorToolButtonAction);
    
    QToolButton* orientationDorsalToolButton = new QToolButton();
    orientationDorsalToolButton->setDefaultAction(this->orientationDorsalToolButtonAction);
    
    QToolButton* orientationVentralToolButton = new QToolButton();
    orientationVentralToolButton->setDefaultAction(this->orientationVentralToolButtonAction);
    
    QToolButton* orientationResetToolButton = new QToolButton();
    orientationResetToolButton->setDefaultAction(this->orientationResetToolButtonAction);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(buttonLayout, 0, 2, 0);
    buttonLayout->addWidget(orientationLeftToolButton);
    buttonLayout->addWidget(orientationRightToolButton);
    buttonLayout->addWidget(orientationAnteriorToolButton);
    buttonLayout->addWidget(orientationPosteriorToolButton);
    buttonLayout->addWidget(orientationDorsalToolButton);
    buttonLayout->addWidget(orientationVentralToolButton);
    
    QToolButton* userViewOneToolButton = new QToolButton();
    userViewOneToolButton->setDefaultAction(this->orientationUserViewOneToolButtonAction);
    
    QToolButton* userViewTwoToolButton = new QToolButton();
    userViewTwoToolButton->setDefaultAction(this->orientationUserViewTwoToolButtonAction);
    
    QToolButton* orientationUserViewSelectToolButton = new QToolButton();
    orientationUserViewSelectToolButton->setDefaultAction(this->orientationUserViewSelectToolButtonAction);
    
    QHBoxLayout* userOrientLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(userOrientLayout, 0, 2, 0);
    userOrientLayout->addWidget(userViewOneToolButton);
    userOrientLayout->addWidget(userViewTwoToolButton);
    //userOrientLayout->addWidget(orientationUserViewAddToolButton);
    userOrientLayout->addWidget(orientationUserViewSelectToolButton);
    userOrientLayout->addStretch();
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(buttonLayout);
    layout->addLayout(userOrientLayout);
    layout->addWidget(orientationResetToolButton, 0, Qt::AlignHCenter);
    
    this->orientationWidgetGroup = new WuQWidgetObjectGroup(this);
    this->orientationWidgetGroup->add(this->orientationLeftToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationRightToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationAnteriorToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationPosteriorToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationDorsalToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationVentralToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationResetToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewOneToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewTwoToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewSelectToolButtonAction);
    this->orientationWidgetGroup->add(this->orientationUserViewSelectToolButtonMenu);

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
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateOrientationWidget(BrowserTabContent* /*browserTabContent*/)
{
    if (this->orientationWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->orientationWidgetGroup->blockSignals(true);
    
    this->orientationWidgetGroup->blockSignals(false);
        
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
    
    this->wholeBrainSurfaceTypeComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceTypeComboBox,
                                          "Select the geometric type of surface for display");
    QObject::connect(this->wholeBrainSurfaceTypeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(wholeBrainSurfaceTypeComboBoxIndexChanged(int)));
    
    this->wholeBrainSurfaceLeftCheckBox = new QCheckBox(" ");
    this->wholeBrainSurfaceLeftMenu = new QMenu("Left", this);
    QObject::connect(this->wholeBrainSurfaceLeftMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(wholeBrainSurfaceLeftMenuTriggered(QAction*)));
    QToolButton* wholeBrainLeftSurfaceToolButton = new QToolButton();
    wholeBrainLeftSurfaceToolButton->setText("Left");
    wholeBrainLeftSurfaceToolButton->setMenu(this->wholeBrainSurfaceLeftMenu);
        WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceLeftCheckBox,
                                          "Enable/Disable display of the left cortical surface");
    QObject::connect(this->wholeBrainSurfaceLeftCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceLeftCheckBoxStateChanged(int)));
    
    this->wholeBrainSurfaceRightCheckBox = new QCheckBox(" ");
    this->wholeBrainSurfaceRightMenu = new QMenu("Right", this);
    QObject::connect(this->wholeBrainSurfaceRightMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(wholeBrainSurfaceRightMenuTriggered(QAction*)));
    QToolButton* wholeBrainRightSurfaceToolButton = new QToolButton();
    wholeBrainRightSurfaceToolButton->setText("Right");
    wholeBrainRightSurfaceToolButton->setMenu(this->wholeBrainSurfaceRightMenu);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceRightCheckBox,
                                          "Enable/Disable display of the right cortical surface");
    QObject::connect(this->wholeBrainSurfaceRightCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceRightCheckBoxStateChanged(int)));
    
    this->wholeBrainSurfaceCerebellumCheckBox = new QCheckBox(" ");
    this->wholeBrainSurfaceCerebellumMenu = new QMenu("Cerebellum", this);
    QObject::connect(this->wholeBrainSurfaceCerebellumMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(wholeBrainSurfaceCerebellumMenuTriggered(QAction*)));
    QToolButton* wholeBrainCerebellumSurfaceToolButton = new QToolButton();
    wholeBrainCerebellumSurfaceToolButton->setText("Cerebellum");
    wholeBrainCerebellumSurfaceToolButton->setMenu(this->wholeBrainSurfaceCerebellumMenu);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceCerebellumCheckBox,
                                          "Enable/Disable display of the cerebellum surface");
    QObject::connect(this->wholeBrainSurfaceCerebellumCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(wholeBrainSurfaceCerebellumCheckBoxStateChanged(int)));
    
    this->wholeBrainSurfaceSeparationLeftRightSpinBox = new QDoubleSpinBox();
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setMinimum(-100000.0);
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setMaximum(100000.0);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationLeftRightSpinBox,
                                          "Adjust the separation of the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationLeftRightSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationLeftRightSpinBoxValueChanged(double)));
    
    this->wholeBrainSurfaceSeparationCerebellumSpinBox = new QDoubleSpinBox();
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setMinimum(-100000.0);
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setMaximum(100000.0);
    WuQtUtilities::setToolTipAndStatusTip(this->wholeBrainSurfaceSeparationCerebellumSpinBox,
                                          "Adjust the separation of the cerebellum from the left and right cortical surfaces");
    QObject::connect(this->wholeBrainSurfaceSeparationCerebellumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(wholeBrainSurfaceSeparationCerebellumSpinBoxSelected(double)));
    
    
    QLabel* separationLabel = new QLabel("Separation");
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(4);
    gridLayout->setHorizontalSpacing(4);
    gridLayout->addWidget(this->wholeBrainSurfaceTypeComboBox, 0, 0, 1, 4);
    gridLayout->addWidget(this->wholeBrainSurfaceLeftCheckBox, 1, 0);
    gridLayout->addWidget(wholeBrainLeftSurfaceToolButton, 1, 1);
    gridLayout->addWidget(this->wholeBrainSurfaceRightCheckBox, 1, 2);
    gridLayout->addWidget(wholeBrainRightSurfaceToolButton, 1, 3);
    gridLayout->addWidget(this->wholeBrainSurfaceCerebellumCheckBox, 2, 0);
    gridLayout->addWidget(wholeBrainCerebellumSurfaceToolButton, 2, 1, 1, 3);
    gridLayout->addWidget(separationLabel, 0, 4);
    gridLayout->addWidget(this->wholeBrainSurfaceSeparationLeftRightSpinBox, 1, 4);
    gridLayout->addWidget(this->wholeBrainSurfaceSeparationCerebellumSpinBox, 2, 4);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(gridLayout);
    
    this->wholeBrainSurfaceOptionsWidgetGroup = new WuQWidgetObjectGroup(this);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceTypeComboBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceLeftCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceLeftMenu);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainLeftSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceRightCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceRightMenu);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainRightSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceCerebellumCheckBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceCerebellumMenu);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(wholeBrainCerebellumSurfaceToolButton);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceSeparationLeftRightSpinBox);
    this->wholeBrainSurfaceOptionsWidgetGroup->add(this->wholeBrainSurfaceSeparationCerebellumSpinBox);
    
    QWidget* w = this->createToolWidget("Surface Options", 
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
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateWholeBrainSurfaceOptionsWidget(BrowserTabContent* browserTabContent)
{
    if (this->wholeBrainSurfaceOptionsWidget->isHidden()) {
        return;
    }
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
 
    //const int tabIndex = this->tabBar->currentIndex();
    ModelDisplayControllerWholeBrain* wholeBrainController = browserTabContent->getSelectedWholeBrainModel();
    const int32_t tabNumber = browserTabContent->getTabNumber();
    
    this->wholeBrainSurfaceOptionsWidgetGroup->blockSignals(true);
    
    std::vector<SurfaceTypeEnum::Enum> availableSurfaceTypes;
    wholeBrainController->getAvailableSurfaceTypes(availableSurfaceTypes);
    
    const SurfaceTypeEnum::Enum selectedSurfaceType = wholeBrainController->getSelectedSurfaceType(tabNumber);
    
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
    
    this->wholeBrainSurfaceLeftCheckBox->setChecked(wholeBrainController->isLeftEnabled(tabNumber));
    this->wholeBrainSurfaceRightCheckBox->setChecked(wholeBrainController->isRightEnabled(tabNumber));
    this->wholeBrainSurfaceCerebellumCheckBox->setChecked(wholeBrainController->isCerebellumEnabled(tabNumber));
    
    this->wholeBrainSurfaceSeparationLeftRightSpinBox->setValue(wholeBrainController->getLeftRightSeparation(tabNumber));
    this->wholeBrainSurfaceSeparationCerebellumSpinBox->setValue(wholeBrainController->getCerebellumSeparation(tabNumber));
    
    this->wholeBrainSurfaceOptionsWidgetGroup->blockSignals(false);
    
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
    QAction* volumeIndicesResetToolButtonAction = WuQtUtilities::createAction("R\nE\nS\nE\nT", 
                                                                         "Reset to the slices indices to those with stereotaxic coordinate (0, 0, 0)", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(volumeIndicesResetActionTriggered()));
    QToolButton* volumeIndicesResetToolButton = new QToolButton;
    volumeIndicesResetToolButton->setDefaultAction(volumeIndicesResetToolButtonAction);
    
    QLabel* parasagittalLabel = new QLabel("P:");
    QLabel* coronalLabel = new QLabel("C:");
    QLabel* axialLabel = new QLabel("A:");
    
    this->volumeIndicesParasagittalCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesParasagittalCheckBox,
                                          "Enable/Disable display of PARASAGITTAL slice");
    QObject::connect(this->volumeIndicesParasagittalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesParasagittalCheckBoxStateChanged(int)));
    
    this->volumeIndicesCoronalCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesCoronalCheckBox,
                                          "Enable/Disable display of CORONAL slice");
    QObject::connect(this->volumeIndicesCoronalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesCoronalCheckBoxStateChanged(int)));
    
    this->volumeIndicesAxialCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesAxialCheckBox,
                                          "Enable/Disable display of AXIAL slice");
    
    QObject::connect(this->volumeIndicesAxialCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesAxialCheckBoxStateChanged(int)));
    
    this->volumeIndicesParasagittalSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesParasagittalSpinBox,
                                          "Change the selected PARASAGITTAL slice");
    QObject::connect(this->volumeIndicesParasagittalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesParasagittalSpinBoxValueChanged(int)));
    
    this->volumeIndicesCoronalSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesCoronalSpinBox,
                                          "Change the selected CORONAL slice");
    QObject::connect(this->volumeIndicesCoronalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesCoronalSpinBoxValueChanged(int)));
    
    this->volumeIndicesAxialSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->volumeIndicesAxialSpinBox,
                                          "Change the selected AXIAL slice");
    QObject::connect(this->volumeIndicesAxialSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesAxialSpinBoxValueChanged(int)));
    
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(2);
    gridLayout->setVerticalSpacing(2);
    gridLayout->addWidget(this->volumeIndicesParasagittalCheckBox, 0, 0);
    gridLayout->addWidget(parasagittalLabel, 0, 1);
    gridLayout->addWidget(this->volumeIndicesParasagittalSpinBox, 0, 2);
    gridLayout->addWidget(this->volumeIndicesCoronalCheckBox, 1, 0);
    gridLayout->addWidget(coronalLabel, 1, 1);
    gridLayout->addWidget(this->volumeIndicesCoronalSpinBox, 1, 2);
    gridLayout->addWidget(this->volumeIndicesAxialCheckBox, 2, 0);
    gridLayout->addWidget(axialLabel, 2, 1);
    gridLayout->addWidget(this->volumeIndicesAxialSpinBox, 2, 2);
    gridLayout->addWidget(volumeIndicesResetToolButton, 0, 3, 3, 1);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(gridLayout);
    layout->addStretch();
    
    this->volumeIndicesWidgetGroup = new WuQWidgetObjectGroup(this);
    this->volumeIndicesWidgetGroup->add(volumeIndicesResetToolButtonAction);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesParasagittalCheckBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesParasagittalSpinBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesCoronalCheckBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesCoronalSpinBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesAxialCheckBox);
    this->volumeIndicesWidgetGroup->add(this->volumeIndicesAxialSpinBox);
    
    QWidget* w = this->createToolWidget("Volume Indices", 
                                        widget, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume indices widget.
 * 
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateVolumeIndicesWidget(BrowserTabContent* /*browserTabContent*/)
{
    if (this->volumeIndicesWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->volumeIndicesWidgetGroup->blockSignals(true);
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    VolumeSliceIndicesSelection* sliceSelection = NULL;
    VolumeFile* vf = NULL;
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController != NULL) {
        if (this->getDisplayedModelController() == volumeController) {
            vf = volumeController->getUnderlayVolumeFile(tabIndex);
            sliceSelection = volumeController->getSelectedVolumeSlices(tabIndex);
            this->volumeIndicesAxialCheckBox->setVisible(false);
            this->volumeIndicesCoronalCheckBox->setVisible(false);
            this->volumeIndicesParasagittalCheckBox->setVisible(false);            
        }
    }
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController != NULL) {
        if (this->getDisplayedModelController() == wholeBrainController) {
            vf = wholeBrainController->getUnderlayVolumeFile(tabIndex);
            sliceSelection = wholeBrainController->getSelectedVolumeSlices(tabIndex);
            this->volumeIndicesAxialCheckBox->setVisible(true);
            this->volumeIndicesCoronalCheckBox->setVisible(true);
            this->volumeIndicesParasagittalCheckBox->setVisible(true);
        }
    }
    
    if (vf != NULL) {
        std::vector<int64_t> dimensions;
        vf->getDimensions(dimensions);
        const int maxAxialDim = (dimensions[2] > 0) ? (dimensions[2] - 1) : 0;
        const int maxCoronalDim = (dimensions[1] > 0) ? (dimensions[1] - 1) : 0;
        const int maxParasagittalDim = (dimensions[0] > 0) ? (dimensions[0] - 1) : 0;
        this->volumeIndicesAxialSpinBox->setMaximum(maxAxialDim);
        this->volumeIndicesCoronalSpinBox->setMaximum(maxCoronalDim);
        this->volumeIndicesParasagittalSpinBox->setMaximum(maxParasagittalDim);
    }
    if (sliceSelection != NULL) {
        this->volumeIndicesAxialCheckBox->setChecked(sliceSelection->isSliceAxialEnabled());
        this->volumeIndicesAxialSpinBox->setValue(sliceSelection->getSliceIndexAxial());
        this->volumeIndicesCoronalCheckBox->setChecked(sliceSelection->isSliceCoronalEnabled());
        this->volumeIndicesCoronalSpinBox->setValue(sliceSelection->getSliceIndexCoronal());
        this->volumeIndicesParasagittalCheckBox->setChecked(sliceSelection->isSliceParasagittalEnabled());
        this->volumeIndicesParasagittalSpinBox->setValue(sliceSelection->getSliceIndexParasagittal());
    }
    
    this->volumeIndicesWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

/**
 * Create the tools widget.
 *
 * @return The tools widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createToolsWidget()
{
    QToolButton* toolBoxToolButton = new QToolButton();
    toolBoxToolButton->setDefaultAction(this->toolsToolBoxToolButtonAction);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addStretch();
    layout->addWidget(toolBoxToolButton);
    
    this->toolsWidgetGroup = new WuQWidgetObjectGroup(this);
    this->toolsWidgetGroup->add(this->toolsToolBoxToolButtonAction);
    
    QWidget* w = this->createToolWidget("Tools", 
                                        widget, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_BOTTOM,
                                        0);
    return w;
}

/**
 * Update the tools widget.
 * 
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateToolsWidget(BrowserTabContent* /*browserTabContent*/)
{
    if (this->toolsWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->toolsWidgetGroup->blockSignals(true);
    
    this->toolsWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

/**
 * Create the window (yoking) widget.
 *
 * @return  The window (yoking) widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createWindowWidget()
{
    QLabel* yokeToLabel = new QLabel("Yoke to:");
    this->windowYokeToTabComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->windowYokeToTabComboBox,
                                          "Select the tab to which the current tab is yoked (view linked)");
    QObject::connect(this->windowYokeToTabComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(windowYokeToTabComboBoxIndexChanged(int)));
    
    this->windowYokeMirroredCheckBox = new QCheckBox("Mirrored");
    WuQtUtilities::setToolTipAndStatusTip(this->windowYokeMirroredCheckBox,
                                          "Enable/Disable left/right mirroring when yoked");
    QObject::connect(this->windowYokeMirroredCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(windowYokeMirroredCheckBoxStateChanged(int)));
    

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 4, 2, 0);
    layout->addWidget(yokeToLabel);
    layout->addWidget(this->windowYokeToTabComboBox);
    layout->addWidget(this->windowYokeMirroredCheckBox);
    
    this->windowWidgetGroup = new WuQWidgetObjectGroup(this);
    this->windowWidgetGroup->add(this->windowYokeToTabComboBox);
    this->windowWidgetGroup->add(this->windowYokeMirroredCheckBox);
    
    QWidget* w = this->createToolWidget("Window", 
                                        widget, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    return w;
}

/**
 * Update the window widget.
 * 
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateWindowWidget(BrowserTabContent* /*browserTabContent*/)
{
    if (this->windowWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->windowWidgetGroup->blockSignals(true);
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    
    this->windowYokeToTabComboBox->clear();
    
    const int32_t numTabs = allTabsEvent.getNumberOfBrowserTabs();
    for (int32_t i = 0; i < numTabs; i++) {
        BrowserTabContent* btc = allTabsEvent.getBrowserTab(i);
        this->windowYokeToTabComboBox->addItem(btc->getName());
    }
    this->windowWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
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
                                             ModelDisplayControllerSurface*)),
                     this,
                     SLOT(surfaceSelectionControlChanged(const StructureEnum::Enum,
                                                         ModelDisplayControllerSurface*)));
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 6, 2, 2);
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
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateSingleSurfaceOptionsWidget(BrowserTabContent* browserTabContent)
{
    if (this->singleSurfaceSelectionWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->singleSurfaceSelectionWidgetGroup->blockSignals(true);
    
    this->surfaceSurfaceSelectionControl->updateControl(browserTabContent->getSurfaceModelSelector());
    
    this->singleSurfaceSelectionWidgetGroup->blockSignals(false);
    
    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
}

/**
 * Create the volume montage widget.
 *
 * @return The volume montage widget.
 */
QWidget* 
BrainBrowserWindowToolBar::createVolumeMontageWidget()
{
    QLabel* rowsLabel = new QLabel("Rows:");
    QLabel* columnsLabel = new QLabel("Columns:");
    QLabel* spacingLabel = new QLabel("Spacing:");
    
    this->montageRowsSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->montageRowsSpinBox,
                                          "Select the number of rows in montage of volume slices");
    QObject::connect(this->montageRowsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageRowsSpinBoxValueChanged(int)));
    
    this->montageColumnsSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->montageColumnsSpinBox,
                                          "Select the number of columns in montage of volume slices");
    QObject::connect(this->montageColumnsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageColumnsSpinBoxValueChanged(int)));

    this->montageSpacingSpinBox = new QSpinBox();
    WuQtUtilities::setToolTipAndStatusTip(this->montageSpacingSpinBox,
                                          "Select the number of slices skipped between displayed montage slices");
    QObject::connect(this->montageSpacingSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageSpacingSpinBoxValueChanged(int)));
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(2);
    gridLayout->addWidget(rowsLabel, 0, 0);
    gridLayout->addWidget(this->montageRowsSpinBox, 0, 1);
    gridLayout->addWidget(columnsLabel, 1, 0);
    gridLayout->addWidget(this->montageColumnsSpinBox, 1, 1);
    gridLayout->addWidget(spacingLabel, 2, 0);
    gridLayout->addWidget(this->montageSpacingSpinBox, 2, 1);

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(gridLayout);
    
    this->volumeMontageWidgetGroup = new WuQWidgetObjectGroup(this);
    this->volumeMontageWidgetGroup->add(this->montageRowsSpinBox);
    this->volumeMontageWidgetGroup->add(this->montageColumnsSpinBox);
    this->volumeMontageWidgetGroup->add(this->montageSpacingSpinBox);
    
    QWidget* w = this->createToolWidget("Montage", 
                                        widget, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume montage widget.
 * 
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateVolumeMontageWidget(BrowserTabContent* /*browserTabContent*/)
{
    if (this->volumeMontageWidget->isHidden()) {
        return;
    }

    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->volumeMontageWidgetGroup->blockSignals(true);
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController != NULL) {
        this->montageRowsSpinBox->setValue(volumeController->getMontageNumberOfRows(tabIndex));
        this->montageColumnsSpinBox->setValue(volumeController->getMontageNumberOfColumns(tabIndex));
        this->montageSpacingSpinBox->setValue(volumeController->getMontageSliceSpacing(tabIndex));
    }
    
    
    this->volumeMontageWidgetGroup->blockSignals(false);

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
    QIcon parasagittalIcon;
    const bool parasagittalIconValid =
    WuQtUtilities::loadIcon(":/view-plane-parasagittal.png", 
                            parasagittalIcon);

    QIcon coronalIcon;
    const bool coronalIconValid =
    WuQtUtilities::loadIcon(":/view-plane-coronal.png", 
                            coronalIcon);

    QIcon axialIcon;
    const bool axialIconValid =
    WuQtUtilities::loadIcon(":/view-plane-axial.png", 
                            axialIcon);
    
    this->volumePlaneParasagittalToolButtonAction = 
    WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::PARASAGITTAL), 
                                                                                      "View the PARASAGITTAL slice", 
                                                                                      this);
    this->volumePlaneParasagittalToolButtonAction->setCheckable(true);
    if (parasagittalIconValid) {
        this->volumePlaneParasagittalToolButtonAction->setIcon(parasagittalIcon);
    }
    
    this->volumePlaneCoronalToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::CORONAL), 
                                                                                 "View the CORONAL slice", 
                                                                                 this);
    this->volumePlaneCoronalToolButtonAction->setCheckable(true);
    if (coronalIconValid) {
        this->volumePlaneCoronalToolButtonAction->setIcon(coronalIcon);
    }

    this->volumePlaneAxialToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::AXIAL), 
                                                                               "View the AXIAL slice", 
                                                                               this);
    this->volumePlaneAxialToolButtonAction->setCheckable(true);
    if (axialIconValid) {
        this->volumePlaneAxialToolButtonAction->setIcon(axialIcon);
    }

    this->volumePlaneAllToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::ALL), 
                                                                             "View the PARASAGITTAL, CORONAL, and AXIAL slices", 
                                                                             this);
    this->volumePlaneAllToolButtonAction->setCheckable(true);
    

    this->volumePlaneActionGroup = new QActionGroup(this);
    this->volumePlaneActionGroup->addAction(this->volumePlaneParasagittalToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneCoronalToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneAxialToolButtonAction);
    this->volumePlaneActionGroup->addAction(this->volumePlaneAllToolButtonAction);
    this->volumePlaneActionGroup->setExclusive(true);
    QObject::connect(this->volumePlaneActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(volumePlaneActionGroupTriggered(QAction*)));

    
    this->volumePlaneViewOrthogonalToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewModeEnum::toGuiNameAbbreviation(VolumeSliceViewModeEnum::ORTHOGONAL),
                                                                                  "View the volume orthogonal axis",                                                                           
                                                                                  this);
    this->volumePlaneViewOrthogonalToolButtonAction->setCheckable(true);
    
    this->volumePlaneViewObliqueToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewModeEnum::toGuiNameAbbreviation(VolumeSliceViewModeEnum::OBLIQUE),
                                                                           "View the volume oblique axis",                                                                           
                                                                           this);
    this->volumePlaneViewObliqueToolButtonAction->setCheckable(true);
    
    this->volumePlaneViewMontageToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewModeEnum::toGuiNameAbbreviation(VolumeSliceViewModeEnum::MONTAGE),
                                                                           "View a montage of orthogonal slices",                                                                           
                                                                           this);
    this->volumePlaneViewMontageToolButtonAction->setCheckable(true);
    
    this->volumePlaneViewActionGroup = new QActionGroup(this);
    this->volumePlaneViewActionGroup->addAction(this->volumePlaneViewOrthogonalToolButtonAction);
    this->volumePlaneViewActionGroup->addAction(this->volumePlaneViewMontageToolButtonAction);
    this->volumePlaneViewActionGroup->addAction(this->volumePlaneViewObliqueToolButtonAction);
    QObject::connect(this->volumePlaneViewActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(volumePlaneViewActionGroupTriggered(QAction*)));
    
    this->volumePlaneResetToolButtonAction = WuQtUtilities::createAction("Reset", 
                                                                         "Reset to remove panning/zooming", 
                                                                         this, 
                                                                         this, 
                                                                         SLOT(volumePlaneResetToolButtonTriggered(bool)));
    
    
    QToolButton* volumePlaneParasagittalToolButton = new QToolButton();
    volumePlaneParasagittalToolButton->setDefaultAction(this->volumePlaneParasagittalToolButtonAction);
    
    QToolButton* volumePlaneCoronalToolButton = new QToolButton();
    volumePlaneCoronalToolButton->setDefaultAction(this->volumePlaneCoronalToolButtonAction);
    
    QToolButton* volumePlaneAxialToolButton = new QToolButton();
    volumePlaneAxialToolButton->setDefaultAction(this->volumePlaneAxialToolButtonAction);
    
    QToolButton* volumePlaneAllToolButton = new QToolButton();
    volumePlaneAllToolButton->setDefaultAction(this->volumePlaneAllToolButtonAction);
    
    QToolButton* volumePlaneViewMontageToolButton = new QToolButton();
    volumePlaneViewMontageToolButton->setDefaultAction(this->volumePlaneViewMontageToolButtonAction);
    
    QToolButton* volumePlaneViewObliqueToolButton = new QToolButton();
    volumePlaneViewObliqueToolButton->setDefaultAction(this->volumePlaneViewObliqueToolButtonAction);
    
    QToolButton* volumePlaneViewOrthogonalToolButton = new QToolButton();
    volumePlaneViewOrthogonalToolButton->setDefaultAction(this->volumePlaneViewOrthogonalToolButtonAction);
    
    QToolButton* volumePlaneResetToolButton = new QToolButton();
    volumePlaneResetToolButton->setDefaultAction(this->volumePlaneResetToolButtonAction);
    
    WuQtUtilities::matchWidgetHeights(volumePlaneParasagittalToolButton,
                                      volumePlaneCoronalToolButton,
                                      volumePlaneAxialToolButton,
                                      volumePlaneAllToolButton);
    
    
    QHBoxLayout* planeLayout1 = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(planeLayout1, 0, 2, 0);
    planeLayout1->addWidget(volumePlaneParasagittalToolButton);
    planeLayout1->addWidget(volumePlaneCoronalToolButton);
    planeLayout1->addWidget(volumePlaneAxialToolButton);
    planeLayout1->addWidget(volumePlaneAllToolButton);
    planeLayout1->addStretch();

    QHBoxLayout* planeLayout2 = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(planeLayout2, 0, 2, 0);
    planeLayout2->addWidget(volumePlaneViewOrthogonalToolButton);
    planeLayout2->addWidget(volumePlaneViewMontageToolButton);
    planeLayout2->addWidget(volumePlaneViewObliqueToolButton);
    planeLayout2->addWidget(volumePlaneResetToolButton);
    planeLayout2->addStretch();
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 2, 0);
    layout->addLayout(planeLayout1);
    layout->addLayout(planeLayout2);
    
    this->volumePlaneWidgetGroup = new WuQWidgetObjectGroup(this);
    this->volumePlaneWidgetGroup->add(this->volumePlaneActionGroup);
    this->volumePlaneWidgetGroup->add(this->volumePlaneResetToolButtonAction);
    
    QWidget* w = this->createToolWidget("Slice Plane", 
                                        widget, 
                                        WIDGET_PLACEMENT_LEFT, 
                                        WIDGET_PLACEMENT_TOP,
                                        0);
    w->setVisible(false);
    return w;
}

/**
 * Update the volume plane orientation widget.
 * 
 * @param modelDisplayController
 *   The active model display controller (may be NULL).
 */
void 
BrainBrowserWindowToolBar::updateVolumePlaneWidget(BrowserTabContent* /*browserTabContent*/)
{
    if (this->volumePlaneWidget->isHidden()) {
        return;
    }
    
    this->incrementUpdateCounter(__CARET_FUNCTION_NAME__);
    
    this->volumePlaneWidgetGroup->blockSignals(true);
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController != NULL) {
        switch (volumeController->getSliceViewPlane(tabIndex)) {
            case VolumeSliceViewPlaneEnum::ALL:
                this->volumePlaneAllToolButtonAction->setChecked(true);
                break;
            case VolumeSliceViewPlaneEnum::AXIAL:
                this->volumePlaneAxialToolButtonAction->setChecked(true);
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                this->volumePlaneCoronalToolButtonAction->setChecked(true);
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                this->volumePlaneParasagittalToolButtonAction->setChecked(true);
                break;
        }
        
        switch(volumeController->getSliceViewMode(tabIndex)) {
            case VolumeSliceViewModeEnum::MONTAGE:
                this->volumePlaneViewMontageToolButtonAction->setChecked(true);
                break;
            case VolumeSliceViewModeEnum::OBLIQUE:
                this->volumePlaneViewObliqueToolButtonAction->setChecked(true);
                break;
            case VolumeSliceViewModeEnum::ORTHOGONAL:
                this->volumePlaneViewOrthogonalToolButtonAction->setChecked(true);
                break;
        }
    }

    
    this->volumePlaneWidgetGroup->blockSignals(false);

    this->decrementUpdateCounter(__CARET_FUNCTION_NAME__);
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
    //QLabel* nameLabel = new QLabel("<html><b>" + name + "<b></html>");
    QLabel* nameLabel = new QLabel("<html>" + name + "</html>");
    
    QWidget* w = new QWidget();
    QGridLayout* layout = new QGridLayout(w);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);    
    WuQtUtilities::setLayoutMargins(layout, 2, 2, 0);
    switch (contentPlacement) {
        case WIDGET_PLACEMENT_BOTTOM:
            //layout->addStretch();
            layout->setRowStretch(0, 100);
            layout->setRowStretch(1, 0);
            layout->addWidget(childWidget, 1, 0, 1, 2);
            break;
        case WIDGET_PLACEMENT_TOP:
            layout->setRowStretch(1, 100);
            layout->setRowStretch(0, 0);
            layout->addWidget(childWidget, 0, 0, 1, 2);
            //layout->addStretch();
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
        WuQtUtilities::setLayoutMargins(horizLayout, 0, 0, 0);
        if (addVerticalBarOnLeftSide) {
            horizLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
            horizLayout->addSpacing(3);
        }
        horizLayout->addWidget(w);
        if (addVerticalBarOnRightSide) {
            horizLayout->addSpacing(3);
            horizLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
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
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Called when a view mode is selected.
 */
void 
BrainBrowserWindowToolBar::viewModeRadioButtonClicked(QAbstractButton*)
{
    CaretLogEntering();
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    if (this->viewModeSurfaceRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE);
    }
    else if (this->viewModeVolumeRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES);
    }
    else if (this->viewModeWholeBrainRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN);
    }
    else {
        btc->setSelectedModelType(ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID);
    }
    
    this->checkUpdateCounter();
    this->updateToolBar();   
    this->updateTabName(-1);
}

/**
 * Called when orientation left button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationLeftToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelDisplayController* mdc = btc->getDisplayedModelController();
    if (mdc != NULL) {
        mdc->leftView(btc->getTabNumber());
        this->updateGraphicsWindow();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation right button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationRightToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelDisplayController* mdc = btc->getDisplayedModelController();
    if (mdc != NULL) {
        mdc->rightView(btc->getTabNumber());
        this->updateGraphicsWindow();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation anterior button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationAnteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelDisplayController* mdc = btc->getDisplayedModelController();
    if (mdc != NULL) {
        mdc->anteriorView(btc->getTabNumber());
        this->updateGraphicsWindow();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation posterior button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationPosteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelDisplayController* mdc = btc->getDisplayedModelController();
    if (mdc != NULL) {
        mdc->posteriorView(btc->getTabNumber());
        this->updateGraphicsWindow();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation dorsal button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationDorsalToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelDisplayController* mdc = btc->getDisplayedModelController();
    if (mdc != NULL) {
        mdc->dorsalView(btc->getTabNumber());
        this->updateGraphicsWindow();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation ventral button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationVentralToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelDisplayController* mdc = btc->getDisplayedModelController();
    if (mdc != NULL) {
        mdc->ventralView(btc->getTabNumber());
        this->updateGraphicsWindow();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation reset button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationResetToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    ModelDisplayController* mdc = btc->getDisplayedModelController();
    if (mdc != NULL) {
        mdc->resetView(btc->getTabNumber());
        this->updateVolumeIndicesWidget(btc);
        this->updateGraphicsWindow();
    }
    
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view one button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewOneToolButtonTriggered(bool /*checked*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view one button is pressed.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewTwoToolButtonTriggered(bool /*checked*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view menu is about to display.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewSelectToolButtonMenuAboutToShow()
{
    CaretLogEntering();
    this->checkUpdateCounter();
}

/**
 * Called when orientation user view selection is made from the menu.
 */
void 
BrainBrowserWindowToolBar::orientationUserViewSelectToolButtonMenuTriggered(QAction* /*action*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
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
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController == NULL) {
        return;
    }

    int32_t comboBoxIndex = this->wholeBrainSurfaceTypeComboBox->currentIndex();
    if (comboBoxIndex >= 0) {
        const int32_t integerCode = this->wholeBrainSurfaceTypeComboBox->itemData(comboBoxIndex).toInt();
        bool isValid = false;
        const SurfaceTypeEnum::Enum surfaceType = SurfaceTypeEnum::fromIntegerCode(integerCode, &isValid);
        if (isValid) {
            wholeBrainController->setSelectedSurfaceType(tabIndex, surfaceType);
            //this->updateUserInterface();
            this->updateVolumeIndicesWidget(btc); // slices may get deselected
            this->updateGraphicsWindow();
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
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController == NULL) {
        return;
    }
    
    wholeBrainController->setLeftEnabled(tabIndex, this->wholeBrainSurfaceLeftCheckBox->isChecked());
    this->updateGraphicsWindow();
}

void 
BrainBrowserWindowToolBar::wholeBrainSurfaceLeftMenuTriggered(QAction*)
{
    CaretLogEntering();
    this->checkUpdateCounter();
}

void 
BrainBrowserWindowToolBar::wholeBrainSurfaceRightMenuTriggered(QAction*)
{
    CaretLogEntering();
    this->checkUpdateCounter();
}

void 
BrainBrowserWindowToolBar::wholeBrainSurfaceCerebellumMenuTriggered(QAction*)
{
    CaretLogEntering();
    this->checkUpdateCounter();
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
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController == NULL) {
        return;
    }
    
    wholeBrainController->setRightEnabled(tabIndex, this->wholeBrainSurfaceRightCheckBox->isChecked());
    this->updateGraphicsWindow();
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
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController == NULL) {
        return;
    }
    
    wholeBrainController->setCerebellumEnabled(tabIndex, this->wholeBrainSurfaceCerebellumCheckBox->isChecked());
    this->updateGraphicsWindow();
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
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController == NULL) {
        return;
    }
    
    wholeBrainController->setLeftRightSeparation(tabIndex, 
                                                  this->wholeBrainSurfaceSeparationLeftRightSpinBox->value());
    this->updateGraphicsWindow();
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
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController == NULL) {
        return;
    }
    
    wholeBrainController->setCerebellumSeparation(tabIndex, 
                                                 this->wholeBrainSurfaceSeparationCerebellumSpinBox->value());
    this->updateGraphicsWindow();
}

/**
 * Called when volume indices RESET tool button is pressed.
 */
void
BrainBrowserWindowToolBar::volumeIndicesResetActionTriggered()
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController != NULL) {
        volumeController->setSlicesToOrigin(tabIndex);
    }
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController != NULL) {
        wholeBrainController->setSlicesToOrigin(tabIndex);
    }
    
    this->updateVolumeIndicesWidget(btc);
    this->updateGraphicsWindow();
}

/**
 * Called when volume indices parasagittal check box is toggled.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesParasagittalCheckBoxStateChanged(int /*state*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController != NULL) {
        if (this->getDisplayedModelController() == wholeBrainController) {
            wholeBrainController->getSelectedVolumeSlices(tabIndex)->setSliceParasagittalEnabled(this->volumeIndicesParasagittalCheckBox->isChecked());
            this->updateGraphicsWindow();
        }
    }
}

/**
 * Called when volume indices coronal check box is toggled.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesCoronalCheckBoxStateChanged(int /*state*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController != NULL) {
        if (this->getDisplayedModelController() == wholeBrainController) {
            wholeBrainController->getSelectedVolumeSlices(tabIndex)->setSliceCoronalEnabled(this->volumeIndicesCoronalCheckBox->isChecked());
            this->updateGraphicsWindow();
        }
    }
}

/**
 * Called when volume indices axial check box is toggled.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesAxialCheckBoxStateChanged(int /*state*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController != NULL) {
        if (this->getDisplayedModelController() == wholeBrainController) {
            wholeBrainController->getSelectedVolumeSlices(tabIndex)->setSliceAxialEnabled(this->volumeIndicesAxialCheckBox->isChecked());
            this->updateGraphicsWindow();
        }
    }
}

/**
 * Called when volume indices parasagittal spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesParasagittalSpinBoxValueChanged(int /*i*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    VolumeSliceIndicesSelection* sliceSelection = NULL;
    if (wholeBrainController != NULL) {
        if (this->getDisplayedModelController() == wholeBrainController) {
            sliceSelection = wholeBrainController->getSelectedVolumeSlices(tabIndex);
        }
    }
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController != NULL) {
        if (this->getDisplayedModelController() == volumeController) {
            sliceSelection = volumeController->getSelectedVolumeSlices(tabIndex);
        }
    }
    
    if (sliceSelection != NULL) {
        sliceSelection->setSliceIndexParasagittal(this->volumeIndicesParasagittalSpinBox->value());
    }
    
    this->updateGraphicsWindow();
}

/**
 * Called when volume indices coronal spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesCoronalSpinBoxValueChanged(int /*i*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    VolumeSliceIndicesSelection* sliceSelection = NULL;

    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController != NULL) {
        if (this->getDisplayedModelController() == wholeBrainController) {
            sliceSelection = wholeBrainController->getSelectedVolumeSlices(tabIndex);
        }
    }
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController != NULL) {
        if (this->getDisplayedModelController() == volumeController) {
            sliceSelection = volumeController->getSelectedVolumeSlices(tabIndex);
        }
    }
    
    if (sliceSelection != NULL) {
        sliceSelection->setSliceIndexCoronal(this->volumeIndicesCoronalSpinBox->value());
    }
    
    this->updateGraphicsWindow();
}

/**
 * Called when volume indices axial spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::volumeIndicesAxialSpinBoxValueChanged(int /*i*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    VolumeSliceIndicesSelection* sliceSelection = NULL;
    
    ModelDisplayControllerWholeBrain* wholeBrainController = btc->getSelectedWholeBrainModel();
    if (wholeBrainController != NULL) {
        if (this->getDisplayedModelController() == wholeBrainController) {
            sliceSelection = wholeBrainController->getSelectedVolumeSlices(tabIndex);
        }
    }
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController != NULL) {
        if (this->getDisplayedModelController() == volumeController) {
            sliceSelection = volumeController->getSelectedVolumeSlices(tabIndex);
        }
    }

    if (sliceSelection != NULL) {        
        sliceSelection->setSliceIndexAxial(this->volumeIndicesAxialSpinBox->value());
    }
    
    this->updateGraphicsWindow();
}

/**
 * Called when window yoke to tab combo box is selected.
 */
void 
BrainBrowserWindowToolBar::windowYokeToTabComboBoxIndexChanged(int /*indx*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
}

/**
 * Called when yoke mirrored checkbox is toggled.
 */
void 
BrainBrowserWindowToolBar::windowYokeMirroredCheckBoxStateChanged(int /*state*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
}

/**
 * Called when a single surface control is changed.
 * @param structure
 *      Structure that is selected.
 * @param surfaceController
 *     Controller that is selected.
 */
void 
BrainBrowserWindowToolBar::surfaceSelectionControlChanged(
                                    const StructureEnum::Enum structure,
                                    ModelDisplayControllerSurface* surfaceController)
{
    if (surfaceController != NULL) {
        BrowserTabContent* btc = this->getTabContentFromSelectedTab();
        ModelDisplayControllerSurfaceSelector* surfaceModelSelector = btc->getSurfaceModelSelector();
        surfaceModelSelector->setSelectedStructure(structure);
        surfaceModelSelector->setSelectedSurfaceController(surfaceController);
        btc->invalidateSurfaceColoring();
        this->updateUserInterface();
        this->updateGraphicsWindow();
    }
    
    this->updateTabName(-1);
    
    this->checkUpdateCounter();    
}

/**
 * Called when volume slice plane button is clicked.
 */
void 
BrainBrowserWindowToolBar::volumePlaneActionGroupTriggered(QAction* action)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    VolumeSliceViewPlaneEnum::Enum plane = VolumeSliceViewPlaneEnum::AXIAL;
    
    if (action == this->volumePlaneAllToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::ALL;
    }
    else if (action == this->volumePlaneAxialToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::AXIAL;
        
    }
    else if (action == this->volumePlaneCoronalToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::CORONAL;
        
    }
    else if (action == this->volumePlaneParasagittalToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::PARASAGITTAL;
    }
    else {
        CaretLogSevere("Invalid volume plane action: " + action->text());
    }
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController == NULL) {
        return;
    }
    
    volumeController->setSliceViewPlane(tabIndex, plane);
    this->updateVolumeIndicesWidget(btc);
    this->updateGraphicsWindow();
}

/**
 * Called when volume view plane button is clicked.
 */
void 
BrainBrowserWindowToolBar::volumePlaneViewActionGroupTriggered(QAction* action)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    VolumeSliceViewModeEnum::Enum mode = VolumeSliceViewModeEnum::ORTHOGONAL;
    
    if (action == this->volumePlaneViewOrthogonalToolButtonAction) {
        mode = VolumeSliceViewModeEnum::ORTHOGONAL;
    }
    else if (action == this->volumePlaneViewMontageToolButtonAction) {
        mode = VolumeSliceViewModeEnum::MONTAGE;
    }
    else if (action == this->volumePlaneViewObliqueToolButtonAction) {
        mode = VolumeSliceViewModeEnum::OBLIQUE;
    }
    else {
        CaretLogSevere("Invalid volume plane view action: " + action->text());
    }
     
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController == NULL) {
        return;
    }
    volumeController->setSliceViewMode(tabIndex, mode);
    this->updateVolumeIndicesWidget(btc);
    this->updateGraphicsWindow();
}

/**
 * Called when volume reset slice view button is pressed.
 */
void 
BrainBrowserWindowToolBar::volumePlaneResetToolButtonTriggered(bool /*checked*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController == NULL) {
        return;
    }
    volumeController->resetView(tabIndex);
    this->updateVolumeIndicesWidget(btc);
    this->updateGraphicsWindow();
}

/**
 * Called when montage rows spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::montageRowsSpinBoxValueChanged(int /*i*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController == NULL) {
        return;
    }
    volumeController->setMontageNumberOfRows(tabIndex, this->montageRowsSpinBox->value());
    this->updateGraphicsWindow();
}

/**
 * Called when montage columns spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::montageColumnsSpinBoxValueChanged(int /*i*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController == NULL) {
        return;
    }
    volumeController->setMontageNumberOfColumns(tabIndex, this->montageColumnsSpinBox->value());
    this->updateGraphicsWindow();
}

/**
 * Called when montage spacing spin box value is changed.
 */
void 
BrainBrowserWindowToolBar::montageSpacingSpinBoxValueChanged(int /*i*/)
{
    CaretLogEntering();
    this->checkUpdateCounter();
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    ModelDisplayControllerVolume* volumeController = btc->getSelectedVolumeModel();
    if (volumeController == NULL) {
        return;
    }
    volumeController->setMontageSliceSpacing(tabIndex, this->montageSpacingSpinBox->value());
    this->updateGraphicsWindow();
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
            BrowserTabContent* btc = this->getTabContentFromSelectedTab();
            getModelEvent->setBrowserTabContent(btc);
            getModelEvent->setModelDisplayController(btc->getDisplayedModelController());
            getModelEvent->setWindowTabNumber(btc->getTabNumber());
            getModelEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();
        
        this->updateToolBar();
    }
    else {
        
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
    if ((tabIndex >= 0) && (tabIndex < this->tabBar->count())) {
        void* p = this->tabBar->tabData(tabIndex).value<void*>();
        BrowserTabContent* btc = (BrowserTabContent*)p;
        return btc;
    }
    
    return NULL;
}

/**
 * Get the model display controller displayed in the selected tab.
 * @return
 *     Model display controller in the selected tab or NULL if 
 *     no model is displayed.
 */
ModelDisplayController* 
BrainBrowserWindowToolBar::getDisplayedModelController()
{
    ModelDisplayController* mdc = NULL;
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    if (btc != NULL) {
        mdc = btc->getDisplayedModelController();
    }
    
    return mdc;
}



