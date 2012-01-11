
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __USER_INPUT_MODE_BORDERS_DECLARE__
#include "UserInputModeBorders.h"
#undef __USER_INPUT_MODE_BORDERS_DECLARE__

#include "Border.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "ModelDisplayController.h"
#include "MouseEvent.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "UserInputModeBordersWidget.h"
#include "UserInputModeView.h"

using namespace caret;
    
/**
 * \class UserInputModeBorders 
 * \brief Processing user input for Borders mode.
 *
 * Processes user input in Border mode which includes
 * drawing and editing borders.
 */

/**
 * Constructor.
 *
 * @param borderBeingDrawnByOpenGL
 *    Border that is displayed in OpenGL area when a border is being drawn
 * @param windowIndex
 *    Index of the browser window using this border processing.
 */
UserInputModeBorders::UserInputModeBorders(Border* borderBeingDrawnByOpenGL,
                                           const int32_t windowIndex)
: CaretObject(),
UserInputReceiverInterface()
{
    this->borderBeingDrawnByOpenGL = borderBeingDrawnByOpenGL;
    this->windowIndex = windowIndex;
    this->mode = MODE_CREATE;
    this->createOperation = CREATE_OPERATION_DRAW;
    this->borderToolsWidget = new UserInputModeBordersWidget(this);
}

/**
 * Destructor.
 */
UserInputModeBorders::~UserInputModeBorders()
{
    
}

/**
 * @return The input mode enumerated type.
 */
UserInputModeBorders::UserInputMode 
UserInputModeBorders::getUserInputMode() const
{
    return UserInputReceiverInterface::BORDERS;
}

/**
 * Draw a border point at the mouse coordinates.
 */
void 
UserInputModeBorders::drawPointAtMouseXY(BrainOpenGLWidget* openGLWidget,
                                         const int32_t mouseX,
                                         const int32_t mouseY)
{
    SurfaceProjectedItem projectedItem;
    openGLWidget->performProjection(mouseX,
                                    mouseY,
                                    projectedItem);
    
    if (projectedItem.isOriginalXYZValid()) {
        float xyz[3];
        projectedItem.getOriginalXYZ(xyz);
        
        AString txt = ("Projected Position: " 
                       + AString::fromNumbers(xyz, 3, ","));
        
        if (projectedItem.getProjectionType() == SurfaceProjectionTypeEnum::BARYCENTRIC) {
            SurfaceProjectionBarycentric* bp = projectedItem.getBarycentricProjection();
            
            txt += ("\nBarycentric Position: " 
                    + AString::fromNumbers(bp->getTriangleAreas(), 3, ",")
                    + "   "
                    + AString::fromNumbers(bp->getTriangleNodes(), 3, ","));
            
            SurfaceProjectedItem* spi = new SurfaceProjectedItem();
            spi->setProjectionType(SurfaceProjectionTypeEnum::BARYCENTRIC);
            spi->setStructure(projectedItem.getStructure());
            SurfaceProjectionBarycentric* spb = spi->getBarycentricProjection();
            spb->setTriangleAreas(bp->getTriangleAreas());
            spb->setTriangleNodes(bp->getTriangleNodes());
            spb->setSignedDistanceAboveSurface(0.0);
            openGLWidget->getBorderBeingDrawn()->addPoint(spi);
        }
        else {
            
        }
        
        CaretLogFiner(txt);
    }
}

/**
 * Called when a mouse events occurs for 'this' 
 * user input receiver.
 *
 * @param mouseEvent
 *     The mouse event.
 * @param browserTabContent
 *     Content of the browser window's tab.
 * @param openGLWidget
 *     OpenGL Widget in which mouse event occurred.
 */
