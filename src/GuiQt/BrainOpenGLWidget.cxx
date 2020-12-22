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

#define __BRAIN_OPENGL_WIDGET_DEFINE__
#include "BrainOpenGLWidget.h"
#undef __BRAIN_OPENGL_WIDGET_DEFINE__

#include <algorithm>
#include <cmath>

#include <QApplication>
#include <QContextMenuEvent>
#include <QGestureEvent>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
#endif
#include <QOpenGLContext>
#include <QTimer>
#include <QToolTip>
#include <QWheelEvent>

#include "AnnotationManager.h"
#include "Border.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLShape.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CursorManager.h"
#include "DataToolTipsManager.h"
#include "DeveloperFlagsEnum.h"
#include "DummyFontTextRenderer.h"
#include "ElapsedTimer.h"
#include "EventBrainReset.h"
#include "EventImageCapture.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "EventBrowserWindowDrawingContent.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventGraphicsTimingOneWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGraphicsWindowShowToolTip.h"
#include "EventIdentificationRequest.h"
#include "EventMovieManualModeRecording.h"
#include "EventUserInterfaceUpdate.h"
#include "EventUserInputModeGet.h"
#include "FtglFontTextRenderer.h"
#include "GestureEvent.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "KeyEvent.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "MouseEvent.h"
#include "MovieRecorder.h"
#include "OffScreenOpenGLRenderer.h"
#include "SelectionManager.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxelEditing.h"
#include "SessionManager.h"
#include "Surface.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsLayoutManualConfiguration.h"
#include "UserInputModeAnnotations.h"
#include "UserInputModeBorders.h"
#include "UserInputModeFoci.h"
#include "UserInputModeImage.h"
#include "UserInputModeTileTabsManualLayout.h"
#include "UserInputModeView.h"
#include "UserInputModeVolumeEdit.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"

using namespace caret;

/**
 * Constructor.
 * 
 * NOTE: We do not use "sharing" of textures and display lists 
 * between windows.  There are some issues with sharing and
 * recreation of OpenGL contexts such as when renderPixmap()
 * function is used.  
 *
 * @param
 *   The parent widget.
 * @param shareWidget
 *   Widget used for sharing OpenGL contexts when the deprecated
 *   QGLWidget is used.  Note that with QOpenGLWidget,
 *   sharing is enabled by calling QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts)
 *   in desktop.cxx
 * @param windowIndex
 *   Index of this window
 */
BrainOpenGLWidget::BrainOpenGLWidget(QWidget* parent,
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
                                     const BrainOpenGLWidget* /*shareWidget*/,
#else
                                     const BrainOpenGLWidget* shareWidget,
#endif
                                     const int32_t windowIndex)
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
: QOpenGLWidget(parent),
#else
: QGLWidget(parent,
            shareWidget),
#endif
windowIndex(windowIndex)
{
    setObjectName("Window_"
                  + AString::number(windowIndex + 1)
                  + ":OpenGLWidget");

    m_mousePositionValid = false;
    m_mousePositionEvent.grabNew(new MouseEvent(NULL,
                                                NULL,
                                                -1,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                0,
                                                m_mouseHistoryXY,
                                                false));
    
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    this->mouseNewDraggingStartedFlag = false;
    
    m_newKeyPressStartedFlag = true;
    
    /*
     * Mouse tracking must be on to receive mouse move events
     * when the mouse is NOT down.  When this property is false
     * mouse move events are only received when the at least
     * one mouse button is down.
     */
    setMouseTracking(true);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BRAIN_RESET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_TIMING_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_IDENTIFICATION_REQUEST);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_IMAGE_CAPTURE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MOVIE_RECORDING_MANUAL_MODE_CAPTURE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    m_openGLContextSharingValid = true;
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Default format is set by desktop.cxx call to initializeDefaultGLFormat().
     */
    setFormat(QSurfaceFormat::defaultFormat());
#else
    /*
     * Test to see if OpenGL context sharing is valid.
     * If there is no sharingWidget, then this is the
     * first window opened.
     */
    if (shareWidget != NULL) {
        if ( ! isSharing()) {
            m_openGLContextSharingValid = false;
        }
    }
#endif
    
//    std::cout << "Share widget: " << std::hex << (uint64_t)shareWidget
//    << ", Context Pointer: " << std::hex << (uint64_t)context()
//    << ", Share Group: " << std::hex << (uint64_t)context()->contextHandle()->shareGroup()
//    << ", Sharing enabled: " << (isSharing() ? "Yes" : "No")
//    << std::endl;
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /* Context is not yet valid for QOpenGLWidget */
    m_contextShareGroupPointer = NULL;
#else
    CaretAssert(context());
    CaretAssert(context()->contextHandle());
    m_contextShareGroupPointer = context()->contextHandle()->shareGroup();
    CaretAssert(m_contextShareGroupPointer);
#endif
    
    s_brainOpenGLWidgets.insert(this);

    /*
     * Get pinch gesture event
     */
    grabGesture(Qt::PinchGesture);
}

/**
 * Destructor.
 */
BrainOpenGLWidget::~BrainOpenGLWidget()
{
    makeCurrent();
    
    this->clearDrawingViewportContents();
    
    EventManager::get()->removeAllEventsFromListener(this);
    
    s_brainOpenGLWidgets.erase(this);
    if (s_brainOpenGLWidgets.empty()) {
        if (s_singletonOpenGL != NULL) {
            delete s_singletonOpenGL;
            s_singletonOpenGL = NULL;
        }
    }
}

/**
 * @return True if OpenGL Context Sharing is valid among
 * multiple graphics windows.
 * Note: If there is one window, we declare sharing valid.
 */
bool
BrainOpenGLWidget::isOpenGLContextSharingValid() const
{
    return m_openGLContextSharingValid;
}


/**
 * Initializes graphics.
 */
void 
BrainOpenGLWidget::initializeGL()
{
    if (s_singletonOpenGL == NULL) {
        /*
         * OpenGL drawing will take ownership of the text renderer
         * and handle deletion of the text renderer.
         */
        BrainOpenGLTextRenderInterface* textRenderer = NULL;
#ifdef HAVE_FREETYPE
        textRenderer = new FtglFontTextRenderer();
        if (! textRenderer->isValid()) {
            delete textRenderer;
            textRenderer = NULL;
            CaretLogWarning("Unable to create FTGL Font Renderer.\n"
                            "No text will be available in graphics window.");
        }
#else
        CaretLogWarning("Unable to create FTGL Font Renderer due to FreeType not found during configuration.\n"
                        "No text will be available in graphics window.");
#endif
        if (textRenderer == NULL) {
            textRenderer = new DummyFontTextRenderer();
        }
        CaretAssert(textRenderer);
        
        s_singletonOpenGL = new BrainOpenGLFixedPipeline(textRenderer);
    }
    
    s_singletonOpenGL->initializeOpenGL();
    
    this->lastMouseX = 0;
    this->lastMouseY = 0;
    this->isMousePressedNearToolBox = false;

    this->setFocusPolicy(Qt::StrongFocus);
    

    CaretLogConfig(getOpenGLInformation());
    
    if (s_defaultGLFormatInitialized == false) {
        CaretLogSevere("PROGRAM ERROR: The default OpenGL has not been set.\n"
                       "Need to call BrainOpenGLWidget::initializeDefaultGLFormat() prior to "
                       "instantiating an instance of this class.");
    }
}

/**
 * @return Information about OpenGL.
 */
