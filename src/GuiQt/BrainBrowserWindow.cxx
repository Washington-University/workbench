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

#include <utility>

#include <QActionGroup>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QListIterator>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScreen>
#include <QTabBar>
#include <QTextEdit>
#include <QToolButton>
#include <QUrl>
#include <QUuid>

#define __BRAIN_BROWSER_WINDOW_DECLARE__
#include "BrainBrowserWindow.h"
#undef __BRAIN_BROWSER_WINDOW_DECLARE__

#include "AboutWorkbenchDialog.h"
#include "ApplicationInformation.h"
#include "BorderFile.h"
#include "BorderFileSplitDialog.h"
#include "Brain.h"
#include "BrainBrowserWindowEditMenuItemEnum.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretFileDialog.h"
#include "CaretFileRemoteDialog.h"
#include "CaretPreferences.h"
#include "CursorDisplayScoped.h"
#include "CziImageFile.h"
#include "DataFileException.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesImages.h"
#include "EventBrowserWindowNew.h"
#include "CaretLogger.h"
#include "ElapsedTimer.h"
#include "EventGetViewportSize.h"
#include "EventBrowserTabReopenAvailable.h"
#include "EventBrowserWindowCreateTabs.h"
#include "EventBrowserWindowContent.h"
#include "EventBrowserWindowGetTabs.h"
#include "EventBrowserWindowPixelSizeInfoEvent.h"
#include "EventBrowserTabIndicesGetAllViewed.h"
#include "EventCaretMappableDataFilesAndMapsInDisplayedOverlays.h"
#include "EventDataFileRead.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventGraphicsTimingOneWindow.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventGraphicsPaintSoonOneWindow.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventTileTabsGridConfigurationModification.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FociProjectionDialog.h"
#include "GapsAndMargins.h"
#include "GuiManager.h"
#include "LockAspectWarningDialog.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelWholeBrain.h"
#include "PlainTextStringBuilder.h"
#include "ProgressReportingDialog.h"
#include "RecentFilesDialog.h"
#include "RecentSceneMenu.h"
#include "SamplesMetaDataManager.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneEnumeratedType.h"
#include "SceneFile.h"
#include "SceneWindowGeometry.h"
#include "SessionManager.h"
#include "SpacerTabContent.h"
#include "SpecFile.h"
#include "SpecFileManagementDialog.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "SurfaceMontageConfigurationAbstract.h"
#include "SurfaceSelectionViewController.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsLayoutManualConfiguration.h"
#include "TileTabsGridConfigurationModifier.h"
#include "WindowTabAspectRatios.h"
#include "WuQDataEntryDialog.h"
#include "WuQDoubleSpinBox.h"
#include "WuQMacroManager.h"
#include "WuQMacroMenu.h"
#include "WuQMessageBox.h"
#include "WuQMessageBoxTwo.h"
#include "WuQTabBar.h"
#include "WuQtUtilities.h"
#include "WuQTextEditorDialog.h"
#include "VtkFileExporter.h"
#include "OmeZarrImageFile.h"


using namespace caret;

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index for this window.
 * @param browserTabContent  
 *    If not NULL, this is the tab displayed in the window.
 *    If NULL, a new tab is created.
 * @param parent
 *    Parent of this object
 * @param flags
 *    Flags for Qt.
 */
BrainBrowserWindow::BrainBrowserWindow(const int browserWindowIndex,
                                       BrowserTabContent* browserTabContent,
                                       const CreateDefaultTabsMode createDefaultTabsMode,
                                       QWidget* parent,
                                       Qt::WindowFlags flags)
: QMainWindow(parent, flags),
m_browserWindowIndex(browserWindowIndex)
{
    m_developMenuAction = NULL;
    
    m_objectNamePrefix = QString("Window%1").arg((int)(browserWindowIndex + 1), 2, 10, QLatin1Char('0'));
    
    std::unique_ptr<EventBrowserWindowContent> bwc = EventBrowserWindowContent::newWindowContent(m_browserWindowIndex);
    EventManager::get()->sendEvent(bwc->getPointer());
    
    m_browserWindowContent = bwc->getBrowserWindowContent();
    CaretAssert(m_browserWindowContent);
    
    if (BrainBrowserWindow::s_firstWindowFlag) {
        BrainBrowserWindow::s_firstWindowFlag = false;
    }
    
    setAttribute(Qt::WA_DeleteOnClose);
    
    setWindowTitle(ApplicationInformation().getName()
                         + " "
                         + AString::number(m_browserWindowIndex + 1));
#if QT_VERSION >= 0x060000
    setWindowTitle(windowTitle()
                   + " Qt6");
#endif
    setObjectName(windowTitle());
    
    BrainOpenGLWidget* shareOpenGLContextWidget = NULL;
    if ( ! s_brainBrowserWindows.empty()) {
        std::set<BrainBrowserWindow*>::iterator iter = s_brainBrowserWindows.begin();
        CaretAssert(iter != s_brainBrowserWindows.end());
        shareOpenGLContextWidget = (*iter)->m_openGLWidget;
        CaretAssert(shareOpenGLContextWidget);
    }
    m_openGLWidget = new BrainOpenGLWidget(this,
                                           shareOpenGLContextWidget,
                                           browserWindowIndex);

    const int openGLSizeX = 500;
    const int openGLSizeY = (WuQtUtilities::isSmallDisplay() ? 200 : 375);
    m_openGLWidget->setMinimumSize(openGLSizeX, 
                                       openGLSizeY);
    
    setCentralWidget(m_openGLWidget);
    
    m_overlayVerticalToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Overlay ToolBox",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_OVERLAYS_VERTICAL,
                                          m_objectNamePrefix,
                                          this);
    m_overlayVerticalToolBox->setAllowedAreas(Qt::LeftDockWidgetArea);
    
    m_overlayHorizontalToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Overlay ToolBox ",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_OVERLAYS_HORIZONTAL,
                                          m_objectNamePrefix,
                                          this);
    m_overlayHorizontalToolBox->setAllowedAreas(Qt::BottomDockWidgetArea);

    if (WuQtUtilities::isSmallDisplay()) {
        m_overlayActiveToolBox = m_overlayVerticalToolBox;
        addDockWidget(Qt::LeftDockWidgetArea, m_overlayVerticalToolBox);
        m_overlayHorizontalToolBox->setVisible(false);
    }
    else {
        m_overlayActiveToolBox = m_overlayHorizontalToolBox;
        addDockWidget(Qt::BottomDockWidgetArea, m_overlayHorizontalToolBox);
        m_overlayVerticalToolBox->setVisible(false);
    }
    
    QObject::connect(m_overlayHorizontalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayHorizontalToolBoxVisibilityChanged(bool)));
    QObject::connect(m_overlayVerticalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayVerticalToolBoxVisibilityChanged(bool)));
    
    m_featuresToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Features ToolBox",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_FEATURES,
                                          m_objectNamePrefix,
                                          this);
    m_featuresToolBox->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_featuresToolBox);
    
    createActionsUsedByToolBar();
    m_overlayToolBoxAction->blockSignals(true);
    m_overlayToolBoxAction->setChecked(true);
    m_overlayToolBoxAction->blockSignals(false);
    m_featuresToolBoxAction->blockSignals(true);
    m_featuresToolBoxAction->setChecked(true);
    m_featuresToolBoxAction->blockSignals(false);
    
    m_toolbar = new BrainBrowserWindowToolBar(m_browserWindowIndex,
                                              browserTabContent,
                                              m_overlayToolBoxAction,
                                              m_featuresToolBoxAction,
                                              m_toolBarLockWindowAndAllTabAspectRatioButton,
                                              m_toolBarUndoUnlockWindowAndAllTabAspectRatioButton,
                                              m_objectNamePrefix,
                                              this);
    m_showToolBarAction = m_toolbar->toolBarToolButtonAction;
    addToolBar(m_toolbar);
    
    createActions();
    
    createMenus();
    
    m_toolbar->updateToolBar();

    processShowOverlayToolBox(m_overlayToolBoxAction->isChecked());
    processHideFeaturesToolBox();
    
    if (browserTabContent == NULL) {
        if (createDefaultTabsMode == CREATE_DEFAULT_TABS_YES) {
            m_toolbar->addDefaultTabsAfterLoadingSpecFile();
        }
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_defaultWindowComponentStatus.isFeaturesToolBoxDisplayed = false;
    m_defaultWindowComponentStatus.isOverlayToolBoxDisplayed  = true;
    m_defaultWindowComponentStatus.isToolBarDisplayed         = true;
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_MENUS_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GET_TABS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_VIEWPORT_SIZE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_PAINT_SOON_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_PAINT_SOON_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_TILE_TABS_MODIFICATION);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);

    if (m_overlayHorizontalToolBox == m_overlayActiveToolBox) {
        /*
         * With Qt5, default height of overlay toolbox at bottom is
         * way too tall. Qt 5.6 added a 'resizeDocks' method and
         * it is used to initialize the height of the overlay toolbox.
         */
        const int toolboxHeight = 150;
        QList<QDockWidget*> docks;
        docks.push_back(m_overlayHorizontalToolBox);
        QList<int> dockSizes;
        dockSizes.push_back(toolboxHeight);
        
        resizeDockWidgets(docks,
                          dockSizes,
                          Qt::Vertical);
    }
    s_brainBrowserWindows.insert(this);
    
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setSurfaceMontageHorizontalGapForWindow(m_browserWindowIndex, 0.0f);
    gapsAndMargins->setSurfaceMontageVerticalGapForWindow(m_browserWindowIndex, 0.0f);
    gapsAndMargins->setVolumeMontageHorizontalGapForWindow(m_browserWindowIndex, 0.0f);
    gapsAndMargins->setVolumeMontageVerticalGapForWindow(m_browserWindowIndex, 0.0f);
    
    /*
     * Allows keyboard events
     */
    setFocusPolicy(Qt::StrongFocus);
}

/**
 * Destructor.
 */