void 
UserInputModeBorders::processMouseEvent(MouseEvent* mouseEvent,
                       BrowserTabContent* browserTabContent,
                       BrainOpenGLWidget* openGLWidget)
{
    ModelDisplayController* modelController = browserTabContent->getModelControllerForDisplay();
    if (modelController != NULL) {
        //const int32_t tabIndex = browserTabContent->getTabNumber();
        //const float dx = mouseEvent->getDx();
        //const float dy = mouseEvent->getDy();
        const int mouseX = mouseEvent->getX();
        const int mouseY = mouseEvent->getY();
        
        switch (this->mode) {
            case MODE_CREATE:
            {
                switch (mouseEvent->getMouseEventType()) {
                    case MouseEventTypeEnum::INVALID:
                        break;
                    case MouseEventTypeEnum::LEFT_CLICKED:
                        this->drawPointAtMouseXY(openGLWidget,
                                                 mouseX,
                                                 mouseY);        
                        mouseEvent->setGraphicsUpdateOneWindowRequested();
                        break;
                    case MouseEventTypeEnum::LEFT_DRAGGED:
                        switch (this->createOperation) {
                            case CREATE_OPERATION_DRAW:
                                this->drawPointAtMouseXY(openGLWidget,
                                                         mouseX,
                                                         mouseY);
                                mouseEvent->setGraphicsUpdateOneWindowRequested();
                                break;
                            case CREATE_OPERATION_TRANSFORM:
                                UserInputModeView::processModelViewTransformation(mouseEvent, 
                                                                                  browserTabContent, 
                                                                                  openGLWidget);
                                break;
                        }
                        break;
                    case MouseEventTypeEnum::LEFT_PRESSED:
                        break;
                    case MouseEventTypeEnum::LEFT_RELEASED:
                        break;
                    case MouseEventTypeEnum::WHEEL_MOVED:
                        break;
                }
            }
                break;
            case MODE_EDIT:
            {
                
            }
                break;
            case MODE_EDIT_POINTS:
            {
                
            }
                break;
            case MODE_UPDATE:
            {
                
            }
                break;
        }
        
    }
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void 
UserInputModeBorders::initialize()
{
    this->borderToolsWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeBorders::finish()
{
    
}

/**
 * @return A widget for display at the bottom of the
 * Browser Window Toolbar when this mode is active.
 * If no user-interface controls are needed, return NULL.
 * The toolbar will take ownership of the widget and
 * delete it so derived class MUST NOT delete the widget.
 */
QWidget* 
UserInputModeBorders::getWidgetForToolBar()
{
    return this->borderToolsWidget;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
UserInputModeBorders::toString() const
{
    return "UserInputModeBorders";
}

/**
 * @return the mode.
 */
UserInputModeBorders::Mode 
UserInputModeBorders::getMode() const
{
    return this->mode;
}

/**
 * Set the mode.
 * @param mode
 *    New value for mode.
 */
void 
UserInputModeBorders::setMode(const Mode mode)
{
    this->mode = mode;
    this->borderToolsWidget->updateWidget();
}

/**
 * @return the create operation.
 */
UserInputModeBorders::CreateOperation 
UserInputModeBorders::getCreateOperation() const
{
    return this->createOperation;
}

/**
 * Set the create operation.
 * @param createOperation
 *    New value for create operation.
 */
void 
UserInputModeBorders::setCreateOperation(const CreateOperation createOperation)
{
    this->createOperation = createOperation;
    this->borderToolsWidget->updateWidget();
}

void 
UserInputModeBorders::createOperationFinish(const AString& name)
{
    this->borderBeingDrawnByOpenGL->setName(name);
    // copy border and add to border file.
    this->borderBeingDrawnByOpenGL->clear();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * Undo (remove last point) from border being drawn.
 */
void 
UserInputModeBorders::createOperationUndo()
{
    this->borderBeingDrawnByOpenGL->removeLastPoint();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * Reset the border being drawn.
 */
void 
UserInputModeBorders::createOperationReset()
{
    this->borderBeingDrawnByOpenGL->clear();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}



