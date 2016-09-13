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

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabBar>
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
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretFileRemoteDialog.h"
#include "CaretPreferences.h"
#include "CursorDisplayScoped.h"
#include "DataFileException.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesImages.h"
#include "DisplayPropertiesVolume.h"
#include "EventBrowserWindowNew.h"
#include "CaretLogger.h"
#include "ElapsedTimer.h"
#include "EventGetViewportSize.h"
#include "EventBrowserWindowCreateTabs.h"
#include "EventDataFileRead.h"
#include "EventMacDockMenuUpdate.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventSpecFileReadDataFiles.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventUserInterfaceUpdate.h"
#include "FileInformation.h"
#include "FociProjectionDialog.h"
#include "GuiManager.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelWholeBrain.h"
#include "PlainTextStringBuilder.h"
#include "ProgressReportingDialog.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneEnumeratedType.h"
#include "SceneFile.h"
#include "SceneWindowGeometry.h"
#include "SessionManager.h"
#include "SpecFile.h"
#include "SpecFileManagementDialog.h"
#include "StructureEnumComboBox.h"
#include "Surface.h"
#include "SurfaceMontageConfigurationAbstract.h"
#include "SurfaceSelectionViewController.h"
#include "TileTabsConfiguration.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "VtkFileExporter.h"

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
: QMainWindow(parent, flags)
{
    m_developMenuAction = NULL;
    
    if (BrainBrowserWindow::s_firstWindowFlag) {
        BrainBrowserWindow::s_firstWindowFlag = false;
    }
    m_viewTileTabsSelected = false;
    
    m_aspectRatio = 1.0;
    
    m_sceneTileTabsConfigurationText = "From Scene: ";
    m_sceneTileTabsConfiguration = new TileTabsConfiguration();
    m_sceneTileTabsConfiguration->setName(m_sceneTileTabsConfigurationText);
    
    m_defaultTileTabsConfiguration = new TileTabsConfiguration();
    m_defaultTileTabsConfiguration->setDefaultConfiguration(true);
    m_defaultTileTabsConfiguration->setName("All Tabs (Default)");
    m_selectedTileTabsConfigurationUniqueIdentifier = m_defaultTileTabsConfiguration->getUniqueIdentifier();
    
    GuiManager* guiManager = GuiManager::get();
    
    setAttribute(Qt::WA_DeleteOnClose);
    
    m_browserWindowIndex = browserWindowIndex;
    
    setWindowTitle(guiManager->applicationName() 
                         + " "
                         + AString::number(m_browserWindowIndex + 1));
    setObjectName(windowTitle());
    
    m_openGLWidget = new BrainOpenGLWidget(this,
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
                                          this);
    m_overlayVerticalToolBox->setAllowedAreas(Qt::LeftDockWidgetArea);
    
    m_overlayHorizontalToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Overlay ToolBox ",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_OVERLAYS_HORIZONTAL,
                                          this);
    m_overlayHorizontalToolBox->setAllowedAreas(Qt::BottomDockWidgetArea);

    if (WuQtUtilities::isSmallDisplay()) {
        m_overlayActiveToolBox = m_overlayVerticalToolBox;
        addDockWidget(Qt::LeftDockWidgetArea, m_overlayVerticalToolBox);
        m_overlayHorizontalToolBox->setVisible(false);
        //m_overlayHorizontalToolBox->toggleViewAction()->trigger();
    }
    else {
        m_overlayActiveToolBox = m_overlayHorizontalToolBox;
        addDockWidget(Qt::BottomDockWidgetArea, m_overlayHorizontalToolBox);
        m_overlayVerticalToolBox->setVisible(false);
        //m_overlayVerticalToolBox->toggleViewAction()->trigger();
    }
    
    QObject::connect(m_overlayHorizontalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayHorizontalToolBoxVisibilityChanged(bool)));
    QObject::connect(m_overlayVerticalToolBox, SIGNAL(visibilityChanged(bool)),
                     this, SLOT(processOverlayVerticalToolBoxVisibilityChanged(bool)));
    
    m_featuresToolBox = 
    new BrainBrowserWindowOrientedToolBox(m_browserWindowIndex,
                                          "Features ToolBox",
                                          BrainBrowserWindowOrientedToolBox::TOOL_BOX_FEATURES,
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
                                              m_windowAspectRatioLockedAction,
                                              m_tabAspectRatioLockedAction,
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
    m_sceneAssistant->add("m_aspectRatio",
                          &m_aspectRatio);
    
    m_defaultWindowComponentStatus.isFeaturesToolBoxDisplayed = m_featuresToolBoxAction->isChecked();
    m_defaultWindowComponentStatus.isOverlayToolBoxDisplayed  = m_overlayToolBoxAction->isChecked();
    m_defaultWindowComponentStatus.isToolBarDisplayed = m_showToolBarAction->isChecked();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_MENUS_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_VIEWPORT_SIZE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
}

/**
 * Destructor.
 */
