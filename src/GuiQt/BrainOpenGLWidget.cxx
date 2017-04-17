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

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
#include <QOpenGLContext>
#endif
#include <QToolTip>
#include <QWheelEvent>

#define __BRAIN_OPENGL_WIDGET_DEFINE__
#include "BrainOpenGLWidget.h"
#undef __BRAIN_OPENGL_WIDGET_DEFINE__

#include "Border.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLShape.h"
#include "BrainOpenGLTextureManager.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CursorManager.h"
#include "DummyFontTextRenderer.h"
#include "EventBrainReset.h"
#include "EventImageCapture.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "EventBrowserWindowContentGet.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGetBrainOpenGLTextRenderer.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventIdentificationRequest.h"
#include "EventOpenGLTexture.h"
#include "EventUserInterfaceUpdate.h"
#include "FtglFontTextRenderer.h"
#include "GuiManager.h"
#include "KeyEvent.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "MouseEvent.h"
#include "QGLWidgetTextRenderer.h"
#include "SelectionManager.h"
#include "SelectionItemAnnotation.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxelEditing.h"
#include "SessionManager.h"
#include "Surface.h"
#include "TileTabsConfiguration.h"
#include "UserInputModeAnnotations.h"
#include "UserInputModeBorders.h"
#include "UserInputModeFoci.h"
#include "UserInputModeImage.h"
#include "UserInputModeView.h"
#include "UserInputModeVolumeEdit.h"
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
 */
BrainOpenGLWidget::BrainOpenGLWidget(QWidget* parent,
                                     const BrainOpenGLWidget* shareWidget,
                                     const int32_t windowIndex)
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
: QOpenGLWidget(parent),
#else
: QGLWidget(parent,
            shareWidget),
#endif
windowIndex(windowIndex),
m_textRenderer(NULL)
{
    this->openGL = NULL;
    this->borderBeingDrawn = new Border();
    
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
                                                false));
    
    this->userInputAnnotationsModeProcessor = new UserInputModeAnnotations(windowIndex);
    this->userInputBordersModeProcessor = new UserInputModeBorders(this->borderBeingDrawn,
                                                                   windowIndex);
    this->userInputFociModeProcessor = new UserInputModeFoci(windowIndex);
    this->userInputImageModeProcessor = new UserInputModeImage(windowIndex);
    this->userInputVolumeEditModeProcessor = new UserInputModeVolumeEdit(windowIndex);
    this->userInputViewModeProcessor = new UserInputModeView();
    this->selectedUserInputProcessor = this->userInputViewModeProcessor;
    this->selectedUserInputProcessor->initialize();
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
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_TEXT_RENDERER_FOR_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_IDENTIFICATION_REQUEST);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_IMAGE_CAPTURE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /*
     * Default format is set by desktop.cxx call to initializeDefaultGLFormat().
     */
    setFormat(QSurfaceFormat::defaultFormat());
#endif
}

/**
 * Destructor.
 */