BrainBrowserWindow::~BrainBrowserWindow()
{
    std::unique_ptr<EventBrowserWindowContent> bwc = EventBrowserWindowContent::deleteWindowContent(m_browserWindowIndex);
    EventManager::get()->sendEvent(bwc->getPointer());
    m_browserWindowContent = NULL;
    
    EventManager::get()->removeAllEventsFromListener(this);
    
    s_brainBrowserWindows.erase(this);
    
    if (s_brainBrowserWindows.empty()) {
    }
    
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
BrainBrowserWindow::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_MENUS_UPDATE) {
        const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        if (m_developMenuAction != NULL) {
            m_developMenuAction->setVisible(prefs->isDevelopMenuEnabled());
            event->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GET_TABS) {
        EventBrowserWindowGetTabs* tabEvent = dynamic_cast<EventBrowserWindowGetTabs*>(event);
        CaretAssert(tabEvent);
        if (tabEvent->getBrowserWindowIndex() == m_browserWindowIndex) {
            std::vector<BrowserTabContent*> tabContent;
            m_toolbar->getAllTabContent(tabContent);
            for (auto tab : tabContent) {
                tabEvent->addBrowserTab(tab);
            }
            tabEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_INDICES_GET_ALL_VIEWED) {
        EventBrowserTabIndicesGetAllViewed* allViewedEvent = dynamic_cast<EventBrowserTabIndicesGetAllViewed*>(event);
        CaretAssert(allViewedEvent);
        
        std::vector<BrowserTabContent*> tabContent;
        if (isTileTabsSelected()) {
            m_toolbar->getAllTabContent(tabContent);
        }
        else {
            BrowserTabContent* btc = getBrowserTabContent();
            if (btc != NULL) {
                tabContent.push_back(btc);
            }
        }
        for (auto tab : tabContent) {
            CaretAssert(tab);
            allViewedEvent->addBrowserTabIndex(tab->getTabNumber());
        }
        allViewedEvent->setEventProcessed();
    }
    else if (event->getEventType()== EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS) {
        EventCaretMappableDataFilesAndMapsInDisplayedOverlays* filesEvent = dynamic_cast<EventCaretMappableDataFilesAndMapsInDisplayedOverlays*>(event);
        CaretAssert(filesEvent);
        
        /*
         * If true, all tabs are included even if tile tabs is off
         */
        const bool useAllTabsFlag(true);
        
        std::vector<BrowserTabContent*> tabContent;
        if (isTileTabsSelected()
            || useAllTabsFlag) {
            m_toolbar->getAllTabContent(tabContent);
        }
        else {
            BrowserTabContent* btc = m_toolbar->getTabContentFromSelectedTab();
            if (btc != NULL) {
                tabContent.push_back(btc);
            }
        }
        
        for (auto btc : tabContent) {
            btc->getFilesAndMapIndicesInOverlays(filesEvent);
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_VIEWPORT_SIZE) {
        EventGetViewportSize* viewportSizeEvent = dynamic_cast<EventGetViewportSize*>(event);
        CaretAssert(viewportSizeEvent);
        
        std::vector<const BrainOpenGLViewportContent*> allViewportContent = m_openGLWidget->getViewportContent();
        
        int32_t viewport[4] = { 0, 0, 0, 0 };
        bool viewportValid = false;
        
        int32_t notBestViewport[4] = { 0, 0, 0, 0 };
        bool notBestViewportValid = false;
        
        switch (viewportSizeEvent->getMode()) {
            case EventGetViewportSize::MODE_SPACER_TAB_INDEX:
                for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                     vpIter != allViewportContent.end();
                     vpIter++) {
                    const SpacerTabIndex requestedSpacerTabIndex = viewportSizeEvent->getSpacerTabIndex();
                    const BrainOpenGLViewportContent* vpContent = *vpIter;
                    if (vpContent != NULL) {
                        SpacerTabContent* stc = vpContent->getSpacerTabContent();
                        if (stc != NULL) {
                            if (requestedSpacerTabIndex == stc->getSpacerTabIndex()) {
                                vpContent->getTabViewportBeforeApplyingMargins(viewport);
                                viewportValid = true;
                                break;
                            }
                        }
                    }
                }
                break;
            case EventGetViewportSize::MODE_SURFACE_MONTAGE:
                if (viewportSizeEvent->getIndex() == m_browserWindowIndex) {
                    /*
                     * Find a surface montage in this window
                     */
                    for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                         vpIter != allViewportContent.end();
                         vpIter++) {
                        const BrainOpenGLViewportContent* vpContent = *vpIter;
                        if (vpContent != NULL) {
                            BrowserTabContent* btc = vpContent->getBrowserTabContent();
                            if (btc != NULL) {
                                const Model* model = btc->getModelForDisplay();
                                if (model->getModelType() == ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE) {
                                    vpContent->getTabViewportBeforeApplyingMargins(viewport);
                                    viewportValid = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            case EventGetViewportSize::MODE_TAB_BEFORE_MARGINS_INDEX:
                for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                     vpIter != allViewportContent.end();
                     vpIter++) {
                    const BrainOpenGLViewportContent* vpContent = *vpIter;
                    if (vpContent != NULL) {
                        BrowserTabContent* btc = vpContent->getBrowserTabContent();
                        if (btc != NULL) {
                            if (btc->getTabNumber() == viewportSizeEvent->getIndex()) {
                                for (std::vector<const BrainOpenGLViewportContent*>::const_iterator vpIter = allViewportContent.begin();
                                     vpIter != allViewportContent.end();
                                     vpIter++) {
                                    const BrainOpenGLViewportContent* vpContent = *vpIter;
                                    if (vpContent->getTabIndex() == viewportSizeEvent->getIndex()) {
                                        vpContent->getTabViewportBeforeApplyingMargins(viewport);
                                        viewportValid = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case EventGetViewportSize::MODE_TAB_AFTER_MARGINS_INDEX:
                for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                     vpIter != allViewportContent.end();
                     vpIter++) {
                    const BrainOpenGLViewportContent* vpContent = *vpIter;
                    if (vpContent != NULL) {
                        BrowserTabContent* btc = vpContent->getBrowserTabContent();
                        if (btc != NULL) {
                            if (btc->getTabNumber() == viewportSizeEvent->getIndex()) {
                                for (std::vector<const BrainOpenGLViewportContent*>::const_iterator vpIter = allViewportContent.begin();
                                     vpIter != allViewportContent.end();
                                     vpIter++) {
                                    const BrainOpenGLViewportContent* vpContent = *vpIter;
                                    if (vpContent->getTabIndex() == viewportSizeEvent->getIndex()) {
                                        vpContent->getModelViewport(viewport);
                                        viewportValid = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case EventGetViewportSize::MODE_VOLUME_MONTAGE:
                if (viewportSizeEvent->getIndex() == m_browserWindowIndex) {
                    /*
                     * Find the viewport content containing the specified tab by index
                     */
                    for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                         vpIter != allViewportContent.end();
                         vpIter++) {
                        const BrainOpenGLViewportContent* vpContent = *vpIter;
                        if (vpContent != NULL) {
                            BrowserTabContent* btc = vpContent->getBrowserTabContent();
                            if (btc != NULL) {
                                const Model* model = btc->getModelForDisplay();
                                if (model->getModelType() == ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES) {
                                    if (btc->getVolumeSliceDrawingType() == VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE) {
                                        vpContent->getTabViewportBeforeApplyingMargins(viewport);
                                        viewportValid = true;
                                        break;
                                    }
                                    else {
                                        vpContent->getTabViewportBeforeApplyingMargins(notBestViewport);
                                        notBestViewportValid = true;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case EventGetViewportSize::MODE_WINDOW_INDEX:
                if (m_browserWindowIndex == viewportSizeEvent->getIndex()) {
                    for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                         vpIter != allViewportContent.end();
                         vpIter++) {
                        const BrainOpenGLViewportContent* vpContent = *vpIter;
                        if (vpContent != NULL) {
                            vpContent->getWindowViewport(viewport);
                            viewportValid = true;
                        }
                    }
                }
                break;
            case EventGetViewportSize::MODE_WINDOW_FROM_TAB_INDEX:
                for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                     vpIter != allViewportContent.end();
                     vpIter++) {
                    const BrainOpenGLViewportContent* vpContent = *vpIter;
                    if (vpContent != NULL) {
                        BrowserTabContent* btc = vpContent->getBrowserTabContent();
                        if (btc != NULL) {
                            if (btc->getTabNumber() == viewportSizeEvent->getIndex()) {
                                /*
                                 * Found Tab so report Window viewport containing tab
                                 */
                                vpContent->getWindowViewport(viewport);
                                viewportValid = true;
                                break;
                            }
                        }
                    }
                }
                break;
        }

        if ( ! viewportValid) {
            if (notBestViewportValid) {
                viewport[0] = notBestViewport[0];
                viewport[1] = notBestViewport[1];
                viewport[2] = notBestViewport[2];
                viewport[3] = notBestViewport[3];
                viewportValid = true;
            }
            else {
                /*
                 * Tab is in this window but not the active tab.
                 * So, use the active tab's viewport
                 */
                if ( ! allViewportContent.empty()) {
                    CaretAssertVectorIndex(allViewportContent, 0);
                    allViewportContent[0]->getTabViewportBeforeApplyingMargins(viewport);
                    viewportValid = true;
                }
            }
        }
        
        if (viewportValid) {
            viewportSizeEvent->setViewportSize(viewport);
            viewportSizeEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_TILE_TABS_MODIFICATION) {
        EventTileTabsGridConfigurationModification* modEvent = dynamic_cast<EventTileTabsGridConfigurationModification*>(event);
        CaretAssert(modEvent);
        
        if (modEvent->getWindowIndex() == this->m_browserWindowIndex) {
            modifyTileTabsConfiguration(modEvent);
        }
    }
    else if ((event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_PAINT_SOON_ONE_WINDOW)
             || (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_PAINT_SOON_ALL_WINDOWS)) {
        /*
         * When in annotations mode, items on the Edit Menu are enabled/disabled
         * based upon the selected annotations.  While we can update the menu items
         * when the menu is about to show, that is sufficient.   The menu items
         * have shortcut keys so it is possible for the selected annotations to change
         * and the user to use a short cut key but the item on the edit menu
         * may still be disabled.  Since a change in the selected annotations is
         * followed by a graphics update event, update the enabled/disabled status
         * of items in the Edit menu when the graphics are updated so that the 
         * shortcut keys will function.
         */
        processEditMenuAboutToShow();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_PIXEL_SIZE_INFO) {
        EventBrowserWindowPixelSizeInfoEvent* pixelEvent(dynamic_cast<EventBrowserWindowPixelSizeInfoEvent*>(event));
        CaretAssert(pixelEvent);
        const QScreen* screen(NULL);
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        switch (pixelEvent->getMode()) {
            case EventBrowserWindowPixelSizeInfoEvent::Mode::WIDGET_POINTER:
                if (isAncestorOf(pixelEvent->getWidget())) {
                    screen = pixelEvent->getWidget()->screen();
                }
                break;
            case EventBrowserWindowPixelSizeInfoEvent::Mode::WINDOW_INDEX:
                screen = m_openGLWidget->screen();
                break;
        }
#else
        QList<QScreen*> allScreens(QGuiApplication::screens());
        if ( ! allScreens.empty()) {
            screen = allScreens.at(0);
        }
#endif
        if (screen != NULL) {
            if (pixelEvent->getWindowIndex() == this->getBrowserWindowIndex()) {
                pixelEvent->setPhysicalDotsPerInch(screen->physicalDotsPerInch());
                pixelEvent->setLogicalDotsPerInch(screen->logicalDotsPerInch());
                pixelEvent->setEventProcessed();
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        updateActionsForLockingAspectRatios();
    }
}

/**
 * Resize the Dock Widgets (same as QMainWindow::resizeDocks in Qt 5.7 and later)
 *
 * @param docks
 *     The dock widgets.
 * @param sizes
 *     Sizes for the dock widgets
 * @param orientation
 *     Orientation for resizing.
 */
void
BrainBrowserWindow::resizeDockWidgets(const QList<QDockWidget *> &docks,
                                      const QList<int> &sizes,
                                      Qt::Orientation orientation)
{
    CaretAssert(docks.size() == sizes.size());
#if QT_VERSION >= 0x050600
    resizeDocks(docks,
                sizes,
                orientation);
#else
    const int32_t numDocks = std::min(docks.size(),
                                      sizes.size());
    for (int32_t i = 0; i < numDocks; i++) {
        CaretAssert(docks[i]);
        BrainBrowserWindowOrientedToolBox* tb = dynamic_cast<BrainBrowserWindowOrientedToolBox*>(docks[i]);
        CaretAssert(tb);
        switch (orientation) {
            case Qt::Horizontal:
                tb->setSizeHintWidth(sizes[i]);
                break;
            case Qt::Vertical:
                tb->setSizeHintHeight(sizes[i]);
                break;
        }
    }
#endif
}

/**
 * @return True if OpenGL Context Sharing is valid among
 * multiple graphics windows.
 * Note: If there is one window, we declare sharing valid.
 */
bool
BrainBrowserWindow::isOpenGLContextSharingValid() const
{
    CaretAssert(m_openGLWidget);
    return m_openGLWidget->isOpenGLContextSharingValid();
}

/**
 * Reset the graphics window size.
 * When the window is created, the graphics is set to a reasonable size
 * by setting the minimum size of the graphics region.  If the minimum
 * size is small,
 */
void
BrainBrowserWindow::resetGraphicsWidgetMinimumSize()
{
    m_openGLWidget->setMinimumSize(100,
                                   100);
}

void
BrainBrowserWindow::setGraphicsWidgetFixedSize(const int32_t width,
                                               const int32_t height)
{
    m_openGLWidget->setFixedSize(width,
                                 height);
}

/**
 * Get the graphics widget size.
 *
 * @param viewportXOut
 *     X-coord of viewport in graphics region (may be non-zero when lock aspect applied)
 * @param viewportYOut
 *     Y-coord of viewport in graphics region (may be non-zero when lock aspect applied)
 * @param viewportWidthOut
 *     Width of the viewport into which graphics are drawn (may be less than
 *     graphicsWidthOut when lock aspect is applied)
 * @param viewportHeightOut
 *     Height of the viewport into which graphics are drawn (may be less than
 *     graphicsHeightOut when lock aspect is applied)
 * @param openGLWidgetWidthOut
 *     True width of the OpenGL Widget.
 * @param openGLWidgetHeightOut
 *     True height of the OpenGL Widget.
 * @param applyLockedAspectRatiosFlag
 *     True if locked tab or window aspect ratio should be applied for
 *     viewport but only if lock aspect is enabled by the user.
 */
void
BrainBrowserWindow::getGraphicsWidgetSize(int32_t& viewportXOut,
                                          int32_t& viewportYOut,
                                          int32_t& viewportWidthOut,
                                          int32_t& viewportHeightOut,
                                          int32_t& openGLWidgetWidthOut,
                                          int32_t& openGLWidgetHeightOut,
                                          const bool applyLockedAspectRatiosFlag) const
{
    viewportXOut = 0;
    viewportYOut = 0;
    openGLWidgetWidthOut  = m_openGLWidget->width();
    openGLWidgetHeightOut = m_openGLWidget->height();
    viewportWidthOut  = m_openGLWidget->width();
    viewportHeightOut = m_openGLWidget->height();
    
    int aspectViewport[4] = {
        0,
        0,
        openGLWidgetWidthOut,
        openGLWidgetHeightOut
    };
    
    if (isTileTabsSelected()) {
        if (isWindowAspectRatioLocked()) {
            BrainOpenGLViewportContent::adjustViewportForAspectRatio(aspectViewport,
                                                                     getAspectRatio());
        }
    }
    else {
        const BrowserTabContent* btc = getBrowserTabContent();
        if (btc != NULL) {
            if (btc->isAspectRatioLocked()) {
                BrainOpenGLViewportContent::adjustViewportForAspectRatio(aspectViewport,
                                                                         btc->getAspectRatio());
            }
            else if (isWindowAspectRatioLocked()) {
                BrainOpenGLViewportContent::adjustViewportForAspectRatio(aspectViewport,
                                                                         getAspectRatio());
            }
        }
    }
    
    if (applyLockedAspectRatiosFlag) {
        const std::vector<const BrainOpenGLViewportContent*> allViewportContent = m_openGLWidget->getViewportContent();
        if ( ! allViewportContent.empty()) {
            CaretAssertVectorIndex(allViewportContent, 0);
            int windowViewport[4];
            allViewportContent[0]->getWindowViewport(windowViewport);
            
            const float windowWidth  = windowViewport[2];
            const float windowHeight = windowViewport[3];
            
            if ((windowWidth > 0)
                && (windowHeight > 0)) {
                if ((windowWidth != viewportWidthOut)
                    || (windowHeight != viewportHeightOut)) {
                    viewportXOut      = windowViewport[0];
                    viewportYOut      = windowViewport[1];
                    viewportWidthOut  = windowWidth;
                    viewportHeightOut = windowHeight;
                }
            }
        }
    }
}



/**
 * @return True if tile tabs is selected, else false.
 */
bool
BrainBrowserWindow::isTileTabsSelected() const
{
    return m_browserWindowContent->isTileTabsEnabled();
}

/**
 * @return the index of this browser window.
 */
int32_t 
BrainBrowserWindow::getBrowserWindowIndex() const
{ 
    return m_browserWindowIndex; 
}

/**
 * Override changeEvent from QWidget
 * @param event
 */
void
BrainBrowserWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent* changeEvent(dynamic_cast<QWindowStateChangeEvent*>(event));

        const Qt::WindowStates oldWindowState((changeEvent != NULL)
                                              ? changeEvent->oldState()
                                              : Qt::WindowStates());

        const Qt::WindowStates newWindowState(QWidget::windowState());

        if (newWindowState != oldWindowState) {
            if (oldWindowState.testFlag(Qt::WindowState::WindowMaximized)) {
            }
            if (oldWindowState.testFlag(Qt::WindowState::WindowMinimized)) {
            }
            if (oldWindowState.testFlag(Qt::WindowState::WindowFullScreen)) {
                /*
                 * Exiting Full Screen - Restore toolbar and toolbox info
                 */
                restoreWindowComponentStatus(m_normalWindowComponentStatus);
            }
            if (oldWindowState.testFlag(Qt::WindowState::WindowNoState)) {
            }

            if (newWindowState.testFlag(Qt::WindowState::WindowMaximized)) {
            }
            if (newWindowState.testFlag(Qt::WindowState::WindowMinimized)) {
            }
            if (newWindowState.testFlag(Qt::WindowState::WindowFullScreen)) {
                /*
                 * Entering Full Screen, save toolbar and toolbox info
                 */
                if ( ! m_restoringSceneNoSaveWindowCompontentStatusFlag) {
                    saveWindowComponentStatus(m_normalWindowComponentStatus);
                }

                /*
                 * Hide Toolboxes in Full Screen
                 */
                processHideFeaturesToolBox();
                processHideOverlayToolBox();
            }
            if (newWindowState.testFlag(Qt::WindowState::WindowNoState)) {
            }
        }
    }
    
    m_restoringSceneNoSaveWindowCompontentStatusFlag = false;
    
    QMainWindow::changeEvent(event);
}

/**
 * Called when the window is requested to close.
 *
 * @param event
 *     CloseEvent that may or may not be accepted
 *     allowing the window to close.
 */
void 
BrainBrowserWindow::closeEvent(QCloseEvent* event)
{
    if (m_closeWithoutConfirmationFlag) {
        m_closeWithoutConfirmationFlag = false;
        event->accept();
        return;
    }
    
    /*
     * If the application is terminating, we can ignore
     * this close event.  If the user selects Exit from
     * the file menu, it displays the exit dialog so we
     * do not need to call GuiManager::allowBrainBrowserWindowToClose()
     * below.  This prevents two exit dialogs from being displayed.
     */
    if (GuiManager::get()->isApplicationTerminating()) {
        event->accept();
        return;
    }
    
    /*
     * The GuiManager may warn user about closing the 
     * window and the user may cancel closing of the window.
     */
    GuiManager* guiManager = GuiManager::get();
    if (guiManager->allowBrainBrowserWindowToClose(this,
                                                   m_toolbar->tabBar->count())) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

/**
 * Called when a key is pressed.
 *
 * @param event
 *     The key event.
 */
void
BrainBrowserWindow::keyPressEvent(QKeyEvent* event)
{
    /*
     * When there is a key press, it may take time to process such
     * as when a macro runs.  If the macro calls QApplication::processEvents(),
     * it may result in this method getting called again by Qt
     * before the macro has completed from a previous key press event.
     * This flag will ignore key press events until a macro
     * has had time to finish
     */
    if (m_keyEventProcessingFlag) {
        return;
    }
    m_keyEventProcessingFlag = true;
    
    
    bool keyEventWasProcessed = false;
    
    /*
     * Pressing the Escape Key exits full screen mode.
     */
    if (event->key() == Qt::Key_Escape) {
        if (event->modifiers() == Qt::NoModifier) {
            if (isFullScreen()) {
                processViewFullScreenSelected();
                keyEventWasProcessed = true;
            }
            else if (isMaximized()) {
                processViewMaximizedSelected();
                keyEventWasProcessed = true;
            }
        }
    }
    
    if ( ! keyEventWasProcessed) {
        keyEventWasProcessed = WuQMacroManager::instance()->runMacroWithShortCutKeyEvent(this,
                                                                                         event);
    }
    
    /*
     * According to the documentation, if the key event was not acted upon,
     * pass it on the base class implementation.
     */
    if ( ! keyEventWasProcessed) {
        QMainWindow::keyPressEvent(event);
    }
    
    m_keyEventProcessingFlag = false;
}

/**
 * Create actions for this window.
 * NOTE: This is called BEFORE the toolbar is created.
 */
void 
BrainBrowserWindow::createActionsUsedByToolBar()
{
    QIcon featuresToolBoxIcon;
    const bool featuresToolBoxIconValid = WuQtUtilities::loadIcon(":/ToolBar/features_toolbox.png", 
                                                         featuresToolBoxIcon);
    
    QIcon overlayToolBoxIcon;
    const bool overlayToolBoxIconValid = WuQtUtilities::loadIcon(":/ToolBar/overlay_toolbox.png",
                                                                  overlayToolBoxIcon);
    
    /*
     * Note: The name of a dock widget becomes its
     * name in the toggleViewAction().  So, use
     * a separate action here so that the name in 
     * the menu is as set here.
     */
    m_overlayToolBoxAction = 
    WuQtUtilities::createAction("Overlay ToolBox",
                                "Overlay ToolBox",
                                this,
                                this,
                                SLOT(processShowOverlayToolBox(bool)));
    m_overlayToolBoxAction->setCheckable(true);
    if (overlayToolBoxIconValid) {
        m_overlayToolBoxAction->setIcon(overlayToolBoxIcon);
        m_overlayToolBoxAction->setIconVisibleInMenu(false);
    }
    else {
        m_overlayToolBoxAction->setIconText("OT");
    }
    m_overlayToolBoxAction->setObjectName(m_objectNamePrefix
                                           + ":ToolBar:ShowOverlayToolBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_overlayToolBoxAction,
                                                         ("Show overlay toolbox in window " + QString::number(m_browserWindowIndex + 1)));

    /*
     * Note: The name of a dock widget becomes its
     * name in the toggleViewAction().  So, use
     * a separate action here so that the name in 
     * the menu is as set here.
     */
    m_featuresToolBoxAction = m_featuresToolBox->toggleViewAction();
    m_featuresToolBoxAction->setCheckable(true);
    QObject::connect(m_featuresToolBoxAction, SIGNAL(triggered(bool)),
                     this, SLOT(processShowFeaturesToolBox(bool)));
    if (featuresToolBoxIconValid) {
        m_featuresToolBoxAction->setIcon(featuresToolBoxIcon);
        m_featuresToolBoxAction->setIconVisibleInMenu(false);
    }
    else {
        m_featuresToolBoxAction->setIconText("LT");
    }
    m_featuresToolBoxAction->setObjectName(m_objectNamePrefix
                                           + ":ToolBar:ShowFeaturesToolBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_featuresToolBoxAction,
                                                         ("Show features toolbox in window " + QString::number(m_browserWindowIndex + 1)));
    
    m_windowMenuLockWindowAspectRatioAction = new QAction(this);
    m_windowMenuLockWindowAspectRatioAction->setCheckable(true);
    m_windowMenuLockWindowAspectRatioAction->setChecked(m_browserWindowContent->isWindowAspectLocked());
    m_windowMenuLockWindowAspectRatioAction->setText("Lock Window Aspect Ratio");
    m_windowMenuLockWindowAspectRatioAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::CTRL | Qt::Key_K));
    QObject::connect(m_windowMenuLockWindowAspectRatioAction, &QAction::triggered,
                     this, &BrainBrowserWindow::processWindowMenuLockWindowAspectRatioTriggered);

    m_windowMenuLockAllTabAspectRatioAction = new QAction(this);
    m_windowMenuLockAllTabAspectRatioAction->setCheckable(true);
    m_windowMenuLockAllTabAspectRatioAction->setChecked(false);
    m_windowMenuLockAllTabAspectRatioAction->setText("Lock All Tab Aspect Ratios");
    m_windowMenuLockAllTabAspectRatioAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_K));
    QObject::connect(m_windowMenuLockAllTabAspectRatioAction, &QAction::triggered,
                     this, &BrainBrowserWindow::processWindowMenuLockAllTabAspectRatioTriggered);
    
    const QString aspectButtonToolTipText("<html>"
                                          "Lock the aspect ratio of the window and all tabs.  "
                                          "Aspect ratios should be locked prior to creating "
                                          "annotations and remain locked while annotations are "
                                          "present."
                                          "<p>"
                                          "Advanced users can right-click (control-click on Mac) "
                                          "to manually adjust the aspect "
                                          "ratio for the selected tab or the window."
                                          "</html>");
    
    m_toolBarLockWindowAndAllTabAspectRatioAction = new QAction(this);
    m_toolBarLockWindowAndAllTabAspectRatioAction->setCheckable(true);
    m_toolBarLockWindowAndAllTabAspectRatioAction->setChecked(false);
    m_toolBarLockWindowAndAllTabAspectRatioAction->setText("Lock Aspect");
    m_toolBarLockWindowAndAllTabAspectRatioAction->setToolTip(aspectButtonToolTipText);
    QObject::connect(m_toolBarLockWindowAndAllTabAspectRatioAction, &QAction::triggered,
                     this, &BrainBrowserWindow::processToolBarLockWindowAndAllTabAspectTriggered);
    m_toolBarLockWindowAndAllTabAspectRatioAction->setObjectName(m_objectNamePrefix
                                                                 + ":ToolBar:LockAspectRatio");
    WuQMacroManager::instance()->addMacroSupportToObject(m_toolBarLockWindowAndAllTabAspectRatioAction,
                                                         ("Lock aspect ratio in window " + QString::number(m_browserWindowIndex + 1)));

    /*
     * Button for locking aspect is passed to ToolBar's constructor
     * and is displayed on the toolbar
     */
    m_toolBarLockWindowAndAllTabAspectRatioButton = new QToolButton();
    m_toolBarLockWindowAndAllTabAspectRatioButton->setDefaultAction(m_toolBarLockWindowAndAllTabAspectRatioAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_toolBarLockWindowAndAllTabAspectRatioButton);
    QObject::connect(m_toolBarLockWindowAndAllTabAspectRatioButton, &QToolButton::customContextMenuRequested,
                     this, &BrainBrowserWindow::processToolBarLockWindowAndAllTabAspectMenu);
    m_toolBarLockWindowAndAllTabAspectRatioButton->setContextMenuPolicy(Qt::CustomContextMenu);
    
    /*
     * Undo unlocking of aspect ratio
     */
    QIcon undoIcon;
    const bool undoIconValid(WuQtUtilities::loadIcon(":/ToolBar/undo.png",
                                                     undoIcon));
    m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction = new QAction();
    if (undoIconValid) {
        m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction->setIcon(undoIcon);
    }
    else {
        m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction->setText("U");
    }
    m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction->setToolTip("Relock to aspect ratios before unlocking");
    m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction->setObjectName(m_objectNamePrefix
                                                                 + ":ToolBar:UndoUnlockAspectRatio");
    QObject::connect(m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction, &QAction::triggered,
                     this, &BrainBrowserWindow::processToolBarUndoUnlockWindowAndAllTabAspectTriggered);
    WuQMacroManager::instance()->addMacroSupportToObject(m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction,
                                                         ("Undo unlock aspect ratio in window " + QString::number(m_browserWindowIndex + 1)));

    m_toolBarUndoUnlockWindowAndAllTabAspectRatioButton = new QToolButton();
    m_toolBarUndoUnlockWindowAndAllTabAspectRatioButton->setDefaultAction(m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_toolBarUndoUnlockWindowAndAllTabAspectRatioButton);
}

/**
 * Is called by toolbar when the user changes input mode to annotations.
 * 
 * @return
 *     True if it is okay to change to annotations mode, else false.
 */
bool
BrainBrowserWindow::changeInputModeToAnnotationsWarningDialog()
{
    LockAspectWarningDialog::Result result = LockAspectWarningDialog::runDialogEnterAnnotationsMode(m_browserWindowIndex);
    
    bool okFlag = true;
    
    switch (result) {
        case LockAspectWarningDialog::Result::CANCEL:
            okFlag = false;
            break;
        case LockAspectWarningDialog::Result::LOCK_ASPECT:
            processToolBarLockWindowAndAllTabAspectsRatios(true);
            break;
        case LockAspectWarningDialog::Result::NO_CHANGES:
            break;
    }
    
    return okFlag;
}

/**
 * Called when the window menu's lock window aspect is triggered by user.
 *
 * @param checked
 *    True if checked, false if unchecked
 */
void
BrainBrowserWindow::processWindowMenuLockWindowAspectRatioTriggered(bool checked)
{
    lockWindowAspectRatio(checked);
    updateActionsForLockingAspectRatios();
}

/**
 * Called when the window menu's lock all tab aspect is triggered by user
 *
 * @param checked
 *    True if checked, false if unchecked
 */
void
BrainBrowserWindow::processWindowMenuLockAllTabAspectRatioTriggered(bool checked)
{
    lockAllTabAspectRatios(checked);
    updateActionsForLockingAspectRatios();
}

/**
 * Lock the window's aspect ratio and if it transitions from 
 * unlocked to locked, set the aspect ratio.
 */
void
BrainBrowserWindow::lockWindowAspectRatio(const bool checked)
{
    const bool oldCheckedStatus = m_browserWindowContent->isWindowAspectLocked();
    m_browserWindowContent->setWindowAspectLocked(checked);
    
    if (checked) {
        if ( ! oldCheckedStatus) {
            m_browserWindowContent->setWindowAspectLockedRatio(getOpenGLWidgetAspectRatio());
        }
    }
}

/**
 * Lock the window's aspect ratio and if it transitions from
 * unlocked to locked, set the aspect ratio.
 */
void
BrainBrowserWindow::lockAllTabAspectRatios(const bool checked)
{
    m_browserWindowContent->setAllTabsInWindowAspectRatioLocked(checked);
    
    if (checked) {
        /*
         * Locking takes place in BrainOpenGLViewportContent
         * when the tab is drawn as the size of the viewport
         * is needed.
         */
    }
    else {
        /*
         * Turn off lock aspect for all tabs
         */
        std::vector<BrowserTabContent*> allTabContent;
        m_toolbar->getAllTabContent(allTabContent);
        
        for (auto tab : allTabContent) {
            tab->setAspectRatioLocked(false);
        }
    }
}

/**
 * Called when the toolbar's lock window and all tab aspect is triggered
 *
 * @param checked
 *    True if checked, false if unchecked
 */
void
BrainBrowserWindow::processToolBarLockWindowAndAllTabAspectTriggered(bool checked)
{
    if (checked) {
        std::vector<BrowserTabContent*> allTabContent;
        m_toolbar->getAllTabContent(allTabContent);
        const int32_t tabCount = static_cast<int32_t>(allTabContent.size());

        const bool lockFlag = LockAspectWarningDialog::runDialogToolBarLockAspect(this,
                                                                                  tabCount);
        if ( ! lockFlag) {
            m_toolBarLockWindowAndAllTabAspectRatioAction->setChecked(false);
            return;
        }
    }
    
    processToolBarLockWindowAndAllTabAspectsRatios(checked);
}

/**
 * Called when the toolbar's undo unlock window and all tab aspect is triggered
 */
void
BrainBrowserWindow::processToolBarUndoUnlockWindowAndAllTabAspectTriggered()
{
    const WindowTabAspectRatios windowTabAspectRatios(m_browserWindowContent->getWindowTabAspectRatios());
    if (windowTabAspectRatios.isValid()) {
        if (windowTabAspectRatios.getWindowIndex() == m_browserWindowIndex) {
            std::vector<BrowserTabContent*> allTabContent;
            m_toolbar->getAllTabContent(allTabContent);

            std::set<int32_t> currentTabs;
            for (const auto tab : allTabContent) {
                currentTabs.insert(tab->getTabNumber());
            }
            
            AString tabsChangedMessage;
            if ( ! windowTabAspectRatios.testMatchingTabs(currentTabs,
                                                          tabsChangedMessage)) {
                if ( ! WuQMessageBox::warningOkCancel(m_toolBarUndoUnlockWindowAndAllTabAspectRatioButton,
                                                      "Tabs have changed since they were locked",
                                                      tabsChangedMessage)) {
                    return;
                }
            }
            
            m_browserWindowContent->setWindowAspectLocked(true);
            m_browserWindowContent->setWindowAspectLockedRatio(windowTabAspectRatios.getWindowAspectRatio());
            
            for (auto tab : allTabContent) {
                const float aspectRatio(windowTabAspectRatios.getTabAspectRatio(tab->getTabNumber()));
                if (aspectRatio > 0.0) {
                    tab->setAspectRatio(aspectRatio);
                    tab->setAspectRatioLocked(true);
                }
            }
            
            m_browserWindowContent->setWindowTabAspectRatios(WindowTabAspectRatios());
            m_browserWindowContent->setAllTabsInWindowAspectRatioLocked(true);
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        }
    }
}

/**
 * Called when the toolbar's lock window and all tab aspect is triggered
 *
 * @param checked
 *    True if checked, false if unchecked
 */
void
BrainBrowserWindow::processToolBarLockWindowAndAllTabAspectsRatios(bool checked)
{
    /*
     * If turning off
     */
    if ( ! checked) {
        CaretAssert(m_browserWindowContent);
        WindowTabAspectRatios windowTabAspectRatios;
        windowTabAspectRatios.setWindowAspectRatio(m_browserWindowIndex,
                                                   m_browserWindowContent->getWindowAspectLockedRatio());
        
        std::vector<BrowserTabContent*> allTabs;
        getAllTabContent(allTabs);
        for (const BrowserTabContent* btc : allTabs) {
            if (btc->isAspectRatioLocked()) {
                windowTabAspectRatios.addTabAspectRatio(btc->getTabNumber(), btc->getAspectRatio());
            }
        }
        m_browserWindowContent->setWindowTabAspectRatios(windowTabAspectRatios);
    }
    lockWindowAspectRatio(checked);
    lockAllTabAspectRatios(checked);
    updateActionsForLockingAspectRatios();
}

/**
 * Called when an item is selected from the lock aspect action menu
 *
 * @param pos
 *     Location of click.
 */
void
BrainBrowserWindow::processToolBarLockWindowAndAllTabAspectMenu(const QPoint& pos)
{
    BrowserTabContent* tabContent = getBrowserTabContent();
    if (tabContent != NULL) {
        QMenu menu;
        QAction* tabAspectAction = menu.addAction("Set Selected Tab Aspect Ratio...");
        QAction* windowAspectAction = menu.addAction("Set Window Aspect Ratio...");
        
        QAction* selectedAction = menu.exec(m_toolBarLockWindowAndAllTabAspectRatioButton->parentWidget()->mapToGlobal(pos));
        
        if (selectedAction == windowAspectAction) {
            float aspectRatio = getAspectRatioFromDialog(AspectRatioMode::WINDOW,
                                                         ("Window "
                                                          + AString::number(m_browserWindowIndex + 1)
                                                          + " Aspect Ratio"),
                                                         getAspectRatio(),
                                                         this);
            if (aspectRatio > 0.0f) {
                aspectRatioDialogUpdateForWindow(aspectRatio);
            }
        }
        else if (selectedAction == tabAspectAction) {
            float aspectRatio = tabContent->getAspectRatio();
            
            if ( ! tabContent->isAspectRatioLocked()) {
                /*
                 * When aspect is NOT locked, need to get current aspect ratio
                 */
                aspectRatio = getOpenGLWidgetAspectRatio();
                
                if (isTileTabsSelected()) {
                    /*
                     * When tile tabs is enabled, find the tab to get its aspect ratio
                     */
                    std::vector<const BrainOpenGLViewportContent*> allViewportContent = m_openGLWidget->getViewportContent();
                    for (auto vc : allViewportContent) {
                        if (vc->getTabIndex() == tabContent->getTabNumber()) {
                            int viewport[4];
                            vc->getModelViewport(viewport);
                            if (viewport[3] > 0) {
                                aspectRatio = (static_cast<float>(viewport[3])
                                               / static_cast<float>(viewport[2]));
                                break;
                            }
                        }
                    }
                }
            }
            
            aspectRatio = getAspectRatioFromDialog(AspectRatioMode::TAB,
                                                   ("Tab "
                                                    + tabContent->getTabName()
                                                    + " Aspect Ratio"),
                                                   aspectRatio,
                                                   this);
            if (aspectRatio > 0.0f) {
                aspectRatioDialogUpdateForTab(aspectRatio);
            }
        }
    }
}

/**
 * Get the new aspect ratio using a dialog.
 *
 * @param aspectRatioMode
 *     Mode (tab or window)
 * @param title
 *     Title for dialog.
 * @param originalAspectRatio
 *     Default value for aspect ratio
 * @param parent
 *     Parent for the dialog.
 */
float
BrainBrowserWindow::getAspectRatioFromDialog(const AspectRatioMode aspectRatioMode,
                                             const QString& title,
                                             const float aspectRatio,
                                             QWidget* parent) const
{
    float aspectRatioOut = -1.0;
    
    WuQDataEntryDialog ded("Set Aspect Ratio",
                           parent);
    ded.setCancelButtonText("");
    QDoubleSpinBox* ratioSpinBox = ded.addDoubleSpinBox(title,
                                                        aspectRatio);
    ratioSpinBox->setSingleStep(0.01);
    ratioSpinBox->setRange(ratioSpinBox->singleStep(), 100.0);
    ratioSpinBox->setDecimals(3);
    ratioSpinBox->setKeyboardTracking(true);
    
    switch (aspectRatioMode) {
        case AspectRatioMode::TAB:
            QObject::connect(ratioSpinBox,  static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             this, &BrainBrowserWindow::aspectRatioDialogUpdateForTab);
            break;
        case AspectRatioMode::WINDOW:
            QObject::connect(ratioSpinBox,  static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             this, &BrainBrowserWindow::aspectRatioDialogUpdateForWindow);
            break;
    }
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        aspectRatioOut = ratioSpinBox->value();
    }
    
    return aspectRatioOut;
}

/**
 * Called when the user manually adjust aspect ratio for all tabs.
 *
 * @param aspectRatio
 *      New aspect ratio.
 */
void
BrainBrowserWindow::aspectRatioDialogUpdateForTab(const double aspectRatio)
{
    BrowserTabContent* tabContent = getBrowserTabContent();
    
    if (aspectRatio > 0.0f) {
        lockAllTabAspectRatios(true);
        tabContent->setAspectRatio(aspectRatio);
        tabContent->setAspectRatioLocked(true);
    }
    
    updateActionsForLockingAspectRatios();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(getBrowserWindowIndex()).getPointer());
}

/**
 * Called when the user manually adjust aspect ratio for window.
 * 
 * @param aspectRatio
 *      New aspect ratio.
 */
void
BrainBrowserWindow::aspectRatioDialogUpdateForWindow(const double aspectRatio)
{
    if (aspectRatio > 0.0f) {
        lockWindowAspectRatio(true);
        m_browserWindowContent->setWindowAspectLockedRatio(aspectRatio);
    }
    
    updateActionsForLockingAspectRatios();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(getBrowserWindowIndex()).getPointer());
}

/**
 * Update the actions for window menu's lock window aspect, window menu's lock
 * all tab aspect, and the toolbar's lock window and all tab aspect.
 */
void
BrainBrowserWindow::updateActionsForLockingAspectRatios()
{
    QSignalBlocker windowAspectBlocker(m_windowMenuLockWindowAspectRatioAction);
    m_windowMenuLockWindowAspectRatioAction->setChecked(m_browserWindowContent->isWindowAspectLocked());
    
    QSignalBlocker tabAspectBlocker(m_windowMenuLockAllTabAspectRatioAction);
    m_windowMenuLockAllTabAspectRatioAction->setChecked(m_browserWindowContent->isAllTabsInWindowAspectRatioLocked());
    
    QSignalBlocker toolbarLockAllBlocker(m_toolBarLockWindowAndAllTabAspectRatioAction);
    m_toolBarLockWindowAndAllTabAspectRatioAction->setChecked(m_browserWindowContent->isWindowAspectLocked()
                                                              && m_browserWindowContent->isAllTabsInWindowAspectRatioLocked());
    
    const bool undoValid(m_browserWindowContent->getWindowTabAspectRatios().isValid());
    m_toolBarUndoUnlockWindowAndAllTabAspectRatioAction->setEnabled(undoValid);
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(getBrowserWindowIndex()).getPointer());
}

/**
 * @return Is the aspect ratio locked?
 */
bool
BrainBrowserWindow::isWindowAspectRatioLocked() const
{
    return m_browserWindowContent->isWindowAspectLocked();
}

/**
 * @return The aspect ratio.
 */
float
BrainBrowserWindow::getAspectRatio() const
{
    if ( ! m_browserWindowContent->isWindowAspectLocked()) {
        /*
         * When aspect NOT locked, use the OpenGL widget's aspect ratio
         */
        m_browserWindowContent->setWindowAspectLockedRatio(getOpenGLWidgetAspectRatio());
    }
    
    return m_browserWindowContent->getWindowAspectLockedRatio();
}

/**
 * Show the surface properties editor dialog.
 */
void
BrainBrowserWindow::processShowSurfacePropertiesDialog()
{
    GuiManager::get()->processShowSurfacePropertiesEditorDialog(this);
}

/**
 * Show the volume properties editor dialog.
 */
void
BrainBrowserWindow::processShowVolumePropertiesDialog()
{
    GuiManager::get()->processShowVolumePropertiesEditorDialog(this);
}

/**
 * Create actions for this window.
 * NOTE: This is called AFTER the toolbar is created.
 */
void 
BrainBrowserWindow::createActions()
{
    CaretAssert(m_toolbar);
    
    GuiManager* guiManager = GuiManager::get();
    
    m_aboutWorkbenchAction =
    WuQtUtilities::createAction("About Workbench...",
                                "Information about Workbench",
                                this,
                                this,
                                SLOT(processAboutWorkbench()));
    
    m_newWindowAction =
    WuQtUtilities::createAction("New Window",
                                "Creates a new window for viewing brain models",
                                QKeySequence(Qt::CTRL | Qt::Key_N),
                                this,
                                this,
                                SLOT(processNewWindow()));
    
    m_newTabAction =
    WuQtUtilities::createAction("New Tab", 
                                "Create a new tab (window pane) in the window",
                                QKeySequence(Qt::CTRL | Qt::Key_T),
                                this,
                                this,
                                SLOT(processNewTab()));
    m_newTabAction->setObjectName(m_objectNamePrefix
                                          + ":Menu:NewTabAction");
    WuQMacroManager::instance()->addMacroSupportToObject(m_newTabAction,
                                                         ("Create new tab in Window " + QString::number(m_browserWindowIndex + 1)));

    m_duplicateTabAction =
    WuQtUtilities::createAction("Duplicate Tab",
                                "Create a new tab (window pane) that duplicates the selected tab in the window",
                                QKeySequence(Qt::CTRL | Qt::Key_D),
                                this,
                                this,
                                SLOT(processDuplicateTab()));
    m_duplicateTabAction->setObjectName(m_objectNamePrefix
                                  + ":Menu:DuplicateTabAction");
    WuQMacroManager::instance()->addMacroSupportToObject(m_duplicateTabAction,
                                                         ("Duplicate tab in Window " + QString::number(m_browserWindowIndex + 1)));


    m_reopenLastClosedTabAction =
    WuQtUtilities::createAction("Reopen Last Closed Tab",
                                "Reopen the last closed tab",
                                QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T),
                                this);
    QObject::connect(m_reopenLastClosedTabAction, &QAction::triggered,
                     this, [=] { guiManager->processReopenLastClosedTab(this); });
    m_reopenLastClosedTabAction->setObjectName(m_objectNamePrefix
                                               + ":Menu:ReopenLastClosedTabAction"); /* NOTE: No Macro support for this item */
    
    m_openOmeZarrDirectoryAction =
    WuQtUtilities::createAction("Open OME-ZARR Directory...",
                                "Open an OME-ZARR directory",
                                this,
                                this,
                                SLOT(processOmeZarrDirectoryOpen()));
    
    m_openFileAction =
    WuQtUtilities::createAction("Open File...", 
                                "Open a data file including a spec file located on the computer",
                                QKeySequence(Qt::CTRL | Qt::Key_O),
                                this,
                                this,
                                SLOT(processDataFileOpen()));
    m_openFileAction->setShortcutContext(Qt::ApplicationShortcut);
    
    m_openLocationAction = 
    WuQtUtilities::createAction("Open Location...", 
                                "Open a data file including a spec file located on a web server (http)",
                                QKeySequence(Qt::CTRL | Qt::Key_L),
                                this,
                                this,
                                SLOT(processDataFileLocationOpen()));
    m_openLocationAction->setShortcutContext(Qt::ApplicationShortcut);
    
    m_openRecentAction =
    WuQtUtilities::createAction("Open Recent...",
                                "Open a recent file or directory",
                                QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O),
                                this,
                                this,
                                SLOT(processOpenRecent()));
    
    m_manageFilesAction =
    WuQtUtilities::createAction("Save/Manage Files...", 
                                "Save and Manage Loaded Files",
                                QKeySequence(Qt::CTRL | Qt::Key_S),
                                this,
                                this,
                                SLOT(processManageSaveLoadedFiles()));
    m_manageFilesAction->setShortcutContext(Qt::ApplicationShortcut);
    
    m_closeSpecFileAction =
    WuQtUtilities::createAction("Close All Files",
                                "Close all loaded files",
                                this,
                                this,
                                SLOT(processCloseAllFiles()));
    
    m_closeTabAction =
    WuQtUtilities::createAction("Close Tab",
                                "Close the active tab (window pane) in the window",
                                QKeySequence(Qt::CTRL | Qt::Key_W),
                                this,
                                m_toolbar,
                                SLOT(closeSelectedTabFromFileMenu()));
    
    m_closeWithoutConfirmationFlag = false;
    m_closeWindowActionConfirmTitle  = "Close Window...";
    m_closeWindowActionNoConfirmTitle = "Close Window";
    m_closeWindowAction =
    WuQtUtilities::createAction(m_closeWindowActionConfirmTitle,
                                "Close the window",
                                QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W),
                                this,
                                this,
                                SLOT(processCloseWindow()));
    
    m_captureImageAction =
    WuQtUtilities::createAction("Capture Image...",
                                "Capture an Image of the windows content",
                                this,
                                this,
                                SLOT(processCaptureImage()));

    m_movieRecordingAction =
    WuQtUtilities::createAction("Movie Recording...",
                                "Record the windows content",
                                this,
                                this,
                                SLOT(processMovieRecording()));
    
    m_preferencesAction =
    WuQtUtilities::createAction("Preferences...",
                                "Edit the preferences",
                                this,
                                this,
                                SLOT(processEditPreferences()));
    
    m_exitProgramAction =
    WuQtUtilities::createAction("Exit", 
                                "Exit (quit) the program",
                                QKeySequence(Qt::CTRL | Qt::Key_Q),
                                this,
                                this,
                                SLOT(processExitProgram()));
    
    m_dataFociProjectAction =
    WuQtUtilities::createAction("Project Foci...",
                                "Project Foci to Surfaces",
                                this,
                                this,
                                SLOT(processProjectFoci()));
    
    m_dataBorderFilesSplitAction =
    WuQtUtilities::createAction("Split Multi-Structure Border File(s)...",
                                "Split Multi-Structure Border File(s",
                                this,
                                this,
                                SLOT(processSplitBorderFiles()));

    m_dataPaletteEditorDialogAction = new QAction("Palette Editor...");
    QObject::connect(m_dataPaletteEditorDialogAction, &QAction::triggered,
                     guiManager, [=]() { guiManager->processShowPaletteEditorDialog(this); } );

    m_viewFullScreenAction = WuQtUtilities::createAction("Full Screen",
                                                         "View using all of screen",
                                                         QKeySequence(Qt::CTRL | Qt::Key_F),
                                                         this);
    QObject::connect(m_viewFullScreenAction, SIGNAL(triggered()),
                     this, SLOT(processViewFullScreenSelected()));
    /*
     * "Full Screen" Fix on MacOS with Qt 5.12:
     * Without this, the menu item may disappear on MacOS
     */
    m_viewFullScreenAction->setMenuRole(QAction::NoRole);

    m_viewMaximizedAction = new QAction(this);
    m_viewMaximizedAction->setText("Maximize");
    m_viewMaximizedAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F));
    QObject::connect(m_viewMaximizedAction, &QAction::triggered,
                     this, &BrainBrowserWindow::processViewMaximizedSelected);
    /*
     * On macOS, this prevents macOS from removing the menu
     */
    m_viewMaximizedAction->setMenuRole(QAction::NoRole);

    /*
     * Note: If shortcut key is changed, also change the shortcut key
     * for the tile tabs configuration dialog menu item to match.
     */
    m_viewTileTabsAction = WuQtUtilities::createAction("Tile Tabs",
                                                       "View all tabs in a tiled layout",
                                                       QKeySequence(Qt::CTRL | Qt::Key_M),
                                                       this);
    QObject::connect(m_viewTileTabsAction, SIGNAL(triggered()),
                     this, SLOT(processViewTileTabs()));
    /*
     * Fix on MacOS with Qt 5.12:
     * Set role to 'NoRole' or else Qt may interpret
     * "Enter Tile Tabs" and "Exit Tile Tabs" as "Enter
     * Full Screen" and remove this item from the menu
     */
    m_viewTileTabsAction->setMenuRole(QAction::NoRole);

    m_viewTileTabsConfigurationDialogAction = WuQtUtilities::createAction("Edit Tile Tabs Configurations...",
                                                                          "",
                                                                          this,
                                                                          this,
                                                                          SLOT(processViewTileTabsConfigurationDialog()));
    m_viewTileTabsConfigurationDialogAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_M));

    m_viewAutomaticTileTabsConfigurationAction = new QAction("Automatic",
                                                             this);
    m_viewAutomaticTileTabsConfigurationAction->setCheckable(true);
    
    m_viewCustomTileTabsConfigurationAction = new QAction("Custom",
                                                          this);
    m_viewCustomTileTabsConfigurationAction->setCheckable(true);
    
    m_viewManualTileTabsConfigurationAction = new QAction("Manual",
                                                          this);
    m_viewManualTileTabsConfigurationAction->setCheckable(true);
    
    QActionGroup* autoCustomGroup = new QActionGroup(this);
    autoCustomGroup->setExclusive(true);
    autoCustomGroup->addAction(m_viewAutomaticTileTabsConfigurationAction);
    autoCustomGroup->addAction(m_viewCustomTileTabsConfigurationAction);
    autoCustomGroup->addAction(m_viewManualTileTabsConfigurationAction);
    QObject::connect(autoCustomGroup, &QActionGroup::triggered,
                     this, &BrainBrowserWindow::processViewTileTabsAutomaticCustomTriggered);
    
    m_gapsAndMarginsAction =
    WuQtUtilities::createAction("Gaps and Margins...",
                                "Adjust the gaps and margins",
                                this,
                                this,
                                SLOT(processGapsAndMargins()));
    
    m_nextTabAction =
    WuQtUtilities::createAction("Next Tab",
                                "Move to the next tab",
                                QKeySequence(Qt::CTRL | Qt::Key_Right),
                                this,
                                m_toolbar,
                                SLOT(nextTab()));
    
    m_previousTabAction =
    WuQtUtilities::createAction("Previous Tab",
                                "Move to the previous tab",
                                QKeySequence(Qt::CTRL | Qt::Key_Left),
                                this,
                                m_toolbar,
                                SLOT(previousTab()));
    
    m_renameSelectedTabAction =
    WuQtUtilities::createAction("Rename Selected Tab...",
                                "Change the name of the selected tab",
                                this,
                                m_toolbar,
                                SLOT(renameTab()));
    
    m_moveTabsInWindowToNewWindowsAction =
    WuQtUtilities::createAction("Move All Tabs in Current Window to New Windows",
                                "Move all but the left most tab to new windows",
                                this,
                                m_toolbar,
                                SLOT(moveTabsToNewWindows()));
    
    m_moveTabsFromAllWindowsToOneWindowAction =
    WuQtUtilities::createAction("Move All Tabs From All Windows Into Selected Window",
                                "Move all tabs from all windows into selected window",
                                this,
                                this,
                                SLOT(processMoveAllTabsToOneWindow()));
    
    m_bringAllToFrontAction =
    WuQtUtilities::createAction("Bring All To Front",
                                "Move all windows on top of other application windows",
                                this,
                                guiManager,
                                SLOT(processBringAllWindowsToFront()));
    
    m_tileWindowsAction = 
    WuQtUtilities::createAction("Tile Windows",
                                "Arrange the windows into grid so that the fill the screen",
                                this,
                                guiManager,
                                SLOT(processTileWindows()));
    
    m_informationDialogAction =
    WuQtUtilities::createAction("Information...",
                                "Show the Informaiton Window",
                                this,
                                guiManager,
                                SLOT(processShowInformationWindow()));
    
    m_helpHcpWebsiteAction =
    WuQtUtilities::createAction("Go to HCP Website...",
                                "Load the HCP Website in your computer's web browser",
                                this,
                                this,
                                SLOT(processHcpWebsiteInBrowser()));
    
    m_helpHcpUsersAction =
    WuQtUtilities::createAction("Workbench Support (Ask a Question)...",
                                "Get Workbench Support at HCP Users Group",
                                this,
                                this,
                                SLOT(processHcpUsersGroup()));

    m_helpHcpFeatureRequestAction =
    WuQtUtilities::createAction("Submit HCP Software Feature Request...",
                                "Go to HCP Feature Request Website in your computer's web browser",
                                this,
                                this,
                                SLOT(processHcpFeatureRequestWebsiteInBrowser()));
    
    m_helpWorkbenchBugReportAction =
    WuQtUtilities::createAction("Report a Workbench Bug...",
                                "Send a Workbench Bug Report",
                                this,
                                this,
                                SLOT(processReportWorkbenchBug()));
    
    m_connectToAllenDatabaseAction =
    WuQtUtilities::createAction("Allen Brain Institute Database...",
                                "Open a connection to the Allen Brain Institute Database",
                                this,
                                this,
                                SLOT(processConnectToAllenDataBase()));
    m_connectToAllenDatabaseAction->setEnabled(false);
    
    m_connectToConnectomeDatabaseAction =
    WuQtUtilities::createAction("Human Connectome Project Database...",
                                "Open a connection to the Human Connectome Project Database",
                                this,
                                this,
                                SLOT(processConnectToConnectomeDataBase()));
    m_connectToConnectomeDatabaseAction->setEnabled(false);
    
    m_developerGraphicsTimingAction =
    WuQtUtilities::createAction(("Time Graphics Update for "
                                 + AString::number(m_developerTimingIterations)
                                 + " Iterations"),
                                "Show the average time for updating the windows graphics",
                                this,
                                this,
                                SLOT(processDevelopGraphicsTiming()));
    
    m_developerGraphicsTimingDurationAction =
    WuQtUtilities::createAction(("Time Graphics Update for "
                                 + AString::number(m_developerTimingDuration, 'f', 0)
                                 + " Seconds"),
                                "Show the average time for updating the windows graphics",
                                this,
                                this,
                                SLOT(processDevelopGraphicsTimingDuration()));
    
    m_developerOpenMPTestingAction =
    WuQtUtilities::createAction("Test OpenMP...",
                                "Test OpenMP with a parallel for loop",
                                this,
                                this,
                                SLOT(processDevelopOpenMPTesting()));
    
    m_developerExportVtkFileAction =
    WuQtUtilities::createAction("Export to VTK File",
                                "Export model(s) to VTK File",
                                this,
                                this,
                                SLOT(processDevelopExportVtkFile()));
    
    m_developerCziFileTransformTestingAction =
    WuQtUtilities::createAction("Test CZI File Transforms...",
                                "Test CZI File Transformation IJK -> XYZ -> IJK",
                                this,
                                this,
                                SLOT(processDevelopCziFileTransformTesting()));
    
    m_developerOmeZarrOpenAction =
    WuQtUtilities::createAction("OME-ZARR Open...",
                                "Test OME-ZARR reading",
                                this,
                                this,
                                SLOT(processDevelopOmeZarrOpenTesting()));
}

/**
 * Create menus for this window.
 */
void 
BrainBrowserWindow::createMenus()
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();

    /*
     * Create the menu bar and add menus to it.
     */
    QMenuBar* menubar = menuBar();
    
    menubar->addMenu(createMenuFile());
    
    menubar->addMenu(createMenuEdit());
    
    menubar->addMenu(createMenuView());
    
    QMenu* dataMenu = createMenuData();
    if (dataMenu != NULL) {
        menubar->addMenu(dataMenu);
    }
    menubar->addMenu(createMenuSurface());
    QMenu* volumeMenu = createMenuVolume();
    if (volumeMenu != NULL) {
        menubar->addMenu(volumeMenu);
    }
    
    QMenu* connectMenu = createMenuConnect();
    if (connectMenu != NULL) {
        menubar->addMenu(connectMenu);
    }
    
    menubar->addMenu(new WuQMacroMenu(this,
                                      menubar));
    
    QMenu* developMenu = createMenuDevelop();
    m_developMenuAction = menubar->addMenu(developMenu);
    m_developMenuAction->setVisible(prefs->isDevelopMenuEnabled());
    
    menubar->addMenu(createMenuWindow());
    
    menubar->addMenu(createMenuHelp());
}

/**
 * @return Create and return the developer menu.
 */
QMenu*
BrainBrowserWindow::createMenuDevelop()
{
    QMenu* menu = new QMenu("Develop",
                            this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(developerMenuAboutToShow()));
    
    const bool showPaletteEditorFlag(false);
    if (showPaletteEditorFlag) {
        menu->addAction(m_dataPaletteEditorDialogAction);
    }
    const bool showExportToVTKFlag(false);
    if (showExportToVTKFlag) {
        menu->addAction(m_developerExportVtkFileAction);
    }
    
    const bool showParallelQtFlag(false);
    if (showParallelQtFlag) {
        QAction* qtParallelTestAction = new QAction("Qt Parallel");
        QObject::connect(qtParallelTestAction, &QAction::triggered,
                         this, &BrainBrowserWindow::processParallelTest);
        menu->addAction(qtParallelTestAction);
    }
    
    menu->addAction(m_developerCziFileTransformTestingAction);
    
    if ( ! menu->isEmpty()) {
        menu->addSeparator();
    }
    menu->addAction(m_developerOpenMPTestingAction);

    menu->addSeparator();
    menu->addAction(m_developerGraphicsTimingAction);
    menu->addAction(m_developerGraphicsTimingDurationAction);
    
    menu->addSeparator();
    menu->addAction(m_developerOmeZarrOpenAction);
    
    return menu;
}

/**
 * Called to test a parallel for loop using Qt
 */
void
BrainBrowserWindow::processParallelTest()
{
    WuQMessageBox::errorOk(this, "Parallel test commented out");
/*
    Will need #include <QtConcurrent>

    std::cout << "Loop started: " << std::endl;
    std::vector<int> list;
    for(int i =0 ; i < 100; i++) list.push_back(i);
    auto future = QtConcurrent::map(list, [=](const int &i) {
        std::cout << " " << i << " ";
    });
    future.waitForFinished();
    std::cout << std::endl;
    std::cout << "Loop done: " << std::endl;
 */
}

/**
 * Called when developer menu is about to show.
 */
void
BrainBrowserWindow::developerMenuAboutToShow()
{
}

/**
 * Create the file menu.
 * @return the file menu.
 */
QMenu* 
BrainBrowserWindow::createMenuFile()
{
    QMenu* menu = new QMenu("File", this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(processFileMenuAboutToShow()));

    menu->addAction(m_aboutWorkbenchAction);
    menu->addAction(m_preferencesAction);
#ifndef CARET_OS_MACOSX
    menu->addSeparator();
#endif // CARET_OS_MACOSX
    menu->addAction(m_newWindowAction);
    menu->addAction(m_newTabAction);
    menu->addAction(m_duplicateTabAction);
    menu->addAction(m_reopenLastClosedTabAction);
    menu->addSeparator();
    menu->addAction(m_openOmeZarrDirectoryAction);
    menu->addAction(m_openFileAction);
    menu->addAction(m_openRecentAction);
    menu->addMenu(new RecentSceneMenu(RecentSceneMenu::MenuLocation::FILE_MENU,
                                      this));
    menu->addAction(m_openLocationAction);
    menu->addAction(m_manageFilesAction);
    menu->addAction(m_closeSpecFileAction);
    menu->addSeparator();
    menu->addAction(m_captureImageAction);
    menu->addAction(m_movieRecordingAction);
    menu->addSeparator();
    menu->addAction(m_closeTabAction);
    menu->addAction(m_closeWindowAction);
    menu->addSeparator();
#ifndef CARET_OS_MACOSX
    menu->addSeparator();
#endif // CARET_OS_MACOSX
    menu->addAction(m_exitProgramAction);
    
    return menu;
}


/**
 * Create an item for the edit menu.
 *
 * @param editMenu
 *     The menu.
 * @param editMenuItem
 *     Enumerated by that is inserted into the edit menu.
 * @return
 *     Action that is created by adding item to menu.
 */
static QAction*
addItemToEditMenu(QMenu* editMenu,
                  const BrainBrowserWindowEditMenuItemEnum::Enum editMenuItem)
{
    QAction* action = editMenu->addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(editMenuItem));
    action->setData(static_cast<int32_t>(BrainBrowserWindowEditMenuItemEnum::toIntegerCode(editMenuItem)));
    action->setShortcut(BrainBrowserWindowEditMenuItemEnum::toShortcut(editMenuItem));
    return action;
}

/**
 * @return A new instance of the edit menu.
 */
QMenu*
BrainBrowserWindow::createMenuEdit()
{
    /*
     * Why is there a space after 'Edit'.
     * If there is not a space, the Mac will see 'Edit' and add
     * two additional menu items "Start Dictation" and 
     * "Emoji and Symbols".  Use of these menu items 
     * will sometimes cause a crash in the FTGL font code.
     */
    m_editMenu = new QMenu("Edit ");

    m_editMenuUndoAction = addItemToEditMenu(m_editMenu,
                                             BrainBrowserWindowEditMenuItemEnum::UNDO);
    m_editMenuRedoAction = addItemToEditMenu(m_editMenu,
                                             BrainBrowserWindowEditMenuItemEnum::REDO);
    
    QAction* cutAction = addItemToEditMenu(m_editMenu,
                                           BrainBrowserWindowEditMenuItemEnum::CUT);
    addItemToEditMenu(m_editMenu,
                      BrainBrowserWindowEditMenuItemEnum::COPY);
    addItemToEditMenu(m_editMenu,
                      BrainBrowserWindowEditMenuItemEnum::PASTE);
    addItemToEditMenu(m_editMenu,
                      BrainBrowserWindowEditMenuItemEnum::PASTE_SPECIAL);
    QKeySequence noKeySequence;
    addItemToEditMenu(m_editMenu,
                      BrainBrowserWindowEditMenuItemEnum::DELETER);
    
    
    QAction* deselectAllAction = NULL;
    const bool addSelectAllFlag = true;
    if (addSelectAllFlag) {
        deselectAllAction = addItemToEditMenu(m_editMenu,
                                            BrainBrowserWindowEditMenuItemEnum::DESELECT_ALL);
        addItemToEditMenu(m_editMenu,
                          BrainBrowserWindowEditMenuItemEnum::SELECT_ALL);
    }
    
    m_editMenu->insertSeparator(cutAction);
    if (deselectAllAction != NULL) {
        m_editMenu->insertSeparator(deselectAllAction);
    }
    
    QObject::connect(m_editMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processEditMenuAboutToShow()));
    QObject::connect(m_editMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processEditMenuItemTriggered(QAction*)));
    
    return m_editMenu;
}

/**
 * Gets called when an item is selected from the edit menu.
 *
 * @param action
 *     Action (menu items) that was selected.
 */
void
BrainBrowserWindow::processEditMenuItemTriggered(QAction* action)
{
    EventGetOrSetUserInputModeProcessor inputEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(inputEvent.getPointer());
    
    UserInputModeAbstract* inputProcessor = inputEvent.getUserInputProcessor();
    if (inputProcessor != NULL) {
        const int actionDataInt = action->data().toInt();
        
        bool validActionDataIntFlag = false;
        const BrainBrowserWindowEditMenuItemEnum::Enum item = BrainBrowserWindowEditMenuItemEnum::fromIntegerCode(actionDataInt,
                                                                                                                  &validActionDataIntFlag);
        if (validActionDataIntFlag) {
            inputProcessor->processEditMenuItemSelection(item);
        }
        else {
            CaretLogSevere("Invalid conversion of integer code "
                           + AString::number(actionDataInt)
                           + " to BrainBrowserWindowEditMenuItemEnum::Enum");
        }
    }
    
    /*
     * If Cut or Copy is selected and successful,
     * the menu needs to be updated so that 
     * paste will be enabled for selection 
     * via a shortcut key.
     */
    processEditMenuAboutToShow();
}

/**
 * Gets called when the edit menu is about to show.
 */
void
BrainBrowserWindow::processEditMenuAboutToShow()
{
    EventGetOrSetUserInputModeProcessor inputEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(inputEvent.getPointer());
    
    /*
     * Get edit menu items that are enabled by the input processor
     */
    std::vector<BrainBrowserWindowEditMenuItemEnum::Enum> editMenuItemsEnabled;
    UserInputModeAbstract* inputProcessor = inputEvent.getUserInputProcessor();
    AString redoMenuItemSuffix;
    AString undoMenuItemSuffix;
    AString pasteText;
    AString pasteSpecialText;
    if (inputProcessor != NULL) {
        inputProcessor->getEnabledEditMenuItems(editMenuItemsEnabled,
                                                redoMenuItemSuffix,
                                                undoMenuItemSuffix,
                                                pasteText,
                                                pasteSpecialText);
    }
    
    /*
     * Enable/Disable each of the edit menu's actions
     */
    QList<QAction*> menuActions = m_editMenu->actions();
    QListIterator<QAction*> menuActionsIterator(menuActions);
    while (menuActionsIterator.hasNext()) {
        QAction* action = menuActionsIterator.next();
        if ( ! action->isSeparator()) {
            const int actionDataInt = action->data().toInt();
            
            bool validActionDataIntFlag = false;
            const BrainBrowserWindowEditMenuItemEnum::Enum editMenuItem = BrainBrowserWindowEditMenuItemEnum::fromIntegerCode(actionDataInt,
                                                                                                                              &validActionDataIntFlag);
            action->setEnabled(false);
            if ( ! editMenuItemsEnabled.empty()) {
                if (validActionDataIntFlag) {
                    if (std::find(editMenuItemsEnabled.begin(),
                                  editMenuItemsEnabled.end(),
                                  editMenuItem) != editMenuItemsEnabled.end()) {
                        action->setEnabled(true);
                        
                        switch (editMenuItem) {
                            case BrainBrowserWindowEditMenuItemEnum::COPY:
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::CUT:
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::DELETER:
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::DESELECT_ALL:
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::PASTE:
                                if (pasteText.isEmpty()) {
                                    action->setText(BrainBrowserWindowEditMenuItemEnum::toGuiName(editMenuItem));
                                }
                                else {
                                    action->setText(pasteText);
                                }
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::PASTE_SPECIAL:
                                if (pasteSpecialText.isEmpty()) {
                                    action->setText(BrainBrowserWindowEditMenuItemEnum::toGuiName(editMenuItem));
                                }
                                else {
                                    action->setText(pasteSpecialText);
                                }
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::REDO:
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::SELECT_ALL:
                                break;
                            case BrainBrowserWindowEditMenuItemEnum::UNDO:
                                break;
                        }
                    }
                }
                else {
                    CaretLogSevere("Invalid conversion of integer code "
                                   + AString::number(actionDataInt)
                                   + " to BrainBrowserWindowEditMenuItemEnum::Enum");
                }
            }
        }
    }
    
    /*
     * Redo menu may have a suffix
     */
    AString redoMenuItemText("Redo");
    if (std::find(editMenuItemsEnabled.begin(),
                  editMenuItemsEnabled.end(),
                  BrainBrowserWindowEditMenuItemEnum::REDO) != editMenuItemsEnabled.end()) {
        if ( ! redoMenuItemSuffix.isEmpty()) {
            redoMenuItemText.append(" "
                                    + redoMenuItemSuffix);
        }
    }
    m_editMenuRedoAction->setText(redoMenuItemText);
    
    /*
     * Undo menu may have a suffix
     */
    AString undoMenuItemText("Undo");
    if (std::find(editMenuItemsEnabled.begin(),
                  editMenuItemsEnabled.end(),
                  BrainBrowserWindowEditMenuItemEnum::UNDO) != editMenuItemsEnabled.end()) {
        if ( ! undoMenuItemSuffix.isEmpty()) {
            undoMenuItemText.append(" "
                                    + undoMenuItemSuffix);
        }
    }
    m_editMenuUndoAction->setText(undoMenuItemText);
}

/**
 * Called to display the overlay toolbox.
 */
void 
BrainBrowserWindow::processShowOverlayToolBox(bool status)
{
    m_overlayActiveToolBox->setVisible(status);
    m_overlayToolBoxAction->blockSignals(true);
    m_overlayToolBoxAction->setChecked(status);
    m_overlayToolBoxAction->blockSignals(false);
    if (status) {
        m_overlayToolBoxAction->setToolTip("Hide Overlay Toolbox");
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBox().getPointer());
    }
    else {
        m_overlayToolBoxAction->setToolTip("Show Overlay Toolbox");
    }
}

