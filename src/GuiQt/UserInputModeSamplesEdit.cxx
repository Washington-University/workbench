
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __USER_INPUT_MODE_SAMPLES_EDIT_DECLARE__
#include "UserInputModeSamplesEdit.h"
#undef __USER_INPUT_MODE_SAMPLES_EDIT_DECLARE__

#include "AnnotationBrowserTab.h"
#include "AnnotationManager.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrowserWindowContent.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabCloseInToolBar.h"
#include "EventBrowserTabSelectInWindow.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventAnnotationPolyhedronGetByLinkedIdentifier.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;
    
/**
 * \class caret::UserInputModeSamplesEdit
 * \brief User Input mode for editing samples
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserIndexIndex
 *     Index of window
 */
UserInputModeSamplesEdit::UserInputModeSamplesEdit(const int32_t browserIndexIndex)
: UserInputModeAnnotations(UserInputModeEnum::Enum::SAMPLES_EDITING,
                           browserIndexIndex)
{
    
}

/**
 * Destructor.
 */
UserInputModeSamplesEdit::~UserInputModeSamplesEdit()
{
}

/**
 * Show a context menu (pop-up menu at mouse location)
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param menuPosition
 *     Point at which menu is displayed (passed to QMenu::exec())
 * @param openGLWidget
 *     OpenGL widget in which context menu is requested
 */
void
UserInputModeSamplesEdit::showContextMenu(const MouseEvent& mouseEvent,
                                          const QPoint& menuPosition,
                                          BrainOpenGLWidget* openGLWidget)
{
    UserInputModeAnnotations::showContextMenu(mouseEvent,
                                              menuPosition,
                                              openGLWidget);
}

/**
 * Process a mouse left click for selection mode.
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param shiftKeyDownFlag
 *     True if shift key is down.
 * @param singleSelectionModeFlag
 *     If true, deselect any other annotations so that only the annotation under mouse is selected
 */
void
UserInputModeSamplesEdit::processMouseSelectAnnotation(const MouseEvent& mouseEvent,
                                                                const bool shiftKeyDownFlag,
                                                                const bool singleSelectionModeFlag)
{
    /*
     * Do normal selection processing
     */
    UserInputModeAnnotations::processMouseSelectAnnotation(mouseEvent,
                                                           shiftKeyDownFlag,
                                                           singleSelectionModeFlag);
}

/**
 * Delete all selected annotations except color bars which are turned off for display
 */