BrainOpenGLWidget::~BrainOpenGLWidget()
{
    makeCurrent();
    
    /** DO NOT delete since not owned by this */
    m_textRenderer = NULL;
    
    this->clearDrawingViewportContents();
    
    if (this->openGL != NULL) {
        delete this->openGL;
        this->openGL = NULL;
    }
    delete this->userInputViewModeProcessor;
    delete this->userInputAnnotationsModeProcessor;
    delete this->userInputBordersModeProcessor;
    delete this->userInputFociModeProcessor;
    delete this->userInputImageModeProcessor;
    delete this->userInputVolumeEditModeProcessor;
    this->selectedUserInputProcessor = NULL; // DO NOT DELETE since it does not own the object to which it points
    
    delete this->borderBeingDrawn;
    
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Initializes graphics.
 */
void 
BrainOpenGLWidget::initializeGL()
{
    if (this->openGL == NULL) {
        this->openGL = new BrainOpenGLFixedPipeline(this->windowIndex,
                                                    createTextRenderer());
    }
    else {
        /*
         * You would think Qt would need to call initializedGL() only once.
         * However, if QGLWidget::renderPixmap() is called, it calls
         * this method since it needs to create a new OpenGL context.
         * Since the OpenGL context contains textures (and the font
         * rendering may use textures), The text renderer must be
         * recreated.  Otherwise, fonts will be garbage in the
         * image produced by QGLWidget::renderPixmap().
         */
        this->openGL->setTextRenderer(createTextRenderer());
    }
    
    this->openGL->initializeOpenGL();
    
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
//             "\n   Accum: " + AString::fromBool(format.accum())
//             + "\n   Accum size: " + AString::number(format.accumBufferSize())
             "\n   Alpha: " + AString::fromBool(format.hasAlpha())
             + "\n   Alpha size: " + AString::number(format.alphaBufferSize())
//             + "\n   Depth: " + AString::fromBool(format.depth())
             + "\n   Depth size: " + AString::number(format.depthBufferSize())
//             + "\n   Direct Rendering: " + AString::fromBool(format.directRendering())
             + "\n   Red size: " + AString::number(format.redBufferSize())
             + "\n   Green size: " + AString::number(format.greenBufferSize())
             + "\n   Blue size: " + AString::number(format.blueBufferSize())
//             + "\n   Double Buffer: " + AString::fromBool(format.doubleBuffer())
//             + "\n   RGBA: " + AString::fromBool(format.rgba())
//             + "\n   Samples: " + AString::fromBool(format.sampleBuffers())
             + "\n   Samples size: " + AString::number(format.samples())
//             + "\n   Stencil: " + AString::fromBool(format.stencil())
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
    
    info += ("\n\n" + this->openGL->getOpenGLInformation());
    
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
    
// Disable check of vertex buffers until OpenGL
// version is fixed on windows (finds 1.1)
//#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
//    int32_t numVertexBuffers = 0;
//#ifdef GL_VERSION_2_0
//    /*
//     * Note: glIsBuffer() was added to OpenGL 2.0
//     */
//    for (GLuint iBuff = 1; iBuff < 1000; iBuff++) {
//        if (glIsBuffer(iBuff)) {
//            numVertexBuffers++;
//        }
//    }
//#endif // GL_VERSION_2_0
//    info += ("\nAt least "
//             + AString::number(numVertexBuffers)
//             + " vertex buffers are allocated");
//#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    info += "\n";
    
    return info;
}

/**
 * @return A new text renderer.
 */
BrainOpenGLTextRenderInterface*
BrainOpenGLWidget::createTextRenderer()
{
    BrainOpenGLTextRenderInterface* textRendererOut = NULL;
    
    /*
     * Create a FTGL font renderer
     */
    if (textRendererOut == NULL){
        textRendererOut = new FtglFontTextRenderer();
        if ( ! textRendererOut->isValid()) {
            CaretLogWarning("Failed to create FTGL text renderer.");
            delete textRendererOut;
            textRendererOut = NULL;
        }
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    /* No Qt Text Renderer in QOpenGLWidget */
#else
    /*
     * Create a Qt text renderer
     */
    if (textRendererOut == NULL) {
        textRendererOut = new QGLWidgetTextRenderer(this);
        if ( ! textRendererOut->isValid()) {
            CaretLogWarning("Failed to create Qt text renderer.");
            delete textRendererOut;
            textRendererOut = NULL;
        }
    }
#endif
    
    if (textRendererOut == NULL) {
        CaretLogSevere("Unable to create a text renderer for OpenGL.  "
                       "No text will be drawn (dummy text renderer).");
        textRendererOut = new DummyFontTextRenderer();
    }
    
    m_textRenderer = textRendererOut;
    
    return textRendererOut;
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
 * @return Pointer to the border that is being drawn.
 */
Border* 
BrainOpenGLWidget::getBorderBeingDrawn()
{
    return this->borderBeingDrawn;
}

/**
 * Clear the contents for drawing into the viewports.
 */
void 
BrainOpenGLWidget::clearDrawingViewportContents()
{
    const int32_t num = static_cast<int32_t>(this->drawingViewportContents.size());
    for (int32_t i = 0; i < num; i++) {
        delete this->drawingViewportContents[i];
    }
    this->drawingViewportContents.clear();
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
    CursorEnum::Enum cursor = this->selectedUserInputProcessor->getCursor();
    
    GuiManager::get()->getCursorManager()->setCursorForWidget(this,
                                                              cursor);
}

/**
 * Paints the graphics.
 */
void 
BrainOpenGLWidget::paintGL()
{
    updateCursor();
    
    this->clearDrawingViewportContents();
    
    int windowViewport[4] = {
        0,
        0,
        this->windowWidth[this->windowIndex],
        this->windowHeight[this->windowIndex]
    };
    
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
    
    EventBrowserWindowContentGet getModelEvent(this->windowIndex);
    EventManager::get()->sendEvent(getModelEvent.getPointer());
    
    if (getModelEvent.isError()) {
        return;
    }
    
    if (bbw->isAspectRatioLocked()) {
        const float aspectRatio = bbw->getAspectRatio();
        if (aspectRatio > 0.0) {
            BrainOpenGLViewportContent::adjustViewportForAspectRatio(windowViewport,
                                                                     aspectRatio);
        }
    }
    
    /*
     * Highlighting of border points
     */
    this->openGL->setDrawHighlightedEndPoints(false);
    if (this->selectedUserInputProcessor == this->userInputBordersModeProcessor) {
        this->openGL->setDrawHighlightedEndPoints(this->userInputBordersModeProcessor->isHighlightBorderEndPoints());
    }
    
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    
    const int32_t numToDraw = getModelEvent.getNumberOfItemsToDraw();
    if (numToDraw == 1) {
        BrainOpenGLViewportContent* vc = BrainOpenGLViewportContent::createViewportForSingleTab(getModelEvent.getTabContentToDraw(0),
                                                                        gapsAndMargins,
                                                                        this->windowIndex,
                                                                        windowViewport);
        this->drawingViewportContents.push_back(vc);
    }
    else if (numToDraw > 1) {
        const int32_t windowWidth  = windowViewport[2];
        const int32_t windowHeight = windowViewport[3];
        
        std::vector<int32_t> rowHeights;
        std::vector<int32_t> columnsWidths;
        
        /*
         * Determine if default configuration for tiles
         */
        TileTabsConfiguration* tileTabsConfiguration = getModelEvent.getTileTabsConfiguration();
        CaretAssert(tileTabsConfiguration);
        
        /*
         * Get the sizes of the tab tiles from the tile tabs configuration
         */
        if ( ! tileTabsConfiguration->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                                windowHeight,
                                                                                numToDraw,
                                                                                rowHeights,
                                                                                columnsWidths)) {
            CaretLogSevere("Tile Tabs Row/Column sizing failed !!!");
            return;
        }
        
        /*
         * Create the viewport drawing contents for all tabs
         */
        std::vector<BrowserTabContent*> allTabs;
        for (int32_t i = 0; i < getModelEvent.getNumberOfItemsToDraw(); i++) {
            allTabs.push_back(getModelEvent.getTabContentToDraw(i));
        }
        
        this->drawingViewportContents = BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabs,
                                                                                                     tileTabsConfiguration,
                                                                                                     gapsAndMargins,
                                                                                                     windowIndex,
                                                                                                     windowViewport,
                                                                                                     getModelEvent.getTabIndexForTileTabsHighlighting());
    }
    
    if (this->selectedUserInputProcessor == userInputBordersModeProcessor) {
        this->openGL->setBorderBeingDrawn(this->borderBeingDrawn);
    }
    else {
        this->openGL->setBorderBeingDrawn(NULL);
    }
    this->openGL->drawModels(GuiManager::get()->getBrain(),
                             this->context(),
                             this->drawingViewportContents);
    
    /*
     * Issue browser window redrawn event
     */
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
    const bool toolTipsEnabled = false;
    if (toolTipsEnabled) {
        if (event->type() == QEvent::ToolTip) {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
            CaretAssert(helpEvent);
            
            QPoint globalXY = helpEvent->globalPos();
            QPoint xy = helpEvent->pos();
            
            static int counter = 0;
            
            std::cout
            << "Displaying tooltip "
            << counter++
            << " at global ("
            << globalXY.x()
            << ", "
            << globalXY.y()
            << ") at pos ("
            << xy.x()
            << ", "
            << xy.y()
            << ")"
            << std::endl;
            
            QToolTip::showText(globalXY,
                               "This is the tooltip " + AString::number(counter));
            
            return true;
        }
    }
    
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
    return QOpenGLWidget::event(event);
#else
    return QGLWidget::event(event);
#endif
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
    
    BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
                                                                               mouseY);
    if (viewportContent != NULL) {
        MouseEvent mouseEvent(viewportContent,
                              this,
                              this->windowIndex,
                              mouseX,
                              mouseY,
                              0,
                              0,
                              mouseX,
                              mouseY,
                              false);
        
        this->selectedUserInputProcessor->showContextMenu(mouseEvent,
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
    const int wheelX = we->x();
    const int wheelY = this->windowHeight[this->windowIndex] - we->y();
    int delta = we->delta();
    delta = MathFunctions::limitRange(delta, -2, 2);
    
    /*
     * Use location of mouse press so that the model
     * being manipulated does not change if mouse moves
     * out of its viewport without releasing the mouse
     * button.
     */
    BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(wheelX,
                                                                               wheelY);
    if (viewportContent != NULL) {
        MouseEvent mouseEvent(viewportContent,
                              this,
                              this->windowIndex,
                              wheelX,
                              wheelY,
                              0,
                              delta,
                              0,
                              0,
                              this->mouseNewDraggingStartedFlag);
        this->selectedUserInputProcessor->mouseLeftDragWithCtrl(mouseEvent);
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
    
    KeyEvent keyEvent(this,
                      this->windowIndex,
                      e->key(),
                      m_newKeyPressStartedFlag,
                      shiftKeyDownFlag);
    
    this->selectedUserInputProcessor->keyPressEvent(keyEvent);
    
    e->accept();
    
    m_newKeyPressStartedFlag = false;
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

        BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
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
                                  this->mouseNewDraggingStartedFlag);
            
            if (keyModifiers == Qt::NoModifier) {
                this->selectedUserInputProcessor->mouseLeftPress(mouseEvent);
            }
            else if (keyModifiers == Qt::ShiftModifier) {
                // not implemented  this->selectedUserInputProcessor->mouseLeftPressWithShift(mouseEvent);
            }
        }
    }
    else {
        this->mousePressX = -10000;
        this->mousePressY = -10000;
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

        {
            /*
             * Mouse button RELEASE event
             */
            BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
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
                                      this->mouseNewDraggingStartedFlag);
                this->selectedUserInputProcessor->mouseLeftRelease(mouseEvent);
            }
        }
        
        /*
         * Use location of mouse press so that the model
         * being manipulated does not change if mouse moves
         * out of its viewport without releasing the mouse
         * button.
         */
        BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(this->mousePressX,
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
                                      this->mouseNewDraggingStartedFlag);
                
                if (keyModifiers == Qt::NoModifier) {
                    this->selectedUserInputProcessor->mouseLeftClick(mouseEvent);
                }
                else if (keyModifiers == Qt::ShiftModifier) {
                    this->selectedUserInputProcessor->mouseLeftClickWithShift(mouseEvent);
                }
                else if (keyModifiers == (Qt::ShiftModifier
                                          | Qt::ControlModifier)) {
                    this->selectedUserInputProcessor->mouseLeftClickWithCtrlShift(mouseEvent);
                }
            }
        }
    }
    
    this->mousePressX = -10000;
    this->mousePressY = -10000;
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
            BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
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
                                      this->mouseNewDraggingStartedFlag);
                
                this->selectedUserInputProcessor->mouseLeftDoubleClick(mouseEvent);
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
    
    this->selectedUserInputProcessor->setMousePosition(m_mousePositionEvent,
                                                       m_mousePositionValid);
}