/**
 * Called when visibility of horizontal overlay toolbox is changed.
 * @param visible
 *    New visibility status.
 */
void 
BrainBrowserWindow::processOverlayHorizontalToolBoxVisibilityChanged(bool visible)
{
    if (visible == false) {
        if (m_overlayActiveToolBox == m_overlayHorizontalToolBox) {
            m_overlayToolBoxAction->blockSignals(true);
            m_overlayToolBoxAction->setChecked(false);
            m_overlayToolBoxAction->blockSignals(false);
        }
    }
}

/**
 * Called when visibility of vertical overlay toolbox is changed.
 * @param visible
 *    New visibility status.
 */
void 
BrainBrowserWindow::processOverlayVerticalToolBoxVisibilityChanged(bool visible)
{
    if (visible == false) {
        if (m_overlayActiveToolBox == m_overlayVerticalToolBox) {
            m_overlayToolBoxAction->blockSignals(true);
            m_overlayToolBoxAction->setChecked(false);
            m_overlayToolBoxAction->blockSignals(false);
        }
    }
}

/**
 * Called when File Menu is about to show.
 */
void 
BrainBrowserWindow::processFileMenuAboutToShow()
{
    if (isMacOptionKeyDown()) {
        m_closeWindowAction->setText(m_closeWindowActionNoConfirmTitle);
    }
    else {
        m_closeWindowAction->setText(m_closeWindowActionConfirmTitle);
    }
    
    EventBrowserTabReopenAvailable reopenAvailableEvent;
    EventManager::get()->sendEvent(reopenAvailableEvent.getPointer());
    m_reopenLastClosedTabAction->setEnabled(reopenAvailableEvent.isReopenValid());
}

