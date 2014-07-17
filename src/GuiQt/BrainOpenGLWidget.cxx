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
#include <QMouseEvent>
#include <QToolTip>
#include <QWheelEvent>

#define __BRAIN_OPENGL_WIDGET_DEFINE__
#include "BrainOpenGLWidget.h"
#undef __BRAIN_OPENGL_WIDGET_DEFINE__

#include "Border.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLShape.h"
#include "BrainOpenGLWidgetContextMenu.h"
#include "BrainOpenGLWidgetTextRenderer.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CursorManager.h"
#include "DummyFontTextRenderer.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "EventBrowserWindowContentGet.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventUserInterfaceUpdate.h"
#include "FtglFontTextRenderer.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "MouseEvent.h"
#include "SelectionManager.h"
#include "SelectionItemSurfaceNode.h"
#include "SessionManager.h"
#include "Surface.h"
#include "TileTabsConfiguration.h"
#include "UserInputModeBorders.h"
#include "UserInputModeFoci.h"
#include "UserInputModeView.h"


using namespace caret;

/**
 * Constructor.
 * 
 * @param
 *   The parent widget.
 */
BrainOpenGLWidget::BrainOpenGLWidget(QWidget* parent,
                                     const int32_t windowIndex)
: QGLWidget(parent)
{
    this->openGL = NULL;
    this->borderBeingDrawn = new Border();

    this->textRenderer = NULL;
    /*
     * Create a FTGL font renderer
     */
    if (this->textRenderer == NULL){
        this->textRenderer = new FtglFontTextRenderer();
        if ( ! this->textRenderer->isValid()) {
            CaretLogWarning("Failed to create FTGL text renderer.");
            delete this->textRenderer;
            this->textRenderer = NULL;
        }
    }
  
    /*
     * If creating previous renderer failed, use QT for text.
     */
    if (this->textRenderer == NULL){
        this->textRenderer = new BrainOpenGLWidgetTextRenderer(this);
        if ( ! this->textRenderer->isValid()) {
            delete this->textRenderer;
            this->textRenderer = NULL;
            CaretLogWarning("Failed to create QT GL text renderer.");
        }
    }

    if (this->textRenderer == NULL) {
        CaretLogSevere("Unable to create a text renderer for OpenGL.");
        this->textRenderer = new DummyFontTextRenderer();
    }
    
    this->windowIndex = windowIndex;
    this->userInputBordersModeProcessor = new UserInputModeBorders(this->borderBeingDrawn,
                                                                   windowIndex);
    this->userInputFociModeProcessor = new UserInputModeFoci(windowIndex);
    this->userInputViewModeProcessor = new UserInputModeView();
    this->selectedUserInputProcessor = this->userInputViewModeProcessor;
    this->selectedUserInputProcessor->initialize();
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_IMAGE_CAPTURE);
}

/**
 * Destructor.
 */
