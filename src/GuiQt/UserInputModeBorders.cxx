
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

#include <memory>

#define __USER_INPUT_MODE_BORDERS_DECLARE__
#include "UserInputModeBorders.h"
#undef __USER_INPUT_MODE_BORDERS_DECLARE__

#include "Border.h"
#include "BorderFile.h"
#include "BorderPropertiesEditorDialog.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventToolBoxSelectionDisplay.h"
#include "EventUserInterfaceUpdate.h"
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
 * \class caret::UserInputModeBorders 
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
    this->mode = MODE_DRAW;
    this->drawOperation = DRAW_OPERATION_CREATE;
    this->editOperation = EDIT_OPERATION_PROPERTIES;
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
        const bool isLeftDragWithAltKeyDown = (isLeftDrag
                                               && mouseEvent->isAltKeyDown());
        const bool isLeftClickWithShiftKeyDown = (isLeftClick
                                                  && mouseEvent->isShiftKeyDown());
        
        switch (this->mode) {
            case MODE_DRAW:
            {
                /*
                 * Shift click is always FINISH
                 */
                if (isLeftClickWithShiftKeyDown) {
                    this->borderToolsWidget->executeFinishOperation();
                }
                else if (this->borderToolsWidget->isDrawModeTransformSelected()) {                
                    if (isLeftDrag || isWheel) {
                        UserInputModeView::processModelViewTransformation(mouseEvent, 
                                                                          browserTabContent, 
                                                                          openGLWidget);
                    }
                }
                else if (isLeftDragWithAltKeyDown) {
                    /*
                     * In drawing mode, but perform surface rotation
                     */
                    mouseEvent->setAltKeyDown(false);
                    UserInputModeView::processModelViewTransformation(mouseEvent, 
                                                                      browserTabContent, 
                                                                      openGLWidget);
                }
                else {
                    switch (this->drawOperation) {
                        case DRAW_OPERATION_CREATE:
                        case DRAW_OPERATION_ERASE:
                        case DRAW_OPERATION_EXTEND:
                        case DRAW_OPERATION_REPLACE:
                            if (isLeftClickOrDrag) {
                                this->drawPointAtMouseXY(openGLWidget,
                                                         mouseX,
                                                         mouseY);
                                mouseEvent->setGraphicsUpdateOneWindowRequested();
                            }
                            break;
                    }
                }
            }
                break;
            case MODE_EDIT:
            {
                switch (this->editOperation) {
                    case EDIT_OPERATION_DELETE:  
                        if (isLeftClick) {
                            IdentificationManager* idManager =
                            openGLWidget->performIdentification(mouseEvent->getX(), mouseEvent->getY());
                            IdentificationItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
                            if (idBorder->isValid()) {
                                BorderFile* borderFile = idBorder->getBorderFile();
                                Border* border = idBorder->getBorder();
                                borderFile->removeBorder(border);
                                mouseEvent->setGraphicsUpdateAllWindowsRequested();
                                mouseEvent->setUserInterfaceUpdateRequested();
                            }
                        }
                        break;
                    case EDIT_OPERATION_PROPERTIES:
                        if (isLeftClick) {
                            IdentificationManager* idManager =
                            openGLWidget->performIdentification(mouseEvent->getX(), mouseEvent->getY());
                            IdentificationItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
                            if (idBorder->isValid()) {
                                BorderFile* borderFile = idBorder->getBorderFile();
                                Border* border = idBorder->getBorder();
                                mouseEvent->setGraphicsUpdateAllWindowsRequested();
                                std::auto_ptr<BorderPropertiesEditorDialog> editBorderDialog(
                                            BorderPropertiesEditorDialog::newInstanceEditBorder(borderFile,
                                                                                                border,
                                                                                                openGLWidget));
                                if (editBorderDialog->exec() == BorderPropertiesEditorDialog::Accepted) {
                                    mouseEvent->setGraphicsUpdateAllWindowsRequested();
                                    mouseEvent->setUserInterfaceUpdateRequested();
                                }
                            }
                        }
                        break;
                }
            }
                break;
            case MODE_SELECT:
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
    this->showHideBorderSelectionToolBox();
}

void 
UserInputModeBorders::showHideBorderSelectionToolBox()
{
    switch (this->mode) {
        case MODE_DRAW:
        case MODE_EDIT:
            EventManager::get()->sendEvent(EventToolBoxSelectionDisplay(this->windowIndex,
                                                                        EventToolBoxSelectionDisplay::DISPLAY_MODE_HIDE).getPointer());
            break;
        case MODE_SELECT:
            EventManager::get()->sendEvent(EventToolBoxSelectionDisplay(this->windowIndex,
                                                                        EventToolBoxSelectionDisplay::DISPLAY_MODE_DISPLAY_BORDERS).getPointer());
            break;
    }
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeBorders::finish()
{
    EventManager::get()->sendEvent(EventToolBoxSelectionDisplay(this->windowIndex,
                                                                EventToolBoxSelectionDisplay::DISPLAY_MODE_HIDE).getPointer());
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
    this->showHideBorderSelectionToolBox();
}

/**
 * @return the create operation.
 */
UserInputModeBorders::DrawOperation 
UserInputModeBorders::getDrawOperation() const
{
    return this->drawOperation;
}

/**
 * Set the create operation.
 * @param createOperation
 *    New value for create operation.
 */
void 
UserInputModeBorders::setDrawOperation(const DrawOperation drawOperation)
{
    this->drawOperation = drawOperation;
    this->borderToolsWidget->updateWidget();
}

/**
 * Finish the border that the user was drawing.
 */
void 
UserInputModeBorders::drawOperationFinish()
{
    this->borderBeingDrawnByOpenGL->clear();

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * Undo (remove last point) from border being drawn.
 */
void 
UserInputModeBorders::drawOperationUndo()
{
    this->borderBeingDrawnByOpenGL->removeLastPoint();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * Reset the border being drawn.
 */
void 
UserInputModeBorders::drawOperationReset()
{
    this->borderBeingDrawnByOpenGL->clear();
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
}

/**
 * @return The edit operation.
 */
UserInputModeBorders::EditOperation 
UserInputModeBorders::getEditOperation() const
{
    return this->editOperation;
}

/**
 * Set the edit operation.
 * @param editOperation
 *   New edit operation.
 */
void 
UserInputModeBorders::setEditOperation(const EditOperation editOperation)
{
    this->editOperation = editOperation;
}

