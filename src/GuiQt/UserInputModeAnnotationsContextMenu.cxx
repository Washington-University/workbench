
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

#define __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_DECLARE__
#include "UserInputModeAnnotationsContextMenu.h"
#undef __USER_INPUT_MODE_ANNOTATIONS_CONTEXT_MENU_DECLARE__

#include <cmath>

#include <QLineEdit>

#include "AnnotationCoordinate.h"
#include "AnnotationCreateDialog.h"
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "AnnotationTextEditorDialog.h"
#include "Brain.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "SelectionItemAnnotation.h"
#include "SelectionManager.h"
#include "WuQDataEntryDialog.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeAnnotationsContextMenu 
 * \brief Context (pop-up) menu for User Input Annotations Mode.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param mouseEvent
 *    The mouse event that caused display of this menu.
 * @param selectionManager
 *    The selection manager, provides data under the cursor.
 * @param browserTabContent
 *    Content of browser tab.
 * @param parentOpenGLWidget
 *    Parent OpenGL Widget on which the menu is displayed.
 */
UserInputModeAnnotationsContextMenu::UserInputModeAnnotationsContextMenu(const MouseEvent& mouseEvent,
                                                                         SelectionManager* selectionManager,
                                                                         BrowserTabContent* browserTabContent,
                                                                         BrainOpenGLWidget* parentOpenGLWidget)
: QMenu(parentOpenGLWidget),
m_mouseEvent(mouseEvent),
m_selectionManager(selectionManager),
m_browserTabContent(browserTabContent),
m_parentOpenGLWidget(parentOpenGLWidget),
m_newAnnotationCreatedByContextMenu(NULL)
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();

    const SelectionItemAnnotation* idAnnotation = selectionManager->getAnnotationIdentification();
    m_annotationFile = idAnnotation->getAnnotationFile();
    m_annotation = idAnnotation->getAnnotation();

    m_textAnnotation = NULL;
    if (m_annotation != NULL) {
        m_textAnnotation = dynamic_cast<AnnotationText*>(m_annotation);
    }
    
    if (m_annotation != NULL) {
        addAction("Copy",
                  this, SLOT(copyAnnotationToAnnotationClipboard()));
    }
    
    if (m_annotation != NULL) {
        addAction("Delete",
                  this, SLOT(deleteAnnotation()));
    }
    
    if (m_textAnnotation != NULL) {
        addAction("Edit Text...",
                  this, SLOT(setAnnotationText()));
    }
    
    if (annotationManager->isAnnotationOnClipboardValid()) {
        addAction("Paste",
                  this, SLOT(pasteAnnotationFromAnnotationClipboard()));
    }
}

/**
 * Destructor.
 */
UserInputModeAnnotationsContextMenu::~UserInputModeAnnotationsContextMenu()
{
}

Annotation*
UserInputModeAnnotationsContextMenu::getNewAnnotationCreatedByContextMenu()
{
    return m_newAnnotationCreatedByContextMenu;
}

/**
 * Copy the annotation to the annotation clipboard.
 */
void
UserInputModeAnnotationsContextMenu::copyAnnotationToAnnotationClipboard()
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->copyAnnotationToClipboard(m_annotationFile,
                                                 m_annotation);
}

/**
 * Delete the annotation.
 */