BrainBrowserWindow::~BrainBrowserWindow()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_defaultTileTabsConfiguration;
    delete m_sceneTileTabsConfiguration;
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
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_VIEWPORT_SIZE) {
        EventGetViewportSize* viewportSizeEvent = dynamic_cast<EventGetViewportSize*>(event);
        CaretAssert(viewportSizeEvent);
        
        std::vector<const BrainOpenGLViewportContent*> allViewportContent = m_openGLWidget->getViewportContent();
        
        int32_t viewport[4] = { 0, 0, 0, 0 };
        bool viewportValid = false;
        
        int32_t notBestViewport[4] = { 0, 0, 0, 0 };
        bool notBestViewportValid = false;
        
        switch (viewportSizeEvent->getMode()) {
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
                break;
            case EventGetViewportSize::MODE_TAB_AFTER_MARGINS_INDEX:
                for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = allViewportContent.begin();
                     vpIter != allViewportContent.end();
                     vpIter++) {
                    const BrainOpenGLViewportContent* vpContent = *vpIter;
                    if (vpContent != NULL) {
                        BrowserTabContent* btc = vpContent->getBrowserTabContent();
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
                                    if (btc->getSliceDrawingType() == VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE) {
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
        }
    }
//    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_GET_VIEWPORT_SIZE) {
//        EventBrowserTabGetViewportSize* viewportSizeEvent = dynamic_cast<EventBrowserTabGetViewportSize*>(event);
//        CaretAssert(viewportSizeEvent);
//        
//        std::vector<BrowserTabContent*> allTabContent;
//        m_toolbar->getAllTabContent(allTabContent);
//        
//        const std::vector<const BrainOpenGLViewportContent*> allViewportContent = m_openGLWidget->getViewportContent();
//        
//        int32_t tabViewport[4] = { 0, 0, 0, 0 };
//        bool tabViewportValid = false;
//        
//        int32_t notBestTabViewport[4] = { 0, 0, 0, 0 };
//        bool notBestTabViewportValid = false;
//        
//        /*
//         * Find the viewport content containing the specified tab by index
//         */
//        for (std::vector<BrowserTabContent*>::iterator tabIter = allTabContent.begin();
//             tabIter != allTabContent.end();
//             tabIter++) {
//            const BrowserTabContent* btc = *tabIter;
//            CaretAssert(btc);
//            
//            switch (viewportSizeEvent->getMode()) {
//                case EventBrowserTabGetViewportSize::MODE_SURFACE_MONTAGE:
//                {
//                    const Model* model = btc->getModelForDisplay();
//                    if (model->getModelType() == ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE) {
//                        for (std::vector<const BrainOpenGLViewportContent*>::const_iterator vpIter = allViewportContent.begin();
//                             vpIter != allViewportContent.end();
//                             vpIter++) {
//                            const BrainOpenGLViewportContent* vpContent = *vpIter;
//                            if (vpContent->getTabIndex() == btc->getTabNumber()) {
//                                vpContent->getTabViewportBeforeApplyingMargins(tabViewport);
//                                tabViewportValid = true;
//                                break;
//                            }
//                        }
//                    }
//                }
//                    break;
//                case EventBrowserTabGetViewportSize::MODE_TAB_INDEX:
//                    if (btc->getTabNumber() == viewportSizeEvent->getIndex()) {
//                        for (std::vector<const BrainOpenGLViewportContent*>::const_iterator vpIter = allViewportContent.begin();
//                             vpIter != allViewportContent.end();
//                             vpIter++) {
//                            const BrainOpenGLViewportContent* vpContent = *vpIter;
//                            if (vpContent->getTabIndex() == viewportSizeEvent->getIndex()) {
//                                vpContent->getTabViewportBeforeApplyingMargins(tabViewport);
//                                tabViewportValid = true;
//                                break;
//                            }
//                        }
//                    }
//                    break;
//                case EventBrowserTabGetViewportSize::MODE_VOLUME_MONTAGE:
//                {
//                    const Model* model = btc->getModelForDisplay();
//                    if (model->getModelType() == ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES) {
//                        for (std::vector<const BrainOpenGLViewportContent*>::const_iterator vpIter = allViewportContent.begin();
//                             vpIter != allViewportContent.end();
//                             vpIter++) {
//                            const BrainOpenGLViewportContent* vpContent = *vpIter;
//                            if (vpContent->getTabIndex() == btc->getTabNumber()) {
//                                if (btc->getSliceDrawingType() == VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE) {
//                                    vpContent->getTabViewportBeforeApplyingMargins(tabViewport);
//                                    tabViewportValid = true;
//                                    break;
//                                }
//                                else {
//                                    vpContent->getTabViewportBeforeApplyingMargins(notBestTabViewport);
//                                    notBestTabViewportValid = true;
//                                }
//                            }
//                        }
//                    }
//                }
//                    break;
//                case EventBrowserTabGetViewportSize::MODE_WINDOW_INDEX:
//                    if (m_browserWindowIndex == viewportSizeEvent->getIndex()) {
//                        for (std::vector<const BrainOpenGLViewportContent*>::const_iterator vpIter = allViewportContent.begin();
//                             vpIter != allViewportContent.end();
//                             vpIter++) {
//                            const BrainOpenGLViewportContent* vpContent = *vpIter;
//                            if (vpContent->getTabIndex() == viewportSizeEvent->getIndex()) {
//                                vpContent->getTabViewportBeforeApplyingMargins(tabViewport);
//                                tabViewportValid = true;
//                                break;
//                            }
//                        }
//                    }
//                    break;
//            }
//            
//            if (tabViewportValid) {
//                break;
//            }
//        }
//        
//        if ( ! tabViewportValid) {
//            if (notBestTabViewportValid) {
//                tabViewport[0] = notBestTabViewport[0];
//                tabViewport[1] = notBestTabViewport[1];
//                tabViewport[2] = notBestTabViewport[2];
//                tabViewport[3] = notBestTabViewport[3];
//                tabViewportValid = true;
//            }
//            else {
//                /*
//                 * Tab is in this window but not the active tab.
//                 * So, use the active tab's viewport
//                 */
//                if ( ! allViewportContent.empty()) {
//                    CaretAssertVectorIndex(allViewportContent, 0);
//                    allViewportContent[0]->getTabViewportBeforeApplyingMargins(tabViewport);
//                    tabViewportValid = true;
//                }
//            }
//        }
//        
//        if (tabViewportValid) {
//            viewportSizeEvent->setViewportSize(tabViewport);
//        }
//    }
    else if ((event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW)
             || (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS)) {
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
 * @param xOut
 *     X-coord of graphics region (may be non-zero when lock aspect applied)
 * @param yOut
 *     Y-coord of graphics region (may be non-zero when lock aspect applied)
 * @param widthOut
 *     Width of the graphics widget.
 * @param heightOut
 *     Height of the graphics widget.
 * @param graphicsWidthOut
 *     True width of graphics region as if no aspect ratio was applied.
 * @param graphicsHeightOut
 *     True height of graphics region as if no aspect ratio was applied.
 * @param applyLockedAspectRatiosFlag
 *     True if locked tab or window aspect ratio should be applied for
 *     graphics region size.
 */
void
BrainBrowserWindow::getGraphicsWidgetSize(int32_t& xOut,
                                          int32_t& yOut,
                                          int32_t& widthOut,
                                          int32_t& heightOut,
                                          int32_t& graphicsWidthOut,
                                          int32_t& graphicsHeightOut,
                                          const bool applyLockedAspectRatiosFlag) const
{
    xOut = 0;
    yOut = 0;
    graphicsWidthOut  = m_openGLWidget->width();
    graphicsHeightOut = m_openGLWidget->height();
    widthOut  = m_openGLWidget->width();
    heightOut = m_openGLWidget->height();
    
    int aspectViewport[4] = {
        0,
        0,
        widthOut,
        heightOut
    };
    
    if (isTileTabsSelected()) {
        if (isAspectRatioLocked()) {
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
            else if (isAspectRatioLocked()) {
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
                if ((windowWidth != widthOut)
                    || (windowHeight != heightOut)) {
                    xOut      = windowViewport[0];
                    yOut      = windowViewport[1];
                    widthOut  = windowWidth;
                    heightOut = windowHeight;
                }
            }
        }
        
//        float aspectLockedWidth  = aspectViewport[2];
//        float aspectLockedHeight = aspectViewport[3];
//        if ((aspectLockedWidth > 0)
//            && (aspectLockedHeight > 0)) {
//            if ((widthOut != aspectLockedWidth)
//                || (heightOut != aspectLockedHeight)) {
//                
//                std::cout << "Viewport adjusted from << " << AString::number(widthOut) << ", " << AString::number(heightOut)
//                << " to " << aspectLockedWidth << ", " << aspectLockedHeight << std::endl;
//                
//                widthOut  = aspectLockedWidth;
//                heightOut = aspectLockedHeight;
//            }
//        }
    }
}



/**
 * @return True if tile tabs is selected, else false.
 */
bool
BrainBrowserWindow::isTileTabsSelected() const
{
    return m_viewTileTabsSelected;
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
        }
    }
    
    /*
     * According to the documentation, if the key event was not acted upon,
     * pass it on the base class implementation.
     */
    if ( ! keyEventWasProcessed) {
        QMainWindow::keyPressEvent(event);
    }
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
    
    m_windowAspectRatioLockedAction = new QAction(this);
    m_windowAspectRatioLockedAction->setCheckable(true);
    m_windowAspectRatioLockedAction->setChecked(false);
    m_windowAspectRatioLockedAction->setText("Lock Window Aspect");
    m_windowAspectRatioLockedAction->setToolTip("Lock aspect ratio of window");
    m_windowAspectRatioLockedAction->setShortcut(Qt::SHIFT + Qt::CTRL+Qt::Key_K);
    QObject::connect(m_windowAspectRatioLockedAction, SIGNAL(toggled(bool)),
                     this, SLOT(processWindowAspectRatioLockedToggled(bool)));
    
    m_tabAspectRatioLockedAction = new QAction(this);
    m_tabAspectRatioLockedAction->setCheckable(true);
    m_tabAspectRatioLockedAction->setText("Lock Tab Aspect");
    m_tabAspectRatioLockedAction->setToolTip("Lock aspect ratio of selected tab");
    m_tabAspectRatioLockedAction->setShortcut(Qt::CTRL+Qt::Key_K);
    QObject::connect(m_tabAspectRatioLockedAction, SIGNAL(toggled(bool)),
                     this, SLOT(processTabAspectRatioLockedToggled(bool)));

    m_lockAllTabsAspectRatioAction = new QAction(this);
    m_lockAllTabsAspectRatioAction->setText("Lock Aspect Ratio for All Tabs");
    m_lockAllTabsAspectRatioAction->setToolTip("Lock aspect ratio of all tabs in this window");
    QObject::connect(m_lockAllTabsAspectRatioAction, SIGNAL(triggered(bool)),
                     this, SLOT(processLockAllTabsAspectRatioTriggered()));
    
    m_unlockAllTabsAspectRatioAction = new QAction(this);
    m_unlockAllTabsAspectRatioAction->setText("Unlock Aspect Ratio for All Tabs");
    m_unlockAllTabsAspectRatioAction->setToolTip("Unlock aspect ratio of all tabs in this window");
    QObject::connect(m_unlockAllTabsAspectRatioAction, SIGNAL(triggered(bool)),
                     this, SLOT(processUnlockAllTabsAspectRatioTriggered()));
}

/**
 * Called when Lock Aspect Ratio for All Tabs is selected.
 */
void
BrainBrowserWindow::processLockAllTabsAspectRatioTriggered()
{
    
    std::vector<const BrainOpenGLViewportContent*> vpContent = m_openGLWidget->getViewportContent();

    if (isTileTabsSelected()) {
        /*
         * When tile tabs is on, set the aspect ratio for each tab that does not
         * already have its aspect ratio locked
         */
        for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = vpContent.begin();
             vpIter != vpContent.end();
             vpIter++) {
            const BrainOpenGLViewportContent* vp = *vpIter;
            CaretAssert(vp);
            
            BrowserTabContent* tabContent = vp->getBrowserTabContent();
            if (tabContent != NULL) {
                if ( ! tabContent->isAspectRatioLocked()) {
                    int32_t tabViewport[4];
                    vp->getTabViewportBeforeApplyingMargins(tabViewport);
                    
                    const float width = tabViewport[2];
                    if (width > 0.0) {
                        const float aspectRatio = tabViewport[3] / width;
                        
                        tabContent->setAspectRatioLocked(true);
                        tabContent->setAspectRatio(aspectRatio);
                    }
                }
            }
        }
    }
    else {
        /*
         * Set the aspect ratio for each tab that does not already have its 
         * aspect ratio locked.  Since only one tab is displayed, use the 
         * size of the graphics region for the aspect ratio.
         */
        
        if ( ! vpContent.empty()) {
            CaretAssertVectorIndex(vpContent, 0);
            const BrainOpenGLViewportContent* vp = vpContent[0];
            
            int32_t tabViewport[4];
            vp->getTabViewportBeforeApplyingMargins(tabViewport);
            
            const float width = tabViewport[2];
            if (width > 0.0) {
                const float aspectRatio = tabViewport[3] / width;
                
                std::vector<BrowserTabContent*> allTabContent;
                getAllTabContent(allTabContent);
                
                for (std::vector<BrowserTabContent*>::iterator iter = allTabContent.begin();
                     iter != allTabContent.end();
                     iter++) {
                    BrowserTabContent* tabContent = *iter;
                    CaretAssert(tabContent);
                    if ( ! tabContent->isAspectRatioLocked()) {
                        tabContent->setAspectRatioLocked(true);
                        tabContent->setAspectRatio(aspectRatio);
                    }
                }
            }
        }
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(getBrowserWindowIndex()).getPointer());
    m_toolbar->updateToolBar();
}