/**
 * Get the viewport content at the given location.
 * @param x
 *    X-coordinate.
 * @param y
 *    Y-coordinate.
 */
BrainOpenGLViewportContent* 
BrainOpenGLWidget::getViewportContentAtXY(const int x,
                                          const int y)
{
    BrainOpenGLViewportContent* viewportContent = NULL;
    const int32_t num = static_cast<int32_t>(this->drawingViewportContents.size());
    for (int32_t i = 0; i < num; i++) {
        int viewport[4];
        this->drawingViewportContents[i]->getTabViewportBeforeApplyingMargins(viewport);
        if ((x >= viewport[0])
            && (x < (viewport[0] + viewport[2]))
            && (y >= viewport[1])
            && (y < (viewport[1] + viewport[3]))) {
            viewportContent = this->drawingViewportContents[i];
            break;
        }
    }
    return viewportContent;
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
    std::vector<const BrainOpenGLViewportContent*> contentOut(this->drawingViewportContents.begin(),
                                                              this->drawingViewportContents.end());
    
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
    BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);

    this->makeCurrent();
    CaretLogFine("Performing selection");
    SelectionManager* idManager = GuiManager::get()->getBrain()->getSelectionManager();
    idManager->reset();
    idManager->setAllSelectionsEnabled(true);
    idManager->getVoxelEditingIdentification()->setEnabledForSelection(false);
    
    if (idViewport != NULL) {
        this->openGL->selectModel(GuiManager::get()->getBrain(),
                                  this->context(),
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
    this->repaint();
    this->doneCurrent();
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
    BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);
    
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
        this->openGL->selectModel(GuiManager::get()->getBrain(),
                                  this->context(),
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
    this->repaint();
    this->doneCurrent();
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
    BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);
    
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
        this->openGL->selectModel(GuiManager::get()->getBrain(),
                                  this->context(),
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
    this->repaint();
    this->doneCurrent();
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
    BrainOpenGLViewportContent* projectionViewport = this->getViewportContentAtXY(x, y);
    
    this->makeCurrent();
    CaretLogFine("Performing projection");
    
    if (projectionViewport != NULL) {
        /*
         * ID coordinate needs to be relative to the viewport
         *
         int vp[4];
         idViewport->getViewport(vp);
         const int idX = x - vp[0];
         const int idY = y - vp[1];
         */
        this->openGL->projectToModel(GuiManager::get()->getBrain(),
                                     this->context(),
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
    this->repaint();
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
    Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    Qt::MouseButtons mouseButtons = me->buttons();
    
    checkForMiddleMouseButton(mouseButtons,
                              button,
                              keyModifiers,
                              true);
    
    const int mouseX = me->x();
    const int mouseY = this->windowHeight[this->windowIndex] - me->y();
    
    if (button == Qt::NoButton) {
        
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
                /*
                 * Use location of mouse press so that the model
                 * being manipulated does not change if mouse moves
                 * out of its viewport without releasing the mouse
                 * button.
                 */
                BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(this->mousePressX,
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
                                          this->mouseNewDraggingStartedFlag);
                    
                    if (keyModifiers == Qt::NoModifier) {
                        this->selectedUserInputProcessor->mouseLeftDrag(mouseEvent);
                    }
                    else if (keyModifiers == Qt::ControlModifier) {
                        this->selectedUserInputProcessor->mouseLeftDragWithCtrl(mouseEvent);
                    }
                    else if (keyModifiers == Qt::ShiftModifier) {
                        this->selectedUserInputProcessor->mouseLeftDragWithShift(mouseEvent);
                    }
                    else if (keyModifiers == Qt::AltModifier) {
                        this->selectedUserInputProcessor->mouseLeftDragWithAlt(mouseEvent);
                    }
                    else if (keyModifiers == (Qt::ShiftModifier
                                              | Qt::ControlModifier)) {
                        this->selectedUserInputProcessor->mouseLeftDragWithCtrlShift(mouseEvent);
                    }
                    
                    this->mouseNewDraggingStartedFlag = false;
                }
            }
            
            this->lastMouseX = mouseX;
            this->lastMouseY = mouseY;
        }
        else if (mouseButtons == Qt::NoButton) {
            BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
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
                                      this->mouseNewDraggingStartedFlag);
                
                if (keyModifiers == Qt::NoModifier) {
                    this->selectedUserInputProcessor->mouseMove(mouseEvent);
                }
                else if (keyModifiers == Qt::ShiftModifier) {
                    this->selectedUserInputProcessor->mouseMoveWithShift(mouseEvent);
                }
            }
            
        }
    }
    
    BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(mouseX,
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
                                                    this->mouseNewDraggingStartedFlag));
        
        this->selectedUserInputProcessor->setMousePosition(m_mousePositionEvent,
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
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_RESET) {
        EventBrainReset* brainResetEvent = dynamic_cast<EventBrainReset*>(event);
        CaretAssert(brainResetEvent);
        
        this->borderBeingDrawn->clear();
        
        brainResetEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_TEXT_RENDERER_FOR_WINDOW) {
        EventGetBrainOpenGLTextRenderer* textRenderEvent = dynamic_cast<EventGetBrainOpenGLTextRenderer*>(event);
        CaretAssert(textRenderEvent);
        
        if (this->windowIndex == textRenderEvent->getWindowIndex()) {
            textRenderEvent->setTextRenderer(m_textRenderer);
            textRenderEvent->setEventProcessed();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS) {
        EventGraphicsUpdateAllWindows* updateAllEvent =
            dynamic_cast<EventGraphicsUpdateAllWindows*>(event);
        CaretAssert(updateAllEvent);
        
        updateAllEvent->setEventProcessed();
        
        if (updateAllEvent->isRepaint()) {
            this->repaint();
        }
        else {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            this->update();
#else
            this->updateGL();
#endif
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {
        EventGraphicsUpdateOneWindow* updateOneEvent =
        dynamic_cast<EventGraphicsUpdateOneWindow*>(event);
        CaretAssert(updateOneEvent);
        
        if (updateOneEvent->getWindowIndex() == this->windowIndex) {
            updateOneEvent->setEventProcessed();
            
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            this->update();
#else
            this->updateGL();
#endif
        }
        else {
            /*
             * If a window is yoked, update its graphics.
             */
            EventBrowserWindowContentGet getModelEvent(this->windowIndex);
            EventManager::get()->sendEvent(getModelEvent.getPointer());
            
            if (getModelEvent.isError()) {
                return;
            }
            
            bool needUpdate = false;
            if (needUpdate) {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
                this->update();
#else
                this->updateGL();
#endif
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE) {
        EventGetOrSetUserInputModeProcessor* inputModeEvent =
        dynamic_cast<EventGetOrSetUserInputModeProcessor*>(event);
        CaretAssert(inputModeEvent);
        
        if (inputModeEvent->getWindowIndex() == this->windowIndex) {
            if (inputModeEvent->isGetUserInputMode()) {
                inputModeEvent->setUserInputProcessor(this->selectedUserInputProcessor);
            }
            else if (inputModeEvent->isSetUserInputMode()) {
                UserInputModeAbstract* newUserInputProcessor = NULL;
                switch (inputModeEvent->getUserInputMode()) {
                    case UserInputModeAbstract::INVALID:
                        CaretAssertMessage(0, "INVALID is NOT allowed for user input mode");
                        break;
                    case UserInputModeAbstract::ANNOTATIONS:
                        newUserInputProcessor = this->userInputAnnotationsModeProcessor;
                        break;
                    case UserInputModeAbstract::BORDERS:
                        newUserInputProcessor = this->userInputBordersModeProcessor;
                        break;
                    case UserInputModeAbstract::FOCI:
                        newUserInputProcessor = this->userInputFociModeProcessor;
                        break;
                    case UserInputModeAbstract::IMAGE:
                        newUserInputProcessor = this->userInputImageModeProcessor;
                        break;
                    case UserInputModeAbstract::VOLUME_EDIT:
                        newUserInputProcessor = this->userInputVolumeEditModeProcessor;
                        break;
                    case UserInputModeAbstract::VIEW:
                        newUserInputProcessor = this->userInputViewModeProcessor;
                        break;
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
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* guiUpdateEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(guiUpdateEvent);
        guiUpdateEvent->setEventProcessed();
        
        this->selectedUserInputProcessor->update();
    }
    else {
        
    }
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
    const int captureOffsetX    = imageCaptureEvent->getCaptureOffsetX();
    const int captureOffsetY    = imageCaptureEvent->getCaptureOffsetY();
    const int captureWidth      = imageCaptureEvent->getCaptureWidth();
    const int captureHeight     = imageCaptureEvent->getCaptureHeight();
    const int outputImageWidth  = imageCaptureEvent->getOutputWidth();
    const int outputImageHeight = imageCaptureEvent->getOutputHeight();
    
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
            repaint();
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            image = grabFramebuffer();
#else
            image = grabFrameBuffer();
#endif
            
            if ((captureOffsetX > 0)
                || (captureOffsetY > 0)) {
                if ((captureWidth > 0)
                    && (captureHeight > 0)) {
                    image = image.copy(captureOffsetX,
                                       captureOffsetY,
                                       captureWidth,
                                       captureHeight);
                }
            }
            
            /*
             * If image was captured successfully and the caller has 
             * requested that the image be a specific size, scale
             * the image to the requested size.
             */
            if ((image.width() > 0)
                && (image.height() > 0)) {
                if ((outputImageWidth > 0)
                    && (outputImageHeight > 0)) {
                    if ((image.width() != outputImageWidth)
                        || (image.height() != outputImageHeight)) {
                        image = image.scaled(outputImageWidth,
                                             outputImageHeight,
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation);
                    }
                }
            }
        }
            break;
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_RENDER_PIXMAP:
        {
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
            WuQMessageBox::errorOk(this, "Render PixMap not implemented with Qt5");
#else
            /*
             * Note 1: QGLWidget::renderPixmap() creates a new OpenGL
             * Context which destroys any textures in the existing
             * OpenGL context.  So, after execution of 
             * QGLWidget::renderPixmap(), we need to create a
             * new text renderer.
             *
             * Note 2: When the user chooses to exclude regions
             * caused by locking of tab/window aspect ratio,
             * the pixmap is rendered to the output width and
             * height and in the correct aspect ratio so when
             * the rendering takes place, there is no empty 
             * region caused by aspect locking that needs to 
             * be excluded.
             */
            BrainOpenGLTextureManager* textureManager = this->openGL->getTextureManager();
            CaretAssert(textureManager);
            textureManager->deleteAllTexturesForWindow(this->windowIndex);
            
            QPixmap pixmap = this->renderPixmap(outputImageWidth,
                                                outputImageHeight);
            image = pixmap.toImage();
            textureManager->deleteAllTexturesForWindow(this->windowIndex);
            this->openGL->setTextRenderer(createTextRenderer());
#endif
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
        this->openGL->getBackgroundColor(backgroundColor);
        imageCaptureEvent->setBackgroundColor(backgroundColor);
    }

    BrainOpenGLShape::setImmediateModeOverride(false);
    
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
    //glfmt.setAccum(false);
    //glfmt.setAlpha(true);
    glfmt.setAlphaBufferSize(8);
    glfmt.setBlueBufferSize(8);
    //glfmt.setDepth(true);
    glfmt.setDepthBufferSize(24);
    //glfmt.setDirectRendering(true);
    //glfmt.setDoubleBuffer(true);
    glfmt.setGreenBufferSize(8);
    //glfmt.setOverlay(false);
    glfmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    glfmt.setRedBufferSize(8);
    glfmt.setRenderableType(QSurfaceFormat::OpenGL);
    //glfmt.setSampleBuffers(false);
    //glfmt.setStencil(false);
    glfmt.setStereo(false);
    glfmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //glfmt.setRgba(true);
    
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
    glfmt.setSampleBuffers(false);
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