QString
BrainOpenGLWidget::getOpenGLInformation()
{
    AString info;
#if QT_VERSION >= 0x050000
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    info += ("Rendering with Qt5 QOpenGLWidget.\n");
#else
    info += ("Rendering with Qt5 QGLWidget (deprecated).\n");
#endif
    info += ("\n");
#endif
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    QSurfaceFormat format = this->format();
    AString swapInfo("Swap Behavior: ");
    switch (format.swapBehavior()) {
        case QSurfaceFormat::DefaultSwapBehavior:
            swapInfo += ("DefaultSwapBehavior");
            break;
        case QSurfaceFormat::SingleBuffer:
            swapInfo += ("SingleBuffer");
            break;
        case QSurfaceFormat::DoubleBuffer:
            swapInfo += ("DoubleBuffer");
            break;
        case QSurfaceFormat::TripleBuffer:
            swapInfo += ("TripleBuffer");
            break;
    }

    info += ("QOpenGLWidget Context:"
             "\n   Alpha: " + AString::fromBool(format.hasAlpha())
             + "\n   Alpha size: " + AString::number(format.alphaBufferSize())
             + "\n   Depth size: " + AString::number(format.depthBufferSize())
             + "\n   Red size: " + AString::number(format.redBufferSize())
             + "\n   Green size: " + AString::number(format.greenBufferSize())
             + "\n   Blue size: " + AString::number(format.blueBufferSize())
             + "\n   Samples size: " + AString::number(format.samples())
             + "\n   Stencil size: " + AString::number(format.stencilBufferSize())
             + "\n   " + swapInfo
             + "\n   Swap Interval: " + AString::number(format.swapInterval())
             + "\n   Stereo: " + AString::fromBool(format.stereo())
             + "\n   Major Version: " + AString::number(format.majorVersion())
             + "\n   Minor Version: " + AString::number(format.minorVersion()));
    
    switch (format.profile()) {
        case QSurfaceFormat::NoProfile:
            info += ("\nOpenGL NoProfile - OpenGL version is lower than 3.2. For 3.2 and newer this is same as CoreProfile.");
            break;
        case QSurfaceFormat::CoreProfile:
            info += ("\nOpenGL CoreProfile - Functionality deprecated in OpenGL version 3.0 is not available.");
            break;
        case QSurfaceFormat::CompatibilityProfile:
            info += ("\nOpenGL CompatibilityProfile - Functionality from earlier OpenGL versions is available.");
            break;
    }
    
#else
    QGLFormat format = this->format();
    info += ("QGLWidget Context:"
             "\n   Accum: " + AString::fromBool(format.accum())
             + "\n   Accum size: " + AString::number(format.accumBufferSize())
             + "\n   Alpha: " + AString::fromBool(format.alpha())
             + "\n   Alpha size: " + AString::number(format.alphaBufferSize())
             + "\n   Depth: " + AString::fromBool(format.depth())
             + "\n   Depth size: " + AString::number(format.depthBufferSize())
             + "\n   Direct Rendering: " + AString::fromBool(format.directRendering())
             + "\n   Red size: " + AString::number(format.redBufferSize())
             + "\n   Green size: " + AString::number(format.greenBufferSize())
             + "\n   Blue size: " + AString::number(format.blueBufferSize())
             + "\n   Double Buffer: " + AString::fromBool(format.doubleBuffer())
             + "\n   RGBA: " + AString::fromBool(format.rgba())
             + "\n   Samples: " + AString::fromBool(format.sampleBuffers())
             + "\n   Samples size: " + AString::number(format.samples())
             + "\n   Stencil: " + AString::fromBool(format.stencil())
             + "\n   Stencil size: " + AString::number(format.stencilBufferSize())
             + "\n   Swap Interval: " + AString::number(format.swapInterval())
             + "\n   Stereo: " + AString::fromBool(format.stereo())
             + "\n   Major Version: " + AString::number(format.majorVersion())
             + "\n   Minor Version: " + AString::number(format.minorVersion()));
#endif
    
    info += ("\n\n" + s_singletonOpenGL->getOpenGLInformation());
    
#if BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    int32_t numDisplayLists = 0;
    for (GLuint iList = 1; iList < 1000; iList++) {
        if (glIsList(iList)) {
            numDisplayLists++;
        }
    }
    info += ("\nAt least "
             + AString::number(numDisplayLists)
             + " display lists are allocated in first OpenGL context.");
#endif // BRAIN_OPENGL_INFO_SUPPORTS_DISPLAY_LISTS
    info += "\n";
    
    return std::move(info);
}

/**
 * Called when widget is resized.
 */
void 
BrainOpenGLWidget::resizeGL(int w, int h)
{
    this->windowWidth[this->windowIndex] = w;
    this->windowHeight[this->windowIndex] = h;
}

/**
 * Clear the contents for drawing into the viewports.
 */
void 
BrainOpenGLWidget::clearDrawingViewportContents()
{
    m_windowContent.clear();
}

/**
 * Update the cursor from the active user input processor.
 */
void
BrainOpenGLWidget::updateCursor()
{
    /*
     * Set the cursor to that requested by the user input processor
     */
    CursorEnum::Enum cursor = getSelectedInputProcessor()->getCursor();
    
    GuiManager::get()->getCursorManager()->setCursorForWidget(this,
                                                              cursor);
}

/**
 * Perform experimental off screen image capture.
 */
QImage
BrainOpenGLWidget::performOffScreenImageCapture(const int32_t imageWidth,
                                                const int32_t imageHeight)
{
    makeCurrent();
    
    QImage image;
    
    OffScreenOpenGLRenderer offscreen(this,
                                      imageWidth,
                                      imageHeight);
    if (offscreen.isError()) {
        WuQMessageBox::errorOk(this,
                               offscreen.getErrorMessage());
        doneCurrent();
        return image;
    }

    const int32_t viewport[4] = { 0, 0, imageWidth, imageHeight };
    
    BrainOpenGLWindowContent windowContent;
    getDrawingWindowContent(viewport,
                            windowContent);
    
    s_singletonOpenGL->drawModels(this->windowIndex,
                                  getSelectedInputMode(),
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  windowContent.getAllTabViewports());
    
    if (offscreen.isError()) {
        WuQMessageBox::errorOk(this,
                               offscreen.getErrorMessage());
        doneCurrent();
        return image;
    }
    
    return offscreen.getImage();
}

/**
 * Get the content for drawing the window.
 * 
 * @param windowViewport
 *     Viewport of the window.
 * @param windowContent
 *     Window content that is updated with window and its tabs.
 */
void
BrainOpenGLWidget::getDrawingWindowContent(const int32_t windowViewportIn[4],
                                           BrainOpenGLWindowContent& windowContent) const
{
    windowContent.clear();
    
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
    
    /*
     * When restoring scene, the first browser window may get closed.
     * This occurs when the user creates a scene that does not contain
     * the first browser window.  The close() method in QWindow may not execute
     * destruction of the window so this widget may still be valid but
     * GuiManager no longer has a pointer to the window since it is being closed.
     * In this case, the window will be NULL.
     *
     * This problem is related to the moving window properties to the SessionManager,
     * in Feb 2018.
     */
    if (bbw == NULL) {
        return;
    }
    
    EventBrowserWindowDrawingContent getModelEvent(this->windowIndex);
    EventManager::get()->sendEvent(getModelEvent.getPointer());
    
    if (getModelEvent.isError()) {
        return;
    }
    
    const int32_t windowViewportBeforeAspectLocking[4] = {
        windowViewportIn[0],
        windowViewportIn[1],
        windowViewportIn[2],
        windowViewportIn[3]
    };
    
    int32_t windowViewport[4] = {
        windowViewportIn[0],
        windowViewportIn[1],
        windowViewportIn[2],
        windowViewportIn[3]
    };
    
    if (bbw->isWindowAspectRatioLocked()) {
        const float aspectRatio = bbw->getAspectRatio();
        if (aspectRatio > 0.0) {
            BrainOpenGLViewportContent::adjustViewportForAspectRatio(windowViewport,
                                                                     aspectRatio);
        }
    }
    
    UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
    
    /*
     * Highlighting of border points
     */
    s_singletonOpenGL->setDrawHighlightedEndPoints(false);
    if (inputProcessor->getUserInputMode() == UserInputModeEnum::Enum::BORDERS) {
        UserInputModeBorders* borderInputProcessor = dynamic_cast<UserInputModeBorders*>(inputProcessor);
        CaretAssert(borderInputProcessor);
        s_singletonOpenGL->setDrawHighlightedEndPoints(borderInputProcessor->isHighlightBorderEndPoints());
    }
    
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    
    const int32_t numberOfTabs = getModelEvent.getNumberOfBrowserTabs();
    std::vector<BrowserTabContent*> allTabs;
    for (int32_t i = 0; i < numberOfTabs; i++) {
        allTabs.push_back(getModelEvent.getBrowserTab(i));
    }
    
    BrowserWindowContent* browserWindowContent = getModelEvent.getBrowserWindowContent();
    CaretAssert(browserWindowContent);

    /*
     * Prior to WB-859 Manual Tile Tabs Layout System: BrainOpenGLViewportContent::createViewportContentForTileTabs() was only
     * called IF tile tabs was enabled AND there was more than one tab.  However, we want a manual layout to function correctly,
     * even if there is only one tab.  So when tile tabs enabled and one tab, draw as single tab for Automatic Grid and Custom Grid
     * but draw using tile tabs for a Manual Configuration.
     */
    bool drawUsingTileTabsFlag(false);
    if (browserWindowContent->isTileTabsEnabled()) {
        if (numberOfTabs > 1) {
            drawUsingTileTabsFlag = true;
        }
        else if (numberOfTabs == 1) {
            switch (browserWindowContent->getTileTabsConfigurationMode()) {
                case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                    break;
                case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                    break;
                case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                    drawUsingTileTabsFlag = true;
                    break;
            }
        }
    }
    
    if (drawUsingTileTabsFlag) {
        const int32_t windowWidth  = windowViewport[2];
        const int32_t windowHeight = windowViewport[3];
        
        const TileTabsLayoutConfigurationTypeEnum::Enum tileTabsConfigType = browserWindowContent->getTileTabsConfigurationMode();
        switch (tileTabsConfigType) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            {
                std::vector<int32_t> rowHeights;
                std::vector<int32_t> columnsWidths;
                
                /*
                 * Determine if default configuration for tiles
                 */
                TileTabsLayoutBaseConfiguration* tileTabsConfiguration = browserWindowContent->getSelectedTileTabsGridConfiguration();
                CaretAssert(tileTabsConfiguration);
                
                TileTabsLayoutGridConfiguration* gridConfiguration = tileTabsConfiguration->castToGridConfiguration();
                CaretAssert(gridConfiguration);
                if (gridConfiguration != NULL) {
                    /*
                     * Get the sizes of the tab tiles from the tile tabs configuration
                     */
                    if (gridConfiguration->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                                     windowHeight,
                                                                                     numberOfTabs,
                                                                                     browserWindowContent->getTileTabsConfigurationMode(),
                                                                                     rowHeights,
                                                                                     columnsWidths)) {
                        
                        /*
                         * Create the viewport drawing contents for all tabs
                         */
                        std::vector<BrainOpenGLViewportContent*> tabViewportContent = BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabs,
                                                                                                                                                   browserWindowContent,
                                                                                                                                                   gapsAndMargins,
                                                                                                                                                   windowViewportBeforeAspectLocking,
                                                                                                                                                   windowViewport,
                                                                                                                                                   this->windowIndex,
                                                                                                                                                   getModelEvent.getTabIndexForTileTabsHighlighting());
                        for (auto tabvp : tabViewportContent) {
                            windowContent.addTabViewport(tabvp);
                        }
                    }
                    else {
                        CaretLogSevere("Tile Tabs Row/Column sizing failed !!!");
                    }
                }
            }
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                /*
                 * Create the viewport drawing contents for all tabs
                 */
                std::vector<BrainOpenGLViewportContent*> tabViewportContent = BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabs,
                                                                                                                                           browserWindowContent,
                                                                                                                                           gapsAndMargins,
                                                                                                                                           windowViewportBeforeAspectLocking,
                                                                                                                                           windowViewport,
                                                                                                                                           this->windowIndex,
                                                                                                                                           getModelEvent.getTabIndexForTileTabsHighlighting());
                for (auto tabvp : tabViewportContent) {
                    windowContent.addTabViewport(tabvp);
                }
                break;
        }
    }
    else if (numberOfTabs >= 1) {
        BrainOpenGLViewportContent* vc = BrainOpenGLViewportContent::createViewportForSingleTab(allTabs,
                                                                                                getModelEvent.getSelectedBrowserTabContent(),
                                                                                                gapsAndMargins,
                                                                                                this->windowIndex,
                                                                                                windowViewportBeforeAspectLocking,
                                                                                                windowViewport);
        windowContent.addTabViewport(vc);
    }
    
    std::vector<BrowserTabContent*> emptyTabVector;
    BrainOpenGLViewportContent* windowViewportContent = BrainOpenGLViewportContent::createViewportForSingleTab(emptyTabVector,
                                                                                                               NULL,
                                                                                                               gapsAndMargins,
                                                                                                               windowIndex,
                                                                                                               windowViewportBeforeAspectLocking,
                                                                                                               windowViewport);
    windowContent.setWindowViewport(windowViewportContent);
}

