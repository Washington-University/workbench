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
/*LICENSE_END*/

#include <algorithm>
#include <cmath>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#define __BRAIN_OPENGL_WIDGET_DEFINE__
#include "BrainOpenGLWidget.h"
#undef __BRAIN_OPENGL_WIDGET_DEFINE__

#include "Border.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLWidgetContextMenu.h"
#include "BrainOpenGLWidgetTextRenderer.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BrowserTabYoking.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventModelDisplayControllerGetAll.h"
#include "EventManager.h"
#include "EventBrowserWindowContentGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventGetOrSetUserInputModeProcessor.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "IdentificationItemSurfaceTriangle.h"
#include "IdentificationItemSurfaceNode.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "ModelDisplayController.h"
#include "ModelDisplayControllerYokingGroup.h"
#include "MouseEvent.h"
#include "Surface.h"
#include "UserInputModeBorders.h"
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
    this->borderBeingDrawn->setColor(CaretColorEnum::RED);
    this->textRenderer = new BrainOpenGLWidgetTextRenderer(this);
    this->windowIndex = windowIndex;
    this->userInputBordersModeProcessor = new UserInputModeBorders(this->borderBeingDrawn,
                                                                   windowIndex);
    this->userInputViewModeProcessor = new UserInputModeView();
    this->selectedUserInputProcessor = this->userInputViewModeProcessor;
    this->selectedUserInputProcessor->initialize();
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GET_OR_SET_USER_INPUT_MODE);
}

/**
 * Destructor.
 */