void
BrainBrowserWindow::processCloseWindow()
{
    if (m_closeWindowAction->text() == m_closeWindowActionConfirmTitle) {
        /*
         * When confirming, just use close slot and it will result
         * in confirmation dialog being displayed.
         */
        m_closeWithoutConfirmationFlag = false;
        close();
    }
    else if (m_closeWindowAction->text() == m_closeWindowActionNoConfirmTitle) {
        m_closeWithoutConfirmationFlag = true;
        close();
    }
    else {
        CaretAssert(0);
    }
}

/**
 * @return Is the Option Key down on a Mac.
 * Always returns false if not a Mac.
 */
bool
BrainBrowserWindow::isMacOptionKeyDown() const
{
    bool keyDown = false;
    
#ifdef CARET_OS_MACOSX
    keyDown = (QApplication::queryKeyboardModifiers() == Qt::AltModifier);
#endif  // CARET_OS_MACOSX
    
    return keyDown;
}

/**
 * Called when view menu is about to show.
 */
void 
BrainBrowserWindow::processViewMenuAboutToShow()
{
    if (isFullScreen()) {
        m_viewFullScreenAction->setText("Exit Full Screen");
    }
    else {
        m_viewFullScreenAction->setText("Enter Full Screen");
    }
    
    if (isMaximized()) {
        m_viewMaximizedAction->setText("Exit Maximized");
    }
    else {
        m_viewMaximizedAction->setText("Enter Maximized");
    }
    
    if (isTileTabsSelected()) {
        m_viewTileTabsAction->setText("Exit Tile Tabs");
    }
    else {
        m_viewTileTabsAction->setText("Enter Tile Tabs");
    }

    m_viewAutomaticTileTabsConfigurationAction->setText(getTileTabsConfigurationLabelText(TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID,
                                                                                          true));
    m_viewCustomTileTabsConfigurationAction->setText(getTileTabsConfigurationLabelText(TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID,
                                                                                       true));
    m_viewManualTileTabsConfigurationAction->setText(getTileTabsConfigurationLabelText(TileTabsLayoutConfigurationTypeEnum::MANUAL,
                                                                                       true));

    BrowserWindowContent* bwc = getBrowerWindowContent();
    switch (bwc->getTileTabsConfigurationMode()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            m_viewAutomaticTileTabsConfigurationAction->setChecked(true);
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            m_viewCustomTileTabsConfigurationAction->setChecked(true);
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            m_viewManualTileTabsConfigurationAction->setChecked(true);
            break;
    }
}

/**
 * @return Label for given tile tabs configuration in View Menu
 * and Tile Tabs Configuration dialog.
 *
 * @param configurationMode
 *     The configuration mode.
 * @param includeRowsAndColumnsIn
 *     Include the number of rows and columns.
 */
AString
BrainBrowserWindow::getTileTabsConfigurationLabelText(const TileTabsLayoutConfigurationTypeEnum::Enum configurationMode,
                                                      const bool includeRowsAndColumnsIn) const
{
    bool includeRowsAndColumns = includeRowsAndColumnsIn;
    AString modeLabel;
    switch (configurationMode) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            modeLabel = "Automatic Grid";
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            modeLabel = "Custom Grid";
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            modeLabel = "Manual";
            break;
    }
    
    std::vector<int32_t> windowTabIndices;
    getAllTabContentIndices(windowTabIndices);
    int32_t configRowCount(0), configColCount(0);
    
    const int32_t windowTabCount = static_cast<int32_t>(windowTabIndices.size());
    AString errorText;
    bool customGridDefaultFlag(false);
    switch (configurationMode) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            TileTabsLayoutGridConfiguration::getRowsAndColumnsForNumberOfTabs(windowTabCount,
                                                                    configRowCount,
                                                                    configColCount);
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
        {
            const TileTabsLayoutGridConfiguration* customConfig = getBrowerWindowContent()->getCustomGridTileTabsConfiguration();
            if (customConfig->isCustomDefaultFlag()) {
                customGridDefaultFlag = true;
                TileTabsLayoutGridConfiguration::getRowsAndColumnsForNumberOfTabs(windowTabCount,
                                                                                  configRowCount,
                                                                                  configColCount);
            }
            else {
                configRowCount = customConfig->getNumberOfRows();
                configColCount = customConfig->getNumberOfColumns();
                const int32_t customTabCount = (configRowCount
                                                * configColCount);
                const int32_t hiddenCount = windowTabCount - customTabCount;
                if (hiddenCount > 0) {
                    errorText = " (configuration too small for all tabs)";
                    includeRowsAndColumns = false;
                }
            }
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            includeRowsAndColumns = false;
            break;
    }
    
    AString rowsColumnsText;
    if (includeRowsAndColumns) {
        if (customGridDefaultFlag) {
            rowsColumnsText = " (Defaults to automatic grid when selected)";
        }
        else {
            rowsColumnsText = (" ("
                               + AString::number(configRowCount)
                               + " Rows, "
                               + AString::number(configColCount)
                               + " Columns)");
        }
    }
    
    const AString textLabel(modeLabel
                            + rowsColumnsText
                            + errorText);
    
    return textLabel;
}