/**
 * Paints the graphics.
 */
void
BrainOpenGLWidget::paintGL()
{
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    if (m_contextShareGroupPointer == NULL) {
        m_contextShareGroupPointer = context()->shareGroup();
        CaretAssert(m_contextShareGroupPointer);
    }
#endif
    
    updateCursor();
    
    this->clearDrawingViewportContents();
    
    const int windowViewport[4] = {
        0,
        0,
        this->windowWidth[this->windowIndex],
        this->windowHeight[this->windowIndex]
    };
    
    getDrawingWindowContent(windowViewport,
                            m_windowContent);
    
    UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
    const UserInputModeEnum::Enum inputMode = inputProcessor->getUserInputMode();
    if (inputMode == UserInputModeEnum::Enum::BORDERS) {
        UserInputModeBorders* borderInputMode = dynamic_cast<UserInputModeBorders*>(inputProcessor);
        CaretAssert(borderInputMode);
        s_singletonOpenGL->setBorderBeingDrawn(borderInputMode->getBorderBeingDrawn());
    }
    else {
        s_singletonOpenGL->setBorderBeingDrawn(NULL);
    }
    s_singletonOpenGL->drawModels(this->windowIndex,
                                  inputMode,
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  m_windowContent.getAllTabViewports());
    
    /*
     * Issue browser window redrawn event
     */
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
    if (bbw != NULL) {
        EventManager::get()->sendEvent(EventBrowserWindowGraphicsRedrawn(bbw).getPointer());
    }
}

/**
 * Override of event handling.
 */
bool
BrainOpenGLWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) {
        if (SessionManager::get()->getDataToolTipsManager()->isEnabled()) {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
            CaretAssert(helpEvent);
            
            QPoint globalXY = helpEvent->globalPos();
            QPoint xyPoint = helpEvent->pos();
            const int32_t x = xyPoint.x();
            const int32_t y = this->height() - xyPoint.y();
            
            DataToolTipsManager* dttm = SessionManager::get()->getDataToolTipsManager();
            CaretAssert(dttm);
            
            AString toolTipText;

            const BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);
            if (idViewport != NULL) {
                BrowserTabContent* browserTabContent = idViewport->getBrowserTabContent();
                if (browserTabContent != NULL) {
                    SelectionManager* selectionManager = performIdentification(x,
                                                                               y,
                                                                               false); // include items in background
                    toolTipText = dttm->getToolTip(GuiManager::get()->getBrain(),
                                                   browserTabContent,
                                                   selectionManager);
                }
            }
            
            if (toolTipText.isEmpty()) {
                QToolTip::hideText();
                event->ignore();
            }
            else {
                const int millisecondsDisplayTime = (3 * 1000);
                QToolTip::showText(globalXY,
                                   toolTipText,
                                   nullptr,
                                   QRect(),
                                   millisecondsDisplayTime);
            }
            
            return true;
        }
    }
    else if (event->type() == QEvent::Gesture) {
        if (SessionManager::get()->getCaretPreferences()->isGuiGesturesEnabled()) {
            /*
             * Qt doc (https://doc.qt.io/qt-5/gestures-overview.html#) shows using static_cast not dynamic_cast.
             */
            QGestureEvent* gestureEvent = static_cast<QGestureEvent*>(event);
            if (processGestureEvent(gestureEvent)) {
                return true;
            }
        }
    }

#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    return QOpenGLWidget::event(event);
#else
    return QGLWidget::event(event);
#endif
}

/**
 * Process a gesture event (pinch or zoom on trackpad)
 */
bool
BrainOpenGLWidget::processGestureEvent(QGestureEvent* gestureEvent)
{
    /*
     * Disable until more time available for proper implementation
     */
    const bool enabledFlag(true);
    if ( ! enabledFlag) {
        return false;
    }
    
    QGesture* pinchGesture = gestureEvent->gesture(Qt::PinchGesture);
    if (pinchGesture != NULL) {
        QPinchGesture* pinch = static_cast<QPinchGesture*>(pinchGesture);
        const QPointF startPoint = pinch->startCenterPoint();
        
        GestureEvent::State gestureState = GestureEvent::State::START;
        bool validFlag(false);
        switch (pinch->state()) {
            case Qt::GestureCanceled:
                break;
            case Qt::GestureFinished:
                gestureState = GestureEvent::State::END;
                validFlag = true;
                break;
            case Qt::GestureStarted:
                gestureState = GestureEvent::State::START;
                validFlag = true;
                break;
            case Qt::GestureUpdated:
                gestureState = GestureEvent::State::UPDATE;
                validFlag = true;
                break;
            case Qt::NoGesture:
                break;
        }
        
        const int gestureStartX = startPoint.x();
        const int gestureStartY = height() - startPoint.y();
        const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(gestureStartX,
                                                                                         gestureStartY);
        if (viewportContent == NULL) {
            validFlag = false;
        }

        if (validFlag) {
            CaretAssert(viewportContent);
            
            UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
            
            QPinchGesture::ChangeFlags changeFlags = pinch->changeFlags();
            if (changeFlags & QPinchGesture::ScaleFactorChanged) {
                float deltaDegrees = pinch->scaleFactor();
                if (deltaDegrees > 1.0) {
                    deltaDegrees = 1;
                }
                else {
                    deltaDegrees = -1;
                }
                
                
                /*
                 * Use location of mouse press so that the model
                 * being manipulated does not change if mouse moves
                 * out of its viewport without releasing the mouse
                 * button.
                 */
                if (viewportContent != NULL) {
                    GestureEvent gestureEvent(viewportContent,
                                              this,
                                              this->windowIndex,
                                              gestureStartX,
                                              gestureStartY,
                                              gestureState,
                                              GestureEvent::Type::PINCH,
                                              pinch->scaleFactor());
                    inputProcessor->gestureEvent(gestureEvent);
                }
            }
            else if (changeFlags & QPinchGesture::RotationAngleChanged) {
                GestureEvent gestureEvent(viewportContent,
                                          this,
                                          this->windowIndex,
                                          gestureStartX,
                                          gestureStartY,
                                          gestureState,
                                          GestureEvent::Type::ROTATE,
                                          pinch->rotationAngle() - pinch->lastRotationAngle());

                inputProcessor->gestureEvent(gestureEvent);
            }
            
            gestureEvent->accept();
            return true;
        }
    }
    
    return false;
}


/**
 * Receive Content Menu events from Qt.
 * @param contextMenuEvent
 *    The context menu event.
 */
void 
BrainOpenGLWidget::contextMenuEvent(QContextMenuEvent* contextMenuEvent)
{
    const int mouseX = contextMenuEvent->x();
    const int mouseY = this->height() - contextMenuEvent->y();
    
    const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                               mouseY);
    if (viewportContent != NULL) {
        std::vector<MouseEvent::XY> emptyHistoryXY;
        MouseEvent mouseEvent(viewportContent,
                              this,
                              this->windowIndex,
                              mouseX,
                              mouseY,
                              0,
                              0,
                              mouseX,
                              mouseY,
                              emptyHistoryXY,
                              false);
        
        UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
        
        inputProcessor->showContextMenu(mouseEvent,
                                        contextMenuEvent->globalPos(),
                                        this);
    }
}