BrainOpenGLWidget::~BrainOpenGLWidget()
{
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

    this->setFocusPolicy(Qt::StrongFocus);
    
    QGLFormat format = this->format();
    
    AString msg = ("Accum: " + AString::fromBool(format.accum())
                   + "\nAccum size: " + AString::number(format.accumBufferSize())
                   + "\nAlpha: " + AString::fromBool(format.alpha())
                   + "\nAlpha size: " + AString::number(format.alphaBufferSize())
                   + "\nDepth: " + AString::fromBool(format.depth())
                   + "\nDepth size: " + AString::number(format.depthBufferSize())
                   + "\nRed size: " + AString::number(format.redBufferSize())
                   + "\nGreen size: " + AString::number(format.greenBufferSize())
                   + "\nBlue size: " + AString::number(format.blueBufferSize())
                   + "\nDouble Buffer: " + AString::fromBool(format.doubleBuffer())
                   + "\nRGBA: " + AString::fromBool(format.rgba())
                   + "\nMajor Version: " + AString::number(format.majorVersion())
                   + "\nMinor Version: " + AString::number(format.minorVersion()));
    CaretLogConfig(msg);
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
 * Paints the graphics.
 */
void 
BrainOpenGLWidget::paintGL()
{
    this->clearDrawingViewportContents();
    
    int viewport[4] = {
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
    
    const int32_t numToDraw = getModelEvent.getNumberOfItemsToDraw();
    if (numToDraw == 1) {
        BrainOpenGLViewportContent* vc = new BrainOpenGLViewportContent(viewport,
                                                                        getModelEvent.getTabContentToDraw(0));
        this->drawingViewportContents.push_back(vc);
    }
    else if (numToDraw > 1) {
        /**
         * Determine the number of rows and columns for the montage.
         * Since screen width typically exceeds height, always have
         * columns greater than or equal to rows.
         */
        int32_t numRows = (int)std::sqrt((double)numToDraw);
        int32_t numCols = numRows;
        int32_t row2 = numRows * numRows;
        if (row2 < numToDraw) {
            numCols++;
        }
        if ((numRows * numCols) < numToDraw) {
            numRows++;
        }
        
        int32_t vpX = 0;
        int32_t vpY = 0;
        const int32_t vpWidth = this->windowWidth[this->windowIndex] / numCols;
        const int32_t vpHeight = this->windowHeight[this->windowIndex] / numRows;
        
        int32_t iModel = 0;
        for (int32_t i = 0; i < numRows; i++) {
            vpX = 0;
            for (int32_t j = 0; j < numCols; j++) {
                if (iModel < numToDraw) {
                    viewport[0] = vpX;
                    viewport[1] = vpY;
                    viewport[2] = vpWidth;
                    viewport[3] = vpHeight;
                    BrainOpenGLViewportContent* vc = 
                       new BrainOpenGLViewportContent(viewport,
                                                   getModelEvent.getTabContentToDraw(iModel));
                    this->drawingViewportContents.push_back(vc);
                }
                iModel++;
                vpX += vpWidth;
            }
            vpY += vpHeight;
        }
    }
    
    if (this->selectedUserInputProcessor == userInputBordersModeProcessor) {
        this->openGL->setBorderBeingDrawn(this->borderBeingDrawn);
    }
    else {
        this->openGL->setBorderBeingDrawn(NULL);
    }
    this->openGL->drawModels(this->drawingViewportContents);
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
    
    IdentificationManager* idManager = this->performIdentification(x, y);
    
    BrainOpenGLWidgetContextMenu contextMenu(idManager,
                                             tabContent,
                                             this);
    contextMenu.exec(contextMenuEvent->globalPos());
}

/**
 * Receive Mouse Wheel events from Qt.
 * @param we
 *   The wheel event.
 */
void 
BrainOpenGLWidget::wheelEvent(QWheelEvent* we)
{
    const Qt::KeyboardModifiers keyModifiers = we->modifiers();
    
    const int wheelX = we->x();
    const int wheelY = this->windowHeight[this->windowIndex] - we->y();
    int delta = we->delta();
    delta = MathFunctions::limitRange(delta, -2, 2);
    
    MouseEvent mouseEvent(MouseEventTypeEnum::WHEEL_MOVED,
                          keyModifiers,
                          wheelX,
                          wheelY,
                          0,
                          delta);
    this->processMouseEvent(&mouseEvent);
    
    we->accept();
}

/**
 * Receive mouse press events from Qt.
 * @param me
 *    The mouse event.
 */
void 
BrainOpenGLWidget::mousePressEvent(QMouseEvent* me)
{
    const Qt::MouseButton button = me->button();
    const Qt::KeyboardModifiers keyModifiers = me->modifiers();
    
    if (button == Qt::LeftButton) {
        const int mouseX = me->x();
        const int mouseY = this->windowHeight[this->windowIndex] - me->y();

        this->mousePressX = mouseX;
        this->mousePressY = mouseY;
        
        MouseEvent mouseEvent(MouseEventTypeEnum::LEFT_PRESSED,
                              keyModifiers,
                              mouseX,
                              mouseY,
                              0,
                              0);
        this->processMouseEvent(&mouseEvent);
        
        this->lastMouseX = mouseX;
        this->lastMouseY = mouseY;

        this->mouseMovementMinimumX = mouseX;
        this->mouseMovementMaximumX = mouseX;
        this->mouseMovementMinimumY = mouseY;
        this->mouseMovementMaximumY = mouseY;
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
    const Qt::MouseButton button = me->button();
    const Qt::KeyboardModifiers keyModifiers = me->modifiers();

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

        if ((absDX <= BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE) 
            && (absDY <= BrainOpenGLWidget::MOUSE_MOVEMENT_TOLERANCE)) {
            MouseEvent mouseEvent(MouseEventTypeEnum::LEFT_CLICKED,
                                  keyModifiers,
                                  mouseX,
                                  mouseY,
                                  dx,
                                  dy);
            this->processMouseEvent(&mouseEvent);
        }
        else {
            MouseEvent mouseEvent(MouseEventTypeEnum::LEFT_RELEASED,
                                  keyModifiers,
                                  mouseX,
                                  mouseY,
                                  dx,
                                  dy);
            this->processMouseEvent(&mouseEvent);
        }
    }
    
    this->mousePressX = -10000;
    this->mousePressY = -10000;
    
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
        this->drawingViewportContents[i]->getViewport(viewport);
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
 * @return
 *    IdentificationManager providing identification information.
 */
IdentificationManager* 
BrainOpenGLWidget::performIdentification(const int x,
                                         const int y)
{
    BrainOpenGLViewportContent* idViewport = this->getViewportContentAtXY(x, y);

    this->makeCurrent();
    CaretLogFine("Performing selection");
    IdentificationManager* idManager = this->openGL->getIdentificationManager();
    idManager->reset();
    idManager->getSurfaceTriangleIdentification()->setEnabledForSelection(true);
    idManager->getSurfaceNodeIdentification()->setEnabledForSelection(true);
    
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
                                  y);
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
    const Qt::MouseButton button = me->button();
    Qt::KeyboardModifiers keyModifiers = me->modifiers();
    
    if (button == Qt::NoButton) {
        if (me->buttons() == Qt::LeftButton) {
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
                
                MouseEvent mouseEvent(MouseEventTypeEnum::LEFT_DRAGGED,
                                      keyModifiers,
                                      mouseX,
                                      mouseY,
                                      dx,
                                      dy);
                this->processMouseEvent(&mouseEvent);
            }
            
            this->lastMouseX = mouseX;
            this->lastMouseY = mouseY;
        }
    }
    
    me->accept();
}

/**
 * Process a mouse event by sending it to the current
 * user input processor.
 *
 * @param mouseEvent
 *    Mouse event for processing.
 */
void 
BrainOpenGLWidget::processMouseEvent(MouseEvent* mouseEvent)
{
    CaretLogFiner(mouseEvent->toString());
    
    if (mouseEvent->isValid()) {        
        /*
         * Use location of mouse press so that the model
         * being manipulated does not change if mouse moves
         * out of its viewport without releasing the mouse
         * button.
         */
        BrainOpenGLViewportContent* viewportContent = NULL;
        
        if (mouseEvent->getMouseEventType() == MouseEventTypeEnum::WHEEL_MOVED) {
            viewportContent = this->getViewportContentAtXY(mouseEvent->getX(), 
                                                           mouseEvent->getY());
        }
        else {
            viewportContent = this->getViewportContentAtXY(this->mousePressX, 
                                                           this->mousePressY);
        }
        
        if (viewportContent != NULL) {
            BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
            if (browserTabContent != NULL) {
                this->selectedUserInputProcessor->processMouseEvent(mouseEvent,
                                                                    browserTabContent,
                                                                    this);
                
                if (mouseEvent->isUserInterfaceUpdateRequested()) {
                    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
                }
                if (mouseEvent->isGraphicsUpdateOneWindowRequested()) {
                    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
                }
                if (mouseEvent->isGraphicsUpdateAllWindowsRequested()) {
                    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                }
            }
        }
    }
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
            
            const int32_t numItemsToDraw = getModelEvent.getNumberOfItemsToDraw();
            bool needUpdate = false;
            if (numItemsToDraw > 0) {
                for (int32_t i = 0; i < numItemsToDraw; i++) {
                    BrowserTabContent* btc = getModelEvent.getTabContentToDraw(0);
                    ModelDisplayController* mdc = btc->getModelControllerForDisplay();
                    if (mdc != NULL) {
                        ModelDisplayControllerYokingGroup* mdcyg = btc->getBrowserTabYoking()->getSelectedYokingGroup();
                        if (mdcyg != NULL) {
                            const YokingTypeEnum::Enum yokingType = mdcyg->getYokingType();
                            if (yokingType != YokingTypeEnum::OFF) {
                                needUpdate = true;
                                break;
                            }
                        }
                    }
                }
            }
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
    else {
        
    }
}

/**
 * Capture an image of the window's graphics area using 
 * the given image size.  If either of the image dimensions
 * is zero, the image will be the size of the graphcis 
 * area.
 *
 * @param imageSizeX
 *    Desired X size of image.
 * @param imageSizeY
 *    Desired X size of image.
 * @return
 *    An image of the graphics area.
 */
QImage 
BrainOpenGLWidget::captureImage(const int32_t imageSizeX,
                                const int32_t imageSizeY)
{
    const int oldSizeX = this->windowWidth[this->windowIndex];
    const int oldSizeY = this->windowHeight[this->windowIndex];

    QPixmap pixmap = this->renderPixmap(imageSizeX,
                                        imageSizeY);
    QImage image = pixmap.toImage();
    
    this->resizeGL(oldSizeX, oldSizeY);
    
    return image;
}