/**
 * Modify the tile tabs configuration.
 *
 * @param modEvent
 *     Event with modification information.
 */
void
BrainBrowserWindow::modifyTileTabsConfiguration(EventTileTabsGridConfigurationModification* modEvent)
{
    CaretAssert(modEvent);
    
    if (modEvent->getWindowIndex() != m_browserWindowIndex) {
        return;
    }
    
    std::vector<const BrainOpenGLViewportContent*> vpContent;
    if (isTileTabsSelected()) {
        vpContent = m_openGLWidget->getViewportContent();
    }
    
    TileTabsGridConfigurationModifier modifier(vpContent,
                                               m_browserWindowIndex,
                                               modEvent);
    
    AString errorMessage;
    if (! modifier.run(errorMessage)) {
        modEvent->setErrorMessage(errorMessage);
        WuQMessageBox::errorOk(this, errorMessage);
    }
    
    /*
     * Update graphics and GUI
     */
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());

    modEvent->setEventProcessed();
}

/**
 * Called when automatic/custom menu item is selected.
 * 
 * @param action
 *     Action that was selected.
 */
void
BrainBrowserWindow::processViewTileTabsAutomaticCustomTriggered(QAction* action)
{
    BrowserWindowContent* bwc = getBrowerWindowContent();
    if (action == m_viewAutomaticTileTabsConfigurationAction) {
        bwc->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID);
    }
    else if (action == m_viewCustomTileTabsConfigurationAction) {
        bwc->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID);
    }
    else if (action == m_viewManualTileTabsConfigurationAction) {
        bwc->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::MANUAL);
    }
    else {
        CaretAssert(0);
    }

    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(getBrowserWindowIndex()).getPointer());
}

/**
 * Create the view menu.
 * @return the view menu.
 */
QMenu* 
BrainBrowserWindow::createMenuView()
{
    QMenu* menu = new QMenu("View", this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(processViewMenuAboutToShow()));
   
    const bool showTileTabsModeMenuFlag(false);
    QMenu* tileTabsModeMenu(NULL);
    if (showTileTabsModeMenuFlag) {
        tileTabsModeMenu = new QMenu("Tile Tabs Configuration Layout");
        tileTabsModeMenu->addAction(m_viewAutomaticTileTabsConfigurationAction);
        tileTabsModeMenu->addAction(m_viewCustomTileTabsConfigurationAction);
        tileTabsModeMenu->addAction(m_viewManualTileTabsConfigurationAction);
    }
    
    m_viewMoveFeaturesToolBoxMenu = createMenuViewMoveFeaturesToolBox();
    m_viewMoveOverlayToolBoxMenu = createMenuViewMoveOverlayToolBox();
    
    menu->addAction(m_showToolBarAction);
    menu->addMenu(m_viewMoveFeaturesToolBoxMenu);
    menu->addMenu(m_viewMoveOverlayToolBoxMenu);
    menu->addSeparator();

    menu->addAction(m_viewFullScreenAction);
    menu->addAction(m_viewMaximizedAction);
    menu->addSeparator();
    menu->addAction(m_gapsAndMarginsAction);
    menu->addSeparator();
    menu->addAction(m_viewTileTabsAction);
    menu->addAction(m_viewTileTabsConfigurationDialogAction);
    if (tileTabsModeMenu != NULL) {
        menu->addMenu(tileTabsModeMenu);
    }
    
    return menu;
}

/**
 * @return Action for tile tabs configuration dialog also used in Tile Layout toolbar
 */
QAction*
BrainBrowserWindow::getViewTileTabsConfigurationDialogAction()
{
    return m_viewTileTabsConfigurationDialogAction;
}

/**
 * @return Create and return the overlay toolbox menu.
 */
QMenu* 
BrainBrowserWindow::createMenuViewMoveFeaturesToolBox()
{
    QMenu* menu = new QMenu("Features Toolbox", this);
    
    menu->addAction("Attach to Right", this, SLOT(processMoveFeaturesToolBoxToRight()));
    menu->addAction("Detach", this, SLOT(processMoveFeaturesToolBoxToFloat()));
    menu->addAction("Hide", this, SLOT(processHideFeaturesToolBox()));
    
    return menu;
}

/**
 * @return Create and return the overlay toolbox menu.
 */
QMenu* 
BrainBrowserWindow::createMenuViewMoveOverlayToolBox()
{
    QMenu* menu = new QMenu("Overlay Toolbox", this);
    
    menu->addAction("Attach to Bottom", this, SLOT(processMoveOverlayToolBoxToBottom()));
    menu->addAction("Attach to Left", this, SLOT(processMoveOverlayToolBoxToLeft()));
    menu->addAction("Detach", this, SLOT(processMoveOverlayToolBoxToFloat()));
    menu->addAction("Hide", this, SLOT(processHideOverlayToolBox()));
    
    return menu;
}

/**
 * Create the connect menu.
 * @return the connect menu.
 */
QMenu* 
BrainBrowserWindow::createMenuConnect()
{
    QMenu* menu = new QMenu("Connect");
    
    if (m_connectToAllenDatabaseAction->isEnabled()) {
        menu->addAction(m_connectToAllenDatabaseAction);
    }
    
    if (m_connectToConnectomeDatabaseAction->isEnabled()) {
        menu->addAction(m_connectToConnectomeDatabaseAction);
    }

    /*
     * If none of the actions are enabled, the menu will be
     * empty so there is no need to display the menu.
     */
    if (menu->isEmpty()) {
        delete menu;
        menu =  NULL;
    }
    
    return menu;
}

/**
 * Create the data menu.
 * @return the data menu.
 */
QMenu* 
BrainBrowserWindow::createMenuData()
{
    QMenu* menu = new QMenu("Data", this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(processDataMenuAboutToShow()));
    
    menu->addAction(m_dataFociProjectAction);
    menu->addAction(m_dataBorderFilesSplitAction);
    
    return menu;
}

/**
 * Called when Data Menu is about to show.
 */
void
BrainBrowserWindow::processDataMenuAboutToShow()
{
    Brain* brain = GuiManager::get()->getBrain();
    bool haveFociFiles = (GuiManager::get()->getBrain()->getNumberOfFociFiles() > 0);
    m_dataFociProjectAction->setEnabled(haveFociFiles);
    
    bool haveMultiStructureBorderFiles = false;
    const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
    for (int32_t i = 0; i < numBorderFiles; i++) {
        if ( ! brain->getBorderFile(i)->isSingleStructure()) {
            haveMultiStructureBorderFiles = true;
            break;
        }
    }
    m_dataBorderFilesSplitAction->setEnabled(haveMultiStructureBorderFiles);    
}

/**
 * Create the surface menu.
 * @return the surface menu.
 */
QMenu* 
BrainBrowserWindow::createMenuSurface()
{
    QMenu* menu = new QMenu("Surface", this);
    

    QAction* infoAction = menu->addAction("Information...",
                                          this,
                                          SLOT(processSurfaceMenuInformation()));
    infoAction->setToolTip("Display information about the surface(s) in the selected tab including: "
                           "Surface Type, Number of Triangles/Vertices, and Extent.");
    
    QAction* propertiesAction = menu->addAction("Properties...",
                                                this,
                                                SLOT(processShowSurfacePropertiesDialog()));
    propertiesAction->setToolTip("Edit surface properties including opacity and default color.");
    
    QAction* primaryAnatAction = menu->addAction("Primary Anatomical...",
                                                 this,
                                                 SLOT(processSurfaceMenuPrimaryAnatomical()));
    primaryAnatAction->setToolTip("Set surfaces used for border projection, foci projection, and "
                                  "selection of surfaces for coordinate translation to and from "
                                  "volumes.");
    
    return menu;
}

/**
 * Called when Primary Anatomical is selected from the surface menu.
 */
void 
BrainBrowserWindow::processSurfaceMenuPrimaryAnatomical()
{
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numBrainStructures = brain->getNumberOfBrainStructures();
    if (numBrainStructures <= 0) {
        return;
    }
    
    WuQDataEntryDialog ded("Primary Anatomical Surfaces",
                           this);
    std::vector<SurfaceSelectionViewController*> surfaceSelectionControls;
    for (int32_t i = 0; i < numBrainStructures; i++) {
        BrainStructure* bs = brain->getBrainStructure(i);
        SurfaceSelectionViewController* ssc = ded.addSurfaceSelectionViewController(StructureEnum::toGuiName(bs->getStructure()), 
                                                                                    bs);
        ssc->setSurface(bs->getPrimaryAnatomicalSurface());
        surfaceSelectionControls.push_back(ssc);
    }
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        for (int32_t i = 0; i < numBrainStructures; i++) {
            BrainStructure* bs = brain->getBrainStructure(i);
            bs->setPrimaryAnatomicalSurface(surfaceSelectionControls[i]->getSurface());
        }
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
}

/**
 * Called when Information is selected from the surface menu.
 */
void 
BrainBrowserWindow::processSurfaceMenuInformation()
{
    BrowserTabContent* btc = getBrowserTabContent();  
    if (btc != NULL) {
        AString txt = "";
        
        Model* mdc = btc->getModelForDisplay();
        ModelSurface* mdcs = dynamic_cast<ModelSurface*>(mdc);
        if (mdcs != NULL) {
            txt += mdcs->getSurface()->getInformation();
        }
        
        ModelWholeBrain* mdcwb = dynamic_cast<ModelWholeBrain*>(mdc);
        if (mdcwb != NULL) {
            std::vector<StructureEnum::Enum> allStructures;
            StructureEnum::getAllEnums(allStructures);
            
            for (std::vector<StructureEnum::Enum>::iterator iter = allStructures.begin();
                 iter != allStructures.end();
                 iter++) {
                const Surface* surface = mdcwb->getSelectedSurface(*iter, btc->getTabNumber());
                if (surface != NULL) {
                    txt += surface->getInformation();
                    txt += "\n";
                }
            }
        }
        
        ModelSurfaceMontage* msm = dynamic_cast<ModelSurfaceMontage*>(mdc);
        if (msm != NULL) {
            std::vector<Surface*> surfaces;
            msm->getSelectedConfiguration(btc->getTabNumber())->getDisplayedSurfaces(surfaces);
            
            for (std::vector<Surface*>::iterator iter = surfaces.begin();
                 iter != surfaces.end();
                 iter++) {
                const Surface* s = *iter;
                txt += s->getInformation();
                txt += "\n";
            }
        }
        
        if (txt.isEmpty() == false) {
            WuQMessageBox::informationOk(this,
                                         txt);
        }
    }
}

/**
 * Create the volume menu.
 * @return the volume menu.
 */
QMenu* 
BrainBrowserWindow::createMenuVolume()
{
    QMenu* menu = new QMenu("Volume", this);
    
    menu->addAction("Properties...",
                    this,
                    SLOT(processShowVolumePropertiesDialog()));
    
    return menu;
}

/**
 * Create the window menu.
 * @return the window menu.
 */
QMenu* 
BrainBrowserWindow::createMenuWindow()
{
    m_moveSelectedTabToWindowMenu = new QMenu("Move Selected Tab to Window");
    QObject::connect(m_moveSelectedTabToWindowMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processMoveSelectedTabToWindowMenuAboutToBeDisplayed()));
    QObject::connect(m_moveSelectedTabToWindowMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processMoveSelectedTabToWindowMenuSelection(QAction*)));
    
    QMenu* menu = new QMenu("Window", this);

    menu->addAction(m_windowMenuLockWindowAspectRatioAction);
    menu->addAction(m_windowMenuLockAllTabAspectRatioAction);
    menu->addSeparator();
    
    menu->addAction(m_nextTabAction);
    menu->addAction(m_previousTabAction);
    menu->addAction(m_renameSelectedTabAction);
    menu->addSeparator();
    menu->addAction(m_moveTabsInWindowToNewWindowsAction);
    menu->addAction(m_moveTabsFromAllWindowsToOneWindowAction);
    menu->addMenu(m_moveSelectedTabToWindowMenu);
    menu->addSeparator();
    
    /*
     * Cannot use the Identify action since it sets the "checkable" attribute
     * and this will add checkbox and checkmark to the menu.  In addition,
     * using the identify action would also hide the help viewer if it is
     * displayed and we don't want that.
     */
    QAction* identifyAction = GuiManager::get()->getIdentifyBrainordinateDialogDisplayAction();
    menu->addAction(identifyAction->text(),
                    this, SLOT(processShowIdentifyBrainordinateDialog()));
    
    menu->addAction(m_informationDialogAction);
    menu->addAction(GuiManager::get()->getSceneDialogDisplayAction());
    menu->addSeparator();
    menu->addAction(m_bringAllToFrontAction);
    menu->addAction(m_tileWindowsAction);
    
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindow::processWindowMenuAboutToShow);
    
    return menu;
}

/**
 * Called when window window menu is about to show
 */
void
BrainBrowserWindow::processWindowMenuAboutToShow()
{
    m_informationDialogAction->setEnabled(GuiManager::get()->getInformationDisplayDialogEnabledAction()->isEnabled());
}

/**
 * Create the help menu.
 * @return the help menu.
 */
QMenu*
BrainBrowserWindow::createMenuHelp()
{
    QMenu* menu = new QMenu("Help", this);
    
    /*
     * Cannot use the Help action since it sets the "checkable" attribute
     * and this will add checkbox and checkmark to the menu.  In addition,
     * using the help action would also hide the help viewer if it is
     * dispalyed and we don't want that.
     */
    QAction* helpAction = GuiManager::get()->getHelpViewerDialogDisplayAction();
    menu->addAction(helpAction->text(),
                    this, SLOT(processShowHelpInformation()));
    menu->addAction(m_helpHcpUsersAction);
    menu->addSeparator();
    menu->addAction(m_helpHcpWebsiteAction);
    menu->addAction(m_helpWorkbenchBugReportAction);
    menu->addAction(m_helpHcpFeatureRequestAction);
    
    return menu;
}

/**
 * Called to show/hide help content.
 */
void
BrainBrowserWindow::processShowHelpInformation()
{
    /*
     * Always display the help viewer when selected from the menu.
     * Even if the help viewer is active it may be under other windows
     * so triggering it will cause it to display.
     */
    QAction* helpAction = GuiManager::get()->getHelpViewerDialogDisplayAction();
    if (helpAction->isChecked()) {
        helpAction->blockSignals(true);
        helpAction->setChecked(false);
        helpAction->blockSignals(false);
    }
    helpAction->trigger();
}

void
BrainBrowserWindow::processShowIdentifyBrainordinateDialog()
{
    /*
     * Always display the identify dialog when selected from the menu.
     * Even if the identify dialog is active it may be under other windows
     * so triggering it will cause it to display.
     */
    GuiManager::get()->showHideIdentfyBrainordinateDialog(true,
                                                          this);
}

/**
 * Time the graphics drawing.
 */
void
BrainBrowserWindow::processDevelopGraphicsTiming()
{
    ElapsedTimer et;
    et.start();
    
    for (int32_t i = 0; i < m_developerTimingIterations; i++) {
        EventManager::get()->sendEvent(EventGraphicsTimingOneWindow(m_browserWindowIndex).getPointer());
    }
    
    const float time = et.getElapsedTimeSeconds() / m_developerTimingIterations;
    const AString timeString = AString::number(time, 'f', 5);
    
    AString fpsString;
    if (time > 0.0) {
        const float fps(1.0 / time);
        fpsString = ("\nFrame per second: "
                     + AString::number(fps, 'f', 3));
    }
    const AString msg = ("Time to draw graphics (seconds): "
                         + timeString
                         + fpsString);
    
    WuQMessageBox::informationOk(this, msg);
}

/**
 * Time the graphics drawing for duration
 */
void
BrainBrowserWindow::processDevelopGraphicsTimingDuration()
{
    ElapsedTimer durationTimer;
    durationTimer.start();
    
    int32_t iterations(0);
    while (durationTimer.getElapsedTimeSeconds() < m_developerTimingDuration) {
        EventManager::get()->sendEvent(EventGraphicsTimingOneWindow(m_browserWindowIndex).getPointer());
        iterations++;
    }
    const float actualDuration(durationTimer.getElapsedTimeSeconds());
    
    if (iterations > 0) {
        const float fps(iterations / actualDuration);
        const float frameTime(actualDuration / iterations);
        
        const AString text("Frames Per Second: "
                           + AString::number(fps, 'f', 6)
                           + "\nAverage Time: "
                           + AString::number(frameTime, 'f', 6)
                           + "\nIterations: "
                           + AString::number(iterations)
                           + "\nDuration (s): "
                           + AString::number(actualDuration, 'f', 3));
        WuQMessageBox::informationOk(this,
                                     text);
    }
}

/**
 * Test OpenMP
 */
void
BrainBrowserWindow::processDevelopOpenMPTesting()
{
#ifdef _OPENMP
    AString message("<html>Maximum number of threads: "
                    + AString::number(omp_get_max_threads())
                    + "<br>This list of numbers is unlikely to be sequential if OpenMP is functioning:<br>");
    
    /*
     * Small loop run in parallel
     */
    CaretMutex mutex;
#pragma omp CARET_PARFOR
    for (int64_t i = 0; i < 100; i++) {
        CaretMutexLocker locked(&mutex);
        message.append(" "
                       + AString::number(i));
    }
    message.append("</html>");
    WuQMessageBox::informationOk(this,
                                 message);
#else
    WuQMessageBox::informationOk(this,
                                 "<html>This version of wb_view was built without OpenMP Support.</html>");
#endif
}

/**
 * Export to VTK file.
 */
void
BrainBrowserWindow::processDevelopExportVtkFile()
{
    static QString previousVtkFileName = "";
    
    BrowserTabContent* btc = getBrowserTabContent();
    if (btc != NULL) {
        const int32_t tabIndex = btc->getTabNumber();
        std::vector<SurfaceFile*> surfaceFiles;
        std::vector<const float*> surfaceFilesColoring;
        
        ModelSurface* modelSurface = btc->getDisplayedSurfaceModel();
        ModelWholeBrain* modelWholeBrain = btc->getDisplayedWholeBrainModel();
        if (modelSurface != NULL) {
            SurfaceFile* sf = modelSurface->getSurface();
            surfaceFiles.push_back(sf);
            surfaceFilesColoring.push_back(sf->getSurfaceNodeColoringRgbaForBrowserTab(tabIndex));
        }
        else if (modelWholeBrain != NULL) {
            std::vector<Surface*> wholeBrainSurfaces = modelWholeBrain->getSelectedSurfaces(tabIndex);
            
            for (std::vector<Surface*>::iterator iter = wholeBrainSurfaces.begin();
                 iter != wholeBrainSurfaces.end();
                 iter++) {
                Surface* surface = *iter;
                
                surfaceFiles.push_back(surface);
                surfaceFilesColoring.push_back(surface->getWholeBrainNodeColoringRgbaForBrowserTab(tabIndex));
            }
        }
        
        
        if (surfaceFiles.empty() == false) {
            QString vtkSurfaceFileFilter = "VTK Poly Data File (*.vtp)";
            
            CaretFileDialog cfd(CaretFileDialog::Mode::MODE_SAVE,
                                this,
                                "Export to VTK File",
                                GuiManager::get()->getBrain()->getCurrentDirectory(),
                                vtkSurfaceFileFilter);
            cfd.selectNameFilter(vtkSurfaceFileFilter);
            cfd.setAcceptMode(QFileDialog::AcceptSave);
            cfd.setFileMode(CaretFileDialog::AnyFile);
            if (previousVtkFileName.isEmpty() == false) {
                cfd.selectFile(previousVtkFileName);
            }
            
            if (cfd.exec() == CaretFileDialog::Accepted) {
                QStringList selectedFiles = cfd.selectedFiles();
                if (selectedFiles.size() > 0) {
                    const QString vtkFileName = selectedFiles[0];
                    if (vtkFileName.isEmpty() == false) {
                        try {
                            previousVtkFileName = vtkFileName;
                            
                            VtkFileExporter::writeSurfaces(surfaceFiles,
                                                           surfaceFilesColoring,
                                                           vtkFileName);
                        }
                        catch (const DataFileException& dfe) {
                            WuQMessageBox::errorOk(this,
                                                   dfe.whatString());
                        }
                    }
                }
            }
        }
        else {
            WuQMessageBox::errorOk(this, "Displayed model does not support exporting to VTK File at this time.");
        }
    }
}

/**
 * Test CZI file transformations
 */