/**
 * Called when Unlock Aspect Ratio for All Tabs is selected.
 */
void
BrainBrowserWindow::processUnlockAllTabsAspectRatioTriggered()
{
    std::vector<BrowserTabContent*> allTabContent;
    getAllTabContent(allTabContent);
    
    for (std::vector<BrowserTabContent*>::iterator iter = allTabContent.begin();
         iter != allTabContent.end();
         iter++) {
        BrowserTabContent* tabContent = *iter;
        CaretAssert(tabContent);
        tabContent->setAspectRatioLocked(false);
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(getBrowserWindowIndex()).getPointer());
    m_toolbar->updateToolBar();
}

/**
 * Called when the Tab aspect ratio action it toggled.
 *
 * @param new tab locked status
 */
void
BrainBrowserWindow::processTabAspectRatioLockedToggled(bool checked)
{
    BrowserTabContent* selectedTab = m_toolbar->getTabContentFromSelectedTab();
    if (selectedTab != NULL) {
        std::vector<const BrainOpenGLViewportContent*> vpContent = m_openGLWidget->getViewportContent();
        for (std::vector<const BrainOpenGLViewportContent*>::iterator vpIter = vpContent.begin();
             vpIter != vpContent.end();
             vpIter++) {
            const BrainOpenGLViewportContent* vp = *vpIter;
            CaretAssert(vp);
            
            BrowserTabContent* tabContent = vp->getBrowserTabContent();
            if (selectedTab == tabContent) {
                int32_t tabViewport[4];
                vp->getTabViewportBeforeApplyingMargins(tabViewport);
                
                const float width = tabViewport[2];
                if (width > 0.0) {
                    const float aspectRatio = tabViewport[3] / width;
                    
                    tabContent->setAspectRatioLocked(checked);
                    if (checked) {
                        tabContent->setAspectRatio(aspectRatio);
                    }
                }
            }
        }
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(getBrowserWindowIndex()).getPointer());
}

/**
 * Called when the Window aspect ratio action it toggled.
 *
 * @param new tab locked status
 */
void
BrainBrowserWindow::processWindowAspectRatioLockedToggled(bool checked)
{
    setAspectRatioLocked(checked);
    if (checked) {
        const float aspectRatio = getOpenGLWidgetAspectRatio();
        if (aspectRatio > 0) {
            setAspectRatio(aspectRatio);
        }
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(getBrowserWindowIndex()).getPointer());
}

/**
 * @return Is the aspect ratio locked?
 */
bool
BrainBrowserWindow::isAspectRatioLocked() const
{
    return m_windowAspectRatioLockedAction->isChecked();
}

/**
 * Set the aspect ratio locked status.
 *
 * @param locked
 *     New status.
 */
void
BrainBrowserWindow::setAspectRatioLocked(const bool locked)
{
    m_windowAspectRatioLockedAction->blockSignals(true);
    m_windowAspectRatioLockedAction->setChecked(locked);
    m_windowAspectRatioLockedAction->blockSignals(false);
}

/**
 * @return The aspect ratio.
 */
float
BrainBrowserWindow::getAspectRatio() const
{
    return m_aspectRatio;
}

/**
 * Set the aspect ratio.
 *
 * @param aspectRatio
 *     New value for aspect ratio.
 */
