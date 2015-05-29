
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __USER_INPUT_MODE_ANNOTATIONS_DECLARE__
#include "UserInputModeAnnotations.h"
#undef __USER_INPUT_MODE_ANNOTATIONS_DECLARE__

#include "AnnotationChangeCoordinateDialog.h"
#include "AnnotationCreateDialog.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationTwoDimensionalShape.h"
#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CursorEnum.h"
#include "EventAnnotation.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "SelectionItemAnnotation.h"
#include "SelectionManager.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "Surface.h"
#include "UserInputModeAnnotationsWidget.h"
#include "WuQMessageBox.h"

using namespace caret;


/**
 * \class caret::UserInputModeAnnotations 
 * \brief Input mode processor for Annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
UserInputModeAnnotations::UserInputModeAnnotations(const int32_t windowIndex)
: UserInputModeView(UserInputModeAbstract::ANNOTATIONS),
m_browserWindowIndex(windowIndex),
m_annotationBeingEdited(NULL),
m_annotationUnderMouse(NULL)
{
    m_mode = MODE_SELECT;
    m_modeNewAnnotationType = AnnotationTypeEnum::ARROW;
    
    m_annotationToolsWidget = new UserInputModeAnnotationsWidget(this,
                                                                 m_browserWindowIndex);
    setWidgetForToolBar(m_annotationToolsWidget);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION);
}

/**
 * Destructor.
 */
UserInputModeAnnotations::~UserInputModeAnnotations()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
UserInputModeAnnotations::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION) {
        EventAnnotation* annotationEvent = dynamic_cast<EventAnnotation*>(event);
        CaretAssert(annotationEvent);
        
        switch (annotationEvent->getMode()) {
            case EventAnnotation::MODE_INVALID:
                break;
            case EventAnnotation::MODE_CREATE_NEW_ANNOTATION_TYPE:
            {
                m_modeNewAnnotationType = annotationEvent->getModeCreateNewAnnotationType();
                setMode(MODE_NEW);
                EventManager::get()->sendEvent(EventAnnotation().setModeDeselectAllAnnotations().getPointer());
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            }
                break;
            case EventAnnotation::MODE_DELETE_ANNOTATION:
                break;
            case EventAnnotation::MODE_DESELECT_ALL_ANNOTATIONS:
                m_annotationBeingEdited = NULL;
                m_annotationUnderMouse  = NULL;
                break;
            case EventAnnotation::MODE_EDIT_ANNOTATION:
            {
                int32_t windowIndex = -1;
                Annotation* annotation = NULL;
                annotationEvent->getModeEditAnnotation(windowIndex,
                                                       annotation);
                if (windowIndex == m_browserWindowIndex) {
                    m_annotationBeingEdited = annotation;
                }
            }
                break;
            case EventAnnotation::MODE_GET_ALL_ANNOTATIONS:
                break;
        }
    }
}


/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeAnnotations::initialize()
{
    //this->borderToolsWidget->updateWidget();
    m_mode = MODE_SELECT;
    m_annotationBeingEdited = NULL;
    m_annotationUnderMouse  = NULL;
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void
UserInputModeAnnotations::finish()
{
    m_mode = MODE_SELECT;
    m_annotationBeingEdited = NULL;
    m_annotationUnderMouse  = NULL;
}

/**
 * Called to update the input receiver for various events.
 */
void
UserInputModeAnnotations::update()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
UserInputModeAnnotations::toString() const
{
    return "UserInputModeBorders";
}

/**
 * @return the mode.
 */
UserInputModeAnnotations::Mode
UserInputModeAnnotations::getMode() const
{
    return m_mode;
}

/**
 * Set the mode.
 * @param mode
 *    New value for mode.
 */
void
UserInputModeAnnotations::setMode(const Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
    }
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeAnnotations::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
    switch (m_mode) {
        case MODE_NEW:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
        case MODE_SELECT:
            cursor = CursorEnum::CURSOR_POINTING_HAND;
            if (m_annotationUnderMouse != NULL) {
                cursor = CursorEnum::CURSOR_FOUR_ARROWS;
                //std::cout << "Requested Four Arrow Cursor" << std::endl;
            }
            break;
        case MODE_SET_COORDINATE_ONE:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
        case MODE_SET_COORDINATE_TWO:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
    }
    
    return cursor;
}