void
BrainBrowserWindow::processDevelopCziFileTransformTesting()
{
    WuQDataEntryDialog ded("Test CZI Transforms",
                           this);
    
    QSpinBox* pixelIndexStepSpinBox = ded.addSpinBox("Pixel Index Step", 5000);
    pixelIndexStepSpinBox->setRange(1, 1000000);
    std::vector<DataFileTypeEnum::Enum> fileType { DataFileTypeEnum::CZI_IMAGE_FILE };
    CaretDataFileSelectionComboBox* comboBox = ded.addFileSelectionComboBox("CZI File", fileType);
    QCheckBox* nonLinearCheckBox = ded.addCheckBox("Include Non-Linear Part of Transformation");
    nonLinearCheckBox->setChecked(true);
    QCheckBox* verboseCheckBox = ded.addCheckBox("Verbose (print each pixel tested)");
    verboseCheckBox->setChecked(true);
    
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const CaretDataFile* dataFile = comboBox->getSelectedFile();
        if (dataFile == NULL) {
            WuQMessageBox::errorOk(this, "No CZI File selected");
            return;
        }
        
        const CziImageFile* cziFile = dataFile->castToCziImageFile();
        if (cziFile == NULL) {
            WuQMessageBox::errorOk(this, "Selected file is not a CZI File");
            return;
        }
        
        AString resultsText;
        QImage resultsImage;
        cziFile->testPixelTransforms(pixelIndexStepSpinBox->value(),
                                     nonLinearCheckBox->isChecked(),
                                     verboseCheckBox->isChecked(),
                                     resultsText,
                                     resultsImage);
        
        WuQDataEntryDialog resultsDialog("Test Results",
                                         this);
        if ( ! resultsImage.isNull()) {
            if ((resultsImage.width() > 0)
                && (resultsImage.height() > 0)) {
                resultsDialog.addImage("", resultsImage);
            }
        }
        QTextEdit* textEdit = resultsDialog.addTextEdit("", resultsText, true);
        textEdit->setFont(WuQtUtilities::getFixedFont());
        resultsDialog.setCancelButtonText("");
        resultsDialog.exec();
    }
    
    GuiManager::beep();
}

/**
 * Test opening OME-ZARR
 */
void
BrainBrowserWindow::processDevelopOmeZarrOpenTesting()
{
    AString filename("/Users/john/caret_data/ome-zarr/QM23.50.001.CX.43.01.ome.zarr");
    OmeZarrImageFile omeZarrFile;
    try {
        omeZarrFile.readFile(filename);
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
}

/**
 * Project foci.
 */
void
BrainBrowserWindow::processProjectFoci()
{    
    FociProjectionDialog fpd(this);
    fpd.exec();
}

/**
 * Split multi-structure border files.
 */
void
BrainBrowserWindow::processSplitBorderFiles()
{
    BorderFileSplitDialog dialog(this);
    dialog.exec();
}

/**
 * Called when capture image is selected.
 */
void 
BrainBrowserWindow::processCaptureImage()
{
    GuiManager::get()->processShowImageCaptureDialog(this);
}

/**
 * Called when movie recording is selected.
 */
void
BrainBrowserWindow::processMovieRecording()
{
    GuiManager::get()->processShowMovieRecordingDialog(this);
}

/**
 * Called when capture image is selected.
 */
void
BrainBrowserWindow::processGapsAndMargins()
{
    GuiManager::get()->processShowGapsAndMarginsDialog(this);
}

/**
 * Called when capture image is selected.
 */
void 
BrainBrowserWindow::processEditPreferences()
{
    GuiManager::get()->processShowPreferencesDialog(this);
}

/**
 * Called when information dialog is selected.
 */
void 
BrainBrowserWindow::processInformationDialog()
{
    GuiManager::get()->processShowInformationDisplayDialog(true);
}

/**
 * Called when close spec file is selected.
 */
void 
BrainBrowserWindow::processCloseAllFiles()
{
    if(!WuQMessageBox::warningYesNo(this,
        "<html>Are you sure you want to close all files?</html>")) return;

    Brain* brain = GuiManager::get()->getBrain();
    brain->resetBrain();
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setBackgroundAndForegroundColorsSceneOverrideMode(CaretPreferenceValueSceneOverrideModeEnum::USER_PREFERENCES);
    prefs->invalidateSceneDataValues();
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
    GuiManager::get()->closeAllOtherWindows(this);
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());    
}

/**
 * Create a new window.
 */
void 
BrainBrowserWindow::processNewWindow()
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();

    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_PAINT_SOON_ONE_WINDOW, 
                                    true);
    EventBrowserWindowNew eventNewBrowser(this, NULL);
    EventManager::get()->sendEvent(eventNewBrowser.getPointer());
    if (eventNewBrowser.isError()) {
        cursor.restoreCursor();
        QMessageBox::critical(this,
                              "",
                              eventNewBrowser.getErrorMessage());
        return;
    }
    const int32_t newWindowIndex = eventNewBrowser.getBrowserWindowCreated()->getBrowserWindowIndex();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(newWindowIndex).getPointer());
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_PAINT_SOON_ONE_WINDOW, 
                                    false);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(newWindowIndex).getPointer());
}

/**
 * Display about workbench dialog.
 */
void 
BrainBrowserWindow::processAboutWorkbench()
{
    AboutWorkbenchDialog awd(this->m_openGLWidget);
    awd.exec();
}

/**
 * Called when open location is selected.
 */
void 
BrainBrowserWindow::processDataFileLocationOpen()
{
    CaretFileRemoteDialog fileRemoteDialog(this);
    fileRemoteDialog.exec();
}

/**
 * Called to open an OME-ZARR directory
 */
void
BrainBrowserWindow::processOmeZarrDirectoryOpen()
{
    /*
     * Setup file selection dialog.
     */
    CaretFileDialog fd(CaretFileDialog::Mode::MODE_OPEN,
                       this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    AString filterName = DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::OME_ZARR_IMAGE_FILE);
    QStringList filenameFilterList;
    filenameFilterList.append(filterName);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::Directory);
//    if ( ! s_previousOpenFileDirectory.isEmpty()) {
//        FileInformation fileInfo(s_previousOpenFileDirectory);
//        if (fileInfo.exists()) {
//            fd.setDirectory(s_previousOpenFileDirectory);
//        }
//    }
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = fd.selectedFiles();
        if ( ! selectedFiles.empty()) {
            /*
             * Load the files.
             */
            std::vector<AString> filenamesVector;
            QStringListIterator nameIter(selectedFiles);
            while (nameIter.hasNext()) {
                const QString name = nameIter.next();
                filenamesVector.push_back(name);
            }
            
            std::vector<DataFileTypeEnum::Enum> dataFileTypesDummyNotUsed;
            loadFiles(this,
                      filenamesVector,
                      dataFileTypesDummyNotUsed,
                      LOAD_SPEC_FILE_WITH_DIALOG,
                      "",
                      "");
            
//            for (auto name : filenamesVector) {
//                CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
//                prefs->addToRecentFilesAndOrDirectories(name);
//            }
        }
//        s_previousOpenFileNameFilter = fd.selectedNameFilter();
//        s_previousOpenFileDirectory  = fd.directory().absolutePath();
//        s_previousOpenFileGeometry   = fd.saveGeometry();
    }

}

/**
 * Called when open data file is selected.
 */
void 
BrainBrowserWindow::processDataFileOpen()
{
    if (s_previousOpenFileNameFilter.isEmpty()) {
        s_previousOpenFileNameFilter = 
            DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::SCENE);
    }
    
    /*
     * Get all file filters.
     */
    std::vector<DataFileTypeEnum::Enum> dataFileTypes;
    DataFileTypeEnum::getAllEnums(dataFileTypes,
                                  DataFileTypeEnum::OPTIONS_NONE);
    QStringList filenameFilterList;
    filenameFilterList.append("Any File (*)");
    for (std::vector<DataFileTypeEnum::Enum>::const_iterator iter = dataFileTypes.begin();
         iter != dataFileTypes.end();
         iter++) {
//        if (*iter == DataFileTypeEnum::OME_ZARR_IMAGE_FILE) {
//            CaretLogWarning("Skipping OME-ZARR for Open Dialog filters");
//            continue;
//        }
        AString filterName = DataFileTypeEnum::toQFileDialogFilterForReading(*iter);
        filenameFilterList.append(filterName);
    }
    
    /*
     * Setup file selection dialog.
     */
    CaretFileDialog fd(CaretFileDialog::Mode::MODE_OPEN,
                       this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::ExistingFiles);
    fd.selectNameFilter(s_previousOpenFileNameFilter);
    if ( ! s_previousOpenFileDirectory.isEmpty()) {
        FileInformation fileInfo(s_previousOpenFileDirectory);
        if (fileInfo.exists()) {
            fd.setDirectory(s_previousOpenFileDirectory);
        }
    }
    
    /*
     * First time dialog is displayed, use list order.
     * Subsequent usage will use "details" or "list" from
     * whatever user last selected.  This functionality is
     * handled by the default implementation of QFileDialog.
     */
    static bool firstTimeFlag(true);
    if (firstTimeFlag) {
        firstTimeFlag = false;
        fd.setViewMode(CaretFileDialog::List);
    }
    
    if ( ! s_previousOpenFileGeometry.isEmpty()) {
        fd.restoreGeometry(s_previousOpenFileGeometry);
    }
    
    AString errorMessages;
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = fd.selectedFiles();
        if ( ! selectedFiles.empty()) {
            /*
             * Load the files.
             */
            std::vector<AString> filenamesVector;
            QStringListIterator nameIter(selectedFiles);
            while (nameIter.hasNext()) {
                const QString name = nameIter.next();
                filenamesVector.push_back(name);
            }
            
            std::vector<DataFileTypeEnum::Enum> dataFileTypesDummyNotUsed;
            loadFiles(this,
                      filenamesVector,
                      dataFileTypesDummyNotUsed,
                      LOAD_SPEC_FILE_WITH_DIALOG,
                      "",
                      "");
            
            std::vector<AString> sceneFileNames;
            for (auto name : filenamesVector) {
                CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
                prefs->addToRecentFilesAndOrDirectories(name);
                
                bool validFlag(false);
                const DataFileTypeEnum::Enum dataFileType(DataFileTypeEnum::fromName(name,
                                                                                     &validFlag));
                if (validFlag) {
                    if (dataFileType == DataFileTypeEnum::SCENE) {
                        sceneFileNames.push_back(name);
                    }
                }
            }
            
            if ( ! sceneFileNames.empty()) {
                Brain* brain(GuiManager::get()->getBrain());
                CaretAssert(brain);
                const int32_t numSceneFiles(brain->getNumberOfSceneFiles());
                if (numSceneFiles > 0) {
                    SceneFile* sceneFile(NULL);
                    /*
                     * Find scene file by name
                     */
                    for (int32_t i = 0; i < numSceneFiles; i++) {
                        sceneFile = brain->getSceneFileWithName(sceneFileNames[i]);
                        if (sceneFile != NULL) {
                            break;
                        }
                    }
                    /*
                     * If scene file not found by name, use last scene file
                     * since it should be the most recently loaded scene file
                     */
                    if (sceneFile == NULL) {
                        sceneFile = brain->getSceneFile(numSceneFiles - 1);
                    }
                    if (sceneFile != NULL) {
                        const int32_t numScenes(sceneFile->getNumberOfScenes());
                        if (numScenes > 0) {
                            /*
                             * Select the recently loaded scene file in the scene dialog
                             */
                            Scene* scene(sceneFile->getSceneAtIndex(0));
                            CaretAssert(scene);
                            const bool showSceneDialogFlag(false);
                            GuiManager::get()->processShowSceneDialogAndScene(this,
                                                                              sceneFile,
                                                                              scene,
                                                                              showSceneDialogFlag);
                        }
                    }
                }
            }
        }
        s_previousOpenFileNameFilter = fd.selectedNameFilter();
        s_previousOpenFileDirectory  = fd.directory().absolutePath();
        s_previousOpenFileGeometry   = fd.saveGeometry();
    }
}

/**
 * Process the Open Recent menu item
 */
void
BrainBrowserWindow::processOpenRecent()
{
    AString directoryOrFileName;
    int32_t sceneIndex(-1);
    const RecentFilesDialog::ResultModeEnum result = RecentFilesDialog::runDialog(RecentFilesDialog::RunMode::OPEN_RECENT,
                                                                                  directoryOrFileName,
                                                                                  sceneIndex,
                                                                                  this);
    
    switch (result) {
        case RecentFilesDialog::ResultModeEnum::CANCEL:
            break;
        case RecentFilesDialog::ResultModeEnum::LOAD_FILES_IN_SPEC_FILE:
            loadFilesFromCommandLine({ directoryOrFileName },
                                     BrainBrowserWindow::LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE);
            break;
        case RecentFilesDialog::ResultModeEnum::LOAD_SCENE_FROM_SCENE_FILE:
            loadSceneFromCommandLine(directoryOrFileName,
                                     AString::number(sceneIndex),
                                     BrainBrowserWindow::LoadSceneFromCommandLineDialogMode::SHOW_YES);
            break;
        case RecentFilesDialog::ResultModeEnum::OPEN_DIRECTORY:
            s_previousOpenFileDirectory = directoryOrFileName;
            processDataFileOpen();
            break;
        case RecentFilesDialog::ResultModeEnum::OPEN_FILE:
        {
            bool validFlag(false);
            DataFileTypeEnum::fromFileExtension(directoryOrFileName, &validFlag);
            if ( ! validFlag) {
                WuQMessageBox::errorOk(this, ("File is not a supported file type: "
                                              + directoryOrFileName));
            }
            else {
                std::vector<AString> filenames;
                filenames.push_back(directoryOrFileName);
                std::vector<DataFileTypeEnum::Enum> dataFileTypesDummyNotUsed;
                loadFiles(this,
                          filenames,
                          dataFileTypesDummyNotUsed,
                          LOAD_SPEC_FILE_WITH_DIALOG,
                          "",
                          "");
            }
        }
            break;
        case RecentFilesDialog::ResultModeEnum::OPEN_OTHER:
            processDataFileOpen();
            break;
    }
}

/**
 * Load files that are on the network
 *
 * @param parentForDialogs,
 *    Parent widget on which dialogs are displayed.
 * @param filenames
 *    List of filenames to read.
 * @param dataFileTypes
 *    Type for each data file (optional, if not available type matched from file's extension).
 * @param username
 *    Username for network file reading
 * @param password
 *    Password for network file reading
 */
bool
BrainBrowserWindow::loadFilesFromNetwork(QWidget* parentForDialogs,
                                         const std::vector<AString>& filenames,
                                         const std::vector<DataFileTypeEnum::Enum> dataFileTypes,
                                         const AString& username,
                                         const AString& password)
{    
    const bool successFlag = loadFiles(parentForDialogs,
                                       filenames,
                                       dataFileTypes,
                                       BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE,
                                       username,
                                       password);
    return successFlag;
}


/**
 * Load the files that were specified on the command line.
 * @param filenames
 *    Names of files on the command line.
 * @param loadSpecFileMode
 *    Specifies handling of SpecFiles
 */
void 
BrainBrowserWindow::loadFilesFromCommandLine(const std::vector<AString>& filenames,
                                             const LoadSpecFileMode loadSpecFileMode)
{
    std::vector<DataFileTypeEnum::Enum> dataFileTypesDummyNotUsed;
    
    AString userName;
    AString password;
    switch (loadSpecFileMode) {
        case LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE:
        {
            /*
             * Spec file might contain a file on the network so try
             * using the saved username and password.
             */
            CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
            prefs->getRemoteFileUserNameAndPassword(userName,
                                                    password);
        }
            break;
        case LOAD_SPEC_FILE_WITH_DIALOG:
            break;
        case LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE:
            break;
    }
    
    loadFiles(this,
              filenames,
              dataFileTypesDummyNotUsed,
              loadSpecFileMode,
              userName,
              password);
    
    for (auto name : filenames) {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->addToRecentFilesAndOrDirectories(name);
    }

}

/**
 * Load a recent scene from the scene file
 */
void
BrainBrowserWindow::loadRecentScene(const AString& sceneFileName,
                                    const AString& sceneName)
{
    /*
     * Scene file may already be loaded
     */
    Brain* brain = GuiManager::get()->getBrain();
    SceneFile* sceneFile(brain->getSceneFileWithName(sceneFileName));
    if (sceneFile == NULL) {
        /*
         * Load the scene file
         */
        EventDataFileRead readFileEvent(brain);
        readFileEvent.addDataFile(DataFileTypeEnum::SCENE,
                                  sceneFileName);
        EventManager::get()->sendEvent(readFileEvent.getPointer());
        
        const int32_t fileIndex(0);
        if (readFileEvent.getEventProcessCount() > 0) {
            if (readFileEvent.isFileError(fileIndex)) {
                WuQMessageBox::errorOk(this,
                                       readFileEvent.getFileErrorMessage(fileIndex));
                return;
            }
        }
        
        CaretDataFile* caretDataFile(readFileEvent.getDataFileRead(fileIndex));
        if (caretDataFile != NULL) {
            sceneFile = dynamic_cast<SceneFile*>(caretDataFile);
        }
    }
    
    if (sceneFile == NULL) {
        WuQMessageBox::errorOk(this,
                               ("Failed to read scene file " + sceneFileName));
        return;
    }
    
    Scene* scene = sceneFile->getSceneWithName(sceneName);
    if (scene == NULL) {
        WuQMessageBox::errorOk(this,
                               ("Scene \""
                                + sceneName
                                + "\" not found in scene file: "
                                + sceneFileName));
        return;
    }

    ProgressReportingDialog progressDialog("Loading " + sceneName,
                                           "Initializing",
                                           this);
    
    const bool showSceneDialogFlag(false);
    GuiManager::get()->processShowSceneDialogAndScene(this,
                                                      sceneFile,
                                                      scene,
                                                      showSceneDialogFlag);
}

/**
 * Load the scene file and the scene with the given name or number
 * @param sceneFileName
 *    Name of scene file.
 * @param sceneNameOrNumber
 *    Name or number of scene.  Name takes precedence over number. 
 *    Scene numbers start at one.
 * @param sceneDialogMode
 *    Mode for showing/closing scene dialog after scene loads
 */
void
BrainBrowserWindow::loadSceneFromCommandLine(const AString& sceneFileName,
                                             const AString& sceneNameOrNumber,
                                             const LoadSceneFromCommandLineDialogMode sceneDialogMode)
{
    std::vector<AString> filenames;
    filenames.push_back(sceneFileName);
    
    loadFilesFromCommandLine(filenames,
                             LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE);
    
    bool haveSceneFileError = true;
    bool haveSceneError = true;
    FileInformation fileInfo(sceneFileName);
    const AString nameNoExt = fileInfo.getFileName();
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numSceneFiles = brain->getNumberOfSceneFiles();
    for (int32_t i = 0; i < numSceneFiles; i++) {
        SceneFile* sf = brain->getSceneFile(i);
        FileInformation fileInfo(sceneFileName);
        if (sf->getFileName().contains(fileInfo.getFileName())) {
            haveSceneFileError = false;
            Scene* scene = sf->getSceneWithName(sceneNameOrNumber);
            if (scene == NULL) {
                bool isValidNumber = false;
                int sceneNumber = sceneNameOrNumber.toInt(&isValidNumber);
                if (isValidNumber) {
                    sceneNumber--;  /* convert to index (numbers start at one) */
                    if ((sceneNumber >= 0)
                        && (sceneNumber < sf->getNumberOfScenes())) {
                        scene = sf->getSceneAtIndex(sceneNumber);
                    }
                }
            }
            
            if (scene != NULL) {
                bool showSceneDialogFlag(false);
                switch (sceneDialogMode) {
                    case LoadSceneFromCommandLineDialogMode::SHOW_NO:
                        break;
                    case LoadSceneFromCommandLineDialogMode::SHOW_YES:
                        showSceneDialogFlag = true;
                        break;
                }
                GuiManager::get()->processShowSceneDialogAndScene(this,
                                                                  sf,
                                                                  scene,
                                                                  showSceneDialogFlag);
                haveSceneError = false;
                break;
            }
        }
    }
    
    if (haveSceneFileError) {
        const AString msg = ("No scene file named \""
                             + sceneFileName
                             + "\" was loaded.");
        WuQMessageBox::errorOk(this, msg);
    }
    else if (haveSceneError) {
        const AString msg = ("No scene with name/number \""
                             + sceneNameOrNumber
                             + "\" found in scene file.");
        WuQMessageBox::errorOk(this, msg);
    }
    
    /* NOTE: File warning dialog is performed by scene dialog */
}

/**
 * Load the given directory in the Open Data File Dialog
 */
void
BrainBrowserWindow::loadDirectoryFromCommandLine(const AString& directoryName)
{
    s_previousOpenFileDirectory = directoryName;
    processDataFileOpen();
}

/**
 * Load data files.  If there are errors, an error message dialog
 * will be displayed.
 *
 * @param parentForDialogs
 *    Parent widget for any dialogs (if NULL this window is used).
 * @param filenames
 *    Names of files.
 * @param dataFileTypes
 *    Type for each filename (optional).  If the type is not present for a 
 *    filename, the type is inferred from the filename's extension.
 * @param loadSpecFileMode
 *    Specifies handling of SpecFiles
 * @param username
 *    Username for network file reading
 * @param password
 *    Password for network file reading
 * @return true if there are no errors, else false.
 */