void
BrainBrowserWindow::setAspectRatio(const float aspectRatio)
{
    m_aspectRatio = aspectRatio;
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
                                Qt::CTRL+Qt::Key_N,
                                this,
                                this,
                                SLOT(processNewWindow()));
    
    m_newTabAction =
    WuQtUtilities::createAction("New Tab", 
                                "Create a new tab (window pane) in the window",
                                Qt::CTRL + Qt::Key_T,
                                this,
                                this,
                                SLOT(processNewTab()));
    
    m_duplicateTabAction =
    WuQtUtilities::createAction("Duplicate Tab",
                                "Create a new tab (window pane) that duplicates the selected tab in the window",
                                Qt::CTRL + Qt::Key_D,
                                this,
                                this,
                                SLOT(processDuplicateTab()));
    
    m_openFileAction =
    WuQtUtilities::createAction("Open File...", 
                                "Open a data file including a spec file located on the computer",
                                Qt::CTRL+Qt::Key_O,
                                this,
                                this,
                                SLOT(processDataFileOpen()));
    
    m_openLocationAction = 
    WuQtUtilities::createAction("Open Location...", 
                                "Open a data file including a spec file located on a web server (http)",
                                Qt::CTRL+Qt::Key_L,
                                this,
                                this,
                                SLOT(processDataFileLocationOpen()));
    
    m_manageFilesAction =
    WuQtUtilities::createAction("Save/Manage Files...", 
                                "Save and Manage Loaded Files",
                                Qt::CTRL + Qt::Key_S,
                                this,
                                this,
                                SLOT(processManageSaveLoadedFiles()));
    
    m_closeSpecFileAction =
    WuQtUtilities::createAction("Close All Files",
                                "Close all loaded files",
                                this,
                                this,
                                SLOT(processCloseAllFiles()));
    
    m_closeTabAction =
    WuQtUtilities::createAction("Close Tab",
                                "Close the active tab (window pane) in the window",
                                Qt::CTRL + Qt::Key_W,
                                this,
                                m_toolbar,
                                SLOT(closeSelectedTab()));
    
    m_closeWithoutConfirmationFlag = false;
    m_closeWindowActionConfirmTitle  = "Close Window...";
    m_closeWindowActionNoConfirmTitle = "Close Window";
    m_closeWindowAction =
    WuQtUtilities::createAction(m_closeWindowActionConfirmTitle,
                                "Close the window",
                                Qt::CTRL + Qt::SHIFT + Qt::Key_W,
                                this,
                                this,
                                SLOT(processCloseWindow()));
    
    m_captureImageAction =
    WuQtUtilities::createAction("Capture Image...",
                                "Capture an Image of the windows content",
                                this,
                                this,
                                SLOT(processCaptureImage()));

    m_recordMovieAction = 
    WuQtUtilities::createAction("Animation Control...",
                                "Animate Brain Surface",
                                this,
                                this,
                                SLOT(processRecordMovie()));
    
    m_preferencesAction = 
    WuQtUtilities::createAction("Preferences...",
                                "Edit the preferences",
                                this,
                                this,
                                SLOT(processEditPreferences()));
    
    m_exitProgramAction =
    WuQtUtilities::createAction("Exit", 
                                "Exit (quit) the program",
                                Qt::CTRL+Qt::Key_Q, 
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
    
    m_viewFullScreenAction = WuQtUtilities::createAction("Full Screen",
                                                         "View using all of screen",
                                                         Qt::CTRL+Qt::Key_F,
                                                         this);
    QObject::connect(m_viewFullScreenAction, SIGNAL(triggered()),
                     this, SLOT(processViewFullScreenSelected()));
    
    /*
     * Note: If shortcut key is changed, also change the shortcut key
     * for the tile tabs configuration dialog menu item to match.
     */
    m_viewTileTabsAction = WuQtUtilities::createAction("Tile Tabs",
                                                       "View all tabs in a tiled layout",
                                                       Qt::CTRL+Qt::Key_M,
                                                       this);
    QObject::connect(m_viewTileTabsAction, SIGNAL(triggered()),
                     this, SLOT(processViewTileTabs()));
    
    m_createAndEditTileTabsAction = WuQtUtilities::createAction("Create and Edit...",
                                                                "Create, Delete, and Edit Tile Tabs Configurations",
                                                                Qt::CTRL + Qt::SHIFT + Qt::Key_M,
                                                                this);

    m_gapsAndMarginsAction =
    WuQtUtilities::createAction("Gaps and Margins...",
                                "Adjust the gaps and margins",
                                this,
                                this,
                                SLOT(processGapsAndMargins()));
    
    m_nextTabAction =
    WuQtUtilities::createAction("Next Tab",
                                "Move to the next tab",
                                Qt::CTRL + Qt::Key_Right,
                                this,
                                m_toolbar,
                                SLOT(nextTab()));
    
    m_previousTabAction =
    WuQtUtilities::createAction("Previous Tab",
                                "Move to the previous tab",
                                Qt::CTRL + Qt::Key_Left,
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
    WuQtUtilities::createAction("Time Graphics Update",
                                "Show the average time for updating the windows graphics",
                                this,
                                this,
                                SLOT(processDevelopGraphicsTiming()));
    
    m_developerExportVtkFileAction = 
    WuQtUtilities::createAction("Export to VTK File",
                                "Export model(s) to VTK File",
                                this,
                                this,
                                SLOT(processDevelopExportVtkFile()));
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
    menu->addAction(m_developerExportVtkFileAction);
    
    /*
     * Hide the Export to VTK menu item
     */
    m_developerExportVtkFileAction->setVisible(false);
    
    menu->addAction(m_developerGraphicsTimingAction);
    
    std::vector<DeveloperFlagsEnum::Enum> developerFlags;
    DeveloperFlagsEnum::getAllEnums(developerFlags);
    
    m_developerFlagsActionGroup = NULL;
    
    if ( ! developerFlags.empty()) {
        menu->addSeparator();
        
        m_developerFlagsActionGroup = new QActionGroup(this);
        m_developerFlagsActionGroup->setExclusive(false);
        QObject::connect(m_developerFlagsActionGroup, SIGNAL(triggered(QAction*)),
                         this, SLOT(developerMenuFlagTriggered(QAction*)));
        
        for (std::vector<DeveloperFlagsEnum::Enum>::iterator iter = developerFlags.begin();
             iter != developerFlags.end();
             iter++) {
            const DeveloperFlagsEnum::Enum flag = *iter;
            
            QAction* action = menu->addAction(DeveloperFlagsEnum::toGuiName(flag));
            action->setCheckable(true);
            action->setData(static_cast<int>(DeveloperFlagsEnum::toIntegerCode(flag)));
            m_developerFlagsActionGroup->addAction(action);
        }
    }
    
    return menu;
}

/**
 * Called when developer menu is about to show.
 */
void
BrainBrowserWindow::developerMenuAboutToShow()
{
    std::vector<DeveloperFlagsEnum::Enum> developerFlags;
    DeveloperFlagsEnum::getAllEnums(developerFlags);
    
    QList<QAction*> actions = m_developerFlagsActionGroup->actions();
    QListIterator<QAction*> iter(actions);
    while (iter.hasNext()) {
        QAction* action = iter.next();
        const int integerCode = action->data().toInt();
        
        bool valid = false;
        const DeveloperFlagsEnum::Enum enumValue = DeveloperFlagsEnum::fromIntegerCode(integerCode,
                                                                                       &valid);
        if (valid) {
            const bool flag = DeveloperFlagsEnum::isFlag(enumValue);
            action->setChecked(flag);
        }
        else {
            CaretLogSevere("Failed to find develper flag for updating menu: "
                           + action->text());
        }
    }
}

/**
 * Called when developer flag is checked/unchecked.
 *
 * @param action
 *    Action that is checked/unchecked.
 */
void
BrainBrowserWindow::developerMenuFlagTriggered(QAction* action)
{
    const int integerCode = action->data().toInt();
    
    bool valid = false;
    const DeveloperFlagsEnum::Enum enumValue = DeveloperFlagsEnum::fromIntegerCode(integerCode,
                                                                                   &valid);
    if (valid) {
        DeveloperFlagsEnum::setFlag(enumValue,
                                    action->isChecked());

        /*
         * Update graphics and GUI
         */
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    else {
        CaretLogSevere("Failed to find develper flag for reading menu: "
                       + action->text());
    }
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
    menu->addSeparator();
    menu->addAction(m_openFileAction);
    menu->addAction(m_openLocationAction);
    
    m_recentSpecFileMenuOpenConfirmTitle = "Open Recent Spec File";
    m_recentSpecFileMenuLoadNoConfirmTitle = "Load All Files in Recent Spec File";

    m_recentSpecFileMenu = menu->addMenu(m_recentSpecFileMenuOpenConfirmTitle);
    QObject::connect(m_recentSpecFileMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processRecentSpecFileMenuAboutToBeDisplayed()));
    QObject::connect(m_recentSpecFileMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processRecentSpecFileMenuSelection(QAction*)));
    
    m_recentSceneFileMenu = menu->addMenu("Open Recent Scene File");
    QObject::connect(m_recentSceneFileMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processRecentSceneFileMenuAboutToBeDisplayed()));
    QObject::connect(m_recentSceneFileMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processRecentSceneFileMenuSelection(QAction*)));
    
    //menu->addAction(m_openFileViaSpecFileAction);
    menu->addAction(m_manageFilesAction);
    menu->addAction(m_closeSpecFileAction);
    menu->addSeparator();
    menu->addAction(m_recordMovieAction);
    menu->addAction(m_captureImageAction);
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
    
    
    QAction* selectAllAction = NULL;
    const bool addSelectAllFlag = true;
    if (addSelectAllFlag) {
        selectAllAction = addItemToEditMenu(m_editMenu,
                                            BrainBrowserWindowEditMenuItemEnum::SELECT_ALL);
    }
    
    m_editMenu->insertSeparator(cutAction);
    if (selectAllAction != NULL) {
        m_editMenu->insertSeparator(selectAllAction);
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
        m_recentSpecFileMenu->setTitle(m_recentSpecFileMenuLoadNoConfirmTitle);
    }
    else {
        m_closeWindowAction->setText(m_closeWindowActionConfirmTitle);
        m_recentSpecFileMenu->setTitle(m_recentSpecFileMenuOpenConfirmTitle);
    }
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
 * Called when Open Recent Spec File Menu is about to be displayed
 * and creates the content of the menu.
 */