/**
 * Receive Mouse Wheel events from Qt.  A wheel event is that same
 * as CTRL-LEFT-DRAG.  The wheel's change in value is reported as
 * change in Y.  Change in X is reported as zero.
 *
 * @param we
 *   The wheel event.
 */
void 
BrainOpenGLWidget::wheelEvent(QWheelEvent* we)
{
    /*
     * Notes 24 Sep 2019
     *
     * QWheelEvent::pixelDelta() is not used since it is only set on Mac.
     *
     * Trackpad usage is when QWheelEvent::source()==Qt::MouseEventSynthesizedBySystem
     * Mouse usage is when QWheelEvent::source()==Qt::MouseEventNotSynthesized
     *
     * Inverted flag is only used by Mouse on MacOS.  It is true when
     * Preferences->Trackpad->Scroll & Zoom is ON.  The Wheel's inverted
     * flag is ignored since Mac alters the sign of the mouse Y-value.
     * It appears that "Scroll Direction:Natural is defaulted ON" on Macs
     *
     * Forward Wheel or Trackpad (moving away from user)
     * ** Negative when MacOS->Preferences->Trackpad->Scroll&Zoom->Scroll Direction:Natural is ON
     * ** Positive when MacOS->Preferences->Trackpad->Scroll&Zoom->Scroll Direction:Natural is OFF
     * ** Negative when MacOS->Preferences->Mouse->Scroll Direction:Natural is ON
     * ** Positive when MacOS->Preferences->Mouse->Scroll Direction:Natural is OFF
     *
     * ** Positive on Linux
     */
    const QPoint angleDelta = we->angleDelta();
    if (angleDelta.isNull()) {
        return;
    }
    
    int32_t deltaDegrees = angleDelta.y();
    if (deltaDegrees == 0) {
        return;
    }

    /*
     * While the mouse/trackpad flags are not used at this time,
     * we also do not have access to a Linux or Windows system
     * with a trackpad. 
     */
    bool mouseFlag(false);
    bool trackpadFlag(false);
    switch (we->source()) {
        case Qt::MouseEventNotSynthesized:
            mouseFlag = true;
            break;
        case Qt::MouseEventSynthesizedByApplication:
            break;
        case Qt::MouseEventSynthesizedByQt:
            break;
        case Qt::MouseEventSynthesizedBySystem:
            trackpadFlag = true;
            break;
    }

    const bool debugFlag(false);
    if (debugFlag) {
        std::cout << "Angle Delta:   " << we->angleDelta().y() << std::endl;
        std::cout << "Inverted:      " << AString::fromBool(we->inverted()) << std::endl;
        std::cout << "Source:        " << (int32_t)we->source() << std::endl;
        std::cout << "Mouse Flag:    " << AString::fromBool(mouseFlag) << std::endl;
        std::cout << "Trackpad Flag: " << AString::fromBool(trackpadFlag) << std::endl;
    }

    /*
     * If not limited, it is way too fast
     */
    const int limitValue(8);
    deltaDegrees = MathFunctions::limitRange(deltaDegrees, -limitValue, limitValue);
    
    /*
     * Use location of mouse press so that the model
     * being manipulated does not change if mouse moves
     * out of its viewport without releasing the mouse
     * button.
     */
    const int wheelX = we->x();
    const int wheelY = this->windowHeight[this->windowIndex] - we->y();
    const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(wheelX,
                                                                                     wheelY);
    if (viewportContent != NULL) {
        std::vector<MouseEvent::XY> emptyHistoryXY;
        MouseEvent mouseEvent(viewportContent,
                              this,
                              this->windowIndex,
                              wheelX,
                              wheelY,
                              0,
                              deltaDegrees,
                              wheelX,
                              wheelY,
                              emptyHistoryXY,
                              this->mouseNewDraggingStartedFlag);
        
        UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
        
        inputProcessor->mouseLeftDragWithCtrl(mouseEvent);
    }

    we->accept();
}

/*
 * A mouse event that is the middle mouse button but with no keys pressed
 * is reported as a SHIFT-LEFT-DRAG and the mouse event is changed.
 *
 * @param mouseButtons
 *     Button state when event was generated
 * @param button
 *     Button that caused the event.
 * @param keyModifiers
 *     Keys that are down, may be modified.
 * @param isMouseMoving
 *     True if mouse is moving, else false.
 */
void
BrainOpenGLWidget::checkForMiddleMouseButton(Qt::MouseButtons& mouseButtons,
                                             Qt::MouseButton& button,
                                             Qt::KeyboardModifiers& keyModifiers,
                                             const bool isMouseMoving)
{
    if (isMouseMoving) {
        if (button == Qt::NoButton) {
            if (mouseButtons == Qt::MiddleButton) {
                if (keyModifiers == Qt::NoModifier) {
                    mouseButtons = Qt::LeftButton;
                    button = Qt::NoButton;
                    keyModifiers = Qt::ShiftModifier;
                }
            }
        }
    }
    else {
        if (button == Qt::MiddleButton) {
            if (keyModifiers == Qt::NoModifier) {
                button = Qt::LeftButton;
                keyModifiers = Qt::ShiftModifier;
            }
        }
    }
}

/**
 * Receive key press events from Qt.
 * @param e
 *    The key event.
 */
void
BrainOpenGLWidget::keyPressEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers keyModifiers = e->modifiers();
    const bool shiftKeyDownFlag = ((keyModifiers & Qt::ShiftModifier) != 0);
    
    bool mouseValidFlag(false);
    const QPoint mousePos = mapFromGlobal(QCursor::pos());
    int32_t mouseX = mousePos.x();
    int32_t mouseY = height() - mousePos.y();
    if ((mouseX >= 0)
        && (mouseX < width())
        && (mouseY >= 0)
        && (mouseY < height())) {
        mouseValidFlag = true;
    }
    
    const BrainOpenGLViewportContent* viewportContent(NULL);
    if (mouseValidFlag) {
        viewportContent = getViewportContentAtXY(mouseX,
                                                 mouseY);
    }
    KeyEvent keyEvent(viewportContent,
                      this,
                      this->windowIndex,
                      e->key(),
                      mouseX,
                      mouseY,
                      mouseValidFlag,
                      m_newKeyPressStartedFlag,
                      shiftKeyDownFlag);
    
    UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
    
    const bool keyWasProcessedFlag = inputProcessor->keyPressEvent(keyEvent);
    
    e->accept();
    
    m_newKeyPressStartedFlag = false;
    
    if ( ! keyWasProcessedFlag) {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        QOpenGLWidget::keyPressEvent(e);
#else
        QGLWidget::keyPressEvent(e);
#endif
    }
}

/**
 * Receive key release events from Qt.
 * @param e
 *    The key event.
 */
void
BrainOpenGLWidget::keyReleaseEvent(QKeyEvent* e)
{
    m_newKeyPressStartedFlag = true;
    
    e->accept();
}

/**
 * Receive mouse press events from Qt.
 * @param me
 *    The mouse event.
 */
void 
BrainOpenGLWidget::mousePressEvent(QMouseEvent* me)
{
    WuQMacroManager::instance()->addMouseEventToRecording(this,
                                                          "Mouse Press in Window" + AString::number(this->windowIndex + 1),
                                                          me);
    
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();
    
    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              false);
    
    /*
     * When the mouse is dragged, a mouse input receiver may want to
     * know that a new dragging has started.
     */
    this->mouseNewDraggingStartedFlag = true;
    
    this->isMousePressedNearToolBox = false;
    
    if (button == Qt::LeftButton) {
        const int mouseX = me->x();
        const int mouseY = this->windowHeight[this->windowIndex] - me->y();

        this->mousePressX = mouseX;
        this->mousePressY = mouseY;
        
        this->lastMouseX = mouseX;
        this->lastMouseY = mouseY;

        this->mouseMovementMinimumX = mouseX;
        this->mouseMovementMaximumX = mouseX;
        this->mouseMovementMinimumY = mouseY;
        this->mouseMovementMaximumY = mouseY;
        
        m_mouseHistoryXY.clear();
        m_mouseHistoryXY.emplace_back(mouseX,
                                      mouseY);
        
        /*
         * The user may intend to increase the size of a toolbox
         * but instead misses the edge of the toolbox when trying
         * to drag the toolbox and make it larger.  So, indicate
         * when the user is very close to the edge of the graphics
         * window.
         */
        const int nearToolBoxDistance = 5;
        if ((mouseX < nearToolBoxDistance) 
            || (mouseX > (this->windowWidth[this->windowIndex] - 5))
            || (mouseY < nearToolBoxDistance) 
            || (mouseY > (this->windowHeight[this->windowIndex] - 5))) {
            this->isMousePressedNearToolBox = true;
        }

        const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                   mouseY);
        if (viewportContent != NULL) {
            MouseEvent mouseEvent(viewportContent,
                                  this,
                                  this->windowIndex,
                                  mouseX,
                                  mouseY,
                                  0,
                                  0,
                                  this->mousePressX,
                                  this->mousePressY,
                                  m_mouseHistoryXY,
                                  this->mouseNewDraggingStartedFlag);
            
            if (keyModifiers == Qt::NoModifier) {
                UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
                
                inputProcessor->mouseLeftPress(mouseEvent);
            }
            else if (keyModifiers == Qt::ShiftModifier) {
                /* not implemented  this->selectedUserInputProcessor->mouseLeftPressWithShift(mouseEvent); */
            }
        }
    }
    else {
        this->mousePressX = -10000;
        this->mousePressY = -10000;
        m_mouseHistoryXY.clear();
    }
    
    me->accept();
}

