
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

#include "Annotation.h"
#include "AnnotationFile.h"
#include "Brain.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CursorEnum.h"
#include "EventAnnotation.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MouseEvent.h"
#include "SelectionItemAnnotation.h"
#include "SelectionManager.h"
#include "UserInputModeAnnotationsWidget.h"

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
m_browserWindowIndex(windowIndex)
{
    m_mode = MODE_NEW;
    
    m_annotationToolsWidget = new UserInputModeAnnotationsWidget(this,
                                                                 m_browserWindowIndex);
    setWidgetForToolBar(m_annotationToolsWidget);
}

/**
 * Destructor.
 */
UserInputModeAnnotations::~UserInputModeAnnotations()
{
}

/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeAnnotations::initialize()
{
    //this->borderToolsWidget->updateWidget();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void
UserInputModeAnnotations::finish()
{
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
//        this->borderBeingDrawnByOpenGL->clear();
//        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_windowIndex).getPointer());
    }
//    m_annotationToolsWidget->updateWidget();
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeAnnotations::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
    switch (m_mode) {
        case MODE_DELETE:
            cursor = CursorEnum::CURSOR_POINTING_HAND;
            break;
        case MODE_EDIT:
            cursor = CursorEnum::CURSOR_POINTING_HAND;
            break;
        case MODE_NEW:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
    }
    
    return cursor;
}

/**
 * Process a mouse left drag with no keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDrag(const MouseEvent& /*mouseEvent*/)
{
}

/**
 * Process a mouse left drag with only the alt key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithAlt(const MouseEvent& /*mouseEvent*/) { }

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
    processMouseLeftClick(mouseEvent,
                          false);
    
    switch (m_mode) {
        case MODE_DELETE:
        {
            //            SelectionManager* idManager =
            //            openGLWidget->performIdentification(mouseX,
            //                                                mouseY,
            //                                                true);
            //            SelectionItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
            //            if (idBorder->isValid()) {
            //                Brain* brain = idBorder->getBrain();
            //                Surface* surface = idBorder->getSurface();
            //                //BorderFile* borderFile = idBorder->getBorderFile();
            //                Border* border = idBorder->getBorder();
            //                this->borderToolsWidget->executeRoiInsideSelectedBorderOperation(brain,
            //                                                                                 surface,
            //                                                                                 border);
            //            }
        }
            break;
        case MODE_EDIT:
        {
//            SelectionManager* idManager =
//            openGLWidget->performIdentification(mouseX,
//                                                mouseY,
//                                                true);
//            SelectionItemBorderSurface* idBorder = idManager->getSurfaceBorderIdentification();
//            if (idBorder->isValid()) {
//                BorderFile* borderFile = idBorder->getBorderFile();
//                if (borderFile->isSingleStructure()) {
//                    switch (this->editOperation) {
//                        case EDIT_OPERATION_DELETE:
//                        {
//                            Border* border = idBorder->getBorder();
//                            borderFile->removeBorder(border);
//                            this->updateAfterBordersChanged();
//                        }
//                            break;
//                        case EDIT_OPERATION_PROPERTIES:
//                        {
//                            Border* border = idBorder->getBorder();
//                            std::auto_ptr<BorderPropertiesEditorDialog> editBorderDialog(
//                                                                                         BorderPropertiesEditorDialog::newInstanceEditBorder(borderFile,
//                                                                                                                                             
//                                                                                                                                             border,
//                                                                                                                                             
//                                                                                                                                             openGLWidget));
//                            if (editBorderDialog->exec() == BorderPropertiesEditorDialog::Accepted) {
//                                this->updateAfterBordersChanged();
//                            }
//                        }
//                            break;
//                    }
//                }
//                else {
//                    WuQMessageBox::errorOk(this->borderToolsWidget,
//                                           borderFile->getObsoleteMultiStructureFormatMessage());
//                }
//            }
        }
            break;
        case MODE_NEW:
            //            this->drawPointAtMouseXY(openGLWidget,
            //                                     mouseX,
            //                                     mouseY);
            //            EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->windowIndex).getPointer());
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
    processMouseLeftClick(mouseEvent,
                          true);
    
    switch (m_mode) {
        case MODE_DELETE:
            break;
        case MODE_EDIT:
            break;
        case MODE_NEW:
            break;
    }
}

/**
 * Select (or deselect) the given annotation.
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param shiftKeyDownFlag
 *     Status of shift key.
 */
void
UserInputModeAnnotations::processMouseLeftClick(const MouseEvent& mouseEvent,
                                                const bool shiftKeyDownFlag)
{
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    //std::cout << "Mouse click: " << mouseX << ", " << mouseY << std::endl;
    
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
    SelectionManager* idManager = openGLWidget->performIdentification(mouseX, mouseY, true);
    SelectionItemAnnotation* annotationID = idManager->getAnnotationIdentification();
    if (annotationID->isValid()) {
        //std::cout << "Selected Annotation: " << qPrintable(annotationID->toString()) << std::endl;
        
        Annotation* selectedAnnotation = annotationID->getAnnotation();
        CaretAssert(selectedAnnotation);
        
        /*
         * If the annotation selected by the user was already selected
         * then the user is deselecting it.
         */
        if (selectedAnnotation->isSelected()) {
            selectedAnnotation->setSelected(false);
            selectedAnnotation = NULL;
        }
        
        /*
         * If the SHIFT key is down, the user is selecting multiple
         * annotations
         */
        if (shiftKeyDownFlag) {
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

    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    
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
    std::vector<AnnotationFile*> allAnnotationFiles;
    GuiManager::get()->getBrain()->getAllAnnotationFiles(allAnnotationFiles);
    for (std::vector<AnnotationFile*>::iterator fileIter = allAnnotationFiles.begin();
         fileIter != allAnnotationFiles.end();
         fileIter++) {
        AnnotationFile* annotationFile = *fileIter;
        annotationFile->setAllAnnotationsSelected(false);
    }
    
    EventManager::get()->sendEvent(EventAnnotation().setModeDeselectAllAnnotations().getPointer());
}


