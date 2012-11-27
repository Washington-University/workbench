
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __USER_INPUT_MODE_FOCI_DECLARE__
#include "UserInputModeFoci.h"
#undef __USER_INPUT_MODE_FOCI_DECLARE__

#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FociFile.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "MouseEvent.h"
#include "Surface.h"
#include "UserInputModeFociWidget.h"
#include "UserInputModeView.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeFoci 
 * \brief Processes user input for foci.
 */

/**
 * Constructor.
 */
UserInputModeFoci::UserInputModeFoci(const int32_t windowIndex)
: CaretObject(),
  m_windowIndex(windowIndex)
{
    m_inputModeFociWidget = new UserInputModeFociWidget(this,
                                                        windowIndex);
    m_mode = MODE_CREATE;
    m_editOperation = EDIT_OPERATION_PROPERTIES;
}

/**
 * Destructor.
 */
UserInputModeFoci::~UserInputModeFoci()
{
    
}

/**
 * @return The input mode enumerated type.
 */
UserInputModeFoci::UserInputMode
UserInputModeFoci::getUserInputMode() const
{
    return UserInputReceiverInterface::FOCI;
}

/**
 * @return the mode.
 */
UserInputModeFoci::Mode
UserInputModeFoci::getMode() const
{
    return m_mode;
}

/**
 * Set the mode.
 * @param mode
 *    New value for mode.
 */
void
UserInputModeFoci::setMode(const Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_windowIndex).getPointer());
    }
    this->m_inputModeFociWidget->updateWidget();
}

/**
 * @return The edit operation.
 */
UserInputModeFoci::EditOperation
UserInputModeFoci::getEditOperation() const
{
    return m_editOperation;
}

/**
 * Set the edit operation.
 * @param editOperation
 *   New edit operation.
 */
