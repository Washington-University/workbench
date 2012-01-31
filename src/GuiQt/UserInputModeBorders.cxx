
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
#include "BorderFile.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "IdentificationItemBorderSurface.h"
#include "IdentificationManager.h"
#include "ModelDisplayController.h"
#include "ModelDisplayControllerSurface.h"
#include "MouseEvent.h"
#include "Surface.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "UserInputModeBordersWidget.h"
#include "UserInputModeView.h"
#include "WuQMessageBox.h"

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
    this->reviseOperation = REVISE_OPERATION_ERASE;
    this->selectOperation = SELECT_CLASS;
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
    
    SurfaceProjectedItem* spi = new SurfaceProjectedItem();
    
    AString txt;
    if (projectedItem.isStereotaxicXYZValid()) {
        spi->setStereotaxicXYZ(projectedItem.getStereotaxicXYZ());
        
        txt += ("Projected Position: " 
                + AString::fromNumbers(projectedItem.getStereotaxicXYZ(), 3, ","));
    }
    
    if (projectedItem.getBarycentricProjection()->isValid()) {
        SurfaceProjectionBarycentric* bp = projectedItem.getBarycentricProjection();
        
        txt += ("\nBarycentric Position: " 
                + AString::fromNumbers(bp->getTriangleAreas(), 3, ",")
                + "   "
                + AString::fromNumbers(bp->getTriangleNodes(), 3, ","));
        
        SurfaceProjectionBarycentric* spb = spi->getBarycentricProjection();
        spb->setProjectionSurfaceNumberOfNodes(bp->getProjectionSurfaceNumberOfNodes());
        spb->setTriangleAreas(bp->getTriangleAreas());
        spb->setTriangleNodes(bp->getTriangleNodes());
        spb->setSignedDistanceAboveSurface(0.0);
        spb->setValid(true);
    }

    if (spi->isStereotaxicXYZValid()
        || spi->getBarycentricProjection()->isValid()) {
        spi->setStructure(projectedItem.getStructure());
        this->borderBeingDrawnByOpenGL->addPoint(spi);
    }
    else {
        delete spi;
    }
    
    CaretLogFiner(txt);
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
        
        const bool isLeftClick = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::LEFT_CLICKED);
        const bool isLeftDrag  = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::LEFT_DRAGGED);
        const bool isWheel     = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::WHEEL_MOVED);
        const bool isLeftClickOrDrag = (isLeftClick || isLeftDrag);
        
        switch (this->mode) {
            case MODE_CREATE:
            {
                switch (this->createOperation) {
                    case CREATE_OPERATION_DRAW:
                        if (isLeftClickOrDrag) {
                            this->drawPointAtMouseXY(openGLWidget,
                                                     mouseX,
                                                     mouseY);
                            mouseEvent->setGraphicsUpdateOneWindowRequested();
                        }
                        break;
                    case CREATE_OPERATION_TRANSFORM:
                        if (isLeftClickOrDrag) {
                            UserInputModeView::processModelViewTransformation(mouseEvent, 
                                                                          browserTabContent, 
                                                                          openGLWidget);
                        }
                        break;
                }
            }
                break;
            case MODE_REVISE:
            {
                switch (this->reviseOperation) {
                    case REVISE_OPERATION_ERASE:
                    case REVISE_OPERATION_EXTEND:
                    case REVISE_OPERATION_REPLACE:
                        if (isLeftClickOrDrag) {
                            this->drawPointAtMouseXY(openGLWidget,
                                                     mouseX,
                                                     mouseY);
                            mouseEvent->setGraphicsUpdateOneWindowRequested();
                        }
                        break;
                    case REVISE_OPERATION_DELETE:  
                        if (isLeftClick) {
                            IdentificationManager* idManager =
                            openGLWidget->performIdentification(mouseEvent->getX(), mouseEvent->getY());
                            IdentificationItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
                            if (idBorder->isValid()) {
                                BorderFile* borderFile = idBorder->getBorderFile();
                                Border* border = idBorder->getBorder();
                                borderFile->removeBorder(border);
                                mouseEvent->setGraphicsUpdateAllWindowsRequested();
                            }
                        }
                        break;
                    case REVISE_OPERATION_REVERSE:
                        if (isLeftClick) {
                            IdentificationManager* idManager =
                            openGLWidget->performIdentification(mouseEvent->getX(), mouseEvent->getY());
                            IdentificationItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
                            if (idBorder->isValid()) {
                                Border* border = idBorder->getBorder();
                                border->reverse();
                                mouseEvent->setGraphicsUpdateAllWindowsRequested();
                            }
                        }
                        break;
                }
            }
                break;
            case MODE_SELECT:
            {
                switch (this->selectOperation) {
                    case SELECT_CLASS:
                        break;
                    case SELECT_NAME:
                        break;
                }
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
    if (this->mode != mode) {
        this->borderBeingDrawnByOpenGL->clear();
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
    }
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

/**
 * Finish the border that the user was drawing.
 */
void 
UserInputModeBorders::createOperationFinish()
{
    this->borderBeingDrawnByOpenGL->clear();

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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

/**
 * @return The revise operation.
 */
UserInputModeBorders::ReviseOperation 
UserInputModeBorders::getReviseOperation() const
{
    return this->reviseOperation;
}

/**
 * Accept the border reivision that the user was drawing.
 */
void 
UserInputModeBorders::reviseOperationAccept()
{
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->windowIndex);
    if (browserWindow == NULL) {
        return;
    }
    BrowserTabContent* btc = browserWindow->getBrowserTabContent();
    if (btc == NULL) {
        return;
    }
    ModelDisplayControllerSurface* surfaceController = btc->getDisplayedSurfaceModel();
    if (surfaceController == NULL) {
        return;
    }
    
    Surface* surface = surfaceController->getSurface();
    Brain* brain = surfaceController->getBrain();
    
    float nearestTolerance = 15;
    BorderFile* borderFile;
    int32_t borderFileIndex; 
    Border* border;
    int32_t borderIndex;
    SurfaceProjectedItem* borderPoint;
    int32_t borderPointIndex;
    float distanceToNearestBorder;
    if (brain->findBorderNearestBorder(surface, 
                                       this->borderBeingDrawnByOpenGL,
                                       Brain::NEAREST_BORDER_TEST_MODE_ENDPOINTS, 
                                       nearestTolerance,
                                       borderFile,
                                       borderFileIndex,
                                       border, 
                                       borderIndex,
                                       borderPoint,
                                       borderPointIndex,
                                       distanceToNearestBorder)) {
        try {
            switch (this->reviseOperation) {
                case REVISE_OPERATION_ERASE:
                    border->reviseEraseFromEnd(surface,
                                               this->borderBeingDrawnByOpenGL);
                    break;
                case REVISE_OPERATION_EXTEND:
                    break;
                case REVISE_OPERATION_REPLACE:
                    break;
                case REVISE_OPERATION_DELETE: 
                    break;
                case REVISE_OPERATION_REVERSE:
                    break;
            }
        }
        catch (BorderException& e) {
            WuQMessageBox::errorOk(this->borderToolsWidget,
                                   e.whatString());
        }
    }
    this->borderBeingDrawnByOpenGL->clear();
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Undo (remove last point) from border being revised.
 */
void 
UserInputModeBorders::reviseOperationUndo()
{
    this->borderBeingDrawnByOpenGL->removeLastPoint();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * Reset the border revision.
 */
void 
UserInputModeBorders::reviseOperationReset()
{
    this->borderBeingDrawnByOpenGL->clear();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * Set the revise operation.
 * @param reviseOperation
 *   New revise operation.
 */
void 
UserInputModeBorders::setReviseOperation(const ReviseOperation reviseOperation)
{
    this->reviseOperation = reviseOperation;
}

/**
 * @return The select operation.
 */
UserInputModeBorders::SelectOperation 
UserInputModeBorders::getSelectOperation() const
{
    return this->selectOperation;
}

/**
 * Set the select operation.
 * @param selectOperation
 *   New select operation.
 */
void 
UserInputModeBorders::setSelectOperation(const SelectOperation selectOperation)
{
    this->selectOperation = selectOperation;
}



