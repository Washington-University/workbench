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

#include <QMouseEvent>

#define __BRAIN_OPENGL_WIDGET_DEFINE__
#include "BrainOpenGLWidget.h"
#undef __BRAIN_OPENGL_WIDGET_DEFINE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventModelDisplayControllerGetAll.h"
#include "EventManager.h"
#include "EventBrowserWindowContentGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "IdentificationItemSurfaceTriangle.h"
#include "IdentificationItemSurfaceNode.h"
#include "Matrix4x4.h"
#include "ModelDisplayController.h"
#include "MouseEvent.h"
#include "Surface.h"
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
    this->windowIndex = windowIndex;
    this->modelController = NULL;
    this->userInputViewModeProcessor = new UserInputModeView();
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW);
}

/**
 * Destructor.
 */
BrainOpenGLWidget::~BrainOpenGLWidget()
{
    if (this->openGL != NULL) {
        delete this->openGL;
        this->openGL = NULL;
    }
    delete this->userInputViewModeProcessor;
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Initializes graphics.
 */
void 
BrainOpenGLWidget::initializeGL()
{
    this->openGL = new BrainOpenGL(); //GuiManager::get()->getBrainOpenGL();
    this->openGL->initializeOpenGL();
    
    this->lastMouseX = 0;
    this->lastMouseY = 0;

    this->setFocusPolicy(Qt::StrongFocus);
}

/**
 * Called when widget is resized.
 */
void 
BrainOpenGLWidget::resizeGL(int w, int h)
{
    this->openGL->updateOrthoSize(0, w, h);
    this->windowWidth[this->windowIndex] = w;
    this->windowHeight[this->windowIndex] = h;
}

/**
 * Paints the graphics.
 */
void 
BrainOpenGLWidget::paintGL()
{
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
    
    this->browserTabContent = getModelEvent.getBrowserTabContent();
    this->modelController = getModelEvent.getModelDisplayController();
    this->windowTabIndex  = getModelEvent.getWindowTabNumber();
    
    this->openGL->drawModel(this->modelController,
                            this->browserTabContent,
                            this->windowTabIndex,
                            viewport);
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
    int viewport[4] = {
        0,
        0,
        this->windowWidth[this->windowIndex],
        this->windowHeight[this->windowIndex]
    };
    
    this->makeCurrent();
    CaretLogFine("Performing selection");
    IdentificationManager* idManager = this->openGL->getIdentificationManager();
    idManager->reset();
    idManager->getSurfaceTriangleIdentification()->setEnabledForSelection(true);
    idManager->getSurfaceNodeIdentification()->setEnabledForSelection(true);
    this->openGL->selectModel(this->modelController, 
                              this->browserTabContent, 
                              this->windowTabIndex, 
                              viewport, 
                              x, 
                              y);
    return idManager;
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
        
        this->userInputViewModeProcessor->processMouseEvent(mouseEvent,
                                    this->browserTabContent,
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
        
        this->updateGL();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ONE_WINDOW) {
        EventGraphicsUpdateOneWindow* updateOneEvent =
        dynamic_cast<EventGraphicsUpdateOneWindow*>(event);
        CaretAssert(updateOneEvent);
        
        if (updateOneEvent->getWindowIndex() == this->windowIndex) {
            updateOneEvent->setEventProcessed();
            
            this->updateGL();
        }
    }
    else {
        
    }
}