void
UserInputModeAnnotationsContextMenu::deleteAnnotation()
{
    CaretAssert(m_annotation);
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    std::vector<Annotation*> selectedAnnotations;
    selectedAnnotations.push_back(m_annotation);
    if ( ! selectedAnnotations.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeDeleteAnnotations(selectedAnnotations);
        annotationManager->applyCommand(undoCommand);
        
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Paste a one-dimensional shape (line) keeping its start to end
 * coordinate orientation.  The start coordinate is pasted at
 * the coordinate in 'coordInfo'.
 *
 * @param oneDimShape
 *     One dimensional shape that will be pasted.
 * @param coordInfo
 *     Coordinate information that will be used for the shape's 'start' coordinate.
 * @return
 *     True if the shape's coordinate was updated for pasting, else false.
 */
bool
pasteOneDimensionalShape(AnnotationOneDimensionalShape* oneDimShape,
                         UserInputModeAnnotations::CoordinateInformation& coordInfo)
{

    bool tabFlag = false;
    bool windowFlag = false;
    
    switch (oneDimShape->getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            tabFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            windowFlag = true;
            break;
    }

    bool validCoordsFlag = false;
    
    if (tabFlag
        || windowFlag) {
        float startXYZ[3];
        float endXYZ[3];
        oneDimShape->getStartCoordinate()->getXYZ(startXYZ);
        oneDimShape->getEndCoordinate()->getXYZ(endXYZ);
        const float diffXYZ[3] = {
            endXYZ[0] - startXYZ[0],
            endXYZ[1] - startXYZ[1],
            endXYZ[2] - startXYZ[2]
        };
        
        if (tabFlag
            && (coordInfo.m_tabIndex >= 0)) {
            startXYZ[0] = coordInfo.m_tabXYZ[0];
            startXYZ[1] = coordInfo.m_tabXYZ[1];
            startXYZ[2] = coordInfo.m_tabXYZ[2];
            oneDimShape->setTabIndex(coordInfo.m_tabIndex);
            validCoordsFlag = true;
        }
        else if (windowFlag
                 && (coordInfo.m_windowIndex >= 0)) {
            startXYZ[0] = coordInfo.m_windowXYZ[0];
            startXYZ[1] = coordInfo.m_windowXYZ[1];
            startXYZ[2] = coordInfo.m_windowXYZ[2];
            oneDimShape->setWindowIndex(coordInfo.m_windowIndex);
            validCoordsFlag = true;
        }
        
        if (validCoordsFlag) {
            endXYZ[0] = startXYZ[0] + diffXYZ[0];
            endXYZ[1] = startXYZ[1] + diffXYZ[1];
            endXYZ[2] = startXYZ[2] + diffXYZ[2];
            
            /*
             * Tab/Window coordinates are percentage ranging [0.0, 100.0]
             * Need to "clip" lines if they exceed the viewport's edges
             */
            const float minCoord = 1.0;
            const float maxCoord = 99.0;
            
            if (endXYZ[0] < minCoord) {
                if (diffXYZ[0] != 0.0) {
                    const float xDist = minCoord - startXYZ[0];
                    const float scaledDistance = std::fabs(xDist / diffXYZ[0]);
                    endXYZ[0] = minCoord;
                    endXYZ[1] = startXYZ[1] + (scaledDistance * diffXYZ[1]);
                }
            }
            else if (endXYZ[0] >= maxCoord) {
                const float xDist = maxCoord - startXYZ[0];
                const float scaledDistance = std::fabs(xDist / diffXYZ[0]);
                endXYZ[0] = maxCoord;
                endXYZ[1] = startXYZ[1] + (scaledDistance * diffXYZ[1]);
            }
            
            if (endXYZ[1] < minCoord) {
                const float yDist = minCoord - startXYZ[1];
                const float scaledDistance = std::fabs(yDist / diffXYZ[1]);
                endXYZ[1] = minCoord;
                endXYZ[0] = startXYZ[0] + (scaledDistance * diffXYZ[0]);
            }
            else if (endXYZ[1] > maxCoord) {
                const float yDist = maxCoord - startXYZ[1];
                const float scaledDistance = std::fabs(yDist / diffXYZ[1]);
                endXYZ[1] = maxCoord;
                endXYZ[0] = startXYZ[0] + (scaledDistance * diffXYZ[0]);
            }
            
            oneDimShape->getStartCoordinate()->setXYZ(startXYZ);
            oneDimShape->getEndCoordinate()->setXYZ(endXYZ);
        }
        
    }
    
    return validCoordsFlag;
}

/**
 * Paste the annotation from the annotation clipboard.
 */
void
UserInputModeAnnotationsContextMenu::pasteAnnotationFromAnnotationClipboard()
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    if (annotationManager->isAnnotationOnClipboardValid()) {
        AnnotationFile* annotationFile = annotationManager->getAnnotationFileOnClipboard();
        Annotation* annotation = annotationManager->getAnnotationOnClipboard()->clone();
        
        BrainOpenGLViewportContent* viewportContent = m_mouseEvent.getViewportContent();
        UserInputModeAnnotations::CoordinateInformation coordInfo;
        UserInputModeAnnotations::getValidCoordinateSpacesFromXY(m_parentOpenGLWidget,
                                                                 viewportContent,
                                                                 m_mouseEvent.getX(),
                                                                 m_mouseEvent.getY(),
                                                                 coordInfo);
        
        bool validCoordsFlag = false;
        
        AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
        if (oneDimShape != NULL) {
            /*
             * Pasting line while preserving its orientation only
             * works for tab and window spaces.
             */
            validCoordsFlag = pasteOneDimensionalShape(oneDimShape,
                                                       coordInfo);
        }
        
        if (! validCoordsFlag) {
            validCoordsFlag = UserInputModeAnnotations::setAnnotationCoordinatesForSpace(annotation,
                                                                                         annotation->getCoordinateSpace(),
                                                                                         &coordInfo,
                                                                                         NULL);
        }
        
        if (validCoordsFlag) {
            annotationFile->addAnnotation(annotation);
            m_newAnnotationCreatedByContextMenu = annotation;
            
            annotationManager->selectAnnotation(AnnotationManager::SELECTION_MODE_SINGLE,
                                                false,
                                                annotation);
        }
        else {
            /*
             * Pasting annotation in its coordinate failed (user may have tried to paste
             * an annotation in surface space where there is no surface).
             */
            delete annotation;

            CaretPointer<AnnotationCreateDialog> annotationDialog(AnnotationCreateDialog::newPasteAnnotation(m_mouseEvent,
                                                                                                             annotationFile,
                                                                                                             annotationManager->getAnnotationOnClipboard(),
                                                                                                             m_parentOpenGLWidget));
            if (annotationDialog->exec() == AnnotationCreateDialog::Accepted) {
                m_newAnnotationCreatedByContextMenu = annotationDialog->getAnnotationThatWasCreated();
            }
        }

        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

/**
 * Set the text for an annotation.
 */
void
UserInputModeAnnotationsContextMenu::setAnnotationText()
{
    AnnotationTextEditorDialog ted(m_textAnnotation,
                                   this);
    /*
     * Note: Y==0 is at top for widget.
     *       Y==0 is at bottom for OpenGL mouse x,y
     */
    QPoint diaglogPos(this->pos().x(),
                      this->pos().y() + 20);
    ted.move(diaglogPos);
    ted.exec();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