void  
BrainBrowserWindow::processRecentSpecFileMenuAboutToBeDisplayed()
{
    m_recentSpecFileMenu->clear();
    
    const int32_t numRecentSpecFiles = BrainBrowserWindow::loadRecentSpecFileMenu(m_recentSpecFileMenu);
    
    if (numRecentSpecFiles > 0) {
        m_recentSpecFileMenu->addSeparator();
        QAction* action = new QAction("Clear Menu",
                                      m_recentSpecFileMenu);
        action->setData("CLEAR_CLEAR");
        m_recentSpecFileMenu->addAction(action);
    }
}

/**
 * Load a menu with recent spec files.  This method only ADDS
 * items to the menu, nothing is removed or cleared.
 *
 * @param recentSpecFileMenu
 *    Menu to which recent spec files are added.
 * @return
 *    Returns the number of recent spec files added to the menu.
 */
int32_t
BrainBrowserWindow::loadRecentSpecFileMenu(QMenu* recentSpecFileMenu)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    std::vector<AString> recentSpecFiles;
    prefs->getPreviousSpecFiles(recentSpecFiles);
    
    const int32_t numRecentSpecFiles = static_cast<int>(recentSpecFiles.size());
    for (int32_t i = 0; i < numRecentSpecFiles; i++) {
        AString actionName;
        AString actionFullPath;
        if (DataFile::isFileOnNetwork(recentSpecFiles[i])) {
            actionName     = recentSpecFiles[i];
            actionFullPath = recentSpecFiles[i];
        }
        else {
            FileInformation fileInfo(recentSpecFiles[i]);
            QString path = fileInfo.getPathName();
            QString name = fileInfo.getFileName();
            if (path.isEmpty() == false) {
                name += (" (" + path + ")");
            }
            actionName = name;
            actionFullPath = fileInfo.getAbsoluteFilePath();
        }
        
        QAction* action = new QAction(actionName,
                                      recentSpecFileMenu);
        /*
         * If this "setData()" action changes you will need to update:
         * (1) BrainBrowserWindow::processRecentSpecFileMenuSelection
         * (2) MacDockMenu::menuActionTriggered
         */
        action->setData(actionFullPath);
        recentSpecFileMenu->addAction(action);
    }
    
    return numRecentSpecFiles;
}

/**
 * Called when an item is selected from the recent spec file
 * menu.
 * @param itemAction
 *    Action of the menu item that was selected.
 */
void 
BrainBrowserWindow::processRecentSpecFileMenuSelection(QAction* itemAction)
{
    //AString errorMessages;
    
    const AString specFileName = itemAction->data().toString();
    if (specFileName == "CLEAR_CLEAR") {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->clearPreviousSpecFiles();
        return;
    }
    
    if ( ! specFileName.isEmpty()) {
        
        SpecFile specFile;
        try {
            specFile.readFile(specFileName);
            
            if (m_recentSpecFileMenu->title() == m_recentSpecFileMenuOpenConfirmTitle) {
                if (GuiManager::get()->processShowOpenSpecFileDialog(&specFile,
                                                                     this)) {
                    m_toolbar->addDefaultTabsAfterLoadingSpecFile();
                }
            }
            else if (m_recentSpecFileMenu->title() == m_recentSpecFileMenuLoadNoConfirmTitle) {
                std::vector<AString> fileNamesToLoad;
                fileNamesToLoad.push_back(specFileName);
                loadFilesFromCommandLine(fileNamesToLoad,
                                         BrainBrowserWindow::LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE);
                m_toolbar->addDefaultTabsAfterLoadingSpecFile();
            }
            else {
                CaretAssert(0);
            }
        }
        catch (const DataFileException& e) {
            //errorMessages += e.whatString();
            QMessageBox::critical(this,
                                  "ERROR",
                                  e.whatString());
            return;
        }

        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}



/**
 * Called when Open Recent Scene File Menu is about to be displayed
 * and creates the content of the menu.
 */
void
BrainBrowserWindow::processRecentSceneFileMenuAboutToBeDisplayed()
{
    m_recentSceneFileMenu->clear();
    
    const int32_t numRecentSceneFiles = BrainBrowserWindow::loadRecentSceneFileMenu(m_recentSceneFileMenu);
    
    if (numRecentSceneFiles > 0) {
        m_recentSceneFileMenu->addSeparator();
        QAction* action = new QAction("Clear Menu",
                                      m_recentSceneFileMenu);
        action->setData("CLEAR_CLEAR");
        m_recentSceneFileMenu->addAction(action);
    }
}

/**
 * Load a menu with recent scene files.  This method only ADDS
 * items to the menu, nothing is removed or cleared.
 *
 * @param recentSceneFileMenu
 *    Menu to which recent scene files are added.
 * @return
 *    Returns the number of recent scene files added to the menu.
 */
int32_t
BrainBrowserWindow::loadRecentSceneFileMenu(QMenu* recentSceneFileMenu)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    std::vector<AString> recentSceneFiles;
    prefs->getPreviousSceneFiles(recentSceneFiles);
    
    const int32_t numRecentSceneFiles = static_cast<int>(recentSceneFiles.size());
    for (int32_t i = 0; i < numRecentSceneFiles; i++) {
        AString actionName;
        AString actionFullPath;
        if (DataFile::isFileOnNetwork(recentSceneFiles[i])) {
            actionName     = recentSceneFiles[i];
            actionFullPath = recentSceneFiles[i];
        }
        else {
            FileInformation fileInfo(recentSceneFiles[i]);
            QString path = fileInfo.getPathName();
            QString name = fileInfo.getFileName();
            if (path.isEmpty() == false) {
                name += (" (" + path + ")");
            }
            actionName = name;
            actionFullPath = fileInfo.getAbsoluteFilePath();
        }
        
        QAction* action = new QAction(actionName,
                                      recentSceneFileMenu);
        /*
         * If this "setData()" action changes you will need to update:
         * (1) BrainBrowserWindow::processRecentSceneFileMenuSelection
         * (2) MacDockMenu::menuActionTriggered
         */
        action->setData(actionFullPath);
        recentSceneFileMenu->addAction(action);
    }
    
    return numRecentSceneFiles;
}