void
UserInputModeFoci::setEditOperation(const EditOperation editOperation)
{
    m_editOperation = editOperation;
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
UserInputModeFoci::processMouseEvent(MouseEvent* mouseEvent,
                                        BrainOpenGLViewportContent* viewportContent,
                                        BrainOpenGLWidget* openGLWidget)
{
    BrowserTabContent* browserTabContent = viewportContent->getBrowserTabContent();
    Model* modelController = browserTabContent->getModelControllerForDisplay();
    if (modelController != NULL) {
        //const int32_t tabIndex = browserTabContent->getTabNumber();
        //const float dx = mouseEvent->getDx();
        //const float dy = mouseEvent->getDy();
        const int mouseX = mouseEvent->getX();
        const int mouseY = mouseEvent->getY();
        
        if (mouseEvent->getMouseEventType() == MouseEventTypeEnum::LEFT_PRESSED) {
            m_mousePressX = mouseX;
            m_mousePressY = mouseY;
        }
        const bool isLeftClick = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::LEFT_CLICKED);
        const bool isLeftDrag  = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::LEFT_DRAGGED);
        const bool isWheel     = (mouseEvent->getMouseEventType() == MouseEventTypeEnum::WHEEL_MOVED);
        const bool isLeftClickOrDrag = (isLeftClick || isLeftDrag);
        const bool isLeftDragWithControlAndShiftKeyDown = (isLeftDrag
                                                           && mouseEvent->isControlAndShiftKeyDown());
        const bool isWheelWithControlAndShiftKeyDown = (isWheel
                                                        && mouseEvent->isControlAndShiftKeyDown());
        const bool isLeftClickWithShiftKeyDown = (isLeftClick
                                                  && mouseEvent->isShiftKeyDown());
        bool doTransformation = false;
        bool doIdentification = false;
        
        SelectionManager* idManager =
            openGLWidget->performIdentification(mouseEvent->getX(),
                                                mouseEvent->getY(),
                                                true);
        if (idManager->getSurfaceNodeIdentification()->isValid()
            && idManager->getVoxelIdentification()->isValid()) {
            //std::cout << "GET BOTH VERTEX AND VOXEL" << std::endl;
        }
        
        switch (m_mode){
            case MODE_CREATE:
                if (isLeftClick
                    && m_inputModeFociWidget->isMouseClickCreateFocusEnabled()) {
                    SelectionItemSurfaceNode* idNode = idManager->getSurfaceNodeIdentification();
                    SelectionItemVoxel* idVoxel = idManager->getVoxelIdentification();
                    if (idNode->isValid()) {
                        if (idNode->isContralateral() == false) {
                            const Surface* surface = idNode->getSurface();
                            CaretAssert(surface);
                            const StructureEnum::Enum structure = surface->getStructure();
                            const int32_t nodeIndex = idNode->getNodeNumber();
                            
                            const AString focusName = (StructureEnum::toGuiName(structure)
                                                       + " Vertex "
                                                       + AString::number(nodeIndex));
                            const float* xyz = surface->getCoordinate(nodeIndex);
                            
                            const AString comment = ("Created from "
                                                     + focusName);

                            Focus* focus = new Focus();
                            focus->setName(focusName);
                            focus->getProjection(0)->setStereotaxicXYZ(xyz);
                            focus->setComment(comment);
                            FociPropertiesEditorDialog::createFocus(focus,
                                                                    browserTabContent,
                                                                    m_inputModeFociWidget);
                        }
                    }
                    else if (idVoxel->isValid()) {
                        const VolumeFile* vf = idVoxel->getVolumeFile();
                        int64_t ijk[3];
                        idVoxel->getVoxelIJK(ijk);
                        float xyz[3];
                        vf->indexToSpace(ijk, xyz);

                        const AString focusName = (vf->getFileNameNoPath()
                                                   + " IJK ("
                                                   + AString::fromNumbers(ijk, 3, ",")
                                                   + ")");
                        
                        const AString comment = ("Created from "
                                                 + focusName);
                        
                        Focus* focus = new Focus();
                        focus->setName(focusName);
                        focus->getProjection(0)->setStereotaxicXYZ(xyz);
                        focus->setComment(comment);
                        FociPropertiesEditorDialog::createFocus(focus,
                                                                browserTabContent,
                                                                m_inputModeFociWidget);
                    }
                }
                else if (isLeftClick) {
                    doIdentification = true;
                }
                else if ((isLeftDrag || isWheel)) {
                    doTransformation = true;
                }
                break;
            case MODE_EDIT:
            {
                if (isLeftClick) {
                    FociFile* fociFile = NULL;
                    Focus*    focus = NULL;
                    
                    SelectionItemFocusVolume* idVolFocus = idManager->getVolumeFocusIdentification();
                    if (idVolFocus->isValid()) {
                        fociFile = idVolFocus->getFociFile();
                        CaretAssert(fociFile);
                        focus    = idVolFocus->getFocus();
                        CaretAssert(focus);
                    }
                    SelectionItemFocusSurface* idFocus = idManager->getSurfaceFocusIdentification();
                    if (idFocus->isValid()) {
                        fociFile = idFocus->getFociFile();
                        CaretAssert(fociFile);
                        focus    = idFocus->getFocus();
                        CaretAssert(focus);
                        
                    }
                    
                    if ((fociFile != NULL)
                        && (focus != NULL)) {
                        switch (m_editOperation) {
                            case EDIT_OPERATION_DELETE:
                                fociFile->removeFocus(focus);
                                updateAfterFociChanged();
                                break;
                            case EDIT_OPERATION_PROPERTIES:
                            {
                                FociPropertiesEditorDialog::editFocus(fociFile,
                                                                      focus,
                                                                      openGLWidget);
                            }
                        }
                    }
                }
                else if ((isLeftDrag || isWheel)) {
                    doTransformation = true;
                }
                
            }
                break;
            case MODE_OPERATIONS:
                if (isLeftClick) {
                    doIdentification = true;
                }
                else if ((isLeftDrag || isWheel)) {
                    doTransformation = true;
                }
                break;
        }
        
        if (doIdentification) {
            UserInputModeView::processModelViewIdentification(viewportContent,
                                                              openGLWidget,
                                                              mouseEvent->getX(),
                                                              mouseEvent->getY());
        }
        
        if (doTransformation) {
            UserInputModeView::processModelViewTransformation(mouseEvent,
                                                              viewportContent,
                                                              openGLWidget,
                                                              m_mousePressX,
                                                              m_mousePressY);
        }
    }
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeFoci::initialize()
{
    m_inputModeFociWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void 
UserInputModeFoci::finish()
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
UserInputModeFoci::getWidgetForToolBar()
{
    return this->m_inputModeFociWidget;
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeFoci::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
//    switch (this->mode) {
//        case MODE_DRAW:
//            if (this->borderToolsWidget->isDrawModeTransformSelected() == false) {
//                cursor = CursorEnum::CURSOR_DRAWING_PEN;
//            }
//            break;
//        case MODE_EDIT:
//            cursor = CursorEnum::CURSOR_POINTING_HAND;
//            break;
//        case MODE_ROI:
//            cursor = CursorEnum::CURSOR_POINTING_HAND;
//            break;
//    }
    
    return cursor;
}

void
UserInputModeFoci::updateAfterFociChanged()
{
    /*
     * Need to update all graphics windows and all border controllers.
     */
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().addFoci().getPointer());
}