/**
 * Process a key press event
 *
 * @param keyEvent
 *     Key event information.
 */
void
UserInputModeAnnotations::keyPressEvent(const KeyEvent& keyEvent)
{
    const int32_t keyCode = keyEvent.getKeyCode();
    switch (keyCode) {
        case Qt::Key_Delete:
        {
            switch (m_mode) {
                case MODE_NEW:
                    break;
                case MODE_SELECT:
                    if (m_annotationBeingEdited != NULL) {
                        if (WuQMessageBox::warningOkCancel(m_annotationToolsWidget,
                                                           "Delete selected annotation?")) {
                            /*
                             * Delete the annotation, deselect the annotation, and update graphics.
                             */
                            EventManager::get()->sendEvent(EventAnnotation().setModeDeleteAnnotation(m_annotationBeingEdited).getPointer());
                            EventManager::get()->sendEvent(EventAnnotation().setModeDeselectAllAnnotations().getPointer());
                            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
                        }
                    }
                    break;
                case MODE_SET_COORDINATE_ONE:
                    break;
                case MODE_SET_COORDINATE_TWO:
                    break;
            }
        }
            break;
        case Qt::Key_Escape:
        {
            bool selectModeFlag = false;
            switch (m_mode) {
                case MODE_NEW:
                    break;
                case MODE_SELECT:
                    break;
                case MODE_SET_COORDINATE_ONE:
                    selectModeFlag = true;
                    break;
                case MODE_SET_COORDINATE_TWO:
                    selectModeFlag = true;
                    break;
            }
            if (selectModeFlag) {
                setMode(MODE_SELECT);
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            }
        }
            break;
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        {
            if (m_annotationBeingEdited != NULL) {
                bool changeCoordFlag = false;
                switch (m_annotationBeingEdited->getCoordinateSpace()) {
                    case AnnotationCoordinateSpaceEnum::MODEL:
                        changeCoordFlag = true;
                        break;
                    case AnnotationCoordinateSpaceEnum::PIXELS:
                        break;
                    case AnnotationCoordinateSpaceEnum::SURFACE:
                        break;
                    case AnnotationCoordinateSpaceEnum::TAB:
                        changeCoordFlag = true;
                        break;
                    case AnnotationCoordinateSpaceEnum::WINDOW:
                        changeCoordFlag = true;
                        break;
                }
                
                if (changeCoordFlag) {
                    const float distance = 0.01;
                    float dx = 0.0;
                    float dy = 0.0;
                    switch (keyCode) {
                        case Qt::Key_Down:
                            dy = -distance;
                            break;
                        case Qt::Key_Left:
                            dx = -distance;
                            break;
                        case Qt::Key_Right:
                            dx = distance;
                            break;
                        case Qt::Key_Up:
                            dy = distance;
                            break;
                        default:
                            CaretAssert(0);
                            break;
                    }
                    
                    AnnotationOneDimensionalShape* oneDim = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotationBeingEdited);
                    AnnotationTwoDimensionalShape* twoDim = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotationBeingEdited);
                    
                    if (oneDim != NULL) {
                        float xyz[3];
                        oneDim->getStartCoordinate()->getXYZ(xyz);
                        xyz[0] += dx;
                        xyz[1] += dy;
                        oneDim->getStartCoordinate()->setXYZ(xyz);
                        
                        oneDim->getEndCoordinate()->getXYZ(xyz);
                        xyz[0] += dx;
                        xyz[1] += dy;
                        oneDim->getEndCoordinate()->setXYZ(xyz);
                    }
                    
                    if (twoDim != NULL) {
                        float xyz[3];
                        twoDim->getCoordinate()->getXYZ(xyz);
                        xyz[0] += dx;
                        xyz[1] += dy;
                        twoDim->getCoordinate()->setXYZ(xyz);
                    }

                    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                }
            }
        }
            break;
    }
}

