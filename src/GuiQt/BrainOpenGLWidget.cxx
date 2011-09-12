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

#include <cmath>
#include <iostream>

#include <QMouseEvent>

#define __BRAIN_OPENGL_WIDGET_DEFINE__
#include "BrainOpenGLWidget.h"
#undef __BRAIN_OPENGL_WIDGET_DEFINE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "EventModelDisplayControllerGetAll.h"
#include "EventManager.h"
#include "EventUpdateAllGraphics.h"
#include "GuiGlobals.h"
#include "Matrix4x4.h"
#include "Surface.h"
#include "ModelDisplayController.h"

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
    
    
//    GuiGlobals::registerBrainOpenGLWidget(this->windowIndex, this);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL);
}

/**
 * Destructor.
 */
BrainOpenGLWidget::~BrainOpenGLWidget()
{
//    GuiGlobals::registerBrainOpenGLWidget(this->windowIndex, NULL);
    
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Set the model controller displayed in this widget.
 *
 * @param modelController
 *    Model controller for display in widget.
 */
void 
BrainOpenGLWidget::setDisplayedModelController(ModelDisplayController* modelController)
{
    this->modelController = modelController;
}

/**
 * Get the model controller displayed in this widget.
 *
 * @return
 *    Model controller displayed in this widget which
 *    may be NULL if there is no viewer for
 *    display.
 */
ModelDisplayController* 
BrainOpenGLWidget::getDisplayedModelController()
{
    EventModelDisplayControllerGetAll getModelEvent;
    EventManager::get()->sendEvent(getModelEvent.getPointer());
    
    return getModelEvent.getFirstModelDisplayController();
}

/**
 * Initializes graphics.
 */
void 
BrainOpenGLWidget::initializeGL()
{
    this->openGL = GuiGlobals::getBrainOpenGL();
    this->openGL->initializeOpenGL();
    
    this->mouseMovedBounds[0] = 0;
    this->mouseMovedBounds[1] = 0;
    this->mouseMovedBounds[2] = 0;
    this->mouseMovedBounds[3] = 0;
    
    this->lastMouseX = 0;
    this->lastMouseY = 0;

    //
    // Mouse button move masks
    // Note: On Macintoshes, Qt::ControlButton is the Apple key
    //
    leftMouseButtonMoveMask        = Qt::NoModifier;
    leftShiftMouseButtonMoveMask   = Qt::ShiftModifier;
    leftControlMouseButtonMoveMask = Qt::ControlModifier;
    leftAltMouseButtonMoveMask     = Qt::AltModifier;
    
    //
    // Mouse button press masks
    // Note: On Macintoshes, Qt::ControlButton is the Apple key
    //
    leftMouseButtonPressMask        = Qt::NoButton;
    leftShiftMouseButtonPressMask   = Qt::ShiftModifier;
    leftControlMouseButtonPressMask = Qt::ControlModifier;
    
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
    
    ModelDisplayController* modelController = this->getDisplayedModelController();
    
    this->openGL->drawModel(GuiGlobals::getBrain(),
                             this->windowIndex,
                             viewport,
                             modelController);
}

void 
BrainOpenGLWidget::mousePressEvent(QMouseEvent* me)
{
    this->lastMouseX = me->x();
    this->lastMouseY = me->y();
}

void 
BrainOpenGLWidget::mouseMoveEvent(QMouseEvent* me)
{
    Qt::KeyboardModifiers bs = me->modifiers();
    Qt::MouseButtons button = me->buttons();
    const int x = me->x();
    const int y = me->y();
    
    mouseMovedBounds[0] = std::min(mouseMovedBounds[0], x);
    mouseMovedBounds[1] = std::min(mouseMovedBounds[1], y);
    mouseMovedBounds[2] = std::max(mouseMovedBounds[2], x);
    mouseMovedBounds[3] = std::max(mouseMovedBounds[3], y);
    
    const int dx = static_cast<int>(std::abs(static_cast<double>(mouseMovedBounds[0] - mouseMovedBounds[2])));
    const int dy = static_cast<int>(std::abs(static_cast<double>(mouseMovedBounds[1] - mouseMovedBounds[3])));
    
    bool doIt = true;
//    if (mouseMode == MOUSE_MODE_VIEW) {
        //
        // Ignore very small movements in view mode to allow ID operation
        //
    const int mouseMoveTolerance = 2;
        if ((dx <= mouseMoveTolerance) && (dy <= mouseMoveTolerance)) {
            doIt = false;
        }
//    }
    
    if (doIt) {
        //QT4if (bs & leftMouseButtonMoveMask) {
        if (button == Qt::LeftButton) {
            const int dx = static_cast<int>((x - lastMouseX));
            const int dy = static_cast<int>((lastMouseY - y));  // origin at top
            
            ModelDisplayController* modelController = this->getDisplayedModelController();
            if (modelController != NULL) {
                //
                // Mouse moved with just left button down
                //
                if (bs == leftMouseButtonMoveMask) {
                    Matrix4x4* rotationMatrix = modelController->getViewingRotationMatrix(this->windowIndex);
                    rotationMatrix->rotateX(-dy);
                    rotationMatrix->rotateY(dx);
                }
                //
                // Mouse moved with control key and left mouse button down
                //
                else if (bs == leftControlMouseButtonMoveMask) {
                    float scale = modelController->getScaling(this->windowIndex);
                    if (dy != 0) {
                        scale += (dy * 0.05);
                    }
                    if (scale < 0.01) scale = 0.01;
                    modelController->setScaling(this->windowIndex, scale);
                }
                //
                // Mouse moved with shift key and left mouse button down
                //
                else if (bs == leftShiftMouseButtonMoveMask) {
                    const float* t1 = modelController->getTranslation(this->windowIndex);
                    float t2[] = { t1[0] + dx, t1[1] + dy, t2[2] };
                    modelController->setTranslation(this->windowIndex, t2);
                }
                //
                // Mouse moved with alt key and left mouse button down
                //
                else if (bs == leftAltMouseButtonMoveMask) {
                }
                
                this->updateGL();
            }
            
        }
    }
    
    lastMouseX = x;
    lastMouseY = y;
}

void 
BrainOpenGLWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_GRAPHICS_UPDATE_ALL) {
        EventUpdateAllGraphics* updateAllEvent =
            dynamic_cast<EventUpdateAllGraphics*>(event);
        CaretAssert(updateAllEvent);
        
        updateAllEvent->setEventProcessed();
        
        std::cout << "Received update graphics event in " << __func__ << std::endl;
        
        this->updateGL();
    }
}