/**
 * Receive mouse button release events from Qt.
 * @param me
 *    The mouse event.
 */
void 
BrainOpenGLWidget::mouseReleaseEvent(QMouseEvent* me)
{
    WuQMacroManager::instance()->addMouseEventToRecording(this,
                                                          "Mouse Release in Window" + AString::number(this->windowIndex + 1),
                                                          me);
    
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();

    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              false);
    
    if (button == Qt::LeftButton) {
        const int mouseX = me->x();
        const int mouseY = this->windowHeight[this->windowIndex] - me->y();
        
        this->mouseMovementMinimumX = std::min(this->mouseMovementMinimumX, mouseX);
        this->mouseMovementMaximumX = std::max(this->mouseMovementMaximumX, mouseX);
        this->mouseMovementMinimumY = std::min(this->mouseMovementMinimumY, mouseY);
        this->mouseMovementMaximumY = std::max(this->mouseMovementMaximumY, mouseY);
        
        const int dx = this->mouseMovementMaximumX - this->mouseMovementMinimumX;
        const int dy = this->mouseMovementMaximumY - this->mouseMovementMinimumY;
        const int absDX = (dx >= 0) ? dx : -dx;
        const int absDY = (dy >= 0) ? dy : -dy;

        UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
        
        {
            /*
             * Mouse button RELEASE event
             */
            const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                       mouseY);
            if (viewportContent != NULL) {
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      0,
                                      0,
                                      this->mousePressX,
                                      this->mousePressY,
                                      m_mouseHistoryXY,
                                      this->mouseNewDraggingStartedFlag);
                inputProcessor->mouseLeftRelease(mouseEvent);
            }
        }
        
        /*
         * Use location of mouse press so that the model
         * being manipulated does not change if mouse moves
         * out of its viewport without releasing the mouse
         * button.
         */
        const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(this->mousePressX,
                                                                                   this->mousePressY);
        if (viewportContent != NULL) {
            if ((absDX <= BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE)
                && (absDY <= BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE)) {
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      dx,
                                      dy,
                                      this->mousePressX,
                                      this->mousePressY,
                                      m_mouseHistoryXY,
                                      this->mouseNewDraggingStartedFlag);
                
                if (keyModifiers == Qt::NoModifier) {
                    inputProcessor->mouseLeftClick(mouseEvent);
                }
                else if (keyModifiers == Qt::ShiftModifier) {
                    inputProcessor->mouseLeftClickWithShift(mouseEvent);
                }
                else if (keyModifiers == (Qt::ShiftModifier
                                          | Qt::ControlModifier)) {
                    inputProcessor->mouseLeftClickWithCtrlShift(mouseEvent);
                }
            }
        }
    }
    
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    m_mouseHistoryXY.clear();
    this->isMousePressedNearToolBox = false;
    
    me->accept();
}

/**
 * Receive mouse button double click events from Qt.
 * @param me
 *    The mouse event.
 */
void
BrainOpenGLWidget::mouseDoubleClickEvent(QMouseEvent* me)
{
    WuQMacroManager::instance()->addMouseEventToRecording(this,
                                                          "Mouse Double Click in Window" + AString::number(this->windowIndex + 1),
                                                          me);

    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();
    
    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              false);
    
    if (button == Qt::LeftButton) {
        if (keyModifiers == Qt::NoModifier) {
            const int mouseX = me->x();
            const int mouseY = this->windowHeight[this->windowIndex] - me->y();
            
            /*
             * Use location of mouse press so that the model
             * being manipulated does not change if mouse moves
             * out of its viewport without releasing the mouse
             * button.
             */
            const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                       mouseY);
            if (viewportContent != NULL) {
                std::vector<MouseEvent::XY> emptyHistoryXY;
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      0,
                                      0,
                                      this->mousePressX,
                                      this->mousePressY,
                                      emptyHistoryXY,
                                      this->mouseNewDraggingStartedFlag);
                
                getSelectedInputProcessor()->mouseLeftDoubleClick(mouseEvent);
            }
        }
    }
    
    this->isMousePressedNearToolBox = false;
    
    me->accept();
}

void
BrainOpenGLWidget::enterEvent(QEvent* /*e*/)
{
    m_mousePositionValid = true;
}

void
BrainOpenGLWidget::leaveEvent(QEvent* /*e*/)
{
    m_mousePositionValid = false;
    
    getSelectedInputProcessor()->setMousePosition(m_mousePositionEvent,
                                                       m_mousePositionValid);
}


/**
 * Get the viewport content at the given location.
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 */
const BrainOpenGLViewportContent*
BrainOpenGLWidget::getViewportContentAtXY(const int x,
                                          const int y)
{
    const BrainOpenGLViewportContent* tabViewportContent = m_windowContent.getTabViewportWithLockAspectXY(x, y);
    if (tabViewportContent != NULL) {
        return tabViewportContent;
    }
    
    /*
     * If not in a tab, then use the window viewport information.
     * This allows selection of annotations in window space that are not
     * within a tab (tab may be small in height due to lock aspect).
     */
    return m_windowContent.getWindowViewport();
}

/**
 * Get the viewport content at the given location WITHOUT aspect locking.
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 */
const BrainOpenGLViewportContent*
BrainOpenGLWidget::getViewportContentManualLayoutWithoutLockAspectAtXY(const int x,
                                                                       const int y)
{
    const BrainOpenGLViewportContent* tabViewportContent = m_windowContent.getTabViewportManualLayoutWithoutAspectLocking(x, y);
    if (tabViewportContent != NULL) {
        return tabViewportContent;
    }
    
    /*
     * If not in a tab, then use the window viewport information.
     * This allows selection of annotations in window space that are not
     * within a tab (tab may be small in height due to lock aspect).
     */
    return m_windowContent.getWindowViewport();
}

/**
 * Get all viewport content.  If tile tabs is ON, the output will contain 
 * viewport content for all tabs.  Otherwise, the output will contain viewport
 * content for only the selected tab.
 *
 * @return Viewport content.
 */
std::vector<const BrainOpenGLViewportContent*>
BrainOpenGLWidget::getViewportContent() const
{
    std::vector<const BrainOpenGLViewportContent*> contentOut = m_windowContent.getAllTabViewports();
    
    return contentOut;
}

/**
 * Perform identification on all items EXCEPT voxel editing.
 *
 * @param x
 *    X-coordinate for identification.
 * @param y
 *    Y-coordinate for identification.
 * @param applySelectionBackgroundFiltering
 *    If true (which is in most cases), if there are multiple items
 *    selected, those items "behind" other items are not reported.
 *    For example, suppose a focus is selected and there is a node
 *    the focus.  If this parameter is true, the node will NOT be
 *    selected.  If this parameter is false, the node will be
 *    selected.
 * @return
 *    SelectionManager providing identification information.
 */
SelectionManager* 
BrainOpenGLWidget::performIdentification(const int x,
                                         const int y,
                                         const bool applySelectionBackgroundFiltering)
{
    const BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);

    this->makeCurrent();
    CaretLogFine("Performing selection");
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    idManager->reset();
    idManager->setAllSelectionsEnabled(true);
    idManager->getVoxelEditingIdentification()->setEnabledForSelection(false);
    
    if (idViewport != NULL) {
        s_singletonOpenGL->selectModel(this->windowIndex,
                                       getSelectedInputMode(),
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  idViewport,
                                  x, 
                                  y,
                                  applySelectionBackgroundFiltering);
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so 
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaintGraphics();
    this->doneCurrent();
#else
    this->repaintGraphics();
#endif
    
    return idManager;
}

/**
 * Perform identification of only annotations.  Identification of other
 * data types is off.
 *
 * @param x
 *    X-coordinate for identification.
 * @param y
 *    Y-coordinate for identification.
 * @return
 *    A pointer to the annotation selection item.  Its
 *    "isValid()" method may be queried to determine
 *    if the selected annotation is valid.
 */
SelectionItemAnnotation*
BrainOpenGLWidget::performIdentificationAnnotations(const int x,
                                                    const int y)
{
    const UserInputModeEnum::Enum inputMode = getSelectedInputMode();
    bool manLayoutFlag(false);
    switch (inputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            break;
        case UserInputModeEnum::Enum::TILE_TABS_MANUAL_LAYOUT_EDITING:
            manLayoutFlag = true;
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            break;
    }

    const BrainOpenGLViewportContent* idViewport = (manLayoutFlag
                                                    ? this->getViewportContentManualLayoutWithoutLockAspectAtXY(x, y)
                                                    : this->getViewportContentAtXY(x, y));

    this->makeCurrent();
    CaretLogFine("Performing selection");
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    idManager->reset();
    idManager->setAllSelectionsEnabled(false);
    SelectionItemAnnotation* annotationID = idManager->getAnnotationIdentification();
    annotationID->setEnabledForSelection(true);
    
    if (idViewport != NULL) {
        /*
         * ID coordinate needs to be relative to the viewport
         *
         int vp[4];
         idViewport->getViewport(vp);
         const int idX = x - vp[0];
         const int idY = y - vp[1];
         */
        s_singletonOpenGL->selectModel(this->windowIndex,
                                       inputMode,
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  idViewport,
                                  x,
                                  y,
                                  true);
    }

#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaintGraphics();
    this->doneCurrent();
#else
    this->repaintGraphics();
#endif
    
    return annotationID;
}