/**
 * Process a mouse left drag with no keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDrag(const MouseEvent& mouseEvent)
{
    if (m_annotationBeingEdited != NULL) {
        if (m_annotationBeingEdited == m_annotationUnderMouse) {
            if (m_annotationBeingEdited->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::WINDOW) {
                AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotationBeingEdited);
                if (twoDimShape != NULL) {
                    const QSize windowSize = mouseEvent.getOpenGLWidget()->size();
                    float xyz[3];
                    twoDimShape->getCoordinate()->getXYZ(xyz);
                    xyz[0] = static_cast<float>(mouseEvent.getX()) / static_cast<float>(windowSize.width());
                    xyz[1] = static_cast<float>(mouseEvent.getY()) / static_cast<float>(windowSize.height());
                    twoDimShape->getCoordinate()->setXYZ(xyz);
                    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                }
            }
        }
    }
}

/**
 * Process a mouse left drag with only the alt key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithAlt(const MouseEvent& /*mouseEvent*/)
{
}

/**
 * Process a mouse left drag with ctrl key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithCtrl(const MouseEvent& /*mouseEvent*/) { }

/**
 * Process a mouse left drag with ctrl and shift keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithCtrlShift(const MouseEvent& /*mouseEvent*/) { }

/**
 * Process a mouse left drag with shift key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithShift(const MouseEvent& /*mouseEvent*/) { }

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftClick(const MouseEvent& mouseEvent)
{
    switch (m_mode) {
        case MODE_NEW:
            processModeNewMouseLeftClick(mouseEvent);
            break;
        case MODE_SELECT:
            break;
        case MODE_SET_COORDINATE_ONE:
            processModeSetCoordinate(mouseEvent);
            break;
        case MODE_SET_COORDINATE_TWO:
            processModeSetCoordinate(mouseEvent);
            break;
    }
}

/**
 * Process a mouse left click with Shift key event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftClickWithShift(const MouseEvent& mouseEvent)
{
    const bool allowSelectionOfMultipleAnnotationsFlag = false;
    if ( ! allowSelectionOfMultipleAnnotationsFlag) {
        return;
    }
    
    switch (m_mode) {
        case MODE_NEW:
            break;
        case MODE_SELECT:
            processMouseSelectAnnotation(mouseEvent,
                                            true);
            break;
        case MODE_SET_COORDINATE_ONE:
            break;
        case MODE_SET_COORDINATE_TWO:
            break;
    }
}

/**
 * Process a mouse left press event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftPress(const MouseEvent& mouseEvent)
{
    switch (m_mode) {
        case MODE_NEW:
            break;
        case MODE_SELECT:
            processMouseSelectAnnotation(mouseEvent,
                                         false);
            break;
        case MODE_SET_COORDINATE_ONE:
            break;
        case MODE_SET_COORDINATE_TWO:
            break;
    }
}

/**
 * Process a mouse move with no buttons down
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseMove(const MouseEvent& mouseEvent)
{
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    Annotation* annotation = openGLWidget->performIdentificationAnnotations(mouseX,
                                                                            mouseY);
    
    const bool updateCursorFlag = (m_annotationUnderMouse != annotation);
    
    m_annotationUnderMouse = annotation;
    
//    if (annotation != NULL) {
//        static int64_t ctr = 0;
//        std::cout << "Over an annotation type " << AnnotationTypeEnum::toGuiName(annotation->getType()) << " " << ctr++ << std::endl;
//    }
    
    if (updateCursorFlag) {
        openGLWidget->updateCursor();
    }
}

/**
 * Get the different types of coordinates at the given mouse location.
 *
 * @param mouseEvent
 *     Information about mouse event including mouse location.
 * @param coordInfoOut
 *     Output containing coordinate information.
 */