bool
BrainBrowserWindow::loadFiles(QWidget* parentForDialogs,
                              const std::vector<AString>& filenames,
                              const std::vector<DataFileTypeEnum::Enum> dataFileTypes,
                              const LoadSpecFileMode loadSpecFileMode,
                              const AString& username,
                              const AString& password)
{
    QWidget* parentWidget = parentForDialogs;
    if (parentWidget == NULL) {
        parentWidget = this;
    }
    
    /*
     * Pick out specific file types.
     */
    AString specFileName;
    std::vector<AString> volumeFileNames;
    std::vector<AString> surfaceFileNames;
    std::vector<AString> otherFileNames;
    std::vector<DataFileTypeEnum::Enum> otherDataFileTypes;
    
    AString typeErrorMessage = "";
    
    const int32_t numFiles = static_cast<int32_t>(filenames.size());
    const int32_t numDataTypes = static_cast<int32_t>(dataFileTypes.size());
    for (int32_t i = 0; i < numFiles; i++) {
        const AString name = filenames[i];
        bool isValidType = false;
        DataFileTypeEnum::Enum fileType = DataFileTypeEnum::UNKNOWN;
        if (i < numDataTypes) {
            fileType = dataFileTypes[i];
            isValidType = true;
        }
        else {
            fileType = DataFileTypeEnum::fromFileExtension(name,
                                                           &isValidType);
            if (isValidType == false) {
                typeErrorMessage.appendWithNewLine("Extension for "
                                                   + name
                                                   + " does not match a suppported file type");
            }
        }
        
        switch (fileType) {
            case DataFileTypeEnum::SPECIFICATION:
                if (specFileName.isEmpty() == false) {
                    QMessageBox::critical(parentWidget,
                                          "ERROR",
                                          "More than one spec file cannot be loaded");
                    return false;
                }
                specFileName = name;
                break;
            case DataFileTypeEnum::SURFACE:
                surfaceFileNames.push_back(name);
                break;
            case DataFileTypeEnum::VOLUME:
                volumeFileNames.push_back(name);
                break;
            default:
                otherFileNames.push_back(name);
                otherDataFileTypes.push_back(fileType);
                break;
        }
    }
    
    if (typeErrorMessage.isEmpty() == false) {
        QMessageBox::critical(parentWidget,
                              "ERROR",
                              typeErrorMessage);
        return false;
    }
    
    /*
     * Load files in this order:
     * (1) Spec File - Limit to one.
     * (2) Volume File
     * (3) Surface File
     * (4) All other files.
     */
    std::vector<std::pair<AString, DataFileTypeEnum::Enum> > filesToLoad;
    const int32_t numVolumeFiles = static_cast<int32_t>(volumeFileNames.size());
    for (int32_t i = 0; i < numVolumeFiles; i++) {
        filesToLoad.push_back(std::make_pair(volumeFileNames[i],
                                             DataFileTypeEnum::VOLUME));
    }

    const int32_t numSurfaceFiles = static_cast<int32_t>(surfaceFileNames.size());
    for (int32_t i = 0; i < numSurfaceFiles; i++) {
        filesToLoad.push_back(std::make_pair(surfaceFileNames[i],
                                             DataFileTypeEnum::SURFACE));
    }

    const int32_t numOtherFiles = static_cast<int32_t>(otherFileNames.size());
    for (int32_t i = 0; i < numOtherFiles; i++) {
        filesToLoad.push_back(std::make_pair(otherFileNames[i],
                                             otherDataFileTypes[i]));
    }
                           

    bool createDefaultTabsFlag = false;
    
    /*
     * If there are no models loaded, will want to create default tabs.
     */
    EventModelGetAll modelGetAllEvent;
    EventManager::get()->sendEvent(modelGetAllEvent.getPointer());
    const int32_t numberOfModels = static_cast<int32_t>(modelGetAllEvent.getModels().size());
    if (numberOfModels <= 0) {
        createDefaultTabsFlag = true;
    }
    
    AString errorMessages;
    
    ElapsedTimer timer;
    timer.start();
    float specFileTimeStart = 0.0;
    float specFileTimeEnd   = 0.0;
    bool sceneFileWasLoaded = false;
    
    /*
     * Load spec file (before data files)
     */
    if (specFileName.isEmpty() == false) {
        SpecFile specFile;
        try {
            FileInformation fileInfo(specFileName);
            if (fileInfo.isRelative()) {
                specFileName = fileInfo.getAbsoluteFilePath();
            }
            if (fileInfo.exists()) {
                SessionManager::get()->getCaretPreferences()->addToRecentFilesAndOrDirectories(specFileName);
            }
            specFile.readFile(specFileName);
            
        }
        catch (const DataFileException& e) {
            errorMessages += e.whatString();
            QMessageBox::critical(parentWidget,
                                  "ERROR",
                                  errorMessages);
            return false;
        }
        
        switch (loadSpecFileMode) {
            case LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE:
            {
                timer.reset(); /* resets timer */
                specFileTimeStart = timer.getElapsedTimeSeconds();
                
                /*
                 * Load all files listed in spec file
                 */
                specFile.setAllFilesSelectedForLoading(true);
                
                EventSpecFileReadDataFiles readSpecFileEvent(GuiManager::get()->getBrain(),
                                                             &specFile);
                if (username.isEmpty() == false) {
                    readSpecFileEvent.setUsernameAndPassword(username,
                                                             password);
                }
                
                ProgressReportingDialog::runEvent(&readSpecFileEvent,
                                                  parentWidget,
                                                  specFile.getFileNameNoPath());
                
                if (readSpecFileEvent.isError()) {
                    if (errorMessages.isEmpty() == false) {
                        errorMessages += "\n";
                    }
                    errorMessages += readSpecFileEvent.getErrorMessage();
                }
                specFileTimeEnd = timer.getElapsedTimeSeconds();
                createDefaultTabsFlag = true;
            }
                break;
            case LOAD_SPEC_FILE_WITH_DIALOG:
            case LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE:
            {
                if (GuiManager::get()->processShowOpenSpecFileDialog(&specFile,
                                                                     this)) {
                    m_toolbar->addDefaultTabsAfterLoadingSpecFile();
                    createDefaultTabsFlag = true;
                }
            }
                break;
        }
        
        sceneFileWasLoaded = specFile.areAllFilesSelectedForLoadingSceneFiles();
    }
    
    /*
     * Prepare to load any data files
     */
    EventDataFileRead loadFilesEvent(GuiManager::get()->getBrain());
    if (username.isEmpty() == false) {
        loadFilesEvent.setUsernameAndPassword(username,
                                             password);
    }
    
    /*
     * Add data files to data file loading event (after loading spec file)
     */
    const int32_t numFilesToLoad = static_cast<int32_t>(filesToLoad.size());
    for (int32_t i = 0; i < numFilesToLoad; i++) {
        AString name = filesToLoad[i].first;
        const DataFileTypeEnum::Enum fileType = filesToLoad[i].second;
        
        loadFilesEvent.addDataFile(fileType,
                                   name);
        
    }
    
    /*
     * Now, load the data files
     */
    const int32_t numberOfValidFiles = loadFilesEvent.getNumberOfDataFilesToRead();
    if (numberOfValidFiles > 0) {
        ProgressReportingDialog::runEvent(&loadFilesEvent,
                                          parentWidget,
                                          "Loading Data Files");
        errorMessages.appendWithNewLine(loadFilesEvent.getErrorMessage());
        
        /*
         * Check for errors
         */
        for (int32_t i = 0; i < numberOfValidFiles; i++) {
            const AString& dataFileName = loadFilesEvent.getDataFileName(i);
            const DataFileTypeEnum::Enum dataFileType = loadFilesEvent.getDataFileType(i);
            
            const AString shortName = FileInformation(dataFileName).getFileName();
            
            if (loadFilesEvent.isFileErrorInvalidStructure(i)) {
                /*
                 * Allow user to specify the structure
                 */
                WuQDataEntryDialog ded("Structure",
                                       parentWidget);
                StructureEnumComboBox* ssc = ded.addStructureEnumComboBox("");
                ded.setTextAtTop(("File \""
                                  + shortName
                                  + "\"\nhas missing or invalid structure, select it's structure."
                                  "\nAfter loading, save file with File Menu->Save Manage Files"
                                  "\nto prevent this error."),
                                 false);
                if (ded.exec() == WuQDataEntryDialog::Accepted) {
                    EventDataFileRead loadFileEventStructure(GuiManager::get()->getBrain());
                    loadFileEventStructure.addDataFile(ssc->getSelectedStructure(),
                                                       dataFileType,
                                                       dataFileName);
                    if (username.isEmpty() == false) {
                        loadFileEventStructure.setUsernameAndPassword(username,
                                                                      password);
                    }
                    
                    ProgressReportingDialog::runEvent(&loadFileEventStructure,
                                                      parentWidget,
                                                      ("Loading " + shortName));
                    if (loadFileEventStructure.isError()) {
                        errorMessages.appendWithNewLine(loadFileEventStructure.getErrorMessage());
                    }
                    else {
                        if (loadFileEventStructure.getNumberOfDataFilesToRead() == 1) {
                            CaretDataFile* cdf = loadFileEventStructure.getDataFileRead(0);
                            if (cdf != NULL) {
                                cdf->setModified();
                            }
                        }
                    }
                }
                else {
                    errorMessages.appendWithNewLine("File \""
                                                       + shortName
                                                       + "\" not loaded due to invalid structure.");
                }
            }
            else if (loadFilesEvent.isFileError(i) == false) {
                if (dataFileType == DataFileTypeEnum::SCENE) {
                    sceneFileWasLoaded = true;
                }                
            }
        }
    }
    
    const float specFileTime = specFileTimeEnd - specFileTimeStart;
    
    const float createTabsStartTime = timer.getElapsedTimeSeconds();
    
    const EventBrowserWindowCreateTabs::Mode tabMode = (createDefaultTabsFlag ?
                                                        EventBrowserWindowCreateTabs::MODE_LOADED_SPEC_FILE :
                                                        EventBrowserWindowCreateTabs::MODE_LOADED_DATA_FILE);
    EventBrowserWindowCreateTabs createTabsEvent(tabMode);
    EventManager::get()->sendEvent(createTabsEvent.getPointer());
    
    const float createTabsTime = timer.getElapsedTimeSeconds() - createTabsStartTime;
    
    const float guiStartTime = timer.getElapsedTimeSeconds();
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    const float guiTime = timer.getElapsedTimeSeconds() - guiStartTime;
    
    if (specFileName.isEmpty() == false) {
        CaretLogInfo("Time to read files from spec file (in GUI) \""
                     + FileInformation(specFileName).getFileName()
                     + "\" was "
                     + AString::number(timer.getElapsedTimeSeconds())
                     + " seconds.\n  Time to read spec data files was "
                     + AString::number(specFileTime)
                     + " seconds.\n  Time to update GUI was "
                     + AString::number(guiTime)
                     + " seconds.\n  Time to create tabs was "
                     + AString::number(createTabsTime));
    }
    
    bool successFlag = true;
    if (errorMessages.isEmpty() == false) {
        WuQMessageBoxTwo::critical(parentWidget,
                                   "ERROR",
                                   errorMessages);
        successFlag = false;
    }
    
    if (sceneFileWasLoaded) {
        GuiManager::get()->processShowSceneDialog(this);
    }
    
    showDataFileReadWarningsDialog();
    
    return successFlag;
}

/**
 * Called when manage/save loaded files is selected.
 */
void 
BrainBrowserWindow::processManageSaveLoadedFiles()
{
    GuiManager::get()->processShowSaveManageFilesDialog(this);
}

/**
 * Exit the program.
 */
void 
BrainBrowserWindow::processExitProgram()
{
    GuiManager::get()->exitProgram(this);
}

/**
 * Update full screen status.
 *
 * @param showFullScreenDisplay
 *    If true, show as full screen, else show as normal screen
 * @param saveRestoreWindowStatus
 *    If true, save/restore the window status
 */
void
BrainBrowserWindow::processViewFullScreen(bool showFullScreenDisplay,
                                          const bool saveRestoreWindowStatus)
{
    if (showFullScreenDisplay == false) {
        EventManager::get()->blockEvent(EventTypeEnum::EVENT_USER_INTERFACE_UPDATE, true);
        showNormal();
        if (saveRestoreWindowStatus) {
            restoreWindowComponentStatus(m_normalWindowComponentStatus);
        }
        EventManager::get()->blockEvent(EventTypeEnum::EVENT_USER_INTERFACE_UPDATE, false);
    }
    else {
        if (saveRestoreWindowStatus) {
            saveWindowComponentStatus(m_normalWindowComponentStatus);
        }
        
        /*
         * Hide and disable Toolbar, Overlay, and Features ToolBox
         */
        m_showToolBarAction->setChecked(true);
        m_showToolBarAction->trigger();
        m_overlayToolBoxAction->setChecked(true);
        m_overlayToolBoxAction->trigger();
        m_featuresToolBoxAction->setChecked(true);
        m_featuresToolBoxAction->trigger();
        
        showFullScreen();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBar().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(m_browserWindowIndex).getPointer());
}


/**
 * Called when view full screen is selected and toggles the status of full screen.
 */
void
BrainBrowserWindow::processViewFullScreenSelected()
{
    if (isFullScreen()) {
        showNormal();
    }
    else {
        showFullScreen();
    }
}

/**
 * Called when view maximized is selected and toggles the status of full screen.
 */
void
BrainBrowserWindow::processViewMaximizedSelected()
{
    if (isMaximized()) {
        showNormal();
    }
    else {
        showMaximized();
    }
}

/**
 * Called when tile tabs is selected and toggles the state of tile tabs.
 */
void
BrainBrowserWindow::processViewTileTabs()
{
    const bool toggledStatus = (! isTileTabsSelected());
    
    setViewTileTabs(toggledStatus);
}

/**
 * Set the status of tile tabs.
 */
