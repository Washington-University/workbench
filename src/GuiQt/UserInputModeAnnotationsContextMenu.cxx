
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
#include "UserInputModeAnnotations.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"

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
UserInputModeAnnotationsContextMenu::UserInputModeAnnotationsContextMenu(UserInputModeAnnotations* userInputModeAnnotations,
                                                                         const MouseEvent& mouseEvent,
                                                                         SelectionManager* selectionManager,
                                                                         BrowserTabContent* browserTabContent,
                                                                         BrainOpenGLWidget* parentOpenGLWidget)
: QMenu(parentOpenGLWidget),
m_userInputModeAnnotations(userInputModeAnnotations),
m_mouseEvent(mouseEvent),
m_selectionManager(selectionManager),
m_browserTabContent(browserTabContent),
m_parentOpenGLWidget(parentOpenGLWidget),
m_newAnnotationCreatedByContextMenu(NULL)
{
    CaretAssert(m_userInputModeAnnotations);
    
    const int32_t browserWindexIndex = m_mouseEvent.getBrowserWindowIndex();
    std::vector<std::pair<Annotation*, AnnotationFile*> > selectedAnnotations;
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->getAnnotationsSelectedForEditing(browserWindexIndex,
                                              selectedAnnotations);
    
    m_annotationFile = NULL;
    m_annotation     = NULL;
    
    m_stereotaxicAndSurfaceAnnotations.clear();
    for (std::vector<std::pair<Annotation*, AnnotationFile*> >::iterator iter = selectedAnnotations.begin();
         iter != selectedAnnotations.end();
         iter++) {
        Annotation* ann = iter->first;
        CaretAssert(ann);
        
        bool stereoOrSurfaceSpaceFlag = false;
        switch (ann->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::PIXELS:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                stereoOrSurfaceSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                stereoOrSurfaceSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        if (stereoOrSurfaceSpaceFlag) {
            m_stereotaxicAndSurfaceAnnotations.push_back(ann);
        }
    }

    bool oneAnnotationSelectedFlag = false;
    if (selectedAnnotations.size() == 1) {
        CaretAssertVectorIndex(selectedAnnotations, 0);
        m_annotationFile = selectedAnnotations[0].second;
        m_annotation     = selectedAnnotations[0].first;
        oneAnnotationSelectedFlag = true;
    }
    
    m_textAnnotation = NULL;
    if (m_annotation != NULL) {
        m_textAnnotation = dynamic_cast<AnnotationText*>(m_annotation);
    }
    
    std::vector<BrainBrowserWindowEditMenuItemEnum::Enum> editMenuItemsEnabled;
    AString redoMenuItemSuffix;
    AString undoMenuItemSuffix;
    AString pasteText;
    AString pasteSpecialText;
    userInputModeAnnotations->getEnabledEditMenuItems(editMenuItemsEnabled,
                                                      redoMenuItemSuffix,
                                                      undoMenuItemSuffix,
                                                      pasteText,
                                                      pasteSpecialText);
    
    /*
     * Cut
     */
    QAction* cutAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::CUT),
                                   this, SLOT(cutAnnnotation()));
    cutAction->setEnabled(oneAnnotationSelectedFlag);
    
    /*
     * Copy
     */
    QAction* copyAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::COPY),
                                    this, SLOT(copyAnnotationToAnnotationClipboard()));
    copyAction->setEnabled(oneAnnotationSelectedFlag);

    /*
     * Delete
     */
    QAction* deleteAction = addAction(BrainBrowserWindowEditMenuItemEnum::toGuiName(BrainBrowserWindowEditMenuItemEnum::DELETER),
                                      this, SLOT(deleteAnnotations()));
    deleteAction->setEnabled( ! selectedAnnotations.empty());
    
    /*
     * Paste
     */
    QAction* pasteAction = addAction(pasteText,
                                     this, SLOT(pasteAnnotationFromAnnotationClipboard()));
    pasteAction->setEnabled(annotationManager->isAnnotationOnClipboardValid());

    /*
     * Paste Special
     */
    QAction* pasteSpecialAction = addAction(pasteSpecialText,
                                           this, SLOT(pasteSpecialAnnotationFromAnnotationClipboard()));
    pasteSpecialAction->setEnabled(annotationManager->isAnnotationOnClipboardValid());

    /*
     * Separator
     */
    addSeparator();
    
    /*
     * Edit Text
     */
    QAction* editTextAction = addAction("Edit Text...",
                                        this, SLOT(setAnnotationText()));
    editTextAction->setEnabled(m_textAnnotation != NULL);
    
    
    /*
     * Separator
     */
    addSeparator();
    
    /*
     * Turn off display in other tabs
     */
    QAction* turnOffDisplayAction = addAction("Turn Off Display in Other Tabs",
                                              this, SLOT(turnOffDisplayInOtherTabs()));
    QAction* turnOnDisplayAction = addAction("Turn On Display in All Tabs",
                                              this, SLOT(turnOnDisplayInAllTabs()));
    turnOffDisplayAction->setDisabled(m_stereotaxicAndSurfaceAnnotations.empty());
    turnOnDisplayAction->setDisabled(m_stereotaxicAndSurfaceAnnotations.empty());
    
    /*
     * Separator
     */
    addSeparator();
    
    /*
     * Group annotations
     */
    QAction* groupAction = addAction(AnnotationGroupingModeEnum::toGuiName(AnnotationGroupingModeEnum::GROUP),
                                     this, SLOT(applyGroupingGroup()));
    groupAction->setEnabled(annotationManager->isGroupingModeValid(browserWindexIndex,
                                                                   AnnotationGroupingModeEnum::GROUP));
    
    /*
     * Ungroup annotations
     */
    QAction* ungroupAction = addAction(AnnotationGroupingModeEnum::toGuiName(AnnotationGroupingModeEnum::UNGROUP),
                                     this, SLOT(applyGroupingUngroup()));
    ungroupAction->setEnabled(annotationManager->isGroupingModeValid(browserWindexIndex,
                                                                     AnnotationGroupingModeEnum::UNGROUP));
    
    /*
     * Regroup annotations
     */
    QAction* regroupAction = addAction(AnnotationGroupingModeEnum::toGuiName(AnnotationGroupingModeEnum::REGROUP),
                                       this, SLOT(applyGroupingRegroup()));
    regroupAction->setEnabled(annotationManager->isGroupingModeValid(browserWindexIndex,
                                                                     AnnotationGroupingModeEnum::REGROUP));
    
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
    CaretAssert(m_annotationFile);
    CaretAssert(m_annotation);
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->copyAnnotationToClipboard(m_annotationFile,
                                                 m_annotation);
}