void
UserInputModeAnnotations::getCoordinatesFromMouseLocation(const MouseEvent& mouseEvent,
                                                          CoordinateInformation& coordInfoOut)
{
    coordInfoOut.reset();
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    SelectionManager* idManager =
    openGLWidget->performIdentification(mouseX,
                                        mouseY,
                                        true);
    
    SelectionItemVoxel* voxelID = idManager->getVoxelIdentification();
    SelectionItemSurfaceNode*  surfaceNodeIdentification = idManager->getSurfaceNodeIdentification();
    if (surfaceNodeIdentification->isValid()) {
        surfaceNodeIdentification->getModelXYZ(coordInfoOut.m_modelXYZ);
        coordInfoOut.m_modelXYZValid = true;
        
        const Surface* surface = surfaceNodeIdentification->getSurface();
        CaretAssert(surface);
        coordInfoOut.m_surfaceNumberOfNodes = surface->getNumberOfNodes();
        coordInfoOut.m_surfaceStructure     = surface->getStructure();
        coordInfoOut.m_surfaceNodeIndex     = surfaceNodeIdentification->getNodeNumber();
        coordInfoOut.m_surfaceNodeOffset    = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
        coordInfoOut.m_surfaceNodeValid     = true;
    }
    else if (voxelID->isValid()) {
        voxelID->getModelXYZ(coordInfoOut.m_modelXYZ);
        coordInfoOut.m_modelXYZValid = true;
    }
    
    BrainOpenGLViewportContent* vpContent = mouseEvent.getViewportContent();
    
    /*
     * In tile tabs, some regions may not contain a tab such
     * as three tabs in a two-by-two configuration
     * or if the user has clicked in a margin
     */
    BrowserTabContent* tabContent = vpContent->getBrowserTabContent();
    if (tabContent != NULL) {
        int tabViewport[4];
        vpContent->getModelViewport(tabViewport);
        const float tabX = (mouseEvent.getX() - tabViewport[0]) / static_cast<float>(tabViewport[2]);
        const float tabY = (mouseEvent.getY() - tabViewport[1]) / static_cast<float>(tabViewport[3]);
        if ((tabX >= 0.0)
            && (tabX < 1.0)
            && (tabY >= 0.0)
            && (tabY <= 1.0)) {
            coordInfoOut.m_tabXYZ[0] = tabX;
            coordInfoOut.m_tabXYZ[1] = tabY;
            coordInfoOut.m_tabXYZ[2] = 0.0;
            coordInfoOut.m_tabIndex  = tabContent->getTabNumber();
        }

//        /*
//         * Normalize tab coordinates (width and height range [0, 1]
//         */
//        coordInfoOut.m_tabXYZ[0] /= tabViewport[2];
//        coordInfoOut.m_tabXYZ[1] /= tabViewport[3];
    }
    
    int windowViewport[4];
    vpContent->getWindowViewport(windowViewport);
    coordInfoOut.m_windowXYZ[0] = windowViewport[0] + mouseEvent.getX(); // tabViewport[0] + tabViewport[2] + mouseEvent.getX();
    coordInfoOut.m_windowXYZ[1] = windowViewport[1] + mouseEvent.getY(); // tabViewport[1] + tabViewport[3] + mouseEvent.getY();
    coordInfoOut.m_windowXYZ[2] = 0.0;
    coordInfoOut.m_windowIndex = vpContent->getWindowIndex();
    
    /*
     * Normalize window coordinates (width and height range [0, 1]
     */
    coordInfoOut.m_windowXYZ[0] /= windowViewport[2];
    coordInfoOut.m_windowXYZ[1] /= windowViewport[3];
}

/**
 * Process a mouse left click to set a coordinate.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::processModeSetCoordinate(const MouseEvent& mouseEvent)
{
    if (m_annotationBeingEdited == NULL) {
        return;
    }

    CoordinateInformation coordInfo;
    UserInputModeAnnotations::getCoordinatesFromMouseLocation(mouseEvent,
                                                              coordInfo);
    
    AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotationBeingEdited);
    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotationBeingEdited);

    AnnotationCoordinate* coordinate = NULL;
    AnnotationCoordinate* otherCoordinate = NULL;
    switch (m_mode) {
        case MODE_NEW:
            break;
        case MODE_SELECT:
            break;
        case MODE_SET_COORDINATE_ONE:
            if (oneDimAnn != NULL) {
                coordinate      = oneDimAnn->getStartCoordinate();
                otherCoordinate = oneDimAnn->getEndCoordinate();
            }
            else if (twoDimAnn != NULL) {
                coordinate = twoDimAnn->getCoordinate();
            }
            break;
        case MODE_SET_COORDINATE_TWO:
            if (oneDimAnn != NULL) {
                coordinate      = oneDimAnn->getEndCoordinate();
                otherCoordinate = oneDimAnn->getStartCoordinate();
            }
            break;
    }
    
    if (coordinate != NULL) {
        StructureEnum::Enum structure = StructureEnum::INVALID;
        int32_t numNodes = -1;
        int32_t nodeIndex = -1;
        float surfaceOffset = 0.0;
        coordinate->getSurfaceSpace(structure, numNodes, nodeIndex, surfaceOffset);
//        coordInfo.m_surfaceStructure = structure;
//        coordInfo.m_surfaceNodeIndex  = nodeIndex;
//        coordInfo.m_surfaceNumberOfNodes = numNodes;
        coordInfo.m_surfaceNodeOffset = surfaceOffset;
    }
    
    AnnotationChangeCoordinateDialog changeCoordDialog(coordInfo,
                                                       m_annotationBeingEdited,
                                                       coordinate,
                                                       otherCoordinate,
                                                       m_annotationToolsWidget);
    if (changeCoordDialog.exec() == AnnotationChangeCoordinateDialog::Accepted) {
        
    }

    setMode(MODE_SELECT);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}


/**
 * Process a mouse left click for new mode.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::processModeNewMouseLeftClick(const MouseEvent& mouseEvent)
{
    AnnotationCreateDialog createAnnotationDialog(mouseEvent,
                                                  m_modeNewAnnotationType,
                                                  mouseEvent.getOpenGLWidget());
    if (createAnnotationDialog.exec() == AnnotationCreateDialog::Accepted) {
        
    }

    setMode(MODE_SELECT);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}


/**
 * Process a mouse left click for selection mode.
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param shiftKeyDownFlag
 *     Status of shift key.
 */