/**
 * Perform identification of only voxel editing.  Identification of other
 * data types is off.
 *
 * @param editingVolumeFile
 *    Volume file that is being edited.
 * @param x
 *    X-coordinate for identification.
 * @param y
 *    Y-coordinate for identification.
 * @return
 *    SelectionManager providing identification information.
 */
SelectionManager*
BrainOpenGLWidget::performIdentificationVoxelEditing(VolumeFile* editingVolumeFile,
                                                     const int x,
                                                     const int y)
{
    const BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);
    
    this->makeCurrent();
    CaretLogFine("Performing selection");
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    idManager->reset();
    idManager->setAllSelectionsEnabled(false);
    SelectionItemVoxelEditing* idVoxelEdit = idManager->getVoxelEditingIdentification();
    idVoxelEdit->setEnabledForSelection(true);
    idVoxelEdit->setVolumeFileForEditing(editingVolumeFile);
    
    if (idViewport != NULL) {
        /*
         * ID coordinate needs to be relative to the viewport
         *
         int vp[4];
         idViewport->getViewport(vp);
         const int idX = x - vp[0];
         const int idY = y - vp[1];
         */
        s_singletonOpenGL->selectModel(this->windowIndex,
                                       getSelectedInputMode(),
                                  GuiManager::get()->getBrain(),
                                  m_contextShareGroupPointer,
                                  idViewport,
                                  x,
                                  y,
                                  true);
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaintGraphics();
    this->doneCurrent();
#else
    this->repaintGraphics();
#endif
    
    return idManager;
}

/**
 * Project the given item to a model.
 *
 * @param x
 *    X-coordinate for projection.
 * @param y
 *    Y-coordinate for projection.
 * @param projectionOut
 *    Projection updated for the given x and y coordinates.
 */