/**
 * Cut the selected annotation.
 */
void
UserInputModeAnnotationsContextMenu::cutAnnnotation()
{
    m_userInputModeAnnotations->cutAnnotation();
}

/**
 * Delete the annotation.
 */
void
UserInputModeAnnotationsContextMenu::deleteAnnotations()
{
    /*
     * Delete the annotation that is under the mouse
     */
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(m_mouseEvent.getBrowserWindowIndex());
    if ( ! selectedAnnotations.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeDeleteAnnotations(selectedAnnotations);
        AString errorMessage;
        if ( ! annotationManager->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Paste the annotation from the annotation clipboard.
 */
void
UserInputModeAnnotationsContextMenu::pasteAnnotationFromAnnotationClipboard()
{
    m_userInputModeAnnotations->pasteAnnotationFromAnnotationClipboard(m_mouseEvent);
}

/**
 * Paste special the annotation from the annotation clipboard.
 */
void
UserInputModeAnnotationsContextMenu::pasteSpecialAnnotationFromAnnotationClipboard()
{
    m_userInputModeAnnotations->pasteAnnotationFromAnnotationClipboardAndChangeSpace(m_mouseEvent);
}

/**
 * Set the text for an annotation.
 */
void
UserInputModeAnnotationsContextMenu::setAnnotationText()
{
    CaretAssert(m_textAnnotation);
    
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

/**
 * Turn off display of annotation in other tabs.
 */
void
UserInputModeAnnotationsContextMenu::turnOffDisplayInOtherTabs()
{
    for (std::vector<Annotation*>::iterator iter = m_stereotaxicAndSurfaceAnnotations.begin();
         iter != m_stereotaxicAndSurfaceAnnotations.end();
         iter++) {
        (*iter)->setItemDisplaySelectedInOneTab(m_browserTabContent->getTabNumber());
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Turn on display of annotation in all tabs.
 */
void
UserInputModeAnnotationsContextMenu::turnOnDisplayInAllTabs()
{
    for (std::vector<Annotation*>::iterator iter = m_stereotaxicAndSurfaceAnnotations.begin();
         iter != m_stereotaxicAndSurfaceAnnotations.end();
         iter++) {
        (*iter)->setItemDisplaySelectedInAllTabs();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Group annotations.
 */
void
UserInputModeAnnotationsContextMenu::applyGroupingGroup()
{
    applyGrouping(AnnotationGroupingModeEnum::GROUP);
}

/**
 * Ungroup annotations.
 */
void
UserInputModeAnnotationsContextMenu::applyGroupingRegroup()
{
    applyGrouping(AnnotationGroupingModeEnum::REGROUP);
}

/**
 * Regroup annotations.
 */
void
UserInputModeAnnotationsContextMenu::applyGroupingUngroup()
{
    applyGrouping(AnnotationGroupingModeEnum::UNGROUP);
}

/**
 * Apply grouping selection.
 *
 * @param grouping
 *     Selected grouping.
 */
void
UserInputModeAnnotationsContextMenu::applyGrouping(const AnnotationGroupingModeEnum::Enum grouping)
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    
    AString errorMessage;
    if ( ! annMan->applyGroupingMode(m_mouseEvent.getBrowserWindowIndex(),
                                     grouping,
                                     errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


