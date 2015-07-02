
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

#include <QLineEdit>

#include "AnnotationCoordinate.h"
#include "AnnotationCreateDialog.h"
#include "AnnotationManager.h"
#include "AnnotationOneDimensionalShape.h"
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
m_parentOpenGLWidget(parentOpenGLWidget)
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
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->deleteAnnotation(m_annotation);
    m_annotation = NULL;

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
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
        const Annotation* annotation = annotationManager->getAnnotationOnClipboard();
        
        
        CaretPointer<AnnotationCreateDialog> annotationDialog(AnnotationCreateDialog::newPasteAnnotation(m_mouseEvent,
                                                                                                         annotationFile,
                                                                                                         annotation,
                                                                                                         m_parentOpenGLWidget));
        if (annotationDialog->exec() == AnnotationCreateDialog::Accepted) {
            
        }

        
//        AnnotationCreateDialog pasteDialog(m_mouseEvent,
//                                           annotationFile,
//                                           annotation,
//                                           m_parentOpenGLWidget);
//        pasteDialog.exec();

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
}