void
BrainOpenGLWidget::performProjection(const int x,
                                     const int y,
                                     SurfaceProjectedItem& projectionOut)
{
    const BrainOpenGLViewportContent* projectionViewport = this->getViewportContentAtXY(x, y);
    
    this->makeCurrent();
    CaretLogFine("Performing projection");
    
    if (projectionViewport != NULL) {
        s_singletonOpenGL->projectToModel(this->windowIndex,
                                          getSelectedInputMode(),
                                     GuiManager::get()->getBrain(),
                                     m_contextShareGroupPointer,
                                     projectionViewport,
                                     x,
                                     y,
                                     projectionOut);
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Note: The QOpenGLWidget always renders in a
     * frame buffer object (see its documentation) so
     * there is no "back" or "front buffer".  Since
     * identification is encoded in the framebuffer,
     * it is necessary to repaint (udpates graphics
     * immediately) to redraw the models.  Otherwise,
     * the graphics flash with strange looking drawing.
     */
    this->repaintGraphics();
    this->doneCurrent();
#endif
}

/**
 * Receive mouse move events from Qt.
 * @param me
 *    The mouse event.
 */
void 
BrainOpenGLWidget::mouseMoveEvent(QMouseEvent* me)
{
    WuQMacroManager::instance()->addMouseEventToRecording(this,
                                                          "Mouse Move in Window" + AString::number(this->windowIndex + 1),
                                                          me);
    
    /*
     * Tooltip will remain displayed for several seconds.
     * So if the user moves the mouse, remove the tooltip.
     */
    QToolTip::hideText();
    
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();

    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              true);
    
    const int mouseX = me->x();
    const int mouseY = this->windowHeight[this->windowIndex] - me->y();
    
    UserInputModeAbstract* inputProcessor = getSelectedInputProcessor();
    
    if (mouseButtons == Qt::LeftButton) {
        this->mouseMovementMinimumX = std::min(this->mouseMovementMinimumX, mouseX);
        this->mouseMovementMaximumX = std::max(this->mouseMovementMaximumX, mouseX);
        this->mouseMovementMinimumY = std::min(this->mouseMovementMinimumY, mouseY);
        this->mouseMovementMaximumY = std::max(this->mouseMovementMaximumY, mouseY);
        
        const int dx = mouseX - this->lastMouseX;
        const int dy = mouseY - this->lastMouseY;
        const int absDX = (dx >= 0) ? dx : -dx;
        const int absDY = (dy >= 0) ? dy : -dy;
        
        if ((absDX > 0)
            || (absDY > 0)) {
            m_mouseHistoryXY.emplace_back(mouseX,
                                          mouseY);
            
            /*
             * Use location of mouse press so that the model
             * being manipulated does not change if mouse moves
             * out of its viewport without releasing the mouse
             * button.
             */
            const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(this->mousePressX,
                                                                                             this->mousePressY);
            if (viewportContent != NULL) {
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      dx,
                                      dy,
                                      this->mousePressX,
                                      this->mousePressY,
                                      m_mouseHistoryXY,
                                      this->mouseNewDraggingStartedFlag);
                
                if (keyModifiers == Qt::NoModifier) {
                    inputProcessor->mouseLeftDrag(mouseEvent);
                }
                else if (keyModifiers == Qt::ControlModifier) {
                    inputProcessor->mouseLeftDragWithCtrl(mouseEvent);
                }
                else if (keyModifiers == Qt::ShiftModifier) {
                    inputProcessor->mouseLeftDragWithShift(mouseEvent);
                }
                else if (keyModifiers == Qt::AltModifier) {
                    inputProcessor->mouseLeftDragWithAlt(mouseEvent);
                }
                else if (keyModifiers == (Qt::ShiftModifier
                                          | Qt::ControlModifier)) {
                    inputProcessor->mouseLeftDragWithCtrlShift(mouseEvent);
                }
                
                this->mouseNewDraggingStartedFlag = false;
            }
        }
        
        this->lastMouseX = mouseX;
        this->lastMouseY = mouseY;
    }
    else if (mouseButtons == Qt::NoButton) {
        const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                                         mouseY);
        if (viewportContent != NULL) {
            MouseEvent mouseEvent(viewportContent,
                                  this,
                                  this->windowIndex,
                                  mouseX,
                                  mouseY,
                                  0,
                                  0,
                                  this->mousePressX,
                                  this->mousePressY,
                                  m_mouseHistoryXY,
                                  this->mouseNewDraggingStartedFlag);
            
            if (keyModifiers == Qt::NoModifier) {
                inputProcessor->mouseMove(mouseEvent);
            }
            else if (keyModifiers == Qt::ShiftModifier) {
                inputProcessor->mouseMoveWithShift(mouseEvent);
            }
        }
    }

    const BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                               mouseY);
    if (viewportContent != NULL) {
        m_mousePositionEvent.grabNew(new MouseEvent(viewportContent,
                                                    this,
                                                    this->windowIndex,
                                                    mouseX,
                                                    mouseY,
                                                    0,
                                                    0,
                                                    this->mousePressX,
                                                    this->mousePressY,
                                                    m_mouseHistoryXY,
                                                    this->mouseNewDraggingStartedFlag));
        
        inputProcessor->setMousePosition(m_mousePositionEvent,
                                                           m_mousePositionValid);
    }
    else {
        
    }
    
    me->accept();
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
BrainOpenGLWidget::receiveEvent(Event* event)
{
    MovieRecorder* movieRecorder = SessionManager::get()->getMovieRecorder();
    MovieRecorderModeEnum::Enum movieRecordingMode = movieRecorder->getRecordingMode();
    
    bool doRepaintGraphicsFlag(false);
    bool doUpdateGraphicsFlag(false);
    int32_t captureManualMovieModeImageRepeatCount(-1);
    
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_RESET) {
        EventBrainReset* brainResetEvent = dynamic_cast<EventBrainReset*>(event);
        CaretAssert(brainResetEvent);
        
        brainResetEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_TIMING_ONE_WINDOW) {
        EventGraphicsTimingOneWindow* timingEvent = dynamic_cast<EventGraphicsTimingOneWindow*>(event);
        CaretAssert(timingEvent);
        
        if (timingEvent->getWindowIndex() == this->windowIndex) {
            doRepaintGraphicsFlag = true;
            timingEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS) {
        EventGraphicsUpdateAllWindows* updateAllEvent =
            dynamic_cast<EventGraphicsUpdateAllWindows*>(event);
        CaretAssert(updateAllEvent);
        
        updateAllEvent->setEventProcessed();
        
        if (updateAllEvent->isRepaint()) {
            doRepaintGraphicsFlag = true;
        }
        else {
            doUpdateGraphicsFlag = true;
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {
        EventGraphicsUpdateOneWindow* updateOneEvent =
        dynamic_cast<EventGraphicsUpdateOneWindow*>(event);
        CaretAssert(updateOneEvent);
        
        if (updateOneEvent->getWindowIndex() == this->windowIndex) {
            updateOneEvent->setEventProcessed();
            if (updateOneEvent->isRepaint()) {
                doRepaintGraphicsFlag = true;
            }
            else {
                doUpdateGraphicsFlag = true;
            }
        }
        else {
            /*
             * If a window is yoked, update its graphics.
             */
            EventBrowserWindowDrawingContent getModelEvent(this->windowIndex);
            EventManager::get()->sendEvent(getModelEvent.getPointer());
            
            if (getModelEvent.isError()) {
                return;
            }
            
            bool needUpdate = false;
            if (needUpdate) {
                doUpdateGraphicsFlag = true;
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_WINDOW_SHOW_TOOL_TIP) {
        EventGraphicsWindowShowToolTip* tipEvent = dynamic_cast<EventGraphicsWindowShowToolTip*>(event);
        CaretAssert(tipEvent);
        
        if (tipEvent->getWindowIndex() == this->windowIndex) {
            std::array<float, 3> windowXYZ = tipEvent->getWindowXYZ();
            
            /*
             * Origin is at top left for Qt, bottom left for OpenGL
             */
            switch (tipEvent->getWindowOrigin()) {
                case EventGraphicsWindowShowToolTip::WindowOrigin::BOTTOM_LEFT:
                    windowXYZ[1] = height() - windowXYZ[1];
                    break;
                case EventGraphicsWindowShowToolTip::WindowOrigin::TOP_LEFT:
                    break;
            }
            
            m_selectedChartPointToolTipInfo.m_position = mapToGlobal(QPoint(windowXYZ[0],
                                                                            windowXYZ[1]));
            m_selectedChartPointToolTipInfo.m_text = tipEvent->getText();
            
            /*
             * We cannot call QToolTip::showText() from here.  If this event is sent
             * when the user changes the value in the index spin box in Chart Layers,
             * the event is sent when the mouse is pressed.  When the user releases the
             * mouse button, the tooltip is removed by Qt and thus, from the user's persepctive,
             * the tooltip will go away almost immediately.
             *
             * Instead, use a QTimer to wait a short amount of time and then display the
             * tooltip containing the selected chart points XY coordinate.  We do a few
             * single shot timers since we want the tooltip to display quickly but
             * we don't know how long until the user releases the mouse.
             *
             * The first parameter to QTimer::singleShot() is the delay in millisecond
             * until the method is called.
             */
            QTimer::singleShot(500, this, &BrainOpenGLWidget::showSelectedChartPointToolTip);
            QTimer::singleShot(750, this, &BrainOpenGLWidget::showSelectedChartPointToolTip);
            QTimer::singleShot(1000, this, &BrainOpenGLWidget::showSelectedChartPointToolTip);
            
            tipEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_IMAGE_CAPTURE) {
        EventImageCapture* imageCaptureEvent = dynamic_cast<EventImageCapture*>(event);
        CaretAssert(imageCaptureEvent);
        
        if (imageCaptureEvent->getBrowserWindowIndex() == this->windowIndex) {
            captureImage(imageCaptureEvent);
            imageCaptureEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_REQUEST) {
        EventIdentificationRequest* idRequestEvent = dynamic_cast<EventIdentificationRequest*>(event);
        CaretAssert(idRequestEvent);
        
        if (idRequestEvent->getWindowIndex() == this->windowIndex) {
            SelectionManager* sm = performIdentification(idRequestEvent->getWindowX(),
                                                         idRequestEvent->getWindowY(),
                                                         false);
            idRequestEvent->setSelectionManager(sm);
            idRequestEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MOVIE_RECORDING_MANUAL_MODE_CAPTURE) {
        EventMovieManualModeRecording* movieEvent = dynamic_cast<EventMovieManualModeRecording*>(event);
        CaretAssert(movieEvent);
        
        const int32_t windowIndex = movieEvent->getBrowserWindowIndex();
        if ((windowIndex < 0)
            || (windowIndex == this->windowIndex)) {
            /*
             * Movie mode may be automatic but override with manual
             * so that images are captured
             */
            movieRecordingMode = MovieRecorderModeEnum::MANUAL;

            const float durationSeconds = movieEvent->getDurationSeconds();
            const float frameRate       = movieRecorder->getFramesRate();
            captureManualMovieModeImageRepeatCount = static_cast<int32_t>(frameRate
                                                                          * durationSeconds);
            if (captureManualMovieModeImageRepeatCount < 1) {
                captureManualMovieModeImageRepeatCount = 1;
            }
            doRepaintGraphicsFlag = true;

            movieEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* guiUpdateEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(guiUpdateEvent);
        guiUpdateEvent->setEventProcessed();
        
        getSelectedInputProcessor()->update();
    }
    else {
        
    }
    
    if (doRepaintGraphicsFlag
        || doUpdateGraphicsFlag) {

        bool captureAutomaticImageForMovieFlag(false);
        if (movieRecorder->getRecordingWindowIndex() == this->windowIndex) {
            switch (movieRecordingMode) {
                case MovieRecorderModeEnum::MANUAL:
                    break;
                case MovieRecorderModeEnum::AUTOMATIC:
                    captureAutomaticImageForMovieFlag = true;
                    doRepaintGraphicsFlag = true;
                    break;
            }
        }

        if (doRepaintGraphicsFlag) {
            repaintGraphics();
            if (captureAutomaticImageForMovieFlag
                || (captureManualMovieModeImageRepeatCount > 0)) {
                const bool showTimingResultFlag(false);
                
                int captureRegionOffsetX(0);
                int captureRegionOffsetY(0);
                int captureRegionWidth(0);
                int captureRegionHeight(0);
                int widgetWidth(0);
                int widgetHeight(0);
                BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
                
                if (browserWindow != NULL) {
                    browserWindow->getGraphicsWidgetSize(captureRegionOffsetX,
                                                         captureRegionOffsetY,
                                                         captureRegionWidth,
                                                         captureRegionHeight,
                                                         widgetWidth,
                                                         widgetHeight,
                                                         true);
                    int outputImageWidth(0);
                    int outputImageHeight(0);
                    movieRecorder->getVideoWidthAndHeight(outputImageWidth,
                                                          outputImageHeight);
                    
                    AString msg;
                    if ((outputImageWidth <= 0)
                        || (outputImageHeight <= 0)) {
                        msg.appendWithNewLine("Movie width="
                                              + AString::number(outputImageWidth)
                                              + ", height="
                                              + AString::number(outputImageHeight)
                                              + " is invalid.");
                    }
                    if ((captureRegionWidth <= 0)
                        || (captureRegionHeight <= 0)) {
                        msg.appendWithNewLine("Movie capture region width="
                                              + AString::number(captureRegionWidth)
                                              + ", height="
                                              + AString::number(captureRegionHeight)
                                              + " is invalid.");
                    }
                    if ( ! msg.isEmpty()) {
                        CaretLogSevere(msg);
                    }
                    else {
                        QImage image;
                        bool imageValid(false);
                        
                        switch (movieRecorder->getCaptureRegionType()) {
                            case MovieRecorderCaptureRegionTypeEnum::GRAPHICS:
                            {
                                bool adjustImageSizeFlag(false);
                                int captureWidth  = outputImageWidth;
                                int captureHeight = outputImageHeight;
                                if ((captureWidth != captureRegionWidth)
                                    || (captureHeight != captureRegionHeight)) {
                                    const float outputAspectRatio = (outputImageHeight / outputImageWidth);
                                    const float captureRegionAspectRatio = (captureRegionHeight / captureRegionWidth);
                                    if (captureRegionAspectRatio > outputAspectRatio) {
                                        const float ratio = outputImageHeight / captureRegionHeight;
                                        captureWidth  = outputImageWidth * ratio;
                                        captureHeight = outputImageHeight;
                                    }
                                    else {
                                        const float ratio = outputImageWidth / captureRegionWidth;
                                        captureHeight = outputImageHeight * ratio;
                                        captureWidth  = outputImageWidth;
                                        
                                    }
                                    adjustImageSizeFlag = true;
                                }
                                
                                ElapsedTimer timer;
                                timer.start();
                                EventImageCapture captureEvent(this->windowIndex,
                                                               captureRegionOffsetX,
                                                               captureRegionOffsetY,
                                                               captureRegionWidth,
                                                               captureRegionHeight,
                                                               captureWidth,
                                                               captureHeight);
                                captureImage(&captureEvent);
                                if (showTimingResultFlag) {
                                    std::cout << "Capture time: " << timer.getElapsedTimeSeconds() << std::endl;
                                }
                                
                                if (captureEvent.isError()) {
                                    CaretLogSevere("Failed to capture image of graphics for movie recording");
                                }
                                else {
                                    image = captureEvent.getImage();
                                    imageValid = true;
                                    if (adjustImageSizeFlag) {
                                        QImage scaledImage = ImageFile::scaleToSizeWithPadding(image,
                                                                                               outputImageWidth,
                                                                                               outputImageHeight);
                                        if (scaledImage.isNull()) {
                                            CaretLogSevere("Failed to scale image for movie recording");
                                        }
                                        else {
                                            image = scaledImage;
                                        }
                                    }
                                }
                            }
                                break;
                            case MovieRecorderCaptureRegionTypeEnum::WINDOW:
                            {
                                BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
                                QRect rect(0, 0,
                                           bbw->width(), bbw->height());
                                QPixmap pm = bbw->grab(rect);
                                if ((pm.width() > 0)
                                    && (pm.height() > 0)) {
                                    image = pm.toImage();
                                    imageValid = true;
                                    
                                    QImage scaledImage = ImageFile::scaleToSizeWithPadding(image,
                                                                                           outputImageWidth,
                                                                                           outputImageHeight);
                                    if (scaledImage.isNull()) {
                                        CaretLogSevere("Failed to scale image for movie recording");
                                    }
                                    else {
                                        image = scaledImage;
                                    }
                                }
                                else {
                                    CaretLogSevere("Failed to capture image of window");
                                }
                            }
                                break;
                        }
                        
                        if (imageValid) {
                            ElapsedTimer imageTimer;
                            imageTimer.start();
                            if (captureAutomaticImageForMovieFlag) {
                                movieRecorder->addImageToMovie(&image);
                            }
                            else if (captureManualMovieModeImageRepeatCount > 0) {
                                movieRecorder->addImageToMovieWithCopies(&image,
                                                                         captureManualMovieModeImageRepeatCount);
                            }
                            if (showTimingResultFlag) {
                                std::cout << "   Image write time: " << imageTimer.getElapsedTimeSeconds() << std::endl;
                            }
                            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_MOVIE_RECORDING_DIALOG_UPDATE);
                        }
                    }
                }
            }
        }
        else if (doUpdateGraphicsFlag) {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            this->update();
#else
            this->updateGL();
#endif
        }
    }
}

/**
 * Show a tooltip containing the selected chart point's coordinate.
 */
void
BrainOpenGLWidget::showSelectedChartPointToolTip()
{
    if ( ! m_selectedChartPointToolTipInfo.m_text.isEmpty()) {
        QToolTip::showText(m_selectedChartPointToolTipInfo.m_position,
                           m_selectedChartPointToolTipInfo.m_text);
    }
}

/**
 * Perform an immediate repaint of the graphics
 */
void
BrainOpenGLWidget::repaintGraphics()
{
    repaint();
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * As of QT 5.12.0.
     *
     * When using QOpenGLWidget, calling repaint() returns before
     * it calls paintGL() so graphics are not updated even though
     * the documentation states that drawing will complete during
     * the call to repaint().  Essentially, repaint() functions
     * the same as update().
     *
     * Two Qt bug reports have been submitted by others:
     * QTBUG-74404, QTBUG-53107.
     *
     * A previous kludge to fix this problem made a call to
     * QApplication::processEvents().  However, it was found
     * to cause a problem with annotation dragging as multiple
     * mouse events were getting issued.
     * Commit: 666b0e7c3d8d443aa9668a5cff986f7b83158488
     *
     * Now, the kludge is to call grabFramebuffer() which I
     * assume has to complete drawing before capturing an
     * image.  This fixes the problem with dragging annotations.
     */
    (void)grabFramebuffer();
#endif
}

/**
 * Capture an image using the parameters from the event.
 *
 * @param imageCaptureEvent
 *    The image capture event.
 */
void
BrainOpenGLWidget::captureImage(EventImageCapture* imageCaptureEvent)
{
    const int oldSizeX = this->windowWidth[this->windowIndex];
    const int oldSizeY = this->windowHeight[this->windowIndex];

    /*
     * Note that a size of zero indicates capture graphics in its
     * current size.
     */
    int captureOffsetX    = imageCaptureEvent->getCaptureOffsetX();
    int captureOffsetY    = imageCaptureEvent->getCaptureOffsetY();
    int captureWidth      = imageCaptureEvent->getCaptureWidth();
    int captureHeight     = imageCaptureEvent->getCaptureHeight();
    int outputImageWidth  = imageCaptureEvent->getOutputWidth();
    int outputImageHeight = imageCaptureEvent->getOutputHeight();
    
    if ((outputImageWidth <= 0)
        || (outputImageHeight <= 0)) {
        BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
        
        if (browserWindow != NULL) {
            int32_t widgetWidth(0), widgetHeight(0);
            browserWindow->getGraphicsWidgetSize(captureOffsetX,
                                                 captureOffsetY,
                                                 captureWidth,
                                                 captureHeight,
                                                 widgetWidth,
                                                 widgetHeight,
                                                 true); /* true => apply lock aspect ratio */
            outputImageWidth  = captureWidth;
            outputImageHeight = captureHeight;
        }
    }
    
    /*
     * Disable tab highlighting when capturing images
     */
    BrainOpenGL::setAllowTabHighlighting(false);
    
    /*
     * Force immediate mode since problems with display lists
     * in image capture.
     */
    BrainOpenGLShape::setImmediateModeOverride(true);
    
    QImage image;
    
    const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    const ImageCaptureMethodEnum::Enum imageCaptureMethod = prefs->getImageCaptureMethod();
    
    switch (imageCaptureMethod) {
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_GRAB_FRAME_BUFFER:
        {
            /*
             * Grab frame buffer seems to have a bug in that it grabs
             * the previous buffer on Mac so repaint to ensure frame
             * buffer is updated.  (repaint() updates immediately,
             * update() is a scheduled update).
             */
            repaintGraphics();
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            image = grabFramebuffer();
#else
            image = grabFrameBuffer();
#endif
            if ((captureWidth > 0)
                && (captureHeight > 0)) {
                image = image.copy(captureOffsetX,
                                   captureOffsetY,
                                   captureWidth,
                                   captureHeight);
            }
            
            if ((outputImageWidth != image.width())
                || (outputImageHeight != image.height())) {
                image = image.scaled(outputImageWidth,
                                     outputImageHeight,
                                     Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation);
            }
            
        }
            break;
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_RENDER_PIXMAP:
        {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            WuQMessageBox::errorOk(this, "Render PixMap not implemented with Qt5");
#else
            /*
             * When the user chooses to exclude regions
             * caused by locking of tab/window aspect ratio,
             * the pixmap is rendered to the output width and
             * height and in the correct aspect ratio so when
             * the rendering takes place, there is no empty 
             * region caused by aspect locking that needs to 
             * be excluded.
             */
            
            QPixmap pixmap = this->renderPixmap(outputImageWidth,
                                                outputImageHeight,
                                                true);
            image = pixmap.toImage();
#endif
        }
            break;
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_OFFSCREEN_FRAME_BUFFER:
        {
            image = performOffScreenImageCapture(outputImageWidth,
                                                 outputImageHeight);
        }
            break;
    }
    
    if ((image.size().width() <= 0)
        || (image.size().height() <= 0)) {
        imageCaptureEvent->setErrorMessage("Image capture appears to have failed (invalid size).");
    }
    else {
        imageCaptureEvent->setImage(image);
        
        uint8_t backgroundColor[3];
        s_singletonOpenGL->getBackgroundColor(backgroundColor);
        imageCaptureEvent->setBackgroundColor(backgroundColor);
    }
    
    BrainOpenGLShape::setImmediateModeOverride(false);
    BrainOpenGL::setAllowTabHighlighting(true);
    
    this->resizeGL(oldSizeX, oldSizeY);
}


/**
 * Initialize the OpenGL format.  This must be called
 * prior to initializing an instance of this class so
 * that the OpenGL is setup properly.
 */
void
BrainOpenGLWidget::initializeDefaultGLFormat()
{
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    QSurfaceFormat glfmt;
    glfmt.setAlphaBufferSize(8);
    glfmt.setBlueBufferSize(8);
    glfmt.setDepthBufferSize(24);
    glfmt.setGreenBufferSize(8);
    glfmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    glfmt.setRedBufferSize(8);
    glfmt.setRenderableType(QSurfaceFormat::OpenGL);
    /*
     * Values greater than zero for setSamples() cause an OpenGL error in
     * glReadPixels().
     * QtBug-43127
     */
    glfmt.setSamples(0); //6);
    glfmt.setStereo(false);
    glfmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    
    glfmt.setMajorVersion(2);
    glfmt.setMinorVersion(1);
    QSurfaceFormat::setDefaultFormat(glfmt);
#else
    QGLFormat glfmt;
    glfmt.setAccum(false);
    glfmt.setAlpha(true);
    glfmt.setAlphaBufferSize(8);
    glfmt.setDepth(true);
    glfmt.setDepthBufferSize(24);
    glfmt.setDirectRendering(true);
    glfmt.setDoubleBuffer(true);
    glfmt.setOverlay(false);
    glfmt.setProfile(QGLFormat::CompatibilityProfile);
    glfmt.setVersion(2, 1);
    glfmt.setSampleBuffers(true);
    glfmt.setStencil(false);
    glfmt.setStereo(false);
    
    glfmt.setRgba(true);
    glfmt.setRedBufferSize(8);
    glfmt.setGreenBufferSize(8);
    glfmt.setBlueBufferSize(8);
    QGLFormat::setDefaultFormat(glfmt);
#endif
    
    s_defaultGLFormatInitialized = true;
}

/**
 * Process a mouse event from the macro system.
 *
 * @param me
 *     The mouse event
 */
void
BrainOpenGLWidget::processMouseEventFromMacro(QMouseEvent* me)
{
    m_mousePositionValid = true;
    switch (me->type()) {
        case QEvent::MouseButtonDblClick:
            mouseDoubleClickEvent(me);
            break;
        case QEvent::MouseButtonPress:
            mousePressEvent(me);
            break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEvent(me);
            break;
        case QEvent::MouseMove:
            mouseMoveEvent(me);
            break;
        default:
            CaretAssert(0);
            break;
    }
    m_mousePositionValid = false;
}

/*
 * @return The selected input processor
 */
UserInputModeAbstract*
BrainOpenGLWidget::getSelectedInputProcessor() const
{
    EventGetOrSetUserInputModeProcessor getInputModeEvent(this->windowIndex);
    EventManager::get()->sendEvent(getInputModeEvent.getPointer());
    UserInputModeAbstract* inputProcessor = getInputModeEvent.getUserInputProcessor();
    CaretAssert(inputProcessor);
    return inputProcessor;
}

/*
 * @return The selected input mode
 */
UserInputModeEnum::Enum
BrainOpenGLWidget::getSelectedInputMode() const
{
    return getSelectedInputProcessor()->getUserInputMode();
}