/**
 * Called when an item is selected from the recent scene file
 * menu.
 * @param itemAction
 *    Action of the menu item that was selected.
 */
void
BrainBrowserWindow::processRecentSceneFileMenuSelection(QAction* itemAction)
{
    //AString errorMessages;
    
    const AString sceneFileName = itemAction->data().toString();
    if (sceneFileName == "CLEAR_CLEAR") {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->clearPreviousSceneFiles();
        return;
    }
    
    if ( ! sceneFileName.isEmpty()) {
        std::vector<AString> filenamesVector;
        filenamesVector.push_back(sceneFileName);
        std::vector<DataFileTypeEnum::Enum> dataFileTypes;
        dataFileTypes.push_back(DataFileTypeEnum::SCENE);
        loadFiles(this,
                  filenamesVector,
                  dataFileTypes,
                  LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE,
                  "",
                  "");
        
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when view menu is about to show.
 */
void 
BrainBrowserWindow::processViewMenuAboutToShow()
{
    m_viewMoveFeaturesToolBoxMenu->setEnabled(isFullScreen() == false);
    m_viewMoveOverlayToolBoxMenu->setEnabled(isFullScreen()== false);
    
    if (isFullScreen()) {
        m_viewFullScreenAction->setText("Exit Full Screen");
    }
    else {
        m_viewFullScreenAction->setText("Enter Full Screen");
    }
    
    if (isTileTabsSelected()) {
        m_viewTileTabsAction->setText("Exit Tile Tabs");
    }
    else {
        m_viewTileTabsAction->setText("Enter Tile Tabs");
    }
}

/**
 * Create the view menu.
 * @return the view menu.
 */
QMenu* 
BrainBrowserWindow::createMenuView()
{
    m_tileTabsMenu = new QMenu("Tile Tabs Configuration");
    QObject::connect(m_tileTabsMenu, SIGNAL(aboutToShow()),
                     this, SLOT(processTileTabsMenuAboutToBeDisplayed()));
    QObject::connect(m_tileTabsMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(processTileTabsMenuSelection(QAction*)));
    
    m_tileTabsMenu->addAction(m_createAndEditTileTabsAction);
    m_tileTabsMenu->addSeparator();
    
    QMenu* menu = new QMenu("View", this);
    QObject::connect(menu, SIGNAL(aboutToShow()),
                     this, SLOT(processViewMenuAboutToShow()));
    
    m_viewMoveFeaturesToolBoxMenu = createMenuViewMoveFeaturesToolBox();
    m_viewMoveOverlayToolBoxMenu = createMenuViewMoveOverlayToolBox();
    
    menu->addAction(m_showToolBarAction);
    menu->addMenu(m_viewMoveFeaturesToolBoxMenu);
    menu->addMenu(m_viewMoveOverlayToolBoxMenu);
    menu->addSeparator();

    menu->addAction(m_viewFullScreenAction);
    menu->addAction(m_viewTileTabsAction);
    menu->addSeparator();
    menu->addAction(m_gapsAndMarginsAction);
    menu->addMenu(m_tileTabsMenu);
    
    return menu;
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
 * Update the Tile Tabs Configuration Menu just before it is displayed.
 */
void
BrainBrowserWindow::processTileTabsMenuAboutToBeDisplayed()
{
    /*
     * Remove all actions for user-defined tile tabs configurations
     */
    QList<QAction*> menuActions = m_tileTabsMenu->actions();
    QListIterator<QAction*> actionIterator(menuActions);
    while (actionIterator.hasNext()) {
        QAction* action = actionIterator.next();
        if (action != m_createAndEditTileTabsAction) {
            if (action->isSeparator() == false) {
                m_tileTabsMenu->removeAction(action);
                delete action;
            }
        }
    }
    
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    preferences->readTileTabsConfigurations();
    std::vector<const TileTabsConfiguration*> configurations = preferences->getTileTabsConfigurationsSortedByName();
    const int32_t numConfigurations = static_cast<int32_t>(configurations.size());
    
    /*
     * Add All Tabs (Default) tile tabs configuration
     */
    QAction* defaultAction = m_tileTabsMenu->addAction(m_defaultTileTabsConfiguration->getName());
    defaultAction->setCheckable(true);
    defaultAction->setData(QVariant(m_defaultTileTabsConfiguration->getUniqueIdentifier()));
    
    /*
     * Add the scene configuration
     */
    QAction* sceneAction = m_tileTabsMenu->addAction(m_sceneTileTabsConfiguration->getName());
    sceneAction->setCheckable(true);
    sceneAction->setData(QVariant(m_sceneTileTabsConfiguration->getUniqueIdentifier()));
    //sceneAction->setEnabled(false);
    
    bool haveSelectedTileTabsConfiguration = false;
    
    /*
     * Add the user defined Tile Tabs configurations
     */
    if (numConfigurations > 0) {
        m_tileTabsMenu->addSeparator();

        for (int32_t i = 0; i < numConfigurations; i++) {
            QAction* action = m_tileTabsMenu->addAction(configurations[i]->getName());
            action->setCheckable(true);
            
            if (m_selectedTileTabsConfigurationUniqueIdentifier == configurations[i]->getUniqueIdentifier()) {
                action->setChecked(true);
                haveSelectedTileTabsConfiguration = true;
            }
            action->setData(QVariant(configurations[i]->getUniqueIdentifier()));
        }
    }
    
    if (m_selectedTileTabsConfigurationUniqueIdentifier == m_sceneTileTabsConfiguration->getUniqueIdentifier()) {
        haveSelectedTileTabsConfiguration = true;
        sceneAction->setChecked(true);
    }
    
    if ( ! haveSelectedTileTabsConfiguration) {
        defaultAction->setChecked(true);
        m_selectedTileTabsConfigurationUniqueIdentifier = m_defaultTileTabsConfiguration->getUniqueIdentifier();
    }
}

/**
 * Process a selection from the Tile Tab Configuration Menu.
 *
 * @param action
 *    Action that was selected.
 */
void
BrainBrowserWindow::processTileTabsMenuSelection(QAction* action)
{
    if (action == m_createAndEditTileTabsAction) {
        processViewTileTabsConfigurationDialog();
    }
    else {
        m_selectedTileTabsConfigurationUniqueIdentifier = action->data().toString();
        
        if (isTileTabsSelected()) {
            EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
        }
    }
}

/*
 * Set the selected tile tabs configuration.  If requested configuration is
 * NULL, the default configuration will be selected.
 *
 * @param configuration
 *    New selection for tile tabs configuration.
 */
void
BrainBrowserWindow::setSelectedTileTabsConfiguration(TileTabsConfiguration* configuration)
{
    if (configuration != NULL) {
        m_selectedTileTabsConfigurationUniqueIdentifier = configuration->getUniqueIdentifier();
    }
    else {
        m_selectedTileTabsConfigurationUniqueIdentifier = m_defaultTileTabsConfiguration->getUniqueIdentifier();
    }
}

/**
 * @return The selected tile tabs configuration.
 */
TileTabsConfiguration*
BrainBrowserWindow::getSelectedTileTabsConfiguration()
{

    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    TileTabsConfiguration* configuration = preferences->getTileTabsConfigurationByUniqueIdentifier(m_selectedTileTabsConfigurationUniqueIdentifier);
    if (configuration != NULL) {
        /*
         * A user configuration is selected
         */
        return configuration;
    }
    else if (m_selectedTileTabsConfigurationUniqueIdentifier == m_defaultTileTabsConfiguration->getUniqueIdentifier()) {
        /*
         * Default configuration is selected
         */
        return m_defaultTileTabsConfiguration;
    }
    else if (m_selectedTileTabsConfigurationUniqueIdentifier == m_sceneTileTabsConfiguration->getUniqueIdentifier()) {
        /*
         * Scene's configuration is selected
         */
        return m_sceneTileTabsConfiguration;
    }
    else {
        /*
         * The selected configuration has been deleted, likely in another browser window
         * so selecte the default configuration
         */
        m_selectedTileTabsConfigurationUniqueIdentifier = m_defaultTileTabsConfiguration->getUniqueIdentifier();
    }
    
    return m_defaultTileTabsConfiguration;
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
    
    menu->addAction("Information...", 
                    this, 
                    SLOT(processSurfaceMenuInformation()));
    
    menu->addAction("Properties...",
                    this,
                    SLOT(processShowSurfacePropertiesDialog()));
    
    menu->addAction("Primary Anatomical...", 
                    this, 
                    SLOT(processSurfaceMenuPrimaryAnatomical()));
    
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
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
//    QMenu* menu = new QMenu("Volume", this);
//    return menu;
    return NULL;
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
    
    menu->addAction(m_windowAspectRatioLockedAction);
    menu->addAction(m_tabAspectRatioLockedAction);
    menu->addAction(m_lockAllTabsAspectRatioAction);
    menu->addAction(m_unlockAllTabsAspectRatioAction);
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
    
    return menu;
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
    
    const int32_t numTimes = 5;
    for (int32_t i = 0; i < numTimes; i++) {
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    }
    
    const float time = et.getElapsedTimeSeconds() / numTimes;
    const AString timeString = AString::number(time, 'f', 5);
    
    const AString msg = ("Time to draw graphics (seconds): "
                         + timeString);
    WuQMessageBox::informationOk(this, msg);
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
            
            CaretFileDialog cfd(this,
                                "Export to VTK File",
                                GuiManager::get()->getBrain()->getCurrentDirectory(),
                                vtkSurfaceFileFilter);
            cfd.selectFilter(vtkSurfaceFileFilter);
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
    //std::cout << "Toolbar height " << m_toolbar->height() << std::endl;
    GuiManager::get()->processShowImageCaptureDialog(this);
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
 * Called when record movie is selected.
 */
void 
BrainBrowserWindow::processRecordMovie()
{
    GuiManager::get()->processShowMovieDialog(this);
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
    prefs->setBackgroundAndForegroundColorsMode(BackgroundAndForegroundColorsModeEnum::USER_PREFERENCES);
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
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

    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW, 
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
    EventManager::get()->blockEvent(EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW, 
                                    false);
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(newWindowIndex).getPointer());
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
 * Called when open data file is selected.
 */
void 
BrainBrowserWindow::processDataFileOpen()
{
    if (s_previousOpenFileNameFilter.isEmpty()) {
        s_previousOpenFileNameFilter = 
            DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::SPECIFICATION);
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
        AString filterName = DataFileTypeEnum::toQFileDialogFilter(*iter);
        filenameFilterList.append(filterName);
    }
    
    /*
     * Setup file selection dialog.
     */
    CaretFileDialog fd(this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(CaretFileDialog::ExistingFiles);
    fd.setViewMode(CaretFileDialog::List);
    fd.selectNameFilter(s_previousOpenFileNameFilter);
    if (s_previousOpenFileDirectory.isEmpty() == false) {
        FileInformation fileInfo(s_previousOpenFileDirectory);
        if (fileInfo.exists()) {
            fd.setDirectory(s_previousOpenFileDirectory);
        }
    }
    
    if ( ! s_previousOpenFileGeometry.isEmpty()) {
        fd.restoreGeometry(s_previousOpenFileGeometry);
    }
    
    AString errorMessages;
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        QStringList selectedFiles = fd.selectedFiles();
        if (selectedFiles.empty() == false) {            
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
        }
        s_previousOpenFileNameFilter = fd.selectedNameFilter();
        s_previousOpenFileDirectory  = fd.directory().absolutePath();
        s_previousOpenFileGeometry   = fd.saveGeometry();
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
    
    loadFiles(this,
              filenames,
              dataFileTypesDummyNotUsed,
              loadSpecFileMode,
              "",
              "");
}

/**
 * Load the scene file and the scene with the given name or number
 * @param sceneFileName
 *    Name of scene file.
 * @param sceneNameOrNumber
 *    Name or number of scene.  Name takes precedence over number. 
 *    Scene numbers start at one.
 */
void
BrainBrowserWindow::loadSceneFromCommandLine(const AString& sceneFileName,
                                             const AString& sceneNameOrNumber)
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
        if (sf->getFileName().contains(sceneFileName)) {
            haveSceneFileError = false;
            Scene* scene = sf->getSceneWithName(sceneNameOrNumber);
            if (scene == NULL) {
                bool isValidNumber = false;
                int sceneNumber = sceneNameOrNumber.toInt(&isValidNumber);
                if (isValidNumber) {
                    sceneNumber--;  // convert to index (numbers start at one)
                    if ((sceneNumber >= 0)
                        && (sceneNumber < sf->getNumberOfScenes())) {
                        scene = sf->getSceneAtIndex(sceneNumber);
                    }
                }
            }
            
            if (scene != NULL) {
                GuiManager::get()->processShowSceneDialogAndScene(this,
                                                                  sf,
                                                                  scene);
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
                timer.reset(); // resets timer
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
        successFlag = false;
        QMessageBox::critical(parentWidget, 
                              "ERROR", 
                              errorMessages);
    }
    
    if (sceneFileWasLoaded) {
        GuiManager::get()->processShowSceneDialog(this);
    }
    
    EventManager::get()->sendEvent(EventMacDockMenuUpdate().getPointer());
    
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
        m_showToolBarAction->setEnabled(false);
        m_overlayToolBoxAction->setChecked(true);
        m_overlayToolBoxAction->trigger();
        m_overlayToolBoxAction->setEnabled(false);
        m_featuresToolBoxAction->setChecked(true);
        m_featuresToolBoxAction->trigger();
        m_featuresToolBoxAction->setEnabled(false);
        
        showFullScreen();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBar().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}


/**
 * Called when view full screen is selected and toggles the status of full screen.
 */
void
BrainBrowserWindow::processViewFullScreenSelected()
{
    const bool toggledStatus = (! isFullScreen());
    processViewFullScreen(toggledStatus,
                          true);
}

/**
 * Called when tile tabs is selected and toggles the state of tile tabs.
 */
void
BrainBrowserWindow::processViewTileTabs()
{
    const bool toggledStatus = (! m_viewTileTabsSelected);
    
    setViewTileTabs(toggledStatus);
}

/**
 * Set the status of tile tabs.
 */
void
BrainBrowserWindow::setViewTileTabs(const bool newStatus)
{
    m_viewTileTabsSelected = newStatus;
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().setWindowIndex(m_browserWindowIndex).addToolBar().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
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
 * Restore the status of window components.
 * @param wcs
 *    Window component status that is restored.
 */
void 
BrainBrowserWindow::restoreWindowComponentStatus(const WindowComponentStatus& wcs)
{
    if (wcs.windowGeometry.isEmpty() == false) {
        restoreGeometry(wcs.windowGeometry);
    }
    if (wcs.windowState.isEmpty() == false) {
        restoreState(wcs.windowState);
    }
    
    m_showToolBarAction->setEnabled(true);
    if (wcs.isToolBarDisplayed) {
        m_showToolBarAction->setChecked(false);
        m_showToolBarAction->trigger();
    }
    else {
        m_showToolBarAction->setChecked(true);
        m_showToolBarAction->trigger();
    }
    
    m_overlayToolBoxAction->setEnabled(true);
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
    
    m_featuresToolBoxAction->setEnabled(true);
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
    wcs.windowGeometry = saveGeometry();
    if (m_featuresToolBoxAction->isChecked()) {
        if (m_featuresToolBox != NULL) {
            wcs.featuresGeometry = m_featuresToolBox->saveGeometry();
        }
    }
    wcs.isToolBarDisplayed = m_showToolBarAction->isChecked();
    wcs.isOverlayToolBoxDisplayed = m_overlayToolBoxAction->isChecked();
    wcs.isFeaturesToolBoxDisplayed  = m_featuresToolBoxAction->isChecked();
    m_showToolBarAction->setEnabled(false);
    m_overlayToolBoxAction->setEnabled(false);
    m_featuresToolBoxAction->setEnabled(false);
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
    m_toolbar->addNewTabCloneContent(previousTabContent);
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
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
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
        toNewWindowAction->setData(qVariantFromValue((void*)NULL));
        m_moveSelectedTabToWindowMenu->addAction(toNewWindowAction);
    }
    
    std::vector<BrainBrowserWindow*> browserWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    for (int32_t i = 0; i < static_cast<int32_t>(browserWindows.size()); i++) {
        if (browserWindows[i] != this) {
            QAction* action = new QAction(browserWindows[i]->windowTitle(),
                                          m_moveSelectedTabToWindowMenu);
            action->setData(qVariantFromValue((void*)browserWindows[i]));
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
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);

    sceneClass->addInteger("m_browserWindowIndex",
                           m_browserWindowIndex);
    sceneClass->addBoolean("m_windowAspectRatioLockedAction",
                           m_windowAspectRatioLockedAction->isChecked());

    /*
     * Save the selected tile tabs configuration as the scene configuration
     */
    if (m_viewTileTabsSelected) {
        /*
         * Note: The number of rows and columns in the default tile tabs
         * configuration is updated each time the graphics region of the 
         * window is drawn in BrainOpenGLWidget::paintGL().
         */
        const TileTabsConfiguration* tileTabs = getSelectedTileTabsConfiguration();
        if (tileTabs != NULL) {
            TileTabsConfiguration writeConfig(*tileTabs);
            writeConfig.setName(QUuid::createUuid().toString());
            sceneClass->addString("m_sceneTileTabsConfiguration",
                                  writeConfig.encodeInXML());
            
//            sceneClass->addString("m_sceneTileTabsConfiguration",
//                                  tileTabs->encodeInXML());
        }
    }
    
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
    sceneClass->addBoolean("m_viewTileTabsAction",
                           m_viewTileTabsSelected);
    
    /*
     * Graphics position and size
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
    
    const bool aspectRatioLocked = sceneClass->getBooleanValue("m_windowAspectRatioLockedAction",
                                                               false);
    m_windowAspectRatioLockedAction->blockSignals(true);
    m_windowAspectRatioLockedAction->setChecked(aspectRatioLocked);
    m_windowAspectRatioLockedAction->blockSignals(false);
    
    /*
     * Restore Unique ID of selected tile tabs configuration.
     * If not valid, use default configuration
     */
    m_selectedTileTabsConfigurationUniqueIdentifier = sceneClass->getStringValue("m_selectedTileTabsConfigurationUniqueIdentifier",
                                                                                 "");
    CaretPreferences* caretPreferences = SessionManager::get()->getCaretPreferences();
    TileTabsConfiguration* selectedConfiguration = caretPreferences->getTileTabsConfigurationByUniqueIdentifier(m_selectedTileTabsConfigurationUniqueIdentifier);
    if (selectedConfiguration == NULL) {
        m_selectedTileTabsConfigurationUniqueIdentifier = m_defaultTileTabsConfiguration->getUniqueIdentifier();
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
        restoreToTabTiles = sceneClass->getBooleanValue("m_viewTileTabsAction",
                                                          false);
        
        /*
         * If tile tabs was saved to the scene, restore it as the scenes tile tabs configuration
         */
        if (restoreToTabTiles) {
            const AString tileTabsConfigString = sceneClass->getStringValue("m_sceneTileTabsConfiguration");
            if ( ! tileTabsConfigString.isEmpty()) {
                m_sceneTileTabsConfiguration->decodeFromXML(tileTabsConfigString);
                m_sceneTileTabsConfiguration->setName(m_sceneTileTabsConfigurationText
                                                      + " "
                                                      + sceneAttributes->getSceneName());
                m_selectedTileTabsConfigurationUniqueIdentifier = m_sceneTileTabsConfiguration->getUniqueIdentifier();
            }
        }
    }

    m_normalWindowComponentStatus = m_defaultWindowComponentStatus;
    processViewFullScreen(restoreToFullScreen,
                          false);
    
    setViewTileTabs(restoreToTabTiles);
    
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
}

///**
// * Get the viewport size for the window.
// *
// * @param w
// *    Output width.
// * @param h
// *    Output height.
// */
//void
//BrainBrowserWindow::getViewportSize(int &w, int &h)
//{
//    m_openGLWidget->getViewPortSize(w,h);
//}

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