void
UserInputModeSamplesEdit::deleteSelectedAnnotations()
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(getUserInputMode());
    if (annotationManager->isAnnotationSelectedForEditingDeletable(getBrowserWindowIndex())) {
        BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(getBrowserWindowIndex()));
        CaretAssert(bbw);

        std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(getBrowserWindowIndex());
        if ( ! selectedAnnotations.empty()) {
            std::set<AnnotationPolyhedron*> polyhedronsToDelete;
            for (Annotation* ann : selectedAnnotations) {
                AnnotationPolyhedron* p(ann->castToPolyhedron());
                if (p != NULL) {
                    polyhedronsToDelete.insert(p);
                }
            }
            
            if ( ! polyhedronsToDelete.empty()) {
                /*
                 * If user has an Retrospective Sample selected but its corresponding
                 * Prospective Sample is NOT selected, this will contain the
                 * corresponding Prospective Samples that are NOT selected
                 */
                std::set<AnnotationPolyhedron*> prospectivePolyhedronsNotSelected;
                
                /*
                 * If user has a Prospective Sample selected but its corresponding
                 * retrospective Sample is NOT selected, this will contain the
                 * corresponding retrospective Samples that are NOT selected
                 */
                std::set<AnnotationPolyhedron*> retrospectivePolyhedronsNotSelected;
                
                for (AnnotationPolyhedron* poly : polyhedronsToDelete) {
                    switch (poly->getPolyhedronType()) {
                        case AnnotationPolyhedronTypeEnum::INVALID:
                            break;
                        case AnnotationPolyhedronTypeEnum::RETROSPECTIVE_SAMPLE:
                        {
                            EventAnnotationPolyhedronGetByLinkedIdentifier polyEvent(NULL,
                                                                                     AnnotationPolyhedronTypeEnum::PROSPECTIVE_SAMPLE,
                                                                                     poly->getLinkedPolyhedronIdentifier());
                            EventManager::get()->sendEvent(polyEvent.getPointer());
                            AnnotationPolyhedron* prospectivePolyedron(polyEvent.getPolyhedron());
                            if (prospectivePolyedron != NULL) {
                                if (std::find(polyhedronsToDelete.begin(),
                                              polyhedronsToDelete.end(),
                                              prospectivePolyedron) == polyhedronsToDelete.end()) {
                                    prospectivePolyhedronsNotSelected.insert(prospectivePolyedron);
                                }
                                else {
                                    /*
                                     * If here, the prospectivePolyedron is one of
                                     * the selected annotations for deletion.
                                     */
                                }
                            }
                        }
                            break;
                        case AnnotationPolyhedronTypeEnum::PROSPECTIVE_SAMPLE:
                        {
                            EventAnnotationPolyhedronGetByLinkedIdentifier polyEvent(NULL,
                                                                                     AnnotationPolyhedronTypeEnum::RETROSPECTIVE_SAMPLE,
                                                                                     poly->getLinkedPolyhedronIdentifier());
                            EventManager::get()->sendEvent(polyEvent.getPointer());
                            AnnotationPolyhedron* retrospectivePolyedron(polyEvent.getPolyhedron());
                            if (retrospectivePolyedron != NULL) {
                                if (std::find(polyhedronsToDelete.begin(),
                                              polyhedronsToDelete.end(),
                                              retrospectivePolyedron) == polyhedronsToDelete.end()) {
                                    retrospectivePolyhedronsNotSelected.insert(retrospectivePolyedron);
                                }
                                else {
                                    /*
                                     * If here, the Restrospective Polyhedron is one of
                                     * the selected annotations for deletion.
                                     */
                                }
                            }
                        }
                            break;
                    }
                }
                
                AString msg;
                if ( ! retrospectivePolyhedronsNotSelected.empty()) {
                    msg.appendWithNewLine("Retrospective sample(s) corresponding to the selected prospective sample(s) will also be deleted.");
                }
                
                if ( ! prospectivePolyhedronsNotSelected.empty()) {
                    msg.appendWithNewLine("Do you also want to delete the prospective sample(s) corresponding to the "
                                          "selected retrospective sample(s)?");
                    
                }
                
                if ( ! prospectivePolyhedronsNotSelected.empty()) {
                    WuQMessageBoxTwo::StandardButton button
                    = WuQMessageBoxTwo::warning(bbw, 
                                                "Warning",
                                                msg,
                                                WuQMessageBoxTwo::createButtonMask(WuQMessageBoxTwo::StandardButton::Yes,
                                                                                   WuQMessageBoxTwo::StandardButton::No,
                                                                                   WuQMessageBoxTwo::StandardButton::Cancel),
                                                WuQMessageBoxTwo::StandardButton::Cancel);
                    if (button == WuQMessageBoxTwo::StandardButton::Yes) {
                        polyhedronsToDelete.insert(prospectivePolyhedronsNotSelected.begin(),
                                                   prospectivePolyhedronsNotSelected.end());
                        polyhedronsToDelete.insert(retrospectivePolyhedronsNotSelected.begin(),
                                                   retrospectivePolyhedronsNotSelected.end());
                    }
                    else if (button == WuQMessageBoxTwo::StandardButton::No) {
                        polyhedronsToDelete.insert(retrospectivePolyhedronsNotSelected.begin(),
                                                   retrospectivePolyhedronsNotSelected.end());
                    }
                    else {
                        polyhedronsToDelete.clear();
                    }
                }
                else if ( ! retrospectivePolyhedronsNotSelected.empty()) {
                    WuQMessageBoxTwo::StandardButton button
                     = WuQMessageBoxTwo::warning(bbw,
                                                 "Warning",
                                                 msg,
                                                 WuQMessageBoxTwo::createButtonMask(WuQMessageBoxTwo::StandardButton::Ok,
                                                                                    WuQMessageBoxTwo::StandardButton::Cancel),
                                                 WuQMessageBoxTwo::StandardButton::Cancel);
                    if (button == WuQMessageBoxTwo::StandardButton::Ok) {
                        polyhedronsToDelete.insert(retrospectivePolyhedronsNotSelected.begin(),
                                                   retrospectivePolyhedronsNotSelected.end());
                    }
                    else {
                        polyhedronsToDelete.clear();
                    }
                }
                else {
                    /* Nothing, all corresponding polygons are selected */
                }
            }
            
            if ( ! polyhedronsToDelete.empty()) {
                std::vector<Annotation*> annotations(polyhedronsToDelete.begin(),
                                                     polyhedronsToDelete.end());
                    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
                    undoCommand->setModeDeleteAnnotations(annotations);
                    AString errorMessage;
                    if ( !  annotationManager->applyCommand(undoCommand,
                                                            errorMessage)) {
                        WuQMessageBox::errorOk(bbw,
                                               errorMessage);
                    }
                    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
                    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
            }
        }
    }
}