void
BrainBrowserWindow::setViewTileTabs(const bool newStatus)
{
    m_browserWindowContent->setTileTabsEnabled(newStatus);
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBar().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * View the Tile Tabs Configuration Dialog.
 */
void
BrainBrowserWindow::processViewTileTabsConfigurationDialog()
{
    GuiManager::get()->processShowTileTabsConfigurationDialog(this);
}

/**
 * Print the status of window components.
 * @param wcs
 *    Window component status that is restored.
 */
void
BrainBrowserWindow::printWindowComponentStatus(const QString& modeText,
                                               const WindowComponentStatus& wcs)
{
    std::cout << modeText << " " << wcs.name
    << " toolbar=" << AString::fromBool(wcs.isToolBarDisplayed)
    << " overlay tb=" << AString::fromBool(wcs.isOverlayToolBoxDisplayed)
    << " features tb=" << AString::fromBool(wcs.isFeaturesToolBoxDisplayed)
    << std::endl;
}

/**
 * Restore the status of window components.
 * @param wcs
 *    Window component status that is restored.
 */
void 
BrainBrowserWindow::restoreWindowComponentStatus(const WindowComponentStatus& wcs)
{
    /*printWindowComponentStatus("Restoring", wcs);*/
    
    if (wcs.windowState.isEmpty() == false) {
        restoreState(wcs.windowState);
    }
    
    if (wcs.isToolBarDisplayed) {
        m_showToolBarAction->setChecked(false);
        m_showToolBarAction->trigger();
    }
    else {
        m_showToolBarAction->setChecked(true);
        m_showToolBarAction->trigger();
    }
    
    if (wcs.isOverlayToolBoxDisplayed) {
        m_overlayToolBoxAction->blockSignals(true);
        m_overlayToolBoxAction->setChecked(false);
        m_overlayToolBoxAction->blockSignals(false);
        m_overlayToolBoxAction->trigger();
    }
    else {
        m_overlayToolBoxAction->blockSignals(true);
        m_overlayToolBoxAction->setChecked(true);
        m_overlayToolBoxAction->blockSignals(false);
        m_overlayToolBoxAction->trigger();
    }
    
    if (wcs.isFeaturesToolBoxDisplayed) {
        m_featuresToolBoxAction->blockSignals(true);
        m_featuresToolBoxAction->setChecked(false);
        m_featuresToolBoxAction->blockSignals(false);
        m_featuresToolBoxAction->trigger();
    }
    else {
        m_featuresToolBoxAction->blockSignals(true);
        m_featuresToolBoxAction->setChecked(true);
        m_featuresToolBoxAction->blockSignals(false);
        m_featuresToolBoxAction->trigger();
    }
    if (m_featuresToolBox != NULL) {
        if (wcs.featuresGeometry.isEmpty() == false) {
            m_featuresToolBox->restoreGeometry(wcs.featuresGeometry);
        }
    }
}

/**
 * Save the status of window components.
 * @param wcs
 *    Will contains status after exit.
 * @param hideComponents
 *    If true, any components (toolbar/toolbox) will be hidden.
 */
void 
BrainBrowserWindow::saveWindowComponentStatus(WindowComponentStatus& wcs)
{
    wcs.windowState = saveState();
    if (m_featuresToolBoxAction->isChecked()) {
        if (m_featuresToolBox != NULL) {
            wcs.featuresGeometry = m_featuresToolBox->saveGeometry();
        }
    }
    wcs.isToolBarDisplayed = m_showToolBarAction->isChecked();
    wcs.isOverlayToolBoxDisplayed = m_overlayToolBoxAction->isChecked();
    wcs.isFeaturesToolBoxDisplayed  = m_featuresToolBoxAction->isChecked();
    
    /*printWindowComponentStatus("Saving", wcs);*/
}

/**
 * Adds a new tab to the window.
 */
void
BrainBrowserWindow::processNewTab()
{
    m_toolbar->addNewTab();
}

/**
 * Adds a new tab to the window.
 */
void
BrainBrowserWindow::processDuplicateTab()
{
    BrowserTabContent* previousTabContent = getBrowserTabContent();
    m_toolbar->addNewDuplicatedTab(previousTabContent);
}

/**
 * Reopen the last closed tab in this window
 * @param reopenTabEvent
 * The reopen event
 */
void
BrainBrowserWindow::reopenLastClosedTab(EventBrowserTabReopenClosed& reopenTabEvent)
{
   m_toolbar->reopenLastClosedTab(reopenTabEvent);
}

/**
 * Called when move all tabs to one window is selected.
 */
void 
BrainBrowserWindow::processMoveAllTabsToOneWindow()
{
    /*
     * Wait cursor
     */
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    std::vector<BrowserTabContent*> otherTabContent;
    GuiManager::get()->closeOtherWindowsAndReturnTheirTabContent(this,
                                                                 otherTabContent);
    
    const int32_t numOtherTabs = static_cast<int32_t>(otherTabContent.size());
    for (int32_t i = 0; i < numOtherTabs; i++) {
        m_toolbar->addNewTabWithContent(otherTabContent[i]);
        m_toolbar->updateToolBar();
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Called when the move tab to window is about to be displayed.
 */
void 
BrainBrowserWindow::processMoveSelectedTabToWindowMenuAboutToBeDisplayed()
{
    m_moveSelectedTabToWindowMenu->clear();

    if (getBrowserTabContent() == NULL) {
        return;
    }
    
    /*
     * Allow movement of tab to new window ONLY if this window
     * contains more than one tab.
     */
    if (m_toolbar->tabBar->count() > 1) {
        QAction* toNewWindowAction = new QAction("New Window",
                                                 m_moveSelectedTabToWindowMenu);
        toNewWindowAction->setData(QVariant::fromValue((void*)NULL));
        m_moveSelectedTabToWindowMenu->addAction(toNewWindowAction);
    }
    
    std::vector<BrainBrowserWindow*> browserWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    for (int32_t i = 0; i < static_cast<int32_t>(browserWindows.size()); i++) {
        if (browserWindows[i] != this) {
            QAction* action = new QAction(browserWindows[i]->windowTitle(),
                                          m_moveSelectedTabToWindowMenu);
            action->setData(QVariant::fromValue((void*)browserWindows[i]));
            m_moveSelectedTabToWindowMenu->addAction(action);
        }
    }    
}

/**
 * Called when move tab to window menu item is selected.
 * This window may close if there are no more tabs after
 * the tab is removed.
 * @param action
 *    Action from menu item that was selected.
 */
void 
BrainBrowserWindow::processMoveSelectedTabToWindowMenuSelection(QAction* action)
{
    if (action != NULL) {
        /*
         * Wait cursor
         */
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        void* p = action->data().value<void*>();
        BrainBrowserWindow* moveToBrowserWindow = (BrainBrowserWindow*)p;
        
        BrowserTabContent* btc = getBrowserTabContent();
        
            
        if (moveToBrowserWindow != NULL) {
            m_toolbar->removeTabWithContent(btc);
            moveToBrowserWindow->m_toolbar->addNewTabWithContent(btc);
        }
        else {
            EventBrowserWindowNew newWindow(this,
                                            btc);
            EventManager::get()->sendEvent(newWindow.getPointer());
            if (newWindow.isError()) {
                cursor.restoreCursor();
                QMessageBox::critical(this,
                                      "",
                                      newWindow.getErrorMessage());
                return;
            }
            m_toolbar->removeTabWithContent(btc);
        }
        
        if (m_toolbar->tabBar->count() <= 0) {
            close();
        }
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

/**
 * Called to move the overlay toolbox to the left side of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToLeft()
{
    moveOverlayToolBox(Qt::LeftDockWidgetArea);
}

/**
 * Called to move the overlay toolbox to the bottom side of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToBottom()
{
    moveOverlayToolBox(Qt::BottomDockWidgetArea);
}

/**
 * Called to move the overlay toolbox to float outside of the window.
 */
void 
BrainBrowserWindow::processMoveOverlayToolBoxToFloat()
{
moveOverlayToolBox(Qt::NoDockWidgetArea);
}

/**
 * Called to hide the overlay toolbox.
 */
void 
BrainBrowserWindow::processHideOverlayToolBox()
{
    processShowOverlayToolBox(false);
}

/**
 * Called to move the layers toolbox to the right side of the window.
 */
void 
BrainBrowserWindow::processMoveFeaturesToolBoxToRight()
{
    moveFeaturesToolBox(Qt::RightDockWidgetArea);
}

/**
 * Called to move the layers toolbox to float outside of the window.
 */
void 
BrainBrowserWindow::processMoveFeaturesToolBoxToFloat()
{
    moveFeaturesToolBox(Qt::NoDockWidgetArea);
}

/**
 * Called to hide the layers tool box.
 */
void 
BrainBrowserWindow::processHideFeaturesToolBox()
{
    if (m_featuresToolBoxAction->isChecked()) {
        m_featuresToolBoxAction->trigger();
    }
}

/**
 * Called to display the layers toolbox.
 */
void 
BrainBrowserWindow::processShowFeaturesToolBox(bool status)
{
    if (status) {
        m_featuresToolBoxAction->setToolTip("Hide Features Toolbox");
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBox().getPointer());
    }
    else {
        m_featuresToolBoxAction->setToolTip("Show Features Toolbox");
    }
}

/**
 * Move the overlay toolbox to the desired location.
 * @param area
 *    DockWidget location.
 */
void 
BrainBrowserWindow::moveFeaturesToolBox(Qt::DockWidgetArea area)
{
    switch (area) {
        case Qt::LeftDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on left");
            break;
        case Qt::RightDockWidgetArea:
            m_featuresToolBox->setFloating(false);
            addDockWidget(Qt::RightDockWidgetArea, 
                                m_featuresToolBox);
            if (m_featuresToolBoxAction->isChecked() == false) {
                m_featuresToolBoxAction->trigger();
            }
            break;
        case Qt::TopDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on top");
            break;
        case Qt::BottomDockWidgetArea:
            CaretAssertMessage(0, "Layers toolbox not allowed on bottom");
            break;
        default:
            m_featuresToolBox->setFloating(true);
            if (m_featuresToolBoxAction->isChecked() == false) {
                m_featuresToolBoxAction->trigger();
            }
            break;
    }
}

/**
 * Move the overlay toolbox to the desired location.
 * @param area
 *    DockWidget location.
 */
void 
BrainBrowserWindow::moveOverlayToolBox(Qt::DockWidgetArea area)
{
    bool isVisible = false;
    switch (area) {
        case Qt::LeftDockWidgetArea:
            m_overlayHorizontalToolBox->setVisible(false);
            m_overlayVerticalToolBox->setFloating(false);
            addDockWidget(Qt::LeftDockWidgetArea, 
                                m_overlayVerticalToolBox);
            m_overlayVerticalToolBox->setVisible(true);
            m_overlayActiveToolBox = m_overlayVerticalToolBox;
            isVisible = true;
            break;
        case Qt::RightDockWidgetArea:
            CaretAssertMessage(0, "Overlay toolbox not allowed on right");
            break;
        case Qt::TopDockWidgetArea:
            CaretAssertMessage(0, "Overlay toolbox not allowed on top");
            break;
        case Qt::BottomDockWidgetArea:
            m_overlayVerticalToolBox->setVisible(false);
            m_overlayHorizontalToolBox->setFloating(false);
            addDockWidget(Qt::BottomDockWidgetArea, 
                                m_overlayHorizontalToolBox);
            m_overlayHorizontalToolBox->setVisible(true);
            m_overlayActiveToolBox = m_overlayHorizontalToolBox;
            isVisible = true;
            break;
        default:
            m_overlayActiveToolBox->setVisible(true);
            m_overlayActiveToolBox->setFloating(true);
            isVisible = true;
            break;
    }

    processShowOverlayToolBox(isVisible);
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
BrainBrowserWindow::removeAndReturnAllTabs(std::vector<BrowserTabContent*>& allTabContent)
{
    m_toolbar->removeAndReturnAllTabs(allTabContent);
}

/**
 * @return Return the active browser tab content in
 * this browser window.
 */
BrowserTabContent* 
BrainBrowserWindow::getBrowserTabContent()
{
    return m_toolbar->getTabContentFromSelectedTab();
}

/**
 * @return Return the active browser tab content in
 * this browser window.
 */
const BrowserTabContent*
BrainBrowserWindow::getBrowserTabContent() const
{
    return m_toolbar->getTabContentFromSelectedTab();
}

/**
 * get browser tab content for tab with specified tab Index
 * @param tabIndex
 *   Desired tabIndex
 * @return Return the active browser tab content in
 * this browser window.
*/
BrowserTabContent*
BrainBrowserWindow::getBrowserTabContent(int tabIndex)
{
    return m_toolbar->getTabContentFromTab(tabIndex);
}

/**
 * Get content of all tabs
 *
 * @param allTabContent
 *    Will contain the tabs in window upon return
 */
void
BrainBrowserWindow::getAllTabContent(std::vector<BrowserTabContent*>& allTabContent) const
{
    m_toolbar->getAllTabContent(allTabContent);
}

/**
 * Get indices of all tabs in window
 *
 * @param allTabContentIndices
 *    Will contain the indices from all tabs in window upon return.
 */
void
BrainBrowserWindow::getAllTabContentIndices(std::vector<int32_t>& allTabContentIndices) const
{
    allTabContentIndices.clear();
    
    std::vector<BrowserTabContent*> allTabContent;
    getAllTabContent(allTabContent);
    
    for (std::vector<BrowserTabContent*>::iterator iter = allTabContent.begin();
         iter != allTabContent.end();
         iter++) {
        const BrowserTabContent* btc = *iter;
        CaretAssert(btc);
        allTabContentIndices.push_back(btc->getTabNumber());
    }
}

/**
 * @return Browser window's content.
 */
BrowserWindowContent*
BrainBrowserWindow::getBrowerWindowContent()
{
    return m_browserWindowContent;
}

/**
 * @return Browser window's content (const method).
 */
const BrowserWindowContent*
BrainBrowserWindow::getBrowerWindowContent() const
{
    return m_browserWindowContent;
}

/**
 * Get the Brain OpenGL Viewport content for all tabs
 *
 * @param viewportContentOut
 *     Contains viewport content on exit
 */
void
BrainBrowserWindow::getAllBrainOpenGLViewportContent(std::vector<const BrainOpenGLViewportContent*>& viewportContentOut) const
{
    viewportContentOut.clear();
    
    viewportContentOut = m_openGLWidget->getViewportContent();
}

/**
 * @return The OpenGL Viewport content for the selected tab
 */
const BrainOpenGLViewportContent*
BrainBrowserWindow::getViewportContentForSelectedTab() const
{
    const BrowserTabContent* activeTabContent = getBrowserTabContent();
    if (activeTabContent == NULL) {
        return NULL;
    }
    
    std::vector<const BrainOpenGLViewportContent*> viewportContent;
    getAllBrainOpenGLViewportContent(viewportContent);
    
    for (const auto& vpc : viewportContent) {
        if (vpc->getBrowserTabContent() == activeTabContent) {
            return vpc;
        }
    }
    return NULL;
}


/**
 * Returns a popup menu for the main window.
 * Overrides that in QMainWindow and prevents the 
 * default context menu from appearing.
 *
"O * nothing available.
 */
QMenu* 
BrainBrowserWindow::createPopupMenu()
{
    return NULL;
}

/**
 * Open a connection to the allen brain institute database.
 */
void 
BrainBrowserWindow::processConnectToAllenDataBase()
{
    GuiManager::get()->processShowAllenDataBaseWebView(this);
}

/**
 * Open a connection to the human connectome project database.
 */
void 
BrainBrowserWindow::processConnectToConnectomeDataBase()
{
    GuiManager::get()->processShowConnectomeDataBaseWebView(this);
}

/**
 * Load the HCP Website into the user's web browser.
 */
void 
BrainBrowserWindow::processHcpWebsiteInBrowser()
{
    QUrl url("https://humanconnectome.org");
    QDesktopServices::openUrl(url);
}

/**
 * Load the HCP Website into the user's web browser.
 */
void
BrainBrowserWindow::processHcpUsersGroup()
{
    QUrl url("https://groups.google.com/a/humanconnectome.org/g/hcp-users");
    QDesktopServices::openUrl(url);
}

/**
 * Report a Workbench bug.
 */
void
BrainBrowserWindow::processReportWorkbenchBug()
{
    GuiManager::get()->processShowBugReportDialog(this,
                                                  m_openGLWidget->getOpenGLInformation());
}

/**
 * Load the HCP Feature Request Website into the user's web browser.
 */
void
BrainBrowserWindow::processHcpFeatureRequestWebsiteInBrowser()
{
    QUrl url("http://humanconnectome.org/contact/feature-request.php");
    QDesktopServices::openUrl(url);
}

/**
 * @rerturn Aspect ratio of the OpenGL widget.
 */
float
BrainBrowserWindow::getOpenGLWidgetAspectRatio() const
{
    const float w = this->m_openGLWidget->width();
    const float h = this->m_openGLWidget->height();
    
    const float aspectRatio = ((w != 0.0) ? (h / w) : 1.0);
    return aspectRatio;
}

/**
 * BrowserWindowContent is owned and saved to and restored from
 * Scenes by SessionManager.  Since SessionManager is save to 
 * Scene before instances of this class, update content
 * of BrowserWindowContent.
 */
void
BrainBrowserWindow::saveBrowserWindowContentForScene()
{
    std::vector<int32_t> allTabIndices;
    getAllTabContentIndices(allTabIndices);
    m_browserWindowContent->setSceneWindowTabIndices(allTabIndices);
    
    const BrowserTabContent* activeTabContent = getBrowserTabContent();
    if (activeTabContent != NULL) {
        m_browserWindowContent->setSceneSelectedTabIndex(activeTabContent->getTabNumber());
    }
        
    m_browserWindowContent->setSceneGraphicsWidth(m_openGLWidget->width());
    m_browserWindowContent->setSceneGraphicsHeight(m_openGLWidget->height());
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
BrainBrowserWindow::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainBrowserWindow",
                                            2); /* Version 2: 07 feb 2018 */
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    /* m_browserWindowIndex used with wb_command -show-scene */
    sceneClass->addInteger("m_browserWindowIndex",
                           m_browserWindowIndex);

    /*
     * Save toolbar
     */
    sceneClass->addClass(m_toolbar->saveToScene(sceneAttributes, 
                                                "m_toolbar"));

    /*
     * Save overlay toolbox
     */
    {
        AString orientationName = "";
        if (m_overlayActiveToolBox == m_overlayHorizontalToolBox) {
            orientationName = "horizontal";
        }
        else if (m_overlayActiveToolBox == m_overlayVerticalToolBox) {
            orientationName = "vertical";
        }
        
        SceneClass* overlayToolBoxClass = new SceneClass("overlayToolBox",
                                                         "OverlayToolBox",
                                                         1);
        overlayToolBoxClass->addString("orientation", 
                                       orientationName);
        overlayToolBoxClass->addBoolean("floating", 
                                        m_overlayActiveToolBox->isFloating());
        overlayToolBoxClass->addBoolean("visible", 
                                        m_overlayActiveToolBox->isVisible());
        sceneClass->addClass(overlayToolBoxClass);
        
        sceneClass->addClass(m_overlayActiveToolBox->saveToScene(sceneAttributes,
                                                                 "m_overlayActiveToolBox"));
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
    
    /*
     * Save features toolbox
     */
    {
        SceneClass* featureToolBoxClass = new SceneClass("featureToolBox",
                                                         "FeatureToolBox",
                                                         1);
        featureToolBoxClass->addBoolean("floating",
                                        m_featuresToolBox->isFloating());
        featureToolBoxClass->addBoolean("visible",
                                        m_featuresToolBox->isVisible());
        sceneClass->addClass(featureToolBoxClass);
        sceneClass->addClass(m_featuresToolBox->saveToScene(sceneAttributes,
                                                            "m_featuresToolBox"));
    }
    
    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                         "geometry"));
    
    sceneClass->addBoolean("isFullScreen",
                           isFullScreen());
    sceneClass->addBoolean("isMaximized",
                           isMaximized());
    
    /*
     * Graphics position and size, used with wb_command -show-scene
     */
    SceneWindowGeometry openGLGeometry(m_openGLWidget);
    sceneClass->addClass(openGLGeometry.saveToScene(sceneAttributes,
                                                    "openGLWidgetGeometry"));
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
BrainBrowserWindow::restoreFromScene(const SceneAttributes* sceneAttributes,
                             const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const int32_t sceneVersion = sceneClass->getVersionNumber();
    
    /*
     * Restore toolbar
     */
    const SceneClass* toolbarClass = sceneClass->getClass("m_toolbar");
    if (toolbarClass != NULL) {
        m_toolbar->restoreFromScene(sceneAttributes,
                                    toolbarClass);
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    if ( ! m_browserWindowContent->isValid()) {
        sceneAttributes->addToErrorMessage("Scene Error: Browser window content is invalid for window "
                                           + AString::number(m_browserWindowIndex + 1));
        return;
    }
    
    /*
     * Restoration status for full screen and tab tiles
     *
     * If "m_screenMode" is found, the scene is an older scene that was
     * created prior to splitting Full Screen and Tile Tabs into 
     * separate functionality.
     */
    bool restoreToFullScreen = false;
    bool restoreToTabTiles   = false;
    const SceneObject* screenModeObject = sceneClass->getObjectWithName("m_screenMode");
    if (screenModeObject != NULL) {
        const SceneEnumeratedType* screenEnum = dynamic_cast<const SceneEnumeratedType*>(screenModeObject);
        if (screenEnum != NULL) {
            const AString screenModeName = screenEnum->stringValue();
            
            if (screenModeName == "NORMAL") {
                
            }
            else if (screenModeName == "FULL_SCREEN") {
                restoreToFullScreen = true;
            }
            else if (screenModeName == "TAB_MONTAGE") {
                restoreToTabTiles = true;
            }
            else if (screenModeName == "TAB_MONTAGE_FULL_SCREEN") {
                restoreToTabTiles   = true;
                restoreToFullScreen = true;
            }
            else {
                CaretLogWarning("Unrecognized obsolete screen mode: "
                                + screenModeName);
            }
        }
    }
    else {
        restoreToFullScreen = sceneClass->getBooleanValue("isFullScreen",
                                                          false);
        
        if (sceneVersion >= 2) {
            restoreToTabTiles = m_browserWindowContent->isTileTabsEnabled();
        }
        else {
            restoreToTabTiles = sceneClass->getBooleanValue("m_viewTileTabsAction",
                                                            false);
        }
        setViewTileTabs(restoreToTabTiles);
    }

    m_restoringSceneNoSaveWindowCompontentStatusFlag = restoreToFullScreen;
    
    m_normalWindowComponentStatus = m_defaultWindowComponentStatus;
    processViewFullScreen(restoreToFullScreen,
                          false);
    
    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    swg.restoreFromScene(sceneAttributes, sceneClass->getClass("geometry"));
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    QApplication::processEvents();
    
    if (restoreToFullScreen == false) {
        /*
         * Restore feature toolbox
         */
        const SceneClass* featureToolBoxClass = sceneClass->getClass("featureToolBox");
        if (featureToolBoxClass != NULL) {
            const bool toolBoxVisible = featureToolBoxClass->getBooleanValue("visible",
                                                                             true);
            const bool toolBoxFloating = featureToolBoxClass->getBooleanValue("floating",
                                                                              false);
            
            if (toolBoxVisible) {
                if (toolBoxFloating) {
                    processMoveFeaturesToolBoxToFloat();
                }
                else {
                    processMoveFeaturesToolBoxToRight();
                }
            }
            m_featuresToolBoxAction->blockSignals(true);
            m_featuresToolBoxAction->setChecked(! toolBoxVisible);
            m_featuresToolBoxAction->blockSignals(false);
            m_featuresToolBoxAction->trigger();
            m_featuresToolBox->restoreFromScene(sceneAttributes,
                                                sceneClass->getClass("m_featuresToolBox"));
            
            /*
             * Toolboxes were not restoring to correct size in Qt5.
             * Qt5.6 adds a new method, QMainWindow::resizeDocks() that 
             * resizes a QDockWidget in one dimension.  Use it to resize
             * the toolbox.
             */
            const SceneClass* featureToolBoxClass = sceneClass->getClass("m_featuresToolBox");
            if (featureToolBoxClass != NULL) {
                const int w = featureToolBoxClass->getIntegerValue("toolboxWidth", -1);
                const int h = featureToolBoxClass->getIntegerValue("toolboxHeight", -1);
                if ((w > 0)
                    && (h > 0)) {
                    QList<QDockWidget*> dockList;
                    dockList.append(m_featuresToolBox);
                    QList<int> sizeList;
                    sizeList.append(w);
                    resizeDockWidgets(dockList, sizeList, Qt::Horizontal);
                }
            }
        }
        
        /*
         * Restore overlay toolbox
         */
        const SceneClass* overlayToolBoxClass = sceneClass->getClass("overlayToolBox");
        if (overlayToolBoxClass != NULL) {
            const AString orientationName = overlayToolBoxClass->getStringValue("orientation",
                                                                                "horizontal");
            const bool toolBoxVisible = overlayToolBoxClass->getBooleanValue("visible",
                                                                             true);
            const bool toolBoxFloating = overlayToolBoxClass->getBooleanValue("floating",
                                                                              false);
            if (orientationName == "horizontal") {
                processMoveOverlayToolBoxToBottom();
            }
            else {
                processMoveOverlayToolBoxToLeft();
            }
            if (toolBoxFloating) {
                processMoveOverlayToolBoxToFloat();
            }
            processShowOverlayToolBox(toolBoxVisible);
            m_overlayActiveToolBox->restoreFromScene(sceneAttributes,
                                                     sceneClass->getClass("m_overlayActiveToolBox"));
            

            /*
             * Toolboxes were not restoring to correct size in Qt5.
             * Qt5 adds a new method, QMainWindow::resizeDocks() that
             * resizes a QDockWidget in one dimension.  Use it to resize
             * the toolbox.
             */
            const SceneClass* activeToolBoxClass = sceneClass->getClass("m_overlayActiveToolBox");
            if (activeToolBoxClass != NULL) {
                const int w = activeToolBoxClass->getIntegerValue("toolboxWidth", -1);
                const int h = activeToolBoxClass->getIntegerValue("toolboxHeight", -1);
                if ((w > 0)
                    && (h > 0)) {
                    QList<QDockWidget*> dockList;
                    dockList.append(m_overlayActiveToolBox);
                    if (orientationName == "horizontal") {
                        QList<int> sizeList;
                        sizeList.append(h);
                        resizeDockWidgets(dockList, sizeList, Qt::Vertical);
                    }
                    else {
                        QList<int> sizeList;
                        sizeList.append(w);
                        resizeDockWidgets(dockList, sizeList, Qt::Horizontal);
                    }
                }
            }
        }
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    const bool maximizedWindow = sceneClass->getBooleanValue("isMaximized",
                                                       false);
    if (maximizedWindow) {
        showMaximized();
    }
    
    /* prevent "failed to restore" messages */
    sceneClass->getIntegerValue("m_browserWindowIndex");
    const SceneClass* openglGeomClass = sceneClass->getClass("openGLWidgetGeometry");
    if (openglGeomClass != NULL) {
        openglGeomClass->setDescendantsRestored(true);
    }
    
    if (sceneVersion >= 2) {
        
    }
    else {
        /* 
         * Before version 2, each tab was allowed its own aspect locking status
         */
        std::vector<BrowserTabContent*> allTabContent;
        m_toolbar->getAllTabContent(allTabContent);
        
        const int32_t lockedCount = std::count_if(allTabContent.begin(),
                                                  allTabContent.end(),
                                                  [](BrowserTabContent* btc) { return btc->isAspectRatioLocked(); });

        EventManager::get()->sendEvent(EventGraphicsPaintSoonOneWindow(getBrowserWindowIndex()).getPointer());

        lockAllTabAspectRatios(lockedCount > 0);
    }

    m_normalWindowComponentStatus = m_defaultWindowComponentStatus;

    updateActionsForLockingAspectRatios();
    
    m_restoringSceneNoSaveWindowCompontentStatusFlag = restoreToFullScreen;
}

/**
 * @return A string describing the object's content.
 */
AString
BrainBrowserWindow::toString() const
{
    AString msg;
    
    msg.appendWithNewLine("Window "
                          + AString::number(getBrowserWindowIndex() + 1)
                          + ":");
    
    const BrowserTabContent* btc = getBrowserTabContent();
    if (btc != NULL) {
        msg.appendWithNewLine(btc->toString());
    }
    
    return msg;
}

/**
 * Get a text description of the window's content.
 *
 * @param descriptionOut
 *    Description of the window's content.
 */
void
BrainBrowserWindow::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Window "
                           + AString::number(getBrowserWindowIndex() + 1)
                           + ":");

    std::vector<BrowserTabContent*> tabContent;
    
    if (isTileTabsSelected()) {
        m_toolbar->getAllTabContent(tabContent);
    }
    else {
        BrowserTabContent* btc = m_toolbar->getTabContentFromSelectedTab();
        if (btc != NULL) {
            tabContent.push_back(btc);
        }
    }
    
    descriptionOut.pushIndentation();
    for (std::vector<BrowserTabContent*>::iterator iter = tabContent.begin();
         iter != tabContent.end();
         iter++) {
        const BrowserTabContent* btc = *iter;
        btc->getDescriptionOfContent(descriptionOut);
    }
    descriptionOut.popIndentation();
}

/**
 * @return Has valid OpenGL.
 */
bool
BrainBrowserWindow::hasValidOpenGL()
{
    return m_openGLWidget->isValid();
}

/**
 * Show the data file read warnings dialog but only
 * if warnings are found in any files.
 */
void
BrainBrowserWindow::showDataFileReadWarningsDialog()
{
    Brain* brain = GuiManager::get()->getBrain();
    if (brain == NULL) {
        return;
    }
    
    std::vector<CaretDataFile*> dataFiles;
    brain->getAllDataFiles(dataFiles);
    
    AString messages;
    for (auto file : dataFiles) {
        const AString msg = file->getFileReadWarnings().replace("\n", "<br>");
        if ( ! msg.isEmpty()) {
            messages.append("<LI>"
                            + file->getFileName()
                            + "<br>"
                            + msg
                            + "</LI>");
        }
    }
    
    if ( ! messages.isEmpty()) {
        messages.insert(0,
                        "<html><ul>");
        messages.append("</ul></html>");
        
        WuQTextEditorDialog::runNonModal("Data File Warnings",
                                         messages,
                                         WuQTextEditorDialog::TextMode::HTML,
                                         WuQTextEditorDialog::WrapMode::YES,
                                         this);
    }
}