BrainOpenGLWidget::~BrainOpenGLWidget()
{
    makeCurrent();
    
    this->clearDrawingViewportContents();
    
    if (this->textRenderer != NULL) {
        delete this->textRenderer;
        this->textRenderer = NULL;
    }
    
    if (this->openGL != NULL) {
        delete this->openGL;
        this->openGL = NULL;
    }
    delete this->userInputViewModeProcessor;
    delete this->userInputBordersModeProcessor;
    delete this->userInputFociModeProcessor;
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
        this->openGL = new BrainOpenGLFixedPipeline(this->textRenderer); //GuiManager::get()->getBrainOpenGL();
    }
    this->openGL->initializeOpenGL();
    
    this->lastMouseX = 0;
    this->lastMouseY = 0;
    this->isMousePressedNearToolBox = false;

    this->setFocusPolicy(Qt::StrongFocus);
    
    QGLFormat format = this->format();
    
    AString msg = ("OpenGL Context:"
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
            
    msg += ("\n\n" + this->openGL->getOpenGLInformation());

    CaretLogConfig(msg);
    
    if (m_openGLVersionInformation.isEmpty()) {
        m_openGLVersionInformation = msg;
    }
    
    if (s_defaultGLFormatInitialized == false) {
        CaretLogSevere("PROGRAM ERROR: The default QGLFormat has not been set.\n"
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
    AString info = m_openGLVersionInformation;
    
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
#ifdef BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    int32_t numVertexBuffers = 0;
    for (GLuint iBuff = 1; iBuff < 1000; iBuff++) {
        if (glIsBuffer(iBuff)) {
            numVertexBuffers++;
        }
    }
    info += ("\nAt least "
             + AString::number(numVertexBuffers)
             + " vertex buffers are allocated");
#endif // BRAIN_OPENGL_INFO_SUPPORTS_VERTEX_BUFFERS
    
    return info;
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

void
BrainOpenGLWidget::getViewPortSize(int &w, int &h)
{
    w = this->windowWidth[this->windowIndex];
    h = this->windowHeight[this->windowIndex];
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
 * Paints the graphics.
 */
void 
BrainOpenGLWidget::paintGL()
{
    /*
     * Set the cursor to that requested by the user input processor
     */
    CursorEnum::Enum cursor = this->selectedUserInputProcessor->getCursor();
    
    GuiManager::get()->getCursorManager()->setCursorForWidget(this,
                                                              cursor);
    
    this->clearDrawingViewportContents();
    
    int windowViewport[4] = {
        0,
        0,
        this->windowWidth[this->windowIndex],
        this->windowHeight[this->windowIndex]
    };
    
    EventBrowserWindowContentGet getModelEvent(this->windowIndex);
    EventManager::get()->sendEvent(getModelEvent.getPointer());

    if (getModelEvent.isError()) {
        return;
    }

    /*
     * Highlighting of border points
     */
    this->openGL->setDrawHighlightedEndPoints(false);
    if (this->selectedUserInputProcessor == this->userInputBordersModeProcessor) {
        this->openGL->setDrawHighlightedEndPoints(this->userInputBordersModeProcessor->isHighlightBorderEndPoints());
    }
    
    const int32_t numToDraw = getModelEvent.getNumberOfItemsToDraw();
    if (numToDraw == 1) {
        BrainOpenGLViewportContent* vc = new BrainOpenGLViewportContent(windowViewport,
                                                                        windowViewport,
                                                                        false,
                                                                        GuiManager::get()->getBrain(),
                                                                        getModelEvent.getTabContentToDraw(0));
        this->drawingViewportContents.push_back(vc);
    }
    else if (numToDraw > 1) {
        int32_t numRows = 0;
        int32_t numCols = 0;
        
        const int32_t windowWidth = this->windowWidth[this->windowIndex];
        const int32_t windowHeight = this->windowHeight[this->windowIndex];
        
        std::vector<int32_t> rowHeights;
        std::vector<int32_t> columnsWidths;
        
        /*
         * Determine if default configuration for tiles
         */
//        bool defaultTabsLayout = true;
        TileTabsConfiguration* tileTabsConfiguration = getModelEvent.getTileTabsConfiguration();
        CaretAssert(tileTabsConfiguration);
//        if (tileTabsConfiguration != NULL) {
//            if (tileTabsConfiguration->isDefaultConfiguration() == false) {
//                defaultTabsLayout = false;
//            }
//        }
        
        /*
         * NOTE: When computing widths and heights, do not round.
         * Rounding may cause the bottom most row or column to extend
         * outside the graphics region.  Shrinking the last row or 
         * column is not desired since it might cause the last model
         * to be drawn slightly smaller than the others.
         */
//        if (defaultTabsLayout) {
        if (tileTabsConfiguration->isDefaultConfiguration()) {
//            /**
//             * Determine the number of rows and columns for the montage.
//             * Since screen width typically exceeds height, always have
//             * columns greater than or equal to rows.
//             */
//            numRows = (int)std::sqrt((double)numToDraw);
//            numCols = numRows;
//            int32_t row2 = numRows * numRows;
//            if (row2 < numToDraw) {
//                numCols++;
//            }
//            if ((numRows * numCols) < numToDraw) {
//                numRows++;
//            }
            
            /*
             * Update number of rows/columns in the default configuration
             * so that if a scene is saved, the correct number of rows
             * and columns are saved to the scene.
             */
            tileTabsConfiguration->updateDefaultConfigurationRowsAndColumns(numToDraw);
            numRows = tileTabsConfiguration->getNumberOfRows();
            numCols = tileTabsConfiguration->getNumberOfColumns();
            
            for (int32_t i = 0; i < numRows; i++) {
                rowHeights.push_back(windowHeight / numRows);
            }
            for (int32_t i = 0; i < numCols; i++) {
                columnsWidths.push_back(windowWidth / numCols);
            }
        }
        else {
            /*
             * Rows/columns from user configuration
             */
            numRows = tileTabsConfiguration->getNumberOfRows();
            numCols = tileTabsConfiguration->getNumberOfColumns();
            
            /*
             * Determine height of each row
             */
            float rowStretchTotal = 0.0;
            for (int32_t i = 0; i < numRows; i++) {
                rowStretchTotal += tileTabsConfiguration->getRowStretchFactor(i);
            }
            CaretAssert(rowStretchTotal > 0.0);
            for (int32_t i = 0; i < numRows; i++) {
                const int32_t h = static_cast<int32_t>((tileTabsConfiguration->getRowStretchFactor(i) / rowStretchTotal)
                                                              * windowHeight);
                
                rowHeights.push_back(h);
            }
            
            /*
             * Determine width of each column
             */
            float columnStretchTotal = 0.0;
            for (int32_t i = 0; i < numCols; i++) {
                columnStretchTotal += tileTabsConfiguration->getColumnStretchFactor(i);
            }
            CaretAssert(columnStretchTotal > 0.0);
            for (int32_t i = 0; i < numCols; i++) {
                const int32_t w = static_cast<int32_t>((tileTabsConfiguration->getColumnStretchFactor(i) / columnStretchTotal)
                                                              * windowWidth);
                columnsWidths.push_back(w);
            }
        }
        
        CaretAssert(numCols == static_cast<int32_t>(columnsWidths.size()));
        CaretAssert(numRows == static_cast<int32_t>(rowHeights.size()));

        /*
         * Verify all rows fit within the window
         */
        int32_t rowHeightsSum = 0;
        for (int32_t i = 0; i < numRows; i++) {
            rowHeightsSum += rowHeights[i];
        }
        if (rowHeightsSum > windowHeight) {
            CaretLogSevere("PROGRAM ERROR: Tile Tabs total row heights exceed window height");
            rowHeights[numRows - 1] -= (rowHeightsSum - windowHeight);
        }
        
        /*
         * Adjust width of last column so that it does not extend beyond viewport
         */
        int32_t columnWidthsSum = 0;
        for (int32_t i = 0; i < numCols; i++) {
            columnWidthsSum += columnsWidths[i];
        }
        if (columnWidthsSum > windowWidth) {
            CaretLogSevere("PROGRAM ERROR: Tile Tabs total row heights exceed window height");
            columnsWidths[numCols - 1] = columnWidthsSum - windowWidth;
        }
        
        CaretLogFiner("Tile Tabs Row Heights: "
                       + AString::fromNumbers(rowHeights, ", "));
        CaretLogFiner("Tile Tabs Column Widths: "
                       + AString::fromNumbers(columnsWidths, ", "));
        
        /*
         * Arrange models left-to-right and top-to-bottom.
         */
        int32_t vpX = 0;
        int32_t vpY = this->windowHeight[this->windowIndex];
        
        int32_t iModel = 0;
        for (int32_t i = 0; i < numRows; i++) {
            const int32_t vpHeight = rowHeights[i];
            vpX = 0;
            vpY -= vpHeight;
            for (int32_t j = 0; j < numCols; j++) {
                const int32_t vpWidth = columnsWidths[j];
                if (iModel < numToDraw) {
                    const int modelViewport[4] = {
                        vpX,
                        vpY,
                        vpWidth,
                        vpHeight
                    };
                    
                    BrowserTabContent* tabContent = getModelEvent.getTabContentToDraw(iModel);
                    const bool highlightTab = (getModelEvent.getTabIndexForTileTabsHighlighting() == tabContent->getTabNumber());
                    BrainOpenGLViewportContent* vc =
                    new BrainOpenGLViewportContent(modelViewport,
                                                   modelViewport,
                                                   highlightTab,
                                                   GuiManager::get()->getBrain(),
                                                   tabContent);
                    this->drawingViewportContents.push_back(vc);
                }
                iModel++;
                vpX += vpWidth;
                
                if (iModel >= numToDraw) {
                    /*
                     * More cells than models for drawing so set loop
                     * indices so that loops terminate
                     */
                    j = numCols;
                    i = numRows;
                }
            }
        }
    }
    
    if (this->selectedUserInputProcessor == userInputBordersModeProcessor) {
        this->openGL->setBorderBeingDrawn(this->borderBeingDrawn);
    }
    else {
        this->openGL->setBorderBeingDrawn(NULL);
    }
    this->openGL->drawModels(this->drawingViewportContents);
    
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
    const bool toolTipsEnabled = false;
    if (toolTipsEnabled) {
        if (event->type() == QEvent::ToolTip) {
            QHelpEvent* helpEvent = dynamic_cast<QHelpEvent*>(event);
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
    
    return QGLWidget::event(event);
}


/**
 * Receive Content Menu events from Qt.
 * @param contextMenuEvent
 *    The context menu event.
 */
void 
BrainOpenGLWidget::contextMenuEvent(QContextMenuEvent* contextMenuEvent)
{
    const int x = contextMenuEvent->x();
    const int y1 = contextMenuEvent->y();
    const int y = this->height() - y1;
    
    BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(x, y);
    if (viewportContent == NULL) {
        return;
    }
    BrowserTabContent* tabContent = viewportContent->getBrowserTabContent();
    if (tabContent == NULL) {
        return;
    }
    
    SelectionManager* idManager = this->performIdentification(x,
                                                                   y,
                                                                   false);
    
    BrainOpenGLWidgetContextMenu contextMenu(idManager,
                                             tabContent,
                                             this);
    contextMenu.exec(contextMenuEvent->globalPos());
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
    MouseEvent mouseEvent(viewportContent,
                          this,
                          this->windowIndex,
                          wheelX,
                          wheelY,
                          0,
                          delta,
                          0,
                          0);
    this->selectedUserInputProcessor->mouseLeftDragWithCtrl(mouseEvent);
    
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
                if (keyModifiers == Qt::NoButton) {
                    mouseButtons = Qt::LeftButton;
                    button = Qt::NoButton;
                    keyModifiers = Qt::ShiftModifier;
                }
            }
        }
    }
    else {
        if (button == Qt::MiddleButton) {
            if (keyModifiers == Qt::NoButton) {
                button = Qt::LeftButton;
                keyModifiers = Qt::ShiftModifier;
            }
        }
    }
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

        /*
         * Use location of mouse press so that the model
         * being manipulated does not change if mouse moves
         * out of its viewport without releasing the mouse
         * button.
         */
        BrainOpenGLViewportContent* viewportContent = this->getViewportContentAtXY(this->mousePressX,
                                                                                   this->mousePressY);
        
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
                                  this->mousePressY);
            
            if (keyModifiers == Qt::NoButton) {
                this->selectedUserInputProcessor->mouseLeftClick(mouseEvent);
            }
            else if (keyModifiers == Qt::ShiftModifier) {
                this->selectedUserInputProcessor->mouseLeftClickWithShift(mouseEvent);
            }
        }
    }
    
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    this->isMousePressedNearToolBox = false;
    
    me->accept();
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
        this->drawingViewportContents[i]->getModelViewport(viewport);
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
 * Perform identification.
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
    
    if (idViewport != NULL) {
        /*
         * ID coordinate needs to be relative to the viewport
         *
        int vp[4];
        idViewport->getViewport(vp);
        const int idX = x - vp[0];
        const int idY = y - vp[1];
         */
        this->openGL->selectModel(idViewport, 
                                  x, 
                                  y,
                                  applySelectionBackgroundFiltering);
    }
    return idManager;
}

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
        this->openGL->projectToModel(projectionViewport,
                                     x,
                                     y,
                                     projectionOut);
    }
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
    
    if (button == Qt::NoButton) {
        if (mouseButtons == Qt::LeftButton) {
            const int mouseX = me->x();
            const int mouseY = this->windowHeight[this->windowIndex] - me->y();
            
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
                
                MouseEvent mouseEvent(viewportContent,
                                      this,
                                      this->windowIndex,
                                      mouseX,
                                      mouseY,
                                      dx,
                                      dy,
                                      this->mousePressX,
                                      this->mousePressY);

                if (keyModifiers == Qt::NoButton) {
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
            }
            
            this->lastMouseX = mouseX;
            this->lastMouseY = mouseY;
        }
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
    if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS) {
        EventGraphicsUpdateAllWindows* updateAllEvent =
            dynamic_cast<EventGraphicsUpdateAllWindows*>(event);
        CaretAssert(updateAllEvent);
        
        updateAllEvent->setEventProcessed();
        
        if (updateAllEvent->isRepaint()) {
            this->repaint();
        }
        else {
            this->updateGL();
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {
        EventGraphicsUpdateOneWindow* updateOneEvent =
        dynamic_cast<EventGraphicsUpdateOneWindow*>(event);
        CaretAssert(updateOneEvent);
        
        if (updateOneEvent->getWindowIndex() == this->windowIndex) {
            updateOneEvent->setEventProcessed();
            
            this->updateGL();
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
                this->updateGL();
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
                UserInputReceiverInterface* newUserInputProcessor = NULL;
                switch (inputModeEvent->getUserInputMode()) {
                    case UserInputReceiverInterface::INVALID:
                        CaretAssertMessage(0, "INVALID is NOT allowed for user input mode");
                        break;
                    case UserInputReceiverInterface::BORDERS:
                        newUserInputProcessor = this->userInputBordersModeProcessor;
                        break;
                    case UserInputReceiverInterface::FOCI:
                        newUserInputProcessor = this->userInputFociModeProcessor;
                        break;
                    case UserInputReceiverInterface::VIEW:
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
    const int imageSizeX = imageCaptureEvent->getImageSizeX();
    const int imageSizeY = imageCaptureEvent->getImageSizeY();
    
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
            image = grabFrameBuffer();
            
            /*
             * If image was captured successfully and the caller has 
             * requested that the image be a specific size, scale
             * the image to the requested size.
             */
            if ((image.width() > 0)
                && (image.height() > 0)) {
                if ((imageSizeX > 0)
                    && (imageSizeY > 0)) {
                    if ((image.width() != imageSizeX)
                        || (image.height() != imageSizeY)) {
                        image = image.scaled(imageSizeX,
                                             imageSizeY,
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation);
                    }
                }
            }
        }
            break;
        case ImageCaptureMethodEnum::IMAGE_CAPTURE_WITH_RENDER_PIXMAP:
        {
            QPixmap pixmap = this->renderPixmap(imageSizeX,
                                                imageSizeY);
            image = pixmap.toImage();
        }
            break;
    }
    
    if ((image.size().width() <= 0)
        || (image.size().height() <= 0)) {
        imageCaptureEvent->setErrorMessage("Image capture appears to have failed (invalid size).");
        return;
    }
    
    imageCaptureEvent->setImage(image);
    
    uint8_t backgroundColor[3];
    this->openGL->getBackgroundColor(backgroundColor);
    imageCaptureEvent->setBackgroundColor(backgroundColor);
    
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
    QGLFormat glfmt;
    glfmt.setAccum(false);
    glfmt.setAlpha(true);
    glfmt.setAlphaBufferSize(8);
    glfmt.setDepth(true);
    glfmt.setDepthBufferSize(24);
    glfmt.setDirectRendering(true);
    glfmt.setDoubleBuffer(true);
    glfmt.setOverlay(false);
    glfmt.setSampleBuffers(false);
    glfmt.setStencil(false);
    glfmt.setStereo(false);
    
    glfmt.setRgba(true);
    glfmt.setRedBufferSize(8);
    glfmt.setGreenBufferSize(8);
    glfmt.setBlueBufferSize(8);
    QGLFormat::setDefaultFormat(glfmt);
    
    s_defaultGLFormatInitialized = true;
}