void
UserInputModeAnnotations::processMouseSelectAnnotation(const MouseEvent& mouseEvent,
                                                const bool shiftKeyDownFlag)
{
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    Annotation* annotationToEdit = NULL;
    
    /*
     * NOTE: When selecting annotations:
     *    (A) When the mouse is clicked WITHOUT the SHIFT key down, the user is in
     *        'single-annotation-selection-mode' and at most one annotation will
     *        be selected when this method completes.
     *    (B) If the mouse is clicked with the SHIFT key down, the user is in
     *        'multi-annotation-selection-mode' and any number of annotation will
     *        be selected when this method completes.
     */
    Annotation* selectedAnnotation = openGLWidget->performIdentificationAnnotations(mouseX,
                                                                                    mouseY);
    if (selectedAnnotation != NULL) {
        CaretAssert(selectedAnnotation);
        
//        /*
//         * If the annotation selected by the user was already selected
//         * then the user is deselecting it.
//         */
//        if (selectedAnnotation->isSelected()) {
//            selectedAnnotation->setSelected(false);
//            selectedAnnotation = NULL;
//        }
        
        /*
         * If the SHIFT key is down, the user is selecting multiple
         * annotations
         */
        if (shiftKeyDownFlag) {
            /*
             * If the annotation selected by the user was already selected
             * then the user is deselecting it.
             */

            if (selectedAnnotation->isSelected()) {
                selectedAnnotation->setSelected(false);
                selectedAnnotation = NULL;
            }
            /*
             * Do not alter the selection status of any other
             * annotations.
             */
        }
        else {
            /*
             * User is in single selection mode so deselect
             * all annotations.
             */
            deselectAllAnnotations();
            if (selectedAnnotation != NULL) {
                selectedAnnotation->setSelected(true);
                m_annotationUnderMouse = selectedAnnotation;
            }
        }
        
        /*
         * If the annotation selected by the user was 
         * not previously selected, now select it.
         */
        if (selectedAnnotation != NULL) {
            selectedAnnotation->setSelected(true);
            
            if ( ! shiftKeyDownFlag) {
                annotationToEdit = selectedAnnotation;
            }
        }
    }
    else {
        /*
         * User did not click over an annotation.
         *
         * If SHIFT key IS NOT DOWN, the user is in single
         * selection mode so deselect all annotations.
         */
        if ( !shiftKeyDownFlag) {
            deselectAllAnnotations();
        }
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    if (annotationToEdit != NULL) {
        EventAnnotation event;
        event.setModeEditAnnotation(m_browserWindowIndex,
                                    annotationToEdit);
        EventManager::get()->sendEvent(event.getPointer());
    }
}

/**
 * Deselect all annotations.
 */
void
UserInputModeAnnotations::deselectAllAnnotations()
{
    EventManager::get()->sendEvent(EventAnnotation().setModeDeselectAllAnnotations().getPointer());
}


